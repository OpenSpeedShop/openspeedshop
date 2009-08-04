////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2009 The Krell Institute. All Rights Reserved.
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
 * Declaration of the AddressSpace class.
 *
 */

#ifndef _OpenSpeedShop_Framework_AddressSpace_
#define _OpenSpeedShop_Framework_AddressSpace_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Path.hxx"

#include <map>
#include <set>



namespace OpenSpeedShop { namespace Framework {

    class LinkedObject;
    class ThreadGroup;
    class Thread;
    class Time;
    
    /**
     * Address space.
     * 
     * Represents the in-memory address space of a process. Maps multiple non-
     * overlapping address ranges to the linked object loaded into the process
     * at those ranges. Provides the facilities for constructing the address
     * space for a process, updating thread(s) in experiment database(s) with
     * this address space, and then identifying those linked objects that need
     * their symbol table constructed.
     *
     * @todo    Currently the criteria for reusing a symbol table for an exist-
     *          ing linked object is if the path names of the linked objects are
     *          identical. This is inadequate since two hosts might have linked
     *          objects with the same name but different contents. For example,
     *          two different versions of "/lib/ld-linux.so.2". In order to
     *          address this shortcoming, a checksum (or similar) should also
     *          be checked in the future.
     *
     * @ingroup Implementation
     */
    class AddressSpace
    {
	
    public:

	bool hasValue(const AddressRange&) const;
	void setValue(const AddressRange&, const Path&, const bool&);

	std::map<AddressRange, std::set<LinkedObject> >
	updateThreads(const ThreadGroup&, const Time&, const bool) const;

	std::map<AddressRange, std::set<LinkedObject> > updateThread(const Thread&) const;
	
    private:

	/**
	 * Address space entry.
	 *
	 * Structure for an entry in the address space describing a single
	 * linked object. Contains the full path name of the linked object and
	 * a flag indicating if it is an executable.
	 */
	struct Entry
	{

	    /** Full path name of this linked object. */
	    Path dm_path;
	    
	    /** Flag indicating if this linked object is an executable. */
	    bool dm_is_executable;

	    /** Constructor from fields. */  
	    Entry(const Path& path, const bool& is_executable) :
		dm_path(path),
		dm_is_executable(is_executable)
	    {
	    }
	    
	};
	
	/** Map non-overlapping address ranges to their entry. */
	std::map<AddressRange, Entry> dm_address_range_to_entry;
	
    };
    
} }



#endif
