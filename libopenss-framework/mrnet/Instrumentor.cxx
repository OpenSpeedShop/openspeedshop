////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the Instrumentor namespace.
 *
 */

#include "Blob.hxx"
#include "Callbacks.hxx"
#include "GlobalTable.hxx"
#include "EntrySpy.hxx"
#include "Frontend.hxx"
#include "Guard.hxx"
#include "Instrumentor.hxx"
#include "Path.hxx"
#include "Senders.hxx"
#include "ThreadGroup.hxx"
#include "ThreadTable.hxx"
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Flag indicating if MRNet has been initialized. */
    bool isMRNetInitialized = false;
    


    /**
     * Initialize MRNet.
     *
     * Performs any initializations that are necessary before the first MRNet
     * operation. This consists of starting the MRNet frontend message pump
     * after registering the callbacks.
     */
    void initializeMRNet()
    {
	Path topology_filename = "~/.openspeedshop/.openss-mrnet-topology";
	
	// Go no further if MRNet is already initialized
	if(isMRNetInitialized)
	    return;

	// Register callbacks with the frontend
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_ATTACHED_TO_THREADS,
				   Callbacks::attachedToThreads);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_CREATED_PROCESS,
				   Callbacks::createdProcess);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE,
				   Callbacks::globalIntegerValue);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_GLOBAL_JOB_VALUE,
				   Callbacks::globalJobValue);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_GLOBAL_STRING_VALUE,
				   Callbacks::globalStringValue);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_INSTRUMENTED,
				   Callbacks::instrumented);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
				   Callbacks::loadedLinkedObject);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_REPORT_ERROR,
				   Callbacks::reportError);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_STDERR,
				   Callbacks::stdErr);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_STDOUT,
				   Callbacks::stdOut);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_SYMBOL_TABLE,
				   Callbacks::symbolTable);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_THREADS_STATE_CHANGED,
				   Callbacks::threadsStateChanged);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT,
				   Callbacks::unloadedLinkedObject);
	Frontend::registerCallback(OPENSS_PROTOCOL_TAG_PERFORMANCE_DATA,
				   Callbacks::performanceData);
	
	// Start the MRNet frontend message pump
	if(getenv("OPENSS_MRNET_TOPOLOGY_FILE") != NULL)
	    topology_filename = getenv("OPENSS_MRNET_TOPOLOGY_FILE");
	Frontend::startMessagePump(topology_filename);

	// MRNet is now initialized
	isMRNetInitialized = true;
    }
    


    /**
     * Finalize MRNet.
     *
     * Performs any finalizations that are necessary after the last MRNet
     * operation. This consists of stopping the MRNet frontend message pump.
     */
    void finalizeMRNet()
    {	
	// Go no further if MRNet isn't initialized
	if(!isMRNetInitialized)
	    return;

	// Stop the MRNet frontend message pump
	Frontend::stopMessagePump();

	// Unregister callbacks with the frontend
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_ATTACHED_TO_THREADS,
				     Callbacks::attachedToThreads);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_CREATED_PROCESS,
				     Callbacks::createdProcess);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE,
				     Callbacks::globalIntegerValue);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_GLOBAL_JOB_VALUE,
				     Callbacks::globalJobValue);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_GLOBAL_STRING_VALUE,
				     Callbacks::globalStringValue);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_INSTRUMENTED,
				     Callbacks::instrumented);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
				     Callbacks::loadedLinkedObject);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_REPORT_ERROR,
				     Callbacks::reportError);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_STDERR,
				     Callbacks::stdErr);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_STDOUT,
				     Callbacks::stdOut);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_SYMBOL_TABLE,
				     Callbacks::symbolTable);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_THREADS_STATE_CHANGED,
				     Callbacks::threadsStateChanged);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT,
				     Callbacks::unloadedLinkedObject);
	Frontend::unregisterCallback(OPENSS_PROTOCOL_TAG_PERFORMANCE_DATA,
				     Callbacks::performanceData);
	
	// MRNet is no longer initialized
	isMRNetInitialized = true;
    }



}



