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
#include "Backend.hxx"
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
 * Dynamic library callback.
 *
 * Callback function called by Dyninst when a dynamic library has been loaded
 * into a process. Sends a message to the frontend describing the linked object
 * that was loaded into, or unloaded from, the address space of the specified
 * process. Also sends a message containing the symbol table of that linked
 * object if it has been loaded (rather than unloded) and hasn't yet been sent
 * to the frontend.
 *
 * @param thread     Thread in the process which loaded or unloaded a
 *                   dynamic library.
 * @param module     Dynamic library that was loaded or unloaded.
 * @param is_load    Boolean "true" if the dynamic library was loaded, or
 *                   "false" if it was unloaded.
 */
void DyninstCallbacks::dynLibrary(BPatch_thread* thread,
				  BPatch_module* module,
				  bool is_load)
{
    // Check preconditions
    Assert(thread != NULL);
    Assert(module != NULL);

    // Get the Dyninst process pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);
    
    // Get the list of threads in this process
    BPatch_Vector<BPatch_thread*> threads_in_process;
    process->getThreads(threads_in_process);
    Assert(!threads_in_process.empty());

    // Names for these threads
    ThreadNameGroup threads;

    // Iterate over each thread in this process
    for(int i = 0; i < threads_in_process.size(); ++i) {
	Assert(threads_in_process[i] != NULL);

	// Add all the names of this thread
	ThreadNameGroup names = 
	    ThreadTable::TheTable.getNames(threads_in_process[i]);
	threads.insert(names.begin(), names.end());

    }
    
    // Get the current time
    Time now = Time::Now();
    
    // Get the file name of this module
    FileName linked_object(*module);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] DyninstCallbacks::"
	       << "dynLibrary(): PID " << process->getPid() << " "
	       << (is_load ? "loaded" : "unloaded") << " \""
	       << linked_object.getPath() << "\"." << std::endl;
        std::cerr << output.str();
    }
#endif
    
    // Was this module loaded into the thread?
    if(is_load) {
	
	// Get the address range of this module
	Address begin(reinterpret_cast<uint64_t>(module->getBaseAddr()));
	Address end = begin + module->getSize();
	AddressRange range(begin, end);
	
	// Send the frontend the "loaded" message for this linked object
	Senders::loadedLinkedObject(threads, now, range,
				    linked_object, false);

	// Are there any experiments for which this linked object is unsent?
	ExperimentGroup unsent =
	    SentFilesTable::TheTable.getUnsent(linked_object,
					       ExperimentGroup(threads));
	if(!unsent.empty()) {
	    
	    // Send the frontend the symbols for this linked object
	    Senders::symbolTable(unsent, linked_object, SymbolTable(*module));
	    
	    // These symbols are now sent for those experiments
	    SentFilesTable::TheTable.addSent(linked_object, unsent);
	    
	}
	
    }

    // Otherwise this module was unloaded from the thread
    else {

	// Send the frontend the "unloaded" message for this linked object
	Senders::unloadedLinkedObject(threads, now, linked_object);
	
    }
}



/**
 * Error callback.
 *
 * Callback function called by Dyninst when an error occurs. Sends a message
 * to the frontend indicating the error that occured.
 *
 * @param severity      Severity of the error.
 * @param number        Error number that occured.
 * @param parameters    Parameters to that error number's descriptive string.
 */
