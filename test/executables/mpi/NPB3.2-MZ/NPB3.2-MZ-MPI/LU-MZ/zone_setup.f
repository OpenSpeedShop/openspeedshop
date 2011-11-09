       subroutine zone_setup(nx, nxmax, ny, nz)

       include 'header.h'
       include 'mpi_stuff.h'

       integer nx(*), nxmax(*), ny(*), nz(*)

       integer           i,  j, zone_no
       integer           id_west, id_east, jd_south, jd_north
       double precision  x_r, y_r, x_smallest, y_smallest

       if (dabs(ratio-1.d0) .gt. 1.d-10) then

c        compute zone stretching only if the prescribed zone size ratio 
c        is substantially larger than unity       

         x_r   = dexp(dlog(ratio)/(x_zones-1))
         y_r   = dexp(dlog(ratio)/(y_zones-1))
         x_smallest = dble(gx_size)*(x_r-1.d0)/(x_r**x_zones-1.d0)
         y_smallest = dble(gy_size)*(y_r-1.d0)/(y_r**y_zones-1.d0)

c        compute tops of intervals, using a slightly tricked rounding
c        to make sure that the intervals are increasing monotonically
c        in size

         do i = 1, x_zones
            x_end(i) = x_smallest*(x_r**i-1.d0)/(x_r-1.d0)+0.45d0
         end do

         do j = 1, y_zones
            y_end(j) = y_smallest*(y_r**j-1.d0)/(y_r-1.d0)+0.45d0
         end do
 
       else

c        compute essentially equal sized zone dimensions

         do i = 1, x_zones
           x_end(i)   = (i*gx_size)/x_zones
         end do

         do j = 1, y_zones
           y_end(j)   = (j*gy_size)/y_zones
         end do

       endif

       x_start(1) = 1
       do i = 1, x_zones
          if (i .ne. x_zones) x_start(i+1) = x_end(i) + 1
          x_size(i)  = x_end(i) - x_start(i) + 1
       end do

       y_start(1) = 1
       do j = 1, y_zones
          if (j .ne. y_zones) y_start(j+1) = y_end(j) + 1
          y_size(j) = y_end(j) - y_start(j) + 1
       end do

       if (npb_verbose .gt. 1 .and. myid .eq. root) then
          write (*,98)
       endif
 98    format(/' Zone sizes:')

       do j = 1, y_zones
         do i = 1, x_zones
           zone_no = (i-1)+(j-1)*x_zones+1
           nx(zone_no) = x_size(i)
           nxmax(zone_no) = nx(zone_no) + 1 - mod(nx(zone_no),2)
           ny(zone_no) = y_size(j)
           nz(zone_no) = gz_size

           id_west  = mod(i-2+x_zones,x_zones)
           id_east  = mod(i,          x_zones)
           jd_south = mod(j-2+y_zones,y_zones)
           jd_north = mod(j,          y_zones)
           iz_west (zone_no) = id_west +  (j-1)*x_zones + 1
           iz_east (zone_no) = id_east +  (j-1)*x_zones + 1
           iz_south(zone_no) = (i-1) + jd_south*x_zones + 1
           iz_north(zone_no) = (i-1) + jd_north*x_zones + 1

           if (npb_verbose .gt. 1 .and. myid .eq. root) then
             write (*,99) zone_no, nx(zone_no), ny(zone_no), 
     $                    nz(zone_no)
           endif
         end do
       end do

 99    format(i5,':  ',i5,' x',i5,' x',i5)

       return
       end


       subroutine zone_starts(num_zones, nx, nxmax, ny, nz, 
     &                        start1, start5,
     $                        qstart_west,  qstart_east,
     $                        qstart_south, qstart_north)

       include 'header.h'
       include 'mpi_stuff.h'

       integer   num_zones
       integer   nx(*), nxmax(*), ny(*), nz(*)
       dimension start1(*), start5(*), qstart_west(*), 
     $           qstart_east(*), qstart_south(*), qstart_north(*)

       integer   zone, zone_size, iz, ip, zone2, ig, id, itmp
       integer   x_face_size, y_face_size
       integer   ip_west, ip_east, ip_south, ip_north

       do iz = 1, proc_num_zones
         zone = proc_zone_id(iz)
         zone_size = nxmax(zone)*ny(zone)*nz(zone)
         if (iz .eq. 1) then
           start1(iz) = 1
           start5(iz) = 1
         endif
         if (iz .ne. proc_num_zones) then
           start1(iz+1) = start1(iz) + zone_size
           start5(iz+1) = start5(iz) + zone_size*5
         else
           if (start1(iz)+zone_size-1 .gt. proc_max_size) then
             write(*,50) zone,proc_max_size,start1(iz)+zone_size-1
             call mpi_abort(MPI_COMM_WORLD, ierror)
             stop
           endif
         endif
   50    format(' Error in size: zone',i5,' proc_max_size',i10,
     &          ' access_size',i10)
       enddo

