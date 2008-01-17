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
 * Definition of the ThreadTable class.
 *
 */

#include "Exception.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
#include "Protocol.h"
#include "ThreadGroup.hxx"
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
    std::map<Thread, Thread::State>()
{
}



/**
 * Test if empty.
 *
 * Returns a boolean value indicating if the thread table is empty.
 *
 * @return    Boolean "true" if the thread table is empty, "false" otherwise.
 */
bool ThreadTable::isEmpty() const
{
    Guard guard_myself(this);
 
    // Return the caller the empty state of this thread table
    return empty();
}



/**
 * Add a thread.
 *
 * Adds the passed thread to this thread table. Adding a thread simply indicates
 * to the thread table that the thread is in use.
 *
 * @note    An assertion failure occurs if an attempt is made to add a thread
 *          more than once.
 *
 * @param thread    Thread to be added.
 */
void ThreadTable::addThread(const Thread& thread)
{    
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    ThreadTable::const_iterator i = find(thread);

    // Check assertions
    Assert(i == end());

    // Add this thread
    insert(std::make_pair(thread, Thread::Disconnected));
}



/**
 * Remove a thread.
 *
 * Removes the passed thread from this thread table. Removing a thread simply
 * indicates to the thread table that the thread is no longer used.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a
 *          thread that isn't in this thread table.
 *
 * @param thread    Thread to be removed.
 */
void ThreadTable::removeThread(const Thread& thread)
{
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    ThreadTable::iterator i = find(thread);

    // Check assertions
    Assert(i != end());

    // Remove this thread
    erase(i);
}



/**
 * Get threads.
 *
 * Returns all the threads in this thread table that match the specified host,
 * process identifier, and, optionally, thread identifier. An empty thread group
 * is returned if no threads match.
 *
 * @param host    Name of the host on which the thread resides.
 * @param pid     Process identifier for the thread.
 * @param tid     Identifier of this thread.
 * @return        Threads which match this criteria.
 */
ThreadGroup ThreadTable::getThreads(const std::string& host,
				    const pid_t& pid,
				    const std::pair<bool, pthread_t>& tid)
{
    Guard guard_myself(this);
    ThreadGroup threads;

    // Iterate over each thread in this thread table
    for(std::map<Thread, Thread::State>::const_iterator 
	    i = begin(); i != end(); ++i)

	// Add this thread if it matches the specified criteria
	if((i->first.getHost() == host) &&
	   (i->first.getProcessId() == pid) &&
	   (i->first.getPosixThreadId() == tid))

	    threads.insert(i->first);

    // Return the threads to the caller
    return threads;    
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
Thread::State ThreadTable::getThreadState(const Thread& thread)
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    ThreadTable::const_iterator i = find(thread);

    // Return the thread's current state to the caller
    return (i == end()) ? Thread::Disconnected : i->second;
}



/**
 * Set a thread's state.
 *
 * Sets the current state of the specified thread. Used by the instrumentor
 * to update the current state of a thread when necessary.
 *
 * @param thread    Thread whose state should be set.
 * @param state     Set this state for the thread.
 */
void ThreadTable::setThreadState(const Thread& thread, 
				 const Thread::State& state)
{
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    ThreadTable::iterator i = find(thread);

    // Check assertions
    Assert(i != end());
    
    // Set the thread's current state to the specified state
    i->second = state;
}



/**
 * Test if a thread is connected.
 *
 * Returns a boolean value indicating if this thread is connected or not.
 *
 * @param thread    Thread whose state should be checked.
 * @return          Boolean "true" if this thread is connected, or "false"
 *                  otherwise.
 */
bool ThreadTable::isConnected(const Thread& thread)
{
    // Find the current state of this thread
    Thread::State current = getThreadState(thread);

    // Return flag indicating if this process is connected to the caller
    return ((current != Thread::Disconnected) &&
            (current != Thread::Connecting) &&
            (current != Thread::Nonexistent));
}



/**
 * Set a thread's state to connecting.
 *
 * Sets the current state of the specified thread to "Connecting" if the
 * thread isn't already connected. Returns a boolean value indicating if
 * the thread was connected.
 *
 * @param thread    Thread whose state should be set to "Connecting".
 * @return          Boolean "true" if this thread was not connected and
 *                  its state was changed, or "false" otherwise.
 */
bool ThreadTable::setConnecting(const Thread& thread)
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    ThreadTable::iterator i = find(thread);
    
    // Check assertions
    Assert(i != end());

    // Is the thread currently connected?
    bool is_connected = ((i->second != Thread::Disconnected) &&
			 (i->second != Thread::Connecting) &&
			 (i->second != Thread::Nonexistent));

    // Set the thread's state to "Connecting" if it isn't connected
    if(!is_connected)
	i->second = Thread::Connecting;
    
    // Return flag indicating if this process was not connected to the caller
    return !is_connected;
}



/**
 * Validate threads.
 *
 * Validates the existence and connectedness of the specified threads. If the
 * threads are found and connected, this function simply returns. Otherwise a
 * ThreadUnavailable or ProcessUnavailable exception is thrown.
 *
 * @param threads    Threads to be validated.
 */
void ThreadTable::validateThreads(const ThreadGroup& threads)
{
    Guard guard_myself(this);

    // Iterate over each thread of this group
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i)
	
	// Check preconditions
	if(!isConnected(*i)) {
	    std::pair<bool, pthread_t> tid = i->getPosixThreadId();
	    if(tid.first)
		throw Exception(Exception::ThreadUnavailable, i->getHost(),
				Exception::toString(i->getProcessId()),
				Exception::toString(tid.second));
	    else
		throw Exception(Exception::ProcessUnavailable, i->getHost(),
				Exception::toString(i->getProcessId()));	
	}
}
