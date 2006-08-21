/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Definition of the OpenSS_FPEHandler() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <pthread.h>
#include <signal.h>
//#include <bits/siginfo.h>
#include <fenv.h>



#ifdef WDH_PER_THREAD_FPEHandler

/** Thread's timer event handling function. */
static __thread OpenSS_FPEEventHandler fpe_handler = NULL;

#else

/** Thread's timer event handling function. */
static OpenSS_FPEEventHandler fpe_handler = NULL;

#endif


/**
 * Signal handler.
 *
 * Called by the operating system's signal handling system each time the running
 * thread is interrupted by an fpe. Passes the signal context to the per-thread
 * fpe event handling function.
 *
 * @param signal    Signal number.
 * @param info      Signal information.
 * @param ptr       Untyped pointer to thread context.
 *
 * @ingroup Implementation
 */
static void fpeSignalHandler(int signal, siginfo_t* info, void* ptr)
{
    /* Call this thread's fpe event handler */
    if(fpe_handler != NULL) {
        Assert(signal == SIGFPE);


	OpenSS_FPEType fpetype;

	switch (info->si_code) {

	    case FPE_FLTDIV:
				fpetype = DivideByZero;
				break;

	    case FPE_FLTOVF:
				fpetype = Overflow;
				break;

	    case FPE_FLTUND:
				fpetype = Underflow;
				break;

	    case FPE_FLTRES:
				fpetype = InexactResult;
				break;

	    case FPE_FLTINV:
				fpetype = InvalidOperation;
				break;

	    case FPE_FLTSUB:
				fpetype = SubscriptOutOfRange;
				break;

/* These should be ignored.
      FPE_INTDIV , "integer divide by zero"
      FPE_INTOVF , "integer overflow"
*/
	    case FPE_INTDIV:
	    case FPE_INTOVF:
	    default:
				fpetype = Unknown;
				break;
	}

	(*fpe_handler)(fpetype, (ucontext_t*)ptr);

    }
}


/**
 * Configure a per-thread fpe handler.
 *
 * Configure a fpe handler.
 * The specified event handler will be called when a FPE signal is delivered.
 *
 * @note    The event measured here is the specific fpe that occured.
 *
 * @param fpe_type   FPE type
 * @param handler    FPE event handler.
 *
 * @ingroup RuntimeAPI
 */
/* fpe_type must be able to set ALL the possible fpe's.
   Or we can "or" them before calling this */
void OpenSS_FPEHandler(const OpenSS_FPEType fpe_type,
		       const OpenSS_FPEEventHandler handler)
{ 
//  fprintf(stderr, "ENTERED OpenSS_FPEHandler\n");
//  fflush(stderr);

#ifdef WDH_PER_THREAD_TIMERS

    /* FIXME: How to handle fpe's for threads. */

#else

    /* eventually use fpe_type arg.  Need to convert from
    OpenSS_FPEType to type (enum) expected by feenableexcept */

/* FIXME: We really want to set the trapping of fp exceptions here.
   But the executeNow call does not seem to preserve the
   changes made by feenableexcept.
*/
   
#if 0
    switch (fpe_type) {
	case DivideByZero:		feenableexcept(FPE_FLTDIV);
	case Overflow:			feenableexcept(FPE_FLTOVF);
	case Underflow:			feenableexcept(FPE_FLTUND);
	case InexactResult:		feenableexcept(FPE_FLTRES);
	case InvalidOperation:		feenableexcept(FPE_FLTINV);
	case SubscriptOutOfRange:	feenableexcept(FPE_FLTSUB);

	case AllFPE:
	default:			
					feenableexcept(FE_ALL_EXCEPT);
    }
#endif

    /* setup the signal handler for SIGFPE */

    struct sigaction action;

    memset(&action, 0, sizeof(action));

    /* Set this signal's action to our signal handler */
    action.sa_sigaction = fpeSignalHandler;
    sigfillset(&(action.sa_mask));
    action.sa_flags = SA_SIGINFO;

    Assert(sigaction(SIGFPE, &action, NULL) == 0);
    
    fpe_handler = handler;

#endif
//  fprintf(stderr, "EXIT OpenSS_FPEHandler\n");
//  fflush(stderr);
}
