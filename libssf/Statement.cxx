////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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

#include "Address.hxx"
#include "AddressSpace.hxx"
#include "Assert.hxx"
#include "Function.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "StatementEntry.hxx"
#include "SymbolTable.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our path name.
 *
 * Returns to the caller the full path name of this statement's source file.
 *
 * @return    Full path name of this statement's source file.
 */
Path Statement::getPath() const
{
    // Check assertions
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
    
    // Return the full path name to the caller
    return Path(dm_symbol_table->getStringTable().getString(dm_entry->path));
}



/**
 * Get our line number.
 *
 * Returns to the caller the line number of this statement.
 *
 * @return    Line number of this statement.
 */
unsigned Statement::getLine() const
{
    // Check assertions
    Assert(dm_entry != NULL);

    // Return the line number to the caller
    return dm_entry->line;
}



/**
 * Get our column number.
 *
 * Returns to the caller the column number of this statement.
 *
 * @return    Column number of this statement.
 */
unsigned Statement::getColumn() const
{
    // Check assertions
    Assert(dm_entry != NULL);

    // Return the column number to the caller
    return dm_entry->column;
}



/**
 * Find all addresses.
 *
 * Finds and returns all the addresses associated with this statement. If no
 * addresses are associated with this statement (unlikely), an empty list is
 * returned.
 *
 * @return    All addresses associated with this statement.
 */
std::vector<Address> Statement::findAllAddresses() const
{
    // TODO: implement
    return std::vector<Address>();
}



/**
 * Find functions.
 *
 * Finds and returns the functions containing this statement. If no function
 * contains this statement (unlikely), an empty list is returned.
 *
 * @return    All functions containing this statement.
 */
std::vector<Function> Statement::findFunctions() const
{
    // TODO: implement
    return std::vector<Function>();
}



/**
 * Default constructor.
 *
 * Constructs a Statement that refers to a non-existent statement entry. Any
 * use of a member function on an object constructed in this way will result in
 * an assertion failure. The only reason this default constructor exists is to
 * allow Optional<Statement> to create an empty optional value.
 */
Statement::Statement() :
    dm_address_space(NULL),
    dm_symbol_table(NULL),
    dm_entry(NULL)
{
}



/**
 * Constructor from an address space, symbol table, and statement entry.
 *
 * Constructs a new Statement for the specified statement entry, contained
 * within the passed symbol table and address space.
 *
 * @param address_space    Address space containing the statement.
 * @param symbol_table     Symbol table containing the statement.
 * @param entry            The statement's entry.
 */
Statement::Statement(const AddressSpace* address_space,
		     const SymbolTable* symbol_table, 
		     const StatementEntry* entry) :
    dm_address_space(address_space),
    dm_symbol_table(symbol_table),
    dm_entry(entry)
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
}
