
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine setbv(u, nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c   set the boundary values of dependent variables
c---------------------------------------------------------------------

      implicit none

      include 'header.h'
      integer nx, nxmax, ny, nz
      double precision u(5,nxmax,ny,nz)

c---------------------------------------------------------------------
c   local variables
c---------------------------------------------------------------------
      integer i, j, k, m
      double precision temp1(5), temp2(5)

!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(k,m,temp2,temp1,i,j)
!$OMP&  SHARED(nz,nx,ny)
c---------------------------------------------------------------------
c   set the dependent variable values along the top and bottom faces
c---------------------------------------------------------------------
!$OMP DO SCHEDULE(STATIC)
      do j = 1, ny
         do i = 1, nx
            call exact( i, j, 1, temp1, nx, ny, nz )
            call exact( i, j, nz, temp2, nx, ny, nz )
            do m = 1, 5
               u( m, i, j, 1 ) = temp1(m)
               u( m, i, j, nz ) = temp2(m)
            end do
         end do
      end do
!$OMP END DO

c---------------------------------------------------------------------
c   set the dependent variable values along north and south faces
c---------------------------------------------------------------------
!$OMP DO SCHEDULE(STATIC)
      do k = 1, nz
         do i = 1, nx
            call exact( i, 1, k, temp1, nx, ny, nz )
            call exact( i, ny, k, temp2, nx, ny, nz )
            do m = 1, 5
               u( m, i, 1, k ) = temp1(m)
               u( m, i, ny, k ) = temp2(m)
            end do
         end do
      end do
!$OMP END DO

c---------------------------------------------------------------------
c   set the dependent variable values along east and west faces
c---------------------------------------------------------------------
      do k = 1, nz
!$OMP DO SCHEDULE(STATIC)
         do j = 1, ny
            call exact( 1, j, k, temp1, nx, ny, nz )
            call exact( nx, j, k, temp2, nx, ny, nz )
            do m = 1, 5
               u( m, 1, j, k ) = temp1(m)
               u( m, nx, j, k ) = temp2(m)
            end do
         end do
!$OMP END DO nowait
      end do
!$OMP END PARALLEL

      return
      end
