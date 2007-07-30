/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration and definition of the HWC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include "PapiAPI.h"



/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Thread-local storage. */
static __thread struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    hwc_data data;             /**< Actual data blob. */

    OpenSS_PCData buffer;      /**< PC sampling data buffer. */

} tls;

/** PAPI event set. */
static int event_set = PAPI_NULL;



/**
 * PAPI event handler.
 *
 * Called by PAPI each time a sample is to be taken. Takes the program counter
 * (PC) address passed by PAPI and places it into the sample buffer. When the
 * sample buffer is full, it is sent to the framework for storage in the
 * experiment's database.
 *
 * @param context    Thread context at timer interrupt.
 */
static void hwcPAPIHandler(int event_set, void* pc, 
			   long_long overflow_vector, void* context)
{
    /* Update the sampling buffer and check if it has been filled */
    if(OpenSS_UpdatePCData((uint64_t)pc, &tls.buffer)) {
	
	/* Send these samples */
	tls.header.time_end = OpenSS_GetTime();	
	tls.header.addr_begin = tls.buffer.addr_begin;
	tls.header.addr_end = tls.buffer.addr_end;
	tls.data.pc.pc_len = tls.buffer.length;
	tls.data.count.count_len = tls.buffer.length;
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_hwc_data, &(tls.data));

	/* Re-initialize the data blob's header */
	tls.header.time_begin = tls.header.time_end;

	/* Re-initialize the sampling buffer */
	tls.buffer.addr_begin = ~0;
	tls.buffer.addr_end = 0;
	tls.buffer.length = 0;
	memset(tls.buffer.hash_table, 0, sizeof(tls.buffer.hash_table));
	
    }
}



/**
 * Start sampling.
 *
 * Starts hardware counter (HWC) sampling for the thread executing this
 * function. Initializes the appropriate thread-local data structures and
 * then enables the sampling counter.
 *
 * @param arguments    Encoded function arguments.
 */
void hwc_start_sampling(const char* arguments)
{
    hwc_start_sampling_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwc_start_sampling_args,
			    &args);
    

    /* Initialize the data blob's header */
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(tls.header));
    
    /* Initialize the actual data blob */
    tls.data.interval = (uint64_t)(args.sampling_rate);
    tls.data.pc.pc_val = tls.buffer.pc;
    tls.data.count.count_val = tls.buffer.count;
   
    /* Initialize the sampling buffer */
    tls.buffer.addr_begin = ~0;
    tls.buffer.addr_end = 0;
    tls.buffer.length = 0;
    memset(tls.buffer.hash_table, 0, sizeof(tls.buffer.hash_table));

    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    if(event_set == PAPI_NULL)
	hwc_init_papi();
    OpenSS_Create_Eventset(&event_set);
    OpenSS_AddEvent(event_set, args.hwc_event);
    OpenSS_Overflow(event_set, args.hwc_event, tls.data.interval,
		    hwcPAPIHandler);
    OpenSS_Start(event_set);
}



/**
 * Stop sampling.
 *
 * Stops hardware counter (HWC) sampling for the thread executing this function.
 * Disables the sampling counter and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void hwc_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    OpenSS_Stop(event_set);
    tls.header.time_end = OpenSS_GetTime();

    /* Are there any unsent samples? */
    if(tls.buffer.length > 0) {
	
	/* Send these samples */
	tls.header.addr_begin = tls.buffer.addr_begin;
	tls.header.addr_end = tls.buffer.addr_end;
	tls.data.pc.pc_len = tls.buffer.length;
	tls.data.count.count_len = tls.buffer.length;
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_hwc_data, &(tls.data));
	
    }
}
