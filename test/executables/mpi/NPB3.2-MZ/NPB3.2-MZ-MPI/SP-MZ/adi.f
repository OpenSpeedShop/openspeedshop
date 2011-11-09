
c---------------------------------------------------------------------
c---------------------------------------------------------------------

       subroutine  adi(rho_i, us, vs, ws, speed, qs, square, 
     $                 rhs, forcing, u, nx, nxmax, ny, nz)

       include 'header.h'

       integer nx, nxmax, ny, nz
       double precision rho_i  (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  us     (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  vs     (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  ws     (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  speed  (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  qs     (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  square (  0:nxmax-1,0:ny-1,0:nz-1), 
     $                  rhs    (5,0:nxmax-1,0:ny-1,0:nz-1), 
     $                  forcing(5,0:nxmax-1,0:ny-1,0:nz-1), 
     $                  u      (5,0:nxmax-1,0:ny-1,0:nz-1)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

       if (timeron) call timer_start(t_rhs)
       call compute_rhs(rho_i, us, vs, ws, speed, qs, square,
     $                  rhs, forcing, u, nx, nxmax, ny, nz)
       if (timeron) call timer_stop(t_rhs)

       if (timeron) call timer_start(t_txinvr)
       call txinvr(rho_i, us, vs, ws, speed, qs, rhs, 
     $             nx, nxmax, ny, nz)
       if (timeron) call timer_stop(t_txinvr)

       call x_solve(rho_i, us, speed, rhs, nx, nxmax, ny, nz)

       call y_solve(rho_i, vs, speed, rhs, nx, nxmax, ny, nz)

       call z_solve(rho_i, us, vs, ws, speed, qs, u, rhs, 
     $              nx, nxmax, ny, nz)

       if (timeron) call timer_start(t_add)
       call add(u, rhs, nx, nxmax, ny, nz)
       if (timeron) call timer_stop(t_add)

       return
       end

