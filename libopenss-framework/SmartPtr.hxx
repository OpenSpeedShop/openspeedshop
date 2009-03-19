////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2009 William Hachfeld. All Rights Reserved.
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
 * Declaration and definition of the SmartPtr template.
 *
 */

#ifndef _OpenSpeedShop_Framework_SmartPtr_
#define _OpenSpeedShop_Framework_SmartPtr_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "TotallyOrdered.hxx"

#include <pthread.h>



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Smart pointer.
     *
     * Template for a smart pointer based on thread-safe reference counting. The
     * type of the pointed to object is specified by the template instantiation.
     * When the last smart pointer to a given object is destroyed, the object
     * itself is also destroyed.
     *     
     * @sa    http://ootips.org/yonat/4dev/smart-pointers.html
     * @sa    http://www.awprofessional.com/articles/article.asp?p=31529
     *
     * @ingroup Utility
     */
    template <typename T>
    class SmartPtr :
	public TotallyOrdered<SmartPtr< T > >
    {

    public:

	/** Default constructor. */
	SmartPtr() :
	    dm_object(NULL),
	    dm_bookkeeping(new Bookkeeping())
	{
	}
	
	/** Constructor from an object pointer. */
	explicit SmartPtr(T* object) :
	    dm_object(object),
	    dm_bookkeeping(new Bookkeeping())
	{
	}

	/** Copy constructor. */
	SmartPtr(const SmartPtr& other) :
	    dm_object(other.dm_object),
	    dm_bookkeeping(other.dm_bookkeeping)
	{
	    // Increment the reference count of this object
	    Assert(dm_bookkeeping != NULL);
	    Assert(pthread_mutex_lock(&(dm_bookkeeping->dm_lock)) == 0);
	    dm_bookkeeping->dm_references++;
	    Assert(pthread_mutex_unlock(&(dm_bookkeeping->dm_lock)) == 0);
	}

	/** Destructor. */
	~SmartPtr()
	{
	    // Decrement the reference count of our object
	    Assert(dm_bookkeeping != NULL);
	    Assert(pthread_mutex_lock(&(dm_bookkeeping->dm_lock)) == 0);
	    unsigned references = --dm_bookkeeping->dm_references;
	    Assert(pthread_mutex_unlock(&(dm_bookkeeping->dm_lock)) == 0);

	    // Destroy our object if we were the last reference
	    if(references == 0) {
		delete dm_object;
		delete dm_bookkeeping;
	    }
	}
	
	/** Operator "=" defined for a SmartPtr object. */
        SmartPtr& operator=(const SmartPtr& other)
	{
	    // Only do an assignment if the object pointers differ
	    if(dm_object != other.dm_object) {
		
		// Decrement the reference count of our object
		Assert(dm_bookkeeping != NULL);
		Assert(pthread_mutex_lock(&(dm_bookkeeping->dm_lock)) == 0);
		unsigned references = --dm_bookkeeping->dm_references;
		Assert(pthread_mutex_unlock(&(dm_bookkeeping->dm_lock)) == 0);
		
		// Destroy our object if we were the last reference
		if(references == 0) {
		    delete dm_object;
		    delete dm_bookkeeping;
		}
		
		// Replace our object with the new object
		dm_object = other.dm_object;
		dm_bookkeeping = other.dm_bookkeeping;
		
		// Increment the reference count of the new object
		Assert(dm_bookkeeping != NULL);
		Assert(pthread_mutex_lock(&(dm_bookkeeping->dm_lock)) == 0);
		dm_bookkeeping->dm_references++;
		Assert(pthread_mutex_unlock(&(dm_bookkeeping->dm_lock)) == 0);
	    }

	    return *this;
	}

	/** Operator "<" defined for two SmartPtr objects. */
	bool operator<(const SmartPtr& other) const
	{
	    return dm_object < other.dm_object;
	}

	/** Operator "->" defined for a SmartPtr object. */
	T* operator->() const
	{
	    Assert(dm_object != NULL);
	    return dm_object;
	}

	/** Operator "*" defined for a SmartPtr object. */
	T& operator*() const
	{
	    Assert(dm_object != NULL);
	    return *dm_object;
	}	    

	/** Test if the pointed-to object is null. */
	bool isNull() const
	{
	    return dm_object == NULL;
	}

    private:

	/** Internal bookkeeping structure. */
	struct Bookkeeping
	{
	    pthread_mutex_t dm_lock;  /**< Mutual exclusion lock. */
	    unsigned dm_references;   /**< Reference count. */
	    
	    /** Default constructor. */
	    Bookkeeping() :
		dm_lock(),
		dm_references(1)
	    {
		Assert(pthread_mutex_init(&dm_lock, NULL) == 0);
	    }

	    /** Destructor. */
	    ~Bookkeeping()
	    {
		Assert(pthread_mutex_destroy(&dm_lock) == 0);
	    }
	    
	};

	/** Pointer to the object. */
	T* dm_object;
	
	/** Pointer to the object's bookkeeping structure. */
	mutable Bookkeeping* dm_bookkeeping;
	
    };
    
} }



#endif
