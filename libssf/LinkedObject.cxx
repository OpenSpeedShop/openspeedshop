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
 * Definition of the LinkedObject class.
 *
 */

#include "Assert.hxx"
#include "Database.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Optional.hxx"
#include "Path.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our thread.
 *
 * Returns the thread containing this linked object.
 *
 * @return    Thread containing this linked object.
 */
Thread LinkedObject::getThread() const
{
    Thread thread;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our context's thread
    BEGIN_TRANSACTION(dm_database);
    validateContext();
    dm_database->prepareStatement(
	"SELECT thread FROM AddressSpaces WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_context);
    while(dm_database->executeStatement())
	thread = Thread(dm_database, dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the thread to the caller
    return thread;
}



/**
 * Get our path.
 *
 * Returns the full path name of this linked object.
 *
 * @return    Full path name of this linked object.
 */
Path LinkedObject::getPath() const
{
    Optional<Path> path;

    // Check assertions
    Assert(!dm_database.isNull());
    
    // Find our full path name
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT Files.path FROM Files JOIN LinkedObjects"
	" ON Files.id = LinkedObjects.file WHERE LinkedObjects.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {	
	if(path.hasValue())
	    throw Database::Corrupted(*dm_database, "file entry is not unique");
	path = Path(dm_database->getResultAsString(1));
    }
    if(!path.hasValue())
	throw Database::Corrupted(*dm_database, "file entry no longer exists");
    END_TRANSACTION(dm_database);
    
    // Return the full path name to the caller
    return path;
}



/**
 * Get our address range.
 *
 * Returns the address range of this linked object.
 *
 * @return    Address range of this linked object.
 */
AddressRange LinkedObject::getAddressRange() const
{
    AddressRange range;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our context's address range
    BEGIN_TRANSACTION(dm_database);
    validateContext();
    dm_database->prepareStatement(
	"SELECT addr_begin, addr_end FROM AddressSpaces WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_context);
    while(dm_database->executeStatement())
	range = AddressRange(dm_database->getResultAsAddress(1),
			     dm_database->getResultAsAddress(2));
    END_TRANSACTION(dm_database);
    
    // Return the address range to the caller
    return range;
}



/**
 * Get our functions.
 *
 * Returns the functions contained within this linked object. An empty list is
 * returned if no functions are found within this linked object (unlikely).
 *
 * @note    This is a relatively high cost operation in terms of time and memory
 *          used. Avoid using this member function if possible.
 *
 * @return    Functions contained within this linked object.
 */
std::vector<Function> LinkedObject::getFunctions() const
{
    std::vector<Function> functions;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our functions
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT id FROM Functions WHERE linked_object = ?;"
	);	
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	functions.push_back(Function(dm_database, 
				     dm_database->getResultAsInteger(1),
				     dm_context));    
    END_TRANSACTION(dm_database);
    
    // Return the functions to the caller
    return functions;
}



/**
 * Default constructor.
 *
 * Constructs a LinkedObject that refers to a non-existent linked object. Any
 * use of a member function on an object constructed in this way will result in
 * an assertion failure.
 */
LinkedObject::LinkedObject() :
    dm_database(NULL),
    dm_entry(0),
    dm_context(0)
{
}



/**
 * Constructor from a linked object entry.
 *
 * Constructs a new LinkedObject for the specified linked object entry within
 * the passed database, given the context of the specified address space entry.
 *
 * @param database    Database containing the linked object.
 * @param entry       Entry (id) for the linked object.
 * @param context     Address space entry (id) context for the linked object.
 */
LinkedObject::LinkedObject(const SmartPtr<Database>& database,
			   const int& entry, const int& context) :
    dm_database(database),
    dm_entry(entry),
    dm_context(context)
{
}



/**
 * Validate our entry.
 *
 * Validates the existence and uniqueness of our entry within our database. If
 * our entry is found and is unique, this function simply returns. Otherwise
 * an exception of type Database::Corrupted is thrown.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 */
void LinkedObject::validateEntry() const
{
    // Find the number of rows matching our entry
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM LinkedObjects WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate
    if(rows == 0)
	throw Database::Corrupted(*dm_database,
				  "linked object entry no longer exists");
    else if(rows > 1)
	throw Database::Corrupted(*dm_database,
				  "linked object entry is not unique");
}



/**
 * Validate our context.
 *
 * Validates the existence and uniqueness of our context within the database. If
 * our context is found and is unique, this function simply returns. Otherwise
 * an exception of type Database::Corrupted is thrown.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 */
void LinkedObject::validateContext() const
{
    // Find the number of rows matching our context
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM AddressSpaces WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_context);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate
    if(rows == 0)
	throw Database::Corrupted(*dm_database,
				  "address space entry no longer exists");
    else if(rows > 1)
	throw Database::Corrupted(*dm_database,
				  "address space entry is not unique");
}
