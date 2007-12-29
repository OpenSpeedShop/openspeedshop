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
 * Declaration of the ExecuteNowEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExecuteNowEntry_
#define _OpenSpeedShop_Framework_ExecuteNowEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Blob.hxx"
#include "InstrumentationEntry.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * "Execute now" instrumentation entry.
     *
     * Instrumentation entry representing the execution of a library function.
     *
     * @ingroup Implementation
     */
    class ExecuteNowEntry :
	public InstrumentationEntry
    {

    public:

	ExecuteNowEntry(BPatch_thread&, const bool&,
			const std::string&, const Blob&);

	virtual InstrumentationEntry* copy(BPatch_thread&) const;
	virtual void install();
	virtual void remove();
	
    private:

	/** Flag indicating if floating-point registers should NOT be saved. */
	bool dm_disable_save_fpr;
	
	/** Name of the library function to be executed. */
	std::string dm_callee;
	
	/** Blob argument to the function. */
	Blob dm_argument;
	
    };

} }



#endif
