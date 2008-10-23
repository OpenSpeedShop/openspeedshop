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
 * Definition of the InstrumentationTable class.
 *
 */

#include "Collector.hxx"
#include "ExecuteAtEntryOrExitEntry.hxx"
#include "ExecuteInPlaceOfEntry.hxx"
#include "ExecuteNowEntry.hxx"
#include "Guard.hxx"
#include "StopAtEntryOrExitEntry.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "InstrumentationTable.hxx"
#include "Utility.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton thread table. */
InstrumentationTable InstrumentationTable::TheTable;



/**
 * Default constructor.
 *
 * Constructs an empty instrumentation table.
 */
InstrumentationTable::InstrumentationTable() :
    Lockable(),
    dm_threads()
{
}



/**
 * Execute a library function now.
 *
 * Add instrumentation to execute the specified library function in the passed
 * thread. Also add a corresponding entry to this instrumentation table.
 *
 * @param thread              Thread in which the function should be executed.
 * @param collector           Collector requesting the execution.
 * @param disable_save_fpr    Boolean "true" if the floating-point registers
 *                            should NOT be saved before executing the library
 *                            function, or "false" if they should be saved.
 * @param callee              Name of the library function to be executed.
 * @param argument            Blob argument to the function.
 */
void InstrumentationTable::addExecuteNow(const ThreadName& thread,
					 const Collector& collector,
					 const bool& disable_save_fpr,
					 const std::string& callee,
					 const Blob& argument)
{
    Guard guard_myself(this);
    
    // Get the Dyninst pointer for this thread
    BPatch_thread* thread_ptr = ThreadTable::TheTable.getPtr(thread);
    Assert(thread_ptr != NULL);

    // Add this thread to the table (or find the existing entry)
    std::map<ThreadName, ThreadEntry>::iterator i =
	dm_threads.insert(std::make_pair(thread, ThreadEntry())).first;

    // Add this collector for the thread (or find the existing entry)
    std::map<Collector, InstrumentationList>::iterator j =
	i->second.dm_collectors.insert(
	    std::make_pair(collector, InstrumentationList())
	    ).first;

    // Create an instrumentation entry and add it to the list
    ExecuteNowEntry* entry = 
	new ExecuteNowEntry(*thread_ptr, disable_save_fpr, callee, argument);
    j->second.push_back(entry);

    // Install the instrumentation
    entry->install();
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * Add instrumentation to execute the specified library function every time
 * another function's entry or exit is executed in the passed thread. Also
 * add a corresponding entry to this instrumentation table.
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
void InstrumentationTable::addExecuteAtEntryOrExit(const ThreadName& thread,
						   const Collector& collector,
						   const std::string& where,
						   const bool& at_entry,
						   const std::string& callee,
						   const Blob& argument)
{
    Guard guard_myself(this);

    // Get the Dyninst pointer for this thread
    BPatch_thread* thread_ptr = ThreadTable::TheTable.getPtr(thread);
    Assert(thread_ptr != NULL);

    // Add this thread to the table (or find the existing entry)
    std::map<ThreadName, ThreadEntry>::iterator i =
	dm_threads.insert(std::make_pair(thread, ThreadEntry())).first;

    // Add this collector for the thread (or find the existing entry)
    std::map<Collector, InstrumentationList>::iterator j =
	i->second.dm_collectors.insert(
	    std::make_pair(collector, InstrumentationList())
	    ).first;

    // Create an instrumentation entry and add it to the list
    ExecuteAtEntryOrExitEntry* entry = 
	new ExecuteAtEntryOrExitEntry(*thread_ptr, where, at_entry,
				      callee, argument);
    j->second.push_back(entry);
    
    // Install the instrumentation
    entry->install();
}



/**
 * Execute a library function in place of another function.
 *
 * Add instrumentation to execute the specified library function in place of
 * another function every other time that other function is called in the
 * passed thread. Also add a corresponding entry to this instrumentation
 * table.
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function to be replaced with the
 *                     library function.
 * @param callee       Name of the library function to be executed.
 */
void InstrumentationTable::addExecuteInPlaceOf(const ThreadName& thread,
					       const Collector& collector,
					       const std::string& where,
					       const std::string& callee)
{
    Guard guard_myself(this);

    // Get the Dyninst pointer for this thread
    BPatch_thread* thread_ptr = ThreadTable::TheTable.getPtr(thread);
    Assert(thread_ptr != NULL);

    // Add this thread to the table (or find the existing entry)
    std::map<ThreadName, ThreadEntry>::iterator i =
	dm_threads.insert(std::make_pair(thread, ThreadEntry())).first;

    // Add this collector for the thread (or find the existing entry)
    std::map<Collector, InstrumentationList>::iterator j =
	i->second.dm_collectors.insert(
	    std::make_pair(collector, InstrumentationList())
	    ).first;

    // Create an instrumentation entry and add it to the list
    ExecuteInPlaceOfEntry* entry = 
	new ExecuteInPlaceOfEntry(*thread_ptr, where, callee);
    j->second.push_back(entry);
    
    // Install the instrumentation
    entry->install();
}



/**
 * Stop at a function's entry or exit.
 *
 * Add instrumentation to stop every time the specified function's entry or
 * exit is executed in the passed thread. Also add a corresponding entry to
 * this instrumentation table.
 *
 * @param thread      Thread which should be stopped.
 * @param where       Name of the function at whose entry/exit the stop
 *                    should occur.
 * @param at_entry    Boolean "true" if instrumenting function's entry
 *                    point, or "false" if function's exit point.
 */
void InstrumentationTable::addStopAtEntryOrExit(const ThreadName& thread,
						const std::string& where,
						const bool& at_entry)
{
    Guard guard_myself(this);

    // Get the Dyninst pointer for this thread
    BPatch_thread* thread_ptr = ThreadTable::TheTable.getPtr(thread);
    Assert(thread_ptr != NULL);

    // Add this thread to the table (or find the existing entry)
    std::map<ThreadName, ThreadEntry>::iterator i =
	dm_threads.insert(std::make_pair(thread, ThreadEntry())).first;

    // Create an instrumentation entry and add it to the list
    StopAtEntryOrExitEntry* entry = 
	new StopAtEntryOrExitEntry(*thread_ptr, where, at_entry);
    i->second.dm_general.push_back(entry);
    
    // Install the instrumentation
    entry->install();
}



/**
 * Remove instrumentation from a thread.
 *
 * Removes the passed thread from this instrumentation table after removing
 * all instrumentation from that thread.
 *
 * @note    Any attempt to remove instrumentation for a thread that has no
 *          instrumentation is silently ignored.
 *
 * @param thread    Thread from which instrumentation should be removed.
 */
void InstrumentationTable::removeInstrumentation(const ThreadName& thread)
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    std::map<ThreadName, ThreadEntry>::iterator i = dm_threads.find(thread);
    
    // Go no further if there is no instrumentation for this thread
    if(i == dm_threads.end())
	return;

    // Note: Ideally we'd use a reverse iterators here, but unfortunately
    //       the GCC 3.4.x compilers don't seem to support such iterators
    //       on STL vectors. For now we use simply index notion instead.
    
    // Iterate over each collector with instrumentation in this thread
    for(std::map<Collector, InstrumentationList>::const_iterator
	    j = i->second.dm_collectors.begin(); 
	j != i->second.dm_collectors.end(); 
	++j)
	
	// Iterate over all instrumentation associated with this collector
	for(InstrumentationList::size_type k = j->second.size(); k > 0; --k) {
	    
	    // Remove this instrumentation from the thread
	    j->second[k - 1]->remove();

	    // Destroy this instrumentation entry
	    delete j->second[k - 1];

	}

    // Iterate over all instrumentation not associated with a collector
    for(InstrumentationList::size_type
	    j = i->second.dm_general.size(); j > 0; --j) {

        // Remove this instrumentation from the thread
        i->second.dm_general[j - 1]->remove();

        // Destroy this instrumentation entry
        delete i->second.dm_general[j - 1];
	
    }

    // Remove this thread from the table
    dm_threads.erase(i);
}



