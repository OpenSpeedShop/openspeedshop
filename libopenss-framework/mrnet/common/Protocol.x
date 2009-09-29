/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the frontend/backend communication protocol.
 *
 */



/**
 * Memory address.
 *
 * All memory addresses are represented using a 64-bit unsigned integer.
 * This allows for a unified representation of both 32-bit and 64-bit
 * address spaces by sacrificing storage space when 32-bit addresses are
 * processed. Various overflow and underflow conditions are checked when
 * arithmetic operations are performed on these addresses.
 */
typedef uint64_t OpenSS_Protocol_Address;



/**
 * Range of memory addresses.
 *
 * A single, closed-ended, range of memory addresses: [begin, end]. Used in
 * several different places for representing a single contiguous portion of
 * an address space, as occupied by a DSO, compilation unit, function, etc.
 */
struct OpenSS_Protocol_AddressRange
{
    OpenSS_Protocol_Address begin;  /**< Beginning of the address range. */
    OpenSS_Protocol_Address end;    /**< End of the address range. */
};



/**
 * Binary large object.
 *
 * Encapsulates a buffer of raw, untyped, binary data. Used to store arguments
 * to functions called by instrumentation.
 *
 * @sa    http://www.hyperdictionary.com/computing/binary+large+object
 */
struct OpenSS_Protocol_Blob
{
    /** Binary data in the blob. */
    uint8_t data<>;
};



/**
 * Address bitmap.
 *
 * A bitmap containing one bit per address within an address range. Used to
 * describe which addresses within an address range are actually attributable
 * to a function or source statement.
 */
struct OpenSS_Protocol_AddressBitmap
{
    /** Address range covered by this bitmap. */
    OpenSS_Protocol_AddressRange range;

    /** Actual bitmap. */
    OpenSS_Protocol_Blob bitmap;
};



/**
 * Collector identifier.
 *
 * Names a specific instance of a collector. To uniquely identify a collector,
 * the experiment and collector's unique identifiers must be specified.
 */
struct OpenSS_Protocol_Collector
{
    /** Unique identifier for the experiment containing this collector. */
    int experiment;

    /** Identifer for this collector within that experiment. */
    int collector;
};



/**
 * Experiment identifier.
 *
 * Names a specific experiment. To uniquely identify an experiment, only the
 * unique identifier of the experiment itself must be specified.
 */
struct OpenSS_Protocol_Experiment
{
    /** Unique identifier for this experiment. */
    int experiment;
};



/**
 * Arbitrary group of experiment identifiers.
 *
 * List holding an arbitrary group of experiment identifiers. No specific
 * relationship is implied between the experiments in a given experiment
 * group.
 */
struct OpenSS_Protocol_ExperimentGroup
{
    /** Experiments in the group. */
    OpenSS_Protocol_Experiment experiments<>;
};



/**
 * File name.
 *
 * Names a single file by providing the full path name of that file. A checksum
 * is also provided, which allows the frontend to verify the availability of the
 * correct file.
 *
 * @note    Currently the exact type of checksum to be used here is left
 *          undefined. It is expected this checksum will be a CRC-64-ISO
 *          or similar.
 */
struct OpenSS_Protocol_FileName
{
    /** Full path name of the file. */
    string path<>;

    /** Checksum calculated on this file. */
    uint64_t checksum;
};



/**
 * Function entry.
 *
 * Describes a single function in a symbol table. Contains the name of the
 * function and a description of the address ranges occupied by the function.
 */
struct OpenSS_Protocol_FunctionEntry
{
    /** Name of this function. */
    string name<>;
    
    /** Address bitmaps describing the occupied address ranges. */
    OpenSS_Protocol_AddressBitmap bitmaps<>;
};



/**
 * Job description entry.
 *
 * A single host/pid pair specifying one process within a job.
 */
struct OpenSS_Protocol_JobEntry
{
    /** Name of the host on which the process resides. */
    string host<>;

    /** Identifier of the process. */
    int64_t pid;
};



/**
 * Job description.
 *
 * List of host/pid pairs describing the processes in a job. No specific
 * relationship is implied by the term "job". This type exists merely as
 * a convenience when passing around a list of host/pid pairs. E.g. when
 * describing the processes in an MPI application.
 */
struct OpenSS_Protocol_Job
{
    /** Entries in the job. */
    OpenSS_Protocol_JobEntry entries<>;
};



/**
 * Statement entry.
 *
 * Describes a single source statement in a symbol table. Contains the name
 * of the source file, line number, and column number of the statement. Also
 * contains a description of the address ranges occupied by the statement.
 */
