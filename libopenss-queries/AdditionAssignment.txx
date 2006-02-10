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
 * Definition of the addition-assignment template function.
 *
 */

#ifndef _OpenSpeedShop_Queries_AdditionAssignment_
#define _OpenSpeedShop_Queries_AdditionAssignment_

#include "Queries.hxx"
#include "ToolAPI.hxx"



/**
 * Addition-assignment operator.
 *
 * Operator "+=" defined for a map of stack traces to a list of items. Combine
 * the two maps such that the result contains the superset of stack traces from
 * both of the original two maps. Stack traces contained in the intersection of
 * the two maps will have a value that is the concatentation of the lists of
 * items from each map.
 *
 * @param lhs    Resulting, combined, map.
 * @param rhs    Map to be added to this map.
 */
template <typename T>
void operator+=(
    std::map<OpenSpeedShop::Framework::StackTrace, std::vector<T > >& lhs,
    const std::map<OpenSpeedShop::Framework::StackTrace, std::vector<T > >& rhs
    )
{
    // Iterate over each key/value pair in the RHS
    for(typename std::map<OpenSpeedShop::Framework::StackTrace, 
	    std::vector<T > >::const_iterator
	    i = rhs.begin(); i != rhs.end(); ++i) {
    
	// Simply add this pair if the key isn't already found in the LHS
	if(lhs.find(i->first) == lhs.end())
	    lhs.insert(*i);
	
	// Otherwise insert the RHS's value onto the end of the LHS' value
	else
	    lhs[i->first].insert(lhs[i->first].end(),
				 i->second.begin(), i->second.end());
	
    }
}



#endif
