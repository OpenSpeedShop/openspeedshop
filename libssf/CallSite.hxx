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
 * Declaration and definition of the CallSite structure.
 *
 */

#ifndef _OpenSpeedShop_Framework_CallSite_
#define _OpenSpeedShop_Framework_CallSite_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    class AddressSpace;
    struct CallSiteEntry;
    class SymbolTable;
    
    /**
     * Call site.
     *
     * Representation of a call site within the call graph. Provides ...
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class CallSite
    {
	
    public:
	


    private:
	
	/** Address space containing this call site. */
	AddressSpace* dm_address_space;
	
	/** Symbol table containing this call site. */
	SymbolTable* dm_symbol_table;
	
	/** Call site's entry in the symbol table. */
	CallSiteEntry* dm_entry;
	
    };
    
} }



#endif
