////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the Application class.
 *
 */

#include "Application.hxx"
#include "Assert.hxx"
#include "Collector.hxx"
#include "Guard.hxx"
#include "Path.hxx"
#include "PluginTable.hxx"
#include "Process.hxx"
#include "SymbolTable.hxx"
#include "Thread.hxx"

#include <algorithm>
#include <stdexcept>
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



/**
 * Destructor.
 *
 * Destroys all objects (threads, collectors, processes, symbol tables, etc.)
 * associated with this application. None of these objects should be used once
 * their containing application is destroyed.
 */
Application::~Application()
{
    Guard guard_myself(this);
    
    // Destroy all threads (implictly destroying processes along the way)
    while(dm_threads.begin() != dm_threads.end())
	removeThread(*(dm_threads.begin()));

    // Destroy all collectors
    while(dm_collectors.begin() != dm_collectors.end())
	removeCollector(*(dm_collectors.begin()));
    
    // Destroy all symbol tables
    for(std::vector<const SymbolTable*>::const_iterator
	    i = dm_symbol_tables.begin(); i != dm_symbol_tables.end(); ++i)
	delete *i;
}



/**
 * Get our threads.
 *
 * Returns to the caller all threads currently in this application. An empty
 * thread group is returned if this application doesn't contain any threads.
 *
 * @return    Threads currently contained within this application.
 */
ThreadGroup Application::getThreads() const
{
    Guard guard_myself(this);
    
    // Return the threads to the caller
    return dm_threads;
}



/**
 * Create a process.
 *
 * Creates a new process to execute the specified command and adds that process
 * to this application. The command is created with the same initial environment
 * (standard file descriptors, environment variables, etc.) as when the tool was
 * started. The process is created in a suspended state.
 *
 * @param command    Command to be executed.
 * @param host       Name of host on which to execute the command.
 ( @return           Newly created thread.
*/
Thread* Application::createProcess(const std::string& command,
				   const std::string& host)
{
    Guard guard_myself(this);    
    
    // Create a process for executing this command
    Process* process = new Process(this, host, command);
    Assert(process != NULL);

    // Add this process to the process table
    dm_processes.addProcess(process->getHost(),
			    process->getProcessId(),
			    process);
    
    // Process starts with a single thread
    Thread* thread = new Thread(process);
    Assert(thread != NULL);
    
    // Add this thread to the thread group
    dm_threads.insert(dm_threads.end(), thread);
    
    // Add this thread to the process' list of references
    dm_processes.addReference(thread, process);    

    // Return the thread to the caller
    return thread;    
}



/**
 * Attach to a process.
 *
 * Attaches to an existing process and adds all threads within that process to
 * this application. The threads' statuses are not affected.
 *
 * @todo    Currently this routine assumes each process has a single thread and
 *          thus creates only one Thread object. Once a mechanism is found to
 *          "discover" the list of threads in a process this can be changed to
 *          support the desired semantics.
 *
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 */
void Application::attachProcess(const pid_t& pid, const std::string& host)
{
    Guard guard_myself(this);

    // Re-use the existing process if one is available
    Process* process = dm_processes.findProcessByName(host, pid);
    if(process == NULL) {
	
	// Attach to the new process
        process = new Process(this, host, pid);
	Assert(process != NULL);
	
	// Add this process to the process table
	dm_processes.addProcess(process->getHost(),
				process->getProcessId(),
				process);
	
    }
    
    // Assume, for now, that the process has a single thread
    Thread* thread = new Thread(process);
    Assert(thread != NULL);
    
    // Add this thread to the thread group
    dm_threads.insert(dm_threads.end(), thread);
    
    // Add this thread to the process' list of references
    dm_processes.addReference(thread, process); 
}



/**
 * Attach to a POSIX thread.
 *
 * Attaches to an existing POSIX thread and adds that thread to this
 * application. The thread's status is not affected.
 *
 * @param pid     Process identifier of the process in which the thread resides.
 * @param tid     Thread identifier for the thread.
 * @param host    Name of the host on which the thread resides.
 */
void Application::attachPosixThread(const pid_t& pid, const pthread_t& tid,
				    const std::string& host)
{
    // TODO: implement
}



