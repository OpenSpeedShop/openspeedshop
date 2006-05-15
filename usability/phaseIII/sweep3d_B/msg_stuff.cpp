
#ifdef PVM
#undef REDUCE
#undef PSEND
#ifdef T3D
#define PSEND
#endif
#endif

#ifdef MPI
#define REDUCE
#endif

      subroutine task_init(myid_r,numtasks_r)

      implicit none

      integer myid_r,numtasks_r

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer info, i
      integer npe_i, npe_j
      character*16 my_name

#ifdef PVM
      integer parent
      character task*19, where*19
      logical fatal
      logical ttyout
      data ttyout /.true./
#ifdef T3D
      integer size
#endif
#endif
#ifdef MPI
      integer size
#endif

#if defined(PVM) || defined(MPI)

#ifdef PVM
#ifndef T3D
      intsize = INTEGER4
      encoding = PVMDATAINPLACE
#else
      intsize = INTEGER8
      encoding = PVMDATARAW
#endif
c start pvm: enroll and get local task identifier:
      call pvmfmytid( mytid )
      if ( mytid .lt. 0) then
        print *,myid,'failure to enroll main program'
        stop
      endif

#ifndef T3D
c try to get optimal comm links:
      call pvmfsetopt( PVMROUTE, PVMROUTEDIRECT, info)

c tty output for tasks, if desired:
      if ( ttyout ) call pvmfcatchout( 1, info )

      group_name = 'sweep3d'
      call pvmfjoingroup( group_name, info)
      if (info .lt. 0) then
        print *,myid,'bad instance number!)'
        call pvmfperror( 'joingroup: ',info)
        call pvmfexit( info )
        stop
      endif
c
c only the parent does i/o for # of processors and spawns tasks:
c
      call pvmfparent(parent)
      if (parent .lt. 0) then
         tids(1) = mytid
         myid = 1
      else
         myid = -1
         tids(1) = parent
      endif
c     print *,myid,mytid,'group instance:',info
#else
      group_name = PVMALL
      call pvmfgsize(group_name,size)
      call pvmfgetpe(mytid,myid)
      mytid = myid
      myid = myid + 1
      tids(1) = 0
c     print *,myid,mytid,size
#endif
#endif
#ifdef MPI
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
c     print *, myid,mytid,size
#endif

      numtasks = 0
      if (myid .eq. 1) then
         open(3,file='input',status='old')
         read (3,*) npe_i, npe_j
         close(3)
         numtasks = npe_i * npe_j
#ifdef PVM
#ifndef T3D
         if (numtasks.gt.maxtasks) then
            print *,'tids array too small'
            call pvmfexit(info)
            stop
         endif
         if (numtasks .le. 0) then
            print *,'bad npe_i/npe_j; try again'
            call pvmfexit(info)
            stop
         endif
         if (numtasks.gt.1) then
            task = 'sweep3d.pvm'
            where = '*'
            call pvmfspawn(task,PVMDEFAULT,where,numtasks-1,tids(2),
     &         info)
            if (info.le.0) then
               print *,'system trouble in pvmfspawn'
               call pvmfexit(info)
               stop
            else if (info.lt.numtasks-1) then
               print *,'partial trouble in pvmfspawn: only',info,
     &              'tasks spawned'
               do i = 2, info+1
                  call pvmfkill(tids(i),info)
               end do
               call pvmfexit(info)
               stop
            endif
         endif
#else
         do i = 1, min(numtasks,maxtasks)
            tids(i) = i - 1
         end do
#endif
#endif
#ifdef MPI
         do i = 1, min(numtasks,maxtasks)
            tids(i) = i - 1
         end do
#endif
      endif


c
c send initialization data to other tasks:
c

#ifdef PVM
#ifndef T3D
      if (myid.eq.1) then
         do i = 1, numtasks
            call snd_int(i, numtasks, 1, 100, info)
         end do
      else
         call rcv_int(1, numtasks, 1, 100, info)
      endif
      call barrier_sync()
