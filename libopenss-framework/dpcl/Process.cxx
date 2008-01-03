////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the Process class.
 *
 */

/* Need the DYNINST_5_1 define. */
#include "BPatch.h"

#include "AddressSpace.hxx"
#include "Blob.hxx"
#include "DataBucket.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "Function.hxx"
#include "Guard.hxx"
#include "GuardWithDPCL.hxx"
#include "LinkedObject.hxx"
#include "OpenSS_Job.h"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "SymbolTable.hxx"
#include "ThreadGroup.hxx"

#include <ltdl.h>
#include <pthread.h>
#include <sstream>
#include <stdlib.h>
#include <vector>



#ifdef DPCL_SUPPORTS_STATEMENT_LIST
#undef DPCL_SUPPORTS_STATEMENT_LIST
#endif

namespace {
    /**
     * Suspend the calling thread.
     *
     * Suspends the calling thread for approximately 250 mS. Used to implement
     * busy loops that are waiting for state changes in threads.
     */
    void suspend()
    {
        // Setup to wait for 250 mS
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = 250 * 1000 * 1000;

        // Suspend ourselves temporarily
        // This while loop ensures that nanosleep will sleep at
        // least the amount of time even if a signal interupts nanosleep.
        while(nanosleep(&wait, &wait));
    }
}


//
// To avoid requiring fully qualified names to be used everywhere, something
// like "using namespace OpenSpeedShop::Framework;" is usually placed near the
// top of our source files. Here, however, that particular formulation doesn't
// work. DPCL defines a class Process in the global namespace and our typical
// using directive creates a conflict. The solution is to place the definitions
// directly inside the namespace. Then we are able to refer to "Process" and
// get "OpenSpeedShop::Framework::Process" unless we explicitly ask for the
// DPCL version via "::Process".
//
namespace OpenSpeedShop { namespace Framework {



/** 
 * Symbol table map.
 *
 * Type defining a mapping from an address range to a symbol table for that
 * address range along with its respective set of linked objects containing
 * that symbol table.
 *
 * @ingroup Implementation
 */
typedef std::map<AddressRange, std::pair<SymbolTable, std::set<LinkedObject> > >
    SymbolTableMap;


/**
 * Symbol table state.
 *
 * Structure for tracking the construction of the symbol tables. Multiple symbol
 * tables are often constructed for a single process, a symbol table can contain
 * multiple DPCL modules, and gathering information for those modules involves
 * issuing multiple asynchronous requests. This structure provides the necessary
 * mechanism for tracking when all the various requests have been completed so
 * that the finished symbol tables can finally be stored for the correct linked
 * objects.
 *
 * @ingroup Implementation
 */
struct SymbolTableState :
    public Lockable
{

    /** Flag indicating if requestAddressSpace() has finished constructing
	this object. */
    bool dm_constructed;
    
    /** Unique name of the process containing this symbol table. */
    std::string dm_name;
    
    /** Number of pending requests for symbol information. */
    unsigned dm_pending_requests;

    /** Symbol tables under construction. */
    SymbolTableMap dm_symbol_tables;
    
    /** Constructor from process' unique name. */
    SymbolTableState(const std::string& name) :
	Lockable(),
	dm_constructed(false),
	dm_name(name),
	dm_pending_requests(0),
	dm_symbol_tables()
    {
    }

};



/**
 * Constructor from process creation.
 *
 * Creates a new process to execute the specified command. The command is
 * created with the same initial environment as when the tool was started.
 * The process is created in a suspended state.
 *
 * @todo    Need to research a better way to handle creating commands that
 *          will support quoted strings, evaluations, I/O redirection, etc.
 *          Looking into what gdb does might be one avenue of research.
 *
 * @param host               Name of host on which to execute the command.
 * @param command            Command to be executed.
 * @param stdout_callback    Standard output stream callback.
 * @param stderr_callback    Standard error stream callback.
 */
Process::Process(const std::string& host, const std::string& command,
		 const OutputCallback stdout_callback, 
		 const OutputCallback stderr_callback) :
    Lockable(),
    in_mpi_startup(false),
#ifndef NDEBUG
    dm_previous_getstate(),
#endif
    dm_process(NULL),
    dm_host(host),
    dm_pid(0),
    dm_stdout_callback(stdout_callback),
    dm_stderr_callback(stderr_callback),
    dm_current_state(),
    dm_is_state_changing(),
    dm_future_state(),
    dm_libraries()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[Created] = Time::Now();
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::Process(\"" << host << "\", \"" 
	       << command << "\", ...)"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Allocate a new DPCL process handle
    dm_process = new ::Process();
    Assert(dm_process != NULL);

    // Extract individual arguments from the command
    std::vector<std::string> args;
    for(std::string::size_type 
	    i = command.find_first_not_of(' ', 0), next = command.find(' ', i);
	i != std::string::npos;
	i = command.find_first_not_of(' ', next), next = command.find(' ', i)) {
	
	// Extract this argument
	args.push_back(
	    command.substr(i, (next == std::string::npos) ? next : next - i)
	    );
	
    }
    
    // Search for executable and replace with its full, normalized, path
    args[0] = searchForExecutable(args[0]);
    
    // Translate the arguments into an argv-style argument list
    const char** argv = new const char*[args.size() + 1];
    Assert(argv != NULL);
    for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
	argv[i] = args[i].c_str();
    argv[args.size()] = NULL;
    
    // Declare access to the external environment variables
    extern char** environ;

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to create a process for executing the command
    AisStatus retval = dm_process->bcreate(dm_host.c_str(),
					   argv[0], argv, ::environ,
					   stdoutCallback, name, 
					   stderrCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("bcreate", retval);
#endif    

    // Destroy argv-style argument list
    delete [] argv;

    // Did the process creation fail?
    if(retval.status() != ASC_success) {

	// Throw an exception indicating command wasn't found
	throw Exception(Exception::CommandNotFound, host, command);
	
    }
    
    // Ask DPCL for the process identifier
    dm_pid = static_cast<pid_t>(dm_process->get_pid());

    // Replace the copy of our unique name with the REAL unique name
    *name = formUniqueName(dm_host, dm_pid);

    // Initialize the state of the process
    dm_current_state[*name] = Thread::Disconnected;
    dm_is_state_changing[*name] = false;
    dm_future_state[*name] = Thread::Disconnected;
    
    // Request an attachment to this process
    requestAttach();
}



/**
 * Constructor from process attachment.
 *
 * Creates a new process object for attaching to an existing process. The
 * object is put into the disconnected state and changeState() must then be
 * used to begin connecting to the actual process.
 *
 * @param host    Name of the host on which the process resides.
 * @param pid     Process identifier for the process.
 */
Process::Process(const std::string& host, const pid_t& pid) :  
    Lockable(),
    in_mpi_startup(false),
#ifndef NDEBUG
    dm_previous_getstate(),
#endif
    dm_process(NULL),
    dm_host(host),
    dm_pid(pid),
    dm_stdout_callback(OutputCallback(NULL, NULL)),
    dm_stderr_callback(OutputCallback(NULL, NULL)),
    dm_current_state(),
    dm_is_state_changing(),
    dm_future_state(),
    dm_libraries()
{
    // Note: Since no other thread can possibly access this object until AFTER
    //       it is created, and since the only DPCL function called here is the
    //       DPCL process handle constructor which only initializes the object's
    //       data members, it is safe to not do any locking here. In fact it
    //       is required. Using the usual guard introduces a deadlock between
    //       Instrumentor::changeState() and Process::attachCallback().
    //
    // GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[Created] = Time::Now();
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::Process(\"" << host << "\", " << pid << ")" 
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Allocate a new DPCL process handle
    dm_process = new ::Process(dm_host.c_str(), dm_pid);
    Assert(dm_process != NULL);

    // Initialize the state of the process
    std::string name = formUniqueName(dm_host, dm_pid);
    dm_current_state[name] = Thread::Disconnected;
    dm_is_state_changing[name] = false;
    dm_future_state[name] = Thread::Disconnected;    
}



/**
 * Destructor.
 *
 * Resumes this process, disconnects from it, and destroys our DPCL process
 * handle.
 */
Process::~Process()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::~Process() for " << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Resume this process and then disconnect from it
    dm_process->resume(NULL, NULL);
    dm_process->disconnect(NULL, NULL);   
    
    // Destroy the actual DPCL process handle
    delete dm_process;
}



/**
 * Get our host name.
 *
 * Return the name of the host on which this process is located.
 *
 * @return    Name of host on which this process is located.
 */
std::string Process::getHost() const
{
    Guard guard_myself(this);

    // Return the host name to the caller
    return dm_host;
}



/**
 * Get our process identifier.
 *
 * Returns the identifier of this process.
 *
 * @return    Identifier of this process.
 */
pid_t Process::getProcessId() const
{
    Guard guard_myself(this);

    // Return the process identifier to the caller
    return dm_pid;
}



/**
 * Test if connected.
 *
 * Returns a boolean value indicating if this process is connected or not.
 *
 * @return    Boolean "true" if this process is connected, "false" otherwise.
 */
bool Process::isConnected()
{
    Guard guard_myself(this);

    // Find the current state of the process
    Thread::State current = dm_current_state[formUniqueName(dm_host, dm_pid)];
    
    // Return flag indicating if this process is connected to the caller
    return ((current != Thread::Disconnected) &&
	    (current != Thread::Connecting) &&
	    (current != Thread::Nonexistent));
}



/**
 * Get a thread's state.
 *
 * Returns the caller the current state of a thread in this process. Since this
 * state changes asynchronously and must be updated across a network, there is a
 * lag between when the actual thread's state changes and when that is reflected
 * here.
2 *
 * @param thread    Thread whose state should be obtained.
 * @return          Current state of this thread.
 */
Thread::State Process::getState(const Thread& thread)
{
    Guard guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Form the unique name of the thread/process
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
    
    // Find the current state of the thread/process
    Thread::State current_state = findCurrentState(name);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::map<std::string, Thread::State>::const_iterator i =
	    dm_previous_getstate.find(name);
	if((i == dm_previous_getstate.end()) || (i->second != current_state)) {
	    dm_previous_getstate[name] = current_state;
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] "
		   << "Process::getState(T" 
		   << EntrySpy(thread).getEntry() << ") for " 
		   << name << " = "
		   << toString(current_state) 
		   << std::endl;
	    std::cerr << output.str();
	}
    }
#endif
    
    // Return our current state to the caller
    return current_state;
}



/**
 * Change a thread's state.
 *
 * Changes the current state of a thread in this process to the passed value.
 * Used to, for example, suspend a thread that was previously running. This
 * function does not wait until the thread has actually completed the state
 * change, and calling getState() immediately following changeState() will not
 * reflect the new state until the change has actually completed.
 *
 * @note    Only one in-progress state change is allowed per thread at any
 *          given time. For example, if you request that a thread be suspended,
 *          you cannot request that it be terminated before the suspension is
 *          completed. A StateAlreadyChanging exception is thrown when multiple
 *          in-progress changes are requested.
 *
 * @note    Some transitions are disallowed because they do not make sense or
 *          cannot be implemented. For example, a terminated thread cannot be
 *          set to a running thread. A StateChangeInvalid exception is thrown
 *          when such an invalid transition is requested.
 *
 * @param thread    Thread whose state should be changed.
 * @param state     Change to this state.
 */
void Process::changeState(const Thread& thread, const Thread::State& state)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Form the unique name of the thread/process
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
    
    // Find the state information of the thread/process
    Thread::State current_state = findCurrentState(name);
    bool is_changing_state = findIsChangingState(name);
    Thread::State future_state = findFutureState(name);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::changeState(T"
	       << EntrySpy(thread).getEntry() << ", " 
	       << toString(state) << ") for " << name
	       << std::endl;
	std::cerr << output.str();
	debugState(name);
    }
#endif

    // Finished if already in the requested state
    if(current_state == state)
	return;
    
    // Finished if already changing to the requested state
    if(is_changing_state && (future_state == state))
	return;
    
    // Disallow multiple, different, in-progress state changes
    if(is_changing_state)
	throw Exception(Exception::StateAlreadyChanging);

    // Handle [ Running | Suspended | Terminated ] --> Disconnected
    if(((current_state == Thread::Running) ||
	(current_state == Thread::Suspended) ||
	(current_state == Thread::Terminated)) &&
       (state == Thread::Disconnected)) {
	
	// Request disconnection from this process
	requestDisconnect();
	
    }
    
    // Handle [ Disconnected | Nonexistent ] --> Connecting
    else if(((current_state == Thread::Disconnected) ||
	     (current_state == Thread::Nonexistent)) &&
	    (state == Thread::Connecting)) {
	
	// Request connection to this process
	requestConnect();
	
    }

    // Handle Suspended --> Running
    else if((current_state == Thread::Suspended) && 
	    (state == Thread::Running)) {

	// Request resumption of this process or thread
	if(!tid.first)
	    requestResume();
	else
	    requestResume(tid.second);
	
    }
    
    // Handle Running --> Suspended
    else if((current_state == Thread::Running) && 
	    (state == Thread::Suspended)) {

	// Request suspension of this process or thread
	if(!tid.first)
	    requestSuspend();
	else
	    requestSuspend(tid.second);

    }

    // Handle [ Running | Suspended ] --> Terminated
    else if(((current_state == Thread::Running) ||
	     (current_state == Thread::Suspended)) &&
	    (state == Thread::Terminated)) {

	// Request destruction of this process or thread
	if(!tid.first)
	    requestDestroy();
	else
	    requestDestroy(tid.second);
	
    }
    
    // Otherwise throw an exception because the state change is invalid
    else
	throw Exception(Exception::StateChangeInvalid,
			toString(current_state), toString(state));
}



