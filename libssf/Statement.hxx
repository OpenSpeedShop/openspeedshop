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

#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class AddressSpace;
    class Function;
    template <typename> class Optional;
    class Path;
    class StatementEntry;
    class SymbolTable;
    class Thread;
    
    /**
     * Source code statement.
     *
     * Representation of a source code statement. Provides member functions for
     * getting the full path name of the statement's source file and its line
     * and column numbers. Additional member functions find the address list
     * for the statement and its containing functions.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Statement
    {
	friend class Function;
	friend class Optional<Statement>;
	friend class Thread;
	
    public:
	
	Path getPath() const;
	unsigned getLine() const;
	unsigned getColumn() const;
	
	std::vector<Address> findAllAddresses() const;
	std::vector<Function> findFunctions() const;
	
    private:

	Statement();
	Statement(const AddressSpace*, const SymbolTable*,
		  const StatementEntry*);
	
	/** Address space containing this statement. */
	const AddressSpace* dm_address_space;

	/** Symbol table containing this statement. */
	const SymbolTable* dm_symbol_table;
	
	/** Statement's entry in the symbol table. */
	const StatementEntry* dm_entry;	
	
    };
    
} }



#endif
