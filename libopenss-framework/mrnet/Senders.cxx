////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the Callbacks namespace.
 *
 */

#include "Blob.hxx"
#include "CollectorImpl.hxx"
#include "Frontend.hxx"
#include "Protocol.h"
#include "Senders.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



namespace {



    /**
     * Convert string for protocol use.
     *
     * Converts the specified C++ string to a C character array as used in
     * protocol messages.
     *
     * @note    The caller assumes responsibility for releasing the C character
     *          array when it is no longer needed.
     *
     * @param in      C++ string to be converted.
     * @retval out    C character array to hold the results.
     */
    void convert(const std::string& in, char*& out)
    {
	if(!in.empty()) {
	    out = new char[in.size()];
	    strcpy(out, in.c_str());
	}
	else
	    out = NULL;
    }
    


    /**
     * Convert collector for protocol use.
     *
     * Converts the identifying information for the specified framework
     * collector object to the structure used in protocol messages.
     *
     * @param in      Collector to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const Collector& in, OpenSS_Protocol_Collector& out)
    {
	out.experiment = CollectorImpl::getExperimentId(in);
	out.collector = CollectorImpl::getCollectorId(in);
    }



    /**
     * Convert thread group for protocol use.
     *
     * Converts the identifying information for the specified framework
     * thread group object to the structure used in protocol messages.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      Thread group to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const ThreadGroup& in, OpenSS_Protocol_ThreadNameGroup& out)
    {
	// Is the thread group non-empty?
	if(!in.empty()) {

	    // Allocate an appropriately sized array of entries
	    out.names.names_len = in.size();
	    out.names.names_val = new OpenSS_Protocol_ThreadName[in.size()];
	    
	    // Iterate over each thread of this group
	    OpenSS_Protocol_ThreadName* ptr = out.names.names_val;
	    for(ThreadGroup::const_iterator
		    i = in.begin(); i != in.end(); ++i, ++ptr) {
		
		// Get the identifying information for this thread
		std::string host = i->getHost();
		pid_t pid = i->getProcessId();
		std::pair<bool, pthread_t> posix_tid = i->getPosixThreadId();
		
		// Fill in the entry fields
		convert(host, ptr->host);
		ptr->pid = pid;
		ptr->posix_tid = posix_tid.first ? posix_tid.second : 0;
		
	    }
	    
	}
	else {
	    out.names.names_len = 0;
	    out.names.names_val = NULL;
	}
    }



    /**
     * Convert thread for protocol use.
     *
     * Converts the identifying information for the specified framework
     * thread object to the structure used in protocol messages.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      Thread to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const Thread& in, OpenSS_Protocol_ThreadNameGroup& out)
    {
	ThreadGroup threads;
	threads.insert(in);
	return convert(threads, out);
    }



}



/**
 * Attach to threads.
 *
 * ...
 *
 * @param threads    Threads to be attached.
 */
void Senders::attachToThread(const ThreadGroup& threads)
{
}



/**
 * Change state of threads.
 *
 * ...
 *
 * @param threads    Threads whose state should be changed.
 * @param state      Change the threads to this state.
 */
void Senders::changeThreadState(const ThreadGroup& threads,
				const Thread::State& state)
{
}



/**
 * Create a thread.
 *
 * ...
 */
void Senders::createProcess()
{
}



/**
 * Detach from threads.
 *
 * ...
 *
 * @param threads    Threads to be detached.
 */
void Senders::detachFromThread(const ThreadGroup& threads)
{
}



/**
 * Execute a library function now.
 *
 * ...
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Senders::executeNow(const ThreadGroup& threads,
			 const Collector& collector,
			 const std::string& callee,
			 const Blob& argument)
{
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * ...
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function at whose entry/exit the library
 *                     function should be executed.
 * @param at_entry     Boolean "true" if instrumenting the function's entry
 *                     point, or "false" if function's exit point.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Senders::executeAtEntryOrExit(const ThreadGroup& threads,
				   const Collector& collector,
				   const std::string& where,
				   const bool& at_entry,
				   const std::string& callee,
				   const Blob& argument)
{
}



/**
 * Execute a library function in place of another function.
 *
 * ...
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void Senders::executeInPlaceOf(const ThreadGroup& threads,
			       const Collector& collector,
			       const std::string& where,
			       const std::string& callee)
{
}



/**
 * Get value of an integer global variable from a thread.
 *
 * Issue a request to the backends for the current value of a signed integer
 * global variable within the specified thread. Used to extract certain types
 * of data, such as MPI job identifiers, from a process.
 *
 * @param thread    Thread from which the global variable should be retrieved.
 * @param global    Name of global variable whose value is being requested.
 */
void Senders::getGlobalInteger(const Thread& thread, const std::string& global)
{
    // Assemble the message
    OpenSS_Protocol_GetGlobalInteger message;
    OpenSS_Protocol_ThreadNameGroup threads;
    convert(thread, threads);
    message.thread.host = threads.names.names_val[0].host;
    message.thread.pid = threads.names.names_val[0].pid;
    message.thread.posix_tid = threads.names.names_val[0].posix_tid;
    convert(global, message.global);

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalInteger),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
			     blob, threads);

    // TODO: free the allocated memory
}



