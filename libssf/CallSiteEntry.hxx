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
 * Declaration and definition of the CallSiteEntry structure.
 *
 */

#ifndef _OpenSpeedShop_Framework_CallSiteEntry_
#define _OpenSpeedShop_Framework_CallSiteEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Address.hxx"



namespace OpenSpeedShop { namespace Framework {
    
    class SymbolTable;
    
    /**
     * Call site entry.
     *
     * Structure for an entry in the symbol table that describes a call site
     * within the call graph. Contains the caller address and, if available,
     * the callee symbol table and address.
     *
     * @ingroup Implementation
     */
    struct CallSiteEntry
    {
	
	/** Address of the caller. */
	Address caller;
	
	/** Symbol table containing the callee. */
	SymbolTable* callee_symbol_table;
	
	/** Address of the callee. */
	Address callee_address;
	
    };
    
} }



#endif
