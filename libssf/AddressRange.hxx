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
 * Declaration and definition of the AddressRange type.
 *
 */

#ifndef _OpenSpeedShop_Framework_AddressRange_
#define _OpenSpeedShop_Framework_AddressRange_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Address.hxx"
#include "Interval.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Range of memory addresses.
     *
     * A single, open-ended, range of memory addresses: [begin, end). Used in
     * many different places for representing a single contiguous portion of an
     * address space, as occupied by a DSO, compilation unit, function, etc.
     *
     * @ingroup Utility
     */
    typedef Interval<Address> AddressRange;

} }



#endif
