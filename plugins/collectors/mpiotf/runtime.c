/*******************************************************************************
** Copyright (c) 2007 Krell Institute. All Rights Reserved.
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
 * Definition of the MPI extended event tracing collector's runtime.
 *
 */

#include "RuntimeAPI.h"
#include "blobs.h"
#include "runtime.h"

int outputOTF = 0;
int onlyOutputOTF = 0;

int vt_enter_user_called = 0;

/* initialized once from environment variable */
int vt_mpitrace = 1;

/* changed with every TRACE_ON/TRACE_OFF */
int vt_mpi_trace_is_on = 1;

#include "vt_openss.h"

int debug_trace = 0;

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
#define StackTraceBufferSize 512

/** Number of event entries in the tracing buffer. */
#define EventBufferSize 192

/** Thread-local storage. */
static __thread struct {
    
    /** Nesting depth within the MPI function wrappers. */
    unsigned nesting_depth;
    
    OpenSS_DataHeader header;  /**< Header for following data blob. */
    mpit_data data;            /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	mpit_event events[EventBufferSize];          /**< MPI call events. */
    } buffer;    
    
} tls;



/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
static void mpit_send_events()
{
//    fprintf(stderr, "mpit_send_events, entered for tls.header.thread=%d\n", tls.header.thread);
//    fflush(stderr);

    /* Set the end time of this data blob */
    tls.header.time_end = OpenSS_GetTime();
    
    /* Send these events */
    OpenSS_Send(&(tls.header), (xdrproc_t)xdr_mpit_data, &(tls.data));
    
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
 * Called by the MPI function wrappers each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 *
 * @param event    Event to be started.
 */
void mpit_start_event(mpit_event* event)
{
if (debug_trace) {
    fprintf(stderr, "mpit_start_event, entered\n");
    fflush(stderr);
}

    /* Increment the MPI function wrapper nesting depth */
    ++tls.nesting_depth;
    
    /* Initialize the event record. */
    memset(event, 0, sizeof(mpit_event));
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
void mpit_record_event(const mpit_event* event, uint64_t function, char * name_string)

{
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry, start, i;

if (debug_trace) {
    fprintf(stderr, "mpit_record_event, entered\n");
    fprintf(stderr, "mpit_record_event, function=0x%x, name_string=%s\n", function, name_string);
    fflush(stderr);
}

    /* Decrement the MPI function wrapper nesting depth */
    --tls.nesting_depth;

    /*
     * Don't record events for any recursive calls to our MPI function wrappers.
     * The place where this occurs is when the MPI implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the MPI library - not in the internal
     * implementation details of that library.
     */
    if(tls.nesting_depth > 0)
	return;
    
    /* Obtain the stack trace from the current thread context */
    OpenSS_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);

    /*
     * Replace the first entry in the call stack with the address of the MPI
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of mpit_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling mpit_record_event().
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
	   StackTraceBufferSize)
	    mpit_send_events();
	
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
	   event, sizeof(mpit_event));
    tls.buffer.events[tls.data.events.events_len].stacktrace = entry;
    tls.data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls.data.events.events_len == EventBufferSize)
	mpit_send_events();
}


void mpiotf_process_vt_otf_gen_env_vars()
{
    if (getenv("OPENSS_ONLY_WRITE_OTF") != NULL) {
       onlyOutputOTF = 1;
       outputOTF = 1;
    } else {
       onlyOutputOTF = 0;
    }

    /* Is writing OTF (open trace format) requested? */
    if (getenv("OPENSS_WRITE_OTF") != NULL) {
       outputOTF = 1;
    } else {
       outputOTF = 0;
    }

}

/**
 * Vampirtrace initialization for OTF generation
 *
 * Starts up the vampirtrace gathering/wrapping.
 * Initializes the appropriate data structures.
 *
 * arguments    none
 */

int mpiotf_openss_vt_init()
{

  mpit_event event;
  int retval; 
  int numprocs, i;
  unsigned char* grpv;
  uint64_t time;

  /* vampirtrace code start */
if (debug_trace) {
  fprintf(stderr, "mpiotf_openss_vt_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
  fflush(stderr);
}

  if (IS_TRACE_ON) {

      TRACE_OFF();

      /* first event?
         -> initialize VT and enter dummy function 'user' */
      if (!vt_open_called) {
        if (debug_trace) {
          fprintf(stderr, "mpiotf_openss_vt_init called, calling vt_open() \n");
          fflush(stderr);
        }
        vt_open();
        if (debug_trace) {
          fprintf(stderr, "mpiotf_openss_vt_init called, returned from vt_open() \n");
          fflush(stderr);
        }
        time = vt_pform_wtime();
        vt_enter_user(&time);
        vt_enter_user_called = 1;
      } else {
        time = vt_pform_wtime();
      }
/*
      vt_enter(&time, vt_mpi_regid[VT__MPI_INIT]);
      retval = PMPI_Init(NULL, NULL);
*/

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init calling vt_mpi_init\n");
      fflush(stderr);
}

      /* initialize mpi event handling */
      vt_mpi_init();

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init returned from calling vt_mpi_init\n");
      fflush(stderr);
}

      PMPI_Comm_size(MPI_COMM_WORLD, &numprocs);

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init returned from calling PMPI_Comm_size, numprocs=%d\n", numprocs);
      fflush(stderr);
}

      /* define communicator for MPI_COMM_WORLD */
      grpv = (unsigned char*)calloc(numprocs / 8 + (numprocs % 8 ? 1 : 0), sizeof(unsigned char));

      for (i = 0; i < numprocs; i++)
        grpv[i / 8] |= (1 << (i % 8));

      vt_def_mpi_comm(0, numprocs / 8 + (numprocs % 8 ? 1 : 0), grpv);

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init returned from calling vt_def_mpi_comm\n");
      fflush(stderr);
}


      free(grpv);

      /* initialize communicator management */
      vt_comm_init();

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init returned from calling vt_comm_init\n");
      fflush(stderr);
}


      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();

    } else {

/*
      retval = PMPI_Init(NULL, NULL);
*/

      /* initialize mpi event handling */
      vt_mpi_init();

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init ELSE returned from calling vt_mpi_init\n");
      fflush(stderr);
}

      PMPI_Comm_size(MPI_COMM_WORLD, &numprocs);

      /* define communicator for MPI_COMM_WORLD */
      grpv = (unsigned char*)calloc(numprocs / 8 + (numprocs % 8 ? 1 : 0), sizeof(unsigned char));
      for (i = 0; i < numprocs; i++)
        grpv[i / 8] |= (1 << (i % 8));

      vt_def_mpi_comm(0, numprocs / 8 + (numprocs % 8 ? 1 : 0), grpv);

      free(grpv);

      /* initialize communicator management */
      vt_comm_init();
    }


    /* vampirtrace code end */

