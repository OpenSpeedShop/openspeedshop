!-------------------------------------------------------------------------!
!                                                                         !
!        N  A  S     P A R A L L E L     B E N C H M A R K S  3.2         !
!                                                                         !
!             M P I    M U L T I - Z O N E    V E R S I O N               !
!                                                                         !
!                           S P - M Z - M P I                             !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    This benchmark is a serial version of the NPB SP code.               !
!    Refer to NAS Technical Reports 95-020 and 99-011 for details.        !
!                                                                         !
!    Permission to use, copy, distribute and modify this software         !
!    for any purpose with or without fee is hereby granted.  We           !
!    request, however, that all derived work reference the NAS            !
!    Parallel Benchmarks 3.2. This software is provided "as is"           !
!    without express or implied warranty.                                 !
!                                                                         !
!    Information on NPB 3.2, including the technical report, the          !
!    original specifications, source code, results and information        !
!    on how to submit new results, is available at:                       !
!                                                                         !
!           http://www.nas.nasa.gov/Software/NPB/                         !
!                                                                         !
!    Send comments or suggestions to  npb@nas.nasa.gov                    !
!                                                                         !
!          NAS Parallel Benchmarks Group                                  !
!          NASA Ames Research Center                                      !
!          Mail Stop: T27A-1                                              !
!          Moffett Field, CA   94035-1000                                 !
!                                                                         !
!          E-mail:  npb@nas.nasa.gov                                      !
!          Fax:     (650) 604-3957                                        !
!                                                                         !
!-------------------------------------------------------------------------!


c---------------------------------------------------------------------
c
c Author: R. Van der Wijngaart
c         W. Saphir
c         H. Jin
c---------------------------------------------------------------------

c---------------------------------------------------------------------
       program SP
c---------------------------------------------------------------------

       include  'header.h'
       include  'mpi_stuff.h'
      
       integer num_zones
       parameter (num_zones=x_zones*y_zones)

       integer   nx(num_zones), nxmax(num_zones), ny(num_zones), 
     $           nz(num_zones)
       dimension start1(num_zones), start5(num_zones),
     $           qstart_west (num_zones), qstart_east (num_zones),
     $           qstart_south(num_zones), qstart_north(num_zones)

c---------------------------------------------------------------------
c   Define all field arrays as one-dimenional arrays, to be reshaped
c---------------------------------------------------------------------
       double precision 
     >   u       (proc_max_size5),
     >   us      (proc_max_size ),
     >   vs      (proc_max_size ),
     >   ws      (proc_max_size ),
     >   qs      (proc_max_size ),
     >   rho_i   (proc_max_size ),
     >   speed   (proc_max_size ),
     >   square  (proc_max_size ),
     >   rhs     (proc_max_size5),
     >   forcing (proc_max_size5),
     >   qbc_ou  (proc_max_bcsize), 
     >   qbc_in  (proc_max_bcsize)

       common /fields/ u, us, vs, ws, qs, rho_i, speed, square, 
     >                 rhs, forcing, qbc_ou, qbc_in

       integer          i, niter, step, fstatus, zone, 
     >                  iz, ip, tot_threads, itimer
       external         timer_read
       double precision mflops, nsur, navg, n3,
     >                  t, tmax, timer_read, trecs(t_last)
       logical          verified
       character        t_names(t_last)*8


       call mpi_setup
       if (.not. active) goto 999

