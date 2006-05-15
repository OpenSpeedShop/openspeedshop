
      subroutine sweep (it,jt, kt, nm, isct, mm,mmo,mmi, mk, myid,
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

      implicit none

      integer it,jt, kt, nm, isct, mm,mmo,mmi, mk, myid
      double precision hi(it), hj(jt), hk(kt)
      double precision di(it), dj(jt), dk(kt)
      double precision phi(it)
      double precision phii(0:it+1)
      double precision  src(it, jt, kt, nm)
      double precision flux(it, jt, kt, nm)
      double precision sigt(it, jt, kt)
      double precision w(mm), mu(mm), eta(mm), tsi(mm)
      double precision wmu(mm), weta(mm), wtsi(mm)
      double precision pn(mm,nm,8)
      integer north, south, east, west
      double precision phiib(jt, mk, mmi)
      double precision phijb(it, mk, mmi)
      double precision phikb(it, jt, mmi)
      integer mdiag(mmi)
      double precision leakage(6)
      logical do_dsa
      integer it_dsa,jt_dsa,kt_dsa
      double precision face(it_dsa,jt_dsa,kt_dsa,3)
      logical do_fixup
      integer nfixed
      integer ibc,jbc,kbc
      integer jt_ibc,kt_ibc,mm_ibc
      integer it_jbc,kt_jbc,mm_jbc
      integer it_kbc,jt_kbc,mm_kbc
      double precision phiibc(jt_ibc,kt_ibc,mm_ibc,0:1,0:1)
      double precision phijbc(it_jbc,kt_jbc,mm_jbc,0:1)
      double precision phikbc(it_kbc,jt_kbc,mm_kbc)

      integer i,j,k,m,n,iq
      integer idiag,ndiag,jkm,jk,mo,mi,mio
      integer i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3
      integer kk,kb,nk,lk
      integer nib,njb
      integer ns_snd,ns_rcv,ns_tag,ew_snd,ew_rcv,ew_tag,info
      double precision ci,cj,ck
      double precision phiir
      double precision leak
      double precision ql,dl,ti,tj,tk
      integer ifixed,jfixed

      integer nmess,mess
      common /h/ nmess,mess


C SWEEP FLOW
C
C DO iq=1,8                 ! octants
C
C  DO mo=1,mmo              ! angle pipelining loop
C
C   DO kk=1,kb              ! k-plane pipelining loop
C
C    RECV E/W               ! recv block I-inflows
C    RECV N/S               ! recv block J-inflows
C
C    DO idiag=1,jt+nk-1+mmi-1  ! JK-diagonals with MMI pipelining (block of work)
C     DO jkm=1,ndiag        ! I-lines on this diagonal
C
C      DO i=1,it            ! source (from Pn moments)
C      ENDDO
C
C      IF .NOT.do_fixups
C       DO i=i0,i1,i2       ! Sn eqn
C       ENDDO
C      ELSE
C       DO i=i0,i1,i2       ! Sn eqn w/ fixups
C       ENDDO
C      ENDIF
C
C      DO i=1,it            ! flux (Pn moments)
C      ENDDO
C
C      DO i=1,it            ! DSA face currents
C      ENDDO
C
C     ENDDO
C    ENDDO
C
C    SEND E/W               ! send block I-outflows
C    SEND N/S               ! send block J-outflows
C
C   ENDDO
C
C  ENDDO
C
C ENDDO

c     nmess = 0
c     mess = 0

      leakage(1) = 0.0d+0
      leakage(2) = 0.0d+0
      leakage(3) = 0.0d+0
      leakage(4) = 0.0d+0
      leakage(5) = 0.0d+0
      leakage(6) = 0.0d+0

      if (do_dsa) then
         do n = 1, 3
         do k = 1, kt_dsa
         do j = 1, jt_dsa
         do i = 1, it_dsa
            face(i,j,k,n) = 0.0d+0
         end do
         end do
         end do
         end do
      endif

      nfixed = 0

c octant iq-loop
c
      DO iq = 1, 8

         call octant (i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3,iq,
     &       it,jt,kt)

         if (i2 .gt. 0) then
c  if i2 is > 0 (right sweep) recv phiib from west & send it east
           ew_rcv = west
           ew_snd = east
           ew_tag = 1000*iq
         else
c  if i2 is < 0 (left sweep)  recv phiib from east & send it west
           ew_rcv = east
           ew_snd = west
           ew_tag = 3000*iq
         endif

         if (j2 .gt. 0) then
c  if j2 is > 0 (up sweep)    recv phijb from south & send it north
           ns_rcv = south
           ns_snd = north
           ns_tag = 2000*iq
         else
c  if j2 is < 0  (down sweep) recv phijb from north & send it south
           ns_rcv = north
           ns_snd = south
           ns_tag = 4000*iq
         endif

         do m = 1, mm
            wmu (m) = i2* mu(m)*w(m)
            weta(m) = j2*eta(m)*w(m)
            wtsi(m) = k2*tsi(m)*w(m)
         end do

c angle pipelining loop (batches of mmi angles)
c
         DO mo = 1, mmo
            mio = (mo-1)*mmi

c K-inflows (k=k0 boundary)
c
            if (k2.lt.0 .or. kbc.eq.0) then
               do mi = 1, mmi
               do j = 1, jt
               do i = 1, it
                  phikb(i,j,mi) = 0.0d+0
               end do
               end do
               end do
            else
               if (do_dsa) then
                  leak = 0.0
                  k = k0 - k2
                  do mi = 1, mmi
                   m = mi + mio
                  do j = 1, jt
                  do i = 1, it
                     phikb(i,j,mi) = phikbc(i,j,m)
                     leak = leak
     &                    + wtsi(m)*phikb(i,j,mi)*di(i)*dj(j)
                     face(i,j,k+k3,3) = face(i,j,k+k3,3)
     &                    + wtsi(m)*phikb(i,j,mi)
                  end do
                  end do
                  end do
                  leakage(5) = leakage(5) + leak
               else
                  leak = 0.0
                  do mi = 1, mmi
                   m = mi + mio
                  do j = 1, jt
                  do i = 1, it
                     phikb(i,j,mi) = phikbc(i,j,m)
                     leak = leak
     &                    + wtsi(m)*phikb(i,j,mi)*di(i)*dj(j)
                  end do
                  end do
                  end do
                  leakage(5) = leakage(5) + leak
               endif
            endif

c k-plane pipelining loop (batches of mk-planes)
c
         kb = (kt+mk-1)/mk
         DO kk = 1, kb

              if (k2.gt.0) then
                 k0 = 1 + (kk-1)*mk
                 k1 = min (k0+mk-1,kt)
                 nk = k1 - k0 + 1
              else
                 k0 = kt - (kk-1)*mk
                 k1 = max (k0-mk+1,1)
                 nk = k0 - k1 + 1
              endif

! this could be *nk* instead of *mk* if all phi{i,j}{b,bc}
! were dimensioned with mmi as the second dimension
              nib = jt*mk*mmi
              njb = it*mk*mmi

c I-inflows for block (i=i0 boundary)
c
              if (ew_rcv .ne. 0) then
                 call rcv_real(ew_rcv, phiib, nib, ew_tag, info)
              else
                 if (i2.lt.0 .or. ibc.eq.0) then
                    do mi = 1, mmi
                    do lk = 1, nk
                    do j = 1, jt
                       phiib(j,lk,mi) = 0.0d+0
                    end do
                    end do
                    end do
                 else
                    leak = 0.0
                    do mi = 1, mmi
                     m = mi + mio
                    do lk = 1, nk
                     k = k0 + sign(lk-1,k2)
                    do j = 1, jt
                       phiib(j,lk,mi) = phiibc(j,k,m,k3,j3)
                       leak = leak
     &                     + wmu(m)*phiib(j,lk,mi)*dj(j)*dk(k)
                    end do
                    end do
                    end do
                    leakage(1) = leakage(1) + leak
                 endif
              endif
              if (do_dsa) then
                 i = i0 - i2
                 do mi = 1, mmi
                  m = mi + mio
                 do lk = 1, nk
                  k = k0 + sign(lk-1,k2)
                 do j = 1, jt
                    face(i+i3,j,k,1) = face(i+i3,j,k,1)
     &                   + wmu(m)*phiib(j,lk,mi)
                 end do
                 end do
                 end do
              endif

c J-inflows for block (j=j0 boundary)
c
              if (ns_rcv .ne. 0) then
                 call rcv_real(ns_rcv, phijb, njb, ns_tag, info)
              else
                 if (j2.lt.0 .or. jbc.eq.0) then
                    do mi = 1, mmi
                    do lk = 1, nk
                    do i = 1, it
                       phijb(i,lk,mi) = 0.0d+0
                    end do
                    end do
                    end do
                 else
                    leak = 0.0
                    do mi = 1, mmi
                     m = mi + mio
                    do lk = 1, nk
                     k = k0 + sign(lk-1,k2)
                    do i = 1, it
                       phijb(i,lk,mi) = phijbc(i,k,m,k3)
                       leak = leak
     &                      + weta(m)*phijb(i,lk,mi)*di(i)*dk(k)
                    end do
                    end do
                    end do
                    leakage(3) = leakage(3) + leak
                 endif
              endif
              if (do_dsa) then
                 j = j0 - j2
                 do mi = 1, mmi
                  m = mi + mio
                 do lk = 1, nk
                  k = k0 + sign(lk-1,k2)
                 do i = 1, it
                    face(i,j+j3,k,2) = face(i,j+j3,k,2)
     &                   + weta(m)*phijb(i,lk,mi)
                 end do
                 end do
                 end do
              endif

c JK-diagonals with MMI pipelined angles (block of work)
c
            do mi = 1, mmi
               mdiag(mi) = 0
            end do

         DO idiag = 1, jt+nk-1+mmi-1
            ndiag = 0
            do mi = mmi, 2, -1
               mdiag(mi) = mdiag(mi-1)
               ndiag = ndiag + mdiag(mi)
            end do
            mdiag(1) = max(min(idiag, jt, nk, jt+nk-idiag),0)
            ndiag = ndiag + mdiag(1)

c I-lines on this diagonal (j/k/m triplets)
c
! DO PARALLEL
!   PRIVATE jkm,jk,mi,m,i,j,k,lk,n
!   PRIVATE ci,cj,ck
!   PRIVATE phi,phii,phiir
!   PRIVATE ql,dl,t,ti,tj,tk,ifixed,jfixed
!   SHARED mmi,mio,idiag,ndiag,mdiag
!   SHARED i0,j0,k0,i1,i2,j2,k2,it,jt,kt
!   SHARED iq,nm,pn,i3,j3,k3,mm,
!   SHARED w,mu,eta,tsi,wmu,weta,wtsi
!   SHARED src,sigt,flux,face
!   SHARED phiib,phijb,phikb
!   SHARED hi,hj,hk,di,dj,dk
!   SHARED do_fixup,do_dsa
!   SHARED nfixed
!   GUARD nfixed
            jfixed = 0
         DO jkm = 1, ndiag
            jk = jkm
            do mi = 1, mmi-1
               if (jk .le. mdiag(mi)) go to 100
               jk = jk - mdiag(mi)
            end do
            mi = mmi
  100       continue
            m = mi + mio
            j = j0 + sign(min(idiag-mi+1,jt)-1-jk+1,j2)
            k = k0 + sign(max(idiag-mi+1-jt,0)+jk-1,k2)
            lk = abs(k-k0) + 1
!
! Following 3 loops can also be used in place of above idiag/jkm loops,
! but then only the mmi loop can be task parallel.
!        DO mi = 1, mmi
!           m = mi + mio
!        DO lk = 1, nk
!           k = k0 + sign(lk-1,k2)
!        DO j = j0, j1, j2

            ck = tsi(m)*hk(k)
            cj = eta(m)*hj(j)

c I-inflow for this I-line
c
            phiir   = phiib(j,lk,mi)
            i = i0 - i2
            phii(i) = phiir

c compute source from Pn moments (I-line)
            do i = 1, it
               phi(i) = src(i,j,k,1)
            end do
            do n = 2, nm
               do i = 1, it
                  phi(i) = phi(i) + pn(m,n,iq)*src(i,j,k,n)
               end do
            end do

         if ( .not. do_fixup) then

c I-line recursion: without flux fixup
c
         DO i = i0, i1, i2
            ci =  mu(m)*hi(i)
c balance equation - recursion on phiir
            dl = ( sigt(i,j,k) + ci + cj + ck )
            dl = 1.0 / dl
            ql = ( phi(i) +
     &             ci*phiir + cj*phijb(i,lk,mi) + ck*phikb(i,j,mi) )
            phi(i) = ql * dl
c auxiliary equations (diamond)
            phiir          = 2.0d+0*phi(i) - phiir
            phii(i)        = phiir
            phijb(i,lk,mi) = 2.0d+0*phi(i) - phijb(i,lk,mi)
            phikb(i,j,mi)  = 2.0d+0*phi(i) - phikb(i,j,mi)
         END DO ! i

         else

c I-line recursion: with flux fixup
c
         DO i = i0, i1, i2
            ci =  mu(m)*hi(i)
c balance equation - recursion on phiir
            dl = ( sigt(i,j,k) + ci + cj + ck )
            ti = 1.0 / dl
            ql = ( phi(i) +
     &             ci*phiir + cj*phijb(i,lk,mi) + ck*phikb(i,j,mi) )
            phi(i) = ql * ti
c auxiliary equations (diamond)
            ti = 2.0d+0*phi(i) - phiir
            tj = 2.0d+0*phi(i) - phijb(i,lk,mi)
            tk = 2.0d+0*phi(i) - phikb(i,j,mi)
c fixup i,j, & k if negative
            ifixed = 0
  111       continue
            if (ti .lt. 0.0d+0) then
               dl = dl - ci
               ti = 1.0 / dl
               ql = ql - 0.5d+0*ci*phiir
               phi(i) = ql * ti
               ti = 0.0d+0
               if (tj .ne. 0.0d+0) tj = 2.0d+0*phi(i) - phijb(i,lk,mi)
               if (tk .ne. 0.0d+0) tk = 2.0d+0*phi(i) - phikb(i,j,mi)
               ifixed = 1
            endif
            if (tj .lt. 0.0d+0) then
               dl = dl - cj
               tj = 1.0 / dl
               ql = ql - 0.5d+0*cj*phijb(i,lk,mi)
               phi(i) = ql * tj
               tj = 0.0d+0
               if (tk .ne. 0.) tk = 2.0d+0*phi(i) - phikb(i,j,mi)
               if (ti .ne. 0.) ti = 2.0d+0*phi(i) - phiir
               ifixed = 1
               go to 111
            endif
            if (tk .lt. 0.0d+0) then
               dl = dl - ck
               tk = 1.0 / dl
               ql = ql - 0.5d+0*ck*phikb(i,j,mi)
               phi(i) = ql * tk
               tk = 0.0d+0
               if (ti .ne. 0.0d+0) ti = 2.0d+0*phi(i) - phiir
               if (tj .ne. 0.0d+0) tj = 2.0d+0*phi(i) - phijb(i,lk,mi)
               ifixed = 1
               go to 111
            endif
c
            phiir          = ti
            phii(i)        = phiir
            phijb(i,lk,mi) = tj
            phikb(i,j,mi)  = tk
            jfixed = jfixed + ifixed
         END DO ! i

         endif

c compute flux Pn moments (I-line)
            do i = 1, it
               flux(i,j,k,1) = flux(i,j,k,1) + w(m)*phi(i)
            end do
            do n = 2, nm
               do i = 1, it
                  flux(i,j,k,n) = flux(i,j,k,n)
     &                 + pn(m,n,iq)*w(m)*phi(i)
               end do
            end do

c compute DSA face currents (I-line)
         if (do_dsa) then
            do i = 1, it
               face(i+i3,j,k,1) = face(i+i3,j,k,1)
     &              + wmu (m)*phii(i)
               face(i,j+j3,k,2) = face(i,j+j3,k,2)
     &              + weta(m)*phijb(i,lk,mi)
               face(i,j,k+k3,3) = face(i,j,k+k3,3)
     &              + wtsi(m)*phikb(i,j,mi)
            end do
         endif

c I-outflow for this I-line
c
            phiib(j,lk,mi) = phiir

c diagonal I-line loops ( j/k/m triplets)
c
         END DO ! jkm
         nfixed = nfixed + jfixed
         END DO ! idiag
!
!        enddo
!        enddo
!        enddo

c block I-outflows (i=i1 boundary)
c
            if (ew_snd .ne. 0) then
               call snd_real(ew_snd, phiib, nib, ew_tag, info)
c              nmess = nmess + 1
c              mess = mess + nib
            else
               if (i2.lt.0 .and. ibc.ne.0) then
                  leak = 0.0
                  do mi = 1, mmi
                   m = mi + mio
                  do lk = 1, nk
                   k = k0 + sign(lk-1,k2)
                  do j = 1, jt
                     phiibc(j,k,m,k3,j3) = phiib(j,lk,mi)
                     leak = leak
     &                    + wmu(m)*phiib(j,lk,mi)*dj(j)*dk(k)
                  end do
                  end do
                  end do
                  leakage(1+i3) = leakage(1+i3) + leak
               else
                  leak = 0.0
                  do mi = 1, mmi
                   m = mi + mio
                  do lk = 1, nk
                   k = k0 + sign(lk-1,k2)
                  do j = 1, jt
                     leak = leak
     &                    + wmu(m)*phiib(j,lk,mi)*dj(j)*dk(k)
                  end do
                  end do
                  end do
                  leakage(1+i3) = leakage(1+i3) + leak
               endif
            endif

c block J-outflows (j=j1 boundary)
c
            if (ns_snd .ne. 0) then
               call snd_real(ns_snd, phijb, njb, ns_tag, info)
c              nmess = nmess + 1
c              mess = mess + njb
            else
               if (j2.lt.0 .and. jbc.ne.0) then
                  leak = 0.0
                  do mi = 1, mmi
                   m = mi + mio
                  do lk = 1, nk
                   k = k0 + sign(lk-1,k2)
                  do i = 1, it
                     phijbc(i,k,m,k3) = phijb(i,lk,mi)
                     leak = leak
     &                    + weta(m)*phijb(i,lk,mi)*di(i)*dk(k)
                  end do
                  end do
                  end do
                  leakage(3+j3) = leakage(3+j3) + leak
               else
                  leak = 0.0
                  do mi = 1, mmi
                   m = mi + mio
                  do lk = 1, nk
                   k = k0 + sign(lk-1,k2)
                  do i = 1, it
                     leak = leak
     &                    + weta(m)*phijb(i,lk,mi)*di(i)*dk(k)
                  end do
                  end do
                  end do
                  leakage(3+j3) = leakage(3+j3) + leak
               endif
            endif

c k-plane pipelining loop (batches of mk-planes)
c
         END DO ! kk

c K-outflows (k=k1 boundary)
c
         if (k2.lt.0 .and. kbc.ne.0) then
            leak = 0.0
            do mi = 1, mmi
             m = mi + mio
            do j = 1, jt
            do i = 1, it
               phikbc(i,j,m) = phikb(i,j,mi)
               leak = leak
     &              + wtsi(m)*phikb(i,j,mi)*di(i)*dj(j)
            end do
            end do
            end do
            leakage(5+k3) = leakage(5+k3) + leak
         else
            leak = 0.0
            do mi = 1, mmi
             m = mi + mio
            do j = 1, jt
            do i = 1, it
               leak = leak
     &              + wtsi(m)*phikb(i,j,mi)*di(i)*dj(j)
            end do
            end do
            end do
            leakage(5+k3) = leakage(5+k3) + leak
         endif

c angle pipelining loop (batches of mmi angles)
c
         END DO ! mo

c octant loop
c
         END DO ! iq

      return
      end
