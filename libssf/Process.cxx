////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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

#include "Application.hxx"
#include "Assert.hxx"
#include "Guard.hxx"
#include "MainLoop.hxx"
#include "Path.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "SymbolTable.hxx"

#include <dpcl.h>
#include <stdexcept>
#include <vector>



/** TEMPORARY HACK. */
static void stdoutCB(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	fputc(*ptr, stdout);
}



/** TEMPORARY HACK. */
static void stderrCB(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	fputc(*ptr, stdout);
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



namespace {

    /**
     * Table of DPCL process handles.
     *
     * Multiple Process objects can exist for a particular "real" process if the
     * tool uses multiple Application objects. The DPCL client library, however,
     * only allows one of its process objects to do a full "attach" to a real
     * process at any given time. This means the underlying DPCL process object
     * (called a "DPCL process handle" here) must be shareable between multiple
     * instances of our Process object. This table tracks the mapping between
     * our Process objects and the DPCL process handles.
     */
    ProcessTable<Process, ::Process> process_handle_table;



    /**
     * Process termination callback.
     *
     * Callback function called by the DPCL main loop when a process terminates.
     * Marks all the process objects associated with this DPCL process handle as
     * terminated.
     */
    void processTerminationCB(GCBSysType, GCBTagType,
			      GCBObjType, GCBMsgType msg)
    {
	std::string host = strtok((char*)msg, ":");
	pid_t pid = atoi(strtok(NULL, ":"));    
	
	// Look up this host/PID pair to find the process
	Guard guard_process_handle_table(process_handle_table);	
	std::set<Process*> processes =
	    process_handle_table.findReferencesByName(host, pid);
	
	// Mark each of these processes as terminated
	for(std::set<Process*>::iterator 
		i = processes.begin(); i != processes.end(); ++i)
	    (*i)->terminateNow();
    }
    
    
    
    /**
     * State changed callback.
     *
     * Callback function called by the DPCL main loop when acknowledgement of a
     * resume, suspend, or destroy request is received. Completes the state
     * change for the process.
     */
    void stateChangedCB(GCBSysType, GCBTagType tag, GCBObjType, GCBMsgType msg)
    {	
	Process* process = reinterpret_cast<Process*>(tag);
	AisStatus* retval = reinterpret_cast<AisStatus*>(msg);
	
	// Check assertions
	Assert(process != NULL);
	Assert(retval != NULL);
	Assert(retval->status() == ASC_success);
	
	// Complete the state change for this process
	process->completeStateChange();
    }
    
    
    
}



/**
 * Constructor from process creation.
 *
 * Creates a new process to execute the specified command. The command is
 * created with the same initial environment (standard file descriptors,
 * environment variables, etc.) as when the tool was started. The process
 * is created in a suspended state.
 *
 * @param application    Application that will contain the new process.
 * @param host           Name of host on which to execute the command.
 * @param command        Command to be executed.
 */
Process::Process(Application* application,
		 const std::string& host, const std::string& command) :
    dm_application(application),
    dm_process(NULL),
    dm_current_state(Thread::Suspended),
    dm_is_state_changing(false),
    dm_future_state(Thread::Suspended),
    dm_host(host),
    dm_pid(0),
    dm_address_space()
{
    Guard guard_process_handle_table(process_handle_table);
    
    // Check assertions
    Assert(dm_application != NULL);
    
    // Start the DPCL main loop if this is the first DPCL process handle
    if(process_handle_table.empty()) {
	MainLoop::start();
	
	// Setup the process termination handler
	GCBFuncType old_cb_fp;
	GCBTagType old_cb_tag;	
	AisStatus retval = 
	    Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
					  processTerminationCB, NULL,
					  &old_cb_fp, &old_cb_tag);
	Assert(retval.status() == ASC_success);	
    }
    
    // Allocate a new DPCL process handle
    dm_process = new ::Process();
    Assert(dm_process != NULL);
    
    // Extract individual arguments from the command
    std::vector<std::string> args;    
    for(std::string::size_type
	    i = 0, next = 0; i != std::string::npos; i = next) {
	
	// Find the next space character in the command
	next = command.find(' ', i);
	
	// Extract this argument
	args.push_back(
	    command.substr(i, (next == std::string::npos) ? next : next - i)
	    );
	
	// Find the next non-space character in the command
	next = command.find_first_not_of(' ', next);	
	
    }
    
    // TODO: try to find a full pathname for argv[0]???
    
    // Translate the arguments into an argv-style argument list
    const char** argv = new const char*[args.size() + 1];
    for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
	argv[i] = args[i].c_str();
    argv[args.size()] = NULL;
    
    // Declare access to the external environment variables
    extern char** environ;
    
    // Ask DPCL to create and attach a process for executing the command
    MainLoop::suspend();    
    AisStatus retval = dm_process->
	bcreate(dm_host.c_str(), argv[0], argv, ::environ,
		stdoutCB, NULL, stderrCB, NULL);
    
    // TODO: stdout and stderr callbacks above don't appear to be working
    
    Assert(retval.status() == ASC_success);
    retval = dm_process->battach();
    Assert(retval.status() == ASC_success);
    MainLoop::resume();
    
    // Destroy argv-style argument list
    delete [] argv;
    
    // Ask DPCL for the process identifier
    dm_pid = static_cast<pid_t>(dm_process->get_pid());
    
    // Add this DPCL process handle to the process handle table
    process_handle_table.addProcess(dm_host, dm_pid, dm_process);
    
    // Add ourselves to this DPCL process handle's list of references
    process_handle_table.addReference(this, dm_process);

    // Update the current in-memory address space of this process
    updateAddressSpace(Time::Now());
}
    
    
    
