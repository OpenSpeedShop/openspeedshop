//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



#if defined( _WIN32 )
#  include <windows.h>
#  include <mmsystem.h>
#else
#  include <sys/time.h>
#  include <sys/resource.h>
#endif

#include <stdio.h>
#include <iostream.h>
#include <ggTimer.h>

/*
 * 12/05 KAI
 * sparc's gettimeofday() takes only 1 parameter, and
 * it's not defined in the /usr/include/sys/time.h !
 * 08/25/95 Also Intel Pentium's (machine Dunsel)
 */
#if defined(sparc) || defined(sun) || defined(i386)
extern "C" {
extern int gettimeofday(struct timeval *);
}
#endif



/******************************************************************************
*
*  Members
*
*/

/*
 * Constructors/destructors
 */

ggTimer::ggTimer() {}

/*
 * Methods
 */

void
ggTimer::start() {
#if defined(sparc) || defined(sun) || defined(i386)
    gettimeofday(&startRTime);
#elif defined( _WIN32 )
    startRTime = timeGetTime();    
#else
    gettimeofday(&startRTime, 0);
#endif
 //   getrusage(RUSAGE_SELF, &startRUsage);
}

void
ggTimer::stop() {
#if defined(sparc) || defined(sun) || defined(i386)
    gettimeofday(&stopRTime);
#elif defined( _WIN32 )
    stopRTime = timeGetTime();    
#else
    gettimeofday(&stopRTime, 0);
#endif
 //   getrusage(RUSAGE_SELF, &stopRUsage);
}

double
ggTimer::real() const {
#if defined( _WIN32 )
    return (stopRTime - startRTime)/1e3;
#else
    long ss = startRTime.tv_sec;
    long sm = startRTime.tv_usec;
    long ts = stopRTime.tv_sec;
    long tm = stopRTime.tv_usec;
    return ts - ss + (tm - sm)/1e6;
#endif
}

double
ggTimer::user() const {
#if defined( _WIN32 )
    return (stopRTime - startRTime)/1e3;
#else
    long ss = startRUsage.ru_utime.tv_sec;
    long sm = startRUsage.ru_utime.tv_usec;
    long ts = stopRUsage.ru_utime.tv_sec;
    long tm = stopRUsage.ru_utime.tv_usec;
    return ts - ss + (tm - sm)/1e6;
#endif
}

double
ggTimer::system() const {
#if defined( _WIN32 )
    return 0.0;
#else
    long ss = startRUsage.ru_stime.tv_sec;
    long sm = startRUsage.ru_stime.tv_usec;
    long ts = stopRUsage.ru_stime.tv_sec;
    long tm = stopRUsage.ru_stime.tv_usec;
    return ts - ss + (tm - sm)/1e6;
#endif
}

/*
 * Friends
 */

ostream &operator<<(ostream &os, const ggTimer &t) {
    char s[100];
    sprintf(s, "%11.1f real %11.1f user %11.1f system",
     t.real(), t.user(), t.system());
    return os << s << endl;
}



/* vi:set ts=4 sw=4: */
