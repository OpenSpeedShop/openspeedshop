////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the DataCache class.
 *
 */

#include "Collector.hxx"
#include "DataCache.hxx"
#include "EntrySpy.hxx"
#include "Guard.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get intersecting identifiers.
 *
 * Returns the performance data blob identifiers associated with the passed
 * collector and thread objects intersecting the passed extent. An empty set
 * is returned if no identifiers are found.
 *
 * @param collector    Collector for which to find identifiers.
 * @param thread       Thread for which to find identifiers.
 * @param extent       Extent with which to intersect.
 * @return             Identifiers intersecting this extent.
 */
std::set<int> DataCache::getIdentifiers(const Collector& collector,
					const Thread& thread,
					const Extent& extent)
{
    Guard guard_myself(this);

    // Construct a key from the collector/thread pair
    std::pair<Collector, Thread> key = std::make_pair(collector, thread);

    // Find this key in the cache (adding it if necessary)
    ExtentGroup& cached = dm_cache.getExtents(key);
    if(cached.empty()) {
	addIdentifiers(key);
	cached = dm_cache.getExtents(key);
    }

    // Intersect the extent with this key's cached identifiers
    std::set<ExtentGroup::size_type> intersection =
	cached.getIntersectionWith(extent);
    
    // Assemble the intersection results into an identifier set
    std::set<int> identifiers;
    for(std::set<ExtentGroup::size_type>::const_iterator
	    i = intersection.begin(); i != intersection.end(); ++i)
	identifiers.insert(dm_cache.getObject(key, *i));
    
    // Return the identifiers to the caller
    return identifiers;
}



/**
 * Add an identifier.
 *
 * Adds the passed identifier for a performance data blob to the cache. Used by
 * DataQueues to insure the cache stays up-to-date as new data arrives.
 *
 * @param database      Database containing the data.
 * @param collector     Collector that gathered the data.
 * @param thread        Thread for which the data was gathered.
 * @param extent        Extent of the data.
 * @param identifier    Identifier of the data.
 */
void DataCache::addIdentifier(const SmartPtr<Database>& database,
			      const int& collector,
			      const int& thread,
			      const Extent& extent, 
			      const int& identifier)
{
    Guard guard_myself(this);

    // Construct a key from the collector/thread pair
    std::pair<Collector, Thread> key = std::make_pair(
	Collector(database, collector), Thread(database, thread)
	);

    // Add this key's identifiers to the cache (if necessary)
    if(dm_cache.getExtents(key).empty())
	addIdentifiers(key);
    
    // Add this identifier to the cache
    dm_cache.addExtent(key, identifier, extent);
}



/**
 * Remove a database.
 *
 * Removes all the identifiers associated with the passed database from
 * the cache.
 *
 * @param database    Database to be removed from the cache.
 */
void DataCache::removeDatabase(const SmartPtr<Database>& database)
{
    Guard guard_myself(this);

    // Allocate these sets outside the transaction's try/catch block
    std::set<Collector> collectors;
    std::set<Thread> threads;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);

    // Find the collectors in this database
    database->prepareStatement("SELECT id FROM Collectors;");
    while(database->executeStatement())
	collectors.insert(Collector(database, database->getResultAsInteger(1)));
    
    // Find the threads in this database
    database->prepareStatement("SELECT id FROM Threads;");
    while(database->executeStatement())
	threads.insert(Thread(database, database->getResultAsInteger(1)));
    
    // End this multi-statement transaction
    END_TRANSACTION(database);

    // Remove all the collector/thread pairs from the cache
    for(std::set<Collector>::const_iterator
	    i = collectors.begin(); i != collectors.end(); ++i)
	for(std::set<Thread>::const_iterator
		j = threads.begin(); j != threads.end(); ++j)
	    dm_cache.removeExtents(std::make_pair(*i, *j));
}



/**
 * Remove a collector.
 *
 * Remove all the identifiers associated with the passed collector from
 * the cache.
 *
 * @param collector    Collector to be removed from the cache.
 */
void DataCache::removeCollector(const Collector& collector)
{
    Guard guard_myself(this);

    // Find the threads in the collector's database
    std::set<Thread> threads;
    SmartPtr<Database> database = EntrySpy(collector).getDatabase();
    BEGIN_TRANSACTION(database);
    database->prepareStatement("SELECT id FROM Threads;");
    while(database->executeStatement())
	threads.insert(Thread(database, database->getResultAsInteger(1)));
    END_TRANSACTION(database);

    // Remove these collector/thread pairs from the cache
    for(std::set<Thread>::const_iterator
	    i = threads.begin(); i != threads.end(); ++i)
	dm_cache.removeExtents(std::make_pair(collector, *i));
}



/**
 * Remove a thread.
 *
 * Remove all the identifiers associated with the passed thread from the cache.
 *
 * @param thread    Thread to be removed from the cache.
 */
void DataCache::removeThread(const Thread& thread)
{
    Guard guard_myself(this);

    // Find the collectors in the thread's database
    std::set<Collector> collectors;
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_TRANSACTION(database);
    database->prepareStatement("SELECT id FROM Collectors;");
    while(database->executeStatement())
	collectors.insert(Collector(database, database->getResultAsInteger(1)));
    END_TRANSACTION(database);

    // Remove these collector/thread pairs from the cache
    for(std::set<Collector>::const_iterator
	    i = collectors.begin(); i != collectors.end(); ++i)
	dm_cache.removeExtents(std::make_pair(*i, thread));
}



/**
 * Add a colletor/thread pair.
 *
 * Adds the passed collector/thread pair to the cache. All the performance data
 * blob identifiers associated with the pair are located and added as a group to
 * the cache. Future queries make use of this cached data.
 *
 * @param key    Collector/thread pair to be added to the cache.
 */
void DataCache::addIdentifiers(const std::pair<Collector, Thread>& key)
{
    SmartPtr<Database> database = EntrySpy(key.first).getDatabase();

    // Check assertions
    Assert(key.first.inSameDatabase(key.second));

    // Find the performance data associated with this key
    BEGIN_TRANSACTION(database);
    EntrySpy(key.first).validate();
    EntrySpy(key.second).validate();
    database->prepareStatement(
	"SELECT ROWID, "
	"       time_begin, time_end, "
	"       addr_begin, addr_end "
	"FROM Data "
	"WHERE collector = ? AND thread = ?;"
	);
    database->bindArgument(1, EntrySpy(key.first).getEntry());
    database->bindArgument(2, EntrySpy(key.second).getEntry());
    while(database->executeStatement())

	dm_cache.addExtent(
	    key, database->getResultAsInteger(1),
	    Extent(
		TimeInterval(database->getResultAsTime(2), 
			     database->getResultAsTime(3)),
		AddressRange(database->getResultAsAddress(4),
			     database->getResultAsAddress(5))
		)
	    );

    END_TRANSACTION(database);
}
