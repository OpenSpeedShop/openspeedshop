///////////////////////////////////////////////////////////////////////////////
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
 * Definition of the SymbolTable class.
 *
 */

#include "Application.hxx"
#include "Path.hxx"
#include "StringTable.hxx"
#include "SymbolTable.hxx"

#include <dpcl.h>

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from an executable or shared library.
 *
 * Creates a new symbol table for the specified executable or shared library.
 * The full path name, run-time address range, and source object list for the
 * executable or shared library must be specified. DPCL is queried in order to
 * construct internal maps that describe the functions, source statements, and
 * call sites that were found at particular address ranges.
 *
 * @param application    Application that will contain the symbol table.
 * @param path           Full path name.
 * @param range          Address range occupied.
 * @param objects        Source object list.
 */
SymbolTable::SymbolTable(Application* application,
			 const Path& path,
			 const AddressRange& range,
			 const std::vector<SourceObj>& objects) :
    dm_application(application),
    dm_linked_object(),
    dm_functions(),
    dm_statements()
{
    
    // TODO: Do we need locking of the application's string table here???

    // Check assertions
    Assert(dm_application != NULL);
    
    // Fill in the linked object entry for this symbol table
    dm_linked_object.range = AddressRange(0, range.getEnd() - range.getBegin());
    dm_linked_object.path = dm_application->dm_string_table.addString(path);
    
    // Iterate over each (module) source object
    for(std::vector<SourceObj>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i)
	
	// Iterate over each child of this module
	for(unsigned j = 0; j < i->child_count(); ++j) {
	    SourceObj child = i->child(j);
	    
	    // Is this child a function object?
	    if(child.src_type() == SOT_function) {
		
		// Get the demangled name of the function
		char name[child.get_demangled_name_length() + 1];
		child.get_demangled_name(name, sizeof(name));
		
		// Create a new function entry for this function
		FunctionEntry function_entry;
		function_entry.range =
		    AddressRange(Address(child.address_start()) -
				 range.getBegin(),
				 Address(child.address_end()) -
				 range.getBegin());
		function_entry.name = dm_application->
		    dm_string_table.addString(name);
		
		// Add this function entry to the function map
		dm_functions.insert(
		    std::make_pair(function_entry.range.getBegin(),
				   function_entry));
		
	    }
	}
}



/**
 * Get our path name.
 *
 * Returns to the caller the full path name of this symbol table's linked
 * object.
 *
 * @return    Full path name of this symbol table's linked object.
 */
Path SymbolTable::getPath() const
{
    // Return the full path name to the caller
    return Path(getStringTable().getString(dm_linked_object.path));
}



/**
 * Get our string table.
 *
 * Returns to the caller the string table used by this symbol table.
 *
 * @return    String table used by this symbol table.
 */
const StringTable& SymbolTable::getStringTable() const
{
    // Check assertions
    Assert(dm_application != NULL);
    
    // Return the string table reference to the caller
    return dm_application->dm_string_table;
}



/**
 * Find all functions.
 *
 * Finds all functions within this symbol table and return a list of the
 * corresponding FunctionEntry objects. An empty list is returned if no
 * functions are found.
 *
 * @return    List of FunctionEntry objects for all functions.
 */
std::vector<const FunctionEntry*> SymbolTable::findAllFunctions() const
{
    // Find all the function entries
    std::vector<const FunctionEntry*> result;
    for(std::map<AddressRange, FunctionEntry>::const_iterator
	    i = dm_functions.begin(); i != dm_functions.end(); ++i)
	result.push_back(&(i->second));
    
    // Return the function entries to the caller
    return result;    
}



/**
 * Find the function at an address.
 *
 * Find the function containing the instruction at the passed query address and
 * return the corresponding FunctionEntry object. The pointer returned will have
 * a null value if no function can be found that contains this instruction.
 *
 * @param address    Query address.
 * @return           FunctionEntry for the function at the query address or a
 *                   null value if no function was found.
 */
const FunctionEntry* SymbolTable::findFunctionAt(const Address& address) const
{
    // Find the function entry at the query address
    std::map<AddressRange, FunctionEntry>::const_iterator
	i = dm_functions.find(address);
    
    // Return the function entry to the caller
    return (i != dm_functions.end()) ? &(i->second) : NULL;
}



/**
 * Find the statement at an address.
 *
 * Find the statement containing the instruction at the passed query address
 * and return the corresponding StatementEntry object. The pointer returned
 * will have a null value if no statement can be found that contains this
 * instruction.
 *
 * @param address    Query address.
 * @return           StatementEntry for the statement at the query address or a
 *                   null value if no function was found.
 */
const StatementEntry* SymbolTable::findStatementAt(const Address& address) const
{
    // Find the statement entry at the query address
    std::map<AddressRange, StatementEntry>::const_iterator
	i = dm_statements.find(address);
    
    // Return the statement entry to the caller
    return (i != dm_statements.end()) ? &(i->second) : NULL;
}
