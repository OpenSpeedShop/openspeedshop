c>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>c
c
      subroutine mpi_setup(num_zones, nx, ny, nz)
c
c  Set up MPI stuff, including
c     - define the active set of processes
c     - set up new communicator
c
      include 'header.h'
c
      integer num_zones, nx(*), ny(*), nz(*)
c
      include 'mpi_stuff.h'
c
      integer no_nodes, color
c
c ... initialize MPI parameters
      call mpi_init(ierror)
      
      call mpi_comm_size(MPI_COMM_WORLD, no_nodes, ierror)
      call mpi_comm_rank(MPI_COMM_WORLD, myid, ierror)

      if (.not. convertdouble) then
         dp_type = MPI_DOUBLE_PRECISION
      else
         dp_type = MPI_REAL
      endif
      
c---------------------------------------------------------------------
c     let node 0 be the root for the group (there is only one)
c---------------------------------------------------------------------
      root = 0
c
      if (no_nodes .lt. num_procs) then
         if (myid .eq. root) write(*, 10) no_nodes, num_procs
   10    format(' Requested MPI processes ',i5,
     &          ' less than the compiled value ',i5)
         call mpi_abort(MPI_COMM_WORLD, ierror)
         stop
      endif
c
      if (myid .ge. num_procs) then
         active = .false.
         color = 1
      else
         active = .true.
         color = 0
      end if
      
      call mpi_comm_split(MPI_COMM_WORLD,color,myid,comm_setup,ierror)
      if (.not. active) return

      call mpi_comm_rank(comm_setup, myid, ierror)
      if (no_nodes .ne. num_procs) then
         if (myid .eq. root) write(*, 20) no_nodes, num_procs
   20    format('Warning: Requested ',i5,' MPI processes, ',
     &          'but the compiled value is ',i5/
     &          'The compiled value is used for benchmarking')
      endif
c
      return
      end
c
c>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>c
c
      subroutine env_setup(tot_threads)
c
c  Set up from environment variables
c
c ... common variables
      include 'header.h'
c
      integer tot_threads
c
      include 'mpi_stuff.h'
c
c ... local variables
      integer ios, curr_threads, ip, mp, group, ip1, ip2
      integer entry_counts(num_procs)
      character envstr*80, line*132
c
c$    integer omp_get_max_threads
c$    external omp_get_max_threads
c
      if (myid .ne. root) goto 80
c
c ... test the OpenMP multi-threading environment
      mp = 0
c$    mp = omp_get_max_threads()
c
c ... master sets up parameters
      call getenv('OMP_NUM_THREADS', envstr)
      if (envstr .ne. ' ' .and. mp .gt. 0) then
         read(envstr,*,iostat=ios) num_threads
         if (ios.ne.0 .or. num_threads.lt.1) num_threads = 1
         if (mp .ne. num_threads) then
            write(*, 10) num_threads, mp
   10       format(' Warning: Requested ',i4,' threads per process,',
     &             ' but the active value is ',i4)
            num_threads = mp
         endif
      else
         num_threads = 1
      endif
c
      call getenv('NPB_MZ_BLOAD', envstr)
      if (envstr .ne. ' ') then
         if (envstr.eq.'on' .or. envstr.eq.'ON') then
            mz_bload = 1
         else if (envstr(1:1).eq.'t' .or. envstr(1:1).eq.'T') then
            mz_bload = 1
         else
            read(envstr,*,iostat=ios) mz_bload
            if (ios.ne.0) mz_bload = 0
         endif
      else
         mz_bload = 1
      endif
c
      call getenv('NPB_MAX_THREADS', envstr)
      max_threads = 0
      if (mz_bload.gt.0 .and. envstr.ne.' ') then
         read(envstr,*,iostat=ios) max_threads
         if (ios.ne.0 .or. max_threads.lt.0) max_threads = 0
         if (max_threads.gt.0 .and. max_threads.lt.num_threads) then
            write(*,20) max_threads, num_threads
   20       format(' Error: max_threads ',i5,
     &             ' is less than num_threads ',i5/
     &             ' Please redefine the value for NPB_MAX_THREADS',
     &             ' or OMP_NUM_THREADS')
            call mpi_abort(MPI_COMM_WORLD, ierror)
            stop
         endif
      endif
