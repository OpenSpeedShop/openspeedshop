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
#include "Address.hxx"
#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
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
    validate("Statements");
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
    validate("Statements");
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
 * Returns the functions containing this statement. An empty set is returned
 * if no function contains this statement.
 *
 * @return    Functions containing this statement.
 */
std::set<Function> Statement::getFunctions() const
{
    std::set<Function> functions;
    
    // Check assertions
    Assert(!dm_database.isNull());
  
    // Find the functions intersecting our address ranges
    BEGIN_TRANSACTION(dm_database);
    validate("Statements");
    dm_database->prepareStatement(
	"SELECT Functions.id, "
	"       Functions.addr_begin, "
	"       Functions.addr_end, "
	"       StatementRanges.addr_begin, "
	"       StatementRanges.addr_end, "
	"       StatementRanges.valid_bitmap "
	"FROM Functions "
	"  JOIN Statements "
	"  JOIN StatementRanges "
	"ON Functions.linked_object = Statements.linked_object "
	"  AND Statements.id = StatementRanges.statement "
	"WHERE StatementRanges.statement = ? "
	"  AND Statements.linked_object = ? "
	"  AND StatementRanges.addr_end >= Functions.addr_begin "
	"  AND StatementRanges.addr_begin < Functions.addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);	
    dm_database->bindArgument(2, dm_context);	
    while(dm_database->executeStatement()) {

	AddressBitmap bitmap(AddressRange(dm_database->getResultAsAddress(4),
	 				  dm_database->getResultAsAddress(5)),
			     dm_database->getResultAsBlob(6));
	
	AddressRange range =
	    AddressRange(dm_database->getResultAsAddress(2),
			 dm_database->getResultAsAddress(3)) &
	    AddressRange(dm_database->getResultAsAddress(4),
			 dm_database->getResultAsAddress(5));
	
	for(Address i = range.getBegin(); i != range.getEnd(); ++i)
	    if(bitmap.getValue(i))
		functions.insert(Function(dm_database,
					  dm_database->getResultAsInteger(1),
					  dm_context));
	
    }
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
    Path path;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our source file's path
    BEGIN_TRANSACTION(dm_database);
    validate("Statements");
    dm_database->prepareStatement(
	"SELECT Files.path "
	"FROM Files "
	"  JOIN Statements "
	"ON Files.id = Statements.file "
	"WHERE Statements.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	if(!path.empty())
	    throw Database::Corrupted(*dm_database, "file entry is not unique");
	path = Path(dm_database->getResultAsString(1));
    }    
    if(path.empty())
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
    validate("Statements");
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
    validate("Statements");
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
 * Returns the address ranges associated with this statement. An empty set is
 * returned if no address ranges are associated with this statement.
 *
 * @return    Address ranges associated with this statement.
 */
std::set<AddressRange> Statement::getAddressRanges() const
{
    std::set<AddressRange> ranges;
    
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Find our addresses
    BEGIN_TRANSACTION(dm_database);
    validate("Statements");
    dm_database->prepareStatement(
	"SELECT AddressSpaces.addr_begin, "
	"       StatementRanges.addr_begin, "
	"       StatementRanges.addr_end, "
	"       StatementRanges.valid_bitmap "
	"FROM AddressSpaces "
	"  JOIN Statements "
	"  JOIN StatementRanges "
	"ON AddressSpaces.linked_object = Statements.linked_object "
	"  AND Statements.id = StatementRanges.statement "
	"WHERE StatementRanges.statement = ? "
	"  AND AddressSpaces.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, dm_context);
    while(dm_database->executeStatement()) {

	AddressBitmap bitmap(AddressRange(dm_database->getResultAsAddress(2),
					  dm_database->getResultAsAddress(3)),
			     dm_database->getResultAsBlob(4));

	bool in_range = false;
	Address range_begin;

	for(Address i = bitmap.getRange().getBegin();
	    i != bitmap.getRange().getEnd();
	    ++i)
	    if(!in_range && bitmap.getValue(i)) {
		in_range = true;
		range_begin = i;
	    }
	    else if(in_range && !bitmap.getValue(i)) {
		in_range = false;
		ranges.insert(
		    AddressRange(dm_database->getResultAsAddress(1) +
				 range_begin,
				 dm_database->getResultAsAddress(1) +
				 i)
		    );
	    }
	if(in_range)
	    ranges.insert(
		AddressRange(dm_database->getResultAsAddress(1) +
			     range_begin,
			     dm_database->getResultAsAddress(1) +
			     bitmap.getRange().getEnd())
		);
	
    }
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
    Entry()
{
}



/**
 * Constructor from a statement entry.
 *
 * Constructs a new Statement for the specified statement entry.
 *
 * @param database    Database containing this statement.
 * @param entry       Identifier for this statement.
 * @param context     Identifier of the context for this statement.
 */
Statement::Statement(const SmartPtr<Database>& database,
		     const int& entry, const int& context) :
    Entry(database, entry, context)
{
}
