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
 * Declaration of the Callbacks namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Callbacks_
#define _OpenSpeedShop_Framework_Callbacks_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    class Blob;

    /**
     * Message callbacks.
     *
     * Namespace containing the message callback functions. These callbacks
     * are registered with the backend message pump by the daemon's main()
     * and will handle all incoming requests from the frontend.
     *
     * @ingroup Implementation
     */
    namespace Callbacks
    {
	void attachToThreads(const Blob&);
	void changeThreadsState(const Blob&);
	void createProcess(const Blob&);
	void detachFromThreads(const Blob&);
	void executeNow(const Blob&);
	void executeAtEntryOrExit(const Blob&);
	void executeInPlaceOf(const Blob&);
	void getGlobalInteger(const Blob&);
	void getGlobalString(const Blob&);
	void getMPICHProcTable(const Blob&);
	void setGlobalInteger(const Blob&);
	void stdIn(const Blob&);
	void stopAtEntryOrExit(const Blob&);
	void uninstrument(const Blob&);
	void MPIStartup(const Blob&);
    }
    
} }



#endif
