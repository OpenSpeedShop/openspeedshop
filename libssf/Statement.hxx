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
 * Declaration of the Statement class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Statement_
#define _OpenSpeedShop_Framework_Statement_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "SmartPtr.hxx"

#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Database;
    class Function;
    class LinkedObject;
    template <typename> class Optional;
    class Path;
    class Thread;
    
    /**
     * Source code statement.
     *
     * Representation of a source code statement. Provides member functions for
     * getting the containing thread, linked object and functions, the full path
     * name of the statement's source file, its line and column numbers, and its
     * address ranges.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Statement
    {
	friend class Function;
	friend class Optional<Statement>;
	friend class Thread;
	
    public:

	Thread getThread() const;
	LinkedObject getLinkedObject() const;
	std::vector<Function> getFunctions() const;
	
	Path getPath() const;
	int getLine() const;
	int getColumn() const;
	
	std::vector<AddressRange> getAddresseRanges() const;
	
    private:

	Statement();
	Statement(const SmartPtr<Database>&, const int&, const int&);
	
	void validateEntry() const;
	void validateContext() const;
	
	/** Database containing this statement. */
	SmartPtr<Database> dm_database;
	
	/** Entry (id) for this statement. */
	int dm_entry;

	/** Address space entry (id) context for this statement. */
	int dm_context;	
	
    };
    
} }



#endif
