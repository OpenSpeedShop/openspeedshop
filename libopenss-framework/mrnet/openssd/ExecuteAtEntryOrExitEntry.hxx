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
 * Declaration of the ExecuteAtEntryOrExitEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExecuteAtEntryOrExitEntry_
#define _OpenSpeedShop_Framework_ExecuteAtEntryOrExitEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Blob.hxx"
#include "InstrumentationEntry.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * "Execute at entry or exit" instrumentation entry.
     *
     * Instrumentation entry representing the execution of a library function
     * at another function's entry or exit.
     *
     * @ingroup Implementation
     */
    class ExecuteAtEntryOrExitEntry :
	public InstrumentationEntry
    {

    public:

	ExecuteAtEntryOrExitEntry(BPatch_thread&, const std::string&,
				  const bool&, const std::string&, const Blob&);

	virtual InstrumentationEntry* copy(BPatch_thread&) const;
	virtual void install();
	virtual void remove();
	
    private:

	/** Name of the function at whose entry/exit execution should occur. */
	std::string dm_where;

	/** Flag indicating if executing at entry or exit of the function. */
	bool dm_at_entry;
	
	/** Name of the library function to be executed. */
	std::string dm_callee;
	
	/** Blob argument to the function. */
	Blob dm_argument;

	/** Handle to the inserted instrumentation. */
	BPatchSnippetHandle* dm_handle;
	
    };

} }



#endif
