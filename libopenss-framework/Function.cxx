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
 * Definition of the Function class.
 *
 */

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "CallSite.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our thread.
 *
 * Returns the thread containing this function.
 *
 * @return    Thread containing this function.
 */
Thread Function::getThread() const
{
    Thread thread;

    // Find our context's thread    
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
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
 * Returns the linked object containing this function.
 *
 * @return    Linked object containing this function.
 */
LinkedObject Function::getLinkedObject() const
{
    LinkedObject linked_object;
    
    // Find our context's linked object
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
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
 * Get our demangled name.
 *
 * Returns the demangled name of this function.
 *
 * @return    Demangled name of this function.
 */
std::string Function::getName() const
{
    std::string name;

    // Find our demangled name
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
    dm_database->prepareStatement(
	"SELECT name FROM Functions WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	name = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the demangled name to the caller
    return name;
}



/**
 * Get our address range.
 *
 * Returns the address range of this function.
 *
 * @return    Address range of this function.
 */
AddressRange Function::getAddressRange() const
{
    AddressRange range;

    // Find our context's base address and our address range
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
    dm_database->prepareStatement(
	"SELECT AddressSpaces.addr_begin, "
	"       Functions.addr_begin, "
	"       Functions.addr_end "
	"FROM AddressSpaces "
	"  JOIN Functions "
	"ON AddressSpaces.linked_object = Functions.linked_object "
	"WHERE Functions.id = ? "
	"  AND AddressSpaces.id = ?;"
	);	
    dm_database->bindArgument(1, dm_entry);	
    dm_database->bindArgument(2, dm_context);	
    while(dm_database->executeStatement())
	range = AddressRange(dm_database->getResultAsAddress(1) +
			     dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(1) +
			     dm_database->getResultAsAddress(3));
    END_TRANSACTION(dm_database);
    
    // Return the address range to the caller	
    return range;
}



/**
 * Get our definitions.
 *
 * Returns the definitions of this function. An empty set is returned if no
 * definitions of this function are found.
 *
 * @return    Definitions of this function.
 */
std::set<Statement> Function::getDefinitions() const
{
    std::set<Statement> definitions;

    // Find the statements containing our beginning address
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
    dm_database->prepareStatement(
	"SELECT Functions.addr_begin, "
	"       Statements.id, "
	"       StatementRanges.addr_begin, "
	"       StatementRanges.addr_end, "
	"       StatementRanges.valid_bitmap "
	"FROM Functions "
	"  JOIN Statements "
	"  JOIN StatementRanges "
	"ON Functions.linked_object = Statements.linked_object "
	"  AND Statements.id = StatementRanges.statement "
	"WHERE Functions.id = ? "
	"  AND Functions.addr_begin >= StatementRanges.addr_begin "
	"  AND Functions.addr_begin < StatementRanges.addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	
	AddressBitmap bitmap(AddressRange(dm_database->getResultAsAddress(3),
	 				  dm_database->getResultAsAddress(4)),
			     dm_database->getResultAsBlob(5));

	if(bitmap.getValue(dm_database->getResultAsAddress(1)))
	    definitions.insert(Statement(dm_database,
					 dm_database->getResultAsInteger(2),
					 dm_context));
	
    }
    END_TRANSACTION(dm_database);
    
    // Return the definitions to the caller
    return definitions;
}



/**
 * Get our statements.
 *
 * Returns the statements associated with this function. An empty set is
 * returned if no statements are associated with this function.
 *
 * @return    Statements associated with this function.
 */
std::set<Statement> Function::getStatements() const
{
    std::set<Statement> statements;

    // Find the statements intersecting our address range
    BEGIN_TRANSACTION(dm_database);
    validate("Functions");
    dm_database->prepareStatement(
	"SELECT Functions.addr_begin, "
	"       Functions.addr_end, "
	"       Statements.id, "
	"       StatementRanges.addr_begin, "
	"       StatementRanges.addr_end, "
	"       StatementRanges.valid_bitmap "
	"FROM Functions "
	"  JOIN Statements "
	"  JOIN StatementRanges "
	"ON Functions.linked_object = Statements.linked_object "
	"  AND Statements.id = StatementRanges.statement "
	"WHERE Functions.id = ? "
	"  AND Functions.addr_end >= StatementRanges.addr_begin"
	"  AND Functions.addr_begin < StatementRanges.addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {

	AddressBitmap bitmap(AddressRange(dm_database->getResultAsAddress(4),
	 				  dm_database->getResultAsAddress(5)),
			     dm_database->getResultAsBlob(6));
	
	AddressRange range =
	    AddressRange(dm_database->getResultAsAddress(1),
			 dm_database->getResultAsAddress(2)) &
	    AddressRange(dm_database->getResultAsAddress(4),
			 dm_database->getResultAsAddress(5));

	for(Address i = range.getBegin(); i != range.getEnd(); ++i)
	    if(bitmap.getValue(i))
		statements.insert(Statement(dm_database,
					    dm_database->getResultAsInteger(3),
					    dm_context));
	
    }
    END_TRANSACTION(dm_database);
    
    // Return the statements to the caller
    return statements;
}



/**
 * Get our callees.
 *
 * Returns the known callees of this function. An empty set is returned if no
 * known callees are found.
 *
 * @todo    Needs to be implemented.
 *
 * @return    All known callees of this function.
 */
std::set<CallSite> Function::getCallees() const
{
    return std::set<CallSite>();
}



/**
 * Get our callers.
 *
 * Returns the known callers of this function. An empty set is returned if no
 * known callers are found.
 *
 * @todo    Needs to be implemented.
 * 
 * @return    All known callers of this function.
 */
std::set<CallSite> Function::getCallers() const
{
    return std::set<CallSite>();
}



/**
 * Default constructor.
 *
 * Constructs a Function that refers to a non-existent function. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Function::Function() :
    Entry()
{
}



/**
 * Constructor from a function entry.
 *
 * Constructs a new Function for the specified function entry.
 *
 * @param database    Database containing this function.
 * @param entry       Identifier for this function.
 * @param context     Identifier of the context for this function.
 */
Function::Function(const SmartPtr<Database>& database,
		   const int& entry, const int& context) :
    Entry(database, entry, context)
{
}
