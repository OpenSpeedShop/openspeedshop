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
 * Definition of the Experiment class.
 *
 */

#include "Assert.hxx"
#include "Collector.hxx"
#include "CollectorGroup.hxx"
#include "Database.hxx"
#include "Experiment.hxx"
#include "Instrumentor.hxx"
#include "Metadata.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

#include <stdexcept>

using namespace OpenSpeedShop::Framework;



namespace {
    
    /**
     * Database schema.
     *
     * Schema for an experiment database. In addition to documenting the various
     * tables, indicies, records, etc. making up the database, these statements
     * are directly executed by the SQL server when creating a new database.
     *
     * @todo    CallSite table needs to be implemented. Also need to add removal
     *          of CallSite table rows to the RemoveUnusedLinkedObjects trigger.
     */
    const char* DatabaseSchema[] = {

	// Open/SpeedShop (Master) Table
	"CREATE TABLE 'Open/SpeedShop' ("
	"    version INTEGER"
	");",
	"INSERT INTO 'Open/SpeedShop' (version) VALUES (1);",
	
	// Thread Table
	"CREATE TABLE Threads ("
	"    id INTEGER PRIMARY KEY,"
	"    host TEXT,"
	"    pid INTEGER DEFAULT NULL,"
	"    posix_tid INTEGER DEFAULT NULL,"
	"    omp_tid INTEGER DEFAULT NULL,"
	"    mpi_rank INTEGER DEFAULT NULL,"
	"    ash INTEGER DEFAULT NULL"
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
	"    file INTEGER" // From Files.id
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
	"    file INTEGER," // From Files.id
	"    line INTEGER,"
	"    column INTEGER"
	");",

	// Statement Ranges Table
	"CREATE TABLE StatementRanges ("
	"    statement INTEGER," // From Statements.id
	"    linked_object INTEGER," // From LinkedObjects.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER"
	");",
	
	// File Table
	"CREATE TABLE Files ("
	"    id INTEGER PRIMARY KEY,"
	"    path TEXT,"
	"    checksum BLOB,"
	"    contents BLOB"
	");",
	
	// Collector Table
	"CREATE TABLE Collectors ("
	"    id INTEGER PRIMARY KEY,"
	"    unique_id TEXT,"
	"    is_collecting INTEGER DEFAULT 0"
	");",
	
	// Parameter Table
	"CREATE TABLE Parameters ("
	"    collector INTEGER," // From Collectors.id
	"    unique_id TEXT,"
	"    value TEXT"
	");",

	// Attachments Table
	"CREATE TABLE Attachments ("
	"    collector INTEGER," // From Collectors.id
	"    thread INTEGER" // From Threads.id
	");",
	
	// Data Table
	"CREATE TABLE Data ("
	"    thread INTEGER," // From Thread.id
	"    collector INTEGER," // From Collectors.id
	"    time_begin INTEGER,"
	"    time_end INTEGER,"
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    raw_data BLOB"
	");",

	// Remove address spaces referencing removed threads
	"CREATE TRIGGER RemoveAddressSpacesOnThreads"
	"    AFTER DELETE ON Threads FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM AddressSpaces WHERE thread = OLD.id;"
	"    END;",
	
	// Remove unused linked objects referencing removed address spaces
	"CREATE TRIGGER RemoveUnusedLinkedObjects"
	"    AFTER DELETE ON AddressSpaces FOR EACH ROW"
	"        WHEN OLD.linked_object NOT IN"
	"            (SELECT linked_object FROM AddressSpaces)"
	"    BEGIN"
	"        DELETE FROM LinkedObjects"
	"            WHERE id = OLD.linked_object;"
	"        DELETE FROM Functions"
	"            WHERE linked_object = OLD.linked_object;"
	"        DELETE FROM StatementRanges"
	"            WHERE linked_object = OLD.linked_object;"	
	"        DELETE FROM Statements"
	"            WHERE id NOT IN (SELECT statement FROM StatementRanges);"
	"        DELETE FROM Files"
	"            WHERE id NOT IN (SELECT file FROM LinkedObjects) AND"
	"                  id NOT IN (SELECT file FROM Statements);"
	"    END;",

	// Remove parameters referencing removed collectors
	"CREATE TRIGGER RemoveParametersOnCollectors"
	"    AFTER DELETE ON Collectors FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM Parameters WHERE collector = OLD.id;"
	"    END;",

	// Remove attachments referencing removed threads or collectors
	"CREATE TRIGGER RemoveAttachmentsOnThreads"
	"    AFTER DELETE ON Threads FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM Attachments WHERE thread = OLD.id;"
	"    END;",
	"CREATE TRIGGER RemoveAttachmentsOnCollectors"
	"    AFTER DELETE ON Collectors FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM Attachments WHERE collector = OLD.id;"
	"    END;",

	// Remove data referencing removed threads or collectors
	"CREATE TRIGGER RemoveDataOnThreads"
	"    AFTER DELETE ON Threads FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM Data WHERE thread = OLD.id;"
	"    END;",
	"CREATE TRIGGER RemoveDataOnCollectors"
	"    AFTER DELETE ON Collectors FOR EACH ROW"
	"    BEGIN"
        "        DELETE FROM Data WHERE collector = OLD.id;"
	"    END;",
	
	// End Of Table Entry
	NULL
    };
    
}



