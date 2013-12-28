////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
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
     * Representation of a single entry within a database table. Not much more
     * than a container grouping together the entry's database pointer, table,
     * and identifier. Member funtions are provided here for comparing and
     * validating entries.
     *
     * @ingroup Implementation
     */
    class Entry :
	public TotallyOrdered<Entry>
    {
	friend class EntrySpy;

    public:

//	bool operator<(const Entry&) const;
bool operator<(const Entry& other) const
{
    // Check preconditions
    //    Assert(dm_table == other.dm_table);

    // Compare the two entries
    if(dm_database == other.dm_database)
         return dm_entry < other.dm_entry;
    return dm_database < other.dm_database;
}

	bool inSameDatabase(const Entry&) const;
	
	void lockDatabase() const;
	void unlockDatabase() const;

    protected:

	/**
	 * Database table enumeration.
	 *
	 * Enumeration defining all the database tables for which an Entry can
	 * be created. This may not enumerate all possible database tables. It
	 * contains only those database tables which have a corresponding Entry
	 * subclass.
	 */
	enum Table {
	    Collectors,     /**< Table of performance data collectors. */
	    Functions,      /**< Table of source code functions. */
	    LinkedObjects,  /**< Table of linked objects. */
        Loops,          /**< Table of loops. */
	    Statements,     /**< Table of source code statements. */
	    Threads         /**< Table of threads of code execution. */
	};
	
	Entry();
	Entry(const SmartPtr<Database>&, const Table&, const int&);
	virtual ~Entry();

	std::string getTable() const;
	
	void validate() const;
	
	/** Database containing this entry. */
        SmartPtr<Database> dm_database;

	/** Table containing this entry. */
	Table dm_table;

	/** Identifier for this entry. */
	int dm_entry;
	
    };
    
} }



#endif
