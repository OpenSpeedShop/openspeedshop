
      subroutine flux_err(it,jt,kt,nm, Flux,Pflux, err)
c
c Compute max relative flux error
c
      implicit none

      integer it,jt,kt,nm
      double precision Flux(it,jt,kt,nm)
      double precision Pflux(it,jt,kt)
      double precision err

      integer i,j,k
      double precision yyy

      err = 0.d+00
      do k = 1, kt
      do j = 1, jt
      do i = 1, it

        if (Flux(i,j,k,1).ne.0.0d+0) then

          yyy = abs( (Flux(i,j,k,1)-Pflux(i,j,k))/Flux(i,j,k,1) )
          err = max( err,yyy )

        endif

      end do
      end do
      end do

c determine global max value (goes into err):

      call global_real_max(err)

      return
      end
