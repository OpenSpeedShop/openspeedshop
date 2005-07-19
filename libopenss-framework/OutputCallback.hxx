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
 * Declaration and definition of the OutputCallback type.
 *
 */

#ifndef _OpenSpeedShop_Framework_OutputCallback_
#define _OpenSpeedShop_Framework_OutputCallback_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <utility>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Standard stream output callback.
     *
     * Pointer to a function that will be called when a process writes to its
     * standard output or error stream. Used to allow tools to capture output
     * from processes they have created.
     *
     * @ingroup Implementation
     */
    typedef std::pair<void (*)(const char*, const int&, void*),
		      void*> OutputCallback;
    
} }



#endif
