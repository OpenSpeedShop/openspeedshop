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
 * Declaration and definition of the MessageCallback type.
 *
 */

#ifndef _OpenSpeedShop_Framework_MessageCallback_
#define _OpenSpeedShop_Framework_MessageCallback_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    class Blob;
	
    /**
     * Message callback.
     *
     * Pointer to a function that is called for handling a message. A blob
     * containing the message is passed to the function as a parameter.
     *
     * @ingroup Implementation
     */
    typedef void (*MessageCallback)(const Blob&);
	
} }



#endif