/**
 * Constructor from process attachment.
 *
 * Attaches to an existing process. The process is assumed to be in the running
 * state.
 *
 * @param application    Application that will contain the process.
 * @param host           Name of the host on which the process resides.
 * @param pid            Process identifier for the process.
 */
Process::Process(Application* application,
		 const std::string& host, const pid_t& pid) :
    dm_application(application),
    dm_process(NULL),
    dm_current_state(Thread::Running),
    dm_is_state_changing(false),
    dm_future_state(Thread::Running),
    dm_host(host),
    dm_pid(pid),
    dm_address_space()
{
    Guard guard_process_handle_table(process_handle_table);
    
    // Check assertions
    Assert(dm_application != NULL);
    
    // Start the DPCL main loop if this is the first DPCL process handle
    if(process_handle_table.empty()) {
	MainLoop::start();
	
	// Setup the process termination handler
	GCBFuncType old_cb_fp;
	GCBTagType old_cb_tag;	
	AisStatus retval = 
	    Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
					  processTerminationCB, NULL,
					  &old_cb_fp, &old_cb_tag);
	Assert(retval.status() == ASC_success);	
    }
    
    // Re-use the existing DPCL process handle if one is available
    dm_process = process_handle_table.findProcessByName(dm_host, dm_pid);
    if(dm_process == NULL) {
	
	// Allocate a new DPCL process handle
	dm_process = new ::Process(dm_host.c_str(), dm_pid);
	Assert(dm_process != NULL);

	// TODO: explore using asynchronous connect and attach
	
	// Ask DPCL to connect and attach to the process
	MainLoop::suspend();    
	AisStatus retval = dm_process->bconnect();
	Assert(retval.status() == ASC_success);
	retval = dm_process->battach();
	Assert(retval.status() == ASC_success);
	MainLoop::resume();	
	
	// Add this DPCL process handle to the process handle table
	process_handle_table.addProcess(dm_host, dm_pid, dm_process);
	
    }
    
    // Add ourselves to this DPCL process handle's list of references
    process_handle_table.addReference(this, dm_process);  

    // Update the current in-memory address space of this process
    updateAddressSpace(Time::Now());
}
    
    

/**
 * Destructor.
 *
 * Removes ourselves from our DPCL process handle's list of references. The
 * handle is disconnected and destroyed if this was the last process that
 * referenced that handle.
 */
Process::~Process()
{
    Guard guard_process_handle_table(process_handle_table);
    Guard guard_myself(this);

    // We are done if no process handle was ever acquired
    if(dm_process == NULL)
	return;
    
    // Remove ourselves from this DPCL process handle's list of references
    process_handle_table.removeReference(this, dm_process);
    
    // Were we the last process to reference this DPCL process handle?
    if(process_handle_table.findReferencesByProcess(dm_process).empty()) {
	
	// Remove this DPCL process handle from the process handle table
	process_handle_table.removeProcess(dm_host, dm_pid, dm_process);

	// Handle Running --> Disconnected
	if(dm_current_state == Thread::Running) {

	    // Ask DPCL to disconnect from the process
	    MainLoop::suspend();    		
	    AisStatus retval = dm_process->disconnect(NULL, NULL);	
	    Assert(retval.status() == ASC_success);	
	    MainLoop::resume();
	    
	}
	
	// Handle Suspended --> Running
	if(dm_current_state == Thread::Suspended) {
	    
	    // Ask DPCL to resume the process
	    MainLoop::suspend();
	    AisStatus retval = dm_process->resume(NULL, NULL);
	    Assert(retval.status() == ASC_success);
	    MainLoop::resume();
	    
	}
	
	// Destroy the actual DPCL process handle
	delete dm_process;
	
    }

    // Stop the DPCL main loop if this was the last DPCL process handle
    if(process_handle_table.empty())
	MainLoop::stop();

    // Destroy each address space in the address space map
    for(std::map<TimeInterval, const AddressSpace*>::const_iterator
	    i = dm_address_space.begin(); i != dm_address_space.end(); ++i)
	delete i->second;
}



