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
 * Definition of the Function class.
 *
 */

#include "AddressSpace.hxx"
#include "Assert.hxx"
#include "CallSite.hxx"
#include "Function.hxx"
#include "FunctionEntry.hxx"
#include "LinkedObject.hxx"
#include "Statement.hxx"
#include "SymbolTable.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our demangled name.
 *
 * Returns to the caller the demangled name of this function.
 *
 * @return    Demangled name of this function.
 */
std::string Function::getName() const
{
    // Check assertions
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
    
    // Return the demangled name to the caller
    return dm_symbol_table->getStringTable().getString(dm_entry->name);
}



/**
 * Find address range.
 *
 * Finds and returns the address range of this function.
 *
 * @return    Address range of this function.
 */
AddressRange Function::findAddressRange() const
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);

    // Find the base address of our symbol table within our address space
    Address base_address = 
	dm_address_space->findBaseAddressBySymbolTable(dm_symbol_table);

    // Return the address range to the caller
    return AddressRange(base_address + dm_entry->range.getBegin().getValue(),
			base_address + dm_entry->range.getEnd().getValue());
}



/**
 * Find linked object.
 *
 * Finds and returns the linked object containing this function.
 *
 * @return    Linked object containing this function.
 */
LinkedObject Function::findLinkedObject() const
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);

    // Create and return the linked object to the caller
    return LinkedObject(dm_address_space, dm_symbol_table,
			dm_symbol_table->getLinkedObjectEntry());
}



/**
 * Find all statements.
 *
 * Finds and returns all statements associated with this function. If no source
 * statement information is available, the Optional returned will not have a
 * value. If no statements are associated with this function (unlikely), an
 * empty list is returned.
 *
 * @return    Optional statements associated with this function.
 */
Optional< std::vector<Statement> > Function::findAllStatements() const
{
    // TODO: implement
    return Optional< std::vector<Statement> >();
}



/**
 * Find definition.
 *
 * Finds and returns the definition of this function. If no source statement
 * information is available, the Optional returned will not have a value.
 *
 * @return    Optional definition of this function.
 */
Optional<Statement> Function::findDefinition() const
{
    // Find the statement in our symbol table at the function's first address
    const StatementEntry* entry =
	dm_symbol_table->findStatementAt(dm_entry->range.getBegin());
    
    // Return an empty optional if no statement was found
    if(entry == NULL)
	return Optional<Statement>();
    
    // Create and return the statement to the caller
    return Optional<Statement>(Statement(dm_address_space, dm_symbol_table,
					 entry));
}



/**
 * Find callees.
 *
 * Finds and returns the known callees of this function. An empty list is
 * returned if no callees are currently known.
 *
 * @return    All known callees of this function.
 */
std::vector<CallSite> Function::findCallees() const
{
    // TODO: implement
    return std::vector<CallSite>();
}



/**
 * Find callers.
 *
 * Finds and returns the known callers of this function. An empty list is
 * returned if no callers are currently known.
 *
 * @return    All known callers of this function.
 */
std::vector<CallSite> Function::findCallers() const
{
    // TODO: implement
    return std::vector<CallSite>();
}



/**
 * Default constructor.
 *
 * Constructs a Function that refers to a non-existent function entry. Any use
 * of a member function on an object constructed in this way will result in an
 * assertion failure. The only reason this default constructor exists is to
 * allow Optional<Function> to create an empty optional value.
 */
Function::Function() :
    dm_address_space(NULL),
    dm_symbol_table(NULL),
    dm_entry(NULL)
{
}



/**
 * Constructor from an address space, symbol table, and function entry.
 *
 * Constructs a new Function for the specified function entry, contained within
 * the passed symbol table and address space.
 *
 * @param address_space    Address space containing the function.
 * @param symbol_table     Symbol table containing the function.
 * @param entry            The function's entry.
 */
Function::Function(const AddressSpace* address_space,
		   const SymbolTable* symbol_table,
		   const FunctionEntry* entry) :
    dm_address_space(address_space),
    dm_symbol_table(symbol_table),
    dm_entry(entry)
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
}