struct OpenSS_Protocol_StatementEntry
{
    /** Name of this statement's source file. */
    OpenSS_Protocol_FileName path;

    /** Line number of this statement. */
    int line;

    /** Column number of this statement. */
    int column;

    /** Address bitmaps describing the occupied address ranges. */
    OpenSS_Protocol_AddressBitmap bitmaps<>;
};



/**
 * Thread name.
 *
 * Names a single thread of code execution. To uniquely identify a thread,
 * the host name and process identifier must be specified. A POSIX thread
 * identifier must also be specified if a specific thread in a process is
 * being named.
 */
struct OpenSS_Protocol_ThreadName
{
    /** Unique identifier for the experiment containing this thread. */
    int experiment;

    /** Name of the host on which this thread is located. */
    string host<>;

    /** Identifier of the process containing this thread. */
    int64_t pid;

    /**
     * Boolean "true" if this name refers to a specified thread in the
     * process and thus includes a POSIX thread identifier, or "false"
     * if the main thread of the process is being named and the POSIX
     * thread identifier should be considered invalid.
     */
    bool has_posix_tid;

    /** POSIX identifier of this thread. */
    int64_t posix_tid;
};



/**
 * Arbitrary group of thread names.
 *
 * List holding an arbitrary group of thread names. No specific relationship
 * is implied between the threads named by a given thread name group. Used to
 * provide a way of applying operations to a group of threads as a whole rather
 * than each individually.
 */
struct OpenSS_Protocol_ThreadNameGroup
{
    /** Names of the threads in the group. */
    OpenSS_Protocol_ThreadName names<>;
};



/**
 * Thread state enumeration.
 *
 * Enumeration defining the state of a thread. This may not enumerate
 * all the possible states in which a thread may find itself on a given
 * system. It contains only those states that are generally of interest
 * to a performance tool.
 */
enum OpenSS_Protocol_ThreadState
{
    Disconnected,  /**< Thread isn't connected (may not even exist). */
    Connecting,    /**< Thread is being connected. */
    Nonexistent,   /**< Thread doesn't exist. */
    Running,       /**< Thread is active and running. */
    Suspended,     /**< Thread has been temporarily suspended. */
    Terminated     /**< Thread has terminated. */
};



/**
 * Time.
 *
 * All time values are represented using a single 64-bit unsigned integer.
 * These integers are interpreted as the number of nanoseconds that have
 * passed since midnight (00:00) Coordinated Universal Time (UTC), on
 * Janaury 1, 1970. This system gives nanosecond resolution for representing
 * times while not running out the clock until sometime in the year 2554.
 */
typedef uint64_t OpenSS_Protocol_Time;



/**
 * Message tags.
 *
 * Integer tag values, associated with each message, that are used to determine
 * that message's type (and associated data structure).
 *
 * @note    All of the following message tags must have a value larger than
 *          FIRST_APPL_TAG as specified in "MRNet.h" (currently "200"). Since
 *          this file is meant to stand alone, it doesn't include "MRNet.h"
 *          directly but implicitly enforces this criterion instead.
 */
%
%#define OPENSS_PROTOCOL_TAG_ESTABLISH_UPSTREAM                 ((int)1000)
%#define OPENSS_PROTOCOL_TAG_SHUTDOWN_BACKENDS                  ((int)1001)
%#define OPENSS_PROTOCOL_TAG_READY_TO_SHUTDOWN			((int)1002)
%
%#define OPENSS_PROTOCOL_TAG_ATTACH_TO_THREADS                  ((int)1100)
%#define OPENSS_PROTOCOL_TAG_ATTACHED_TO_THREADS                ((int)1101)
%#define OPENSS_PROTOCOL_TAG_CHANGE_THREADS_STATE               ((int)1102)
%#define OPENSS_PROTOCOL_TAG_CREATE_PROCESS                     ((int)1103)
%#define OPENSS_PROTOCOL_TAG_CREATED_PROCESS                    ((int)1104)
%#define OPENSS_PROTOCOL_TAG_DETACH_FROM_THREADS                ((int)1105)
%#define OPENSS_PROTOCOL_TAG_EXECUTE_NOW                        ((int)1106)
%#define OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT           ((int)1107)
%#define OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF                ((int)1108)
%#define OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER                 ((int)1109)
%#define OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING                  ((int)1110)
%#define OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE               ((int)1111)
%#define OPENSS_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE               ((int)1112)
%#define OPENSS_PROTOCOL_TAG_GLOBAL_JOB_VALUE                   ((int)1113)
%#define OPENSS_PROTOCOL_TAG_GLOBAL_STRING_VALUE                ((int)1114)
%#define OPENSS_PROTOCOL_TAG_INSTRUMENTED			((int)1115)
%#define OPENSS_PROTOCOL_TAG_LOADED_LINKED_OBJECT               ((int)1116)
%#define OPENSS_PROTOCOL_TAG_REPORT_ERROR                       ((int)1117)
%#define OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER                 ((int)1118)
%#define OPENSS_PROTOCOL_TAG_STDERR                             ((int)1119)
%#define OPENSS_PROTOCOL_TAG_STDIN                              ((int)1120)
%#define OPENSS_PROTOCOL_TAG_STDOUT                             ((int)1121)
%#define OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT              ((int)1122)
%#define OPENSS_PROTOCOL_TAG_SYMBOL_TABLE                       ((int)1123)
%#define OPENSS_PROTOCOL_TAG_THREADS_STATE_CHANGED              ((int)1124)
%#define OPENSS_PROTOCOL_TAG_UNINSTRUMENT                       ((int)1125)
%#define OPENSS_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT             ((int)1126)
%#define OPENSS_PROTOCOL_TAG_MPI_STARTUP                        ((int)1127)
%
%#define OPENSS_PROTOCOL_TAG_PERFORMANCE_DATA                   ((int)10000)



