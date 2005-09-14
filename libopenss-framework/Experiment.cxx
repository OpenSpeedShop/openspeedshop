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
 * Definition of the Experiment class.
 *
 */

#include "CollectorGroup.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Experiment.hxx"
#include "Instrumentor.hxx"
#include "ThreadGroup.hxx"

#ifdef HAVE_ARRAYSVCS
#include <arraysvcs.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



namespace {
    
    /**
     * Database schema.
     *
     * Schema for an experiment database. In addition to documenting the various
     * tables, indicies, records, etc. making up the database, these statements
     * are directly executed by the SQL server when creating a new database.
     *
     * @todo    CallSite table needs to be implemented.
     */
    const char* DatabaseSchema[] = {

	// Open|SpeedShop Table
	"CREATE TABLE \"Open|SpeedShop\" ("
	"    version INTEGER"
	");",
	"INSERT INTO \"Open|SpeedShop\" (version) VALUES (1);",
	
	// Thread Table
	"CREATE TABLE Threads ("
	"    id INTEGER PRIMARY KEY,"
	"    host TEXT,"
	"    pid INTEGER DEFAULT NULL,"
	"    posix_tid INTEGER DEFAULT NULL,"
	"    openmp_tid INTEGER DEFAULT NULL,"
	"    mpi_rank INTEGER DEFAULT NULL"
	");",
	
	// Address Space Table
	"CREATE TABLE AddressSpaces ("
	"    id INTEGER PRIMARY KEY,"
	"    thread INTEGER," // From Thread.id
	"    time_begin INTEGER,"
	"    time_end INTEGER,"
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    linked_object INTEGER" // From LinkedObjects.id
	");",
	
	// Linked Object Table
	"CREATE TABLE LinkedObjects ("
	"    id INTEGER PRIMARY KEY,"
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    file INTEGER," // From Files.id
	"    is_executable INTEGER"
	");",
	
	// Function Table
	"CREATE TABLE Functions ("
	"    id INTEGER PRIMARY KEY,"
	"    linked_object INTEGER," // From LinkedObjects.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    name TEXT"
	");",

	// Call Site Table
	// "CREATE TABLE CallSites ("
	// "    id INTEGER PRIMARY KEY"
	// ");",
	
	// Statement Table
	"CREATE TABLE Statements ("
	"    id INTEGER PRIMARY KEY,"
	"    linked_object INTEGER," // From LinkedObjects.id
	"    file INTEGER," // From Files.id
	"    line INTEGER,"
	"    \"column\" INTEGER"
	");",

	// Statement Range Table
	"CREATE TABLE StatementRanges ("
	"    statement INTEGER," // From Statements.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    valid_bitmap BLOB"
	");",
        "CREATE INDEX IndexStatementRangesByStatement "
	"  ON StatementRanges (statement);",
	
	// File Table
	"CREATE TABLE Files ("
	"    id INTEGER PRIMARY KEY,"
	"    path TEXT,"
	"    checksum BLOB DEFAULT NULL,"
	"    contents BLOB DEFAULT NULL"
	");",
	
	// Collector Table
	"CREATE TABLE Collectors ("
	"    id INTEGER PRIMARY KEY,"
	"    unique_id TEXT,"
	"    parameter_data BLOB DEFAULT NULL"
	");",

	// Collecting Table
	"CREATE TABLE Collecting ("
	"    collector INTEGER," // From Collectors.id
	"    thread INTEGER," // From Threads.id
	"    is_postponed INTEGER"
	");",
	
	// Data Table
	"CREATE TABLE Data ("
	"    collector INTEGER," // From Collectors.id
	"    thread INTEGER," // From Thread.id
	"    time_begin INTEGER,"
	"    time_end INTEGER,"
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    data BLOB"
	");",

	// End Of Table Entry
	NULL
    };

}



//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif

/**
 * Get the local host name.
 *
 * Returns the name of the host on which we are executing. This information is
 * obtained directly from the operating system.
 *
 * @return    Name of the local host.
 */
