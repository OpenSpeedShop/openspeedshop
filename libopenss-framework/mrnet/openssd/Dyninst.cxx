////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the Dyninst namespace.
 *
 */

#include "AddressRange.hxx"
#include "Assert.hxx"
#include "Backend.hxx"
#include "Dyninst.hxx"
#include "ExperimentGroup.hxx"
#include "FileName.hxx"
#include "Senders.hxx"
#include "SentFilesTable.hxx"
#include "SymbolTable.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



namespace {



    /**
     * Get experiments for a process.
     *
     * Returns the group of experiments which contain the specified process.
     * An empty set is returned if no experiment are found.
     *
     * @param process    Process whose experiments are to be found.
     * @return           Group of experiments containing this process.
     */
    ExperimentGroup getExperiments(/* const */ BPatch_process& process)
    {
	ExperimentGroup experiments;

	// Get the list of threads in this process
	BPatch_Vector<BPatch_thread*> threads;
	process.getThreads(threads);
	Assert(!threads.empty());

	// Iterate over each thread in this process
        for(int i = 0; i < threads.size(); ++i) {
            Assert(threads[i] != NULL);
	    
	    // Add all of this thread's experiments
	    ThreadNameGroup names = ThreadTable::TheTable.getNames(threads[i]);
	    for(ThreadNameGroup::const_iterator
		    j = names.begin(); j != names.end(); ++j)
		experiments.insert(Experiment(*j));
	    
	}

	// Return the experiment group to the caller
	return experiments;
    }



}



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
void OpenSpeedShop::Framework::Dyninst::dynLibrary(BPatch_thread* thread,
						   BPatch_module* module,
						   bool is_load)
{
    // Check preconditions
    Assert(thread != NULL);
    Assert(module != NULL);

    // Get the current time
    Time now = Time::Now();
    
    // Get the Dyninst process pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

    // Get the file name of this module
    FileName linked_object(*module);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "dynLibrary(): PID " << process->getPid() << " "
	       << (is_load ? "loaded" : "unloaded") << " \""
	       << linked_object.getPath() << "\"." << std::endl;
        std::cerr << output.str();
    }
