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
#include "Guard.hxx"
#include "Instrumentor.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Create underlying thread.
 *
 * Creates the specified thread as a new process to execute the passed command.
 * The command is created with the same initial environment (standard file
 * descriptors, environment variables, etc.) as when the tool was started. The
 * process is created in a suspended state. When a thread is created, it is
 * automatically attached.
 *
 * @param thread     Thread to be created.
 * @param command    Command to be executed.
 */
void Instrumentor::createUnderlyingThread(const Thread& thread,
					  const std::string& command)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Create a new process to execute this command
	process = SmartPtr<Process>(new Process(thread.getHost(), command));
	Assert(!process.isNull());
	
	// Add this thread to the process table
	ProcessTable::TheTable.addThread(thread, process);
    }
    
    // Update the thread's process identifier
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_TRANSACTION(database);
    database->prepareStatement("UPDATE Threads SET pid = ? WHERE id = ?;");
    database->bindArgument(1, static_cast<int>(process->getProcessId()));
    database->bindArgument(2, EntrySpy(thread).getEntry());
    while(database->executeStatement());    
    END_TRANSACTION(database);
    
    // Update the current in-memory address space of this thread
    process->updateAddressSpace(thread, Time::Now());
}



/**
 * Attach to underlying thread.
 *
 * Attaches the specified thread to its underlying thread in the operating
 * system. Once a thread is attached, the instrumentor can perform operations
 * (such as changing its state) on this thread.
 *
 * @param thread    Thread to be attached.
 */
void Instrumentor::attachUnderlyingThread(const Thread& thread)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Look for a pre-existing process for this thread
	process = ProcessTable::TheTable.getProcessByName(
	    Process::formUniqueName(thread.getHost(), thread.getProcessId()));
	
	// Attach to the process if we haven't already
	if(process.isNull())
	    process = SmartPtr<Process>(new Process(thread.getHost(),
						    thread.getProcessId()));
	Assert(!process.isNull());
	
	// Add this thread to the process table
	ProcessTable::TheTable.addThread(thread, process);  
    }
    
    // Update the current in-memory address space of this thread
    process->updateAddressSpace(thread, Time::Now());
}



/**
 * Detach from underlying thread.
 *
 * Detaches the specified thread from its underlying thread in the operating
 * system. Once a thread is detached, the instrumentor can no longer perform
 * operations (such as changing its state) on this thread without first being
 * attached again. The underlying thread in the operating system is <em>not</em>
 * destroyed. If the thread was in the suspended state, it is put into the
 * running state before being detached.
 *
 * @param thread    Thread to be detached.
 */
void Instrumentor::detachUnderlyingThread(const Thread& thread)
{
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
    
	// Remove the thread (and possibly the process) from the process table
	ProcessTable::TheTable.removeThread(thread);
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
Thread::State Instrumentor::getThreadState(const Thread& thread)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
    }
    
    // Return the thread's current state to the caller
    return (process.isNull()) ? Thread::Terminated : process->getState();
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
 * @pre    Only applies to a thread which has been attached to an underlying
 *         thread. An exception of type std::logic_error is thrown if called
 *         for a thread that is not attached.
 *
 * @todo    Currently DPCL provides the ability to change the state of an entire
 *          process only - not that of a single thread. For now, if a thread's
 *          state if changed, <em>all</em> threads in the process containing
 *          that thread will have their state changed.
 *
 * @param thread    Thread whose state should be changed.
 * @param state     Change the theread to this state.
 */
void Instrumentor::changeThreadState(const Thread& thread, 
				     const Thread::State& state)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull())
	throw std::logic_error(
	    "Cannot change the state of an unattached thread."
	    );
    
    // Request a state change from the process
    process->changeState(state);    
}



/**
 * Load library into a thread.
 *
 * Loads the passed library into the specified thread. Collectors use this
 * function to load their runtime library(ies). Separated loads of the same
 * library into a single thread do <em>not</em> result in multiple loaded
 * copies. Instead a reference count is maintained and the library will not
 * be unloaded until each reference is released via unloadLibrary().
 *
 * @pre    Only applies to a thread which has been attached to an underlying
 *         thread. An exception of type std::logic_error is thrown if called
 *         for a thread that is not attached.
 *
 * @param thread     Thread into which the library should be loaded.
 * @param library    Name of library to be loaded.
 */
void Instrumentor::loadLibrary(const Thread& thread,
			       const std::string& library)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
    }

    // Check preconditions
    if(process.isNull())
	throw std::logic_error(
	    "Cannot load a library into an unattached thread."
	    );

    // Request the library be loaded into the process
    process->loadLibrary(library);    
}



/**
 * Unload library from a thread.
 *
 * Unloads the passed library from the specified thread. Collectors use this
 * function to unload their runtime library(ies). The library isn't actually
 * unloaded until each call to loadLibrary() for that library has been matched
 * by a corresponding call to unloadLibrary().
 *
 * @pre    Only applies to a thread which has been attached to an underlying
 *         thread. An exception of type std::logic_error is thrown if called
 *         for a thread that is not attached.
 *
 * @param thread     Thread from which the library should be unloaded.
 * @param library    Name of library to be unloaded.
 */
void Instrumentor::unloadLibrary(const Thread& thread,
				 const std::string& library)
{
    SmartPtr<Process> process;
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Get the process for this thread (if any)
	process = ProcessTable::TheTable.getProcessByThread(thread);
    } 

    // Check preconditions
    if(process.isNull())
	throw std::logic_error(
	    "Cannot unload a library from an unattached thread."
	    );
    
    // Request the library be unloaded from the process
    process->unloadLibrary(library);    
}



/**
 * Execute a library function in a thread.
 *
 * Immediately execute the specified function in the specified thread. Used by
 * collectors to execute function in their runtime library(ies).
 *
 * @pre    Only applies to a thread which has been attached to an underlying
 *         thread. An exception of type std::logic_error is thrown if called
 *         for a thread that is not attached.
 *
 * @todo    Currently DPCL does not provide the ability to specify the thread
 *          within a process that will execute a probe expression. For now, the
 *          only guarantee that can be made is that <em>some</em> thread in the
 *          process containing the specified thread will execute the expression.
 *
 * @param thread      Thread in which the function should be executed.
 * @param library     Name of library containing function to be executed.
 * @param function    Name of function to be executed.
 * @param argument    Blob argument to the function.
 */
void Instrumentor::execute(const Thread& thread,
			   const std::string& library,
			   const std::string& function,
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
    if(process.isNull())
	throw std::logic_error(
	    "Cannot execute a function in an unattached thread."
	    );

    // Request the function be executed by the process
    process->execute(library, function, argument);
}
