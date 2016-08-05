////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007-2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2014 The Krell Institute. All Rights Reserved.
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

#include "AddressBitmap.hxx"
#include "AddressSpace.hxx"
#include "CollectorGroup.hxx"
#include "DataCache.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Experiment.hxx"
#include "Function.hxx"
#include "FunctionCache.hxx"
#include "Instrumentor.hxx"
#include "LinkedObject.hxx"
#include "Loop.hxx"
#include "LoopCache.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "StatementCache.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"
#include "ThreadName.hxx"

#ifdef HAVE_ARRAYSVCS
#include <arraysvcs.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <unistd.h>

#include <stdio.h>
#include <fcntl.h>
#include <list>
#include <string>

using namespace OpenSpeedShop::Framework;



namespace {
    
    /**
     * Database schema.
     *
     * Schema for an experiment database. In addition to documenting the various
     * tables, indicies, records, etc. making up the database, these statements
     * are directly executed by the SQL server when creating a new database.
     */
    const char* DatabaseSchema[] = {

	// Open|SpeedShop Table
	"CREATE TABLE \"Open|SpeedShop\" ("
	"    version INTEGER"
	");",
	"INSERT INTO \"Open|SpeedShop\" (version) VALUES (7);",
	
	// Thread Table
	"CREATE TABLE Threads ("
	"    id INTEGER PRIMARY KEY,"
	"    command TEXT DEFAULT NULL,"
	"    host TEXT,"
	"    pid INTEGER DEFAULT NULL,"
	"    posix_tid INTEGER DEFAULT NULL,"
	"    openmp_tid INTEGER DEFAULT NULL,"
	"    mpi_rank INTEGER DEFAULT NULL,"
	"    mpi_impl TEXT DEFAULT NULL"
	");",
	"CREATE INDEX IndexThreadByHostPidTid ON Threads(host,pid,posix_tid);",
	
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
	"CREATE INDEX IndexAddressSpacesByLinkedObject "
	"  ON AddressSpaces (linked_object);",
	"CREATE INDEX IndexAddressSpacesByThreadAndLinkedObject "
	"  ON AddressSpaces (thread, linked_object);",

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
	"    name TEXT"
	");",
	"CREATE INDEX IndexFunctionsByName ON Functions (name);",
	"CREATE INDEX IndexFunctionsByLinkedObject "
	"  ON Functions (linked_object);",

	// Function Range Table
	"CREATE TABLE FunctionRanges ("
	"    function INTEGER," // From Functions.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    valid_bitmap BLOB"
	");",
    "CREATE INDEX IndexFunctionRangesByFunction "
	"  ON FunctionRanges (function);",
	
    // Loop Table
	"CREATE TABLE Loops ("
	"    id INTEGER PRIMARY KEY,"
	"    linked_object INTEGER," // From LinkedObjects.id
    "    addr_head INTEGER"
	");",
	"CREATE INDEX IndexLoopsByLinkedObject "
	"  ON Loops (linked_object);",
    
    // Loop Range Table
	"CREATE TABLE LoopRanges ("
	"    loop INTEGER," // From Loops.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    valid_bitmap BLOB"
	");",
    "CREATE INDEX IndexLoopRangesByLoop "
	"  ON LoopRanges (loop);",
	
	// Statement Table
	"CREATE TABLE Statements ("
	"    id INTEGER PRIMARY KEY,"
	"    linked_object INTEGER," // From LinkedObjects.id
	"    file INTEGER," // From Files.id
	"    line INTEGER,"
	"    \"column\" INTEGER"
	");",
	"CREATE INDEX IndexStatementsByLinkedObject "
	"  ON Statements (linked_object);",

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
	"CREATE INDEX IndexFilesByPath ON Files (path);",
	
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
	"    id INTEGER PRIMARY KEY,"
	"    collector INTEGER," // From Collectors.id
	"    thread INTEGER," // From Thread.id
	"    time_begin INTEGER,"
	"    time_end INTEGER,"
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    data BLOB"
	");",
	"CREATE INDEX IndexDataByCollectorThread ON Data (collector,thread);",

        // View Reuse Table
	"CREATE TABLE Views ("
	"    id INTEGER PRIMARY KEY,"
	"    view_cmd TEXT," 
        "    view_data BLOB"
	");",

	// End Of Table Entry
	NULL
    };



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



#ifndef NDEBUG
/** Flag indicating if debuging for MPI jobs is enabled. */
bool Experiment::is_debug_mpijob_enabled = 
    (getenv("OPENSS_DEBUG_MPIJOB") != NULL);

/** Flag indicating if debuging for offline experiments is enabled. */
bool Experiment::is_debug_offline_enabled = 
    (getenv("OPENSS_DEBUG_OFFLINE") != NULL);

/** Flag indicating if debuging for save/reuse data views is enabled. */
bool Experiment::is_debug_reuse_views_enabled = 
    (getenv("OPENSS_DEBUG_REUSE_VIEWS") != NULL);
#endif



#ifdef HAVE_DPCL
#include "dpcl/ProcessTable.hxx"
#endif

/**
 * Get the DPCL daemon listener port name.
 *
 * Returns the name of the DPCL daemon ("dpcld") listener port name in the form
 * "[host]:[port]". An empty string is returned if the listener isn't operating.
 * The listener is used when manually starting ("dpcld -p [host]:[port]") DPCL
 * daemons rather than using the normal xinetd based mechanism.
 *
 * @return    Name of the dpcld listener port in the form "[host]:[port]".
 */
std::string Experiment::getDpcldListenerPort()
{   
    // Return the dpcld listener port name to the caller
#ifdef HAVE_DPCL
    return ProcessTable::TheTable.getDpcldListenerPort();
#else
    return std::string();
#endif
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
 * Get the canonical name of a host.
 *
 * Returns the canonical name of the specified host. This information is
 * obtained directly from the operating system.
 *
 * @param host    Name of host for which to get canonical name.
 * @return        Canonical name of that host.
 */
std::string Experiment::getCanonicalName(const std::string& host)
{
    std::string canonical = host;

    // Interested in IPv4 protocol information only (including canonical name)
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    hints.ai_protocol = PF_INET;
    
    // Request address information for this host
    struct addrinfo* results = NULL;
    getaddrinfo(host.c_str(), NULL, &hints, &results);
    
    // Was the specified name for the loopback device?
    if((results != NULL) && 
       (ntohl(reinterpret_cast<struct sockaddr_in*>
	      (results->ai_addr)->sin_addr.s_addr) == INADDR_LOOPBACK)) {

	// Free the address information
	freeaddrinfo(results);

	// Request address information for the local host name
	results = NULL;
	getaddrinfo(getLocalHost().c_str(), NULL, &hints, &results);
	
    }

    // Did we get address information?
    if(results != NULL) {

	// Use the canonical name if one was provided
	if(results->ai_canonname != NULL)
	    canonical = results->ai_canonname;

	// Free the address information
	freeaddrinfo(results);

    }

    // Return the canonical name to the caller
    return canonical;
}

std::string Experiment::getHostnameFromIP(const std::string& ip)
{
    std::string  hostName;
    struct hostent *ht;
    struct in_addr s_addr;

    // Return ip if it is not a valid IP address.
    if ((inet_aton(ip.c_str(), &s_addr)) == 0) {
	return ip;
    }

    // Return ip if no host name can be resolved from the ip address.
    if ((ht = gethostbyaddr((char *)&s_addr, sizeof(s_addr), AF_INET)) == NULL) {
	return ip;
    }

    // Return the host name resolved from the ip address.
    hostName = ht->h_name;
    return hostName;
}



/**
 * Test accessibility of an experiment database.
 *
 * Returns a boolean value indicating if the specified experiment database is
 * accessible. Simply confirms that the database exists, is accessible by the
 * database engine, and contains a "Open|SpeedShop" table with a single row.
 * It does not check that any of the other tables match the experiment database
 * schema or that the tables are internally consistent.
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
    BEGIN_WRITE_TRANSACTION(database);
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
    
    // Update the experiment's database schema if necessary
    if(getVersion() == 1)
        updateToVersion2();
    if(getVersion() == 2)
        updateToVersion3();
    if(getVersion() == 3)
        updateToVersion4();
    if(getVersion() == 4)
        updateToVersion5();
    if(getVersion() == 5)
        updateToVersion6();
    if(getVersion() == 6)
        updateToVersion7();

#if (BUILD_INSTRUMENTOR == 1)
    // Iterate over each thread in this experiment
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator
            i = threads.begin(); i != threads.end(); ++i) {
        
        // Retain this thread in the instrumentor
        Instrumentor::retain(*i);
        
    }
#endif

    // Add this experiment's database to the data queues
    DataQueues::addDatabase(dm_database);
    
    // Set the experiment's rerun count to 0
    setRerunCount(-1);
    
    // Set the experiment's instrumentor type to default to offline
    setIsInstrumentorOffline(true);
    
#if ( BUILD_CBTF == 1 )
    setInstrumentorUsesCBTF(true);
#else
    setInstrumentorUsesCBTF(false);
#endif
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
    // Note: Experiment objects can be allocated on the stack and exceptions
    //       are thrown in several of the following operations under the right
    //       (wrong?) circumstances. So if the tool receives an exception, and
    //       if the subsequent stack unwind causes this destructor to be called,
    //       and IF one of the following operations throws an exception, then
    //       we are in big, big, trouble. The C++ standard specifies that the
    //       terminate() function is called immediately. Since there isn't a
    //       whole lot we can do about a failure in any of the following, we
    //       chose to ignore such failures for now.

    // Postpone all performance data collection
    try {
        getThreads().postponeCollecting(getCollectors());
    }
    catch(...) {
    }    

    // Remove this experiment's database from the data queues
    try {
        DataQueues::removeDatabase(dm_database);
    }
    catch(...) {
    }    

    // Iterate over each thread in this experiment
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
            i = threads.begin(); i != threads.end(); ++i) {
	    
        // Release this thread in the instrumentor
        Instrumentor::release(*i);
        
    }

    // Remove experiment's database from the various caches
    DataQueues::TheCache.removeDatabase(dm_database);
    Function::TheCache.removeDatabase(dm_database);
    Loop::TheCache.removeDatabase(dm_database);
    Statement::TheCache.removeDatabase(dm_database);
}




/**
 * Determine the name of the MPI implementation being used by the
 * given thread.  Possibilities are "mpich", "mpt", "openmpi".
 *
 * @param thread            The thread
 *
 * @return    string containing the name of the MPI implementation
 */
static std::string getMPIImplementationName(const Thread& thread)
{

#ifndef NDEBUG
/** Flag indicating if debuging for MPI jobs is enabled. */
bool is_debug_mpijob_enabled = (getenv("OPENSS_DEBUG_MPIJOB") != NULL);
#endif

#ifndef NDEBUG
   if(is_debug_mpijob_enabled) {
      std::stringstream output;
      output << "Enter Experiment::getMPIImplementationName" << " "
             << std::endl;
      std::cerr << output.str();
   }
#endif

#ifdef HAVE_MPI
    /* Automatic MPI Implementation Detection of the MPI application being analyzed */
    bool is_lam = false;
    bool is_lampi = false;
    bool is_openmpi = false;
    bool is_mpt = false;
    bool is_mpich2 = false;
    bool is_mpich1 = false;
    bool is_mvapich = false;
    bool is_mvapich2 = false;

    // Did we sucessfully create and connect to the thread?
    if(thread.isState(Thread::Suspended)) {

        // Is thread a "lampi" MPI implementation?
        if(thread.getFunctionByName("lampi_environ_init").first) {
            is_lampi = true;
        }

        // Is thread a "mpt" SGI MPT MPI implementation?
        if(thread.getFunctionByName("MPI_debug_breakpoint").first) {
            is_mpt = true;
        }

        // Is thread a "lam/mpi" MPI implementation?
        if(thread.getFunctionByName("lam_tv_init").first) {
            is_lam = true;
        }

        // Is thread a "openmpi" MPI implementation?
        if(thread.getFunctionByName("ompi_mpi").first ||
           thread.getFunctionByName("ompi_mpi_comm_world").first ||
           thread.getFunctionByName("orterun").first) {

            is_openmpi = true;

        }

    } else {

#ifndef NDEBUG
        if(is_debug_mpijob_enabled) {
           std::stringstream output;
           output << "In getMPIImplementationName THREAD IS NOT SUSPENDED, is it suspended" << thread.isState(Thread::Suspended) << " "
                  << "In getMPIImplementationName is it running? = " << thread.isState(Thread::Running) << " "
                  << "In getMPIImplementationName is it connecting? = " << thread.isState(Thread::Connecting) << " "
                  << "In getMPIImplementationName is it nonexistent? = " << thread.isState(Thread::Nonexistent) << " "
                  << "In getMPIImplementationName is it disconnected? = " << thread.isState(Thread::Disconnected) << " "
                  << "In getMPIImplementationName is it terminated? = " << thread.isState(Thread::Terminated) << " "
                  << std::endl;
           std::cerr << output.str();
         }
#endif
    }

    /*
     * The environment variable OPENSS_MPI_IMPLEMENTATION can be set
     * to override the automatic MPI implementation detection process.
     */

    const char* env_variable_name = "OPENSS_MPI_IMPLEMENTATION";
    const char* value = getenv(env_variable_name);

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
       std::stringstream output;
       output << "In Experiment::getMPIImplementationName getenv(OPENSS_MPI_IMPLEMENTATION), value= " << value << " "
              << std::endl;
       std::cerr << output.str();
    }
#endif

    // If no environment variable setting for the mpi implementation type
    // then use the automatically determined version.
    if (!value) {
       if (is_openmpi) {
         value = "openmpi";
       } else if (is_lampi) {
         value = "lampi";
       } else if (is_mpt) {
         value = "mpt";
       } else if (is_lam) {
         value = "lam";
       } else if (is_mpich2) {
         value = "mpich2";
       } else if (is_mpich1) {
         value = "mpich";
       } else if (is_mvapich) {
         value = "mvapich";
       } else if (is_mvapich2) {
         value = "mvapich2";
       } else {
         value = "";
       }
       
#ifndef NDEBUG
       if(is_debug_mpijob_enabled) {
           std::stringstream output;
           output << "In Experiment::getMPIImplementationName after boolean is_xxx checks, value= " << value << " "
                  << std::endl;
          std::cerr << output.str();
       }
#endif
    } 

    if (value) {
#if JEG_TARGET_CHANGE
	std::string impl_names = ALL_TARGET_MPI_IMPL_NAMES;
        if (impl_names.empty()) {
	   impl_names = ALL_MPI_IMPL_NAMES;
        }
#else
	std::string impl_names = ALL_MPI_IMPL_NAMES;
#endif
	std::string search_target = std::string(" ") + value + " ";
	if (impl_names.find(search_target) == std::string::npos) {
	    throw Exception(Exception::MPIImplChoiceInvalid,
			    value, impl_names);
	}
#ifndef NDEBUG
        if(is_debug_mpijob_enabled) {
            std::stringstream output;
            output << "RETURN from Experiment::getMPIImplementationName, value= " << value << " "
                   << std::endl;
            std::cerr << output.str();
        }
#endif

	return value;
    }

    /*
     * The old AC_PKG_MPI code in acinclude.m4 caused the entire
     * OpenSpeedShop build to use the first MPI implementation it
     * found.  This temporary code reproduces that behavior, with the
     * help of the new AC_PKG_MPI code.
     */
#if JEG_TARGET_CHANGE
    std::string return_name = DEFAULT_TARGET_MPI_IMPL_NAME;
    if (return_name.empty()) {
      return_name = DEFAULT_MPI_IMPL_NAME;
    return return_name;
#else
    return DEFAULT_MPI_IMPL_NAME;
#endif

#else /* ifndef HAVE_MPI */
    Assert(false);
    return "";
#endif /* ifndef HAVE_MPI */

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
 *
 * Get our experiment database version.
 *
 * Returns the version of the database for this experiment.
 *
 * @return    Version of the database for this experiment.
 */
int Experiment::getVersion() const
{
    int version = 0;

    // Find our version
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement("SELECT version FROM \"Open|SpeedShop\";");
    while(dm_database->executeStatement())
	version = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);

    // Return the version to the caller
    return version;
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
 * @param numBE              Min number of mrnet backends if using cbtf.
 * @param stdout_callback    Standard output stream callback for the process.
 * @param stderr_callback    Standard error stream callback for the process.
 * @return                   Newly created threads.
 */
ThreadGroup Experiment::createProcess(
    const std::string& command,
    const std::string& host,
    uint64_t numBE,
    const OutputCallback stdout_callback,
    const OutputCallback stderr_callback) const
{   
    ThreadGroup threads;
    bool is_mpt_job = false;
    bool is_mpich_job = false;

    // Get the canonical name of this host
    std::string canonical = getCanonicalName(host);

    // Allocate the thread outside the transaction's try/catch block
    Thread thread;

    //std::cerr << "INSERT INTO Threads (command, host) " << command
    //    << " , " << canonical << std::endl;
    //std::cerr << "INSERT INTO Threads numBE=" << numBE << std::endl;

    // Create the thread entry in the database
    BEGIN_WRITE_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"INSERT INTO Threads (command, host) VALUES (?, ?);"
	);
    dm_database->bindArgument(1, command);
    dm_database->bindArgument(2, canonical);
    while(dm_database->executeStatement());
    thread = Thread(dm_database, dm_database->getLastInsertedUID());
    threads.insert(thread);
    END_TRANSACTION(dm_database);

    //std::cerr << "Thread constructer with pid " << thread.getProcessId()
    //    <<  std::endl;

    // Create the underlying thread for executing the command
    try {
	// Instrumentor::retain(thread);  (Implied by Following Line)
#if defined(BUILD_CBTF)
	// will need to pass the collector from the cli...
	//std::string collector("pcsamp");
	std::string collector;
	CollectorGroup current_cgrp = getCollectors();
	CollectorGroup::iterator ci;
	for (ci = current_cgrp.begin(); ci != current_cgrp.end(); ci++) {
	    Collector C = *ci;
	    collector = C.getMetadata().getUniqueId();
	}
	Instrumentor::create(thread, command,
			     collector, numBE, instrumentorUsesCBTF,
			     stdout_callback, stderr_callback);
#else
	Instrumentor::create(thread, command, stdout_callback, stderr_callback);
#endif
    }
    catch(...) {

	// Remove this thread from the database
	BEGIN_WRITE_TRANSACTION(dm_database);
	dm_database->prepareStatement("DELETE FROM Threads WHERE id = ?;");
	dm_database->bindArgument(1, EntrySpy(thread).getEntry());
	while(dm_database->executeStatement());
	END_TRANSACTION(dm_database);

	// Re-throw exception upwards
	throw;

    }

    //std::cerr << "After Instrumentor::create with pid "
    //    << thread.getProcessId() <<  std::endl;
    //std::cerr << "THREAD STATE "
    //    << OpenSpeedShop::Framework::toString(thread.getState())
    //    << std::endl;
 
    // Wait until the thread finishes connecting
    while(thread.isState(Thread::Connecting))
	suspend();
    
    // Did we sucessfully create and connect to the thread?
    if(thread.isState(Thread::Suspended)) {

	// Initially assume thread isn't an "mpirun" process
	std::pair<bool, std::string> is_mpirun(false, "");

	// Is thread a "mpirun" process for an MPICH-style MPI implementation?
	if(thread.getFunctionByName("MPIR_Breakpoint").first) {
	    is_mpirun = std::make_pair(true, "MPIR_Breakpoint");
	    is_mpich_job = true;
	    is_mpt_job = false;
        }

	// Is thread a "mpirun" process for the SGI MPT MPI implementation?
	if(thread.getFunctionByName("MPI_debug_breakpoint").first) {
	    is_mpirun = std::make_pair(true, "MPI_debug_breakpoint");
	    is_mpich_job = false;
	    is_mpt_job = true;
        }

	// Martin: add environment variable to ignore MPI attach
	if (is_mpirun.first)
	  {
	    if (getenv("OPENSS_DISABLE_MPISTARTUP")!=NULL)
	      {
		is_mpirun.first=false;
	      }
	  }
	
#ifndef NDEBUG
        if(is_debug_mpijob_enabled) {
            std::stringstream output;
            output << "[TID " << pthread_self() << "] "
                   << " Experiment::createProcess, thread.getProcessId() ="
		   << thread.getProcessId() << " is_mpich_job=" << is_mpich_job
                   << " is_mpt_job=" << is_mpt_job << " is_mpirun.first="
		   << is_mpirun.first
                   << " is_mpirun.second=" << is_mpirun.second
		   << "thread.getHost()=" << thread.getHost()
                   << std::endl;
            std::cerr << output.str();
        }
#endif


	// Is this thread a "mpirun" process for a supported MPI implementation?
	if(is_mpirun.first) {

	    // Notify the Instrumentor that this thread is the mpi starter thread.
	    // The instrumentor will disable addressSpaceChangeCallback and
	    // threadListChangeCallback while running up to the MPIR_Breakpoint.
	    Instrumentor::setMPIStartup(thread,true);

	    // Insert a stop at the entry of the stop function
	    Instrumentor::stopAtEntryOrExit(thread, is_mpirun.second, true);

	    // Martin: mark the process as being debugged
            // Only do the MPIR_debug_gate code for mpich type MPI implementations
            // Not mpt.
            if (is_mpich_job) {
	      Instrumentor::setGlobal(thread,"MPIR_being_debugged",1);
            }

	    // Resume the thread
	    thread.changeState(Thread::Running);
	    
	    // Wait until the thread reaches the stop point
	    suspend();
	    while(!thread.isState(Thread::Suspended)) {
		suspend();
	    }
	    
	    // Notify Instrumentor that it is safe to restore the addressSpaceChangeCallback
	    // and threadListChangeCallback.
	    Instrumentor::setMPIStartup(thread,false);

	    // Attach to the entire MPI job
	    threads = attachMPIJob(thread.getProcessId(), thread.getHost());

            // Only do the MPIR_debug_gate code for mpich type MPI implementations
            // Not mpt.
            if (is_mpich_job) {
               for(ThreadGroup::const_iterator
	   	       i = threads.begin(); i != threads.end(); ++i) {
                   Instrumentor::setGlobal(*i,"MPIR_debug_gate",1);
               }
            }


	}

    }

    //std::cerr << "Thread RETURN pid " << thread.getProcessId() <<  std::endl;
    //std::cerr << "RETURNING FROM Experiment::createProcess threads size = "
    //    << threads.size() << std::endl;
 
    // Return the threads to the caller
    return threads;
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
#if HAVE_MPI
    std::string mpi_implementation_name = "";
#endif

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Experiment::attachMPIJob(" << pid << ", \"" << host << "\")"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Attach to the specified process
    threads = attachProcess(pid, host);

    // Is there at least one connected (suspended) thread in this process?
    ThreadGroup connected = threads.getSubsetWithState(Thread::Suspended);
    if(connected.empty())
	return threads;

    // Look for any available MPI job information in this process
    Job job;
    bool is_mpt_job = getMPIJobFromMPT(*(connected.begin()), job);
    bool is_mpich_job = is_mpt_job ? false : getMPIJobFromMPICH(*(connected.begin()), job);

#if HAVE_MPI
    // If it is an mpi job go examine the mpirun/srun mpi driver to see what
    // mpi implementation is being activated inside openss.

    if (is_mpt_job || is_mpich_job) {

      mpi_implementation_name = getMPIImplementationName(*(connected.begin()));

      // Update this process' MPI implementation in the database
      for(ThreadGroup::const_iterator i = connected.begin(); i != connected.end(); ++i) {
#ifndef NDEBUG
         if(is_debug_mpijob_enabled) {
           fprintf(stderr, "setting mpi_implementation_name=%s\n", mpi_implementation_name.c_str());
         }
#endif
         (*i).setMPIImplementation(mpi_implementation_name);
      }

    }
#endif
    
   
    // Canonicalize the host names in the MPI job information
    for(Job::iterator i = job.begin(); i != job.end(); ++i)
	*i = std::make_pair(getCanonicalName(i->first), i->second);

    // Attach to this MPI job
    connected = attachJob(job);

    // Add the threads in this MPI job to the overall thread group
    threads.insert(connected.begin(), connected.end());

    // Note: A process' position in the MPICH process table also indicates its
    //       MPI rank, starting with zero. That ordering is preserved in the Job
    //       object constructed by getMPIJobFromMPICH(). So the rank number to
    //       be put into the mpi_rank database field can be found simply by
    //       incrementing the rank as we iterate through the job table. Things
    //       aren't so easy for MPT. There we have to go to the process and
    //       find the value of the global symbol "MPI_debug_rank" instead.

    // Iterate over all processes found to be in this MPI job
    int rank = 0;
    for(Job::const_iterator i = job.begin(); i != job.end(); ++i) {
	
	// Is this an MPT job?
	if(is_mpt_job) {
	    
	    // Get the list of threads in this process
	    ThreadGroup in_process = getThreadsInProcess(i->second, i->first);
	    
	    // Skip setting the rank number if it cannot be accessed
	    int64_t mpt_rank;
            if(in_process.empty() || ( Instrumentor::getGlobal(*(in_process.begin()),
                                       "MPI_debug_rank",
                                       mpt_rank) == false )) {
		continue;
            }
	    rank = static_cast<int>(mpt_rank);
	    
	}
	
	// Update this process' MPI rank in the database
	BEGIN_WRITE_TRANSACTION(dm_database);
	dm_database->prepareStatement(
	    "UPDATE Threads SET mpi_rank = ? WHERE host = ? AND pid = ?;"
	    );
	dm_database->bindArgument(1, rank);
	dm_database->bindArgument(2, i->first);
	dm_database->bindArgument(3, i->second);	
	while(dm_database->executeStatement());
	END_TRANSACTION(dm_database);

#if HAVE_MPI
	// Update this process' MPI implementation in the database
//        *i->setMPI_Impl(mpi_implementation_name);
//        if(is_debug_mpijob_enabled) {
//          fprintf(stderr, "UPDATE Threads SET mpi_impl, mpi_implementation_name = %s \n", mpi_implementation_name.c_str());
//        }

	// Update this process' MPI implementation in the database
	BEGIN_WRITE_TRANSACTION(dm_database);
	dm_database->prepareStatement(
	    "UPDATE Threads SET mpi_impl = ? WHERE host = ? AND pid = ?;");
	dm_database->bindArgument(1, mpi_implementation_name);
	dm_database->bindArgument(2, i->first);
	dm_database->bindArgument(3, i->second);	
	while(dm_database->executeStatement());
	END_TRANSACTION(dm_database);
#endif

	// Is this an "MPICH" job?
	if(is_mpich_job)
	    ++rank;
	
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
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 * @return        Attached threads.
 */
ThreadGroup Experiment::attachProcess(const pid_t& pid,
				      const std::string& host) const
{
    // Get the canonical name of this host
    std::string canonical = getCanonicalName(host);

    // Construct a job containing this process
    Job job;
    job.push_back(std::make_pair(canonical, pid));

    // Attach to this job
    ThreadGroup threads = attachJob(job);

    // Return the threads to the caller
    return attachJob(job);
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
    Assert(dm_database == EntrySpy(thread).getDatabase());

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);
    EntrySpy(thread).validate();

    // Stop all performance data collection for this thread
    thread.getCollectors().stopCollecting(thread);
    thread.getPostponedCollectors().stopCollecting(thread);
    
    // Release this thread in the instrumentor
    Instrumentor::release(thread);
    
    // Remove this thread from the performance data cache
    DataQueues::TheCache.removeThread(thread);

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
	"WHERE id "
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

    // Remove unused function ranges
    dm_database->prepareStatement(
	"DELETE FROM FunctionRanges "
	"WHERE function "
	"  NOT IN (SELECT DISTINCT id FROM Functions)"
	);
    while(dm_database->executeStatement());

    // Remove unused loops
    dm_database->prepareStatement(
        "DELETE FROM Loops "
        "WHERE linked_object "
        "  NOT IN (SELECT DISTINCT linked_object FROM AddressSpaces);"
        );
    while(dm_database->executeStatement());

    // Remove unused loop ranges
    dm_database->prepareStatement(
        "DELETE FROM LoopRanges "
        "WHERE loop "
        "  NOT IN (SELECT DISTINCT id FROM Loops)"
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
    BEGIN_WRITE_TRANSACTION(dm_database);

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
    Assert(dm_database == EntrySpy(collector).getDatabase());
    
    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);
    EntrySpy(collector).validate();
    
    // Stop all performance data collection for this collector
    collector.getThreads().stopCollecting(collector);
    collector.getPostponedThreads().stopCollecting(collector);

    // Remove this collector from the performance data cache
    DataQueues::TheCache.removeCollector(collector);
    
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
 * Get functions by name pattern.
 *
 * Returns the functions that match the passed name pattern. An empty set is
 * returned if no such function can be found. Wildcards are expressed using
 * standard UNIX file globbing syntax (e.g. "pthread_*").
 *
 * @param pattern    Name pattern to find.
 * @return           Functions matching this name pattern.
 */
std::set<Function>
Experiment::getFunctionsByNamePattern(const std::string& pattern) const
{
    std::set<Function> functions;

    // Find the functions matching this name
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"SELECT id FROM Functions WHERE name GLOB ?;"
	);
    dm_database->bindArgument(1, pattern);
    while(dm_database->executeStatement())
	functions.insert(Function(dm_database,
				  dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the functions to the caller
    return functions;
}



/**
 * Get linked objects by path pattern.
 *
 * Returns the linked objects that match the passed path pattern. An empty set
 * is returned if no such linked object can be found. Wildcards are expressed
 * using standard UNIX file globbing syntax (e.g. "libpthread*").
 *
 * @param pattern    Path pattern to find.
 * @return           Linked objects matching this path pattern.
 */
std::set<LinkedObject>
Experiment::getLinkedObjectsByPathPattern(const std::string& pattern) const
{
    std::set<LinkedObject> linked_objects;

    // Find the linked objects matching this path
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"SELECT LinkedObjects.id "
	"FROM LinkedObjects "
	"  JOIN Files "
	"ON LinkedObjects.file = Files.id "
	"WHERE Files.path GLOB ?;"
	);
    dm_database->bindArgument(1, pattern);
    while(dm_database->executeStatement())
	linked_objects.insert(LinkedObject(dm_database,
					   dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the linked objects to the caller
    return linked_objects;
}



/**
 * Get functions by path pattern.
 *
 * Returns the functions that match the passed path pattern. An empty set is
 * returned if no such function can be found. Wildcards are expressed using
 * standard UNIX file globbing syntax (e.g. "libpthread*").
 *
 * @todo    Currently the performance of this query is less than optimal.
 *          Improvements can probably be made once the speed of address/address
 *          queries on the database have been accelerated.
 *
 * @param pattern    Path pattern to find.
 * @return           Functions matching this path pattern.
 */
std::set<Function>
Experiment::getFunctionsByPathPattern(const std::string& pattern) const
{
    std::set<Function> functions;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);

    // Find the statements matching this path
    std::set<Statement> statements = getStatementsByPathPattern(pattern);

    // Iterate over each of these statements
    for(std::set<Statement>::const_iterator
	    i = statements.begin(); i != statements.end(); ++i) {
	
	// Find the functions containing this statement
	std::set<Function> functions_in_statement = i->getFunctions();

	// Add these functions to the results
	functions.insert(functions_in_statement.begin(),
			 functions_in_statement.end());

    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Return the functions to the caller
    return functions;
}



/**
 * Get statements by path pattern.
 *
 * Returns the statements that match the passed path pattern. An empty set is
 * returned if no such statement can be found. Wildcards are expressed using
 * standard UNIX file globbing syntax (e.g. "libpthread*"). 
 *
 * @param pattern    Path pattern to find.
 * @return           Statements matching this path pattern.
 */
std::set<Statement> 
Experiment::getStatementsByPathPattern(const std::string& pattern) const
{
    std::set<Statement> statements;

    // Find the statements matching this path
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"SELECT Statements.id "
	"FROM Statements "
	"  JOIN Files "
	"ON Statements.file = Files.id "
	"WHERE Files.path GLOB ?;"
	);
    dm_database->bindArgument(1, pattern);
    while(dm_database->executeStatement())
	statements.insert(Statement(dm_database,
				    dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the statements to the caller
    return statements;
}



/**
 * Flush performance data.
 *
 * Flushes any queued performance data for this experiment to the experiment's
 * database. All incoming performance data will eventually be flushed to the
 * experiment's database irrespective of whether or not this function is ever
 * called. Utilizing this function simply allows a tool to give hints as to
 * when it would like to see the flush happen (e.g. immediately after all the
 * processes in an experiment terminate).
 */
void Experiment::flushPerformanceData() const
{
    // Flush performance data in the data queues for this experiment's database
    DataQueues::flushDatabase(dm_database);
}



/**
 * Get performance data extent.
 *
 * Returns the extent of performance data within this experiment. An empty
 * extent is returned if there is no performance data in this experiment.
 *
 * @return    Extent of the performance data in this experiment.
 */
Extent Experiment::getPerformanceDataExtent() const
{
    Extent extent;

    // Find our performance data extent
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"SELECT time_begin, time_end, addr_begin, addr_end FROM Data;"
	);
    while(dm_database->executeStatement())
	if(extent.isEmpty())
	    extent = Extent(TimeInterval(dm_database->getResultAsTime(1),
					 dm_database->getResultAsTime(2)),
			    AddressRange(dm_database->getResultAsAddress(3),
					 dm_database->getResultAsAddress(4)));
	else
	    extent |= Extent(TimeInterval(dm_database->getResultAsTime(1),
					  dm_database->getResultAsTime(2)),
			     AddressRange(dm_database->getResultAsAddress(3),
					  dm_database->getResultAsAddress(4)));
    END_TRANSACTION(dm_database);

    // Return the extent to the caller
    return extent;
}

/**
 *
 *  This routine extracts the command that was or will be 
 *  used to run the application that is the subject of this
 *  experiment. 
 *
 */
std::string Experiment::getApplicationCommand() 
{

    std::string EmptyString = "";

    // Table mapping original thread names to their corresponding collectors
    std::vector<std::pair<ThreadName, CollectorGroup> > names_to_collectors;

    // Iterate over each original thread in this experiment
    ThreadGroup new_threads, original_threads = getThreads();
    for(ThreadGroup::const_iterator
	    i = original_threads.begin(); i != original_threads.end(); ++i) {

	// Construct the name of this thread and add it to the table
	ThreadName name(*i);
	names_to_collectors.push_back(std::make_pair(name, i->getCollectors()));

	// Ignore this thread if it wasn't created directly by the framework
	if(name.getCommand().first) {
          return name.getCommand().second;

#ifndef NDEBUG
          if(is_debug_mpijob_enabled) {
	      std::stringstream output;
      	      output << "[TID " << pthread_self() << "] "
	             << "Experiment::getApplicationCommand: name.getCommand().first=" 
	             << name.getCommand().first  << " name.getCommand().second= " << name.getCommand().second << ""
	             << " name.getHost()= "  <<  name.getHost() << ""
	             << std::endl;
      	      std::cerr << output.str();
          }
#endif
        }

   } // end for
   return EmptyString;
}

/**
 *
 *  This routine stores/saves the command that will be 
 *  used to run the application that is the subject of this
 *  experiment. 
 *
 */
void Experiment::setApplicationCommand(const char *newCommand, bool trust_me) 
{
    // Table mapping original thread names to their corresponding collectors
    std::vector<std::pair<ThreadName, CollectorGroup> > names_to_collectors;

    // Iterate over each original thread in this experiment
    ThreadGroup new_threads, original_threads = getThreads();
    for(ThreadGroup::const_iterator
	    i = original_threads.begin(); i != original_threads.end(); ++i) {

	// Construct the name of this thread and add it to the table
	ThreadName name(*i);
	names_to_collectors.push_back(std::make_pair(name, i->getCollectors()));

	// Ignore this thread if it wasn't created directly by the framework
	if(trust_me || name.getCommand().first) {
           // Set the application command
          (*i).setCommand(newCommand);
#ifndef NDEBUG
          if(is_debug_mpijob_enabled) {
	      std::stringstream output;
      	      output << "[TID " << pthread_self() << "] "
	             << "Experiment::setApplicationCommand: name.getCommand().first=" 
	             << name.getCommand().first  << " name.getCommand().second= " << name.getCommand().second << ""
	             << " name.getHost()= "  <<  name.getHost() << ""
	             << std::endl;
      	      std::cerr << output.str();
          }
#endif
        }
   } // end for
}

/**
 * Prepare to rerun the experiment.
 *
 * this experiment is reissued. The resulting new threads are matched against
 * the original threads when possible. This allows the data collectors applied
 * to the new threads to mimic those applied to the original threads. Original
 * threads are then removed, leaving the new threads ready to be run.
 *
 * @todo    Using a vector to hold the table mapping original thread names
 *          to their corresponding collectors is less than optimal since it
 *          requires a linear-time search. A map would be a better choice
 *          but a resonable definition of less-than for ThreadName objects
 *          will be required first.
 *
 * @param stdout_callback    Standard output stream callback for the processes.
 * @param stderr_callback    Standard error stream callback for the processes.
 */
void Experiment::prepareToRerun(const OutputCallback stdout_callback,
				const OutputCallback stderr_callback) const
{
    // Table mapping original thread names to their corresponding collectors
    std::vector<std::pair<ThreadName, CollectorGroup> > names_to_collectors;

    // Iterate over each original thread in this experiment
    ThreadGroup new_threads, original_threads = getThreads();
    for(ThreadGroup::const_iterator
	    i = original_threads.begin(); i != original_threads.end(); ++i) {

	// Construct the name of this thread and add it to the table
	ThreadName name(*i);
	names_to_collectors.push_back(std::make_pair(name, i->getCollectors()));

	// Ignore this thread if it wasn't created directly by the framework
	if(!name.getCommand().first)
	    continue;

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Experiment::prepareToRerun: name.getCommand().first=" 
	       << name.getCommand().first  << " name.getCommand().second= " << name.getCommand().second << ""
	       << " name.getHost()= "  <<  name.getHost() << ""
	       << std::endl;
	std::cerr << output.str();
    }
#endif

	// Recreate the process on the same host
        ThreadGroup threads = createProcess(name.getCommand().second, 
					    name.getHost(), 0,
					    stdout_callback, stderr_callback);
	new_threads.insert(threads.begin(), threads.end());

    }

    // Iterate over each new thread added to this experiment
    for(ThreadGroup::const_iterator
	    i = new_threads.begin(); i != new_threads.end(); ++i) {
	ThreadName name(*i);

	// Look for an original thread matching this one
	for(std::vector<std::pair<ThreadName, CollectorGroup> >::iterator
		j = names_to_collectors.begin();
	    j != names_to_collectors.end();
	    ++j)
	    if(j->first == name) {

		// Apply collectors from the original thread to the new thread
		j->second.startCollecting(*i);

		// Allow each original thread to be used only once
		names_to_collectors.erase(j);

		break;
	    }

    }

    // Remove all the original threads from the experiment
    for(ThreadGroup::const_iterator
	    i = original_threads.begin(); i != original_threads.end(); ++i)
	removeThread(*i);
}



/**
 * Get our executables.
 *
 * Returns the set of linked objects which are executables. An empty set is
 * returned if no executables can be found.
 *
 * @return    Executables in this experiment.
 */
std::set<LinkedObject> Experiment::getExecutables() const
{
    std::set<LinkedObject> executables;

    // Find all the executables
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"SELECT id from LinkedObjects WHERE is_executable = 1;"
	);
    while(dm_database->executeStatement())
	executables.insert(
            LinkedObject(dm_database, dm_database->getResultAsInteger(1))
	    );
    END_TRANSACTION(dm_database);

    // Return the executables to the caller
    return executables;
}



/**
 * Attach to a job.
 *
 * Initiates attaches to the processes in the specified job <em>if</em> those
 * processes are not already found within the experiment database. Waits until
 * the issued attaches complete and returns the previously and newly connected
 * threads.
 *
 * @param job    Job which is to be attached.
 * @return       Previously and newly attached threads.
 */
ThreadGroup Experiment::attachJob(const Job& job) const
{
    ThreadGroup threads;
    
    // Allocate the thread group outside the transaction's try/catch block
    ThreadGroup connecting;

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);

    // Iterate over each process in this job
    for(Job::const_iterator i = job.begin(); i != job.end(); ++i) {

	// Get any existing threads in this process from the database
	ThreadGroup existing = getThreadsInProcess(i->second, i->first);
    
	// Were there any existing threads in this process?
	if(!existing.empty()) {

	    // Add these threads to the overall thread group
	    threads.insert(existing.begin(), existing.end());
	    
	}
	else {

	    // Create a thread entry in the database
	    dm_database->prepareStatement(
	        "INSERT INTO Threads (host, pid) VALUES (?, ?);"
		);
	    dm_database->bindArgument(1, i->first);
	    dm_database->bindArgument(2, i->second);
	    while(dm_database->executeStatement());
	    Thread thread(dm_database, dm_database->getLastInsertedUID());

	    // Add this thread to the connecting thread group
	    connecting.insert(thread);
	    
	}

    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 

#if (BUILD_INSTRUMENTOR == 1)
    // Initiate the attaches if necessary
    if(!connecting.empty()) {

	// Iterate over each thread in the connecting thread group
	for(ThreadGroup::const_iterator 
		i = connecting.begin(); i != connecting.end(); ++i) {
	    
	    // Retain this thread in the instrumentor
	    Instrumentor::retain(*i);
	    
	}

	// Connect to the threads in the instrumentor
	Instrumentor::changeState(connecting, Thread::Connecting);

    }
#endif

    // Iterate until we are done connecting to processes
    while(!connecting.empty()) {
	
	// Wait until at least one process is no longer connecting (completed)
	while(connecting.areAllState(Thread::Connecting))
	    suspend();

	// Separate into groups of incomplete and completed processes
	ThreadGroup incomplete, completed;
	for(ThreadGroup::const_iterator 
		i = connecting.begin(); i != connecting.end(); ++i)
	    if(i->isState(Thread::Connecting))
		incomplete.insert(*i);
	    else
		completed.insert(*i);

	// Iterate over each of the completed processes
	for(ThreadGroup::const_iterator 
		i = completed.begin(); i != completed.end(); ++i) {
	    
	    // Get the list of threads in this process
	    ThreadGroup in_process = 
		getThreadsInProcess(i->getProcessId(), i->getHost());
	    	    
	    // Merge these additional threads into the overall thread group
	    threads.insert(in_process.begin(), in_process.end());
	    
	}

	// Indicate the incomplete processes are the ones still connecting
	connecting = incomplete;
	
    }

    // Return the threads to the caller
    return threads;
}



/**
 * Get the threads in a process.
 *
 * Returns (from the experiment database) all the threads within the specified
 * process. An empty thread gruop is returned if no threads are contained within
 * the process.
 *
 * @param pid     Process identifier for the process.
 * @param host    Name of the host on which the process resides.
 * @return        Threads within that process.
 */
ThreadGroup Experiment::getThreadsInProcess(const pid_t& pid, 
					    const std::string& host) const
{
    ThreadGroup threads;
    
    // Find the threads in the specified process
    BEGIN_TRANSACTION(dm_database);
    dm_database->prepareStatement(	
	"SELECT id FROM Threads WHERE host = ? AND pid = ?;"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    while(dm_database->executeStatement())
	threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);

    // Return the threads to the caller
    return threads;
}



/**
 * Update our schema to version 2.
 *
 * Updates the schema of this experiment's database to version 2. Adds an empty
 * command column to the thread table, and updates the database's schema version
 * number.
 */
void Experiment::updateToVersion2() const
{
    // Update procedure
    const char* UpdateProcedure[] = {

	// Add a column field to the Threads table
	"ALTER TABLE Threads ADD COLUMN command TEXT DEFAULT NULL;",
	
	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 2;",

	// End Of Table Entry
	NULL
    };

    // Apply the update procedure
    BEGIN_WRITE_TRANSACTION(dm_database);
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
	dm_database->prepareStatement(UpdateProcedure[i]);
	while(dm_database->executeStatement());
    }
    END_TRANSACTION(dm_database);
}



/**
 * Update our schema to version 3.
 *
 * Updates the schema of this experiment's database to version 3. Adds an empty
 * MPI implementation column to the thread table, and updates the database's
 * schema version number.
 */
void Experiment::updateToVersion3() const
{
    // Update procedure
    const char* UpdateProcedure[] = {

	// Add a MPI implementation field to the Threads table
	"ALTER TABLE Threads ADD COLUMN mpi_impl TEXT DEFAULT NULL;",
	
	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 3;",

	// End Of Table Entry
	NULL
    };

    // Apply the update procedure
    BEGIN_WRITE_TRANSACTION(dm_database);
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
	dm_database->prepareStatement(UpdateProcedure[i]);
	while(dm_database->executeStatement());
    }
    END_TRANSACTION(dm_database);
}



/**
 * Update our schema to version 4.
 *
 * Updates the schema of this experiment's database to version 4. Translates
 * the existing function information into the new format, removes the older
 * information, and updates the database's schema version number.
 */
void Experiment::updateToVersion4() const
{
    // Update procedure
    const char* UpdateProcedure[] = {

	// Remove indicies on the existing Functions table
        "DROP INDEX IF EXISTS IndexFunctionsByName;",
        "DROP INDEX IF EXISTS IndexFunctionsByLinkedObject;",

	// Temporarily rename the existing Functions table
        "ALTER TABLE Functions RENAME TO OldFunctions;",
	
	// Create the new Function table and its indicies
        "CREATE TABLE Functions ("
        "    id INTEGER PRIMARY KEY,"
        "    linked_object INTEGER," // From LinkedObjects.id
        "    name TEXT"
	");",
        "CREATE INDEX IndexFunctionsByName ON Functions (name);",
        "CREATE INDEX IndexFunctionsByLinkedObject "
        "  ON Functions (linked_object);",

	// Create the new FunctionRanges table and its index
        "CREATE TABLE FunctionRanges ("
        "    function INTEGER," // From Functions.id
        "    addr_begin INTEGER,"
        "    addr_end INTEGER,"
        "    valid_bitmap BLOB"
        ");",
        "CREATE INDEX IndexFunctionRangesByFunction "
        "  ON FunctionRanges (function);",

	// Translate the existing information into the new format
	"INSERT INTO Functions"
	"  SELECT id, linked_object, name FROM OldFunctions;",
	"INSERT INTO FunctionRanges (function, addr_begin, addr_end)"
	"  SELECT id, addr_begin, addr_end FROM OldFunctions;",

	// Remove the previous Functions table
	"DROP TABLE OldFunctions;",

	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 4;",

	// End Of Table Entry
	NULL
    };

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);
    
    // Apply the update procedure
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
	dm_database->prepareStatement(UpdateProcedure[i]);
	while(dm_database->executeStatement());
    }

    // Find all the function ranges in this database
    dm_database->prepareStatement(
        "SELECT function, addr_begin, addr_end FROM FunctionRanges;"
	);
    while(dm_database->executeStatement()) {

	int function = dm_database->getResultAsInteger(1);
	Address addr_begin = dm_database->getResultAsAddress(2);
	Address addr_end = dm_database->getResultAsAddress(3);
	
	// Construct a valid bitmap for this (entire) function range
	AddressBitmap valid_bitmap(AddressRange(addr_begin, addr_end));
	for(Address addr = addr_begin; addr < addr_end; ++addr)
	    valid_bitmap.setValue(addr, true);
	
	// Set the valid bitmap of this function range
	dm_database->prepareStatement(
	    "UPDATE FunctionRanges "
	    "SET valid_bitmap = ? "
	    "WHERE function = ? "
	    "  AND addr_begin = ? "
	    "  AND addr_end = ?;"
	    );
	dm_database->bindArgument(1, valid_bitmap.getBlob());
	dm_database->bindArgument(2, function);
	dm_database->bindArgument(3, addr_begin);
	dm_database->bindArgument(4, addr_end);
	while(dm_database->executeStatement());	
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);    
}




/**
 * Update our schema to version 5.
 *
 * Updates the schema of this experiment's database to version 5
 * add new indexs for Data table. Improves performance of building
 * the DataCache where a full table scan was being performed on
 * the data table when selecting rowid's for the cache.
 * Performance issue was identified by a 6000 pe pepc-pcsamp databsae.
 */
void Experiment::updateToVersion5() const
{
    // Update procedure
    const char* UpdateProcedure[] = {

	"CREATE INDEX IndexDataByCollectorThread ON Data (collector,thread);",
	"CREATE INDEX IndexThreadByHostPidTid ON Threads(host,pid,posix_tid);",

	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 5;",

	// End Of Table Entry
	NULL
    };

    // Apply the update procedure
    BEGIN_WRITE_TRANSACTION(dm_database);
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
	dm_database->prepareStatement(UpdateProcedure[i]);
	while(dm_database->executeStatement());
    }
    END_TRANSACTION(dm_database);
}