/**
 * Get our state.
 *
 * Returns the caller the current state of the process. Since this state
 * changes asynchronously and must be updated across a network, there is
 * a lag between when the actual process' state changes and when that is
 * reflected here.
 *
 * @return    Current state of this process.
 */
Thread::State Process::getState() const
{
    Guard guard_myself(this);
    
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
 * @note    Some transitions are disallowed because they do not make sense or
 *          cannot be implemented. For example, a terminated process cannot be
 *          set to a running process. An exception of type std::logic_error is
 *          thrown when such an invalid transition is requested.
 *
 * @note    Only one in-progress state change is allowed per process at any
 *          given time. For example, if you request that a process be suspended,
 *          you cannot request that it be terminated before the suspension is
 *          completed. An exception of type std::logic_error is thrown when
 *          multiple in-progress changes are requested.
 *
 * @param state    Change to this state.
 */
void Process::changeState(const Thread::State& state)
{
    Guard guard_myself(this);
    
    // Finished if already in the requested state
    if(dm_current_state == state)
	return;
    
    // Finished if already changing to the requested state
    if(dm_is_state_changing && (dm_future_state == state))
	return;
    
    // Disallow multiple, different, in-progress state changes
    if(dm_is_state_changing)
	throw std::logic_error(
	    "Cannot change a process' state when a different state change "
	    "is in-progress but hasn't completed.");
    
    // Disallow Terminated --> [ Running | Suspended ]
    if(dm_current_state == Thread::Terminated)
	throw std::logic_error(
	    "Cannot " + 
	    std::string(((state == Thread::Suspended) ? "suspend" : "run")) + 
	    " a terminated process.");
    
    // Check assertions
    Assert(dm_process != NULL);
    
    // Handle Suspended --> Running
    if((dm_current_state == Thread::Suspended) && (state == Thread::Running)) {

	// Ask DPCL to resume the process
	MainLoop::suspend();
	AisStatus retval = dm_process->resume(stateChangedCB, this);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Indicate this thread is changing to the running state
	dm_is_state_changing = true;
	dm_future_state = Thread::Running;
	
    }
    
    // Handle Running --> Suspended
    if((dm_current_state == Thread::Running) && (state == Thread::Suspended)) {

	// Ask DPCL to suspend the process
	MainLoop::suspend();
	AisStatus retval = dm_process->suspend(stateChangedCB, this);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Indicate this thread is changing to the suspended state
	dm_is_state_changing = true;
	dm_future_state = Thread::Suspended;	
	
    }
    
    // Handle [Suspended | Running ] --> Terminated
    if(state == Thread::Terminated) {
	
	// Ask DPCL to destroy the process
	MainLoop::suspend();
	AisStatus retval = dm_process->destroy(stateChangedCB, this);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Indicate this thread is changing to the terminated state
	dm_is_state_changing = true;
	dm_future_state = Thread::Terminated;
	
    }
}
    


/**
 * Get our host name.
 *
 * Returns to the caller the name of the host on which the process is
 * located. This information may be obtained directly from the process
 * or cached internally.
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
 * Returns to the caller the identifier of the process containing the
 * thread. This information may be obtained directly from the thread or
 * cached internally.
 *
 * @return    Identifier of process containing this thread.
 */
pid_t Process::getProcessId() const
{
    Guard guard_myself(this);
    
    // Return the process identifier to the caller
    return dm_pid;
}



/**
 * Find the address space.
 *
 * Finds and returns the address space of this process at the specified time.
 *
 * @param time    Query time.
 * @return        Address space at that time.
 */    
const AddressSpace* Process::findAddressSpace(const Time& time) const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_address_space.size() > 0);
    
    // Find the address space at the query time
    std::map<TimeInterval, const AddressSpace*>::const_iterator
	i = dm_address_space.find(time);
    Assert(i != dm_address_space.end());
    
    // Return the address space to the caller
    return i->second;     
}



/**
 * Complete a state change.
 *
 * Called by stateChangedCB() when the process completes its in-progress
 * state change. Simply sets the current state to the future state and marks
 * the process as no longer undergoing a state change.
 */
void Process::completeStateChange()
{
    Guard guard_myself(this);
    
    // Check assertions
    Assert(dm_is_state_changing);

    // Set the current state equal to the future state
    dm_current_state = dm_future_state;
    dm_is_state_changing = false;
}



