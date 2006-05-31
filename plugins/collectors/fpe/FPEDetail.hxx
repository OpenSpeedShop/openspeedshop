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
 * Declaration and definition of the FPEDetail structure.
 *
 */

#ifndef _FPEDetail_
#define _FPEDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"

namespace OpenSpeedShop { namespace Framework {

// see /usr/include/fenv.h and /usr/include/bits/fenv.h

    enum FPEType {
       InexactResult,
       Underflow,
       Overflow,
       DivisionByZero,
       Unnormal,
       Invalid,
       Unknown
    };

    /**
     * FPE event tracing collector details. 
     *
     * Encapsulate the details metric for the FPE event tracing collector.
     */
    struct FPEDetail :
	public TotallyOrdered<FPEDetail>
    {
	Time   dm_time;            /**< Time spent in the call. */	
        FPEType dm_type;  /**< Type of floating-point exception. */ 

	/** Operator "<" defined for two FPEDetail objects. */
	bool operator<(const FPEDetail& other) const
	{
	    return dm_type < other.dm_type;
	    return dm_time < other.dm_time;
        }

    };
    
} }



#endif
