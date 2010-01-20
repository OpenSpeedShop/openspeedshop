////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008-2009 Krell Institute. All Rights Reserved.
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

#include "ExperimentGroup.hxx"
#include "Guard.hxx"
#include "StdStreamPipes.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Utility.hxx"
#include <iostream>


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
    dm_ptr_to_names(),
    dm_ptr_to_pipes(),
    dm_ptr_to_state()
{
}



/**
 * Add a thread.
 *
 * Adds the passed thread, and the corresponding Dyninst thread object pointer,
 * to this thread table. Standard stream pipes can also optionally be associated
 * with this thread.
 *
 * @note    An assertion failure occurs if an attempt is made to add a thread
 *          more than once.
 *
 * @param thread    Thread to be added.
 * @param ptr       Dyninst thread object pointer.
 * @param pipes     Pipes associated with the standard streams of this thread.
 */
void ThreadTable::addThread(const ThreadName& thread, BPatch_thread* ptr,
			    const SmartPtr<StdStreamPipes>& pipes)
{
    Guard guard_myself(this);

#if 0
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();
    if (tid.first) {
      std:: cout << "OpenSpeedShop::ThreadTable::addThread() tid.second=" << tid.second << std::endl;
    } else {
      std:: cout << "OpenSpeedShop::ThreadTable::addThread() NO TID"  << std::endl;
    }
    std:: cout << "OpenSpeedShop::ThreadTable::addThread() thread.getProcessId()=" << thread.getProcessId() << std::endl;

#endif

    // Find the entries (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::const_iterator i =
	dm_name_to_ptr.find(thread);
    std::multimap<BPatch_thread*, ThreadName>::const_iterator j =
	dm_ptr_to_names.lower_bound(ptr);
    for(; j != dm_ptr_to_names.upper_bound(ptr); ++j)
	if(j->second == thread)
	    break;
    std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator k =
	dm_ptr_to_pipes.find(ptr);
    std::map<BPatch_thread*, OpenSS_Protocol_ThreadState>::const_iterator l =
	dm_ptr_to_state.find(ptr);
    
    // Check assertions
    Assert(i == dm_name_to_ptr.end());
    Assert(j == dm_ptr_to_names.upper_bound(ptr));
    Assert((k == dm_ptr_to_pipes.end()) || (k->second == pipes));
    Assert(l == dm_ptr_to_state.end());
    
    // Add this thread
    dm_name_to_ptr.insert(std::make_pair(thread, ptr));
    dm_ptr_to_names.insert(std::make_pair(ptr, thread));
    dm_ptr_to_pipes.insert(std::make_pair(ptr, pipes));
    dm_ptr_to_state.insert(std::make_pair(ptr, Disconnected));
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
    std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::iterator k =
	dm_ptr_to_pipes.find(ptr);
    std::map<BPatch_thread*, OpenSS_Protocol_ThreadState>::iterator l =
	dm_ptr_to_state.find(ptr);
    
    // Check assertions
    Assert(i != dm_name_to_ptr.end());
    Assert(j != dm_ptr_to_names.upper_bound(ptr));
    Assert(k != dm_ptr_to_pipes.end());
    Assert(l != dm_ptr_to_state.end());
    
    // Remove this thread
    dm_name_to_ptr.erase(i);
    dm_ptr_to_names.erase(j);
    if(dm_ptr_to_names.count(ptr) == 0)
	dm_ptr_to_pipes.erase(k);
    dm_ptr_to_state.erase(l);
}

