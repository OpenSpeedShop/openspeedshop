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
 * Declaration of the Application class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Application_
#define _OpenSpeedShop_Framework_Application_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CollectorGroup.hxx"
#include "Lockable.hxx"
#include "ProcessTable.hxx"
#include "StringTable.hxx"
#include "ThreadGroup.hxx"

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

    class Collector;
    class Path;
    class Process;
    class SymbolTable;
    class Thread;

    /**
     * Group of related application threads.
     *
     * Container holding a group of related threads from a target application
     * along with the collectors gathering performance data for those threads.
     * This class is the top-level interface by which a tool interacts with the
     * framework. Member functions are provided for adding individual threads
     * to an application, creating collectors, and accessing the thread and
     * collector groups. All collected performance data for an application can
     * be saved to, and restored from, a file using member functions provided
     * here.
     *
     * @note    New threads created by any thread already within an application
     *          are automatically added to that application. The new thread will
     *          also be attached to any collectors to which its parent thread
     *          had been previously attached. As an example, a process P is
     *          added to an application A via A.attachProcess(), a collector C
     *          is created in A via A.createCollector(), and P is then attached
     *          to C via C->attachThread(P). Process P subsequently creates a 
     *          child process Q via fork(). The framework will automatically 
     *          attach to Q in A and attach Q to C. The next time application A,
     *          or collector C, is queried for its list of threads, process Q
     *          will be found in that list.
     *
     * @note    The term "related" is loosely defined here. In reality no direct
     *          parent/child relationship is enforced or necessarily implied
     *          between threads within an application. The term simply expresses
     *          the typical usage scenario.
     *
     * @ingroup ToolAPI
     */
    class Application :
	private Lockable
    {
	friend class Process;
	friend class SymbolTable;	
	
    public:
	
	~Application();
	
	ThreadGroup getThreads() const;

	Thread* createProcess(const std::string&,
			      const std::string& = getLocalHost());
	
	void attachProcess(const pid_t&, const std::string& = getLocalHost());
	void attachPosixThread(const pid_t&, const pthread_t&,
			       const std::string& = getLocalHost());
#ifdef HAVE_OPENMP
	void attachOpenMPThread(const pid_t&, const int&,
				const std::string& = getLocalHost());
#endif
#ifdef HAVE_ARRAY_SERVICES	
	void attachArraySession(const ash_t&,
				const std::string& = getLocalHost());
#endif

	void removeThread(Thread*);

	CollectorGroup getCollectors() const;
	Collector* createCollector(const std::string&);
	void removeCollector(Collector*);
	
	void saveFile(const Path&) const;
	void restoreFile(const Path&);
	
    private:

	static std::string getLocalHost();

	void addSymbolTable(const SymbolTable*);
	const SymbolTable* findSymbolTable(const Path&) const;
	
	/** Group of all threads. */
	ThreadGroup dm_threads;
	
	/** Group of all collectors. */
	CollectorGroup dm_collectors;
	
	/** Table of all processes. */
	ProcessTable<Thread, Process> dm_processes;

	/** List of all symbol tables. */
	std::vector<const SymbolTable*> dm_symbol_tables;
	
	/** String table. */
	StringTable dm_string_table;
	
    };
    
} }



#endif
