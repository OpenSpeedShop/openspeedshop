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
#include "InstrumentationTable.hxx"
#include "Senders.hxx"
#include "SentFilesTable.hxx"
#include "SymbolTable.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Time.hxx"
#include "Watcher.hxx"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif

using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Watcher;



namespace {


    bool is_in_mpi_startup = false;

    /**
     * Symbol table map.
     *
     * Type defining a mapping from an address range to a symbol table for
     * that address range along with the experiments for which that symbol
     * table is currently unsent.
     */
    typedef std::map<AddressRange,
	             std::pair<SymbolTable, ExperimentGroup> > SymbolTableMap;



    /**
     * Implement getting the MPICH process table.
     *
     * Implements getting the current value of the specified MPICH process
     * table. The value is returned as a Job rather than directly as an array
     * of MPIR_PROCDESC structures. This makes managing the memory associated
     * with the table more obvious. If the table wasn't found, the first value
     * in the pair returned will be "false".
     *
     * @note    This function is templated on the address type in order to
     *          handle tables found in both 32-bit and 64-bit processes.
     *
     * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
     *
     * @param process                Process from which to get the table.
     * @param MPIR_proctable         Table whose value is being requested.
     * @param MPIR_proctable_size    Size of the table whose value is being
     *                               requested.
     * @param value                  Pair containing the current value of
     *                               that table.
     */
    template <typename T>
    void getMPICHProcTableImpl(
        /* const */ BPatch_process& process,
	/* const */ BPatch_variableExpr& MPIR_proctable,
	const int64_t& MPIR_proctable_size,
	std::pair<bool, Job>& value
        )
    {
	// Define type for an entry in the table
	typedef struct {
	    T   host_name;        /* Something we can pass to inet_addr */
	    T   executable_name;  /* The name of the image */
	    int pid;              /* The pid of the process */
	} MPIR_PROCDESC;

	// Initially assume the table will not be read properly
	value = std::make_pair(false, Job());

	// Get the image pointer for this process
	BPatch_image* image = process.getImage();
	Assert(image != NULL);

	// Read the base address of the table
	T base_addr = 0;
	MPIR_proctable.readValue(&base_addr);


	// Iterate over each entry in the table
	for(int64_t i = 0; i < MPIR_proctable_size; ++i) {

	    MPIR_PROCDESC raw_value;

           // Construct a variable for this entry
           // Allow openssd to use either dyninst 5.2 or 6.0.
#if (DYNINST_MAJOR == 5) && (DYNINST_MINOR == 2)
           BPatch_variableExpr variable(
               &process,
               (void*)(base_addr + (i * sizeof(MPIR_PROCDESC))),
               sizeof(MPIR_PROCDESC)
               );
	    // Read the entry
	    variable.readValue(&raw_value);
#else
	    BPatch_type * mytype = BPatch::bpatch->createScalar("mpirPE",sizeof(MPIR_PROCDESC));
	    BPatch_variableExpr* variable =
			process.createVariable(base_addr + (i * sizeof(MPIR_PROCDESC)),mytype);
	    // Read the entry
	    variable->readValue(&raw_value);
#endif


	    // Read the host name for this entry
	    std::string host_name;
	    image->readString(raw_value.host_name, host_name);

	    // Add this entry to the job value
	    value.second.push_back(std::make_pair(host_name, raw_value.pid));

	}

	// Inform the caller that the table was successfully read
	value.first = true;
    }



}



