////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the ThreadSpy class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ThreadSpy_
#define _OpenSpeedShop_Framework_ThreadSpy_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Thread.hxx"



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Thread spy.
     *
     * Class providing access to a Thread object's private data members. It is
     * desirable that the users of the Tool API not have access to these data
     * members, thus the reason they are private in the first place. However
     * the instrumentor needs access to them so that it can update the database
     * containing the thread. Normally this would be a simple matter of making
     * the instrumentor a friend of the Thread class. But instrumentors are
     * implemented as static functions rather than member functions of a class.
     * Listing a large number of these as friends becomes tedious and error
     * prone. Instead the instrumentor can use this class to "spy" into the
     * private data memebers of a Thread object.
     *
     * @ingroup Implementation
     */
    class ThreadSpy
    {
	
    public:

	/** Constructor from a Thread. */
	ThreadSpy(const Thread& thread) :
	    dm_thread(thread)
	{
	}

	/** Get the database containing this thread. */
	SmartPtr<Database> getDatabase() const
	{
	    return dm_thread.dm_database;
	}

	/** Get the entry (id) for the thread. */
	int getEntry() const
	{
	    return dm_thread.dm_entry;
	}
		
    private:

	/** Thread being spied. */
	const Thread& dm_thread;
	
    };
    
} }



#endif
