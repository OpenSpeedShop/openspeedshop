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
 * Declaration of the Entry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Entry_
#define _OpenSpeedShop_Framework_Entry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Database.hxx"
#include "SmartPtr.hxx"
#include "TotallyOrdered.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    class EntrySpy;

    /**       
     * Entry within a database table.
     *
     * Representation of a single entry within a database table. Really not much
     * more than a container grouping together the entry's database pointer, its
     * identifier, and its address space entry (context) identifier. Member
     * functions are provided here for comparing and validating entries.
     *
     * @ingroup Implementation
     */
    class Entry :
	public TotallyOrdered<Entry>
    {
	friend class EntrySpy;

    public:

	bool operator<(const Entry&) const;

	bool inSameDatabase(const Entry&) const;
	
	void lockDatabase() const;
	void unlockDatabase() const;
	
    protected:
	
	Entry();
	Entry(const SmartPtr<Database>&, const int&, const int&);
	virtual ~Entry();
	
	void validate(const std::string&) const;
	
	/** Database containing this entry. */
        SmartPtr<Database> dm_database;

	/** Identifier for this entry. */
	int dm_entry;
	
	/** Identifier of the context (address space entry) for this entry. */
	int dm_context;
	
    };
    
} }



#endif
