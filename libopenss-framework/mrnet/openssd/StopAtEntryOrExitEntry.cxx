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
 * Definition of the StopAtEntryOrExitEntry class.
 *
 */

#include "StopAtEntryOrExitEntry.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from a thread and fields.
 *
 * Constructs a new StopAtEntryOrExitEntry for a breakpoint at a function's
 * entry or exit in the specified thread.
 *
 * @param thread      Thread in which to install instrumentation.
 * @param where       Name of the functino at whose entry/exit the stop
 *                    should occur.
 * @param at_entry    Boolean "true" if instrumenting function's entry
 *                    point, or "false" if function's exit point.
 */
StopAtEntryOrExitEntry::StopAtEntryOrExitEntry(BPatch_thread& thread,
					       const std::string& where,
					       const bool& at_entry) :
    InstrumentationEntry(thread),
    dm_where(where),
    dm_at_entry(at_entry)
{
}



/**
 * Copy instrumentation.
 *
 * Implement copying instrumentation to another thread.
 *
 * @param thread    Thread for the new instrumentation entry.
 */
InstrumentationEntry* StopAtEntryOrExitEntry::copy(BPatch_thread& thread) const
{
    return new StopAtEntryOrExitEntry(thread, dm_where, dm_at_entry);
}



/**
 * Install instrumentation.
 *
 * Implement installation of instrumentation into the thread.
 */
void StopAtEntryOrExitEntry::install()
{
    // TODO: implement!
}



/**
 * Remove instrumentation.
 *
 * Implement removal of instrumentation from the thread.
 */
void StopAtEntryOrExitEntry::remove()
{
    // TODO: implement!
}
