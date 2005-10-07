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
 * Declaration and definition of the Interval template.
 *
 */

#ifndef _OpenSpeedShop_Framework_Interval_
#define _OpenSpeedShop_Framework_Interval_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "TotallyOrdered.hxx"

#include <functional>
#include <iostream>
#include <sstream>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Half-closed interval.
     *
     * Template for a half-closed interval: [begin, end). The value type is
     * specified by the template instantiation. Member functions are defined
     * for many of the common interval operations (e.g. intersection, union,
     * and containment). In order for a type to qualify as an interval value it
     * must: have a default constructor and a constructor from a value, allow
     * assignment between values, be totally ordered, and be subtractable (also
     * defining difference_type to the result type of subtraction).
     *
     * @ingroup Utility
     */
    template <typename T>
    class Interval :
	public TotallyOrdered<Interval< T > >
    {

    public:

	/** Default constructor. */
	Interval() :
	    dm_begin(),
	    dm_end()
	{
	}

	/** Constructor from a begin/end value pair. */
	Interval(const T& begin, const T& end) :
	    dm_begin(begin),
	    dm_end(end)
	{
	    Assert(dm_begin < dm_end);
	}

	/** Constructor from a value. */
	Interval(const T& value) :
	    dm_begin(value),
	    dm_end(value + 1)
	{
	}
	
	/** Operator "<" defined for two Interval objects. */
	bool operator<(const Interval& other) const
	{
	    if(dm_begin < other.dm_begin)
		return true;
	    if(dm_begin > other.dm_begin)
		return false;
	    return dm_end < other.dm_end;
	}

	/** Operator "&=" (intersection) for two Interval objects. */
	Interval& operator&=(const Interval& other)
	{
	    if(dm_begin < other.dm_begin)
		dm_begin = other.dm_begin;	   
	    if(dm_end > other.dm_end)
		dm_end = other.dm_end;
	    if(dm_end < dm_begin)
		dm_begin = dm_end;
	    return *this;
	}

	/** Operator "&" defined in terms of "&=". */
	Interval operator&(const Interval& other) const
	{
	    return Interval(*this) &= other;
	}

	/** Operator "|=" (union) for two Interval objects. */
	Interval& operator|=(const Interval& other)
	{
	    if(!other.isEmpty()) {
		if(isEmpty())
		    *this = other;
		else {
		    if(dm_begin > other.dm_begin)
			dm_begin = other.dm_begin;
		    if(dm_end < other.dm_end)
			dm_end = other.dm_end;
		}
	    }
	    return *this;
	}

	/** Operator "|" defined in terms of "|=". */
	Interval operator|(const Interval& other) const
	{
	    return Interval(*this) |= other;
	}

	/** Type conversion operator to std::string. */
	operator std::string() const
	{
	    std::ostringstream stream;
	    stream << "[ " << dm_begin << ", " << dm_end << " )";
	    return stream.str();
	}

	/** Operator "<<" defined for std::ostream. */
	friend std::ostream& operator<<(std::ostream& stream,
					const Interval& object)
	{
	    stream << static_cast<std::string>(object);
	    return stream;
	}

	/** Read-only data member accessor function. */
	const T& getBegin() const
	{ 
	    return dm_begin;
	}

	/** Read-only data member accessor function. */
	const T& getEnd() const
	{
	    return dm_end; 
	}
	
	/** Test if this interval is empty. */
	bool isEmpty() const
	{ 
	    return dm_begin == dm_end;
	}

	/** Width of this interval. */
	typename T::value_type getWidth() const
	{
	    return dm_end.getValue() - dm_begin.getValue();
	}

	/** Test if this interval contains a value. */
	bool doesContain(const T& value) const
	{
	    return (value >= dm_begin) && (value < dm_end);
	}

	/** Test if this interval contains another interval. */
	bool doesContain(const Interval& range) const
	{
	    return doesContain(range.dm_begin) && doesContain(range.dm_end - 1);
	}
	
	/** Test if this interval intersects another interval. */
	bool doesIntersect(const Interval& range) const
	{
	    return !(*this & range).isEmpty();
	}

    private:
	
	/** Closed beginning of the interval. */
	T dm_begin;
	
	/** Open end of the interval. */
	T dm_end;
	
    };

} }



namespace std {
    
    /**
     * Less-than predicate for half-closed intervals.
     *
     * Partial template specialization of less<> for Interval<>. One of the
     * reasons an interval class is defined is to allow multiple non-overlapping
     * intervals to be placed into STL associative containers such that they can
     * be searched for a particular value in logarithmic time. E.g. search for
     * an address (value) in a set of address ranges (intervals) via std::map.
     * Associative containers cannot search for the value directly, but can
     * search for the interval [value, value + 1) that represents the value.
     * Interval<>'s "constructor from a value" makes the conversion from value
     * to interval simple and convenient. However, the containers still use the
     * default ordering defined by Interval<>'s overloaded < operator, and that
     * operator defines a total, rather than simply strict weak, ordering. The
     * result is that when an interval overlaps another, they are not considered
     * equivalent. This template redefines the default ordering used by
     * associative containers of Interval<> to impose a strict weak ordering in
     * which two such intervals will be considered equivalent. Doing so makes
     * the search for a value within non-overlapping intervals work as expected.
     */
    template <typename T>
    struct less<OpenSpeedShop::Framework::Interval< T > > :
	binary_function<const OpenSpeedShop::Framework::Interval< T >&,
			const OpenSpeedShop::Framework::Interval< T >&,
			bool>
    {

	/** Evaluator for this predicate. */
	bool
	operator()(const OpenSpeedShop::Framework::Interval< T >& lhs,
		   const OpenSpeedShop::Framework::Interval< T >& rhs) const
	{
	    if(lhs.getEnd() <= rhs.getBegin())
		return true;
	    return false;
	}
	
    };

}



#endif
