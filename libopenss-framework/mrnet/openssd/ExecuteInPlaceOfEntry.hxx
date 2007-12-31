////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration of the ExecuteInPlaceOfEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExecuteInPlaceOfEntry_
#define _OpenSpeedShop_Framework_ExecuteInPlaceOfEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "InstrumentationEntry.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * "Execute in place of" instrumentation entry.
     *
     * Instrumentation entry representing the execution of a library function
     * in place of another function.
     *
     * @ingroup Implementation
     */
    class ExecuteInPlaceOfEntry :
	public InstrumentationEntry
    {

    public:

	ExecuteInPlaceOfEntry(BPatch_thread&, const std::string&,
			      const std::string&);

	virtual InstrumentationEntry* copy(BPatch_thread&) const;
	virtual void install();
	virtual void remove();
	
    private:

	/** Name of the function to be replaced with the library function. */
	std::string dm_where;

	/** Name of the library function to be executed. */
	std::string dm_callee;

	/** Handle to the flag used by the instrumentation. */
	BPatch_variableExpr* dm_flag;
	
	/** Handle to the inserted instrumentation. */
	BPatchSnippetHandle* dm_handle;
	
    };

} }



#endif
