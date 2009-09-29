////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008-2009 Krell Institute. All Rights Reserved.
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
 * Declaration of the ThreadTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ThreadTable_
#define _OpenSpeedShop_Framework_ThreadTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "SmartPtr.hxx"
#include "StdStreamPipes.hxx"
#include "ThreadName.hxx"

#include <BPatch.h>
#include <map>
#include <set>



namespace OpenSpeedShop { namespace Framework {

    class ExperimentGroup;
    class ThreadNameGroup;

    /**
     * Thread table.
     *
     * Table used to keep track of Dyninst thread object pointers. Dyninst does
     * provide a list of active thread pointers, but using it to find a specific
     * thread requires a linear time search. This table provides the same search
     * in logarithmic time.
     *
     * @ingroup Implementation
     */
    class ThreadTable :
	private Lockable
    {

    public:

	static ThreadTable TheTable;

	static BPatch_thread* getPtrDirectly(const ThreadName&);
	
	ThreadTable();
	
	void addThread(const ThreadName&, BPatch_thread*,
		       const SmartPtr<StdStreamPipes>& = 
		           SmartPtr<StdStreamPipes>());
	void removeThread(const ThreadName&, BPatch_thread*);

        std::set<pid_t> getActivePids() const;
        std::vector<std::pair<pid_t, std::pair< std::pair< bool, pthread_t>, std::string> > > getActivePidsAndHosts() const;
	
	BPatch_thread* getPtr(const ThreadName&) const;
	ThreadNameGroup getNames(/* const */ BPatch_thread*) const;
	ThreadNameGroup getNames(/* const */ BPatch_process*) const;
	ExperimentGroup getExperiments(/* const */ BPatch_process&) const;

	SmartPtr<StdStreamPipes>
	getStdStreamPipes(/* const */ BPatch_thread*) const;

	std::set<int> getStdInFDs() const;
	std::set<int> getStdErrFDs() const;
	std::set<int> getStdOutFDs() const;

	ThreadNameGroup getNames(const int&) const;

	OpenSS_Protocol_ThreadState
	getThreadState(/* const */ BPatch_thread*) const;

	void setThreadState(/* const */ BPatch_thread*,
			    const OpenSS_Protocol_ThreadState&);
	void setThreadState(const ThreadNameGroup&,
			    const OpenSS_Protocol_ThreadState&);
	
    private:

	/** Map thread names to their Dyninst thread object pointer. */
	std::map<ThreadName, BPatch_thread*> dm_name_to_ptr;

	/** Map Dyninst thread object pointers to their names. */
	std::multimap<BPatch_thread*, ThreadName> dm_ptr_to_names;

	/** Map Dyninst thread object pointers to their pipes. */
	std::map<BPatch_thread*, SmartPtr<StdStreamPipes> > dm_ptr_to_pipes;

	/** Map Dyninst thread object pointers to their state. */
	std::map<BPatch_thread*, OpenSS_Protocol_ThreadState> dm_ptr_to_state;
	
    };

} }



#endif
