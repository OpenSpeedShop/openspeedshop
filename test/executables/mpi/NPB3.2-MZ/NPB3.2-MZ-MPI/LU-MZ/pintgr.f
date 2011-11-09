
c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine pintgr(u, phi1, phi2, frc, nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

      implicit none

      include 'header.h'
      integer          nx, nxmax, ny, nz
      double precision u(5,nxmax,ny,nz), frc,
     $                 phi1(problem_size,problem_size),
     $                 phi2(problem_size,problem_size)

c---------------------------------------------------------------------
c  local variables
c---------------------------------------------------------------------
      integer i, j, k
      double precision  frc1, frc2, frc3

c---------------------------------------------------------------------
c   initialize
c---------------------------------------------------------------------
!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(k,i,j)
!$OMP&  SHARED(nz,nx,ny)
!$OMP DO
      do j = 1,ny
        do i = 1,nx
          phi1(i,j) = 0.d0
          phi2(i,j) = 0.d0
        end do
      end do
!$OMP END DO

!$OMP DO
      do j = 2,ny-2
         do i = 2,nx-1

            k = 3

            phi1(i,j) = c2*(u(5,i,j,k) -
     >             0.5d0 * (u(2,i,j,k)**2 + u(3,i,j,k)**2 +
     >                      u(4,i,j,k)**2) / u(1,i,j,k) )

            k = nz-1

            phi2(i,j) = c2*(u(5,i,j,k) -
     >             0.5d0 * (u(2,i,j,k)**2 + u(3,i,j,k)**2 +
     >                      u(4,i,j,k)**2) / u(1,i,j,k) )
         end do
      end do
!$OMP END DO nowait

!$OMP SINGLE
      frc1 = 0.0d0
!$OMP END SINGLE

!$OMP DO REDUCTION(+:frc1)
      do j = 2,ny-3
         do i = 2, nx-2
            frc1 = frc1 + (phi1(i,j)   + phi1(i+1,j)   +
     >                     phi1(i,j+1) + phi1(i+1,j+1) +
     >                     phi2(i,j)   + phi2(i+1,j)   +
     >                     phi2(i,j+1) + phi2(i+1,j+1))
         end do
      end do
!$OMP END DO nowait
!$OMP END PARALLEL

      frc1 = dxi * deta * frc1

c---------------------------------------------------------------------
c   initialize
c---------------------------------------------------------------------
!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(i,k)
!$OMP&  SHARED(ny,nx,nz)
!$OMP DO
      do k = 1,nz
        do i = 1,nx
          phi1(i,k) = 0.
          phi2(i,k) = 0.
        end do
      end do
!$OMP END DO

!$OMP DO
      do k = 3, nz-1
         do i = 2, nx-1
            phi1(i,k) = c2*(u(5,i,2,k) -
     >             0.5d0 * (u(2,i,2,k)**2 + u(3,i,2,k)**2 +
     >                      u(4,i,2,k)**2 ) / u(1,i,2,k) )
         end do
      end do
!$OMP END DO nowait

!$OMP DO
      do k = 3, nz-1
         do i = 2, nx-1
            phi2(i,k) = c2*(u(5,i,ny-2,k) -
     >             0.5d0 * (u(2,i,ny-2,k)**2 + u(3,i,ny-2,k)**2 +
     >                      u(4,i,ny-2,k)**2 ) / u(1,i,ny-2,k) )
         end do
      end do
!$OMP END DO nowait


!$OMP SINGLE
      frc2 = 0.0d0
!$OMP END SINGLE

!$OMP DO REDUCTION(+:frc2)
      do k = 3, nz-2
         do i = 2, nx-2
            frc2 = frc2 + (phi1(i,k)   + phi1(i+1,k)   +
     >                     phi1(i,k+1) + phi1(i+1,k+1) +
     >                     phi2(i,k)   + phi2(i+1,k)   +
     >                     phi2(i,k+1) + phi2(i+1,k+1))
         end do
      end do
!$OMP END DO nowait
!$OMP END PARALLEL


      frc2 = dxi * dzeta * frc2

c---------------------------------------------------------------------
c   initialize
c---------------------------------------------------------------------
!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(j,k)
!$OMP&  SHARED(nx,ny,nz)
!$OMP DO
      do k = 1,nz
        do j = 1,ny
          phi1(j,k) = 0.d0
          phi2(j,k) = 0.d0
        end do
      end do
!$OMP END DO

!$OMP DO
      do k = 3, nz-1
         do j = 2, ny-2
            phi1(j,k) = c2*(u(5,2,j,k) -
     >             0.5d0 * (u(2,2,j,k)**2 + u(3,2,j,k)**2 + 
     >                      u(4,2,j,k)**2)   / u(1,2,j,k) )
         end do
      end do
!$OMP END DO nowait

!$OMP DO
      do k = 3, nz-1
         do j = 2, ny-2
            phi2(j,k) = c2*(u(5,nx-1,j,k) -
     >             0.5d0 * (u(2,nx-1,j,k)**2 + u(3,nx-1,j,k)**2 +
     >                      u(4,nx-1,j,k)**2)  / u(1,nx-1,j,k) )
         end do
      end do
!$OMP END DO nowait

!$OMP SINGLE
      frc3 = 0.0d0
!$OMP END SINGLE

!$OMP DO REDUCTION(+:frc3)
      do k = 3, nz-2
         do j = 2, ny-3
            frc3 = frc3 + (phi1(j,k)   + phi1(j+1,k)   +
     >                     phi1(j,k+1) + phi1(j+1,k+1) +
     >                     phi2(j,k)   + phi2(j+1,k)   +
     >                     phi2(j,k+1) + phi2(j+1,k+1))
         end do
      end do
!$OMP END DO nowait
!$OMP END PARALLEL


      frc3 = deta * dzeta * frc3
      frc = 0.25d0 * ( frc1 + frc2 + frc3 )

      return
      end
