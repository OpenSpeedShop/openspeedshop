////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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

#include "Backend.hxx"
#include "Blob.hxx"
#include "SymbolTable.hxx"

#include <algorithm>
#include <BPatch_function.h>
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
#include <BPatch_statement.h>
#endif
#include <deque>

#if (DYNINST_MAJOR == 8) 
#include <BPatch_flowGraph.h>
#endif

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from linked object file name and address range.
 *
 * Constructs an empty symbol table. Sets the linked object file name and
 * address range for this symbol table to the specified values.
 */
SymbolTable::SymbolTable(const FileName& linked_object,
			 const AddressRange& range) :
    dm_linked_object(linked_object),
    dm_range(range),
    dm_functions(),
    dm_statements()
{
}



/**
 * Add a module.
 *
 * Add the symbols of the specified module to this symbol table.
 *
 * @param image     Image containing the module to be added.
 * @param module    Module to be added.
 */
void SymbolTable::addModule(/* const */ BPatch_image& image,
			    /* const */ BPatch_module& module)
{
    // Get the address range of this module
    Address module_begin(reinterpret_cast<uintptr_t>(module.getBaseAddr()));
    Address module_end = module_begin + module.getSize();
    AddressRange module_range(module_begin, module_end);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	char buffer[PATH_MAX];
	module.getFullName(buffer, sizeof(buffer));
        std::stringstream output;
        output << "[TID " << pthread_self() << "] Callbacks::"
	       << "addModule(" << buffer << ") @ " << module_range << std::endl;
        std::cerr << output.str();
    }
