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
 * Declaration and definition of the ProcessTable template.
 *
 */

#ifndef _OpenSpeedShop_Framework_ProcessTable_
#define _OpenSpeedShop_Framework_ProcessTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Lockable.hxx"

#include <map>
#include <set>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * Process table.
     *
     * Template used for tracking singular process objects that are referenced
     * by more than one other object. The process and reference object types
     * are specified by the template instantiation. Used in the Process class
     * to track the Process to DPCL process handle mapping. Also used in the
     * Application class to track the Thread to Process mapping.
     *
     * @ingroup Implementation
     *
     * @todo    The reference-to-process map may be unnecessary and could
     *          possibly be removed in the future.
     */
    template <typename R, typename P>
    class ProcessTable :
	public Lockable
    {
	
    public:

	/** Test if there are no processes in the table. */
	bool empty() const
	{
	    return dm_name_to_process.empty();
	}
	
	/** Add a process. */
	void addProcess(const std::string& host, const pid_t& pid, P* process)
	{
	    // Check preconditions	    
	    Assert(dm_name_to_process.find(std::make_pair(host, pid)) ==
		   dm_name_to_process.end());
	    Assert(dm_process_to_reference.find(process) ==
		   dm_process_to_reference.end());
	    
	    // Add this process
	    dm_name_to_process[std::make_pair(host, pid)] = process;
	    dm_process_to_reference[process] = std::set<R*>();
	}
	
	/** Add a reference to a process. */
	void addReference(R* reference, P* process)
	{
	    // Check preconditions
	    Assert(dm_process_to_reference.find(process) !=
		   dm_process_to_reference.end());
	    Assert(dm_reference_to_process.find(reference) ==
		   dm_reference_to_process.end());
	    
	    // Add this reference
	    (dm_process_to_reference.find(process))->second.insert(reference);
	    dm_reference_to_process[reference] = process;	    
	}
	
	/** Remove a process. */
	void removeProcess(const std::string& host, const pid_t& pid,
			   P* process)
	{
	    // Check preconditions
	    Assert(dm_name_to_process.find(std::make_pair(host, pid)) !=
		   dm_name_to_process.end());
	    Assert(dm_process_to_reference.find(process) !=
		   dm_process_to_reference.end());
	    Assert(dm_process_to_reference.find(process)->second.empty());
	    
	    // Remove this process
	    dm_name_to_process.erase(std::make_pair(host, pid));
	    dm_process_to_reference.erase(process);
	}
	
	/** Remove a reference to a process. */
	void removeReference(R* reference, P* process)
	{
	    // Check preconditions
	    Assert(dm_process_to_reference.find(process) !=
		   dm_process_to_reference.end());
	    Assert(dm_reference_to_process.find(reference) !=
		   dm_reference_to_process.end());
	    
	    // Remove this reference
	    (dm_process_to_reference.find(process))->second.erase(reference);
	    dm_reference_to_process.erase(reference);
	}

	/** Find a process by its host/PID pair. */
	P* findProcessByName(const std::string& host, const pid_t& pid) const
	{
	    typename std::map<std::pair<std::string, pid_t>, P*>::const_iterator
		i = dm_name_to_process.find(std::make_pair(host, pid));
	    return (i != dm_name_to_process.end()) ? i->second : NULL;
	}

	/** Find a process by a reference. */
	P* findProcessByReference(R* reference) const
	{
	    typename std::map<R*, P*>::const_iterator
		i = dm_reference_to_process.find(reference);
	    return (i != dm_reference_to_process.end()) ? i->second : NULL;
	}
	
	/** Find the references for a process. */
	std::set<R*> findReferencesByProcess(P* process) const
	{
	    typename std::map<P*, std::set<R*> >::const_iterator
		i = dm_process_to_reference.find(process);
	    return (i != dm_process_to_reference.end()) ? i->second :
		std::set<R*>();
	}	

	/** Find references by host/PID pair. */
	std::set<R*> findReferencesByName(const std::string& host,
						const pid_t& pid) const
	{
	    P* process = findProcessByName(host, pid);
	    return (process != NULL) ? findReferencesByProcess(process) :
		std::set<R*>();
	}
	
    private:

	/** Map host/PID pairs to their process. */
	std::map<std::pair<std::string, pid_t>, P*> dm_name_to_process;
	
	/** Map processes to their references. */
	std::map<P*, std::set<R*> > dm_process_to_reference;
	
	/** Map references to their process. */
	std::map<R*, P*> dm_reference_to_process;
	
    };

} }



#endif
