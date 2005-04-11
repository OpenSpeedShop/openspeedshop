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
#include "Exception.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
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

    // Find our context's thread
    BEGIN_TRANSACTION(dm_database);
    validate("LinkedObjects");
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
    Path path;

    // Find our full path name
    BEGIN_TRANSACTION(dm_database);
    validate("LinkedObjects");
    dm_database->prepareStatement(
	"SELECT Files.path "
	"FROM Files "
	"  JOIN LinkedObjects "
	"ON Files.id = LinkedObjects.file "
	"WHERE LinkedObjects.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	if(!path.empty())
	    throw Exception(Exception::EntryNotUnique, "Files",
			    "<LinkedObjects-Referenced>");
	path = Path(dm_database->getResultAsString(1));
    }
    if(path.empty())
	throw Exception(Exception::EntryNotFound, "Files",
			"<LinkedObjects-Referenced>");
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

    // Find our context's address range
    BEGIN_TRANSACTION(dm_database);
    validate("LinkedObjects");
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
 * Returns the functions contained within this linked object. An empty set is
 * returned if no functions are found within this linked object.
 *
 * @note    This is a relatively high cost operation in terms of time and memory
 *          used. Avoid using this member function if possible.
 *
 * @return    Functions contained within this linked object.
 */
std::set<Function> LinkedObject::getFunctions() const
{
    std::set<Function> functions;

    // Find our functions
    BEGIN_TRANSACTION(dm_database);
    validate("LinkedObjects");
    dm_database->prepareStatement(
	"SELECT id FROM Functions WHERE linked_object = ?;"
	);	
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	functions.insert(Function(dm_database, 
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
    Entry()
{
}



/**
 * Constructor from a linked object entry.
 *
 * Constructs a new LinkedObject for the specified linked object entry.
 *
 * @param database    Database containing this linked object.
 * @param entry       Identifier for this linked object.
 * @param context     Identifier of the context for this linked object.
 */
LinkedObject::LinkedObject(const SmartPtr<Database>& database,
			   const int& entry, const int& context) :
    Entry(database, entry, context)
{
}
