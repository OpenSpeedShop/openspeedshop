/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the IO event tracing collector's runtime.
 *
 */
/* #define DEBUG 1 */
#include "RuntimeAPI.h"
#include "runtime.h"

#if defined (OPENSS_OFFLINE)
#include "io_offline.h"
#endif

#if defined (OPENSS_USE_FILEIO)
#include "OpenSS_FileIO.h"
#endif

#include "IOTraceableFunctions.h"

/** Number of overhead frames in each stack frame to be skipped. */
#if defined(OPENSS_OFFLINE)
const unsigned OverheadFrameCount = 2;
#else
#if defined(__linux) && defined(__ia64)
const unsigned OverheadFrameCount = 2 /*3*/;
#else
const unsigned OverheadFrameCount = 2;
#endif
#endif

/*
 * NOTE: For some reason GCC doesn't like it when the following three macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Maximum number of frames to allow in each stack trace. */
/* what is a reasonable default here. 32? */
#define MaxFramesPerStackTrace 64 /*64*/

/** Number of stack trace entries in the tracing buffer. */
/** event.stacktrace buffer is 64*8=512 bytes */
/** allows for 6 unique stacktraces (384*8/512) */
#define StackTraceBufferSize (OpenSS_BlobSizeFactor * 384)


/** Number of event entries in the tracing buffer. */
/** io_event is 32 bytes */
#define EventBufferSize (OpenSS_BlobSizeFactor * 415)

/** Thread-local storage. */
static __thread struct {
    
    /** Nesting depth within the IO function wrappers. */
    unsigned nesting_depth;
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    io_data data;              /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	io_event events[EventBufferSize];          /**< IO call events. */
    } buffer;    
    
    char io_traced[PATH_MAX];
} tls;

/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
/*
NO DEBUG PRINT STATEMENTS HERE.
*/
static void io_send_events()
{
    /* Set the end time of this data blob */
    tls.header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"IO Collector runtime sends data:\n");
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
    OpenSS_Send(&(tls.header), (xdrproc_t)xdr_io_data, &(tls.data));
    
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
 * Start an event.
 *
 * Called by the IO function wrappers each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 *
 * @param event    Event to be started.
 */
/*
NO DEBUG PRINT STATEMENTS HERE.
*/
void io_start_event(io_event* event)
{
    /* Increment the IO function wrapper nesting depth */
    ++tls.nesting_depth;

    /* Initialize the event record. */
    memset(event, 0, sizeof(io_event));

}


    
/**
 * Record an event.
 *
 * Called by the IO function wrappers each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 * @param function    Address of the IO function for which the event is being
 *                    recorded.
 */
/*
NO DEBUG PRINT STATEMENTS HERE IF TRACING "write, __libc_write".
*/
void io_record_event(const io_event* event, uint64_t function)
{
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry = 0, start, i;
    unsigned pathindex = 0;

#ifdef DEBUG
fprintf(stderr,"ENTERED io_record_event, sizeof event=%d, sizeof stacktrace=%d, NESTING=%d\n",sizeof(io_event),sizeof(stacktrace),tls.nesting_depth);
#endif

    /* Decrement the IO function wrapper nesting depth */
    --tls.nesting_depth;

    /*
     * Don't record events for any recursive calls to our IO function wrappers.
     * The place where this occurs is when the IO implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the IO library - not in the internal
     * implementation details of that library.
     */
    if(tls.nesting_depth > 0) {
#ifdef DEBUG
	fprintf(stderr,"io_record_event RETURNS EARLY DUE TO NESTING\n");
#endif
	return;
    }
    
    /* Newer versions of libunwind now make io calls (open a file in /proc/<self>/maps)
     * that cause a thread lock in the libunwind dwarf parser. We are not interested in
     * any io done by libunwind while we get the stacktrace for the current context.
     * So we need to bump the nesting_depth before requesting the stacktrace and
     * then decrement nesting_depth after aquiring the stacktrace
     */

    ++tls.nesting_depth;
    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);
    --tls.nesting_depth;

    /*
     * Replace the first entry in the call stack with the address of the IO
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of io_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling io_record_event().
     */
    if(stacktrace_size > 0)
	stacktrace[0] = function;
    
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
	   StackTraceBufferSize) {
#ifdef DEBUG
fprintf(stderr,"StackTraceBufferSize is full, call io_send_events\n");
#endif
	    io_send_events();
	}
	
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
	   event, sizeof(io_event));
    tls.buffer.events[tls.data.events.events_len].stacktrace = entry;
    tls.data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls.data.events.events_len == EventBufferSize) {
#ifdef DEBUG
fprintf(stderr,"Event Buffer is full, call io_send_events\n");
#endif
	io_send_events();
    }

}



