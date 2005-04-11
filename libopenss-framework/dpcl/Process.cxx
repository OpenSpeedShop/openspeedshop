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

#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "ExperimentTable.hxx"
#include "Guard.hxx"
#include "MainLoop.hxx"
#include "Path.hxx"
#include "Process.hxx"
#include "ProcessTable.hxx"
#include "SmartPtr.hxx"
#include "StatementBuilder.hxx"
#include "Time.hxx"

#include <dpcl.h>
#include <ltdl.h>
#include <sstream>



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
     * Performance data callback.
     *
     * Callback function called by the DPCL main loop when performance data is
     * sent to the tool. Simply passes this data on to the proper function to be
     * stored in an experiment database.
     */
    void performanceDataCB(GCBSysType sys, GCBTagType,
			   GCBObjType, GCBMsgType msg)
    {
	// Store this performance data in the correct experiment database
	ExperimentTable::TheTable.storePerformanceData(Blob(sys.msg_size, msg));
    }
    


    /**
     * Process termination callback.
     *
     * Callback function called by the DPCL main loop when a process terminates.
     * Marks the process as terminated.
     */
    void processTerminationCB(GCBSysType, GCBTagType,
			      GCBObjType, GCBMsgType msg)
    {
	std::string name = std::string(reinterpret_cast<char*>(msg));
	SmartPtr<Process> process;

	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Get the process (if any) by its unique name
	    process = ProcessTable::TheTable.getProcessByName(name);
	}
	
	// Go no further if the process no longer exists
	if(process.isNull())
	    return;
	
	// Mark the process as terminated
	process->terminateNow();
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
	std::string name = std::string(reinterpret_cast<char*>(tag));
	AisStatus* retval = reinterpret_cast<AisStatus*>(msg);
	SmartPtr<Process> process;
	
	// Check assertions
	Assert(retval != NULL);
	Assert(retval->status() == ASC_success);

	// Critical section touching the process table
	{
	    Guard guard_process_table(ProcessTable::TheTable);
	    
	    // Get the process (if any) by its unique name
	    process = ProcessTable::TheTable.getProcessByName(name);
	}
	
	// Go no further if the process no longer exists
	if(process.isNull())
	    return;
	
	// Complete the state change for the process
	process->completeStateChange();
    }



    /**
     * Standard error callback.
     *
     * Callback function called by the DPCL main loop when a created process
     * sends data to its standard error stream. Simply redirect the output to
     * the tool's standard error stream.
     */
    void stderrCB(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
    {
	char* ptr = (char*)msg;
	for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	    fputc(*ptr, stderr);
	fflush(stderr);
    }
    


    /**
     * Standard out callback.
     *
     * Callback function called by the DPCL main loop when a created process
     * sends data to its standard out stream. Simply redirect the output to the
     * tool's standard out stream.
     */
    void stdoutCB(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
    {
	char* ptr = (char*)msg;
	for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	    fputc(*ptr, stdout);
	fflush(stdout);
    }



}



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
 * created with the same initial environment (standard file descriptors,
 * environment variables, etc.) as when the tool was started. The process
 * is created in a suspended state.
 *
 * @note    A CommandNotFound exception is thrown if the thread cannot be
 *          created for any reason (host doesn't exist, specified command
 *          couldn't be found, etc.)
 *
 * @todo    Should probably try to find a full pathname for argv[0].
 *
 * @param host       Name of host on which to execute the command.
 * @param command    Command to be executed.
 */
Process::Process(const std::string& host, const std::string& command) :
    dm_process(NULL),
    dm_host(host),
    dm_pid(0),
    dm_current_state(Thread::Suspended),
    dm_is_state_changing(false),
    dm_future_state(Thread::Suspended),
    dm_library_name_to_entry()
{
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Start the DPCL main loop if this is the first process
	if(ProcessTable::TheTable.isEmpty()) {
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
    
    // Translate the arguments into an argv-style argument list
    const char** argv = new const char*[args.size() + 1];
    for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
	argv[i] = args[i].c_str();
    argv[args.size()] = NULL;
    
    // Declare access to the external environment variables
    extern char** environ;
    
    // Ask DPCL to create and attach a process for executing the command
    MainLoop::suspend();    
    AisStatus retval = dm_process->bcreate(dm_host.c_str(),
					   argv[0], argv, ::environ,
					   stdoutCB, NULL, stderrCB, NULL);
    if(retval.status() == ASC_failure) {
	MainLoop::resume();
	throw Exception(Exception::CommandNotFound, host, command);
    }
    Assert(retval.status() == ASC_success);
    retval = dm_process->battach();
    Assert(retval.status() == ASC_success);
    MainLoop::resume();
    
    // Destroy argv-style argument list
    delete [] argv;

    // Ask DPCL for the process identifier
    dm_pid = static_cast<pid_t>(dm_process->get_pid());
}
    
    
    
/**
 * Constructor from process attachment.
 *
 * Attaches to an existing process. The process is assumed to be in the running
 * state.
 *
 * @note    A ThreadUnavailable exception is thrown if the thread cannot be
 *          attached for any reason (host or process identifier doesn't exist,
 *          etc.)
 * 
 * @todo    Explore using asynchronous connect/attach.
 *
 * @param host    Name of the host on which the process resides.
 * @param pid     Process identifier for the process.
 */
Process::Process(const std::string& host, const pid_t& pid) :
    dm_process(NULL),
    dm_host(host),
    dm_pid(pid),
    dm_current_state(Thread::Running),
    dm_is_state_changing(false),
    dm_future_state(Thread::Running),
    dm_library_name_to_entry()
{
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);

	// Start the DPCL main loop if this is the first process
	if(ProcessTable::TheTable.isEmpty()) {
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
    }

    // Allocate a new DPCL process handle
    dm_process = new ::Process(dm_host.c_str(), dm_pid);
    Assert(dm_process != NULL);

    // NOTE: It appears that either ASC_invalid_pid or ASC_unknown_status are
    //       returned when the specified process doesn't exist. The former makes
    //       sense, but the later not so much. It appears to be caused when the
    //       the communication daemon sends a response to the client library
    //       that doesn't contain the properly formatted response data. Why
    //       would THAT occur?

    // Ask DPCL to connect and attach to the process
    MainLoop::suspend();    
    AisStatus retval = dm_process->bconnect();    
    if((retval.status() == ASC_invalid_pid) ||
       (retval.status() == ASC_unknown_status)) {
	MainLoop::resume();
	throw Exception(Exception::ThreadUnavailable,
			host, Exception::toString(pid));
    }
    Assert(retval.status() == ASC_success);
    retval = dm_process->battach();
    Assert(retval.status() == ASC_success);
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

    // TODO: unload all libraries?

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
	
	// Ask DPCL to disconnect from the process
	MainLoop::suspend();    		
	retval = dm_process->disconnect(NULL, NULL);	
	Assert(retval.status() == ASC_success);	
	MainLoop::resume();
	
    }

    // Destroy the actual DPCL process handle
    delete dm_process;
    
    // Critical section touching the process table
    {
	Guard guard_process_table(ProcessTable::TheTable);
	
	// Stop the DPCL main loop if this was the last process
	if(ProcessTable::TheTable.isEmpty())
	    MainLoop::stop();
    }
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

    // Finished if already in the requested state
    if(dm_current_state == state)
	return;
    
    // Finished if already changing to the requested state
    if(dm_is_state_changing && (dm_future_state == state))
	return;
    
    // Disallow multiple, different, in-progress state changes
    if(dm_is_state_changing)
	throw Exception(Exception::StateAlreadyChanging);
    
    // Disallow Terminated --> [ Running | Suspended ]
    if(dm_current_state == Thread::Terminated)
	throw Exception(Exception::StateChangeInvalid, "terminated",
			(state == Thread::Suspended) ? "suspend" : "run");
    
    // Cast the process' name to the proper type for use below
    std::string name = formUniqueName(dm_host, dm_pid);
    void* name_ptr =
	const_cast<void*>(reinterpret_cast<const void*>(name.c_str()));
    
    // Handle Suspended --> Running
    if((dm_current_state == Thread::Suspended) && (state == Thread::Running)) {

	// Ask DPCL to resume the process
	MainLoop::suspend();
	AisStatus retval = dm_process->resume(stateChangedCB, name_ptr);
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
	AisStatus retval = dm_process->suspend(stateChangedCB, name_ptr);
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
	AisStatus retval = dm_process->destroy(stateChangedCB, name_ptr);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Indicate this thread is changing to the terminated state
	dm_is_state_changing = true;
	dm_future_state = Thread::Terminated;
	
    }
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
 * Update address space.
 *
 * Called when a thread within this process is first created or attached, and
 * any time a shared library is loaded or unloaded.  Requests the module list
 * from DPCL and uses that list to update the database containing the passed
 * thread with the current in-memory address space of that thread. Symbol
 * information is created (or existing information reused) for each linked
 * object that is located.
 *
 * @todo    DPCL provides some amount of call site information. Processing this
 *          needs to be added once the CallSite object is properly defined.
 *
 * @todo    Currently the criteria for reusing symbol information from an
 *          existing linked object is if the path names of the linked objects
 *          are identical. This is inadequate since two hosts might have linked
 *          objects with the same name but different contents. For example, two
 *          different versions of "/lib/ld-linux.so.2". In order to address this
 *          shortcoming, a checksum (or similar) should also be compared.
 *
 * @todo    Since multiple calls to the processXYZ() functions can take a good
 *          amount of time, parallelizing those calls represents an opportunity
 *          to improve performance.
 *
 * @param thread    Thread within this process to be updated.
 * @param when      Time at which update occured.
 */
void Process::updateAddressSpace(const Thread& thread, const Time& when)
{
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

    // Begin a transaction on this thread's database
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_TRANSACTION(database);
    
    //
    // Use Time::TheBeginning() as the "time_begin" value of the to-be-created
    // address space entries if there are currently no address space entries
    // for this thread. Otherwise set "time_begin" to be "when".
    //
    Time time_begin = Time::TheBeginning();
    database->prepareStatement(
	"SELECT COUNT(*) FROM AddressSpaces WHERE thread = ?;"
	);
    database->bindArgument(1, EntrySpy(thread).getEntry());
    while(database->executeStatement())
	if(database->getResultAsInteger(1) > 0)
	    time_begin = when;
    
    // Update time interval of active address space entries for this thread
    database->prepareStatement(
	"UPDATE AddressSpaces SET time_end = ?"
	" WHERE thread = ? AND time_end = ?;"
	);
    database->bindArgument(1, when);
    database->bindArgument(2, EntrySpy(thread).getEntry());
    database->bindArgument(3, Time::TheEnd());
    while(database->executeStatement());
    
    // Iterate over each linked object entry in the table
    for(table_t::iterator i = objects.begin(); i != objects.end(); ++i) {

	// Is there an existing linked object in the database?
	int linked_object = -1;
	database->prepareStatement(
	    "SELECT LinkedObjects.id FROM LinkedObjects JOIN Files"
	    " ON LinkedObjects.file = Files.id WHERE Files.path = ?;"
	    );
	database->bindArgument(1, i->second.first);
	while(database->executeStatement())
	    linked_object = database->getResultAsInteger(1);
	
	// Parse symbol information if it isn't present in the database
	if(linked_object == -1) {
	    
	    // Create the file entry
	    database->prepareStatement("INSERT INTO Files (path) VALUES (?);");
	    database->bindArgument(1, i->second.first);
	    while(database->executeStatement());
	    int file = database->getLastInsertedUID();
	    
	    // Create the linked object entry
	    database->prepareStatement(
		"INSERT INTO LinkedObjects (addr_begin, addr_end, file)"
		" VALUES (0, ?, ?);"
		);
	    database->bindArgument(1, Address(i->first.getEnd() - 
					      i->first.getBegin()));
	    database->bindArgument(2, file);	    
	    while(database->executeStatement());
	    linked_object = database->getLastInsertedUID();

	    // Process the function information for this module
	    processFunctions(thread, linked_object,
			     i->first, i->second.second);
	    
	    // Process the statement information for this module
	    processStatements(thread, linked_object,
			      i->first, i->second.second);
	    
	}
	
	// Create an address space entry for this linked object
	database->prepareStatement(
	    "INSERT INTO AddressSpaces (thread, time_begin, time_end,"
	    " addr_begin, addr_end, linked_object)"
	    "VALUES (?, ?, ?, ?, ?, ?);"
	    );
	database->bindArgument(1, EntrySpy(thread).getEntry());
	database->bindArgument(2, time_begin);
	database->bindArgument(3, Time::TheEnd());
	database->bindArgument(4, i->first.getBegin());
	database->bindArgument(5, i->first.getEnd());
	database->bindArgument(6, linked_object);
	while(database->executeStatement());	
	
    }
    
    // End the transaction on this thread's database
    END_TRANSACTION(database);    
}



/**
 * Process function information.
 *
 * Process function information for the specified source objects. DPCL
 * is queried for each function's address range and demangled name. This
 * information is filled into the database for the specified thread.
 *
 * @param thread           Thread that contains the linked object.
 * @param linked_object    Entry identifier for the linked object.
 * @param range            Address range occupied.
 * @param objects          List of source objects in this linked object.
 */
void Process::processFunctions(const Thread& thread,
			       const int& linked_object,
			       const AddressRange& range,
			       std::vector<SourceObj>& objects) const
{
    // Begin a transaction on this thread's database
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_TRANSACTION(database);
    
    // Iterate over each source object in this linked object
    for(std::vector<SourceObj>::iterator
	    o = objects.begin(); o != objects.end(); ++o) {
	
	// Ask DPCL to expand this (module) source object
	MainLoop::suspend();
	AisStatus retval = o->bexpand(*dm_process);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Iterate over each function in this source object
	for(int f = 0; f < o->child_count(); ++f)
	    if(o->child(f).src_type() == SOT_function) {
		SourceObj function = o->child(f);

		// Get the start/end address of the function
		Address start = function.address_start();
		Address end = function.address_end();


		// Note: Various conditions exist under which "garbage" appears
		//       in the function list. For example, there are a bunch of
		//       functions in "libc.so.6" of the form "__*_nocancel"
		//       that are zero length (their start and end addresses are
		//       the same). The following sanity checks help keep these
		//       unusable function entries out of the symbol database.

		// Discard any functions where (end <= start)
		if(end <= start)
		    continue;

		// Discard functions not contained in this linked object
		if(!range.doesContain(start) || !range.doesContain(end))
		    continue;
		

		// Convert the start/end addresses to linked object offsets
		start = start - range.getBegin();
		end = end - range.getBegin();
		
		// Get the demangled name of the function
		char name[function.get_demangled_name_length() + 1];
		function.get_demangled_name(name, sizeof(name));
		
		// Create the function entry
		database->prepareStatement(
		    "INSERT INTO Functions"
		    " (linked_object, addr_begin, addr_end, name)"
		    " VALUES (?, ?, ?, ?);"
		    );
		database->bindArgument(1, linked_object);
		database->bindArgument(2, start);
		database->bindArgument(3, end);
		database->bindArgument(4, name);
		while(database->executeStatement());	
		
	    }
	
    }    
    
    // End the transaction on this thread's database
    END_TRANSACTION(database);
}



/**
 * Process statement information.
 *
 * Process statement information for the specified source objects. DPCL is
 * queried for each statement's source file, line number, column number, and
 * list of addresses. This information is passed to a statement builder that
 * processes the data into a form that is then stored into the database for
 * the specified thread.
 *
 * @param thread           Thread that contains the linked object.
 * @param linked_object    Entry identifier for the linked object.
 * @param range            Address range occupied.
 * @param objects          List of source objects in this linked object.
 */
void Process::processStatements(const Thread& thread,
				const int& linked_object,
				const AddressRange& range,
				std::vector<SourceObj>& objects) const
{
    // Allocate a statement builder for this linked object
    StatementBuilder builder;
    
    // Iterate over each source object in this linked object
    for(std::vector<SourceObj>::iterator
	    o = objects.begin(); o != objects.end(); ++o) {

	// Ask DPCL for all the statements in this (module) source object
	MainLoop::suspend();
	AisStatus retval;
	StatementInfoList* statements =
	    o->bget_all_statements(*dm_process, &retval);
	Assert(statements != NULL);
	Assert(retval.status() == ASC_success);
	MainLoop::resume();
	
	// Iterate over each source file in this source object
	for(int f = 0; f < statements->get_count(); ++f) {
	    StatementInfo info = statements->get_entry(f);
	    
	    // Iterate over each statement in this source file
	    for(int s = 0; s < info.get_line_count(); ++s) {
		StatementInfoLine line = info.get_line_entry(s);
		
		// Iterate over each address in this statement
		for(int a = 0; a < line.get_address_count(); ++a) {

		    // Get the address
		    Address addr = line.get_address_entry(a);


		    // Note: Various conditions exist under which "garbage"
		    //       appears in the statement list. For example, several
		    //       versions of GCC produced DWARF line information
		    //       with small offsets rather than addresses under
		    //       some circumstances. The following sanity checks
		    //       help keep these unusable statement entries out of
		    //       the symbol database.

		    // Discard statements not contained in this linked object
		    if(!range.doesContain(addr))
			continue;


		    // Convert the address to a linked object offset
		    addr = addr - range.getBegin();
		    
		    // Add this entry to the builder
		    builder.addEntry(info.get_filename(), line.get_line(),
				     line.get_column(), addr);
		    
		}
		
	    }
	    
	}
	
	// Destroy the statement information returned by DPCL
	delete statements;   

    }
    
    // Process and store this linked object's statements
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    builder.processAndStore(database, linked_object);
}



/**
 * Load a library.
 *
 * Loads the specified library into this process. Each process maintains a list
 * of loaded libraries and their reference counts. If the specified library is
 * already loaded, its reference count is simply incremented. Otherwise the
 * library is located and loaded into the process.
 *
 * @note    Libraries are located using libltdl and the standard search path
 *          established by the CollectorPluginTable class. A LibraryNotFound
 *          exception is thrown if the library can't be located.
 *
 * @param library    Name of library to be loaded.
 */
void Process::loadLibrary(const std::string& library)
{
    Guard guard_myself(this);

    // Find the entry for this library
    std::map<std::string, LibraryEntry>::iterator
	i = dm_library_name_to_entry.find(library);

    // Simply increment the library's reference count if its already loaded
    if(i != dm_library_name_to_entry.end()) {	
	i->second.references++;
	return;
    }
    
    // Can we open this library as a libltdl module?
    lt_dlhandle handle = lt_dlopenext(library.c_str());
    if(handle == NULL)
	throw Exception(Exception::LibraryNotFound, library);
    
    // Get the full path of the library
    const lt_dlinfo* info = lt_dlgetinfo(handle);
    Assert(info != NULL);
    Path full_path = info->filename;
    
    // Close the module handle
    Assert(lt_dlclose(handle) == 0);
    
    // Ask DPCL to load the library
    ProbeModule* module = new ProbeModule(full_path.c_str());
    MainLoop::suspend();
    AisStatus retval = dm_process->bload_module(module);
    Assert(retval.status() == ASC_success);
    MainLoop::resume();	
    
    // Create an entry for this library
    LibraryEntry entry;
    entry.name = library;
    entry.path = full_path;
    entry.module = module;
    entry.references = 1;
    entry.functions = std::map<std::string, int>();
    entry.messaging = NULL;
    
    // Iterate over each function in this library
    for(int i = 0; i < module->get_count(); ++i) {
	
	// Obtain the function's name
	unsigned length = module->get_name_length(i);
	char* buffer = new char[length];
	module->get_name(i, buffer, length);
	
	// Add this function to the list of functions for this library
	entry.functions.insert(std::make_pair(buffer, i));
	
	// Destroy the allocate function name
	delete [] buffer;
	
    }
    
    // Initialize messaging for this library
    initializeMessaging(entry);
    
    // Add this entry to the list of libraries
    dm_library_name_to_entry.insert(std::make_pair(library, entry));    
}
    
    

/**
 * Unload a library.
 *
 * Unloads the specified library from this process. Each process maintains a
 * list of loaded libraries and their reference counts. If the specified library
 * has more than one reference, its reference count is simply decremeneted.
 * Otherwise the library is actually unloaded from the process.
 *
 * @pre    A library must be loaded before it can be unloaded. An assertion
 *         failure occurs if the library is not already loaded into the process.
 *
 * @param library    Name of library to be unloaded.
 */
void Process::unloadLibrary(const std::string& library)
{
    Guard guard_myself(this);

    // Find the entry for this library
    std::map<std::string, LibraryEntry>::iterator
	i = dm_library_name_to_entry.find(library);

    // Check preconditions
    Assert(i != dm_library_name_to_entry.end());
    
    // Decrement the library's reference count
    i->second.references--;

    // Return if there are still references to this library
    if(i->second.references > 0)
	return;
    
    // Finalize messaging for this library
    finalizeMessaging(i->second);
    
    // Ask DPCL to unload the library
    MainLoop::suspend();
    AisStatus retval = dm_process->bunload_module(i->second.module);
    Assert(retval.status() == ASC_success);
    MainLoop::resume();
    delete i->second.module;
    
    // Remove this entry from the list of libraries
    dm_library_name_to_entry.erase(i);    
}

    

/**
 * Execute a library function.
 *
 * Immediately executes the specified function in this process.
 *
 * @pre    A library must be loaded before a function within it can be executed.
 *         An assertion failure occurs if the library is not already loaded into
 *         the process.
 *
 * @pre    The function to be executed must be found in the specified library.
 *         A LibraryFuncNotFound exception is thrown if the function cannot be
 *         found within the specified library.
 *
 * @param library     Name of library containing function to be executed.
 * @param function    Name of function to be executed.
 * @param argument    Blob argument to the function.
 */
void Process::execute(const std::string& library,
		      const std::string& function,
		      const Blob& argument)
{    
    Guard guard_myself(this);

    // Find the library's entry
    std::map<std::string, LibraryEntry>::const_iterator
	i = dm_library_name_to_entry.find(library);
    
    // Check preconditions
    Assert(i != dm_library_name_to_entry.end());

    // Find the function's entry within the library
    std::map<std::string, int>::const_iterator
	j = i->second.functions.find(function);
    
    // Check preconditions
    if(j == i->second.functions.end())
	throw Exception(Exception::LibraryFuncNotFound, library, function);
    
    // Obtain a probe expression reference for the function
    ProbeExp function_exp = i->second.module->get_reference(j->second);

    // Create a probe experssion for the argument (first encoded as a string)
    ProbeExp args_exp[1] = { ProbeExp(argument.getStringEncoding().c_str()) };
    
    // Create a probe expression for the function call
    ProbeExp call_exp = function_exp.call(1, args_exp);
    
    // Ask DPCL to execute the function in this process    
    MainLoop::suspend();
    AisStatus retval = dm_process->bexecute(call_exp, NULL, NULL);
    Assert(retval.status() == ASC_success);
    MainLoop::resume();    
}



/**
 * Initialize messaging.
 *
 * Initialize messaging for the specified library within this process. Inserts
 * a probe at the entry point of sleep() to call a known function within the
 * library. That function, when called, squirrels away the message handle that
 * is passed into it for future use by the library. Once the probe has been
 * inserted, sleep(0) is called to force that initialization process to occur.
 *
 * @note    All of this is necessary because DPCL is designed to only provide a
 *          communication mechanism from the process to the tool in the context
 *          of a DPCL probe. Since collectors need to be able to send data at
 *          <em>any</em> time, we need to setup an otherwise-unnecessary "fake"
 *          probe to establish a usable message handle for the library.
 *
 * @param library    Library entry for which messaging is to be initialized.
 */
void Process::initializeMessaging(LibraryEntry& library)
{
    Guard guard_myself(this);

    // Find openss_set_msg_handle() within this library
    std::map<std::string, int>::const_iterator
	i = library.functions.find("openss_set_msg_handle");
    
    // Was openss_set_msg_handle() found?
    if(i == library.functions.end())
	return;
    
    // Obtain a probe expression reference to openss_set_msg_handle()
    ProbeExp openss_set_msg_handle_exp =
	library.module->get_reference(i->second);
    Assert(openss_set_msg_handle_exp.get_node_type() == CEN_function_ref);
    
    // Iterate over each module associated with this process
    SourceObj program = dm_process->get_program_object();
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);
	
	// Iterate over each function of this module
	for(int f = 0; f < module.child_count(); ++f)
	    if(module.child(f).src_type() == SOT_function) {
		SourceObj function = module.child(f);
		
		// Get the demangled name of the function
		char name[function.get_demangled_name_length() + 1];
		function.get_demangled_name(name, sizeof(name));

		// Was this function sleep()?
		if(strcmp(name, "__sleep") != 0)
		    continue;

		// Obtain a probe expression reference to sleep()
		ProbeExp sleep_exp = function.reference();
		Assert(sleep_exp.get_node_type() == CEN_function_ref);
		
		// Obtaint the entry point to sleep()
		InstPoint sleep_entry;
		for(int p = 0; p < function.exclusive_point_count(); ++p)
		    if(function.exclusive_point(p).get_type() ==
		       IPT_function_entry)
			sleep_entry = function.exclusive_point(p);
		Assert(sleep_entry.get_type() == IPT_function_entry);

		// Note: In theory Ais_msg_handle should be the only parameter
		//       to openss_set_msg_handle(). But a bug in DPCL seems to
		//       cause probe activation to fail if exactly one parameter
		//       is passed. An extra, zero, parameter causes a different
		//       code path to be executed which seems to work fine. The
		//       offending DPCL code is "dpcl/src/daemon/src/PEtoBP.C"
		//       around line #475.
		
		// Ask DPCL to install and activate the messaging probe
		ProbeExp args_exp[2] = { Ais_msg_handle, ProbeExp(0) };
		ProbeExp call_exp = openss_set_msg_handle_exp.call(2, args_exp);
		GCBFuncType callback = performanceDataCB;
		GCBTagType tag = 0;
		ProbeHandle handle;
		MainLoop::suspend();
		AisStatus retval = 
		    dm_process->binstall_probe(1, &call_exp, &sleep_entry,
					       &callback, &tag, &handle);	
		Assert(retval.status() == ASC_success);
		retval = dm_process->bactivate_probe(1, &handle);
		Assert(retval.status() == ASC_success);
		MainLoop::resume();
		
		// Ask DPCL to execute sleep(0) in this process
		ProbeExp sleep_args_exp[1] = { ProbeExp(0) };
		ProbeExp sleep_call_exp = sleep_exp.call(1, sleep_args_exp);
		MainLoop::suspend();
		retval = dm_process->bexecute(sleep_call_exp, NULL, NULL);
		Assert(retval.status() == ASC_success);
		MainLoop::resume();        
		
		// Save a copy of the messaging probe's handle for this library
		library.messaging = new ProbeHandle(handle);		

		// Return now that messaging is initialized
		return;
		
	    }
	
    }
}



/**
 * Finalize messaging.
 *
 * Finalize messaging for the specified library within this process. Removes the
 * probe previously inserted at the entry point of sleep().
 *
 * @param library    Library entry for which messaging is to be finalized.
 */
void Process::finalizeMessaging(LibraryEntry& library)
{
    Guard guard_myself(this);
    
    // Was a messaing probe installed for this library?
    if(library.messaging == NULL)
	return;
    
    // Ask DPCL to deactivate and remove the messaging probe
    MainLoop::suspend();
    AisStatus retval = dm_process->bdeactivate_probe(1, library.messaging);
    Assert(retval.status() == ASC_success);
    retval = dm_process->bremove_probe(1, library.messaging);
    Assert(retval.status() == ASC_success);
    MainLoop::resume();
    
    // Destroy the handle to the messaging probe
    delete library.messaging;
    library.messaging = NULL;
}



} }  // OpenSpeedShop::Framework
