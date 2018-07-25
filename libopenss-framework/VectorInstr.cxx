////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2018 Krell Institute. All Rights Reserved.
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
 * Definition of the VectorInstr class.
 *
 */

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "ExtentGroup.hxx"
#include "Function.hxx"
#include "FunctionCache.hxx"
#include "LinkedObject.hxx"
#include "Loop.hxx"
#include "LoopCache.hxx"
#include "Statement.hxx"
#include "StatementCache.hxx"
#include "Thread.hxx"
#include "VectorInstr.hxx"
#include "VectorInstrCache.hxx"

using namespace OpenSpeedShop::Framework;

/**
 * Get our threads.
 *
 * Returns the threads containing this vector instruction.
 *
 * @return    Threads containing this vector instruction.
 */
std::set<Thread> VectorInstr::getThreads() const
{
    std::set<Thread> threads;
    
    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.thread
    //       FROM AddressSpaces
    //         JOIN VectorInstrs
    //       ON AddressSpaces.linked_object = VectorInstrs.linked_object
    //       WHERE VectorInstrs.id = <dm_entry>;
    //
    //       However the implementation below, combined with an index on
    //       AddressSpaces(linked_object), was found to be quite a bit faster.
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM VectorInstrs WHERE VectorInstrs.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
	int linked_object = dm_database->getResultAsInteger(1);	
        
	// Find all the threads containing our linked object
	dm_database->prepareStatement(
	    "SELECT thread FROM AddressSpaces WHERE linked_object = ?;"
	    );
	dm_database->bindArgument(1, linked_object);
	while(dm_database->executeStatement())
	    threads.insert(Thread(dm_database, 
				  dm_database->getResultAsInteger(1)));
        
    }	
    if(threads.empty())
	throw Exception(Exception::EntryNotFound, "Threads",
			"<VectorInstrs-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}

/**
 * Get our extent in a thread.
 *
 * Returns the extent of this vector instruction within the specified thread. An empty extent
 * is returned if this vector instruction isn't present within the specified thread.
 *
 * @pre    The thread must be in the same experiment as the vector instruction. An
 *         assertion failure occurs if the thread is in a different
 *         experiment than the vector instruction.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this vector instruction in that thread.
 */
ExtentGroup VectorInstr::getExtentIn(const Thread& thread) const
{
    ExtentGroup extent;
    
    // Check assertions
    Assert(inSameDatabase(thread));

    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.time_begin,
    //              AddressSpaces.time_end,
    //              AddressSpaces.addr_begin,
    //              VectorInstrRanges.addr_begin,
    //              VectorInstrRanges.addr_end,
    //              VectorInstrRanges.valid_bitmap
    //       FROM AddressSpaces
    //         JOIN VectorInstrs
    //         JOIN VectorInstrRanges
    //       ON AddressSpaces.linked_object = VectorInstrs.linked_object
    //         AND VectorInstrs.id = VectorInstrRanges.vectorinstr
    //       WHERE AddressSpaces.thread = <thread.dm_entry>
    //         AND VectorInstrRanges.vectorinstr = <dm_entry>;
    //
    //       However the implementation below, combined with indices on
    //       AddressSpaces(linked_object, thread) & VectorInstrRanges(vectorinstr), was
    //       found to be quite a bit faster.
    
    // Find our linked object and address ranges with associated bitmaps
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
	"SELECT VectorInstrs.linked_object, "
	"       VectorInstrRanges.addr_begin, "
	"       VectorInstrRanges.addr_end, "
	"       VectorInstrRanges.valid_bitmap "
	"FROM VectorInstrRanges "
	"  JOIN VectorInstrs "
	"ON VectorInstrRanges.vectorinstr = VectorInstrs.id "
	"WHERE VectorInstrs.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
	int linked_object = dm_database->getResultAsInteger(1);
	std::set<AddressRange> ranges =
	    AddressBitmap(AddressRange(dm_database->getResultAsAddress(2),
				dm_database->getResultAsAddress(3)),
				dm_database->getResultAsBlob(4)).getContiguousRanges(true);
        
	// Find all the uses of this linked object in the specified thread
	dm_database->prepareStatement(
	    "SELECT time_begin, "
	    "       time_end, "
	    "       addr_begin "
	    "FROM AddressSpaces "
	    "WHERE thread = ? "
	    "  AND linked_object = ?;"
	    );
	dm_database->bindArgument(1, EntrySpy(thread).getEntry());
	dm_database->bindArgument(2, linked_object);
	while(dm_database->executeStatement()) {
            
	    // Iterate over the addresss ranges for this vector instruction
	    for(std::set<AddressRange>::const_iterator
		    i = ranges.begin(); i != ranges.end(); ++i)
		extent.push_back(
		    Extent(TimeInterval(dm_database->getResultAsTime(1),
					dm_database->getResultAsTime(2)),
			   AddressRange(dm_database->getResultAsAddress(3) +
					    i->getBegin(),
					dm_database->getResultAsAddress(3) +
					    i->getEnd()))
		    );
            
	}
        
    }
    END_TRANSACTION(dm_database);
    
    // Return the extent to the caller    
    return extent;
}

/**
 * Get our linked object.
 *
 * Returns the linked object containing this vector instruction.
 *
 * @return    Linked object containing this vector instruction.
 */
