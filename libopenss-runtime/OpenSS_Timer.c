/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the OpenSS_Timer() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>



/** Mutual exclusion lock for accessing shared state. */
static pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

/** Number of threads currently using timers (shared state). */
static unsigned num_threads = 0;

/** Old SIGPROF signal handler action (shared state). */
struct sigaction original_sigprof_action;



/** Type defining the items stored in thread-local storage. */
typedef struct {

    /** Timer event handling function. */
    OpenSS_TimerEventHandler timer_handler;

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0xBAD0BEEF;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif



/**
 * Signal handler.
 *
 * Called by the operating system's signal handling system each time the
 * running thread is interrupted by our timer. Passes the signal context
 * to the per-thread timer event handling function.
 *
 * @param signal    Signal number.
 * @param info      Signal information.
 * @param ptr       Untyped pointer to thread context.
 *
 * @ingroup Implementation
 */
static void signalHandler(int signal, siginfo_t* info, void* ptr)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

/* FIXME:NOTE
 * With explicit TLS and dyninst instrumentation this signalhandler
 * is being called in new pthreads as they are created BUT before
 * the start_samping call in the collector runtime has been called.
 * It is the start_sampling functions with allocate the timer tls.
 * Therefore the assert was causing premature aborts for online.
 * Once the start_sampling function is called then we will get the
 * tls for this thread and can continue to set up this threads timer.
 * This does not miss any samples AFAICT since dyninst has not yet
 * called our start_sampling functions via executeNow in the collector
 * runtimes.  Tested with test/executables/threads.
 */
    if (tls == NULL) {
	/* we have not yet officially started the timer from
	 * the collector runtime. Just return.
	 */
	return;
    }

    /* Call this thread's timer event handler */
    if(tls->timer_handler != NULL)
	(*tls->timer_handler)((ucontext_t*)ptr);
}



/**
 * Configure a per-thread timer.
 *
 * Configure a timer to interrupt the currently executing thread at a specified
 * interval. The specified event handler will be called at each interrupt. Any
 * previously configured timer is first removed. If the specified interval is
 * zero and/or the event handler is null, no new timer is configured.
 *
 * @note    The time measured here is CPU seconds spent executing the thread.
 *
 * @param interval   Timer interval (in nanoseconds).
 * @param handler    Timer event handler.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_Timer(uint64_t interval, const OpenSS_TimerEventHandler handler)
{
    struct sigaction action;
    struct itimerval spec;

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

    /* Disable the timer for this thread */
    memset(&spec, 0, sizeof(spec));
    Assert(setitimer(ITIMER_PROF, &spec, NULL) == 0);

    /* Obtain exclusive access to shared state */
    Assert(pthread_mutex_lock(&mutex_lock) == 0);

    /* Is this thread enabling its timer? */
    if((interval > 0) && (handler != NULL)) {

	/* Is this the first thread using a timer? */
	if(num_threads == 0) {

	    /* Set the SIGPROF signal action to our signal handler */
	    memset(&action, 0, sizeof(action));
	    action.sa_sigaction = signalHandler;
	    sigfillset(&(action.sa_mask));
	    action.sa_flags = SA_SIGINFO;
	    Assert(sigaction(SIGPROF, &action, &original_sigprof_action) == 0);

	}
	
	/* Is this thread using a timer now were it wasn't previously? */
	if(tls->timer_handler == NULL) {
	    
	    /* Increment the timer usage thread count */
	    ++num_threads;
	    
	}

    }
    
    /* Is this thread disabling its timer? */
    if((interval == 0) || (handler == NULL)) {
	
	/* Decrement the timer usage thread count */
	--num_threads;

	/* Was this the last thread using the timer? */
	if(num_threads == 0) {
	    
	    /* Return the SIGPROF signal action to its original value */
	    Assert(sigaction(SIGPROF, &original_sigprof_action, NULL) == 0);
	    
	}	
	
    }
    
    /** Release exclusive access to shared state */
    Assert(pthread_mutex_unlock(&mutex_lock) == 0);
    
    /* Is this thread enabling a new timer? */
    if((interval > 0) && (handler != NULL)) {

	/* Configure the new timer event handler for this thread */
	tls->timer_handler = handler;
	
	/* Enable a timer for this thread */
	spec.it_interval.tv_sec = interval / (uint64_t)(1000000000);
	spec.it_interval.tv_usec =
	    (interval % (uint64_t)(1000000000)) / (uint64_t)(1000);
	spec.it_value.tv_sec = spec.it_interval.tv_sec;
	spec.it_value.tv_usec = spec.it_interval.tv_usec;
	Assert(setitimer(ITIMER_PROF, &spec, NULL) == 0);
	
    }    
    else {

	/* Remove the timer event handler for this thread */
	tls->timer_handler = NULL;
	
    }
}
