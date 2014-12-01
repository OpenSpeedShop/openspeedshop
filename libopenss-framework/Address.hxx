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
 * Declaration and definition of the Address class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Address_
#define _OpenSpeedShop_Framework_Address_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "TotallyOrdered.hxx"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <stdint.h>
#include <iomanip>
#include <iostream>
#include <limits>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Memory address.
     *
     * All memory addresses are represented using a 64-bit unsigned integer.
     * This allows for a unified representation of both 32-bit and 64-bit
     * address spaces by sacrificing storage space when 32-bit addresses are
     * processed. Various overflow and underflow conditions are checked when
     * arithmetic operations are performed on these addresses.
     *
     * @ingroup Utility
     *
     * @todo    Promoting a 32-bit address to a 64-bit address and then doing
     *          arithmetic checks in operator "+=" and "-" for integer overflow
     *          is of questionable utility. This should be thought about more.
     */
    class Address :
	public TotallyOrdered<Address>
    {
	
    public:

	/** Create the lowest possible address value. */
	static Address TheLowest()
	{
	    return Address(std::numeric_limits<value_type>::min());
	}

	/** Create the highest possible time value. */
	static Address TheHighest()
	{
	    return Address(std::numeric_limits<value_type>::max());
	}

	/** Type representing a memory address. */
	typedef uint64_t value_type;
	
	/** Type representing the difference between two memory addresses. */
	typedef int64_t difference_type;
	
	/** Default constructor. */
	Address() :
	    dm_value(0x0)
	{
	}

	/** Constructor from an address value. */
	Address(const value_type& value) :
	    dm_value(value)
	{
	}

	/** Operator "<" defined for two Address objects. */
	bool operator<(const Address& other) const
	{
	    return dm_value < other.dm_value;
	}

	/** Operator "+=" defined for two Address objects. */
	Address& operator+=(const Address& other)
	{
	    value_type result = dm_value + other.dm_value;
	    Assert(result >= dm_value);
	    dm_value = result;
	    return *this;
	}
	
	/** Operator "+=" defined for an Address object and a signed offset. */
	Address& operator+=(const difference_type& other)
	{
	    value_type result = dm_value + other;
//	    Assert((other > 0) || (result <= dm_value));
	    if ((other <= 0) && (result > dm_value)) {
              std::cerr << "ASSERT other > 0 WOULD HAVE HAPPENED other=" 
                        << other << " result=" << result << " dm_value=" 
                        << dm_value << std::endl;
            }
//          Assert((other < 0) || (result >= dm_value));
            if ((other >= 0) && (result < dm_value)) {
              std::cerr << "ASSERT other < 0 WOULD HAVE HAPPENED other=" 
                        << other << " result=" << result << " dm_value=" 
                        << dm_value << std::endl;
            }
	    dm_value = result;
	    return *this;
	}

	/** Operator "+" defined in terms of "+=". */
	template <typename T>
	Address operator+(const T& other) const
	{
	    return Address(*this) += other;
	}

	/** Operator "-" defined for two Address objects. */
	difference_type operator-(const Address& other) const
	{
	    difference_type result = dm_value - other.dm_value;
	    Assert((*this > other) || (result <= 0));
            Assert((*this < other) || (result >= 0));
	    return result;
	}

	/** Operator "++" (prefix) defined in terms of "+=". */
	Address& operator++()
	{
	    return *this += 1;
	}
	
	/** Operator "++" (postfix) defined in terms of "++" (prefix). */
	Address operator++(int)
	{
	    Address copy = *this;
	    operator++();
	    return copy;
	}

	/** Operator "--" (prefix) defined in terms of "+=". */
	Address& operator--()
	{
	    return *this += -1;
	}
	
	/** Operator "--" (postfix) defined in terms of "--" (prefix). */
	Address operator--(int)
	{
	    Address copy = *this;
	    operator--();
	    return copy;
	}
	
	/** Operator "<<" defined for std::ostream. */
	friend std::ostream& operator<<(std::ostream& stream,
					const Address& object)
	{
	    std::ios_base::fmtflags saved_flags = stream.flags();
	    std::streamsize saved_width = stream.width();
	    stream << "0x"
		   << std::hex << std::setfill('0')
		   << std::setw((object.dm_value > 
				 std::numeric_limits<uint32_t>::max()) ? 16 : 8)
		   << object.dm_value;
	    stream.flags(saved_flags);
	    stream.width(saved_width);
	    return stream;
	}

	/** Read-only data member accessor function. */
	const value_type& getValue() const
	{ 
	    return dm_value;
	}
	
    private:

	/** Memory address value. */
	value_type dm_value;
	
    };
	    
} }



#endif
