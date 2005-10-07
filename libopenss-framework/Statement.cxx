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

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "ExtentGroup.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our threads.
 *
 * Returns the threads containing this statement.
 *
 * @return    Threads containing this statement.
 */
std::set<Thread> Statement::getThreads() const
{
    std::set<Thread> threads;

    // Find the threads containing our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT DISTINCT AddressSpaces.thread "
	"FROM Statements "
	"  JOIN AddressSpaces "
	"ON Statements.linked_object = AddressSpaces.linked_object "
	"WHERE Statements.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    if(threads.empty())
	throw Exception(Exception::EntryNotFound, "Threads",
			"<Statements-Referenced>");
    END_TRANSACTION(dm_database);    
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our extent in a thread.
 *
 * Returns the extent of this statement within the specified thread. An
 * empty extent is returned if this statement isn't present within the
 * specified thread.
 *
 * @pre    The thread must be in the same experiment as the statement. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the statement.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this statement in that thread.
 */
ExtentGroup Statement::getExtentIn(const Thread& thread) const
{
    ExtentGroup extent;

    // Check assertions
    Assert(inSameDatabase(thread));

    // Find our extent in the specified thread
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
	"SELECT AddressSpaces.time_begin, "
	"       AddressSpaces.time_end, "
	"       AddressSpaces.addr_begin, "
        "       StatementRanges.addr_begin, "
        "       StatementRanges.addr_end, "
        "       StatementRanges.valid_bitmap "
	"FROM StatementRanges "
	"  JOIN Statements "
	"  JOIN AddressSpaces "
	"ON StatementRanges.statement = Statements.id "
	"  AND Statements.linked_object = AddressSpaces.linked_object "
        "WHERE StatementRanges.statement = ? "
        "  AND AddressSpaces.thread = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, EntrySpy(thread).getEntry());
    while(dm_database->executeStatement()) {

        AddressBitmap bitmap(AddressRange(dm_database->getResultAsAddress(4),
                                          dm_database->getResultAsAddress(5)),
                             dm_database->getResultAsBlob(6));

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
		extent.push_back(
		    Extent(TimeInterval(dm_database->getResultAsTime(1),
					dm_database->getResultAsTime(2)),
			   AddressRange(dm_database->getResultAsAddress(3) +
					range_begin,
					dm_database->getResultAsAddress(3) +
					i))
		    );
            }
	
        if(in_range)
	    extent.push_back(
		Extent(TimeInterval(dm_database->getResultAsTime(1),
				    dm_database->getResultAsTime(2)),
		       AddressRange(dm_database->getResultAsAddress(3) +
				    range_begin,
				    dm_database->getResultAsAddress(3) +
				    bitmap.getRange().getEnd()))
		);
	
    }
    END_TRANSACTION(dm_database);
        
    // Return the extent to the caller    
    return extent;
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

    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM Statements WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);	
    while(dm_database->executeStatement())
	linked_object = LinkedObject(dm_database, 
				     dm_database->getResultAsInteger(1));
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
    
    // Find the functions intersecting our address ranges
    BEGIN_TRANSACTION(dm_database);
    validate();
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
	"WHERE Statements.id = ? "
	"  AND StatementRanges.addr_end >= Functions.addr_begin "
	"  AND StatementRanges.addr_begin < Functions.addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
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
					  dm_database->getResultAsInteger(1)));
	
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

    // Find our source file's path
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT Files.path "
	"FROM Statements "
	"  JOIN Files "
	"ON Statements.file = Files.id "
	"WHERE Statements.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	if(!path.empty())
	    throw Exception(Exception::EntryNotUnique, "Files",
			    "<Statements-Referenced>");
	path = Path(dm_database->getResultAsString(1));
    }    
    if(path.empty())
	throw Exception(Exception::EntryNotFound, "Files",
			"<Statements-Referenced>");
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

    // Find our line number
    BEGIN_TRANSACTION(dm_database);
    validate();
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

    // Find our column number
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT \"column\" FROM Statements WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	column = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the column number to the caller
    return column;
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
 */
Statement::Statement(const SmartPtr<Database>& database, const int& entry):
    Entry(database, Entry::Statements, entry)
{
}