/**
 * Update our schema to version 6.
 *
 * Updates the schema of this experiment's database to version 6. Adds empty
 * loop tables and updates the database's schema version number.
 */
void Experiment::updateToVersion6() const
{
    // Update procedure
    const char* UpdateProcedure[] = {

    // Loop Table
	"CREATE TABLE Loops ("
	"    id INTEGER PRIMARY KEY,"
	"    linked_object INTEGER," // From LinkedObjects.id
    "    addr_head INTEGER"
	");",
	"CREATE INDEX IndexLoopsByLinkedObject "
	"  ON Loops (linked_object);",
    
    // Loop Range Table
	"CREATE TABLE LoopRanges ("
	"    loop INTEGER," // From Loops.id
	"    addr_begin INTEGER,"
	"    addr_end INTEGER,"
	"    valid_bitmap BLOB"
	");",
    "CREATE INDEX IndexLoopRangesByLoop "
	"  ON LoopRanges (loop);",
	
	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 6;",

	// End Of Table Entry
	NULL
    };

    // Apply the update procedure
    BEGIN_WRITE_TRANSACTION(dm_database);
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
        dm_database->prepareStatement(UpdateProcedure[i]);
        while(dm_database->executeStatement());
    }
    END_TRANSACTION(dm_database);
}