c---------------------------------------------------------------------
c      Read input file (if it exists), else take
c      defaults from parameters
c---------------------------------------------------------------------
       if (myid .eq. root) then

         write(*, 1000)
         open (unit=2,file='inputsp-mz.data',status='old', 
     >         iostat=fstatus)

         timeron = .false.
         if (fstatus .eq. 0) then
           write(*,*) 'Reading from input file inputsp-mz.data'
           read (2,*) niter
           read (2,*) dt
           read (2,*) itimer
           close(2)

           if (niter .eq. 0)  niter = niter_default
           if (dt .eq. 0.d0)  dt    = dt_default
           if (itimer .gt. 0) timeron = .true.

         else
           niter = niter_default
           dt    = dt_default
         endif

         write(*, 1001) x_zones, y_zones
         write(*, 1002) niter, dt
         write(*, 1003) num_procs
       endif
 1000  format(//,' NAS Parallel Benchmarks (NPB3.2-MZ-MPI)',
     >          ' - SP-MZ MPI+OpenMP Benchmark', /)
 1001  format(' Number of zones: ', i3, ' x ', i3)
 1002  format(' Iterations:      ', i3, '    dt: ', f10.6)
 1003  format(' Number of active processes: ', i5/)

       call mpi_bcast(niter, 1, MPI_INTEGER,
     >                root, comm_setup, ierror)
       call mpi_bcast(dt, 1, dp_type,
     >                root, comm_setup, ierror)
       call mpi_bcast(timeron, 1, MPI_LOGICAL,
     >                root, comm_setup, ierror)

       if (timeron) then
         t_names(t_total) = 'total'
         t_names(t_rhsx) = 'rhsx'
         t_names(t_rhsy) = 'rhsy'
         t_names(t_rhsz) = 'rhsz'
         t_names(t_rhs) = 'rhs'
         t_names(t_xsolve) = 'xsolve'
         t_names(t_ysolve) = 'ysolve'
         t_names(t_zsolve) = 'zsolve'
         t_names(t_rdis1) = 'qbc_copy'
         t_names(t_rdis2) = 'qbc_comm'
         t_names(t_tzetar) = 'tzetar'
         t_names(t_ninvr) = 'ninvr'
         t_names(t_pinvr) = 'pinvr'
         t_names(t_txinvr) = 'txinvr'
         t_names(t_add) = 'add'
       endif

       call env_setup(tot_threads)

       call zone_setup(nx, nxmax, ny, nz)

       call map_zones(num_zones, nx, ny, nz, tot_threads)
       call zone_starts(num_zones, nx, nxmax, ny, nz, start1, start5, 
     $                  qstart_west,  qstart_east, 
     $                  qstart_south, qstart_north)

       call set_constants

       do iz = 1, proc_num_zones
         zone = proc_zone_id(iz)
         call exact_rhs(forcing(start5(iz)), 
     $                  nx(zone), nxmax(zone), ny(zone), nz(zone))
         call initialize(u(start5(iz)), 
     $                   nx(zone), nxmax(zone), ny(zone), nz(zone))
       end do


       do i = 1, t_last
          call timer_clear(i)
       end do

c---------------------------------------------------------------------
c      do one time step to touch all code, and reinitialize
c---------------------------------------------------------------------

       call exch_qbc(u, qbc_ou, qbc_in, nx, nxmax, ny, nz, 
     $               start5, qstart_west, qstart_east, 
     $               qstart_south, qstart_north, npb_verbose)


       do iz = 1, proc_num_zones
         zone = proc_zone_id(iz)

         call adi(rho_i(start1(iz)), us(start1(iz)), 
     $            vs(start1(iz)), ws(start1(iz)), 
     $            speed(start1(iz)), qs(start1(iz)), 
     $            square(start1(iz)), rhs(start5(iz)), 
     $            forcing(start5(iz)), u(start5(iz)),
     $            nx(zone), nxmax(zone), ny(zone), nz(zone))
       end do

       do iz = 1, proc_num_zones
         zone = proc_zone_id(iz)
         call initialize(u(start5(iz)), 
     $                   nx(zone), nxmax(zone), ny(zone), nz(zone))
       end do

       do i = 1, t_last
          call timer_clear(i)
       end do

       call mpi_barrier(comm_setup, ierror)

       call timer_start(1)

