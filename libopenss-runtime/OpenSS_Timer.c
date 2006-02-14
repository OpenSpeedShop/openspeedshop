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



#ifdef WDH_PER_THREAD_TIMERS

/** Access-controlled timer signal number. */
static struct {
    int num;               /**< Actual signal number value. */
    pthread_mutex_t lock;  /**< Mutual exclusion lock for this number. */
} timer_signal = {
    -1, PTHREAD_MUTEX_INITIALIZER
};

/** Thread's timer event handling function. */
static __thread OpenSS_TimerEventHandler timer_handler = NULL;

/** Thread's timer id. */
static __thread timer_t timer_id;

#else

/** Thread's timer event handling function. */
static OpenSS_TimerEventHandler timer_handler = NULL;

#endif



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
void OpenSS_Timer(uint64_t interval, OpenSS_TimerEventHandler handler)
{
#ifdef WDH_PER_THREAD_TIMERS

    int i;
    struct sigaction action;
    sigevent_t delivery;
    struct itimerspec spec;
    
    /* Obtain exclusive access to the timer signal number */
    Assert(pthread_mutex_lock(&(timer_signal.lock)) == 0);

    /* Has a timer signal number been selected? */
    if(timer_signal.num == -1) {
	
	/* Iterate over each of the POSIX real-time signals */
	for(i = SIGRTMIN; i <= SIGRTMAX; ++i) {
	    
	    /* Get this signal's current action */
	    Assert(sigaction(i, NULL, &action) == 0);
	    
	    /* Is this signal's action the default? */
	    if(action.sa_handler == SIG_DFL) {
		
		/* Set this signal's action to our signal handler */
		action.sa_sigaction = signalHandler;
		sigfillset(&(action.sa_mask));
		action.sa_flags = SA_SIGINFO;		
		Assert(sigaction(i, &action, NULL) == 0);
		
		/* Indicate the timer signal number has now been selected */
		timer_signal.num = i;
		break;
		
	    }
	    
	}

	/* Timer signal number must now be selected */
	Assert(timer_signal.num != -1);
	
    }

    /** Release exclusive access to the timer signal number */
    Assert(pthread_mutex_unlock(&(timer_signal.lock)) == 0);
    
    /* Delete any existing timer for this thread */
    if(timer_handler != NULL)	
	Assert(timer_delete(timer_id) == 0);

    /* Configure the new timer event handler for this thread */
    timer_handler = handler;
    
    /*
     * Note: Ideally "sigev_notify" below should have the value SIGEV_THREAD_ID
     *       and the clock used would be CLOCK_THREAD_CPUTIME_ID. Using either
     *       of these with timer_create() on Linux/IA32 results in an "Invalid
     *       argument" error.
     */

    /* Create a new timer for this thread */
    delivery.sigev_signo = timer_signal.num;
    delivery.sigev_notify = SIGEV_SIGNAL;
    Assert(timer_create(CLOCK_MONOTONIC, &delivery, &timer_id) == 0);
    
    /* Configure specified interval period */
    spec.it_interval.tv_sec = interval / (uint64_t)(1000000000);
    spec.it_interval.tv_nsec = interval % (uint64_t)(1000000000);
    spec.it_value.tv_sec = spec.it_interval.tv_sec;
    spec.it_value.tv_nsec = spec.it_interval.tv_nsec;
    Assert(timer_settime(timer_id, 0, &spec, NULL) == 0);

#else

    struct sigaction action;
    struct itimerval spec;

    /* Set this signal's action to our signal handler */
    action.sa_sigaction = signalHandler;
    sigfillset(&(action.sa_mask));
    action.sa_flags = SA_SIGINFO;		
    Assert(sigaction(SIGPROF, &action, NULL) == 0);
    
    /* Configure the new timer event handler for this thread */
    timer_handler = handler;
    
    /* Create an interval timer for this thread */
    spec.it_interval.tv_sec = interval / (uint64_t)(1000000000);
    spec.it_interval.tv_usec = 
	(interval % (uint64_t)(1000000000)) / (uint64_t)(1000);
    spec.it_value.tv_sec = spec.it_interval.tv_sec;
    spec.it_value.tv_usec = spec.it_interval.tv_usec;
    Assert(setitimer(ITIMER_PROF, &spec, NULL) == 0); 

#endif
}
