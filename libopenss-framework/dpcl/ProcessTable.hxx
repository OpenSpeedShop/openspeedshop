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

#include "Lockable.hxx"
#include "SmartPtr.hxx"
#include "Thread.hxx"

#include <map>
#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Process;

    /**
     * Process table.
     *
     * Multiple Thread objects can refer to a single underlying Process object.
     * There exists a one-to-one relationship between Process objects and their
     * underlying DPCL process objects (also called DPCL "process handles"). The
     * DPCL client library allows only one of its process handles to do a full
     * attach to a given underlying process in the system at a time. This means
     * the DPCL process handle, and thus the Process objects, must be shareable
     * between multiple instances of Thread. This class maintains the mapping
     * between the Thread and Process objects.
     *
     * @ingroup Implementation
     */
    class ProcessTable :
	public Lockable
    {
	
    public:
	
	static ProcessTable TheTable;
	
	bool isEmpty() const;
	
	void addThread(const Thread&, const SmartPtr<Process>&);
	void removeThread(const Thread&);
	
	SmartPtr<Process> getProcessByName(const std::string&) const;
        SmartPtr<Process> getProcessByThread(const Thread&) const;
        std::set<Thread> getThreadsByProcess(const SmartPtr<Process>&) const;
        std::set<Thread> getThreadsByName(const std::string&) const;
	
    private:
	
	/** Map unique process names to their process. */
	std::map<std::string, SmartPtr<Process> > dm_name_to_process;
	
	/** Map processes to their threads. */
	std::map<SmartPtr<Process>, std::set<Thread> > dm_process_to_threads;
	
	/** Map threads to their process. */
	std::map<Thread, SmartPtr<Process> > dm_thread_to_process;
	
    };
    
} }



#endif
