////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2018 Krell Institute. All Rights Reserved.
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
 * Definition of the InlineFunction class.
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
#include "Path.hxx"
#include "InlineFunction.hxx"
#include "InlineFunctionCache.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our threads.
 *
 * Returns the threads containing this inline function.
 *
 * @return    Threads containing this inline function.
 */
std::set<Thread> InlineFunction::getThreads() const
{
    std::set<Thread> threads;

    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.thread
    //       FROM AddressSpaces
    //         JOIN Statements
    //       ON AddressSpaces.linked_object = Statements.linked_object
    //       WHERE Statements.id = <dm_entry>;
    //
    //       However the implementation below, combined with an index on
    //       AddressSpaces(linked_object), was found to be quite a bit faster.
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM InlinedFunctions WHERE InlinedFunctions.id = ?;"
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
                        "<InlineFunction-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our extent in a thread.
 *
 * Returns the extent of this inline function within the specified thread. An
 * empty extent is returned if this inline function isn't present within the
 * specified thread.
 *
 * @pre    The thread must be in the same experiment as the inline function. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the inline function.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this inline function in that thread.
 */
ExtentGroup InlineFunction::getExtentIn(const Thread& thread) const
{
    ExtentGroup extent;

    // Check assertions
    Assert(inSameDatabase(thread));
    
    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.time_begin,
    //              AddressSpaces.time_end,
    //	            AddressSpaces.addr_begin,
    //              StatementRanges.addr_begin,
    //              StatementRanges.addr_end,
    //              StatementRanges.valid_bitmap
    //       FROM AddressSpaces
    //         JOIN Statements
    //         JOIN StatementRanges
    //       ON AddressSpaces.linked_object = Statements.linked_object
    //         AND Statements.id = StatementRanges.statement
    //       WHERE AddressSpaces.thread = <thread.dm_entry>
    //         AND StatementRanges.statement = <dm_entry>;
    //
    //       However the implementation below, combined with indices on
    //       AddressSpaces(linked_object, thread) & StatementRanges(statement),
    //       was found to be quite a bit faster.

    // Find our linked object and address ranges with associated bitmaps
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
        "SELECT InlinedFunctions.linked_object, "
        "       InlinedFunctionsRanges.addr_begin, "
        "       InlinedFunctionsRanges.addr_end, "
        "       InlinedFunctionsRanges.valid_bitmap "
        "FROM InlinedFunctionsRanges "
        "  JOIN InlinedFunctions "
        "ON InlinedFunctionsRanges.inline = InlinedFunctions.id "
        "WHERE InlinedFunctions.id = ?;"
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
            
            // Iterate over the addresss ranges for this inline
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
 * Returns the linked object containing this inline function.
 *
 * @return    Linked object containing this inline function.
 */
LinkedObject InlineFunction::getLinkedObject() const
{
    LinkedObject linked_object;

    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM InlinedFunctions WHERE id = ?;"
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
 * Get our inlined function name.
 *
 * Returns the inlined function name.
 *
 * @return    inlined function name.
 */
std::string InlineFunction::getName() const
{
    std::string functionname;
    BEGIN_TRANSACTION(dm_database);
    validate();
#if 1
    dm_database->prepareStatement(
        "SELECT name FROM InlinedFunctions WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        functionname = dm_database->getResultAsString(1);
#else
    // Ideally we could put the function name into Functions
    // table and the retrieve that using the same scheme we
    // use to put the file path into Files table.
    // But that alas does not seem to work. (require the
    // InlineFunctions table entry name be INTEGER.
    dm_database->prepareStatement(
        "SELECT Functions.name "
        "FROM InlinedFunctions "
        "  JOIN Functions "
        "ON InlinedFunctions.name = Functions.id "
        "WHERE InlinedFunctions.id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        functionname = dm_database->getResultAsString(1);
    }    
#endif
    END_TRANSACTION(dm_database);

    // Return the full path name to the caller
    return functionname;
}



/**
 * Get our path.
 *
 * Returns the full path name of this inline function's source file.
 *
 * @return    Full path name of this inline function's source file.
 */
Path InlineFunction::getPath() const
{
    Path path;

    // Find our source file's path
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT Files.path "
        "FROM InlinedFunctions "
        "  JOIN Files "
        "ON InlinedFunctions.file = Files.id "
        "WHERE InlinedFunctions.id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        if(!path.empty())
            throw Exception(Exception::EntryNotUnique, "Files",
                            "<InlinedFunctions-Referenced>");
        path = Path(dm_database->getResultAsString(1));
    }    
    if(path.empty()) {
#if 0
        throw Exception(Exception::EntryNotFound, "Files",
                        "<InlinedFunctions-Referenced>");
#endif
	path = "Unknown_File";
    }
    END_TRANSACTION(dm_database);
    
    // Return the full path name to the caller
    return path;
}



/**
 * Get our line number.
 *
 * Returns the line number of this inline function.
 *
 * @return    Line number of this inline function.
 */
int InlineFunction::getLine() const
{
    int line = 0;

    // Find our line number
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT line FROM InlinedFunctions WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        line = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the line number to the caller
    return line;
}



/**
 * Get our column number.
 *
 * Returns the column number of this inline function.
 *
 * @return    Column number of this inline function.
 */
int InlineFunction::getColumn() const
{
    int column = 0;

    // Find our column number
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT \"column\" FROM InlinedFunctions WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        column = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the column number to the caller
    return column;
}



/**
 * Get our functions.
 *
 * Returns the functions containing this statement. An empty set is returned
 * if no function contains this statement.
 *
 * @return    Functions containing this statement.
 */
std::set<Function> InlineFunction::getFunctions() const
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



/** Statement cache. */
InlineFunctionCache InlineFunction::TheCache;



/**
 * Default constructor.
 *
 * Constructs a InlineFunction that refers to a non-existent inline function.
 * Any use of a member function on an object constructed in this way will
 * result in an assertion failure.
 */
InlineFunction::InlineFunction() :
    Entry()
{
}



/**
 * Constructor from a inline entry.
 *
 * Constructs a new InlineFunction for the specified inline entry.
 *
 * @param database    Database containing this inline function.
 * @param entry       Identifier for this inline function.
 */
InlineFunction::InlineFunction(const SmartPtr<Database>& database, const int& entry):
    Entry(database, Entry::InlineFunctions, entry)
{
}



/**
 * Get our linked object and extent.
 *
 * Returns the linked object containing, and extent of, this inline.
 *
 * @retval linked_object    Linked object containing this inline.
 * @retval extent           Extent of this inline.
 */
void InlineFunction::getLinkedObjectAndExtent(LinkedObject& linked_object,
                                         ExtentGroup& extent) const
{
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT InlinedFunctions.linked_object, "
        "       InlinedFunctionsRanges.addr_begin, "
        "       InlinedFunctionsRanges.addr_end, "
        "       InlinedFunctionsRanges.valid_bitmap "
        "FROM InlinedFunctionsRanges "
        "  JOIN InlinedFunctions "
        "ON InlinedFunctionsRanges.inline = InlinedFunctions.id "
        "WHERE InlinedFunctions.id = ?;"
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
