////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
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
 * Definition of the DataQueues namespace.
 *
 */

#include "Address.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "Collector.hxx"
#include "Database.hxx"
#include "DataCache.hxx"
#include "DataQueues.hxx"
#include "Experiment.hxx"
#include "SmartPtr.hxx"
#include "Thread.hxx"
#include "Time.hxx"
#include "TimeInterval.hxx"

#include "OpenSS_DataHeader.h"

#if defined BUILD_CBTF
#include "KrellInstitute/Messages/DataHeader.h"
#endif

#include <deque>
#include <pthread.h>
#include <map>
#include <sstream>

using namespace OpenSpeedShop::Framework;



namespace {
    
    /** Exclusive access lock for this unnamed namespace's variables. */
    pthread_mutex_t exclusive_access_lock = 
	PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    
    /** Next available identifier value. */
    int next_identifier = 0;
    
    /** Map databases to their identifier. */
    std::map<SmartPtr<Database>, int> database_to_identifier;
    
    /** Map identifiers to their database. */
    std::map<int, SmartPtr<Database> > identifier_to_database;
    
    /** Map identifiers to their queue. */
    std::map<int, SmartPtr<std::deque<Blob> > > identifier_to_queue;

#ifndef NDEBUG
    /** Flag indicating if debugging for this namespace is enabled. */
    bool is_debug_enabled = (getenv("OPENSS_DEBUG_DATAQUEUES") != NULL);

    /** Cumulative number of performance data blobs that have been enqueued. */
    uint64_t debug_enqueued_count = 0;
    
    /** Cumulative number of bytes (within blobs) that have been enqueued. */
    uint64_t debug_enqueued_bytes = 0;
    
    /** Cumulative number of performance data blobs that have been stored. */
    uint64_t debug_stored_count = 0;
    
    /** Cumulative number of bytes (within blobs) that have been stored. */
    uint64_t debug_stored_bytes = 0;



    /**
     * Display performance statistics.
     *
     * Displays performance statistics for the passed flush operation to the
     * standard error stream. Reported information includes cumulative number
     * of data blobs entering/leaving the data queues, and the instantaneous
     * write rate for the flush.
     *
     * @param interval    Time interval over which data was flushed.
     * @param bytes       Number of bytes flushed.
     */
    void debugPerformanceStatistics(const TimeInterval& interval,
				    const uint64_t& bytes)
    {
	// Calculate the write rate in kilobytes/second
	double write_rate = 
	    (static_cast<double>(bytes) * 1000000000.0) /
	    (static_cast<double>(interval.getWidth()) * 1024.0);
	
	// Build the string to be displayed
	std::stringstream output;

	output << "[ "
	       << "In: " << debug_enqueued_count
	       << " (" << debug_enqueued_bytes << ")"
	       << ", Out: " << debug_stored_count
	       << " (" << debug_stored_bytes << ")"
	       << " ]   "
	       << std::setiosflags(std::ios::fixed)
	       << std::setprecision(1);	
	
	if(write_rate < 1024.0)
	    output << write_rate << " KB/s";
	else if(write_rate < (1024.0 * 1024.0))
	    output << (write_rate / 1024.0) << " MB/s";
	else
	    output << (write_rate / (1024.0 * 1024.0)) << " GB/s";
	
	output << std::endl;
	
	// Display the string to the standard error stream
	std::cerr << output.str();
    }
#endif  // NDEBUG



