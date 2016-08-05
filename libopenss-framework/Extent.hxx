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
 * Declaration and definition of the Extent class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Extent_
#define _OpenSpeedShop_Framework_Extent_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "TimeInterval.hxx"
#include "TotallyOrdered.hxx"

#include <iostream>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Extent in time and space.
     *
     * Half-closed interval in two dimensions: time and address space. Member
     * functions are defined for many of the common interval operations (e.g.
     * intersection, union, and containment). Used for representing when/where
     * a DSO, compilation unit, function, etc. was located within a thread.
     *
     * @ingroup Utility
     */
    class Extent :
	public TotallyOrdered<Extent>
    {

    public:

	/** Default constructor. */
	Extent() :
	    dm_interval(),
	    dm_range()
	{
	}

	/** Constructor from time interval and address range pair. */
	Extent(const TimeInterval& interval, const AddressRange& range) :
	    dm_interval(interval),
	    dm_range(range)
	{
	}

	/** Operator "<" defined for two Extent objects. */
	bool operator<(const Extent& other) const
	{
	    if(dm_interval < other.dm_interval)
		return true;
	    if(dm_interval > other.dm_interval)
		return false;
	    return dm_range < other.dm_range;
	}

	/** Operator "&=" (intersection) for two Extent objects. */
        Extent& operator&=(const Extent& other)
        {
	    dm_interval &= other.dm_interval;
	    dm_range &= other.dm_range;
            return *this;
        }

        /** Operator "&" defined in terms of "&=". */
        Extent operator&(const Extent& other) const
        {
            return Extent(*this) &= other;
        }

	/** Operator "|=" (union) for two Extent objects. */
	Extent& operator|=(const Extent& other)
	{
	    dm_interval |= other.dm_interval;
	    dm_range |= other.dm_range;
	    return *this;
	}
	
	/** Operator "|" defined in terms of "|=". */
        Extent operator|(const Extent& other) const
        {
            return Extent(*this) |= other;
        }

	/** Operator "<<" defined for std::ostream. */
	friend std::ostream& operator<<(std::ostream& stream,
					const Extent& object)
	{
	    stream << object.dm_interval << object.dm_range;
	    return stream;
	}

	/** Read-only data member accessor function. */
	const TimeInterval& getTimeInterval() const
	{
	    return dm_interval;
	}

	/** Read-only data member accessor function. */
	const AddressRange& getAddressRange() const
	{
	    return dm_range;
	}

	/** Test if this extent is empty. */
	bool isEmpty() const
	{
	    return dm_interval.isEmpty() || dm_range.isEmpty();
	}

	/** Test if this extent intersects another extent. */
	bool doesIntersect(const Extent& extent) const
        {
            return !(*this & extent).isEmpty();
        }	

    private:

	/** Time interval. */
	TimeInterval dm_interval;

	/** Address range. */
	AddressRange dm_range;	

    };

} }



#endif
