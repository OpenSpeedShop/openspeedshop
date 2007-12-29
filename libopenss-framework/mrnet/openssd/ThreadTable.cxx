////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the ThreadTable class.
 *
 */

#include "Guard.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Utility.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton thread table. */
ThreadTable ThreadTable::TheTable;



/**
 * Get Dyninst thread object pointer for a thread.
 *
 * Returns the Dyninst thread object pointer for the specified thread. The
 * pointer is found by consulting Dyninst's list of active thread pointers
 * directly. A null pointer is returned if the thread cannot be found.
 *
 * @param thread    Thread whose Dyninst thread object pointer is to be found.
 * @return          Dyninst thread object pointer.
 */
BPatch_thread* ThreadTable::getPtrDirectly(const ThreadName& thread)
{
    // Return a null immediately if the specified thread isn't on this host
    if(getCanonicalName(thread.getHost()) != getCanonicalName(getLocalHost()))
	return NULL;
    
    // Get the POSIX thread identifier for this thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();
	
    // Get Dyninst's list of active processes
    BPatch* bpatch = BPatch::getBPatch();
    Assert(bpatch != NULL);
    BPatch_Vector<BPatch_process*>* processes = bpatch->getProcesses();
    Assert(processes != NULL);
    
    // Iterate over each active process
    for(int i = 0; i < processes->size(); ++i) {
	Assert((*processes)[i] != NULL);
	
	// Skip this process if it doesn't have the correct PID
	if(thread.getProcessId() != (*processes)[i]->getPid())
	    continue;
	
	// Get the list of threads in this process
	BPatch_Vector<BPatch_thread*> threads;
	(*processes)[i]->getThreads(threads);
	Assert(!threads.empty());

	// Iterate over each thread in this process
	for(int j = 0; j < threads.size(); ++j) {
	    Assert(threads[j] != NULL);
	    
	    // Return the pointer if this is the correct thread
	    if(!tid.first || (tid.second == threads[j]->getTid()))
		return threads[j];
	    
	}
	
    }
    
    // Otherwise return a null because the specified thread wasn't found
    return NULL;
}



/**
 * Default constructor.
 *
 * Constructs an empty thread table.
 */
ThreadTable::ThreadTable() :
    Lockable(),
    dm_name_to_ptr(),
    dm_ptr_to_names()
{
}



/**
 * Add a thread.
 *
 * Adds the passed thread, and the corresponding Dyninst thread object pointer,
 * to this thread table.
 *
 * @note    An assertion failure occurs if an attempt is made to add a thread
 *          more than once.
 *
 * @param thread    Thread to be added.
 * @param ptr       Dyninst thread object pointer.
 */
void ThreadTable::addThread(const ThreadName& thread, BPatch_thread* ptr)
{
    Guard guard_myself(this);

    // Find the entries (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::const_iterator i =
	dm_name_to_ptr.find(thread);
    std::multimap<BPatch_thread*, ThreadName>::const_iterator j =
	dm_ptr_to_names.lower_bound(ptr);
    for(; j != dm_ptr_to_names.upper_bound(ptr); ++j)
	if(j->second == thread)
	    break;
    
    // Check assertions
    Assert(i == dm_name_to_ptr.end());
    Assert(j == dm_ptr_to_names.upper_bound(ptr));
    
    // Add this thread
    dm_name_to_ptr.insert(std::make_pair(thread, ptr));
    dm_ptr_to_names.insert(std::make_pair(ptr, thread));
}



/**
 * Remove a thread.
 *
 * Removes the passed thread from this thread table.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a
 *          thread that isn't in this thread table.
 *
 * @param thread    Thread to be removed.
 */
void ThreadTable::removeThread(const ThreadName& thread, BPatch_thread* ptr)
{
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::iterator i =
	dm_name_to_ptr.find(thread);
    std::multimap<BPatch_thread*, ThreadName>::iterator j =
	dm_ptr_to_names.lower_bound(ptr);
    for(; j != dm_ptr_to_names.upper_bound(ptr); ++j)
	if(j->second == thread)
	    break;
    
    // Check assertions
    Assert(i != dm_name_to_ptr.end());
    Assert(j != dm_ptr_to_names.upper_bound(ptr));
    
    // Remove this thread
    dm_name_to_ptr.erase(i);
    dm_ptr_to_names.erase(j);
}



/**
 * Get Dyninst thread object pointer for a thread.
 *
 * Returns the Dyninst thread object pointer for the specified thread. A null
 * pointer is returned if the thread cannot be found.
 *
 * @param thread    Thread whose Dyninst thread object pointer is to be found.
 * @return          Dyninst thread object pointer for that thread.
 */
BPatch_thread* ThreadTable::getPtr(const ThreadName& thread) const
{
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::const_iterator i =
	dm_name_to_ptr.find(thread);

    // Return the Dyninst thread object pointer to the caller
    return (i != dm_name_to_ptr.end()) ? i->second : NULL;
}



/**
 * Get thread names for a Dyninst thread object pointer.
 *
 * Returns the thread names for the specified Dyninst thread object pointer.
 * An empty thread name group is returned if the pointer cannot be found.
 *
 * @param ptr    Dyninst thread object pointer whose names are to be found.
 * @return       Thread names for that pointer.
 */
ThreadNameGroup ThreadTable::getNames(BPatch_thread* ptr) const
{
    Guard guard_myself(this);

    // Find the entries (if any) for this Dyninst thread object pointer
    ThreadNameGroup names;
    for(std::multimap<BPatch_thread*, ThreadName>::const_iterator
	    j = dm_ptr_to_names.lower_bound(ptr);
	j != dm_ptr_to_names.upper_bound(ptr);
	++j)
	names.insert(j->second);
    
    // Return the thread names to the caller
    return names;
}