/**
 * Execute a library function now.
 *
 * Immediately executes the specified library function in a thread of this
 * process. The library is loaded into the process first if necessary.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Process::executeNow(const Collector& collector, 
			 const Thread& thread,
			 const std::string& callee, 
			 const Blob& argument,
			 const bool& disableSaveFPR)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Get the unique name of this thread (or the process containing it)
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeNow(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << callee << "\", ...) for " << name
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Find the requested "callee" function
    std::pair<LibraryEntry*, ProbeExp> callee_entry =
	findLibraryFunction(collector, callee);

    //
    // Create a probe expression for the code sequence:
    //
    //     callee(StringEncode(argument))
    //

    ProbeExp args_exp[1] = { ProbeExp(argument.getStringEncoding().c_str()) };
    
    ProbeExp expression;
    if (disableSaveFPR)
        expression = callee_entry.second.call(1, args_exp).disableSaveFPR();
    else
        expression = callee_entry.second.call(1, args_exp);
    
    // Request the instrumentation be executed
    ProbeHandle handle;
    if(!tid.first)
	requestExecute(expression, NULL, NULL);
    else
	requestExecute(expression, NULL, NULL, tid.second);
    
    // Add the empty probe handle to the probes for this thread
    callee_entry.first->dm_probes.insert(std::make_pair(thread, handle));
}



/**
 * Execute a library function at another function's entry/exit.
 *
 * Executes the specified library function every time another function's entry
 * or exit is executed in a thread of this process. The library is loaded into
 * the process first if necessary.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param where        Name of the function at whose entry/exit the library
 *                     function should be executed.
 * @param at_entry     Boolean "true" if instrumenting function's entry point,
 *                     or "false" if function's exit point.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Process::executeAtEntryOrExit(const Collector& collector,
				   const Thread& thread,
				   const std::string& where,
				   const bool& at_entry,
				   const std::string& callee,
				   const Blob& argument)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Get the unique name of this thread (or the process containing it)
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeAtEntryOrExit(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << where << "\", " 
	       << (at_entry ? "Entry" : "Exit") << ", \""
	       << callee << "\", ...) for " << name
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Find the requested "where" function and return if it couldn't be found
    SourceObj where_srcobj = findFunction(where);
    if(where_srcobj.src_type() != SOT_function)
	return;

    // Find the requested "callee" function
    std::pair<LibraryEntry*, ProbeExp> callee_entry =
	findLibraryFunction(collector, callee);
    
    //
    // Create a probe expression for the code sequence:
    //
    //     callee(StringEncode(argument))
    //
    
    ProbeExp args_exp[1] = { ProbeExp(argument.getStringEncoding().c_str()) };
    
    ProbeExp expression = callee_entry.second.call(1, args_exp);    
    
    // Iterate over the entry/exit points to the "where" function
    for(int p = 0; p < where_srcobj.exclusive_point_count(); ++p)
	if(where_srcobj.exclusive_point(p).get_type() ==
	   (at_entry ? IPT_function_entry : IPT_function_exit)) {
	    InstPoint point = where_srcobj.exclusive_point(p);
	    
	    // Request the instrumentation be installed and activated
	    ProbeHandle handle = !tid.first ?
		requestInstallAndActivate(expression, point, NULL, NULL) :
		requestInstallAndActivate(expression, point, NULL, NULL, 
					  tid.second);
	    
	    // Add this probe handle to the probes for this thread
	    callee_entry.first->dm_probes.insert(
		std::make_pair(thread, handle)
		);
	    
	}
}



/**
 * Execute a library function in place of another function.
 *
 * Executes the specified library function in place of another function every
 * other time that other function is called in a thread of this process. The
 * library is loaded into the process first if necessary.
 *
 * @note    The library function <em>must</em> take exactly the same parameters
 *          as the function it replaces and return the same type of value. It
 *          also <em>must</em> call the function it replaces once and only once.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void Process::executeInPlaceOf(const Collector& collector,
			       const Thread& thread,
			       const std::string& where,
			       const std::string& callee)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Get the unique name of this thread (or the process containing it)
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
        
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeInPlaceOf(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << where << "\", \"" 
	       << callee << "\") for " << name
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Find the requested "where" function and return if it couldn't be found
    SourceObj where_srcobj = findFunction(where);
    if(where_srcobj.src_type() != SOT_function)
	return;

    // Find the requested "callee" function
    std::pair<LibraryEntry*, ProbeExp> callee_entry =
	findLibraryFunction(collector, callee);

    // Ask DPCL to allocate a variable in this process for storing a flag
    int32_t initial_value = 0;
    AisStatus retval;
    ProbeExp flag_exp = 
	dm_process->balloc_mem(int32_type(), (void*)(&initial_value), retval);
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from balloc_mem", retval);
#endif
    if(retval.status() != ASC_success)
	return;

    // Add this variable to the variables for this thread
    callee_entry.first->dm_variables.insert(std::make_pair(thread, flag_exp));
    
    //
    // Create a probe expression (with recursion enabled) for the code sequence:
    //
    //     if(flag == 0) {
    //         flag = 1;
    //         jump wrapper();
    //     } else
    //         flag = 0;
    //

    ProbeExp expression = (
	(flag_exp == 0).ifelse(
	    flag_exp.assign(1).sequence(callee_entry.second.jump()),
	    flag_exp.assign(0)
	    )
	).recursive();

    // Iterate over the entry points to the "where" function
    for(int p = 0; p < where_srcobj.exclusive_point_count(); ++p)
	if(where_srcobj.exclusive_point(p).get_type() == IPT_function_entry) {
		char buffer[where_srcobj.get_demangled_name_length() + 1];
		where_srcobj.get_demangled_name(buffer, sizeof(buffer));
		char altbuffer[where_srcobj.get_alt_name_length() + 1];
		where_srcobj.get_alt_name(altbuffer, sizeof(altbuffer));

	    InstPoint point = where_srcobj.exclusive_point(p);
	    
	    // Request the instrumentation be installed and activated
	    ProbeHandle handle = !tid.first ?
		requestInstallAndActivate(expression, point, NULL, NULL) :
		requestInstallAndActivate(expression, point, NULL, NULL,
					  tid.second);
	    
	    // Add this probe handle to the probes for this thread
	    callee_entry.first->dm_probes.insert(
		std::make_pair(thread, handle)
		);
	    
	}
}



/**
 * Remove instrumentation.
 *
 * Removes all the instrumentation associated with the specified collector and
 * thread from this process.
 *
 * @param collector    Collector for which to remove instrumentation.
 * @param thread       Thread from which instrumentation should be removed.
 */
void Process::uninstrument(const Collector& collector, const Thread& thread)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Get the unique name of this thread (or the process containing it)
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
            
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::uninstrument(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ") for " << name
	       << std::endl;
	std::cerr << output.str();
    }
#endif   

    // Iterate over each library loaded into this process
    for(std::map<std::pair<Collector, std::string>, LibraryEntry>::iterator
	    i = dm_libraries.begin(); i != dm_libraries.end();) {

	// Ignore this library if it isn't associated with specified collector
	if(i->second.dm_collector != collector) {
	    ++i;
	    continue;
	}

	// Remove all probes associated with the thread from this process
	for(std::multimap<Thread, ProbeHandle>::iterator
		j = i->second.dm_probes.lower_bound(thread); 
	    j != i->second.dm_probes.upper_bound(thread); 
	    ++j)

	    // Note: Probes resulting from executeNow() have empty, dummy, 
	    //       probe handles that don't actually need to be removed.	
	    
	    if(j->second.get_point().get_type() != IPT_invalid) {

		// Request the probe be deactivated and removed
		requestDeactivateAndRemove(j->second);
		
	    }

	// Remove all probes associated with the thread from this library
	i->second.dm_probes.erase(thread);

	// Remove all variables associated with the thread from this process
	for(std::multimap<Thread, ProbeExp>::iterator
		j = i->second.dm_variables.lower_bound(thread); 
	    j != i->second.dm_variables.upper_bound(thread); 
	    ++j) {
	    
	    // Request the memory for this variable be freed
	    requestFree(j->second);
	    
	}
	
	// Remove all variables associated with the thread from this library
	i->second.dm_probes.erase(thread);

	// Have all the probes & variables for this library been removed?
	if(i->second.dm_probes.empty() && i->second.dm_variables.empty()) {
	    
	    // Request the library (module) be unloaded from this process
	    requestUnloadModule(i->second);
	    
	    // Remove the library from this process's library list
	    std::map<std::pair<Collector, std::string>, LibraryEntry>::iterator
	        to_erase = i;
	    ++i;
	    dm_libraries.erase(to_erase);	    
	    
	}
	else
	    ++i;
	
    }
}



/**
 * Stop at a function's entry/exit.
 *
 * Stops every time the specified function's entry or exit is executed in a
 * thread of this process.
 *
 * @todo    Currently the (breakpoint) instrumentation used to implement the
 *          stop isn't tracked. Thus the instrumentation is not removed when
 *          disconnecting from the process. That should change in the future.
 *
 * @param thread       Thread which should be stopped.
 * @param where        Name of the function at whose entry/exit the stop should
 *                     occur.
 * @param at_entry     Boolean "true" if instrumenting function's entry point,
 *                     or "false" if function's exit point.
 */
