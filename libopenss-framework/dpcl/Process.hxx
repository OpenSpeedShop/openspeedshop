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
#include "Collector.hxx"
#include "Path.hxx"
#include "Thread.hxx"

#include <dpcl.h>
#include <map>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Lockable;
    struct SymbolTableState;
    class ThreadGroup;
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
	bool isConnected() const;

	void executeNow(const Collector&, const Thread&,
			const std::string&, const Blob&);
	void executeAtEntryOrExit(const Collector&, const Thread&,
				  const Function&, const bool&,
				  const std::string&, const Blob&);
	
	void uninstrument(const Collector&, const Thread&);
	
    private:

#ifndef NDEBUG
	static bool is_debug_enabled;
	
	static void debugCallback(const std::string&, const std::string&);
	static void debugDPCL(const std::string&, const AisStatus&);
	void debugState() const;
	void debugRequest(const std::string&) const;
#endif
	
	static void initialize();
	static void finalize();

	static Path searchForExecutable(const Path&);

	static std::pair<std::string, std::string> 
	parseLibraryFunctionName(const std::string&);	

	static void finishSymbolTableProcessing(SymbolTableState*);

	static void activateProbeCallback(GCBSysType, GCBTagType, 
					  GCBObjType, GCBMsgType);	
	static void attachCallback(GCBSysType, GCBTagType, 
				   GCBObjType, GCBMsgType);
	static void connectCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void deactivateProbeCallback(GCBSysType, GCBTagType,
					    GCBObjType, GCBMsgType);
	static void destroyCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void disconnectCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void executeCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void expandCallback(GCBSysType, GCBTagType,
				   GCBObjType, GCBMsgType);
	static void installProbeCallback(GCBSysType, GCBTagType,
					 GCBObjType, GCBMsgType);
	static void loadModuleCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void performanceDataCallback(GCBSysType, GCBTagType,
					    GCBObjType, GCBMsgType);
	static void removeProbeCallback(GCBSysType, GCBTagType,
					GCBObjType, GCBMsgType);
	static void resumeCallback(GCBSysType, GCBTagType,
				   GCBObjType, GCBMsgType);
	static void statementsCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void stderrCallback(GCBSysType, GCBTagType,
				   GCBObjType, GCBMsgType);
	static void stdoutCallback(GCBSysType, GCBTagType,
				   GCBObjType, GCBMsgType);
	static void suspendCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void terminationCallback(GCBSysType, GCBTagType,
					GCBObjType, GCBMsgType);
	static void unloadModuleCallback(GCBSysType, GCBTagType,
					 GCBObjType, GCBMsgType);
	
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
	 * Structure for an entry in the library table. Describes a single
	 * library that has been loaded into the process for a collector.
	 */
	struct LibraryEntry {
	    
	    /** Collector for which the library was loaded. */
	    Collector dm_collector;
	    
	    /** Name of this library. */
	    std::string dm_name;
	    
	    /** Full path of this library. */
	    Path dm_path;
	    
	    /** Probe module for this library. */
	    ProbeModule dm_module;

	    /** Map function names to their probe module indicies. */
	    std::map<std::string, int> dm_functions;
	    
	    /** Messaging probe for this library. */
	    ProbeHandle dm_messaging;

	    /** Probes used with this library. */
	    std::multimap<Thread, ProbeHandle> dm_probes;
	    
	    /** Constructor from collector and library name. */
	    LibraryEntry(const Collector& collector, const std::string& name) :
		dm_collector(collector),
		dm_name(name),
		dm_path(),
		dm_module(),
		dm_functions(),
		dm_messaging(),
		dm_probes()
	    {
	    }

	};
	
	/** Map loaded libraries to their entries. */
	std::map<std::pair<Collector, std::string>, LibraryEntry> dm_libraries;
	
	void requestAddressSpace(const ThreadGroup&, const Time&);	
	void requestAttach();
	void requestConnect();
	void requestDeactivateAndRemove(ProbeHandle);
	void requestDestroy();
	void requestDisconnect();
	void requestExecute(ProbeExp, GCBFuncType, GCBTagType);
	ProbeHandle requestInstallAndActivate(ProbeExp, InstPoint,
					      GCBFuncType, GCBTagType);
	void requestLoadModule(LibraryEntry&);
	void requestResume();
	void requestSuspend();
	void requestUnloadModule(LibraryEntry&);

	void initializeMessaging(LibraryEntry&);
	void finalizeMessaging(LibraryEntry&);

	std::pair<ProbeExp, std::multimap<Thread, ProbeHandle>::iterator>
	prepareCallTo(const Collector&, const Thread&,
		      const std::string&, const Blob&);
	
    };
    
} }



#endif
