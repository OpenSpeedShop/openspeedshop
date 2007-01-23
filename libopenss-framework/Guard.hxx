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
 * Declaration and definition of the Guard class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Guard_
#define _OpenSpeedShop_Framework_Guard_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Lockable.hxx"
#include "NonCopyable.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Guard access to lockable object.
     *
     * Class that guards access to a lockable object by providing exclusive
     * access to that object within a given scope. The object to be guarded
     * is specified at object construction. Exclusive access is immediately
     * acquired when the guard is created and is released when the guard is
     * destroyed. Creating guard objects on the stack within a particular
     * scope simplifies locking by automatically releasing exclusive access
     * when that scope is exited.
     *
     * @sa    http://www.cs.wustl.edu/~schmidt/PDF/ScopedLocking.pdf
     *
     * @ingroup Utility
     */
    class Guard :
	public NonCopyable
    {

    public:

	/** Constructor from a lockable object. */
	explicit Guard(const Lockable& object) :
	    dm_object(&object)
	{
	    Assert(dm_object != NULL);
	    dm_object->acquireLock();
	}

	/** Constructor from a lockable object pointer. */
	explicit Guard(const Lockable* object) :
	    dm_object(object)
	{
	    Assert(dm_object != NULL);
	    dm_object->acquireLock();
	}

	/** Destructor. */
	virtual ~Guard()
	{	    
	    Assert(dm_object != NULL);
	    dm_object->releaseLock();
	}
	
    private:

	/** Pointer to the lockable object. */
	const Lockable* dm_object;

    };

} }



#endif
