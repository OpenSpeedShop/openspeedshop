////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
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

#include "Assert.hxx"
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
 * @pre    The two entries must be in the same database table type. An assertion
 *         failure occurs if the two entries are from different database table
 *         types.
 *
 * @param other    Entry to compare against.
 * @return         Boolean "true" if we are less than the other entry, "false"
 *                 otherwise.
 */	
#if 0
bool Entry::operator<(const Entry& other) const
{
    // Check preconditions
//    Assert(dm_table == other.dm_table);
    
    // Compare the two entries
    if(dm_database == other.dm_database)
	return dm_entry < other.dm_entry;
    return dm_database < other.dm_database;
}
#endif



/**
 * Test if in same database.
 *
 * Compare this entry's database aginst the database of the passed entry.
 * Returns a boolean value indicating if the entries are in the same database
 * or not.
 *
 * @param entry    Entry to compare against.
 * @return         Boolean "true" if this entry is in the same database as the
 *                 passed entry, "false" otherwise.
 */
bool Entry::inSameDatabase(const Entry& entry) const
{
    return dm_database == entry.dm_database;
}



/**
 * Lock our database.
 *
 * Locks this entry's database before beginning a prolonged operation on that
 * database. Using this function to lock the database is not strictly necessary
 * as each individual query on the database will perform locking when required.
 * Performing explicit locking can, however, improve performance dramatically
 * for such queries that would otherwise require thousands of locking operations
 * instead of just one.
 */
void Entry::lockDatabase() const
{
    dm_database->beginTransaction();
}



/**
 * Unlock our database.
 *
 * Unlocks this entry's database after completing a prolonged operation on that
 * database. Must be called once for each previous call to lockDatabase() in
 * order to properly release the database once the long operations has been
 * completed.
 */
void Entry::unlockDatabase() const
{
    dm_database->commitTransaction();
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
    dm_table(),
    dm_entry()
{
}



/**
 * Constructor from an entry.
 *
 * Constructs a new Entry for the specified database table entry.
 *
 * @param database    Database containing this entry.
 * @param table       Database table containing this entry.
 * @param entry       Identifier for this entry.
 */
Entry::Entry(const SmartPtr<Database>& database,
	     const Table& table, const int& entry) :
    dm_database(database),
    dm_table(table),
    dm_entry(entry)
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
 * Get our database table.
 *
 * Returns the name of our database table. Simply returns the string containing
 * the textual representation of the entry's database table enumeration value
 * (e.g. "Collectors" for Table::Collectors).
 *
 * @return    Name of our database table.
 */ 
std::string Entry::getTable() const
{
    switch(dm_table) {
    case Collectors:
        return "Collectors";
    case Functions:
        return "Functions";
    case LinkedObjects:
        return "LinkedObjects";
    case Loops:
        return "Loops";
    case Statements:
        return "Statements";
    case Threads:
        return "Threads";
    default:
        return "";
    }
}



/**
 * Validate this entry.
 *
 * Validates the existence and uniqueness of this entry within its database
 * table. If the entry is found and unique, this function simply returns.
 * Otherwise an EntryNotFound or EntryNotUnique exception is thrown.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 */
void Entry::validate() const
{
#ifdef WDH_DO_PROPER_VALIDATION

    // Note: The following code to do validation was found to be eatting up a
    //       pretty significant amount of time. It serves primarily as a sanity
    //       check for when multiple tools are accessing the same database, but
    //       that never happens right now. So for now it is just #ifdef'ed out.

    // Find the number of rows matching our entry's identifier
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM " + getTable() + " WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate the entry
    if(rows == 0)
	throw Exception(Exception::EntryNotFound,
			getTable(), Exception::toString(dm_entry));
    else if(rows > 1)
	throw Exception(Exception::EntryNotUnique,
			getTable(), Exception::toString(dm_entry));
#endif
}
