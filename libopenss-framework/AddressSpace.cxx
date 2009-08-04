////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2009 The Krell Institute. All Rights Reserved.
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
 * Definition of the AddressSpace class.
 *
 */

#include "AddressSpace.hxx"
#include "EntrySpy.hxx"
#include "LinkedObject.hxx"
#include "ThreadGroup.hxx"
#include "Time.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Test for a linked object.
 *
 * Returns a boolean value indicating if there is a linked object in this
 * address space for the specified address range.
 *
 * @param range    Address range to be tested.
 * @return         Boolean "true" if there is a linked object at this range,
 *                 "false" otherwise.
 */
bool AddressSpace::hasValue(const AddressRange& range) const
{
    // Find the entry (if any) that overlaps this address range
    std::map<AddressRange, Entry>::const_iterator 
	i = dm_address_range_to_entry.find(range);

    // Indicate to the caller if the range was in this address space
    return (i != dm_address_range_to_entry.end()) && (i->first == range);
}



/**
 * Set a linked object.
 *
 * Sets the linked object in this address space at the specified address range
 * to the specified value. Setting an address range that was previously set will
 * result in the original linked object being replaced with this linked object.
 *
 * @note    In theory an address space will never contain overlapping address
 *          ranges. Either the range should not be found at all, or the found
 *          range should be an exact match. There are various cases, however,
 *          where bogus symbol information results in this happening. This 
 *          function contains sanity checks that help keep the experiment
 *          database, if not 100% correct, at least useable under such
 *          circumstances.
 *
 * @param range            Address range to be set.
 * @param path             Full path name of the linked object.
 * @param is_executable    Flag indicating if the linked object is an
 *                         executable.
 */
void AddressSpace::setValue(const AddressRange& range,
			    const Path& path, const bool& is_executable)
{
    // Find the entry (if any) that overlaps this address range
    std::map<AddressRange, Entry>::iterator 
	i = dm_address_range_to_entry.find(range);

    // Is there no overlapping entry for this range?
    if(i == dm_address_range_to_entry.end()) {
	
	// Add this entry to the address space
	dm_address_range_to_entry.insert(
	    std::make_pair(
		range, 
		Entry(path, is_executable))
	    );
	
    }

    // Otherwise was the overlapping entry an exact match for this range?
    else if(i->first == range) {
	
	// Replace the existing entry in this address space
	i->second = Entry(path, is_executable);
	
    }
}



/**
 * Update threads.
 *
 * Updates the specified threads with this address space. Examines each thread's
 * database to determine if it already contains the symbol table information for
 * each of the linked objects in this address space. Returns a structure that
 * specifies which symbol tables were not found and thus need to be constructed.
 *
 * @param threads    Threads to be updated.
 * @param when       Time at which the update occured.
 * @return           Map from address ranges to linked objects for those linked
 *                   objects that need their symbol table constructed.
 */