#else
      call bcast_int(numtasks, 1, 100, 1, info)
      if (numtasks.gt.maxtasks) then
         print *,'tids array too small'
         call task_end()
         stop
      endif
      if (numtasks .le. 0) then
         print *,'bad npe_i/npe_j; try again'
         call task_end()
         stop
      endif
      if (numtasks .ne. size) then
         if (myid.eq.1) print *,'tasks do not match T3D size'
         call task_end()
         stop
      endif
#endif
#endif
#ifdef MPI
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
#endif

      call bcast_int(tids, numtasks, 101, 1, info)
      do i = 1, numtasks
         if ( mytid .eq. tids(i) ) myid = i
      end do
c     print *,myid,'tids exchanged:',(tids(i),i=1,numtasks)

c    Make sure every task has finished starting

      call barrier_sync()
c     print *,myid,'taskinit barrier_sync complete'

#else
c
c single processor
      myid = 1
      mytid = 0
      tids(1) = mytid
      numtasks = 1
#endif

c     ihost = hostnm_(my_name)
c     print *,myid,mytid,' running on ',my_name

      myid_r = myid
      numtasks_r = numtasks

      return
      end


      subroutine task_end()

      implicit none

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer info

#ifdef PVM
#ifndef T3D
      call pvmflvgroup( group_name, info)
#endif
      call pvmfexit( info )
      if (myid.ne.1) stop
#endif
#ifdef MPI
      call mpi_finalize(info)
#endif

      return
      end


      subroutine snd_real(dest, value, size, tag, info)

      implicit none

      integer dest, size, tag, info
      double precision value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

c     print *,myid,'snd_real: ',dest,size,tag
      info = 0
#ifdef PVM
#ifdef PSEND
      call pvmfpsend( tids(dest), tag, value, size, REAL8, info)
#else
      call pvmfinitsend ( encoding, info )
      call pvmfpack ( REAL8, value, size, 1, info )
      call pvmfsend ( tids(dest), tag, info )
#endif
#endif
#ifdef MPI
      call mpi_send(value, size, MPI_DOUBLE_PRECISION,
     &     tids(dest), tag, MPI_COMM_WORLD, info)
#endif

      return
      end


      subroutine rcv_real(orig, value, size, tag, info)

      implicit none

      integer orig, size, tag, info
      double precision value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer orig_tid
#if defined(PVM) && defined(PSEND)
      integer atid,atag,alen
#endif
#ifdef MPI
      integer status(MPI_STATUS_SIZE)
#endif

      info = 0
#ifdef PVM
      if (orig .eq. -1) then
        orig_tid = -1
      else
        orig_tid = tids(orig)
      endif
#ifdef PSEND
      call pvmfprecv( orig_tid, tag, value, size, REAL8,
     &     atid, atag, alen, info)
#else
      call pvmfrecv ( orig_tid, tag, info )
      call pvmfunpack ( REAL8, value, size, 1, info )
#endif
#endif
#ifdef MPI
      if (orig .eq. -1) then
        orig_tid = MPI_ANY_SOURCE
      else
        orig_tid = tids(orig)
      endif
      call mpi_recv(value, size, MPI_DOUBLE_PRECISION,
     &     orig_tid, tag, MPI_COMM_WORLD, status, info)
#endif
c     print *,myid,'rcv_real: ',orig,size,tag,orig_tid

      return
      end


      subroutine snd_int(dest, value, size, tag, info)

      implicit none

      integer dest, size, tag, info
      integer value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

c     print *,myid,'snd_int: ',dest,size,tag,value(1),tids(dest)
      info = 0
#ifdef PVM
#ifdef PSEND
      call pvmfpsend( tids(dest), tag, value, size, intsize, info)
#else
      call pvmfinitsend ( encoding, info )
      call pvmfpack ( intsize, value, size, 1, info )
      call pvmfsend ( tids(dest), tag, info )
#endif
#endif
#ifdef MPI
      call mpi_send(value, size, MPI_INTEGER,
     &     tids(dest), tag, MPI_COMM_WORLD, info)
