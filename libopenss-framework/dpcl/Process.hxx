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
#include "CollectorGroup.hxx"
#include "Job.hxx"
#include "OutputCallback.hxx"
#include "Path.hxx"
#include "Thread.hxx"

#include <dpcl.h>
#include <map>
#include <pthread.h>
#include <set>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Lockable;
    class ProcessTable;
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
	friend class ProcessTable;

    public:

	Process(const std::string&, const std::string&,
		const OutputCallback, const OutputCallback);
	Process(const std::string&, const pid_t&);
	
	~Process();

	std::string getHost() const;
        pid_t getProcessId() const;

	bool isConnected();
	
	Thread::State getState(const Thread&);
	void changeState(const Thread&, const Thread::State&);

	void executeNow(const Collector&, const Thread&,
			const std::string&, const Blob&, const bool&);
	void executeAtEntryOrExit(const Collector&, const Thread&,
				  const std::string&, const bool&,
				  const std::string&, const Blob&);
	void executeInPlaceOf(const Collector&, const Thread&,
			      const std::string&, const std::string&);	

	void uninstrument(const Collector&, const Thread&);

	void stopAtEntryOrExit(const Thread&, const std::string&, const bool&);

	bool getGlobal(const std::string&, int64_t&);
	bool getGlobal(const std::string&, std::string&);
	bool getMPICHProcTable(Job&);
	
        //Martin: added routine to set value
        bool setGlobal(const std::string& global, int64_t value);

	bool inMPIStartup();
	void setMPIStartup (bool);

    private:

#ifndef NDEBUG	
	static bool is_debug_enabled;
	static bool is_debug_perf_enabled;

	/**
	 * Performance data events enumeration.
	 *
	 * Enumeration defining all the different event times that are recorded
	 * as part of the per-process performance data.
	 */
	enum PerformanceDataEvents {
	    Created,              /**< Process object created. */
	    ConnectIssued,        /**< DPCL connect was issued. */
	    ConnectAcknowledged,  /**< DPCL connect was acknowledged. */
	    ConnectCompleted,     /**< DPCL connect was completed. */
	    AttachIssued,         /**< DPCL attach was issued. */
	    AttachAcknowledged,   /**< DPCL attach was acknowledged. */
	    AttachCompleted,      /**< DPCL attach was completed. */
	    GetThreadsIssued,     /**< DPCL bget_threads was issued. */
	    GetThreadsCompleted,  /**< DPCL bget_threads was completed. */
	    RqstAddrSpcEntered,   /**< Entered requestAddressSpace(). */
	    RqstAddrSpcIssue,     /**< Issue DPCL symbol table requests. */
	    RqstAddrSpcExited,    /**< Exited requestAddressSpace(). */
	    FSTPEntered,          /**< Entered finishSymbolTableProcess(). */
	    Ready                 /**< Process object ready for use. */
	};

	/** Performance data for this process. */
	mutable std::map<PerformanceDataEvents, Time> dm_perf_data;

	/** Previous values returned by getState(). */
	mutable std::map<std::string, Thread::State> dm_previous_getstate;
	
	static void debugCallback(const std::string&, const std::string&);
	static void debugDPCL(const std::string&, const AisStatus&);
	static void debugBuffer(std::ostream&, const char*, const unsigned&);
	void debugState(const std::string&) const;
	void debugRequest(const std::string&, const std::string&) const;
