////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration of the Senders namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Senders_
#define _OpenSpeedShop_Framework_Senders_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Thread.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class ThreadGroup;

    /**
     * Message senders.
     *
     * Namespace containing the message sender functions. These functions
     * package and send outgoing requests to the backends.
     *
     * @ingroup Implementation
     */
    namespace Senders
    {
	void attachToThreads(const ThreadGroup&);
	void changeThreadsState(const ThreadGroup&, const Thread::State&);
	void createProcess(const Thread&, const std::string&, const Blob&);
	void detachFromThreads(const ThreadGroup&);
	void executeNow(const ThreadGroup&, const Collector&,
			const bool&, const std::string&, const Blob&);
	void executeAtEntryOrExit(const ThreadGroup&, const Collector&,
				  const std::string&, const bool&,
				  const std::string&, const Blob&);
	void executeInPlaceOf(const ThreadGroup&, const Collector&,
			      const std::string&, const std::string&);
	void getGlobalInteger(const Thread&, const std::string&);
	void getGlobalString(const Thread&, const std::string&);
	void getMPICHProcTable(const Thread&, const std::string&);
	void setGlobalInteger(const Thread&, const std::string&,
			      const int64_t&);
	void stdIn(const Thread&, const Blob&);
	void stopAtEntryOrExit(const ThreadGroup&,
			       const std::string&, const bool&);
	void uninstrument(const ThreadGroup&, const Collector&);
	void MPIStartup(const ThreadGroup&, const bool&);
    }
    
} }



#endif
