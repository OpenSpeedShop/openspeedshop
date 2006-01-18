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
 * Declaration of the MainLoop namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_MainLoop_
#define _OpenSpeedShop_Framework_MainLoop_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * DPCL main loop.
     *
     * Namespace containing procedural functions for starting, suspending, 
     * resuming, and stopping the DPCL main loop. That loop is responsible for
     * processing incoming, asynchronous, messages coming from processes under
     * DPCL's control. In order to handle those messages in a timely manner,
     * the main loop is run within a separate thread. Unfortunately the DPCL
     * client library isn't thread-safe and thus a mechanism is provided here
     * for insuring that only one thread accesses that library at a given time.
     *
     * @ingroup Implementation
     */
    namespace MainLoop
    {
	std::string start();
	void suspend();
	void resume();
	void stop();
    }
    
} }



#endif