/**
 * Test accessibility of an experiment database.
 *
 * Returns a boolean value indicating if the specified experiment database is
 * accessible. Simply confirms that the database exists, is accessible by the
 * database engine, and contains a "Open/SpeedShop" table with a single row.
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

	// Verify there is a "Open/SpeedShop" table containing a single row
	BEGIN_TRANSACTION(database);
	database->prepareStatement("SELECT COUNT(*) FROM 'Open/SpeedShop';");
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
 * @note    An exception of type std::runtime_error is thrown if the
 *          experiment database cannot be created for any reason (including
 *          the pre-existence of the named database).
 *
 * @param name    Name of the database to be created.
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
 * Constructor from an experiment database name.
 *
 * Constructs an object for accessing the specified experiment database.
 * Any threads in this experiment that correspond to an underlying thread
 * are automatically reattached.
 *
 * @note    An exception of type std::runtime_error is thrown if the experiment
 *          database cannot be opened for any reason.
 *
 * @param name    Name of the experiment database to be accessed.
 */
Experiment::Experiment(const std::string& name) :
    dm_database(SmartPtr<Database>(new Database(name)))
{
    // Verify the experiment database is accessible
    if(!isAccessible(name))
	throw std::runtime_error("Experiment database \"" +
				 dm_database->getName() +
				 "\" can't be opened.");
    
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
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);

    // Stop data collection for all collectors
    CollectorGroup collectors = getCollectors();
    for(CollectorGroup::const_iterator
	    i = collectors.begin(); i != collectors.end(); ++i)
	if(i->isCollecting())
	    i->stopCollecting();
    
    // Detach from all of the underlying threads
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i)
	Instrumentor::detachUnderlyingThread(*i);

    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 
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
	threads.push_back(Thread(dm_database,
				 dm_database->getResultAsInteger(1)));    
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
 * @note    An exception of type std::runtime_error is thrown if the thread
 *          cannot be created for any reason (host doesn't exist, specified
 *          command cannot be executed, etc.)
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
 * Attach to a process.
 *
 * Attaches to an existing process and adds all threads within that process to
 * this experiment. The threads' statuses are not affected.
 *
 * @note    An exception of type std::runtime_error is thrown if the process
 *          cannot be attached for any reason (host or process doesn't exist,
 *          etc.)
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
    threads.push_back(thread);    
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
 * @note    An exception of type std::runtime_error is thrown if the thread
 *          cannot be attached for any reason (host or process doesn't exist,
 *          etc.)
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



#ifdef HAVE_OPENMP
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
#endif



#ifdef HAVE_ARRAY_SERVICES
/**
 * Attach to an array session.
 *
 * Attaches to to an existing array session and adds all threads within that
 * array session to this experiment. The threads' statuses are not affected.
 *
 * @todo    Currently this routine is unimplemented.
 *
 * @param ash     Handle for the array session.
 * @param host    Name of the host on which the array session resides.
 * @return        Attached threads.
 */
ThreadGroup Experiment::attachArraySession(const ash_t& ash,
					   const std::string& host) const
{
    return ThreadGroup();
}
#endif



