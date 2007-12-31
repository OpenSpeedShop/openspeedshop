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

#include "Assert.hxx"
#include "DyninstCallbacks.hxx"
#include "ExecuteAtEntryOrExitEntry.hxx"

#include <BPatch_function.h>

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
    dm_argument(argument),
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
    // Return immediately if the instrumentation is already installed
    if(dm_is_installed)
	return;

    // Get the Dyninst process pointer for the thread to be instrumented
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);

    // Find the "where" and "callee" functions
    BPatch_function* where =
	DyninstCallbacks::findFunction(*process, dm_where);
    BPatch_function* callee =
	DyninstCallbacks::findLibraryFunction(*process, dm_callee);

    if((where != NULL) && (callee != NULL)) {
	
	//
	// Create instrumentation snippet for the code sequence:
	//
	//     if(threadIndexExpr() == dm_thread.getLWP())
	//         dm_callee(StringEncode(dm_argument)
	//
	
	BPatch_constExpr argument(dm_argument.getStringEncoding().c_str());
	BPatch_Vector<BPatch_snippet*> arguments;
	arguments.push_back(&argument);

	BPatch_ifExpr expression(
	    BPatch_boolExpr(BPatch_eq,
			    BPatch_threadIndexExpr(),
			    BPatch_constExpr(dm_thread.getLWP())),
	    BPatch_funcCallExpr(*callee, arguments)
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
void ExecuteAtEntryOrExitEntry::remove()
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