/**
 * Update our schema to version 7.
 *
 * Updates the schema of this experiment's database to version 7. Adds empty
 * view reuse table and updates the database's schema version number.
 */
void Experiment::updateToVersion7() const
{
    // Update procedure
    const char* UpdateProcedure[] = {
    // create table  VIEWS  ( id INTEGER PRIMARY KEY, view_cmd TEXT DEFAULT NULL,  view_data BLOB DEFAULT NULL); 

    // View Reuse Table
	"CREATE TABLE IF NOT EXISTS Views ("
	"    id INTEGER PRIMARY KEY,"
	"    view_cmd TEXT," 
        "    view_data BLOB"
	");",
	
	// Update the database's schema version number
	"UPDATE \"Open|SpeedShop\" SET version = 7;",

	// End Of Table Entry
	NULL
    };

    // Apply the update procedure
    BEGIN_WRITE_TRANSACTION(dm_database);
    for(int i = 0; UpdateProcedure[i] != NULL; ++i) {
        dm_database->prepareStatement(UpdateProcedure[i]);
        while(dm_database->executeStatement());
    }
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
bool Experiment::getMPIJobFromMPT(const Thread& thread, Job& job)
{
#ifdef HAVE_ARRAYSVCS

    bool is_mpt_job = true;
    int64_t ash = 0;
    std::string array = "";

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Experiment::getMPIJobFromMPT("
	       << thread.getHost() << ":" << thread.getProcessId() << ", ...)"
	       << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Attempt to access the ASH and array name from this thread
    is_mpt_job &= Instrumentor::getGlobal(thread, "MPI_debug_ash", ash);
    is_mpt_job &= Instrumentor::getGlobal(thread, "MPI_debug_array", array);

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "is_mpt_job = " << (is_mpt_job ? "true" : "false")
	       << std::endl
	       << "[TID " << pthread_self() << "] "
	       << "MPI_debug_ash   = " << ash << std::endl
	       << "[TID " << pthread_self() << "] "
	       << "MPI_debug_array = \"" << array << "\"" << std::endl;
	std::cerr << output.str();
    }
#endif
    
    // Go no further if this thread isn't in an MPT MPI job
    if(!is_mpt_job)
	return false;
    
    // Open the MPT array services server on the host where this thread resides
    asserver_t server = asopenserver(thread.getHost().c_str(), -1);

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "asopenserver(\"" << thread.getHost().c_str() 
	       << "\", -1) = " << server 
	       << std::endl;
	if(server == 0) {
	    output << "[TID " << pthread_self() << "] "
		   << "asgeterror() = " << asgeterror()
		   << " (\"" << asstrerror(asgeterror()) << "\")" << std::endl;
	}
	std::cerr << output.str();
    }
#endif
      
    // Go no further if the MPT array services server couldn't be opened
    if(server == 0)
	return false;

    // Get the list of hosts and their corresponding pids for this session
    asarraypidlist_t* list =
	aspidsinash_array(server, array.empty() ? NULL : array.c_str(), ash);

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "aspidsinash_array(" << server << ", ";
	if(array.empty())
	    output << "NULL";
	else
	    output << "\"" << array << "\"";
	output << ", " << ash << ") = " << list << std::endl;
	if(list == NULL) {
	    output << "[TID " << pthread_self() << "] "
		   << "asgeterror() = " << asgeterror()
		   << " (\"" << asstrerror(asgeterror()) << "\")" << std::endl;
	}
	std::cerr << output.str();
    }
