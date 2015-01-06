/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2008-2013 The Krell Institute. All Rights Reserved.
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
 * Definition of the MPI event tracing collector's runtime.
 *
 */

#include "RuntimeAPI.h"
#include "UnwindAPI.h"
#include "runtime.h"

#include "MPITraceableFunctions.h"

/** Number of overhead frames in each stack frame to be skipped. */
#if defined(__linux) && defined(__ia64)
const unsigned OverheadFrameCount = 3;
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
#define MaxFramesPerStackTrace 64

/** Number of stack trace entries in the tracing buffer. */
#define StackTraceBufferSize (OpenSS_BlobSizeFactor * 384)

/** Number of event entries in the tracing buffer. */
#define EventBufferSize (OpenSS_BlobSizeFactor * 554)

/** Thread-local storage. */
typedef struct {
    
    /** Nesting depth within the MPI function wrappers. */
    unsigned nesting_depth;
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    mpi_data data;            /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	mpi_event events[EventBufferSize];          /**< MPI call events. */
    } buffer;    
    
#if defined (OPENSS_OFFLINE)
    char mpi_traced[PATH_MAX];
#endif
    int do_trace;
} TLS;


#if defined(OPENSS_OFFLINE)
extern void offline_sent_data(int);
#endif


#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF9;
int mpi_init_tls_done = 0;
#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

void defer_trace(int defer_tracing) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);

    if (tls == NULL) {
       tls = malloc(sizeof(TLS));
       Assert(tls != NULL);
       OpenSS_SetTLS(TLSKey, tls);
       mpi_init_tls_done = 1;
    }
#else
    TLS* tls = &the_tls;
#endif

    tls->do_trace = defer_tracing;
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"MPI setting defer_trace to %d for HOST %s, pid %lld, posix_tid %llu\n",
		tls->do_trace, tls->header.host, tls->header.pid, tls->header.posix_tid);
    }
#endif
}


/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
static void mpi_send_events(TLS *tls)
{
    /* Set the end time of this data blob */
    tls->header.time_end = OpenSS_GetTime();

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"MPI mpi_send_events SENDS DATA for HOST %s, pid %lld, posix_tid %llu\n",
		tls->header.host, tls->header.pid, tls->header.posix_tid);
        fprintf(stderr,"time(%llu,%llu) addr range [%#llx, %#llx] "
		" stacktraces_len(%d) events_len(%d)\n",
            tls->header.time_begin,tls->header.time_end,
	    tls->header.addr_begin,tls->header.addr_end,
	    tls->data.stacktraces.stacktraces_len,
            tls->data.events.events_len);
    }
#endif

#if defined(OPENSS_OFFLINE)
    offline_sent_data(1);
#endif

    /* Send these events */
    OpenSS_Send(&(tls->header), (xdrproc_t)xdr_mpi_data, &(tls->data));
    
    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.events.events_len = 0;    
}
    


/**
 * Start an event.
 *
 * Called by the MPI function wrappers each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 *
 * @param event    Event to be started.
 */
void mpi_start_event(mpi_event* event)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Increment the MPI function wrapper nesting depth */
    ++tls->nesting_depth;
}


    
/**
 * Record an event.
 *
 * Called by the MPI function wrappers each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 * @param function    Address of the MPI function for which the event is being
 *                    recorded.
 */
void mpi_record_event(const mpi_event* event, uint64_t function)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry, start, i;

    /* Decrement the MPI function wrapper nesting depth */
    --tls->nesting_depth;

    /*
     * Don't record events for any recursive calls to our MPI function wrappers.
     * The place where this occurs is when the MPI implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the MPI library - not in the internal
     * implementation details of that library.
     */
    if(tls->nesting_depth > 0) {
#ifdef DEBUG
	fprintf(stderr,"mpi_record_event RETURNS EARLY DUE TO NESTING\n");
#endif
	return;
    }
    
    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);

    /*
     * Replace the first entry in the call stack with the address of the MPI
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of mpi_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling mpi_record_event().
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
#ifndef NDEBUG
	    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	      fprintf(stderr,"RANK (%d,%llu) SENDING DUE TO StackTraceBufferSize, %d * %d = %d\n",
		getpid(), event->start_time,
		tls->data.stacktraces.stacktraces_len,
		sizeof(uint64_t),
		(tls->data.stacktraces.stacktraces_len * sizeof(uint64_t)) );
	      fprintf(stderr,"EVENTBufferSize, %d * %d = %d\n",
		tls->data.events.events_len,
		sizeof(mpi_event),
		tls->data.events.events_len * sizeof(mpi_event));
	      fprintf(stderr,"RANK (%d) TOTAL SENT %d\n",  getpid(),
		(tls->data.stacktraces.stacktraces_len * sizeof(uint64_t)) +
		(tls->data.events.events_len * sizeof(mpi_event)));
	    }
#endif
	    mpi_send_events(tls);
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
	   event, sizeof(mpi_event));
    tls->buffer.events[tls->data.events.events_len].stacktrace = entry;
    tls->data.events.events_len++;

    /* Send events if the tracing buffer is now filled with events */
    if((tls->data.events.events_len ) >= EventBufferSize) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"RANK (%d, %llu) SENDING DUE TO EventBufferSize, %d * %d = %d\n",
		getpid(), event->start_time,
		tls->data.events.events_len,
		sizeof(mpi_event),
		tls->data.events.events_len * sizeof(mpi_event));
	    fprintf(stderr,"StackTraceBufferSize, %d * %d = %d\n",
		tls->data.stacktraces.stacktraces_len,
		sizeof(uint64_t),
		tls->data.stacktraces.stacktraces_len * sizeof(uint64_t));
	    fprintf(stderr,"RANK (%d) TOTAL SENT %d\n",  getpid(),
		(tls->data.stacktraces.stacktraces_len * sizeof(uint64_t)) +
		(tls->data.events.events_len * sizeof(mpi_event)));
	}
