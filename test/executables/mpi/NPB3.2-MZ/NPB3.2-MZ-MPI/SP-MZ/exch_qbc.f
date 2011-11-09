       subroutine exch_qbc(u, qbc_ou, qbc_in, nx, nxmax, ny, nz, 
     $                     start5, qstart_west, qstart_east, 
     $                     qstart_south, qstart_north, iprn_msg)

       include 'header.h'
       include 'mpi_stuff.h'

       dimension start5(*), qstart_west(*),  qstart_east(*), 
     $           qstart_south(*), qstart_north(*)
       integer   nx(*), nxmax(*), ny(*), nz(*), iprn_msg
       double precision u(*), qbc_ou(*), qbc_in(*)

       integer   nnx, nnxmax, nny, nnz, zone_no, iz, ip, ig,
     $           ip_west,    ip_east,    ip_south,    ip_north, 
     $           izone_west, izone_east, jzone_south, jzone_north,
     $           nr, b_size1, b_size2, c_size, m_size, num_msgs, 
     $           n, tag, iodd
       integer   MSG_TAG
       parameter (MSG_TAG=10000)


c      copy data to qbc buffer
       if (timeron) call timer_start(t_rdis1)
       do iz = 1, proc_num_zones
           zone_no = proc_zone_id(iz)
           nnx    = nx(zone_no)
           nnxmax = nxmax(zone_no)
           nny    = ny(zone_no)
           nnz    = nz(zone_no)

           call copy_x_face(u(start5(iz)),
     $                      qbc_ou(qstart_west(zone_no)),
     $                      nnx, nnxmax, nny, nnz, 1, 'out')

           call copy_x_face(u(start5(iz)),
     $                      qbc_ou(qstart_east(zone_no)),
     $                      nnx, nnxmax, nny, nnz, nnx-2, 'out')


           call copy_y_face(u(start5(iz)),
     $                      qbc_ou(qstart_south(zone_no)),
     $                      nnx, nnxmax, nny, nnz, 1, 'out')

           call copy_y_face(u(start5(iz)),
     $                      qbc_ou(qstart_north(zone_no)),
     $                      nnx, nnxmax, nny, nnz, nny-2, 'out')

       end do
       if (timeron) call timer_stop(t_rdis1)


c      exchange qbc buffers
       if (timeron) call timer_start(t_rdis2)

       do ig = 1, num_procs
          ip = pcomm_group(ig)

          if (mod(myid, 2).eq.0) then
             iodd = 0
             if (mod(ip, 2).eq.0 .and. myid .gt. ip) iodd = 1
          else
             iodd = 1
             if (mod(ip, 2).eq.1 .and. myid .gt. ip) iodd = 0
          endif

          if (ip .eq. 0) then
             c_size = qcomm_size(ip+1)
          else
             c_size = qcomm_size(ip+1) - qcomm_size(ip)
          endif

          nr = 0
          if (c_size .gt. 0) then
             num_msgs = c_size / MSG_SIZE
             if (num_msgs .eq. 0) num_msgs = 1
             m_size = (c_size + num_msgs - 1)/ num_msgs

             if (iprn_msg .gt. 0)
     >         write(*,*) 'myid,msgs',myid,ip,num_msgs,m_size

             qoffset = qcomm_size(ip+1) - c_size + 1
             tag = MSG_TAG
             do n = 1, num_msgs

               if (nr .ge. max_reqs) then
                  call mpi_waitall(nr, requests, statuses, ierror)
                  nr = 0
                  tag = MSG_TAG
               endif

               if (qoffset+m_size-1 .gt. qcomm_size(ip+1)) then
                  m_size = qcomm_size(ip+1) - qoffset + 1
               endif

               if (iodd .eq. 0) then
                  call mpi_isend(qbc_ou(qoffset), m_size, 
     >                           dp_type, ip, tag+myid,  
     >                           comm_setup, requests(nr+1), ierror)

                  call mpi_irecv(qbc_in(qoffset), m_size, 
     >                           dp_type, ip, tag+ip,  
     >                           comm_setup, requests(nr+2), ierror)
               else
                  call mpi_irecv(qbc_in(qoffset), m_size, 
     >                           dp_type, ip, tag+ip,  
     >                           comm_setup, requests(nr+1), ierror)

                  call mpi_isend(qbc_ou(qoffset), m_size, 
     >                           dp_type, ip, tag+myid,  
     >                           comm_setup, requests(nr+2), ierror)
               endif

               nr = nr + 2
               qoffset = qoffset + m_size
               tag = tag + num_procs
             end do
          else if (c_size .lt. 0) then
             write(*,*) 'error: integer overflow', myid, ip, c_size
             call mpi_abort(MPI_COMM_WORLD, ierror)
             stop
          endif

          if (nr .gt. 0) then
             call mpi_waitall(nr, requests, statuses, ierror)
          endif

       enddo

       if (timeron) call timer_stop(t_rdis2)


