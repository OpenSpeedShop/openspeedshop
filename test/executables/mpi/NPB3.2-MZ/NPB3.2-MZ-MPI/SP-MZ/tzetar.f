
c---------------------------------------------------------------------
c---------------------------------------------------------------------

       subroutine  tzetar(us, vs, ws, speed, qs, u, rhs, 
     $                    nx, nxmax, ny, nz)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c   block-diagonal matrix-vector multiplication                       
c---------------------------------------------------------------------

       include 'header.h'

       integer nx, nxmax, ny, nz
       double precision us   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  vs   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  ws   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  speed(  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  qs   (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  rhs  (5,0:nxmax-1,0:ny-1,0:nz-1),
     $                  u    (5,0:nxmax-1,0:ny-1,0:nz-1)

       integer i, j, k
       double precision  t1, t2, t3, ac, xvel, yvel, zvel, r1, r2, r3, 
     >                   r4, r5, btuz, ac2u, uzik1

!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(t3,t2,t1,btuz,uzik1,r5,r4,r3,
!$OMP& r2,r1,ac2u,ac,zvel,yvel,xvel,i,j,k)
!$OMP&  SHARED(c2iv,bt,nx,ny,nz)
       do    k = 1, nz-2
          do    j = 1, ny-2
             do    i = 1, nx-2

                xvel = us(i,j,k)
                yvel = vs(i,j,k)
                zvel = ws(i,j,k)
                ac   = speed(i,j,k)

                ac2u = ac*ac

                r1 = rhs(1,i,j,k)
                r2 = rhs(2,i,j,k)
                r3 = rhs(3,i,j,k)
                r4 = rhs(4,i,j,k)
                r5 = rhs(5,i,j,k)      

                uzik1 = u(1,i,j,k)
                btuz  = bt * uzik1

                t1 = btuz/ac * (r4 + r5)
                t2 = r3 + t1
                t3 = btuz * (r4 - r5)

                rhs(1,i,j,k) = t2
                rhs(2,i,j,k) = -uzik1*r2 + xvel*t2
                rhs(3,i,j,k) =  uzik1*r1 + yvel*t2
                rhs(4,i,j,k) =  zvel*t2  + t3
                rhs(5,i,j,k) =  uzik1*(-xvel*r2 + yvel*r1) + 
     >                    qs(i,j,k)*t2 + c2iv*ac2u*t1 + zvel*t3

             end do
          end do
       end do
!$OMP END PARALLEL DO

       return
       end
