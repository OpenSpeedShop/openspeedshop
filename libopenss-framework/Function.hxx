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
 * Declaration of the Function class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Function_
#define _OpenSpeedShop_Framework_Function_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Entry.hxx"
#include "Optional.hxx"
#include "SmartPtr.hxx"

#include <string>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class CallSite;
    class Database;
    class LinkedObject;
    class Statement;
    class Thread;
    
    /**
     * Source code function.
     *
     * Representation of a source code function. Provides member functions for
     * getting the containing thread and linked object, demangled name, address
     * range, definition, callees, and callers of this function.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Function :
	public Entry
    {
	friend class LinkedObject;
	friend class Optional<Function>;
	friend class Statement;
	friend class Thread;
	
    public:
	
	Thread getThread() const;
	LinkedObject getLinkedObject() const;
	
	std::string getName() const;
	AddressRange getAddressRange() const;
	
	Optional<Statement> getDefinition() const;
	std::vector<Statement> getStatements() const;
	std::vector<CallSite> getCallees() const;
	std::vector<CallSite> getCallers() const;
	
    private:
	
	Function();
	Function(const SmartPtr<Database>&, const int&, const int&);
	
    };
    
} }



#endif
