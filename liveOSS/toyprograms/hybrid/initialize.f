c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine  initialize(u, nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c     This subroutine initializes the field variable u using 
c     tri-linear transfinite interpolation of the boundary values     
c---------------------------------------------------------------------

      include 'header.h'
      
      integer nx, nxmax, ny, nz
      double precision u(5,0:nxmax-1,0:ny-1,0:nz-1)
      integer i, j, k, m, ix, iy, iz
      double precision  xi, eta, zeta, Pface(5,3,2), Pxi, Peta, 
     >     Pzeta, temp(5)

c---------------------------------------------------------------------
c  Later (in compute_rhs) we compute 1/u for every element. A few of 
c  the corner elements are not used, but it convenient (and faster) 
c  to compute the whole thing with a simple loop. Make sure those 
c  values are nonzero by initializing the whole thing here. 
c---------------------------------------------------------------------
!$OMP PARALLEL DEFAULT(SHARED) PRIVATE(temp,Pzeta,Peta,Pxi,m,iz,iy,
!$OMP& Pface,ix,xi,i,eta,j,zeta,k)
!$OMP&  SHARED(dnxm1,nx,dnym1,ny,dnzm1,nz)
!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         do j = 0, ny-1
            do i = 0, nx-1
               do m = 1, 5
                  u(m,i,j,k) = 1.0
               end do
            end do
         end do
      end do
!$OMP END DO nowait
c---------------------------------------------------------------------



c---------------------------------------------------------------------
c     first store the "interpolated" values everywhere on the zone    
c---------------------------------------------------------------------

!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         zeta = dble(k) * dnzm1
         do j = 0, ny-1
            eta = dble(j) * dnym1
            do i = 0, nx-1
               xi = dble(i) * dnxm1
                  
               do ix = 1, 2
                  call exact_solution(dble(ix-1), eta, zeta, 
     >                    Pface(1,1,ix))
               enddo

               do iy = 1, 2
                  call exact_solution(xi, dble(iy-1) , zeta, 
     >                    Pface(1,2,iy))
               enddo

               do iz = 1, 2
                  call exact_solution(xi, eta, dble(iz-1),   
     >                    Pface(1,3,iz))
               enddo

               do m = 1, 5
                  Pxi   = xi   * Pface(m,1,2) + 
     >                    (1.0d0-xi)   * Pface(m,1,1)
                  Peta  = eta  * Pface(m,2,2) + 
     >                    (1.0d0-eta)  * Pface(m,2,1)
                  Pzeta = zeta * Pface(m,3,2) + 
     >                    (1.0d0-zeta) * Pface(m,3,1)
                     
                  u(m,i,j,k) = Pxi + Peta + Pzeta - 
     >                    Pxi*Peta - Pxi*Pzeta - Peta*Pzeta + 
     >                    Pxi*Peta*Pzeta

               enddo
            enddo
         enddo
      enddo
!$OMP END DO nowait

c---------------------------------------------------------------------
c     now store the exact values on the boundaries        
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c     west face                                                  
c---------------------------------------------------------------------
      i = 0
      xi = 0.0d0
!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         zeta = dble(k) * dnzm1
         do j = 0, ny-1
            eta = dble(j) * dnym1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO nowait

c---------------------------------------------------------------------
c     east face                                                      
c---------------------------------------------------------------------

      i = nx-1
      xi = 1.0d0
!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         zeta = dble(k) * dnzm1
         do j = 0, ny-1
            eta = dble(j) * dnym1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO nowait

c---------------------------------------------------------------------
c     south face                                                 
c---------------------------------------------------------------------
      j = 0
      eta = 0.0d0
!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         zeta = dble(k) * dnzm1
         do i = 0, nx-1
            xi = dble(i) * dnxm1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO nowait


c---------------------------------------------------------------------
c     north face                                    
c---------------------------------------------------------------------
      j = ny-1
      eta = 1.0d0
!$OMP DO SCHEDULE(STATIC)
      do k = 0, nz-1
         zeta = dble(k) * dnzm1
         do i = 0, nx-1
            xi = dble(i) * dnxm1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO

c---------------------------------------------------------------------
c     bottom face                                       
c---------------------------------------------------------------------
      k = 0
      zeta = 0.0d0
!$OMP DO SCHEDULE(STATIC)
      do j = 0, ny-1
         eta = dble(j) * dnym1
         do i =0, nx-1
            xi = dble(i) *dnxm1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO nowait

c---------------------------------------------------------------------
c     top face     
c---------------------------------------------------------------------
      k = nz-1
      zeta = 1.0d0
!$OMP DO SCHEDULE(STATIC)
      do j = 0, ny-1
         eta = dble(j) * dnym1
         do i =0, nx-1
            xi = dble(i) * dnxm1
            call exact_solution(xi, eta, zeta, temp)
            do m = 1, 5
               u(m,i,j,k) = temp(m)
            enddo
         enddo
      enddo
!$OMP END DO nowait
!$OMP END PARALLEL

      return
      end


c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine lhsinit(lhs, size)
      implicit none
      integer size
      double precision lhs(5,5,3,0:size)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

      integer i, m, n

      i = size
c---------------------------------------------------------------------
c     zero the whole left hand side for starters
c---------------------------------------------------------------------
      do m = 1, 5
         do n = 1, 5
            lhs(m,n,1,0) = 0.0d0
            lhs(m,n,2,0) = 0.0d0
            lhs(m,n,3,0) = 0.0d0
            lhs(m,n,1,i) = 0.0d0
            lhs(m,n,2,i) = 0.0d0
            lhs(m,n,3,i) = 0.0d0
         enddo
      enddo

c---------------------------------------------------------------------
c     next, set all diagonal values to 1. This is overkill, but convenient
c---------------------------------------------------------------------
      do m = 1, 5
         lhs(m,m,2,0) = 1.0d0
         lhs(m,m,2,i) = 1.0d0
      enddo

      return
      end



