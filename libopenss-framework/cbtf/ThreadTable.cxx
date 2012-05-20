////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
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
#include "Frontend.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
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
    dm_thread_to_state(),
    dm_thread_to_callbacks()
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
    return dm_thread_to_state.empty();
}

/**
 * Set the number of backend processes requested
 *
 * @return    void
 */
void ThreadTable::setNumBE( uint64_t num_BE ) 
{
    Guard guard_myself(this);
 
    // Return the caller the empty state of this thread table
    number_of_backends = num_BE;
}


/**
 * Get the number of backend processes requested
 *
 * @return    number of backend processes
 */
uint64_t ThreadTable::getNumBE() const
{
    Guard guard_myself(this);
 
    // Return the caller the empty state of this thread table
    return number_of_backends;

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
 * @param thread             Thread to be added.
 * @param stdout_callback    Standard output stream callback.
 * @param stderr_callback    Standard error stream callback.
 */
void ThreadTable::addThread(const Thread& thread,
			    const OutputCallback stdout_callback,
			    const OutputCallback stderr_callback)
{    
    Guard guard_myself(this);

    // Find the entry (if any) for this thread
    std::map<Thread, Thread::State>::const_iterator i =
	dm_thread_to_state.find(thread);
    std::map<Thread, std::pair<OutputCallback, 
	                       OutputCallback> >::const_iterator j = 
	dm_thread_to_callbacks.find(thread);

    // Check assertions
    Assert(i == dm_thread_to_state.end());
    Assert(j == dm_thread_to_callbacks.end());

    // Add this thread
    dm_thread_to_state.insert(std::make_pair(thread, Thread::Disconnected));
    dm_thread_to_callbacks.insert(
        std::make_pair(thread, std::make_pair(stdout_callback, stderr_callback))
	);
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
    std::map<Thread, Thread::State>::iterator i = 
	dm_thread_to_state.find(thread);

    // Check assertions
    Assert(i != dm_thread_to_state.end());

    // Remove this thread
    dm_thread_to_state.erase(i);
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
				    const std::pair<bool, pthread_t>& tid) const
{
    Guard guard_myself(this);
    ThreadGroup threads;

    // Iterate over each thread in this thread table
    for(std::map<Thread, Thread::State>::const_iterator 
	    i = dm_thread_to_state.begin(); i != dm_thread_to_state.end(); ++i)

	// Add this thread if it matches the specified criteria
	if((i->first.getHost() == host) &&
	   (i->first.getProcessId() == pid) &&
	   (i->first.getPosixThreadId().first == tid.first) &&
	   (!tid.first || (i->first.getPosixThreadId().second == tid.second)))

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
Thread::State ThreadTable::getThreadState(const Thread& thread) const
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    std::map<Thread, Thread::State>::const_iterator i = 
	dm_thread_to_state.find(thread);

    // Return the thread's current state to the caller
    return (i == dm_thread_to_state.end()) ? Thread::Disconnected : i->second;
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
    std::map<Thread, Thread::State>::iterator i = 
	dm_thread_to_state.find(thread);

    // Check assertions
    Assert(i != dm_thread_to_state.end());
    
    // Set the thread's current state to the specified state
    i->second = state;
}



/**
 * Get a thread's stdout callback.
 *
 * Returns the standard output stream callback of the specified thread.
 *
 * @param thread    Thread whose stdout callback should be obtained.
 * @return          Standard output stream callback for that thread.
 */
OutputCallback ThreadTable::getStdoutCallback(const Thread& thread) const
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    std::map<Thread, std::pair<OutputCallback, 
	                       OutputCallback> >::const_iterator i = 
	dm_thread_to_callbacks.find(thread);

    // Return the thread's stdout callback to the caller
    return (i == dm_thread_to_callbacks.end()) ? 
	OutputCallback() : i->second.first;
}



/**
 * Get a thread's stderr callback.
 *
 * Returns the standard error stream callback of the specified thread.
 *
 * @param thread    Thread whose stderr callback should be obtained.
 * @return          Standard error stream callback for that thread.
 */
OutputCallback ThreadTable::getStderrCallback(const Thread& thread) const
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this thread
    std::map<Thread, std::pair<OutputCallback, 
	                       OutputCallback> >::const_iterator i = 
	dm_thread_to_callbacks.find(thread);

    // Return the thread's stderr callback to the caller
    return (i == dm_thread_to_callbacks.end()) ? 
	OutputCallback() : i->second.second;
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
bool ThreadTable::isConnected(const Thread& thread) const
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
    std::map<Thread, Thread::State>::iterator i = 
	dm_thread_to_state.find(thread);
    
    // Check assertions
    Assert(i != dm_thread_to_state.end());

    // Is the thread currently connected?
    bool is_connected = ((i->second != Thread::Disconnected) &&
			 (i->second != Thread::Connecting) &&
			 (i->second != Thread::Nonexistent));

    // Does the thread's host have an MRNet backend?
    //bool has_backend = Frontend::hasBackend(thread.getHost());
    bool has_backend = true;

    // Set the thread's state to "Connecting" if it isn't connected
    if(!is_connected && has_backend)
	i->second = Thread::Connecting;
    
    // Return flag indicating if this process was not connected to the caller
//    std::cerr << "SET CONNECTING RETURNS  is_connected " << is_connected << " has_backend " << has_backend << std::endl;
#if 0
    std::cerr << " i->second THREAD STATE is " << OpenSpeedShop::Framework::Callbacks::toString( i->second ) << std::endl;
#endif
    return !is_connected && has_backend;
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
	//if(!Frontend::hasBackend(i->getHost()) || !isConnected(*i)) {
	if( !isConnected(*i)) {
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