#endif

    // Go no further if the list couldn't be accessed
    if(list == NULL)
	return false;
        
    // Iterate over each machine in the array
    for(int i = 0; i < list->nummachines; ++i)
	
        // Iterate over each PID on this machine in the job
        for(int j = 0; j < list->machines[i]->numpids; ++j) {

	    // Add this host/pid to the MPI job information
	    job.push_back(std::make_pair(list->machines[i]->machname,
                                         list->machines[i]->pids[j]));

#ifndef NDEBUG
	    if(is_debug_mpijob_enabled) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] "
		       << "job += " << list->machines[i]->machname
		       << ":" << list->machines[i]->pids[j]
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	}
    
    // Free the list of hosts and their corresponding pids
    asfreearraypidlist(list, ASFLF_FREEDATA);

    // Close our handle to the MPT array services server
    ascloseserver(server);
    
    return true;

#else
    return false;
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
bool Experiment::getMPIJobFromMPICH(const Thread& thread, Job& job)
{
    bool is_mpich_job = true;
    Job table;

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "Experiment::getMPIJobFromMPICH("
	       << thread.getHost() << ":" << thread.getProcessId() << ", ...)"
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    // Attempt to access the MPICH process table from this thread
    is_mpich_job &= Instrumentor::getMPICHProcTable(thread, table);

    //    
    // On LANL's bproc systems running OpenMPI, the host names that appear in
    // the MPICH "process-finding" interface are the raw node numbers rather
    // than the real host name. E.g. "128" instead of "n128". The following
    // code is a hack to fix this... Each host name that consists entirely
    // of numerals is prepended with "n".
    //
    {
	for(Job::iterator i = table.begin(); i != table.end(); ++i)
	    if(i->first.find_first_not_of("0123456789") == std::string::npos)
		i->first.insert(0, "n");
    }

    // Lampi may send a raw IP address rather than the real host name.
    {
	for(Job::iterator i = table.begin(); i != table.end(); ++i)
	    if(i->first.find_first_not_of(".0123456789") == std::string::npos) {
		std::string hostName = getHostnameFromIP(i->first);
		i->first.swap(hostName);
	    }
    }

#ifndef NDEBUG
    if(is_debug_mpijob_enabled) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] "
	       << "is_mpich_job = " << (is_mpich_job ? "true" : "false")
	       << std::endl;
	for(Job::const_iterator i = table.begin(); i != table.end(); ++i)
	    output << "[TID " << pthread_self() << "]     "
		   << "job += " << i->first << ":" << i->second << std::endl;
	std::cerr << output.str();
    }