std::vector<std::pair<pid_t, std::pair<std::pair<bool, pthread_t>, std::string> > > ThreadTable::getActivePidsAndHosts() const
{
    Guard guard_myself(this);
    std::vector< std::pair< pid_t, std::pair< std::pair< bool, pthread_t >, std::string > > > returnSet;

    returnSet.clear();

    // Find the entry (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::const_iterator i = dm_name_to_ptr.begin();
    for(; i!=dm_name_to_ptr.end(); ++i) {

#if 0
       .... jeg --- moved this to the Watcher code.  Now return the canonical name from this routine, as it did before.

       // Kind of Kludge - the database names are created from gethostname (non-canonical) but getHost returns canonical.
       // So, here we look for the first '.' and take the characters prior to it as the non-canonical name
       std::string tmp_str =  i->first.getHost();
       int dot_loc = i->first.getHost().find('.');
       std::string new_tmp_str = tmp_str.substr (0,dot_loc);
#endif

#if 0
       std::cout << "ThreadTable::getActivePidsAndHosts(), i->first.getProcessId()=" << i->first.getProcessId() << std::endl;
       std::cout << "ThreadTable::getActivePidsAndHosts(), i->first.getHost()=" << i->first.getHost() << std::endl;
//       std::cout << "ThreadTable::getActivePidsAndHosts(), i->first.getPosixThreadId()=" << i->first.getPosixThreadId() << std::endl;
//       std::cout << "ThreadTable::getActivePidsAndHosts(), new_tmp_str=" << new_tmp_str << std::endl;
#endif

       returnSet.push_back(std::make_pair( 
                              i->first.getProcessId(), 
                              std::make_pair(
                                     i->first.getPosixThreadId(), 
                                     i->first.getHost())
                              ));

    }
    // Return the Dyninst thread object pointer to the caller
    return (returnSet);
}


/**
 * Get list of pids that are now active
 *
 * Returns the set of pids now active (that are being monitored) in the daemon.
 *
 * @return          Set of pids that are being monitored
 */