/**
 * Dynamic library callback.
 *
 * Callback function called by Dyninst when a dynamic library has been loaded
 * into a process. Sends a message to the frontend describing the linked object
 * that was loaded into, or unloaded from, the address space of the specified
 * process. Also sends a message containing the symbol table of that linked
 * object if it has been loaded (rather than unloaded) and hasn't yet been sent
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
    // ignore dlopen and dlclose that occur while we are in mpi startup phase.
    // Since we are not actively collecting data at this point we do not want
    // these entries (and the time it takes to process then in dyninst).
    if (is_in_mpi_startup) {
	return;
    }

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

    // Ignore the callbacks for the Dyninst default module
    if(linked_object.getPath() == "DEFAULT_MODULE")
	return;
    
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

	    // Get the Dyninst image pointer for this thread
	    BPatch_image* image = process->getImage();
	    Assert(image != NULL);

	    // Get the list of modules in this thread
	    BPatch_Vector<BPatch_module*>* modules = image->getModules();
	    Assert(modules != NULL);

	    // Symbol table for this module
	    SymbolTable symbol_table(linked_object, range);

	    // Iterate over each module in this thread
	    for(int i = 0; i < modules->size(); ++i) {
		module = (*modules)[i];
		Assert(module != NULL);

		// Get the address range of this module
		begin = reinterpret_cast<uintptr_t>(module->getBaseAddr());
		end = begin + module->getSize();
		range = AddressRange(begin, end);
		
		// Add this module to the symbol table (if necessary)
		if(range == symbol_table.getRange())
		    symbol_table.addModule(*image, *module);
		
	    }
	    
	    // Send the frontend the symbols for this linked object
	    Senders::symbolTable(unsent, linked_object, symbol_table);
	    
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
    if (is_in_mpi_startup) {
	return;
    }

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

      // Copy instrumentation to the thread
      Dyninst::copyInstrumentation(threads, threads);
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

#define WDH_TEMPORARY_HACK 1

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
    if (is_in_mpi_startup) {
	return;
    }
    // Check preconditions
    Assert(parent != NULL);
    Assert(child != NULL);

#ifndef WDH_TEMPORARY_HACK
    //
    // TODO: Temporarily detach from the child process and ignore it. Dyninst
    //       is currently failing when OpenMPI calls fork() on Fedora 8 systems.
    //
    if(child != NULL) {
	BPatch_process* child_process = child->getProcess();
	child_process->detach(true);
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        BPatch_process* parent_process = parent->getProcess();
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "postFork(): PID " << parent_process->getPid()
	       << " forked PID " << child_process->getPid() 
	       << " DETACHING." << std::endl;
        std::cerr << output.str();
    }
#endif
	return;
    }
#endif  // WDH_TEMPORARY_HACK

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
    ExperimentGroup experiments = 
	ThreadTable::TheTable.getExperiments(*parent_process);

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
// FIXME: It would be nice to have a flag to decide if we really
// want to attach to these forked threads and get symbols and instrument.
// Mainly set this flag for cases like openmpi where the mpi startup
// work is may not of interest to the user.
	
	// During mpi startup we are not yet collecting data.
	// This prevents orte forked mpirun processes from polluting the database.
	// It also speeds up mpi startup with openmpi and any mpi launcher
	// that uses fork.
	if (!is_in_mpi_startup) {
            // Send the frontend the list of threads that were forked
            Senders::attachedToThreads(threads_forked);

	    // Send the frontend all the symbol information for these threads
	    Dyninst::sendSymbolsForThread(threads_forked);
	}
	
        // Send the frontend a message indicating these threads are running
        Senders::threadsStateChanged(threads_forked, Running);

	// FIXME: any attempt to copy instrumentation during openmpi startup
	// is causing the rank processes to crash. Possibly the forked mpirun
	// processes that start the actual rank process (e.g. nbody) are already
	// gone by this time.  Mybe we need to call getNames outside of the
	// copyInstrumentation call and then call it if we get any valid names.
	
	// During mpi startup we are not yet collecting data. So no instrumentation.
	// It also speeds up mpi startup with openmpi and any mpi launcher
	// that uses fork.
	if (!is_in_mpi_startup) {
	    // Copy instrumentation to the new process
	    Dyninst::copyInstrumentation(ThreadTable::TheTable.getNames(parent),
				     threads_forked);
	}

    }
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
    if (is_in_mpi_startup) {
        return;
    }

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
    ExperimentGroup experiments =
	ThreadTable::TheTable.getExperiments(*process);

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

	// Copy instrumentation to the new thread
	Dyninst::copyInstrumentation(ThreadTable::TheTable.getNames(process),
				     threads_created);

    }
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

    // Note: The following "watcher" call is necessary in order to insure that
    //       this thread's performance data makes its way into the experiment
    //       database. Otherwise it is possible for the frontend to receive the
    //       termination notification and, if this is the last thread, shutdown
    //       before the performance data can make its way to the frontend and
    //       into the database.

    // Insure the thread's performance data is sent to the frontend
    OpenSpeedShop::Watcher::watchProcess(threads);

    // Send the frontend the list of threads that have terminated
    Senders::threadsStateChanged(threads, Terminated);
}



/**
 * Copy instrumentation from one set of threads to another.
 *
 * Copies all instrumentation from the specified source threads to the
 * specified destination threads and sends a series of messages to the
 * frontend describing any instrumentation additions which have occured.
 *
 * @param source         Threads from which instrumentation should be copied.
 * @param destination    Threads to which instrumentation should be copied.
 */
