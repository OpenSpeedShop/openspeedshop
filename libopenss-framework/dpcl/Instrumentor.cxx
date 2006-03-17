////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "Guard.hxx"
#include "Instrumentor.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



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
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Add this thread to the process table
	ProcessTable::TheTable.addThread(thread);	
    }
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
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Remove this thread from the process table
	ProcessTable::TheTable.removeThread(thread);
    }
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
			  const OutputCallback stdout_cb,
			  const OutputCallback stderr_cb)
{
    // Allocate a new Process object for executing this command
    SmartPtr<Process> process =
        SmartPtr<Process>(new Process(thread.getHost(), command,
                                      stdout_cb, stderr_cb));
    Assert(!process.isNull());

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Add this process to the process table
	ProcessTable::TheTable.addProcess(process);

	// Update the thread's process identifier
	SmartPtr<Database> database = EntrySpy(thread).getDatabase();
	BEGIN_TRANSACTION(database);
	database->prepareStatement("UPDATE Threads SET pid = ? WHERE id = ?;");
	database->bindArgument(1, static_cast<int>(process->getProcessId()));
	database->bindArgument(2, EntrySpy(thread).getEntry());
	while(database->executeStatement());    
	END_TRANSACTION(database);

	// Add this thread to the process table
	ProcessTable::TheTable.addThread(thread);
    }
}



/**
 * Get a thread's state.
 *
 * Returns the current state of the specified thread. Since this state changes
 * asynchronously, and may be updated across a network, there is a lag between
 * when the actual thread's state changes and when it is reflected here.
 *
 * @todo    Currently DPCL provides the ability to get the state of an entire
 *          process only - not that of a single thread. For now, <em>all</em>
 *          threads in a given process will have the same state.
 *
 * @param thread    Thread whose state should be obtained.
 * @return          Current state of the thread.
 */
Thread::State Instrumentor::getState(const Thread& thread)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
    }
    
    // Return the thread's current state to the caller
    return (process.isNull()) ? Thread::Disconnected : process->getState();
}



/**
 * Change a thread's state.
 *
 * Changes the current state of the specified thread to the passed value. Used 
 * to, for example, suspend a thread that was previously running. This function
 * does not wait until the thread has actually completed the state change, and
 * calling getThreadState() immediately following changeThreadState() will not
 * reflect the new state until the change has actually completed.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *         There is one exception - a disconnected thread may be changed to
 *         the "connecting" state.
 *
 * @todo    Currently DPCL provides the ability to change the state of an entire
 *          process only - not that of a single thread. For now, if a thread's
 *          state if changed, <em>all</em> threads in the process containing
 *          that thread will have their state changed.
 *
 * @param thread    Thread whose state should be changed.
 * @param state     Change the theread to this state.
 */
void Instrumentor::changeState(const Thread& thread, const Thread::State& state)
{
    SmartPtr<Process> process;
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
	
	// Are we attempting to connect to this process for the first time?
	if(process.isNull() && (state == Thread::Connecting)) {
	    
	    // Allocate a new Process object for connecting to this process
	    process = SmartPtr<Process>(new Process(thread.getHost(),
						    thread.getProcessId()));
	    Assert(!process.isNull());
	    
	    // Add this process to the process table
	    ProcessTable::TheTable.addProcess(process);
	    
	}
    }
    
    // Check preconditions
    if(process.isNull())
	throw Exception(Exception::ThreadUnavailable, thread.getHost(),
			Exception::toString(thread.getProcessId()));
    
    // Request a state change from the process
    process->changeState(state);    
}



/**
 * Execute a library function now.
 *
 * Immediately execute the specified library function in the specified thread.
 * Used by collectors to execute functions in their runtime library.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @todo    Currently DPCL does not provide the ability to specify the thread
 *          within a process that will execute a probe expression. For now, the
 *          only guarantee that can be made is that <em>some</em> thread in the
 *          process containing the specified thread will execute the expression.
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Instrumentor::executeNow(const Thread& thread, 
			      const Collector& collector,
			      const std::string callee, 
			      const Blob& argument)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the library function be executed by the process
    process->executeNow(collector, thread, callee, argument);
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * Executes the specified library function every time another function's entry
 * or exit is executed in the specified thread. Used by collectors to execute
 * functions in their runtime library.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @todo    Currently DPCL does not provide the ability to specify the thread
 *          within a process that will execute a probe expression. For now, the
 *          only guarantee that can be made is that <em>all</em> threads in the
 *          process containing the specified thread will execute the expression.
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function at whose entry/exit the library
 *                     function should be executed.
 * @param at_entry     Boolean "true" if instrumenting function's entry point,
 *                     or "false" if function's exit point.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Instrumentor::executeAtEntryOrExit(const Thread& thread,
					const Collector& collector,
					const std::string& where, 
					const bool& at_entry,
					const std::string& callee, 
					const Blob& argument)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the library function be executed in the process
    process->executeAtEntryOrExit(collector, thread, where, at_entry, 
				  callee, argument);
}



/**
 * Execute a library function in place of another function.
 *
 * Executes the specified library function in place of another function every
 * other time that other function is called. Used by collectors to create
 * wrappers around functions for the purposes of gathering performance data on
 * their execution.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @todo    Currently DPCL does not provide the ability to specify the thread
 *          within a process that will execute a probe expression. For now, the
 *          only guarantee that can be made is that <em>all</em> threads in the
 *          process containing the specified thread will execute the expression.
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void Instrumentor::executeInPlaceOf(const Thread& thread,
				    const Collector& collector,
				    const std::string& where, 
				    const std::string& callee)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the library function be executed in the process
    process->executeInPlaceOf(collector, thread, where, callee);
}



/**
 * Remove instrumentation from a thread.
 *
 * Removes all instrumentation associated with the specified collector from the
 * specified thread. Used by collectors to indicate when they are done using any
 * instrumentation they placed in the thread.
 *
 * @pre    Only applies to a thread which is connected. A ThreadUnavailable
 *         exception is thrown if called for a thread that is not connected.
 *
 * @param thread       Thread from which instrumentation should be removed.
 * @param collector    Collector which is removing instrumentation.
 */	
void Instrumentor::uninstrument(const Thread& thread, 
				const Collector& collector)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the collector's instrumentation be removed from the process
    process->uninstrument(collector, thread);
}



/**
 * Get value of an integer global variable from a thread.
 *
 * Gets the current value of a signed integer global variable within this
 * thread. Used to extract certain types of data, such as MPI job identifiers,
 * from the process.
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
			     int64_t& value)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the global variable be retrieved from the process
    return process->getGlobal(global, value);
}



/**
 * Get value of a string global variable from a thread.
 *
 * Gets the current value of a character string global variable within this
 * thread. Used to extract certain types of data, such as MPI job identifiers,
 * from the process.
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
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the global variable be retrieved from the process
    return process->getGlobal(global, value);
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
    SmartPtr<Process> process;
     
    // Critical section touching the process table
    {
        Guard guard_process_table(ProcessTable::TheTable);

        // Get the process for this thread (if any)
        process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull() || !process->isConnected())
        throw Exception(Exception::ThreadUnavailable, thread.getHost(),
                        Exception::toString(thread.getProcessId()));
    
    // Request the MPICH process table be retrieved from the process
    return process->getMPICHProcTable(value);
}
