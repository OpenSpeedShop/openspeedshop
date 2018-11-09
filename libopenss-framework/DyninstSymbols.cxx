////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2018 The Krell Institute. All Rights Reserved.
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
 * Definition of the DyninstSymbols namespace.
 *
 */

#include "Address.hxx"
#include "AddressRange.hxx"
#include "DyninstSymbols.hxx"
#include "LinkedObject.hxx"

#include <BPatch.h>
#include <BPatch_addressSpace.h>
#include <BPatch_basicBlock.h>
#include <BPatch_basicBlockLoop.h>
#include <BPatch_flowGraph.h>
#include <BPatch_function.h>
#include <BPatch_image.h>
#include <BPatch_statement.h>
#include <BPatch_module.h>
#include <BPatch_Vector.h>

#include "InstructionDecoder.h"
#include "InstructionCache.h"

using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;

using namespace OpenSpeedShop::Framework;

static bool vector_info_already_printed = false;
static bool loop_info_already_printed = false;


/**
 * Loop information.
 *
 * Structure for per-loop information obtained from Dyninst. Contains the head
 * address and all address ranges of the loop. Hidden within this file because
 * it is only used to return data from the internal getLoopsAt() to getLoops().
 */
struct LoopInfo
{
    /** Head address of the loop. */
    OpenSpeedShop::Framework::Address dm_head;
    
    /** All address ranges of the loop. */
    std::vector<AddressRange> dm_ranges;
    
    /** Constructor from the loop's head address. */
    LoopInfo(const OpenSpeedShop::Framework::Address& head): dm_head(head), dm_ranges() { }
};

struct VectorInstrInfo
{
    /** Address of the vector instruction. Current focus is on AVX512 */
    OpenSpeedShop::Framework::Address dm_vector_addr;

    /** Maximum hardware allowed instruction vector length */
    int32_t dm_max_instr_vl;

    /** Actual instruction vector length detected by O|SS */
    int32_t dm_actual_vl;

    /** Actual instruction vector instruction op code redendered to a string */
    std::string dm_instr_opcode;
    
    /** Constructor from the vector instruction address. */
     VectorInstrInfo(const OpenSpeedShop::Framework::Address& dm_vector_addr): dm_vector_addr() { }
};



#ifndef NDEBUG
/** Flag indicating if debugging for loop information is enabled. */
bool is_debug_enabled = (getenv("OPENSS_DEBUG_DYNINSTAPI_LOOPS") != NULL);
bool is_debug_vector_enabled = (getenv("OPENSS_DEBUG_VECTOR") != NULL);
bool is_debug_vector_details_enabled = (getenv("OPENSS_DEBUG_VECTOR_DETAILS") != NULL);
#endif

/**
 * Detect VectorInstrInfo instruction occurrences from the sampled addresses for a linked object.
 *
 * Get the addresses where vector instructions were sampled for the specified linked 
 * object and add it to the given symbol tables.  Current focus is on AVX512 instructions.
 *
 */
