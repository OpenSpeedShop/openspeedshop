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
 * Definition of the ExperimentTable class.
 *
 */

#include "Address.hxx"
#include "Blob.hxx"
#include "Exception.hxx"
#include "Experiment.hxx"
#include "ExperimentTable.hxx"
#include "Guard.hxx"
#include "runtime/OpenSS_DataHeader.h"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton experiment table. */
ExperimentTable ExperimentTable::TheTable;



/**
 * Add an experiment.
 *
 * Adds the passed experiment to this experiment table. The experiment is given
 * the next available unique integer as its identifier.
 *
 * @note    An assertion failure occurs if an attempt is made to add a null
 *          experiment or to add an experiment more than once.
 *
 * @param experiment    Experiment to be added.
 */
void ExperimentTable::addExperiment(const Experiment* experiment)
{
    Guard guard_myself(this);

    // Check assertions
    Assert(experiment != NULL);
    Assert(dm_experiment_to_identifier.find(experiment) == 
	   dm_experiment_to_identifier.end());

    // Add this experiment
    int identifier = dm_next_identifier++;
    dm_identifier_to_experiment[identifier] = experiment;
    dm_experiment_to_identifier[experiment] = identifier;
}



/**
 * Remove an experiment.
 *
 * Removes the passed experiment from this experiment table.
 *
 * @note    An assertion failure occurs if an attempt is made to remove a null
 *          experiment or to remove an experiment that isn't in this experiment
 *          table.
 *
 * @param experiment    Experiment to be removed.
 */
void ExperimentTable::removeExperiment(const Experiment* experiment)
{
    Guard guard_myself(this);

    // Check assertions
    Assert(experiment != NULL);
    Assert(dm_experiment_to_identifier.find(experiment) != 
	   dm_experiment_to_identifier.end());

    // Remove this experiment
    int identifier = dm_experiment_to_identifier[experiment];    
    dm_identifier_to_experiment.erase(identifier);
    dm_experiment_to_identifier.erase(experiment);
}



/**
 * Get experiment identifier.
 *
 * Returns the experiment identifier of the experiment associated with the
 * specified database. An invalid identifier is returned if no such experiment
 * can be found. Used by collectors to specify for which experiment data is to
 * be collected.
 *
 * @param database    Database to be identified.
 * @return            Identifier of the experiment associated with that database
 *                    or an invalid identifier if no such experiment is found.
 */
int ExperimentTable::getIdentifier(const SmartPtr<Database>& database) const
{
    Guard guard_myself(this);
    
    // Iterate over every experiment in the experiment table
    for(std::map<const Experiment*, int>::const_iterator
	    i = dm_experiment_to_identifier.begin();
	i != dm_experiment_to_identifier.end();
	++i)
	
	// Is this the experiment associated with the specified database?
	if(i->first->dm_database == database)
	    return i->second;
    
    // Otherwise return an invalid identifier (-1 is always invalid)
    return -1;    
}



/**
 * Store performance data in an experiment.
 *
 * Stores the specified performance data into the correct experiment database.
 * The blob containing the data is assumed to have been encoded by OpenSS_Send
 * and is thus prepended with a performance data header. That header is decoded
 * in order to locate the correct experiment database and then create a properly
 * indexed entry for the actual data.
 *
 * @note    Various timing issues can result in corner cases where, for example,
 *          data arrives after a particular experiment, collector, or thread has
 *          been destroyed/removed. It was deemed best to silently ignore the
 *          data under these circumstances.
 *
 * @parma blob    Blob containing the performance data.
 */
void ExperimentTable::storePerformanceData(const Blob& blob) const
{
    Guard guard_myself(this);

    // Decode the performance data header
    OpenSS_DataHeader header;
    memset(&header, 0, sizeof(header));
    unsigned header_size =
	blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_OpenSS_DataHeader),
			    &header);

    // Silently ignore the data if the address range is invalid
    if(header.addr_begin >= header.addr_end)
	return;

    // Silently ignore the data if the time interval is invalid
    if(header.time_begin >= header.time_end)
	return;

    // Find the experiment to contain this data
    std::map<int, const Experiment*>::const_iterator
	i = dm_identifier_to_experiment.find(header.experiment);
    
    // Silently ignore the data if the experiment cannot be found
    if(i == dm_identifier_to_experiment.end())
	return;
    
    // Find the database for this experiment
    SmartPtr<Database> database = i->second->dm_database;
    
    // Store the data in this database
    try {
	
	// Begin a multi-statement transaction
	BEGIN_TRANSACTION(database);

	// Validate that the specified collector exists
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Collectors WHERE id = ?;"
	    );
	database->bindArgument(1, header.collector);
	while(database->executeStatement()) {
	    int rows = database->getResultAsInteger(1);
	    if(rows == 0)
		throw Exception(Exception::EntryNotFound, "Collectors",
				Exception::toString(header.collector));
	    else if(rows > 1)
		throw Exception(Exception::EntryNotUnique, "Collectors",
				Exception::toString(header.collector));
	}
	
	// Validate that the specified thread exists
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Threads WHERE id = ?;"
	    );
	database->bindArgument(1, header.thread);
	while(database->executeStatement()) {
	    int rows = database->getResultAsInteger(1);
	    if(rows == 0)
		throw Exception(Exception::EntryNotFound, "Threads",
				Exception::toString(header.thread));
	    else if(rows > 1)
		throw Exception(Exception::EntryNotUnique, "Threads",
				Exception::toString(header.thread));
	}

	// Calculate the size and location of the actual data
	unsigned data_size = blob.getSize() - header_size;
	const void* data_ptr =
	    &(reinterpret_cast<const char*>(blob.getContents())[header_size]);

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
	
	// End this multi-statement transaction
	END_TRANSACTION(database);
	
    }
    
    // Silently ignore the data if any exceptions were thrown
    catch(...) {	
    }
}