#endif
        
    // Go no further if this thread isn't in an MPICH MPI job
    if(!is_mpich_job)
	return false;

    // Add this table to the MPI job information
    job.insert(job.end(), table.begin(), table.end());

    return true;
}



// Used to update the THREAD table for offline experiments.
void Experiment::updateThreads(const pid_t& pid,
			       const pthread_t& tid,
			       const int& rank,
                               const std::string& host) const
{
    std::string canonical = getCanonicalName(host);

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);


    if (rank < 0 ) {

	// Create a thread entry in the database with no rank
	dm_database->prepareStatement(
            "INSERT INTO Threads (host, pid, posix_tid) VALUES (?, ?, ?);"
	    );
	dm_database->bindArgument(1, canonical);
	dm_database->bindArgument(2, pid);
	dm_database->bindArgument(3, tid);
	while(dm_database->executeStatement());

    } else {

	// Create a thread entry in the database with mpi rank
	dm_database->prepareStatement(
            "INSERT INTO Threads (host, pid, posix_tid, mpi_rank) VALUES (?, ?, ?, ?);"
	    );
	dm_database->bindArgument(1, canonical);
	dm_database->bindArgument(2, pid);
	dm_database->bindArgument(3, tid);
	dm_database->bindArgument(4, rank);
	while(dm_database->executeStatement());

    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 
}

// Used to update the THREAD table for offline experiments.
void Experiment::updateThreads(const pid_t& pid,
			       const pthread_t& tid,
			       const int& omptid,
			       const int& rank,
                               const std::string& host) const
{
    std::string canonical = getCanonicalName(host);

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);


    if (rank < 0 ) {

	// Create a thread entry in the database with no rank
	dm_database->prepareStatement(
            "INSERT INTO Threads (host, pid, posix_tid, openmp_tid) VALUES (?, ?, ?, ?);"
	    );
	dm_database->bindArgument(1, canonical);
	dm_database->bindArgument(2, pid);
	dm_database->bindArgument(3, tid);
	dm_database->bindArgument(4, omptid);
	while(dm_database->executeStatement());

    } else {

	// Create a thread entry in the database with mpi rank
	dm_database->prepareStatement(
            "INSERT INTO Threads (host, pid, posix_tid, openmp_tid, mpi_rank) VALUES (?, ?, ?, ? ,?);"
	    );
	dm_database->bindArgument(1, canonical);
	dm_database->bindArgument(2, pid);
	dm_database->bindArgument(3, tid);
	dm_database->bindArgument(4, omptid);
	dm_database->bindArgument(5, rank);
	while(dm_database->executeStatement());

    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 
}


//
// LOOP TODO: compressDB() needs to be updated to prune the loop data.
// 