void OpenSpeedShop::Framework::Dyninst::copyInstrumentation(
    const ThreadNameGroup& source,
    const ThreadNameGroup& destination
    )
{
    // Create an empty table for tracking instrumentation updates to be sent
    
    typedef std::map<Collector, ThreadNameGroup> InstrumentationUpdates;
    
    InstrumentationUpdates updates;
    
    // Iterate over every source thread
    for(ThreadNameGroup::const_iterator
	    i = source.begin(); i != source.end(); ++i)
	
	// Iterate over every destination thread
	for(ThreadNameGroup::const_iterator
		j = destination.begin(); j != destination.end(); ++j) {

	    // Instrument the destination thread based on the source thread
	    std::set<Collector> collectors = 
		InstrumentationTable::TheTable.copyInstrumentation(*i, *j);

	    // Iterate over the collectors that instrumented this thread
	    for(std::set<Collector>::const_iterator
		    k = collectors.begin(); k != collectors.end(); ++k) {
		
		// Add the pairing to the table of instrumentation updates
		InstrumentationUpdates::iterator l = 
		    updates.insert(std::make_pair(*k, ThreadNameGroup())).first;
		Assert(l != updates.end());
		l->second.insert(*j);
		
	    }
	    
	}
    
    // Iterate over all the updates that need to be sent
    for(InstrumentationUpdates::const_iterator
	    i = updates.begin(); i != updates.end(); ++i)
	if(!i->second.empty()) {
	    
	    // Send the frontend the list of threads that have been instrumented
	    Senders::instrumented(i->second, i->first);
	    
	}
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
    BPatch_variableExpr* variable = image->findVariable(name.c_str(),false);

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

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] ENTERED Dyninst::"
	       << "getGlobal(PID " << process.getPid() 
	       << ", \"" << global << std::endl;
	std::cerr << output.str();
    }
#endif

    // Find the global variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, global);
    if(variable == NULL)
	return;

    // Get the name and size of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    std::string type_name(type->getName() ? type->getName() : "");
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "getGlobal(PID " << process.getPid() 
	       << ", \"" << global << "\", <reference>): type_name = \"" 
	       << type_name << "\", type_size = " << type_size << std::endl;
	std::cerr << output.str();
    }
