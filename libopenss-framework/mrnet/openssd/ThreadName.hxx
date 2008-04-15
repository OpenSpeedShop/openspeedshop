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
 * Declaration of the ThreadName class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ThreadName_
#define _OpenSpeedShop_Framework_ThreadName_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Protocol.h"
#include "TotallyOrdered.hxx"
#include "Utility.hxx"

#include <BPatch.h>
#include <pthread.h>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Thread name.
     *
     * Names a single thread of code execution. To uniquely identify a thread,
     * the experiment, host name, and process identifier must be specified. A
     * POSIX thread identifier must also be specified if a specific thread in
     * a process is being named.
     *
     * @note    This class has intentionally been given an identical name to
     *          the similar class in the framework library itself. In the near
     *          future, the experiment database functionality may be split into
     *          a separate library and utilized by the daemon. If that happens,
     *          this class will be removed in favor of the existing framework
     *          class.
     *
     * @ingroup Implementation
     */
    class ThreadName :
	public TotallyOrdered<ThreadName>
    {

    public:

	/** Constructor from fields. */
	ThreadName(const int& experiment, const std::string& host,
		   const pid_t& pid, const std::pair<bool, pthread_t>& tid) :
	    dm_experiment(experiment),
	    dm_host(host),
	    dm_pid(pid),
	    dm_tid(tid)
	{
	}

	/** Constructor from an experiment and BPatch_thread object. */
	ThreadName(const int& experiment, /* const */ BPatch_thread& thread) :
	    dm_experiment(experiment),
	    dm_host(getCanonicalName(getLocalHost())),
	    dm_pid(),
	    dm_tid()
	{
	    BPatch_process* process = thread.getProcess();
	    Assert(process != NULL);
	    dm_pid = process->getPid();
	    dm_tid = std::make_pair(process->isMultithreadCapable(),
				    static_cast<pthread_t>(thread.getTid()));
	}

	/** Constructor from a OpenSS_Protocol_ThreadName object. */
	ThreadName(const OpenSS_Protocol_ThreadName& object) :
	    dm_experiment(object.experiment),
	    dm_host(object.host),
	    dm_pid(object.pid),
	    dm_tid(std::make_pair(object.has_posix_tid, object.posix_tid))
	{
	}

	/** Type conversion to a OpenSS_Protocol_ThreadName object. */
	operator OpenSS_Protocol_ThreadName() const
	{
	    OpenSS_Protocol_ThreadName object;
	    object.experiment = dm_experiment;
	    convert(dm_host, object.host);
	    object.pid = dm_pid;
	    object.has_posix_tid = dm_tid.first;
	    object.posix_tid = dm_tid.second;
	    return object;
	}
		   
	/** Operator "<" defined for two ThreadName objects. */
	bool operator<(const ThreadName& other) const
	{
            if(dm_experiment < other.dm_experiment)
                return true;
            if(dm_experiment > other.dm_experiment)
                return false;
	    if(dm_host < other.dm_host)
		return true;
	    if(dm_host > other.dm_host)
		return false;
	    if(!dm_tid.first && !other.dm_tid.first) {
		return dm_pid < other.dm_pid;
	    }
	    else {
		if(dm_pid < other.dm_pid)
		    return true;
		if(dm_pid > other.dm_pid)
		    return false;
		return (dm_tid.first && other.dm_tid.first) ?
		    (dm_tid.second < other.dm_tid.second) : !dm_tid.first;
	    }
	}

        /** Read-only data member accessor function. */
        const int& getExperiment() const
        {
            return dm_experiment;
        }
	
	/** Read-only data member accessor function. */
	const std::string& getHost() const
	{
	    return dm_host;
	}

	/** Read-only data member accessor function. */
	const pid_t& getProcessId() const
	{
	    return dm_pid;
	}
	
	/** Read-only data member accessor function. */
	const std::pair<bool, pthread_t>& getPosixThreadId() const
	{
	    return dm_tid;
	}

    private:

	/** Unique identifier for the experiment containing this thread. */
	int dm_experiment;

	/** Name of the host on which this thread is located. */
	std::string dm_host;

	/** Identifier of process containing this thread. */
	pid_t dm_pid;

	/** Pair containing POSIX identifier of this thread. */
	std::pair<bool, pthread_t> dm_tid;

    };
    
} }



#endif
