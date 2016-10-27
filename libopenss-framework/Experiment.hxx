////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2014 The Krell Institute. All Rights Reserved.
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

#include "AddressRange.hxx"
#include "Database.hxx"
#include "Job.hxx"
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
    class Extent;
    class Function;
    class LinkedObject;
    class Statement;
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

	static std::string getDpcldListenerPort();
	
	static std::string getLocalHost();
	static std::string getCanonicalName(const std::string&);
	static std::string getHostnameFromIP(const std::string& ip);
	
	static bool isAccessible(const std::string&);
	static void create(const std::string&);
	static void remove(const std::string&);

	explicit Experiment(const std::string&);
	virtual ~Experiment();

	void renameTo(const std::string&) const;
	void copyTo(const std::string&) const;
	
	std::string getName() const;
	int getVersion() const;

	ThreadGroup getThreads() const;
	
	ThreadGroup createProcess(const std::string&,
				  const std::string& = getLocalHost(),
                                  uint64_t numBE = 0,
				  const OutputCallback = 
				      OutputCallback(static_cast<OutputCallback::first_type>(NULL), static_cast<OutputCallback::second_type>(NULL)),
				  const OutputCallback = 
				      OutputCallback(static_cast<OutputCallback::first_type>(NULL), static_cast<OutputCallback::second_type>(NULL))) const;
	
	ThreadGroup attachMPIJob(const pid_t&,
				 const std::string& = getLocalHost()) const;
	ThreadGroup attachProcess(const pid_t&,
				  const std::string& = getLocalHost()) const;
	
	void removeThread(const Thread&) const;

        // offline uses this to update database from raw data files.
	void updateThreads(const pid_t&,
			   const pthread_t&,
			   const int&,
			   const std::string&) const;
	// offline mode for cbtf-krell collectors.
	void updateThreads(const pid_t&,
			   const pthread_t&,
			   const int&,
			   const int&,
			   const std::string&) const;

	// pruning unneeded entries in database.
	void compressDB() const;

	CollectorGroup getCollectors() const;
	Collector createCollector(const std::string&) const;
	void removeCollector(const Collector&) const;

	std::set<Function> getFunctionsByNamePattern(const std::string&) const;

	std::set<LinkedObject> getLinkedObjectsByPathPattern(
	    const std::string&) const;
	std::set<Function> getFunctionsByPathPattern(
	    const std::string&) const;
	std::set<Statement> getStatementsByPathPattern(
	    const std::string&) const;

	void flushPerformanceData() const;
	Extent getPerformanceDataExtent() const;
	
	std::string getApplicationCommand();
	void setApplicationCommand(const char *, bool trust_me);

	void prepareToRerun(const OutputCallback =
			        OutputCallback(static_cast<OutputCallback::first_type>(NULL), static_cast<OutputCallback::second_type>(NULL)),
			    const OutputCallback =
			        OutputCallback(static_cast<OutputCallback::first_type>(NULL), static_cast<OutputCallback::second_type>(NULL))) const;

	std::set<LinkedObject> getExecutables() const;

        bool getIsInstrumentorOffline () {return isInstrumentorOffline;}
        void setIsInstrumentorOffline (bool flag) {isInstrumentorOffline = flag;}
        bool getInstrumentorUsesCBTF () {return instrumentorUsesCBTF;}
        void setInstrumentorUsesCBTF (bool flag) {instrumentorUsesCBTF = flag;}


        int numBEprocs;

        void setBEprocCount(int count) {
           //std::cerr << "Enter setBEprocCount, count=" << count << std::endl;
           numBEprocs = count;
        }

        int getBEprocCount() {
           //std::cerr << "Enter getBEprocCount, numBEprocs=" << numBEprocs
           //    << std::endl;
           return numBEprocs;
        }


        int rerunCount;

        void setRerunCount(int count) {
           rerunCount = count;
        }

        void incrementRerunCount() {
           rerunCount = rerunCount + 1;
        }

        int getRerunCount() {
           return rerunCount;
        }

        void setDatabaseViewHeader( std::string db_name,  std::string view_cmd_arg);
        int searchForViewCommandHeaderMatch( std::string db_name,  std::string view_cmd_arg, bool exact_match );
        //bool addViewCommandAndDataEntries( std::string db_name,  std::string view_cmd_arg, Blob view_data_arg );
        bool addViewCommandAndDataEntries( std::string db_name,  std::string view_cmd_arg, std::string view_data_filename_arg, std::ostream *outs, int view_file_header_offset);
        bool getViewFromExistingCommandEntry(std::string db_name, std::string view_cmd_arg, std::string &returned_view_data, int &size_of_returned_view_data);
        std::vector<std::string> getSavedCommandList(std::string db_name) const;
        void removeViewTableEntry(const std::string view_cmd_arg, const int views_id_arg, const OpenSpeedShop::Framework::SmartPtr<OpenSpeedShop::Framework::Database> database_arg) const;
        void removeNonCrossSessionViews(std::string db_name);
	bool addView(std::string&, std::string& );
		
    private:

	ThreadGroup attachJob(const Job&) const;
	ThreadGroup getThreadsInProcess(const pid_t&, const std::string&) const;

	void updateToVersion2() const;
	void updateToVersion3() const;
	void updateToVersion4() const;
	void updateToVersion5() const;
	void updateToVersion6() const;
	void updateToVersion7() const;

#ifndef NDEBUG
	static bool is_debug_mpijob_enabled;
	static bool is_debug_offline_enabled;
	static bool is_debug_reuse_views_enabled;
#endif

	static bool getMPIJobFromMPT(const Thread&, Job&);
	static bool getMPIJobFromMPICH(const Thread&, Job&);

        bool isInstrumentorOffline;
        bool instrumentorUsesCBTF;

	/** Experiment database. */
	SmartPtr<Database> dm_database;

    };
    
} }



#endif
