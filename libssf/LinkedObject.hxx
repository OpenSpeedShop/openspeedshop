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
 * Declaration of the LinkedObject class.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObject_
#define _OpenSpeedShop_Framework_LinkedObject_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"

#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class AddressSpace;
    class Function;
    struct LinkedObjectEntry;
    template <typename> class Optional;
    class Path;
    class SymbolTable;
    class Thread;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (linked object).
     * Provides a member function for getting the full path name of the linked
     * object. Additional member functions find the address range of the linked
     * object and the list of functions contained within it.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObject
    {
	friend class Function;
	friend class Optional<LinkedObject>;
	friend class Thread;
	
    public:

	Path getPath() const;
	
	AddressRange findAddressRange() const;
	std::vector<Function> findAllFunctions() const;
	
    private:

	LinkedObject();
	LinkedObject(const AddressSpace*, const SymbolTable*,
		     const LinkedObjectEntry*);
	
	/** Address space containing this linked object. */
	const AddressSpace* dm_address_space;

	/** Symbol table containing this linked object. */
	const SymbolTable* dm_symbol_table;
	
	/** Linked object's entry in the symbol table. */
	const LinkedObjectEntry* dm_entry;

    };
    
} }



#endif
