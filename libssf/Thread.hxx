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
 * Declaration of the Thread class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Thread_
#define _OpenSpeedShop_Framework_Thread_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "Optional.hxx"
#include "Time.hxx"

#ifdef HAVE_ARRAY_SERVICES
#include <arraysvcs.h>
#endif
#include <pthread.h>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Application;
    class Function;
    class LinkedObject;
    class Process;
    class Statement;
    
    /**
     * Single thread of code execution.
     *
     * Represents a single thread of code execution to which the performance
     * tool has previously attached itself. Member functions are provided for
     * querying and/or changing the execution state, obtaining identifying
     * information, and querying symbol information. This allows performance
     * tools to relate raw performance data gathered by collectors to the
     * source code constructs such as functions or statements.
     *
     * @note    The existence (creation and destruction) of a Thread object may
     *          not directly mirror the existence of the real underlying thread.
     *          One cannot create a Thread for an underlying thread that did
     *          not exist. An underlying thread, however, may terminate and the
     *          corresponding Thread object will continue to exist. This class
     *          serves merely as a proxy for performing operations on the thread
     *          if and when it exists.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Thread :
	private Lockable
    {
	friend class Application;
	
    public:

	/**
	 * Thread state enumeration.
	 *
	 * Enumeration defining the state of a thread. This may not enumerate
	 * all the possible states in which a thread may find itself on a given
	 * system. It contains only those states that are generally of interest
	 * to a performance tool.
	 */
	enum State {
	    Running,      /**< Thread is active and running. */
	    Suspended,    /**< Thread has been temporarily suspended. */
	    Terminated    /**< Thread has terminated. */
	};
	
	State getState() const;
	bool isState(const State&) const;
	void changeState(const State&);
	
	std::string getHost() const;
	pid_t getProcessId() const;
	Optional<pthread_t> getPosixThreadId() const;
#ifdef HAVE_OPENMP
	Optional<int> getOmpThreadId() const;
#endif
#ifdef HAVE_MPI
	Optional<int> getMpiRank() const;
#endif
#ifdef HAVE_ARRAY_SERVICES
	Optional<ash_t> getArraySessionHandle() const;
#endif
	
	std::vector<LinkedObject> findAllLinkedObjects(
	    const Time& = Time::Now()) const;
	std::vector<Function> findAllFunctions(
	    const Time& = Time::Now()) const;
	
	Optional<LinkedObject> findLinkedObjectAt(
	    const Address&, const Time& = Time::Now()) const;
	Optional<Function> findFunctionAt(
	    const Address&, const Time& = Time::Now()) const;
	Optional<Statement> findStatementAt(
	    const Address&, const Time& = Time::Now()) const;
	
    private:
	
	Thread(Process*);
	~Thread();
	
	/** Process containing this thread. */
	Process* dm_process;
	
    };
    
} }



#endif
