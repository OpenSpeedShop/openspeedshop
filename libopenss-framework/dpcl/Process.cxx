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

#include "AddressSpace.hxx"
#include "Blob.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "Function.hxx"
#include "Guard.hxx"
#include "LinkedObject.hxx"
#include "MainLoop.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "SymbolTable.hxx"
#include "ThreadGroup.hxx"

#include <ltdl.h>
#include <pthread.h>
#include <sstream>
#include <stdlib.h>
#include <vector>



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



#ifndef NDEBUG
/** Flag indicating if debugging for this class is enabled. */
bool Process::is_debug_enabled = false;



/**
 * Display callback debugging information.
 *
 * Displays debugging information for the passed callback, and process unique
 * name, to the standard error stream. Reported information includes the active
 * thread, the name of the callback function being called, and the unique name
 * of the process for which the callback was called.
 *
 * @param callback    String name of the callback (e.g. "attach").
 * @param name        Unique name of the process for which callback is called.
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
 * Display state debugging information.
 *
 * Displays debugging information for the state of this process to the standard
 * error stream. Reported information includes the active thread, the unique
 * name of the process, its current state, a boolean indicating whether it has
 * a pending state change, and its future state.
 */
void Process::debugState() const
{
    Guard guard_myself(this);

    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "State of " << formUniqueName(dm_host, dm_pid) << " is "
	   << toString(dm_current_state);
    if(dm_is_state_changing)
	output << " --> " << toString(dm_future_state);
    output << std::endl;
    
    // Display the string to the standard error stream
    std::cerr << output.str();
}



/**
 * Display request debugging information.
 *
 * Displays debugging information for the passed request, on this process, to
 * the standard error stream. Reported information includes the active thread,
 * the name of the request function being called, and the unique name of the
 * process on which the request was made.
 *
 * @param request    String name of the request (e.g. "Attach").
 */
void Process::debugRequest(const std::string& request) const
{
    Guard guard_myself(this);

    // Build the string to be displayed
    std::stringstream output;
    output << "[TID " << pthread_self() << "] "
	   << "Process::request" << request << "() on "
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
    dm_process(NULL),
    dm_host(host),
    dm_pid(0),
    dm_stdout_callback(stdout_callback),
    dm_stderr_callback(stderr_callback),
    dm_current_state(Thread::Disconnected),
    dm_is_state_changing(false),
    dm_future_state(Thread::Disconnected),
    dm_libraries()
{
#ifndef NDEBUG
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
    MainLoop::suspend();    
    AisStatus retval = dm_process->bcreate(dm_host.c_str(),
					   argv[0], argv, ::environ,
					   stdoutCallback, name, 
					   stderrCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("bcreate", retval);
#endif    
    MainLoop::resume();

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
    
    // Request an attachment to this process
    MainLoop::suspend();
    requestAttach();
    MainLoop::resume();
}



/**
 * Constructor from process attachment.
 *
 * Attaches to an existing process. The process is put into the suspended state
 * as the result of attaching.
 *
 * @param host    Name of the host on which the process resides.
 * @param pid     Process identifier for the process.
 */
Process::Process(const std::string& host, const pid_t& pid) :  
    Lockable(),
    dm_process(NULL),
    dm_host(host),
    dm_pid(pid),
    dm_stdout_callback(OutputCallback(NULL, NULL)),
    dm_stderr_callback(OutputCallback(NULL, NULL)),
    dm_current_state(Thread::Disconnected),
    dm_is_state_changing(false),
    dm_future_state(Thread::Disconnected),
    dm_libraries()
{
#ifndef NDEBUG
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
    
    // Request a connection to this process
    MainLoop::suspend();
    requestConnect();
    MainLoop::resume();
}



/**
 * Destructor.
 *
 * Start the process running if it is currently suspended, disconnect from the
 * process, and destroy our DPCL process handle.
 */
Process::~Process()
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::~Process() for " << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Force the disconnection from this process
    MainLoop::suspend();
    if(dm_current_state == Thread::Suspended)
	dm_process->resume(NULL, NULL);
    dm_process->disconnect(NULL, NULL);   
    MainLoop::resume();
    
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
    // Return the process identifier to the caller
    return dm_pid;
}



/**
 * Get our state.
 *
 * Returns the caller the current state of this process. Since this state
 * changes asynchronously and must be updated across a network, there is a lag
 * between when the actual process' state changes and when that is reflected
 * here.
 *
 * @return    Current state of this process.
 */
Thread::State Process::getState() const
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getState() for " << formUniqueName(dm_host, dm_pid)
	       << " = " << toString(dm_current_state) 
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Return our current state to the caller
    return dm_current_state;
}