/**
 * Get value of a string global variable from a thread.
 *
 * Issue a request to the backends for the current value of a character string
 * global variable within the specified thread. Used to extract certain types
 * of data, such as MPI job identifiers, from a process.
 *
 * @param thread    Thread from which the global variable should be retrieved.
 * @param global    Name of global variable whose value is being requested.
 */
void Senders::getGlobalString(const Thread& thread, const std::string& global)
{
    // Assemble the message
    OpenSS_Protocol_GetGlobalString message;
    OpenSS_Protocol_ThreadNameGroup threads;
    convert(thread, threads);
    message.thread.host = threads.names.names_val[0].host;
    message.thread.pid = threads.names.names_val[0].pid;
    message.thread.posix_tid = threads.names.names_val[0].posix_tid;
    convert(global, message.global);

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalString),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
			     blob, threads);

    // TODO: free the allocated memory
}



/**
 * Get value of the MPICH process table from a thread.
 *
 * Issue a request to the backends for the current value of the MPICH process
 * table within the specified thread. Used to obtain this information for the
 * purposes of attaching to an entire MPI job.
 *
 * @param thread    Thread from which the MPICH process table should be
 *                  retrieved.
 */
void Senders::getMPICHProcTable(const Thread& thread)
{
    // Assemble the message
    OpenSS_Protocol_GetMPICHProcTable message;
    OpenSS_Protocol_ThreadNameGroup threads;
    convert(thread, threads);
    message.thread.host = threads.names.names_val[0].host;
    message.thread.pid = threads.names.names_val[0].pid;
    message.thread.posix_tid = threads.names.names_val[0].posix_tid;
    
    // Encode the message into a blob
    Blob blob(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetMPICHProcTable),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
			     blob, threads);


    // TODO: free the allocated memory
}



/**
 * Set value of an integer global variable in a thread.
 *
 * Issue a request to the backends for a change to the current value of a
 * signed integer global variable within the specified thread. Used to modify
 * certain values, such as MPI debug gates, in a process.
 *
 * @param thread    Thread in which the global variable value should be set.
 * @param global    Name of global variable whose value is being set.
 * @param value     New value of that variable.
 */
void Senders::setGlobalInteger(const Thread& thread,
			       const std::string& global,
			       const int64_t& value)
{
    // Assemble the message
    OpenSS_Protocol_SetGlobalInteger message;
    OpenSS_Protocol_ThreadNameGroup threads;
    convert(thread, threads);
    message.thread.host = threads.names.names_val[0].host;
    message.thread.pid = threads.names.names_val[0].pid;
    message.thread.posix_tid = threads.names.names_val[0].posix_tid;
    convert(global, message.global);
    message.value = value;
    
    // Encode the message into a blob
    Blob blob(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SetGlobalInteger),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
			     blob, threads);

    // TODO: free the allocated memory
}



/**
 * Stop at a function's entry or exit.
 *
 * Issue a request to the backends for a stop every time the specified
 * function's entry or exit is executed in the specified threads. Used
 * by the framework to implement MPI job creation.
 *
 * @param threads     Threads which should be stopped.
 * @param where       Name of the function at whose entry/exit the stop
 *                    should occur.
 * @param at_entry    Boolean "true" if instrumenting function's entry
 *                    point or "false" if function's exit point.
 */
void Senders::stopAtEntryOrExit(const ThreadGroup& threads,
				const std::string& where,
				const bool& at_entry)
{
    // Assemble the message
    OpenSS_Protocol_StopAtEntryOrExit message;
    convert(threads, message.threads);
    convert(where, message.where);
    message.at_entry = at_entry;

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StopAtEntryOrExit),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
			     blob, message.threads);

    // TODO: free the allocated memory
}



/**
 * Remove instrumentation from threads.
 *
 * Issue a request to the backends for the removal of all instrumentation
 * associated with the specified collector from the specified threads. Used
 * by collectors to indicate when they are done using any instrumentation
 * they have placed in threads.
 *
 * @param threads      Threads from which instrumentation should be removed.
 * @param collector    Collector which is removing instrumentation
 */
void Senders::uninstrument(const ThreadGroup& threads,
			   const Collector& collector)
{
    // Assemble the message
    OpenSS_Protocol_Uninstrument message;
    convert(threads, message.threads);
    convert(collector, message.collector);
    
    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Uninstrument),
	&message
	);
    
    // Send this message to the appropriate backends
    Frontend::sendToBackends(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
			     blob, message.threads);

    // TODO: free the allocated memory
}
