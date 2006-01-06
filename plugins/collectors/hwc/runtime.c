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
 * Declaration and definition of the HWC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include "PapiAPI.h"

/* Forward Declarations */
void hwc_start_sampling(const char*);
void hwc_stop_sampling(const char*);

int EventSet = PAPI_NULL;

/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Thread-local storage. */
#ifdef WDH_PER_THREAD_DATA_COLLECTION
static __thread struct {
#else
static struct {
#endif

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    hwc_data data;          /**< Actual data blob. */

    OpenSS_PCData pcdata;      /**< Raw PC data. */

} tls;

/* utility to re-initialize data items that are modified during sampling. */
static void reset_data ()
{
        /* Re-initialize the data blob's header */
        tls.header.time_begin = tls.header.time_end;
        tls.header.time_end = 0;
        tls.header.addr_begin = ~0;
        tls.header.addr_end = 0;

        /* Re-initialize the actual data blob */
        tls.data.pc.pc_len = 0;
        tls.data.count.count_len = 0;

        /* Re-initialize the hash table */
        memset(tls.pcdata.hash_table, 0, sizeof(tls.pcdata.hash_table));

        /* Re-Initialize the pcdata address range */
        tls.pcdata.addr_begin = ~0;
        tls.pcdata.addr_end = 0;
}

/* utility to send samples when buffer is full and reinitialize
 * the data. Updates final address range from pcdata struct.
 * called from handler when buffer is full and from hwc_stop_sampling
 */
static void send_samples ()
{
        /* Update the buffer lengths in the data blob's header */
        tls.data.pc.pc_len = tls.pcdata.pc_len;
        tls.data.count.count_len = tls.pcdata.count_len;

        /* no pc samples - just return. */
        if(tls.data.pc.pc_len == 0) {
            return;
        }

        /* Update the address interval in the data blob's header */
        tls.header.addr_begin = tls.pcdata.addr_begin;
        tls.header.addr_end = tls.pcdata.addr_end;

        /* Send these samples */
        tls.header.time_end = OpenSS_GetTime();
        OpenSS_Send(&(tls.header), (xdrproc_t)xdr_hwc_data, &(tls.data));

        /* Re-initialize remaining blob data. */
        reset_data();
}

/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    Multple samples at the same PC address within a given sample buffer
 *          are indicated by incrementing a sample count rather than simply
 *          repeating the PC address. This helps compress the data in the common
 *          case where a degree of spatial locality is present. A hash table is
 *          used to accelerate the determination of whether or not an addresss
 *          is already in the sample buffer. This concept is losely based on the
 *          technique employed by Digital/Compaq/HP's DCPI.
 *
 * @sa    http://h30097.www3.hp.com/dcpi/src-tn-1997-016a.html
 * 
 * @param context    Thread context at timer interrupt.
 */

void
hwcPAPIHandler(int EventSet, void *address,
					  long_long overflow_vector,
					  void* context)
{
    unsigned bucket, entry;
    uint64_t pc = (uint64_t) address;

    bool_t is_buffer_full = OpenSS_UpdatePCData(pc, &tls.pcdata);
    
    /* Is the sample buffer full? */
    if(is_buffer_full) {
	send_samples();
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
void hwc_start_sampling(const char* arguments)
{
    hwc_init_papi();

    hwc_start_sampling_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwc_start_sampling_args,
			    &args);
    
    /* Initialize the data blob's header */
    tls.header.experiment = args.experiment;
    tls.header.collector = args.collector;
    tls.header.thread = args.thread;    
    tls.data.interval = (uint64_t)(args.sampling_rate);

    /* Initialize the actual data blob */
    tls.data.pc.pc_val = tls.pcdata.buffer.pc;
    tls.data.count.count_val = tls.pcdata.buffer.count;
    
    /* Re-initialize remaining blob data. */
    reset_data();

    /* Begin sampling */
    /* This is time based rather than event based sampling */
    /* We need to set a threshhold on hwc events and sample when the */
    /* threshold is hit */
    tls.header.time_begin = OpenSS_GetTime();

    papithreshold = (uint64_t)(args.sampling_rate);

    OpenSS_Create_Eventset(&EventSet);
    OpenSS_AddEvent(EventSet,args.hwc_event);
    OpenSS_Overflow(EventSet,args.hwc_event,
			 papithreshold, hwcPAPIHandler);
    OpenSS_Start(EventSet);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void hwc_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    /* This is event based rather than timer based sampling */
    /* We need to set a threshhold on hwc events and sample when the */
    /* threshold is hit */
    OpenSS_Timer(0, NULL);

    if (EventSet == PAPI_NULL) {
	/*fprintf(stderr,"hwc_stop_sampling RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
	return;
    }

    OpenSS_Stop(EventSet);

    /* Send any samples remaining in the sample buffer */
    send_samples();
}
