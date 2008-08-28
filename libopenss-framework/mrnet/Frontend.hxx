////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration of the Frontend namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Frontend_
#define _OpenSpeedShop_Framework_Frontend_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>

#include "MessageCallback.hxx"
#include "Protocol.h"



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Path;

    /**
     * MRNet frontend.
     *
     * Namespace containing procedural functions for registering/unregistering
     * callbacks with, starting, and stopping the MRNet frontend message pump.
     * This pump is responsible for receiving and procdessing incoming messages
     * from the backends. A function is also provided for sending messages to
     * the backends.
     *
     * @ingroup Implementation
     */
    namespace Frontend
    {
	void registerCallback(const int&, const MessageCallback);
	void unregisterCallback(const int&, const MessageCallback);

	void startMessagePump(const Path&);
	void stopMessagePump();

	void sendToBackends(const int&, const Blob&,
			    const OpenSS_Protocol_ThreadNameGroup&);
	void sendToAllBackends(const int&, const Blob&);

	bool hasBackend(const std::string&);

#ifndef NDEBUG
	bool isDebugEnabled();
	bool isPerfDataDebugEnabled();
	bool isStdioDebugEnabled();
	bool isSymbolsDebugEnabled();
#endif
    }
    
} }



#endif