c
      call getenv('NPB_VERBOSE', envstr)
      npb_verbose = 0
      if (envstr.ne.' ') then
         read(envstr,*,iostat=ios) npb_verbose
         if (ios.ne.0) npb_verbose = 0
      endif
c
      do ip = 1, num_procs
         proc_num_threads(ip) = num_threads
         proc_group(ip) = 0
      end do
c
      open(2, file='loadsp-mz.data', status='old', iostat=ios)
      if (ios.eq.0) then
         write(*,*) 'Reading load factors from loadsp-mz.data'

         if (mz_bload .ge. 1) then
            mz_bload = -mz_bload
         endif

         do ip = 1, num_procs
            entry_counts(ip) = 0
         end do

         do while (.true.)
   25       read(2,'(a)',end=40,err=40) line
            if (line.eq.' ' .or. line(1:1).eq.'#') goto 25

            call decode_line(line, ip1, ip2, curr_threads, group, ios)
            if (ios .ne. 0) goto 40

            if (mz_bload .lt. 0 .and. group .gt. 0) then
               mz_bload = -mz_bload
            endif

            if (curr_threads .lt. 1) curr_threads = 1
            if (mp .le. 0) curr_threads = 1
            if (ip1.lt.0) ip1 = 0
            if (ip2.ge.num_procs) ip2 = num_procs - 1

            do ip = ip1+1, ip2+1
               proc_num_threads(ip) = curr_threads
               proc_group(ip) = group
               entry_counts(ip) = entry_counts(ip) + 1
            end do
         end do
   40    close(2)

         do ip = 1, num_procs
            if (entry_counts(ip) .eq. 0) then
               write(*,*) '*** Error: Missing entry for proc ',ip-1
               call mpi_abort(MPI_COMM_WORLD, ierror)
               stop
            else if (entry_counts(ip) .gt. 1) then
               write(*,*) '*** Warning: Multiple entries for proc ',
     &                    ip-1, ', only the last one used'
            endif
         end do

         ip1 = 1
      else
         write(*,*) 'Use the default load factors with threads'
         ip1 = 0
      endif

      if (ip1 .gt. 0 .or. npb_verbose .gt. 0) then
         ip1 = 0
         do ip = 1, num_procs
            if (ip .eq. 1 .or.
     &          proc_num_threads(ip) .ne. curr_threads .or.
     &          proc_group(ip) .ne. group) then

               ip2 = ip-2
               if (ip2 .gt. ip1+1) write(*,*) '    ...'
               if (ip2 .gt. ip1)
     &            write(*,30) ip2, curr_threads, group

               curr_threads = proc_num_threads(ip)
               group = proc_group(ip)

               ip1 = ip - 1
               write(*,30) ip1, curr_threads, group

            else if (ip .eq. num_procs) then
               ip2 = ip-1
               if (ip2 .gt. ip1+1) write(*,*) '    ...'
               write(*,30) ip2, curr_threads, group
            endif
         end do
   30    format('  proc',i6,'  num_threads =',i5,
     >          '  group =',i5)
      endif
c
c ... broadcast parameters to all processes
   80 call mpi_bcast(num_threads, 1, mpi_integer, root, 
     &               comm_setup, ierror)
      call mpi_bcast(mz_bload, 1, mpi_integer, root, 
     &               comm_setup, ierror)
      call mpi_bcast(max_threads, 1, mpi_integer, root, 
     &               comm_setup, ierror)
      call mpi_bcast(proc_num_threads, num_procs, mpi_integer, root, 
     &               comm_setup, ierror)
      call mpi_bcast(proc_group, num_procs, mpi_integer, root, 
     &               comm_setup, ierror)
      call mpi_bcast(npb_verbose, 1, mpi_integer, root, 
     &               comm_setup, ierror)
