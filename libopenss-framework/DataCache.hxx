////////////////////////////////////////////////////////////////////////////////
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
 * Declaration of the DataCache class.
 *
 */

#ifndef _OpenSpeedShop_Framework_DataCache_
#define _OpenSpeedShop_Framework_DataCache_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ExtentTable.hxx"
#include "Lockable.hxx"

#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Collector;
    class Thread;

    /**
     * Performance data cache.
     *
     * Cache of performance data blob identifiers stored on a per collector and
     * thread basis. Provides a query for finding the identifiers, for a given
     * collector and thread, that intersect the specified extents.
     *
     * @note    Database queries that performed extent/extent intersections,
     *          e.g. Collector::getMetricValues(), were found to be performing
     *          poorly. This was due to SQLite being unable to utilize more
     *          than a single index during a given query. This class helps
     *          alleviate this performance bottleneck by caching the extent
     *          information in memory.
     *
     * @ingroup Implementation
     */
    class DataCache :
	private Lockable
    {

    public:

	std::set<int> getIdentifiers(const Collector&, const Thread&,
				     const Extent&);

	void addIdentifier(const SmartPtr<Database>& database,
			   const int&, const int&, const Extent&, const int&);

	void removeDatabase(const SmartPtr<Database>&);
	void removeCollector(const Collector&);
	void removeThread(const Thread&);
    
    private:
	
	/** Extent table containing data id cache. */
	ExtentTable<std::pair<Collector, Thread>, int> dm_cache;
	
	void addIdentifiers(const std::pair<Collector, Thread>&);

    };



} }



#endif
