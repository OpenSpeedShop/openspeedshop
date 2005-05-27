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
 * Definition of the Collector class.
 *
 */

#include "Collector.hxx"
#include "CollectorPluginTable.hxx"
#include "ThreadGroup.hxx"

#include <typeinfo>

using namespace OpenSpeedShop::Framework;



/**
 * Get all available collectors.
 *
 * Returns the metadata for all available collectors. An empty set is returned
 * if no collector plugins were found.
 *
 * @return    Metadata for all available collectors.
 */
std::set<Metadata> Collector::getAvailable()
{
    // Defer to the collector plugin table
    return CollectorPluginTable::TheTable.getAvailable();
}



/**
 * Copy constructor.
 *
 * Constructs a new Collector by copying the specified collector. The compiler
 * provided default is insufficient here because we need to instantiate another,
 * separate, implementation for the new copy.
 *
 * @param other    Collector to be copied.
 */
Collector::Collector(const Collector& other) :
    Entry(other.dm_database, other.dm_entry, 0),
    dm_impl(NULL)
{
}



/**
 * Destructor.
 *
 * Destroys the collector's implementation if it had one.
 */
Collector::~Collector()
{
    // Destroy the implementation (if any)
    if(dm_impl != NULL)
	CollectorPluginTable::TheTable.destroy(dm_impl);
}



/**
 * Assignment operator.
 *
 * Operator "=" defined for a Collector object. The compiler provided default
 * is insufficient here because we need to instantiate another, separate,
 * implementation for the new copy.
 * 
 * @param other    Collector to be copied.
 */
Collector& Collector::operator=(const Collector& other)
{
    // Only do the actual assignment if the collectors differ
    if(*this != other) {
	
	// Destroy our current implementation (if any)
	if(dm_impl != NULL)
	    CollectorPluginTable::TheTable.destroy(dm_impl);
	
	// Replace our collector with the new collector
	Entry::operator=(other);
	dm_impl = NULL;
	
    }
    
    // Return ourselves to the caller
    return *this;
}



/**
 * Get our metadata.
 *
 * Returns the metadata of this collector.
 *
 * @note    Information will be limited to the collector's unique identiifer
 *          when the collector's implementation cannot be instantiated.
 *
 * @return    Metadata for this collector.
 */
Metadata Collector::getMetadata() const
{
    // Defer to our implementation (if any)    
    if(dm_impl == NULL)
	instantiateImpl();
    if(dm_impl != NULL)
        return *dm_impl;
    
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the partial metadata to the caller
    return Metadata(unique_id, "", "", typeid(Collector));
}



/**
 * Get our parameters.
 *
 * Returns the metadata for all parameters of this collector. An empty set is
 * returned if this collector has no parameters.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @return    Metadata for all parameters of this collector.
 */
std::set<Metadata> Collector::getParameters() const
{
    // Check preconditions
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    } 
    
    // Defer to our implementation
    return dm_impl->getParameters();    
}



/**
 * Get our metrics.
 *
 * Returns the metadata for all metrics of this collector. An empty set is
 * returned if this collector has no metrics (unlikely).
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @return    Metadata for all metrics of this collector.
 */
std::set<Metadata> Collector::getMetrics() const
{
    // Check preconditions
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    } 

    // Defer to our implementation
    return dm_impl->getMetrics();
}



/**
 * Get our threads.
 *
 * Returns all the threads for which this collector is collecting performance
 * data. An empty thread group is returned if this collector isn't collecting
 * performance data for any threads.
 *
 * @return    Threads for which this collector is collecting performance data.
 */
