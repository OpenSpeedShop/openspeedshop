////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2012,2013 The Krell Institute. All Rights Reserved.
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

#include "Entry.hxx"
#include "Time.hxx"

#include <pthread.h>
#include <set>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class CollectorGroup;
    class Database;
    class DataCache;
    class Experiment;
    class Function;
    class LinkedObject;
    class Loop;
    class Path;
    class Process;
    template <typename> class SmartPtr;
    class Statement;
    class ThreadGroup;

    namespace Callbacks {
        void attachedToThreads(const Blob&);
    }
    
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
     * @note    The existence (creation and destruction) of a Thread object does
     *          not directly mirror the existence of the real underlying thread.
     *          One can create a Thread for an underlying thread that did not
     *          exist. An underlying thread may terminate and the corresponding
     *          Thread object will continue to exist. This class serves as a
     *          proxy for performing operations on the real thread if and when
     *          it exists.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Thread :
        public Entry
    {
        friend class Collector;
        friend class DataCache;
        friend class Experiment;
        friend class Function;
        friend class LinkedObject;
        friend class Loop;
        friend class Process;
        friend class Statement;
        friend class ThreadGroup;
        
        friend void Callbacks::attachedToThreads(const Blob&);
	
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
            Disconnected,  /**< Thread isn't connected (may not even exist). */
            Connecting,    /**< Thread is being connected. */
            Nonexistent,   /**< Thread doesn't exist. */
            Running,       /**< Thread is active and running. */
            Suspended,     /**< Thread has been temporarily suspended. */
            Terminated     /**< Thread has terminated. */
        };
        
        State getState() const;
        bool isState(const State&) const;
        void changeState(const State&) const;
        
        std::pair<bool, std::string> getCommand() const;
        void setCommand(const char *) const;	
        std::pair<bool, std::string> getMPIImplementation() const;
        void setMPIImplementation(const std::string) const;	
        std::string getHost() const;
        pid_t getProcessId() const;
        std::pair<bool, pthread_t> getPosixThreadId() const;
        std::pair<bool, int> getOpenMPThreadId() const;
        std::pair<bool, int> getMPIRank() const;
        
        std::set<LinkedObject> getLinkedObjects() const;
        std::set<Function> getFunctions() const;
        std::set<Loop> getLoops() const;
        std::set<Statement> getStatements() const;
        
        std::pair<bool, LinkedObject> getLinkedObjectAt(
            const Address&, const Time& = Time::Now()) const;
        std::pair<bool, Function> getFunctionAt(
            const Address&, const Time& = Time::Now()) const;
        std::set<Loop> getLoopsAt(
            const Address&, const Time& = Time::Now()) const;
        std::set<Statement> getStatementsAt(
            const Address&, const Time& = Time::Now()) const;
        
        // Used by Experiment::compressDB to prune an OpenSpeedShop database of
        // any entries not found in the experiments sampled addresses.
        std::pair<std::pair<bool, Function> , std::set<Statement> >
	    getFunctionAndStatementsAt(const Address& address) const;
        
        std::pair<bool, LinkedObject> getExecutable(
            const Time& = Time::Now()) const;	
        std::pair<bool, Function> getFunctionByName(const std::string&) const;
        std::set<Loop> getLoopsBySourceFile(const Path&) const;
        std::set<Statement> getStatementsBySourceFile(const Path&) const;
        
        CollectorGroup getCollectors() const;
        CollectorGroup getPostponedCollectors() const;
        
        Thread(const SmartPtr<Database>&, const int&);
        
    private:
        
        Thread();
        
        bool doesSiblingExist(const pthread_t&) const;
        void setPosixThreadId(const pthread_t&) const;	
        Thread createCopy() const;
        
    };
        
    std::string toString(const Thread::State&);
        
} }



#endif