#endif

    // Get the list of functions in this module
    BPatch_Vector<BPatch_function*>* functions = module.getProcedures(true);
    Assert(functions != NULL);
    
    // Iterate over each function in this module
    for(int i = 0; i < functions->size(); ++i) {
	Assert((*functions)[i] != NULL);

	// Get the mangled name of this function
	char name_buffer[16 * 1024] = "";
	(*functions)[i]->getMangledName(name_buffer, sizeof(name_buffer));
	std::string name = name_buffer;
	
	// Add this function to the table (or find the existing entry)
	FunctionTable::iterator j =
	    dm_functions.insert(
	        std::make_pair(name, std::vector<AddressRange>())
		).first;

	// Get the list of basic blocks in this function
	BPatch_Set<BPatch_basicBlock*> basic_blocks;
	BPatch_flowGraph* cfg = (*functions)[i]->getCFG();
	if(cfg == NULL) {
#ifndef NDEBUG
	    if(Backend::isSymbolsDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "addModule(): Function "
		       << (name.empty() ? "<unknown>" : name)
		       << ": no control flow graph."
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
	    
	    continue;
	}
	cfg->getAllBasicBlocks(basic_blocks);
	
	// Iterate over each basic block in this function
	for(BPatch_Set<BPatch_basicBlock*>::iterator
		k = basic_blocks.begin(); k != basic_blocks.end(); k++) {

	    // Get the begin/end address of the basic block
	    Address begin((*k)->getStartAddress());
	    Address end((*k)->getEndAddress());

	    // Sanity checks
	    if(end <= begin) {
		
#ifndef NDEBUG
		if(Backend::isSymbolsDebugEnabled()) {
		    std::stringstream output;
		    output << "[TID " << pthread_self() << "] Callbacks::"
			   << "addModule(): Function "
			   << (name.empty() ? "<unknown>" : name)
			   << ": begin (" << Address(begin) 
			   << ") >= end (" << Address(end) << ")."
			   << std::endl;
		    std::cerr << output.str();
		}
#endif
    
		continue;
	    }
	    if((begin < module_begin) || (begin >= module_end) ||
	       (end <= module_begin) || (end > module_end)) {
#ifndef NDEBUG
		if(Backend::isSymbolsDebugEnabled()) {
		    std::stringstream output;
		    output << "[TID " << pthread_self() << "] Callbacks::"
			   << "addModule(): Function "
			   << (name.empty() ? "<unknown>" : name)
			   << ": begin=" << Address(begin) 
			   << ", end=" << Address(end)
			   << ": is outside the module." << std::endl;
		    std::cerr << output.str();
		}
#endif
		
		continue;
	    }

	    // Form address range of the function relative to module beginning
	    AddressRange range(begin - module_begin, end - module_begin);
	    
	    // Is this address range shared by more than one function?
	    BPatch_Vector<BPatch_function*> all_functions;
#ifdef WDH_TEMPORARY_HACK
	    //
	    // TODO: Temporarily disable the shared code region detection
	    //       due to a bug in Dyninst.
	    //
	    image.findFunction(
		static_cast<Dyninst::Address>(begin.getValue()),
		all_functions
		);
#endif  // WDH TEMPORARY HACK
	    if(all_functions.size() > 1) {

#ifndef NDEBUG
		if(Backend::isSymbolsDebugEnabled()) {
		    std::stringstream output;
		    output << "[TID " << pthread_self() << "] Callbacks::"
			   << "addModule(): Function "
			   << (name.empty() ? "<unknown>" : name)
			   << ": begin=" << Address(begin)
			   << ", end=" << Address(end)
			   << ": is a shared code region." << std::endl;
		    std::cerr << output.str();
		}
#endif

		// Name the pseudo-function for this shared address range
		std::ostringstream pseudo_name;
		pseudo_name << "Shared Code Region @ +" << range.getBegin();

		// Add this pseudo-function to the table (or find existing)
		FunctionTable::iterator l =
		    dm_functions.insert(
		        std::make_pair(pseudo_name.str(),
				       std::vector<AddressRange>())
			).first;

		// Add this address range to the found/added function
		l->second.push_back(range);

	    }

	    // Otherwise this is just a plain-vanilla function
	    else {

		// Add this address range to the found/added function
		j->second.push_back(range);

	    }

	}
	
    }

    // Get the list of statements in this module
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
    std::vector<BPatch_statement> statements;
    module.getStatements(statements);
#else
    std::vector<struct BPatch_module::Statement> statements = module.getStatements();
#endif
    
    // Iterate over each statement in this module
    for(int i = 0; i < statements.size(); ++i) {

	// Construct a StatementEntry for this statement
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
	StatementEntry entry(FileName(statements[i].fileName()),
                             statements[i].lineNumber(), statements[i].lineOffset());
#else
	StatementEntry entry(FileName(statements[i].path),
			     statements[i].line, statements[i].column);
#endif

	// Add this statement to the table (or find the existing entry)
	StatementTable::iterator j =
	    dm_statements.insert(
	        std::make_pair(entry, std::vector<AddressRange>())
		).first;

	// Get the begin/end addresses of the statement
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
	Address begin((uint64_t)statements[i].startAddr());
	Address end((uint64_t)statements[i].endAddr());
#else
	Address begin(statements[i].begin);
	Address end(statements[i].end);
#endif

	// Sanity checks
	if(end <= begin) {

#ifndef NDEBUG
	    if(Backend::isSymbolsDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
		       << "addModule(): Statement " << statements[i].fileName() 
		       << ", line " << statements[i].lineNumber()
		       << ", column " << statements[i].lineOffset()
#else
		       << "addModule(): Statement " << statements[i].path 
		       << ", line " << statements[i].line
		       << ", column " << statements[i].column
#endif
		       << ": begin (" << Address(begin) 
		       << ") >= end (" << Address(end) << ")."
		       << std::endl;
		std::cerr << output.str();
	    }
#endif
    
 	    continue;
 	}
	if((begin < module_begin) || (begin >= module_end) ||
	   (end <= module_begin) || (end > module_end)) {

#ifndef NDEBUG
	    if(Backend::isSymbolsDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
#if (DYNINST_MAJOR == 6) || (DYNINST_MAJOR == 5 && DYNINST_MINOR == 2) || (DYNINST_MAJOR == 7 || DYNINST_MAJOR == 8)
		       << "addModule(): Statement " << statements[i].fileName() 
		       << ", line " << statements[i].lineNumber()
		       << ", column " << statements[i].lineOffset()
#else
		       << "addModule(): Statement " << statements[i].path 
		       << ", line " << statements[i].line
		       << ", column " << statements[i].column
#endif
		       << ": begin=" << Address(begin) 
		       << ", end=" << Address(end)
		       << ": is outside the module." << std::endl;
		std::cerr << output.str();
	    }
#endif
    
 	    continue;
 	}

	// Form address range of the statement relative to module beginning
	AddressRange range(begin - module_begin, end - module_begin);

	// Add this address range to the found/added statement
	j->second.push_back(range);

    } 
} 