/**
 * Change our state.
 *
 * Changes the current state of this process to the passed value. Used to, for
 * example, suspend a process that was previously running. This function does
 * not wait until the process has actually completed the state change, and
 * calling getState() immediately following changeState() will not reflect the
 * new state until the change has actually completed.
 *
 * @note    Only one in-progress state change is allowed per process at any
 *          given time. For example, if you request that a process be suspended,
 *          you cannot request that it be terminated before the suspension is
 *          completed. A StateAlreadyChanging exception is thrown when multiple
 *          in-progress changes are requested.
 *
 * @note    Some transitions are disallowed because they do not make sense or
 *          cannot be implemented. For example, a terminated process cannot be
 *          set to a running process. A StateChangeInvalid exception is thrown
 *          when such an invalid transition is requested.
 *
 * @param state    Change to this state.
 */
void Process::changeState(const Thread::State& state)
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::changeState(" << toString(state) << ") for "
	       << formUniqueName(dm_host, dm_pid) 
	       << std::endl;
	std::cerr << output.str();
	debugState();
    }
#endif

    // Finished if already in the requested state
    if(dm_current_state == state)
	return;
    
    // Finished if already changing to the requested state
    if(dm_is_state_changing && (dm_future_state == state))
	return;
    
    // Disallow multiple, different, in-progress state changes
    if(dm_is_state_changing)
	throw Exception(Exception::StateAlreadyChanging);

    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Handle [ Running | Suspended | Terminated ] --> Disconnected
    if(((dm_current_state == Thread::Running) ||
	(dm_current_state == Thread::Suspended) ||
	(dm_current_state == Thread::Terminated)) &&
       (state == Thread::Disconnected)) {
	
	// Request disconnection from this process
	MainLoop::suspend();
	requestDisconnect();
	MainLoop::resume();
	
    }
    
    // Handle [ Disconnected | Nonexistent ] --> Connecting
    else if(((dm_current_state == Thread::Disconnected) ||
	(dm_current_state == Thread::Nonexistent)) &&
       (state == Thread::Connecting)) {

	// Request connection to this process
	MainLoop::suspend();
	requestConnect();
	MainLoop::resume();
	
    }

    // Handle Suspended --> Running
    else if((dm_current_state == Thread::Suspended) &&
	    (state == Thread::Running)) {

	// Request resumption of this process
	MainLoop::suspend();
	requestResume();
	MainLoop::resume();
	
    }
    
    // Handle Running --> Suspended
    else if((dm_current_state == Thread::Running) &&
	    (state == Thread::Suspended)) {

	// Request suspension of this process
	MainLoop::suspend();
	requestSuspend();
	MainLoop::resume();

    }

    // Handle [ Running | Suspended ] --> Terminated
    else if(((dm_current_state == Thread::Running) ||
	     (dm_current_state == Thread::Suspended)) &&
	    (state == Thread::Terminated)) {

	// Request destruction of this process
	MainLoop::suspend();
	requestDestroy();
	MainLoop::resume();
	
    }
    
    // Otherwise throw an exception because the state change is invalid
    else
	throw Exception(Exception::StateChangeInvalid,
			toString(dm_current_state), toString(state));
}



/**
 * Test if connected.
 *
 * Returns a boolean value indicating if the process is connected or not. Really
 * just a convenience function, which could be built on top of getState(), for
 * when simpler conditional statements are desired.
 *
 * @return    Boolean "true" if this process is connected, "false" otherwise.
 */
bool Process::isConnected() const
{
    Guard guard_myself(this);

    // Return flag indicating if this process is connected to the caller
    return ((dm_current_state != Thread::Disconnected) &&
	    (dm_current_state != Thread::Connecting) &&
	    (dm_current_state != Thread::Nonexistent));
}



