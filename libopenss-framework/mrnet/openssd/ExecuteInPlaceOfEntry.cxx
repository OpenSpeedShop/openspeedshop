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
 * Definition of the ExecuteInPlaceOfEntry class.
 *
 */

#include "Assert.hxx"
#include "Backend.hxx"
#include "Dyninst.hxx"
#include "ExecuteInPlaceOfEntry.hxx"
#include "ThreadName.hxx"
#include "Utility.hxx"

#include <BPatch_function.h>
#include <iostream>
#include <sstream>

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
    dm_callee(callee),
    dm_flag(NULL),
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
    // Return immediately if the instrumentation is already installed
    if(dm_is_installed)
        return;

    // Return immediately if the thread is terminated
    if(dm_thread.isTerminated()) {
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteInPlaceOfEntry::"
		   << "install(): Cannot instrument terminated thread {"
		   << toString(ThreadName(-1, dm_thread)) << " }." << std::endl;
	    std::cerr << output.str();
 	}
#endif

	return;
    }

    // Get the Dyninst process pointer for the thread to be instrumented
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);

    // Find the "where" and "callee" functions
    BPatch_function* where = Dyninst::findFunction(*process, dm_where);
    BPatch_function* callee = Dyninst::findLibraryFunction(*process, dm_callee);
    if((where != NULL) && (callee != NULL)) {

	// Request allocation of a variable in this process for storing a flag
	BPatch_image* image = process->getImage();
	Assert(image != NULL);
	BPatch_type* type = image->findType("unsigned int");
	Assert(type != NULL);
	dm_flag = process->malloc(*type);
        
	//
	// Create instrumentation snippet for the code sequence:
	//
	//     if(tidExpr() == dm_thread.getTid()) {
	//         if(dm_flag == 0) {
	//             dm_flag = 1;
	//             jump dm_wrapper();
	//         } else
	//             dm_flag = 0;
	//     }
	//

	BPatch_arithExpr statement1(BPatch_assign,
				    *dm_flag,
				    BPatch_constExpr((unsigned int)1));
	BPatch_funcJumpExpr statement2(*where);

	BPatch_Vector<BPatch_snippet*> sequence;
	sequence.push_back(&statement1);
	sequence.push_back(&statement2);

	BPatch_ifExpr body(
	    BPatch_boolExpr(BPatch_eq,
			    *dm_flag,
			    BPatch_constExpr((unsigned int)0)),
	    BPatch_sequence(sequence),
	    BPatch_arithExpr(BPatch_assign,
			     *dm_flag,
			     BPatch_constExpr((unsigned int)0))
	    );

        BPatch_ifExpr expression(
	    BPatch_boolExpr(BPatch_eq,
			    BPatch_tidExpr(process),
			    BPatch_constExpr(dm_thread.getTid())),
	    body
            );

        // Find the entry points of the "where" function
        BPatch_Vector<BPatch_point*>* points = 
	    where->findPoint(BPatch_locEntry);
        Assert(points != NULL);

	// Instruct Dyninst to allow recursive calls into instrumentation
        BPatch* bpatch = BPatch::getBPatch();
        Assert(bpatch != NULL);
	bool saved_tramp_recursive = bpatch->isTrampRecursive();
	bpatch->setTrampRecursive(true);
        
        // Request the instrumentation be inserted
	if(dm_thread.getTid() == 0)
	    dm_handle = process->insertSnippet(body, *points);
	else
	    dm_handle = process->insertSnippet(expression, *points);
        Assert(dm_handle != NULL);
	
	// Restore the allowance of recursive calls to its previous state
        bpatch->setTrampRecursive(saved_tramp_recursive);
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteInPlaceOfEntry::"
		   << "install(): Execute " << dm_callee << "() in place of "
		   << dm_where << "() in thread { "
		   << toString(ThreadName(-1, dm_thread)) << " }." << std::endl;
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
void ExecuteInPlaceOfEntry::remove()
{
    // Return immediately if the instrumentation is already removed
    if(!dm_is_installed)
        return;

    // Return immediately if the thread is terminated
    if(dm_thread.isTerminated()) {
	
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteInPlaceOfEntry::"
		   << "remove(): Cannot uninstrument terminated thread {"
		   << toString(ThreadName(-1, dm_thread)) << " }." << std::endl;
	    std::cerr << output.str();
 	}
#endif

	return;
    }
    
    // Get the Dyninst process pointer for the thread to be instrumented
    BPatch_process* process = dm_thread.getProcess();
    Assert(process != NULL);
    
    // Was the instrumentation actually inserted?
    if((dm_flag != NULL) && (dm_handle != NULL)) {
	
        // Request the instrumentation be removed
        process->deleteSnippet(dm_handle);

	// Free the memory containing the flag
	process->free(*dm_flag);

#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] ExecuteInPlaceOfEntry::"
		   << "remove(): Execute " << dm_callee << "() in place of "
		   << dm_where << "() in thread { "
		   << toString(ThreadName(-1, dm_thread)) << " }." << std::endl;
	    std::cerr << output.str();
	}
#endif
		
    }
    
    // Instrumentation is no longer installed
    dm_is_installed = false;
}
