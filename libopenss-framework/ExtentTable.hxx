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
 * Declaration of the ExtentTable template.
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
     * Table of per-thread extents.
     *
     * Template for a container holding the extents of a given set of source
     * objects (linked objects, functions, or statements) within a group of
     * threads. Each source object can have extents within each thread that
     * vary both in number and location. An extent group is used to hold all
     * the extents for all the source objects within a given thread. A map
     * of extents to source objects is also maintained on a per-thread basis.
     *
     * Used to speed metric evaluation for large numbers of threads by allowing
     * symbol information in the database to be processed from disk into memory
     * only once.
     *
     * @ingroup ToolAPI
     */
    template <typename T>
    class ExtentTable
    {

    public:

	/** Default constructor. */
	ExtentTable::ExtentTable() :
	    dm_extents(),
	    dm_extent_to_object()
	{
	}

	/** Add an extent for a given thread and source object. */
	void addExtent(const Thread& thread,
		       const T& object,
		       const Extent& extent)
	{
	    std::map<Thread, ExtentGroup>::iterator i = dm_extents.find(thread);
	    if(i == dm_extents.end())
		i = dm_extents.insert(
		    std::make_pair(thread, ExtentGroup())
		    ).first;
	    Assert(i != dm_extents.end());
	    i->second.push_back(extent);
	    typename std::map<Thread, std::vector<T > >::iterator
		j = dm_extent_to_object.find(thread);
	    if(j == dm_extent_to_object.end())
		j = dm_extent_to_object.insert(
		    std::make_pair(thread, std::vector<T >())
		    ).first;
	    Assert(j != dm_extent_to_object.end());
	    j->second.push_back(object);
	}

	/** Get the extents within a thread. */
	ExtentGroup& getExtents(const Thread& thread)
	{
	    std::map<Thread, ExtentGroup>::iterator i = dm_extents.find(thread);
	    Assert(i != dm_extents.end());
	    return i->second;
	}

	/** Get the source object for the given extent index within a thread. */
	const T& getObject(const Thread& thread, 
			   const ExtentGroup::size_type& index) const
	{
	    typename std::map<Thread, std::vector<T > >::const_iterator
		i = dm_extent_to_object.find(thread);
	    Assert(i != dm_extent_to_object.end());
	    Assert(index < i->second.size());
	    return i->second[index];
	}

    private:

	/** Extent groups containing the extents. */
	std::map<Thread, ExtentGroup> dm_extents;

	/** Direct-indexed maps of extents to source objects. */
	std::map<Thread, std::vector<T> > dm_extent_to_object;

    };
	


} }



#endif