/**
 * Conversion to OpenSS_Protocol_SymbolTable.
 *
 * Convert this symbol table to an OpenSS_Protocol_SymbolTable object.
 *
 * @note    The caller assumes responsibility for releasing all allocated
 *          memory when it is no longer needed.
 *
 * @note    The "experiments" and "linked_object" fields of the returned object
 *          are not initialized to any value. It is the responsibility of the
 *          caller to fill in these fields.
 *
 * @return    An OpenSS_Protocol_SymbolTable containing the contents of this
 *            symbol table.
 */
SymbolTable::operator OpenSS_Protocol_SymbolTable() const
{
    OpenSS_Protocol_SymbolTable object;

    // Allocate an appropriately sized array of functions
    object.functions.functions_len = dm_functions.size();
    object.functions.functions_val =
	reinterpret_cast<OpenSS_Protocol_FunctionEntry*>(malloc(
	    std::max(static_cast<FunctionTable::size_type>(1),
		     dm_functions.size()) *
	    sizeof(OpenSS_Protocol_FunctionEntry)
	    ));
	
    // Iterate over all the functions in this symbol table
    int idx = 0;
    for(FunctionTable::const_iterator
	    i = dm_functions.begin(); i != dm_functions.end(); ++i, ++idx) {
	OpenSS_Protocol_FunctionEntry& entry = 
	    object.functions.functions_val[idx];

	// Convert the function's name
	OpenSpeedShop::Framework::convert(i->first, entry.name);
	
	// Partition this function's address ranges into bitmaps
	std::vector<AddressBitmap> bitmaps = partitionAddressRanges(i->second);
	
	// Convert the function's bitmaps
	convert(bitmaps, entry.bitmaps.bitmaps_len, entry.bitmaps.bitmaps_val);
	
    }

    // Allocate an appropriately sized array of statements
    object.statements.statements_len = dm_statements.size();
    object.statements.statements_val =
	reinterpret_cast<OpenSS_Protocol_StatementEntry*>(malloc(
	    std::max(static_cast<StatementTable::size_type>(1),
		     dm_statements.size()) *
	    sizeof(OpenSS_Protocol_StatementEntry)
	    ));
    
    // Iterate over all the statements in this symbol table
    idx = 0;
    for(StatementTable::const_iterator
	    i = dm_statements.begin(); i != dm_statements.end(); ++i, ++idx) {
	OpenSS_Protocol_StatementEntry& entry =
	    object.statements.statements_val[idx];
	
	// Convert the statement's path, line, and column
	entry.path = i->first.dm_path;
	entry.line = i->first.dm_line;
	entry.column = i->first.dm_column;
	
	// Partition this statement's address ranges into bitmaps
	std::vector<AddressBitmap> bitmaps = partitionAddressRanges(i->second);
	
	// Convert the function's bitmaps
	convert(bitmaps, entry.bitmaps.bitmaps_len, entry.bitmaps.bitmaps_val);
	
    }

    // Return the conversion to the caller
    return object;
}



/**
 * Partition address ranges.
 *
 * Partitions address ranges into address bitmaps. The addresses for a function
 * or statement are stored as pairings of an address range and a bitmap - one
 * bit per address in the range - that describes which addresses within the
 * range are associated with the function or statement. In the common case where
 * the addresses exhibit a high degree of spatial locality, storing a single
 * address range and bitmap is very effective. But there are cases, such as
 * inlined functions, where the degree of spatial locality is minimal. Under
 * such circumstances, a single bitmap can grow very large and it is more space
 * efficient to use multiple bitmaps that individually exhibit spatial locality.
 * This function iteratively subdivides all the addresses until each bitmap
 * exhibits a "sufficient" amount of spatial locality.
 *
 * @note    The criteria for subidiving an address set is as follows. The
 *          widest gap (spacing) between two adjacent addresses is found. If
 *          the number of bits required to encode this gap in the bitmap is
 *          greater than the number of bits required to create an AddressRange
 *          object, then the set is partitioned at this, widest, gap.
 *
 * @param ranges    Address ranges to be partitioned.
 * @return          Address bitmaps representing these address ranges.
 */
