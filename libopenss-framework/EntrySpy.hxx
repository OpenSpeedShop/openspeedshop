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
 * Declaration and definition of the EntrySpy class.
 *
 */

#ifndef _OpenSpeedShop_Framework_EntrySpy_
#define _OpenSpeedShop_Framework_EntrySpy_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Entry.hxx"



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Entry spy.
     *
     * Class providing access to an Entry object's protected data members. It
     * is desirable that the users of the Tool API not have access to these data
     * members, thus the reason they are protected in the first place. However
     * there are places, internally, where access to these members is needed.
     * Normally this would be a simple matter of making the necessary functions
     * and classes friends of Entry. But this list would be quite long. Instead
     * its easier to use this class to allow internal functions to "spy" into
     * the protected data members of any Entry object.
     *
     * @ingroup Implementation
     */
    class EntrySpy
    {
	
    public:

	/** Constructor from an Entry. */
	EntrySpy(const Entry& entry) :
	    dm_entry(entry)
	{
	}

	/** Get the database containing this entry. */
	SmartPtr<Database> getDatabase() const
	{
	    return dm_entry.dm_database;
	}

	/** Get the database table containing this entry. */
	std::string getTable() const
	{
	    return dm_entry.getTable();
	}

	/** Get the identifier for this entry. */
	int getEntry() const
	{
	    return dm_entry.dm_entry;
	}
	
	/** Validate this entry. */
	void validate() const
	{
	    dm_entry.validate();
	}
		
    private:

	/** Entry being spied. */
	const Entry& dm_entry;
	
    };
    
} }



#endif
