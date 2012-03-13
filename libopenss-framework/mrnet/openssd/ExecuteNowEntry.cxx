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
 * Definition of the ExecuteNowEntry class.
 *
 */

#include "Assert.hxx"
#include "Backend.hxx"
#include "Dyninst.hxx"
#include "ExecuteNowEntry.hxx"
#include "ThreadName.hxx"
#include "Utility.hxx"

#include <iostream>
#include <sstream>

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
    // Return immediately if the instrumentation is already installed
    if(dm_is_installed)
	return;

    // Return immediately if the thread is terminated
#if (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);
    if( process->isTerminated()) {
#else
    if(dm_thread.isTerminated()) {
#endif
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteNowEntry::"
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

    // Find the "callee" function
    BPatch_function* callee = Dyninst::findLibraryFunction(*process, dm_callee);
    if(callee != NULL) {

	//
	// Create instrumentation snippet for the code sequence:
	//
	//     dm_callee(StringEncode(dm_argument))
	//
	
	BPatch_constExpr argument(dm_argument.getStringEncoding().c_str());
	BPatch_Vector<BPatch_snippet*> arguments;
	arguments.push_back(&argument);

	BPatch_funcCallExpr expression(*callee, arguments);
	
	// Instruct Dyninst to either save or not save FPRs as appropriate
	BPatch* bpatch = BPatch::getBPatch();
	Assert(bpatch != NULL);
	bool saved_fpr = bpatch->isSaveFPROn();
	bpatch->setSaveFPR(!dm_disable_save_fpr);

	// Request the instrumentation be executed
	dm_thread.oneTimeCode(expression);
	
	// Restore the saving of FPRs to its previous state
	bpatch->setSaveFPR(saved_fpr);

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteNowEntry::"
		   << "install(): Execute " << dm_callee << "(\"" 
		   << dm_argument.getStringEncoding() << "\") in thread "
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
void ExecuteNowEntry::remove()
{
    // Instrumentation is no longer installed
    dm_is_installed = false;
}