std::vector<VectorInstrInfo> detectVectorInstructions(const OpenSpeedShop::Framework::Address& address, BPatch_image& image)
{
    std::vector<VectorInstrInfo> retval;

    // Iterate over each module within the specified image
    BPatch_Vector<BPatch_module*>* modules = image.getModules();

    if (modules == NULL) {
	return retval;
    }
    
    for (unsigned int m = 0; m < modules->size(); ++m)
    {
	BPatch_module* module = (*modules)[m];
	if (module == NULL) {
	    continue;
	}

        // Compute the linked object address range, to determine if the sample address
        // exists in the address range for the linked object to save search time.
	OpenSpeedShop::Framework::Address module_base = 
						(uint64_t)module->getBaseAddr();
	OpenSpeedShop::Framework::AddressRange m_range(module_base,module_base + 
							(uint64_t)module->getSize());

        // Adding the module_base to a normalized address should
        // match a sample address (or function definition start address or callstack address etc). 
	OpenSpeedShop::Framework::Address base_adjusted_address(module_base + address);
	if ( !(m_range.doesContain(base_adjusted_address)) ) {
	    continue;
	}

	// Find the function(s) containing the specified address
	BPatch_Vector<BPatch_function*> functions;

	// Find the function the address is contained in 
	module->findFunctionByAddress((void*)((base_adjusted_address).getValue()), 
							functions, false, false);
        
	// loop through the functions to get access to the instructions via parseAPI
	for (unsigned int f = 0; f < functions.size(); ++f)
	{

	    BPatch_function* function = functions[f];
	    if (function == NULL) {
		continue;
	    }

#ifndef NDEBUG
	    if (is_debug_vector_details_enabled) {
		std::cerr << "detectVectorInstructions: function "
			  << f << " of " << functions.size() << " contains address:" 
			  << address << " base_adjusted_address:" 
			  << base_adjusted_address.getValue() << std::endl;
	    }
#endif

            // Convert the BPatch function to parse function, to get at the instructions
	    Dyninst::ParseAPI::Function* parse_function = Dyninst::ParseAPI::convert(function);
	    for (const auto& block : parse_function->blocks()) {

		Dyninst::ParseAPI::Block::Insns block_instructions;
		block->getInsns(block_instructions);

		OpenSpeedShop::Framework::Address current = block->start();

		int64_t block_size = block->size();
		for (const auto& instruction : block_instructions)
		{
		    if (base_adjusted_address >= instruction.first &&
			base_adjusted_address <= instruction.first+block_size ) {

			if (instruction.first == base_adjusted_address) {
#ifndef NDEBUG
			    if (is_debug_vector_details_enabled) {
				std::cerr << "detectVectorInstructions address match for:" 
					  << std::hex << instruction.first
				          << " vector instr opcode:" 
#if DyninstAPI_VERSION_MAJOR >= 10
					  << instruction.second.format(instruction.first).c_str() 
#else
					  << instruction.second->format(instruction.first).c_str() 
#endif
					  << std::endl;
			    }
#endif
			    // Get the instruction category and see if this instruction is vector
			    // Then look at the operand sizes for each operand in the instruction
#if DyninstAPI_VERSION_MAJOR >= 10
			    InsnCategory category = instruction.second.getCategory();
#else
			    InsnCategory category = instruction.second->getCategory();
#endif
			    if (category == c_VectorInsn) {
				std::vector<InstructionAPI::Operand> operands;
#if DyninstAPI_VERSION_MAJOR >= 10
				instruction.second.getOperands(operands);
#else
				instruction.second->getOperands(operands);
#endif
				int max_size = 0;
				for (unsigned i = 0; i < operands.size(); i++) {
				    InstructionAPI::Expression::Ptr value = operands[i].getValue();
				    if (value->size() > max_size) {
					max_size = value->size();
				    }
				}

				// This is the spot where we can determine what size of operand we 
				// are looking for vector detection would require checking 
				// for max_size == 64, if finding AVX512 instructions.
				// Here we default to AVX512 by setting these variables.  This is
				// a failsafe, as OPENSS_VINSTR_OPSIZE should always be set by
				// ossdriver.in.

				// ossdriver.in sets OPENSS_VINSTR_OPSIZE to either 512, 256, or
				// 128.   Since Dyninst uses byte sizes, we divide by BITSPERBYTE.

				int64_t vinstr_opsize_in_bytes = 512 ;
				int64_t vinstr_opsize_in_bits = 512 ;
				if (getenv("OPENSS_VINSTR_OPSIZE") != NULL) {
				    vinstr_opsize_in_bits = atoi(getenv("OPENSS_VINSTR_OPSIZE"));
				}
				vinstr_opsize_in_bytes = vinstr_opsize_in_bits / BITSPERBYTE ;

				if (max_size >= vinstr_opsize_in_bytes) {
				    // Use the normalized address to create the necessary 
				    // vector information to return.
				    VectorInstrInfo info(address);
				    info.dm_vector_addr = address;
				    info.dm_max_instr_vl = max_size;
				    info.dm_actual_vl = 0;
#if DyninstAPI_VERSION_MAJOR >= 10
				    info.dm_instr_opcode = instruction.second.format(instruction.first);
#else
				    info.dm_instr_opcode = instruction.second->format(instruction.first);
#endif
#ifndef NDEBUG
				    if (is_debug_vector_enabled) {
					std::cerr << "detectVectorInstructions module_base:" << module_base
					             << " address:" << address
					             << " push back values "
					             << " dm_vector_addr:" << info.dm_vector_addr
					             << " dm_max_instr_vl:" << info.dm_max_instr_vl
					             << " dm_instr_opcode:" << info.dm_instr_opcode
					             << std::endl;
				    }
#endif

                                    retval.push_back(info);
                                    break;

				} // if max_size check

			    } // if vector category

			    continue;

			} // end  if (instruction.first == address) 
                    
		    }
                 
		} // end for block_instructions

	    } // end for parse->block

	} // end for f

    } // end for m

#ifndef NDEBUG
    if (is_debug_vector_details_enabled) {
	std::cerr << "EXIT detectVectorInstructions, address=" << std::hex << address
		  << " After modules loop, modules->size()=" << modules->size() << std::endl;
	std::cerr << "" << std::endl;
    }
#endif
    return retval;
}

