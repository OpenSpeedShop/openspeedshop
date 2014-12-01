/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2011 Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the PC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"

/** Type defining the items stored in thread-local storage. */
typedef struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    pcsamp_data data;          /**< Actual data blob. */

    OpenSS_PCData buffer;      /**< PC sampling data buffer. */

    bool_t defer_sampling;

} TLS;


#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


#if defined (OPENSS_OFFLINE)
extern void offline_sent_data(int);
static void pcsampTimerHandler(const ucontext_t* context);

void pcsamp_start_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->defer_sampling=FALSE;
}

void pcsamp_stop_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
    tls->defer_sampling=TRUE;
}
#endif


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
static void pcsampTimerHandler(const ucontext_t* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if(tls->defer_sampling == TRUE) {
	return;
    }

    
    /* Obtain the program counter (PC) address from the thread context */
    uint64_t pc = OpenSS_GetPCFromContext(context);

    /* Update the sampling buffer and check if it has been filled */
    if(OpenSS_UpdatePCData(pc, &tls->buffer)) {

	/* Send these samples */
	tls->header.time_end = OpenSS_GetTime();
	tls->header.addr_begin = tls->buffer.addr_begin;
	tls->header.addr_end = tls->buffer.addr_end;
	tls->data.pc.pc_len = tls->buffer.length;
	tls->data.count.count_len = tls->buffer.length;


#ifndef NDEBUG
        if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"pcsampTimerHandler sends data:\n");
            fprintf(stderr,"time_end(%lld) addr range [%#llx, %#llx] pc_len(%d) count_len(%d)\n",
                tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
                tls->data.count.count_len);
        }
#endif

	OpenSS_Send(&tls->header, (xdrproc_t)xdr_pcsamp_data, &tls->data);

#if defined(OPENSS_OFFLINE)
	offline_sent_data(1);
#endif

	/* Re-initialize the data blob's header */
	tls->header.time_begin = tls->header.time_end;

	/* Re-initialize the sampling buffer */
	tls->buffer.addr_begin = ~0;
	tls->buffer.addr_end = 0;
	tls->buffer.length = 0;
	memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));

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
void pcsamp_start_sampling(const char* arguments)
{
    pcsamp_start_sampling_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->defer_sampling=FALSE;

    /* Decode the passed function arguments */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_pcsamp_start_sampling_args,
			    &args);
    
    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to OpenSS_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    
    OpenSS_DataHeader local_data_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector,
				&local_data_header);
    memcpy(&tls->header, &local_data_header, sizeof(OpenSS_DataHeader));
    OpenSS_SetSendToFile(&(tls->header), "pcsamp", "openss-data");
    
    /* Initialize the actual data blob */
    tls->data.interval = 
	(uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls->data.pc.pc_val = tls->buffer.pc;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));

    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls->data.interval, pcsampTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void pcsamp_stop_sampling(const char* arguments)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    /* Stop sampling */
    OpenSS_Timer(0, NULL);

    tls->header.time_end = OpenSS_GetTime();

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {

	/* Send these samples */
	tls->header.addr_begin = tls->buffer.addr_begin;
	tls->header.addr_end = tls->buffer.addr_end;
	tls->data.pc.pc_len = tls->buffer.length;
	tls->data.count.count_len = tls->buffer.length;

#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "pcsamp_stop_sampling:\n");
	    fprintf(stderr, "time_end(%lld) addr range[%#llx, %#llx] pc_len(%d) count_len(%d)\n",
		tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
		tls->data.count.count_len);
	}
#endif

	OpenSS_Send(&(tls->header), (xdrproc_t)xdr_pcsamp_data, &(tls->data));

#if defined(OPENSS_OFFLINE)
	offline_sent_data(1);
#endif
	
    }

    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
}