#ifdef HAVE_OPENMP
/**
 * Attach to an OpenMP thread.
 *
 * Attaches to an existing OpenMP thread and adds that thread to this
 * application. The thread's status is not affected.
 *
 * @param pid     Process identifier of the process in which the thread resides.
 * @param tid     Thread identifier for the thread.
 * @param host    Name of the host on which the thread resides.
 */
void Application::attachOpenMPThread(const pid_t& pid, const int& tid,
				     const std::string& host)
{
    // TODO: implement
}
#endif



#ifdef HAVE_ARRAY_SERVICES
/**
 * Attach to an array session.
 *
 * Attaches to to an existing array session and adds all threads within that
 * array session to this application. The threads' statuses are not affected.
 *
 * @param ash     Handle for the array session.
 * @param host    Name of the host on which the array session resides.
 */
void Application::attachArraySession(const ash_t& ash,
				     const std::string& host)
{
    // TODO: implement
}
#endif



/**
 * Remove a thread.
 *
 * Removes the specified thread from this application. The thread is removed
 * from any collectors to which it was previously attached and any performance
 * data that was associated with the thread is destroyed. The Thread object
 * itself is also destoryed, so the caller must insure that no pointers to this
 * thread are used once it has been removed. The real underlying thread in the
 * system is <em>not</em> destroyed. If the thread was in the suspended state,
 * it is put into the running state before being removed.
 *
 * @pre    Null threads cannot be removed. An exception of type
 *         std::invalid_argument is thrown if the thread is null.
 *
 * @pre    Threads must be in the application to be removed. An exception
 *         of type std::invalid_argument is thrown if the thread is not in the
 *         application.
 *
 * @param thread    Thread to be removed.
 */
void Application::removeThread(Thread* thread)
{
    Guard guard_myself(this);
    
    // Check preconditions
    
    if(thread == NULL)	
	throw std::invalid_argument("Cannot remove a null thread.");
    
    if(find(dm_threads.begin(), dm_threads.end(), thread) == dm_threads.end())
	throw std::invalid_argument("Cannot remove a thread that isn't in the "
				    "application.");
    
    // Iterate over each collector in this application
    for(CollectorGroup::const_iterator
	    i = dm_collectors.begin(); i != dm_collectors.end(); ++i) {
	
	// Get the collector's thread group
	ThreadGroup threads = (*i)->getThreads();
	
	// Is this thread within the collector's thread group?
	if(find(threads.begin(), threads.end(), thread) != threads.end())
	    
	    // Remove this thread from the collector
	    (*i)->removeThread(thread);
	
    }
    
    // Remove this thread from its process' list of references
    dm_processes.removeReference(thread, thread->dm_process);
    
    // Was this the last thread to reference this process?
    if(dm_processes.findReferencesByProcess(thread->dm_process).empty()) {
	
	// Remove this process from the process table
	dm_processes.removeProcess(thread->dm_process->getHost(),
				   thread->dm_process->getProcessId(),
				   thread->dm_process);
	
	// Destroy the process
	delete thread->dm_process;
	
    }
    
    // Remove this thread from the thread group
    dm_threads.erase(find(dm_threads.begin(), dm_threads.end(), thread));
    
    // Destroy the thread
    delete thread;    
}



/**
 * Get our collectors.
 *
 * Returns to the caller all collectors currently in this application. An
 * empty collector group is returned if this application doesn't contain any
 * collectors.
 *
 * @return    Collectors currently contained within this application.
 */
CollectorGroup Application::getCollectors() const
{
    Guard guard_myself(this);
    
    // Return the collectors to the caller
    return dm_collectors;
}



/**
 * Create a collector instance.
 *
 * Creates an instance of the collector with the specified unique identifier
 * and the instance to this application.
 *
 * @pre    Instances can only be created for collectors for which a corresond-
 *         ing plugin was found. An exception of type std::invalid_argument is
 *         thrown if a plugin was not found for the unique identifier.
 *
 * @param unique_id    Unique identifier of collector to be created.
 * @return             New instance of the named collector.
 */
Collector* Application::createCollector(const std::string& unique_id)
{
    // Create an instance of the named collector
    Collector* collector = 
	Collector::dm_plugin_table.instantiateCollector(unique_id);
    Assert(collector != NULL);
    
    // Add this collector to the collector group
    dm_collectors.insert(dm_collectors.end(), collector);
    
    // Return the new collector instance to the caller
    return collector;
}