/** Get the loops containing the specified address. */
std::vector<LoopInfo> getLoopsAt(const OpenSpeedShop::Framework::Address& address, BPatch_image& image)
{
    std::vector<LoopInfo> retval;
    
    // Iterate over each module within the specified image
    
    BPatch_Vector<BPatch_module*>* modules = image.getModules();
    
    if (modules == NULL)
    {
	return retval;
    }
    
    for (unsigned int m = 0; m < modules->size(); ++m)
    {
	BPatch_module* module = (*modules)[m];
        
	if (module == NULL)
	{
	    continue;
	}
        
	OpenSpeedShop::Framework::Address module_base = (uint64_t)module->getBaseAddr();

	// Find the function(s) containing the specified address

	BPatch_Vector<BPatch_function*> functions;

	module->findFunctionByAddress((void*)((module_base + address).getValue()), functions, false, false);
        
	for (unsigned int f = 0; f < functions.size(); ++f)
	{
	    BPatch_function* function = functions[f];
            
	    if (function == NULL)
	    {
		continue;
	    }

	    // Find the loops containing the specified address
            
	    BPatch_flowGraph* cfg = function->getCFG();
            
	    if (cfg == NULL)
	    {
		continue;
	    }
            
	    BPatch_Vector<BPatch_basicBlockLoop*> loops;
	    cfg->getLoops(loops);
            
	    for (unsigned int l = 0; l < loops.size(); ++l)
	    {
		BPatch_basicBlockLoop* loop = loops[l];
                
		if ((loop == NULL) || !loop->containsAddressInclusive( (module_base + address).getValue()))
		{
		    continue;
                }
                
		// A loop containing this address has been found! Rejoice!
		// And, of course, obtain the loop's head address and basic
		// block address ranges...

		#if DyninstAPI_VERSION_MAJOR >= 9
		
		    // Need to use the new dyninst API for finding the head of the loop
		    // One possibility - from wdh - might be to call getLoopEntries() to 
		    // get the basic block of each entry. Then, for each of these, take 
		    // the first address of that basic block and query the source
		    // file/line containing that address. Assuming that all line numbers are within
		    // a single source file, the minimum line number is probably reasonably the
		    // loop definition. And the first address in that basic block would be the one
		    // to use for “addr_head” in the Open|SS database.

		    BPatch_basicBlock* head;
		    std::vector<BPatch_basicBlock*> entries;

		    loop->getLoopEntries(entries);

		    // bbe: Loop through the basic block entries 
		    std::vector<BPatch_basicBlock*>::iterator bbe;

		    // filesAndlines: Return value file names and line numbers from getSourceLines
		    std::vector<BPatch_statement > filesAndlines ;
                  
		    // Loop through the loops basic blocks, get the starting address of the block
		    // Then use that address to get the filename and line number for that address
		    // We are looking for the minimum line number for the blocks in the loop to use
		    // as the loop head basic block.

		    head = entries[0]; // give an initial value to the loop head
		    for (bbe = entries.begin(); bbe != entries.end(); ++bbe) {
                     
#if 1
			unsigned long module_base = (uint64_t)module->getBaseAddr();
			unsigned long bbstartAddr =  (*bbe)->getStartAddress() - module_base;
#else
			unsigned long module_base = (uint64_t)module->getBaseAddr();
			unsigned long bbstartAddr = (*bbe)->getStartAddress() ;
#endif

			bool linesFound = module->getSourceLines( bbstartAddr, filesAndlines);

			if (linesFound) {
			    std::vector<BPatch_statement>::iterator lf_dx;
			    for (lf_dx = filesAndlines.begin(); lf_dx != filesAndlines.end(); ++lf_dx) {
				continue;
			    }
			} // linesFound
                    
		    } // entries
		#else
		    BPatch_basicBlock* head = loop->getLoopHead();
		#endif
                
		if (head == NULL)
		{
		    continue;
		}

		// Use the loop head basic block to create the necessary loop information to return
		LoopInfo info(OpenSpeedShop::Framework::Address(head->getStartAddress()) - module_base);

		BPatch_Vector<BPatch_basicBlock*> blocks;
		loop->getLoopBasicBlocks(blocks);
                
		for (unsigned int i = 0; i < blocks.size(); ++i)
		{
		    BPatch_basicBlock* block = blocks[i];

		    if (block != NULL)
		    {
			info.dm_ranges.push_back(
			    OpenSpeedShop::Framework::AddressRange(
				OpenSpeedShop::Framework::Address(block->getStartAddress()) - module_base,
				OpenSpeedShop::Framework::Address(block->getEndAddress()) - module_base
			    )
			);
		    }
		}
                
		retval.push_back(info);
                
	    } // l
	} // f
    } // m

    return retval;
}