// TODO: Finish code to prune existing databases of database
// entries that do not correspond to any of the sampled data.
// The resulting database will still use the same disk space and
// therefore will need to have the sqlite3 VACUUM command
// run on the database to completely removed the unused space
// and reduce the disk space consumed by the pruned database.
void Experiment::compressDB() const
{
    PCBuffer data_addr_buffer;

    std::string Data_File_Name = getName();
    std::cerr << "Experiment::compressDB: found existing database "
	<< Data_File_Name  << std::endl;

    std::string new_data_base_name = "Orig-" + Data_File_Name;
    copyTo(new_data_base_name);

    std::cerr << "Experiment::compressDB: existing database copied to "
	      << new_data_base_name  << std::endl;

    std::string collector_name;
    CollectorGroup cgrp = getCollectors();
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
	Collector c = *ci;
	Metadata m = c.getMetadata();
	collector_name = m.getUniqueId();
    }

    // create an Extent covering all time and all possible addresses
    TimeInterval ti(Time::TheBeginning(),Time::TheEnd());
    AddressRange ar(Address::TheLowest(),Address::TheHighest());
    //Extent e(ti,ar);
    Extent e = getPerformanceDataExtent();

    // Add extent to extent group for use with getUniquePCValues.
    ExtentGroup eg;
    eg.push_back(e);

    // Gather all threads in this experiment.
    ThreadGroup Expthreads = getThreads();

    // Set of all functions currently found in database.
    // If a function is found to contain an address from
    // the sampled data, it is removed from TGRPFunctions.
    // Any remaining functions in TGRPFunctions are removed from
    // the experiment database.
    std::set<Function> TGRPFunctions = Expthreads.getFunctions();

    // Set of all statements currently found in database.
    // If a statement is found to contain an address from
    // the sampled data, it is removed from TGRPStatements.
    // Any remaining statements in TGRPStatements are removed from
    // the experiment database.
    // TODO: Preserve at least the statement that corresponds to
    // the first statement in each function that does contain data!
    std::set<Statement> TGRPStatements = Expthreads.getStatements();

    std::set<std::string> EXPAF;
    std::set<Statement> EXPAS;
    std::set<Function> EXP_Funcs;
    std::set<LinkedObject> EXPLO;

    // create set of executables found in this experiment
    std::set<LinkedObject> exesUsedInExp;
    for(ThreadGroup::const_iterator i = Expthreads.begin();
				    i != Expthreads.end(); ++i) {
	exesUsedInExp.insert((*i).getExecutable().second);
    }

    // Create set that maps each unique executable to it's threadgroup
    // We use this to find all unique sampled addresses for all
    // threads that have this executable in the current experiment.
    // example: all threads that are the mpi program run by mpirun.
    std::set<std::pair<LinkedObject,ThreadGroup> > exeToThreads;
    for(std::set<LinkedObject>::const_iterator
		lo = exesUsedInExp.begin();
		lo != exesUsedInExp.end(); ++lo) {

	ThreadGroup tgrp = Expthreads.getSubsetWithLinkedObject((*lo));
	LinkedObject l = (*lo);

	std::cerr << "Experiment::compressDB: Found " << tgrp.size()
		<< " threads for " << (*lo).getPath() << std::endl;

	exeToThreads.insert(std::make_pair((*lo),tgrp));
    } 

    std::cerr << "Experiment::compressDB: Found  " << exeToThreads.size()
	<< " executables with threads" << std::endl;

    for(std::set<std::pair<LinkedObject,ThreadGroup> >::const_iterator
		myexe = exeToThreads.begin();
		myexe != exeToThreads.end(); ++myexe) {

	std::cerr << "Experiment::compressDB: Examining threads for "
		<< (*myexe).first.getPath() << std::endl;

	bool has_sampled_data = false;
	ThreadGroup exethreads = (*myexe).second;
	ThreadGroup threads_with_no_data;
	int total_pcs = 0;

	// find the unique set of addresses that where sampled
	// for this threadgroup.
	for(ThreadGroup::const_iterator t = exethreads.begin();
				    t != exethreads.end(); ++t) {


	    Collector c = *getCollectors().begin();
	    Metadata m = c.getMetadata();
	    std::string collector_name = m.getUniqueId();

	    c.getUniquePCValues(*t, eg, &data_addr_buffer);

	
	    if ( data_addr_buffer.length == total_pcs ) {
	        // Did not find any new sampled addresses.
		std::cerr << "Experiment::compressDB: Thread "
		    << EntrySpy((*t)).getEntry()
		    << " did not have any new addresses." << std::endl;
		//threads_with_no_data.insert(*t);
		continue;
	    } else {
	        // Found new sampled addresses in this thread.
		std::cerr << "Experiment::compressDB: Thread "
		    << EntrySpy((*t)).getEntry()
		    << " has new data addresses." << std::endl;
		has_sampled_data = true;
		total_pcs =  data_addr_buffer.length;
	    }

	} // end for threads

// DEBUG
#if 1
	    std::cerr << "Experiment::compressDB: Found "
		<< data_addr_buffer.length
	        << " unique addresses in sampled data for exe "
	        << (*myexe).first.getPath() << std::endl;
#endif

	// For this executable threadgroup, look for functions and
	// statements that correspond to sample data.
	// Loop through the unique addresses.
	for (unsigned ii = 0; ii < data_addr_buffer.length; ii++) {
	    Address addr = Address(data_addr_buffer.pc[ii]);

	    bool found_func;
	    bool found_stmt;

	    int funcs_found = 0;
	    int stmts_found = 0;
	    int lobjs_found = 0;

	    // Now loop through threads.  We do this since some
	    // statements and functions may be sampled in one thread
	    // and possibly not any of the others.
	    for(ThreadGroup::const_iterator t = exethreads.begin();
				    t != exethreads.end(); ++t) {
	    
		found_func = false;
		found_stmt = false;

	    // This crashed processing an mvapich example from zeus.
	    // This could have been due to the pthreading used by
	    // the mvapich installed on zeus. Database is zeus-X1.4.openss.
#if 1
		std::pair<bool, LinkedObject> l =
			(*t).getLinkedObjectAt(addr,Time::Now());
	    
		if (l.first) {
		    lobjs_found++;
		    if (EXPLO.find(l.second) == EXPLO.end()) {
			std::cerr << "Found LinkedObj " << l.second.getPath()
			    << " at " << addr
			    << " in thread " << EntrySpy(*t).getEntry()
			    << " at index " << ii
			    << std::endl;
			EXPLO.insert(l.second);
			ExtentGroup leg = l.second.getExtentIn(*t);
			for (ExtentGroup::iterator ei = leg.begin(); ei != leg.end(); ei++) {
			    std::cerr << "LinkedObj has extent "
				<< "Time " << (*ei).getTimeInterval()
				<< "AddrRange " << (*ei).getAddressRange()
				<< std::endl;
			}
		    }
		} else {
#if 0
		    std::cerr << "Experiment::compressDB:"
			<< "  DID NOT FIND LINKEDOBJECT FOR " << addr
			<< " in thread " << EntrySpy(*t).getEntry()
			<< " at index " << ii
			<< std::endl;
#endif
		}
#endif

// TODO:
// Want to find start address of a function.
// In this case, passed function id would be from a function that
// was found to contain an address sample.
// If so, whe use the address to find the first statement of
// the function so we can keep it.  Some of the expview metrics
// will show the start address and statement of a function.
// e.g. expview -v Functions.

		// Test for function at this address.
		std::pair<std::pair<bool, Function> , std::set<Statement> >
			fs = (*t).getFunctionAndStatementsAt(addr);

	        std::pair<bool, Function> f = fs.first;

		AddressRange frange;
		if (f.first) {
		    found_func = true;
		    funcs_found++;
		    if (EXPAF.find(f.second.getName()) == EXPAF.end()) {

			EXPAF.insert(f.second.getName());
			EXP_Funcs.insert(f.second);
			AddressRange addrFrange(f.second.getAddressRange());
			frange = addrFrange;

#if 1
			std::cerr << "Experiment::compressDB: ADD Function "
			    << f.second.getName() << " at " << addr
		            << " in thread " << EntrySpy(*t).getEntry()
			    << " frange is " << frange
			    << std::endl;
#endif
		    }
		} else {
#if 1
		    std::cerr << "Experiment::compressDB:"
			<< "  DID NOT FIND FUNCTION FOR " << addr
			<< " in thread " << EntrySpy(*t).getEntry()
			<< " at index " << ii
			<< std::endl;
#endif
		}

		// Find any statements at this address.
		std::set<Statement> s = fs.second;

		//s = (*t).getStatementsAt(addr,Time::Now());

		std::set<Statement> s_fbegin =
			(*t).getStatementsAt(frange.getBegin(),Time::Now());
		std::set<Statement> s_fend =
			(*t).getStatementsAt(frange.getEnd(),Time::Now());
#if 1
		if (s.size() == 0 ) {
		    std::cerr << "Experiment::compressDB:"
			<< " DID NOT FIND STATMENT FOR " << addr
			<< " in thread " << EntrySpy(*t).getEntry()
			<< " at index " << ii
			<< std::endl;
		}
#endif

		std::set<Function> s_funcs;
		for (std::set<Statement>::const_iterator si = s.begin();
							 si != s.end(); ++si) {

#if 0
		    std::cerr << "Experiment::compressDB: Statement "
			<< (*si).getPath() << ": " << (*si).getLine()
		        << " at " << addr << " in thread "
			<< EntrySpy(*t).getEntry()
		        << std::endl;
#endif
		    EXPAS.insert(*si);
		    found_stmt = true;
		    stmts_found++;
		    //s_funcs = (*si).getFunctions();
		}

		for (std::set<Statement>::const_iterator si = s_fbegin.begin();
							 si != s_fbegin.end();
							 ++si) {
		    EXPAS.insert(*si);
		    stmts_found++;
		}

		for (std::set<Statement>::const_iterator si = s_fend.begin();
							 si != s_fend.end();
							 ++si) {
		    EXPAS.insert(*si);
		    stmts_found++;
		}

		// No need to examine any more threads for this function
		// since we found one for this address...
		// FIXME: How about statements though.
		if (found_func) {
#if 1
		    std::cerr << "Experiment::compressDB: Found function in "
			<< " in thread " << EntrySpy(*t).getEntry()
			<< std::endl;
#endif
		    break;
		}

	    } // end for executable threads

#if 1
	    if (!found_func) {
		std::cerr << "Experiment::compressDB:"
		    << " DID NOT FIND FUNCTION for "
		    << addr << std::endl;
	    }
	    if (!found_func && !found_stmt) {
		std::cerr << "Experiment::compressDB:"
		    << " DID NOT FIND STATEMENT for "
		    << addr << std::endl;
	    }
#endif
	    std::cerr << "Experiment::compressDB: "
		<< " funcs found " << funcs_found
		<< " stmts found " << stmts_found
		<< " lobjs found " << lobjs_found
		<< " for address " << addr
		<< " at index " << ii
		<< std::endl;
	} // end for data_addr_buffer

	data_addr_buffer.length = 0;
	memset(data_addr_buffer.hash_table,
		0, sizeof(data_addr_buffer.hash_table));

	// This executables threads contained no sampled addresses.
	if (!has_sampled_data) {
	    std::cerr << "Experiment::compressDB: No data for "
		<< (*myexe).first.getPath() << std::endl;

	    for(ThreadGroup::const_iterator t = threads_with_no_data.begin();
				    t != threads_with_no_data.end(); ++t) {
		std::cerr << "Experiment::compressDB: Thread "
		    << EntrySpy((*t)).getEntry() << " has no data"
		    << " removing from database." << std::endl;
		//removeThread(*t);
	    }
	    continue;
	}

	// Report function statistics.
	std::cerr << "Experiment::compressDB: Found "
	    << TGRPFunctions.size() << " Total Functions "
	    << "in executable " << (*myexe).first.getPath() << std::endl;
	std::cerr << "Experiment::compressDB: Found "
	    << EXPAF.size() << " Functions with data." << std::endl;
	std::cerr << "Experiment::compressDB: Deleting "
	    << TGRPFunctions.size() - EXPAF.size()
	    << " Functions." << std::endl;

	// Prepare list of functions to remove from database.
	for (std::set<Function>::const_iterator fi = EXP_Funcs.begin();
					  fi != EXP_Funcs.end(); ++fi) {
	    if (TGRPFunctions.find(*fi) != TGRPFunctions.end()) {
		TGRPFunctions.erase(*fi);
	    }
	}

	// Now remove unneeded functions.
	//removeFunctions(TGRPFunctions);
	// Begin a multi-statement transaction
	BEGIN_WRITE_TRANSACTION(dm_database);

	std::set<Function>::iterator  fi;
	for (fi = TGRPFunctions.begin(); fi != TGRPFunctions.end(); fi++) {
	    dm_database->prepareStatement(
		"DELETE FROM Functions WHERE id = ?;"
		);
	    dm_database->bindArgument(1, EntrySpy((*fi)).getEntry());
	    while(dm_database->executeStatement());
	}

	END_TRANSACTION(dm_database);

	// Report statement statistics.
	std::cerr << "Experiment::compressDB: Found "
	    << TGRPStatements.size() << " Total Statements."
	    << "in executable " << (*myexe).first.getPath() << std::endl;
	std::cerr << "Experiment::compressDB: Found "
	    << EXPAS.size() << " Statements with data." << std::endl;
	std::cerr << "Experiment::compressDB: Deleting "
	    << TGRPStatements.size() - EXPAS.size()
	    << " Statements." << std::endl;

	// Prepare list of statements to remove from database.
	for (std::set<Statement>::const_iterator si = EXPAS.begin();
					   si != EXPAS.end(); ++si) {
	    if (TGRPStatements.find(*si) != TGRPStatements.end()) {
		TGRPStatements.erase(*si);
	    }
	}

	// Now remove unneeded statements.
	//removeStatements(TGRPStatements);
	// Begin a multi-statement transaction
	BEGIN_WRITE_TRANSACTION(dm_database);

	std::set<Statement>::iterator  si;
	for (si = TGRPStatements.begin(); si != TGRPStatements.end(); si++) {
	    dm_database->prepareStatement(
		"DELETE FROM Statements WHERE id = ?;"
		);
	    dm_database->bindArgument(1, EntrySpy((*si)).getEntry());
	    while(dm_database->executeStatement());
	}

	dm_database->prepareStatement(
	    "DELETE FROM StatementRanges "
	    "WHERE statement "
	    "  NOT IN (SELECT DISTINCT id FROM Statements)"
	    );
	while(dm_database->executeStatement());

	dm_database->prepareStatement(
	    "DELETE FROM Files "
	    "WHERE id NOT IN (SELECT DISTINCT file FROM LinkedObjects) "
	    "  AND id NOT IN (SELECT DISTINCT file FROM Statements);"
	    );
	while(dm_database->executeStatement());

	END_TRANSACTION(dm_database);

	// For now, leave in threads with potentially no data
	// in a threadgroup where some threads have data...
#if 0
	for(ThreadGroup::const_iterator t = threads_with_no_data.begin();
				    t != threads_with_no_data.end(); ++t) {
	    std::cerr << "Thread " << EntrySpy((*t)).getEntry()
		<< " has no data"
		<< " removing from database." << std::endl;
	    removeThread(*t);
	}
#endif

    } // end for exes

    // Now clean up any remaining linkedobjects, files, addressspaces.
    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);
    dm_database->prepareStatement(
	"DELETE FROM LinkedObjects "
	"WHERE id NOT IN (SELECT DISTINCT linked_object FROM Functions) "
	"  AND id NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(dm_database->executeStatement());

    dm_database->prepareStatement(
	"DELETE FROM Files "
	"WHERE id NOT IN (SELECT DISTINCT file FROM LinkedObjects) "
	"  AND id NOT IN (SELECT DISTINCT file FROM Statements);"
	);
    while(dm_database->executeStatement());

    dm_database->prepareStatement(
	"DELETE FROM AddressSpaces "
	"WHERE linked_object NOT IN (SELECT DISTINCT id FROM LinkedObjects) "
	"  AND linked_object NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(dm_database->executeStatement());

    END_TRANSACTION(dm_database);

}


/**
 * Set the view command into the database as a place holder for the view data to be stored later.
 *
 * It searches the database Views table for a matching command.  If not found it stores the command
 * into the database.
 *
 * @return   None
 */

void Experiment::setDatabaseViewHeader( std::string db_name,  std::string view_cmd_arg)
{
     int id_of_match = -1;
#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
         std::stringstream output;
         output << "ENTER Experiment::setDatabaseViewHeader, db_name=" << db_name 
                << " view_cmd_arg=" << view_cmd_arg << std::endl;
         std::cerr << output.str();
      }
#endif

  // If we don't find the command already in the database add it
  id_of_match = searchForViewCommandHeaderMatch( db_name, view_cmd_arg, true /* exact match */) ;
  if ( id_of_match < 0) {

     SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
         std::stringstream output;
         output << "IN Experiment::setDatabaseViewHeader, ADDING VIEW_CMD, db_name=" << db_name 
                << " view_cmd_arg=" << view_cmd_arg << std::endl;
         std::cerr << output.str();
      }
#endif


     // Find out if there are any entries in the database table for saved views
     // If not insert, otherwise add to the table
     BEGIN_TRANSACTION(database);
     database->prepareStatement(
                   "INSERT INTO Views (view_cmd) "
                   "VALUES (?) ;"
                   );
     database->bindArgument(1, view_cmd_arg);
     while( database->executeStatement() );
     END_TRANSACTION(database);

  } else {

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
         std::stringstream output;
         output << "IN Experiment::setDatabaseViewHeader, MATCHING VIEW_CMD found for, db_name=" << db_name 
                << " view_cmd_arg=" << view_cmd_arg << " id_of_match=" << id_of_match << std::endl;
         std::cerr << output.str();
      }