/**
 * Execute a library function now.
 *
 * Immediately executes the specified library function in this process. The
 * library is loaded into the process first if necessary.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void Process::executeNow(const Collector& collector, 
			 const Thread& thread,
			 const std::string& callee, 
			 const Blob& argument)
{
    Guard guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeNow(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << callee << "\", ...) for "
	       << formUniqueName(dm_host, dm_pid) 
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
    
    ProbeExp expression = callee_entry.second.call(1, args_exp);
    
    // Request the instrumentation be executed in this process
    MainLoop::suspend();
    ProbeHandle handle;
    requestExecute(expression, NULL, NULL);
    MainLoop::resume();
    
    // Add the empty probe handle to the probes for this thread
    callee_entry.first->dm_probes.insert(std::make_pair(thread, handle));
}



/**
 * Execute a library function at another function's entry/exit.
 *
 * Executes the specified library function every time another function's entry
 * or exit is executed in this process. The library is loaded into the process
 * first if necessary.
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
    Guard guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeAtEntryOrExit(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << where << "\", "
	       << (at_entry ? "Entry" : "Exit") << ", \""
	       << callee << "\", ...) for "
	       << formUniqueName(dm_host, dm_pid) 
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
	    
	    // Request the instrumentation be inserted into this process
	    MainLoop::suspend();
	    ProbeHandle handle = 
		requestInstallAndActivate(expression, point, NULL, NULL);
	    MainLoop::resume();    
	    
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
 * other time that other function is called. The library is loaded into the
 * process first if necessary.
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
    Guard guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::executeInPlaceOf(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ", \"" 
	       << where << "\", \""
	       << callee << "\") for "
	       << formUniqueName(dm_host, dm_pid) 
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
    MainLoop::suspend();
    ProbeExp flag_exp = 
	dm_process->balloc_mem(int32_type(), (void*)(&initial_value), retval);
    MainLoop::resume();
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
	    
	    // Request the instrumentation be inserted into this process
	    MainLoop::suspend();
	    ProbeHandle handle = 
		requestInstallAndActivate(expression, point, NULL, NULL);
	    MainLoop::resume();    
	    
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
    Guard guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::uninstrument(C"
	       << EntrySpy(collector).getEntry()  << ", T"
	       << EntrySpy(thread).getEntry() << ") for "
	       << formUniqueName(dm_host, dm_pid) 
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
		MainLoop::suspend();
		requestDeactivateAndRemove(j->second);
		MainLoop::resume();		
		
	    }

	// Remove all probes associated with the thread from this library
	i->second.dm_probes.erase(thread);

	// Remove all variables associated with the thread from this process
	for(std::multimap<Thread, ProbeExp>::iterator
		j = i->second.dm_variables.lower_bound(thread); 
	    j != i->second.dm_variables.upper_bound(thread); 
	    ++j) {
	    
	    // Request the memory for this variable be freed
	    MainLoop::suspend();
	    requestFree(j->second);
	    MainLoop::resume();	
	    
	}
	
	// Remove all variables associated with the thread from this library
	i->second.dm_probes.erase(thread);

	// Have all the probes & variables for this library been removed?
	if(i->second.dm_probes.empty() && i->second.dm_variables.empty()) {
	    
	    // Request the library (module) be unloaded from this process
	    MainLoop::suspend();
	    requestUnloadModule(i->second);
	    MainLoop::resume();
	    
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
    Guard guard_myself(this);
    
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
    
    // Ask DPCL to execute the probe expression in this process
    MainLoop::suspend();
    AisStatus retval =
	dm_process->bexecute(expression, getIntegerCallback, &value);
    MainLoop::resume();
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif
    
    // Indicate to the caller if the value was retrieved
    return retval.status() == ASC_success;
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
    Guard guard_myself(this);
    
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

    // Get the string from the process
    return getString(variable.reference(), value);
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
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @retval value    Current value of the table.
 * @return          Boolean "true" if the table's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::getMPICHProcTable(Job& value)
{
    Guard guard_myself(this);
    
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

    // Get the value of the "MPIR_proctable_size" variable
    int64_t size = 0;
    if(!getGlobal("MPIR_proctable_size", size))
	return false;

    // Find the "MPIR_proctable" variable
    SourceObj variable = findVariable("MPIR_proctable");
    
    // Go no further if the "MPIR_proctable" global could not be found
    if(variable.src_type() != SOT_data)
	return false;

    // Define types for 32-bit and 64-bit versions of the table

    typedef struct {
	uint32_t host_name;        /* Something we can pass to inet_addr */
	uint32_t executable_name;  /* The name of the image */
	int    pid;                /* The pid of the process */
    } MPIR_PROCDESC_32;
    
    typedef struct {
	uint64_t host_name;        /* Something we can pass to inet_addr */
	uint64_t executable_name;  /* The name of the image */
	int    pid;                /* The pid of the process */
    } MPIR_PROCDESC_64;

    // Determine if this is a 32-bit or 64-bit process    
    SourceObj program = dm_process->get_program_object();
    bool is_64bit = (program.get_program_type() == SOL_lp64);

    // Calculate the number of bytes in the process table
    size_t num_bytes = size * 
	(is_64bit ? sizeof(MPIR_PROCDESC_64) : sizeof(MPIR_PROCDESC_32));

    //
    // Create a probe expression for the code sequence:
    //
    //     Ais_send(Ais_msg_handle, &table, num_bytes)
    //
    
    ProbeExp args_exp[3] = { 
	Ais_msg_handle,
	variable.reference(),
	ProbeExp(static_cast<int32_t>(num_bytes))
    };

    ProbeExp expression = Ais_send.call(3, args_exp);
    
    // Define a pointer to the retrieved process table
    void* buffer = NULL;
    
    // Ask DPCL to execute the probe expression in this process
    MainLoop::suspend();
    AisStatus retval = 
	dm_process->bexecute(expression, getBlobCallback, &buffer);
    MainLoop::resume();
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif
    bool succeeded = (retval.status() == ASC_success);
    
    // Extract the table data if it was retrieved
    if(buffer != NULL) {
    
	// Iterate over each entry in the table    
	for(int i = 0; i < size; ++i)
	    
	    if(is_64bit) {
		
		// Cast the table to the appropriate type
		MPIR_PROCDESC_64* table = 
		    reinterpret_cast<MPIR_PROCDESC_64*>(buffer);
		
		// Get the host name string from the process
		std::string host_name;
		succeeded &= getString(ProbeExp(table[i].host_name), host_name);
		
		// Insert this host/pid pair into the result
		value.insert(std::make_pair(host_name, table[i].pid));
		
	    }
	    else {

		// Cast the table to the appropriate type
		MPIR_PROCDESC_32* table = 
		    reinterpret_cast<MPIR_PROCDESC_32*>(buffer);
		
		// Get the host name string from the process
		std::string host_name;
		succeeded &= getString(ProbeExp(table[i].host_name), host_name);
		
		// Insert this host/pid pair into the result
		value.insert(std::make_pair(host_name, table[i].pid));

	    }

    }

    // Indicate to the caller if the value was retrieved
    return succeeded;
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

    // Note: Since this function is called only once, after everyone has
    //       finished with the symbol table(s), the locking that would usually
    //       be necessary here isn't needed.
    