/**
 * Remove a thread.
 *
 * Removes the specified thread from this experiment. The thread is removed
 * from any collectors to which it was previously attached and any performance
 * data that was associated with the thread is destroyed. The underlying thread
 * in the operating system is <em>not</em> destroyed. If the thread was in the
 * suspended state, it is put into the running state before being removed.
 *
 * @pre    Threads must be in the experiment to be removed. An exception
 *         of type std::invalid_argument or Database::Corrupted is thrown if
 *         the thread is not in the experiment.
 *
 * @param thread    Thread to be removed.
 */
void Experiment::removeThread(const Thread& thread) const
{
    // Check preconditions
    if(thread.dm_database != dm_database)
	throw std::invalid_argument("Cannot remove a thread that isn't in the "
                                    "experiment.");
    
    // Detach from the underlying thread
    Instrumentor::detachUnderlyingThread(thread);
    
    // Remove this thread
    BEGIN_TRANSACTION(dm_database);
    thread.validateEntry();
    dm_database->prepareStatement("DELETE FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, thread.dm_entry);
    while(dm_database->executeStatement());    
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
    // Implementation Note: Collector's constructor uses an SQL query to find
    //                      its unique identifier so that it can instantiate a
    //                      collector implementation. Since we use an SQL query
    //                      here to find the collectors, and Database does not
    //                      allow multiple in-flight SQL statements, the lookup
    //                      of the collector list had to be separated from the
    //                      creation of the collector objects.

    // Find our collectors
    std::vector<int> entries;
    BEGIN_TRANSACTION(dm_database);    
    dm_database->prepareStatement("SELECT id FROM Collectors;");
    while(dm_database->executeStatement())
	entries.push_back(dm_database->getResultAsInteger(1));    
    END_TRANSACTION(dm_database);
    
    // Create the collectors
    CollectorGroup collectors;
    for(std::vector<int>::const_iterator
	    i = entries.begin(); i != entries.end(); ++i)
	collectors.push_back(Collector(dm_database, *i));
    
    // Return the collectors to the caller
    return collectors;
}



/**
 * Create a collector.
 *
 * Creates an instance of the collector with the specified unique identifier
 * and adds the instance to this experiment.
 *
 * @note    Instances can only be created for collectors for which a corresond-
 *          ing plugin was found. An exception of type std::invalid_argument is
 *          thrown if a plugin was not found for the unique identifier, or if
 *          any one of a number of other unlikely failures occurs, such as the
 *          plugin being found at startup but subsequently moved.
 *
 * @param unique_id    Unique identifier of collector to be created.
 * @return             Newly created collector.
 */
Collector Experiment::createCollector(const std::string& unique_id) const
{
    Collector collector;

    // Create the collector
    BEGIN_TRANSACTION(dm_database); 
    dm_database->prepareStatement(
	"INSERT INTO Collectors (unique_id) VALUES (?);"
	);
    dm_database->bindArgument(1, unique_id);
    while(dm_database->executeStatement());
    collector = Collector(dm_database, dm_database->getLastInsertedUID());
    if(collector.dm_impl == NULL)
	throw std::invalid_argument("Cannot create a collector instance for "
				    "unknown unique identifier \"" + unique_id +
				    "\".");
    END_TRANSACTION(dm_database);
    
    // Return the collector to the caller
    return collector;
}



/**
 * Remove a collector.
 *
 * Removes the specified collector from this experiment. Data collection is
 * stopped, any threads that were previously attached to the collector are
 * detached, and any performance data that was associated with the collector is
 * destroyed.
 *
 * @pre    Collectors must be in the experiment to be removed. An exception
 *         of type std::invalid_argument or Database::Corrupted is thrown if
 *         the collector is not in the experiment.
 *
 * @param collector    Collector to be removed.
 */
void Experiment::removeCollector(const Collector& collector) const
{
    // Check preconditions
    if(collector.dm_database != dm_database)
	throw std::invalid_argument("Cannot remove a collector that isn't in "
                                    "the experiment.");
    
    // Stop data collection for this collector
    collector.stopCollecting();
    
    // Remove this collector
    BEGIN_TRANSACTION(dm_database);    
    collector.validateEntry();
    dm_database->prepareStatement("DELETE FROM Collectors WHERE id = ?;");
    dm_database->bindArgument(1, collector.dm_entry);
    while(dm_database->executeStatement());    
    END_TRANSACTION(dm_database);    
}



//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// we haven't found it, so define it directly for now.
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