if (debug_trace) {
      fprintf(stderr, "mpiotf_openss_vt_init exit IS_TRACE_ON = %d \n", IS_TRACE_ON);
      fflush(stderr);
}

      return retval;
}

/**
 * Start tracing.
 *
 * Starts MPI extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void mpiotf_start_tracing(const char* arguments)
{
    mpit_start_tracing_args args;
    int status;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
                            (xdrproc_t)xdr_mpit_start_tracing_args,
                            &args);
    
    /* Initialize the MPI function wrapper nesting depth */
    tls.nesting_depth = 0;

if (debug_trace) {
    fprintf(stderr, "mpiotf_start_tracing, entered\n");
    fflush(stderr);
}

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

    /* Set the begin time of this data blob */
    tls.header.time_begin = OpenSS_GetTime();

    /* Read env vars to see if we are generating otf */
    mpiotf_process_vt_otf_gen_env_vars();

#if 0
    /* Initialize vampirtrace for mpi tracing and generation of OTF files */
    if (outputOTF) {
      status = mpiotf_openss_vt_init();
    }
#endif

if (debug_trace) {
    fprintf(stderr, "mpiotf_start_tracing, exiting status=%d\n", status);
    fflush(stderr);
}

}



/**
 * Stop tracing.
 *
 * Stops MPI extended event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void mpiotf_stop_tracing(const char* arguments)
{
if (debug_trace) {
    fprintf(stderr, "mpiotf_stop_tracing, entered\n");
    fflush(stderr);
}

    /* Send events if there are any remaining in the tracing buffer */
    if(tls.data.events.events_len > 0)
	mpit_send_events();

if (debug_trace) {
    fprintf(stderr, "mpiotf_stop_tracing, exiting\n");
    fflush(stderr);
}

}