void Process::stopAtEntryOrExit(const Thread& thread,
				const std::string& where,
				const bool& at_entry)
{
    GuardWithDPCL guard_myself(this);

    // Get the thread identifier of the specified thread
    std::pair<bool, pthread_t> tid = thread.getPosixThreadId();

    // Note: The following code to insure MPI breakpoints are always process-
    //       wide and the getpid() conditional around the instrumentation are
    //       both hacks. They insure that stops for MPIR_Breakpoint are hit
    //       correctly on LLNL's SLURM based MPICH implementation.

    // Insure stops for MPIR_Breakpoint are always process-wide
    if(where == "MPIR_Breakpoint")
        tid.first = false;

    // Get the unique name of this thread (or the process containing it)
    std::string name = !tid.first ? 
	formUniqueName(dm_host, dm_pid) :
	formUniqueName(dm_host, dm_pid, tid.second);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::stopAtEntryOrExit(T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << where << "\", " 
	       << (at_entry ? "Entry" : "Exit") << ") for " << name
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Find the requested "where" function and return if it couldn't be found
    SourceObj where_srcobj = findFunction(where);
    if(where_srcobj.src_type() != SOT_function)
	return;

    // Find the getpid() function
    SourceObj getpid_func = findFunction("getpid");

    // Go no further if getpid() could not be found
    if(getpid_func.src_type() != SOT_function)
        return;

    //
    // Create a probe expression for the code sequence:
    //
    //     if(getpid() == pid) {
    //         Ais_send(Ais_msg_handle, "", 1)
    //         BREAKPOINT;
    //     }
    //

    ProbeExp args_exp[3] = {
	Ais_msg_handle,
	ProbeExp(""),
	ProbeExp(1)
    };

    ProbeExp expression =
        (getpid_func.reference().call(0, NULL) == ProbeExp(dm_pid)).ifelse(
            Ais_send.call(3, args_exp).sequence(ProbeExp::breakpoint())
            );

    // Allocate a copy of our unique name
    std::string* name_copy = new std::string(name);
    Assert(name_copy != NULL);    
    
    // Iterate over the entry/exit points to the "where" function
    for(int p = 0; p < where_srcobj.exclusive_point_count(); ++p)
	if(where_srcobj.exclusive_point(p).get_type() ==
	   (at_entry ? IPT_function_entry : IPT_function_exit)) {
	    InstPoint point = where_srcobj.exclusive_point(p);

	    // Request a breakpoint be installed and activated
	    ProbeHandle handle = !tid.first ?
		requestInstallAndActivate(expression, point, 
					  stoppedCallback, name_copy) :
		requestInstallAndActivate(expression, point,
					  stoppedCallback, name_copy,
					  tid.second);
	    
	}
}



/**
 * Get an integer global variable's value.
 *
 * Gets the current value of a signed integer global variable within this
 * process. The value is always returned as a signed 64-bit integer and is
 * promoted to this size when necessary. Returns a boolean value indicating
 * if the variable's value was succesfully retrieved.
 *
 * @param global    Name of global variable whose value is being requested.
 * @retval value    Current value of that variable.
 * @return          Boolean "true" if the variable's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::getGlobal(const std::string& global, int64_t& value)
{
    GuardWithDPCL guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getGlobal(\"" << global << "\", <integer>) for "
	       << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Find the requested global variable
    SourceObj variable = findVariable(global);

    // Go no further if the requested global could not be found
    if(variable.src_type() != SOT_data)
	return false;
    
    // Determine whether this variable is a signed integer and its size
    unsigned size = 0;
    switch(variable.get_data_type().get_node_type()) {
	
    case DEN_int8_type: size = sizeof(int8_t); break;
    case DEN_int16_type: size = sizeof(int16_t); break;
    case DEN_int32_type: size = sizeof(int32_t); break;
    case DEN_int64_type: size = sizeof(int64_t); break;
	
    // Go no further if the variable isn't a known integer type
    default: 
	return false;

    }

    //
    // Create a probe expression for the code sequence:
    //
    //     Ais_send(Ais_msg_handle, &global, size)
    //

    ProbeExp args_exp[3] = { 
	Ais_msg_handle,
	variable.reference().address(),
	size
    };

    ProbeExp expression = Ais_send.call(3, args_exp);

    // Define a data bucket to hold the retrieved integer
    DataBucket<int64_t> bucket;

    // Ask DPCL to execute the probe expression in this process
    AisStatus retval =
	dm_process->bexecute(expression, getIntegerCallback, &bucket);
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif
    if(retval.status() != ASC_success)
	return false;

    // Note: Data arriving from the Ais_send() cannot be accepted and placed
    //       into the data bucket unless the DPCL main loop is running. Since
    //       the GuardWithDPCL object above disables the main loop, it must
    //       be temporarily resumed here or a deadlock will occur.
    
    // Wait until the incoming integer arrives in the data bucket
    releaseLock();
    MainLoop::resume();
    value = bucket.getValue();
    MainLoop::suspend();
    acquireLock();

    // Indicate to the caller that the value was retrieved
    return true;
}



//Martin: added routine to set variable in mutatee
/**
 * Set an integer global variable's value.
 *
 * Sets the value of a signed integer global variable within this
 * process. Returns a boolean value indicating
 * if the variable's value was succesfully set.
 *
 * @param global    Name of global variable whose value is being requested.
 * @return          Boolean "true" if the variable's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::setGlobal(const std::string& global, int64_t value)
{
    GuardWithDPCL guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::setGlobal(\"" << global << "\", <integer>) for "
	       << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Find the requested global variable
    SourceObj variable = findVariable(global);

    // Go no further if the requested global could not be found
    if(variable.src_type() != SOT_data)
	return false;
    
    // Determine whether this variable is a signed integer and its size
    unsigned size = 0;
    ProbeExp inval;
    switch(variable.get_data_type().get_node_type()) {
	
    case DEN_int32_type: 
      size = sizeof(int32_t); 
      inval = ProbeExp((int32_t) value);
      break;

    case DEN_int64_type: 
      size = sizeof(int64_t); 
      inval = ProbeExp((uint64_t) value);
      break;
	
    // Go no further if the variable isn't a known integer type
    default: 
	return false;

    }

    //
    // Create a probe expression for the code sequence:
    //
    //     variable.reference().assign(inval)
    //

    ProbeExp expression = variable.reference().assign(inval);

    // Ask DPCL to execute the probe expression in this process
    AisStatus retval =
	dm_process->bexecute(expression, setIntegerCallback, (void*) NULL);
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif
    if(retval.status() != ASC_success)
	return false;

    // Indicate to the caller that the value was set
    return true;
}


/**
 * Get a string global variable's value.
 *
 * Gets the current value of a character string global variable within this
 * process. The value is returned as a C++ standard string rather than a C
 * character array. This makes managing the memory associated with the string
 * more obvious. Returns a boolean value indicating if the variable's value
 * was successfully retrieved.
 *
 * @param global    Name of global variable whose value is being requested.
 * @retval value    Current value of that variable.
 * @return          Boolean "true" if the variable's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::getGlobal(const std::string& global, std::string& value)
{
    GuardWithDPCL guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getGlobal(\"" << global << "\", <string>) for "
	       << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Find the requested global variable
    SourceObj variable = findVariable(global);

    // Go no further if the requested global could not be found
    if(variable.src_type() != SOT_data)
	return false;

    // Find the strlen() function
    SourceObj strlen_func = findFunction("strlen");    

    // Go no further if strlen() could not be found
    if(strlen_func.src_type() != SOT_function)
	return false;

    //
    // Create a probe expression for the code sequence:
    //
    //     if(&global)
    //         Ais_send(Ais_msg_handle, &global, strlen(&global) + 1)
    //     else
    //         Ais_send(Ais_msg_handle, "", 1)
    //

    ProbeExp send_empty_args_exp[3] = {
	Ais_msg_handle,
	ProbeExp(""),
	ProbeExp(1)
    };

    ProbeExp strlen_args_exp[1] = { 
	variable.reference()
    };

    ProbeExp send_args_exp[3] = {
	Ais_msg_handle, 
	variable.reference(),
	strlen_func.reference().call(1, strlen_args_exp) + ProbeExp(1)
    };

    ProbeExp expression = variable.reference().ifelse(
	Ais_send.call(3, send_args_exp),
	Ais_send.call(3, send_empty_args_exp)
	);
    
    // Define a data bucket to hold the retrieved character array
    DataBucket<char*> bucket;

    // Ask DPCL to execute the probe expression in this process
    AisStatus retval = 
	dm_process->bexecute(expression, getBlobCallback, &bucket);
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif
    if(retval.status() != ASC_success)
	return false;

    // Note: Data arriving from the Ais_send() cannot be accepted and placed
    //       into the data bucket unless the DPCL main loop is running. Since
    //       the GuardWithDPCL object above disables the main loop, it must
    //       be temporarily resumed here or a deadlock will occur.
    
    // Wait until the incoming character array arrives in the data bucket
    releaseLock();
    MainLoop::resume();
    char* buffer = bucket.getValue();
    MainLoop::suspend();
    acquireLock();
	
    // Extract and return the string's value if it was retrieved
    if(buffer != NULL) {
	value = std::string(buffer);
	delete [] buffer;
    }

    // Indicate to the caller that the value was retrieved
    return true;
}



/**
 * Get the MPICH process table.
 * 
 * Gets the current value of the MPICH process table within this process. The
 * value is returned as a Job rather than directly as an array of MPIR_PROCDESC
 * structures. This makes managing the memory associated with the table more
 * obvious. Returns a boolean value indicating if the table's value was
 * successfully retrieved.
 *
 * @todo    A better method for locating the "libopenss-framework-mpich.so"
 *          library needs to be added in the future. For now we simply look for
 *          it in the compile-time library directory. The best time to do this
 *          would be as part of the "remove libtool" project when the plugin
 *          loader is redone.
 * 
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @retval value    Current value of the table.
 * @return          Boolean "true" if the table's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::getMPICHProcTable(Job& value)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getMPICHProcTable(<job>) for "
	       << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Find the "MPIR_proctable_size" and "MPIR_proctable" variables
    SourceObj proctable_size_variable = findVariable("MPIR_proctable_size");
    SourceObj proctable_variable = findVariable("MPIR_proctable");

    // Go no further if either variable could not be found
    if((proctable_size_variable.src_type() != SOT_data) || 
       (proctable_variable.src_type() != SOT_data))
	return false;

    // Get the alternative install install path to be used to find
    // the MPIR_proctable for mpi job creation and attach.
    std::string openss_prefix_path;
    if(getenv("OPENSS_PREFIX") != NULL) {
	openss_prefix_path = getenv("OPENSS_PREFIX");
    }


#ifndef NDEBUG
    if(!openss_prefix_path.empty()) {
      if(is_debug_enabled) {
  	  std::stringstream output;
	  output << "[TID " << pthread_self() << "] "
	         << "Process::getMPICHProcTable openss_prefix_path()= "
	         << openss_prefix_path + '/' + std::string(LIBRARY_ABINAME)
	         << " Process::getMPICHProcTable std::string(LIBRARY_DIR)= "
	         << std::string(LIBRARY_DIR)
	         << std::endl;
	  std::cerr << output.str();
      }
    } // end path != NULL
#endif

    // If the alternative install install path is set, use it to find
    // the MPIR_proctable, otherwise use the existing LIBRARY_DIR ltdl method. 
    ProbeModule module;
    if (!openss_prefix_path.empty()) {
      // Ask DPCL to load the "libopenss-framework-mpich.so" library
      module = ProbeModule(
  	  (openss_prefix_path + '/' + std::string(LIBRARY_ABINAME) +
	   std::string("/libopenss-framework-mpich.so")).c_str()
  	);
    } else {
      module = ProbeModule(
  	  (std::string(LIBRARY_DIR) + 
	   std::string("/libopenss-framework-mpich.so")).c_str()
  	);
    }
    AisStatus retval = dm_process->bload_module(&module);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to bload_module", retval);
#endif
    if(retval.status() != ASC_success)
	return false;

    // Flag indicating if value was successfully retrieved
    bool succeeded = false;
    
    // Iterate over each function in this library
    for(int f = 0; f < module.get_count(); ++f) {
	
	// Obtain the function's name
	char buffer[module.get_name_length(f) + 1];
	module.get_name(f, buffer, sizeof(buffer));

	// Is this the "OpenSS_GetMPICHProcTable" function?
	if(std::string(buffer) == std::string("OpenSS_GetMPICHProcTable")) {

	    //
	    // Create a probe expression for the code sequence:
	    //
	    //     OpenSS_GetMPICHProcTable(Ais_msg_handle,
	    //                              &MPIR_proctable_size,
	    //                              &MPIR_proctable)
	    //
	    
	    ProbeExp args_exp[3] = { 
		Ais_msg_handle,
		proctable_size_variable.reference().address(),
		proctable_variable.reference().address()
	    };
	    
	    ProbeExp expression = module.get_reference(f).call(3, args_exp);
	    
	    // Define a data bucket to hold the retrieved process table
	    DataBucket<Job> bucket;
	    
	    // Ask DPCL to execute the probe expression in this process
	    retval = dm_process->bexecute(expression, getJobCallback, &bucket);
#ifndef NDEBUG
	    if(is_debug_enabled)
		debugDPCL("response from bexecute", retval);
#endif
	    if(retval.status() != ASC_success)
		break;
	    
	    // Note: Data arriving from the Ais_send() cannot be accepted and
	    //       placed into the data bucket unless the DPCL main loop is
	    //       running. Since the GuardWithDPCL object above disables
	    //       the main loop, it must be temporarily resumed here or a
	    //       deadlock will occur.
    
	    // Wait until the incoming process table arrives in the data bucket
	    releaseLock();
	    MainLoop::resume();
	    for(Job j = bucket.getValue(); !j.empty(); j = bucket.getValue())
		value.insert(value.end(), j.begin(), j.end());
	    MainLoop::suspend();
	    acquireLock();
	    succeeded = true;
	    
	    // No need to look at any more functions in this library
	    break;
	}

    }

    // Ask DPCL to unload the module
    retval = dm_process->bunload_module(&module);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to bunload_module", retval);
#endif
    
    // Indicate to the caller if the value was retrieved
    return succeeded;
}



#ifndef NDEBUG
/** Flag indicating if debugging for this class is enabled. */
bool Process::is_debug_enabled = (getenv("OPENSS_DEBUG_PROCESS") != NULL);



/** Flag indicating if performance debugging for this class is enabled. */
bool Process::is_debug_perf_enabled = 
		  (getenv("OPENSS_DEBUG_PERF_PROCESS") != NULL);



/**
 * Display DPCL callback debugging information.
 *
 * Displays debugging information for the passed DPCL callback to the standard
 * error stream. Reported information includes the active thread, the name of
 * the callback function being called, and the unique name of the process or
 * thread for which the callback was called.
 *
 * @param callback    String name of the callback (e.g. "attach").
 * @param name        Unique name of the process or thread for which callback
 *                    is called.
 */
void Process::debugCallback(const std::string& callback,
			    const std::string& name)
{
    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "Process::" << callback << "Callback() on " << name
	   << std::endl;
    
    // Display the string to the standard error stream
    std::cerr << output.str();
}



/**
 * Display DPCL call debugging information.
 *
 * Displays debugging information for the passed DPCL call to the standard error
 * stream. Reported information includes the active thread, the name of the DPCL
 * call being made, and the return value from DPCL.
 *
 * @param function    String name of the DPCL function (e.g. "Process::attach").
 * @param retval      Return value from that function.
 */
void Process::debugDPCL(const std::string& function, const AisStatus& retval)
{
    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "DPCL " << function << "() = " << retval.status_name()
	   << std::endl;
    
    // Display the string to the standard error stream
    std::cerr << output.str();
}



/**
 * Display a data buffer.
 *
 * Displays the passed data buffer to the specified stream. Reports each data
 * byte, sixteen per line, in both hexadecimal and character representations.
 *
 * @param stream    Stream to which the buffer should be displayed.
 * @param ptr       Pointer to the data buffer.
 * @param size      Length of the data buffer (in bytes).
 */
void Process::debugBuffer(std::ostream& stream, 
			  const char* ptr, const unsigned& size)
{
    static const unsigned BytesPerLine = 16;

    // Save the current formatting options of the stream for later restoration
    std::ios_base::fmtflags saved_flags = stream.flags();
    std::streamsize saved_width = stream.width();

    // Iterate over each byte in the data buffer
    for(unsigned i = 0; i < size; i += BytesPerLine) {

	// Indent each line by four spaces
	stream << "    ";

	// Display the hexadecimal representation
	for(unsigned j = 0; j < BytesPerLine; j++) {
	    if((i + j) < size)
		stream << std::hex << std::setfill('0') << std::setw(2)
		       << static_cast<unsigned>(
			   static_cast<uint8_t>(ptr[i + j]
			       )) << " ";
	    else
		stream << "   ";
	}

	// Indent another two spaces before the character representation
	stream << "  ";

	// Display the character representation
	for(unsigned j = 0; j < BytesPerLine; j++) {
	    if((i + j) < size) {
		if(isprint(ptr[i + j]))
		    stream << ptr[i + j];
		else
		    stream << ".";
	    }
	    else
		stream << " ";
	}

	// Complete this line
	stream << std::endl;
	
    }

    // Restore the original formatting options of the stream
    stream.flags(saved_flags);
    stream.width(saved_width);
}



/**
 * Display state debugging information.
 *
 * Displays debugging information for the state of a process or thread to the
 * standard error stream. Reported information includes the active thread, the
 * unique name of the process or thread, its current state, a boolean indicating
 * whether it has a pending state change, and its future state.
 *
 * @param name    Unique name of the process or thread.
 */
void Process::debugState(const std::string& name) const
{
    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "State of " << name << " is "
	   << toString(findCurrentState(name));
    if(findIsChangingState(name))
	output << " --> " << toString(findFutureState(name));
    output << std::endl;
    
    // Display the string to the standard error stream
    std::cerr << output.str();
}



/**
 * Display request debugging information.
 *
 * Displays debugging information for the passed request, on this process, to
 * the standard error stream. Reported information includes the active thread,
 * the name of the request function being called, its arguments, and the unique
 * name of the process on which the request was made.
 *
 * @param request      String name of the request (e.g. "Attach").
 * @param arguments    String arguments of the request (e.g. "10, 30").
 */
void Process::debugRequest(const std::string& request, 
			   const std::string& arguments) const
{
    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "Process::request" << request << "(" << arguments << ") on "
	   << formUniqueName(dm_host, dm_pid)
	   << std::endl;
    
    // Display the string to the standard error stream
    std::cerr << output.str();
}
#endif  // NDEBUG 



/**
 * Form a process' unique name.
 *
 * Returns the unique name for a process. The name is defined as being the
 * string "[host]:[pid]", where [host] is the name of the host on which the
 * process resides, and [pid] is the identifier of the process on that host.
 * This unique name is used for identifying processes within messages sent
 * between hosts.
 *
 * @param host    Name of the host on which the process resides.
 * @param pid     Process identifier for the process.
 * @return        Unique name of this process.
 */
std::string Process::formUniqueName(const std::string& host, const pid_t& pid)
{
    std::stringstream name;
    name << host << ":" << pid;
    return name.str();
}



/**
 * Form a thread's unique name.
 *
 * Returns the unique name for a thread. The name is defined as being the
 * string "[host]:[pid]:[tid]", where [host] is the name of the host on which
 * the process resides, [pid] is the identifier of the process on that host,
 * and [tid] is the identifier of the thread within that process. This unique
 * name is used for identifying threads within messages sent between hosts.
 *
 * @param host    Name of the host on which the thread resides.
 * @param pid     Identifier of the process containing the thread.
 * @param tid     Thread identifier for the thread.
 * @return        Unique name of this thread.
 */
std::string Process::formUniqueName(const std::string& host, 
				    const pid_t& pid,
				    const pthread_t& tid)
{
    std::stringstream name;
    name << host << ":" << pid << ":" << tid;
    return name.str();
}



/**
 * Get unique process name from unique name.
 *
 * Returns the unique process name from the specified unique process or thread
 * name. I.e. if the specified unique name is for a process, the name itself is
 * simply returned. But if the specfied unique name is for a thread, the unique
 * name of the process containing that thread is returned.
 *
 * @param name    Unique name of a process or thread.
 * @return        Unique name of the process.
 */
std::string Process::getProcessFromUniqueName(const std::string& name)
{
    // Find the ":" separator between the host name and process identifier
    std::string::size_type separator = name.find_first_of(':');    
    if(separator == std::string::npos)
	return std::string();

    // Find the ":" separator between the process and thread identifiers
    separator = name.find_first_of(':', separator + 1);
    if(separator == std::string::npos)
	return name;
    
    // Unique process name is everything up to the second separator
    return name.substr(0, separator);
}



/**
 * Get thread identifier from unique name.
 *
 * Returns the thread identifier from the specified unique process or thread
 * name. I.e. if the specified name is for a process, an empty name is returned.
 * But if the specified unique name is for a thread, the thread identifier (as
 * a string) for that thread is returned.
 *
 * @param name    Unique name of process or thread.
 * @return        Thread identifier of the thread.
 */
std::string Process::getThreadFromUniqueName(const std::string& name)
{
    // Find the ":" separator between the host name and process identifier
    std::string::size_type separator = name.find_first_of(':');    
    if(separator == std::string::npos)
	return std::string();

    // Find the ":" separator between the process and thread identifiers
    separator = name.find_first_of(':', separator + 1);
    if(separator == std::string::npos)
	return std::string();

    // Unique process name is everything after the second separator
    return name.substr(separator + 1, name.size() - separator - 1);
}



/**
 * Search for an executable.
 *
 * Returns the full, normalized, path name of the specified executable. This is
 * accomplished by using the current path of the tool's environment to search
 * for an executable. If the specified executable already has an absolyte path,
 * or if an executable cannot be found, the originally specified path is
 * returned unchanged.
 *
 * @param executable   Executable to be found.
 * @return             Full, normalized, path of the executable if found, or
 *                     the original executable name if not.
 */
Path Process::searchForExecutable(const Path& executable)
{
    // Return path unchanged if it is an absolute path
    if(executable.isAbsolute())
	return executable;

    // Return normalized path if it is an executable
    if(executable.getNormalized().isExecutable())
	return executable.getNormalized();
    
    // Get the binary search path
    if(getenv("PATH") != NULL) {
	std::string path = getenv("PATH");
	
	// Iterate over individual directories in the search path
	for(std::string::size_type 
		i = path.find_first_not_of(':', 0), next = path.find(':', i);
	    i != std::string::npos;
	    i = path.find_first_not_of(':', next), next = path.find(':', i)) {
	    
	    // Extract this directory
	    Path directory = 
		path.substr(i, (next == std::string::npos) ? next : next - i);
	    
	    // Assmeble the candidate and check if it is an executable
	    Path candidate = (directory + executable).getNormalized();
	    if(candidate.isExecutable())
		return candidate;
	    
	}
	
    }
    
    // Otherwise return the path unchanged
    return executable;
}



/**
 * Parse a library function name.
 *
 * Returns the separate library and function names parsed from the specified
 * library function name. If the passed name cannot be parsed, the returned
 * library and function names will be empty.
 *
 * @param function    Name of the library function to be parsed.
 * @return            Pair containing the parsed library and function names.
 */
std::pair<std::string, std::string> 
Process::parseLibraryFunctionName(const std::string& function)
{
    // Find the ":" separator between the library and function names
    std::string::size_type separator = function.find_first_of(':');    
    if(separator == std::string::npos)
	return std::pair<std::string, std::string>();
    
    // Library name is everything up to the separator (trimming spaces)
    std::string library_name =
	function.substr(0, separator);
    std::string::size_type trim_left = library_name.find_first_not_of(' ');
    std::string::size_type trim_right = library_name.find_last_not_of(' ');    
    if(trim_left < trim_right)
	library_name = 
	    library_name.substr(trim_left, trim_right - trim_left + 1);
    
    // Function name is everything after the separator (trimming spaces)
    std::string function_name =
	function.substr(separator + 1, function.size() - separator - 1);
    trim_left = function_name.find_first_not_of(' ');
    trim_right = function_name.find_last_not_of(' ');    
    if(trim_left < trim_right)
	function_name = 
	    function_name.substr(trim_left, trim_right - trim_left + 1);
    
    // Return the parsed library and function names to the caller
    return std::make_pair(library_name, function_name);
}



/**
 * Finish symbol table processing.
 *
 * Finishes symbol table processing by storing the specfied symbol table(s) for
 * the necessary linked object(s), updates the process' state, and destroys the
 * passed structure.
 *
 * @param state    Symbol table state structure to finish.
 */
void Process::finishSymbolTableProcessing(SymbolTableState* state)
{
    SmartPtr<Process> process;

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(state->dm_name);
    }
    
#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (process->dm_perf_data.find(FSTPEntered) == process->dm_perf_data.end()))
	process->dm_perf_data[FSTPEntered] = Time::Now();    
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::finishSymbolTableProcessing() for "
	       << state->dm_name
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Note: Since this function is called only once, after everyone has
    //       finished with the symbol table(s), the locking on "state" that 
    //       would usually be necessary here isn't needed.
    
    // Iterate over each symbol table in this state structure
    for(SymbolTableMap::iterator i = state->dm_symbol_tables.begin();
	i != state->dm_symbol_tables.end();
	++i) {
	
	// Iterate over each linked object for this symbol table
	for(std::set<LinkedObject>::const_iterator j = i->second.second.begin();
	    j != i->second.second.end(); 
	    ++j) {

	    // Process and store this symbol table for this linked object
	    i->second.first.processAndStore(*j);
	    
	}
	
    }
    
    // Critical section touching the process
    if(!process.isNull()) {
        Guard guard_process(*process);

	// Only proceed if process state is changing from "connecting"
	if(process->findIsChangingState(state->dm_name) && 
	   (process->findCurrentState(state->dm_name) == Thread::Connecting)) {

	    // Process has completed the state change
	    process->setCurrentState(state->dm_name,
				     process->findFutureState(state->dm_name));
	    
#ifndef NDEBUG
	    if(is_debug_perf_enabled)
		process->dm_perf_data[Ready] = Time::Now();
	    if(is_debug_enabled)
		process->debugState(state->dm_name);
#endif
	    
	}
    }    	
    
    // Destroy the heap-allocated state structure
    delete state;    
}