std::vector<AddressBitmap>
SymbolTable::partitionAddressRanges(const std::vector<AddressRange>& ranges)
{
    std::vector<AddressBitmap> bitmaps;

    // Set the partitioning criteria
    static const Address::difference_type PartitioningCriteria =
	8 * (sizeof(uint32_t) + 2 * sizeof(uint64_t));

    // Construct the set of unique addresses in these address ranges
    std::set<Address> addresses;
    for(std::vector<AddressRange>::const_iterator
	    i = ranges.begin(); i != ranges.end(); ++i)
	for(Address j = i->getBegin(); j != i->getEnd(); ++j)
	    addresses.insert(j);
    
    // Initialize a queue with this initial, input, set of addresses
    std::deque<std::set<Address> > queue(1, addresses);

    // Iterate until the queue is empty
    while(!queue.empty()) {
        std::set<Address> i = queue.front();
        queue.pop_front();

	// Handle special case for empty sets (ignore them)
        if(i.size() == 0)
            continue;

	// Handle special case for single-element sets
        if(i.size() == 1) {

	    // Create and populate an address bitmap for this address set
	    AddressBitmap bitmap(AddressRange(*(i.begin()), *(i.rbegin()) + 1));
	    for(std::set<Address>::const_iterator
                    j = i.begin(); j != i.end(); ++j)
                bitmap.setValue(*j, true);

	    // Add this bitmap to the results
	    bitmaps.push_back(bitmap);
	    
            continue;
        }
        
	// Pair specifying the widest gap in this address set
        std::pair<std::set<Address>::const_iterator, Address::difference_type>
            widest_gap = std::make_pair(i.begin(), 0);
	
        // Iterate over each adjacent pair of addresses in this set
        for(std::set<Address>::const_iterator
		prev = i.begin(), current = ++i.begin();
            current != i.end();
            ++prev, ++current) {

            // Calculate the gap between this adjacent address pair
            Address::difference_type gap =
                AddressRange(*prev, *current).getWidth() - 1;

	    // Is this gap the widest so far?
            if(gap > widest_gap.second) {
		widest_gap.first = current;
                widest_gap.second = gap;
            }
            
        }

	// Is the widest gap greater than our partitioning criteria?
        if(widest_gap.second > PartitioningCriteria) {

	    // Partition the set at this gap
	    queue.push_back(std::set<Address>(i.begin(), widest_gap.first));
            queue.push_back(std::set<Address>(widest_gap.first, i.end()));

	}
        
        // Otherwise keep this set unpartitioned
        else {

	    // Create and populate an address bitmap for this address set
	    AddressBitmap bitmap(AddressRange(*(i.begin()), *(i.rbegin()) + 1));
            for(std::set<Address>::const_iterator 
		    j = i.begin(); j != i.end(); ++j)
                bitmap.setValue(*j, true);

	    // Add this bitmap to the results
	    bitmaps.push_back(bitmap);

	}

    }

    // Return the bitmaps to the caller
    return bitmaps;
}



/**
 * Convert bitmaps for protocol use.
 *
 * Converts the specified address bitmaps to the structure used in protocol
 * messages.
 *
 * @note    The caller assumes responsibility for releasing all allocated
 *          memory when it is no longer needed.
 *
 * @param bitmaps         Address bitmaps to be converted.
 * @retval bitmaps_len    Length of array containing the results.
 * @retval bitmaps_val    Array containing the results.
 */
void SymbolTable::convert(const std::vector<AddressBitmap>& bitmaps,
			  u_int& bitmaps_len,
			  OpenSS_Protocol_AddressBitmap*& bitmaps_val)
{
    // Allocate an appropriately sized array of bitmaps
    bitmaps_len = bitmaps.size();
    bitmaps_val = 
	reinterpret_cast<OpenSS_Protocol_AddressBitmap*>(malloc(
            std::max(static_cast<std::vector<AddressBitmap>::size_type>(1),
		     bitmaps.size()) *
	    sizeof(OpenSS_Protocol_AddressBitmap)
	    ));
    
    // Iterate over all the bitmaps
    for(std::vector<AddressBitmap>::size_type i = 0; i < bitmaps.size(); ++i) {
	OpenSS_Protocol_AddressBitmap& entry = bitmaps_val[i];
	
	// Convert the bitmap's address range
	entry.range.begin = bitmaps[i].getRange().getBegin().getValue();
	entry.range.end = bitmaps[i].getRange().getEnd().getValue();
	
	// Convert the actual bitmap
	OpenSpeedShop::Framework::convert(bitmaps[i].getBlob(), entry.bitmap);
	
    }
}