#endif

      return
      end


      subroutine rcv_int(orig, value, size, tag, info)

      implicit none

      integer orig, size, tag, info
      integer value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer orig_tid
#if defined(PVM) && defined(PSEND)
      integer atid,atag,alen
#endif
#ifdef MPI
      integer status(MPI_STATUS_SIZE)
#endif

      info = 0
#ifdef PVM
      if (orig .eq. -1) then
        orig_tid = -1
      else
        orig_tid = tids(orig)
      endif
#ifdef PSEND
      call pvmfprecv( orig_tid, tag, value, size, intsize,
     &     atid, atag, alen, info)
#else
      call pvmfrecv ( orig_tid, tag, info )
      call pvmfunpack ( intsize, value, size, 1, info )
#endif
#endif
#ifdef MPI
      if (orig .eq. -1) then
        orig_tid = MPI_ANY_SOURCE
      else
        orig_tid = tids(orig)
      endif
      call mpi_recv(value, size, MPI_INTEGER,
     &     orig_tid, tag, MPI_COMM_WORLD, status, info)
#endif
c     print *,myid,'rcv_int: ',orig,size,tag,value(1),orig_tid

      return
      end


      subroutine bcast_real(value, size, tag, root, info)

      implicit none

      integer size, tag, root, info
      double precision value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      if (numtasks .eq. 1) return

c     print *,myid,'bcast_real: ',size,tag,root
      info = 0
#ifdef PVM
      if (myid .eq. root) then
        call pvmfinitsend ( encoding, info )
        call pvmfpack ( REAL8, value, size, 1, info )
        call pvmfbcast(group_name, tag, info )
      else
        call pvmfrecv (tids(root), tag, info)
        call pvmfunpack ( REAL8, value, size, 1, info )
      endif
#endif
#ifdef MPI
      call mpi_bcast(value, size, MPI_DOUBLE_PRECISION,
     &       tids(root), MPI_COMM_WORLD, info)
#endif

      return
      end


      subroutine bcast_int(value, size, tag, root, info)

      implicit none

      integer size, tag, root, info
      integer value(size)

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      if (numtasks .eq. 1) return

c     print *,myid,'bcast_int: ',size,tag,root
      info = 0
#ifdef PVM
      if (myid .eq. root) then
        call pvmfinitsend ( encoding, info )
        call pvmfpack ( intsize, value, size, 1, info )
        call pvmfbcast(group_name, tag, info )
      else
        call pvmfrecv (tids(root), tag, info)
        call pvmfunpack ( intsize, value, size, 1, info )
      endif
#endif
#ifdef MPI
      call mpi_bcast(value, size, MPI_INTEGER,
     &       tids(root), MPI_COMM_WORLD, info)
#endif

      return
      end


      subroutine global_real_max(value)

      implicit none

      double precision value

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer msgtag, i, info
      double precision y

      info = 0

#ifdef REDUCE
#ifdef PVM
      call pvmfgetinst(group_name,tids(1),i)
      call pvmfreduce(PVMMAX,value,1,REAL8,333,group_name,i,info)
      if (myid .eq. 1) then
        call pvmfinitsend ( encoding, info )
        call pvmfpack ( REAL8, value, 1, 1, info )
        call pvmfbcast(group_name, 334, info )
      else
        call pvmfrecv (tids(1), 334, info)
        call pvmfunpack ( REAL8, value, 1, 1, info )
      endif
#endif
#ifdef MPI
      call mpi_allreduce(value,y,1,MPI_DOUBLE_PRECISION,MPI_MAX,
     &     MPI_COMM_WORLD,info)
      value = y
#endif
#else
      msgtag = 333
      if (myid .eq. 1) then
c  receives values from nodes and find the max
         do i = 2, numtasks
            call rcv_real(-1, y, 1, msgtag, info)
            if (info.ne.0) print *,myid,'global_real_max: recv trouble'
            value = max(value,y)
         end do
      else
c  tasks send their local max value to host:
         call snd_real(1, value, 1, msgtag, info)
         if (info.ne.0) print *,myid,'global_real_max: send trouble'
      endif
