
c---------------------------------------------------------------------
c---------------------------------------------------------------------

        subroutine verify(no_time_steps, verified, num_zones,
     $                    rho_i, us, vs, ws, speed, qs, square, 
     $                    rhs, forcing, u, nx, nxmax, ny, nz, 
     $                    start1, start5)


c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c  verification routine                         
c---------------------------------------------------------------------

        include 'header.h'
        include 'mpi_stuff.h'

        integer zone, num_zones
        double precision rho_i(*), us(*), vs(*), ws(*), speed(*), 
     $                   qs(*), square(*), rhs(*), forcing(*), u(*)

        double precision xcrref(5),xceref(5),xcrdif(5),xcedif(5), 
     >                   epsilon, xce(5), xcr(5), dtref, 
     $                   xce_sub(5), xcr_sub(5)
        integer m, no_time_steps, niterref, iz, ip
        integer nx(*), nxmax(*), ny(*), nz(*)
        dimension start1(*), start5(*)
        logical verified

c---------------------------------------------------------------------
c   tolerance level
c---------------------------------------------------------------------
        epsilon = 1.0d-08

c---------------------------------------------------------------------
c   compute the error norm and the residual norm
c---------------------------------------------------------------------

        do m = 1, 5
          xcr(m) = 0.d0
          xce(m) = 0.d0
        end do

        do iz = 1, proc_num_zones
          zone = proc_zone_id(iz)
          call error_norm (xce_sub, u(start5(iz)), 
     $                     nx(zone), nxmax(zone), ny(zone), nz(zone))
          call compute_rhs(rho_i(start1(iz)), us(start1(iz)), 
     $                     vs(start1(iz)), ws(start1(iz)), 
     $                     speed(start1(iz)), qs(start1(iz)), 
     $                     square(start1(iz)), rhs(start5(iz)), 
     $                     forcing(start5(iz)), u(start5(iz)), 
     $                     nx(zone), nxmax(zone), ny(zone), nz(zone))

          call rhs_norm   (xcr_sub, rhs(start5(iz)), 
     $                     nx(zone), nxmax(zone), ny(zone), nz(zone))

          do m = 1, 5
            xcr(m) = xcr(m) + xcr_sub(m) / dt
            xce(m) = xce(m) + xce_sub(m)
          end do

        end do

        do m = 1, 5
          xcr_sub(m) = xcr(m)
          xce_sub(m) = xce(m)
        end do
        call mpi_reduce(xcr_sub, xcr, 5, dp_type, MPI_SUM, 
     >                  root, comm_setup, ierror)
        call mpi_reduce(xce_sub, xce, 5, dp_type, MPI_SUM, 
     >                  root, comm_setup, ierror)

        if (myid .ne. root) return

        verified = .true.

        do m = 1,5
           xcrref(m) = 1.0
           xceref(m) = 1.0
        end do

c---------------------------------------------------------------------
c    reference data for class S
c---------------------------------------------------------------------
        if ( class .eq. 'S') then
           dtref = 1.5d-2
           niterref = 100

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.7698876173566d+01
           xcrref(2) = 0.1517766790280d+01
           xcrref(3) = 0.2686805141546d+01
           xcrref(4) = 0.1893688083690d+01
           xcrref(5) = 0.1369739859738d+02

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.9566808043467d+01
           xceref(2) = 0.3894109553741d+01
           xceref(3) = 0.4516022447464d+01
           xceref(4) = 0.4099103995615d+01
           xceref(5) = 0.7776038881521d+01

c---------------------------------------------------------------------
c    reference data for class W
c---------------------------------------------------------------------
        elseif ( class .eq. 'W') then
           dtref = 1.5d-3
           niterref = 400

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.1887636218359d+03
           xcrref(2) = 0.1489637963542d+02
           xcrref(3) = 0.4851711701400d+02
           xcrref(4) = 0.3384633608154d+02
           xcrref(5) = 0.4036632495857d+03

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.2975895149929d+02
           xceref(2) = 0.1341508175806d+02
           xceref(3) = 0.1585310846491d+02
           xceref(4) = 0.1450916426713d+02
           xceref(5) = 0.5854137431023d+02

c---------------------------------------------------------------------
c    reference data for class A
c---------------------------------------------------------------------
        elseif ( class .eq. 'A') then
           dtref = 1.5d-3
           niterref = 400

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.2800097900548d+03
           xcrref(2) = 0.2268349014438d+02
           xcrref(3) = 0.7000852739901d+02
           xcrref(4) = 0.5000771004061d+02
           xcrref(5) = 0.5552068537578d+03

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.3112046666578d+02
           xceref(2) = 0.1172197785348d+02
           xceref(3) = 0.1486616708032d+02
           xceref(4) = 0.1313680576292d+02
           xceref(5) = 0.7365834058154d+02

c---------------------------------------------------------------------
c    reference data for class B
c---------------------------------------------------------------------
        elseif ( class .eq. 'B') then
           dtref = 1.0d-3
           niterref = 400

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.5190422977921d+04
           xcrref(2) = 0.3655458539065d+03
           xcrref(3) = 0.1261126592633d+04
           xcrref(4) = 0.1002038338842d+04
           xcrref(5) = 0.1075902511165d+05

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.5469182054223d+03
           xceref(2) = 0.4983658028989d+02
           xceref(3) = 0.1418301776602d+03
           xceref(4) = 0.1097717156175d+03
           xceref(5) = 0.1260195162174d+04