#endif

    // Can this variable be interpreted as an integer type?

    if(type_size == 1) {
	if((type_name.find("unsigned char") != std::string::npos) ||
	   (type_name.find("uint8_t") != std::string::npos)) {

	    uint8_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
	else if((type_name.find("char") != std::string::npos) ||
		(type_name.find("int8_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int8_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
    }

    else if(type_size == 2) {
	if((type_name.find("unsigned short") != std::string::npos) ||
	   (type_name.find("uint16_t") != std::string::npos)) {

	    uint16_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
	else if((type_name.find("short") != std::string::npos) ||
		(type_name.find("int16_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int16_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
    }

    else if(type_size == 4) {
	if((type_name.find("unsigned int") != std::string::npos) ||
	   (type_name.find("unsigned long") != std::string::npos) ||
	   (type_name.find("uint32_t") != std::string::npos)) {

	    uint32_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
	else if((type_name.find("int") != std::string::npos) ||
		(type_name.find("long") != std::string::npos) ||
		(type_name.find("int32_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int32_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
    }

    else if(type_size == 8) {
	if((type_name.find("unsigned long long") != std::string::npos) ||
	   (type_name.find("uint64_t") != std::string::npos)) {

	    uint64_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
	else if((type_name.find("long long") != std::string::npos) ||
		(type_name.find("int64_t") != std::string::npos) ||
		(type_name.find("ash_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int64_t raw_value = 0;
	    variable->readValue(&raw_value);
	    value = std::make_pair(true, static_cast<int64_t>(raw_value));

	}
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
    std::string type_name(type->getName() ? type->getName() : "");
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "setGlobal(PID " << process.getPid() 
	       << ", \"" << global << "\", <reference>): type_name = \"" 
	       << type_name << "\", type_size = " << type_size << std::endl;
	std::cerr << output.str();
    }
#endif

    // Can this variable be interpreted as an integer type?

    if(type_size == 1) {
	if((type_name.find("unsigned char") != std::string::npos) ||
	   (type_name.find("uint8_t") != std::string::npos)) {

	    uint8_t raw_value = static_cast<uint8_t>(value);
	    variable->writeValue(&raw_value);	    

	}
	else if((type_name.find("char") != std::string::npos) ||
		(type_name.find("int8_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int8_t raw_value = static_cast<int8_t>(value);
	    variable->writeValue(&raw_value);

	}
    }

    else if(type_size == 2) {
	if((type_name.find("unsigned short") != std::string::npos) ||
	   (type_name.find("uint16_t") != std::string::npos)) {

	    uint16_t raw_value = static_cast<uint16_t>(value);
	    variable->writeValue(&raw_value);	    

	}
	else if((type_name.find("short") != std::string::npos) ||
		(type_name.find("int16_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int16_t raw_value = static_cast<int16_t>(value);
	    variable->writeValue(&raw_value);

	}
    }

    else if(type_size == 4) {
	if((type_name.find("unsigned int") != std::string::npos) ||
	   (type_name.find("unsigned long") != std::string::npos) ||
	   (type_name.find("uint32_t") != std::string::npos)) {

	    uint32_t raw_value = static_cast<uint32_t>(value);
	    variable->writeValue(&raw_value);	    

	}
	else if((type_name.find("int") != std::string::npos) ||
		(type_name.find("long") != std::string::npos) ||
		(type_name.find("int32_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int32_t raw_value = static_cast<int32_t>(value);
	    variable->writeValue(&raw_value);

	}
    }

    else if(type_size == 8) {
	if((type_name.find("unsigned long long") != std::string::npos) ||
	   (type_name.find("uint64_t") != std::string::npos)) {

	    uint64_t raw_value = static_cast<uint64_t>(value);
	    variable->writeValue(&raw_value);	    

	}
	else if((type_name.find("long long") != std::string::npos) ||
		(type_name.find("int64_t") != std::string::npos) ||
		(type_name == "<no type>") || (type_name == "")) {

	    int64_t raw_value = static_cast<int64_t>(value);
	    variable->writeValue(&raw_value);

	}
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

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] ENTERED Dyninst::"
	       << "getGlobal(PID " << process.getPid() 
	       << ", \"" << global << std::endl;
	std::cerr << output.str();
    }
#endif

    // Get the image pointer for this process
    BPatch_image* image = process.getImage();
    Assert(image != NULL);

    // Find the global variable
    BPatch_variableExpr* variable = 
	Dyninst::findGlobalVariable(process, global);
    if(variable == NULL)
	return;

    // Get the name of the type of this variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    std::string type_name(type->getName() ? type->getName() : "");
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "getGlobal(PID " << process.getPid() 
	       << ", \"" << global << "\", <reference>): type_name = \""
	       << type_name << "\"" << std::endl;
	std::cerr << output.str();
    }
#endif

    // Can this variable be interpreted as a character pointer type?

    if(((type_size == 4) || (type_size == 8)) &&
       ((type_name.find("char*") != std::string::npos) ||
	(type_name == "<no type>") || (type_name == ""))) {

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

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] ENTERED Dyninst::"
	       << "getMPICHProcTable(PID " << process.getPid() 
	       << ", \"" << std::endl;
	std::cerr << output.str();
    }
#endif

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

    // Get the name and size of the type of the "MPIR_proctable" variable
    const BPatch_type* type = variable->getType();
    if(type == NULL)
	return;
    std::string type_name(type->getName() ? type->getName() : "");
    unsigned type_size = const_cast<BPatch_type*>(type)->getSize();

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "getMPICHProcTable(PID " << process.getPid() 
	       << ", <reference>): type_name = \"" << type_name
	       << "\", type_size = " << type_size << std::endl;
	std::cerr << output.str();
    }
#endif

    // Can this variable be interpreted as a MPIR_PROCDESC pointer type?

    if((type_size == 4) &&
       ((type_name == "MPIR_PROCDESC *") || 
	(type_name == "<no_type>") || (type_name == ""))) {

	getMPICHProcTableImpl<uint32_t>(process, *variable,
	 				MPIR_proctable_size.second, value);

    }

    else if((type_size == 8) &&
	    ((type_name == "MPIR_PROCDESC *") ||
	     (type_name == "<no_type>") || (type_name == ""))) {

	getMPICHProcTableImpl<uint64_t>(process, *variable,
	 				MPIR_proctable_size.second, value);

    }
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

    // File name of the executable and Dyninst's default module
    FileName executable(*image), default_module("DEFAULT_MODULE");
    
    // Symbol tables under construction
    SymbolTableMap symbol_tables;

    // Iterate over each module in this thread
    for(int i = 0; i < modules->size(); ++i) {
	BPatch_module* module = (*modules)[i];
	Assert(module != NULL);
	
	// Get the file name of this module
	FileName linked_object = 
	    module->isSharedLib() ? FileName(*module) : executable;
	
	// Get the address range of this module
	Address begin(reinterpret_cast<uintptr_t>(module->getBaseAddr()));
	Address end = begin + module->getSize();
	AddressRange range(begin, end);
	
	// Look for an existing symbol table for this module
	SymbolTableMap::iterator j = symbol_tables.find(range);

	// Create a new symbol table for this module if one doesn't exist
	if(j == symbol_tables.end()) {

	    j = symbol_tables.insert(std::make_pair(
	        range,
		std::make_pair(
		    SymbolTable(linked_object, range),
		    SentFilesTable::TheTable.getUnsent(
		        linked_object, ExperimentGroup(threads)
			)
		    )
		)).first;
   
	}
	
	//
	// Replace this symbol table's linked object file name with the
	// current one iff the symbol table's name is DEFAULT_MODULE and
	// the current one is not. Also replace the experiments for which
	// this symbol table is unsent.
	// 

	if((j->second.first.getLinkedObject() == default_module) &&
	   (linked_object != default_module)) {
	    
	    j->second.first.setLinkedObject(linked_object);

	    j->second.second = SentFilesTable::TheTable.getUnsent(
			           linked_object, ExperimentGroup(threads)
				   );

	}

	// Are there any experiments for which the symbol table is unsent?
	if(!j->second.second.empty()) {

	    // Add this module to the symbol table
	    j->second.first.addModule(*image, *module);
	    
	}
	
    }

    // Iterate over each symbol table
    for(SymbolTableMap::const_iterator 
	    i = symbol_tables.begin(); i != symbol_tables.end(); ++i) {

	// Send the frontend the initial "loaded" for the symbol tables
	Senders::loadedLinkedObject(
	    threads, now,
	    i->second.first.getRange(),
	    i->second.first.getLinkedObject(),
	    (i->second.first.getLinkedObject() == executable)
	    );

	// Are there any experiments for which the symbol table are unsent?
	if(!i->second.second.empty()) {

	    // Send the frontend the symbols for this shared library
	    Senders::symbolTable(
		i->second.second,
		i->second.first.getLinkedObject(), i->second.first
		);
	    
	    // This shared library is now sent for those experiments
	    SentFilesTable::TheTable.addSent(
		i->second.first.getLinkedObject(), i->second.second
		);
	    
	}

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

	// Iterate over each thread in this process
	for(int j = 0; j < threads.size(); ++j) {
	    BPatch_thread* thread = threads[j];
	    Assert(thread != NULL);

	    // Is the thread's state in the thread table out of date?
	    OpenSS_Protocol_ThreadState table_state =
		ThreadTable::TheTable.getThreadState(thread);

	    // FIXME DPM: Some threads may already be Terminated in the
	    // ThreadTable.  Do not allow them to be set to Running.
	    // Is this due to a race condition on the dyninst_state?
	    if( !((table_state == Terminated) && (dyninst_state == Running)) &&
		(table_state != Nonexistent) && (table_state != dyninst_state)) {

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

// Notification that we are mpi startup phase. This tells other Dyninst callbacks
// that we are not interested in symbols or instrumenting until the mpi
// startup flag is set back to false.
void OpenSpeedShop::Framework::Dyninst::setMPIStartup(bool mpi_startup_val)
{
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
        std::stringstream output;
	output << "[TID " << pthread_self() << "] Dyninst::"
	       << "setMPIStartup()  mpi_startup_val = " << mpi_startup_val << std::endl;
        std::cerr << output.str();
    }
#endif
   is_in_mpi_startup = mpi_startup_val;

    // Get Dyninst's list of active processes
    BPatch* bpatch = BPatch::getBPatch();
    Assert(bpatch != NULL);
    if (mpi_startup_val) {
        bpatch->registerDynLibraryCallback(NULL);
        bpatch->registerPostForkCallback(NULL);
        bpatch->registerThreadEventCallback(BPatch_threadCreateEvent,NULL);
        bpatch->registerThreadEventCallback(BPatch_threadDestroyEvent,NULL);
        bpatch->registerExecCallback(NULL);
    } else {
        bpatch->registerDynLibraryCallback(
            OpenSpeedShop::Framework::Dyninst::dynLibrary
            );
        bpatch->registerPostForkCallback(
            OpenSpeedShop::Framework::Dyninst::postFork
            );
        bpatch->registerThreadEventCallback(
            BPatch_threadCreateEvent,
            OpenSpeedShop::Framework::Dyninst::threadCreate
            );
        bpatch->registerThreadEventCallback(
            BPatch_threadDestroyEvent,
            OpenSpeedShop::Framework::Dyninst::threadDestroy
            );
        bpatch->registerExecCallback(
            OpenSpeedShop::Framework::Dyninst::exec
            );
    }
}
