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
 * Declaration and definition of the Optional template.
 *
 */

#ifndef _OpenSpeedShop_Framework_Optional_
#define _OpenSpeedShop_Framework_Optional_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Optional value.
     *
     * Template for an optionally stored value. The value's type is specified by
     * the template instantiation. Used in many places to represent parameters,
     * data members, etc. that may or may not be assigned an actual value.
     *
     * @sa    http://www.boost.org/libs/optional/doc/optional.html
     *
     * @ingroup Utility
     */
    template <typename T>
    class Optional
    {
	
    public:
	
	/** Default constructor. */
	Optional() :
	    dm_has_value(false),
	    dm_value()
	{
	}

	/** Constructor from a value of our type. */
	explicit Optional(const T& value) :
	    dm_has_value(true),
	    dm_value(value)
	{
	}
	
	/** Operator "=" defined for a value of our type. */
	Optional& operator=(const T& value)
	{
	    dm_has_value = true;
	    dm_value = value;
	    return *this;
	}

	/** Conversion operator to a value of our type. */
	operator T() const
	{
	    return getValue();
	}

	/** Test if a value was stored. */
	bool hasValue() const
	{
	    return dm_has_value;
	}
	
	/** Get our stored value. */
	const T& getValue() const
	{
	    Assert(dm_has_value);
	    return dm_value;
	}

	/** Drop the stored value. */
	void dropValue()
	{
	    dm_has_value = false;
	}
		
    private:
	
	/** Flag indicating if this object contains a value or not. */
	bool dm_has_value;
	
	/** Our value (if any). */
	T dm_value;
	
    };
    
} }



#endif
