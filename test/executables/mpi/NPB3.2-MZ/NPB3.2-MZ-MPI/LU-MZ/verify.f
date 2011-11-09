
c---------------------------------------------------------------------
c---------------------------------------------------------------------

        subroutine verify(xcr, xce, xci, verified)

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c  verification routine                         
c---------------------------------------------------------------------

        implicit none
        include 'header.h'

        double precision xcr(5), xce(5), xci
        double precision xcrref(5),xceref(5),xciref, 
     >                   xcrdif(5),xcedif(5),xcidif,
     >                   epsilon, dtref
        integer m, itmaxref
        logical verified

c---------------------------------------------------------------------
c   tolerance level
c---------------------------------------------------------------------
        epsilon = 1.0d-08

        do m = 1, 5
           xcrref(m) = 1.0d0
           xceref(m) = 1.0d0
        enddo
        xciref   = 1.0d0

        verified = .true.

        if (class .eq. 'S') then
           dtref = 5.0d-1
           itmaxref = 50

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.3778579699366d+01
           xcrref(2) = 0.3120418698065d+00
           xcrref(3) = 0.8386213407018d+00
           xcrref(4) = 0.4452165980488d+00
           xcrref(5) = 0.7808656756434d+01

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.2429480066305d+02
           xceref(2) = 0.9072817470024d+01
           xceref(3) = 0.1032621825644d+02
           xceref(4) = 0.9256791727838d+01
           xceref(5) = 0.1639045777714d+02

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.4964435445706d+02

        elseif (class .eq. 'W') then
           dtref = 1.5d-3
           itmaxref = 300

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.8285060230339d+03
           xcrref(2) = 0.5753415004693d+02
           xcrref(3) = 0.2023477570531d+03
           xcrref(4) = 0.1586275182502d+03
           xcrref(5) = 0.1733925947816d+04

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.7514670702651d+02
           xceref(2) = 0.9776687033238d+01
           xceref(3) = 0.2141754291209d+02
           xceref(4) = 0.1685405918675d+02
           xceref(5) = 0.1856944519722d+03

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.3781055348911d+03

        elseif (class .eq. 'A') then
           dtref = 2.0d+0
           itmaxref = 250

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.1131574877175d+04
           xcrref(2) = 0.7965206944742d+02
           xcrref(3) = 0.2705587159526d+03
           xcrref(4) = 0.2129567530746d+03
           xcrref(5) = 0.2260584655432d+04

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.1115694885382d+03
           xceref(2) = 0.1089257673798d+02
           xceref(3) = 0.2905379922066d+02
           xceref(4) = 0.2216126755530d+02
           xceref(5) = 0.2501762341026d+03

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.5904992211511d+03

        elseif (class .eq. 'B') then
           dtref = 2.0d+0
           itmaxref = 250
 
c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.1734656959567d+05
           xcrref(2) = 0.1238977748533d+04
           xcrref(3) = 0.4123885357100d+04
           xcrref(4) = 0.3613705834056d+04
           xcrref(5) = 0.3531187871586d+05

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.1781612313296d+04
           xceref(2) = 0.1177971120769d+03
           xceref(3) = 0.4233792871440d+03
           xceref(4) = 0.3577260438230d+03
           xceref(5) = 0.3659958544012d+04

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.6107041476456d+04

        elseif (class .eq. 'C') then
           dtref = 2.0d+0
           itmaxref = 250

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.4108743427233d+05
           xcrref(2) = 0.3439004802235d+04
           xcrref(3) = 0.9961331392486d+04
           xcrref(4) = 0.8321426758084d+04
           xcrref(5) = 0.7463792419218d+05

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.3429276307955d+04
           xceref(2) = 0.2336680861825d+03
           xceref(3) = 0.8216363109621d+03
           xceref(4) = 0.7143809828225d+03
           xceref(5) = 0.7057470798773d+04

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.1125826349653d+05

        elseif (class .eq. 'D') then
           dtref = 1.0d+0
           itmaxref = 300

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.3282253166388d+06
           xcrref(2) = 0.3490781637713d+05
           xcrref(3) = 0.8610311978292d+05
           xcrref(4) = 0.7004896022603d+05
           xcrref(5) = 0.4546838584391d+06

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.6620775619126d+04
           xceref(2) = 0.5229798207352d+03
           xceref(3) = 0.1620218261697d+04
           xceref(4) = 0.1404783445006d+04
           xceref(5) = 0.1222629805121d+05

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.2059421629621d+05

        elseif (class .eq. 'E') then
           dtref = 0.5d+0
           itmaxref = 300

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.1539988626779d+07
           xcrref(2) = 0.1742224758490d+06
           xcrref(3) = 0.4153598861059d+06
           xcrref(4) = 0.3468381400447d+06
           xcrref(5) = 0.2054406022038d+07

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.8021145134635d+04
           xceref(2) = 0.6932079823791d+03
           xceref(3) = 0.1998959591111d+04
           xceref(4) = 0.1725962639357d+04
           xceref(5) = 0.1389447024442d+05
 