/**
 * Instrumentation activation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * activated in a process or thread. Contains only debugging code for now.
 */
void Process::activateProbeCallback(GCBSysType, GCBTagType tag,
				    GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("activateProbe", *name);
    	debugDPCL("response from activate_probe", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Address space change callback.
 *
 * Callback function called by the DPCL main loop when the address space of a
 * process has been changed. Issues an asynchronous request for the process'
 * new address space.
 */
void Process::addressSpaceChangeCallback(GCBSysType, GCBTagType,
					 GCBObjType, GCBMsgType msg)
{
    std::string name = std::string(reinterpret_cast<char*>(msg));
    SmartPtr<Process> process;
    ThreadGroup threads;

#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("addressSpaceChange", name);
#endif

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(name);

	// Attempt to locate the threads in this process
	threads = ProcessTable::TheTable.getThreadsByName(name);
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;

    // Ignore addressSpace changes while in mpi startup.
    if(process->inMPIStartup())
	return;

    // Request the current in-memory address space of this process
    process->requestAddressSpace(threads, Time::Now(), false);
}



/**
 * Process attachment callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * attached. Updates databases as necessary with any additional threads found
 * in this process and then issues an asynchronous request for the process'
 * address space.
 */
void Process::attachCallback(GCBSysType, GCBTagType tag, 
			     GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;

    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("attach", *name);
	debugDPCL("response from attach", *status);
    }
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
    }

#ifndef NDEBUG
    if(is_debug_perf_enabled && !process.isNull())
	process->dm_perf_data[AttachCompleted] = Time::Now();
#endif

    // Go no further if the process is no longer in the process table
    if(process.isNull()) {

	// Destroy the heap-allocated name string
	delete name;
	    
	return;
    }

    // Did the attach fail?
    if(status->status() != ASC_success) {
	    
	// Request disconnection from this process
	process->requestDisconnect();
	
    }
    else {

	// Original and added sets of threads for this process
	ThreadGroup original, added;
	
	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Get the original set of threads in this process
	    original = ProcessTable::TheTable.getThreadsByProcess(process);
	}
	
	// Get the identifiers of the threads in this process
	std::set<pthread_t> tids;
	process->getPosixThreadIds(tids);
	tids.erase(static_cast<pthread_t>(0));

#ifndef DEBUG
	if(is_debug_enabled) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] "
		   << "tids = { ";
	    for(std::set<pthread_t>::const_iterator
		    i = tids.begin(); i != tids.end(); ++i) {
		if(i != tids.begin())
		    output << ", ";
		output << *i;
	    }
	    output << " }" << std::endl;
	    std::cerr << output.str();
	}
#endif
	
	// Iterate over the original threads associated with this process
	for(ThreadGroup::const_iterator
		i = original.begin(); i != original.end(); ++i) {
	    
	    // Get this thread's thread identifier
	    std::pair<bool, pthread_t> tid = i->getPosixThreadId();
	    
	    // Is this thread a placeholder for the entire process?
	    if(!tid.first) {
		
		// Iterate over each thread identifier in this process
		for(std::set<pthread_t>::const_iterator
			j = tids.begin(); j != tids.end(); ++j) {
		    
		    // Replace the placeholder with the first real thread
		    if(j == tids.begin())
			i->setPosixThreadId(*j);
		    
		    // Copy the placeholder to represent subsequent threads
		    else {
			Thread t = i->createCopy();
			t.setPosixThreadId(*j);
			added.insert(t);
		    }
		    
		}
		
	    }
	    
	}
	
	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Add all the new threads to the process table
	    for(ThreadGroup::const_iterator
		    i = added.begin(); i != added.end(); ++i)
		ProcessTable::TheTable.addThread(*i);
	}
	
	// Combine the original and added threads into a single group
	ThreadGroup threads;
	threads.insert(original.begin(), original.end());
	threads.insert(added.begin(), added.end());
	
	// Note: In theory it would be possible to complete the process'
	//       state change at this point. Doing so, however, could allow
	//       certain other actions (such as starting data collection)
	//       to proceed before symbol table information was acquired.
	//       And starting data collection, for example, requires that
	//       the symbol table information be available. So for now we
	//       hold off completing the state change until after all the
	//       symbol table information has been acquired.
	
	// Request the current in-memory address space of this process
	process->requestAddressSpace(threads, Time::Now(), true);
	
    }

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Process connection callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * connected. Issues an asynchronous attach to the process.
 */
void Process::connectCallback(GCBSysType, GCBTagType tag, 
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("connect", *name);
	debugDPCL("response from connect", *status);
    }
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
    }

#ifndef NDEBUG
    if(is_debug_perf_enabled && !process.isNull())
	process->dm_perf_data[ConnectCompleted] = Time::Now();
#endif
	
    // Critical section touching the process
    if(!process.isNull()) {
	Guard guard_process(*process);

	// Does the specified process not exist?
	if(status->status() == ASC_invalid_pid) {

	    // Process is in the "nonexistent" state
	    process->setCurrentState(*name, Thread::Nonexistent);

#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState(*name);
#endif

	}

	// Did the connect otherwise fail?
	else if(status->status() != ASC_success) {

	    // Process is in the "disconnected" state
	    process->setCurrentState(*name, Thread::Disconnected);

#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState(*name);
#endif
	    
	}
	else {

	    // Request attachment to this process
	    process->requestAttach();

	}
	
    }

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Instrumentation deactivation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * deactivated in a process or thread. Contains only debugging code for now.
 */
void Process::deactivateProbeCallback(GCBSysType, GCBTagType tag,
				      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("deactivateProbe", *name);
    	debugDPCL("response from deactivate_probe", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Process or thread destruction callback.
 *
 * Callback function called by the DPCL main loop when a process or thread has
 * been destroyed. Updates the appropriate state entry.
 */
void Process::destroyCallback(GCBSysType, GCBTagType tag, 
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("destroy", *name);
    	debugDPCL("response from destroy", *status);
    }
#endif

    // Is the process or thread destroyed?
    if((status->status() == ASC_success) ||
       (status->status() == ASC_already_destroyed)) {
	    
	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Attempt to locate the process by its unique name
	    process = ProcessTable::TheTable.getProcessByName(
		getProcessFromUniqueName(*name)
		);
	}
	
	// Critical section touching the process
	if(!process.isNull()) {
	    Guard guard_process(*process);
	    
	    // Process or thread is in the "terminated" state
	    process->setCurrentState(*name, Thread::Terminated);
	    
#ifndef NDEBUG
            if(is_debug_enabled)
                process->debugState(*name);
#endif
	    
        }
	
    }
    
    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Process disconnection callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * disconnected. Updates the appropriate state entry.
 */
void Process::disconnectCallback(GCBSysType, GCBTagType tag, 
				 GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("disconnect", *name);
    	debugDPCL("response from disconnect", *status);
    }
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
    }

    // Critical section touching the process
    if(!process.isNull()) {
	Guard guard_process(*process);

	// Process is in the "disconnected" state
	process->setCurrentState(*name, Thread::Disconnected);

#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState(*name);
#endif
	
    }

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Instrumentation execution callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * executed in a process or thread. Contains only debugging code for now.
 */
