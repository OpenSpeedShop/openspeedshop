          program stress_test
!=====================================================
!  Simple contrived program to see how systems perform
!  repeated OpenMP multi-threaded tests.
!  This test repeatedly "smooths" or "filters" a 3-d
!  (gridded) array, starting either from a random or
!  deterministic data array.
! 
!  This makes a good system "stress test", since the
!  arrays can be as larger or small as required, and
!  the job can be run for as long or as short as
!  required (by varying the number of filtering passes).
!  The 7-point filter around the centre-point in a 3-d
!  lattice structure poses a performance challenge for
!  scalar processors!
!
!  This version creates an extended numerical domain by
!  filling a "shadow zone" around the global boundary
!  with boundary values.  This means that points i+/-1,
!  j+/-1,k+/-1 are always defined over the physical
!  domain.
!
!  The program reads 8 input variables, from 6 lines
!  of standard input, or redirected from a file. 
!  See "program parameters" below.
!=====================================================
       implicit none
       integer nx,ny,nz, i,j,k, iseed, icount, maxcount
       integer iofreq
!orig       parameter (nx=200, ny=200, nz=200 )
!
       real xran1, twopi,xdist,ydist,zdist
       real, allocatable :: arr_in(:,:,:), arr_out(:,:,:)
       real valmax, wght, rwght3,rwght4,rwght5,rwght6
!
!
!=====================================================
!      Some program parameters:
!=====================================================
      read(*,*) maxcount  
      read(*,*) nx,ny,nz
      read(*,*) iofreq
      read(*,*) wght
      read(*,*) valmax
      read(*,*) iseed
!
!      valmax = 100.00
!      iseed = -135
!      wght = 0.01     ! Read this in...
        rwght3 = 1.0 - 3.0*wght
        rwght4 = 1.0 - 4.0*wght
        rwght5 = 1.0 - 5.0*wght
        rwght6 = 1.0 - 6.0*wght
!
!
!orig      allocate (arr_in(nx,ny,nz))
!orig      allocate (arr_out(nx,ny,nz))
      allocate (arr_in(nx+2,ny+2,nz+2))
! Extra points are the "shadow zone"
      allocate (arr_out(nx+2,ny+2,nz+2))
!
      print *, 'Maxcount:        ',maxcount
      print *, 'nx,ny,nz:        ',nx,ny,nz
      print *, 'Output interval: ',iofreq
      print *, 'Weight (wght):   ',wght
      print *, 'Value range:     ',valmax
      print *, 'Random seed:     ',iseed
      if(iseed.eq.0) then
         print *, 'Using deterministic initialization'
      else
         print *, 'Using random initialization'
      endif
      print *, ' '
!
!=====================================================
!  Initialize the main data array,
!  either deterministically, or with random numbers:
!=====================================================
      if(iseed.eq.0) then
        twopi = 8.0*atan(1.0)
        do k=1,nz
          zdist=twopi*float(k-1)/float(nz-1)
          do j=1,ny
            ydist=twopi*float(j-1)/float(ny-1)
            do i=1,nx
              xdist=twopi*float(i-1)/float(nx-1)
        arr_in(i+1,j+1,k+1) = valmax*Cos(7.*xdist)*cos(13.*ydist)*
     &              cos(11.*zdist)
            enddo
          enddo
        enddo
!
      else
!      
       do k=1,nz
         do j=1,ny
           do i=1,nx
             arr_in(i+1,j+1,k+1) = valmax*(-1. + 2.0*xran1(iseed))
           enddo
         enddo
       enddo
      endif
!
      if(iseed.eq.0) then
        write(*,*) 'Initial (deterministic) selected values are:'
      else
        write(*,*) 'Initial (random) selected values are:'
      endif
!orig        write(*,100) (arr_in(2,2,k),k=2,nz-1,nz/7)
        write(*,100) (arr_in(3,3,k),k=3,nz,nz/7)
!
!
      open (unit=12,file='stresstest.dat',form='formatted')
      write(12,*) 'The Parameters of this run are:'
      write(12,*) 'Maxcount:        ',maxcount
      write(12,*) 'nx,ny,nz:        ',nx,ny,nz
      write(12,*) 'Output interval: ',iofreq
      write(12,*) 'Weight (wght):   ',wght
      write(12,*) 'Value range:     ',valmax
      write(12,*) 'Random seed:     ',iseed
      if(iseed.eq.0) then
         write(12,*) 'Using deterministic initialization'
      else
         write(12,*) 'Using random initialization'
      endif
      write(12,*) ' '
      write(12,*) 'Initial random values are (i,j,k=1,10):'
!     write (12,100) (arr_in(i,j,k),i=1,10),j=1,10),k=1,10)
!orig write (12,100) (arr_in(10,10,k),k=1,10)
      write (12,100) (arr_in(11,11,k),k=2,11)
      write (12,*) ' '
!
!
!=======================================================
!     Start of outermost loop over smoothing iterations:
!=======================================================
!
      do icount=0,maxcount
!
!
!     Several different smoothing loops for different data sections:
!
!     Fill in "shadow zones" around all sides of the main array -
!     just copy the surface values into the shadow zone.  This
!     simplifies
!     the filtering process by avoiding special treatment for surfaces,
!     edges and corners.
!
!$OMP PARALLEL DO PRIVATE(i,j,k)
      do k=2,nz+1
! East/west faces:
          do j=2,ny+1
            arr_in(1,j,k) = arr_in(2,j,k)
            arr_in(nx+2,j,k) = arr_in(nx+1,j,k)
          enddo
