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
 * Declaration and definition of the Hardware time collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include "PapiAPI.h"
#include "UnwindAPI.h"

#include <libunwind.h>

#if UNW_TARGET_X86 || UNW_TARGET_X86_64
# define STACK_SIZE     (128*1024)      /* On x86, SIGSTKSZ is too small */
#else
# define STACK_SIZE     SIGSTKSZ
#endif

/** Number of entries in the sample buffer. */
#define BufferSize 1024

/** Man number of frames for callstack collection */
#define MAXFRAMES 100

/** Type defining the items stored in thread-local storage. */
typedef struct {

    OpenSS_DataHeader header;       /**< Header for following data blob. */
    hwctime_data data;             /**< Actual data blob. */

    /** Sample buffer. */
    struct {
	uint64_t bt[BufferSize];    /**< Stack trace (PC) addresses. */
	uint8_t  count[BufferSize]; /**< count value greater than 0 is top */
				    /**< of stack. A count of 255 indicates */
				    /**< another instance of this stack may */
				    /**< exist in buffer bt. */
    } buffer;    

    int EventSet;
} TLS;

static int hwctime_papi_init_done = 0;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF6;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


#if defined (OPENSS_OFFLINE)
extern void offline_sent_data(int);

void hwctime_resume_papi()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwctime_papi_init_done == 0 || tls == NULL)
	return;
    OpenSS_Start(tls->EventSet);
}

void hwctime_suspend_papi()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwctime_papi_init_done == 0 || tls == NULL)
	return;
    OpenSS_Stop(tls->EventSet, NULL);
}
#endif

/* utility to send samples when needed */
static void send_samples(TLS *tls)
{
    /* Send these samples */
    tls->header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"hwctime send_samples: sends data:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] bt_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,
	    tls->header.addr_end,tls->data.bt.bt_len);
    }
#endif

    OpenSS_Send(&(tls->header),(xdrproc_t)xdr_hwctime_data,&(tls->data));

#if defined(OPENSS_OFFLINE)
    offline_sent_data(1);
#endif

    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Re-initialize the actual data blob */
    tls->data.bt.bt_len = 0;
    tls->data.count.count_len = 0;

    /* Re-initialize the sampling buffer */
    memset(tls->buffer.bt, 0, sizeof(tls->buffer.bt));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
}

static int total = 0;
static int stacktotal = 0;

/**
 * PAPI event handler.
 *
 * Called by PAPI_overflow each time a sample is to be taken. 
 * Extract the PC address for each frame in the current stack trace and store
 * them into the sample buffer. Terminate each stack trace with a NULL address.
 * When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at papi overflow.
 */
