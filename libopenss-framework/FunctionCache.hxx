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
 * Declaration of the FunctionCache class.
 *
 */

#ifndef _OpenSpeedShop_Framework_FunctionCache_
#define _OpenSpeedShop_Framework_FunctionCache_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ExtentTable.hxx"
#include "LinkedObject.hxx"
#include "Lockable.hxx"
#include "Function.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Function cache.
     *
     * Cache of functions stored on a per linked object basis. Provides a query
     * for finding the functions, within a given linked object, that intersect
     * the specified extents.
     *
     * @note    Database queries that performed extent/extent intersections,
     *          e.g. Statement::getFunctions(), were found to be performing
     *          poorly. This was due to SQLite being unable to utilize more
     *          than a single index during a given query. This class helps
     *          alleviate this performance bottleneck by caching the extent
     *          information in memory.
     *
     * @ingroup Implementation
     */
    class FunctionCache :
	private Lockable
    {

    public:

	std::set<Function> getFunctions(const LinkedObject&,
					const ExtentGroup&);

	void removeDatabase(const SmartPtr<Database>&);

    private:

	/** Extent table containing function cache. */
	ExtentTable<LinkedObject, Function> dm_cache;

	void addLinkedObject(const LinkedObject&);

    };
	


} }



#endif
