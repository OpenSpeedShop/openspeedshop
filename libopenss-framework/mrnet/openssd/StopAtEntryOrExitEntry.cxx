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

#include "Assert.hxx"
#include "DyninstCallbacks.hxx"
#include "StopAtEntryOrExitEntry.hxx"

#include <BPatch_function.h>

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
    dm_at_entry(at_entry),
    dm_handle(NULL)
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
    // Return immediately if the instrumentation is already installed
    if(dm_is_installed)
        return;

    // Get the Dyninst process pointer for the thread to be instrumented
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);

    // Find the "where" function
    BPatch_function* where =
        DyninstCallbacks::findFunction(*process, dm_where);

    if(where != NULL) {

	//
	// Create instrumentation snippet for the code sequence:
	//
	//     if(threadIndexExpr() == dm_thread.getLWP())
	//         Breakpoint;
	//

	BPatch_ifExpr expression(
            BPatch_boolExpr(BPatch_eq,
                            BPatch_threadIndexExpr(),
                            BPatch_constExpr(dm_thread.getLWP())),
	    BPatch_breakPointExpr()
            );
	
        // Find the entry/exit points of the "where" function
        BPatch_Vector<BPatch_point*>* points =
            where->findPoint(dm_at_entry ? BPatch_locEntry : BPatch_locExit);
        Assert(points != NULL);
        
        // Request the instrumentation be inserted
        dm_handle = process->insertSnippet(expression, *points);
        Assert(dm_handle != NULL);
	
    }
    
    // Instrumentation is now installed
    dm_is_installed = true;
}



/**
 * Remove instrumentation.
 *
 * Implement removal of instrumentation from the thread.
 */
void StopAtEntryOrExitEntry::remove()
{
    // Return immediately if the instrumentation is already removed
    if(!dm_is_installed)
        return;

    // Get the Dyninst process pointer for the thread to be instrumented
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);

    // Was the instrumentation actually inserted?
    if(dm_handle != NULL) {
    
        // Request the instrumentation be removed
        process->deleteSnippet(dm_handle);
    
    }

    // Instrumentation is no longer installed
    dm_is_installed = false;
}