    /**
     * Store performance data.
     *
     * Stores the specified performance data into the specified experiment
     * database. The blob is assumed to have been encoded by OpenSS_Send and is
     * thus prepended with a performance data header. That header is decoded in
     * order to create a properly indexed entry for the actual data.
     *
     * @todo    Collector runtimes used to pass a thread identifier directly
     *          within the performance data header. When the instrumentor API
     *          was changed to support direct instrumentation of thread groups,
     *          rather than individual threads, this changed. Since collectors
     *          could no longer directly give their runtimes a single thread
     *          identifier specifying the thread for which data was gathered,
     *          the runtimes now have to obtain the thread identification data
     *          themselves. The only such information they have available is
     *          the host name, process identifier, and POSIX thread identifier
     *          and that is what is now passed in the performance data header.
     *          Thus this routine must now do a search for every blob to find
     *          the correct thread identifier from this information. Currently
     *          the search is done directly on the database every time. This
     *          may prove too slow and an in-memory cache in the same spirit
     *          as the DataCache class may be required.
     *
     * @note    Various timing issues can result in cases where, for example,
     *          data arrives after a particular collector, or thread has been
     *          destroyed/removed. It was deemed best to silently ignore the
     *          data under these circumstances.
     *
     * @param database    Database to contain the performance data.
     * @param blob        Blob containing the performance data.
     */
    void storePerformanceData(const SmartPtr<Database>& database,
			      const Blob& blob)
    {
	bool ignore_data = false;

	// Begin a multi-statement transaction
	BEGIN_WRITE_TRANSACTION(database);	
	
	// Decode the performance data header
#if defined(BUILD_CBTF)
	CBTF_DataHeader header;
	memset(&header, 0, sizeof(header));
	unsigned header_size = blob.getXDRDecoding(
	    reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
	    );
#else
	OpenSS_DataHeader header;
	memset(&header, 0, sizeof(header));
	unsigned header_size = blob.getXDRDecoding(
	    reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader), &header
	    );
#endif
	
	// Silently ignore the data if the address range is invalid
	if(header.addr_begin >= header.addr_end) {
	    ignore_data = true;

#ifndef NDEBUG
	    if (is_debug_enabled) {
	    std::cerr << "storePerformanceData: IGNORE DATA header.addr_begin "
		<< Address(header.addr_begin)
		<< " >= header.addr_end "
		<< Address(header.addr_end)
		<< std::endl;
	    }
#endif
	}
	
	// Silently ignore the data if the time interval is invalid
	if(header.time_begin >= header.time_end) {
	    ignore_data = true;
#ifndef NDEBUG
	    if (is_debug_enabled) {
	    std::cerr << "storePerformanceData: IGNORE DATA header.time_begin "
		<< Time(header.time_begin) << ">= header.time_end "
		<< Time(header.time_end) << std::endl;
	    }
#endif
	}
	
	// Note: It is assumed here that the experiment identifier for this
	//       blob corresponds to the database into which we are writing
	//       this data.
	
