/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2008,2009 The Krell Institute. All Rights Reserved.
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
/* #define DEBUG 1 */
#include "RuntimeAPI.h"
#include "UnwindAPI.h"
#include "runtime.h"
#include "IOTTraceableFunctions.h"

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
/* assume a pathname has at most 256 chars with terminating NULL.*/
/* space for pathnames */ 

/* PathBufferSize was previously defined: #define PathBufferSize(OpenSS_BlobSizeFactor*2048) */
/* jeg via dpm - change (5/19/2015) for bug found at INL 2/23/15 with this assert:           */
/* Assertion (*xdrproc)(&xdrs, (void*)data) == TRUE failed in file OpenSS_Send.c at line 76  */
/* Adjust the buffer size to smaller size, which can hold 16 paths of length 256 characters. */
#define PathBufferSize  (4096)


/* iot_event is 80 bytes */
#define EventBufferSize (OpenSS_BlobSizeFactor * 140)

/** Thread-local storage. */
typedef struct {
    
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
    
#if defined (OPENSS_OFFLINE)
    char iot_traced[PATH_MAX];
#endif
    int do_trace;
} TLS;


#if defined(OPENSS_OFFLINE)
extern void offline_sent_data(int);
#endif


/* FIXME: is this thread safe? */
char currentpathname[PATH_MAX];


#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF8;
int iot_init_tls_done = 0;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

void defer_trace(int defer_tracing) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->do_trace = defer_tracing;
}

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
static void iot_send_events(TLS *tls)
{
    /* Set the end time of this data blob */
    tls->header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"IO Collector runtime sends data:\n");
        fprintf(stderr,"time_end(%llu) addr range [%#llx, %#llx] "
		" stacktraces_len(%d) events_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,tls->header.addr_end,
	    tls->data.stacktraces.stacktraces_len,
            tls->data.events.events_len);
    }
#endif

#if defined(OPENSS_OFFLINE)
    offline_sent_data(1);
#endif

    /* Send these events */
    OpenSS_Send(&(tls->header), (xdrproc_t)xdr_iot_data, &(tls->data));
    
    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.events.events_len = 0;    
    tls->data.pathnames.pathnames_len = 1;
    tls->buffer.pathnames[0] = 0;
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Increment the IO function wrapper nesting depth */
    ++tls->nesting_depth;

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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->do_trace = 0;

    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry = 0, start, i;
    unsigned pathindex = 0;

#if 0
fprintf(stderr,"ENTERED iot_record_event, sizeof event=%d, sizeof stacktrace=%d\n",sizeof(iot_event),sizeof(stacktrace));
#endif

    /* need to search pathnames to see if this pathname already exists. */
    int curpathlen = 0;
    if (currentpathname[0] > 0) {
	curpathlen = strlen(currentpathname);
    }

    for(start = 0, i = 0;
	(i < curpathlen) &&
	    ((start + i) < tls->data.pathnames.pathnames_len);
	++i)
    {
	/* Do the i'th charactes differ? */
	if(currentpathname[i] != tls->buffer.pathnames[start + i]) {
	    
	    /* Advance in the tracing buffer to the end of this stack trace */
	    for(start += i;
		(tls->buffer.pathnames[start] != 0) &&
		    (start < tls->data.pathnames.pathnames_len);
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
	char *s = &(tls->buffer.pathnames[entry]);
	printf("iot_record_event: tls->buffer.pathnames[%d]=%s, currentpathname=%s\n",entry, s, currentpathname );
#endif

    /* Otherwise add this pathname to the pathnames buffer */
    } else {
	
	/* Send events if there is insufficient room for this pathname */
	if((tls->data.pathnames.pathnames_len + curpathlen + 1) >=
	   PathBufferSize) {
#ifdef DEBUG
fprintf(stderr,"PathBufferSize is full, call iot_send_events\n");
#endif
	    iot_send_events(tls);
	}
	
	/* Add each char in the pathname to the pathnames buffer. */	
	entry = tls->data.pathnames.pathnames_len;
	for(i = 0; i < curpathlen; ++i) {
	    
	    /* Add the i'th frame to the tracing buffer */
	    tls->buffer.pathnames[entry + i] = currentpathname[i];
	}

	pathindex = entry;
	
	/* Add a terminating zero  to the pathnames buffer */
	tls->buffer.pathnames[entry + curpathlen] = 0;
	
	/* Set the new size of the pathnames buffer */
	tls->data.pathnames.pathnames_len += (curpathlen + 1);
	
    }

    /* Decrement the IO function wrapper nesting depth */
    --tls->nesting_depth;

    /*
     * Don't record events for any recursive calls to our IO function wrappers.
     * The place where this occurs is when the IO implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the IO library - not in the internal
     * implementation details of that library.
     */
    if(tls->nesting_depth > 0) {
#ifdef DEBUG
	fprintf(stderr,"iot_record_event RETURNS EARLY DUE TO NESTING\n");
#endif
	return;
    }
    
    /* Newer versions of libunwind now make io calls (open a file in /proc/<self>/maps)
     * that cause a thread lock in the libunwind dwarf parser. We are not interested in
     * any io done by libunwind while we get the stacktrace for the current context.
     * So we need to bump the nesting_depth before requesting the stacktrace and
     * then decrement nesting_depth after aquiring the stacktrace
     */

    ++tls->nesting_depth;
    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);
    --tls->nesting_depth;

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
	    ((start + i) < tls->data.stacktraces.stacktraces_len);
	++i)
	
	/* Do the i'th frames differ? */
	if(stacktrace[i] != tls->buffer.stacktraces[start + i]) {
	    
	    /* Advance in the tracing buffer to the end of this stack trace */
	    for(start += i;
		(tls->buffer.stacktraces[start] != 0) &&
		    (start < tls->data.stacktraces.stacktraces_len);
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
	if((tls->data.stacktraces.stacktraces_len + stacktrace_size + 1) >=
	   StackTraceBufferSize) {
#ifdef DEBUG
fprintf(stderr,"StackTraceBufferSize is full, call iot_send_events\n");
#endif
	    iot_send_events(tls);
	}
	
	/* Add each frame in the stack trace to the tracing buffer. */	
	entry = tls->data.stacktraces.stacktraces_len;
	for(i = 0; i < stacktrace_size; ++i) {
	    
	    /* Add the i'th frame to the tracing buffer */
	    tls->buffer.stacktraces[entry + i] = stacktrace[i];
	    
	    /* Update the address interval in the data blob's header */
	    if(stacktrace[i] < tls->header.addr_begin)
		tls->header.addr_begin = stacktrace[i];
	    if(stacktrace[i] > tls->header.addr_end)
		tls->header.addr_end = stacktrace[i];
	    
	}
	
	/* Add a terminating zero frame to the tracing buffer */
	tls->buffer.stacktraces[entry + stacktrace_size] = 0;
	
	/* Set the new size of the tracing buffer */
	tls->data.stacktraces.stacktraces_len += (stacktrace_size + 1);
	
    }
    
    /* Add a new entry for this event to the tracing buffer. */
    memcpy(&(tls->buffer.events[tls->data.events.events_len]),
	   event, sizeof(iot_event));
    tls->buffer.events[tls->data.events.events_len].stacktrace = entry;
    tls->buffer.events[tls->data.events.events_len].pathindex = pathindex;
    tls->data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls->data.events.events_len == EventBufferSize) {
#ifdef DEBUG
fprintf(stderr,"Event Buffer is full, call iot_send_events\n");
#endif
	iot_send_events(tls);
    }

    tls->do_trace = 1;
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

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
	//fprintf(stderr,"iot_start_tracing sets TLSKey %#x\n",TLSKey);
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
    iot_init_tls_done = 1;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_iot_start_tracing_args,
                            &args);


