////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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
 * Declaration of the Instrumentor namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Instrumentor_
#define _OpenSpeedShop_Framework_Instrumentor_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Job.hxx"
#include "OutputCallback.hxx"
#include "Thread.hxx"

#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class Function;
    class ThreadGroup;

    /**
     * Instrumentor.
     *
     * Namespace containing the thread instrumentor abstraction layer. Provides
     * the means for creating, attaching to, and otherwise manipulating and
     * instrumenting threads in the operating system. Isolating these functions
     * into a separete unit allows for easier substitution of alternate
     * instrumentation mechanisms in the future.
     *
     * @ingroup Implementation
     */
    namespace Instrumentor
    {
	void retain(const Thread&);
	void release(const Thread&);

#if defined(BUILD_CBTF)
	void create(const Thread&, const std::string&,
		    const std::string&,
		    const uint64_t numBE, const bool,
		    const OutputCallback, const OutputCallback);
#else
	void create(const Thread&, const std::string&,
		    const OutputCallback, const OutputCallback);
#endif

	Thread::State getState(const Thread&);
	void changeState(const ThreadGroup&, const Thread::State&);

	void executeNow(const ThreadGroup&, const Collector&,
			const std::string, const Blob&, const bool&);
	void executeAtEntryOrExit(const ThreadGroup&, const Collector&,
				  const std::string&, const bool&,
				  const std::string&, const Blob&);
	void executeInPlaceOf(const ThreadGroup&, const Collector&,
			      const std::string&, const std::string&);

	void uninstrument(const ThreadGroup&, const Collector&);

	void stopAtEntryOrExit(const Thread&, const std::string&, const bool&);
	
	bool getGlobal(const Thread&, const std::string&, int64_t&);
	bool getGlobal(const Thread&, const std::string&, std::string&);
	bool getMPICHProcTable(const Thread&, Job&);

	void setGlobal(const Thread&, const std::string&, int64_t);
	
	void setMPIStartup(const Thread&, const bool);
	bool inMPIStartup(const Thread&);
    }
    
} }



#endif
