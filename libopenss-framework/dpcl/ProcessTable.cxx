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
 * Definition of the ProcessTable class.
 *
 */

#include "Assert.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton process table. */
ProcessTable ProcessTable::TheTable;



/**
 * Test if table is empty.
 *
 * Returns a boolean value indicating if this process table is empty.
 *
 * @return    Boolean "true" if this process table is empty, "false" otherwise.
 */
bool ProcessTable::isEmpty() const
{
    return dm_name_to_process.empty();
}



/**
 * Add a thread.
 *
 * Adds the passed thread to this process table. If the passed thread is the
 * first thread within a given process, that process is also added to this
 * process table.
 *
 * @note     An assertion failure occurs if an attempt is made to add a thread
 *           more than once.
 *
 * @param thread     Thread to be added.
 * @param process    Process containing the added thread.
 */
void ProcessTable::addThread(const Thread& thread,
			     const SmartPtr<Process>& process)
{
    // Check assertions
    Assert(dm_thread_to_process.find(thread) == dm_thread_to_process.end());

    // Add the process if this is the first thread using it
    if(dm_process_to_threads.find(process) == dm_process_to_threads.end()) {
	std::string name = Process::formUniqueName(process->getHost(),
						   process->getProcessId());
	dm_name_to_process[name] = process;
	dm_process_to_threads[process] = std::set<Thread>();
    }
   
    // Add this thread
    (dm_process_to_threads.find(process))->second.insert(thread);
    dm_thread_to_process[thread] = process;
}



/**
 * Remove a thread.
 *
 * Removes the passed thread from this process table. If the passed thread was
 * the last thread within a given process, that process is also removed from
 * this process table.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a
 *          thread that isn't in this process table.
 *
 * @param thread    Thread to be removed.
 */
void ProcessTable::removeThread(const Thread& thread)
{
    // Check assertions
    Assert(dm_thread_to_process.find(thread) != dm_thread_to_process.end());
    
    // Find this thread's process
    std::map<Thread, SmartPtr<Process> >::const_iterator
	i = dm_thread_to_process.find(thread);
    
    // Remove this thread
    (dm_process_to_threads.find(i->second))->second.erase(thread);
    dm_thread_to_process.erase(thread);
    
    // Remove the process if this was the last thread using it
    if((dm_process_to_threads.find(i->second))->second.empty()) {
	dm_name_to_process.erase(Process::formUniqueName(
				     i->second->getHost(),
				     i->second->getProcessId()));	
	dm_process_to_threads.erase(i->second);	
    }
}



/**
 * Get a process by its name.
 *
 * Returns the process corresponding to the passed unique process name. A null
 * pointer is returned if the passed name isn't in this process table.
 *
 * @todo    For some reason GCC insists on a fully-qualified class name
 *          OpenSpeedShop::Framework::Process in this function's definition.
 *          Not quite sure why... Maybe this can be fixed?
 *
 * @param name    Query name.
 * @return        Process with this name.
 */
SmartPtr<OpenSpeedShop::Framework::Process>
ProcessTable::getProcessByName(const std::string& name) const
{
    std::map<std::string, SmartPtr<Process> >::const_iterator
	i = dm_name_to_process.find(name);
    return (i != dm_name_to_process.end()) ? i->second : SmartPtr<Process>();
}



/**
 * Get a process by a thread.
 *
 * Returns the process containing the passed thread. A null pointer is returned
 * if the passed thread isn't in this process table.
 *
 * @todo    For some reason GCC insists on a fully-qualified class name
 *          OpenSpeedShop::Framework::Process in this function's definition.
 *          Not quite sure why... Maybe this can be fixed?
 *
 * @param thread    Query thread.
 * @return          Process for this thread.
 */
SmartPtr<OpenSpeedShop::Framework::Process>
ProcessTable::getProcessByThread(const Thread& thread) const
{
    std::map<Thread, SmartPtr<Process> >::const_iterator
	i = dm_thread_to_process.find(thread);
    return (i != dm_thread_to_process.end()) ? i->second : SmartPtr<Process>();
}



/**
 * Get the threads for a process.
 *
 * Returns all the threads contained in the passed process. An empty set is
 * returned if the passed process isn't in this process table.
 *
 * @param process    Query process.
 * @return           Set of threads for this process.
 */
std::set<Thread>
ProcessTable::getThreadsByProcess(const SmartPtr<Process>& process) const
{
    std::map<SmartPtr<Process>, std::set<Thread> >::const_iterator
	i = dm_process_to_threads.find(process);
    return (i != dm_process_to_threads.end()) ? i->second : std::set<Thread>();
}



/**
 * Get threads by their process' unique name.
 *
 * Returns all the threads for the passed unique process name. An empty set is
 * returned if the passed name isn't in this process table.
 *
 * @param name    Query name.
 * @return        Set of threads for the process of this name.
 */
std::set<Thread> ProcessTable::getThreadsByName(const std::string& name) const
{
    SmartPtr<Process> process = getProcessByName(name);
    return (!process.isNull()) ?
	getThreadsByProcess(process) : std::set<Thread>();
}