/**
 * Start tracing.
 *
 * Starts IO extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void io_start_tracing(const char* arguments)
{
    io_start_tracing_args args;

#if defined (OPENSS_USE_FILEIO)
    /* Create the rawdata output file prefix. */
    /* fpe_stop_tracing will append */
    /* a tid as needed for the actuall .openss-xdrtype filename */
    OpenSS_CreateFilePrefix("io");
#endif

#if defined (OPENSS_OFFLINE)

    /* TODO: need to handle arguments for offline collectors */
    args.collector=1;
    args.experiment=0; /* DataQueues index start at 0.*/
    /* traced functions here? */

    /* Initialize the info blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader */
    /* was not safe on ia64 systems. */
    OpenSS_DataHeader local_info_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector,
				&(local_info_header));
    memcpy(&tlsinfo.header, &local_info_header, sizeof(OpenSS_DataHeader));

    tlsinfo.header.time_begin = OpenSS_GetTime();

    openss_expinfo local_info;
    OpenSS_InitializeParameters(&(local_info));
    memcpy(&tlsinfo.info, &local_info, sizeof(openss_expinfo));
    tlsinfo.info.collector = "io";
    tlsinfo.info.exename = strdup(OpenSS_exepath);

    char* io_traced = getenv("OPENSS_IO_TRACED");

    /* If OPENSS_IO_TRACED is set to a valid list of io functions, trace only
     * those functions.
     * If OPENSS_IO_TRACED is set and is empty, trace all functions.
     * For any misspelled function name in OPENSS_IO_TRACED, silently ignore.
     * If all names in OPENSS_IO_TRACED are misspelled or not part of
     * TraceableFunctions, nothing will be traced.
     */

    if (io_traced != NULL && strcmp(io_traced,"") != 0) {
	tlsinfo.info.traced = strdup(io_traced);
	strcpy(tls.io_traced,io_traced);
    } else {
	tlsinfo.info.traced = strdup(traceable);
	strcpy(tls.io_traced,traceable);
    }

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"io_start_tracing sends tlsinfo:\n");
        fprintf(stderr,"collector=%s, hostname=%s, pid=%d, OpenSS_rawtid=%lx\n",
            tlsinfo.info.collector,tlsinfo.header.host,
	    tlsinfo.header.pid,tlsinfo.header.posix_tid);
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
                            (xdrproc_t)xdr_io_start_tracing_args,
                            &args);
#endif
 
    /* Initialize the IO function wrapper nesting depth */
    tls.nesting_depth = 0;

    /* Initialize the data blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader was not */
    /* safe on ia64 systems. */
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

    /* Set the begin time of this data blob */
    tls.header.time_begin = OpenSS_GetTime();
}



/**
 * Stop tracing.
 *
 * Stops IO extended event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void io_stop_tracing(const char* arguments)
{
    /* Send events if there are any remaining in the tracing buffer */
    if(tls.data.events.events_len > 0)
	io_send_events();
}

bool_t io_do_trace(const char* traced_func)
{
#if defined (OPENSS_OFFLINE)
    /* See if this function has been selected for tracing */

    char *tfptr, *saveptr, *tf_token;
    tfptr = strdup(tls.io_traced);
    int i;
    for (i = 1;  ; i++, tfptr = NULL) {
	tf_token = strtok_r(tfptr, ":,", &saveptr);
	if (tf_token == NULL)
	    break;
	if ( strcmp(tf_token,traced_func) == 0) {
	
    	    if (tfptr)
		free(tfptr);
	    return TRUE;
	}
    }

    /* Remove any nesting due to skipping io_start_event/io_record_event for
     * potentially nested iop calls that are not being traced.
     */

    if (tls.nesting_depth > 1)
	--tls.nesting_depth;

    return FALSE;
#else
    /* Always return true for dynamic instrumentors since these collectors
     * can be passed a list of traced functions for use with executeInPlaceOf.
     */

    return TRUE;
#endif
}