void DyninstCallbacks::error(BPatchErrorLevel severity, int number,
			     const char* const* parameters)
{
    std::string text;

    // Attach the error's severity to the error text
    switch(severity) {
    case BPatchFatal: text = "BPatchFatal"; break;
    case BPatchSerious: text = "BPatchSerious"; break;
    case BPatchWarning: text = "BPatchWarning"; break;
    case BPatchInfo: text = "BPatchInfo"; break;
    default: text = "?"; break;
    }
    text += ": ";

    // Attach the formatted error string to the error text
    char buffer[16384];
    BPatch::formatErrorString(buffer, sizeof(buffer),
			      BPatch::getEnglishErrorString(number),
			      parameters);
    text += buffer;

    // Only send serious and fatal errors to the frontend
    if((severity == BPatchFatal) || (severity == BPatchSerious)) {

	// Send the frontend a report of the error
	Senders::reportError(text);

    }

    // Display the error to the stdout stream
    std::cout << text << std::endl;
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
 * Exit callback.
 *
 * Callback function called by Dyninst when a process has exited. Sends a
 * message to the frontend indicating that all the threads in this process
 * have terminated.
 *
 * @param thread       Thread in the process that has terminated.
 * @param exit_type    Description of how the process exited.
 */
void DyninstCallbacks::exit(BPatch_thread* thread, BPatch_exitType exit_type)
{
    // Check preconditions
    Assert(thread != NULL);

    // Get the Dyninst process pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] DyninstCallbacks::"
	       << "exit(): PID " << process->getPid() << " exited with \"";
	switch(exit_type) {
	case NoExit:
	    output << "NoExit"; 
	    break;
	case ExitedNormally: 
	    output << "ExitedNormally"; 
	    break;
	case ExitedViaSignal: 
	    output << "ExitedViaSignal"; 
	    break;
	default: 
	    output << "?"; 
	    break;
	}
	output << "\"." << std::endl;
        std::cerr << output.str();
    }
#endif
    
    // Get the list of threads in this process
    BPatch_Vector<BPatch_thread*> threads_in_process;
    process->getThreads(threads_in_process);
    Assert(!threads_in_process.empty());

    // Names for these threads
    ThreadNameGroup threads;

    // Iterate over each thread in this process
    for(int i = 0; i < threads_in_process.size(); ++i) {
	Assert(threads_in_process[i] != NULL);
	
	// Add all the names of this thread
	ThreadNameGroup names = 
	    ThreadTable::TheTable.getNames(threads_in_process[i]);
	threads.insert(names.begin(), names.end());
	
    } 

    // Send the frontend the list of threads that have terminated
    Senders::threadsStateChanged(threads, Terminated);
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
 * Find a function.
 *
 * Finds the named function in the specified process and returns the Dyninst
 * function pointer for that function to the caller. A null pointer is returned
 * if the function cannot be found.
 *
 * @note    This function isn't a real Dyninst callback function but rather a
 *          utility function that is used in several other places. This seemed
 *          as good a place as any to put it.
 *
 * @param process    Process in which to find the function.
 * @param name       Name of the function to be found.
 * @return           Dyninst function pointer for the named function, or
 *                   a null pointer if the function could not be found.
 */
BPatch_function*
DyninstCallbacks::findFunction(/* const */ BPatch_process& process,
			       const std::string& name)
{
    // Get the image pointer for this process
    BPatch_image* image = process.getImage();
    Assert(image != NULL);

    // Attempt to find the requested function
    BPatch_Vector<BPatch_function*> functions;
    if(image->findFunction(name.c_str(), functions, false, true, true) == NULL)
	functions.clear();
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] DyninstCallbacks::"
	       << "findFunction(PID " << process.getPid() << ", \"" << name 
	       << "\") found " << functions.size() << " match"
	       << ((functions.size() == 1) ? "" : "es") << "." << std::endl;
        std::cerr << output.str();
    }
#endif
    
    // Return the first matching function (if any were found) to the caller
    return functions.empty() ? NULL : functions[0];
}



/**
 * Find a library function.
 *
 * Finds the named library function in the specified process. The library
 * containing this function is loaded into the process first if necessary.
 * The Dyninst function pointer is returned to the caller. A null pointer
 * is returned if the function cannot be found.
 *
 * @note    This function isn't a real Dyninst callback function but rather a
 *          utility function that is used in several other places. This seemed
 *          as good a place as any to put it.
 *
 * @param process    Process in which to find the library function.
 * @param name       Name of the library function to be found.
 * @return           Dyninst function pointer for the named library function,
 *                   or a null pointer if it could not be found.
 */
BPatch_function*
DyninstCallbacks::findLibraryFunction(/* const */ BPatch_process& process,
				      const std::string& name)
{
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] DyninstCallbacks::"
	       << "findLibraryFunction(PID " << process.getPid() 
	       << ", \"" << name << "\")" << std::endl;
        std::cerr << output.str();
    }