#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::finishSymbolTableProcessing() for "
	       << state->dm_name
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
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
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(state->dm_name);
    }

    // Only proceed if the process is in the process table
    if(!process.isNull()) {

	// Critical section touching the process
	{
	    Guard guard_process(*process);
	    
	    // Only proceed if process state is changing from "connecting"
	    if(process->dm_is_state_changing &&
	       (process->dm_current_state == Thread::Connecting)) {
		
		// Indicate process' future state is its current state
		process->dm_current_state = process->dm_future_state;
		process->dm_is_state_changing = false;
		
#ifndef NDEBUG
		if(is_debug_enabled)
		    process->debugState();
#endif
	
	    }
	}
	
    }	
    
    // Destroy the heap-allocated state structure
    delete state;    
}



/**
 * Instrumentation activation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * activated in a process. Contains only debugging code for now.
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
 * Process attachment callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * attached. Locates the appropriate process and updates it state.
 */
void Process::attachCallback(GCBSysType, GCBTagType tag, 
			     GCBObjType, GCBMsgType msg)
{
    std::string* name = reinterpret_cast<std::string*>(tag);
    AisStatus* status = reinterpret_cast<AisStatus*>(msg);
    SmartPtr<Process> process;
    ThreadGroup threads;

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

	// Attempt to locate the threads in this process by their unique name
	threads = ProcessTable::TheTable.getThreadsByName(*name);
	
	// Destroy the heap-allocated name string
	delete name;	
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;

    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Did the attach fail?
	if(status->status() != ASC_success) {
	    
	    // Request disconnection from this process
	    process->requestDisconnect();
	    
	}
	else {

	    // Note: In theory it would be possible to complete the process'
	    //       state change at this point. Doing so, however, could allow
	    //       certain other actions (such as starting data collection)
	    //       to proceed before symbol table information was acquired.
	    //       And starting data collection, for example, requires that
	    //       the symbol table information be available. So for now we
	    //       hold off completing the state change until after all the
	    //       symbol table information has been acquired.
	    
	    // Request the current in-memory address space of this process
	    process->requestAddressSpace(threads, Time::Now());
	    
	}
    }
}



