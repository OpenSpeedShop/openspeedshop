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
#include "Database.hxx"
#include "Guard.hxx"
#include "Instrumentor.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "ThreadSpy.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



/*
 * See "../Implementor.hxx" for this function's description.
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
    SmartPtr<Database> database = ThreadSpy(thread).getDatabase();
    BEGIN_TRANSACTION(database);
    database->prepareStatement("UPDATE Threads SET pid = ? WHERE id = ?;");
    database->bindArgument(1, static_cast<int>(process->getProcessId()));
    database->bindArgument(2, ThreadSpy(thread).getEntry());
    while(database->executeStatement());    
    END_TRANSACTION(database);
    
    // Update the current in-memory address space of this thread
    process->updateAddressSpace(thread, Time::Now());
}



/*
 * See "../Implementor.hxx" for this function's description.
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



/*
 * See "../Implementor.hxx" for this function's description.
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



/*
 * See "../Implementor.hxx" for this function's description.
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



/*
 * See "../Implementor.hxx" for this function's description.
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
    
    // Unattached threads cannot have their state changed
    if(process.isNull())
	switch(state) {
	case Thread::Running:
	    throw std::logic_error("Cannot run a terminated process.");
	case Thread::Suspended:
	    throw std::logic_error("Cannot suspend a terminated process.");
	case Thread::Terminated:
	    throw std::logic_error("Cannot terminate a terminated process.");
	default:
	    Assert((state == Thread::Running) ||
		   (state == Thread::Suspended) ||
		   (state == Thread::Terminated));
	}
    
    // Request a state change from the process
    process->changeState(state);    
}
