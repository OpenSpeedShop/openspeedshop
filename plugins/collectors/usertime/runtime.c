/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration and definition of the User time collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "UnwindAPI.h"
#include "blobs.h"

#if UNW_TARGET_X86 || UNW_TARGET_X86_64
# define STACK_SIZE     (128*1024)      /* On x86, SIGSTKSZ is too small */
#else
# define STACK_SIZE     SIGSTKSZ
#endif

/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Number of entries in the sample buffer. */
#define BufferSize  1024

/** Man number of frames for callstack collection */
#define MAXFRAMES 100

#define OPENSS_HANDLE_UNWIND_SEGV 1
#if defined(OPENSS_HANDLE_UNWIND_SEGV)
#include <setjmp.h>
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    OpenSS_DataHeader header;	/**< Header for following data blob. */
    usertime_data data;		/**< Actual data blob. */

    /** Sample buffer. */
    struct {
	uint64_t bt[BufferSize];    /**< Stack trace (PC) addresses. */
	uint8_t  count[BufferSize]; /**< count value greater than 0 is top */
				    /**< of stack. A count of 255 indicates */
				    /**< another instance of this stack may */
				    /**< exist in buffer bt. */
    } buffer;    

    bool_t defer_sampling;

#if defined(OPENSS_HANDLE_UNWIND_SEGV)
    sigjmp_buf unwind_jmp;
    int unwind_segvcount;
    bool_t is_unwinding;
    int sample_count;
#endif

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF4;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

#if defined (OPENSS_OFFLINE)
extern void offline_sent_data(int);
static void usertimeTimerHandler(const ucontext_t* context);

void usertime_start_timer()
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

void usertime_stop_timer()
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


#if defined(OPENSS_HANDLE_UNWIND_SEGV)
int OpenSS_Usertime_SEGVhandler(int sig, siginfo_t *siginfo, void *context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL) {
	return 1;
    }

    if (tls != NULL && tls->is_unwinding) {
        tls->unwind_segvcount++;
        siglongjmp(tls->unwind_jmp,9);
	return 0;
    }
    return 1;
}

// Implement a sigsegv,sigbus handler specific to usertime.
// We see both sigsegv or sigbus when unwinding fails do to
// any number of reasons (libunwind).  So install a handler for both.
// libmonitor provides an easy to use sigaction override so use it.:)
int OpenSS_Usertime_SetSEGVhandler(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
    tls->is_unwinding = FALSE;
    tls->unwind_segvcount = 0;

    int rval = monitor_sigaction(SIGSEGV, &OpenSS_Usertime_SEGVhandler, 0, NULL);
    if (rval != 0) {
        fprintf(stderr,"usertime unwinder SIGSEGV handler failed to install\n");
    }
    rval = monitor_sigaction(SIGBUS, &OpenSS_Usertime_SEGVhandler, 0, NULL);
    if (rval != 0) {
        fprintf(stderr,"usertime unwinder SIGBUS handler failed to install\n");
    }
    return rval;
}
#endif


/* utility to send samples when needed */
static void send_samples(TLS *tls)
{
    /* Send these samples */
    tls->header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"usertime send_samples %s, %lld sends data:\n",tls->header.host,tls->header.pid);
        fprintf(stderr,"time_end(%llu) addr range [%#llx, %#llx] bt_len(%d) count_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,
	    tls->header.addr_end,tls->data.bt.bt_len,
            tls->data.count.count_len);
    }
#endif

    OpenSS_Send(&(tls->header),(xdrproc_t)xdr_usertime_data,&(tls->data));

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

    //fprintf(stderr,"Enter utimeHandler context pc: %lx\n",OpenSS_GetPCFromContext(context));

    int framecount = 0;
    int stackindex = 0;
    uint64_t framebuf[MAXFRAMES];
    uint64_t beginaddr = tls->header.addr_begin;
    uint64_t endaddr = tls->header.addr_end;

    memset(framebuf,0, sizeof(framebuf));

#if defined(OPENSS_HANDLE_UNWIND_SEGV)
    tls->sample_count++;
    /* flag handling of unwind */
    tls->is_unwinding = TRUE;

    int ourlongjmp = sigsetjmp(tls->unwind_jmp, 1);
    if (ourlongjmp == 0 ) {
#endif

    /* get stack address for current context and store them into framebuf. */

#if defined(__linux) && defined(__x86_64)

#if defined(USE_FASTTRACE)
    OpenSS_GetStackTrace(TRUE, 0,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#else
    OpenSS_GetStackTraceFromContext (context, TRUE, 0,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif

#else
    OpenSS_GetStackTraceFromContext (context, TRUE, 0,
                        MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif
    //fprintf(stderr,"utimeHandler framecount is %d\n",framecount);

#if defined(OPENSS_HANDLE_UNWIND_SEGV)
    } else {
    }

    /* flag handling of unwind */
    tls->is_unwinding = FALSE;
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
	//fprintf(stderr,"frame[%d]: %p\n",i,framebuf[i]);

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
 * Starts user time sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void usertime_start_sampling(const char* arguments)
{
    usertime_start_sampling_args args;

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

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_usertime_start_sampling_args,
			    &args);
    
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
    OpenSS_SetSendToFile(&(tls->header), "usertime", "openss-data");

    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Initialize the actual data blob */
    tls->data.interval = (uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls->data.bt.bt_len = 0;
    tls->data.bt.bt_val = tls->buffer.bt;
    tls->data.count.count_len = 0;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    memset(tls->buffer.bt, 0, sizeof(tls->buffer.bt));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));

#if defined(OPENSS_HANDLE_UNWIND_SEGV)
    memset((void *)tls->unwind_jmp, '\0', sizeof(tls->unwind_jmp));
    /* install segv handler */
    int rval = OpenSS_Usertime_SetSEGVhandler();
    if (rval != 0) {
        fprintf(stderr,"No handler for unwind SIGSEGV\n");
    }
    tls->sample_count = 0;
#endif

    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls->data.interval, usertimeTimerHandler);
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