void Process::executeCallback(GCBSysType, GCBTagType tag,
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    

    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("execute", *name);
    	debugDPCL("response from execute", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Module expanded callback.
 *
 * Callback function called by the DPCL main loop when a module has been
 * expanded. Locates all the functions in this module and stores the relevant
 * data in the appropriate symbol table (when necessary). Stores all symbol
 * table(s) in the appropriate database(s) once all pending requests have been
 * completed.
 */
void Process::expandCallback(GCBSysType, GCBTagType tag, 
			     GCBObjType obj, GCBMsgType msg)
{
    SymbolTableState* state = reinterpret_cast<SymbolTableState*>(tag);
    SourceObj* module = reinterpret_cast<SourceObj*>(obj);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(state != NULL);
    Assert(module != NULL);
    Assert(status != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("expand", state->dm_name);
    	debugDPCL("response from expand", *status);
    }
#endif

    // Critical section touching the symbol table state
    bool requests_completed = false;
    {
	Guard guard_state(state);

	// Did the expand succeed?
	if(status->status() == ASC_success) {

	    // Address offset for this module in this process (zero by default)
	    int64_t offset = 0;
	    
	    // Critical section touching the process table
	    {
		Guard guard_process_table(ProcessTable::TheTable);
		
		// Attempt to locate the process by its unique name
		SmartPtr<Process> process = 
		    ProcessTable::TheTable.getProcessByName(state->dm_name);
		
		// Critical section touching the process
		if(!process.isNull()) {
		    Guard guard_process(*process);
		    
		    // Obtain address offset for this module in this process
		    offset = module->get_offset_in_process(process->dm_pid);
		    
		}
	    }
	    
	    // Obtain the address range occupied by the module
	    AddressRange range(
		static_cast<Address>(module->address_start() + offset),
		static_cast<Address>(module->address_end() + offset)
		);

	    // Should this module's functions be stored in a symbol table?
	    if(state->dm_symbol_tables.find(range) !=
	       state->dm_symbol_tables.end()) {
		
		// Locate the appropriate symbol table for this module
		SymbolTable& symbol_table = 
		    state->dm_symbol_tables.find(range)->second.first;
		
		// Iterate over each function in this module
		for(int f = 0; f < module->child_count(); ++f)
		    if(module->child(f).src_type() == SOT_function) {
			SourceObj function = module->child(f);
			
			// Get the start/end address of the function
			// Note: As of dyninst 5.1, the addresses returned by dyninst
			// to dpcl already contain the module offset. Adding the offset
			// was preventing functions from libraries like libmpi.so from
			// being added to the database since they could not find an
			// address range to map to.
#if defined(DYNINST_5_1)
			Address start = function.address_start();
			Address end = function.address_end();
#else
			Address start = function.address_start() + offset;
			Address end = function.address_end() + offset;
#endif
			
			// Get the mangled name of the function
			char name[function.get_mangled_name_length() + 1];
			function.get_mangled_name(name, sizeof(name));
			
			// Add this function to the symbol table
			symbol_table.addFunction(start, end, name);
			
		    }
		
	    }
	    
	}

	// Decrement the pending request count
        requests_completed = 
	    (--state->dm_pending_requests == 0) && state->dm_constructed;	
    }

    // Finish symbol table processing if all requests have been completed
    if(requests_completed)
	finishSymbolTableProcessing(state);
}



/**
 * Free memory callback.
 *
 * Callback function called by the DPCL main loop when memory has been freed in
 * a process. Contains only debugging code for now.
 */
void Process::freeMemCallback(GCBSysType, GCBTagType tag,
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("freeMem", *name);
    	debugDPCL("response from free_mem", *status);
    }
#endif
    
    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Get blob callback.
 *
 * Callback function called by the DPCL main loop when a blob of data is being
 * returned. Allocates and stores a copy of the returned data, placing a pointer
 * to this copy in the specified data bucket.
 */
void Process::getBlobCallback(GCBSysType sys, GCBTagType tag,
			      GCBObjType, GCBMsgType msg)
{
    DataBucket<char*>* bucket = reinterpret_cast<DataBucket<char*>*>(tag);

    // Check assertions
    Assert(bucket != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getBlobCallback()"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Store a copy of the character array in the specified data bucket
    if(sys.msg_size > 0) {
	char* temp = new char[sys.msg_size];
	memcpy(temp, msg, sys.msg_size);
	bucket->setValue(temp);
    }
}



/**
 * Get integer callback.
 *
 * Callback function called by the DPCL main loop when an integer is being
 * returned. Stores the integer in the specified data bucket, promoting the
 * integer to 64-bit when necessary.
 *
 * @todo    Problems will occur here if the endianness of the processor running
 *          the instrumented process and the processor running the tool differ.
 *          This is one of only a few places were we aren't dealing with the
 *          endianness issue, so this should eventually be fixed.
 */
void Process::getIntegerCallback(GCBSysType sys, GCBTagType tag,
				 GCBObjType, GCBMsgType msg)
{
    DataBucket<int64_t>* bucket = reinterpret_cast<DataBucket<int64_t>*>(tag);

    // Check assertions
    Assert(bucket != NULL);
    Assert((sys.msg_size == sizeof(int8_t)) ||
	   (sys.msg_size == sizeof(int16_t)) ||
	   (sys.msg_size == sizeof(int32_t)) ||
	   (sys.msg_size == sizeof(int64_t)));
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getIntegerCallback()"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Store the integer in the specified data bucket
    if(sys.msg_size == sizeof(int8_t))
	bucket->setValue(
	    static_cast<int64_t>(*reinterpret_cast<int8_t*>(msg))
	    );
    else if(sys.msg_size == sizeof(int16_t))
	bucket->setValue(
	    static_cast<int64_t>(*reinterpret_cast<int16_t*>(msg))
	    );
    else if(sys.msg_size == sizeof(int32_t))
	bucket->setValue(
	    static_cast<int64_t>(*reinterpret_cast<int32_t*>(msg))
	    );
    else
	bucket->setValue(
	    static_cast<int64_t>(*reinterpret_cast<int64_t*>(msg))
	    );
}


/**
 * set integer callback.
 *
 * Callback function called by the DPCL main loop when interger has been set
 * in a process or thread. Contains only debugging code for now.
 *
 * @todo    Problems will occur here if the endianness of the processor running
 *          the instrumented process and the processor running the tool differ.
 *          This is one of only a few places were we aren't dealing with the
 *          endianness issue, so this should eventually be fixed.
 */
void Process::setIntegerCallback(GCBSysType, GCBTagType tag,
				   GCBObjType, GCBMsgType msg)
{
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::setIntegerCallback()"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

}


/**
 * Get job callback.
 *
 * Callback function called by the DPCL main loop when a blob of data
 * containing a job description is being returned. Store the job in the
 * specified data bucket.
 */
void Process::getJobCallback(GCBSysType sys, GCBTagType tag,
			     GCBObjType, GCBMsgType msg)
{
    DataBucket<Job>* bucket = reinterpret_cast<DataBucket<Job>*>(tag);

    // Check assertions
    Assert(bucket != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getJobCallback()"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Decode this data blob
    Blob blob(sys.msg_size, msg);
    OpenSS_Job job;
    memset(&job, 0, sizeof(job));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_OpenSS_Job), &job);
    
    // Translate the OpenSS_Job structure into a Job object
    Job value;
    for(unsigned i = 0; i < job.processes.processes_len; ++i) {

	// Insert this host/pid pair into the result
	value.push_back(std::make_pair(
			    std::string(job.processes.processes_val[i].host),
			    job.processes.processes_val[i].pid
			    ));
    
    }

    // Store the job in the specified data bucket
    bucket->setValue(value);

    // Store an empty job in the specified data bucket to indicate the end of
    // a complete job description (if the end has actually been reached)
    if(job.is_last)
	bucket->setValue(Job());
    
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_OpenSS_Job),
	     reinterpret_cast<char*>(&job));
}



/**
 * Instrumentation installation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * installed in a process or thread. Contains only debugging code for now.
 */
