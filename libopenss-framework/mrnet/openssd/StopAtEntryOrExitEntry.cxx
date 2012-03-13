////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
#include "Backend.hxx"
#include "Dyninst.hxx"
#include "StopAtEntryOrExitEntry.hxx"
#include "ThreadName.hxx"
#include "Utility.hxx"

#include <BPatch_function.h>
#include <iostream>
#include <sstream>

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

    // Return immediately if the thread is terminated
#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);
    if(process->isTerminated()) {
#else
    if(dm_thread.isTerminated()) {
#endif
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] StopAtEntryOrExitEntry::"
		   << "install(): Cannot instrument terminated thread "
		   << toString(ThreadName(-1, dm_thread)) << "." << std::endl;
	    std::cerr << output.str();
 	}
#endif

	return;
    }

    // Get the Dyninst process pointer for the thread to be instrumented
#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    process = dm_thread.getProcess();
#else
    BPatch_process* process = dm_thread.getProcess();
#endif
    Assert(process != NULL);

    // Find the "where" function
    BPatch_function* where = Dyninst::findFunction(*process, dm_where);
    if(where != NULL) {

	//
	// Create the instrumentation snippet for the code sequence:
	//
	//     if(tidExpr() == dm_thread.getTid())
	//         Breakpoint;
	//

	BPatch_breakPointExpr body;

	BPatch_ifExpr expression(
	    BPatch_boolExpr(BPatch_eq,
			    BPatch_threadIndexExpr(),
			    BPatch_constExpr(dm_thread.getBPatchID())),
	    body
            );
	
        // Find the entry/exit points of the "where" function
        BPatch_Vector<BPatch_point*>* points =
            where->findPoint(dm_at_entry ? BPatch_locEntry : BPatch_locExit);
        Assert(points != NULL);
        
        // Request the instrumentation be inserted

	BPatch_addressSpace* procAddrSpace = (BPatch_addressSpace *) process;

        // Note: The following extra conditional term checking dm_where is a
        //       hack. It causes stops for MPIR_Breakpoint to be process-wide,
        //       which is necessary on LLNL's SLURM based MPICH implementation.
        //       In that implementation, the call to MPIR_Breakpoint does not
        //       occur in the main thread.
	if(process->isMultithreadCapable() && (dm_where != "MPIR_Breakpoint"))
	    dm_handle = procAddrSpace->insertSnippet(expression, *points);
	else
	    dm_handle = procAddrSpace->insertSnippet(body, *points);
        Assert(dm_handle != NULL);
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] StopAtEntryOrExitEntry::"
		   << "install(): Stop at "<< (dm_at_entry ? "entry" : "exit") 
		   << " of "<< dm_where << "() in thread "
		   << toString(ThreadName(-1, dm_thread)) << "." << std::endl;
	    std::cerr << output.str();
	}
#endif
	
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

    // Return immediately if the thread is terminated
#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);
    if(process->isTerminated()) {
#else
    if(dm_thread.isTerminated()) {
#endif
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] StopAtEntryOrExitEntry::"
		   << "remove(): Cannot uninstrument terminated thread "
		   << toString(ThreadName(-1, dm_thread)) << "." << std::endl;
	    std::cerr << output.str();
 	}
#endif

	return;
    }

    // Get the Dyninst process pointer for the thread to be instrumented
#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    process = dm_thread.getProcess();
#else
    BPatch_process* process = dm_thread.getProcess();
#endif
    Assert(process != NULL);

    // Was the instrumentation actually inserted?
    if(dm_handle != NULL) {
    
        // Request the instrumentation be removed
        process->deleteSnippet(dm_handle);

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] StopAtEntryOrExitEntry::"
		   << "remove(): Stop at "<< (dm_at_entry ? "entry" : "exit") 
		   << " of "<< dm_where << "() in thread "
		   << toString(ThreadName(-1, dm_thread)) << "." << std::endl;
	    std::cerr << output.str();
	}
#endif
    
    }

    // Instrumentation is no longer installed
    dm_is_installed = false;
}
