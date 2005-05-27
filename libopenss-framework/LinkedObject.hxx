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
 * Declaration of the LinkedObject class.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObject_
#define _OpenSpeedShop_Framework_LinkedObject_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Entry.hxx"

#include <set>



namespace OpenSpeedShop { namespace Framework {

    class Database;
    class Function;
    class Path;
    template <typename> class SmartPtr;
    class Statement;
    class Thread;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (a "linked object").
     * Provides member functions for getting the containing thread, full path
     * name, address range, and the list of all functions contained within this
     * linked object.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObject :
	public Entry
    {
	friend class Function;
	friend class Statement;
	friend class Thread;
	
    public:

	Thread getThread() const;

	Path getPath() const;
	AddressRange getAddressRange() const;
	bool isExecutable() const;
	
	std::set<Function> getFunctions() const;
	
    private:

	LinkedObject();
	LinkedObject(const SmartPtr<Database>&, const int&, const int&);
	
    };
    
} }



#endif
