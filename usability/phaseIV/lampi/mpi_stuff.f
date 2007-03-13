      subroutine task_init(myid_r,numtasks_r)

      implicit none

      integer myid_r,numtasks_r





      include 'mpif.h'


      include 'msg_stuff.h'

      integer info, i
      integer npe_i, npe_j
      character*16 my_name
      integer size
c start mpi:
c
      call mpi_init( info )
      if ( info .ne. 0) then
        print *,myid,'failure to enroll mpi program'
        stop
      endif

      call mpi_comm_size(MPI_COMM_WORLD, size, info)

      call mpi_comm_rank(MPI_COMM_WORLD, mytid, info)

      myid = mytid + 1
      tids(1) = 0
c print *, myid,mytid,size


      numtasks = 0
      if (myid .eq. 1) then
         open(3,file='input',status='old')
         read (3,*) npe_i, npe_j
         close(3)
         numtasks = npe_i * npe_j
         do i = 1, min(numtasks,maxtasks)
            tids(i) = i - 1
         end do

      endif


c
c send initialization data to other tasks:
c
      call bcast_int(numtasks, 1, 100, 1, info)
      if (numtasks.gt.maxtasks) then
         if (myid.eq.1) print *,'tids array too small'
         call task_end()
         stop
      endif
      if (numtasks .le. 0) then
         if (myid.eq.1) print *,'bad npe_i/npe_j; try again'
         call task_end()
         stop
      endif
      if (numtasks .ne. size) then
         if (myid.eq.1) print *,'tasks do not match mpi_comm_size'
         call task_end()
         stop
      endif


      call bcast_int(tids, numtasks, 101, 1, info)
      do i = 1, numtasks
         if ( mytid .eq. tids(i) ) myid = i
      end do
c print *,myid,'tids exchanged:',(tids(i),i=1,numtasks)

c Make sure every task has finished starting

      call barrier_sync()
c print *,myid,'taskinit barrier_sync complete'
c ihost = hostnm_(my_name)
c print *,myid,mytid,' running on ',my_name

      myid_r = myid
      numtasks_r = numtasks

      return
      end


      subroutine task_end()

      implicit none





      include 'mpif.h'


      include 'msg_stuff.h'

      integer info
      call mpi_finalize(info)


      return
      end


      subroutine snd_real(dest, value, size, tag, info)

      implicit none

      integer dest, size, tag, info
      double precision value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

c print *,myid,'snd_real: ',dest,size,tag
      info = 0
      call mpi_send(value, size, MPI_DOUBLE_PRECISION,
     & tids(dest), tag, MPI_COMM_WORLD, info)


      return
      end


      subroutine rcv_real(orig, value, size, tag, info)

      implicit none

      integer orig, size, tag, info
      double precision value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

      integer orig_tid




      integer status(MPI_STATUS_SIZE)


      info = 0
      if (orig .eq. -1) then
        orig_tid = MPI_ANY_SOURCE
      else
        orig_tid = tids(orig)
      endif
      call mpi_recv(value, size, MPI_DOUBLE_PRECISION,
     & orig_tid, tag, MPI_COMM_WORLD, status, info)

c print *,myid,'rcv_real: ',orig,size,tag,orig_tid

      return
      end


      subroutine snd_int(dest, value, size, tag, info)

      implicit none

      integer dest, size, tag, info
      integer value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

c print *,myid,'snd_int: ',dest,size,tag,value(1),tids(dest)
      info = 0
      call mpi_send(value, size, MPI_INTEGER,
     & tids(dest), tag, MPI_COMM_WORLD, info)


      return
      end


      subroutine rcv_int(orig, value, size, tag, info)

      implicit none

      integer orig, size, tag, info
      integer value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

      integer orig_tid




      integer status(MPI_STATUS_SIZE)


      info = 0
      if (orig .eq. -1) then
        orig_tid = MPI_ANY_SOURCE
      else
        orig_tid = tids(orig)
      endif
      call mpi_recv(value, size, MPI_INTEGER,
     & orig_tid, tag, MPI_COMM_WORLD, status, info)

c print *,myid,'rcv_int: ',orig,size,tag,value(1),orig_tid

      return
      end


      subroutine bcast_real(value, size, tag, root, info)

      implicit none

      integer size, tag, root, info
      double precision value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

      if (numtasks .eq. 1) return

c print *,myid,'bcast_real: ',size,tag,root
      info = 0
      call mpi_bcast(value, size, MPI_DOUBLE_PRECISION,
     & tids(root), MPI_COMM_WORLD, info)


      return
      end


      subroutine bcast_int(value, size, tag, root, info)

      implicit none

      integer size, tag, root, info
      integer value(size)





      include 'mpif.h'


      include 'msg_stuff.h'

      if (numtasks .eq. 1) return

c print *,myid,'bcast_int: ',size,tag,root
      info = 0
      call mpi_bcast(value, size, MPI_INTEGER,
     & tids(root), MPI_COMM_WORLD, info)


      return
      end


      subroutine global_real_max(value)

      implicit none

      double precision value





      include 'mpif.h'


      include 'msg_stuff.h'

      integer msgtag, i, info
      double precision y

      info = 0
      call mpi_allreduce(value,y,1,MPI_DOUBLE_PRECISION,MPI_MAX,
     & MPI_COMM_WORLD,info)
      value = y
      return
      end


      subroutine global_real_sum(value)

      implicit none

      double precision value





      include 'mpif.h'


      include 'msg_stuff.h'

      integer msgtag, i, info
      double precision y

      info = 0
      call mpi_allreduce(value,y,1,MPI_DOUBLE_PRECISION,MPI_SUM,
     & MPI_COMM_WORLD,info)
      value = y
      return
      end


      subroutine global_int_sum(value)

      implicit none

      integer value





      include 'mpif.h'


      include 'msg_stuff.h'

      integer msgtag, i, info
      integer y

      info = 0
      call mpi_allreduce(value,y,1,MPI_INTEGER,MPI_SUM,
     & MPI_COMM_WORLD,info)
      value = y
      return
      end


      subroutine barrier_sync()

      implicit none





      include 'mpif.h'


      include 'msg_stuff.h'

      integer info

      if (numtasks .eq. 1) return

c print *,myid,'barrier_sync:'




      call mpi_barrier(MPI_COMM_WORLD,info)


      return
      end
