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

#include "Assert.hxx"
#include "Collector.hxx"
#include "CollectorGroup.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Experiment.hxx"
#include "ExperimentTable.hxx"
#include "Instrumentor.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

#include <pthread.h>
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
	"CREATE TABLE 'Open|SpeedShop' ("
	"    version INTEGER"
	");",
	"INSERT INTO 'Open|SpeedShop' (version) VALUES (1);",
	
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
	"    column INTEGER"
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
	database->prepareStatement("SELECT COUNT(*) FROM 'Open|SpeedShop';");
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
 * are automatically reattached.
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
    
    // Find our threads
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Attempt to attach to the underlying thread
	try {
	    Instrumentor::attachUnderlyingThread(*i);
	}
	catch(...) {
	}
	
    }    

    // Add this experiment to the experiment table
    ExperimentTable::TheTable.addExperiment(this);
}



/**
 * Destructor.
 *
 * Stops all data collection in this experiment, detaches from the underlying
 * threads, and closes the experiment's database reference. The database will
 * remain open if other objects (Thread, Collector, etc.) still exist and
 * refer to it.
 */
Experiment::~Experiment()
{
    // Remove this experiment from the experiment table
    ExperimentTable::TheTable.removeExperiment(this);
    
    // Postpone all performance data collection
    getThreads().postponeCollecting(getCollectors());
    
    // Iterate over each thread in this experiment
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Detach from the underlying thread
	Instrumentor::detachUnderlyingThread(*i);
	
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
 * (standard file descriptors, environment variables, etc.) as when the tool was
 * started. The process is created in a suspended state.
 *
 * @param command    Command to be executed.
 * @param host       Name of host on which to execute the command.
 * @return           Newly created thread.
 */
Thread Experiment::createProcess(const std::string& command,
				 const std::string& host) const
{   
    Thread thread;

    // Create the thread and an underlying thread for executing the command
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement("INSERT INTO Threads (host) VALUES (?);");
    dm_database->bindArgument(1, host);
    while(dm_database->executeStatement());
    thread = Thread(dm_database, dm_database->getLastInsertedUID());    
    Instrumentor::createUnderlyingThread(thread, command);    
    END_TRANSACTION(dm_database);
    
    // Return the thread to the caller
    return thread;
}



/**
 * Attach to an MPI job.
 *
 * Attaches to an existing process that is part of an MPI job and adds all the
 * threads within that job to this experiment. The threads' statuses are not
 * affected.
 *
 * @todo    Currently this routine is unimplemented.
 *
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 * @return        Attached threads.
 */
ThreadGroup Experiment::attachMPIJob(const pid_t& pid,
				     const std::string& host) const
{
    return ThreadGroup();
}



/**
 * Attach to a process.
 *
 * Attaches to an existing process and adds all threads within that process to
 * this experiment. The threads' statuses are not affected.
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
    
    // Create the threads for this process and attach to the underlying threads
    BEGIN_TRANSACTION(dm_database);    
    dm_database->prepareStatement(
	"INSERT INTO Threads (host, pid) VALUES (?, ?);"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    while(dm_database->executeStatement());
    Thread thread(dm_database, dm_database->getLastInsertedUID());
    Instrumentor::attachUnderlyingThread(thread);
    threads.insert(thread);    
    END_TRANSACTION(dm_database);
    
    // Return the thread to the caller
    return threads;  
}



/**
 * Attach to a POSIX thread.
 *
 * Attaches to an existing POSIX thread and adds that thread to this experiment.
 * The thread's status is not affected.
 *
 * @param pid     Process identifier of the process in which the thread resides.
 * @param tid     Thread identifier for the thread.
 * @param host    Name of the host on which the thread resides.
 * @return        Attached thread.
 */
Thread Experiment::attachPosixThread(const pid_t& pid, const pthread_t& tid,
				     const std::string& host) const
{
    Thread thread;
    
    // Create the thread and attach to the underlying thread
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"INSERT INTO Threads (host, pid, posix_tid) VALUES (?, ?, ?);"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    dm_database->bindArgument(3, static_cast<int>(tid));    
    while(dm_database->executeStatement());
    thread = Thread(dm_database, dm_database->getLastInsertedUID());
    Instrumentor::attachUnderlyingThread(thread);
    END_TRANSACTION(dm_database);
    
    // Return the thread to the caller
    return thread;
}



/**
 * Attach to an OpenMP thread.
 *
 * Attaches to an existing OpenMP thread and adds that thread to this
 * experiment. The thread's status is not affected.
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
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    EntrySpy(thread).validate("Threads");

    // Stop all performance data collection for this thread
    thread.getCollectors().stopCollecting(thread);
    thread.getPostponedCollectors().stopCollecting(thread);
    
    // Detach from the underlying thread
    Instrumentor::detachUnderlyingThread(thread);
    
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