/**
 * Attach to threads.
 *
 * Issued by the frontend to request the specified threads be attached.
 */
struct OpenSS_Protocol_AttachToThreads
{
    /** Threads to be attached. */
    OpenSS_Protocol_ThreadNameGroup threads;
};



/**
 * Attached to threads.
 *
 * Issued by a backend to indicate the specified threads were attached.
 *
 * @note    This message is always sent in response to an AttachToThreads
 *          request. But it also sent by a backend when a thread has been
 *          automatically attached, e.g. to a fork(), pthread_create(),
 *          etc.
 */
struct OpenSS_Protocol_AttachedToThreads
{
    /** Threads that were attached. */
    OpenSS_Protocol_ThreadNameGroup threads;
};



/**
 * Change state of threads.
 *
 * Issued by the frontend to request that the current state of every thread
 * in the specified group be changed to the specified value. Used to, for
 * example, suspend threads that were previously running.
 */
struct OpenSS_Protocol_ChangeThreadsState
{
    /** Threads whose state should be changed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Change the threads to this state. */
    OpenSS_Protocol_ThreadState state;
};



/**
 * Create a thread.
 *
 * Issued by the frontend to request the specified thread be created as a new
 * process to execute the specified command. The command is created with the
 * specified initial environment and the process is created in a suspended
 * state.
 */
struct OpenSS_Protocol_CreateProcess
{
    /** Thread to be created (only the host name is actually used). */
    OpenSS_Protocol_ThreadName thread;

    /** Command to be executed. */
    string command<>;

    /** Environment in which to execute the command. */
    OpenSS_Protocol_Blob environment;
};



/**
 * Created a process.
 *
 * Issued by a backend to indicate the specified process was created.
 *
 * @note    Sent only in response to a CreateProcess request.
 */
struct OpenSS_Protocol_CreatedProcess
{
    /** Original thread as specfied in the CreateProcess request. */
    OpenSS_Protocol_ThreadName original_thread;

    /** Process that was created. */
    OpenSS_Protocol_ThreadName created_thread;
};



/**
 * Detach from threads.
 *
 * Issued by the frontend to request the specified threads be detached.
 */
struct OpenSS_Protocol_DetachFromThreads
{
    /** Threads to be detached. */
    OpenSS_Protocol_ThreadNameGroup threads;
};



/**
 * Execute a library function now.
 *
 * Issued by the frontend to request the immediate execution of the specified
 * library function in the specified threads. Used by collectors to execute
 * functions in their runtime library.
 */
struct OpenSS_Protocol_ExecuteNow
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    OpenSS_Protocol_Collector collector;
    
    /**
     * Boolean "true" if the floating-point registers should NOT be saved before
     * executing the library function, or "false" if they should be saved.
     */
    bool disable_save_fpr;

    /** Name of the library function to be executed. */
    string callee<>;

    /** Blob argument to the function. */
    OpenSS_Protocol_Blob argument;
};



/**
 * Execute a library function at another function's entry or exit.
 *
 * Issued by the frontend to request the execution of the specified library
 * function every time another function's entry or exit is executed in the
 * specified threads. Used by collectors to execute functions in their
 * runtime library.
 */