std::string Experiment::getLocalHost()
{
    // Obtain the local host name from the operating system
    char buffer[HOST_NAME_MAX];
    Assert(gethostname(buffer, sizeof(buffer)) == 0);
    
    // Return the local host name to the caller
    return buffer;
}



/**
 * Test accessibility of an experiment database.
 *
 * Returns a boolean value indicating if the specified experiment database is
 * accessible. Simply confirms that the database exists, is accessible by the
 * database engine, and contains a "Open|SpeedShop" table with a single row.
 * It does not check that any of the other tables match the experiment database
 * schema or that the tablesare internally consistent.
 *
 * @param name    Name of the experiment database to be tested.
 * @return        Boolean "true" if the experiment database is accessible,
 *                "false" otherwise.
 */
bool Experiment::isAccessible(const std::string& name)
{
    bool is_accessible = true;

    try {

	// Open the experiment database
	SmartPtr<Database> database = SmartPtr<Database>(new Database(name));

	// Verify there is a "Open|SpeedShop" table containing a single row
	BEGIN_TRANSACTION(database);
	database->prepareStatement("SELECT COUNT(*) FROM \"Open|SpeedShop\";");
	while(database->executeStatement())
	    is_accessible = (database->getResultAsInteger(1) == 1);
	END_TRANSACTION(database);
	
    }
    catch(...) {
	is_accessible = false;
    }
    
    // Return the experiment database's accessibility to the caller
    return is_accessible;
}



/**
 * Create an experiment database.
 *
 * Creates a new, empty, experiment database with the specified name.
 *
 * @param name    Name of the experiment database to be created.
 */
void Experiment::create(const std::string& name)
{
    // Create an open the database
    Database::create(name);
    SmartPtr<Database> database = SmartPtr<Database>(new Database(name));

    // Apply the experiment database schema
    BEGIN_TRANSACTION(database);
    for(int i = 0; DatabaseSchema[i] != NULL; ++i) {
	database->prepareStatement(DatabaseSchema[i]);
	while(database->executeStatement());
    }
    END_TRANSACTION(database);
}



/**
 * Remove an experiment database.
 *
 * Removes the experiment database with the specified name.
 *
 * @param name    Name of the experiment database to be removed.
 */
void Experiment::remove(const std::string& name)
{
    // Remove the database
    Database::remove(name);
}



/**
 * Constructor from an experiment database name.
 *
 * Constructs an object for accessing the specified experiment database.
 * Any threads in this experiment that correspond to an underlying thread
 * must be explicitly connected when/if needed.
 *
 * @note    A DatabaseInvalid exception is thrown if the specified name is not
 *          a valid experiment database.
 *
 * @param name    Name of the experiment database to be accessed.
 */
Experiment::Experiment(const std::string& name) :
    dm_database(SmartPtr<Database>(new Database(name)))
{
    // Verify the experiment database is accessible
    if(!isAccessible(name))
	throw Exception(Exception::DatabaseInvalid, dm_database->getName());
    
    // Iterate over each thread in this experiment
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Retain this thread in the instrumentor
	Instrumentor::retain(*i);
	
    }

    // Add this experiment's database to the data queues
    DataQueues::addDatabase(dm_database);
}



/**
 * Destructor.
 *
 * Stops all data collection in this experiment, detaches from the underlying
 * threads (if not used by other experiments), and close the experiment's
 * database reference. The database will remain open if other objects (Thread,
 * Collector, etc.) still exist and refer to it.
 */
Experiment::~Experiment()
{
    // Postpone all performance data collection
    getThreads().postponeCollecting(getCollectors());

    // Remove this experiment's database from the data queues
    DataQueues::removeDatabase(dm_database);
    
    // Iterate over each thread in this experiment
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Release this thread in the instrumentor
	Instrumentor::release(*i);
	
    }
}



/**
 * Rename this experiment database.
 *
 * Renames this experiment database to the specified name. The experiment
 * database is unmodified and still accessible via this object.
 *
 * @param name    New name of the database for this experiment.
 */
void Experiment::renameTo(const std::string& name) const
{
    // Rename the database
    dm_database->renameTo(name);
}



