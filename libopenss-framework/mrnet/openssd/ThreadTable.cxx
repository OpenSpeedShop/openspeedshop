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
#include "ThreadTable.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton thread table. */
ThreadTable ThreadTable::TheTable;



/**
 * Default constructor.
 *
 * Constructs an empty thread table.
 */
ThreadTable::ThreadTable() :
    Lockable(),
    std::map<ThreadName, BPatch_thread*>()
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
    Guard guard_this(this);

    // Find the entry (if any) for this thread
    ThreadTable::const_iterator i = find(thread);
    
    // Check assertions
    Assert(i != end());
    
    // Add this thread
    insert(std::make_pair(thread, ptr));
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
void ThreadTable::removeThread(const ThreadName& thread)
{
    Guard guard_this(this);

    // Find the entry (if any) for this thread
    ThreadTable::iterator i = find(thread);
    
    // Check assertions
    Assert(i != end());
    
    // Remove this thread
    erase(i);
}



/**
 * Get Dyninst thread object pointer for a thread.
 *
 * Returns the Dyninst thread object pointer for the specified thread. A null
 * pointer is returned if the thread cannot be found.
 *
 * @param thread    Thread whose Dyninst thread object pointer is to be found.
 * @return          Dyninst thread object pointer.
 */
BPatch_thread* ThreadTable::getPtr(const ThreadName& thread)
{
    Guard guard_this(this);

    // Find the entry (if any) for this thread
    ThreadTable::const_iterator i = find(thread);

    // Return the Dyninst thread object pointer to the caller
    return (i != end()) ? i->second : NULL;
}
