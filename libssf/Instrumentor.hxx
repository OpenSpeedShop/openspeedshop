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
 * Declaration of the Instrumentor namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Instrumentor_
#define _OpenSpeedShop_Framework_Instrumentor_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Thread.hxx"

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Instrumentor.
     *
     * Namespace containing the thread instrumentor abstraction layer. Provides
     * the means for creating, attaching to, and otherwise manipulating and
     * instrumenting threads in the operating system. Isolating these functions
     * into a separete unit allows for easier substitution of alternate
     * instrumentation mechanisms in the future.
     *
     * @ingroup Implementation
     */
    namespace Instrumentor
    {
	
	/**
	 * Create underlying thread.
	 *
	 * Creates the specified thread as a new process to execute the passed
	 * command. The command is created with the same initial environment
	 * (standard file descriptors, environment variables, etc.) as when the
	 * tool was started. The process is created in a suspended state. When
	 * a thread is created, it is automatically attached.
	 *
	 * @note    An exception of type std::runtime_error is thrown if the
	 *          thread cannot be created for any reason (host doesn't exist,
	 *          specified command cannot be executed, etc.)
	 *
	 * @param thread     Thread to be created.
	 * @param command    Command to be executed.
	 */
	void createUnderlyingThread(const Thread& thread,
				    const std::string& command);
	
	/**
	 * Attach to underlying thread.
	 *
	 * Attaches the specified thread to its underlying thread in the
	 * operating system. Once a thread is attached, the instrumentor can
	 * perform operations (such as changing its state) on this thread.
	 *
	 * @note    An exception of type std::runtime_error is thrown if the
	 *          thread cannot be attached for any reason (host or process
	 *          identifier doesn't exist, etc.)
	 *
	 * @param thread    Thread to be attached.
	 */
	void attachUnderlyingThread(const Thread&);
	
	/**
	 * Detach from underlying thread.
	 *
	 * Detaches the specified thread from its underlying thread in the
	 * operating system. Once a thread is detached, the instrumentor can
	 * no longer perform operations (such as changing its state) on this
	 * thread without first being attached again. The underlying thread
	 * in the operating system is <em>not</em> destroyed. If the thread
	 * was in the suspended state, it is put into the running state before
	 * being detached.
	 *
	 * @param thread    Thread to be detached.
	 */
	void detachUnderlyingThread(const Thread&);
	
	/**
	 * Get a thread's state.
	 *
	 * Returns the current state of the specified thread. Since this state
	 * changes asynchronously, and may be updated across a network, there is
	 * a lag between when the actual thread's state changes and when it is
	 * reflected here.
	 *
	 * @todo    Currently DPCL provides the ability to get the state of an
	 *          entire process only - not that of a single thread. For now,
	 *          in the DPCL implementation, <em>all</em> threads in a given
	 *          process will have the same state.
	 *
	 * @param thread    Thread whose state should be obtained.
	 * @return          Current state of the thread.
	 */
	Thread::State getThreadState(const Thread& thread);	

	/**
	 * Change our state.
	 *
	 * Changes the current state of the specified thread to the passed
	 * value. Used to, for example, suspend a thread that was previously
	 * running. This function does not wait until the thread has actually
	 * completed the state change, and calling getThreadState() immediately
	 * following changeThreadState() will not reflect the new state until
	 * the change has actually completed.
	 *
	 * @note    Some transitions are disallowed because they do not make
	 *          sense or cannot be implemented. For example, a terminated
	 *          thread cannot be set to a running thread. An exception of
	 *          type std::logic_error is thrown when such an invalid
	 *          transition is requested.
	 *
	 * @note    Only one in-progress state change is allowed per thread at 
	 *          any given time. For example, if you request that a thread be
	 *          suspended, you cannot request that it be terminated before
	 *          the suspension is completed. An exception of type
	 *          std::logic_error is thrown when multiple in-progress
	 *          changes are requested.
	 *
	 * @todo    Currently DPCL provides the ability to change the state of
	 *          an entire process only - not that of a single thread. For
	 *          now, in the DPCL implementation, if a thread's state is
	 *          changed, <em>all</em> threads in the process containing
	 *          that thread will have their state changed.
	 *
	 * @param thread    Thread whose state should be changed.
	 * @param state     Change the theread to this state.
	 */
	void changeThreadState(const Thread&, const Thread::State&);

    }
    
} }



#endif
