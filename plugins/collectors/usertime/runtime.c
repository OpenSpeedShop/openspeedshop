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
#include "LibunwindAPI.h"
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

/** possible number of signal handler overhead frames */
#define FRAMEOVERHEAD 4

/** Man number of frames for callstack collection */
#define MAXFRAMES 100

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
	uint8_t  count[BufferSize]; /**< count value greater than 0 is top */
				    /**< of stack. A count of 255 indicates */
				    /**< another instance of this stack may */
				    /**< exist in buffer bt. */
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
    tls.data.count.count_len = 0;
}

bool_t cmp_samplebuffer(int framecount, int *stackindex, uint64_t *framebuf)
{
    int i,j;
    bool_t found = FALSE;
    /* search individual stacks via count/indexing array */
    for (i = 0; i < tls.data.count.count_len ; i++ )
    {
	/* a count > 0 indexes the top of stack in the data buffer. */
	/* a count == 255 indicates this stack is at the count limit. */
	if (tls.buffer.count[i] == 0) {
	    continue;
	}
	if (tls.buffer.count[i] == 255) {
	    found = FALSE;
	    continue;
	}

	/* see if the stack addresses match */
	for (j = 0; j < framecount ; j++ )
	{
	    if ( tls.buffer.bt[i+j] != framebuf[j] ) {
		   found = FALSE;
		   break;
	    }
	}

	if ( j == framecount) {
	    found = TRUE;
	    *stackindex = i;
	    return found;
	}
    }
    return found;
}

static void update_samplebuffer(int framecount, uint64_t *framebuf)
{
	/* add frames to sample buffer, compute addresss range */
	int i;
	for (i = 0; i < framecount ; i++)
	{
	    /* always add address to buffer bt */
	    tls.buffer.bt[tls.data.bt.bt_len] = framebuf[i];

	    /* top of stack indicated by a positive count. */
	    /* all other elements are 0 */
	    if (i > 0 ) {
		tls.buffer.count[tls.data.count.count_len] = 0;
	    } else {
		tls.buffer.count[tls.data.count.count_len] = 1;
	    }

	    if (framebuf[i] < tls.header.addr_begin ) {
		tls.header.addr_begin = framebuf[i];
	    }
	    if (framebuf[i] > tls.header.addr_end ) {
		tls.header.addr_end = framebuf[i];
	    }
	    tls.data.bt.bt_len++;
	    tls.data.count.count_len++;
	}
}

/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. 
 * Extract the PC address for each frame in the current stack trace and store
 * them into the sample buffer. For each address that represents the
 * top of a unique stack update it's count in the count buffer.
 * If a stack count reaches 255 in the count buffer, start a new stack
 * entry in the sample buffer.
 * When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at timer interrupt.
 */
/*
 * void OpenSS_GetStackTraceFromContext (
 * 			const ucontext_t* context,
 *                        int overhead,
 *                        int maxframes,
 *                        int* framecount,
 *                        uint64_t* framebuf)
 *
 * Call OpenSS_GetStackTraceFromContext for the given context.
 * OpenSS_GetStackTraceFromContext will return a buffer of maxframes
 * stack addresses.  Each stack trace buffer is tested to see if it fits
 * in the remaining sample buffer (via framecount).
 * The beginaddr and endaddr of each returned stack trace is tested
 * against the data in the current blob and updated as needed.
*/

static void usertimeTimerHandler(const ucontext_t* context)
{
    bool_t stack_already_exists = FALSE;
    int framecount = 0;
    int stackindex = 0;
    uint64_t framebuf[MAXFRAMES];
    uint64_t beginaddr = tls.header.addr_begin;
    uint64_t endaddr = tls.header.addr_end;

    /* get stack address for current context and store them into framebuf
     * with a terminating 0.
     * The number of overhead frames for usertime is 3.
    */

    OpenSS_GetStackTraceFromContext (context, FRAMEOVERHEAD /* overhead */,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;

    stack_already_exists = cmp_samplebuffer(framecount, &stackindex, framebuf);


    /* if the stack already exisits in the buffer, update its count
     * and return. If the stack is already at the count limit, cmp_samplebuffer
     * will return false so a new stack entry will be created even though
     * the stack may already exist in the buffer.
     * Otherwise, add the new stack to the buffer
    */
    if (stack_already_exists && tls.buffer.count[stackindex] < 255 ) {
	/* update count for this stack */
	tls.buffer.count[stackindex] = tls.buffer.count[stackindex] + 1;
	return;
    }

    /* Will this stack trace fit into the sample buffer? */
    /* update_samplebuffer adds the 0 stack termination marker and */
    /* updates tls.data.bt.bt_len. */

    int buflen = tls.data.bt.bt_len + framecount;
    if ( buflen <= BufferSize) {
	update_samplebuffer(framecount,framebuf);
    } else {
        /* sample buffer has no room for these stack frames.*/
	/* send the current sample buffer. (will init a new buffer) */
        send_samples();

	update_samplebuffer(framecount,framebuf);
    }
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

    memset(tls.buffer.bt, 0, sizeof(tls.buffer.bt));
    memset(tls.buffer.count, 0, sizeof(tls.buffer.count));

    /* Initialize the actual data blob */
    tls.data.interval = (uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls.data.bt.bt_len = 0;
    tls.data.bt.bt_val = tls.buffer.bt;
    tls.data.count.count_len = 0;
    tls.data.count.count_val = tls.buffer.count;
    
    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls.data.interval, usertimeTimerHandler);
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
    if(tls.data.bt.bt_len > 0) {

#if 0  /* debug only - shows stacks and counts*/
	int i;
	for (i=0; i < tls.data.bt.bt_len; i++ )
	{
fprintf(stderr,"usertime_stop_sampling: tls.buffer.bt.[%d]=%#lx, count=%d\n",i,tls.buffer.bt[i], tls.buffer.count[i]);
	}
	for (i=0; i < tls.data.count.count_len; i++ )
	{
fprintf(stderr,"usertime_stop_sampling: tls.buffer.count[%d]=%d\n",i,tls.buffer.count[i]);
	}
#endif

	send_samples();
    }
}
