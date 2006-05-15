
      subroutine initialize(it,jt,kt, mm,isct,nm, dx,dy,dz,
     & it_g,jt_g,istart,jstart, ibc,jbc,kbc,
     & Sigt,Srcx,Sigs, w, mu, eta, tsi, pn,
     & hi,hj,hk, di,dj,dk, Flux)

      implicit none

      integer it,jt,kt, it_g,jt_g, mm,isct,nm
      double precision dx,dy,dz
      integer istart,jstart
      integer ibc,jbc,kbc
      double precision Sigt(it,jt,kt)
      double precision Srcx(it,jt,kt)
      double precision Sigs(it,jt,kt,isct+1)
      double precision w(mm), mu(mm), eta(mm), tsi(mm)
      double precision pn(mm,nm,8)
      double precision hi(it), hj(jt), hk(kt)
      double precision di(it), dj(jt), dk(kt)
      double precision Flux(it,jt,kt,nm)

      integer i,j,k,n

c
c  initialize cross sections and source
c
      call initxs(it,jt,kt, it_g,jt_g, istart,jstart,
     &         isct, Sigt,Srcx,Sigs, ibc,jbc,kbc)
c
c  sn constants
c
      call initsnc(mm, w, mu, eta, tsi, nm, pn)
c
c  geometry setup (2./dx)
c
      call initgeom(it,jt,kt, hi,hj,hk, di,dj,dk, dx,dy,dz,
     &    it_g,jt_g, istart,jstart, ibc,jbc,kbc)

c
c  zero out the flux
c
      do n = 1, nm
        do k = 1, kt
          do j = 1, jt
            do i = 1, it
              Flux(i,j,k,n) = 0.0d+00
            end do
          end do
        end do
      end do

      return
      end


      subroutine initxs(it,jt,kt, it_g,jt_g, istart,jstart,
     &         isct, Sigt,Srcx,Sigs, ibc,jbc,kbc)
c
c  initialize cross sections and source
c
      implicit none

      integer it,jt,kt, it_g,jt_g, istart,jstart, isct
      integer ibc,jbc,kbc
      double precision Sigt(it,jt,kt)
      double precision Srcx(it,jt,kt)
      double precision Sigs(it,jt,kt,isct+1)

      integer i,j,k
      integer i_3,j_3,k_3
      integer i_l,i_r,j_l,j_r,k_l,k_r
      integer istop,jstop
      integer i_g,j_g

c set the cross sections
c
      do k = 1, kt
      do j = 1, jt
      do i = 1, it
            Sigt(i,j,k) = 1.0d+0
            Sigs(i,j,k,1) = 0.5d+0
      end do
      end do
      end do
      if (isct.gt.0) then
         do k = 1, kt
         do j = 1, jt
         do i = 1, it
               Sigs(i,j,k,2) = (2*1+1)*0.2d+0
         end do
         end do
         end do
      end if

c zero out the external source
c
      do k = 1, kt
      do j = 1, jt
      do i = 1, it
            Srcx(i,j,k) = 0.0d+0
      end do
      end do
      end do

c  make center (1/3)-cubed grid points have a unit source
c
      if (it_g.ge.3) then
         i_3 = (it_g + 1)/3
      else
         i_3 = 0
      endif
      if (ibc.eq.0) then
         i_l = i_3
         i_r = i_3
      else
         i_l = 0
         i_r = it_g - i_3
      endif
      if (jt_g.ge.3) then
         j_3 = (jt_g + 1)/3
      else
         j_3 = 0
      endif
      if (jbc.eq.0) then
         j_l = j_3
         j_r = j_3
      else
         j_l = 0
         j_r = jt_g - j_3
      endif
      if (kt.ge.3) then
         k_3 = (kt + 1)/3
      else
         k_3 = 0
      endif
      if (kbc.eq.0) then
         k_l = k_3
         k_r = k_3
      else
         k_l = 0
         k_r = kt - k_3
      endif
      istop = istart + it - 1
      jstop = jstart + jt - 1

      do k = 1+k_l, kt-k_r
      do j_g = max(1+j_l,jstart), min(jt_g-j_r,jstop)
      do i_g = max(1+i_l,istart), min(it_g-i_r,istop)
         j = j_g - jstart + 1
         i = i_g - istart + 1
         Srcx(i,j,k) = 1.0d+0
      end do
      end do
      end do

      return
      end


      subroutine initsnc(mm, w, mu, eta, tsi, nm, pn)
c
c Initialize Sn directions
c
      implicit none

      integer mm, nm
      double precision w(mm), mu(mm), eta(mm), tsi(mm)
      double precision pn(mm,nm,8)

      integer m
      integer i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3,iq

      if (mm.eq.6) then         ! S6

      mu(1) = .23009194d+00
      eta(1) = .94557676d+00
      w(1) = .16944656d+00 / 8.d+00

      mu(2) = .68813432d+00
      eta(2) = mu(2)
      w(2) = .16388677d+00 / 8.d+00

      mu(3) = mu(1)
      eta(3) = mu(2)
      w(3) = w(2)

      mu(4) = eta(1)
      eta(4) = mu(1)
      w(4) = w(1)

      mu(5) = mu(2)
      eta(5) = mu(1)
      w(5) = w(2)

      mu(6) = mu(1)
      eta(6) = mu(1)
      w(6) = w(1)

      else if (mm.eq.3) then    ! S4

      mu(1) = 0.30163878d+00
      eta(1) = 0.90444905d+00
      w(1) = 1.0d+00/3.0d+00 / 8.d+00

      mu(2) = eta(1)
      eta(2) = mu(1)
      w(2) = w(1)

      mu(3) = mu(1)
      eta(3) = mu(1)
      w(3) = w(1)

      else
        call task_end()
        stop 'unsupported quadrature'
      endif

      do m=1,mm
         tsi(m) = sqrt((1.0-mu(m)**2)-eta(m)**2)
      end do

      do iq = 1, 8
         do m = 1, mm
            pn(m,1,iq) = 1.0d+00
         end do
         if (nm .eq. 4) then
            call octant (i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3,iq,
     &            1,1,1)
            do m = 1, mm
               pn(m,2,iq) = i2*mu (m)
               pn(m,3,iq) = j2*eta(m)
               pn(m,4,iq) = k2*tsi(m)
            end do
         endif
      end do

      return
      end


      subroutine initgeom(it,jt,kt, hi,hj,hk, di,dj,dk, dx,dy,dz,
     &    it_g,jt_g, istart,jstart, ibc,jbc,kbc)

      implicit none

      integer it, jt, kt
      double precision hi(it), hj(jt), hk(kt)
      double precision di(it), dj(jt), dk(kt)
      double precision dx, dy, dz
      integer it_g,jt_g, istart,jstart
      integer ibc,jbc,kbc

      integer i,j,k
      integer i_g,j_g,k_g

      do i = 1, it
            i_g = i - 1 + istart
            di(i) = dx
            hi(i) = 2.d+00/dx
      end do
      do j = 1, jt
            j_g = j - 1 + jstart
            dj(j) = dy
            hj(j) = 2.d+00/dy
      end do
      do k = 1, kt
            dk(k) = dz
            hk(k) = 2.d+00/dz
      enddo

      return
      end