void Process::installProbeCallback(GCBSysType, GCBTagType tag,
				   GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("installProbe", *name);
    	debugDPCL("response from install_probe", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Load module callback.
 *
 * Callback function called by the DPCL main loop when a module has been loaded
 * into a process. Contains only debugging code for now.
 */
void Process::loadModuleCallback(GCBSysType, GCBTagType tag,
				 GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    

    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("loadModule", *name);
    	debugDPCL("response from load_module", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Out-of-band data callback.
 *
 * Callback function called by the DPCL main loop when out-of-band data has
 * been received. Simply enqueues this performance data for later storage in
 * an experiment database.
 */
void Process::outOfBandDataCallback(GCBSysType sys, GCBTagType,
				    GCBObjType, GCBMsgType msg)
{
#if !defined(NDEBUG) && defined(WDH_SHOW_RECEIVED_DATA)
    printf("outOfBandDataCallback(sys.msg_size = %d, msg = %p)\n",
	   sys.msg_size, msg);
    debugBuffer(std::cout, (const char*)msg, sys.msg_size);
    fflush(stdout);
#endif

    // Enqueue this performance data
    DataQueues::enqueuePerformanceData(Blob(sys.msg_size, msg));
}



/**
 * Instrumentation removal callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * removed from a process or thread. Contains only debugging code for now.
 */
void Process::removeProbeCallback(GCBSysType, GCBTagType tag,
				  GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("removeProbe", *name);
    	debugDPCL("response from remove_probe", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Process or thread resumption callback.
 *
 * Callback function called by the DPCL main loop when a process or thread has
 * been resumed. Updates the appropriate state entry.
 */
void Process::resumeCallback(GCBSysType, GCBTagType tag, 
			     GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("resume", *name);
	debugDPCL("response from resume", *status);
    }
#endif

    // Did the resume succeed?
    if(status->status() == ASC_success) {
	
	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Attempt to locate the process by its unique name
	    process = ProcessTable::TheTable.getProcessByName(
		getProcessFromUniqueName(*name)
		);
	}
	
	// Critical section touching the process
	if(!process.isNull()) {
	    Guard guard_process(*process);
	    
	    // Process or thread is in the "running" state
	    process->setCurrentState(*name, Thread::Running);
	    
#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState(*name);
#endif
	    
	}
	
    }
    
    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Module statements callback.
 *
 * Callback function called by the DPCL main loop when a module's statements
 * have been received. Locates all the statements in this module and stores the
 * relevant data in the appropriate symbol table (when necessary). Stores all
 * symbol table(s) in the appropriate database(s) once all pending requests
 * have been completed.
 */
void Process::statementsCallback(GCBSysType, GCBTagType tag, 
				 GCBObjType obj, GCBMsgType msg)
{
    SymbolTableState* state = reinterpret_cast<SymbolTableState*>(tag);
    SourceObj* module = reinterpret_cast<SourceObj*>(obj);
#ifdef DPCL_SUPPORTS_STATEMENT_LIST
    struct get_statement_list_msg_t* retval =
	reinterpret_cast<struct get_statement_list_msg_t*>(msg);
#else
    struct get_stmt_info_list_msg_t* retval =
	reinterpret_cast<struct get_stmt_info_list_msg_t*>(msg);
#endif
    
    // Check assertions
    Assert(state != NULL);
    Assert(module != NULL);
    Assert(retval != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("statements", state->dm_name);
#ifdef DPCL_SUPPORTS_STATEMENT_LIST
	debugDPCL("response from get_statement_list", retval->status);
#else
    	debugDPCL("response from get_all_statements", retval->status);
#endif
    }
#endif

    // Critical section touching the symbol table state
    bool requests_completed = false;
    {
	Guard guard_state(state);

	// Did the statement request succeed?
	if(retval->status.status() == ASC_success) {

	    // Address offset for this module in this process (zero by default)
	    int64_t offset = 0;
	
	    // Critical section touching the process table
	    {
		Guard guard_process_table(ProcessTable::TheTable);
		
		// Attempt to locate the process by its unique name
		SmartPtr<Process> process = 
		    ProcessTable::TheTable.getProcessByName(state->dm_name);
		
		// Critical section touching the process
		if(!process.isNull()) {
		    Guard guard_process(*process);
		    
		    // Obtain address offset for this module in this process
		    offset = module->get_offset_in_process(process->dm_pid);
		    
		}
	    }
	    
	    // Obtain the address range occupied by the module
	    AddressRange range(
		static_cast<Address>(module->address_start() + offset),
		static_cast<Address>(module->address_end() + offset)
		);
	    
	    // Should this module's statements be stored in a symbol table?	
	    if(state->dm_symbol_tables.find(range) !=
	       state->dm_symbol_tables.end()) {
		
		// Locate the appropriate symbol table for this module
		SymbolTable& symbol_table = 
		    state->dm_symbol_tables.find(range)->second.first;
	    
#ifdef DPCL_SUPPORTS_STATEMENT_LIST
		// Access the statement list from the return value
		StatementList* statements = retval->statement_list_p;
		Assert(statements != NULL);

		// Iterate over each statement in this module
		statements->resetToFirstStatement();
		while(true) {

		    // Get the next statement
		    AisAddress begin, end;
		    std::string path;
		    int line, column;
		    if(!statements->getStatement(begin, end,
						 path, line, column))
			break;

		    // Add this statement to the symbol table
		    symbol_table.addStatement(
			Address(begin + offset), Address(end + offset),
			path, line, column
			);
		    
		}
#else
		// Access the statement information from the return value
		StatementInfoList* statements = retval->stmt_info_list_p;
		Assert(statements != NULL);
		
		// Iterate over each source file in this module
		for(int f = 0; f < statements->get_count(); ++f) {
		    StatementInfo info = statements->get_entry(f);
		    
		    // Iterate over each statement in this source file
		    for(int s = 0; s < info.get_line_count(); ++s) {
			StatementInfoLine line = info.get_line_entry(s);
		    
			// Iterate over each address range in this statement
			for(int r = 0; 
			    (r + 1) < line.get_address_count(); 
			    r += 2) {

			    // Add this statement to the symbol table
			    symbol_table.addStatement(
				Address(line.get_address_entry(r) + offset),
				Address(line.get_address_entry(r + 1) + offset),
				info.get_filename(), 
				line.get_line(), 
				line.get_column()
				);
			    
			}
			
		    }
		    
		}
#endif
		
	    }

	}

	// Decrement the pending request count
	requests_completed =
            (--state->dm_pending_requests == 0) && state->dm_constructed;
    }

    // Destroy the statement information returned by DPCL
#ifdef DPCL_SUPPORTS_STATEMENT_LIST
    delete reinterpret_cast<StatementList*>(retval->statement_list_p);
#else
    delete reinterpret_cast<StatementInfoList*>(retval->stmt_info_list_p);
#endif
    
    // Finish symbol table processing if all requests have been completed
    if(requests_completed)
	finishSymbolTableProcessing(state);
}



/**
 * Standard error callback.
 *
 * Callback function called by the DPCL main loop when a created process sends
 * data to its standard error stream. Passes the output to the standard error
 * stream callback for the process, or redirects it to the tool's standard
 * error stream if no callback was specified.
 */
void Process::stderrCallback(GCBSysType sys, GCBTagType tag, 
			     GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    SmartPtr<Process> process;

    // Check assertions
    Assert(name != NULL);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("stderr", *name);
#endif

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
    }
    
    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;

    // Note: A critical section touching the process is not required here
    //       because the standard error stream callback is set once upon object
    //       creation and then never changed again.

    // Is there a standard error stream callback set for this process?
    if(process->dm_stderr_callback.first != NULL) {
	
	// Call the standard error stream callback
	(*(process->dm_stderr_callback.first))
	    ((char*)msg, sys.msg_size, process->dm_stderr_callback.second);
	
    }
    else {
	
	// Redirect output to tool's standard error stream
	char* ptr = (char*)msg;
	for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	    fputc(*ptr, stderr);
	fflush(stderr);
    }	
}



/**
 * Standard out callback.
 *
 * Callback function called by the DPCL main loop when a created process sends
 * data to its standard out stream. Passes the output to the standard out stream
 * callback for the process, or redirects it to the tool's standard out stream
 * if no callback was specified.
 */
void Process::stdoutCallback(GCBSysType sys, GCBTagType tag, 
			     GCBObjType, GCBMsgType msg)
{    
    std::string* name = reinterpret_cast<std::string*>(tag);
    SmartPtr<Process> process;

    // Check assertions
    Assert(name != NULL);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("stdout", *name);
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;

    // Note: A critical section touching the process is not required here
    //       because the standard out stream callback is set once upon object
    //       creation and then never changed again.
    
    // Is there a standard output stream callback set for this process?
    if(process->dm_stdout_callback.first != NULL) {
	
	// Call the standard output stream callback
	(*(process->dm_stdout_callback.first))
	    ((char*)msg, sys.msg_size, process->dm_stdout_callback.second);
	
    }
    else {
	
	// Redirect output to tool's standard output stream
	char* ptr = (char*)msg;
	for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	    fputc(*ptr, stdout);
	fflush(stdout);
    }	
}



/**
 * Process or thread stopped callback.
 *
 * Callback function called by the DPCL main loop when a process or thread has
 * been stopped at a breakpoint. Updates the appropriate state entry.
 */
void Process::stoppedCallback(GCBSysType, GCBTagType tag, 
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("stopped", *name);
#endif

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(
	    getProcessFromUniqueName(*name)
	    );
    }
    
    // Critical section touching the process
    if(!process.isNull()) {
	Guard guard_process(*process);

	// Note: The process is already suspended at this point, but DPCL
	//       doesn't know that. Asking it to suspend the process does
	//       no harm and insures that DPCL's internal view of the
	//       process state matches our own.

	// Ask DPCL to asynchronously suspend this process
	AisStatus retval = process->dm_process->suspend(NULL, NULL);
#ifndef NDEBUG
	if(is_debug_enabled)
	    debugDPCL("request to suspend", retval);
#endif
	
	// Process or thread is in the "suspended" state
	process->setCurrentState(*name, Thread::Suspended);
	
#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState(*name);
#endif
	
    }

    // Note: It is impossible to tell how many times the stop will be executed
    //       so don't destroy the heap-allocated name string here or subsequent
    //       stops will find an invalid pointer.
    //
    // delete name;
}



/**
 * Process or thread suspension callback.
 *
 * Callback function called by the DPCL main loop when a process or thread has
 * been suspended. Updates the appropriate state entry.
 */
void Process::suspendCallback(GCBSysType, GCBTagType tag, 
			      GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    
    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("suspend", *name);
    	debugDPCL("response from suspend", *status);
    }
#endif

    // Did the suspend succeed?
    if(status->status() == ASC_success) {

	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Attempt to locate the process by its unique name
	    process = ProcessTable::TheTable.getProcessByName(
		getProcessFromUniqueName(*name)
		);
	}
	
	// Critical section touching the process
	if(!process.isNull()) {
	    Guard guard_process(*process);
	    
	    // Process or thread is in the "suspended" state
	    process->setCurrentState(*name, Thread::Suspended);
	    
#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState(*name);
#endif
	    
        }
	
    }

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Process termination callback.
 *
 * Callback function called by the DPCL main loop when a process has terminated.
 * Updates the appropriate state entry.
 */
void Process::terminationCallback(GCBSysType, GCBTagType,
				  GCBObjType, GCBMsgType msg)
{
    std::string name = std::string(reinterpret_cast<char*>(msg));
    SmartPtr<Process> process;

#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("termination", name);
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(name);
    }

    // Critical section touching the process
    if(!process.isNull()) {
        Guard guard_process(*process);

	// Process is in the "suspended" state
	process->setCurrentState(name, Thread::Terminated);

#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState(name);
#endif

    }
}



/**
 * Thread list change callback.
 *
 * Callback function called by the DPCL main loop when the thread list of a
 * process has been changed. Updates databases as necessary with the new thread
 * if one is being created and then issues an asynchronous request for the
 * process' address space. Updates the appropriate state entry.
 */
void Process::threadListChangeCallback(GCBSysType, GCBTagType,
				       GCBObjType, GCBMsgType msg)
{
    
    std::string name = std::string(reinterpret_cast<char*>(msg));
    SmartPtr<Process> process;

#ifndef NDEBUG
    if(is_debug_enabled)
	debugCallback("threadListChange", name);
#endif
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(
	    getProcessFromUniqueName(name)
	    );
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;

    // Ignore thread list changes while in mpi startup.
    if(process->inMPIStartup())
	return;

    // Get the thread identifier of the thread being created or destroyed
    std::string tid_string = getThreadFromUniqueName(name);
    if(tid_string.empty())
	return;
    pthread_t tid = 
	static_cast<pthread_t>(strtoul(tid_string.c_str(), NULL, 10));
    if(tid == 0)
	return;

    // Get the identifiers of the threads in this process
    std::set<pthread_t> tids;
    if(!process->getPosixThreadIds(tids))
	return;
    tids.erase(static_cast<pthread_t>(0));

    // Is the thread being created?
    bool is_created = (tids.find(tid) != tids.end());

    // Update databases as necessary if a new thread is being created
    if(is_created) {
	
	// Original and added sets of threads for this process
	ThreadGroup original, added;
	
	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Get the original set of threads in this process
	    original = ProcessTable::TheTable.getThreadsByProcess(process);
	}

	CollectorGroup collectors;
        // We need to update the newly created threads corresponding to ranks
        // on thread based mpi implementations.  So, at this point we will
        // get the mpi implementation set into existing/original threads/ranks
        // and place the implementation name into the newly created thread/rank.
        // Look for references to the next data items for the code to do this.
        bool has_mpiimpl = false;
        std::pair<bool, std::string> mpiImpl; 
        std::string mpiImplname = "";
	// Iterate over the original threads associated with this process
	for(ThreadGroup::const_iterator
		i = original.begin(); i != original.end(); ++i) {

              // Don't test if we already have it.
            if (!has_mpiimpl) {
                 mpiImpl = (*i).getMPIImplementation();
            }

            // only find it once.
            if(mpiImpl.first && !has_mpiimpl) {
               has_mpiimpl= true;
               mpiImplname = mpiImpl.second;
            } 

	    // Does created thread not exist in original thread's database?
	    if(!i->doesSiblingExist(tid)) {

		// Copy the original thread for representing the created thread
		Thread t = i->createCopy();
		t.setPosixThreadId(tid);
                if (has_mpiimpl) {
                    t.setMPIImplementation(mpiImplname);
                } 
		added.insert(t);			    
		
	    } 

	    collectors = i->getCollectors();

	}

	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Add all the new threads to the process table
	    for(ThreadGroup::const_iterator
		    i = added.begin(); i != added.end(); ++i) {
		ProcessTable::TheTable.addThread(*i);

            }
	}

	// Request the current in-memory address space of this process.
	// Since this is handling new threads (pthreads) created after the
	// experiment was first created, we need to attach an existing collectors
	// in the experiment to each new thread.  The process is suspended
	// at this point so other threads do not run while we are getting
	// the address space and attaching the collectors.
	if(!added.empty()) {
	    if (added.isAnyState(Thread::Running)) {
	      {
		Guard guard_process(*process);
		process->setCurrentState(name, Thread::Suspended);
		// Wait until the threads are suspended
		suspend();
		while(!added.areAllState(Thread::Suspended)) {
		    suspend();
		}
	      }
	    }

	    // request address space and attach any existing collectors.
	    process->requestAddressSpace(added, Time::Now(), true);
	    added.startCollecting(collectors);

	    if (added.isAnyState(Thread::Suspended)) {
	      {
		Guard guard_process(*process);
		process->setCurrentState(name, Thread::Running);
		// Wait until the threads are suspended
		suspend();
		while(added.isAnyState(Thread::Suspended)) {
		    suspend();
		}
	      }
	    }
	}

    }

    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Thread is in either the "running" or "terminated" state
	process->setCurrentState(name, is_created ? Thread::Running : 
				 Thread::Terminated);
	
#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState(name);
#endif
    }
}



/**
 * Unload module callback.
 *
 * Callback function called by the DPCL main loop when a module has been
 * unloaded from a process. Contains only debugging code for now.
 */
void Process::unloadModuleCallback(GCBSysType, GCBTagType tag,
				   GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);    

    // Check assertions
    Assert(name != NULL);
    Assert(status != NULL);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("unloadModule", *name);
    	debugDPCL("response from unload_module", *status);
    }
#endif

    // Destroy the heap-allocated name string
    delete name;
}



/**
 * Request process' address space.
 *
 * Requests the module list for this process from DPCL and uses that list to
 * update the specified thread(s) with the current in-memory address space of
 * this process. Asynchronously requests that DPCL obtain symbol information
 * for each linked object in the process' address space. The requests are made
 * only when actually necessary. After completing such requests, DPCL will call
 * expandCallback() and statementsCallback().
 *
 * NOTE: See the comments in AddressSpace.cxx for updateThreads regarding
 * the update_time_interval parameter.
 *
 * @todo    DPCL provides some amount of call site information. Processing this
 *          needs to be added once the CallSite object is properly defined.
 *
 * @param threads    Thread(s) within this process to be updated.
 * @param when       Time at which update occured.
 * @param update_time_interval   Flag to indicate whether to update time_end to when value.       
 */
void Process::requestAddressSpace(const ThreadGroup& threads,
				  const Time& when,
				  const bool update_time_interval)
{    
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (dm_perf_data.find(RqstAddrSpcEntered) == dm_perf_data.end()))
	dm_perf_data[RqstAddrSpcEntered] = Time::Now();
    if(is_debug_enabled)	
	debugRequest("AddressSpace", "");
#endif
    
    // Address space for this process
    AddressSpace address_space;

    // Flag indicating if the executable has been found yet
    bool found_executable = false;

    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);

	// Obtain the address offset for this module in this process
	int64_t offset = module.get_offset_in_process(dm_pid);

	// Obtain the address range occupied by the module
	AddressRange range(
	    static_cast<Address>(module.address_start() + offset),
	    static_cast<Address>(module.address_end() + offset)
	    );

	// Obtain the name of the module
	std::string name = "";
	if(module.module_name_length() > 0) {
	    char buffer[module.module_name_length() + 1];
	    module.module_name(buffer, module.module_name_length() + 1);
	    name = buffer;
	}
	
#ifndef NDEBUG
	if(is_debug_enabled) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] "
		   << "Module " << range << " " << name
		   << std::endl;
	    std::cerr << output.str();
	}
#endif	

	// Note: When an entry for this address range already exists in the
	//       address space, assume that we are dealing with the executable,
	//       which always contains two or more DPCL modules. Update the
	//       entry for this address range to use the name of the program as
	//       the linked object name and indicate it is an executable.
	
	// Is this address range already in the address space?
	if(address_space.hasValue(range)) {

	    // Is the executable still unidentified?
	    if(!found_executable) {

		// Indicate we have now found the executable
		found_executable = true;
		
		// Obtain the name of the program
		if(program.program_name_length() > 0) {
		    char buffer[program.program_name_length() + 1];
		    program.program_name(buffer, 
					 program.program_name_length() + 1);
		    name = buffer;
		}
		
		// Update the address range's entry in the address space
		address_space.setValue(range, name, true);
		
	    }
	    
	}

	// Otherwise create the address range's entry in the address space
	else if(name != std::string("DEFAULT_MODULE"))
	    address_space.setValue(range, name, false);
	
    }
    
    // Update the specified threads with this address space

    std::map<AddressRange, std::set<LinkedObject> > needed =
	address_space.updateThreads(threads, when, update_time_interval);

    // Allocate and initialize state structure for tracking the requests
    SymbolTableState* state = 
	new SymbolTableState(formUniqueName(dm_host, dm_pid));
    Assert(state != NULL);

#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (dm_perf_data.find(RqstAddrSpcIssue) == dm_perf_data.end()))
	dm_perf_data[RqstAddrSpcIssue] = Time::Now();
#endif

    // Critical section touching the symbol table state
    bool requests_completed = false;
    {
	Guard guard_state(state);

	// Iterate over each needed symbol table for this address space
	for(std::map<AddressRange, std::set<LinkedObject> >::const_iterator
		t = needed.begin(); t != needed.end(); ++t) {

	    // Add an empty symbol table to the state for this address range
	    state->dm_symbol_tables.insert(
		std::make_pair(t->first,
			       std::make_pair(SymbolTable(t->first), t->second))
		);
	    
	}
	
	// Iterate over each module associated with this process (again)
	for(int m = 0; m < program.child_count(); ++m) {
	    SourceObj module = program.child(m);

	    // Obtain the address offset for this module in this process
	    int64_t offset = module.get_offset_in_process(dm_pid);

	    // Obtain the address range occupied by the module
	    AddressRange range(
		static_cast<Address>(module.address_start() + offset),
		static_cast<Address>(module.address_end() + offset)
		);

	    //
	    // Only expand the module if its symbols are needed OR it doesn't
	    // already have children. The first case insures that the symbol
	    // table is always filled in properly. The second case insures that
	    // every module has children, and thus is instrumentable, before
	    // the process is marked as connected.
	    //
	    if((state->dm_symbol_tables.find(range) !=
		state->dm_symbol_tables.end()) ||
	       (module.child_count() == 0)) {
		
		// Ask DPCL to asynchronously expand this module
		AisStatus retval = module.expand(
		    *dm_process, expandCallback, state
		    );
#ifndef NDEBUG
		if(is_debug_enabled) 
		    debugDPCL("request to expand", retval);
#endif
	    
		// Increment the pending request count if the request succeeded
		if(retval.status() == ASC_success)
		    state->dm_pending_requests++;

	    }

	    // Only get the module's statements if its symbols are needed
	    if(state->dm_symbol_tables.find(range) !=
	       state->dm_symbol_tables.end()) {
		
		// Ask DPCL to asynchronously get this module's statements
#ifdef DPCL_SUPPORTS_STATEMENT_LIST
		AisStatus retval = module.get_statement_list(
		    *dm_process, statementsCallback, state
		    );
#ifndef NDEBUG
		if(is_debug_enabled) 
		    debugDPCL("request to get_statement_list", retval);
#endif	
#else
		AisStatus retval = module.get_all_statements(
		    *dm_process, statementsCallback, state, module
		    );
#ifndef NDEBUG
		if(is_debug_enabled) 
		    debugDPCL("request to get_all_statements", retval);
#endif	
#endif
		
		// Increment the pending request count if the request succeeded
		if(retval.status() == ASC_success)
		    state->dm_pending_requests++;
		
	    }
	    
	}

	// Indicate construction of the state tracking structure is complete
	state->dm_constructed = true;
	
	// Are all the requests completed?
	requests_completed =
	    (state->dm_pending_requests == 0) && state->dm_constructed;
	
    }