#endif

	  } 

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "EXIT Experiment::setDatabaseViewHeader, db_name=" << db_name 
		<< " view_cmd_arg=" << view_cmd_arg << std::endl;
	 std::cerr << output.str();
      }
#endif

}

/**
 * Search to see if a saved view command corresponds to the view_cmd_arg exists in the database.
 *
 * It searches the database Views table for a matching command.  If found it returns true
 * otherwise the value returned is set to false to indicate a matching command was not found.
 *
 * @return   The success/failure return value.
 */

int Experiment::searchForViewCommandHeaderMatch( std::string db_name,  std::string view_cmd_arg, bool exact_match )
{
// Search for a matching view command in the database then return the generated 
// performance information back to the caller.
 int id_of_match = -1;
#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "ENTER Experiment::searchForViewCommandHeaderMatch, db_name=" << db_name 
		<< " view_cmd_arg=" << view_cmd_arg << std::endl;
	 std::cerr << output.str();
      }
#endif

  SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));
  int return_val = -1;

  // Find out if there are any entries in the database table for saved views
  // If not insert, otherwise add to the table

  BEGIN_TRANSACTION(database);
  database->prepareStatement(
		"SELECT id, view_cmd FROM Views;"
		);
  while( database->executeStatement() ) {
    if(!database->getResultIsNull(2)) {

      std::string view_cmd_in_db = database->getResultAsString(2);

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << " Experiment::searchForViewCommandHeaderMatch, view_cmd_in_db=" << view_cmd_in_db 
		<< " view_cmd_arg=" << view_cmd_arg << " size of view_cmd_in_db=" << view_cmd_in_db.size() 
		<< " size of view_cmd_arg=" << view_cmd_arg.size() << std::endl;
	 std::cerr << output.str();
      }
#endif

      if (exact_match && view_cmd_in_db.compare(view_cmd_arg) == 0) {
	id_of_match = database->getResultAsInteger(1);

#ifndef NDEBUG
	if(is_debug_reuse_views_enabled) {
	   std::stringstream output;
	   output << " Experiment::searchForViewCommandHeaderMatch, EXACT EARLY RETURN TRUE" << std::endl;
	   std::cerr << output.str();
	}
#endif
	return_val = id_of_match;
	break;
      } else if (!exact_match && view_cmd_in_db.compare(0, view_cmd_arg.size(),view_cmd_arg) == 0) {
	id_of_match = database->getResultAsInteger(1);
#ifndef NDEBUG
	if(is_debug_reuse_views_enabled) {
	   std::stringstream output;
	   output << " Experiment::searchForViewCommandHeaderMatch, SUBSTR MATCH, EARLY RETURN TRUE" << std::endl;
	   std::cerr << output.str();
	}
#endif
	return_val = id_of_match;
	break;
      }
    }
  }
  END_TRANSACTION(database);

#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
      std::stringstream output;
      output << "EXIT Experiment::searchForViewCommandHeaderMatch, return_val=" << return_val 
	     << " view_cmd_arg=" << view_cmd_arg << std::endl;
      std::cerr << output.str();
  }
#endif

  return return_val;

}

/**
 * Store a CLI saved view to the database that corresponds to the view_cmd_arg that is passed in.
 *
 * It searches the database Views table for a matching command.  If found it stores 
 * the data view into the database for future reuse.  If a matching command is not found,
 * the return value is false to indicate a matching command was not found.
 *
 * @return   The success/failure return value.
 */

bool Experiment::addViewCommandAndDataEntries(  std::string db_name,  
						std::string view_cmd_arg, 
						std::string view_data_filename_arg, 
						std::ostream *view_outstream, 
						int view_file_header_offset)
{
  int id_of_match = -1 ;

  // Search for a matching view command in the database then store the generated 
  // performance information into the database.

  bool return_val = false;

#ifndef NDEBUG
	if(is_debug_reuse_views_enabled) {
	     std::stringstream output;
	     output << "ENTER Experiment::addViewCommandAndDataEntries, db_name=" << db_name 
		    << " view_cmd_arg=" << view_cmd_arg 
		    << " view_file_header_offset=" << view_file_header_offset 
		    << " view_data_filename_arg=" << view_data_filename_arg << std::endl;
	     std::cerr << output.str();
	}
#endif

  SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));

  setDatabaseViewHeader( db_name, view_cmd_arg);
  

  // Find out if there are any entries in the database table for saved views
  // If not insert, otherwise add to the table

  BEGIN_TRANSACTION(database);
  database->prepareStatement(
		"SELECT id, view_cmd FROM Views;"
		);
  while( database->executeStatement() ) {

    if(!database->getResultIsNull(2)) {

      std::string view_cmd_in_db = database->getResultAsString(2);

      if (view_cmd_in_db.compare(view_cmd_arg) == 0) {
	id_of_match = database->getResultAsInteger(1);
	return_val = true;

#ifndef NDEBUG
	if(is_debug_reuse_views_enabled) {
	     std::stringstream output;
	     output << " Experiment::addViewCommandAndDataEntries, FOUND ID OF MATCH, id_of_match=" 
		    << id_of_match << " view_cmd_in_db=" << view_cmd_in_db << std::endl;
	     std::cerr << output.str();
	}
#endif

	// break into the code to update the view blob
	break;
      }
    }
  }
  END_TRANSACTION(database);

  std::ostringstream *ostring_stream_pstr;
//  std::string pstr ;
  std::string pstr_temp ;

#if 1
  const int copyBufferSize = 4096;

  int header_length = view_file_header_offset;
  //FILE *fd = fopen(view_data_filename_arg.c_str(),"r");
  int fd = open(view_data_filename_arg.c_str(),O_RDONLY);
  char* read_buffer = (char *) malloc(copyBufferSize);

  if ( fd ) {
     //int num = read( fd, read_buffer, copyBufferSize );

     //memset(read_buffer, 0, copyBufferSize);
    for(int num = 1; num > 0;) {

       //num = fread(read_buffer, 1, copyBufferSize, fd);
       num = read( fd, read_buffer, copyBufferSize);
       Assert((num >= 0) || ((num == -1) /* && (errno == EINTR) */));
       //std::cerr << "In Experiment::addViewCommandAndDataEntries, READ NUM=" << num << std::endl;

       if (num > 0) {

#if 0
	 std::cerr << "In Experiment::addViewCommandAndDataEntries, reading from:" 
		 << " view_data_filename_arg=" << view_data_filename_arg 
		 << " num=" << num << " read_buffer[0]=" << read_buffer[0] 
		 << " read_buffer[header_length]=" << read_buffer[header_length] 
		 <<  std::endl;

	 for (int j=0; j<copyBufferSize; j++) {
	    std::cerr << "In Experiment::addViewCommandAndDataEntries, j=" << j << " read_buffer[j]=" <<  read_buffer[j] << std::endl;
	 }
#endif

	 //std::string ppstr(read_buffer + header_length);
	 //pstr = ppstr;
	 for(int i = header_length; i < num; i++) {
	    // Write bytes to the destination file
	    pstr_temp.push_back( read_buffer[i] ) ;
	 }
	 //std::cerr << "In Experiment::addViewCommandAndDataEntries, FIRST_TIME, num=" << num << " pstr_temp=" << pstr_temp << std::endl;
	 // After first time through, the header is not a concern, only need to skip for the initial read_buffer copy.
	 header_length = 0;

	 if (num != copyBufferSize) {
	    // need to end the string
	    pstr_temp.push_back( '\0' ) ;
	    break;
	 }

      } // for num
    } // num > 0

  } // fd ok

#else
  ostring_stream_pstr = (std::ostringstream*)(view_outstream);
  pstr = ostring_stream_pstr->str();
#endif

  std::string pstr(pstr_temp.c_str());

#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
    std::cerr << "In Experiment::addViewCommandAndDataEntries, size of pstr=" << pstr.size() << " length of pstr=" << pstr.length() << std::endl;
  }
#endif

  if (id_of_match > 0 && return_val == true ) {

#ifndef NDEBUG
    if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "In Experiment::addViewCommandAndDataEntries, stringstream, view_cmd_arg=" << view_cmd_arg 
		<< " about to write view_data string to the database, id_of_match=" << id_of_match 
		<< " pstr=" << pstr << std::endl;
	 std::cerr << output.str();
    }
#endif

    BEGIN_WRITE_TRANSACTION(database);
       database->prepareStatement(
	      "UPDATE Views SET view_data = ? WHERE id = ? AND view_cmd = ?;"
       );
       database->bindArgument(1, pstr);
       database->bindArgument(2, id_of_match);
       database->bindArgument(3, view_cmd_arg);
       while(database->executeStatement());
       END_TRANSACTION(database);
  }

#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "EXIT Experiment::addViewCommandAndDataEntries, db_name=" << db_name 
		<< " view_cmd_arg=" << view_cmd_arg << std::endl;
	 std::cerr << output.str();
  }
#endif

  return return_val;

}

/**
 * Get a CLI saved view from the database that corresponds to the view_cmd_arg that is passed in.
 *
 * Returns to the caller, the char * data that was stored in the database.
 * It searches the database Views table for matching commands.  If found it returns 
 * a status of true and the data through the returned_view_data data structure.
 * If not found, the return value is false to indicate a matching command was not found.
 *
 * @return   The char * data that was stored in the database and the success/failure return value.
 */

bool Experiment::getViewFromExistingCommandEntry( std::string db_name,  std::string view_cmd_arg, std::string &returned_view_data, int &size_of_returned_view_data )
{
  int id_of_match = -1 ;

  // Search for a matching view command in the database then return the generated 
  // performance information back to the caller.

  bool return_val = false;

#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "ENTER Experiment::getViewFromExistingCommandEntry, db_name=" << db_name 
		<< " view_cmd_arg=" << view_cmd_arg << std::endl;
	 std::cerr << output.str();
  }
