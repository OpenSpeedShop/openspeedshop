
      subroutine inner (it, jt, kt, nm, isct, mm,mmo,mmi, mk,
     & dx, dy, dz, epsi, err, iprint,
     & it_g, jt_g, istart, jstart,
     & myid, numtasks, north, south, east, west,
     & its,t0,t1,et0,et1,
     & Sigt,Pflux,Srcx,Sigs,Flux,Src,
     & hi,hj,hk, di,dj,dk, w,mu,eta,tsi, wmu,weta,wtsi, pn,
     & Phi,Phii,Phiib,Phijb,Phikb,mdiag,
     & do_dsa,Face,it_dsa,jt_dsa,kt_dsa, ifixups,nfixups,
     & ibc,jbc,kbc,
     & Phiibc,jt_ibc,kt_ibc,mm_ibc,
     & Phijbc,it_jbc,kt_jbc,mm_jbc,
     & Phikbc,it_kbc,jt_kbc,mm_kbc)

      implicit none

      integer it,jt,kt,nm,isct,mm,mmo,mmi,mk,iprint,its
      integer it_g,jt_g,istart,jstart
      integer myid, numtasks
      integer north, south, east, west
      double precision dx, dy, dz, epsi, err
      double precision t0,t1,et0,et1
      double precision  Sigt(it,jt,kt)
      double precision Pflux(it,jt,kt)
      double precision  Srcx(it,jt,kt)
      double precision  Sigs(it,jt,kt,isct+1)
      double precision  Flux(it,jt,kt,nm)
      double precision   Src(it,jt,kt,nm)
      double precision hi(it), hj(jt), hk(kt)
      double precision di(it), dj(jt), dk(kt)
      double precision w(mm), mu(mm), eta(mm), tsi(mm)
      double precision wmu(mm), weta(mm), wtsi(mm)
      double precision pn(mm,nm,8)
      double precision Phi(it)
      double precision Phii(0:it+1)
      double precision Phiib(jt,mk,mmi)
      double precision Phijb(it,mk,mmi)
      double precision Phikb(it,jt,mmi)
      integer mdiag(mmi)
      logical do_dsa
      integer it_dsa,jt_dsa,kt_dsa
      double precision Face(it_dsa,jt_dsa,kt_dsa,3)
      integer ifixups,nfixups
      integer ibc,jbc,kbc
      integer jt_ibc,kt_ibc,mm_ibc
      integer it_jbc,kt_jbc,mm_jbc
      integer it_kbc,jt_kbc,mm_kbc
      double precision Phiibc(jt_ibc,kt_ibc,mm_ibc,0:1,0:1)
      double precision Phijbc(it_jbc,kt_jbc,mm_jbc,0:1)
      double precision Phikbc(it_kbc,jt_kbc,mm_kbc)

      integer i,j,k,n
      logical do_fixup
      integer nfixed
      double precision Leakage(6)
      double precision sourcex, absorption, volume

      integer nmess,mess
      common /h/ nmess,mess

c
c Initialize
c
      call initialize(it,jt,kt, mm,isct,nm, dx,dy,dz,
     & it_g,jt_g,istart,jstart, ibc,jbc,kbc,
     & Sigt,Srcx,Sigs, w, mu, eta, tsi, pn,
     & hi,hj,hk, di,dj,dk, Flux)

      nfixups = 0
      if (myid .eq. 1) then
         print *, 'Iteration Monitor:'
      endif


c***************************************************************************

c
c only the parent times the run:
c
      call barrier_sync()
      if (myid .eq. 1) then
        call timers(t0,et0)
      endif

      its = 0

  111 continue                          ! top of iteration loop

      its = its + 1

      call source(it,jt,kt,isct,nm, Src,Srcx,Sigs,Flux,Pflux)

      nfixed = 0
      if (ifixups.gt.0) then
         do_fixup = .true.
      else if (ifixups.eq.0) then
         do_fixup = .false.
      else
         do_fixup = .false.
         if (its.gt.-ifixups) do_fixup = .true.
      endif

      call sweep (it,jt, kt, nm, isct, mm,mmo,mmi, mk, myid,
     1  hi, hj, hk, di, dj, dk, Phi, Phii,
     2  Src, Flux, Sigt,
     3  w,mu,eta,tsi, wmu,weta,wtsi, pn,
     4  north, south, east, west,
     5  Phiib,Phijb,Phikb,mdiag, Leakage,
     6  do_dsa,Face,it_dsa,jt_dsa,kt_dsa, do_fixup,nfixed,
     7  ibc,jbc,kbc,
     8  Phiibc,jt_ibc,kt_ibc,mm_ibc,
     9  Phijbc,it_jbc,kt_jbc,mm_jbc,
     A  Phikbc,it_kbc,jt_kbc,mm_kbc)

      call global_int_sum(nfixed)
      nfixups = nfixups + nfixed

c     if (nmess.gt.0)
c    &  print *,myid,'there were ',nmess,' messages of length ',mess,
c    &   ' average len: ',mess/nmess

      call flux_err(it,jt,kt,nm, Flux,Pflux, err)

      if (myid .eq. 1) print *,' its = ', its, ' err = ', err,
     &  '  fixs = ',nfixed

      if ( (err.gt.epsi .and. epsi.ge.0.0d+0) .or.
     &  (its.lt.int(-epsi+.99) .and. epsi.lt.0.0d+0) ) go to 111

      call barrier_sync()
      if (myid.eq.1) then
        call timers(t1,et1)
      endif

c***************************************************************************


c     print *, myid, ' all done!'

c
c  diagnostics
      sourcex = 0.0d+00
      absorption = 0.0d+00
      do k = 1, kt
        do j = 1, jt
          do i = 1, it
            volume = di(i)*dj(j)*dk(k)
            sourcex = sourcex + Srcx(i,j,k)*volume
            absorption = absorption + (Sigt(i,j,k)-Sigs(i,j,k,1))*
     &               Flux(i,j,k,1)*volume
          enddo
        enddo
      enddo
      call global_real_sum(sourcex)
      call global_real_sum(absorption)
      do n = 1, 6
         call global_real_sum(leakage(n))
      end do

      if (myid .eq. 1) then
        print *, 'Balance quantities:'
        print *, ' External Source: ',sourcex
        print *, ' Absorption:      ',absorption
        print *, ' I-leakages:      ',leakage(1),leakage(2)
        print *, ' J-leakages:      ',leakage(3),leakage(4)
        print *, ' K-leakages:      ',leakage(5),leakage(6)
      endif

      if (iprint.eq.1) then
        do n=1, numtasks
        if (myid .eq. n) then
          print *,'Task',myid,'    ',istart,':',istart+it-1,
     &                        '    ',jstart,':',jstart+jt-1
          print *,'scalar flux:'
          do j = 1, jt
            print 100, j-1+jstart,(flux(i,j,5,1),i=1,it)
          end do
          if (isct.gt.0) then
            print *,'mu moment:'
            do j = 1, jt
              print 100, j-1+jstart,(flux(i,j,5,2),i=1,it)
            end do
            print *,'eta moment:'
            do j = 1, jt
              print 100, j-1+jstart,(flux(i,j,5,3),i=1,it)
            end do
            print *,'xi moment:'
            do j = 1, jt
              print 100, j-1+jstart,(flux(i,j,5,4),i=1,it)
            end do
          endif
        endif
        call barrier_sync()
        end do
      endif

      return
 100  format (2x,i9,6(f17.14,' '))
      end