c ...  for 'qbc_in'
       qoffset = 1
       do 10 ip = 0, num_procs-1

         if (ip .eq. myid) goto 10

         do 15 zone = 1, num_zones
           x_face_size = (ny(zone)-2)*(nz(zone)-2)*5
           y_face_size = (nx(zone)-2)*(nz(zone)-2)*5

           zone2 = iz_west(zone)
           ip_east  = zone_proc_id(zone)
           if (ip_east .eq. ip .and. zone_proc_id(zone2) .lt. 0) then
             qstart2_east(zone2) = qoffset
             qoffset = qoffset + x_face_size
           endif

           zone2 = iz_east(zone)
           ip_west  = zone_proc_id(zone)
           if (ip_west .eq. ip .and. zone_proc_id(zone2) .lt. 0) then
             qstart2_west(zone2) = qoffset
             qoffset = qoffset + x_face_size
           endif

           zone2 = iz_south(zone)
           ip_north = zone_proc_id(zone)
           if (ip_north .eq. ip .and. zone_proc_id(zone2) .lt. 0) then
             qstart2_north(zone2) = qoffset
             qoffset = qoffset + y_face_size
           endif

           zone2 = iz_north(zone)
           ip_south = zone_proc_id(zone)
           if (ip_south .eq. ip .and. zone_proc_id(zone2) .lt. 0) then
             qstart2_south(zone2) = qoffset
             qoffset = qoffset + y_face_size
           endif
   15    continue

   10  continue

c ...  for 'qbc_out'
       qoffset = 1
       do 20 ip = 0, num_procs-1

         if (ip .eq. myid) goto 25

         do 30 zone = 1, num_zones
           if (zone_proc_id(zone) .ge. 0) goto 30

           x_face_size = (ny(zone)-2)*(nz(zone)-2)*5
           y_face_size = (nx(zone)-2)*(nz(zone)-2)*5

           ip_west  = zone_proc_id(iz_west(zone))
           if (ip_west .eq. ip) then
             qstart_west(zone) = qoffset
             qoffset = qoffset + x_face_size
           endif

           ip_east  = zone_proc_id(iz_east(zone))
           if (ip_east .eq. ip) then
             qstart_east(zone) = qoffset
             qoffset = qoffset + x_face_size
           endif

           ip_south = zone_proc_id(iz_south(zone))
           if (ip_south .eq. ip) then
             qstart_south(zone) = qoffset
             qoffset = qoffset + y_face_size
           endif

           ip_north = zone_proc_id(iz_north(zone))
           if (ip_north .eq. ip) then
             qstart_north(zone) = qoffset
             qoffset = qoffset + y_face_size
           endif
   30    continue

   25    qcomm_size(ip+1) = qoffset - 1
   20  continue

c ...  for intra-process zone copy
       do 40 zone = 1, num_zones
         if (zone_proc_id(zone) .ge. 0) goto 40

         ip_west  = zone_proc_id(iz_west(zone))
         ip_east  = zone_proc_id(iz_east(zone))
         ip_south = zone_proc_id(iz_south(zone))
         ip_north = zone_proc_id(iz_north(zone))

         x_face_size = (ny(zone)-2)*(nz(zone)-2)*5
         y_face_size = (nx(zone)-2)*(nz(zone)-2)*5

         if (ip_west  .lt. 0) then
           qstart_west(zone)  = qoffset
           qoffset = qoffset + x_face_size
         endif
         if (ip_east  .lt. 0) then
           qstart_east(zone)  = qoffset
           qoffset = qoffset + x_face_size
         endif
         if (ip_south .lt. 0) then
           qstart_south(zone) = qoffset
           qoffset = qoffset + y_face_size
         endif
         if (ip_north .lt. 0) then
           qstart_north(zone) = qoffset
           qoffset = qoffset + y_face_size
         endif
   40  continue

c ...  set up cyclic communication group
       iz = 1
       do while (iz .lt. myid)
         iz = iz * 2
       enddo
       if (iz .gt. myid) iz = iz / 2

       do ig = 1, num_procs
         pcomm_group(ig) = ig - 1
       enddo
       do ig = num_procs+1, num_procs2
         pcomm_group(ig) = -1
       enddo

       id = 0
       do while (iz .ge. 1)
         if (id+iz .gt. myid) goto 45
         do ig = 1, num_procs, iz*2
           do ip = ig, ig+iz-1
              itmp = pcomm_group(ip)
              pcomm_group(ip) = pcomm_group(ip+iz)
              pcomm_group(ip+iz) = itmp
           enddo
         enddo
         id = id + iz
   45    iz = iz/2
       enddo

       ip = 1
       do ig = 1, num_procs
         do while (pcomm_group(ip) .lt. 0)
           ip = ip + 1
         enddo
         pcomm_group(ig) = pcomm_group(ip)
         ip = ip + 1
       enddo

       if (npb_verbose .gt. 1) then
         do iz = 1, proc_num_zones
           zone = proc_zone_id(iz)
           write(*,60) myid, iz, zone, start1(iz), start5(iz)
         enddo
         write(*,70) myid, qoffset-1
   60    format(' myid',i5,' iz=',i5,' zone=',i5,
     &          ' start1=',i10,' start5=',i10)
   70    format(' myid',i5,' qcomm_size=',i10)

         do ig = 1, num_procs
           ip = pcomm_group(ig) + 1
           if (ip .eq. 1) then
             qoffset = qcomm_size(ip)
           else
             qoffset = qcomm_size(ip) - qcomm_size(ip-1)
           endif
           write(*,80) myid, ip-1, qoffset, qcomm_size(ip)
         enddo
   80    format(' myid',i5,' proc',i5,' qcomm_size',2(1x,i10))
       endif

       return
       end
