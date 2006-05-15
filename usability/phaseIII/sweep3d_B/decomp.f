
      subroutine decomp(it_g,jt_g, it,jt, istart,jstart, kt,
     &       myid,numtasks,npe_i,npe_j,mype_i,mype_j,
     &       north,south,east,west)

      implicit none

      integer it_g,jt_g, it,jt, istart,jstart, kt
      integer myid,numtasks,npe_i,npe_j,mype_i,mype_j
      integer north,south,east,west

      integer remainder

c
c set position of each task in the processor grid (mype_i, mype_j):
c
      mype_i = mod(myid-1, npe_i) + 1
      mype_j = (myid-1) / npe_i + 1
c     print *,myid,'has mype_i/mype_j: ',mype_i,mype_j

c
c  Neighbor processors
c
      north = 0
      south = 0
      east  = 0
      west  = 0
      if ( mype_j .ne. npe_j )  north = myid + npe_i
      if ( mype_j .ne. 1 )      south = myid - npe_i
      if ( mype_i .ne. npe_i )  east  = myid + 1
      if ( mype_i .ne. 1 )      west  = myid - 1

      if (north.eq.0 .and. south.eq.0 .and.
     &     east.eq.0 .and.  west.eq.0 .and.
     &     numtasks.gt.1) then
          print *,' neighbor trouble in DECOMP '
          call task_end()
          stop
      endif

c
c  Local grid size
c
      it = it_g / npe_i
      istart = it * (mype_i-1) + 1
      remainder = mod(it_g, npe_i)
      if (mype_i .le. remainder) then
        it = it + 1
        istart = istart + mype_i - 1
      else
        istart = istart + remainder
      endif

      jt = jt_g / npe_j
      jstart = jt * (mype_j-1) + 1
      remainder = mod(jt_g, npe_j)
      if (mype_j .le. remainder) then
        jt = jt + 1
        jstart = jstart + mype_j - 1
      else
        jstart = jstart + remainder
      endif

c     istop = istart + it - 1
c     jstop = jstart + jt - 1
c     print *,myid,' has i from ',istart ,' to ',istop
c     print *,myid,' has j from ',jstart ,' to ',jstop
c     print *,myid,' has n/s/e/w: ',north,south,east,west

      return
      end