	// Validate that the specified collector exists
	int collector_rows = 0;
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Collectors WHERE id = ?;"
	    );
	database->bindArgument(1, header.collector);
	while(database->executeStatement())		
	    collector_rows = database->getResultAsInteger(1);
	if(collector_rows != 1) {
	    ignore_data = true;
#ifndef NDEBUG
	    if (is_debug_enabled) {
	    std::cerr << "storePerformanceData: IGNORE DATA"
		<< " collector_rows != 1 " << std::endl;
	    }
#endif
	}

	// Note: See the "todo" item in this function's description.

	// Find the identifier of the specified thread
	std::string host = Experiment::getCanonicalName(header.host);
	int thread = 0;
	database->prepareStatement(
	    "SELECT id "
	    "FROM Threads "
	    "WHERE host = ? "
	    "  AND pid = ? "
	    "  AND posix_tid = ?;"
	    );
	database->bindArgument(1, host);
	database->bindArgument(2, static_cast<int>(header.pid));
	database->bindArgument(3, static_cast<pthread_t>(header.posix_tid));
	while(database->executeStatement())
	    thread = database->getResultAsInteger(1);
	if(thread == 0) {
	    database->prepareStatement(
	        "SELECT id "
		"FROM Threads "
		"WHERE host = ? "
		"  AND pid = ? "
		"  AND posix_tid IS NULL;"
		);
	    database->bindArgument(1, host);
	    database->bindArgument(2, static_cast<int>(header.pid));
	    while(database->executeStatement())
		thread = database->getResultAsInteger(1); 
	}

	//
	// TEMPORARY HACK (WDH APR-26-2008)
	//
	// The following code temporarily handles the fact that we are seeing
	// forked processes for which Dyninst provides a TID of zero, and the
	// thread's database entry is created with posix_tid=0. Later when the
	// collector applies a TID to the performance data blob, it finds the
	// real TID and uses it. Thus the TIDs don't match and this function
	// tosses the data.
	//
	// I believe Dyninst's reporting of the TID is in error and needs to
	// be fixed. But until then, we'll assume that only the first thread
	// in each process may be misidentified, and if are unable to find an
	// appropriate thread using the usual means above, we'll accept one
	// that has posix_tid=0 (iff the host name and PID match correctly).
	//
	if(thread == 0) {
	    database->prepareStatement(
	        "SELECT id "
		"FROM Threads "
		"WHERE host = ? "
		"  AND pid = ? "
		"  AND posix_tid = ?;"
		);
	    database->bindArgument(1, host);
	    database->bindArgument(2, static_cast<int>(header.pid));
	    database->bindArgument(3, static_cast<pthread_t>(0));
	    while(database->executeStatement())
		thread = database->getResultAsInteger(1); 
	}

	if(thread == 0) {
	    ignore_data = true;
#ifndef NDEBUG
	    if (is_debug_enabled) {
	    std::cerr << "storePerformanceData: IGNORE DATA thread is  0 ??? "
		<< thread << " ignore_data is " << ignore_data << std::endl;
	    }
#endif
	}

	// Calculate the size and location of the actual data
	unsigned data_size = blob.getSize() - header_size;
	const void* data_ptr =
	    &(reinterpret_cast<const char*>(blob.getContents())[header_size]);
	
	//
	// Create an entry for this data
	if(!ignore_data) {
	    database->prepareStatement(
		"INSERT INTO Data "
		"(collector, thread, time_begin, time_end, "
		"  addr_begin, addr_end, data) "
		"VALUES (?, ?, ?, ?, ?, ?, ?);"
		);
	    database->bindArgument(1, header.collector);
	    database->bindArgument(2, thread);
	    database->bindArgument(3, Time(header.time_begin));
	    database->bindArgument(4, Time(header.time_end));
	    database->bindArgument(5, Address(header.addr_begin));
	    database->bindArgument(6, Address(header.addr_end));
	    database->bindArgument(7, Blob(data_size, data_ptr));
	    while(database->executeStatement());  
	}
	
	// Add this data to the performance data cache
	if(!ignore_data)
	    DataQueues::TheCache.addIdentifier(
		database, header.collector, thread,
		Extent(TimeInterval(Time(header.time_begin),
				    Time(header.time_end)),
		       AddressRange(Address(header.addr_begin),
				    Address(header.addr_end))),
		database->getLastInsertedUID()
		);
	
	// End this multi-statement transaction
	END_TRANSACTION(database);
    }

}



/** Performance data cache. */
DataCache DataQueues::TheCache;



/**
 * Add a database.
 *
 * Adds the passed database to the data queues. The database is given the next
 * available unique integer as its identifier. Once added, incoming performance
 * data can be queued for eventual writing to this database.
 *
 * @note    An assertion failure occurs if an attempt is made to add a null
 *          database or to add a database more than once.
 *
 * @param database    Database to be added.
 */
void DataQueues::addDatabase(const SmartPtr<Database>& database)
{
    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);

    // Check assertions
    Assert(!database.isNull());
    Assert(database_to_identifier.find(database) ==
	   database_to_identifier.end());

    // Add this database
    int identifier = next_identifier++;
    database_to_identifier[database] = identifier;
    identifier_to_database[identifier] = database;
    identifier_to_queue[identifier] = 
	SmartPtr<std::deque<Blob> >(new std::deque<Blob>());

    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0);
}



