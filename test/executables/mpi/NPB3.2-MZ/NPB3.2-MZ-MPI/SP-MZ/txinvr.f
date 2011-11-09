
c---------------------------------------------------------------------
c---------------------------------------------------------------------

       subroutine  txinvr(rho_i, us, vs, ws, speed, qs, rhs,
     $                    nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c block-diagonal matrix-vector multiplication                  
c---------------------------------------------------------------------

       include 'header.h'

       integer nx, nxmax, ny, nz
       double precision rho_i(  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  us   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  vs   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  ws   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  speed(  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  qs   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  rhs  (5,0:nxmax-1,0:ny-1,0:nz-1)

       integer i, j, k
       double precision t1, t2, t3, ac, ru1, uu, vv, ww, r1, r2, r3, 
     >                  r4, r5, ac2inv

!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(t3,t2,t1,r5,r4,r3,r2,r1,
!$OMP& ac2inv,ac,ww,vv,uu,ru1,i,j,k)
!$OMP&  SHARED(bt,c2,nx,ny,nz)
       do    k = 1, nz-2
          do    j = 1, ny-2
             do    i = 1, nx-2

                ru1 = rho_i(i,j,k)
                uu = us(i,j,k)
                vv = vs(i,j,k)
                ww = ws(i,j,k)
                ac = speed(i,j,k)
                ac2inv = 1.0d0 / ( ac*ac )

                r1 = rhs(1,i,j,k)
                r2 = rhs(2,i,j,k)
                r3 = rhs(3,i,j,k)
                r4 = rhs(4,i,j,k)
                r5 = rhs(5,i,j,k)

                t1 = c2 * ac2inv * ( qs(i,j,k)*r1 - uu*r2  - 
     >                  vv*r3 - ww*r4 + r5 )
                t2 = bt * ru1 * ( uu * r1 - r2 )
                t3 = ( bt * ru1 * ac ) * t1

                rhs(1,i,j,k) = r1 - t1
                rhs(2,i,j,k) = - ru1 * ( ww*r1 - r4 )
                rhs(3,i,j,k) =   ru1 * ( vv*r1 - r3 )
                rhs(4,i,j,k) = - t2 + t3
                rhs(5,i,j,k) =   t2 + t3

             end do
          end do
       end do
!$OMP END PARALLEL DO

       return
       end


