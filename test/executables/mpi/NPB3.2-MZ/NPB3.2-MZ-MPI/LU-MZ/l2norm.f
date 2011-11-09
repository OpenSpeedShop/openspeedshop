
c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine l2norm (v, sum, nx, nxmax, ny, nz)
c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c   to compute the l2-norm of vector v.
c---------------------------------------------------------------------

      implicit none

c---------------------------------------------------------------------
c  input parameters
c---------------------------------------------------------------------
      integer nx, nxmax, ny, nz
      double precision  v(5,nxmax,ny,nz), sum(5)

c---------------------------------------------------------------------
c  local variables
c---------------------------------------------------------------------
      integer i, j, k, m
      double precision  sum_cap(5)


      do m = 1, 5
         sum(m) = 0.0d+00
      end do

!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(sum_cap,m,i,j,k)
!$OMP&  SHARED(nx,ny,nz)
      do m=1,5
         sum_cap(m)=0.0d0
      enddo
      do k = 2, nz-1
!$OMP DO SCHEDULE(STATIC)
         do j = 2, ny-1
            do i = 2, nx-1
               do m = 1, 5
                  sum_cap(m) = sum_cap(m) + v(m,i,j,k) * v(m,i,j,k)
               end do
            end do
         end do
!$OMP END DO nowait
      end do
      do m=1,5
!$OMP ATOMIC
         sum(m)=sum(m)+sum_cap(m)
      enddo
!$OMP END PARALLEL

      do m = 1, 5
         sum(m) = dsqrt(sum(m) / (dble(nz-2)*dble(ny-2)*dble(nx-2)))
      end do

      return
      end