/**
 * Remove a database.
 *
 * Removes the passed database from the data queues. Any performance data queued
 * up for this database is flushed before the database is removed. Any incoming
 * performance data received for this database after it is removed will be
 * discarded.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a null
 *          database or to remove a database that isn't currently added.
 *
 * @param database    Database to be removed.
 */
void DataQueues::removeDatabase(const SmartPtr<Database>& database)
{
    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);
    
    // Check assertions
    Assert(!database.isNull());
    Assert(database_to_identifier.find(database) !=
	   database_to_identifier.end());

    // Flush any performance data that might remain in the database's queue
    flushDatabase(database);
    
    // Find the database's identifier
    int identifier = database_to_identifier[database];
    
    // Remove this database
    database_to_identifier.erase(database);
    identifier_to_database.erase(identifier);
    identifier_to_queue.erase(identifier);    
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0); 
}



/**
 * Get database.
 *
 * Returns the database for the specified database identifier. A null pointer
 * is returned if no such database has been added to the data queues.
 *
 * @param identifier    Identifier of the database to be found.
 * @return              Database for that identifier or a null pointer
 *                      if no such database is found.
 */
SmartPtr<Database> DataQueues::getDatabase(const int& identifier)
{
    SmartPtr<Database> database;

    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);

    // Find the identifier's database
    std::map<int, SmartPtr<Database> >::const_iterator i =
	identifier_to_database.find(identifier);
    if(i != identifier_to_database.end())
	database = i->second;
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0); 
    
    // Return the database to the caller
    return database;
}



/**
 * Get database identifier.
 *
 * Returns the database identifier for the specified database. An invalid
 * identifier (-1) is returned if no such database has been added to the data
 * queues.
 *
 * @param database    Database to be identified.
 * @return            Identifier of the database or an invalid identifier (-1)
 *                    if no such database is found.
 */
int DataQueues::getDatabaseIdentifier(const SmartPtr<Database>& database)
{
    int identifier = -1;

    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);

    // Find the database's identifier
    std::map<SmartPtr<Database>, int>::const_iterator i =
	database_to_identifier.find(database);
    if(i != database_to_identifier.end())
	identifier = i->second;
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0); 
    
    // Return the identifier to the caller
    return identifier;
}



/**
 * Flush a database.
 *
 * Flushes any performance data in the data queues for the passed database.
 * Performance data is flushed until the queue has been emptied.
 *
 * @note    An assertion failure occurs if an attempt is made to flush a null
 *          database or to flush a database that isn't currently added.
 *
 * @param database    Database to be flushed.
 */
void DataQueues::flushDatabase(const SmartPtr<Database>& database)
{
    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);
    
#ifndef NDEBUG
    // Performance statistics
    uint64_t debug_written_bytes = 0;
    Time debug_start_time = Time::Now();
#endif

    // Check assertions
    Assert(!database.isNull());
    Assert(database_to_identifier.find(database) !=
	   database_to_identifier.end());

    // Find the database's identifier and queue
    int identifier = database_to_identifier[database];
    SmartPtr<std::deque<Blob> > queue = identifier_to_queue[identifier];
    
    // Flush the database's queue if non-empty
    if(!queue->empty()) {
	
	// Begin a multi-statement transaction
	BEGIN_WRITE_TRANSACTION(database);

	// Iterate over each blob in the queue
	while(!queue->empty()) {

#ifndef NDEBUG
	    // Update performance statistics
	    debug_written_bytes += queue->front().getSize();
	    debug_stored_count++;
	    debug_stored_bytes += queue->front().getSize();
#endif

	    // Store this blob in the correct database
	    storePerformanceData(database, queue->front());

	    // Pop this blob off the queue
	    queue->pop_front();
	    
	}

	// End this multi-statement transaction
	END_TRANSACTION(database);

    }

// JEG change from DEBUG to NDEBUG
#ifndef NDEBUG
    // Performance statistics
    Time debug_stop_time = Time::Now();
    
    // Display performance statistics
    if(is_debug_enabled && (debug_written_bytes > 0))
	debugPerformanceStatistics(TimeInterval(debug_start_time, 
						debug_stop_time),
				   debug_written_bytes);    
