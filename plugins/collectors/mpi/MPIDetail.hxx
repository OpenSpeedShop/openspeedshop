////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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
 * Declaration and definition of the MPIDetail structure.
 *
 */

#ifndef _MPIDetail_
#define _MPIDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * MPI event tracing collector details. 
     *
     * Encapsulate the details metric (inclusive or exclusive) for the MPI event
     * tracing collector.
     */
    struct MPIDetail :
	public TotallyOrdered<MPIDetail>
    {
	TimeInterval dm_interval;  /**< Begin/End time of the call. */
	double dm_time;            /**< Time spent in the call. */	
        std::pair<int, uint64_t> dm_id;  /**< Rank(or Pid) and pthread id of the Event */

	/** Operator "<" defined for two MPIDetail objects. */
	bool operator<(const MPIDetail& other) const
	{
	    if(dm_interval < other.dm_interval)
                return true;
            if(dm_interval > other.dm_interval)
                return false;
	    return dm_time < other.dm_time;
        }

    };
    
} }



#endif
