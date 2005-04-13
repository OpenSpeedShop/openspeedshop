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
 * Definition of the ThreadGroup class.
 *
 */

#include "Collector.hxx"
#include "CollectorGroup.hxx"
#include "Database.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Obtain the subset of threads in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Matching threads are added to a new thread group that is returned as the
 * result. Useful for obtaining, for example, the subset of all suspended
 * threads within an existing thread group.
 *
 * @param state    State to compare against.
 * @return         Subset of threads that were in the passed state.
 */
ThreadGroup ThreadGroup::getSubsetWithState(const Thread::State& state) const
{
    ThreadGroup subset;
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(i->isState(state))
	    subset.insert(subset.end(), *i);
    return subset;
}



/**
 * Test if any thread is in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Returns a boolean value indicating if any of the threads in the group were in
 * the passed state.
 *
 * @param state    State to compare against.
 * @return         Boolean "true" if any thread in the group was in the passed
 *                 state, "false" otherwise.
 */
bool ThreadGroup::isAnyState(const Thread::State& state) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(i->isState(state))
	    return true;
    return false;
}



/**
 * Test if all threads are in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Returns a boolean value indicating if all threads in the group were in the
 * passed state.
 *
 * @param state    State to compare against.
 * @return         Boolean "true" if all threads in the group were in the passed
 *                 state, "false" otherwise.
 */
bool ThreadGroup::areAllState(const Thread::State& state) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(!i->isState(state))
	    return false;
    return true;
}



/**
 * Change all threads to a particular state.
 *
 * Changes the current state of every thread in the group to the passed value.
 * Used to, for example, suspend threads that were previously running. This
 * function does not wait until the threads have actually completed the state
 * change.
 *
 * @param state    Change to this state.
 */
void ThreadGroup::changeState(const Thread::State& state)
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	i->changeState(state);
}



/**
 * Start data collection for all threads.
 *
 * Starts collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to start data collection.
 */
void ThreadGroup::startCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.startCollecting(*i);
}



/**
 * Start data collection for all threads.
 *
 * Starts collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to start data collection.
 */
void ThreadGroup::startCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->startCollecting(*i);
}



/**
 * Postpone data collection for all threads.
 *
 * Postpones collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to postpone data collection.
 */
void ThreadGroup::postponeCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.startCollecting(*i);
}



/**
 * Postpone data collection for all threads.
 *
 * Postpones collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to postpone data collection.
 */
void ThreadGroup::postponeCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->postponeCollecting(*i); 
}



/**
 * Stop data collection for all threads.
 *
 * Stops collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to stop data collection.
 */
void ThreadGroup::stopCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.stopCollecting(*i);
}



/**
 * Stop data collection for all threads.
 *
 * Stops collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to stop data collection.
 */
void ThreadGroup::stopCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->stopCollecting(*i); 
}
