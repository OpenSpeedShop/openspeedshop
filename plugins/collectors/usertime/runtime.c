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
 * Declaration and definition of the User time collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "libunwind.h"
#include "blobs.h"

/* Forward Declarations */
void usertime_start_sampling(const char*);
void usertime_stop_sampling(const char*);


/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Number of entries in the sample buffer. */
#define BufferSize 1024

/** Thread-local storage. */
#ifdef WDH_PER_THREAD_DATA_COLLECTION
static __thread struct {
#else
static struct {
#endif

    OpenSS_DataHeader header;       /**< Header for following data blob. */
    usertime_data data;             /**< Actual data blob. */

    /** Sample buffer. */
    struct {
	uint64_t bt[BufferSize];    /**< Stack trace (PC) addresses. */
				    /**< Each stack is terminated by */
				    /**< a NULL address.             */
    } buffer;    
    
} tls;

/* utility to send samples when needed */
static void send_samples()
{
    /* Send these samples */
    tls.header.time_end = OpenSS_GetTime();
    OpenSS_Send(&(tls.header),(xdrproc_t)xdr_usertime_data,&(tls.data));

    /* Re-initialize the data blob's header */
    tls.header.time_begin = tls.header.time_end;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;

    /* Re-initialize the actual data blob */
    tls.data.bt.bt_len = 0;
}


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. 
 * Extract the PC address for each frame in the current stack trace and store
 * them into the sample buffer. Terminate each stack trace with a NULL address.
 * When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at timer interrupt.
 */
static void usertimeTimerHandler(ucontext_t* context)
{
    unw_word_t ip;	/* current stack trace pc address */

    /* Obtain the program counter current address from the thread context */
    /* and unwind the stack from there. */
    /* Libunwind provides it's own get context routines and the libunwind */
    /* documentation suggests that we use that. */
    
    unw_context_t uc;	          /* libunwind context */
    unw_cursor_t cursor;          /* libunwind stack cursor (pointer) */

    unw_getcontext (&uc);    /* get the libunwind context */
    if (unw_init_local (&cursor, &uc) < 0) {
	/* handle error if we get a negative stack pointer */
	/*panic ("unw_init_local failed!\n"); */
    }
   
    /* 
     * Loop through stack address and add to the sample buffer
     * (i.e. the current fame pc value at the stack trace cursor).
     * We incur 3 frames of overhead for the signal handler.
     * So we skip the first three frames of each stack.
     * 
     * If buffer is full, send the samples we have, and start
     * filling buffer again. Each stack trace is terminated by
     * a NULL address.
    */
    int overhead_marker = 0;  /* marker to count signal handler overhead*/
    int rval = 0;  /* return value from libunwind calls */
    do
    {
	/* get current stack address pointed to by cursor */
	unw_get_reg (&cursor, UNW_REG_IP, &ip);

	unw_word_t off;

	/* are we past the 3 frame? */
	if (overhead_marker > 2) {

	    /* add frame address to buffer */
            tls.buffer.bt[tls.data.bt.bt_len] = ip;

            /* Update the address interval in the data blob's header */
            if(ip < tls.header.addr_begin) {
	        tls.header.addr_begin = (long) ip;
	    }
            if(ip > tls.header.addr_end) {
	        tls.header.addr_end = (long) ip;
	    }
	    tls.data.bt.bt_len++;

            /* Is the sample buffer full? */
            if(tls.data.bt.bt_len == BufferSize) {
    	        /* Send these samples */
	        send_samples();
            }

	} else {
	    /* increment past signal handler overhead */
	    overhead_marker++;
	}

	/* step (unwind) the cursor to the preceding frame */
	rval = unw_step (&cursor);
	if (rval < 0) {
	    unw_get_reg (&cursor, UNW_REG_IP, &ip);
	    printf ("FAILURE: unw_step() returned %d for ip=%lx\n",
		  rval, (long) ip);
	}
    }
    while (rval > 0);

    /* terminate this stack trace with a NULL and bump bt_len*/
    tls.buffer.bt[tls.data.bt.bt_len] = 0L;
    tls.data.bt.bt_len++;

    /* Is the sample buffer full? */
    if(tls.data.bt.bt_len == BufferSize) {
	send_samples();
    }

    /* reset the signal handler overhead marker */
    overhead_marker = 0;
}

/**
 * Start sampling.
 *
 * Starts user time sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void usertime_start_sampling(const char* arguments)
{
    usertime_start_sampling_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_usertime_start_sampling_args,
			    &args);
    
    /* Initialize the data blob's header */
    tls.header.experiment = args.experiment;
    tls.header.collector = args.collector;
    tls.header.thread = args.thread;    
    tls.header.time_begin = 0;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;

    /* Initialize the actual data blob */
    tls.data.interval = (uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls.data.bt.bt_len = 0;
    tls.data.bt.bt_val = tls.buffer.bt;
    
    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls.data.interval, usertimeTimerHandler);

    /* 
     * WDH: Temporary hack to insure that usertime_stop_sampling() is called
     *      before the process exits. This insures that any samples remaining
     *      in the sampling buffer are still sent. Eventually this will be
     *      replaced with an instrumentation call to usertime_stop_sampling()
     *      instead.
     */
    atexit(usertime_stop_sampling);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void usertime_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    OpenSS_Timer(0, NULL);
    tls.header.time_end = OpenSS_GetTime();

    /* Send any samples remaining in the sample buffer */
    if(tls.data.bt.bt_len > 0)
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_usertime_data, &(tls.data));
}