/**
 * Get loops for a linked object.
 *
 * Get loop information for the specified linked object and add it to the given
 * symbol tables. Only obtain information about those loops which contain one or
 * more of the specified unique addresses.
 *
 * @param unique_addresses    Restrict the collected information to loops
 *                            that contain one or more of these addresses.
 * @param linked_object       Linked object for which to get loops.
 * @param symbol_tables       Symbol tables to contain the loop information.
 *
 * @note    The set of unique addresses is intentionally non-const in order to
 *          allow this function to update that list. This is a dirty trick that
 *          is used to insure that OfflineExperiment resolves all the statements
 *          containing loop head addresses.
 */
void DyninstSymbols::getLoops(std::set<OpenSpeedShop::Framework::Address>& unique_addresses,
                              const LinkedObject& linked_object,
                              SymbolTableMap& symbol_tables)
{

    // Find the specified linked object's possible address ranges
    std::set<AddressRange> ranges = linked_object.getAddressRange();

    // Open the specified linked object with Dyninst
    
    BPatch bpatch;
    
    BPatch_addressSpace* space = bpatch.openBinary(
        linked_object.getPath().c_str(), false
        );
    
    if (space == NULL)
    {
        std::cout << "WARNING: Dyninst failed to open the linked object \"" 
                  << linked_object.getPath() << "\"!" << std::endl;
        return;
    }
    
    BPatch_image* image = space->getImage();
    
    if (image == NULL)
    {
        std::cout << "WARNING: Dyninst failed to open the linked object \"" 
                  << linked_object.getPath() << "\"!" << std::endl;
        return;
    }

    // Make a copy of the specified set of unique addresses and use the copy
    // to track which addresses have been resolved to their containing loop.
    // Keep iterating until the copy is empty.
    
    std::set<OpenSpeedShop::Framework::Address> unique_addresses_copy(unique_addresses);

    while (!unique_addresses_copy.empty())
    {
        
        OpenSpeedShop::Framework::Address next = *unique_addresses_copy.begin();

        // Find which locations (if any) of the specified linked object
        // contain this address. Since the linked object might be found
        // at multiple different base addresses, more than one location
        // may be found.
        
        for (std::set<AddressRange>::const_iterator i = 
                 ranges.begin(); i != ranges.end(); ++i)
        {
            if (!i->doesContain(next))
            {
                continue;
            }

            // Find which symbol table (if any) correponds to this address
            // range. It is unclear to me why this is necessary, but Don's
            // code in SymtabAPISymbols does this, so when in Rome...
                
            if (symbol_tables.find(*i) == symbol_tables.end())
            {
                continue;
            }
            
            SymbolTable& symbol_table = symbol_tables.find(*i)->second.first;

            // Find the loops (if any) that contain this address. Note
            // that getLoopsAt() expects an address which is relative to
            // the beginning of the linked object.

            std::vector<LoopInfo> loops = 
                getLoopsAt(next - i->getBegin(), *image);
            
            for(std::vector<LoopInfo>::const_iterator
                    j = loops.begin(); j != loops.end(); ++j)
            {
                
                // Add this loop's address ranges to the symbol table. Note
                // that findLoop() returns addresses which are relative to
                // the beginning of the linked object. Also note that the
                // loop's head address is inserted back into the caller's
                // set of unique addresses. This is an ugly trick used to
                // insure that OfflineExperiment resolves all of the loop 
                // head addresses to statements, also insuring that all of
                // the loop definitions are available.
                
                OpenSpeedShop::Framework::Address head(i->getBegin() + j->dm_head);
                
                unique_addresses.insert(head); /* NOT unique_addresses_copy! */
                
                for (std::vector<AddressRange>::const_iterator
                         k = j->dm_ranges.begin(); k != j->dm_ranges.end(); ++k)
                {
                    OpenSpeedShop::Framework::Address begin(i->getBegin() + k->getBegin());
                    OpenSpeedShop::Framework::Address end(i->getBegin() + k->getEnd());

                    symbol_table.addLoop(begin, end, head);

                    // Now erase from our copy of the unique addresses all
                    // such addresses which are covered by this loop. There
                    // is no need to look them up again since they have now
                    // been resolved.
                    
                    unique_addresses_copy.erase(
                        unique_addresses_copy.lower_bound(begin),
                        unique_addresses_copy.upper_bound(end)
                        );

                } // k
            } // j
        } // i

        // If this address couldn't be resolved it will still be found in
        // our copy of unique addresses. And if isn't erased, we'll simply
        // loop endlessly trying to resolve that same address.
        
        unique_addresses_copy.erase(next);
        
    } // while (!unique_addresses_copy.empty())

}