/**
 * Retain a thread.
 *
 * Retains a thread in this instrumentor. Retaining a thread simply indicates
 * to the instrumentor that the thread is currently in use. This information is
 * used by the instrumentor to decide when it is appropriate to disconnect from
 * the underlying process in the operating system that contains this thread.
 *
 * @param thread    Thread to be retained.
 */
void Instrumentor::retain(const Thread& thread)
{
    // Add this thread to the thread table
    ThreadTable::TheTable.addThread(thread);
}



/**
 * Release a thread.
 *
 * Releases a thread in this instrumentor. Releasing a thread simply indicates
 * to the instrumentor that the thread is no longer in use. This information is
 * used by the instrumentor to decide when it is appropriate to disconnect from
 * the underlying thread in the operating system that contains this thread.
 *
 * @param thread    Thread to be released.
 */
void Instrumentor::release(const Thread& thread)
{
    // Is this thread connected?
    if(ThreadTable::TheTable.isConnected(thread)) {
	
	// Construct a thread group containing this thread
	ThreadGroup threads;
	threads.insert(thread);
	
	// Request these threads be detached
	Senders::detachFromThreads(threads);    

    }

    // Remove this thread from the thread table
    ThreadTable::TheTable.removeThread(thread);

    // Finalize MRNet if the thread table is empty and MRNet is initialized
    if(ThreadTable::TheTable.isEmpty() && isMRNetInitialized)
	finalizeMRNet();
}



/**
 * Create a thread.
 *
 * Creates the specified thread as a new process to execute the passed command.
 * The command is created with the same initial environment as when the tool was
 * started. The process is created in a suspended state.
 *
 * @param thread             Thread to be created.
 * @param command            Command to be executed.
 * @param stdout_callback    Standard output stream callback for the process.
 * @param stderr_callback    Standard error stream callback for the process.
 */
void Instrumentor::create(const Thread& thread, 
			  const std::string& command,
			  const OutputCallback stdout_callback,
			  const OutputCallback stderr_callback)
{
    // Initialize MRNet if it hasn't been yet
    if(!isMRNetInitialized)
	initializeMRNet();
    
    // Add this thread to the thread table
    ThreadTable::TheTable.addThread(thread, stdout_callback, stderr_callback);
    ThreadTable::TheTable.setConnecting(thread);

    //
    // Update the thread's process identifier to be the negative of the
    // thread's identifier. This is a placeholder value that is replaced
    // with the real process identifier by Callbacks::createdProcess().
    // 
   
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_WRITE_TRANSACTION(database);
    database->prepareStatement("UPDATE Threads SET pid = ? WHERE id = ?;");
    database->bindArgument(1, - EntrySpy(thread).getEntry());
    database->bindArgument(2, EntrySpy(thread).getEntry());
    while(database->executeStatement());    
    END_TRANSACTION(database);

    // Use access to the external environment variables
    // Assemble the environment variables into a single concatenated buffer
    unsigned env_size = 0;
    for(int i = 0; environ[i] != NULL; ++i)
	env_size += strlen(environ[i]) + 1;
    char* env = new char[env_size];
    for(int i = 0, j = 0; environ[j] != NULL; i += strlen(environ[j]) + 1, ++j)
	strcpy(&(env[i]), environ[j]);

    // Request the process be created
    Senders::createProcess(thread, command, Blob(env_size, env));

    // Destroy the concatenated environment variable buffer
    delete [] env;
}



/**
 * Get a thread's state.
 *
 * Returns the current state of the specified thread. Since this state changes
 * asynchronously, and may be updated across a network, there is a lag between
 * when the actual thread's state changes and when it is reflected here.
 *
 * @param thread    Thread whose state should be obtained.
 * @return          Current state of the thread.
 */
Thread::State Instrumentor::getState(const Thread& thread)
{
    // Return the thread's current state to the caller
    return ThreadTable::TheTable.getThreadState(thread);
}



/**
 * Change state of threads.
 *
 * Changes the current state of the specified threads to the passed value. Used
 * to, for example, suspend threads that were previously running. This function
 * does not wait until the threads have actually completed the state change, and
 * calling getState() immediately following changeState() will reflect the new
 * state immediately.
 *
 * @pre    Only applies to threads which are connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected. There is one exception - disconnected threads may be
 *         changed to the "connecting" state.
 *
 * @param threads    Threads whose state should be changed.
 * @param state      Change the threads to this state.
 */
