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
 * Definition of the LinkedObject class.
 *
 */

#include "Assert.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "ExtentGroup.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Loop.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our threads.
 *
 * Returns the threads containing this linked object.
 *
 * @return    Threads containing this linked object.
 */
std::set<Thread> LinkedObject::getThreads() const
{
    std::set<Thread> threads;

    // Find the threads containing this linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT thread FROM AddressSpaces WHERE linked_object = ?;"
        );    
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    if(threads.empty())
        throw Exception(Exception::EntryNotFound, "Threads",
                        "<LinkedObjects-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our extent in a thread.
 *
 * Returns the extent of this linked object within the specified thread.
 * An empty extent is returned if this linked object isn't present within
 * the specified thread.
 *
 * @pre    The thread must be in the same experiment as the linked object. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the linked object.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this linked object in that thread.
 */
ExtentGroup LinkedObject::getExtentIn(const Thread& thread) const
{
    ExtentGroup extent;

    // Check assertions
    Assert(inSameDatabase(thread));

    // Find our extent in the specified thread
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
        "SELECT time_begin, "
        "       time_end, "
        "       addr_begin, "
        "       addr_end "
        "FROM AddressSpaces "
        "WHERE thread = ? "
        "  AND linked_object = ?;"
        );    
    dm_database->bindArgument(1, EntrySpy(thread).getEntry());
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement())
        extent.push_back(
            Extent(TimeInterval(dm_database->getResultAsTime(1),
                                dm_database->getResultAsTime(2)),
                   AddressRange(dm_database->getResultAsAddress(3),
                                dm_database->getResultAsAddress(4)))
            );
    END_TRANSACTION(dm_database);
    
    // Return the extent to the caller    
    return extent;
}



/**
 * Get our path.
 *
 * Returns the full path name of this linked object.
 *
 * @return    Full path name of this linked object.
 */
Path LinkedObject::getPath() const
{
    Path path;

    // Find our full path name
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT Files.path "
        "FROM LinkedObjects "
        "  JOIN Files "
        "ON LinkedObjects.file = Files.id "
        "WHERE LinkedObjects.id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        if(!path.empty())
            throw Exception(Exception::EntryNotUnique, "Files",
                            "<LinkedObjects-Referenced>");
        path = Path(dm_database->getResultAsString(1));
    }
    if(path.empty())
        throw Exception(Exception::EntryNotFound, "Files",
                        "<LinkedObjects-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the full path name to the caller
    return path;
}



/**
 * Test if we are an executable.
 *
 * Returns a boolean value indicating if this linked object is an executable or
 * not (i.e. a shared library). This does not necessarily correlate one-to-one
 * with the "executable" bit in the file system. It means, instead, that this
 * linked object was the "a.out" for one of the processes in the experiment.
 *
 * @return    Boolean "true" if this linked object is an executable,
 *            "false" otherwise.
 */
bool LinkedObject::isExecutable() const
{
    bool is_executable;

    // Find if we are an executable
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT is_executable FROM LinkedObjects WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        is_executable = dm_database->getResultAsInteger(1) != 0;
    END_TRANSACTION(dm_database);
    
    // Return the flag to the caller
    return is_executable;
}



/**
 * Get our functions.
 *
 * Returns the functions contained within this linked object. An empty set is
 * returned if no functions are found within this linked object.
 *
 * @return    Functions contained within this linked object.
 */
std::set<Function> LinkedObject::getFunctions() const
{
    std::set<Function> functions;
    
    // Find our functions
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT id FROM Functions WHERE linked_object = ?;"
        );	
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        functions.insert(Function(dm_database, 
                                  dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the functions to the caller
    return functions;
}



/**
 * Get our loops.
 *
 * Returns the loops contained within this linked object. An empty set is
 * returned if no loops are found within this linked object.
 *
 * @return    Loops contained within this linked object.
 */
std::set<Loop> LinkedObject::getLoops() const
{
    std::set<Loop> loops;
    
    // Find our loops
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT id FROM Loops WHERE linked_object = ?;"
        );	
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        loops.insert(Loop(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the Loops to the caller
    return loops;
}



/**
 * Get our statements.
 *
 * Returns the statements contained within this linked object. An empty set is
 * returned if no statements are found within this linked object.
 *
 * @return    Statements contained within this linked object.
 */
std::set<Statement> LinkedObject::getStatements() const
{
    std::set<Statement> statements;

    // Find our statements
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT id FROM Statements WHERE linked_object = ?;"
        );	
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        statements.insert(Statement(dm_database,
                                    dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the statements to the caller
    return statements;
}



/**
 * Used by the Offline Experiment BFDSymbols code to find the address range
 * of a linked object in the database. This is used to restrict the search
 * of performance sample addresses to those found in the linked object being
 * processed.
 */
std::set<AddressRange> LinkedObject::getAddressRange() const
{
    std::set<AddressRange> addressrange;

    // Find the linked object containing the requested address/time
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT addr_begin, "
        "       addr_end "
        "FROM AddressSpaces WHERE linked_object = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        Address addr_b = dm_database->getResultAsAddress(1);
        Address addr_e = dm_database->getResultAsAddress(2);
        addressrange.insert(AddressRange(addr_b,addr_e));
        if (addressrange.empty())
            throw Exception(Exception::EntryNotFound, "Address",
                            "<LinkedObjects-Referenced>");
    }
    END_TRANSACTION(dm_database);
    
    // Return the addresses to the caller
    return addressrange;
}



/**
 * Default constructor.
 *
 * Constructs a LinkedObject that refers to a non-existent linked object. Any
 * use of a member function on an object constructed in this way will result in
 * an assertion failure.
 */
LinkedObject::LinkedObject() :
    Entry()
{
}



/**
 * Constructor from a linked object entry.
 *
 * Constructs a new LinkedObject for the specified linked object entry.
 *
 * @param database    Database containing this linked object.
 * @param entry       Identifier for this linked object.
 */
LinkedObject::LinkedObject(const SmartPtr<Database>& database,
                           const int& entry) :
    Entry(database, Entry::LinkedObjects, entry)
{
}
