////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 The Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the HWCSampDetail structure.
 *
 */

#ifndef _HWCSampDetail_
#define _HWCSampDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"

#define OpenSS_NUMCOUNTERS 6

namespace OpenSpeedShop { namespace Framework {

    /**
     * Hardware time sampling collector detail.
     *
     * Encapsulate the detail metric (inclusive or exclusive) for the hardware
     * time sampling collector.
     */
    struct HWCSampDetail :
	public TotallyOrdered<HWCSampDetail>
    {
	uint64_t dm_count;   /**< Number of samples at the address. */
	double dm_time;   /**< Time attributable to the address. */
	uint64_t dm_event_values[OpenSS_NUMCOUNTERS];   /**< Number of samples at the address. */

	/* Initialize all fields. */
        HWCSampDetail()
	{
          dm_count = 0;
          dm_time = 0.0;
	  for (int i = 0; i < OpenSS_NUMCOUNTERS; i++) {
	    dm_event_values[i]  = 0;
	  }
	}
	

	/** Operator "<" defined for two HWCSampDetail objects. */
	bool operator<(const HWCSampDetail& other) const
	{
	  for (int i = 0; i < OpenSS_NUMCOUNTERS; i++) {
	    if(dm_event_values[i] < other.dm_event_values[i])
                return true;
            if(dm_event_values[i] > other.dm_event_values[i])
                return false;
	  }
          return dm_time < other.dm_time;
        }

	/** Operator "+=" defined for two HWCSampDetail objects. */
	HWCSampDetail& operator+=(const HWCSampDetail& other)
	{
          dm_time += other.dm_time;
	  for (int i = 0; i < OpenSS_NUMCOUNTERS; i++) {
	    dm_event_values[i] += other.dm_event_values[i];
	  }
	  return *this;
	}

    };
    
} }



#endif
