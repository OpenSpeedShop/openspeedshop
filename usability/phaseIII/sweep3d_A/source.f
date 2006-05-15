
      subroutine source(it,jt,kt,isct,nm, Src,Srcx,Sigs,Flux,Pflux)
c
c Compute source from external source plus scattering moments
c
      implicit none

      integer it,jt,kt,isct,nm
      double precision  Srcx(it,jt,kt)
      double precision  Sigs(it,jt,kt,isct+1)
      double precision  Flux(it,jt,kt,nm)
      double precision   Src(it,jt,kt,nm)
      double precision Pflux(it,jt,kt)

      integer i,j,k

      if (isct.eq.0) then
         do k = 1, kt
         do j = 1, jt
         do i = 1, it
            Src(i,j,k,1) = Srcx(i,j,k)
     $                     + Sigs(i,j,k,1)*Flux(i,j,k,1)
            Pflux(i,j,k) = Flux(i,j,k,1)
            Flux(i,j,k,1) = 0.0d+00
         end do
         end do
         end do
      else
         do k = 1, kt
         do j = 1, jt
         do i = 1, it
            Src(i,j,k,1) = Srcx(i,j,k)
     $                     + Sigs(i,j,k,1)*Flux(i,j,k,1)
            Src(i,j,k,2) = Sigs(i,j,k,2)*Flux(i,j,k,2)
            Src(i,j,k,3) = Sigs(i,j,k,2)*Flux(i,j,k,3)
            Src(i,j,k,4) = Sigs(i,j,k,2)*Flux(i,j,k,4)
            Pflux(i,j,k) = Flux(i,j,k,1)
            Flux(i,j,k,1) = 0.0d+00
            Flux(i,j,k,2) = 0.0d+00
            Flux(i,j,k,3) = 0.0d+00
            Flux(i,j,k,4) = 0.0d+00
         end do
         end do
         end do
      endif

      return
      end
