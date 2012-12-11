////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
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
 * Declaration of the PthreadCollector TraceableFunctions
 *
 */

#ifndef _PthreadTraceableFunctions_
#define _PthreadTraceableFunctions_
 
    static const char* TraceableFunctions[] = {

        "pthread_create",
        "pthread_mutex_init",
        "pthread_mutex_destroy",
        "pthread_mutex_lock",
        "pthread_mutex_trylock",
        "pthread_mutex_unlock",
        "pthread_cond_init",
        "pthread_cond_destroy",
        "pthread_cond_signal",
        "pthread_cond_broadcast",
        "pthread_cond_wait",
        "pthread_cond_timedwait",

	/* End Of Table Entry */
	NULL
    };

#if defined(CBTF_SERVICE_USE_OFFLINE)
        static const char * traceable = \
	"pthread_create,pthread_mutex_init,pthread_mutex_destroy,pthread_mutex_lock,pthread_mutex_trylock,pthread_mutex_unlock,pthread_cond_init,pthread_cond_destroy,pthread_cond_signal,pthread_cond_broadcast,pthread_cond_wait,pthread_cond_timedwait";

#endif

#endif
