////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the DataBucket template.
 *
 */

#ifndef _OpenSpeedShop_Framework_DataBucket_
#define _OpenSpeedShop_Framework_DataBucket_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "NonCopyable.hxx"

#include <pthread.h>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Data bucket.
     *
     * Template for a synchronized data bucket. The value type is specified by
     * the template instantiation. Member functions are defined for setting and
     * getting the value in the bucket. These operations are synchronized using
     * a condition variable such that getting the value from an empty bucket
     * blocks the executing thread until such time that a value has been placed
     * into the bucket by another thread.
     *
     * @ingroup Implementation
     */
    template <typename T>
    class DataBucket :
	public NonCopyable
    {

    public:

	/** Default constructor. */
	DataBucket() :
	    dm_value(),
	    dm_has_value(false)
	{
	    Assert(pthread_mutex_init(&dm_mutex_lock, NULL) == 0);
	    Assert(pthread_cond_init(&dm_cv, NULL) == 0);
	}

	/** Set the value in this bucket. */
	void setValue(const T& value)
	{
	    Assert(pthread_mutex_lock(&dm_mutex_lock) == 0);
	    while(dm_has_value == true)
		Assert(pthread_cond_wait(&dm_cv, &dm_mutex_lock) == 0);
	    dm_value = value;
	    dm_has_value = true;
	    Assert(pthread_cond_signal(&dm_cv) == 0);
	    Assert(pthread_mutex_unlock(&dm_mutex_lock) == 0);
	}
	
	/** Get the value from this bucket. */
	T getValue()
	{
	    Assert(pthread_mutex_lock(&dm_mutex_lock) == 0);
	    while(dm_has_value == false)
		Assert(pthread_cond_wait(&dm_cv, &dm_mutex_lock) == 0);
	    T value = dm_value;
	    dm_has_value = false;
	    Assert(pthread_cond_signal(&dm_cv) == 0);
	    Assert(pthread_mutex_unlock(&dm_mutex_lock) == 0);
	    return value;
	}

    private:
	
	/** Bucket's contents. */
	T dm_value;
	
	/** Boolean flag indicating if the bucket has a valid value. */
	bool dm_has_value;
	
	/** Mutual exclusion lock for this bucket. */
	pthread_mutex_t dm_mutex_lock;
	
	/** Condition variable for this bucket. */
	pthread_cond_t dm_cv;

    };

} }



#endif