struct OpenSS_Protocol_ExecuteAtEntryOrExit
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    OpenSS_Protocol_Collector collector;

    /**
     * Name of the function at whose entry/exit
     * the library function should be executed.
     */
    string where<>;

    /**
     * Boolean "true" if instrumenting function's entry
     * point, or "false" if function's exit point.
     */
    bool at_entry;

    /** Name of the library function to be executed. */
    string callee<>;

    /** Blob argument to the function. */
    OpenSS_Protocol_Blob argument;
};



/**
 * Execute a library function in place of another function.
 *
 * Issued by the frontend to request the execution of the specified library
 * function in place of another function every other time that other function
 * is called. Used by collectors to create wrappers around functions for the
 * purposes of gathering performance data on their execution.
 */
struct OpenSS_Protocol_ExecuteInPlaceOf
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    OpenSS_Protocol_Collector collector;

    /** Name of the function to be replaced with the library function. */
    string where<>;

    /** Name of the library function to be executed. */
    string callee<>;
};



/**
 * Get value of an integer global variable from a thread.
 *
 * Issued by the frontend to request the current value of a signed integer
 * global variable within the specified thread. Used to extract certain
 * types of data, such as MPI job identifiers, from a process.
 */
struct OpenSS_Protocol_GetGlobalInteger
{
    /** Thread from which the global variable value should be retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Get value of a string global variable from a thread.
 *
 * Issued by the frontend to request the current value of a character string
 * global variable within the specified thread. Used to extract certain types
 * of data, such as MPI job identifiers, from a process.
 */
struct OpenSS_Protocol_GetGlobalString
{
    /** Thread from which the global variable value should be retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Get value of the MPICH process table from a thread.
 *
 * Issued by the frontend to request the current value of the MPICH process
 * table within the specified thread. Used to obtain this information for
 * the purposes of attaching to an entire MPI job.
 */
struct OpenSS_Protocol_GetMPICHProcTable
{
    /** Thread from which the MPICH process table should be retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Value of an integer global variable.
 *
 * Issued by a backend to return the current value of a signed integer
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetGlobalInteger request.
 */
struct OpenSS_Protocol_GlobalIntegerValue
{
    /** Thread from which the global variable value was retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;

    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    int64_t value;
};



/**
 * Value of a job description variable.
 *
 * Issued by a backend to return the current value of a job description
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetMPICHProcTable request.
 */
struct OpenSS_Protocol_GlobalJobValue
{
    /** Thread from which the global variable value was retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;
    
    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    OpenSS_Protocol_Job value;
};



/**
 * Value of a string global variable.
 *
 * Issued by a backend to return the current value of a character string
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetGlobalString request.
 */
struct OpenSS_Protocol_GlobalStringValue
{
    /** Thread from which the global variable value was retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;

    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    string value<>;
};



/**
 * Threads have been instrumented.
 *
 * Issued by the backend to indicate that the specified collector has placed
 * instrumentation in the specified threads.
 *
 * @note    Sent only when the backend automatically applies instrumentation
 *          to newly created threads. Not to report completion of frontend-
 *          requested instrumentation.
 */
struct OpenSS_Protocol_Instrumented
{
    /** Threads to which instrumentation was applied. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Collector which is instrumenting. */
    OpenSS_Protocol_Collector collector;
};



/**
 * Linked object has been loaded.
 *
 * Issued by a backend to indicate that the specified linked object has
 * been loaded into the address space of the specified threads. Includes
 * the time at which the load occurred as well as a description of what
 * was loaded.
 *
 * @note    This message is always sent multiple times in conjunction with
 *          an AttachedToThreads message. But it is also sent by a backend
 *          when a linked object is loaded due to a dlopen().
 */
struct OpenSS_Protocol_LoadedLinkedObject
{
    /** Threads which loaded the linked object. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Time at which the linked object was loaded. */
    OpenSS_Protocol_Time time;
    
    /** Address range at which this linked object was loaded. */
    OpenSS_Protocol_AddressRange range;

    /** Name of the linked object's file. */
    OpenSS_Protocol_FileName linked_object;

    /**
     * Boolean "true" if this linked object is an
     * executable, or "false" otherwise.
     */
    bool is_executable;
};



/**
 * Report an error.
 *
 * Issued by a backend to indicate that an error has occured.
 */
struct OpenSS_Protocol_ReportError
{
    /** Text describing the error that has occured. */
    string text<>;
};



/**
 * Set value of an integer global variable in a thread.
 *
 * Issued by the frontend to request a change to the current value of a signed
 * integer global variable within the specified thread. Used to modify certain
 * values, such as MPI debug gates, in a process.
 */
struct OpenSS_Protocol_SetGlobalInteger
{
    /** Thread in which the global variable value should be set. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being set. */
    string global<>;

