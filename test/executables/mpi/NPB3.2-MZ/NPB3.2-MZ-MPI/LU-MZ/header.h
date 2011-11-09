
c---------------------------------------------------------------------
c---------------------------------------------------------------------
c---  header.h
c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c   npbparams.h defines parameters that depend on the class and 
c   number of nodes
c---------------------------------------------------------------------

      include 'npbparams.h'

c---------------------------------------------------------------------
c   parameters which can be overridden in runtime config file
c   isiz1,isiz2,isiz3 give the maximum size
c   ipr = 1 to print out verbose information
c   omega = 2.0 is correct for all classes
c   tolrsd is tolerance levels for steady state residuals
c---------------------------------------------------------------------
      integer ipr_default
      parameter (ipr_default = 1)
      double precision omega_default
      parameter (omega_default = 1.2d0)
      double precision tolrsd1_def, tolrsd2_def, tolrsd3_def, 
     >                 tolrsd4_def, tolrsd5_def
      parameter (tolrsd1_def=1.0e-08, 
     >          tolrsd2_def=1.0e-08, tolrsd3_def=1.0e-08, 
     >          tolrsd4_def=1.0e-08, tolrsd5_def=1.0e-08)

      double precision c1, c2, c3, c4, c5
      parameter( c1 = 1.40d+00, c2 = 0.40d+00,
     >           c3 = 1.00d-01, c4 = 1.00d+00,
     >           c5 = 1.40d+00 )

c---------------------------------------------------------------------
c   grid
c---------------------------------------------------------------------
      double precision  dxi, deta, dzeta
      double precision  tx1, tx2, tx3
      double precision  ty1, ty2, ty3
      double precision  tz1, tz2, tz3

      common/cgcon/ dxi, deta, dzeta,
     >              tx1, tx2, tx3,
     >              ty1, ty2, ty3,
     >              tz1, tz2, tz3

c---------------------------------------------------------------------
c   dissipation
c---------------------------------------------------------------------
      double precision dx1, dx2, dx3, dx4, dx5
      double precision dy1, dy2, dy3, dy4, dy5
      double precision dz1, dz2, dz3, dz4, dz5
      double precision dssp

      common/disp/ dx1,dx2,dx3,dx4,dx5,
     >             dy1,dy2,dy3,dy4,dy5,
     >             dz1,dz2,dz3,dz4,dz5,
     >             dssp

      integer  x_start(x_zones), x_end(x_zones), x_size(x_zones),
     >         y_start(y_zones), y_end(y_zones), y_size(y_zones)
      common /zones/ x_start, x_end, x_size, y_start, y_end, y_size

c---------------------------------------------------------------------
c   output control parameters
c---------------------------------------------------------------------
      integer ipr, inorm, npb_verbose

      common/cprcon/ ipr, inorm, npb_verbose

c---------------------------------------------------------------------
c   newton-raphson iteration control parameters
c---------------------------------------------------------------------
      integer itmax, invert
      double precision  dt, omega, tolrsd(5), ttotal

      common/ctscon/ dt, omega, tolrsd, ttotal, itmax, invert

c---------------------------------------------------------------------
c   coefficients of the exact solution
c---------------------------------------------------------------------
      double precision ce(5,13)

      common/cexact/ ce

c---------------------------------------------------------------------
c   1-d working arrays
c---------------------------------------------------------------------
      double precision  flux(5,problem_size), 
     >                  utmp(6,problem_size),
     >                  rtmp(5,problem_size)

      common/work_1d/ flux, utmp, rtmp
!$omp threadprivate( /work_1d/ )

c---------------------------------------------------------------------
c   timers
c---------------------------------------------------------------------
      integer t_rhsx,t_rhsy,t_rhsz,t_rhs,t_jacld,t_blts,t_jacu,
     >        t_buts,t_add,t_l2norm,t_rdis1,t_rdis2,t_last,t_total
      parameter (t_total = 1)
      parameter (t_rhsx = 2)
      parameter (t_rhsy = 3)
      parameter (t_rhsz = 4)
      parameter (t_rhs = 5)
      parameter (t_jacld = 6)
      parameter (t_blts = 7)
      parameter (t_jacu = 8)
      parameter (t_buts = 9)
      parameter (t_add = 10)
      parameter (t_l2norm = 11)
      parameter (t_rdis1 = 12)
      parameter (t_rdis2 = 13)
      parameter (t_last = 13)
      logical timeron
      double precision maxtime

      common/timer/maxtime,timeron


c---------------------------------------------------------------------
c   end of include file
c---------------------------------------------------------------------
