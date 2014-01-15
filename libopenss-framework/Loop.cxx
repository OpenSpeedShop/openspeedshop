////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Krell Institute. All Rights Reserved.
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
 * Definition of the Loop class.
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

using namespace OpenSpeedShop::Framework;



/**
 * Get our threads.
 *
 * Returns the threads containing this loop.
 *
 * @return    Threads containing this loop.
 */
std::set<Thread> Loop::getThreads() const
{
    std::set<Thread> threads;
    
    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.thread
    //       FROM AddressSpaces
    //         JOIN Loops
    //       ON AddressSpaces.linked_object = Loops.linked_object
    //       WHERE Loops.id = <dm_entry>;
    //
    //       However the implementation below, combined with an index on
    //       AddressSpaces(linked_object), was found to be quite a bit faster.
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM Loops WHERE Loops.id = ?;"
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
                        "<Loops-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our extent in a thread.
 *
 * Returns the extent of this loop within the specified thread. An empty extent
 * is returned if this loop isn't present within the specified thread.
 *
 * @pre    The thread must be in the same experiment as the loop. An
 *         assertion failure occurs if the thread is in a different
 *         experiment than the loop.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this loop in that thread.
 */
ExtentGroup Loop::getExtentIn(const Thread& thread) const
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
    //              LoopRanges.addr_begin,
    //              LoopRanges.addr_end,
    //              LoopRanges.valid_bitmap
    //       FROM AddressSpaces
    //         JOIN Loops
    //         JOIN LoopRanges
    //       ON AddressSpaces.linked_object = Loops.linked_object
    //         AND Loops.id = LoopRanges.loop
    //       WHERE AddressSpaces.thread = <thread.dm_entry>
    //         AND LoopRanges.loop = <dm_entry>;
    //
    //       However the implementation below, combined with indices on
    //       AddressSpaces(linked_object, thread) & LoopRanges(loop), was
    //       found to be quite a bit faster.
    
    // Find our linked object and address ranges with associated bitmaps
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
        "SELECT Loops.linked_object, "
        "       LoopRanges.addr_begin, "
        "       LoopRanges.addr_end, "
        "       LoopRanges.valid_bitmap "
        "FROM LoopRanges "
        "  JOIN Loops "
        "ON LoopRanges.loop = Loops.id "
        "WHERE Loops.id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
        int linked_object = dm_database->getResultAsInteger(1);
        std::set<AddressRange> ranges =
            AddressBitmap(AddressRange(dm_database->getResultAsAddress(2),
                                       dm_database->getResultAsAddress(3)),
                          dm_database->getResultAsBlob(4)).
            getContiguousRanges(true);
        
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
            
            // Iterate over the addresss ranges for this loop
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
 * Returns the linked object containing this loop.
 *
 * @return    Linked object containing this loop.
 */
LinkedObject Loop::getLinkedObject() const
{
    LinkedObject linked_object;
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM Loops WHERE id = ?;"
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
 * Returns the definitions of this loop. An empty set is returned if no
 * definitions of this loop are found.
 *
 * @return    Definitions of this loop.
 */
std::set<Statement> Loop::getDefinitions() const
{
    // Find our linked object and extent
    LinkedObject linked_object;
    ExtentGroup extent;
    getLinkedObjectAndExtent(linked_object, extent);

    // Find our head address
    Address head;
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT addr_head FROM Loops WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        head = dm_database->getResultAsAddress(1);
    END_TRANSACTION(dm_database);

    //std::cerr << "Loop::getDefinitions(), head=" << head << std::endl;

    // Use the loop's head address as its extent
    Extent first(TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                 AddressRange(head));
    extent.clear();
    extent.push_back(first);
    
    // Use the statement cache to find our definitions
    std::set<Statement> definitions =
        Statement::TheCache.getStatements(linked_object, extent);
    
    // Return the definitions to the caller
    return definitions;
}



/**
 * Get our functions.
 *
 * Returns the functions containing this loop. An empty set is returned if no
 * function contains this loop.
 *
 * @return    Functions containing this loop.
 */
std::set<Function> Loop::getFunctions() const
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
 * Returns the statements associated with this loop. An empty set is returned
 * if no statements are associated with this loop.
 *
 * @return    Statements associated with this loop.
 */
std::set<Statement> Loop::getStatements() const
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



/** Loop cache. */
LoopCache Loop::TheCache;



/**
 * Default constructor.
 *
 * Constructs a Loop that refers to a non-existent loop. Any use of a member
 * function on an object constructed in this way will result in an assertion
 * failure.
 */
Loop::Loop() :
    Entry()
{
}



/**
 * Constructor from a loop entry.
 *
 * Constructs a new Loop for the specified loop entry.
 *
 * @param database    Database containing this loop.
 * @param entry       Identifier for this loop.
 */
Loop::Loop(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, Entry::Loops, entry)
{
}



/**
 * Get our linked object and extent.
 *
 * Returns the linked object containing, and extent of, this loop.
 *
 * @retval linked_object    Linked object containing this loop.
 * @retval extent           Extent of this loop.
 */
void Loop::getLinkedObjectAndExtent(LinkedObject& linked_object,
                                    ExtentGroup& extent) const
{
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT Loops.linked_object, "
        "       LoopRanges.addr_begin, "
        "       LoopRanges.addr_end, "
        "       LoopRanges.valid_bitmap "
        "FROM LoopRanges "
        "  JOIN Loops "
        "ON LoopRanges.loop = Loops.id "
        "WHERE Loops.id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
        linked_object = LinkedObject(dm_database,
                                     dm_database->getResultAsInteger(1));
        
        std::set<AddressRange> ranges =
            AddressBitmap(AddressRange(dm_database->getResultAsAddress(2),
                                       dm_database->getResultAsAddress(3)),
                          dm_database->getResultAsBlob(4)).
            getContiguousRanges(true);
        
        for(std::set<AddressRange>::const_iterator
                i = ranges.begin(); i != ranges.end(); ++i)
            extent.push_back(
                Extent(TimeInterval(Time::TheBeginning(), Time::TheEnd()), *i)
                );
        
    }
    END_TRANSACTION(dm_database);
}
