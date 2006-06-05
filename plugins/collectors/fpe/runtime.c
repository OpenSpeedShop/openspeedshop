/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the FPE event tracing collector's runtime.
 *
 */

#include "RuntimeAPI.h"
#include "blobs.h"
#include <fenv.h>


/*
 * NOTE: For some reason GCC doesn't like it when the following three macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Maximum number of frames to allow in each stack trace. */
#define MaxFramesPerStackTrace 64 /*64*/

/** Number of stack trace entries in the tracing buffer. */
#define StackTraceBufferSize 384

/** Number of event entries in the tracing buffer. */
#define EventBufferSize 554

/** Thread-local storage. */
#ifdef WDH_PER_THREAD_DATA_COLLECTION
static __thread struct {
#else
static struct {
#endif
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    fpe_data data;            /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	fpe_event events[EventBufferSize];          /**< FPE call events. */
    } buffer;    
    
} tls;



/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
static void fpe_send_events()
{
    /* Set the end time of this data blob */
    tls.header.time_end = OpenSS_GetTime();
    
    /* Send these events */
    OpenSS_Send(&(tls.header), (xdrproc_t)xdr_fpe_data, &(tls.data));
    
    /* Re-initialize the data blob's header */
    tls.header.time_begin = tls.header.time_end;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls.data.stacktraces.stacktraces_len = 0;
    tls.data.events.events_len = 0;    
}

/**
 * Start trapping all FPE's.
 *
 */
void fpe_enable_fpes()
{
    feenableexcept(FE_ALL_EXCEPT);
}

    
/**
 * Record an event.
 *
 * Called by the FPE handler each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 */
void fpe_record_event(const fpe_event* event, const ucontext_t* context)
{
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry, start, i;

    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(context, TRUE, 0,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);

    /*
     * Search the tracing buffer for an existing stack trace matching the stack
     * trace from the current thread context. For now do a simple linear search.
     */
    for(start = 0, i = 0;
	(i < stacktrace_size) &&
	    ((start + i) < tls.data.stacktraces.stacktraces_len);
	++i)
	
	/* Do the i'th frames differ? */
	if(stacktrace[i] != tls.buffer.stacktraces[start + i]) {
	    
	    /* Advance in the tracing buffer to the end of this stack trace */
	    for(start += i;
		(tls.buffer.stacktraces[start] != 0) &&
		    (start < tls.data.stacktraces.stacktraces_len);
		++start);
	    
	    /* Advance in the tracing buffer past the terminating zero */
	    ++start;
	    
	    /* Begin comparing at the zero'th frame again */
	    i = 0;
	    
	}
    
    /* Did we find a match in the tracing buffer? */
    if(i == stacktrace_size)
	entry = start;
    
    /* Otherwise add this stack trace to the tracing buffer */
    else {
	
	/* Send events if there is insufficient room for this stack trace */
	if((tls.data.stacktraces.stacktraces_len + stacktrace_size + 1) >=
	   StackTraceBufferSize)
	    fpe_send_events();
	
	/* Add each frame in the stack trace to the tracing buffer. */	
	entry = tls.data.stacktraces.stacktraces_len;
	for(i = 0; i < stacktrace_size; ++i) {
	    
	    /* Add the i'th frame to the tracing buffer */
	    tls.buffer.stacktraces[entry + i] = stacktrace[i];
	    
	    /* Update the address interval in the data blob's header */
	    if(stacktrace[i] < tls.header.addr_begin)
		tls.header.addr_begin = stacktrace[i];
	    if(stacktrace[i] > tls.header.addr_end)
		tls.header.addr_end = stacktrace[i];
	    
	}
	
	/* Add a terminating zero frame to the tracing buffer */
	tls.buffer.stacktraces[entry + stacktrace_size] = 0;
	
	/* Set the new size of the tracing buffer */
	tls.data.stacktraces.stacktraces_len += (stacktrace_size + 1);
	
    }
    
    /* Add a new entry for this event to the tracing buffer. */
    memcpy(&(tls.buffer.events[tls.data.events.events_len]),
	   event, sizeof(fpe_event));
    tls.buffer.events[tls.data.events.events_len].stacktrace = entry;
    tls.data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls.data.events.events_len == EventBufferSize)
	fpe_send_events();
}


/**
 * Floating Point Exception (FPE) handler.
 *
 * @param fpetype    The fpe type (from si_code) for this SIGFPE.
 * @param context    Thread context at SIGFPE interrupt.
 */
static void fpeHandler(const OpenSS_FPEType fpetype, const ucontext_t* context)
{
    /* Initialize the event record. */
    fpe_event event;
    memset(&event, 0, sizeof(fpe_event));

    fpe_type fpexception = FPE_FE_UNKNOWN;

    switch (fpetype) {
	case Overflow:
			fpexception = FPE_FE_OVERFLOW;
			break;
	case Underflow:
			fpexception = FPE_FE_UNDERFLOW;
			break;
	case DivideByZero:
			fpexception = FPE_FE_DIVBYZERO;
			break;
	case InexactResult:
			fpexception = FPE_FE_INEXACT;
			break;
	case InvalidOperation:
			fpexception = FPE_FE_INVALID;
			break;
	case SubscriptOutOfRange:
			fpexception = FPE_FE_UNNORMAL;
			break;
	case Unknown:
	default:
			fpexception = FPE_FE_UNKNOWN;
			break;
    }

    event.fpexception = fpexception;
    event.start_time = OpenSS_GetTime();

    fpe_record_event(&event, context);

    uint64_t newaddr;
    uint64_t where = OpenSS_GetPCFromContext(context);
    
    int instr_length = OpenSS_GetInstrLength(where);

#if defined(__linux) && defined(__ia64)
    /* for IA64 OpenSS_GetInstrLength returns the length of
       an instruction bundle. We need to determine the
       address of the bundle where the fp exception occurred
       and add the instr_length to that.
       An IA64 instruction bundle is 128 bits (0x10).
    */

    newaddr = (where - (where & 0xf)) + instr_length;
#else
    newaddr = where + instr_length;
#endif

    ucontext_t* uc = (ucontext_t*) context;
    OpenSS_SetPCInContext(newaddr, uc);
}


/**
 * Start tracing.
 *
 * Starts FPE extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void fpe_start_tracing(const char* arguments)
{
    fpe_start_tracing_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_fpe_start_tracing_args,
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

    /* Set the begin time of this data blob */
    tls.header.time_begin = OpenSS_GetTime();

    /* Enable FPE trapping and install the handler.
       FIXME: eventually, we likely will remove the OpenSS_FPEtype arg
       in the call to OpenSS_FPEHandler. We will need fpe_enable_fpes
       to accept an OpenSS_FPEtype argument to allow user to select
       which FPE's to trap. For now we trap all FPE's.
    */
    fpe_enable_fpes();
    OpenSS_FPEHandler(AllFPE,fpeHandler);
}


/**
 * Stop tracing.
 *
 * Stops FPE extended event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void fpe_stop_tracing(const char* arguments)
{
    /* Send events if there are any remaining in the tracing buffer */
    if(tls.data.events.events_len > 0)
	fpe_send_events();
}
