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
 * Definition of the IOT event tracing collector's runtime.
 *
 */

#include "RuntimeAPI.h"
#include "runtime.h"

/** Number of overhead frames in each stack frame to be skipped. */
#if defined(__linux) && defined(__ia64)
const unsigned OverheadFrameCount = 2 /*3*/;
#else
const unsigned OverheadFrameCount = 2;
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
#define StackTraceBufferSize 384


/** Number of event entries in the tracing buffer. */
/* assume a pathname has at most 256 chars with terminating NULL.*/
#define PathBufferSize  2048 /* space for pathnames */ 
#define EventBufferSize 140  /* iot_event is 80 bytes */

/** Thread-local storage. */
static __thread struct {
    
    /** Nesting depth within the IO function wrappers. */
    unsigned nesting_depth;
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    iot_data data;              /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t  stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	iot_event events[EventBufferSize];            /**< IO call events. */
	char      pathnames[PathBufferSize];          /**< pathname buffer */
    } buffer;    
    
} tls;

char currentpathname[PATH_MAX];

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
static void iot_send_events()
{
    /* Set the end time of this data blob */
    tls.header.time_end = OpenSS_GetTime();
    
    /* Send these events */
    OpenSS_Send(&(tls.header), (xdrproc_t)xdr_iot_data, &(tls.data));
    
    /* Re-initialize the data blob's header */
    tls.header.time_begin = tls.header.time_end;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls.data.stacktraces.stacktraces_len = 0;
    tls.data.events.events_len = 0;    
    tls.data.pathnames.pathnames_len = 1;
    tls.buffer.pathnames[0] = 0;
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
void iot_start_event(iot_event* event)
{
    /* Increment the IO function wrapper nesting depth */
    ++tls.nesting_depth;
    
    /* Initialize the event record. */
    memset(event, 0, sizeof(iot_event));

    memset(currentpathname,0, sizeof(currentpathname));
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
void iot_record_event(const iot_event* event, uint64_t function)
{
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry = 0, start, i;
    unsigned pathindex = 0;

#ifdef DEBUG
fprintf(stderr,"ENTERED iot_record_event, sizeof event=%d, sizeof stacktrace=%d\n",sizeof(iot_event),sizeof(stacktrace));
#endif

    /* need to search pathnames to see if this pathname already exists. */
    int curpathlen = 0;
    if (currentpathname[0] > 0) {
	curpathlen = strlen(currentpathname);
    }

    for(start = 0, i = 0;
	(i < curpathlen) &&
	    ((start + i) < tls.data.pathnames.pathnames_len);
	++i)
    {
	/* Do the i'th charactes differ? */
	if(currentpathname[i] != tls.buffer.pathnames[start + i]) {
	    
	    /* Advance in the tracing buffer to the end of this stack trace */
	    for(start += i;
		(tls.buffer.pathnames[start] != 0) &&
		    (start < tls.data.pathnames.pathnames_len);
		++start);
	    
	    /* Advance in the pathnames buffer past the terminating zero */
	    ++start;

	    /* Begin comparing at the zero'th frame again */
	    i = 0;
	}
    }

    /* Did we find a match in the pathnames buffer? */
    if(i == curpathlen) {
	entry = pathindex = start;
#ifdef DEBUG
	char *s = &(tls.buffer.pathnames[entry]);
	printf("iot_record_event: tls.buffer.pathnames[%d]=%s, currentpathname=%s\n",entry, s, currentpathname );
#endif

    /* Otherwise add this pathname to the pathnames buffer */
    } else {
	
	/* Send events if there is insufficient room for this pathname */
	if((tls.data.pathnames.pathnames_len + curpathlen + 1) >=
	   PathBufferSize) {
#ifdef DEBUG
fprintf(stderr,"PathBufferSize is full, call iot_send_events\n");
#endif
	    iot_send_events();
	}
	
	/* Add each char in the pathname to the pathnames buffer. */	
	entry = tls.data.pathnames.pathnames_len;
	for(i = 0; i < curpathlen; ++i) {
	    
	    /* Add the i'th frame to the tracing buffer */
	    tls.buffer.pathnames[entry + i] = currentpathname[i];
	}

	pathindex = entry;
	
	/* Add a terminating zero  to the pathnames buffer */
	tls.buffer.pathnames[entry + curpathlen] = 0;
	
	/* Set the new size of the pathnames buffer */
	tls.data.pathnames.pathnames_len += (curpathlen + 1);
	
    }

    /* Decrement the IO function wrapper nesting depth */
    --tls.nesting_depth;

    /*
     * Don't record events for any recursive calls to our IO function wrappers.
     * The place where this occurs is when the IO implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the IO library - not in the internal
     * implementation details of that library.
     */
    if(tls.nesting_depth > 0)
	return;
    
    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);

    /*
     * Replace the first entry in the call stack with the address of the IO
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of iot_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling iot_record_event().
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
fprintf(stderr,"StackTraceBufferSize is full, call iot_send_events\n");
#endif
	    iot_send_events();
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
	   event, sizeof(iot_event));
    tls.buffer.events[tls.data.events.events_len].stacktrace = entry;
    tls.buffer.events[tls.data.events.events_len].pathindex = pathindex;
    tls.data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls.data.events.events_len == EventBufferSize) {
#ifdef DEBUG
fprintf(stderr,"Event Buffer is full, call iot_send_events\n");
#endif
	iot_send_events();
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
void iot_start_tracing(const char* arguments)
{
    iot_start_tracing_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_iot_start_tracing_args,
                            &args);
    
    /* Initialize the IO function wrapper nesting depth */
    tls.nesting_depth = 0;

    /* Initialize the data blob's header */
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(tls.header));
    tls.header.time_begin = 0;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls.data.stacktraces.stacktraces_len = 0;
    tls.data.stacktraces.stacktraces_val = tls.buffer.stacktraces;
    tls.data.events.events_len = 0;
    tls.data.events.events_val = tls.buffer.events;
    tls.data.pathnames.pathnames_len = 1;
    tls.buffer.pathnames[0] = 0;
    tls.data.pathnames.pathnames_val = tls.buffer.pathnames;

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
void iot_stop_tracing(const char* arguments)
{
    /* Send events if there are any remaining in the tracing buffer */
    if(tls.data.events.events_len > 0)
	iot_send_events();
}
