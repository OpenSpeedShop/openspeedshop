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
 * Declaration and definition of the GuardWithDPCL class.
 *
 */

#ifndef _OpenSpeedShop_Framework_GuardWithDPCL_
#define _OpenSpeedShop_Framework_GuardWithDPCL_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Lockable.hxx"
#include "MainLoop.hxx"
#include "NonCopyable.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Guard access to a lockable object and DPCL.
     *
     * This class is identical to the Guard class with one exception. Before
     * locking the specified object the DPCL main event loop is suspended and
     * it is resumed again after unlocking the object.
     *
     * @note    Callbacks in the Process class implictly obtain the "lock" for
     *          the DPCL main event loop first and then lock individual process
     *          objects. There is no simple way to change this. Thus in order
     *          to assure deadlock free operation, members of the Process class 
     *          should always suspend the DPCL main event loop (if they intend 
     *          to) before locking the Process object on which they operate.
     *          This class helps insure that happens.
     *
     * @ingroup Utility
     */
    class GuardWithDPCL :
	public NonCopyable
    {

    public:

	/** Constructor from a lockable object. */
	explicit GuardWithDPCL(const Lockable& object) :
	    dm_object(&object)
	{
	    Assert(dm_object != NULL);
	    MainLoop::suspend();
	    dm_object->acquireLock();
	}

	/** Constructor from a lockable object pointer. */
	explicit GuardWithDPCL(const Lockable* object) :
	    dm_object(object)
	{
	    Assert(dm_object != NULL);
	    MainLoop::suspend();
	    dm_object->acquireLock();
	}

	/** Destructor. */
	~GuardWithDPCL()
	{	    
	    Assert(dm_object != NULL);
	    dm_object->releaseLock();
	    MainLoop::resume();
	}
	
    private:

	/** Pointer to the lockable object. */
	const Lockable* dm_object;

    };

} }



#endif
