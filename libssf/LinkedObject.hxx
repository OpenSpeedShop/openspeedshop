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
 * Declaration of the LinkedObject class.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObject_
#define _OpenSpeedShop_Framework_LinkedObject_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "SmartPtr.hxx"

#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Database;
    class Function;
    template <typename> class Optional;
    class Path;
    class Statement;
    class Thread;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (linked object).
     * Provides member functions for getting the containing thread, full path
     * name, address range, and the list of all functions contained within this
     * linked object.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObject
    {
	friend class Function;
	friend class Optional<LinkedObject>;
	friend class Statement;
	friend class Thread;
	
    public:

	Thread getThread() const;

	Path getPath() const;
	AddressRange getAddressRange() const;
	
	std::vector<Function> getFunctions() const;
	
    private:

	LinkedObject();
	LinkedObject(const SmartPtr<Database>&, const int&, const int&);
	
	void validateEntry() const;
	void validateContext() const;
	
	/** Database containing this linked object. */
	SmartPtr<Database> dm_database;
	
	/** Entry (id) for this linked object. */
	int dm_entry;

	/** Address space entry (id) context for this linked object. */
	int dm_context;
	
    };
    
} }



#endif
