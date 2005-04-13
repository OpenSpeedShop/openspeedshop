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
 * Declaration of the CollectorGroup class.
 *
 */

#ifndef _OpenSpeedShop_Framework_CollectorGroup_
#define _OpenSpeedShop_Framework_CollectorGroup_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Collector.hxx"

#include <set>



namespace OpenSpeedShop { namespace Framework {

    class Thread;
    class ThreadGroup;

    /**
     * Arbitrary group of collectors.
     *
     * Container holding an arbitrary group of collectors. No specific
     * relationship is implied between the collectors within a given collector
     * group. The class is used simply to provide a more convenient way of
     * applying operators to a group of collectors as a whole rather than each
     * individually.
     *
     * @ingroup ToolAPI
     */
    class CollectorGroup :
	public std::set<Collector>
    {
	
    public:

	void startCollecting(const Thread&) const;
	void startCollecting(const ThreadGroup&) const;
	void postponeCollecting(const Thread&) const;
	void postponeCollecting(const ThreadGroup&) const;
        void stopCollecting(const Thread&) const;
        void stopCollecting(const ThreadGroup&) const;
	
    };
    
} }



#endif