c
      tot_threads = 0
      do ip = 1, num_procs
         tot_threads = tot_threads + proc_num_threads(ip)
      end do
      if (myid .eq. root) then
         if (mp .gt. 0)
     &      write(*, 1004) tot_threads, dble(tot_threads)/num_procs
      endif
 1004 format(' Total number of threads: ', i6,
     &       '  (', f5.1, ' threads/process)')
c
      return
      end
c
c>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>c
c
      subroutine decode_line(line, ip1, ip2, curr_threads, group, ios)
      implicit none
c
c  decode a line from the load data file
c  format:  ip1[:ip2] curr_threads group
c
      character line*(*)
      integer ip1, ip2, curr_threads, group, ios
c
      integer is, n
c
      ios = -1
c
      n  = len(line)
      is = 1
      do while (is.le.n .and. line(is:is).ne.':')
         if (line(is:is).eq.'!') n = is
         is = is + 1
      end do
c
      if (is .gt. n) then
         read(line,*,err=90,end=90) ip1, curr_threads, group
         ip2 = ip1
      else if (is.eq.1 .or. is.eq.n) then
         go to 90
      else
         read(line(:is-1),*,err=90,end=90) ip1
         read(line(is+1:),*,err=90,end=90) ip2, curr_threads, group
      endif
c
      if (ip2 .lt. ip1) then
         is  = ip2
         ip2 = ip1
         ip1 = is
      endif
      ios = 0
c
   90 return
      end
c
c>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>c
c
      subroutine get_comm_index(zone, iproc, comm_index)
c
      include 'header.h'
c
      include 'mpi_stuff.h'
c
c  Calculate the communication index of a zone within a processor group
c
      integer zone, iproc, comm_index
c
c     local variables
      integer izone, jzone 
c
      jzone  = (zone - 1)/x_zones + 1
      izone  = mod(zone - 1, x_zones) + 1
c
      comm_index = 0
      if (proc_zone_id(iz_west(zone)) .eq. iproc)
     $   comm_index = comm_index + y_size(jzone)
      if (proc_zone_id(iz_east(zone)) .eq. iproc)
     $   comm_index = comm_index + y_size(jzone)
      if (proc_zone_id(iz_south(zone)) .eq. iproc)
     $   comm_index = comm_index + x_size(izone)
      if (proc_zone_id(iz_north(zone)) .eq. iproc)
     $   comm_index = comm_index + x_size(izone)
c
      return
      end
c
c>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>c
c
      subroutine map_zones(num_zones, nx, ny, nz, tot_threads)
c
c  Perform zone-process mapping for load balance
c
      include 'header.h'
c
      integer num_zones, nx(*), ny(*), nz(*), tot_threads
c
      include 'mpi_stuff.h'
c
c     local variables
      integer z_order(max_zones)
      integer zone, iz, z2, mz, np, ip, zone_comm, comm_index
      integer imx, imn, inc, icur_size
      double precision tot_size, cur_size, max_size, ave_size
      double precision zone_size(max_zones)
      double precision diff_ratio, tot_group_size
c
      integer group, ipg, tot_group_threads
      integer proc_group_flag(num_procs)
c
c ... sort the zones in decending order
      tot_size = 0.d0
      do iz = 1, num_zones
         zone_size(iz) = nx(iz)*ny(iz)*nz(iz)
         z_order(iz) = iz
         tot_size = tot_size + zone_size(iz)
      end do
      do iz = 1, num_zones-1
         cur_size = zone_size(z_order(iz))
         mz = iz
         do z2 = iz+1, num_zones
            if (cur_size.lt.zone_size(z_order(z2))) then
               cur_size = zone_size(z_order(z2))
               mz = z2
            endif
         end do
         if (mz .ne. iz) then
            z2 = z_order(iz)
            z_order(iz) = z_order(mz)
            z_order(mz) = z2
         endif
      end do