#endif

    // Get the names for this process
    ThreadNameGroup threads = ThreadTable::TheTable.getNames(process);
        
    // Was this module loaded into the thread?
    if(is_load) {
	
	// Get the address range of this module
	Address begin(reinterpret_cast<uintptr_t>(module->getBaseAddr()));
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
void OpenSpeedShop::Framework::Dyninst::error(BPatchErrorLevel severity, 
					      int number,
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
 * Exec callback.
 *
 * Callback function called by Dyninst when a process has performed an exec()
 * call. Sends the frontend the new symbol information for the process.
 *
 * @param thread    Thread that has performed the exec() call.
 */
void OpenSpeedShop::Framework::Dyninst::exec(BPatch_thread* thread)
{
    // Check preconditions
    Assert(thread != NULL);

    // Get the Dyninst process pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "exec(): PID " << process->getPid() 
	       << " has called exec()." << std::endl;
        std::cerr << output.str();
    }
#endif

    // TODO: Do we need to "close out" the old set of threads for this
    //       process? Or will Dyninst report threadDestroy() events for
    //       those threads before exec()?

    // Get the names for this process
    ThreadNameGroup threads = ThreadTable::TheTable.getNames(process);
    
    // Send the frontend all the symbol information for these threads
    Dyninst::sendSymbolsForThread(threads);

    // TODO: copy instrumentation from the process back into itself
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
void OpenSpeedShop::Framework::Dyninst::exit(BPatch_thread* thread,
					     BPatch_exitType exit_type)
{
    // Check preconditions
    Assert(thread != NULL);

    // Get the Dyninst process pointer for this thread
    BPatch_process* process = thread->getProcess();
    Assert(process != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
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

    // Get the names for this process
    ThreadNameGroup threads = ThreadTable::TheTable.getNames(process);
    
    // Send the frontend the list of threads that have terminated
    Senders::threadsStateChanged(threads, Terminated);
}



/**
 * Fork callback.
 *
 * Callback function called by Dyninst when a process has forked. Sends the
 * frontend a list of threads that were forked and the symbol information for
 * those threads.
 *
 * @param parent    Parent process that has forked.
 * @param child     Child process that was forked.
 */
void OpenSpeedShop::Framework::Dyninst::postFork(BPatch_thread* parent,
						 BPatch_thread* child)
{
    // Check preconditions
    Assert(parent != NULL);
    Assert(child != NULL);

    // Get the Dyninst process pointers for these threads
    BPatch_process* parent_process = parent->getProcess();
    Assert(parent_process != NULL);
    BPatch_process* child_process = child->getProcess();
    Assert(child_process != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "postFork(): PID " << parent_process->getPid()
	       << " forked PID " << child_process->getPid() 
	       << "." << std::endl;
        std::cerr << output.str();
    }
#endif

    // Get the experiments containing the parent process
    ExperimentGroup experiments = getExperiments(*parent_process);

    // Get the list of threads in the child process
    BPatch_Vector<BPatch_thread*> child_threads;
    child_process->getThreads(child_threads);
    Assert(!child_threads.empty());

    // Iterate over each thread in the child process
    ThreadNameGroup threads_forked;
    for(int i = 0; i < child_threads.size(); ++i) {
	Assert(child_threads[i] != NULL);

	// Iterate over the experiments containing the parent process
	for(ExperimentGroup::const_iterator
		j = experiments.begin(); j != experiments.end(); ++j) {
	    
	    // Add this thread to the thread table and group of forked threads
	    ThreadName name(j->getExperiment(), *(child_threads[i]));
	    ThreadTable::TheTable.addThread(name, child_threads[i]);
	    threads_forked.insert(name);

	}
	
    }

    // Were any threads actually forked?
    if(!threads_forked.empty()) {
	
        // Send the frontend the list of threads that were forked
        Senders::attachedToThreads(threads_forked);

	// Send the frontend all the symbol information for these threads
	Dyninst::sendSymbolsForThread(threads_forked);
	
        // Send the frontend a message indicating these threads are running
        Senders::threadsStateChanged(threads_forked, Running);
    }
    
    // TODO: copy instrumentation from the parent process to the child process 
}



/**
 * Thread creation callback.
 *
 * Callback function called by Dyninst when a thread has been created. Sends
 * the frontend a list of threads that were created and the symbol information
 * for those threads.
 *
 * @param process    Process containing the thread that has been created.
 * @param thread     Thread that has been created.
 */
void OpenSpeedShop::Framework::Dyninst::threadCreate(BPatch_process* process,
						     BPatch_thread* thread)
{
    // Check preconditions
    Assert(process != NULL);
    Assert(thread != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "threadCreate(): TID " 
       	       << static_cast<size_t>(thread->getTid()) << " of PID "
	       << process->getPid() << " was created." << std::endl;
	std::cerr << output.str();
    }
#endif

    // Has this thread already been attached?
    if(!ThreadTable::TheTable.getNames(thread).empty()) {
#ifndef NDEBUG
	if(Backend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Dyninst::"
		   << "threadCreate(): TID " 
		   << static_cast<size_t>(thread->getTid()) << " of PID "
		   << process->getPid() << " is already attached." << std::endl;
	    std::cerr << output.str();
	}
#endif
	return;
    }
    
    // Get the experiments containing the process
    ExperimentGroup experiments = getExperiments(*process);
    
    // Iterate over the experiments containing the process
    ThreadNameGroup threads_created;
    for(ExperimentGroup::const_iterator
	    i = experiments.begin(); i != experiments.end(); ++i) {
	
	// Add this thread to the thread table and group of created threads
	ThreadName name(i->getExperiment(), *thread);
	ThreadTable::TheTable.addThread(name, thread);
	threads_created.insert(name);
	
    }
    
    // Were any threads actually created?
    if(!threads_created.empty()) {
	
	// Send the frontend the list of threads that were created
	Senders::attachedToThreads(threads_created);
	
	// Send the frontend all the symbol information for these threads
	Dyninst::sendSymbolsForThread(threads_created);
	
	// Send the frontend a message indicating these threads are running
	Senders::threadsStateChanged(threads_created, Running);
    }
    
    // TODO: copy instrumentation from the process to the new thread
}



/**
 * Thread destruction callback.
 *
 * Callback function called by Dyninst when a thread has been destroyed. Sends
 * a message to the frontend indicating that the thread has terminated.
 *
 * @param process    Process containing the thread that has been destroyed.
 * @param thread     Thread that has been destroyed
 */
void OpenSpeedShop::Framework::Dyninst::threadDestroy(BPatch_process* process,
						      BPatch_thread* thread)
{
    // Check preconditions
    Assert(process != NULL);
    Assert(thread != NULL);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "threadDestroy(): TID " 
	       << static_cast<size_t>(thread->getTid()) << " of PID "
	       << process->getPid() << " was destroyed." << std::endl;
	std::cerr << output.str();
    }
#endif

    // Get the names for this thread
    ThreadNameGroup threads = ThreadTable::TheTable.getNames(thread);
    
    // Send the frontend the list of threads that have terminated
    Senders::threadsStateChanged(threads, Terminated);
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
BPatch_function* OpenSpeedShop::Framework::Dyninst::findFunction(
    /* const */ BPatch_process& process,
    const std::string& name
    )
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
	output << "[TID " << pthread_self() << "] Dyninst::"
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
 * Find a global variable.
 *
 * Finds the named global variable in the specified process and returns the
 * Dyninst variable expression pointer for that global variable to teh caller.
 * A null pointer is returend if the global variable cannot be found.
 *
 * @note    This function isn't a real Dyninst callback function but rather a
 *          utility function that is used in several other places. This seemed
 *          as good a place as any to put it.
 *
 * @param process    Process in which to find the global variable.
 * @param name       Name of the global variable to be found.
 * @return           Dyninst variable expression pointer for the named
 *                   function, or a null pointer if the global variable
 *                   could not be found.
 */
BPatch_variableExpr* OpenSpeedShop::Framework::Dyninst::findGlobalVariable(
    /* const */ BPatch_process& process,
    const std::string& name
    )
{
    // Get the image pointer for this process
    BPatch_image* image = process.getImage();
    Assert(image != NULL);

    // Attempt to find the requested global variable
    BPatch_variableExpr* variable = image->findVariable(name.c_str());

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "findGlobalVariable(PID " << process.getPid() 
	       << ", \"" << name << "\") found "
	       << ((variable == NULL) ? "0 matches" : "1 match")
	       << "." << std::endl;
        std::cerr << output.str();
    }
#endif

    // Return the matching variable (if one was found) to the caller
    return variable;
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
BPatch_function* OpenSpeedShop::Framework::Dyninst::findLibraryFunction(
    /* const */ BPatch_process& process,
    const std::string& name
    )
{
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
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
		output << "[TID " << pthread_self() << "] Dyninst::"
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
		output << "[TID " << pthread_self() << "] Dyninst::"
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
	output << "[TID " << pthread_self() << "] Dyninst::"
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
 * Get an integer global variable's value.
 *
 * Gets the current value of a signed or unsigned integer global variable
 * within the specified process. The value is always returned as a signed
 * 64-bit integer and is promoted/cast to this size as necessary. If the
 * global variable wasn't found, the first value in the pair returned will
 * be "false".
 *
 * @param process    Process from which to get the global variable.
 * @param global     Name of the global variable whose value is being requested.
 * @param value      Pair containing the current value of that variable.
 */
void OpenSpeedShop::Framework::Dyninst::getGlobal(
    /* const */ BPatch_process& process,
    const std::string& global,
    std::pair<bool, int64_t>& value
    )
{
    // Initially assume the global variable will not be found
    value = std::make_pair(false, int64_t());

    // Find the global variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, global);
    if(variable == NULL)
	return;

    // Get the name and size of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    const char* type_name = type->getName();
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();
    
    // Is the variable a signed/unsigned integer type?
    
    if(!strcmp(type_name, "unsigned char") && (type_size == 1)) {
	unsigned char raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    else if(!strcmp(type_name, "char") && (type_size == 1)) {
	char raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    
    else if(!strcmp(type_name, "unsigned short") && (type_size == 2)) {
	unsigned short raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    else if(!strcmp(type_name, "short") && (type_size == 2)) {
	short raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    
    else if(!strcmp(type_name, "unsigned int") && (type_size == 4)) {
	unsigned int raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    else if(!strcmp(type_name, "int") && (type_size == 4)) {
	int raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    
    else if(!strcmp(type_name, "unsigned long long") && (type_size == 8)) {
	unsigned long long raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
    else if(!strcmp(type_name, "long long") && (type_size == 8)) {
	long long raw_value;
	variable->readValue(&raw_value);
	value = std::make_pair(true, static_cast<int64_t>(raw_value));
    }
}



/**
 * Set an integer global variable's value.
 *
 * Sets the current value of a signed or unsigned integer global variable
 * within the specified process. The new value is always specified as a
 * signed 64-bit integer and is demoted/cast from this size as necessary.
 *
 * @param process    Process in which the global variable should be set.
 * @param global     Name of the global variable whose value is being set.
 * @param value      New value of that variable.
 */
void OpenSpeedShop::Framework::Dyninst::setGlobal(
    /* const */ BPatch_process& process,
    const std::string& global,
    int64_t& value
    )
{
    // Find the global variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, global);
    if(variable == NULL)
	return;
	
    // Get the name and size of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    const char* type_name = type->getName();
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();
    
    // Is the variable a signed/unsigned integer type?
    
    if(!strcmp(type_name, "unsigned char") && (type_size == 1)) {
	unsigned char raw_value = static_cast<unsigned char>(value);
	variable->writeValue(&raw_value);
    }
    else if(!strcmp(type_name, "char") && (type_size == 1)) {
	char raw_value = static_cast<char>(value);
	variable->writeValue(&raw_value);
    }
    
    else if(!strcmp(type_name, "unsigned short") && (type_size == 2)) {
	unsigned short raw_value = 
	    static_cast<unsigned short>(value);
	variable->writeValue(&raw_value);
    }
    else if(!strcmp(type_name, "short") && (type_size == 2)) {
	short raw_value = static_cast<short>(value);
	variable->writeValue(&raw_value);
    }
    
    else if(!strcmp(type_name, "unsigned int") && (type_size == 4)) {
	unsigned int raw_value = static_cast<unsigned int>(value);
	variable->writeValue(&raw_value);
    }
    else if(!strcmp(type_name, "int") && (type_size == 4)) {
	int raw_value = static_cast<int>(value);
	variable->writeValue(&raw_value);
    }
    
    else if(!strcmp(type_name, "unsigned long long") && (type_size == 8)) {
	unsigned long long raw_value =
	    static_cast<unsigned long long>(value);
	variable->writeValue(&raw_value);
    }
    else if(!strcmp(type_name, "long long") && (type_size == 8)) {
	long long raw_value = static_cast<long long>(value);
	variable->writeValue(&raw_value);
    }
}



/**
 * Get a string global variable's value.
 *
 * Gets the current value of a character string gobal variable within the
 * specified process. The value is returned as a C++ standard string rather
 * than a C character array. This makes managing the memory associated with
 * the string more obvious. If the global variable wasn't found, the first
 * value in the pair returned will be "false".
 *
 * @param process    Process from which to get the global variable.
 * @param global     Name of the global variable whose value is being requested.
 * @param value      Pair containing the current value of that variable.
 */
void OpenSpeedShop::Framework::Dyninst::getGlobal(
    /* const */ BPatch_process& process,
    const std::string& global,
    std::pair<bool, std::string>& value
    )
{
    // Initially assume the global variable will not be found
    value = std::make_pair(false, std::string());

    // Get the image pointer for this process
    BPatch_image* image = process.getImage();
    Assert(image != NULL);

    // Find the global variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, global);
    if(variable == NULL)
	return;

    // Get the name and size of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    const char* type_name = type->getName();
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();
    
    // Is the variable a character pointer type?
    
    if(!strcmp(type_name, "char*")) {
	std::string raw_value;
	image->readString(variable, raw_value);	
	value = std::make_pair(true, raw_value);
    }    
}



/**
 * Get the MPICH process table.
 *
 * Gets the current value of the MPICH process table within the specified
 * process. The value is returned as a Job rather than directly as an array
 * of MPIR_PROCDESC structures. This makes managing the memory associated
 * with the table more obvious. If the table wasn't found, the first value
 * in the pair returned will be "false".
 *
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @param process    Process from which to get the table.
 * @param value      Pair containing the current value of that table.
 */
void OpenSpeedShop::Framework::Dyninst::getMPICHProcTable(
    /* const */ BPatch_process& process,
    std::pair<bool, Job>& value
    )
{
    // Initially assume the table will not be found
    value = std::make_pair(false, Job());

    // Find the value of "MPIR_proctable_size"
    std::pair<bool, int64_t> MPIR_proctable_size;
    getGlobal(process, "MPIR_proctable_size", MPIR_proctable_size);
    if(!MPIR_proctable_size.first)
	return;
    
    // Find the "MPIR_proctable" variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, "MPIR_proctable");
    if(variable == NULL)
	return;

    // Get the name and size of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;

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
 * @param threads    Threads for which symbols should be sent.
 */
void OpenSpeedShop::Framework::Dyninst::sendSymbolsForThread(
    const ThreadNameGroup& threads
    )
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
	Address begin(reinterpret_cast<uintptr_t>(
            (*modules)[i]->getBaseAddr()
	    ));
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



/**
 * Send thread state updates.
 *
 * Sends a series of messages to the frontend describing any thread state
 * changes which have occured but have not yet been reported. Accomplished
 * by comparing Dyninst's current notion of every known thread's state with
 * that thread's state as stored in the thread table.
 */
void OpenSpeedShop::Framework::Dyninst::sendThreadStateUpdates()
{
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "sendThreadStateUpdates()" << std::endl;
        std::cerr << output.str();
    }
#endif

    // Get Dyninst's list of active processes
    BPatch* bpatch = BPatch::getBPatch();
    Assert(bpatch != NULL);
    BPatch_Vector<BPatch_process*>* processes = bpatch->getProcesses();
    Assert(processes != NULL);

    // Create an empty table for tracking thread state updates to be sent

    typedef std::map<OpenSS_Protocol_ThreadState, ThreadNameGroup> StateUpdates;

    StateUpdates updates;

    updates.insert(std::make_pair(Disconnected, ThreadNameGroup()));
    updates.insert(std::make_pair(Running, ThreadNameGroup()));
    updates.insert(std::make_pair(Suspended, ThreadNameGroup()));
    updates.insert(std::make_pair(Terminated, ThreadNameGroup()));

    // Iterate over each active process
    for(int i = 0; i < processes->size(); ++i) {
	BPatch_process* process = (*processes)[i];
	Assert(process != NULL);
	
	// Determine the current state of this process
	OpenSS_Protocol_ThreadState dyninst_state = Running;
	if(process->isDetached())
	    dyninst_state = Disconnected;
	else if(process->isTerminated())
	    dyninst_state = Terminated;
	else if(process->isStopped())
	    dyninst_state = Suspended;
	
	// Get the list of threads in this process
	BPatch_Vector<BPatch_thread*> threads;
	process->getThreads(threads);
	Assert(!threads.empty());

	// Iterate over each thread in this process
	for(int j = 0; j < threads.size(); ++j) {
	    BPatch_thread* thread = threads[j];
	    Assert(thread != NULL);

	    // Is the thread's state in the thread table out of date?
	    OpenSS_Protocol_ThreadState table_state =
		ThreadTable::TheTable.getThreadState(thread);
	    if((table_state != Nonexistent) && (table_state != dyninst_state)) {

		// Get all the names of this thread
		ThreadNameGroup names = ThreadTable::TheTable.getNames(thread);

		// Add these threads to the table of state updates
		StateUpdates::iterator k = updates.find(dyninst_state);
		Assert(k != updates.end());
		k->second.insert(names.begin(), names.end());
		
	    }
	    
	}
	
    }

    // Iterate over all the updates that need to be sent
    for(StateUpdates::const_iterator
	    i = updates.begin(); i != updates.end(); ++i)
	if(!i->second.empty()) {
	    
	    // Send the frontend the list of threads that have changed state
	    Senders::threadsStateChanged(i->second, i->first);
    
	}
}
