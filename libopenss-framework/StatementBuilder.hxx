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
 * Declaration of the StatementBuilder class.
 *
 */

#ifndef _OpenSpeedShop_Framework_StatementBuilder_
#define _OpenSpeedShop_Framework_StatementBuilder_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <set>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Database;
    class Path;
    template <typename> class SmartPtr;

    /**
     * Statement builder.
     * 
     * The symbol information for source statements must be processed fairly
     * extensively before being stored in the database. Information for a single
     * statement can be scattered throughout the linked object, repeats of the
     * same information are removed, etc. Also, the format in which the source
     * statment data is stored in the database is more complex than a simple
     * list of (source file name, line number, column number, address) tuples.
     * This class encapsulates these various processing steps.
     *
     * @ingroup implementation
     */
    class StatementBuilder
    {

    public:
	
	void addEntry(const Path&, const int&, const int&, const Address&);
	void processAndStore(SmartPtr<Database>&, const int&);
	
    private:

	static std::vector<std::set<Address> >
	partitionAddressSet(const std::set<Address>&);
	
	/**
	 * Statement entry.
	 *
	 * Structure for a source statement's entry in the statement builder's
	 * internal tables. Contains the full path name, line number, and column
	 * number of the source statement.
	 */
	struct Entry {
	    
	    Path dm_path;   /**< Path name of this statement's source file. */
	    int dm_line;    /**< Line number of this statement. */
	    int dm_column;  /**< Column number of this statement. */
	    
	    /** Constructor from fields. */
	    Entry(const Path& path, const int& line, const int& column) :
		dm_path(path),
		dm_line(line),
		dm_column(column)
	    {
	    }

	    /** Operator "<" defined for two Entry objects. */
	    bool operator<(const Entry& other) const
	    {
		if(dm_path < other.dm_path)
		    return true;
		if(dm_path > other.dm_path)
		    return false;
		if(dm_line < other.dm_line)
		    return true;
		if(dm_line > other.dm_line)
		    return false;
		return dm_column < other.dm_column;		
	    }

	};
	
	/** Map statement entries to their set of addresses. */
	std::map<Entry, std::set<Address> > dm_entry_to_addresses;
	
    };

} }



#endif