/**
 * Process connection callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * connected. Locates the appropriate process and issues an asynchronous attach
 * to that process.
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

	// Destroy the heap-allocated name string
	delete name;
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	

    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Does the specified process not exist?
	if(status->status() == ASC_invalid_pid) {

	    // Indicate process' current state is "nonexistent"
	    process->dm_current_state = Thread::Nonexistent;
	    process->dm_is_state_changing = false;

#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState();
#endif

	}

	// Did the connect otherwise fail?
	else if(status->status() != ASC_success) {

	    // Indicate process' current state is "disconnected"
	    process->dm_current_state = Thread::Disconnected;
	    process->dm_is_state_changing = false;

#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState();
#endif
	    
	}
	else {

	    // Request attachment to this process
	    process->requestAttach();

	}
    }
}



/**
 * Instrumentation deactivation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * deactivated in a process. Contains only debugging code for now.
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
 * Process destruction callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * destroyed. Locates the appropriate process and updates its state.
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
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);

	// Destroy the heap-allocated name string
	delete name;
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	

    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Is the process destroyed?
	if((status->status() == ASC_success) ||
	   (status->status() == ASC_already_destroyed)) {

	    // Indicate process' current state is "terminated"
	    process->dm_current_state = Thread::Terminated;
	    process->dm_is_state_changing = false;
	 
#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState();
#endif
	    
	}
	else {
	    
	    // Request disconnection from this process
	    process->requestDisconnect();
	    
	}
    }
}



/**
 * Process disconnection callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * disconnected. Locates the appropriate process and updates its state.
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

	// Destroy the heap-allocated name string
	delete name;
    }

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Check assertions
	Assert(process->dm_is_state_changing && 
	       (process->dm_future_state == Thread::Disconnected));

	// Indicate process' current state is "disconnected"
	process->dm_current_state = Thread::Disconnected;
	process->dm_is_state_changing = false;

#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState();
#endif

    }
}



/**
 * Instrumentation execution callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * executed in a process. Contains only debugging code for now.
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

    // Obtain the address range occupied by the module
    AddressRange range(static_cast<Address>(module->address_start()),
		       static_cast<Address>(module->address_end()));
    
    // Critical section touching the symbol table state
    bool requests_completed = false;
    {
	Guard guard_state(state);

	// Should this module's functions be stored in a symbol table?
	if(state->dm_symbol_tables.find(range) !=
	   state->dm_symbol_tables.end()) {

            // Locate the appropriate symbol table for this module's statements
	    SymbolTable& symbol_table = 
		state->dm_symbol_tables.find(range)->second.first;

	    // Iterate over each function in this module
	    for(int f = 0; f < module->child_count(); ++f)
		if(module->child(f).src_type() == SOT_function) {
		    SourceObj function = module->child(f);
		    
		    // Get the start/end address of the function
		    Address start = function.address_start();
		    Address end = function.address_end();
		    
		    // Get the demangled name of the function
		    char name[function.get_demangled_name_length() + 1];
		    function.get_demangled_name(name, sizeof(name));
		    
		    // Add this function to the symbol table
		    symbol_table.addFunction(start, end, name);

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
 * to this copy in the specified location.
 */
