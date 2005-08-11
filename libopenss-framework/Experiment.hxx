////////////////////////////////////////////////////////////////////////////////
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
 * Declaration of the Experiment class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Experiment_
#define _OpenSpeedShop_Framework_Experiment_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Database.hxx"
#include "NonCopyable.hxx"
#include "OutputCallback.hxx"
#include "SmartPtr.hxx"

#include <pthread.h>
#include <set>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Collector;
    class CollectorGroup;
    class Thread;
    class ThreadGroup;
    
    /**
     * Performance experiment.
     *
     * Container holding a group of threads along with the collectors gathering
     * performance data for those threads. This class is the top-level interface
     * by which a tool interacts with the framework. Member functions are
     * provided for querying, adding to, and removing from, the thread and
     * collector groups. All persistent state information for the threads and
     * collectors, along with all collected performance data, is stored in a
     * database whose name is specified at object construction.
     *
     * @note    New threads created by any thread already within an experiment
     *          are automatically added to that experiment. The new thread will
     *          also be attached to any collectors to which its parent thread
     *          had been previously attached. As an example, a process P is
     *          added to an experiment E via E.attachProcess(), a collector C
     *          is created in E via E.createCollector(), and P is then attached
     *          to C via C.attachThread(P). Process P subsequently creates a
     *          child process Q via fork(). The experiment will automatically
     *          attach to Q in E and attach Q to C. The next time experiment E,
     *          or collector C, is queried for its list of threads, process Q
     *          will be found in that list.
     *
     * @ingroup ToolAPI
     */
    class Experiment :
	private NonCopyable
    {

    public:

	static std::string getLocalHost();

	static bool isAccessible(const std::string&);
	static void create(const std::string&);
	static void remove(const std::string&);

	explicit Experiment(const std::string&);
	~Experiment();

	void renameTo(const std::string&) const;
	void copyTo(const std::string&) const;
	
	std::string getName() const;

	ThreadGroup getThreads() const;
	
	Thread createProcess(const std::string&,
			     const std::string& = getLocalHost(),
			     const OutputCallback = 
			         OutputCallback(NULL, NULL),
			     const OutputCallback = 
			         OutputCallback(NULL, NULL)) const;
	
	ThreadGroup attachMPIJob(const pid_t&,
				 const std::string& = getLocalHost()) const;
	ThreadGroup attachProcess(const pid_t&,
				  const std::string& = getLocalHost()) const;
	Thread attachPosixThread(const pid_t&, const pthread_t&,
				 const std::string& = getLocalHost()) const;
	Thread attachOpenMPThread(const pid_t&, const int&,
				  const std::string& = getLocalHost()) const;
	
	void removeThread(const Thread&) const;
	
	CollectorGroup getCollectors() const;
	Collector createCollector(const std::string&) const;
	void removeCollector(const Collector&) const;
	
    private:

	/** Type containing MPI job information (a set of host/pid pairs). */
	typedef std::set<std::pair<std::string, pid_t> > mpi_job;
	
	static void getMPIJobFromMPT(const Thread&, mpi_job&);
	static void getMPIJobFromEtnus(const Thread&, mpi_job&);
	
	/** Experiment database. */
	SmartPtr<Database> dm_database;

    };
    
} }



#endif
