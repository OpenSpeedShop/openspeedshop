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

#include "Protocol.h"

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
     * Names a single thread of code execution. To unqiuely identify a thread,
     * the host name and process identifier must be specified. A POSIX thread
     * identifier must also be specified if a specific thread in a process is
     * being named.
     *
     * @note    This class has intentionally been given an identical name to
     *          the similar class in the framework library itself. In the near
     *          future, the experiment database functionality may be split into
     *          a separate library and utilized by the daemon. If this happens,
     *          this class will be removed in favor of the existing framework
     *          class.
     *
     * @ingroup Implementation
     */
    class ThreadName
    {

    public:

	/** Constructor from a OpenSS_Protocol_ThreadName object. */
	ThreadName(const OpenSS_Protocol_ThreadName& object) :
	    dm_host(object.host),
	    dm_pid(object.pid),
	    dm_tid(std::make_pair(object.has_posix_tid, object.posix_tid))
	{
	}

	/** Type conversion to a OpenSS_Protocol_ThreadName object. */
	operator OpenSS_Protocol_ThreadName() const
	{
	    OpenSS_Protocol_ThreadName object;
	    if(!dm_host.empty()) {
		object.host = new char[dm_host.size()];
		strcpy(object.host, dm_host.c_str());
	    }
	    else
		object.host = NULL;
	    object.pid = dm_pid;
	    object.has_posix_tid = dm_tid.first;
	    object.posix_tid = dm_tid.second;
	    return object;
	}
		   
	/** Operator "<" defined for two ThreadName objects. */
	bool operator<(const ThreadName& other) const
	{
	    if(dm_host < other.dm_host)
		return true;
	    if(dm_host > other.dm_host)
		return false;
	    if(dm_pid < other.dm_pid)
		return true;
	    if(dm_pid > other.dm_pid)
		return false;

	    // TODO: finish implementing!

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

	/** Name of the host on which this thread is located. */
	std::string dm_host;

	/** Identifier of process containing this thread. */
	pid_t dm_pid;

	/** Pair containing POSIX identifier of this thread. */
	std::pair<bool, pthread_t> dm_tid;

    };
    
} }



#endif
