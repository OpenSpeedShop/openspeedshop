/*******************************************************************************
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Specification of the client/server communication protocol.
 *
 */



/**
 * Thread name.
 *
 * Names a single thread of code execution. To uniquely identify a thread,
 * the host name, process identifier, and POSIX thread identifier must all
 * be specified.
 */
struct OpenSS_Protocol_ThreadName
{
    /** Name of the host on which this thread is located.  */
    string host<>;

    /** Identifier of the process containing this thread. */
    int64_t pid;

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
    Running,    /**< Thread is active and running. */
    Suspended,  /**< Thread has been temporarily suspended. */
    Terminated  /**< Thread has terminated. */
};



/**
 * Range of memory addresses.
 *
 * A single, closed-ended, range of memory addresses: [begin, end]. Used in
 * several different places for representing a single contiguous portion of
 * an address space, as occupied by a DSO, compilation unit, function, etc.
 */
struct OpenSS_Protocol_AddressRange
{
    uint64_t begin;  /**< Beginning of the address range. */
    uint64_t end;    /**< End of the address range. */
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
    opaque bitmap<>;
};



/**
 * File name.
 *
 * Names a single file by providing the full path name of that file. A checksum
 * is also provided, which allows the client to verify the availability of the
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
    opaque checksum<>;
};



/**
 * Address space entry.
 *
 * Describes a single linked object loaded into the address space of a thread.
 * Contains the address range at which the linked object is loaded into the
 * thread, the name of the linked object's file, and a flag indicating if this
 * linked object is an executable.
 */
struct OpenSS_Protocol_AddressSpaceEntry
{
    /** Address range at which this linked object is loaded. */
    OpenSS_Protocol_AddressRange range;

    /** Name of the linked object's file. */
    OpenSS_Protocol_FileName linked_object;

    /** Flag indicating if this linked object is an executable. */
    bool is_executable;
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
 * Job description entry.
 *
 * A single host/pid pair specifying one process within a job.
 */
struct OpenSS_Protocol_JobEntry
{
    /** Name of the host on which the process resides. */
    string host<HOST_NAME_MAX>;

    /** Identifier of the process. */
    uint32_t pid;
};



/**
 * Message tags.
 *
 * ...
 */

#define OPENSS_PROTOCOL_TAG_DATA                               ((int)0)

#define OPENSS_PROTOCOL_TAG_ATTACH_TO_THREAD                   ((int)1)
#define OPENSS_PROTOCOL_TAG_DETACH_FROM_THREAD                 ((int)2)
#define OPENSS_PROTOCOL_TAG_CREATE_PROCESS                     ((int)3)
#define OEPNSS_PROTOCOL_TAG_ATTACHED_TO_THREAD                 ((int)4)
#define OPENSS_PROTOCOL_TAG_ADDRESS_SPACE_CHANGED              ((int)5)

#define OPENSS_PROTOCOL_TAG_SYMBOL_TABLE                       ((int)6)
#define OPENSS_PROTOCOL_TAG_CHANGE_THREAD_STATE                ((int)7)
#define OPENSS_PROTOCOL_TAG_THREAD_STATE_CHANGED               ((int)8)
#define OPENSS_PROTOCOL_TAG_EXECUTE_NOW                        ((int)9)
#define OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT           ((int)10)

#define OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF                ((int)11)
#define OPENSS_PROTOCOL_TAG_UNINSTRUMENT                       ((int)12)
#define OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT              ((int)13)
#define OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER                 ((int)14)
#define OPENSS_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE               ((int)15)

#define OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING                  ((int)16)
#define OPENSS_PROTOCOL_TAG_GLOBAL_STRING_VALUE                ((int)17)
#define OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE               ((int)18)
#define OPENSS_PROTOCOL_TAG_JOB                                ((int)19)
#define OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER                 ((int)20)

#define OPENSS_PROTOCOL_TAG_REPORT_ERROR                       ((int)21)



/**
 * Attach to threads.
 *
 * Issued by the client to request the specified threads be attached.
 */
struct OpenSS_Protocol_AttachToThread
{
    /** Threads to be attached. */
    OpenSS_Protocol_ThreadNameGroup threads;
};



/**
 * Detach from threads.
 *
 * Issued by the client to request the specified threads be detached.
 */
struct OpenSS_Protocol_DetachFromThread
{
    /** Threads to be detached. */
    OpenSS_Protocol_ThreadNameGroup threads;
};



/**
 * Create a thread.
 *
 * Creates the specified thread as a new process to execute the the specified
 * command. The command is created with the same initial environment as when
 * the tool was started. The process is created in a suspended state.
 */
struct OpenSS_Protocol_CreateProcess
{
    /** Thread to be created (only the host name is actually used). */
    OpenSS_Protocol_ThreadName thread;

    /** Command to be executed. */
    string command<>;

    /** Blob containing the environment in which to execute the command. */
    opaque environment<>;
};



/**
 * ...
 *
 * ...
 */
struct OpenSS_Protocol_AttachedToThread
{
    /* ... */
    int dummy; /* Dummy to temporarily avoid error in rpcgen */
};



/**
 * Address space has changed.
 *
 * Issued by the server to indicate that the address space of the specified
 * thread has changed. The time at which the change occured, and a description
 * of the new address space, is included.
 *
 * @note    This message is always sent in conjunction with an AttachedToThread
 *          response. But it is also sent by the server when the address space
 *          changes asynchronously due to a dlopen(), dlclose(), etc.
 */
struct OpenSS_Protocol_AddressSpaceChanged
{
    /** Thread whose address space has changed. */
    OpenSS_Protocol_ThreadName thread;

    /** Time at which the change occured. */
    uint64_t time;

