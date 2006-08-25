///////////////////////////////////////////////////////////////////////////////
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
 * Declaration of the ProcessTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ProcessTable_
#define _OpenSpeedShop_Framework_ProcessTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <string>
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Lockable;
    class Process;
    template <typename> class SmartPtr;
    class Thread;
    class ThreadGroup;

    /**
     * Process table.
     *
     * There exists a one-to-one relationship between Process objects and their
     * underlying DPCL process objects (also called DPCL "process handles"). The
     * DPCL client library allows only one of its process handles to do a full
     * attach to a given underlying process on the system at a given time. This
     * implies the DPCL process handle, and thus the Process objects, must be
     * shareable between multiple uses.
     *
     * Multiple Thread objects within an experiment can refer to a single
     * process. Worse yet, more than one Experiment object can exist at once,
     * and each of these may contain multiple Thread objects that all refer to
     * a given process. This class maintains a mapping between the Thread and
     * Process objects that makes tracking all of this possible.
     *
     * To complicate things even further, Process objects do not always exist
     * for the group of threads contained within that process. It is often
     * desirable for the tool to remain disconnected from the actual processes
     * on the system. This class also facilitates tracking which threads are
     * actually connected to a Process object, and also allows the Process
     * objects to be released at an appropriate time.
     *
     * @ingroup Implementation
     */
    class ProcessTable :
	public Lockable,
	private std::map<std::string, 
			 std::pair<SmartPtr<Process>, ThreadGroup> >
    {
	
    public:
	
	static ProcessTable TheTable;
	
	ProcessTable();
	~ProcessTable();

	/** Read-only data member accessor function. */
	const std::string& getDpcldListenerPort() const
	{
	    return dm_dpcld_listener_port;
	}

	void addProcess(const SmartPtr<Process>&);
	void removeProcess(const SmartPtr<Process>&);
	void addThread(const Thread&);
	void removeThread(const Thread&);
	
	SmartPtr<Process> getProcessByName(const std::string&) const;
        SmartPtr<Process> getProcessByThread(const Thread&) const;
        ThreadGroup getThreadsByName(const std::string&) const;
        ThreadGroup getThreadsByProcess(const SmartPtr<Process>&) const;

    private:

	/** Name of the dpcld listener port in the form "[host]:[port]". */
	std::string dm_dpcld_listener_port;

#ifndef NDEBUG
	void debugPerformanceStatistics();
#endif
	
    };
    
} }



#endif