#endif
	mpi_send_events(tls);
	tls->data.events.events_len = 0;
    }
    
}



/**
 * Start tracing.
 *
 * Starts MPI extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void mpi_start_tracing(const char* arguments)
{
    mpi_start_tracing_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
    if (tls == NULL) {
       tls = malloc(sizeof(TLS));
       Assert(tls != NULL);
       OpenSS_SetTLS(TLSKey, tls);
       mpi_init_tls_done = 1;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTERED mpi_start_tracing for %d\n",getpid());
    }
#endif


    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_mpi_start_tracing_args,
                            &args);

#if defined(OPENSS_OFFLINE)
    const char* mpi_traced = getenv("OPENSS_MPI_TRACED");
    if (mpi_traced != NULL && strcmp(mpi_traced,"") != 0) {
        strcpy(tls->mpi_traced,mpi_traced);
    } else {
        strcpy(tls->mpi_traced,all);
    }
#endif

    /* Initialize the MPI function wrapper nesting depth */
    tls->nesting_depth = 0;

    /* Initialize the data blob's header */
    /* Passing &(tls->header) to OpenSS_InitializeDataHeader was not */
    /* safe on ia64 systems. */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_header));
    memcpy(&tls->header, &local_header, sizeof(OpenSS_DataHeader));
    OpenSS_SetSendToFile(&(tls->header), "mpi", "openss-data");

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"INIT HOST %s, PID %lld, POSIX_TID %llu\n",
	tls->header.host, tls->header.pid, tls->header.posix_tid);
    }
#endif


    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;
    tls->data.events.events_len = 0;
    tls->data.events.events_val = tls->buffer.events;

    unsetenv("LD_PRELOAD");

    /* Set the begin time of this data blob */
    tls->header.time_begin = OpenSS_GetTime();
}



/**
 * Stop tracing.
 *
 * Stops MPI extended event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void mpi_stop_tracing(const char* arguments)
{

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Send events if there are any remaining in the tracing buffer */
    if(tls->data.events.events_len > 0) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"RANK (%d) SENDING DUE TO FINISHED, %d * %d = %d\n",
		getpid(),
		tls->data.stacktraces.stacktraces_len,
		sizeof(uint64_t),
		(tls->data.stacktraces.stacktraces_len * sizeof(uint64_t)) );
	    fprintf(stderr,"EVENTBufferSize, %d * %d = %d\n",
		tls->data.events.events_len,
		sizeof(mpi_event),
		tls->data.events.events_len * sizeof(mpi_event));
	    fprintf(stderr,"RANK (%d) TOTAL SENT %d\n",  getpid(),
		(tls->data.stacktraces.stacktraces_len * sizeof(uint64_t)) +
		(tls->data.events.events_len * sizeof(mpi_event)));
	}
#endif
        defer_trace(0);

	mpi_send_events(tls);
    }
}

bool_t mpi_do_trace(const char* traced_func)
{

#if defined (OPENSS_OFFLINE)

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    /* libmonitor can start tracing one of the traced functons
     * immediately (even before the start_tracing function is
     * called). i.e. when we do a pcsamp experiment the MPI_Init
     * is traced and mpi_do_trace is called prior to having
     * set up the initial TLS via mallocc and OpenSS_SetTLS. 
     * Complicating this further is io tracing which also can
     * start tracing before it's TLS is initialized.
     * So we add a test to see if the collector TLS is initialized.
     * mpi_init_tls_done, io_init_tls_done, etc. for iot.
     * This is only an issue for collectors that must also trace
     * MPI_Init to get the mpi rank information.
     */
    TLS* tls;
    if (mpi_init_tls_done) {
	tls = OpenSS_GetTLS(TLSKey);
    } else {
	return FALSE;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

   if (tls->do_trace == 0) {
        if (tls->nesting_depth > 1)
            --tls->nesting_depth;
        return FALSE;
    }


    /* See if this function has been selected for tracing */

    char *tfptr, *saveptr, *tf_token;
    tfptr = strdup(tls->mpi_traced);
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

    /* Remove any nesting due to skipping mpi_start_event/mpi_record_event for
     * potentially nested iop calls that are not being traced.
     */

    if (tls->nesting_depth > 1)
	--tls->nesting_depth;

    return FALSE;
#else
    /* Always return true for dynamic instrumentors since these collectors
     * can be passed a list of traced functions for use with executeInPlaceOf.
     */

    return TRUE;
#endif
}
