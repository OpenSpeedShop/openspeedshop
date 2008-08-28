////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
 * Declaration of the InstrumentationTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_InstrumentationTable_
#define _OpenSpeedShop_Framework_InstrumentationTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "InstrumentationEntry.hxx"
#include "Lockable.hxx"

#include <map>
#include <set>
#include <string>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class ThreadName;

    /**
     * Instrumentation table.
     *
     * Table used to keep track of what instrumentation has been inserted into
     * the threads. Facilitates later removal of that instrumentation and making
     * identical copies of the instrumentation to other threads.
     *
     * @ingroup Implementation
     */
    class InstrumentationTable :
	private Lockable
    {

    public:

	static InstrumentationTable TheTable;

	InstrumentationTable();

	void addExecuteNow(const ThreadName&, const Collector&,
			   const bool&, const std::string&, const Blob&);
	void addExecuteAtEntryOrExit(const ThreadName&, const Collector&,
				     const std::string&, const bool&,
				     const std::string&, const Blob&);
	void addExecuteInPlaceOf(const ThreadName&, const Collector&,
				 const std::string&, const std::string&);
	void addStopAtEntryOrExit(const ThreadName&,
				  const std::string&, const bool&);

	void removeInstrumentation(const ThreadName&);
	void removeInstrumentation(const ThreadName&, const Collector&);

	std::set<Collector> copyInstrumentation(const ThreadName&, 
						const ThreadName&);
	
    private:

	/** Type representing a list of instrumentation. */
	typedef std::vector<InstrumentationEntry*> InstrumentationList;

	/**
	 * Thread entry.
	 *
	 * Structure containing lists of instrumentation associated with
	 * a thread.
	 */
	struct ThreadEntry
	{
	    /** Instrumentation not associated with any collector. */
	    InstrumentationList dm_general;
	    
	    /** Instrumentation associated with specific collectors. */
	    std::map<Collector, InstrumentationList> dm_collectors;

	};
	
	/** Threads in this instrumentation table. */
	std::map<ThreadName, ThreadEntry> dm_threads;
	
    };
    
} }



#endif
