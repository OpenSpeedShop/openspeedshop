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
 * Declaration and definition of the AddressSpace class.
 *
 */

#ifndef _OpenSpeedShop_Framework_AddressSpace_
#define _OpenSpeedShop_Framework_AddressSpace_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Address.hxx"
#include "AddressRange.hxx"

#include <map>
#include <vector>



namespace OpenSpeedShop { namespace Framework {
    
    class SymbolTable;
    
    /**
     * Address space.
     *
     * Representaton of the in-memory address space image of a process at a
     * particular point in time. Encapsulates a two-way mapping of symbol table
     * objects to their address ranges within the process. Member functions
     * are provided for adding symbol tables (and their address ranges) to
     * an address space, for querying list of symbol tables, querying the
     * symbol table at a given address, and querying the base address of a
     * symbol table.
     *
     * @note    Currently the implementation of this class assumes that each
     *          symbol table is contained with a process no more than once. If
     *          that assumption is not true for some platforms, things will
     *          have to change here.
     *
     * @ingroup Implementation
     */
    class AddressSpace
    {
	
    public:
	
	/** Add a symbol table. */
	void addSymbolTable(const AddressRange& address_range,
			    const SymbolTable* symbol_table)
	{
	    // Check preconditions
	    Assert(dm_range_to_table.find(address_range) ==
		   dm_range_to_table.end());
	    Assert(dm_table_to_range.find(symbol_table) ==
		   dm_table_to_range.end());

	    // Add this symbol table
	    dm_range_to_table[address_range] = symbol_table;
	    dm_table_to_range[symbol_table] = address_range;
	}	
	
	/** Find all symbol tables. */
	std::vector<const SymbolTable*> findSymbolTables() const
	{
	    std::vector<const SymbolTable*> symbol_tables;
	    for(std::map<AddressRange, const SymbolTable*>::const_iterator
		    i = dm_range_to_table.begin();
		i != dm_range_to_table.end();
		++i)
		symbol_tables.push_back(i->second);
	    return symbol_tables;
	}

	/** Find a symbol table by an address. */
	const SymbolTable*
	findSymbolTableByAddress(const Address& address) const
	{
	    std::map<AddressRange, const SymbolTable*>::const_iterator
		i = dm_range_to_table.find(address);
	    return (i != dm_range_to_table.end()) ? i->second : NULL;
	}
	
	/** Find a base address by its symbol table. */
	Address
	findBaseAddressBySymbolTable(const SymbolTable* symbol_table) const
	{
	    std::map<const SymbolTable*, AddressRange>::const_iterator
		i = dm_table_to_range.find(symbol_table);
	    return (i != dm_table_to_range.end()) ?
		i->second.getBegin() : Address();
	}
	
    private:

	/** Map address ranges to their symbol tables. */
	std::map<AddressRange, const SymbolTable*> dm_range_to_table;

	/** Map symbol tables to their address ranges. */
	std::map<const SymbolTable*, AddressRange> dm_table_to_range;
	
    };
    
} }



#endif
