c
      include 'mpif.h'
c
      integer   max_zones
      parameter (max_zones=x_zones*y_zones)
      integer   proc_zone_id(max_zones), zone_proc_id(max_zones),
     &          proc_zone_count(num_procs), 
     &          proc_num_threads(num_procs), proc_num_zones,
     &          proc_group(num_procs),
     &          iz_west(max_zones), iz_east(max_zones),
     &          iz_south(max_zones), iz_north(max_zones)
      double precision proc_zone_size(num_procs)
      common /mpi_cmn1/ proc_zone_size, proc_zone_id, zone_proc_id, 
     &          proc_zone_count, proc_num_threads, proc_num_zones,
     &          proc_group, iz_west, iz_east, iz_south, iz_north
c
      integer   myid, root, comm_setup, ierror, dp_type
      integer   num_threads, mz_bload, max_threads
      logical   active
      common /mpi_cmn2/ myid, root, comm_setup, ierror, active, 
     &          dp_type, num_threads, mz_bload, max_threads
c
c ... Two adjustable parameters for MPI communication
c     max_reqs  -- max. number of async message requests
c     MSG_SIZE  -- optimal message size (in words) for communication
      integer   max_reqs, MSG_SIZE
      parameter (max_reqs=32, MSG_SIZE=400000)
c
      integer   requests(max_reqs), statuses(MPI_STATUS_SIZE,max_reqs)
      common /mpi_cmn3/ requests, statuses
c
      integer   pcomm_group(num_procs2)
      dimension qcomm_size(num_procs),
     &          qstart2_west (max_zones), qstart2_east (max_zones),
     &          qstart2_south(max_zones), qstart2_north(max_zones)
      common /mpi_cmn4/ qcomm_size, qstart2_west, qstart2_east,
     &          qstart2_south, qstart2_north, pcomm_group

