////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
#include "Backend.hxx"
#include "Callbacks.hxx"
#include "Collector.hxx"
#include "Dyninst.hxx"
#include "InstrumentationTable.hxx"
#include "Path.hxx"
#include "Protocol.h"
#include "Senders.hxx"
#include "StdStreamPipes.hxx"
#include "ThreadName.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Utility.hxx"

#include <BPatch.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace OpenSpeedShop::Framework;



namespace {

    bool is_in_mpi_startup = false;

    /**
     * Filter threads by local host.
     *
     * Returns the subset of threads from the specified thread name group
     * that are on this local host. An empty subset is returned if none of
     * the threads are on this local host.
     *
     * @param threads    Threads to be filtered.
     * @return           Subset of these threads on this local host.
     */
    ThreadNameGroup filterByLocalHost(const ThreadNameGroup& threads)
    {
	ThreadNameGroup filtered;

	// Get the canonical local host name
	std::string local_host = getCanonicalName(getLocalHost());

	// Iterate over each thread
	for(ThreadNameGroup::const_iterator
		i = threads.begin(); i != threads.end(); ++i)
	    if(getCanonicalName(i->getHost()) == local_host)
		filtered.insert(*i);
	
	// Return the filtered threads to the caller
	return filtered;
    }



}



/**
 * Attach to threads.
 *
 * Callback function called by the backend message pump when a request to
 * attach to one or more threads is received. Instructs Dyninst to attach
 * to any threads that weren't already attached, then sends the frontend
 * a list of the threads that were attached and the symbol information
 * for those threads.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::attachToThreads(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_AttachToThreads message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachToThreads),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message)
		<< " is_in_mpi_startup = " << is_in_mpi_startup;
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be attached
    ThreadNameGroup threads_to_attach = filterByLocalHost(message.threads);
    ThreadNameGroup threads_attached;
    for(ThreadNameGroup::const_iterator
	    i = threads_to_attach.begin(); i != threads_to_attach.end(); ++i) {
	
	// Has this thread already been attached?
	if(ThreadTable::TheTable.getPtr(*i) != NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "attachToThreads(): Thread " << toString(*i) 
		       << " is already attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}

	// Has this thread already been attached in a different experiment?
	BPatch_thread* thread = ThreadTable::getPtrDirectly(*i);
	if(thread != NULL) {
	    
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "attachToThreads(): Thread " << toString(*i)
		       << " is already attached in another experiment." 
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	    // Add this thread to the thread table and group of attached threads
	    ThreadName name(i->getExperiment(), *thread);
	    ThreadTable::TheTable.addThread(name, thread);
	    threads_attached.insert(name);
	    
	    continue;
	}

	// Otherwise attach to the process containing the specified thread
	BPatch* bpatch = BPatch::getBPatch();
	Assert(bpatch != NULL);

        BPatch_process* process = NULL;

#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
        process = BPatch::bpatch->processAttach("", i->getProcessId(), BPatch_normalMode);
	if(process == NULL) {
#else
	thread = bpatch->attachProcess(NULL, i->getProcessId());
	if(thread == NULL) {
#endif


#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "attachToThreads(): Thread " << toString(*i)
		       << " could not be attached. Does it exist?"
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	    // Send the frontend a message indicating the thread doesn't exist
	    ThreadNameGroup threads;
	    threads.insert(*i);
	    Senders::threadsStateChanged(threads, Nonexistent);
	    
	    continue;
	}

#if (DYNINST_MAJOR < 7)
	process = thread->getProcess();
	Assert(process != NULL);
#endif

	
	// Get the list of threads in this process
	BPatch_Vector<BPatch_thread*> threads;
	process->getThreads(threads);
	Assert(!threads.empty());
	
	// Iterate over each thread in this process
	for(int j = 0; j < threads.size(); ++j) {
	    Assert(threads[j] != NULL);

	    // Add this thread to the thread table and group of attached threads
	    ThreadName name(i->getExperiment(), *(threads[j]));
	    ThreadTable::TheTable.addThread(name, threads[j]);
	    threads_attached.insert(name);

#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "attachToThreads(): Thread " << toString(name)
		       << " is attached."
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
		    
	}
	
    }

    // Were any threads actually attached?
    if(!threads_attached.empty()) {
    
	// Send the frontend the list of threads that were attached
	Senders::attachedToThreads(threads_attached);
	
	// Iterate over each thread that was attached
	for(ThreadNameGroup::const_iterator 
		i = threads_attached.begin(); i != threads_attached.end(); ++i)
	    
	    // Send the frontend all the symbol information for this thread
	    if (!is_in_mpi_startup) {
	        Dyninst::sendSymbolsForThread(*i);
	    }
	// Send the frontend a message indicating these threads are suspended
	Senders::threadsStateChanged(threads_attached, Suspended);
    }
}



/**
 * Change state of threads.
 *
 * Callback function called by the backend message pump when a request to
 * change the state of one or more threads is received. Instructs Dyninst
 * to make the requested state changes, waits for them to complete, then
 * sends the frontend the list of threads that were changed (and to what
 * state they were changed).
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::changeThreadsState(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ChangeThreadsState message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ChangeThreadsState),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be changed
    ThreadNameGroup threads_to_change = filterByLocalHost(message.threads);
    ThreadNameGroup threads_changed;
    for(ThreadNameGroup::const_iterator
	    i = threads_to_change.begin(); i != threads_to_change.end(); ++i) {

	// Get the Dyninst thread and process pointers for this thread
	BPatch_thread* thread = ThreadTable::TheTable.getPtr(*i);
	if(thread == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "changeThreadsState(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}
	BPatch_process* process = thread->getProcess();
	Assert(process != NULL);

	// Change the state of the process containing this thread
	bool did_change = false;
	switch(message.state) {
	case Running:
	    did_change = process->continueExecution();
	    break;
	case Suspended:
	    did_change = process->stopExecution();
	    break;
	case Terminated:
	    did_change = process->terminateExecution();
	    break;
	default:
	    break;
	}

	// Did the state of the process change?
	if(did_change) {

	    // Add all of these threads to the group of changed threads
	    ThreadNameGroup threads(i->getExperiment(), *process);
	    threads_changed.insert(threads.begin(), threads.end());
	    
	}
	
    }
    
    // Was the state of any thread actually changed?
    if(!threads_changed.empty()) {
	
	// Send the frontend the list of threads that were changed
	Senders::threadsStateChanged(threads_changed, message.state);
	
    }
}



/**
 * Create a process.
 *
 * Callback function called by the backend message pump when a request to
 * create a process is received. Instructs Dyninst to create the process,
 * then sends the frontend a list of the threads that were created and the
 * symbol information for those threads.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::createProcess(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_CreateProcess message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreateProcess),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Ignore the message if the process isn't being created on this local host
    if(getCanonicalName(ThreadName(message.thread).getHost()) != 
       getCanonicalName(getLocalHost()))
	return;
    
    // Extract the individual arguments from the command
    std::string command = message.command;
    std::vector<std::string> args;
    for(std::string::size_type 
	    i = command.find_first_not_of(' ', 0), next = command.find(' ', i);
	i != std::string::npos;
	i = command.find_first_not_of(' ', next), next = command.find(' ', i)) {
	
	// Extract this argument
	args.push_back(
	    command.substr(i, (next == std::string::npos) ? next : next - i)
	    );
	
    }

    // Extract the individual environment variables from the environment
    std::vector<std::string> env;
    for(const char* i = 
	    reinterpret_cast<char*>(message.environment.data.data_val);
	i < (reinterpret_cast<char*>(message.environment.data.data_val) +
	     message.environment.data.data_len);
	i += strlen(i) + 1) {
	
	// Extract this environment variable
	env.push_back(i);
	
	// Is this the binary search path?
	if(std::string(i).find("PATH=") == 0) {
	    
	    // Replace the executable with its full, normalized, path
	    args[0] = searchForExecutable(&(i[5]), Path(args[0]));
	    
	}
	
    }
	
    // Translate the arguments into an argv-style argument list
    const char** argv = new const char*[args.size() + 1];
    for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
	argv[i] = args[i].c_str();
    argv[args.size()] = NULL;
    
    // Translate the environment into an envp-style environment
    const char** envp = new const char*[env.size() + 1];
    for(std::vector<std::string>::size_type i = 0; i < env.size(); ++i)
	envp[i] = env[i].c_str();
    envp[env.size()] = NULL;

    // Setup the pipes used for accessing the process' standard streams
    SmartPtr<StdStreamPipes> pipes(new StdStreamPipes());
    
    // Create the process
    BPatch* bpatch = BPatch::getBPatch();
    Assert(bpatch != NULL);
    // Ask dyninst to set the process threaded if we load the libpthread library in
#if 0
    bpatch->forceMultithreadCapable();
#endif
    BPatch_process* process =
	bpatch->processCreate(argv[0], argv, envp,
			      pipes->getStdInForCreatedProcess(),
			      pipes->getStdOutForCreatedProcess(),
			      pipes->getStdErrForCreatedProcess());
    
    // Destroy argv-style argument list and envp-style environment
    delete [] argv;
    delete [] envp;
    
    // Check for proper creation of the process
    if(process == NULL) {
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "createProcess(): Process " << args[0]
		   << "  could not be created. Does the executable exist?"
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
	
	// Send the frontend a message indicating the thread doesn't exist
	ThreadNameGroup threads;
	threads.insert(ThreadName(message.thread));
	Senders::threadsStateChanged(threads, Nonexistent);
	
	return;
    }

    // Get the list of threads in this process
    BPatch_Vector<BPatch_thread*> threads;
    process->getThreads(threads);
    Assert(!threads.empty());
    
    // Iterate over each thread in this process
    ThreadNameGroup threads_created;
    for(int j = 0; j < threads.size(); ++j) {
	// FIXME DPM: due to unknown race condition it is possible
	// that a thread in this list may be NULL. Just skip it.
	// This allows the threads testcase to progress past this point.
	//Assert(threads[j] != NULL);
	if (threads[j] == NULL) {
	    continue;
	}
	
	// Add this thread to the thread table and group of attached threads
	ThreadName name(ThreadName(message.thread).getExperiment(), 
			*(threads[j]));
	ThreadTable::TheTable.addThread(name, threads[j], pipes);
	threads_created.insert(name);
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "createProcess(): Thread " << toString(name)
		   << " was created."
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
	
    }
    
    // Were any threads actually created?
    if(!threads_created.empty()) {

	// Send the frontend the process that was created
	Senders::createdProcess(ThreadName(message.thread),
				ThreadName(message.thread.experiment,
					   getCanonicalName(getLocalHost()),
					   process->getPid(),
					   std::make_pair(false, 0)));
    	
	// Send the frontend the list of threads that were created
	Senders::attachedToThreads(threads_created);
	
	// Iterate over each thread that was created
	for(ThreadNameGroup::const_iterator 
		i = threads_created.begin(); i != threads_created.end(); ++i) {
	    
	    // Send the frontend all the symbol information for this thread
	    // If we are in the mpi startup phase we are not even close to collecting
	    // any data. So we will not need symbols for these threads.

	    if (!is_in_mpi_startup) {
	        Dyninst::sendSymbolsForThread(*i);
	    }
	}
	
	// Send the frontend a message indicating these threads are suspended
	Senders::threadsStateChanged(threads_created, Suspended);
	
    }
}



/**
 * Detach from threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::detachFromThreads(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_DetachFromThreads message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_DetachFromThreads),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Execute a library function now.
 *
 * Callback function called by the backend message pump when a request to
 * execute a library function in one or more threads is received. Passes the
 * request (for each thread individually) on to the instrumentation table,
 * which instructs Dyninst to perform the actual instrumentation (and keeps
 * track of the instrumentation).
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeNow(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteNow message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteNow),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be instrumented
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "executeNow(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}

	// Pass the request on to the instrumentation table
	InstrumentationTable::TheTable.
	    addExecuteNow(*i, Collector(message.collector),
			  message.disable_save_fpr,
			  std::string(message.callee),
			  Blob(message.argument.data.data_len,
			       message.argument.data.data_val));
	
    }
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * Callback function called by the backend message pump when a request to
 * insert instrumentation into one or more threads is received. Passes the
 * request (for each thread individually) on to the instrumentation table,
 * which instructs Dyninst to perform the actual instrumentation (and keeps
 * track of the instrumentation).
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeAtEntryOrExit(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteAtEntryOrExit message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteAtEntryOrExit),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be instrumented
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "executeAtEntryOrExit(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}

	// Pass the request on to the instrumentation table
	InstrumentationTable::TheTable.
	    addExecuteAtEntryOrExit(*i, Collector(message.collector),
				    std::string(message.where),
				    message.at_entry,
				    std::string(message.callee),
				    Blob(message.argument.data.data_len,
					 message.argument.data.data_val));
	
    }
}



/**
 * Execute a library function in place of another function.
 *
 * Callback function called by the backend message pump when a request to
 * insert instrumentation into one or more threads is received. Passes the
 * request (for each thread individually) on to the instrumentation table,
 * which instructs Dyninst to perform the actual instrumentation (and keeps
 * track of the instrumentation).
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeInPlaceOf(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteInPlaceOf message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteInPlaceOf),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be instrumented
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "executeInPlaceOf(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}

	// Pass the request on to the instrumentation table
	InstrumentationTable::TheTable.
	    addExecuteInPlaceOf(*i, Collector(message.collector),
				std::string(message.where),
				std::string(message.callee));
	
    }
}



/**
 * Get value of an integer global variable from a thread.
 *
 * Callback function called by the backend message pump when a request to get
 * the value of an integer global variable from a thread is received. Instructs
 * Dyninst to obtain the value and sends it to the frontend.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getGlobalInteger(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetGlobalInteger message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalInteger),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Ignore the message if the requested thread isn't on this local host
    if(getCanonicalName(ThreadName(message.thread).getHost()) != 
       getCanonicalName(getLocalHost()))
	return;

    // Get the Dyninst thread and process pointers for this thread
    BPatch_thread* thread = ThreadTable::TheTable.getPtr(message.thread);
    if(thread == NULL) {
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getGlobalInteger(): Thread " << toString(message.thread)
		   << " is not attached." << std::endl;
	    std::cerr << output.str();
	}
#endif
	return;
    }

    BPatch_process* process = NULL;
    process = thread->getProcess();
    Assert(process != NULL);

#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8 )

    if(!process->isStopped()) {
#else
    if(!thread->isStopped()) {
#endif

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getGlobalInteger(): Thread " << toString(message.thread)
		   << " is not stopped. Race conditions may result." 
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
    }

    // Get the global variable
    std::pair<bool, int64_t> value = std::make_pair(false, 0);
    Dyninst::getGlobal(*process, message.global, value);

    // Send the frontend a message with the value (if found) of the variable
    Senders::globalIntegerValue(message.thread, message.global, 
				value.first, value.second);
}



/**
 * Get value of a string global variable from a thread.
 *
 * Callback function called by the backend message pump when a request to get
 * the value of a string global variable from a thread is received. Instructs
 * Dyninst to obtain the value and sends it to the frontend.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getGlobalString(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetGlobalString message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalString),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Ignore the message if the requested thread isn't on this local host
    if(getCanonicalName(ThreadName(message.thread).getHost()) != 
       getCanonicalName(getLocalHost()))
	return;

    // Get the Dyninst thread and process pointers for this thread
    BPatch_thread* thread = ThreadTable::TheTable.getPtr(message.thread);
    if(thread == NULL) {
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getGlobalString(): Thread " << toString(message.thread)
		   << " is not attached." << std::endl;
	    std::cerr << output.str();
	}
#endif
	return;
    }

    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    if(!process->isStopped()) {
#else
    if(!thread->isStopped()) {
#endif

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getGlobalString(): Thread " << toString(message.thread)
		   << " is not stopped. Race conditions may result." 
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
    }

    // Get the global variable
    std::pair<bool, std::string> value = std::make_pair(false, std::string());
    Dyninst::getGlobal(*process, message.global, value);

    // Send the frontend a message with the value (if found) of the variable
    Senders::globalStringValue(message.thread, message.global,
			       value.first, value.second);
}



/**
 * Get value of the MPICH process table from a thread.
 *
 * Callback function called by the backend message pump when a request to get
 * the value of a MPICH process table from a thread is received. Instructs
 * Dyninst to obtain the value and sends it to the frontend.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getMPICHProcTable(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetMPICHProcTable message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetMPICHProcTable),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Ignore the message if the requested thread isn't on this local host
    if(getCanonicalName(ThreadName(message.thread).getHost()) != 
       getCanonicalName(getLocalHost()))
	return;

    // Get the Dyninst thread and process pointers for this thread
    BPatch_thread* thread = ThreadTable::TheTable.getPtr(message.thread);
    if(thread == NULL) {
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getMPICHProcTable(): Thread " << toString(message.thread)
		   << " is not attached." << std::endl;
	    std::cerr << output.str();
	}
#endif
	return;
    }

    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    if(!process->isStopped()) {
#else
    if(!thread->isStopped()) {
#endif

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "getMPICHProcTable(): Thread " << toString(message.thread)
		   << " is not stopped. Race conditions may result." 
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
    }

    // Get the global variable
    std::pair<bool, Job> value = std::make_pair(false, Job());
    Dyninst::getMPICHProcTable(*process, value);

    // Send the frontend a message with the value (if found) of the variable
    Senders::globalJobValue(message.thread, message.global,
			    value.first, value.second);
}



/**
 * Set value of an integer global variable in a thread.
 *
 * Callback function called by the backend message pump when a request to set
 * the value of an integer global variable in a thread is received. Instructs
 * Dyninst to set the value.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::setGlobalInteger(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_SetGlobalInteger message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SetGlobalInteger),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Ignore the message if the requested thread isn't on this local host
    if(getCanonicalName(ThreadName(message.thread).getHost()) != 
       getCanonicalName(getLocalHost()))
	return;

    // Get the Dyninst thread and process pointers for this thread
    BPatch_thread* thread = ThreadTable::TheTable.getPtr(message.thread);
    if(thread == NULL) {
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "setGlobalInteger(): Thread " << toString(message.thread)
		   << " is not attached." << std::endl;
	    std::cerr << output.str();
	}
#endif
	return;
    }

    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    if(!process->isStopped()) {
#else
    if(!thread->isStopped()) {
#endif
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "setGlobalInteger(): Thread " << toString(message.thread)
		   << " is not stopped. Race conditions may result." 
		   << std::endl;
	    std::cerr << output.str();
	}
#endif
    }

    // Set the global variable
    Dyninst::setGlobal(*process, message.global, message.value);
}



/**
 * Standard input stream.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::stdIn(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_StdIn message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdIn),
	&message
	);

#ifndef NDEBUG
    if(Backend::isStdioDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif
    
    // TODO: implement!
}



/**
 * Stop at a function's entry or exit.
 *
 * Callback function called by the backend message pump when a request to
 * insert instrumentation into one or more threads is received. Passes the
 * request (for each thread individually) on to the instrumentation table,
 * which instructs Dyninst to perform the actual instrumentation (and keeps
 * track of the instrumentation).
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::stopAtEntryOrExit(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_StopAtEntryOrExit message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StopAtEntryOrExit),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be instrumented
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "stopAtEntryOrExit(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}
	
	// Pass the request on to the instrumentation table
	InstrumentationTable::TheTable.
	    addStopAtEntryOrExit(*i,
				 std::string(message.where),
				 message.at_entry);
	
    }
}



/**
 * Remove instrumentation from threads.
 *
 * Callback function called by the backend message pump when a request to
 * uninstrument one or more threads is received. Passes the request (for each
 * thread individually) on to the instrumentation table, which instructs
 * Dyninst to remove the instrumentation.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::uninstrument(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_Uninstrument message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Uninstrument),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be uninstrumented
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "uninstrument(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}
	
	// Pass the request on to the instrumentation table
	InstrumentationTable::TheTable.
	    removeInstrumentation(*i, Collector(message.collector));
	
    }
}

void Callbacks::MPIStartup(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_MPIStartup message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_MPIStartup),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread to be notified that we are in mpi startup phase.
    ThreadNameGroup threads = filterByLocalHost(message.threads);
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Has this thread not been attached?
	if(ThreadTable::TheTable.getPtr(*i) == NULL) {
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "MPIStartup(): Thread " << toString(*i)
		       << " is not attached." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    continue;
	}
	
	// Pass the mpi startup flag on to Dyninst class.
	is_in_mpi_startup = message.in_mpi_startup;
	Dyninst::setMPIStartup(message.in_mpi_startup);
    }
}