c      copy data from qbc buffer
       if (timeron) call timer_start(t_rdis1)
       do iz = 1, proc_num_zones
           zone_no = proc_zone_id(iz)
           nnx    = nx(zone_no)
           nnxmax = nxmax(zone_no)
           nny    = ny(zone_no)
           nnz    = nz(zone_no)

           ip_west   = zone_proc_id(iz_west(zone_no))
           ip_east   = zone_proc_id(iz_east(zone_no))
           ip_south  = zone_proc_id(iz_south(zone_no))
           ip_north  = zone_proc_id(iz_north(zone_no))

           if (ip_west .ge. 0) then
               call copy_x_face(u(start5(iz)),
     $                      qbc_in(qstart2_west(zone_no)),
     $                      nnx, nnxmax, nny, nnz, 0, 'in')
           else
               izone_west = iz_west(zone_no)
               call copy_x_face(u(start5(iz)),
     $                      qbc_ou(qstart_east(izone_west)),
     $                      nnx, nnxmax, nny, nnz, 0, 'in')
           endif

           if (ip_east .ge. 0) then
               call copy_x_face(u(start5(iz)),
     $                      qbc_in(qstart2_east(zone_no)),
     $                      nnx, nnxmax, nny, nnz, nnx-1, 'in')
           else
               izone_east = iz_east(zone_no)
               call copy_x_face(u(start5(iz)),
     $                      qbc_ou(qstart_west(izone_east)),
     $                      nnx, nnxmax, nny, nnz, nnx-1, 'in')
           endif

           if (ip_south .ge. 0) then
               call copy_y_face(u(start5(iz)),
     $                      qbc_in(qstart2_south(zone_no)),
     $                      nnx, nnxmax, nny, nnz, 0, 'in')
           else
               jzone_south = iz_south(zone_no)
               call copy_y_face(u(start5(iz)),
     $                      qbc_ou(qstart_north(jzone_south)),
     $                      nnx, nnxmax, nny, nnz, 0, 'in')
           endif

           if (ip_north .ge. 0) then
               call copy_y_face(u(start5(iz)),
     $                      qbc_in(qstart2_north(zone_no)),
     $                      nnx, nnxmax, nny, nnz, nny-1, 'in')
           else
               jzone_north = iz_north(zone_no)
               call copy_y_face(u(start5(iz)),
     $                      qbc_ou(qstart_south(jzone_north)),
     $                      nnx, nnxmax, nny, nnz, nny-1, 'in')
           endif

       end do
       if (timeron) call timer_stop(t_rdis1)

       return
       end


       subroutine copy_y_face(u, qbc, nx, nxmax, ny, nz, jloc, dir)

       implicit         none

       integer          nx, nxmax, ny, nz, i, j, k, loc, jloc, m
       double precision u(5,0:nxmax-1,0:ny-1,0:nz-1), qbc(*)
       character        dir*(*)

       j = jloc
       loc = 1
       if (dir(1:2) .eq. 'in') then
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(m,i,k,loc)
!$OMP&  SHARED(j,nx,nz)
         do k = 1, nz-2
           loc=1+(k-1)*(nx-2)*5
           do i = 1, nx-2
             do m = 1, 5
               u(m,i,j,k) = qbc(loc) 
               loc = loc + 1
             end do
           end do
         end do
!$OMP END PARALLEL DO
       else if (dir(1:3) .eq. 'out') then
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(m,i,k,loc)
!$OMP&  SHARED(j,nx,nz)
         do k = 1, nz-2
           loc=1+(k-1)*(nx-2)*5
           do i = 1, nx-2
             do m = 1, 5
               qbc(loc) = u(m,i,j,k) 
               loc = loc + 1
             end do
           end do
         end do
!$OMP END PARALLEL DO
       else
         write (*,*) 'Erroneous data designation: ', dir
         stop
       endif

       return
       end


       subroutine copy_x_face(u, qbc, nx, nxmax, ny, nz, iloc, dir)

       implicit         none

       integer          nx, nxmax, ny, nz, i, j, k, loc, iloc, m
       double precision u(5,0:nxmax-1,0:ny-1,0:nz-1), qbc(*)
       character        dir*(*)

       i = iloc
       loc = 1
       if (dir(1:2) .eq. 'in') then
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(m,j,k,loc)
!$OMP&  SHARED(i,ny,nz)
         do k = 1, nz-2
           loc=1+(k-1)*(ny-2)*5
           do j = 1, ny-2
             do m = 1, 5
               u(m,i,j,k) = qbc(loc)
               loc = loc + 1
             end do
           end do
         end do
!$OMP END PARALLEL DO
       else if (dir(1:3) .eq. 'out') then
!$OMP PARALLEL DO DEFAULT(SHARED) PRIVATE(m,j,k,loc)
!$OMP&  SHARED(i,ny,nz)
         do k = 1, nz-2
           loc=1+(k-1)*(ny-2)*5
           do j = 1, ny-2
             do m = 1, 5
               qbc(loc) = u(m,i,j,k)
               loc = loc + 1
             end do
           end do
         end do
!$OMP END PARALLEL DO
       else
         write (*,*) 'Erroneous data designation: ', dir
         stop
       endif

       return
       end