c
      if (npb_verbose .gt. 1 .and. myid .eq. root) then
         write(*,10)
         do iz = 1, num_zones
            z2 = z_order(iz)
            write(*,15) iz,z2,nx(z2),ny(z2),nz(z2),zone_size(z2)
         end do
      endif
   10 format(/' Sorted zones:'/
     &       '  seq. zone    nx    ny    nz    size')
   15 format(i5,':',4(1x,i5),1x,f9.0)
c
c ... use a simple bin-packing scheme to balance the load among processes
      do ip = 1, num_procs
         proc_zone_count(ip) = 0
         proc_zone_size(ip) = 0.d0
      end do
      do iz = 1, num_zones
         proc_zone_id(iz) = -1
      end do

      iz = 1
      do while (iz .le. num_zones)
c
c  ...   the current most empty processor
         np = 1
         cur_size = proc_zone_size(1)
         do ip = 2, num_procs
            if (cur_size.gt.proc_zone_size(ip)) then
               np = ip
               cur_size = proc_zone_size(ip)
            endif
         end do
         ip = np - 1
c
c  ...   get a zone that has the largest communication index with
c        the current group and does not worsen the computation balance
         mz = z_order(iz)
         if (iz .lt. num_zones) then
            call get_comm_index(mz, ip, zone_comm)
            do z2 = iz+1, num_zones
               zone = z_order(z2)

               diff_ratio = (zone_size(z_order(iz)) - 
     &                      zone_size(zone)) / zone_size(z_order(iz))
               if (diff_ratio .gt. 0.05D0) goto 120

               if (proc_zone_id(zone) .lt. 0) then
                  call get_comm_index(zone, ip, comm_index)
                  if (comm_index .gt. zone_comm) then
                     mz = zone
                     zone_comm = comm_index
                  endif
               endif
            end do
         endif
c
c  ...   assign the zone to the current processor group
  120    proc_zone_id(mz) = ip
         proc_zone_size(np) = proc_zone_size(np) + zone_size(mz)
         proc_zone_count(np) = proc_zone_count(np) + 1
c
c  ...   skip the previously assigned zones
         do while (iz.le.num_zones)
            if (proc_zone_id(z_order(iz)).lt.0) goto 130
            iz = iz + 1
         end do
  130    continue
      end do
c
c ... move threads around if needed
      mz = 1
      if (tot_threads.eq.num_procs .or. mz_bload.lt.1) mz = 0
c
      if (mz .ne. 0) then
c
         do ipg = 1, num_procs
            proc_group_flag(ipg) = 0
         end do
c
         ipg = 1
c
c ...    balance load within a processor group
  200    do while (ipg .le. num_procs)
            if (proc_group_flag(ipg) .eq. 0) goto 210
            ipg = ipg + 1
         end do
  210    if (ipg .gt. num_procs) goto 300
c
         group = proc_group(ipg)
         tot_group_size = 0.d0
         tot_group_threads = 0
         do ip = ipg, num_procs
            if (proc_group(ip) .eq. group) then
               proc_group_flag(ip) = 1
               tot_group_size = tot_group_size + proc_zone_size(ip)
               tot_group_threads = tot_group_threads + 
     &                             proc_num_threads(ip)
            endif
         end do
c
         ave_size = tot_group_size / tot_group_threads
c
c  ...   distribute size evenly among threads
         icur_size = 0
         do ip = 1, num_procs
            if (proc_group(ip) .ne. group) goto 220
            proc_num_threads(ip) = proc_zone_size(ip) / ave_size
            if (proc_num_threads(ip) .lt. 1)
     &          proc_num_threads(ip) = 1
            if (max_threads .gt. 0 .and. 
     &          proc_num_threads(ip) .gt. max_threads) 
     &          proc_num_threads(ip) = max_threads
            icur_size = icur_size + proc_num_threads(ip)
  220    end do
         mz = tot_group_threads - icur_size
