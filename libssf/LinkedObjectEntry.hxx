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
 * Declaration and definition of the LinkedObjectEntry structure.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObjectEntry_
#define _OpenSpeedShop_Framework_LinkedObjectEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "StringTable.hxx"



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Linked object entry.     
     *
     * Structure for an entry in the symbol table that describes a linked
     * object. Contains the address range covered by this linked object and its
     * full path name.
     *
     * @ingroup Implementation
     */
    struct LinkedObjectEntry
    {
	
	/** Address range covered by this linked object. */
	AddressRange range;

	/** String table key for the linked object's full path name. */
	StringTable::key_type path;
	
    };
    
} }



#endif