c---------------------------------------------------------------------
c    reference data for class C
c---------------------------------------------------------------------
        elseif ( class .eq. 'C') then
           dtref = 0.67d-3
           niterref = 400

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.5886814493676d+05
           xcrref(2) = 0.3967324375474d+04
           xcrref(3) = 0.1444126529019d+05
           xcrref(4) = 0.1210582211196d+05
           xcrref(5) = 0.1278941567976d+06

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.6414069213021d+04
           xceref(2) = 0.4069468353404d+03
           xceref(3) = 0.1585311908719d+04
           xceref(4) = 0.1270243185759d+04
           xceref(5) = 0.1441398372869d+05

c---------------------------------------------------------------------
c    reference data for class D
c---------------------------------------------------------------------
        elseif ( class .eq. 'D') then
           dtref = 0.3d-3
           niterref = 500

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.7650595424723d+06
           xcrref(2) = 0.5111519817683d+05
           xcrref(3) = 0.1857213937602d+06
           xcrref(4) = 0.1624096784059d+06
           xcrref(5) = 0.1642416844328d+07

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.8169589578340d+05
           xceref(2) = 0.5252150843148d+04
           xceref(3) = 0.1984739188642d+05
           xceref(4) = 0.1662852404547d+05
           xceref(5) = 0.1761381855235d+06

c---------------------------------------------------------------------
c    reference data for class E
c---------------------------------------------------------------------
        elseif ( class .eq. 'E') then
           dtref = 0.2d-3
           niterref = 500

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.5058298119039d+07
           xcrref(2) = 0.3576837494299d+06
           xcrref(3) = 0.1230856227329d+07
           xcrref(4) = 0.1093895671677d+07
           xcrref(5) = 0.1073671658903d+08

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.5288293042051d+06
           xceref(2) = 0.3471875724140d+05
           xceref(3) = 0.1282998930808d+06
           xceref(4) = 0.1095483394612d+06
           xceref(5) = 0.1129716454231d+07

c---------------------------------------------------------------------
c    reference data for class F
c---------------------------------------------------------------------
        elseif ( class .eq. 'F') then
           dtref = 0.1d-3
           niterref = 500

c---------------------------------------------------------------------
c    Reference values of RMS-norms of residual.
c---------------------------------------------------------------------
           xcrref(1) = 0.3974469160412d+08
           xcrref(2) = 0.3260760921834d+07
           xcrref(3) = 0.9756215393494d+07
           xcrref(4) = 0.8278472138497d+07
           xcrref(5) = 0.7547269314441d+08

c---------------------------------------------------------------------
c    Reference values of RMS-norms of solution error.
c---------------------------------------------------------------------
           xceref(1) = 0.3475757666334d+07
           xceref(2) = 0.2386799228183d+06
           xceref(3) = 0.8436705443034d+06
           xceref(4) = 0.7339112115118d+06
           xceref(5) = 0.7327832757877d+07

           if (no_time_steps .eq. 50) then

           niterref = 50
           xcrref(1) = 0.3198801286787d+09
           xcrref(2) = 0.3435698123358d+08
           xcrref(3) = 0.8489831174901d+08
           xcrref(4) = 0.6940707552477d+08
           xcrref(5) = 0.4478684103255d+09

           xceref(1) = 0.6761099692230d+07
           xceref(2) = 0.5361561494769d+06
           xceref(3) = 0.1662878706114d+07
           xceref(4) = 0.1443852092060d+07
           xceref(5) = 0.1260678700480d+08

           endif
        else
           dtref = 0.0d0
           niterref = 0
           verified = .false.
        endif

c---------------------------------------------------------------------
c    Compute the difference of solution values and the known reference values.
c---------------------------------------------------------------------
        do m = 1, 5
           
           xcrdif(m) = dabs((xcr(m)-xcrref(m))/xcrref(m)) 
           xcedif(m) = dabs((xce(m)-xceref(m))/xceref(m))
           
        enddo

c---------------------------------------------------------------------
c    Output the comparison of computed results to known cases.
c---------------------------------------------------------------------

        write(*, 1990) class
 1990   format(' Verification being performed for class ', a)
        write (*,2000) epsilon
 2000   format(' accuracy setting for epsilon = ', E20.13)
        if (dabs(dt-dtref) .gt. epsilon) then  
           verified = .false.
           write (*,1000) dtref
 1000      format(' DT does not match the reference value of ', 
     >              E15.8)
        else if (no_time_steps .ne. niterref) then
           verified = .false.
           write (*,1002) niterref
 1002      format(' NITER does not match the reference value of ', 
     >              I5)
        endif
      
        write (*,2001) 

 2001   format(' Comparison of RMS-norms of residual')
        do m = 1, 5
           if (xcrdif(m) .le. epsilon) then
              write (*,2011) m,xcr(m),xcrref(m),xcrdif(m)
           else 
              verified = .false.
              write (*,2010) m,xcr(m),xcrref(m),xcrdif(m)
           endif
        enddo

        write (*,2002)

 2002   format(' Comparison of RMS-norms of solution error')
        do m = 1, 5
           if (xcedif(m) .le. epsilon) then
              write (*,2011) m,xce(m),xceref(m),xcedif(m)
           else
              verified = .false.
              write (*,2010) m,xce(m),xceref(m),xcedif(m)
           endif
        enddo
        
 2010   format(' FAILURE: ', i2, E20.13, E20.13, E20.13)
 2011   format('          ', i2, E20.13, E20.13, E20.13)
        
        if (verified) then
           write(*, 2020)
 2020      format(' Verification Successful')
        else
           write(*, 2021)
 2021      format(' Verification failed')
        endif

        return
        end
