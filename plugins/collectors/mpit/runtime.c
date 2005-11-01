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
 * Definition of the MPI extended event tracing collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "runtime.h"



/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Number of stack trace entries in the tracing buffer. */
#define StackTraceBufferSize 1024

/** Number of event entries in the tracing buffer. */
#define EventBufferSize 256

/** Thread-local storage. */
#ifdef WDH_PER_THREAD_DATA_COLLECTION
static __thread struct {
#else
static struct {
#endif

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    mpit_data data;            /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	mpit_event events[EventBufferSize];          /**< MPI call events. */
    } buffer;    

} tls;



/**
 * ...
 *
 * ...
 *
 * @param event    ...
 */
void mpit_record_event(const mpit_event* event)
{
    ucontext_t context;

    /* Obtain the current thread context */
    OpenSS_GetContext(&context);
    
    /* Obtain the stack trace from this context */
    /* ... */

    /* ... */

}



/**
 * Start tracing.
 *
 * Starts MPI extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void mpit_start_tracing(const char* arguments)
{
    mpit_start_tracing_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_mpit_start_tracing_args,
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
    tls.data.stacktraces.stacktraces_len = 0;
    tls.data.stacktraces.stacktraces_val = tls.buffer.stacktraces;
    tls.data.events.events_len = 0;
    tls.data.events.events_val = tls.buffer.events;

    /* Begin tracing */
    tls.header.time_begin = OpenSS_GetTime();
}



/**
 * Stop tracing.
 *
 * Stops MPI extended event tracing for the thread executing this function.
 * Sends any traces remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void mpit_stop_sampling(const char* arguments)
{
    /* Stop tracing */
    tls.header.time_end = OpenSS_GetTime();

    /* Send any traces remaining in the trace buffer */
    if(tls.data.events.events_len > 0)
        OpenSS_Send(&(tls.header), (xdrproc_t)xdr_mpit_data, &(tls.data));
}