#if defined(OPENSS_OFFLINE)

    /* If OPENSS_IOT_TRACED is set to a valid list of io functions, trace only
     * those functions.
     * If OPENSS_IOT_TRACED is set and is empty, trace all functions.
     * For any misspelled function name in OPENSS_IOT_TRACED, silently ignore.
     * If all names in OPENSS_IOT_TRACED are misspelled or not part of
     * TraceableFunctions, nothing will be traced.
     */
    const char* iot_traced = getenv("OPENSS_IOT_TRACED");

    if (iot_traced != NULL && strcmp(iot_traced,"") != 0) {
	strcpy(tls->iot_traced,iot_traced);
    } else {
	strcpy(tls->iot_traced,traceable);
    }
#endif

    /* Initialize the IO function wrapper nesting depth */
    tls->nesting_depth = 0;

    /* Initialize the data blob's header */
    /* Passing &(tls->header) to OpenSS_InitializeDataHeader was not */
    /* safe on ia64 systems. */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_header));
    memcpy(&tls->header, &local_header, sizeof(OpenSS_DataHeader));
    OpenSS_SetSendToFile(&(tls->header), "iot","openss-data");

    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;
    tls->data.events.events_len = 0;
    tls->data.events.events_val = tls->buffer.events;
    tls->data.pathnames.pathnames_len = 1;
    tls->buffer.pathnames[0] = 0;
    tls->data.pathnames.pathnames_val = tls->buffer.pathnames;

    /* Set the begin time of this data blob */
    tls->header.time_begin = OpenSS_GetTime();
    tls->do_trace = 1;
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    defer_trace(0);

    /* Send events if there are any remaining in the tracing buffer */
    if(tls->data.events.events_len > 0)
	iot_send_events(tls);
}

bool_t iot_do_trace(const char* traced_func)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls;
    if (iot_init_tls_done) {
        tls = OpenSS_GetTLS(TLSKey);
    } else {
	return FALSE;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#if defined (OPENSS_OFFLINE)

    if (tls->do_trace == 0) {
	if (tls->nesting_depth > 1)
	    --tls->nesting_depth;
	return FALSE;
    }

    /* See if this function has been selected for tracing */
    char *tfptr, *saveptr, *tf_token;
    tfptr = strdup(tls->iot_traced);
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

    /* Remove any nesting due to skipping iot_start_event/iot_record_event for
     * potentially nested iop calls that are not being traced.
     */

    if (tls->nesting_depth > 1)
	--tls->nesting_depth;

    return FALSE;
#else
    /* Always return true for dynamic instrumentors since these collectors
     * can be passed a list of traced functions for use with executeInPlaceOf.
     */

    if (tls->do_trace == 0) {
	if (tls->nesting_depth > 1)
	    --tls->nesting_depth;
	return FALSE;
    }
    return TRUE;
#endif
}
