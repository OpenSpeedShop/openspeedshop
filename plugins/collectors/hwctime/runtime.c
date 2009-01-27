/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007 Krell Institute.  All Rights Reserved.
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
				    /**< Each stack is terminated by */
				    /**< a NULL address.             */
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
    OpenSS_Stop(tls->EventSet);
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

    OpenSS_SetSendToFile("hwctime","openss-data");
    OpenSS_Send(&(tls->header),(xdrproc_t)xdr_hwctime_data,&(tls->data));

    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Re-initialize the actual data blob */
    tls->data.bt.bt_len = 0;
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
void
hwctimePAPIHandler(int EventSet, void *address, long_long overflow_vector,
		    void* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Obtain the program counter (PC) address from the thread context */
    /* We will test passedpc against the first stack frame address */
    /* to see if we have to skip any signal handler overhead. */
    /* Suse and SLES may not have the signal handler overhead. */
    uint64_t passedpc;
    passedpc = OpenSS_GetPCFromContext(context);

    unw_word_t ip;	/* current stack trace pc address */

    /* Obtain the program counter current address from the thread context */
    /* and unwind the stack from there. */
    /* Libunwind provides it's own get context routines and the libunwind */
    /* documentation suggests that we use that. */
    

    unw_cursor_t cursor;          /* libunwind stack cursor (pointer) */
    unw_context_t uc;	          /* libunwind context */

#if UNW_TARGET_IA64
    unw_getcontext (&uc);         /* get the libunwind context */
#else
    /* copy passed context to a libunwind context */
    memcpy(&uc, context, sizeof(ucontext_t));
#endif

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

	/* PAPI introduces one more frame of overhead than found in usertime */
	/* are we already past the 4 frames of signal handler overhead? */
        if (overhead_marker == 0 && passedpc == ip) {
            overhead_marker = 4; /* we started past the overhead */
        }

	/* add frame address if we are past any signal handler overhead */
	if (overhead_marker > 3) {
	    /* add frame address to buffer */
            tls->buffer.bt[tls->data.bt.bt_len] = (uint64_t) ip;

            /* Update the address interval in the data blob's header */
            if(ip < tls->header.addr_begin) {
	        tls->header.addr_begin = (uint64_t) ip;
	    }
            if(ip > tls->header.addr_end) {
	        tls->header.addr_end = (uint64_t) ip;
	    }
	    tls->data.bt.bt_len++;

            /* Is the sample buffer full? */
            if(tls->data.bt.bt_len == BufferSize) {
    	        /* Send these samples */
	        send_samples(tls);
            }
	    stacktotal++;

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
    tls->buffer.bt[tls->data.bt.bt_len] = 0L;
    tls->data.bt.bt_len++;

    /* Is the sample buffer full? */
    if(tls->data.bt.bt_len == BufferSize) {
	send_samples(tls);
    }

    /* reset the signal handler overhead marker */
    overhead_marker = 0;
    total++;
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
    hwctime_start_sampling_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Decode the passed function arguments */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwctime_start_sampling_args,
			    &args);

    int hwctime_papithreshold = (uint64_t)(args.sampling_rate);
    
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
    
    /* Initialize the actual data blob */
    tls->data.interval = (uint64_t)(args.sampling_rate);
    tls->data.bt.bt_len = 0;
    tls->data.bt.bt_val = tls->buffer.bt;


    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();

    if(hwctime_papi_init_done == 0) {
	hwc_init_papi();
	tls->EventSet = PAPI_NULL;
	hwctime_papi_init_done = 1;
    }

    int hwctime_papi_event = PAPI_NULL;
#if defined (OPENSS_OFFLINE)
    const char* hwctime_event_param = getenv("OPENSS_HWC_EVENT");
    if (hwctime_event_param != NULL) {
        hwctime_papi_event=get_papi_eventcode((char *)hwctime_event_param);
    } else {
        hwctime_papi_event=get_papi_eventcode("PAPI_TOT_CYC");
    }
#else
    hwctime_papi_event = args.hwctime_event;
#endif

    OpenSS_Create_Eventset(&tls->EventSet);
    OpenSS_AddEvent(tls->EventSet, hwctime_papi_event);
    OpenSS_Overflow(tls->EventSet, hwctime_papi_event,
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

    /* Stop sampling */
    OpenSS_Stop(tls->EventSet);
    tls->header.time_end = OpenSS_GetTime();

    if (tls->EventSet == PAPI_NULL) {
	/*fprintf(stderr,"hwctime_stop_sampling RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
        return;
    }


/* debug used to compute space needs for data blobs */
#if 0
fprintf(stderr,"hwctime_stop_sampling: total calls to handler= %d\n",total);
fprintf(stderr,"hwctime_stop_sampling: stacktotal = %d\n",stacktotal);
fprintf(stderr,"hwctime_stop_sampling: values[0] = %d\n",values[0]);
fprintf(stderr,"hwctime_stop_sampling: values[1] = %d\n",values[1]);
#endif

    /* Send any samples remaining in the sample buffer */
    if(tls->data.bt.bt_len > 0) {
	send_samples(tls);
    }

    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
}
