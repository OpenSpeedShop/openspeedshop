///////////////////////////////////////////////////////////////////////////////
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
 * Declaration of the SymbolTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_SymbolTable_
#define _OpenSpeedShop_Framework_SymbolTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Address.hxx"
#include "AddressRange.hxx"
#include "FunctionEntry.hxx"
#include "LinkedObjectEntry.hxx"
#include "StatementEntry.hxx"

#include <map>
#include <string>
#include <vector>

class ::SourceObj;



namespace OpenSpeedShop { namespace Framework {

    class Application;
    class Path;
    class StringTable;

    /**
     * Symbol table.
     *
     * Symbol table information for a single executable or library that was
     * found in one or more threads. Upon symbol table construction, DPCL is
     * used to construct maps that describe the functions, source statements,
     * and call sites that were found at particular address ranges. Member
     * functions are provided for querying that information.
     *
     * @note    Addresses within a symbol table are stored as positive offsets
     *          from the first text address in the linked object. Doing so helps
     *          facilitate the sharing of a single symbol table between multiple
     *          processes that may use the same linked object at different run-
     *          time load addresses.
     *
     * @note    Symbol tables have not been made lockable since once they are
     *          created with the constructor, they can never be modified. In
     *          theory all access to them afterwards is read-only and thus
     *          thread-safe without locking.
     *
     * @ingroup Implementation
     */
    class SymbolTable {

    public:

	SymbolTable(Application*, const Path&, const AddressRange&,
		    const std::vector<SourceObj>&);
	
	Path getPath() const;
	const StringTable& getStringTable() const;

	/** Read-only data member accessor function. */
	const LinkedObjectEntry* getLinkedObjectEntry() const
	{
	    return &dm_linked_object;
	}

	std::vector<const FunctionEntry*> findAllFunctions() const;

	const FunctionEntry* findFunctionAt(const Address&) const;
	const StatementEntry* findStatementAt(const Address&) const;
	
    private:

	/** Application containing this symbol table. */
	Application* dm_application;
	
	/** Description of this linked object. */
	LinkedObjectEntry dm_linked_object;
	
	/** Map address ranges to source functions. */
	std::map<AddressRange, FunctionEntry> dm_functions;
	
	/** Map address ranges to source statements. */
	std::map<AddressRange, StatementEntry> dm_statements;
	
	// TODO: add call site information

    };
    
} }



#endif
