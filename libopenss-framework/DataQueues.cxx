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
#include "runtime/OpenSS_DataHeader.h"

#include <deque>
#include <pthread.h>
#include <map>

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



    /**
     * Flush performance data to database.
     *
     * Stores the performance data in the specified queue into the specified
     * experiment database. The blobs containing the data are assumed to have
     * been encoded by OpenSS_Send and are thus prepended with a performance
     * data header. That header is decoded in order to create a properly
     * indexed entry for the actual data.
     *
     * @note    Various timing issues can result in cases where, for example,
     *          data arrives after a particular collector, or thread has been
     *          destroyed/removed. It was deemed best to silently ignore the
     *          data under these circumstances.
     *
     * @param database    Database to contain the performance data.
     * @param queue       Queue containing the performance data.
     */
    void flushPerformanceData(const SmartPtr<Database>& database,
			      const SmartPtr<std::deque<Blob> >& queue)
    {
	// Begin a multi-statement transaction
	BEGIN_TRANSACTION(database);	

	// Iterate over each blob in the queue
	for(std::deque<Blob>::const_iterator
		i = queue->begin(); i != queue->end(); ++i) {

	    // Decode the performance data header
	    OpenSS_DataHeader header;
	    memset(&header, 0, sizeof(header));
	    unsigned header_size = i->getXDRDecoding(
		reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader), &header
		);
	    
	    // Silently ignore the data if the address range is invalid
	    if(header.addr_begin >= header.addr_end)
		continue;
	    
	    // Silently ignore the data if the time interval is invalid
	    if(header.time_begin >= header.time_end)
		continue;

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
		continue;
	
	    // Validate that the specified thread exists
	    int thread_rows = 0;
	    database->prepareStatement(
		"SELECT COUNT(*) FROM Threads WHERE id = ?;"
		);
	    database->bindArgument(1, header.thread);
	    while(database->executeStatement())
		thread_rows = database->getResultAsInteger(1);
	    if(thread_rows != 1)
		continue;
	    
	    // Calculate the size and location of the actual data
	    unsigned data_size = i->getSize() - header_size;
	    const void* data_ptr =
		&(reinterpret_cast<const char*>(i->getContents())[header_size]);
	    
	    // Create an entry for this data
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
    if(!identifier_to_queue[identifier]->empty())
	flushPerformanceData(database, identifier_to_queue[identifier]);
    
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
 * @todo    Add good criteria for flushing the database's queue.
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
	
	// Add this blob to the database's queue
	i->second->push_back(blob);

	// Note: Currently we flush the queue after every data blob. So, in
	//       effect, every data blob is written directly to the database.
	//       This needs to be replaced with some reasonable criteria that
	//       groups the blobs into larger transactions while keeping worst
	//       case latency low. Writing every, say, 1 second might be a
	//       reasonable first step.
	
	// Flush the database's queue if non-empty
	if(!i->second->empty()) {
	    flushPerformanceData(identifier_to_database[header.experiment],
				 identifier_to_queue[header.experiment]);
	    identifier_to_queue[header.experiment] =
		SmartPtr<std::deque<Blob> >(new std::deque<Blob>());
	}
	
    }
    
    // Release exclusive access to our unnamed namespace variables
    Assert(pthread_mutex_unlock(&exclusive_access_lock) == 0);
}