#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (dm_perf_data.find(RqstAddrSpcExited) == dm_perf_data.end()))
	dm_perf_data[RqstAddrSpcExited] = Time::Now();
#endif
    
    // Finish symbol table processing if all requests have been completed
    if(requests_completed)
        finishSymbolTableProcessing(state);    
}



/**
 * Request process attachment.
 *
 * Asynchronously requests that DPCL attaches to this process. Attempts to issue
 * the request and then immediately returns. After completing the request, DPCL
 * will call attachCallback().
 */
void Process::requestAttach()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)	
	debugRequest("Attach", "");
#endif
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[AttachIssued] = Time::Now();
#endif

    // Ask DPCL to asynchronously attach to this process
    AisStatus retval = dm_process->attach(attachCallback, name);
#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[AttachAcknowledged] = Time::Now();
    if(is_debug_enabled) 
	debugDPCL("request to attach", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
	// Request disconnection from this process
	requestDisconnect();
	
    }
    else {

	// Process is in the "connecting" state
	setCurrentState(*name, Thread::Connecting);
	
	// Process is changing to the "suspended" state
	setFutureState(*name, Thread::Suspended);
	
#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request process connection.
 *
 * Asynchronously requests that DPCL connects to this process. Attempts to issue
 * the request and then immediately returns. After completing the request, DPCL
 * will call connectCallback().
 */
void Process::requestConnect()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Connect", "");
#endif
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[ConnectIssued] = Time::Now();
#endif

    // Ask DPCL to asynchronously connect to this process
    AisStatus retval = dm_process->connect(connectCallback, name);
#ifndef NDEBUG
    if(is_debug_perf_enabled)
	dm_perf_data[ConnectAcknowledged] = Time::Now();
    if(is_debug_enabled) 
	debugDPCL("request to connect", retval);
#endif
    if(retval.status() != ASC_success) {

	// Process is in the "disconnected" state
	setCurrentState(*name, Thread::Disconnected);

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState(*name);
#endif
	
	// Destroy the heap-allocated unique name string
	delete name;

    }
    else {

	// Process is in the "connecting" state
	setCurrentState(*name, Thread::Connecting);
	
	// Process is changing to the "suspended" state
	setFutureState(*name, Thread::Suspended);
	
#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request instrumentation deactivation and removal.
 *
 * Asynchronously requests that DPCL deactivate and remove instrumentation from
 * this process. Attempts to issue the requests and then immediately returns.
 * After completing the requests, DPCL call call deactivateProbeCallback() and
 * removeProbeCallback().
 *
 * @param handle    Handle to the instrumentation to be deactivated and removed.
 */
void Process::requestDeactivateAndRemove(ProbeHandle handle)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("DeactivateAndRemove", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously deactivate the probe in this process
    AisStatus retval = 
	dm_process->deactivate_probe(1, &handle, deactivateProbeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to deactivate_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;

    }

    // Allocate another copy of our unique name
    name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to asynchronously remove the probe from this process
    retval = 
	dm_process->remove_probe(1, &handle, removeProbeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to remove_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
}



/**
 * Request process destruction.
 *
 * Asynchronously requests that DPCL destroy this process. Attempts to issue
 * the request and then immediately returns. After completing the request, DPCL
 * will call destroyCallback().
 */
void Process::requestDestroy()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
        debugRequest("Destroy", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously destroy to this process
    AisStatus retval = dm_process->destroy(destroyCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to destroy", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Process is changing to the "terminated" state
	setFutureState(*name, Thread::Terminated);

#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request thread destruction.
 *
 * Asynchronously requests that DPCL destroy a thread in this process.
 * Attempts to issue the request and then immediately returns. After
 * completing the request, DPCL will call destroyCallback().
 *
 * @param tid    Thread identifier of the thread to be destroyed.
 */
void Process::requestDestroy(pthread_t tid)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << tid;
	debugRequest("Destroy", arguments.str());
    }
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid, tid));
    Assert(name != NULL);

    // Note: When we request that Dyninst terminate a single thread it
    //       currently terminates the entire process containing that thread.
    //       All indications from the Dyninst folks are that per-thread
    //       termination will be available in the future. For now simply
    //       indicate the reality that the entire process is being destroyed.
    {
	delete name;
	name = new std::string(formUniqueName(dm_host, dm_pid));
	Assert(name != NULL);
    }

    // Ask DPCL to asynchronously destroy to this thread
    AisStatus retval = dm_process->destroy(destroyCallback, name/*, tid*/);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to destroy", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Thread is changing to the "terminated" state
	setFutureState(*name, Thread::Terminated);
	
#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request process disconnection.
 *
 * Asynchronously requests that DPCL disconnect this process. Attempts to issue
 * the request and then immediately returns. After completing the request, DPCL
 * will call disconnectCallback().
 */
void Process::requestDisconnect()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Disconnect", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously disconnect from this process
    AisStatus retval = dm_process->disconnect(disconnectCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to disconnect", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
    else {

	// Process is changing to the "disconnected" state
	setFutureState(*name, Thread::Disconnected);

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState(*name);
#endif
	
    }
}



/**
 * Request instrumentation execution.
 *
 * Asynchronously requests that DPCL execute instrumentation in this process.
 * Attempts to issue the request and then immediately returns. After completing
 * the request, DPCL will call executeCallback().
 *
 * @param expression    Probe expression to be executed.
 * @param callback      Callback to receive data from this instrumentation.
 * @param tag           Tag to be sent to the above callback.
 */
void Process::requestExecute(ProbeExp expression, 
			     GCBFuncType callback, 
			     GCBTagType tag)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Execute", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously execute the probe expression in this process
    AisStatus retval = 
	dm_process->execute(expression, callback, tag, executeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to execute", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
}



/**
 * Request instrumentation execution.
 *
 * Asynchronously requests that DPCL execute instrumentation in a thread in
 * this process. Attempts to issue the request and then immediately returns.
 * After completing the request, DPCL will call executeCallback().
 *
 * @param expression    Probe expression to be executed.
 * @param callback      Callback to receive data from this instrumentation.
 * @param tag           Tag to be sent to the above callback.
 * @param tid           Thread identifier of the thread in which to execute
 *                      the instrumentation.
 */
void Process::requestExecute(ProbeExp expression, 
			     GCBFuncType callback, 
			     GCBTagType tag,
			     pthread_t tid)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << "..., " << tid;
	debugRequest("Execute", arguments.str());
    }
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid, tid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously execute the probe expression in this process
    AisStatus retval = dm_process->execute(expression, callback, 
					   tag, executeCallback, name, tid);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to execute", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
}



/**
 * Request memory release.
 *
 * Asynchronously requests that DPCL free memory in this process. Attempts to
 * issue the request and then immediately returns. After completing the request,
 * DPCL will call freeMemCallback().
 *
 * @param expression    Probe expression for the memory to be freed.
 */
void Process::requestFree(ProbeExp expression)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Free", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL To asynchronously free the memory in this process
    AisStatus retval = dm_process->free_mem(expression, freeMemCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to free_mem", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
}



/**
 * Request instrumentation installation and activation.
 *
 * Asynchronously requests that DPCL install and activate instrumentation in
 * this process. Attempts to issue the requests and then immediately returns.
 * After completing the requests, DPCL will call installCallback() and
 * activateCallback().
 *
 * @param expression    Probe expression to be installed and activated.
 * @param point         Point at which instrumentation should be installed
 *                      and activated.
 * @param callback      Callback to receive data from this instrumentation.
 * @param tag           Tag to be sent to the above callback.
 * @return              Handle to the resulting instrumentation.
 */
ProbeHandle Process::requestInstallAndActivate(ProbeExp expression, 
					       InstPoint point,
					       GCBFuncType callback,
					       GCBTagType tag)
{
    GuardWithDPCL guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("InstallAndActivate", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously install the probe in this process
    ProbeHandle handle;
    AisStatus retval = dm_process->install_probe(1, &expression, &point,
						 &callback, &tag,
						 installProbeCallback, name,
						 &handle);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to install_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;

    }

    // Allocate another copy of our unique name
    name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to asynchronously activate the probe in this process
    retval = 
	dm_process->activate_probe(1, &handle, activateProbeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to activate_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }

    // Return the probe handle to the caller
    return handle;
}



/**
 * Request instrumentation installation and activation.
 *
 * Asynchronously requests that DPCL install and activate instrumentation in a
 * thread in this process. Attempts to issue the requests and then immediately
 * returns. After completing the requests, DPCL will call installCallback() and
 * activateCallback().
 *
 * @param expression    Probe expression to be installed and activated.
 * @param point         Point at which instrumentation should be installed
 *                      and activated.
 * @param callback      Callback to receive data from this instrumentation.
 * @param tag           Tag to be sent to the above callback.
 * @param tid           Thread identifier of the thread in which to install
 *                      and activate the instrumentation.
 * @return              Handle to the resulting instrumentation.
 */
ProbeHandle Process::requestInstallAndActivate(ProbeExp expression, 
					       InstPoint point,
					       GCBFuncType callback,
					       GCBTagType tag,
					       pthread_t tid)
{
    GuardWithDPCL guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << "..., " << tid;
	debugRequest("InstallAndActivate", arguments.str());
    }
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid, tid));
    Assert(name != NULL);

    // FIXME:
    // Get the threads in this process. If there is only one thread we will
    // assume this thread represents the main process and not pass the tid
    // argument to install_probe.  This works around an issue with processes that have
    // linked in libpthreads.so but have not actually call pthread_create to
    // create any threads.  Such threads have a 64bit value for the thread id
    // of the main process and the dpcl thread wrapper in dpcl/src/lib/src/Process.C
    // fails when it compares "tid" to the value from dyninst's pthread_self().
    // If dyninst eventually supports per thread point instrumentation,
    // this can be revisited.

    std::set<pthread_t> tids;
    getPosixThreadIds(tids);

    // Ask DPCL to asynchronously install the probe in this process
    ProbeHandle handle;
    AisStatus retval;
    if (tids.size() == 1 ) {
	retval = dm_process->install_probe(1, &expression, &point,
						 &callback, &tag,
						 installProbeCallback, name,
						 &handle);
    } else {
	retval = dm_process->install_probe(1, &expression, &point,
						 &callback, &tag,
						 installProbeCallback, name,
						 &handle, tid);
    }

#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to install_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;

    }

    // Allocate another copy of our unique name
    name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to asynchronously activate the probe in this process
    retval = 
	dm_process->activate_probe(1, &handle, activateProbeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to activate_probe", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }

    // Return the probe handle to the caller
    return handle;
}



/**
 * Request that a process load a module.
 *
 * Asynchronously requests that DPCL load a library (module) into this process.
 * Attempts to issue the request and then immediately returns. After completing
 * the request, DPCL will call loadModuleCallback().
 *
 * @note    Libraries are located using libltdl and the standard search path
 *          established by the CollectorPluginTable class. If the library can't
 *          be located, the dm_path, dm_module, and dm_functions fields of the
 *          library entry will be left unchanged.
 *
 * @param library    Library entry to be loaded.
 */
void Process::requestLoadModule(LibraryEntry& library)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << "C" << EntrySpy(library.dm_collector).getEntry() 
		  << ", \"" << library.dm_name << "\"";
	debugRequest("LoadModule", arguments.str());
    }
#endif

    // Can we open this library as a libltdl module?
    lt_dlhandle handle = lt_dlopenext(library.dm_name.c_str());
    if(handle == NULL)
	return;
    
    // Get the full path of the library
    const lt_dlinfo* info = lt_dlgetinfo(handle);
    Assert(info != NULL);
    library.dm_path = info->filename;
    
    // Close the module handle
    Assert(lt_dlclose(handle) == 0);
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously load this module
    library.dm_module = ProbeModule(library.dm_path.c_str());
    AisStatus retval = dm_process->load_module(&(library.dm_module), 
					       loadModuleCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to load_module", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;

    }

    // Clear the list of functions for this library
    library.dm_functions.clear();
        
    // Iterate over each function in this library
    for(int f = 0; f < library.dm_module.get_count(); ++f) {
	
	// Obtain the function's name
	char buffer[library.dm_module.get_name_length(f) + 1];
	library.dm_module.get_name(f, buffer, sizeof(buffer));
	
	// Add this function to the list of functions for this library
	library.dm_functions.insert(std::make_pair(buffer, f));
	
    }
}



/**
 * Request process resumption.
 *
 * Asynchronously requests that DPCL resume this process. Attempts to issue the
 * request and then immediately returns. After completing the request, DPCL will
 * call resumeCallback().
 */
