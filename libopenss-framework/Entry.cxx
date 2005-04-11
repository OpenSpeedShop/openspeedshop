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
 * Definition of the Entry class.
 *
 */

#include "Database.hxx"
#include "Entry.hxx"
#include "Exception.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Less-than operator.
 *
 * Operator "<" defined for two Entry objects. Allowing the placement of Entry
 * objects into STL associative containers is the primary reason for defining
 * this operator.
 *
 * @note    Comparing two Entry objects of differing types (both inheriting from
 *          Entry) is allowed, but nonsensical. It was not easy to prevent that
 *          from happening here without adding a data member containing the
 *          database table's name. And that would have increased the size of
 *          Entry objects considerably. So it was decided to simply allow the
 *          nonsensical comparison.
 *
 * @param other    Entry to compare against.
 * @return         Boolean "true" if we are less than the other entry, "false"
 *                 otherwise.
 */	
bool Entry::operator<(const Entry& other) const
{
    if(dm_database == other.dm_database)
	return dm_entry < other.dm_entry;
    return dm_database < other.dm_database;
}



/**
 * Default constructor.
 *
 * Constructs an Entry that refers to a non-existent database table entry. Any
 * use of a member function on an object constructed in this way will result in
 * an assertion failure.
 */
Entry::Entry() :
    dm_database(NULL),
    dm_entry(0),
    dm_context(-1)
{
}



/**
 * Constructor from an entry.
 *
 * Constructs a new Entry for the specified database table entry.
 *
 * @param database    Database containing this entry.
 * @param entry       Identifier for this entry.
 * @param context     Identifier of the context (address space entry) for this
 *                    entry.
 */
Entry::Entry(const SmartPtr<Database>& database,
	     const int& entry, const int& context) :
    dm_database(database),
    dm_entry(entry),
    dm_context(context)
{
}



/**
 * Destructor.
 *
 * Does nothing. Present only to insure proper destruction of our data members
 * in the presence of inheritance.
 */
Entry::~Entry()
{
}



/**
 * Validate this entry.
 *
 * Validates the existence and uniqueness of this entry and its context within
 * its database table. If the entry and its context are found and unique, this
 * function simply returns. Otherwise an exception of type Database::Corrupted
 * is thrown. If the entry has no context (the identifier of the context is
 * zero), then the context is not validated.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 *
 * @param table    Name of this entry's database table.
 */
void Entry::validate(const std::string& table) const
{
    // Find the number of rows matching our entry's identifier
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM " + table + " WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate the entry
    if(rows == 0)
	throw Exception(Exception::EntryNotFound,
			table, Exception::toString(dm_entry));
    else if(rows > 1)
	throw Exception(Exception::EntryNotUnique,
			table, Exception::toString(dm_entry));
    
    // Is the context identifier greater than zero?
    if(dm_context > 0) {

	// Find the number of rows matching our context's identifier
	int rows = 0;
	dm_database->prepareStatement(
	    "SELECT COUNT(*) FROM AddressSpaces WHERE id = ?;"
	    );
	dm_database->bindArgument(1, dm_context);
	while(dm_database->executeStatement())
	    rows = dm_database->getResultAsInteger(1);
	
	// Validate the context
	if(rows == 0)
	    throw Exception(Exception::EntryNotFound,
			    table, Exception::toString(dm_context));
	else if(rows > 1)
	    throw Exception(Exception::EntryNotUnique,
			    table, Exception::toString(dm_context));
    }    
}
