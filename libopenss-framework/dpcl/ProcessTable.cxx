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
#include "Lockable.hxx"
#include "MainLoop.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "SmartPtr.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton process table. */
ProcessTable ProcessTable::TheTable;



/**
 * Default constructor.
 *
 * Performs any DPCL initializations that are necessary before the first process
 * is created and added to the process table. This consists of starting the DPCL
 * main loop, initializing the DPCL process termination and out-of-band data
 * handlers, and configuring any debugging variables.
 */
ProcessTable::ProcessTable() :
    Lockable(),
    std::map<std::string, std::pair<SmartPtr<Process>, ThreadGroup> >(),
    dm_dpcld_listener_port()
{
#ifndef NDEBUG
    if(Process::is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "ProcessTable::ProcessTable() starting DPCL main loop"
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Start the DPCL main loop
    dm_dpcld_listener_port = MainLoop::start();
    
    // Setup the process termination handler
    GCBFuncType old_callback;
    GCBTagType old_tag;	
    AisStatus retval = 
	Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
				      Process::terminationCallback, NULL,
				      &old_callback, &old_tag);
    Assert(retval.status() == ASC_success);	

    // Setup the thread creation/termination handler
    retval =
	Ais_override_default_callback(AIS_NEW_THREAD_LIST_AVAIL,
				      Process::threadListChangeCallback, NULL,
				      &old_callback, &old_tag);
    Assert(retval.status() == ASC_success);

    // Setup the dlopen/dlclose handler
    retval =
	Ais_override_default_callback(AIS_NEW_PROGRAM_OBJ_AVAIL,
				      Process::addressSpaceChangeCallback, NULL,
				      &old_callback, &old_tag);
    Assert(retval.status() == ASC_success);
    
    // Setup the out-of-band data handler
    retval = 
	Ais_override_default_callback(AIS_OUTOFBAND_DATA,
				      Process::outOfBandDataCallback, NULL,
				      &old_callback, &old_tag);
    Assert(retval.status() == ASC_success);
}



/**
 * Destructor.
 *
 * Performs any DPCL finalizations that are necessary after the last process is
 * destroyed and removed from the process table. This consists of stopping the
 * DPCL main loop.
 */
