////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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

#include <vector>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Arbitrary group of collectors.
     *
     * Container holding an arbitrary group of collectors. No specific
     * relationship is implied between the collectors within a given collector
     * group. The class is used simply to provide a more convenient way of
     * applying operators to a group of collectors as a whole rather than each
     * individually.
     *
     * @todo    Implement additional collective operations.
     *
     * @ingroup ToolAPI
     */
    class CollectorGroup :
	public std::vector<Collector>
    {
	
    public:

	void startCollecting();
        void stopCollecting();
	
    };
    
} }



#endif
