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
 * Declaration of the Instrumentor namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Instrumentor_
#define _OpenSpeedShop_Framework_Instrumentor_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Thread.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Blob;

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
	void createUnderlyingThread(const Thread&, const std::string&);	
	void attachUnderlyingThread(const Thread&);
	void detachUnderlyingThread(const Thread&);
	Thread::State getThreadState(const Thread&);
	void changeThreadState(const Thread&, const Thread::State&);
	void loadLibrary(const Thread&, const std::string&);
	void unloadLibrary(const Thread&, const std::string&);
	void execute(const Thread&, const std::string&, const std::string&,
		     const Blob&);
    }
    
} }



#endif
