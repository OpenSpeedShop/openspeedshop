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
 * Declaration and definition of the TotallyOrdered template.
 *
 */

#ifndef _OpenSpeedShop_Framework_TotallyOrdered_
#define _OpenSpeedShop_Framework_TotallyOrdered_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * Totally-ordered type.
     *
     * Helper template that automagically defines the operators "==", "!=", ">",
     * "<=", and ">=" for its derived types in terms of their "<" operator. It
     * is assumed that the derived types' "<" operator imposes a total ordering
     * on objects of that type. Don't use this template if it doesn't!
     *
     * @sa    http://www.boost.org/libs/utility/operators.htm
     *
     * @ingroup Utility
     */
    template <typename T>
    struct TotallyOrdered
    {
	
    public:
	
	/** Operator "==" defined in terms of "<". */
	friend bool operator==(const T& x, const T& y)
	{
	    return !(x < y) && !(y < x);
	}

	/** Operator "!=" defined in terms of "<". */
	friend bool operator!=(const T& x, const T& y)
	{
	    return (x < y) || (y < x);
	}
	
	/** Operator ">" defined in terms of "<". */
	friend bool operator>(const T& x, const T& y)
	{
	    return y < x;
	}

	/** Operator "<=" defined in terms of "<". */
	friend bool operator<=(const T& x, const T& y)
	{
	    return !(y < x);
	}

	/** Operator ">=" defined in terms of "<". */
	friend bool operator>=(const T& x, const T& y)
	{
	    return !(x < y);
	}
	
    };
    
} }



#endif
