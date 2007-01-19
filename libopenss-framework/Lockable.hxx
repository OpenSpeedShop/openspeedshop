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
 * Declaration and definition of the Lockable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Lockable_
#define _OpenSpeedShop_Framework_Lockable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "NonCopyable.hxx"

#include <pthread.h>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Lockable object.
     *
     * An abstract base class that makes its derived classes non-copyable and
     * lockable. It does this via inheritance from NonCopyable and by defining
     * a mutual exclusion lock. Member functions for acquiring and releasing
     * that lock are provided. The class is made abstract by giving it a
     * protected default constructor and destructor.
     *
     * @sa    http://zthread.sourceforge.net/html/classZThread_1_1Lockable.html
     *
     * @ingroup Utility
     */
    class Lockable :
	public NonCopyable
    {

    public:

	/** Acquire exclusive access to the object. */
	void acquireLock() const
	{
	    Assert(pthread_mutex_lock(&dm_mutex_lock) == 0);
	}

	/** Release exclusive access to the object. */
	void releaseLock() const
	{
	    Assert(pthread_mutex_unlock(&dm_mutex_lock) == 0);
	}

    protected:
	
	/** Default constructor. */
	Lockable()
	{
	    pthread_mutexattr_t attr;
	    Assert(pthread_mutexattr_init(&attr) == 0);
	    Assert(pthread_mutexattr_settype(&attr,
					     PTHREAD_MUTEX_RECURSIVE_NP) == 0);
	    Assert(pthread_mutex_init(&dm_mutex_lock, &attr) == 0);
	    Assert(pthread_mutexattr_destroy(&attr) == 0);
	}
	
	/** Destructor. */
	virtual ~Lockable()
	{	    
	}

    private:
	
	/** Mutual exclusion lock. */
	mutable pthread_mutex_t dm_mutex_lock;
	
    };
    
} }



#endif