ThreadGroup Collector::getThreads() const
{
    ThreadGroup threads;

    // Find the threads for which we are collecting
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT thread "
	"FROM Collecting "
	"WHERE collector = ? "
	"  AND is_postponed = 0;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our postponed threads.
 *
 * Returns all the threads for which this collector has postponed collection of
 * performance data. An empty thread group is returned if this collector isn't
 * postponing collection of performance data for any threads.
 *
 * @return    Threads for which this collector has postponed collection of
 *            performance data.
 */
ThreadGroup Collector::getPostponedThreads() const
{
    ThreadGroup threads;

    // Find the threads for which we are postponing collection
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT thread "
	"FROM Collecting "
	"WHERE collector = ? "
	"  AND is_postponed = 1;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads; 
}



/**
 * Start data collection.
 *
 * Starts collection of performance data by this collector for the specified
 * thread. Data collection can be stopped temporarily via postponeCollecting()
 * or permanently via stopCollecting(). All data that is collected is available
 * via the collector's metrics.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the collector.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @note    Any attempt to start collection on a thread for which this collector
 *          is already collecting data will be silently ignored.
 *
 * @param thread    Thread for which to start collecting performance data.
 */
void Collector::startCollecting(const Thread& thread) const
{
    // Check preconditions
    Assert(thread.dm_database == dm_database);
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    }
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    thread.validate("Threads");

    // Are we collecting and/or postponed for this thread?
    bool is_collecting = false;
    bool is_postponed = true;
    dm_database->prepareStatement(
	"SELECT is_postponed "
	"FROM Collecting "
	"WHERE collector = ? "
	"  AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement()) {
	is_collecting = true;
	is_postponed = dm_database->getResultAsInteger(1) != 0;
    }

    // Create the collecting entry if it wasn't present in the database
    if(!is_collecting) {
	dm_database->prepareStatement(
	    "INSERT INTO Collecting "
	    "  (collector, thread, is_postponed) "
	    "VALUES (?, ?, 1);"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
    }
    
    // Only start collection if we are postponed
    if(is_postponed) {

	// Defer to our implementation
	dm_impl->startCollecting(*this, thread);
	
	// Note in the database that we are no longer postponed
	dm_database->prepareStatement(
	    "UPDATE Collecting "
	    "SET is_postponed = 0 "
	    "WHERE collector = ? "
	    "  AND thread = ?;"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Postpone data collection.
 *
 * Postpone collection of performance data by this collector for the specified
 * thread. Data collection can be resumed again via startCollecting(). All data
 * that was collected is available via the collector's metrics.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the collector.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @note    Any attempt to postpone collection on a thread for which this
 *          collector is not collecting data will be silently ignored.
 *
 * @param thread    Thread for which to postpone collecting performance data.
 */
void Collector::postponeCollecting(const Thread& thread) const
{
    // Check preconditions
    Assert(thread.dm_database == dm_database);
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    }

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    thread.validate("Threads");

    // Are we collecting and/or postponed for this thread?
    bool is_collecting = false;
    bool is_postponed = true;
    dm_database->prepareStatement(
	"SELECT is_postponed "
	"FROM Collecting "
	"WHERE collector = ? "
	"  AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement()) {
	is_collecting = true;
	is_postponed = dm_database->getResultAsInteger(1) != 0;
    }

    // Only stop collection if we are collecting and not postponed
    if(is_collecting && !is_postponed) {

	// Defer to our implementation
	dm_impl->stopCollecting(*this, thread);
	
	// Note in the database that we are now postponed
	dm_database->prepareStatement(
	    "UPDATE Collecting "
	    "SET is_postponed = 1 "
	    "WHERE collector = ? "
	    "  AND thread = ?;"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 
}



/**
 * Stop data collection.
 *
 * Stops collection of performance data by this collector for the specified
 * thread. Data collection can be start again via startCollecting(). All data
 * that was collected is available via the collector's metrics.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the collector.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @note    Any attempt to stop collection on a thread for which this collector
 *          is not collecting data will be silently ignored.
 *
 * @param thread    Thread for which to stop collecting performance data.
 */
void Collector::stopCollecting(const Thread& thread) const
{
    // Check preconditions
    Assert(thread.dm_database == dm_database);
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    }

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    thread.validate("Threads");

    // Are we collecting and/or postponed for this thread?
    bool is_collecting = false;
    bool is_postponed = true;
    dm_database->prepareStatement(
	"SELECT is_postponed "
	"FROM Collecting "
	"WHERE collector = ? "
	"  AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement()) {
	is_collecting = true;
	is_postponed = dm_database->getResultAsInteger(1) != 0;
    }

    // Only proceed further if we are already collecting
    if(is_collecting) {

	// Only stop collection if we aren't postponed
	if(!is_postponed) {
	    
	    // Defer to our implementation
	    dm_impl->stopCollecting(*this, thread);
	    
	}
	
	// Note in the database we are no longer collecting for this thread
	dm_database->prepareStatement(
	    "DELETE FROM Collecting WHERE collector = ? AND thread = ?;"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database); 
}



/**
 * Default constructor.
 *
 * Constructs a Collector that refers to a non-existent collector. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Collector::Collector() :
    Entry(),
    dm_impl(NULL)
{
}



/**
 * Constructor from a collector entry.
 *
 * Constructs a new Collector for the specified collector entry.
 *
 * @param database    Database containing this collector.
 * @param entry       Identifier for this collector.
 */
Collector::Collector(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, entry, 0),
    dm_impl(NULL)
{
}



/**
 * Instantiate our implementation.
 *
 * Instantiates an implementation for this collector. If the instantiation fails
 * for any reason, the collector's implementation pointer will still be null.
 */
void Collector::instantiateImpl() const
{
    // Check assertions
    Assert(dm_impl == NULL);
    
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Attempt to instantiate an implementation for this collector
    dm_impl = CollectorPluginTable::TheTable.instantiate(unique_id);  
}



/**
 * Get our parameter data.
 *
 * Return the parameter data of this collector.
 *
 * @return    Parameter data for this collector.
 */
Blob Collector::getParameterData() const
{
    Blob data;

    // Find our parameter data
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT parameter_data FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        data = dm_database->getResultAsBlob(1);
    END_TRANSACTION(dm_database);

    // Return the parameter data to the caller
    return data;
}



/**
 * Set our parameter data.
 *
 * Sets the paramter data of this collector.
 *
 * @param data    Parameter data to be set.
 */
void Collector::setParameterData(const Blob& data) const
{
    // Update our parameter data
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
	"UPDATE Collectors SET parameter_data = ? WHERE id = ?;"
	);
    dm_database->bindArgument(1, data);
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement());
    END_TRANSACTION(dm_database);
}
