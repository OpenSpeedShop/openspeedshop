////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration and definition of the ExtentTable template.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExtentTable_
#define _OpenSpeedShop_Framework_ExtentTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "ExtentGroup.hxx"
#include "Thread.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Table of extents.
     *
     * Template for a container holding the extents of a given set of objects
     * (e.g. functions) within a group (e.g. a thread). Each object can have
     * extents within each group that vary both in number and location. An
     * ExtentGroup is used to hold all the extents for all objects within a
     * given group. A map of extents to objects is also maintained on a per-
     * group basis.
     *
     * Used to accelerate view processing by allowing large numbers of extents
     * to be processed from disk into memory once.
     *
     * @ingroup ToolAPI
     */
    template <typename TG, typename TO>
    class ExtentTable
    {

    public:

	/** Default constructor. */
	ExtentTable() :
	    dm_extents(),
	    dm_extent_to_object()
	{
	}

	/** Add an extent for a given group and object. */
	void addExtent(const TG& group, const TO& object,
		       const Extent& extent)
	{
	    typename std::map<TG, ExtentGroup>::iterator i = 
		dm_extents.find(group);
	    if(i == dm_extents.end())
		i = dm_extents.insert(
		    std::make_pair(group, ExtentGroup())
		    ).first;
	    Assert(i != dm_extents.end());
	    i->second.push_back(extent);
	    typename std::map<TG, std::vector<TO > >::iterator j = 
		dm_extent_to_object.find(group);
	    if(j == dm_extent_to_object.end())
		j = dm_extent_to_object.insert(
		    std::make_pair(group, std::vector<TO >())
		    ).first;
	    Assert(j != dm_extent_to_object.end());
	    j->second.push_back(object);
	}

	/** Remove the extents for the given group. */
	void removeExtents(const TG& group)
	{
	    dm_extents.erase(group);
	    dm_extent_to_object.erase(group);	    
	}

	/** Get the extents for the given group. */
	ExtentGroup& getExtents(const TG& group)
	{
	    typename std::map<TG, ExtentGroup>::iterator i = 
		dm_extents.find(group);
	    if(i == dm_extents.end())
		i = dm_extents.insert(
		    std::make_pair(group, ExtentGroup())
		    ).first;
	    Assert(i != dm_extents.end());
	    return i->second;
	}

	/** Get the object for the given group and extent index. */
	const TO& getObject(const TG& group, 
			    const ExtentGroup::size_type& index) const
	{
	    typename std::map<TG, std::vector<TO > >::const_iterator i =
		dm_extent_to_object.find(group);	    
	    Assert(i != dm_extent_to_object.end());
	    Assert(index < i->second.size());
	    return i->second[index];
	}

    private:

	/** Extent groups containing the extents. */
	std::map<TG, ExtentGroup> dm_extents;

	/** Direct-indexed maps of extents to source objects. */
	std::map<TG, std::vector<TO > > dm_extent_to_object;

    };
	


} }



#endif
