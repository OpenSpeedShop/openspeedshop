////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the ExecuteNowEntry class.
 *
 */

#include "ExecuteNowEntry.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from a thread and fields.
 *
 * Constructs a new ExecuteNowEntry for executing a library function in the
 * specified thread.
 *
 * @param thread              Thread in which to install instrumentation.
 * @param disable_save_fpr    Boolean "true" if the floating-point registers
 *                            should NOT be saved before executing the library
 *                            function, or "false" if they should be saved.
 * @param callee              Name of the library function to be executed.
 * @param argument            Blob argument to the function.
 */
ExecuteNowEntry::ExecuteNowEntry(BPatch_thread& thread,
				 const bool& disable_save_fpr,
				 const std::string& callee,
				 const Blob& argument) :
    InstrumentationEntry(thread),
    dm_disable_save_fpr(disable_save_fpr),
    dm_callee(callee),
    dm_argument(argument)
{
}



/**
 * Copy instrumentation.
 *
 * Implement copying instrumentation to another thread.
 *
 * @param thread    Thread for the new instrumentation entry.
 */
InstrumentationEntry* ExecuteNowEntry::copy(BPatch_thread& thread) const
{
    return new ExecuteNowEntry(thread, dm_disable_save_fpr,
			       dm_callee, dm_argument);
}



/**
 * Install instrumentation.
 *
 * Implement installation of instrumentation into the thread.
 */
void ExecuteNowEntry::install()
{
    // TODO: implement!
}



/**
 * Remove instrumentation.
 *
 * Implement removal of instrumentation from the thread.
 */
void ExecuteNowEntry::remove()
{
    // TODO: implement!
}