std::map<AddressRange, std::set<LinkedObject> >
AddressSpace::updateThreads(const ThreadGroup& threads,
			    const Time& when,
			    const bool update_interval) const
{
    std::map<AddressRange, std::set<LinkedObject> > needed_symbol_tables;
    
    // Iterate over each specified thread
    for(ThreadGroup::const_iterator
	    t = threads.begin(); t != threads.end(); ++t) {
	
	// Begin a transaction on this thread's database
	SmartPtr<Database> database = EntrySpy(*t).getDatabase();
	BEGIN_WRITE_TRANSACTION(database);
	
	//
	// Use Time::TheBeginning() as the "time_begin" value of the to-be-
	// created address space entries if there are currently no address
	// space entries for this thread. Otherwise set "time_begin" to be
	// "when".
	//
	Time time_begin = Time::TheBeginning();
	database->prepareStatement(
	    "SELECT COUNT(*) FROM AddressSpaces WHERE thread = ?;"
	    );
	database->bindArgument(1, EntrySpy(*t).getEntry());
	while(database->executeStatement())
	    if(database->getResultAsInteger(1) > 0)
		time_begin = when;
	
	// Update time interval of active address space entries for this thread
	//
	// FIXME:
	// The openmpi mpirun command (when configured --enable-dlopen) uses dlopen
	// to load in the openmpi mca plugins that handle starting up the mpi rank
	// processes. This causes some 20 plugins to be loaded which in turn initiates
	// addressSpaceChangeCallbacks for each plugin.  As each callback is made,
	// updateThreads is called and an entry in the AddressSpaces table is created
	// with Time::TheEnd.  Subsequent addressSpaceChangeCallbacks made while the
	// framework is still processing previous addressSpaceChangeCallbacks can cause
	// the update of the time_end from Time::TheEnd to the current "when" time to be wrong
	// for previous addressSpaceChangeCallbacks that have not yet completed their
	// requestAddressSpace calls (and therefore have not finished expanding the
	// module that initiated the addressSpaceChangeCallback). This causes entries in
	// the AddressSpace table of the database to have invalid time intervals.
	//
	// It should also be noted that an addressSpaceChangeCallback only informs the
	// framework that a dlopen has occured. Since we handle these requests asynchronously,
	// the application may unload the module before our actual requestAddressSpace is made
	// (or while we are asking dpcl/dyninst to expand the module). In this case, we will
	// have entries in the AddressSpace table of the database for which no module was
	// actually expanded and such entries will have no symbol information.
	//
	// The update_interval flag is set "false" only by addressSpaceChangeCallback to
	// prevent the updating of the time_end for any requestAddressSpace initiated by
	// an addressSpaceChangeCallback.  This means that any AddressSpace in the
	// database caused by a dlopen during a mutatee's execution will have
	// Time::TheEnd as it's time_end. All other requestAddressSpace calls will
	// allow the updating of the time_end to when in the code below.
	//
	if (update_interval) {
	    database->prepareStatement(
	        "UPDATE AddressSpaces "
	        "SET time_end = ? "
	        "WHERE thread = ? "
	        "  AND time_end = ?;"
	        );
	    database->bindArgument(1, when);
	    database->bindArgument(2, EntrySpy(*t).getEntry());
	    database->bindArgument(3, Time::TheEnd());
	    while(database->executeStatement());
	}

	// Iterate over each linked object entry in this address space
	for(std::map<AddressRange, Entry>::const_iterator
		e = dm_address_range_to_entry.begin();
	    e != dm_address_range_to_entry.end();
	    ++e) {
	    
	    // Is there an existing linked object in the database?
	    int linked_object = -1;
	    database->prepareStatement(
		"SELECT LinkedObjects.id "
		"FROM LinkedObjects "
		"  JOIN Files "
		"ON LinkedObjects.file = Files.id "
		"WHERE Files.path = ?;"
		);
	    database->bindArgument(1, e->second.dm_path);
	    while(database->executeStatement())
		linked_object = database->getResultAsInteger(1);
	    bool needs_symbol_table = (linked_object == -1);

	    // Create this linked object if it wasn't present in the database
	    if(linked_object == -1) {
		
		// Create the file entry
		database->prepareStatement(
		    "INSERT INTO Files (path) VALUES (?);"
		    );
		database->bindArgument(1, e->second.dm_path);
		while(database->executeStatement());
		int file = database->getLastInsertedUID();
		
		// Create the linked object entry
		database->prepareStatement(
		    "INSERT INTO LinkedObjects "
		    "  (addr_begin, addr_end, file, is_executable) "
		    "VALUES (0, ?, ?, ?);"
		    );
		database->bindArgument(1, Address(e->first.getEnd() - 
						  e->first.getBegin()));
		database->bindArgument(2, file);    
		database->bindArgument(3, e->second.dm_is_executable);
		while(database->executeStatement());
		linked_object = database->getLastInsertedUID();
		
	    }
	    
	    // Create an address space entry for this linked object
	    database->prepareStatement(
		"INSERT INTO AddressSpaces "
		"  (thread, time_begin, time_end, "
		"   addr_begin, addr_end, linked_object) "
		"VALUES (?, ?, ?, ?, ?, ?);"
		);
	    database->bindArgument(1, EntrySpy(*t).getEntry());
	    database->bindArgument(2, time_begin);
	    database->bindArgument(3, Time::TheEnd());
	    database->bindArgument(4, e->first.getBegin());
	    database->bindArgument(5, e->first.getEnd());
	    database->bindArgument(6, linked_object);
	    while(database->executeStatement());

	    // Does a symbol table need to be built for this linked object?
	    if(needs_symbol_table) {
		
		// Create entry for this symbol table (or find existing one)
		std::map<AddressRange, std::set<LinkedObject> >::iterator
		    i = needed_symbol_tables.insert(
			std::make_pair(
			    e->first,
			    std::set<LinkedObject>())
			).first;
		
		// Add this linked object to the entry
		i->second.insert(LinkedObject(database, linked_object));
		
	    }
	    
	}
	
	// End the transaction on this thread's database
	END_TRANSACTION(database);    
	
    }

    // Return the needed symbol tables to the caller
    return needed_symbol_tables;
}



