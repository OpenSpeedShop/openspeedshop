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
 * Definition of the ExecuteInPlaceOfEntry class.
 *
 */

#include "ExecuteInPlaceOfEntry.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from a thread and fields.
 *
 * Constructs a new ExecuteInPlaceOfEntry for executing a library function in
 * place of another function in the specified thread.
 *
 * @param thread    Thread in which to install instrumentation.
 * @param where     Name of the function to be replaced with the library
 *                  function.
 * @param callee    Name of the library function to be executed.
 */
ExecuteInPlaceOfEntry::ExecuteInPlaceOfEntry(BPatch_thread& thread,
					     const std::string& where,
					     const std::string& callee) :
    InstrumentationEntry(thread),
    dm_where(where),
    dm_callee(callee)
{
}



/**
 * Copy instrumentation.
 *
 * Implement copying instrumentation to another thread.
 *
 * @param thread    Thread for the new instrumentation entry.
 */
InstrumentationEntry* ExecuteInPlaceOfEntry::copy(BPatch_thread& thread) const
{
    return new ExecuteInPlaceOfEntry(thread, dm_where, dm_callee);
}



/**
 * Install instrumentation.
 *
 * Implement installation of instrumentation into the thread.
 */
void ExecuteInPlaceOfEntry::install()
{
    // TODO: implement!
}



/**
 * Remove instrumentation.
 *
 * Implement removal of instrumentation from the thread.
 */
void ExecuteInPlaceOfEntry::remove()
{
    // TODO: implement!
}