/**
 * Remove instrumentation from a thread for a collector.
 *
 * Removes all instrumentation associated with the specified collector from
 * the specified thread and removes it from this instrumentation table.
 *
 * @note    Any attempt to remove instrumentation for a thread which the
 *          collector hasn't instrumented is silently ignored.
 *
 * @param thread       Thread from which instrumentation should be removed.
 * @param collector    Collector which is removing instrumentation.
 */
void InstrumentationTable::removeInstrumentation(const ThreadName& thread,
						 const Collector& collector)
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    std::map<ThreadName, ThreadEntry>::iterator i = dm_threads.find(thread);
    
    // Go no further if there is no instrumentation for this thread
    if(i == dm_threads.end())
	return;

    // Find the entry (if any) for this collector
    std::map<Collector, InstrumentationList>::iterator j =
	i->second.dm_collectors.find(collector);

    // Go no further if there is no instrumentation for this collector
    if(j == i->second.dm_collectors.end())
	return;

    // Note: Ideally we'd use a reverse iterator here, but unfortunately
    //       the GCC 3.4.x compilers don't seem to support such iterators
    //       on STL vectors. For now we use simply index notion instead.

    // Iterate over all instrumentation associated with this collector
    for(InstrumentationList::size_type k = j->second.size(); k > 0; --k) {

	// Remove this instrumentation from the thread
	j->second[k - 1]->remove();
	
	// Destroy this instrumentation entry
	delete j->second[k - 1];
	
    }
    
    // Remove this collector's instrumentation from the table
    i->second.dm_collectors.erase(j);
}