/**
 * Update thread.
 *
 * This is used by the OfflineExperiment class.
 * Updates the specified thread with this address space. Examines the thread's
 * database to determine if it already contains the symbol table information for
 * each of the linked objects in this address space. Returns a structure that
 * specifies which symbol tables were not found and thus need to be constructed.
 *
 * @param thread     Thread to be updated.
 * @return           Map from address ranges to linked objects for those linked
 *                   objects that need their symbol table constructed.
 */
std::map<AddressRange, std::set<LinkedObject> >
AddressSpace::updateThread(const Thread& thread) const
{
    std::map<AddressRange, std::set<LinkedObject> > needed_symbol_tables;

    // Begin a transaction on this thread's database
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();
    BEGIN_WRITE_TRANSACTION(database);

    //
    // Use Time::TheBeginning() as the "time_begin" value of the to-be-
    // created address space entries.
    //
	Time time_begin = Time::TheBeginning();
	
    // Iterate over each linked object entry in this address space
    for(std::map<AddressRange, Entry>::const_iterator
		e = dm_address_range_to_entry.begin();
	    e != dm_address_range_to_entry.end();
	    ++e) {
	    
	// Is there an existing linked object in the database?
	int linked_object = -1;
	database->prepareStatement(
		"SELECT LinkedObjects.id "
		"FROM LinkedObjects "
		"  JOIN Files "
		"ON LinkedObjects.file = Files.id "
		"WHERE Files.path = ?;"
		);
	database->bindArgument(1, e->second.dm_path);
	while(database->executeStatement())
		linked_object = database->getResultAsInteger(1);

	// Create this linked object if it wasn't present in the database
	if(linked_object == -1) {

	    // Create the file entry
	    database->prepareStatement(
		    "INSERT INTO Files (path) VALUES (?);"
		    );
	    database->bindArgument(1, e->second.dm_path);
	    while(database->executeStatement());

	    int file = database->getLastInsertedUID();
		
	    // Create the linked object entry
	    database->prepareStatement(
		    "INSERT INTO LinkedObjects "
		    "  (addr_begin, addr_end, file, is_executable) "
		    "VALUES (0, ?, ?, ?);"
		    );
	    database->bindArgument(1, Address(e->first.getEnd() - 
						  e->first.getBegin()));
	    database->bindArgument(2, file);    
	    database->bindArgument(3, e->second.dm_is_executable);
	    while(database->executeStatement());

	    linked_object = database->getLastInsertedUID();
		
	}
	    
	// Create an address space entry for this linked object
	database->prepareStatement(
		"INSERT INTO AddressSpaces "
		"  (thread, time_begin, time_end, "
		"   addr_begin, addr_end, linked_object) "
		"VALUES (?, ?, ?, ?, ?, ?);"
		);
	database->bindArgument(1, EntrySpy(thread).getEntry());
	database->bindArgument(2, time_begin);
	database->bindArgument(3, Time::TheEnd());
	database->bindArgument(4, e->first.getBegin());
	database->bindArgument(5, e->first.getEnd());
	database->bindArgument(6, linked_object);
	while(database->executeStatement());

	// Does a symbol table need to be built for this linked object?

	// Create entry for this symbol table (or find existing one)
	std::map<AddressRange, std::set<LinkedObject> >::iterator
		    i = needed_symbol_tables.insert(
			std::make_pair(
			    e->first,
			    std::set<LinkedObject>())
			).first;

	// Add this linked object to the entry
	i->second.insert(LinkedObject(database, linked_object));

    }

    // End the transaction on this thread's database
    END_TRANSACTION(database);    

    // Return the needed symbol tables to the caller
    return needed_symbol_tables;
}