ProcessTable::~ProcessTable()
{
#ifndef NDEBUG
    if(Process::is_debug_perf_enabled)
	debugPerformanceStatistics();
#endif

    // Insure all processes are destroyed before stopping the DPCL main loop
    clear();
    
#ifndef NDEBUG
    if(Process::is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "ProcessTable::~ProcessTable() stopping DPCL main loop"
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Stop the DPCL main loop
    MainLoop::stop();
}



/**
 * Add a process.
 *
 * Adds the passed process to this process table. The process is automatically
 * associated with any threads in this table that are contained by it.
 *
 * @note    An assertion failure occurs if an attempt is made to add a process
 *          that already exists in this process table.
 *
 * @param process    Process to be added.
 */
void ProcessTable::addProcess(const SmartPtr<Process>& process)
{
    // Form the unique process name for this process
    std::string name = 
	Process::formUniqueName(process->getHost(), process->getProcessId());
    
    // Find the entry (if any) for this unique process name
    ProcessTable::iterator entry = find(name);

    // Create an entry for this unique process name if it didn't already exist
    if(entry == end())
	entry = insert(
	    std::make_pair(
		name, 
		std::make_pair(SmartPtr<Process>(), ThreadGroup()))
	    ).first;
    
    // Check assertions
    Assert(entry->second.first.isNull());
    
    // Place this process in the appropriate entry of the process table
    entry->second.first = process;
}



/**
 * Remove a process.
 *
 * Removes a process from this process table. Causes the process table to
 * release its smart pointer to this process. The process isn't destroyed
 * until all such smart pointer references to the process are released.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a
 *          process that isn't in this process table.
 *
 * @param process    Process to be removed.
 */
void ProcessTable::removeProcess(const SmartPtr<Process>& process)
{
    // Form the unique process name for this process
    std::string name = 
	Process::formUniqueName(process->getHost(), process->getProcessId());
    
    // Find the entry (if any) for this unique process name
    ProcessTable::iterator entry = find(name);  

    // Check assertions
    Assert(entry != end());
    Assert(!entry->second.first.isNull());

    // Remove this process
    entry->second.first = SmartPtr<Process>();
}



/**
 * Add a thread.
 *
 * Adds a thread to this process table. Adding a thread simply indicates to
 * the process table that the thread is in use. This information is used to
 * locate the process associated with this thread, and can be used to decide
 * when it is appropriate to remove that process from the process table.
 *
 * @note    An assertion failure occurs if an attempt is made to add a thread
 *          more than once.
 *
 * @param thread    Thread to be added.
 */
void ProcessTable::addThread(const Thread& thread)
{
    // Form the unique process name for this thread
    std::string name = 
	Process::formUniqueName(thread.getHost(), thread.getProcessId());

    // Find the entry (if any) for this unique process name
    ProcessTable::iterator entry = find(name);

    // Create an entry for this unique process name if it didn't already exist
    if(entry == end())
	entry = insert(
	    std::make_pair(
		name, 
		std::make_pair(SmartPtr<Process>(), ThreadGroup()))
	    ).first;
    
    // Find the thread within this entry
    ThreadGroup::iterator i = entry->second.second.find(thread);

    // Check assertions
    Assert(i == entry->second.second.end());
    
    // Add this thread to the appropriate entry of the process table
    entry->second.second.insert(thread); 
}



/**
 * Remove a thread.
 *
 * Removes a thread from this process table. Removing a thread simply indicates
 * to the process table that the thread is no longer used. This information can
 * be used to decide when it is appropriate to remove a process from the process
 * table.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a
 *          thread that isn't in this process table.
 *
 * @param thread    Thread to be removed.
 */
void ProcessTable::removeThread(const Thread& thread)
{
    // Form the unique process name for this thread
    std::string name = 
	Process::formUniqueName(thread.getHost(), thread.getProcessId());
    
    // Find the entry (if any) for this unique process name
    ProcessTable::iterator entry = find(name);
    
    // Check assertions
    Assert(entry != end());
    
    // Find the thread within this entry
    ThreadGroup::iterator i = entry->second.second.find(thread);
    
    // Check assertions
    Assert(i != entry->second.second.end());
    
    // Remove this thread
    entry->second.second.erase(i);
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
    ProcessTable::const_iterator entry = find(name);
    return (entry != end()) ? entry->second.first : SmartPtr<Process>();
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
    std::string name = 
	Process::formUniqueName(thread.getHost(), thread.getProcessId());
    ProcessTable::const_iterator entry = find(name);
    return (entry != end()) ? entry->second.first : SmartPtr<Process>();
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
ThreadGroup ProcessTable::getThreadsByName(const std::string& name) const
{
    ProcessTable::const_iterator entry = find(name);
    return (entry != end()) ? entry->second.second : ThreadGroup();
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
ThreadGroup
ProcessTable::getThreadsByProcess(const SmartPtr<Process>& process) const
{
    std::string name = 
	Process::formUniqueName(process->getHost(), process->getProcessId());
    ProcessTable::const_iterator entry = find(name);
    return (entry != end()) ? entry->second.second : ThreadGroup();
}



#ifndef NDEBUG
/**
 * Display performance statistics.
 *
 * Displays performance statistics for the processes in this process table
 * to the standard error stream. Reported information includes the minimum,
 * average, and maximum time spent during various phases of attaching to a
 * process.
 */
void ProcessTable::debugPerformanceStatistics()
{
    static const unsigned InitialIndent = 2;
    static const unsigned TimeWidth = 8;

    static const struct {

	/** Performance data event code. */
	Process::PerformanceDataEvents dm_event;
	
	/** Description of that event code. */
	std::string dm_description;

    } Table[] = {

	{ Process::Created, "Started (Process Object Created)" },
	{ Process::ConnectIssued, "DPCL connect() Issued" },
	{ Process::ConnectAcknowledged, "DPCL connect() Acknowledged" },
	{ Process::ConnectCompleted, "DPCL connect() Completed" },
	{ Process::AttachIssued, "DPCL attach() Issued" },
	{ Process::AttachAcknowledged, "DPCL attach() Acknowledged" },
	{ Process::AttachCompleted, "DPCL attach() Completed" },
	{ Process::GetThreadsIssued, "DPCL bget_threads() Issued" },
	{ Process::GetThreadsCompleted, "DPCL bget_threads() Completed" },
	{ Process::RqstAddrSpcEntered, "Enter requestAddressSpace()" },
	{ Process::RqstAddrSpcIssue, "Begin Issuing Symbol Table Requests" },
	{ Process::RqstAddrSpcExited, "Exit requestAddressSpace()" },
	{ Process::FSTPEntered, "Enter finishSymbolTableProcessing()" },
	{ Process::Ready, "Finished (Process Object Ready)" },

	{ Process::Ready, "" }  // End Of Table Entry

    };

    // Display the header
    std::cerr << std::endl << std::endl << std::endl
	      << std::setw(InitialIndent) << " "
	      << "SUMMARY OF ATTACH PERFORMANCE FOR " << size() << " PROCESS" 
	      << ((size() == 1) ? "" : "ES") << std::endl << std::endl
	      << std::setw(InitialIndent) << " "
	      << std::setw(TimeWidth) << "Minimum " << " "
	      << std::setw(TimeWidth) << "Average " << " "
	      << std::setw(TimeWidth) << "Maximum " << std::endl	
	      << std::setw(InitialIndent) << " "
	      << std::setw(TimeWidth) << "Time(mS)" << " "
	      << std::setw(TimeWidth) << "Time(mS)" << " "
	      << std::setw(TimeWidth) << "Time(mS)" << "  "
	      << "Event" << std::endl << std::endl;

    // Iterate over each performance data event to be shown
    for(unsigned i = 0; !Table[i].dm_description.empty(); ++i) {

	// Initialize statistics for this event
	uint64_t minimum = 0, average = 0, maximum = 0, num = 0;
	
	// Iterate over each process in this process table
	for(ProcessTable::const_iterator j = begin(); j != end(); ++j) {

	    // Skip this process if it didn't record this event
	    if(j->second.first->dm_perf_data.find(Table[i].dm_event) ==
	       j->second.first->dm_perf_data.end())
		continue;

	    // Calculate the "created-relative" event time for this process
	    Time::difference_type t = 
		j->second.first->dm_perf_data[Table[i].dm_event] -
		j->second.first->dm_perf_data[Process::Created];
	    
	    // Update the minimum, average, and maximum as appropriate
	    if((j == begin()) || (t < minimum))
		minimum = t;
	    average += t;
	    if((j == begin()) || (t > maximum))
		maximum = t;
	    ++num;	
   
	}

	// Complete the average calculation
	average = (num > 0) ? (average / num) : average;
		
	// Display this line of statistics
	std::cerr << std::setw(InitialIndent) << " "
		  << std::setw(TimeWidth) << (minimum / 1000000) << " "
		  << std::setw(TimeWidth) << (average / 1000000) << " "
		  << std::setw(TimeWidth) << (maximum / 1000000) << "  "
		  << Table[i].dm_description << std::endl;
	
    }

    // Display the tailer    
    std::cerr << std::endl << std::endl << std::endl;
}
#endif
