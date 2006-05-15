      subroutine read_input(it_g,jt_g,kt,mm,isct,dx,dy,dz,epsi,
     &     iprint,do_dsa,ifixups,ibc,jbc,kbc,mk,isn,nm,
     &     myid,numtasks,npe_i,npe_j,mmo,mmi,ncpu)

      implicit none

      integer it_g,jt_g,kt,mm,isct
      double precision dx, dy, dz, epsi
      integer iprint
      logical do_dsa
      integer ifixups
      integer ibc,jbc,kbc
      integer mk,isn,nm
      integer myid,numtasks,npe_i,npe_j
      integer mmo,mmi
      integer ncpu

      integer idsa
      integer info
      integer idata(20)
      double precision data(4)

      if (myid .eq. 1) then
        open (unit=10,file='input',status='old')
        read (10,*) npe_i, npe_j, mk, mmi, ncpu
        if (numtasks.eq.1) then
           npe_i = 1
           npe_j = 1
        endif
        read (10,*) it_g, jt_g, kt, mm, isct
        read (10,*) dx, dy, dz, epsi
        read (10,*) ibc, jbc, kbc
        read (10,*) iprint,idsa,ifixups
        close (10)
        idata(1) = it_g
        idata(2) = jt_g
        idata(3) = kt
        idata(4) = mm
        idata(5) = isct
        idata(6) = iprint
        idata(7) = ibc
        idata(8) = jbc
        idata(9) = kbc
        idata(10)= mk
        idata(11)= idsa
        idata(12)= ifixups
        idata(13)= npe_i
        idata(14)= npe_j
        idata(15)= mmi
        idata(16)= ncpu
        data(1) = dx
        data(2) = dy
        data(3) = dz
        data(4) = epsi
      endif

      call bcast_int (idata,16,1001,1,info)
      call bcast_real(data,4,1002,1,info)
      it_g  = idata(1)
      jt_g  = idata(2)
      kt    = idata(3)
      mm    = idata(4)
      isct  = idata(5)
      iprint= idata(6)
      ibc   = idata(7)
      jbc   = idata(8)
      kbc  = idata(9)
      mk    = idata(10)
      idsa  = idata(11)
      ifixups = idata(12)
      npe_i = idata(13)
      npe_j = idata(14)
      mmi   = idata(15)
      ncpu  = idata(16)
      dx    = data(1)
      dy    = data(2)
      dz    = data(3)
      epsi  = data(4)
c
c  do some checking:
c
      if (mm.eq.3) then
        isn=4
      else if (mm.eq.6) then
        isn=6
      else
        call task_end()
        stop 'unsupported mm'
      endif

c     nm = (isct+1)*(isct+1)    ! flux & source angular moments
      if (isct .eq. 0) then
        nm=1
      else if (isct .eq. 1) then
        nm=4
      else
        call task_end()
        stop 'unsupported isct'
      endif

c
c mk controls the number of k-planes packaged per message
c for low latencies, mk=1 provides the best parallel efficiency at the
c    expense of many short-length messages
c for high latencies, mk=kt provides the fewest long-length messages
c    to hide latency but at the expense of lower parallel efficiency
c
      mk = min (mk,kt)
      if (npe_i*npe_j .le. 1) mk = kt

      if (npe_i*npe_j .le. 1) mmi = mm
      mmo = mm/mmi
      if (mm .ne. mmo*mmi) then
        call task_end()
        stop 'mmi is not an integral factor of mm'
      endif

      if (idsa.ne.0) then
         do_dsa = .true.
      else
         do_dsa = .false.
      endif

      return
      end