!
! North/south faces:
          do i=2,nx+1
            arr_in(i,1,k) = arr_in(i,2,k)
            arr_in(i,ny+2,k) = arr_in(i,ny+1,k)
          enddo
      enddo
!$OMP END PARALLEL DO
!
!
! Top/botttom faces:
!$OMP PARALLEL DO PRIVATE(i,j)
      do j=2,ny+1
        do i=2,nx+1
          arr_in(i,j,1) = arr_in(i,j,2)
          arr_in(i,j,nz+2) = arr_in(i,j,nz+1)
        enddo
      enddo
!$OMP END PARALLEL DO
!
!
!
! Main body of (3-d) data:
!$OMP PARALLEL DO PRIVATE(i,j,k)
      do k=2,nz+1
        do j=2,ny+1
          do i=2,nx+1
            arr_out(i,j,k) = rwght6*arr_in(i,j,k) + wght*(
     &         arr_in(i-1,j,k) + arr_in(i+1,j,k) +
     &         arr_in(i,j-1,k) + arr_in(i,j+1,k) +
     &         arr_in(i,j,k-1) + arr_in(i,j,k+1) )
          enddo
        enddo
      enddo
!$OMP END PARALLEL DO
!
!
!
!--- Do a 2nd smoothing pass to update arr_in for next iteration:
!
!  First, fill the shadow zone around arr_out:
!
!$OMP PARALLEL DO PRIVATE(i,j,k)
      do k=2,nz+1
! East/west faces:
          do j=2,ny+1
            arr_out(1,j,k) = arr_out(2,j,k)
            arr_out(nx+2,j,k) = arr_out(nx+1,j,k)
          enddo
!
! North/south faces:
          do i=2,nx+1
            arr_out(i,1,k) = arr_out(i,2,k)
            arr_out(i,ny+2,k) = arr_out(i,ny+1,k)
          enddo
      enddo
!$OMP END PARALLEL DO
!
!
! Top/botttom faces:
!$OMP PARALLEL DO PRIVATE(i,j)
      do j=2,ny+1
        do i=2,nx+1
          arr_out(i,j,1) = arr_out(i,j,2)
          arr_out(i,j,nz+2) = arr_out(i,j,nz+1)
        enddo
      enddo
!$OMP END PARALLEL DO
!
!
!    Main body of data:
!$OMP PARALLEL DO PRIVATE(i,j,k)
      do k=2,nz+1
        do j=2,ny+1
          do i=2,nx+1
            arr_in(i,j,k) = rwght6*arr_out(i,j,k) + wght*(
     &         arr_out(i-1,j,k) + arr_out(i+1,j,k) +
     &         arr_out(i,j-1,k) + arr_out(i,j+1,k) +
     &         arr_out(i,j,k-1) + arr_out(i,j,k+1) )
          enddo
        enddo
      enddo
!$OMP END PARALLEL DO
!
!
     
      if(icount.eq.0) write(*,*) 'Successively smoothed values are:'
      if(icount.lt.10 .or. mod(icount,iofreq).eq.0)
     &       write(*,100) (arr_in(3,3,k),k=3,nz,nz/7)
!    &       write(*,100) (arr_in(2,2,k),k=2,nz-1,nz/7)
!
!
      enddo    ! (end of outer-most "iteration" loop over icount)
!
!================================================================
!
!
 999  write(12,*) 'Final (smoothed?) values are (i,j,k=1,10):'
!     write (12,100) (((arr_in(i,j,k),i=1,10),j=1,10),k=1,10)
!orig      write (12,100) (arr_in(10,10,k),k=1,10)
      write (12,100) (arr_in(11,11,k),k=2,11)
!
      stop 'Normal end, max smoothing iterations completed.'
 100  format(6e12.4)
      end




    
      function xran1(idum)
c--------------------------------------------------------------------
c     Routine from Numerical Recipes to return a uniform random
c     deviate between 0.0 and 1.0.  Set idum to any negative number
c     to initialize or reinitialize the sequence.
c--------------------------------------------------------------------
      parameter (nn=97)
      parameter (m1=259200, ia1=7141, ic1=54773, rm1=1./m1)
      parameter (m2=134456, ia2=8121, ic2=28411, rm2=1./m2)
      parameter (m3=243000, ia3=4561, ic3=51349)
      real xran1, r(nn)
      save r, ix1,ix2,ix3
c
      data iff /0/
      if (idum.lt.0 .or. iff.eq.0) then
        iff = 1
        ix1 = mod(ic1-idum,m1)     ! seed the first routine
        ix1 = mod(ia1*ix1+ic1,m1)
        ix2 = mod(ix1,m2)          ! and use it to seed the second
        ix1 = mod(ia1*ix1+ic1,m1)
        ix3 = mod(ix1,m3)          ! and to seed the third
c
        do 11,j=1,nn                ! fill the table with sequential
          ix1 = mod(ia1*ix1+ic1,m1) ! random deviates generated by the
          ix2 = mod(ia2*ix2+ic2,m2) ! first two routines
          r(j) = (float(ix1)+float(ix2)*rm2)*rm1
 11     continue
c
        idum = 1
      endif
c
      ix1 = mod(ia1*ix1+ic1,m1)
      ix2 = mod(ia2*ix2+ic2,m2)
      ix3 = mod(ia3*ix3+ic3,m3)
c
      j = 1+(nn*ix3)/m3  ! use 3rd sequence for no. between 1 and 97
c
      if(j.gt.nn.or.j.lt.1) pause
      xran1 = r(j)        ! return that table entry
      r(j) = (float(ix1)+float(ix2)*rm2)*rm1    ! and refill it
      return
      end

