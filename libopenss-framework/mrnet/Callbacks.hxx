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
     * are registered with the message pump by the daemon's main() and will
     * handle all the incoming requests from the framework.
     *
     * @ingroup Implementation
     */
    namespace Callbacks
    {
	void attachedToThread(const Blob&);
	void globalIntegerValue(const Blob&);
	void globalJobValue(const Blob&);
	void globalStringValue(const Blob&);
	void loadedLinkedObject(const Blob&);
	void reportError(const Blob&);
	void symbolTable(const Blob&);
	void threadsStateChanged(const Blob&);
	void unloadedLinkedObject(const Blob&);
    }
    
} }



#endif
