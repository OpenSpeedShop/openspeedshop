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



/* $Id$ */



#ifndef GGTIMER_H
#define GGTIMER_H


#if defined( _WIN32 )
#  include <windows.h>
#  include <mmsystem.h>
#else
#  include <sys/time.h>
#  include <sys/resource.h>
#endif

#if defined(sparc) || defined(sun) || defined(i386) || defined(os390)
#if !defined(RUSAGE_SELF)
#define RUSAGE_SELF     0
#define RUSAGE_CHILDREN -1
#ifdef os390
typedef timer_t timeval;
#endif 
struct  rusage {
        timeval ru_utime;           /* user time used */
        timeval ru_stime;           /* system time used */
        long    ru_maxrss;
#define ru_first        ru_ixrss
        long    ru_ixrss;               /* XXX: 0 */
        long    ru_idrss;               /* XXX: sum of rm_asrss */
        long    ru_isrss;               /* XXX: 0 */
        long    ru_minflt;              /* any page faults not requiring I/O */
        long    ru_majflt;              /* any page faults requiring I/O */
        long    ru_nswap;               /* swaps */
        long    ru_inblock;             /* block input operations */
        long    ru_oublock;             /* block output operations */
        long    ru_msgsnd;              /* messages sent */
        long    ru_msgrcv;              /* messages received */
        long    ru_nsignals;            /* signals received */
        long    ru_nvcsw;               /* voluntary context switches */
        long    ru_nivcsw;              /* involuntary " */
#define ru_last         ru_nivcsw
};
#endif
#endif


/******************************************************************************
* ggTimer
*/

class ggTimer {

    friend ostream &operator<<(ostream &os, const ggTimer &t);

public:

    ggTimer();

    void start();
    void stop();

    double real() const;
    double user() const;
    double system() const;

protected:

#if defined( _WIN32 )
    long startRTime;
    long stopRTime;
#else
    struct rusage startRUsage;
    struct rusage stopRUsage;
#endif
#if !defined(os390) && !defined(_WIN32 ) 
    struct timeval startRTime;
    struct timeval stopRTime;
#endif
};



#endif



/* vi:set ts=4 sw=4: */
