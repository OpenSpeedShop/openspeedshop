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
 * Declaration of the Function class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Function_
#define _OpenSpeedShop_Framework_Function_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Optional.hxx"

#include <string>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class AddressSpace;
    class CallSite;
    struct FunctionEntry;
    class LinkedObject;
    class Statement;
    class SymbolTable;
    class Thread;
    
    /**
     * Source code function.
     *
     * Representation of a source code function. Provides a member function for
     * getting the demangled name of the function. Additional member functions
     * find the address range of the function, its containing linked object, its
     * statements, location of its definition, and its callers and callees.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Function
    {
	friend class LinkedObject;
	friend class Optional<Function>;
	friend class Statement;
	friend class Thread;
	
    public:

	std::string getName() const;
	
	AddressRange findAddressRange() const;
	LinkedObject findLinkedObject() const;
	Optional< std::vector<Statement> > findAllStatements() const;
	Optional<Statement> findDefinition() const;
	std::vector<CallSite> findCallees() const;
	std::vector<CallSite> findCallers() const;
	
    private:

	Function();
	Function(const AddressSpace*, const SymbolTable*, const FunctionEntry*);
	
	/** Address space containing this function. */
	const AddressSpace* dm_address_space;

	/** Symbol table containing this function. */
	const SymbolTable* dm_symbol_table;
	
	/** Function's entry in the symbol table. */
	const FunctionEntry* dm_entry;
	
    };
    
} }



#endif