void Instrumentor::changeState(const ThreadGroup& threads,
			       const Thread::State& state)
{
    // Are we attempting to connect to threads for the first time?
    if(state == Thread::Connecting) {

	// Initialize MRNet if it hasn't been yet
	if(!isMRNetInitialized)
	    initializeMRNet();
	
	// Build the set of threads that need to be attached
	ThreadGroup threads_to_attach;
	for(ThreadGroup::const_iterator
		i = threads.begin(); i != threads.end(); ++i)
	    if(ThreadTable::TheTable.setConnecting(*i))
		threads_to_attach.insert(*i);

	// Request these threads be attached
	if(!threads_to_attach.empty())
	    Senders::attachToThreads(threads_to_attach);
	
    }
    else {
	
	// Check preconditions
	Assert(isMRNetInitialized);
	ThreadTable::TheTable.validateThreads(threads);

	// Request a state change from the threads
	Senders::changeThreadsState(threads, state);

	// Update these threads with their new state
	for(ThreadGroup::const_iterator
		i = threads.begin(); i != threads.end(); ++i)
	    ThreadTable::TheTable.setThreadState(*i, state);
	
    }
}



/**
 * Execute a library function now.
 *
 * Immediately execute the specified library function in the specified threads.
 * Used by collectors to execute functions in their runtime library.
 *
 * @pre    Only applies to threads which are connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected.
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Instrumentor::executeNow(const ThreadGroup& threads, 
			      const Collector& collector,
			      const std::string callee, 
			      const Blob& argument,
			      const bool& disableSaveFPR)
{
    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the library function be executed by these threads
    Senders::executeNow(threads, collector, disableSaveFPR, callee, argument);
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * Executes the specified library function every time another function's entry
 * or exit is executed in the specified threads. Used by collectors to execute
 * functions in their runtime library.
 *
 * @pre    Only applies to threads which are connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected.
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function at whose entry/exit the library
 *                     function should be executed.
 * @param at_entry     Boolean "true" if instrumenting function's entry point,
 *                     or "false" if function's exit point.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Instrumentor::executeAtEntryOrExit(const ThreadGroup& threads,
					const Collector& collector,
					const std::string& where, 
					const bool& at_entry,
					const std::string& callee, 
					const Blob& argument)
{
    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the library function be executed in these threads
    Senders::executeAtEntryOrExit(threads, collector, where,
				  at_entry, callee, argument);
}



/**
 * Execute a library function in place of another function.
 *
 * Executes the specified library function in place of another function every
 * other time that other function is called. Used by collectors to create
 * wrappers around functions for the purposes of gathering performance data on
 * their execution.
 *
 * @pre    Only applies to threads which are connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected.
 *
 * @param threads      Threads in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void Instrumentor::executeInPlaceOf(const ThreadGroup& threads,
				    const Collector& collector,
				    const std::string& where, 
				    const std::string& callee)
{
    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the library function be executed in these threads
    Senders::executeInPlaceOf(threads, collector, where, callee);
}



/**
 * Remove instrumentation from threads.
 *
 * Removes all instrumentation associated with the specified collector from the
 * specified threads. Used by collectors to indicate when they are done using
 * any instrumentation they have placed in threads.
 *
 * @pre    Only applies to threads which are connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected.
 *
 * @param threads      Threads from which instrumentation should be removed.
 * @param collector    Collector which is removing instrumentation.
 */	
void Instrumentor::uninstrument(const ThreadGroup& threads, 
				const Collector& collector)
{
    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);
    
    // Request the collector's instrumentation be removed from these threads
    Senders::uninstrument(threads, collector);
}



