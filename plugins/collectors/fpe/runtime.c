/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include <fenv.h>

#if defined (OPENSS_OFFLINE)
#include "fpe_offline.h"
#endif

#if defined (OPENSS_USE_FILEIO)
#include "OpenSS_FileIO.h"
#endif


/*
 * NOTE: For some reason GCC doesn't like it when the following three macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/*  Produce debug output about the reason for the exception
*/
#define DEBUG 

/** Maximum number of frames to allow in each stack trace. */
#define MaxFramesPerStackTrace 64 /*64*/

/** Number of stack trace entries in the tracing buffer. */
#define StackTraceBufferSize (OpenSS_BlobSizeFactor * 384)

/** Number of event entries in the tracing buffer. */
#define EventBufferSize (OpenSS_BlobSizeFactor * 554)

/** Thread-local storage. */
static __thread struct {
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    fpe_data data;            /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	fpe_event events[EventBufferSize];          /**< FPE call events. */
    } buffer;    
    
} tls;

#ifdef DEBUG
void fpe_print_cause(const OpenSS_FPEType fpetype)
{
    fprintf(stderr,
      "FPE reason %d = \"%s\"\n",
      fpetype,
      fpetype == DivideByZero ? "FP divide by zero"                :
      fpetype == Overflow ? "FP overflow"                      :
      fpetype == Underflow ? "FP underflow"                     :
      fpetype == InexactResult ? "FP inexact result"                :
      fpetype == InvalidOperation ? "FP invalid operation"             :
      fpetype == SubscriptOutOfRange ? "subscript out of range"           :
      "unknown"
    );
}
#endif


/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
static void fpe_send_events()
{
//fprintf(stderr,"ENTER fpe_send_events\n");
    /* Set the end time of this data blob */
    tls.header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"FPE Collector runtime sends data:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] "
		" stacktraces_len(%d) events_len(%d)\n",
            tls.header.time_end,tls.header.addr_begin,tls.header.addr_end,
	    tls.data.stacktraces.stacktraces_len,
            tls.data.events.events_len);
    }
#endif

#if defined (OPENSS_USE_FILEIO)
    /* Create the openss-raw file name for this exe-collector-pid-tid */
    /* Default is to create openss-raw files in /tmp */
    OpenSS_CreateOutfile("openss-data");
#endif
    
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
 * @param event    Event to be started.
 */
void fpe_enable_fpes()
{
/* fprintf(stderr,"ENTER fpe_enable_fpes\n"); */
#if 0
    feenableexcept(FE_ALL_EXCEPT);
#else
    feenableexcept(FE_DIVBYZERO|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW);
#endif
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

//    fprintf(stderr,"ENTER fpe_record_event\n");

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
/*    fprintf(stderr,"ENTER fpeHandler\n"); */

#ifdef DEBUG
     fpe_print_cause(fpetype); 
#endif

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

/*    fprintf(stderr,"OpenSS_GetInstrLength(%#lx) returns %d\n",
	    where, instr_length); */
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
/* fprintf(stderr,"ENTER fpe_start_tracing\n"); */
    fpe_start_tracing_args args;

#if defined (OPENSS_USE_FILEIO)
    /* Create the rawdata output file prefix. */
    /* fpe_stop_tracing will append */
    /* a tid as needed for the actuall .openss-xdrtype filename */
    OpenSS_CreateFilePrefix("fpe");
#endif

#if defined (OPENSS_OFFLINE)
    /* TODO: need to handle arguments for offline collectors */
    args.collector=1;
    args.experiment=0; /* DataQueues index start at 0.*/

    /* Initialize the info blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader */
    /* was not safe on ia64 systems. */
    OpenSS_DataHeader local_info_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector,
                                &(local_info_header));
    memcpy(&tlsinfo.header, &local_info_header, sizeof(OpenSS_DataHeader));

    tlsinfo.header.time_begin = OpenSS_GetTime();

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    tlsinfo.info.collector = "fpe";
    tlsinfo.info.hostname = strdup(hostname);
    tlsinfo.info.exename = strdup(OpenSS_exepath);
    tlsinfo.info.pid = getpid();
#if defined (OPENSS_USE_FILEIO)
    tlsinfo.info.tid = OpenSS_rawtid;
#endif

#ifdef DEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"fpe_start_tracing sends tlsinfo:\n");
        fprintf(stderr,"collector=%s, hostname=%s, pid=%d, OpenSS_rawtid=%lx\n",
            tlsinfo.info.collector,tlsinfo.info.hostname,
            tlsinfo.info.pid,tlsinfo.info.tid);
    }
#endif

    /* create the openss-info data and send it */
#if defined (OPENSS_USE_FILEIO)
    OpenSS_CreateOutfile("openss-info");
#endif
    tlsinfo.header.time_end = OpenSS_GetTime();
    OpenSS_Send(&(tlsinfo.header),
                (xdrproc_t)xdr_openss_expinfo,
                &(tlsinfo.info));

#else
    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_fpe_start_tracing_args,
                            &args);
#endif
    
    /* Initialize the data blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader was not safe on ia64 systems.
     */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_header));
    memcpy(&tls.header, &local_header, sizeof(OpenSS_DataHeader));

    tls.header.time_begin = 0;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls.data.stacktraces.stacktraces_len = 0;
    tls.data.stacktraces.stacktraces_val = tls.buffer.stacktraces;
    tls.data.events.events_len = 0;
    tls.data.events.events_val = tls.buffer.events;

#if defined (OPENSS_OFFLINE)
    tlsobj.objs.objname = NULL;
    tlsobj.objs.addr_begin = ~0;
    tlsobj.objs.addr_end = 0;
#endif

    /* Set the begin time of this data blob */
    tls.header.time_begin = OpenSS_GetTime();

    /* Enable FPE trapping and install the handler.
       FIXME: eventually, we likely will remove the OpenSS_FPEtype arg
       in the call to OpenSS_FPEHandler. We will need fpe_enable_fpes
       to accept an OpenSS_FPEtype argument to allow user to select
       which FPE's to trap. For now we trap all FPE's.
    */

/*    fprintf(stderr,"fpe_start_tracing: calling OpenSS_FPEHandler\n"); */

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
/* fprintf(stderr,"ENTER fpe_stop_tracing\n"); */
    /* Send events if there are any remaining in the tracing buffer */
    if(tls.data.events.events_len > 0)
	fpe_send_events();
}