static void
hwctimePAPIHandler(int EventSet, void *address, long_long overflow_vector, void* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    int framecount = 0;
    int stackindex = 0;
    uint64_t framebuf[MAXFRAMES];
    uint64_t beginaddr = tls->header.addr_begin;
    uint64_t endaddr = tls->header.addr_end;

    memset(framebuf,0, sizeof(framebuf));

    /* get stack address for current context and store them into framebuf. */

#if defined(__linux) && defined(__x86_64)
    /* The latest version of libunwind provides a fast trace
     * backtrace function we now use. We need to manually
     * skip signal frames when using that unwind function.
     * For PAPI's handler we need to skip 6 frames of
     * overhead.
     */

#if defined(USE_FASTTRACE)
    OpenSS_GetStackTrace(FALSE, 6,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#else
    OpenSS_GetStackTraceFromContext (context, TRUE, 0,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif

#else
    OpenSS_GetStackTraceFromContext (context, TRUE, 0,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif

    bool_t stack_already_exists = FALSE;

    int i, j;
    /* search individual stacks via count/indexing array */
    for (i = 0; i < tls->data.count.count_len ; i++ )
    {
	/* a count > 0 indexes the top of stack in the data buffer. */
	/* a count == 255 indicates this stack is at the count limit. */
	if (tls->buffer.count[i] == 0) {
	    continue;
	}
	if (tls->buffer.count[i] == 255) {
	    continue;
	}

	/* see if the stack addresses match */
	for (j = 0; j < framecount ; j++ )
	{
	    if ( tls->buffer.bt[i+j] != framebuf[j] ) {
		   break;
	    }
	}

	if ( j == framecount) {
	    stack_already_exists = TRUE;
	    stackindex = i;
	}
    }

    /* if the stack already exisits in the buffer, update its count
     * and return. If the stack is already at the count limit.
    */
    if (stack_already_exists && tls->buffer.count[stackindex] < 255 ) {
	/* update count for this stack */
	tls->buffer.count[stackindex] = tls->buffer.count[stackindex] + 1;
	return;
    }

    /* sample buffer has no room for these stack frames.*/
    int buflen = tls->data.bt.bt_len + framecount;
    if ( buflen > BufferSize) {
	/* send the current sample buffer. (will init a new buffer) */
	send_samples(tls);
    }

    /* add frames to sample buffer, compute addresss range */
    for (i = 0; i < framecount ; i++)
    {
	/* always add address to buffer bt */
	tls->buffer.bt[tls->data.bt.bt_len] = framebuf[i];

	/* top of stack indicated by a positive count. */
	/* all other elements are 0 */
	if (i > 0 ) {
	    tls->buffer.count[tls->data.count.count_len] = 0;
	} else {
	    tls->buffer.count[tls->data.count.count_len] = 1;
	}

	if (framebuf[i] < tls->header.addr_begin ) {
	    tls->header.addr_begin = framebuf[i];
	}
	if (framebuf[i] > tls->header.addr_end ) {
	    tls->header.addr_end = framebuf[i];
	}
	tls->data.bt.bt_len++;
	tls->data.count.count_len++;
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
void hwctime_start_sampling(const char* arguments)
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

    hwctime_start_sampling_args args;
    memset(&args, 0, sizeof(args));

    /* set defaults */ 
    int hwctime_papithreshold = THRESHOLD*2;
    char* hwctime_papi_event = "";

    /* Decode the passed function arguments */
#if defined (OPENSS_OFFLINE)
    char* hwctime_event_param = getenv("OPENSS_HWCTIME_EVENT");
    if (hwctime_event_param != NULL) {
        hwctime_papi_event=hwctime_event_param;
    } else {
	hwctime_papi_event="PAPI_TOT_CYC";
    }

    const char* sampling_rate = getenv("OPENSS_HWCTIME_THRESHOLD");
    if (sampling_rate != NULL) {
        hwctime_papithreshold=atoi(sampling_rate);
    }
    args.collector = 1;
    args.experiment = 0;
    tls->data.interval = hwctime_papithreshold;
#else
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwctime_start_sampling_args,
			    &args);
    hwctime_papithreshold = (uint64_t)(args.sampling_rate);
    hwctime_papi_event = args.hwctime_event;
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
    OpenSS_SetSendToFile(&(tls->header), "hwctime","openss-data");

    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Initialize the actual data blob */
    tls->data.bt.bt_len = 0;
    tls->data.bt.bt_val = tls->buffer.bt;
    tls->data.count.count_len = 0;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    memset(tls->buffer.bt, 0, sizeof(tls->buffer.bt));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));

    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();

    if(hwctime_papi_init_done == 0) {
	hwc_init_papi();
	tls->EventSet = PAPI_NULL;
	hwctime_papi_init_done = 1;
    }

    unsigned papi_event_code = get_papi_eventcode(hwctime_papi_event);

    OpenSS_Create_Eventset(&tls->EventSet);
    OpenSS_AddEvent(tls->EventSet, papi_event_code);
    OpenSS_Overflow(tls->EventSet, papi_event_code,
		    hwctime_papithreshold, hwctimePAPIHandler);
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
void hwctime_stop_sampling(const char* arguments)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    if (tls->EventSet == PAPI_NULL) {
	/*fprintf(stderr,"hwctime_stop_sampling RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
        return;
    }

    /* Stop sampling */
    OpenSS_Stop(tls->EventSet, NULL);

    tls->header.time_end = OpenSS_GetTime();

    /* Are there any unsent samples? */
    if(tls->data.bt.bt_len > 0) {

#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "hwctime_stop_sampling calls send_samples.\n");
	}
#endif

	/* Send these samples */
	send_samples(tls);
    }

/* debug used to compute space needs for data blobs */
#if 0
fprintf(stderr,"hwctime_stop_sampling: total calls to handler= %d\n",total);
fprintf(stderr,"hwctime_stop_sampling: stacktotal = %d\n",stacktotal);
fprintf(stderr,"hwctime_stop_sampling: values[0] = %d\n",values[0]);
fprintf(stderr,"hwctime_stop_sampling: values[1] = %d\n",values[1]);
#endif

    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
}