#endif    

    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0); 
}



/**
 * Enqueue performance data.
 *
 * Enqueues the specified performance data for the correct experiment database.
 * The blob containing the data is assumed to have been encoded by OpenSS_Send
 * and is thus prepended with a performance data header. That header is decoded
 * in order to locate the correct queue into which to place the data.
 *
 * @note    Various timing issues can result in cases where data arrives after
 *          its experiment has been removed. It was deemed best to silently
 *          ignore the data under those circumstances.
 *
 * @parma blob    Blob containing the performance data.
 */
void DataQueues::enqueuePerformanceData(const Blob& blob)
{
    // Decode the performance data header
#if defined(BUILD_CBTF)
    CBTF_DataHeader header;
    memset(&header, 0, sizeof(header));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader),
			&header);
#else
    OpenSS_DataHeader header;
    memset(&header, 0, sizeof(header));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader),
			&header);
#endif
    
    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);
    
    // Find the database's queue
    std::map<int, SmartPtr<std::deque<Blob> > >::iterator
	i = identifier_to_queue.find(header.experiment);
    if(i != identifier_to_queue.end()) {

#ifndef NDEBUG
	// Update performance statistics
	debug_enqueued_count++;
	debug_enqueued_bytes += blob.getSize();	
#endif
	
	// Add this blob to the database's queue
	i->second->push_back(blob);

    }
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0);
}



/**
 * Flush performance data.
 *
 * Flushes performance data from the data queues to the correct experiment
 * databases. Performance data is flushed until either the queues have been
 * emptied or a set maximum length of time has passed. Any data remaining in
 * the queues after the maxmium time is exceeded will be left there for the
 * next flush. Imposing such a maximum flush time prevents the flushing from
 * monopolizing the time of the calling thread.
 */
void DataQueues::flushPerformanceData()
{

    const int64_t maximumFlushTime = 1000 * 1000 * 1000;  // 1 second

    // Keep track of whether we've exceeded the maximum flush time
    bool is_time_up = false;
    Time start_time = Time::Now();

    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);

#ifndef NDEBUG
    // Performance statistics
    uint64_t debug_written_bytes = 0;
    Time debug_start_time = Time::Now();
#endif
    
    // Iterate over each database queue
    for(std::map<int, SmartPtr<std::deque<Blob> > >::const_iterator
	    i = identifier_to_queue.begin(); 
	!is_time_up && (i != identifier_to_queue.end()); 
	++i)
	
	// Flush this queue if it is non-empty
	if(!i->second->empty()) {
	    
	    // Begin a multi-statement transaction
	    BEGIN_WRITE_TRANSACTION(identifier_to_database[i->first]);	

	    // Iterate over each blob in the queue
	    while(!is_time_up && !i->second->empty()) {
		
#ifndef NDEBUG
		// Update performance statistics
		debug_written_bytes += i->second->front().getSize();
		debug_stored_count++;
		debug_stored_bytes += i->second->front().getSize();
#endif
		
		// Store this blob in the correct database
		storePerformanceData(identifier_to_database[i->first],
				     i->second->front());

		// Pop this blob off the queue
		i->second->pop_front();

		// Has the maximum flush time been exceeded?
		if((Time::Now() - start_time) > maximumFlushTime)
		    is_time_up == true;
		
	    }
 
	    // End this multi-statement transaction
	    END_TRANSACTION(identifier_to_database[i->first]);
	    

	} else {
	}
 
// JEG CHANGED FROM DEBUG TO NDEBUG
#ifndef NDEBUG
    // Performance statistics
    Time debug_stop_time = Time::Now();
    
    // Display performance statistics
    if(is_debug_enabled && (debug_written_bytes > 0))
	debugPerformanceStatistics(TimeInterval(debug_start_time, 
						debug_stop_time),
				   debug_written_bytes);    
#endif    

    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0);
}
