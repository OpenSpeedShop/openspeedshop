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
 * Declaration of the AddressBitmap class.
 *
 */

#ifndef _OpenSpeedShop_Framework_AddressBitmap_
#define _OpenSpeedShop_Framework_AddressBitmap_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"

#include <set>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Blob;

    /**
     * Address bitmap.
     *
     * A bitmap containing one bit per address within an address range. Used,
     * when storing source statements in an experiment database, to indicate
     * which addresses within an address range are actually attributable to
     * the source statement.
     *
     * @ingroup Implementation
     */
    class AddressBitmap
    {
	
    public:

	AddressBitmap(const AddressRange&);
	AddressBitmap(const AddressRange&, const Blob&);
	
	/** Read-only data member accessor function. */
	const AddressRange& getRange() const
	{
	    return dm_range;
	}

	void setValue(const Address&, const bool&);
	bool getValue(const Address&) const;
	
	Blob getBlob() const;

	std::set<AddressRange> getContiguousRanges(const bool&) const;

    private:

	/** Address range covered by this bitmap. */
	AddressRange dm_range;

	/** Actual bitmap. */
	std::vector<bool> dm_bitmap;
	
    };
    
} }



#endif
