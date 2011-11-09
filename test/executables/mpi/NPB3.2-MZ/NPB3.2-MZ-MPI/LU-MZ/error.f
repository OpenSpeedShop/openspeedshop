c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine error(u, errnm, nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c
c   compute the solution error
c
c---------------------------------------------------------------------

      implicit none

      integer nx, nxmax, ny, nz
      double precision u(5,nxmax,ny,nz), errnm(5)

c---------------------------------------------------------------------
c  local variables
c---------------------------------------------------------------------
      integer i, j, k, m
      double precision  tmp
      double precision  u000ijk(5)
      double precision  errnm_cap(5)

      do m = 1, 5
         errnm(m) = 0.0d0
      end do

!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(errnm_cap,tmp,m,u000ijk,i,j,k)
!$OMP&  SHARED(nx,ny,nz)
      do m=1,5
         errnm_cap(m)=0.0d0
      enddo
      do k = 2, nz-1
!$OMP DO SCHEDULE(STATIC)
         do j = 2, ny-1
            do i = 2, nx-1
               call exact( i, j, k, u000ijk, nx, ny, nz )
               do m = 1, 5
                  tmp = ( u000ijk(m) - u(m,i,j,k) )
                  errnm_cap(m) = errnm_cap(m) + tmp ** 2
               end do
            end do
         end do
!$OMP END DO nowait
      end do
      do m=1,5
!$OMP ATOMIC
         errnm(m)=errnm(m)+errnm_cap(m)
      enddo
!$OMP END PARALLEL

      do m = 1, 5
         errnm(m) = dsqrt(errnm(m) / (dble(nz-2)*dble(ny-2)*dble(nx-2)))
      end do

      return
      end