c  master broadcasts global max
      info = 0
      msgtag = 334
      call bcast_real(value, 1, msgtag, 1, info)
      if (info.ne.0) print *,myid,'global_real_max: bcast trouble'
#endif

      return
      end


      subroutine global_real_sum(value)

      implicit none

      double precision value

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer msgtag, i, info
      double precision y

      info = 0

#ifdef REDUCE
#ifdef PVM
      call pvmfgetinst(group_name,tids(1),i)
      call pvmfreduce(PVMSUM,value,1,REAL8,444,group_name,i,info)
      if (myid .eq. 1) then
        call pvmfinitsend ( encoding, info )
        call pvmfpack ( REAL8, value, 1, 1, info )
        call pvmfbcast(group_name, 445, info )
      else
        call pvmfrecv (tids(1), 445, info)
        call pvmfunpack ( REAL8, value, 1, 1, info )
      endif
#endif
#ifdef MPI
      call mpi_allreduce(value,y,1,MPI_DOUBLE_PRECISION,MPI_SUM,
     &     MPI_COMM_WORLD,info)
      value = y
#endif
#else
      msgtag  = 444
      if (myid .eq. 1) then
c  receive values from nodes and sum them IN ORDER
         do i = 2, numtasks
            call rcv_real(i, y, 1, msgtag, info)
            if (info.ne.0) print *,myid,'global_real_sum: recv trouble'
            value = value + y
         end do
      else
c  tasks send their local value to host:
         call snd_real(1, value, 1, msgtag, info)
         if (info.ne.0) print *,myid,'global_real_sum: send trouble'
      endif
c  master broadcasts global sum
      info = 0
      msgtag = 445
      call bcast_real(value, 1, msgtag, 1, info)
      if (info.ne.0) print *,myid,'global_real_sum: bcast trouble'
#endif

      return
      end


      subroutine global_int_sum(value)

      implicit none

      integer value

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer msgtag, i, info
      integer y

      info = 0

#ifdef REDUCE
#ifdef PVM
      call pvmfgetinst(group_name,tids(1),i)
      call pvmfreduce(PVMSUM,value,1,intsize,444,
     &         group_name,i,info)
      if (myid .eq. 1) then
        call pvmfinitsend ( encoding, info )
        call pvmfpack ( intsize, value, 1, 1, info )
        call pvmfbcast(group_name, 445, info )
      else
        call pvmfrecv (tids(1), 445, info)
        call pvmfunpack ( intsize, value, 1, 1, info )
      endif
#endif
#ifdef MPI
      call mpi_allreduce(value,y,1,MPI_INTEGER,MPI_SUM,
     &     MPI_COMM_WORLD,info)
      value = y
#endif
#else
      msgtag  = 444
      if (myid .eq. 1) then
c  receive values from nodes and sum them IN ANY ORDER
         do i = 2, numtasks
            call rcv_real(-1, y, 1, msgtag, info)
            if (info.ne.0) print *,myid,'global_int_sum: recv trouble'
            value = value + y
         end do
      else
c  tasks send their local value to host:
         call snd_real(1, value, 1, msgtag, info)
         if (info.ne.0) print *,myid,'global_int_sum: send trouble'
      endif
c  master broadcasts global sum
      info = 0
      msgtag = 445
      call bcast_int(value, 1, msgtag, 1, info)
      if (info.ne.0) print *,myid,'global_int_sum: bcast trouble'
#endif

      return
      end


      subroutine barrier_sync()

      implicit none

#ifdef PVM
      include 'fpvm3.h'
#endif
#ifdef MPI
      include 'mpif.h'
#endif

      include 'msg_stuff.h'

      integer info

      if (numtasks .eq. 1) return

c     print *,myid,'barrier_sync:'
#ifdef PVM
      call pvmfbarrier(group_name, numtasks, info)
#endif
#ifdef MPI
      call mpi_barrier(MPI_COMM_WORLD,info)
#endif

      return
      end
