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
 * Declaration and definition of the example collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"



/** Number of entries in the sample buffer. */
#define BufferSize 1024

/** Thread-local storage. */
static __thread struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    example_data data;         /**< Actual data blob. */

    /** Sample buffer. */
    struct {
        uint64_t pc[BufferSize];  /**< Program counter (PC) addresses. */
    } buffer;
    
} tls;



/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @param context    Thread context at timer interrupt.
 */
static void exampleTimerHandler(const ucontext_t* context)
{
    uint64_t pc;

    /* Obtain the program counter (PC) address from the thread context */
    pc = OpenSS_GetPCFromContext(context);

    /* Add a new entry for this PC address to the sample buffer */
    tls.buffer.pc[tls.data.pc.pc_len++] = pc;

    /* Update the address interval in the data blob's header */
    if(pc < tls.header.addr_begin)
	tls.header.addr_begin = pc;
    if(pc > tls.header.addr_end)
	tls.header.addr_end = pc;

    /* Is the sample buffer full? */
    if(tls.data.pc.pc_len == BufferSize) {
	
	/* Send these samples */
	tls.header.time_end = OpenSS_GetTime();
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_example_data, &(tls.data));

	/* Re-initialize the data blob's header */
	tls.header.time_begin = tls.header.time_end;
	tls.header.time_end = 0;
	tls.header.addr_begin = ~0;
	tls.header.addr_end = 0;
	
	/* Re-initialize the actual data blob */
	tls.data.pc.pc_len = 0;

    }    
}



/**
 * Start sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void example_start_sampling(const char* arguments)
{
    example_start_sampling_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_example_start_sampling_args,
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
    tls.data.pc.pc_len = 0;
    tls.data.pc.pc_val = tls.buffer.pc;
    
    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls.data.interval, exampleTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void example_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    OpenSS_Timer(0, NULL);
    tls.header.time_end = OpenSS_GetTime();
    
    /* Send any samples remaining in the sample buffer */
    if(tls.data.pc.pc_len > 0)
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_example_data, &(tls.data));
}
