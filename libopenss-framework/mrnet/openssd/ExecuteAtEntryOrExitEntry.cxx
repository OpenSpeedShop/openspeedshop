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
 * Definition of the ExecuteAtEntryOrExitEntry class.
 *
 */

#include "ExecuteAtEntryOrExitEntry.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from a thread and fields.
 *
 * Constructs a new ExecuteAtEntryOrExitEntry for executing a library function
 * at another function's entry or exit in the specified thread.
 *
 * @param thread      Thread in which to install instrumentation.
 * @param where       Name of function at whose entry/exit the library
 *                    function should be executed.
 * @param at_entry    Boolean "true" if instrumenting function's entry point,
 *                    or "false" if function's exit point.
 * @param callee      Name of the library function to be executed.
 * @param argument    Blob argument to the function.
 */
ExecuteAtEntryOrExitEntry::ExecuteAtEntryOrExitEntry(BPatch_thread& thread,
						     const std::string& where,
						     const bool& at_entry,
						     const std::string& callee,
						     const Blob& argument) :
    InstrumentationEntry(thread),
    dm_where(where),
    dm_at_entry(at_entry),
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
InstrumentationEntry*
ExecuteAtEntryOrExitEntry::copy(BPatch_thread& thread) const
{
    return new ExecuteAtEntryOrExitEntry(thread, dm_where, dm_at_entry,
					 dm_callee, dm_argument);
}



/**
 * Install instrumentation.
 *
 * Implement installation of instrumentation into the thread.
 */
void ExecuteAtEntryOrExitEntry::install()
{
    // TODO: implement!
}



/**
 * Remove instrumentation.
 *
 * Implement removal of instrumentation from the thread.
 */
void ExecuteAtEntryOrExitEntry::remove()
{
    // TODO: implement!
}