c
c  ...   take care of any remainers
         inc = 1
         if (mz .lt. 0) inc = -1
         do while (mz .ne. 0)
            max_size = 0.d0
            imx = 0
            do ip = 1, num_procs
               if (proc_group(ip) .ne. group) goto 230
               if (mz .gt. 0) then
                  cur_size = proc_zone_size(ip) / proc_num_threads(ip)
                  if (cur_size.gt.max_size .and. (max_threads.le.0
     &                .or. proc_num_threads(ip).lt.max_threads)) then
                     max_size = cur_size
                     imx = ip
                  endif
               else if (proc_num_threads(ip) .gt. 1) then
                  cur_size = proc_zone_size(ip) / 
     &                       (proc_num_threads(ip)-1)
                  if (max_size.eq.0 .or. cur_size.lt.max_size) then
                     max_size = cur_size
                     imx = ip
                  endif
               endif
  230       end do
            proc_num_threads(imx) = proc_num_threads(imx) + inc
            mz = mz - inc
         end do
c
         goto 200
      endif
c
c ... print the mapping
  300 if (npb_verbose .gt. 0 .and. myid .eq. root) then
         write(*,20)
         do ip = 1, num_procs
            write(*,25) ip-1,proc_zone_count(ip),
     &            proc_zone_size(ip),proc_num_threads(ip),
     &            proc_zone_size(ip)/proc_num_threads(ip)
            do iz = 1, num_zones
               if (proc_zone_id(iz) .eq. ip-1) then
                  write(*,30) iz, zone_size(iz)
               endif
            end do
         end do
      endif
   20 format(/' Zone-process mapping:'/
     &       '  proc nzones  zone_size nthreads size_per_thread')
   25 format(i5,2x,i5,2x,f10.0,2x,i5,3x,f10.0)
   30 format(3x,'zone ',i5,2x,f9.0)
c
      if (myid .eq. root) then
         do ip = 1, num_procs
            cur_size = proc_zone_size(ip)/proc_num_threads(ip)
            if (ip.eq.1 .or. cur_size.gt.max_size) then
               imx = ip
               max_size = cur_size
            endif
            if (ip.eq.1 .or. cur_size.lt.ave_size) then
               imn = ip
               ave_size = cur_size
            endif
         end do

         if (npb_verbose .gt. 0) then
            write(*,*)
            write(*,35) 'Max', imx-1, proc_zone_count(imx),
     &                  proc_zone_size(imx),proc_num_threads(imx)
            write(*,35) 'Min', imn-1, proc_zone_count(imn),
     &                  proc_zone_size(imn),proc_num_threads(imn)
         endif
   35    format(1x,a,': proc=',i5,' nzones=',i5,' size=',f10.0,
     &          ' nthreads=',i5)

         write(*,40) tot_size / max_size
      endif
   40 format(/' Calculated speedup = ',f9.2/)
c
c ... reorganize list of zones for this process
      zone = 0
      do iz = 1, num_zones
         zone_proc_id(iz) = proc_zone_id(iz)
         if (proc_zone_id(iz) .eq. myid) then
            zone = zone + 1
            proc_zone_id(zone) = iz
c           negative number indicates a zone assigned to this process
c           this information is used in exch_qbc
            zone_proc_id(iz) = -zone
         endif
      end do
      proc_num_zones = zone
      if (zone .ne. proc_zone_count(myid+1)) then
         if (myid .eq. root)
     &      write(*,*) 'Warning: ',myid, ': mis-matched zone counts -', 
     &           zone, proc_zone_count(myid+1)
      endif
c
c ... set number of threads for this process
      group = proc_group(myid+1)
      np = 0
      do ip = 1, num_procs
         if (proc_group(ip) .eq. group) then
            proc_group(ip) = np
            np = np + 1
            proc_num_threads(np) = proc_num_threads(ip)
         endif
      end do
      ipg = proc_group(myid+1)
      if (npb_verbose.gt.1) then
         write(*,50) myid, group, np, ipg, proc_num_threads(ipg+1)
      endif
   50 format(' myid',i5,' group',i5,' group_size',i5,
     &       ' group_pid',i5,' threads',i4)
c$    call omp_set_num_threads(proc_num_threads(ipg+1))
c
c ... pin-to-node within one process group
c      call smp_pinit_thread(np, ipg, proc_num_threads)
c
      return
      end