LinkedObject VectorInstr::getLinkedObject() const
{
    LinkedObject linked_object;
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM VectorInstrs WHERE id = ?;"
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
 * Get our definitions.
 *
 * Returns the definitions of this vector instructions. An empty set is returned if no
 * definitions of this vector instructions are found.
 *
 * @return    Definitions of this vector instructions.
 */
std::set<Statement> VectorInstr::getDefinitions() const
{
    // Find our linked object and extent
    LinkedObject linked_object;
    ExtentGroup extent;
    getLinkedObjectAndExtent(linked_object, extent);

    // Find our vector instruction address
    Address vector_instr_addr;
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT addr_begin FROM VectorInstrs WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	vector_instr_addr = dm_database->getResultAsAddress(1);
    END_TRANSACTION(dm_database);

    //std::cerr << "VectorInstr::getDefinitions(), vector_instr_addr=" << vector_instr_addr << std::endl;

    // Use the vector instruction's address as its extent
    Extent first(TimeInterval(Time::TheBeginning(), Time::TheEnd()),
		 AddressRange(vector_instr_addr));
    extent.clear();
    extent.push_back(first);
    
    // Use the statement cache to find our definitions
    std::set<Statement> definitions =
	Statement::TheCache.getStatements(linked_object, extent);
    
    // Return the definitions to the caller
    return definitions;
}


/**
 * Get the vector instruction OpCode 
 *
 * Returns the op code for the vector instruction
 *
 * @return    Instruction opcode for this vector instruction.
 */
std::string VectorInstr::getOpCode() const
{
    std::string opcode;

    // Find our opcode
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT opcode FROM VectorInstrs WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	opcode = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);

    // Return the opcode to the caller
    return opcode;
}


/**
 * Get the maximum vector openand size/length for this vector instruction
 *
 * Returns the maximum operand length for the vector instruction
 * Basically, the max operand size for the vector instruction
 *
 * @return  machine vector length for this vector instruction.
 */
int VectorInstr::getMaxOperandSize() const
{
    int machinevl;

    // Find our maximum vector instruction operand size
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT max_instr_vl FROM VectorInstrs WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        machinevl = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);

    // Return the opcode to the caller
    return machinevl;
}


/**
 * Get our functions.
 *
 * Returns the functions containing this vector instruction. An empty set is returned if no
 * function contains this vector instruction.
 *
 * @return    Functions containing this vector instruction.
 */
std::set<Function> VectorInstr::getFunctions() const
{
    // Find our linked object and extent
    LinkedObject linked_object;
    ExtentGroup extent;
    getLinkedObjectAndExtent(linked_object, extent);
    
    // Use the function cache to find our functions
    std::set<Function> functions =
	Function::TheCache.getFunctions(linked_object, extent);
    
    // Return the functions to the caller
    return functions;
}

/**
 * Get our statements.
 *
 * Returns the statements associated with this vector instruction. An empty set is returned
 * if no statements are associated with this vector instruction.
 *
 * @return    Statements associated with this vector instruction.
 */
std::set<Statement> VectorInstr::getStatements() const
{
    // Find our linked object and extent
    LinkedObject linked_object;
    ExtentGroup extent;
    getLinkedObjectAndExtent(linked_object, extent);
    
    // Use the statement cache to find our statements
    std::set<Statement> statements =
	Statement::TheCache.getStatements(linked_object, extent);
    
    // Return the statements to the caller
    return statements;
}

/** VectorInstr cache. */
VectorInstrCache VectorInstr::TheCache;

/**
 * Default constructor.
 *
 * Constructs a VectorInstr that refers to a non-existent vector instruction. Any use of a member
 * function on an object constructed in this way will result in an assertion
 * failure.
 */
VectorInstr::VectorInstr() :
    Entry()
{
}

/**
 * Constructor from a vector instruction entry.
 *
 * Constructs a new VectorInstr for the specified vector instruction entry.
 *
 * @param database    Database containing this vector instruction.
 * @param entry       Identifier for this vector instruction.
 */
VectorInstr::VectorInstr(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, Entry::VectorInstrs, entry)
{
}


/**
 * Get our linked object and extent.
 *
 * Returns the linked object containing, and extent of, this vector instruction.
 *
 * @retval linked_object    Linked object containing this vector instruction.
 * @retval extent           Extent of this vector instruction.
 */
void VectorInstr::getLinkedObjectAndExtent(LinkedObject& linked_object,
                                    ExtentGroup& extent) const
{
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT VectorInstrs.linked_object, "
	"       VectorInstrRanges.addr_begin, "
	"       VectorInstrRanges.addr_end, "
	"       VectorInstrRanges.valid_bitmap "
	"FROM VectorInstrRanges "
	"  JOIN VectorInstrs "
	"ON VectorInstrRanges.vectorinstr = VectorInstrs.id "
	"WHERE VectorInstrs.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
	linked_object = LinkedObject(dm_database,
				     dm_database->getResultAsInteger(1));
        
        std::set<AddressRange> ranges =
	    AddressBitmap(AddressRange(dm_database->getResultAsAddress(2),
				dm_database->getResultAsAddress(3)),
				dm_database->getResultAsBlob(4)).getContiguousRanges(true);
        
	for(std::set<AddressRange>::const_iterator
		i = ranges.begin(); i != ranges.end(); ++i)
	    extent.push_back(
		Extent(TimeInterval(Time::TheBeginning(), Time::TheEnd()), *i)
		);
        
    }
    END_TRANSACTION(dm_database);
}