void Process::getBlobCallback(GCBSysType sys, GCBTagType tag,
			      GCBObjType, GCBMsgType msg)
{
    char** value = reinterpret_cast<char**>(tag);

    // Check assertions
    Assert(value != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::getBlobCallback()"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Store a copy of the character array in the specified location
    if(sys.msg_size > 0) {
	*value = new char[sys.msg_size];
	memcpy(*value, msg, sys.msg_size);
    }
}



/**
 * Get integer callback.
 *
 * Callback function called by the DPCL main loop when an integer is being
 * returned. Stores the integer in the specified location, promoting the integer
 * to 64-bit when necessary.
 *
 * @todo    Problems will occur here if the endianness of the processor running
 *          the instrumented process and the processor running the tool differ.
 *          This is one of only a few places were we aren't dealing with the
 *          endianness issue, so this should eventually be fixed.
 */
void Process::getIntegerCallback(GCBSysType sys, GCBTagType tag,
				 GCBObjType, GCBMsgType msg)
{
    int64_t* value = reinterpret_cast<int64_t*>(tag); 

    // Check assertions
    Assert(value != NULL);
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

    // Store the integer in the specified location
    if(sys.msg_size == sizeof(int8_t))
	*value = static_cast<int64_t>(*reinterpret_cast<int8_t*>(msg));
    else if(sys.msg_size == sizeof(int16_t))
	*value = static_cast<int64_t>(*reinterpret_cast<int16_t*>(msg));    
    else if(sys.msg_size == sizeof(int32_t))
	*value = static_cast<int64_t>(*reinterpret_cast<int32_t*>(msg));
    else
	*value = static_cast<int64_t>(*reinterpret_cast<int64_t*>(msg));
}



/**
 * Instrumentation installation callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * installed in a process. Contains only debugging code for now.
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
    // Enqueue this performance data
    DataQueues::enqueuePerformanceData(Blob(sys.msg_size, msg));
}



/**
 * Instrumentation removal callback.
 *
 * Callback function called by the DPCL main loop when instrumentation has been
 * removed from a process. Contains only debugging code for now.
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
 * Process resumption callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * resumed. Locates the appropriate process and updates its state.
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

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);

	// Destroy the heap-allocated name string
	delete name;
    }
	
    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Check assertions
	Assert(process->dm_is_state_changing && 
	       (process->dm_future_state == Thread::Running));
	
	// Did the resume fail?
	if(status->status() != ASC_success) {
	    
	    // Request disconnection from this process
	    process->requestDisconnect();
	    
	}
	else {
	    
	    // Indicate process' current state is "running"
	    process->dm_current_state = Thread::Running;
	    process->dm_is_state_changing = false;
	    
#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState();
#endif
	    
	}
    }
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
    struct get_stmt_info_list_msg_t* retval =
	reinterpret_cast<struct get_stmt_info_list_msg_t*>(msg);
    
    // Check assertions
    Assert(state != NULL);
    Assert(module != NULL);
    Assert(retval != NULL);

#ifndef NDEBUG
    if(is_debug_enabled) {
	debugCallback("statements", state->dm_name);
    	debugDPCL("response from get_all_statements", retval->status);
    }
#endif
    
    // Obtain the address range occupied by the module
    AddressRange range(static_cast<Address>(module->address_start()),
		       static_cast<Address>(module->address_end()));

    // Critical section touching the symbol table state
    bool requests_completed = false;
    {
	Guard guard_state(state);

	// Should this module's statements be stored in a symbol table?	
	if(state->dm_symbol_tables.find(range) !=
	   state->dm_symbol_tables.end()) {

            // Locate the appropriate symbol table for this module's statements
	    SymbolTable& symbol_table = 
		state->dm_symbol_tables.find(range)->second.first;
	    
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
		    for(int r = 0; (r + 1) < line.get_address_count(); r += 2) {

			// Add this statement to the symbol table
			symbol_table.addStatement(
			    Address(line.get_address_entry(r)),
			    Address(line.get_address_entry(r + 1)),
			    info.get_filename(), 
			    line.get_line(), 
			    line.get_column()
			    );
			
		    }
		    
		}
		
	    }
	
	}

	// Decrement the pending request count
	requests_completed =
            (--state->dm_pending_requests == 0) && state->dm_constructed;
    }

    // Destroy the statement information returned by DPCL
    delete reinterpret_cast<StatementInfoList*>(retval->stmt_info_list_p);
    
    // Finish symbol table processing if all requests have been completed
    if(requests_completed)
	finishSymbolTableProcessing(state);
}



/**
 * Standard error callback.
 *
 * Callback function called by the DPCL main loop when a created process sends
 * data to its standard error stream. Simply redirect the output to the tool's
 * standard error stream.
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
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

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
}



/**
 * Standard out callback.
 *
 * Callback function called by the DPCL main loop when a created process sends
 * data to its standard out stream. Simply redirect the output to the tool's 
 * standard out stream.
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
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

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
}



/**
 * Process suspension callback.
 *
 * Callback function called by the DPCL main loop when a process has been
 * suspended Locates the appropriate process and updates its state.
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

    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Attempt to locate the process by its unique name
	process = ProcessTable::TheTable.getProcessByName(*name);
	
	// Destroy the heap-allocated name string
	delete name;
    }
	
    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Check assertions
	Assert(process->dm_is_state_changing && 
	       (process->dm_future_state == Thread::Suspended));
	
	// Did the suspend fail?
	if(status->status() != ASC_success) {
	    
	    // Request disconnection from this process
	    process->requestDisconnect();
	    
	}
	else {

	    // Indicate process' current state is "suspended"
	    process->dm_current_state = Thread::Suspended;
	    process->dm_is_state_changing = false;
	 
#ifndef NDEBUG
	    if(is_debug_enabled)
		process->debugState();
#endif
	    
	}
    }
}



/**
 * Process termination callback.
 *
 * Callback function called by the DPCL main loop when a process has terminated.
 * Locates the appropriate process and updates its state.
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

    // Go no further if the process is no longer in the process table
    if(process.isNull())
	return;	
    
    // Critical section touching the process
    {
	Guard guard_process(*process);

	// Indicate process' state is "terminated"
	process->dm_current_state = Thread::Terminated;
	process->dm_is_state_changing = false;

#ifndef NDEBUG
	if(is_debug_enabled)
	    process->debugState();
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
 * @todo    DPCL provides some amount of call site information. Processing this
 *          needs to be added once the CallSite object is properly defined.
 *
 * @param threads    Thread(s) within this process to be updated.
 * @param when       Time at which update occured.
 */
void Process::requestAddressSpace(const ThreadGroup& threads, const Time& when)
{    
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)	
	debugRequest("AddressSpace");
#endif
    
    // Address space for this process
    AddressSpace address_space;

    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);

	// Obtain the address range occupied by the module
	AddressRange range(static_cast<Address>(module.address_start()),
			   static_cast<Address>(module.address_end()));

	// Obtain the name of the module
	std::string name = "";
	if(module.module_name_length() > 0) {
	    char buffer[module.module_name_length() + 1];
	    module.module_name(buffer, module.module_name_length() + 1);
	    name = buffer;
	}
	
	// Indicate this address range does not contain the executable
	bool is_executable = false;
	
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
	    
	    // Obtain the name of the program
	    if(program.program_name_length() > 0) {
		char buffer[program.program_name_length() + 1];
		program.program_name(buffer, program.program_name_length() + 1);
		name = buffer;
	    }

	    // Indicate this address range does contain the executable
	    is_executable = true;
	    
	}

	// Create/update the entry for this address range in the address space
	address_space.setValue(range, name, is_executable);
	
    }
    
    // Update the specified threads with this address space
    std::map<AddressRange, std::set<LinkedObject> > needed =
	address_space.updateThreads(threads, when);

    // Allocate and initialize state structure for tracking the requests
    SymbolTableState* state = 
	new SymbolTableState(formUniqueName(dm_host, dm_pid));
    Assert(state != NULL);

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

	    // Obtain the address range occupied by the module
	    AddressRange range(static_cast<Address>(module.address_start()),
			       static_cast<Address>(module.address_end()));

	    // Only expand the module if it doesn't already have children
	    if(module.child_count() == 0) {

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
		AisStatus retval = module.get_all_statements(
		    *dm_process, statementsCallback, state, module
		    );
#ifndef NDEBUG
		if(is_debug_enabled) 
		    debugDPCL("request to get_all_statements", retval);
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)	
	debugRequest("Attach");