/**
 * Stop at a function's entry or exit.
 *
 * Stops every time the specified function's entry or exit is executed in the
 * specified thread. Used by the framework to implement MPI job creation.
 *
 * @pre    Only applies to a thread which is connected. A ProcessUnavailable or
 *         ThreadUnavailable exception is thrown if called for a thread that is
 *         not connected.
 *
 * @param thread      Thread which should be stopped.
 * @param where       Name of the function at whose entry/exit the stop should
 *                    occur.
 * @param at_entry    Boolean "true" if instrumenting function's entry point,
 *                    or "false" if function's exit point.
 */
void Instrumentor::stopAtEntryOrExit(const Thread& thread,
				     const std::string& where, 
				     const bool& at_entry)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);

    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the stop be added to these threads
    Senders::stopAtEntryOrExit(threads, where, at_entry);
}



/**
 * Get value of an integer global variable from a thread.
 *
 * Gets the current value of a signed integer global variable within this
 * thread. Used to extract certain types of data, such as MPI job identifiers,
 * from a process.
 *
 * @pre    Only applies to a process which is connected. A ProcessUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @param thread    Thread from which the global variable value should be
 *                  retrieved.
 * @param global    Name of global variable whose value is being requested.
 * @retval value    Current value of that variable.
 * @return          Boolean "true" if the variable's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Instrumentor::getGlobal(const Thread& thread,
			     const std::string& global,
			     int64_t& value)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);

    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the global variable be retrieved from the thread
    Senders::getGlobalInteger(thread, global);
    return GlobalTable::TheTable.waitForValue(thread, global, value);
}



/**
 * Get value of a string global variable from a thread.
 *
 * Gets the current value of a character string global variable within this
 * thread. Used to extract certain types of data, such as MPI job identifiers,
 * from a process.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @param thread    Thread from which the global variable value should be
 *                  retrieved.
 * @param global    Name of global variable whose value is being requested.
 * @retval value    Current value of that variable.
 * @return          Boolean "true" if the variable's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Instrumentor::getGlobal(const Thread& thread,
			     const std::string& global,
			     std::string& value)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);

    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the global variable be retrieved from the thread
    Senders::getGlobalString(thread, global);
    return GlobalTable::TheTable.waitForValue(thread, global, value);
}



/**
 * Get value of the MPICH process table from a thread.
 *
 * Gets the current value of the MPICH process table within this thread. Used
 * to obtain this information for the purposes of attaching to an entire MPI
 * job.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @param thread    Thread from which the MPICH process table should be
 *                  retrieved. 
 * @retval value    Current value of the MPICH process table.
 * @return          Boolean "true" if the MPICH process table was successfully
 *                  retrieved, "false" otherwise.
 */
bool Instrumentor::getMPICHProcTable(const Thread& thread, Job& value)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);

    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the MPICH process table be retrieved from the thread
    const std::string global = "MPIR_proctable";
    Senders::getMPICHProcTable(thread, global);
    return GlobalTable::TheTable.waitForValue(thread, global, value);
}



/**
 * Set value of an integer global variable in a thread.
 *
 * Sets the value of a signed integer global variable within this thread.
 * Used to modify certain values, such as MPI debug gates, in a process.
 *
 * @pre    Only applies to a process which is connected. A ProcessUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @param thread    Thread in which the global variable value should be set.
 * @param global    Name of global variable whose value is being set.
 * @param value     New value of that variable.
 */
void Instrumentor::setGlobal(const Thread& thread,
			     const std::string& global,
			     int64_t value)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);

    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the global variable be set in the process
    Senders::setGlobalInteger(thread, global, value);
}



/**
 * Set the MPI startup flag.
 *
 * Ignored by MRNet instrumentor.
 */
void Instrumentor::setMPIStartup(const Thread& thread, const bool mpi_startup)
{
    // Construct a thread group containing this thread
    ThreadGroup threads;
    threads.insert(thread);
 
    // Check preconditions
    Assert(isMRNetInitialized);
    ThreadTable::TheTable.validateThreads(threads);

    // Request the stop be added to these threads
    Senders::MPIStartup(threads, mpi_startup);
}



/**
 * Test the MPI startup flag.
 *
 * Ignored by the MRNet instrumentor - always returns "false".
 */
bool Instrumentor::inMPIStartup(const Thread&)
{
    return false;
}