#include <iostream>
/**
 * Copy instrumentation from one thread to another.
 *
 * Copies all instrumentation from the specified source thread to the specified
 * destination thread and add the new instrumentation to this instrumentation
 * table.
 *
 * @note    Any attempt to copy instrumentation from a thread that has no
 *          instrumentation is silently ignored.
 *
 * @param source         Thread from which instrumentation should be copied.
 * @param destination    Thread to which instrumentation should be copied.
 * @return               Set of collectors that instrumented the thread.
 */
std::set<Collector>
InstrumentationTable::copyInstrumentation(const ThreadName& source,
					  const ThreadName& destination)
{
    Guard guard_myself(this);

    // Find the entry (if any) for the source thread
    std::map<ThreadName, ThreadEntry>::iterator i = dm_threads.find(source);
    
    // Go no further if there is no instrumentation for this thread
    if(i == dm_threads.end())
	return std::set<Collector>();

    // Get the Dyninst pointer for the destination thread
    BPatch_thread* thread = ThreadTable::TheTable.getPtr(destination);
    Assert(thread != NULL);

    // Is this the first time the destination thread was encountered?
    bool is_new_thread = (dm_threads.find(destination) == dm_threads.end());

    // Add the destination thread to the table (or find the existing entry)
    std::map<ThreadName, ThreadEntry>::iterator j =
	dm_threads.insert(std::make_pair(destination, ThreadEntry())).first;

    // Iterate over all instrumentation not associated with a collector
    if(is_new_thread)
	for(InstrumentationList::const_iterator
		k = i->second.dm_general.begin(); 
	    k != i->second.dm_general.end(); 
	    ++k) {
	    
	    // Copy this instrumentation entry and add it to the list
	    InstrumentationEntry* entry = (*k)->copy(*thread);
	    j->second.dm_general.push_back(entry);
	    
	    // Install the instrumentation
	    entry->install();
	    
	}

    // Iterate over each collector with instrumentation in the source thread
    std::set<Collector> collectors;
    for(std::map<Collector, InstrumentationList>::const_iterator
	    k = i->second.dm_collectors.begin(); 
	k != i->second.dm_collectors.end(); 
	++k) {

	// Skip this collector if it already instrumented the destination thread
	if(j->second.dm_collectors.find(k->first) !=
	   j->second.dm_collectors.end())
	    continue;

	// Add this collector to the set that instrumented the thread
	collectors.insert(k->first);
	
	// Add this collector for the destination thread (or get existing entry)
	std::map<Collector, InstrumentationList>::iterator l =
	    j->second.dm_collectors.insert(
	        std::make_pair(k->first, InstrumentationList())
		).first;

	// Iterate over all instrumentation associated with this collector
	for(InstrumentationList::const_iterator
		m = k->second.begin(); m != k->second.end(); ++m) {

	    // Copy this instrumentation entry and add it to the list
	    l->second.push_back((*m)->copy(*thread));
	    
	}

	// Iterate over all instrumentation associated with this collector
	for(InstrumentationList::iterator
		m = l->second.begin(); m != l->second.end(); ++m) {

	    // Install the instrumentation
	    (*m)->install();

	}

    }

    // Return the set of collectors that instrumented the thread
    return collectors;
}