c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.2334131124791d+05

        elseif (class .eq. 'F') then
           dtref = 0.2d+0
           itmaxref = 300

c---------------------------------------------------------------------
c   Reference values of RMS-norms of residual
c---------------------------------------------------------------------
           xcrref(1) = 0.7116424271317d+07
           xcrref(2) = 0.8159357680842d+06
           xcrref(3) = 0.1930561069782d+07
           xcrref(4) = 0.1633447037519d+07
           xcrref(5) = 0.9417323380798d+07

c---------------------------------------------------------------------
c   Reference values of RMS-norms of solution error
c---------------------------------------------------------------------
           xceref(1) = 0.8648720989200d+04
           xceref(2) = 0.7774221260694d+03
           xceref(3) = 0.2175462599498d+04
           xceref(4) = 0.1875280641999d+04
           xceref(5) = 0.1457903413233d+05

c---------------------------------------------------------------------
c   Reference value of surface integral
c---------------------------------------------------------------------
           xciref    = 0.2448986519022d+05

           if (itmax .eq. 30) then

           itmaxref = 30
           xcrref(1) = 0.3814950058736d+08
           xcrref(2) = 0.4280439009977d+07
           xcrref(3) = 0.1016353864923d+08
           xcrref(4) = 0.8627208852987d+07
           xcrref(5) = 0.5024448179760d+08

           xceref(1) = 0.8903253221139d+04
           xceref(2) = 0.8129462858441d+03
           xceref(3) = 0.2248648703838d+04
           xceref(4) = 0.1937258920446d+04
           xceref(5) = 0.1485251162647d+05

           xciref    = 0.2792087395236d+05

           endif
        else
           dtref = 0.0d+0
           itmaxref = 0
           verified = .FALSE.
        endif

c---------------------------------------------------------------------
c    Compute the difference of solution values and the known reference values.
c---------------------------------------------------------------------
        do m = 1, 5
           
           xcrdif(m) = dabs((xcr(m)-xcrref(m))/xcrref(m)) 
           xcedif(m) = dabs((xce(m)-xceref(m))/xceref(m))
           
        enddo
        xcidif = dabs((xci - xciref)/xciref)


c---------------------------------------------------------------------
c    Output the comparison of computed results to known cases.
c---------------------------------------------------------------------

        write(*, 1990) class
 1990   format(/, ' Verification being performed for class ', a)
        write (*,2000) epsilon
 2000   format(' Accuracy setting for epsilon = ', E20.13)
        if (dabs(dt-dtref) .gt. epsilon) then  
           verified = .false.
           write (*,1000) dtref
 1000      format(' DT does not match the reference value of ', 
     >              E15.8)
        else if (itmax .ne. itmaxref) then
           verified = .false.
           write (*,1002) itmaxref
 1002      format(' ITMAX does not match the reference value of ', 
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
        
 2010   format(' FAILURE: ', i2, 2x, E20.13, E20.13, E20.13)
 2011   format('          ', i2, 2x, E20.13, E20.13, E20.13)
        
        write (*,2025)
 2025   format(' Comparison of surface integral')

        if (xcidif .le. epsilon) then
           write(*, 2032) xci, xciref, xcidif
        else
           verified = .false.
           write(*, 2031) xci, xciref, xcidif
        endif

 2031   format(' FAILURE: ', 4x, E20.13, E20.13, E20.13)
 2032   format('          ', 4x, E20.13, E20.13, E20.13)

        if (verified) then
           write(*, 2020)
 2020      format(' Verification Successful')
        else
           write(*, 2021)
 2021      format(' Verification failed')
        endif

        return
        end