/**
 * Remove a collector.
 *
 * Removes the specified collector from this application. Any performance data
 * gathered by this collector is destroyed as is the Collector object itself.
 * The caller must insure that no pointers to this collector are used once it
 * has been destroyed.
 *
 * @pre    Null collectors cannot be removed. An exception of type
 *         std::invalid_argument is thrown if the collector is null.
 *
 * @pre    Collectors must be in the application to be removed. An exception
 *         of type std::invalid_argument is thrown if the collector is not in
 *         the application.
 *
 * @param collector    Collector to be removed.
 */
void Application::removeCollector(Collector* collector)
{
    Guard guard_myself(this);

    // Check preconditions
    
    if(collector == NULL)
	throw std::invalid_argument("Cannot remove a null collector.");
    
    if(find(dm_collectors.begin(), dm_collectors.end(), collector) ==
       dm_collectors.end())
	throw std::invalid_argument("Cannot remove a collector that isn't in "
				    "the application.");
    
    // Remvoe this collector from the collector group
    dm_collectors.erase(find(dm_collectors.begin(), dm_collectors.end(),
			     collector));
    
    // Destroy the collector
    Collector::dm_plugin_table.destroyCollector(collector);
}



/**
 * Save collected data to a file.
 *
 * Saves all collected performance data to a file. Any information necessary to
 * recreate the application's current thread and collector state is also saved
 * to the file.
 *
 * @param filename    Path of file to be saved.
 */
void Application::saveFile(const Path& filename) const
{
    // TODO: implement
}



/**
 * Restore previously collected data from a file.
 *
 * Restores previously collected performance data from a file. All threads that
 * were present in the application are recreated with "terminated" status. All
 * collectors that were present in the application are also recreated in the
 * "not collecting" state. Any threads that existed in the application prior to
 * the restore are removed from the application first.
 *
 * @note    While Thread and Collector objects are created for each thread and
 *          collector that were present in the application. this does not imply
 *          real underlying threads are created. Restored threads cannot be
 *          continued. New collectors cannot be added to restored threads. And
 *          restored collectors cannot collect new data.
 *
 * @param filename    Path of file from which to restore.
 */
void Application::restoreFile(const Path& filename)
{
    // TODO: implement
}



//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// we haven't found it, so define it directly for now.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif



/**
 * Get the local host name.
 *
 * Returns to the caller the name of the host on which we are executing. This
 * information is obtained directly from the operating system.
 *
 * @return    Name of the local host.
 */
std::string Application::getLocalHost()
{
    // Obtain the local host name from the operating system
    char buffer[HOST_NAME_MAX];
    int retval = gethostname(buffer, HOST_NAME_MAX);
    Assert(retval == 0);
    
    // Return the local host name to the caller
    return buffer;
}



/**
 * Add a symbol table.
 *
 * Adds the passed symbol table to this application's list of symbol tables.
 * The Application object assumes responsibility for destroying the symbol table
 * when it is no longer needed.
 *
 * @param symbol_table    Symbol table to be added.
 */
void Application::addSymbolTable(const SymbolTable* symbol_table)
{
    // Check assertions
    Assert(symbol_table != NULL);
    
    // Insert this symbol table into our list of all symbol tables.
    dm_symbol_tables.push_back(const_cast<SymbolTable*>(symbol_table));
}



/**
 * Find a symbol table.
 *
 * Finds and returns the symbol table that corresponds to the linked object with
 * the specified path. A null value is returned if no symbol table was found.
 *
 * @todo    Currently the criteria for locating a matching symbol table is if
 *          the path names of the symbol table's linked objects are identical.
 *          This, technically, is inadequate since two hosts might have linked
 *          objects with the same name but different contents. For example, two
 *          different versions of "/lib/ld-linux.so.2". In order to address this
 *          shortcoming, a checksum (or similar) should also be compared.
 *
 * @param filename    Path of the linked object.
 * @return            Symbol table for this linked object or a null value if a
 *                    symbol table was not found.
 */
const SymbolTable* Application::findSymbolTable(const Path& filename) const
{    
    // Iterate over each symbol table looking for a matching entry
    for(std::vector<const SymbolTable*>::const_iterator
	    i = dm_symbol_tables.begin(); i != dm_symbol_tables.end(); ++i)
	if((*i)->getPath() == filename)
	    return *i;
    
    // Return NULL to indicate no matching symbol table was found
    return NULL;
}