#endif

    // Parse the library function name into separate library and function names
    std::pair<std::string, std::string> parsed = parseLibraryFunctionName(name);

    // Go no further if parsing failed
    if(parsed.first.empty() || parsed.second.empty())
	return NULL;
    
    // Get the image pointer for this process
    BPatch_image* image = process.getImage();
    Assert(image != NULL);

    // Attempt to find the library containing the requested function
    BPatch_module* module = image->findModule(parsed.first.c_str(), true);

    // Does this library need to be loaded?
    if(module == NULL) {

	// Search for the library and find its full, normalized, path
	Path library = searchForLibrary(parsed.first);
	if(!library.isFile())
	    library = searchForLibrary(parsed.first + ".so");
	
	// Request that Dyninst load this library into the process
	if(!process.loadLibrary(library.c_str())) {
	    
#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] DyninstCallbacks::"
		       << "findLibraryFunction() failed to load \""
		       << library << "\"." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	    return NULL;
	}

	// Attempt to find (again) the library containing the requested function
	module = image->findModule(parsed.first.c_str(), true);
	if(module == NULL) {

#ifndef NDEBUG
	    if(Backend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] DyninstCallbacks::"
		       << "findLibraryFunction() failed to find \""
		       << library << "\" after loading." << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	    return NULL;
	}
	
    }
    
    // Attempt to find the requested function
    BPatch_Vector<BPatch_function*> functions;
    if(module->findFunction(parsed.second.c_str(), functions,
			    false, true, true, false) == NULL)
	functions.clear();
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] DyninstCallbacks::"
	       << "findLibraryFunction() found " << functions.size() 
	       << " match" << ((functions.size() == 1) ? "" : "es") << "." 
	       << std::endl;
        std::cerr << output.str();
    }
#endif
    
    // Return the first matching function (if any were found) to the caller
    return functions.empty() ? NULL : functions[0];
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

    // Get the Dyninst thread pointer for these threads
    BPatch_thread* thread = NULL;
    for(ThreadNameGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	BPatch_thread* this_thread = ThreadTable::TheTable.getPtr(*i);
	Assert(this_thread != NULL);
	Assert((thread == NULL) || (thread == this_thread));
	thread = this_thread;
    }
    Assert(thread != NULL);

    // Get the Dyninst process and image pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);
    BPatch_image* image = process->getImage();
    Assert(image != NULL);

    // Get the list of modules in this thread
    BPatch_Vector<BPatch_module*>* modules = image->getModules();
    Assert(modules != NULL);

    // Get the file name of the executable
    FileName executable_linked_object(*image);

    // Are there any experiments for which the executable is unsent?
    ExperimentGroup executable_unsent = 
	SentFilesTable::TheTable.getUnsent(executable_linked_object,
					   ExperimentGroup(threads));

    // Start with an empty address range and symbol table for the executable
    AddressRange executable_range;
    SymbolTable executable_symbol_table;

    // Iterate over each module in this thread
    for(int i = 0; i < modules->size(); ++i) {
	Assert((*modules)[i] != NULL);

	// Get the address range of this module
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

	    // Get the file name of this module
	    FileName linked_object(*(*modules)[i]);

	    // Send the frontend the initial "loaded" for this linked object
	    Senders::loadedLinkedObject(threads, now, range,
					linked_object, false);

	    // Are there any experiments for which this linked object is unsent?
	    ExperimentGroup unsent =
		SentFilesTable::TheTable.getUnsent(linked_object,
						   ExperimentGroup(threads));
	    if(!unsent.empty()) {

		// Send the frontend the symbols for this linked object
		Senders::symbolTable(unsent, linked_object,
				     SymbolTable(*(*modules)[i]));

		// These symbols are now sent for those experiments
		SentFilesTable::TheTable.addSent(linked_object, unsent);
		
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
	SentFilesTable::TheTable.addSent(executable_linked_object,
					 executable_unsent);
	
    }
}
