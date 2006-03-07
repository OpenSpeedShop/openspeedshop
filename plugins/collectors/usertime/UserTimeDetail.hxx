////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the UserTimeDetail structure.
 *
 */

#ifndef _UserTimeDetail_
#define _UserTimeDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Stack trace sampling collector detail.
     *
     * Encapsulate the detail metric (inclusive or exclusive) for the stack
     * trace sampling collector.
     */
    struct UserTimeDetail :
	public TotallyOrdered<UserTimeDetail>
    {
	uint64_t dm_count;  /**< Number of samples at the stack trace. */
	double dm_time;     /**< Time attributable to the stack trace. */

	/** Operator "<" defined for two UserTimeDetail objects. */
	bool operator<(const UserTimeDetail& other) const
	{
	    if(dm_count < other.dm_count)
                return true;
            if(dm_count > other.dm_count)
                return false;
	    return dm_time < other.dm_time;
        }

	/** Operator "+=" defined for two UserTimeDetail objects. */
	UserTimeDetail& operator+=(const UserTimeDetail& other)
	{
	    dm_count += other.dm_count;
	    dm_time += other.dm_time;
	    return *this;
	}

    };
    
} }



#endif
