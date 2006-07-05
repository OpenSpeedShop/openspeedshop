/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
#include <time.h>



/** Mutual exclusion lock for accessing shared timer state. */
static pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

/** Number of threads currently using timers (shared state). */
static unsigned num_threads = 0;

/** Old SIGPROF signal handler action (shared state). */
struct sigaction original_sigprof_action;

/** Timer event handling function (per-thread). */
static __thread OpenSS_TimerEventHandler timer_handler = NULL;



/**
 * Signal handler.
 *
 * Called by the operating system's signal handling system each time the running
 * thread is interrupt by our timer. Passes the signal context to the per-thread
 * timer event handling function.
 *
 * @param signal    Signal number.
 * @param info      Signal information.
 * @param ptr       Untyped pointer to thread context.
 *
 * @ingroup Implementation
 */
static void signalHandler(int signal, siginfo_t* info, void* ptr)
{
    /* Call this thread's timer event handler */
    if(timer_handler != NULL)
	(*timer_handler)((ucontext_t*)ptr);
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

    /* Disable the timer for this thread */
    memset(&spec, 0, sizeof(spec));
    Assert(setitimer(ITIMER_PROF, &spec, NULL) == 0);

    /* Obtain exclusive access to shared timer state */
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
	if(timer_handler == NULL) {
	    
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
    
    /** Release exclusive access to shared timer state */
    Assert(pthread_mutex_unlock(&mutex_lock) == 0);
    
    /* Is this thread enabling a new timer? */
    if((interval > 0) && (handler != NULL)) {

	/* Configure the new timer event handler for this thread */
	timer_handler = handler;
	
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
	timer_handler = NULL;
	
    }
}
