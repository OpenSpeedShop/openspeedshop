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
 * Definition of the Statement class.
 *
 */

#include "Assert.hxx"
#include "Database.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Optional.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our thread.
 *
 * Returns the thread containing this statement.
 *
 * @return    Thread containing this statement.
 */
Thread Statement::getThread() const
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
 * Get our linked object.
 *
 * Returns the linked object containing this statement.
 *
 * @return    Linked object containing this statement.
 */
LinkedObject Statement::getLinkedObject() const
{
    LinkedObject linked_object;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our context's linked object
    BEGIN_TRANSACTION(dm_database);
    validateContext();
    dm_database->prepareStatement(
	"SELECT linked_object FROM AddressSpaces WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_context);	
    while(dm_database->executeStatement())
	linked_object = LinkedObject(dm_database,
				     dm_database->getResultAsInteger(1),
				     dm_context);
    END_TRANSACTION(dm_database);
    
    // Return the linked object to the caller
    return linked_object;
}



/**
 * Get functions.
 *
 * Returns the functions containing this statement. If no function contains this
 * statement (unlikely), an empty list is returned.
 *
 * @return    Functions containing this statement.
 */
std::vector<Function> Statement::getFunctions() const
{
    std::vector<Function> functions;

    // Check assertions
    Assert(!dm_database.isNull());
  
    // Find the functions intersecting our address ranges
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    validateContext();
    dm_database->prepareStatement(
	"SELECT Functions.id FROM Functions JOIN StatementRanges"
	" ON Functions.linked_object = StatementRanges.linked_object"
	" WHERE StatementRanges.statement = ?"
	"   AND StatementRanges.linked_object ="
	"       (SELECT linked_object FROM AddressSpaces WHERE id = ?)"
	"   AND StatementRanges.addr_end >= Functions.addr_begin"
	"   AND StatementRanges.addr_begin < Functions.addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);	
    dm_database->bindArgument(2, dm_context);	
    while(dm_database->executeStatement())
	functions.push_back(Function(dm_database,
				     dm_database->getResultAsInteger(1),
				     dm_context));    
    END_TRANSACTION(dm_database);
    
    // Return the functions to the caller
    return functions;
}



/**
 * Get our path.
 *
 * Returns the full path name of this statement's source file.
 *
 * @return    Full path name of this statement's source file.
 */
Path Statement::getPath() const
{
    Optional<Path> path;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our source file's path
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->beginTransaction();
    dm_database->prepareStatement(
	"SELECT Files.path FROM Files JOIN Statements"
	" ON Files.id = Statements.file WHERE Statements.id = ?;"
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
 * Get our line number.
 *
 * Returns the line number of this statement.
 *
 * @return    Line number of this statement.
 */
int Statement::getLine() const
{
    int line;

    // Check assertions
    Assert(!dm_database.isNull());
	
    // Find our line number
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT line FROM Statements WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	line = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the line number to the caller
    return line;
}



/**
 * Get our column number.
 *
 * Returns the column number of this statement.
 *
 * @return    Column number of this statement.
 */
int Statement::getColumn() const
{
    int column;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our column number
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT column FROM Statements WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	column = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the column number to the caller
    return column;
}



/**
 * Get our address ranges.
 *
 * Returns the address ranges associated with this statement. An empty list is
 * returned if no addresses are associated with this statement (unlikely).
 *
 * @return    Addresse ranges associated with this statement.
 */
std::vector<AddressRange> Statement::getAddresseRanges() const
{
    std::vector<AddressRange> ranges;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our context's base address and our address ranges
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    validateContext();
    dm_database->prepareStatement(
	"SELECT AddressSpaces.addr_begin,"
	"       StatementRanges.addr_begin, StatementRanges.addr_end"
	" FROM AddressSpaces JOIN StatementRanges"
	" ON AddressSpaces.linked_object = StatementRanges.linked_object"
	" WHERE StatementRanges.statement = ?"
	"   AND AddressSpaces.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);	
    dm_database->bindArgument(2, dm_context);	
    while(dm_database->executeStatement())
	ranges.push_back(AddressRange(
			     dm_database->getResultAsAddress(1) +
			     dm_database->getResultAsAddress(2).getValue(),
			     dm_database->getResultAsAddress(1) +
			     dm_database->getResultAsAddress(3).getValue())); 
    END_TRANSACTION(dm_database);
    
    // Return the address ranges to the caller
    return ranges;
}



/**
 * Default constructor.
 *
 * Constructs a Statement that refers to a non-existent statement. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Statement::Statement() :
    dm_database(NULL),
    dm_entry(0),
    dm_context(0)
{
}



/**
 * Constructor from a statement entry.
 *
 * Constructs a new Statement for the specified statement entry within the
 * passed database, given the context of the specified address space entry.
 *
 * @param database    Database containing the statement.
 * @param entry       Entry (id) for the statement.
 * @param context     Address space entry (id) context for the statement.
 */
Statement::Statement(const SmartPtr<Database>& database,
		     const int& context, const int& entry) :
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
void Statement::validateEntry() const
{
    // Find the number of rows matching our entry
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Statements WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate
    if(rows == 0)
	throw Database::Corrupted(*dm_database,
				  "statement entry no longer exists");
    else if(rows > 1)
	throw Database::Corrupted(*dm_database, 
				  "statement entry is not unique");
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
void Statement::validateContext() const
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