/**
 * Copy this experiment database.
 *
 * Copies this experiment database to the specified name. The original
 * experiment database is unmodified and still accessible via this object.
 * Accessing the copy involves creating a new Experiment object with the
 * copy's name.
 *
 * @param name    Name of the created copy.
 */
void Experiment::copyTo(const std::string& name) const
{
    // Copy the database
    dm_database->copyTo(name);
}



/**
 * Get our experiment database name.
 *
 * Returns the name of the database for this experiment.
 *
 * @return    Name of the database for this experiment.
 */
std::string Experiment::getName() const
{
    // Return the database name to the caller
    return dm_database->getName();
}



/**
 * Get our threads.
 *
 * Returns all threads currently in this experiment. An empty thread group is
 * returned if this experiment doesn't contain any threads.
 *
 * @return    Threads currently contained within this experiment.
 */
ThreadGroup Experiment::getThreads() const
{
    ThreadGroup threads;
    
    // Find our threads
    BEGIN_TRANSACTION(dm_database);    
    dm_database->prepareStatement("SELECT id FROM Threads;");
    while(dm_database->executeStatement())
	threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Create a process.
 *
 * Creates a new process to execute the specified command and adds that process
 * to this experiment. The command is created with the same initial environment
 * as when the tool was started. The process is created in a suspended state.
 *
 * @param command            Command to be executed.
 * @param host               Name of host on which to execute the command.
 * @param stdout_callback    Standard output stream callback for the process.
 * @param stderr_callback    Standard error stream callback for the process.
 * @return                   Newly created thread.
 */
Thread Experiment::createProcess(const std::string& command,
				 const std::string& host,
				 const OutputCallback stdout_callback,
				 const OutputCallback stderr_callback) const
{   
    Thread thread;

    // Create the thread entry in the database
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement("INSERT INTO Threads (host) VALUES (?);");
    dm_database->bindArgument(1, host);
    while(dm_database->executeStatement());
    thread = Thread(dm_database, dm_database->getLastInsertedUID()); 
    END_TRANSACTION(dm_database);

    // Create the underlying thread for executing the command
    try {
	// Instrumentor::retain(thread);  (Implied by Following Line)
	Instrumentor::create(thread, command, stdout_callback, stderr_callback);
    }
    catch(...) {

	// Remove this thread from the database
	dm_database->prepareStatement("DELETE FROM Threads WHERE id = ?;");
	dm_database->bindArgument(1, EntrySpy(thread).getEntry());
	while(dm_database->executeStatement());    

	// Re-throw exception upwards
	throw;
    }
    
    // Return the thread to the caller
    return thread;
}



/**
 * Attach to an MPI job.
 *
 * Attaches to an existing process that is part of an MPI job and adds all the
 * threads within that job to this experiment. The thread(s) are suspended as a
 * result of attaching.
 *
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 * @return        Attached threads.
 */
ThreadGroup Experiment::attachMPIJob(const pid_t& pid,
				     const std::string& host) const
{
    ThreadGroup threads;
    
    // Attach to the initial, specified, process
    threads = attachProcess(pid, host);
    
    // Wait until no thread in that process is still connecting
    while(threads.isAnyState(Thread::Connecting))
	sleep(1);
    
    // Is there at least one connected (suspended) thread?
    ThreadGroup connected = threads.getSubsetWithState(Thread::Suspended);
    if(!connected.empty()) {
	
	// Grab an arbitrary (the first) connected thread
	Thread thread = *(connected.begin());
	
	// Look for any available MPI job information for this thread
	Job job;
	getMPIJobFromMPT(thread, job);
	getMPIJobFromMPICH(thread, job);
	
	// Iterate over any processes found to be in our MPI job
	for(Job::const_iterator i = job.begin(); i != job.end(); ++i) {
	    
	    // Attach to this process
	    ThreadGroup additional_threads = attachProcess(i->second, i->first);
	    
	    // Merge these additional threads into the overall thread group
	    threads.insert(additional_threads.begin(), 
			   additional_threads.end());
	    
	}
	
    }

    // Return the threads to the caller
    return threads;  
}



/**
 * Attach to a process.
 *
 * Attaches to an existing process and adds all threads within that process to
 * this experiment. The thread(s) are suspended as a result of attaching.
 *
 * @note    If the process already exists within this experiment, its existing
 *          threads are returned without attaching again.
 *
 * @todo    Currently this routine assumes each process has a single thread and
 *          thus creates only one Thread object. Once a mechanism is found to
 *          "discover" the list of threads in a process this can be changed to
 *          support the desired semantics.
 *
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 * @return        Attached threads.
 */
ThreadGroup Experiment::attachProcess(const pid_t& pid,
				      const std::string& host) const
{
    ThreadGroup threads;

    // Allocate this flag outside the transaction's try/catch block
    bool is_attached = false;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);

    // Are there any existing thread(s) for this process?
    dm_database->prepareStatement(	
	"SELECT id FROM Threads WHERE host = ? AND pid = ?;"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    while(dm_database->executeStatement())
	threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    is_attached = !threads.empty();

    // Create the thread(s) entries if they weren't present in the database
    if(!is_attached) {
	dm_database->prepareStatement(
	    "INSERT INTO Threads (host, pid) VALUES (?, ?);"
	    );
	dm_database->bindArgument(1, host);
	dm_database->bindArgument(2, pid);
	while(dm_database->executeStatement());
	Thread thread(dm_database, dm_database->getLastInsertedUID());
	threads.insert(thread);	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Actually attach if we weren't already
    if(!is_attached) {

	// Iterate over each of the thread(s)
	for(ThreadGroup::const_iterator 
		i = threads.begin(); i != threads.end(); ++i) {
	    
	    // Retain this thread in the instrumentor
	    Instrumentor::retain(*i);
	    
	    // Connect to this thread in the instrumentor
	    Instrumentor::changeState(*i, Thread::Connecting);
	    
	}

    }
    
    // Return the threads to the caller
    return threads;  
}



/**
 * Attach to a POSIX thread.
 *
 * Attaches to an existing POSIX thread and adds that thread to this experiment.
 * The thread is suspended as a result of attaching.
 *
 * @todo    Currently this routine is unimplemented.
 *
 * @param pid     Process identifier of the process in which the thread resides.
 * @param tid     Thread identifier for the thread.
 * @param host    Name of the host on which the thread resides.
 * @return        Attached thread.
 */
Thread Experiment::attachPosixThread(const pid_t& pid, const pthread_t& tid,
				     const std::string& host) const
{
    return Thread();
}



/**
 * Attach to an OpenMP thread.
 *
 * Attaches to an existing OpenMP thread and adds that thread to this
 * experiment.  The thread is suspended as a result of attaching.
 *
 * @todo    Currently this routine is unimplemented.
 *
 * @param pid     Process identifier of the process in which the thread resides.
 * @param tid     Thread identifier for the thread.
 * @param host    Name of the host on which the thread resides.
 * @return        Attached thread.
 */
Thread Experiment::attachOpenMPThread(const pid_t& pid, const int& tid,
				      const std::string& host) const
{
    return Thread();
}



/**
 * Remove a thread.
 *
 * Removes the specified thread from this experiment. The thread is removed
 * from any collectors to which it was previously attached and any performance
 * data that was associated with the thread is destroyed. The underlying thread
 * in the operating system is <em>not</em> destroyed. If the thread was in the
 * suspended state, it is put into the running state before being removed.
 *
 * @pre    Threads must have originated from this experiment to be removed. An
 *         assertion failure occurs if the thread was never in this experiment.
 *
 * @param thread    Thread to be removed.
 */
void Experiment::removeThread(const Thread& thread) const
{
    // Check preconditions
    Assert(EntrySpy(thread).getDatabase() == dm_database);

    // Stop all performance data collection for this thread
    thread.getCollectors().stopCollecting(thread);
    thread.getPostponedCollectors().stopCollecting(thread);
    
    // Release this thread in the instrumentor
    Instrumentor::release(thread);
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    EntrySpy(thread).validate("Threads");

    // Remove this thread
    dm_database->prepareStatement("DELETE FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, EntrySpy(thread).getEntry());
    while(dm_database->executeStatement());    

    // Remove address spaces referencing the removed thread
    dm_database->prepareStatement(
	"DELETE FROM AddressSpaces WHERE thread = ?;"
	);
    dm_database->bindArgument(1, EntrySpy(thread).getEntry());
    while(dm_database->executeStatement());    

    // Remove any data associated with this thread
    dm_database->prepareStatement("DELETE FROM Data WHERE thread = ?;");
    dm_database->bindArgument(1, EntrySpy(thread).getEntry());
    while(dm_database->executeStatement());    
    
    // Remove unused linked objects
    dm_database->prepareStatement(
	"DELETE FROM LinkedObjects "
	"WHERE linked_object "
	"  NOT IN (SELECT DISTINCT linked_object FROM AddressSpaces);"
	);
    while(dm_database->executeStatement());
    
    // Remove unused functions
    dm_database->prepareStatement(
	"DELETE FROM Functions "
	"WHERE linked_object "
	"  NOT IN (SELECT DISTINCT linked_object FROM AddressSpaces);"
	);
    while(dm_database->executeStatement());

    // Remove unused statements
    dm_database->prepareStatement(
	"DELETE FROM Statements "
	"WHERE linked_object "
	"  NOT IN (SELECT DISTINCT linked_object FROM AddressSpaces);"
	);
    while(dm_database->executeStatement());

    // Remove unused statement ranges
    dm_database->prepareStatement(
	"DELETE FROM StatementRanges "
	"WHERE statement "
	"  NOT IN (SELECT DISTINCT id FROM Statements)"
	);
    while(dm_database->executeStatement());
    
    // Remove unused files
    dm_database->prepareStatement(
	"DELETE FROM Files "
	"WHERE id NOT IN (SELECT DISTINCT file FROM LinkedObjects) "
	"  AND id NOT IN (SELECT DISTINCT file FROM Statements);"
	);
    while(dm_database->executeStatement());
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Get our collectors.
 *
 * Returns all collectors currently in this experiment. An empty collector group
 * is returned if this experiment doesn't contain any collectors.
 *
 * @return    Collectors currently contained within this experiment.
 */
CollectorGroup Experiment::getCollectors() const
{
    CollectorGroup collectors;
    
    // Find our collectors
    BEGIN_TRANSACTION(dm_database);    
    dm_database->prepareStatement("SELECT id FROM Collectors;");
    while(dm_database->executeStatement())
	collectors.insert(Collector(dm_database,
				    dm_database->getResultAsInteger(1)));    
    END_TRANSACTION(dm_database);

    // Return the collectors to the caller
    return collectors;
}



/**
 * Create a collector.
 *
 * Creates an instance of the collector with the specified unique identifier
 * and adds the instance to this experiment.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @param unique_id    Unique identifier of collector to be created.
 * @return             Newly created collector.
 */
Collector Experiment::createCollector(const std::string& unique_id) const
{
    Collector collector;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);

    // Create the collector
    dm_database->prepareStatement(
	"INSERT INTO Collectors (unique_id) VALUES (?);"
	);
    dm_database->bindArgument(1, unique_id);
    while(dm_database->executeStatement());
    collector = Collector(dm_database, dm_database->getLastInsertedUID());

    // Check preconditions
    if(collector.dm_impl == NULL) {
	collector.instantiateImpl();
	if(collector.dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable, unique_id);
    }   

    // Update collector with default parameter values
    collector.setParameterData(collector.dm_impl->getDefaultParameterValues());
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
    
    // Return the collector to the caller
    return collector;
}



/**
 * Remove a collector.
 *
 * Removes the specified collector from this experiment. All data collection for
 * this collector is stopped, and any performance data that was associated with
 * this collector is destroyed.
 *
 * @pre    Collectors must have originated from this experiment to be removed.
 *         An assertion failure occurs if the collector was never in this
 *         experiment.
 *
 * @param collector    Collector to be removed.
 */
void Experiment::removeCollector(const Collector& collector) const
{
    // Check preconditions
    Assert(EntrySpy(collector).getDatabase() == dm_database);
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    EntrySpy(collector).validate("Collectors");
    
    // Stop all performance data collection for this collector
    collector.getThreads().stopCollecting(collector);
    collector.getPostponedThreads().stopCollecting(collector);
    
    // Remove this collector
    dm_database->prepareStatement("DELETE FROM Collectors WHERE id = ?;");
    dm_database->bindArgument(1, EntrySpy(collector).getEntry());
    while(dm_database->executeStatement());    
    
    // Remove any data associated with this collector
    dm_database->prepareStatement("DELETE FROM Data WHERE collector = ?;");
    dm_database->bindArgument(1, EntrySpy(collector).getEntry());
    while(dm_database->executeStatement());    
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Get MPI job information from MPT.
 *
 * Returns MPI job information for the specified thread. Looks for specific 
 * global variables that indicate the specified thread is part of an "mpirun"
 * process in the SGI MPT (Message Passing Toolkit) MPI implementation. If
 * these variables are found, array services (part of MPT) is used to lookup
 * and return all the host/pid pairs that are part of the MPI job containing
 * this thread. No information is returned if the specified thread isn't part
 * of an MPT mpirun process.
 *
 * @param thread    Thread for which to retrieve MPI job information.
 * @retval job      MPI job information for this thread.
 */
void Experiment::getMPIJobFromMPT(const Thread& thread, Job& job)
{
#ifdef HAVE_ARRAYSVCS

    bool is_mpt_job = true;
    int64_t ash = 0;
    std::string array = "";
    
    // Attempt to access the ASH and array name from this thread
    is_mpt_job &= Instrumentor::getGlobal(thread, "MPI_debug_ash", ash);
    is_mpt_job &= Instrumentor::getGlobal(thread, "MPI_debug_array", array);
    
    // Go no further if this thread isn't in an MPT MPI job
    if(!is_mpt_job)
	return;
    
    // Open the MPT array services server on the host where this thread resides
    asserver_t server = asopenserver(thread.getHost().c_str(), -1);

    // Go no further if the MPT array services server couldn't be opened
    if(server == 0)
	return;
    
    // Get the list of hosts and their corresponding pids for this session
    asarraypidlist_t* list =
	aspidsinash_array(server, array.empty() ? NULL : array.c_str(), ash);

    // Go no further if the list couldn't be accessed
    if(list == NULL)
	return;
    
    // Iterate over each machine in the array
    for(int i = 0; i < list->nummachines; ++i)
	
        // Iterate over each PID on this machine in the job
        for(int j = 0; j < list->machines[i]->numpids; ++j)

	    // Add this host/pid to the MPI job information
	    job.insert(std::make_pair(list->machines[i]->machname,
				      list->machines[i]->pids[j]));
    
    // Free the list of hosts and their corresponding pids
    asfreearraypidlist(list, ASFLF_FREEDATA);

    // Close our handle to the MPT array services server
    ascloseserver(server);
    
#endif
}



/**
 * Get MPI job information from MPICH interface.
 *
 * Returns MPI job information for the specified thread. Looks for specific
 * global variables that indicate the specified thread is part of an MPI job
 * that supports the MPICH "process-finding" interface. If these variables
 * are found, they are used to retrieve and return all the host/pid pairs
 * that are part of the MPI job containing this thread. No information is
 * returned if the specified thread doesn't contain the MPICH process-finding
 * interface. 
 *
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @param thread    Thread for which to retrieve MPI job information.
 * @retval job      MPI job information for this thread.
 */
void Experiment::getMPIJobFromMPICH(const Thread& thread, Job& job)
{
    bool is_mpich_job = true;
    Job table;

    // Attempt to access the MPICH process table from this thread
    is_mpich_job &= Instrumentor::getGlobalMPICHProcTable(thread, table);
    
    // Go no further if this thread isn't in an MPICH MPI job
    if(!is_mpich_job)
	return;

    // Add this table to the MPI job information
    job.insert(table.begin(), table.end());    
}
