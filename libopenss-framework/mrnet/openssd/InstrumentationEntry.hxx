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
 * Declaration of the InstrumentationEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_InstrumentationEntry_
#define _OpenSpeedShop_Framework_InstrumentationEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <BPatch.h>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Instrumentation entry.
     *
     * Abstract base class for all instrumentation entries. Each such entry
     * represents a piece of instrumentation that can be installed into, and
     * removed from, a thread. Used by the instrumentation table to do so.
     *
     * @ingroup Implementation
     */
    class InstrumentationEntry
    {

    public:

	/** Read-only data member accessor function. */
        const bool& getIsInstalled() const
	{
	    return dm_is_installed;
	}
	
	/**
	 * Copy instrumentation.
	 *
	 * Pure virtual member function that defines the interface by which the
	 * instrumentation table polymorphically copies instrumentation from one
	 * thread to another.
	 *
	 * @param thread    Thread for the new instrumentation entry.
	 */
	virtual InstrumentationEntry* copy(BPatch_thread& thread) const = 0;
	
	/**
	 * Install instrumentation.
	 *
	 * Pure virtual member function that defines the interface by which the
	 * instrumentation table requests this instrumentation be installed.
	 *
	 * @note    Any attempt to install instrumentation that has already
	 *          been installed will be silently ignored.
	 */
	virtual void install() = 0;
	
	/**
	 * Remove instrumentation.
	 *
	 * Pure virtual member function that defines the interface by which the
	 * instrumentation table requests this instrumentation be removed.
	 *
	 * @note    Any attempt to remove instrumentation that has already
	 *          been removed will be silently ignored.
	 */
	virtual void remove() = 0;
	
    protected:

	/** Constructor from thread. */
	InstrumentationEntry(BPatch_thread& thread) :
	    dm_is_installed(false),
	    dm_thread(thread)
	{
	}
	
	/** Flag indicating if the instrumentation is installed. */
	bool dm_is_installed;
	
	/** Thread containing this instrumentation. */
	BPatch_thread& dm_thread;
	
    };
    
} }



#endif
