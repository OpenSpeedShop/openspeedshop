/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007,2008,2009 Krell Institute.  All Rights Reserved.
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

/** Type defining the items stored in thread-local storage. */
typedef struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    hwc_data data;             /**< Actual data blob. */

    OpenSS_PCData buffer;      /**< PC sampling data buffer. */

    int EventSet;
} TLS;

static int hwc_papi_init_done = 0;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF5;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


#if defined (OPENSS_OFFLINE)
extern void offline_sent_data(int);

void hwc_resume_papi()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;
    OpenSS_Start(tls->EventSet);
}

void hwc_suspend_papi()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;
    OpenSS_Stop(tls->EventSet, NULL);
}
#endif

/* utility to send samples when needed */
static void send_samples(TLS *tls)
{
    /* Send these samples */
    tls->header.time_end = OpenSS_GetTime();
    tls->header.addr_begin = tls->buffer.addr_begin;
    tls->header.addr_end = tls->buffer.addr_end;
    tls->data.pc.pc_len = tls->buffer.length;
    tls->data.count.count_len = tls->buffer.length;


#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"HWC send_samples DATA:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,
	    tls->header.addr_end,tls->data.pc.pc_len);
    }
#endif

    OpenSS_Send(&(tls->header),(xdrproc_t)xdr_hwc_data,&(tls->data));

#if defined(OPENSS_OFFLINE)
    offline_sent_data(1);
#endif

    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;


    /* Re-initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));
}

/**
 * PAPI event handler.
 *
 * Called by PAPI each time a sample is to be taken. Takes the program counter
 * (PC) address passed by PAPI and places it into the sample buffer. When the
 * sample buffer is full, it is sent to the framework for storage in the
 * experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at papi overflow.
 */
static void
hwcPAPIHandler(int EventSet, void* pc, long_long overflow_vector, void* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Update the sampling buffer and check if it has been filled */
    if(OpenSS_UpdatePCData((uint64_t)pc, &tls->buffer)) {

#ifndef NDEBUG
        if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"hwcPAPIHandler sends data buffer length is %d\n",tls->buffer.length);
        }
#endif

	/* Send these samples */
	send_samples(tls);

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
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    hwc_start_sampling_args args;
    memset(&args, 0, sizeof(args));

    /* set defaults */ 
    int hwc_papithreshold = THRESHOLD*2;
    char* hwc_papi_event = "PAPI_TOT_CYC";

    /* Decode the passed function arguments */
#if defined (OPENSS_OFFLINE)
    char* hwc_event_param = getenv("OPENSS_HWC_EVENT");
    if (hwc_event_param != NULL) {
        hwc_papi_event=hwc_event_param;
    }

    const char* sampling_rate = getenv("OPENSS_HWC_THRESHOLD");
    if (sampling_rate != NULL) {
        hwc_papithreshold=atoi(sampling_rate);
    }
    args.collector = 1;
    args.experiment = 0;
    tls->data.interval = hwc_papithreshold;
#else
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwc_start_sampling_args,
			    &args);
    hwc_papithreshold = (uint64_t)(args.sampling_rate);
    hwc_papi_event = args.hwc_event;
    tls->data.interval = (uint64_t)(args.sampling_rate);
#endif

    
    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to OpenSS_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_header));
    memcpy(&tls->header, &local_header, sizeof(OpenSS_DataHeader));
    OpenSS_SetSendToFile(&(tls->header), "hwc","openss-data");

    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Initialize the actual data blob */
    tls->data.pc.pc_val = tls->buffer.pc;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));

    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();

    if(hwc_papi_init_done == 0) {
	hwc_init_papi();
	tls->EventSet = PAPI_NULL;
	hwc_papi_init_done = 1;
    }

    unsigned papi_event_code = get_papi_eventcode(hwc_papi_event);

    OpenSS_Create_Eventset(&tls->EventSet);
    OpenSS_AddEvent(tls->EventSet, papi_event_code);
    OpenSS_Overflow(tls->EventSet, papi_event_code,
		    hwc_papithreshold, hwcPAPIHandler);
    OpenSS_Start(tls->EventSet);
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    if (tls->EventSet == PAPI_NULL) {
	/*fprintf(stderr,"hwc_stop_sampling RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
        return;
    }

    /* Stop sampling */
    OpenSS_Stop(tls->EventSet, NULL);

    tls->header.time_end = OpenSS_GetTime();

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {

#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "hwc_stop_sampling calls send_samples.\n");
	}
#endif

	/* Send these samples */
	send_samples(tls);

    }

    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
}