void Process::requestResume()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
        debugRequest("Resume", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously resume this process
    AisStatus retval = dm_process->resume(resumeCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to resume", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Process is changing to the "running" state
	setFutureState(*name, Thread::Running);

#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request thread resumption.
 *
 * Asynchronously requests that DPCL resume a thread in this process.
 * Attempts to issue the request and then immediately returns. After
 * completing the request, DPCL will call resumeCallback().
 *
 * @param tid    Thread identifier of the thread to be resumed.
 */
void Process::requestResume(pthread_t tid)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << tid;
	debugRequest("Resume", arguments.str());
    }
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid, tid));
    Assert(name != NULL);

    // Note: When we request that Dyninst continue a single thread it
    //       currently continues the entire process containing that thread.
    //       All indications from the Dyninst folks are that per-thread
    //       continuation will be available in the future. For now simply
    //       indicate the reality that the entire process is resuming.
    {
	delete name;
	name = new std::string(formUniqueName(dm_host, dm_pid));
	Assert(name != NULL);
    }

    // Ask DPCL to asynchronously resume this thread
    AisStatus retval = dm_process->resume(resumeCallback, name/*, tid*/);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to resume", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Thread is changing to the "running" state
	setFutureState(*name, Thread::Running);

#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request process suspension.
 *
 * Asynchronously requests that DPCL suspend this process. Attempts to issue the
 * request and then immediately returns. After completing the request, DPCL will
 * call suspendCallback().
 */
void Process::requestSuspend()
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
        debugRequest("Suspend", "");
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously suspend this process
    AisStatus retval = dm_process->suspend(suspendCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to suspend", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Process is changing to the "suspended" state
	setFutureState(*name, Thread::Suspended);
	
#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request thread suspension.
 *
 * Asynchronously requests that DPCL suspend a thread in this process.
 * Attempts to issue the request and then immediately returns. After
 * completing the request, DPCL will call suspendCallback().
 *
 * @param tid    Thread identifier of the thread to be suspended.
 */
void Process::requestSuspend(pthread_t tid)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << tid;
	debugRequest("Suspend", arguments.str());
    }
#endif

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid, tid));
    Assert(name != NULL);

    // Note: When we request that Dyninst stop a single thread it currently
    //       stops the entire process containing that thread. All indications
    //       from the Dyninst folks are that per-thread stops will be available
    //       in the future. For now simply indicate the reality that the entire
    //       process is suspending.
    {
	delete name;
	name = new std::string(formUniqueName(dm_host, dm_pid));
	Assert(name != NULL);
    }

    // Ask DPCL to asynchronously suspend this thread
    AisStatus retval = dm_process->suspend(suspendCallback, name/*, tid*/);
#ifndef NDEBUG
    if(is_debug_enabled)
        debugDPCL("request to suspend", retval);
#endif
    if(retval.status() != ASC_success) {

        // Destroy the heap-allocated unique name string
        delete name;

    }
    else {

	// Thread is changing to the "suspended" state
	setFutureState(*name, Thread::Suspended);
	
#ifndef NDEBUG
        if(is_debug_enabled)
            debugState(*name);
#endif

    }
}



/**
 * Request that a process unload a module.
 *
 * Asynchronously requests that DPCL unload a library (module) from this
 * process. Attempts to issue the request, and then immediately returns.
 * After completing the request, DPCL will call unloadModuleCallback().
 *
 * @param library    Library entry to be unloaded.
 */
void Process::requestUnloadModule(LibraryEntry& library)
{
    GuardWithDPCL guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream arguments;
	arguments << "C" << EntrySpy(library.dm_collector).getEntry() 
		  << ", \"" << library.dm_name << "\")";
	debugRequest("UnloadModule", arguments.str());
    }
#endif
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to asynchronously unload this module
    AisStatus retval = dm_process->unload_module(&(library.dm_module), 
						 unloadModuleCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to unload_module", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;
	
    }
}



/**
 * Find the current state.
 *
 * Returns the current state of the specified process or thread. Assumes that
 * the process' current state should be used if the current state for a thread
 * cannot be found.
 *
 * @param name    Unique name of the process or thread to be found.
 * @return        Process or thread's current state.
 */
Thread::State Process::findCurrentState(const std::string& name) const
{
    Guard guard_myself(this);

    // Look for the specified name in the map of current states
    std::map<std::string, Thread::State>::const_iterator i =
	dm_current_state.find(name);
    
    // Look for the process' name if the specified name wasn't found
    if(i == dm_current_state.end())
	i = dm_current_state.find(getProcessFromUniqueName(name));
    
    // Check assertions
    Assert(i != dm_current_state.end());
    
    // Return the current state to the caller
    return i->second;
}



/**
 * Find if state is changing.
 *
 * Returns a flag indicating if the specified process or thread is undergoing
 * a state change. Assumes that the process' flag should be used if the flag
 * for a thread cannot be found.
 *
 * @param name    Unique name of the process or thread to be found.
 * @return        Boolean "true" if the process or thread is changing state,
 *                "false" otherwise.
 */
bool Process::findIsChangingState(const std::string& name) const
{
    Guard guard_myself(this);

    // Look for the specified name in the map of state change flags
    std::map<std::string, bool>::const_iterator i =
	dm_is_state_changing.find(name);
    
    // Look for the process' name if the specified name wasn't found
    if(i == dm_is_state_changing.end())
	i = dm_is_state_changing.find(getProcessFromUniqueName(name));
    
    // Check assertions
    Assert(i != dm_is_state_changing.end());
    
    // Return the state change flag to the caller
    return i->second;
}



/**
 * Find the future state.
 *
 * Returns the future state of the specified process or thread. Assumes that
 * the process' future state should be used if the future state for a thread
 * cannot be found.
 *
 * @param name    Unique name of the process or thread to be found.
 * @return        Process or thread's future state.
 */
Thread::State Process::findFutureState(const std::string& name) const
{
    Guard guard_myself(this);

    // Look for the specified name in the map of future states
    std::map<std::string, Thread::State>::const_iterator i =
	dm_future_state.find(name);
    
    // Look for the process' name if the specified name wasn't found
    if(i == dm_future_state.end())
	i = dm_future_state.find(getProcessFromUniqueName(name));
    
    // Check assertions
    Assert(i != dm_future_state.end());
    
    // Return the future state to the caller
    return i->second;
}



/**
 * Set the current state.
 *
 * Sets the current state of the specified process or thread to the given value
 * and indicates that the process or thread is not changing state.
 *
 * @note    Insures that any state change in the process applies to all threads
 *          in the process as well.
 *
 * @param name     Unique name of the process or thread to be set.
 * @param state    New current state of that process or thread.
 */
void Process::setCurrentState(const std::string& name, 
			      const Thread::State& state)
{
    Guard guard_myself(this);

    // Is the specified name a process name?
    if(name == getProcessFromUniqueName(name)) {

	// All threads must have the same current state as the process
	dm_current_state.clear();
	dm_is_state_changing.clear();
	dm_future_state.clear();
	
	// Set the current state of the process
	dm_current_state[name] = state;
	dm_is_state_changing[name] = false;
	dm_future_state[name] = state;
	
    }

    // Is the process' current state the same as the thread's new state?
    else if(findCurrentState(getProcessFromUniqueName(name)) == state) {
	
	// Simply erase the thread-specific state information
	dm_current_state.erase(name);
	dm_is_state_changing.erase(name);
	dm_future_state.erase(name);
	
    }

    // Thread is in a different state than the process
    else {
	
	// Set the current state of the thread
	dm_current_state[name] = state;
	dm_is_state_changing[name] = false;
	dm_future_state[name] = state;
	
    }
}



/**
* Set the future state.
*
* Sets the future state of the specified process or thread to the given value
* and indicates that the process or thread is changing state.
*
* @note    Insures that any state change in the process applies to all threads
*          in the process as well.
*
* @param name     Unique name of the process or thread to be set.
* @param state    New future state of that process or thread.
*/
void Process::setFutureState(const std::string& name, 
			     const Thread::State& state)
{
    Guard guard_myself(this);

    // Is the specified name a process name?
    if(name == getProcessFromUniqueName(name)) {

	// All threads must have the same future state as the process
	dm_is_state_changing.clear();
	dm_future_state.clear();

    }
    
    // Set the future state of the process or thread
    dm_is_state_changing[name] = true;
    dm_future_state[name] = state;
}



/**
 * Find a function.
 *
 * Finds the named function in this process and returns a source object for that
 * function to the caller. A source object of type SOT_unknown_type is returned
 * if the function cannot be found.
 *
 * @param name    Name of the function to be found.
 * @return        Source object for the named function or a source object of
 *                type SOT_unknown_type if the function could not be found.
 */
SourceObj Process::findFunction(const std::string& name) const
{
    Guard guard_myself(this);

    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);
	
	// Iterate over each function in this module
	for(int f = 0; f < module.child_count(); ++f)
	    if(module.child(f).src_type() == SOT_function) {
                SourceObj function = module.child(f);
		
		// Get the name of this function
		char buffer[function.get_demangled_name_length() + 1];
		function.get_demangled_name(buffer, sizeof(buffer));

		// Get the alternate (weak) name of this function
		char altbuffer[function.get_alt_name_length() + 1];
		function.get_alt_name(altbuffer, sizeof(altbuffer));

		// Return this source object if it is the requested function
		if(std::string(buffer) == name ||
		   std::string(altbuffer) == name) {
		    return function;
		}
		
	    }

    }

    // Return a default constructed source object if the function wasn't found
    return SourceObj();    
}



/**
 * Find a variable.
 *
 * Finds the named variable in this process and returns a source object for that
 * variable to the caller. A source object of type SOT_unknown_type is returned
 * if the variable cannot be found.
 *
 * @param name    Name of the variable to be found.
 * @return        Source object for the named variable or a source object of
 *                type SOT_unknown_type if the variable could not be found.
 */
SourceObj Process::findVariable(const std::string& name) const
{
    Guard guard_myself(this);

    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);
	
	// Iterate over each variable in this module
	for(int v = 0; v < module.child_count(); ++v)
	    if(module.child(v).src_type() == SOT_data) {
                SourceObj variable = module.child(v);
		
		// Get the name of this variable
                char buffer[variable.get_variable_name_length() + 1];
                variable.get_variable_name(buffer, sizeof(buffer));
		
		// Return this source object if it is the requested variable
		if(std::string(buffer) == name)
		    return variable;
		
	    }

    }
    
    // Return a default constructed source object if the variable wasn't found
    return SourceObj();    
}



/**
 * Find a library function.
 *
 * Finds the named library function in this process. The library containing
 * this function is loaded into the process first if necessary. A pointer to
 * the library's entry, and a probe expression for the function, are returned.
 *
 * @pre    Libraries are located using libltdl and the standard search path
 *         established by the CollectorPluginTable class. A LibraryNotFound
 *         exception is thrown if the library can't be located.
 *
 * @pre    The function to be executed must be found in the specified library.
 *         A LibraryFuncNotFound exception is thrown if the function cannot be
 *         found within the specified library. 
 *
 * @param collector    Collector requesting the library function.
 * @param name         Name of the library function to be found.
 * @return             Pair containing a pointer to the library's entry and a
 *                     probe expression for the function.
 */
std::pair<Process::LibraryEntry*, ProbeExp>
Process::findLibraryFunction(const Collector& collector,
			     const std::string& name)
{
    GuardWithDPCL guard_myself(this);
    
    // Parse the library function name into separate library and function names
    std::pair<std::string, std::string> parsed = parseLibraryFunctionName(name);
    
    // Check preconditions
    if(parsed.first.empty() || parsed.second.empty())
	throw Exception(Exception::LibraryNotFound, name,
			"library function name could not be parsed");
    
    // Find the entry for this library
    std::map<std::pair<Collector, std::string>, LibraryEntry>::iterator
	i = dm_libraries.find(std::make_pair(collector, parsed.first));
    
    // Does the library need to be loaded into the process for this collector?
    if(i == dm_libraries.end()) {
	
	// Create an entry for this library
	LibraryEntry entry(collector, parsed.first);
	
	// Request the library (module) be loaded into this process
	requestLoadModule(entry);
	
	// Add this entry to the list of libraries if the library was found
	if(!entry.dm_path.empty())
	    i = dm_libraries.insert(std::make_pair(std::make_pair(collector, 
								  parsed.first),
						   entry)).first;
	
    }
    
    // Check preconditions
    if(i == dm_libraries.end()) {
	const char* libltdl_error = lt_dlerror();
	throw Exception(Exception::LibraryNotFound, parsed.first,
			libltdl_error ? libltdl_error : "unknown error");
    }
    
    // Find the function's entry within the library
    std::map<std::string, int>::const_iterator
        j = i->second.dm_functions.find(parsed.second);
    
    // Check preconditions
    if(j == i->second.dm_functions.end())
        throw Exception(Exception::LibraryFuncNotFound,
			parsed.first, parsed.second);

    // Obtain a probe expression reference for the function
    ProbeExp function_exp = i->second.dm_module.get_reference(j->second);

    // Return results to the caller
    return std::make_pair(&(i->second), function_exp);
}



/**
 * Get thread identifiers.
 *
 * Gets the POSIX thread identifier of all the threads within this process. The
 * value is returned as a STL set of thread identifiers. Returns a boolean value
 * indiciating if the thread identifiers were successfully retrieved.
 *
 * @retval value   Current identifiers of all threads within this process.
 * @return         Boolean "true" if the thread identifiers were successfully
 *                 retrieved, "false" otherwise.
 */
bool Process::getPosixThreadIds(std::set<pthread_t>& value) const
{
    GuardWithDPCL guard_myself(this);

    // Ask DPCL for the current list of threads in this process
    AisStatus retval;
#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (dm_perf_data.find(GetThreadsIssued) == dm_perf_data.end()))
	dm_perf_data[GetThreadsIssued] = Time::Now();
#endif	
    ThreadInfoList* list = dm_process->bget_threads(&retval);
#ifndef NDEBUG
    if(is_debug_perf_enabled && 
       (dm_perf_data.find(GetThreadsCompleted) == dm_perf_data.end()))
	dm_perf_data[GetThreadsCompleted] = Time::Now();
    if(is_debug_enabled)
    	debugDPCL("response from bget_threads", retval);
#endif
    if((retval.status() != ASC_success) || (list == NULL))
	return false;

    // Extract and return the thread identifiers if they were retrived
    value.clear();
    for(int i = 0; i < list->get_count(); ++i)
	value.insert(list->get_entry(i).getTID());

    // Indicate to the caller that the value was retrieved
    return true;
}

bool Process::inMPIStartup()
{
    Guard guard_myself(this);
    return in_mpi_startup;
}

void Process::setMPIStartup (bool newstate)
{
    Guard guard_myself(this);
    in_mpi_startup = newstate;
}


} }  // namespace OpenSpeedShop::Framework
