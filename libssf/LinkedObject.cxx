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
 * Definition of the LinkedObject class.
 *
 */

#include "AddressSpace.hxx"
#include "Assert.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "LinkedObjectEntry.hxx"
#include "Path.hxx"
#include "SymbolTable.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our path name.
 *
 * Returns to the caller the full path name of this linked object.
 *
 * @return    Full path name of this linked object.
 */
Path LinkedObject::getPath() const
{
    // Check assertions
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
     
    // Return the full path name to the caller
    return Path(dm_symbol_table->getStringTable().getString(dm_entry->path));
}



/**
 * Find address range.
 *
 * Finds and returns the address range of this linked object.
 *
 * @return    Address range of this linked object.
 */
AddressRange LinkedObject::findAddressRange() const
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
 * Find all functions.
 *
 * Finds and returns all the functions contained within this linked object. An
 * empty list is returned if no functions are found within this linked object
 * (unlikely).
 *
 * @note    This is a relatively high cost operation in terms of time and memory
 *          used. Avoid using this member function if at all possible.
 *
 * @return    All functions in this linked object.
 */
std::vector<Function> LinkedObject::findAllFunctions() const
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    
    // Find all functions in our symbol table
    std::vector<const FunctionEntry*> functions =
	dm_symbol_table->findAllFunctions();
    
    // Create the functions and return them to the caller
    std::vector<Function> result;
    for(std::vector<const FunctionEntry*>::const_iterator
	    i = functions.begin(); i != functions.end(); ++i)
	result.push_back(Function(dm_address_space, dm_symbol_table, *i));
    return result;
}



/**
 * Default constructor.
 *
 * Constructs a LinkedObject that refers to a non-existent linked object entry.
 * Any use of a member function on an object constructed in this way will result
 * in an assertion failure. The only reason this default constructor exists is
 * to allow Optional<LinkedObject> to create an empty optional value.
 */
LinkedObject::LinkedObject() :
    dm_address_space(NULL),
    dm_symbol_table(NULL),
    dm_entry(NULL)
{
}



/**
 * Constructor from an address space, symbol table, and linked object entry.
 *
 * Constructs a new LinkedObject for the specified linked object entry,
 * represented by the passed symbol table and contained within the passed
 * address space.
 *
 * @param address_space    Address space containing the linked object.
 * @param symbol_table     Symbol table representing the linked object.
 * @param entry            The linked object's entry.
 */
LinkedObject::LinkedObject(const AddressSpace* address_space,
			   const SymbolTable* symbol_table, 
			   const LinkedObjectEntry* entry) :
    dm_address_space(address_space),
    dm_symbol_table(symbol_table),
    dm_entry(entry)
{
    // Check assertions
    Assert(dm_address_space != NULL);
    Assert(dm_symbol_table != NULL);
    Assert(dm_entry != NULL);
}