#endif

	static std::string formUniqueName(const std::string&, const pid_t&);
	static std::string formUniqueName(const std::string&,
					  const pid_t&, const pthread_t&);
	static std::string getProcessFromUniqueName(const std::string&);
	static std::string getThreadFromUniqueName(const std::string&);

	static Path searchForExecutable(const Path&);

	static std::pair<std::string, std::string> 
	parseLibraryFunctionName(const std::string&);	

	static void finishSymbolTableProcessing(SymbolTableState*);

	static void activateProbeCallback(GCBSysType, GCBTagType, 
					  GCBObjType, GCBMsgType);
	static void addressSpaceChangeCallback(GCBSysType, GCBTagType, 
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
	static void freeMemCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void getBlobCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void getIntegerCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void setIntegerCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void getJobCallback(GCBSysType, GCBTagType,
				   GCBObjType, GCBMsgType);
	static void installProbeCallback(GCBSysType, GCBTagType,
					 GCBObjType, GCBMsgType);
	static void loadModuleCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType);
	static void outOfBandDataCallback(GCBSysType, GCBTagType,
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
	static void stoppedCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void suspendCallback(GCBSysType, GCBTagType,
				    GCBObjType, GCBMsgType);
	static void terminationCallback(GCBSysType, GCBTagType,
					GCBObjType, GCBMsgType);
	static void threadListChangeCallback(GCBSysType, GCBTagType,
					     GCBObjType, GCBMsgType);
	static void unloadModuleCallback(GCBSysType, GCBTagType,
					 GCBObjType, GCBMsgType);
	
	/** DPCL process handle. */
	::Process* dm_process;
	
	/** Name of host on which this process is located. */
        std::string dm_host;
	
        /** Identifier of this process. */
        pid_t dm_pid;

	bool in_mpi_startup;

	/** Standard output stream callback. */
	OutputCallback dm_stdout_callback;
	
	/** Standard error stream callback. */
	OutputCallback dm_stderr_callback;

	/** Current state of process/thread. */
	std::map<std::string, Thread::State> dm_current_state;

	/** Flags indicating if process/thread is changing state. */
	std::map<std::string, bool> dm_is_state_changing;

	/** Future state of process/thread. */
	std::map<std::string, Thread::State> dm_future_state;

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
	    
	    /** Probes used with this library. */
	    std::multimap<Thread, ProbeHandle> dm_probes;

	    /** Variables used with this library. */
	    std::multimap<Thread, ProbeExp> dm_variables;
	    
	    /** Constructor from collector and library name. */
	    LibraryEntry(const Collector& collector, const std::string& name) :
		dm_collector(collector),
		dm_name(name),
		dm_path(),
		dm_module(),
		dm_functions(),
		dm_probes()
	    {
	    }

	};
	
	/** Map loaded libraries to their entries. */
	std::map<std::pair<Collector, std::string>, LibraryEntry> dm_libraries;
	
	void requestAddressSpace(const ThreadGroup&, const Time&, const bool);	
	void requestAttach();
	void requestConnect();
	void requestDeactivateAndRemove(ProbeHandle);
	void requestDestroy();
	void requestDestroy(pthread_t);
	void requestDisconnect();
	void requestExecute(ProbeExp, GCBFuncType, GCBTagType);
	void requestExecute(ProbeExp, GCBFuncType, GCBTagType, pthread_t);
	void requestFree(ProbeExp);
	ProbeHandle requestInstallAndActivate(ProbeExp, InstPoint,
					      GCBFuncType, GCBTagType);
	ProbeHandle requestInstallAndActivate(ProbeExp, InstPoint,
					      GCBFuncType, GCBTagType, 
					      pthread_t);
	void requestLoadModule(LibraryEntry&);
	void requestResume();
	void requestResume(pthread_t);
	void requestSuspend();
	void requestSuspend(pthread_t);
	void requestUnloadModule(LibraryEntry&);

	Thread::State findCurrentState(const std::string&) const;
	bool findIsChangingState(const std::string&) const;
	Thread::State findFutureState(const std::string&) const;
	void setCurrentState(const std::string&, const Thread::State&);
	void setFutureState(const std::string&, const Thread::State&);

	SourceObj findFunction(const std::string&) const;
	SourceObj findVariable(const std::string&) const;

	std::pair<LibraryEntry*, ProbeExp>
	findLibraryFunction(const Collector&, const std::string&);	
	
	bool getPosixThreadIds(std::set<pthread_t>&) const;

    };
    
} }



#endif