/**
 * Terminate now.
 *
 * Called by processTerminationCB() when the process terminates. Simply sets the
 * current state and future state to terminated.
 */
void Process::terminateNow()
{
    Guard guard_myself(this);

    // Set the current and future state to terminated
    dm_current_state = Thread::Terminated;
    dm_future_state = Thread::Terminated;
}



/**
 * Update our address space.
 *
 * Called when the process is first started or attached, and any time a shared
 * library is loaded or unloaded. Requests the list of modules from DPCL and
 * uses that list to construct a description of the process' address space. A
 * symbol table is created (or an existing one reused) for each linked object
 * that is located. The resulting address space is then added to our internal
 * map of address spaces as a function of time.
 *
 * @param time    Time at which update occured.
 */
void Process::updateAddressSpace(const Time& time)
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_application != NULL);
    Assert(dm_process != NULL);

    // Define a type pairing a linked object name with its source object list
    typedef std::pair<Path, std::vector<SourceObj> > table_entry_t;    
    
    // Define a type mapping address ranges to this pairing
    typedef std::map<AddressRange, table_entry_t> table_t;
    
    // Table mapping address ranges to linked object names and source objects
    table_t objects;
    
    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);
	
	// Obtain the name of the module
	std::string module_name = "";
	if(module.module_name_length() > 0) {
	    char buffer[module.module_name_length() + 1];
	    module.module_name(buffer, module.module_name_length() + 1);
	    module_name = buffer;
	}
	
	// Obtain the start and end addresses of the module
	AddressRange range(static_cast<Address>(module.address_start()),
			   static_cast<Address>(module.address_end()));

	// Look for an entry in the table for this address range
	table_t::iterator i = objects.find(range);
	
	// Create an entry if one wasn't found
	if(i == objects.end())
	    i = objects.insert(
		std::make_pair(range,
			       std::make_pair(Path(module_name),
					      std::vector<SourceObj>()))
		).first;
	
	//
	// Otherwise if an entry was found, assume that we are dealing with the
	// executable, which can contain multiple DPCL modules. Use the name of
	// the executable as the linked object name if this is the second module
	// to be added to the entry.
	//
	else if(i->second.second.size() == 1) {

	    // Obtain the name of the program
	    std::string program_name = "";
	    if(program.program_name_length() > 0) {
		char buffer[program.program_name_length() + 1];
		program.program_name(buffer, program.program_name_length() + 1);
		program_name = buffer;
	    }
	    
	    // Use the program name as this entry's linked object name
	    i->second.first = Path(program_name);
	    
	}
	
	// Add the source object to this entry
	i->second.second.push_back(module);
	
    }

    // New address space that is constructed for the current time period
    AddressSpace* address_space = new AddressSpace();
    
    // Iterate over each linked object entry in the table
    for(table_t::iterator i = objects.begin(); i != objects.end(); ++i) {
	
	// Is there an existing symbol table for this linked object?
	const SymbolTable* symbol_table = dm_application->
	    findSymbolTable(i->second.first);
	
	// Create a symbol table if there isn't one already
	if(symbol_table == NULL) {

	    // Iterate over each source object in this linked object
	    for(std::vector<SourceObj>::iterator j = i->second.second.begin();
		j != i->second.second.end();
		++j) {
		
		// Ask DPCL to expand this (module) source object
		MainLoop::suspend();
		AisStatus retval = j->bexpand(*dm_process);
		Assert(retval.status() == ASC_success);
		MainLoop::resume();
		
	    }
	    
	    // Create the new symbol table
	    symbol_table = new SymbolTable(dm_application, i->second.first,
					   i->first, i->second.second);
	    
	    // Add the new symbol table to the application
	    dm_application->addSymbolTable(symbol_table);
	    
	}
	
	// Add the symbol table (new or existing) to the address space
	address_space->addSymbolTable(i->first, symbol_table);
	
    }
    
    // Is this the first address space for this process?
    if(dm_address_space.empty())
	dm_address_space.insert(
	    std::make_pair(
		TimeInterval(Time::TheBeginning(), Time::TheEnd()),
		address_space));
    
    // Otherwise add this address space to the existing map for this process
    else {
	
	// Find our most recent address space
	std::map<TimeInterval, const AddressSpace*>::iterator i =
	    dm_address_space.end();
	
	// Reinsert this address space with an updated time interval
	dm_address_space.insert(
	    std::make_pair(TimeInterval(i->first.getBegin(), time),
			   i->second));
	
	// Erase the old entry
	dm_address_space.erase(i);
	
	// Add the newly construct address space
	dm_address_space.insert(
	    std::make_pair(TimeInterval(time, Time::TheEnd()),
			   address_space));
	
    }

}



} }  // OpenSpeedShop::Framework