std::set<pid_t> ThreadTable::getActivePids() const
{
    Guard guard_myself(this);
    std::set<pid_t> returnSet;

    returnSet.clear();

    // Find the entry (if any) for this thread
    std::map<ThreadName, BPatch_thread*>::const_iterator i = dm_name_to_ptr.begin();
    for(; i!=dm_name_to_ptr.end(); ++i) {
#if 0
       std::cout << "ThreadTable::getActivePids(), i->first.getProcessId()=" << i->first.getProcessId() << std::endl; 
       std::cout << "ThreadTable::getActivePids(), i->first.getHost()=" << i->first.getHost() << std::endl; 
#endif
       returnSet.insert(i->first.getProcessId());
    }
    // Return the Dyninst thread object pointer to the caller
    return (returnSet);
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
ThreadNameGroup ThreadTable::getNames(/* const */ BPatch_thread* ptr) const
{
    Guard guard_myself(this);

    // Find the entries (if any) for this Dyninst thread object pointer
    ThreadNameGroup names;
    for(std::multimap<BPatch_thread*, ThreadName>::const_iterator
	    i = dm_ptr_to_names.lower_bound(ptr);
	i != dm_ptr_to_names.upper_bound(ptr);
	++i)
	names.insert(i->second);
    
    // Return the thread names to the caller
    return names;
}



/**
 * Get thread names for a Dyninst process object pointer.
 *
 * Returns the thread names for the specified Dyninst process object pointer.
 * An empty thread name group is returned if none of the threads from this
 * process can be found.
 *
 * @param process    Dyninst process object pointer whose names are to be found.
 * @return           Thread names for all threads in that process.
 */
ThreadNameGroup ThreadTable::getNames(/* const */ BPatch_process* process) const
{
    Guard guard_myself(this);

    // Get the list of threads in this process
    BPatch_Vector<BPatch_thread*> threads;
    process->getThreads(threads);

    ThreadNameGroup names;

    // We seem to be racing with dyninst when openmpi forks the
    // mpirun into a new process and then forks that process into
    // the actuall rank process. The forked mpirun appears to be gone.
    // Seems safe to just return an empty ThreadNameGroup.
    // Fixes the odd aborts seen with openmpi 1.3.3 and nbody.
    // Also needed to not detach from the forked child in Dyninst::postFork.
    //  
    // Disabled this assert: Assert(!threads.empty());
    if (threads.empty()) {
	return names;
    }

    // Iterate over each thread in this process
    for(int i = 0; i < threads.size(); ++i) {
	Assert(threads[i] != NULL);
	
	// Find the entries (if any) for this Dyninst thread object pointer
	for(std::multimap<BPatch_thread*, ThreadName>::const_iterator
		j = dm_ptr_to_names.lower_bound(threads[i]);
	    j != dm_ptr_to_names.upper_bound(threads[i]);
	    ++j)
	    names.insert(j->second);
    }
    
    // Return the thread names to the caller
    return names;
}



/**
 * Get experiments for a Dyninst process object pointer.
 *
 * Returns the group of experiments which contain the specified Dyninst process
 * object pointer. An empty experiment group is returned if no experiments are
 * found.
 *
 * @param process    Dyninst process object pointer whose experiments are
 *                   to be found.
 * @return           Group of experiments containing that process.
 */
ExperimentGroup ThreadTable::getExperiments(
    /* const */ BPatch_process& process
    ) const
{
    Guard guard_myself(this);

    // Get the host and PID of this Dyninst process object pointer
    std::string host(getCanonicalName(getLocalHost()));
    pid_t pid = process.getPid();
    
    // Find the experiments for this Dyninst process object pointer
    ExperimentGroup experiments;
    for(std::map<ThreadName, BPatch_thread*>::const_iterator
	    i = dm_name_to_ptr.begin(); i != dm_name_to_ptr.end(); ++i)
	if((i->first.getHost() == host) && (i->first.getProcessId() == pid))
	    experiments.insert(Experiment(i->first));
    
    // Return the experiment group to the caller
    return experiments;
}



/**
 * Get standard stream pipes for a Dyninst thread object pointer.
 *
 * Returns the standard stream pipes for the specified Dyninst thread object
 * pointer. A null pointer is returned if the pointer cannot be found.
 *
 * @param ptr    Dyninst thread object pointer whose pipes are to be found.
 * @return       Standard stream pipes for that pointer.
 */
SmartPtr<StdStreamPipes>
ThreadTable::getStdStreamPipes(/* const */ BPatch_thread* ptr) const
{    
    Guard guard_myself(this);

    // Find the entry (if any) for this Dyninst thread object pointer
    std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator i =
	dm_ptr_to_pipes.find(ptr);

    // Return the pipes to the caller
    return (i != dm_ptr_to_pipes.end()) ? 
	i->second : SmartPtr<StdStreamPipes>();
}



/**
 * Get the standard input stream file descriptors.
 *
 * Returns the set of standard input stream file descriptors for use by the
 * backend. An empty set is returned if none of the threads have standard
 * stream pipes associated with them.
 *
 * @return    Standard input stream file descriptors.
 */
std::set<int> ThreadTable::getStdInFDs() const
{
    Guard guard_myself(this);

    // Find the standard input stream file descriptors for the backend
    std::set<int> fds;
    for(std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator
	    i = dm_ptr_to_pipes.begin(); i != dm_ptr_to_pipes.end(); ++i)
	if(!i->second.isNull())
	    fds.insert(i->second->getStdInForBackend());
    
    // Return the file descriptors to the caller
    return fds;   
}



/**
 * Get the standard error stream file descriptors.
 *
 * Returns the set of standard error stream file descriptors for use by the
 * backend. An empty set is returned if none of the threads have standard
 * stream pipes associated with them.
 *
 * @return    Standard error stream file descriptors.
 */
std::set<int> ThreadTable::getStdErrFDs() const
{
    Guard guard_myself(this);

    // Find the standard error stream file descriptors for the backend
    std::set<int> fds;
    for(std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator
	    i = dm_ptr_to_pipes.begin(); i != dm_ptr_to_pipes.end(); ++i)
	if(!i->second.isNull())
	    fds.insert(i->second->getStdErrForBackend());
    
    // Return the file descriptors to the caller
    return fds;   
}



/**
 * Get the standard output stream file descriptors.
 *
 * Returns the set of standard output stream file descriptors for use by the
 * backend. An empty set is returned if none of the threads have standard
 * stream pipes associated with them.
 *
 * @return    Standard output stream file descriptors.
 */
std::set<int> ThreadTable::getStdOutFDs() const
{
    Guard guard_myself(this);

    // Find the standard output stream file descriptors for the backend
    std::set<int> fds;
    for(std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator
	    i = dm_ptr_to_pipes.begin(); i != dm_ptr_to_pipes.end(); ++i)
	if(!i->second.isNull())
	    fds.insert(i->second->getStdOutForBackend());
    
    // Return the file descriptors to the caller
    return fds;   
}



/**
 * Get thread names for a standard stream file descriptor.
 *
 * Returns the thread names for the specified standard stream file descriptor.
 * An empty thread name group is returned if the file descriptor cannot be
 * found.
 *
 * @note    Currently a linear search is being used to find the thread names.
 *          The assumption is that the number of threads which actually have
 *          stream pipes should be very small. If this is found to no longer
 *          hold true, some type of map from file descriptors to names might
 *          become necessary.
 *
 * @param fd    File descriptor whose names are to be found.
 * @return      Thread names for that file descriptor.
 */
ThreadNameGroup ThreadTable::getNames(const int& fd) const
{
    Guard guard_myself(this);

    // Find the entries (if any) for this file descriptor
    ThreadNameGroup names;
    for(std::map<BPatch_thread*, SmartPtr<StdStreamPipes> >::const_iterator
	    i = dm_ptr_to_pipes.begin(); i != dm_ptr_to_pipes.end(); ++i)
	if(!i->second.isNull() &&
	   ((i->second->getStdInForBackend() == fd) ||
	    (i->second->getStdOutForBackend() == fd) ||
	    (i->second->getStdErrForBackend() == fd))) {
	    ThreadNameGroup names_to_insert = getNames(i->first);
	    names.insert(names_to_insert.begin(), names_to_insert.end());
	}
    
    // Return the thread names to the caller
    return names;
}



/**
 * Get a thread's state.
 *
 * Returns the current state of the thread for the specified Dyninst thread
 * object pointer. The "Nonexistent" state is returned if the specified thread
 * isn't in this thread table.
 *
 * @param ptr    Dyninst thread object pointer for the thread whose state
 *               should be obtained.
 * @return       Current state of the thread or "Nonexistent" if the thread
 *               could not be found.
 */
OpenSS_Protocol_ThreadState
ThreadTable::getThreadState(/* const */ BPatch_thread* ptr) const
{
    Guard guard_myself(this);

    // Find the entry (if any) for this Dyninst thread object pointer
    std::map<BPatch_thread*, OpenSS_Protocol_ThreadState>::const_iterator i =
	dm_ptr_to_state.find(ptr);

    // Return the thread's current state to the caller
    return (i == dm_ptr_to_state.end()) ? Nonexistent : i->second;
}



/**
 * Set a thread's state.
 *
 * Sets the current state of the thread for the specified Dyninst thread
 * object pointer to the passed value.
 *
 * @param ptr      Dyninst thread object pointer for the thread whose
 *                 state should be set.
 * @param state    State to which this thread should be set.
 */
void ThreadTable::setThreadState(/* const */ BPatch_thread* ptr,
				 const OpenSS_Protocol_ThreadState& state)
{
    Guard guard_myself(this);
    
    // Find the entry (if any) for this Dyninst thread object pointer
    std::map<BPatch_thread*, OpenSS_Protocol_ThreadState>::iterator i =
	dm_ptr_to_state.find(ptr);

    // Set the thread's current state to the specified state (if it was found)
    if(i != dm_ptr_to_state.end())
	i->second = state;
}



/**
 * Set threads' state.
 *
 * Sets the current state of every thread in the specified thread name group
 * to the passed value.
 *
 * @param threads    Threads whose state should be set.
 * @param state      State to which these threads should be set.
 */
void ThreadTable::setThreadState(const ThreadNameGroup& threads,
				 const OpenSS_Protocol_ThreadState& state)
{
    Guard guard_myself(this);

    // Set each thread's state
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i)	
	setThreadState(getPtr(*i), state);
}
