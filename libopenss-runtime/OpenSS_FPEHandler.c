/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
#include <stdlib.h>
#include <fenv.h>



/** Mutual exclusion lock for accessing shared state. */
static pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

/** Number of threads currently using FPE handlers (shared state). */
static unsigned num_threads = 0;

/** Old SIGFPE signal handler action (shared state). */
struct sigaction original_sigfpe_action;



/** Type defining the items stored in thread-local storage. */
typedef struct {

    /** FPE event handling function. */
    OpenSS_FPEEventHandler fpe_handler;

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0xBEEF0FAD;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif



/**
 * Signal handler.
 *
 * Called by the operating system's signal handling system each time the
 * running thread is interrupted by an FPE. Passes the signal context to
 * the per-thread FPE event handling function.
 *
 * @param signal    Signal number.
 * @param info      Signal information.
 * @param ptr       Untyped pointer to thread context.
 *
 * @ingroup Implementation
 */
static void signalHandler(int signal, siginfo_t* info, void* ptr)
{
//fprintf(stderr,"ENTERED FPE signalHandler with signal %d\n",signal);

    OpenSS_FPEType fpe_type = Unknown;

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Call this thread's FPE event handler */
    if(tls->fpe_handler != NULL) {
	switch (info->si_code) {
	case FPE_FLTDIV:
	    fpe_type = DivideByZero;
	    break;
	case FPE_FLTOVF:
	    fpe_type = Overflow;
	    break;
	case FPE_FLTUND:
	    fpe_type = Underflow;
	    break;
	case FPE_FLTRES:
	    fpe_type = InexactResult;
	    break;
	case FPE_FLTINV:
	    fpe_type = InvalidOperation;
	    break;
	case FPE_FLTSUB:
	    fpe_type = SubscriptOutOfRange;
	    break;
	case FPE_INTDIV:  /* Ignored: integer divide by zero */
	case FPE_INTOVF:  /* Ignored: integer overflow */
	default:
	    fpe_type = Unknown;
	    break;
	}

	(*tls->fpe_handler)(fpe_type, (ucontext_t*)ptr);
    }
}



/**
 * Configure a per-thread FPE handler.
 *
 * Configure a FPE handler to be called every time the current executing
 * thread generates the specified type of FPE. Any previously configured
 * handler is first removed. If the event handler is null, no new handler
 * is configured.
 *
 * @note    The event measured here is the specific FPE that occured.
 *
 * @param fpe_type    Type of FPE to handle.
 * @handler           FPE event handler.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_FPEHandler(OpenSS_FPEType fpe_type,
		       const OpenSS_FPEEventHandler handler)
{
//fprintf(stderr,"ENTERED FPE OpenSS_FPEHandler with fpe_type %d\n",fpe_type);
    struct sigaction action;

    /* Create and/or access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
    if(tls == NULL) {
	tls = malloc(sizeof(TLS));
	Assert(tls != NULL);
	OpenSS_SetTLS(TLSKey, tls);
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* eventually use fpe_type arg.  Need to convert from
       OpenSS_FPEType to type (enum) expected by feenableexcept */
    
    /* FIXME: We really want to set the trapping of fp exceptions here.
       But the executeNow call does not seem to preserve the
       changes made by feenableexcept.
    */
   
#if 0
    switch (fpe_type) {
    case DivideByZero: feenableexcept(FPE_FLTDIV);
    case Overflow: feenableexcept(FPE_FLTOVF);
    case Underflow: feenableexcept(FPE_FLTUND);
    case InexactResult: feenableexcept(FPE_FLTRES);
    case InvalidOperation: feenableexcept(FPE_FLTINV);
    case SubscriptOutOfRange: feenableexcept(FPE_FLTSUB);
    case AllFPE:
    default:			
	feenableexcept(FE_ALL_EXCEPT);
    }
#endif

    /* Obtain exclusive access to shared state */
    Assert(pthread_mutex_lock(&mutex_lock) == 0);

    /* Is this thread enabling its FPE handler? */
    if(handler != NULL) {
	
	/* Is this the first thread using a FPE handler? */
	if(num_threads == 0) {

	    /* Set the SIGFPE signal action to our signal handler */
	    memset(&action, 0, sizeof(action));
	    action.sa_sigaction = signalHandler;
	    sigfillset(&(action.sa_mask));
	    action.sa_flags = SA_SIGINFO;
	    Assert(sigaction(SIGFPE, &action, &original_sigfpe_action) == 0);

	}
	
	/* Is this thread using a FPE handler now were it wasn't previously? */
	if(tls->fpe_handler == NULL) {
	    
	    /* Increment the FPE handler usage thread count */
	    ++num_threads;
	    
	}

    }
    
    /* Is this thread disabling its FPE handler? */
    if(handler == NULL) {
	
	/* Decrement the FPE handler usage thread count */
	--num_threads;

	/* Was this the last thread using a FPE handler? */
	if(num_threads == 0) {
	    
	    /* Return the SIGFPE signal action to its original value */
	    Assert(sigaction(SIGFPE, &original_sigfpe_action, NULL) == 0);
	    
	}	
	
    }
    
    /** Release exclusive access to shared state */
    Assert(pthread_mutex_unlock(&mutex_lock) == 0);
    
    /* Is this thread enabling a new FPE handler? */
    if(handler != NULL) {

	/* Configure the new FPE event handler for this thread */
	tls->fpe_handler = handler;
	
    }    
    else {

	/* Remove the FPE event handler for this thread */
	tls->fpe_handler = NULL;
	
    }
}
