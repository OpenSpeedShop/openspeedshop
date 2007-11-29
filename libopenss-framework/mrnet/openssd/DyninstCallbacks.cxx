////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the DyninstCallbacks namespace.
 *
 */

#include "AddressRange.hxx"
#include "Assert.hxx"
#include "DyninstCallbacks.hxx"
#include "ExperimentGroup.hxx"
#include "FileName.hxx"
#include "Senders.hxx"
#include "SentFilesTable.hxx"
#include "SymbolTable.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * ...
 *
 * ...
 *
 * @param thread     ...
 * @param module     ...
 * @param is_load    ...
 */
void DyninstCallbacks::dynLibrary(BPatch_thread* thread,
				  BPatch_module* module,
				  bool is_load)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param severity      ...
 * @param number        ...
 * @param parameters    ...
 */
void DyninstCallbacks::error(BPatchErrorLevel severity, int number,
			     const char* const* parameters)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param thread    ...
 */
void DyninstCallbacks::exec(BPatch_thread* thread)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param thread       ...
 * @param exit_type    ...
 */
void DyninstCallbacks::exit(BPatch_thread* thread, BPatch_exitType exit_type)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param parent    ...
 * @param child     ...
 */
void DyninstCallbacks::postFork(BPatch_thread* parent, BPatch_thread* child)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param process    ...
 * @param thread     ...
 */
void DyninstCallbacks::threadCreate(BPatch_process* process,
				    BPatch_thread* thread)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param process    ...
 * @param thread     ...
 */
void DyninstCallbacks::threadDestroy(BPatch_process* process,
				     BPatch_thread* thread)
{
    // TODO: implement!
}



/**
 * Send symbols for a thread.
 *
 * Sends a series of messages to the frontend describing the initial set of
 * linked objects loaded into the address space of the specified thread. Also
 * sends messages containing the symbol tables of those linked objects which
 * have not already been sent to the frontend.
 *
 * @note    This function isn't a real Dyninst callback function but rather a
 *          utility function that is used by several of the Dyninst callbacks.
 *          It is also used, however, by several of the message callbacks, so
 *          this seemed as good a place as any to put this.
 *
 * @param threads    Threads for which symbols should be sent.
 */
void DyninstCallbacks::sendSymbolsForThread(const ThreadNameGroup& threads)
{
    // Get the current time
    Time now = Time::Now();

    // Obtain the Dyninst thread pointer for these threads
    BPatch_thread* thread = NULL;
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	BPatch_thread* this_thread = ThreadTable::TheTable.getPtr(*i);
	Assert(this_thread != NULL);
	Assert((thread == NULL) || (thread == this_thread));
	thread = this_thread;
    }
    Assert(thread != NULL);
    
    // Obtain the Dyninst process and image pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);
    BPatch_image* image = process->getImage();
    Assert(image != NULL);

    // Obtain the list of modules in this thread
    BPatch_Vector<BPatch_module*>* modules = image->getModules();
    Assert(modules != NULL);

    // Obtain the file name of the executable
    FileName executable_linked_object(*image);

    // Are there any experiments for which the executable is unsent?
    ExperimentGroup executable_unsent = 
	SentFilesTable::TheTable.getUnsent(ExperimentGroup(threads),
					   executable_linked_object);

    // Start with an empty address range and symbol table for the executable
    AddressRange executable_range;
    SymbolTable executable_symbol_table;
    
    // Iterate over each module in this thread
    for(int i = 0; i < modules->size(); ++i) {
	Assert((*modules)[i] != NULL);

	// Obtain the address range of this module
	Address begin(reinterpret_cast<uint64_t>((*modules)[i]->getBaseAddr()));
	Address end = begin + (*modules)[i]->getSize();
	AddressRange range(begin, end);

	// Is this module part of the executable?
	if(!(*modules)[i]->isSharedLib()) {

	    // Add this module's address range to that of the executable
	    executable_range |= range;
	    
	    // Are there any experiments for which the executable is unsent?
	    if(!executable_unsent.empty()) {

		// Add this module's symbols to that of the executable
		executable_symbol_table.addModule(*(*modules)[i]);
	    
	    }

	}

	// Otherwise this module is a shared library
	else {

	    // Obtain the file name of this module
	    FileName linked_object(*(*modules)[i]);
	    
	    // Send the frontend the initial "loaded" for this linked object
	    Senders::loadedLinkedObject(threads, now, range,
					linked_object, false);

	    // Are there any experiments for which this linked object is unsent?
	    ExperimentGroup unsent =
		SentFilesTable::TheTable.getUnsent(ExperimentGroup(threads),
						   linked_object);
	    if(!unsent.empty()) {
		
		// Send the frontend the symbols for this linked object
		Senders::symbolTable(unsent, linked_object,
				     SymbolTable(*(*modules)[i]));
		
		// These symbols are now sent for those experiments
		SentFilesTable::TheTable.addSent(unsent, linked_object);
		
	    }
	    
	}
	
    }
    
    // Send the frontend the initial "loaded" for the executable
    Senders::loadedLinkedObject(threads, now, executable_range, 
				executable_linked_object, true);
    
    // Are there any experiments for which the executable is unsent?
    if(!executable_unsent.empty()) {
	
	// Send the frontend the symbols for the executable
	Senders::symbolTable(executable_unsent,
			     executable_linked_object,
			     executable_symbol_table);
	
	// The executable is now sent for those experiments
	SentFilesTable::TheTable.addSent(executable_unsent,
					 executable_linked_object);
	
    }
}
