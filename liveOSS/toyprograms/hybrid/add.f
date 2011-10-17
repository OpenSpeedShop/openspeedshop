c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine  add(u, rhs, nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c     addition of update to the vector u
c---------------------------------------------------------------------

      include 'header.h'

      integer nx, nxmax, ny, nz
      double precision rhs(5,0:nxmax-1,0:ny-1,0:nz-1), 
     $                 u  (5,0:nxmax-1,0:ny-1,0:nz-1)

      integer i, j, k, m

      if (timeron) call timer_start(t_add)
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(m,i,j,k)
!$OMP&  SHARED(nx,ny,nz)
      do     k = 1, nz-2
         do     j = 1, ny-2
            do     i = 1, nx-2
               do    m = 1, 5
                  u(m,i,j,k) = u(m,i,j,k) + rhs(m,i,j,k)
               enddo
            enddo
         enddo
      enddo
!$OMP END PARALLEL DO
      if (timeron) call timer_stop(t_add)

      return
      end