#endif
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);
    
    // Ask DPCL to asynchronously attach to this process
    AisStatus retval = dm_process->attach(attachCallback, name);
#ifndef NDEBUG
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

	// Indicate we are "connecting", changing to "suspended"
	dm_current_state = Thread::Connecting;
	dm_is_state_changing = true;
	dm_future_state = Thread::Suspended;

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState();
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Connect");
#endif
    
    // Allocate a copy of our unique name
    std::string* name = new std::string(formUniqueName(dm_host, dm_pid));
    Assert(name != NULL);

    // Ask DPCL to asynchronously connect to this process
    AisStatus retval = dm_process->connect(connectCallback, name);
#ifndef NDEBUG
    if(is_debug_enabled) 
	debugDPCL("request to connect", retval);
#endif
    if(retval.status() != ASC_success) {
	
	// Destroy the heap-allocated unique name string
	delete name;

	// Indicate our current state is "disconnected"
	dm_current_state = Thread::Disconnected;
	dm_is_state_changing = false;
	
    }
    else {

	// Indicate we are "connecting", changing to "suspended"
	dm_current_state = Thread::Connecting;
	dm_is_state_changing = true;
	dm_future_state = Thread::Suspended;
	
    }    

#ifndef NDEBUG
    if(is_debug_enabled)
	debugState();