    /** Entries describing the new address space of the thread. */
    OpenSS_Protocol_AddressSpaceEntry entries<>;
};



/**
 * Symbol table.
 *
 * Issued by the server to provide the client with the symbol table for a
 * single linked object.
 *
 * @note    All addresses in the symbol table are relative to the base load
 *          address of the linked object, allowing this data to be easily
 *          reused for multiple threads which may have loaded the linked
 *          object at differing addresses.
 */
struct OpenSS_Protocol_SymbolTable
{
    /** Name of the linked object's file. */
    OpenSS_Protocol_FileName linked_object;

    /** Functions contained in this linked object. */
    OpenSS_Protocol_FunctionEntry functions<>;

    /** Statements contained in this linked object. */
    OpenSS_Protocol_StatementEntry statements<>;
};



/**
 * Change state of threads.
 *
 * Issued by the client to request that the current state of every thread
 * in the specified group be changed to the specified value. Used to, for
 * example, suspend threads that were previously running.
 */
struct OpenSS_Protocol_ChangeThreadState
{
    /** Threads whose state should be changed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Change the threads to this state. */
    OpenSS_Protocol_ThreadState state;
};



/**
 * Thread states have changed.
 *
 * Issued by the server to indicate that the current state of every thread
 * in the specified group has changed to the specified value.
 *
 * @note    While this message is sent in response to a ChangeThreadState
 *          request, it is also sent by the server when thread states change
 *          asynchronously due to thread termination, etc.
 */
struct OpenSS_Protocol_ThreadStateChanged
{
    /** Threads whose state has changed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** State to which these threads have changed. */
    OpenSS_Protocol_ThreadState state;
};



/**
 * Execute a library function now.
 *
 * Issued by the client to request the immediate execution of the specified
 * library function in the specified threads. Used by collectors to execute
 * functions in their runtime library.
 */
struct OpenSS_Protocol_ExecuteNow
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Identifier of the collector requesting the execution. */
    int64_t collector;
    
    /** Name of the library function to be executed. */
    string callee<>;

    /** Blob argument to the function. */
    opaque argument<>;
};



/**
 * Execute a library function at another function's entry or exit.
 *
 * Issued by the client to request the execution of the specified library
 * function every time another function's entry or exit is executed in the
 * specified threads. Used by collectors to execute functions in their
 * runtime library.
 */
struct OpenSS_Protocol_ExecuteAtEntryOrExit
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Identifier of the collector requesting the execution. */
    int64_t collector;

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
    opaque argument<>;    
};



/**
 * Execute a library function in place of another function.
 *
 * Issued by the client to request the execution of the specified library
 * function in place of another function every other time that other function
 * is called. Used by collectors to create wrappers around functions for the
 * purposes of gathering performance data on their execution.
 */
struct OpenSS_Protocol_ExecuteInPlaceOf
{
    /** Threads in which the function should be executed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Identifier of the collector requesting the execution. */
    int64_t collector;

    /** Name of the function to be replaced with the library function. */
    string where<>;

    /** Name of the library function to be executed. */
    string callee<>;
};



/**
 * Remove instrumentation from threads.
 *
 * Issued by the client to request the removal of all instrumentation associated
 * with the specified collector from the specified threads. Used by collectors
 * to indicate when they are done using any instrumentation they have placed in
 * threads.
 */
struct OpenSS_Protocol_Uninstrument
{
    /** Threads from which instrumentation should be removed. */
    OpenSS_Protocol_ThreadNameGroup threads;

    /** Identifier of the collector which is removing instrumentation. */
    int64_t collector;
};



/**
 * Stop at a function's entry or exit.
 *
 * Issued by the client to request a stop every time the specified function's
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
 * Get value of an integer global variable from a thread.
 *
 * Issued by the client to request the current value of a signed integer
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
 * Value of an integer global variable.
 *
 * Issued by the server to return the current value of a signed integer
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

    /** Current value of that variable. */
    int64_t value<>;
};



/**
 * Get value of a string global variable from a thread.
 *
 * Issued by the client to request the current value of a character string
 * global variable within the specified thread. Used to extract certain types
 * of data, such as MPI job identifiers, from a process.
 *
 * @note    Sent only in response to a GetGlobalString request.
 */
struct OpenSS_Protocol_GetGlobalString
{
    /** Thread from which the global variable value should be retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Value of a string global variable.
 *
 * Issued by the server to return the current value of a character string
 * global variable within a thread.
 */
struct OpenSS_Protocol_GlobalStringValue
{
    /** Thread from which the global variable value was retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;

    /** Current value of that variable. */
    string value<>;
};



/**
 * Get value of the MPICH process table from a thread.
 *
 * Issued by the client to request the current value of the MPICH process
 * table within the specified thread. Used to obtain this information for
 * the purposes of attaching to an entire MPI job.
 */
struct OpenSS_Protocol_GetMPICHProcTable
{
    /** Thread from which the MPICH process table should be retrieved. */
    OpenSS_Protocol_ThreadName thread;
};



/**
 * Job description.
 *
 * Issued by the server to return a job description. Contains a variable-length
 * array of host/pid pairs, one for each process in the job.
 *
 * @note    Currently only sent in response to a GetMPICHProcTable request,
 *          but intended to generically describe jobs.
 */
struct OpenSS_Protocol_Job
{
    /** Thread from which the job description was retrieved. */
    OpenSS_Protocol_ThreadName thread;

    /** List of processes in this job. */
    OpenSS_Protocol_JobEntry processes<>;
};



/**
 * Set value of an integer global variable in a thread.
 *
 * Issued by the client to request a change to the current value of a signed
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
 * ...
 *
 * ...
 */
struct OpenSS_Protocol_ReportError
{
    /* ... */
    int dummy; /* Dummy to temporarily avoid error in rpcgen */
};