#endif

  SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));

  // Find out if there are any entries in the database table for saved views
  // If so, return the saved data view data to the caller.

  BEGIN_TRANSACTION(database);
  database->prepareStatement(
		"SELECT id, view_cmd FROM Views;"
               );
  while( database->executeStatement() ) {
    if(!database->getResultIsNull(2)) {

      // pull out the view command from the database to use in search for the corresponding database entry
      std::string view_cmd_in_db = database->getResultAsString(2);

      if (view_cmd_in_db.compare(view_cmd_arg) == 0) {
       
	id_of_match = database->getResultAsInteger(1);
        return_val = true;

#ifndef NDEBUG
        if(is_debug_reuse_views_enabled) {
              std::stringstream output;
              output << "Experiment::getViewFromExistingCommandEntry, EARLY RETURN AFTER UPDATE, id_of_match=" 
                     << id_of_match << " view_cmd_in_db=" << view_cmd_in_db << std::endl;
              std::cerr << output.str();
        }
#endif

        // break to get into the code to update the view blob
        break;
      }
    }
  }
  END_TRANSACTION(database);

  std::string view_cmd_tmp;
  std::string View_String;
  if (id_of_match > 0 && return_val == true ) {

#ifndef NDEBUG
           if(is_debug_reuse_views_enabled) {
               std::stringstream output;
               output << "In Experiment::getViewFromExistingCommandEntry, reading view_data string from db, db_name=" 
                         << db_name << " view_cmd_arg=" << view_cmd_arg << std::endl;
               std::cerr << output.str();
           }
#endif

    BEGIN_TRANSACTION(database);

       database->prepareStatement( "SELECT view_data FROM Views WHERE id = ? AND view_cmd = ?;" );
       database->bindArgument(1, id_of_match);
       database->bindArgument(2, view_cmd_arg);

       while(database->executeStatement()) {
         if(!database->getResultIsNull(1)) {
           View_String = database->getResultAsString(1);
#ifndef NDEBUG
           if(is_debug_reuse_views_enabled) {
               std::stringstream output;
               output << "Experiment::getViewFromExistingCommandEntry, -1- View_String=" << View_String << std::endl;
               std::cerr << output.str();
           }
#endif
         } else {
           return_val = false;
         }
       }
  
       if (return_val) {

#ifndef NDEBUG
         if(is_debug_reuse_views_enabled) {
             std::stringstream output;
             output << "Experiment::getViewFromExistingCommandEntry -2-, View_String.size()=" << View_String.size() << " View_String=" << View_String << std::endl;
             std::cerr << output.str();
         }
#endif


         size_of_returned_view_data = View_String.size();
         returned_view_data =  (char*) View_String.c_str();

#if 0
         for (int j=0; j<200; j++) {
            std::cerr << "In Experiment::getViewFromExistingCommandEntry, j=" << j 
                      << " returned_view_data[j]=" << returned_view_data[j] << std::endl;
         }
#endif
       }

       END_TRANSACTION(database);
  }

#ifndef NDEBUG
   if(is_debug_reuse_views_enabled) {
      std::stringstream output;
      output << "EXIT Experiment::getViewFromExistingCommandEntry, db_name=" << db_name 
             << " view_cmd_arg=" << view_cmd_arg << std::endl;
      std::cerr << output.str();
   }
#endif

  return return_val;

}


/**
 * Return a vector of strings containing all the known saved view commands.
 *
 * Returns all CLI command string representations currently saved in this database. 
 * An empty string is returned if this database doesn't contain any saved commands.
 *
 * @return    CLI command strings representing the view data currently being cached in this database.
 */
std::vector<std::string> Experiment::getSavedCommandList(std::string db_name) const
{
    std::vector<std::string> cmd_list;
    SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));
   
    // Find our threads
    BEGIN_TRANSACTION(database);
    database->prepareStatement("SELECT view_cmd FROM Views;");
    while(database->executeStatement())
        cmd_list.push_back( database->getResultAsString(1));
    END_TRANSACTION(database);
   
    // Return the threads to the caller
    return cmd_list;
}


/**
 * Remove a single saved view table (Views) entry.
 *
 * Removes the specified table entry from this experiments views table. 
 *
 * @param input_view_cmd    View command that indicates the entry to be removed.
 */
void Experiment::removeViewTableEntry(const std::string view_cmd_arg, 
                                      const int views_id_arg,
                                      const OpenSpeedShop::Framework::SmartPtr<OpenSpeedShop::Framework::Database> database_arg) const
{

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
         std::stringstream output;
         output << "ENTER Experiment::removeViewTableEntry, view_cmd_arg=" << view_cmd_arg 
                << " views_id_arg=" << views_id_arg << std::endl;
         std::cerr << output.str();
      }
#endif
    
    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(database_arg);
    
    // Remove this views table entry
    database_arg->prepareStatement("DELETE FROM Views WHERE id = ?;");
    database_arg->bindArgument(1, views_id_arg);
    while(database_arg->executeStatement());    
    
    // End this multi-statement transaction
    END_TRANSACTION(database_arg);
}


/**
 * Remove view command and data entries from the database if they are not reliably usable across sessions.
 *
 * It searches the database Views table for the non-cross session commands (cview, cviewcluster, cviewcreate).  
 * Then remove them from the database.
 *
 * @args     Database name
 * @return   None
 */

void Experiment::removeNonCrossSessionViews(std::string db_name)
{
#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
         std::stringstream output;
         output << "ENTER Experiment::removeNonCrossSessionViews, db_name=" << db_name << std::endl;
         std::cerr << output.str();
      }
#endif
  
  std::string view_cmd_arg ;
  std::list<std::string> nonCrossSessionCmds ;
  nonCrossSessionCmds.push_back(std::string("cviewcluster"));
  nonCrossSessionCmds.push_back(std::string("cviewcreate"));
  nonCrossSessionCmds.push_back(std::string("cview"));

  std::list<std::string> all_cmds_in_db ;

  SmartPtr<Database> database = SmartPtr<Database>(new Database(db_name));

//  std::vector<std::string> cmd_list = getSavedCommandList();


  // Find all the view_cmds in the database 
  BEGIN_TRANSACTION(database);    
  database->prepareStatement("SELECT view_cmd FROM Views;");
  while(database->executeStatement())
    all_cmds_in_db.push_back(database->getResultAsString(1));
  END_TRANSACTION(database);
    

  std::string view_cmd_in_db ;

  for(std::list<std::string>::const_iterator j = all_cmds_in_db.begin(); j != all_cmds_in_db.end(); ++j)
  {
    view_cmd_in_db = *j;

    for(std::list<std::string>::const_iterator i = nonCrossSessionCmds.begin(); i != nonCrossSessionCmds.end(); ++i)
    {
     view_cmd_arg = *i;
     if ( view_cmd_in_db.compare(0, view_cmd_arg.size(),view_cmd_arg) == 0) {

         // If we find the command, then remove it from the database 
         int id_of_match =  searchForViewCommandHeaderMatch( db_name, view_cmd_arg, false /* exact match */ );
         if ( id_of_match > 0) {


#ifndef NDEBUG
         if(is_debug_reuse_views_enabled) {
             std::stringstream output;
             output << "IN Experiment::removeNonCrossSessionViews, REMOVING VIEW_CMD, db_name=" << db_name 
                    << " view_cmd_arg=" << view_cmd_arg << std::endl;
             std::cerr << output.str();
          }
#endif

         removeViewTableEntry(view_cmd_arg, 
                              id_of_match,
                              database);

         } else {

#ifndef NDEBUG
          if(is_debug_reuse_views_enabled) {
             std::stringstream output;
             output << "IN Experiment::removeNonCrossSessionViews, NO MATCHING VIEW_CMD found for, db_name=" << db_name 
                    << " view_cmd_arg=" << view_cmd_arg << std::endl;
             std::cerr << output.str();
          }
#endif

         } 

      } // end if command in db substr matches with non-cross-session commands
    } // end loop over non-cross-session commands
  } // end loop over commands in database

#ifndef NDEBUG
      if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "EXIT Experiment::removeNonCrossSessionViews, db_name=" << db_name 
		<< " view_cmd_arg=" << view_cmd_arg << std::endl;
	 std::cerr << output.str();
      }
#endif

}

/**
 * Store a view to the database that represents the view_data of the pass view_cmd.
 *
 * It searches the database Views table for a matching command.  If found it stores 
 * the data view into the database for future reuse.  If a matching command is not found,
 * the return value is false to indicate a matching command was not found.
 *
 * @return   The success/failure return value.
 */

bool Experiment::addView(std::string& viewcommand, std::string& viewdata )
{
  // Search for a matching view command in the database then store the generated 
  // performance information into the database.

  int id_of_match = -1;
  bool return_val = false;
  bool found_cmd = true;

#ifndef NDEBUG
     std::stringstream output;
	if(is_debug_reuse_views_enabled) {
	     output << "ENTER Experiment::addView" << " viewcommand=" << viewcommand << std::endl;
	     std::cerr << output.str();
	}
#endif


  // Find any existing entry in the database table for saved views
  BEGIN_TRANSACTION(dm_database);
  dm_database->prepareStatement(
		"SELECT id FROM Views where view_cmd = ?;"
		);
  
  dm_database->bindArgument(1, viewcommand);
  while(dm_database->executeStatement())
	    found_cmd = (dm_database->getResultAsInteger(1) > 0);
  END_TRANSACTION(dm_database);


  std::ostringstream *ostring_stream_pstr;
//  std::string pstr ;
  std::string pstr_temp ;

#if 0
  const int copyBufferSize = 4096;

  int header_length = view_file_header_offset;
  //FILE *fd = fopen(view_data_filename_arg.c_str(),"r");
  int fd = open(view_data_filename_arg.c_str(),O_RDONLY);
  char* read_buffer = (char *) malloc(copyBufferSize);

  if ( fd ) {
     //int num = read( fd, read_buffer, copyBufferSize );

     //memset(read_buffer, 0, copyBufferSize);
    for(int num = 1; num > 0;) {

       //num = fread(read_buffer, 1, copyBufferSize, fd);
       num = read( fd, read_buffer, copyBufferSize);
       Assert((num >= 0) || ((num == -1) /* && (errno == EINTR) */));
       //std::cerr << "In Experiment::addViewCommandAndDataEntries, READ NUM=" << num << std::endl;

       if (num > 0) {

#if 0
	 std::cerr << "In Experiment::addViewCommandAndDataEntries, reading from:" 
		 << " view_data_filename_arg=" << view_data_filename_arg 
		 << " num=" << num << " read_buffer[0]=" << read_buffer[0] 
		 << " read_buffer[header_length]=" << read_buffer[header_length] 
		 <<  std::endl;

	 for (int j=0; j<copyBufferSize; j++) {
	    std::cerr << "In Experiment::addViewCommandAndDataEntries, j=" << j << " read_buffer[j]=" <<  read_buffer[j] << std::endl;
	 }
#endif

	 //std::string ppstr(read_buffer + header_length);
	 //pstr = ppstr;
	 for(int i = header_length; i < num; i++) {
	    // Write bytes to the destination file
	    pstr_temp.push_back( read_buffer[i] ) ;
	 }
	 //std::cerr << "In Experiment::addViewCommandAndDataEntries, FIRST_TIME, num=" << num << " pstr_temp=" << pstr_temp << std::endl;
	 // After first time through, the header is not a concern, only need to skip for the initial read_buffer copy.
	 header_length = 0;

	 if (num != copyBufferSize) {
	    // need to end the string
	    pstr_temp.push_back( '\0' ) ;
	    break;
	 }

      } // for num
    } // num > 0

  } // fd ok

  std::string pstr(pstr_temp.c_str());

#else
  std::ostream *view_outstream;
  std::string pstr ;
  ostring_stream_pstr = (std::ostringstream*)(view_outstream);
  pstr = ostring_stream_pstr->str();
#endif


#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
    std::cerr << "In Experiment::addView view.size:" << pstr.size() << " pstr.length:" << pstr.length() << std::endl;
  }
#endif

  if (id_of_match > 0 && return_val == true ) {

#ifndef NDEBUG
    if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "In Experiment::addView UPDATE view viewcommand:" << viewcommand 
		<< " id:" << id_of_match 
		<< " view:" << pstr << std::endl;
	 std::cerr << output.str();
    }
#endif

    BEGIN_WRITE_TRANSACTION(dm_database);
       dm_database->prepareStatement(
	      "UPDATE Views SET view_data = ? WHERE id = ? AND view_cmd = ?;"
       );
       dm_database->bindArgument(1, pstr);
       dm_database->bindArgument(2, id_of_match);
       dm_database->bindArgument(3, viewcommand);
       while(dm_database->executeStatement());
       END_TRANSACTION(dm_database);
  }

#ifndef NDEBUG
  if(is_debug_reuse_views_enabled) {
	 std::stringstream output;
	 output << "EXIT Experiment::addView"
		<< " view command:" << viewcommand << std::endl;
	 std::cerr << output.str();
  }
#endif

  return return_val;

}
