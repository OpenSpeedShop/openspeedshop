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
 * Declaration of the Process class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Process_
#define _OpenSpeedShop_Framework_Process_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Lockable.hxx"
#include "Path.hxx"
#include "Thread.hxx"

#include <map>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <vector>

class ProbeHandle;
class ProbeModule;
class Process;
class SourceObj;



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Time;

    /**
     * Process.
     *
     * A single process to which we have attached ourselves. Mainly exists to
     * provide a wrapper for DPCL process handles that can be shared by multiple
     * Thread objects. Member functions are provided here for querying and/or
     * manipulating the process.
     *
     * @ingroup Implementation
     */
    class Process :
	private Lockable
    {

    public:

	static std::string formUniqueName(const std::string&, const pid_t&);
	
	Process(const std::string&, const std::string&);
	Process(const std::string&, const pid_t&);
	
	~Process();
	
	std::string getHost() const;
        pid_t getProcessId() const;
	
	Thread::State getState() const;
	void changeState(const Thread::State&);
	
	void completeStateChange();
	void terminateNow();

	void updateAddressSpace(const Thread&, const Time&);
	void processFunctions(const Thread&, const int&, const AddressRange&,
			      std::vector<SourceObj>&) const;	
	void processStatements(const Thread&, const int&, const AddressRange&, 
			       std::vector<SourceObj>&) const;

	void loadLibrary(const std::string&);
	void unloadLibrary(const std::string&);
	void execute(const std::string&, const std::string&, const Blob&);

    private:
	
	static Path searchForExecutable(const Path&);

	/** DPCL process handle. */
	::Process* dm_process;

	/** Name of host on which this process is located. */
        std::string dm_host;
	
        /** Identifier of this process. */
        pid_t dm_pid;
	
	/** Current state of this process. */
	Thread::State dm_current_state;
	
	/** Flag indicating if process is changing state. */
	bool dm_is_state_changing;
	
	/** Future state of this process. */
	Thread::State dm_future_state;

	/**
	 * Library entry.
	 *
	 * Structure for an entry in the library table describing a single
	 * library that has been loaded into the process. Contains the name,
	 * full path, probe module, and reference count for the library.
	 */
	struct LibraryEntry {

	    /** Name of this library. */
	    std::string name;
	    
	    /** Full path of this library. */
	    Path path;
	    
	    /** Probe module for this library. */
	    ProbeModule* module;
	    
	    /** Number of references to this library. */
	    unsigned references;

	    /** Map function names to their probe module indicies. */
	    std::map<std::string, int> functions;

	    /** Handle of the messaing probe for this library. */
	    ProbeHandle* messaging;
	    
	};

	/** Map loaded library names to their entries. */
	std::map<std::string, LibraryEntry> dm_library_name_to_entry;
	
	void initializeMessaging(LibraryEntry&);
	void finalizeMessaging(LibraryEntry&);
	
    };
    
} }



#endif
