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
 * Declaration of the ThreadGroup class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ThreadGroup_
#define _OpenSpeedShop_Framework_ThreadGroup_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Thread.hxx"

#include <set>



namespace OpenSpeedShop { namespace Framework {

    class Collector;

    /**
     * Arbitrary group of threads.
     *
     * Container holding an arbitrary group of threads. No specific relationship
     * is implied between the threads within a given thread group. The class is
     * used simply to provide a more convenient way of applying operations to a
     * group of threads as a whole rather than each individually.
     *
     * @ingroup ToolAPI
     */
    class ThreadGroup :
	public std::set<Thread>
    {
	
    public:
	
	ThreadGroup getSubsetWithState(const Thread::State&) const;
	bool isAnyState(const Thread::State&) const;
	bool areAllState(const Thread::State&) const;
	void changeState(const Thread::State&);

	void startCollecting(const Collector&) const;
	void stopCollecting(const Collector&) const;
	
    };
    
} }



#endif
