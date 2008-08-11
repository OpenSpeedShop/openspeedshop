////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 Krell Institute. All Rights Reserved.
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
 * Declaration of the Watcher namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Watch_
#define _OpenSpeedShop_Framework_Watch_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <map>
#include "Blob.hxx"

namespace OpenSpeedShop { namespace Watcher {

    void Watcher();
    void startWatching();
    void stopWatching();
    void* fileIOmonitorThread(void*);

    /** Identifier of the monitor thread. */
    pthread_t fileIOmonitor_tid;

#ifndef NDEBUG
    bool isDebugEnabled();
#endif

} }

#endif
