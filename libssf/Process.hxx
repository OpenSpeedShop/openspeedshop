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
 * Declaration of the Process class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Process_
#define _OpenSpeedShop_Framework_Process_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressSpace.hxx"
#include "Lockable.hxx"
#include "Thread.hxx"
#include "Time.hxx"
#include "TimeInterval.hxx"

#include <map>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

class ::Process;



namespace OpenSpeedShop { namespace Framework {

    class Application;
    
    /**
     * Process.
     *
     * Represents a single process to which the performance tool has attached
     * itself. Member functions are provided for querying and/or changing the
     * execution state, obtaining identifying information, and querying symbol
     * information.
     *
     * @note    Provides the underlying implementation for the Thread class and
     *          establishes the many-to-one relationship between threads and the
     *          underlying process that contains them.
     *
     * @ingroup Implementation
     */
    class Process :
	private Lockable
    {

    public:

	Process(Application*, const std::string&, const std::string&);
	Process(Application*, const std::string&, const pid_t&);
	
	~Process();

	Thread::State getState() const;
	void changeState(const Thread::State&);
	
	std::string getHost() const;
	pid_t getProcessId() const;

	const AddressSpace* findAddressSpace(const Time&) const;
	
	void completeStateChange();
	void terminateNow();
	void updateAddressSpace(const Time&);
	
    private:
	
	/** Application containing this process. */
	Application* dm_application;
	
	/** DPCL process handle. */
	::Process* dm_process;
	
	/** Current state of this process. */
	Thread::State dm_current_state;
	
	/** Flag indicating if process is changing state. */
	bool dm_is_state_changing;
	
	/** Future state of this process. */
	Thread::State dm_future_state;
	
	/** Name of host on which this process is located. */
        std::string dm_host;
	
        /** Identifier of this process. */
        pid_t dm_pid;
	
	/** Address space of this process as a function of time. */
	std::map<TimeInterval, const AddressSpace*> dm_address_space;
	
    };
    
} }



#endif