/**
 * Get vector instructon information for a linked object.
 *
 * Get vector instructon information for the specified linked object and add it to the given
 * symbol tables. Only obtain information about vector instruction usage which contain one or
 * more of the specified unique addresses.
 *
 * @param unique_addresses    Restrict the collected information to statements
 *                            that contain one or more of these addresses.
 * @param linked_object       Linked object for which to get vector information.
 * @param symbol_tables       Symbol tables to contain the vector instruction usage information.
 *
 */
void DyninstSymbols::getVectorInstrs(std::set<OpenSpeedShop::Framework::Address>& unique_addresses,
				     const LinkedObject& linked_object,
				     SymbolTableMap& symbol_tables)
{

    int vector_unique_count_processed = 0;
    int vector_unique_count_skipped = 0;
    int vector_instructions_found = 0;

    // Find the specified linked object's possible address ranges
    std::set<AddressRange> ranges = linked_object.getAddressRange();

#ifndef NDEBUG
    if (is_debug_vector_details_enabled) {
	std::cerr << "ENTER getVectorInstrs, linkedobject=" << linked_object.getPath() << std::endl;
	for (std::set<AddressRange>::const_iterator i = ranges.begin(); i != ranges.end(); ++i)
	{
	    std::cerr << "linkedobject=" << linked_object.getPath() << " AddressRange: " << std::hex << *i << std::endl;
	}
    }
#endif
    
    // Open the specified linked object with Dyninst
    BPatch bpatch;
    BPatch_addressSpace* space = bpatch.openBinary(
	linked_object.getPath().c_str(), false
	);
    
    if (space == NULL)
    {
	std::cout << "WARNING: Dyninst failed to open the linked object \"" 
		  << linked_object.getPath() << "\"!" << std::endl;
	return;
    }
    
    BPatch_image* image = space->getImage();
    
    if (image == NULL)
    {
	std::cout << "WARNING: Dyninst failed to open the linked object \"" 
		  << linked_object.getPath() << "\"!" << std::endl;
        return;
    }

    // Make a copy of the specified set of unique addresses and use the copy
    // to track which addresses have been resolved to their containing loop.
    // Keep iterating until the copy is empty.
    
    std::set<OpenSpeedShop::Framework::Address> unique_addresses_copy(unique_addresses);

    while (!unique_addresses_copy.empty())
    {
        
	OpenSpeedShop::Framework::Address next = *unique_addresses_copy.begin();

	// Find which locations (if any) of the specified linked object
	// contain this address. Since the linked object might be found
	// at multiple different base addresses, more than one location
	// may be found.
        
	for (std::set<AddressRange>::const_iterator i = ranges.begin(); i != ranges.end(); ++i)
	{
	    if (!i->doesContain(next))
	    {
		vector_unique_count_skipped = vector_unique_count_skipped + 1;
		continue;
	    }

	    // Find which symbol table (if any) correponds to this address
	    // range. It is unclear to me why this is necessary, but Don's
	    // code in SymtabAPISymbols does this, so when in Rome...
                
	    if (symbol_tables.find(*i) == symbol_tables.end())
	    {
		vector_unique_count_skipped = vector_unique_count_skipped + 1;
		continue;
	    }
            
	    SymbolTable& symbol_table = symbol_tables.find(*i)->second.first;

	    std::vector<VectorInstrInfo> vinstructions = detectVectorInstructions(next - i->getBegin(), *image);

	    // Process the vector instruction hits here, before we call detectVectorInstructions again

	    if (vinstructions.size() == 1) {

		std::vector<VectorInstrInfo>::const_iterator j = vinstructions.begin();

		symbol_table.addVectorInstr(j->dm_vector_addr, j->dm_max_instr_vl, j->dm_actual_vl, j->dm_instr_opcode);

		vector_instructions_found = vector_instructions_found + 1;
	    }
            
	    vector_unique_count_processed = vector_unique_count_processed + 1;

	} // i

	// If this address couldn't be resolved it will still be found in
	// our copy of unique addresses. And if isn't erased, we'll simply
	// loop endlessly trying to resolve that same address.
        
	unique_addresses_copy.erase(next);

        
    } // while (!unique_addresses_copy.empty())

#ifndef NDEBUG
    if (is_debug_vector_enabled) {
	std::cerr << "EXIT getVectorInstrs, linkedobject=" << linked_object.getPath() 
		  << " Instructions: Processed=" << std::dec << vector_unique_count_processed 
		  << " Skipped=" << std::dec << vector_unique_count_skipped 
		  << " Vector Found=" << std::dec << vector_instructions_found 
		  << std::endl;
    }
#endif

}