    /** New value of that variable. */
    int64_t value;
};



/**
 * Standard error stream.
 *
 * Issued by a backend to indicate that data was received on a created process'
 * standard error stream. Includes the data and the thread that generated that
 * data.
 */
struct OpenSS_Protocol_StdErr
{
    /** Thread which generated the data on its standard error stream. */
    OpenSS_Protocol_ThreadName thread;

    /** Data that was generated. */
    OpenSS_Protocol_Blob data;
};



/**
 * Standard input stream.
 *
 * Issued by the frontend to request that data be written to the standard input
 * stream of a created process. Includes the data and the thread that should
 * receive that data.
 */
struct OpenSS_Protocol_StdIn
{
    /** Thread whose standard input stream should be written. */
    OpenSS_Protocol_ThreadName thread;

    /** Data to be written. */
    OpenSS_Protocol_Blob data;
};



/**
 * Standard output stream.
 *
 * Issued by a backend to indicate that data was received on a created process'
 * standard output stream. Includes the data and the thread that generated that
 * data.
 */
struct OpenSS_Protocol_StdOut
{
    /** Thread which generated the data on its standard output stream. */
    OpenSS_Protocol_ThreadName thread;

    /** Data that was generated. */
    OpenSS_Protocol_Blob data;
};



/**
 * Stop at a function's entry or exit.
 *
 * Issued by the frontend to request a stop every time the specified function's
 * entry or exit is executed in the specified threads. Used by the framework
 * to implement MPI job creation.
 */
struct OpenSS_Protocol_StopAtEntryOrExit
{
    /** Threads which should be stopped. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Name of the function at whose entry/exit the stop should occur. */
    string where<>;

    /**
     * Boolean "true" if instrumenting function's entry
     * point, or "false" if function's exit point.
     */
    bool at_entry;
};



/**
 * Symbol table.
 *
 * Issued by a backend to provide the frontend with the symbol table for a
 * single linked object.
 *
 * @note    All addresses in the symbol table are relative to the base load
 *          address of the linked object, allowing this data to be easily
 *          reused for multiple threads which may have loaded the linked
 *          object at differing addresses.
 */
struct OpenSS_Protocol_SymbolTable
{
    /** Experiments referencing the linked object. */
    OpenSS_Protocol_ExperimentGroup experiments;

    /** Name of the linked object's file. */
    OpenSS_Protocol_FileName linked_object;

    /** Functions contained in this linked object. */
    OpenSS_Protocol_FunctionEntry functions<>;

    /** Statements contained in this linked object. */
    OpenSS_Protocol_StatementEntry statements<>;
};



/**
 * Thread's state has changed.
 *
 * Issued by a backend to indicate that the current state of every thread
 * in the specified group has changed to the specified value.
 *
 * @note    While this message is sent in response to a ChangeThreadState
 *          request, it is also sent by a backend when thread states change
 *          asynchronously due to thread termination, etc.
 */
struct OpenSS_Protocol_ThreadsStateChanged
{
    /** Threads whose state has changed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** State to which these threads have changed. */
    OpenSS_Protocol_ThreadState state;
};



/**
 * Remove instrumentation from threads.
 *
 * Issued by the frontend to request the removal of all instrumentation
 * associated with the specified collector from the specified threads. Used
 * by collectors to indicate when they are done using any instrumentation
 * they have placed in threads.
 */
struct OpenSS_Protocol_Uninstrument
{
    /** Threads from which instrumentation should be removed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Collector which is removing instrumentation. */
    OpenSS_Protocol_Collector collector;
};



/**
 * Linked object has been unloaded.
 *
 * Issued by a backend to indicate that the specified linked object has
 * been unloaded from the address space of the specified threads. Includes
 * the time at which the unload occurred as well as a description of what
 * was unloaded.
 *
 * @note    This message is sent only when a linked object is unloaded
 *          due to a dlclose() in the threads.
 */
struct OpenSS_Protocol_UnloadedLinkedObject
{
    /** Threads which unloaded the linked object. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Time at which the linked object was unloaded. */
    OpenSS_Protocol_Time time;
    
    /** Name of the linked object's file. */
    OpenSS_Protocol_FileName linked_object;
};

struct OpenSS_Protocol_MPIStartup
{
    /** Threads which are in MPI startup. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /**
     * Boolean "true" if threads are in MPI startup.
     */
    bool in_mpi_startup;
};