c---------------------------------------------------------------------
c      start the benchmark time step loop
c---------------------------------------------------------------------

      do  step = 1, niter

         if (mod(step, 20) .eq. 0 .or. step .eq. 1) then
            if (myid .eq. root) write(*, 200) step
 200        format(' Time step ', i4)
         endif

         call exch_qbc(u, qbc_ou, qbc_in, nx, nxmax, ny, nz, 
     $                 start5, qstart_west, qstart_east, 
     $                 qstart_south, qstart_north, 0)

         do iz = 1, proc_num_zones
           zone = proc_zone_id(iz)
           call adi(rho_i(start1(iz)), us(start1(iz)), 
     $              vs(start1(iz)), ws(start1(iz)), 
     $              speed(start1(iz)), qs(start1(iz)), 
     $              square(start1(iz)), rhs(start5(iz)), 
     $              forcing(start5(iz)), u(start5(iz)),
     $              nx(zone), nxmax(zone), ny(zone), nz(zone))
         end do

       end do

       call timer_stop(1)
       tmax = timer_read(1)

c---------------------------------------------------------------------
c      perform verification and print results
c---------------------------------------------------------------------
       
       call verify(niter, verified, num_zones,
     $             rho_i, us, vs, ws, speed, qs, square, rhs, 
     $             forcing, u, nx, nxmax, ny, nz, start1, start5)

       t = tmax
       call mpi_reduce(t, tmax, 1, dp_type, MPI_MAX, 
     >                 root, comm_setup, ierror)

       if (myid .ne. root) goto 900

       mflops = 0.0d0
       if( tmax .ne. 0. ) then
          do zone = 1, num_zones
            n3 = dble(nx(zone))*ny(zone)*nz(zone)
            navg = (nx(zone) + ny(zone) + nz(zone))/3.0
            nsur = (nx(zone)*ny(zone) + nx(zone)*nz(zone) +
     >              ny(zone)*nz(zone))/3.0
            mflops = mflops + float( niter ) * 1.0d-6 *
     >              (881.174d0 * n3 - 4683.91d0 * nsur
     >               + 11484.5d0 * navg - 19272.4d0)
     >              / tmax
          end do
       endif

       call print_results('SP-MZ', class, gx_size, gy_size, gz_size, 
     >                   niter, tmax, mflops, num_procs, tot_threads,
     >                   '          floating point', 
     >                   verified, npbversion,compiletime, cs1, cs2, 
     >                   cs3, cs4, cs5, cs6, '(none)')

c---------------------------------------------------------------------
c      More timers
c---------------------------------------------------------------------
 900   if (.not.timeron) goto 999

       do i=1, t_last
          trecs(i) = timer_read(i)
       end do

       if (myid .gt. 0) then
          call mpi_recv(i, 1, MPI_INTEGER, 0, 1000, 
     >                  comm_setup, statuses, ierror)
          call mpi_send(trecs, t_last, dp_type, 0, 1001, 
     >                  comm_setup, ierror)
          goto 999
       endif

       ip = 0
       if (tmax .eq. 0.0) tmax = 1.0
 910   write(*,800) ip, proc_num_threads(ip+1)
 800   format(' Myid =',i5,'   num_threads =',i4/
     >        '  SECTION   Time (secs)')
       do i=1, t_last
          write(*,810) t_names(i), trecs(i), trecs(i)*100./tmax
          if (i.eq.t_rhs) then
             t = trecs(t_rhsx) + trecs(t_rhsy) + trecs(t_rhsz)
             write(*,820) 'sub-rhs', t, t*100./tmax
             t = trecs(t_rhs) - t
             write(*,820) 'rest-rhs', t, t*100./tmax
          elseif (i.eq.t_rdis2) then
             t = trecs(t_rdis1) + trecs(t_rdis2)
             write(*,820) 'exch_qbc', t, t*100./tmax
          endif
 810      format(2x,a8,':',f9.3,'  (',f6.2,'%)')
 820      format('    --> total ',a8,':',f9.3,'  (',f6.2,'%)')
       end do

       ip = ip + 1
       if (ip .lt. num_procs) then
          call mpi_send(myid, 1, MPI_INTEGER, ip, 1000, 
     >                  comm_setup, ierror)
          call mpi_recv(trecs, t_last, dp_type, ip, 1001, 
     >                  comm_setup, statuses, ierror)
          write(*,*)
          goto 910
       endif

 999   continue
       call mpi_barrier(MPI_COMM_WORLD, ierror)
       call mpi_finalize(ierror)

       end