#endif
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("DeactivateAndRemove");
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Destroy");
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
	
	// Request disconnection from this process
	requestDisconnect();
	
    }
    else {

	// Indicate we are changing state to "terminated"
	dm_is_state_changing = true;
	dm_future_state = Thread::Terminated;

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState();
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Disconnect");
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

	// Indicate our current state is "disconnected"
	dm_current_state = Thread::Disconnected;
	dm_is_state_changing = false;
	dm_future_state = Thread::Disconnected;	
	
    }
    else {

	// Indicate we are changing state to "disconnected"
	dm_is_state_changing = true;
	dm_future_state = Thread::Disconnected;
	
    }

#ifndef NDEBUG
    if(is_debug_enabled)
	debugState();
#endif
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
			     GCBFuncType callback, GCBTagType tag)
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Execute");
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Free");
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
    Guard guard_myself(this);
    
#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("InstallAndActivate");
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::requestLoadModule(C"
	       << EntrySpy(library.dm_collector).getEntry() << ", \"" 
	       << library.dm_name << "\") on " 
	       << formUniqueName(dm_host, dm_pid)
	       << std::endl;
	std::cerr << output.str();
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Resume");
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
	
	// Request disconnection from this process
	requestDisconnect();
	
    }
    else {

	// Indicate we are changing state to "running"
	dm_is_state_changing = true;
	dm_future_state = Thread::Running;

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState();
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled)
	debugRequest("Suspend");
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
	
	// Request disconnection from this process
	requestDisconnect();
	
    }
    else {
	
	// Indicate we are changing state to "suspended"
	dm_is_state_changing = true;
	dm_future_state = Thread::Suspended;

#ifndef NDEBUG
	if(is_debug_enabled)
	    debugState();
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
    Guard guard_myself(this);

#ifndef NDEBUG
    if(is_debug_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Process::requestUnloadModule(C"
	       << EntrySpy(library.dm_collector).getEntry() << ", \""
	       << library.dm_name << "\") on " 
	       << formUniqueName(dm_host, dm_pid)
	       << std::endl;
	std::cerr << output.str();
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
    Guard guard_myself(this);
    
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
	MainLoop::suspend();
	requestLoadModule(entry);
	MainLoop::resume();
	
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
 * Get a string.
 *
 * Gets the current value of a character string within this process. The value
 * is returned as a C++ standard string rather than a C character array. This
 * makes managing the memory associated with the string more obvious. Returns
 * a boolean value indicating if the string's value was successfully retrieved.
 *
 * @param where     Probe expression for the address of the string whose value
 *                  is being requested.
 * @retval value    Current value of that string.
 * @return          Boolean "true" if the string's value was successfully
 *                  retrieved, "false" otherwise.
 */
bool Process::getString(const ProbeExp& where, std::string& value) const
{
    Guard guard_myself(this);
    
    // Find the strlen() function
    SourceObj strlen_func = findFunction("strlen");    

    // Go no further if strlen() could not be found
    if(strlen_func.src_type() != SOT_function)
	return false;

    //
    // Create a probe expression for the code sequence:
    //
    //     if(where)
    //         Ais_send(Ais_msg_handle, where, strlen(where) + 1)
    //     else
    //         Ais_send(Ais_msg_handle, "", 1)
    //

    ProbeExp send_empty_args_exp[3] = {
	Ais_msg_handle,
	ProbeExp(""),
	ProbeExp(1)
    };

    ProbeExp strlen_args_exp[1] = { 
	where
    };

    ProbeExp send_args_exp[3] = {
	Ais_msg_handle, 
	where,
	strlen_func.reference().call(1, strlen_args_exp) + ProbeExp(1)
    };

    ProbeExp expression = where.ifelse(
	Ais_send.call(3, send_args_exp),
	Ais_send.call(3, send_empty_args_exp)
	);
    
    // Define a pointer to the retrieved character array
    char* buffer = NULL;
    
    // Ask DPCL to execute the probe expression in this process
    MainLoop::suspend();
    AisStatus retval = 
	dm_process->bexecute(expression, getBlobCallback, &buffer);
    MainLoop::resume();
#ifndef NDEBUG
    if(is_debug_enabled)
    	debugDPCL("response from bexecute", retval);
#endif

    // Extract and return the string's value if it was retrieved
    if(buffer != NULL) {
	value = std::string(buffer);
	delete [] buffer;	
    }
    
    // Indicate to the caller if the value was retrieved
    return retval.status() == ASC_success; 
}



} }  // namespace OpenSpeedShop::Framework
