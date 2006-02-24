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
 * Definition of the DataQueues namespace.
 *
 */

#include "Address.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "DataQueues.hxx"
#include "SmartPtr.hxx"
#include "Time.hxx"

#include "OpenSS_DataHeader.h"

#include <deque>
#include <pthread.h>
#include <map>
#include <sstream>

using namespace OpenSpeedShop::Framework;



namespace {
    
    /** Exclusive access lock for this unnamed namespace's variables. */
    pthread_mutex_t exclusive_access_lock = PTHREAD_MUTEX_INITIALIZER;
    
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
    bool is_debug_enabled = false;

    /** Cumulative number of performance data blobs that have been enqueued. */
    uint64_t debug_enqueued_count = 0;
    
    /** Cumulative number of bytes (within blobs) that have been enqueued. */
    uint64_t debug_enqueued_bytes = 0;
    
    /** Cumulative number of performance data blobs that have been stored. */
    uint64_t debug_stored_count = 0;
    
    /** Cumulative number of bytes (within blobs) that have been stored. */
    uint64_t debug_stored_bytes = 0;

#endif



    /**
     * Store performance data.
     *
     * Stores the specified performance data into the specified experiment
     * database. The blob is assumed to have been encoded by OpenSS_Send and is
     * thus prepended with a performance data header. That header is decoded in
     * order to create a properly indexed entry for the actual data.
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
	OpenSS_DataHeader header;
	memset(&header, 0, sizeof(header));
	unsigned header_size = blob.getXDRDecoding(
	    reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader), &header
	    );
	
	// Silently ignore the data if the address range is invalid
	if(header.addr_begin >= header.addr_end)
	    ignore_data = true;
	
	// Silently ignore the data if the time interval is invalid
	if(header.time_begin >= header.time_end)
	    ignore_data = true;
	
	// Note: It is assumed here that the experiment identifier for this
	//       blob corresponds to the database into which we are writting
	//       this data.
	
	// Validate that the specified collector exists
	int collector_rows = 0;
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Collectors WHERE id = ?;"
	    );
	database->bindArgument(1, header.collector);
	while(database->executeStatement())		
	    collector_rows = database->getResultAsInteger(1);
	if(collector_rows != 1)
	    ignore_data = true;
	
	// Validate that the specified thread exists
	int thread_rows = 0;
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Threads WHERE id = ?;"
	    );
	database->bindArgument(1, header.thread);
	while(database->executeStatement())
	    thread_rows = database->getResultAsInteger(1);
	if(thread_rows != 1)
	    ignore_data = true;
	
	// Calculate the size and location of the actual data
	unsigned data_size = blob.getSize() - header_size;
	const void* data_ptr =
	    &(reinterpret_cast<const char*>(blob.getContents())[header_size]);
	
	// Create an entry for this data
	if(!ignore_data) {
	    database->prepareStatement(
		"INSERT INTO Data "
		"(collector, thread, time_begin, time_end, "
		"  addr_begin, addr_end, data) "
		"VALUES (?, ?, ?, ?, ?, ?, ?);"
		);
	    database->bindArgument(1, header.collector);
	    database->bindArgument(2, header.thread);
	    database->bindArgument(3, Time(header.time_begin));
	    database->bindArgument(4, Time(header.time_end));
	    database->bindArgument(5, Address(header.addr_begin));
	    database->bindArgument(6, Address(header.addr_end));
	    database->bindArgument(7, Blob(data_size, data_ptr));
	    while(database->executeStatement());  
	}
	
	// End this multi-statement transaction
	END_TRANSACTION(database);
    }



}



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

#ifndef NDEBUG
    // Is debugging enabled?
    if(getenv("OPENSS_DEBUG_DATAQUEUES") != NULL)
	is_debug_enabled = true;
#endif
    
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

    // Find the database's identifier
    int identifier = database_to_identifier[database];

    // Flush the database's queue if non-empty
    if(!identifier_to_queue[identifier]->empty()) {

	// Begin a multi-statement transaction
	BEGIN_WRITE_TRANSACTION(database);

	// Iterate over each blob in the queue
	while(!identifier_to_queue[identifier]->empty()) {

	    // Store this blob in the correct database
	    storePerformanceData(database,
				 identifier_to_queue[identifier]->front());

	    // Pop this blob off the queue
	    identifier_to_queue[identifier]->pop_front();
	    
	}

	// End this multi-statement transaction
	END_TRANSACTION(database);

    }
    
    // Remove this database
    database_to_identifier.erase(database);
    identifier_to_database.erase(identifier);
    identifier_to_queue.erase(identifier);    
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0); 
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
    OpenSS_DataHeader header;
    memset(&header, 0, sizeof(header));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader),
			&header);
    
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
 * database. ...
 */
void DataQueues::flushPerformanceData()
{
    const int64_t maximumFlushTime = 1000 * 1000 * 1000;  // 1 second

    // Keep track of whether we've exceeded the maximum flush time
    bool is_time_up = false;
    Time start_time = Time::Now();

#ifndef NDEBUG
    // Performance statistics
    uint64_t debug_written_bytes = 0;
    Time debug_start_time = Time::Now();
#endif
    
    // Acquire exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_lock(&exclusive_access_lock) == 0);

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
	    
	}

#ifndef DEBUG
    // Performance statistics
    Time debug_stop_time = Time::Now();
    
    // Display performance statistics
    if(is_debug_enabled && (debug_written_bytes > 0)) {

	// Calculate the write rate in kilobytes/second
	double write_rate = (debug_stop_time <= debug_start_time) ? 0.0 :
	    (static_cast<double>(debug_written_bytes) * 1000000000.0) /
	    (static_cast<double>(debug_stop_time - debug_start_time) * 1024.0);
	
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
#endif    

    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0);
}
