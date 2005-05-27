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
 * Definition of the CollectorGroup class.
 *
 */

#include "CollectorGroup.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Start data collection for all collectors.
 *
 * Starts collection of performance data on the specified thread for all
 * collectors in the group.
 *
 * @param thread    Thread for which to start data collection.
 */
void CollectorGroup::startCollecting(const Thread& thread) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	i->startCollecting(thread);
}



/**
 * Start data collection for all collectors.
 *
 * Starts collection of performance data on the specified threads for all
 * collectors in the group.
 *
 * @param threads    Threads for which to start data collection.
 */
void CollectorGroup::startCollecting(const ThreadGroup& threads) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	for(ThreadGroup::const_iterator
		j = threads.begin(); j != threads.end(); ++j)
	    i->startCollecting(*j);
}



/**
 * Postpone data collection for all collectors.
 *
 * Postpones collection of performance data on the specified thread for all
 * collectors in the group.
 *
 * @param thread    Thread for which to postpone data collection.
 */
void CollectorGroup::postponeCollecting(const Thread& thread) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	i->postponeCollecting(thread);
}



/**
 * Postpone data collection for all collectors.
 *
 * Postpones collection of performance data on the specified threads for all
 * collectors in the group.
 *
 * @param threads    Threads for which to postpone data collection.
 */
void CollectorGroup::postponeCollecting(const ThreadGroup& threads) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	for(ThreadGroup::const_iterator
		j = threads.begin(); j != threads.end(); ++j)
	    i->postponeCollecting(*j);
}



/**
 * Stop data collection for all collectors.
 *
 * Stops collection of performance data on the specified thread for all
 * collectors in the group.
 *
 * @param thread    Thread for which to stop data collection.
 */
void CollectorGroup::stopCollecting(const Thread& thread) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	i->stopCollecting(thread);
}



/**
 * Stop data collection for all collectors.
 *
 * Stops collection of performance data on the specified threads for all
 * collectors in the group.
 *
 * @param threads    Threads for which to stop data collection.
 */
void CollectorGroup::stopCollecting(const ThreadGroup& threads) const
{
    for(CollectorGroup::const_iterator i = begin(); i != end(); ++i)
	for(ThreadGroup::const_iterator
		j = threads.begin(); j != threads.end(); ++j)
	    i->stopCollecting(*j);
}
