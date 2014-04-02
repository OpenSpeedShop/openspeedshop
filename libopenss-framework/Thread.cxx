////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007-2014 The Krell Institute. All Rights Reserved.
// Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the Thread class.
 *
 */

#include "AddressBitmap.hxx"
#include "CollectorGroup.hxx"
#include "Function.hxx"
#include "Instrumentor.hxx"
#include "LinkedObject.hxx"
#include "Loop.hxx"
#include "Path.hxx"
#include "Statement.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our state.
 *
 * Returns the current state of this thread. Since this state changes
 * asynchronously and may be updated across a network, there is a lag between
 * when the actual thread's state changes and when that is reflected here.
 *
 * @return    Current state of this thread.
 */
Thread::State Thread::getState() const
{
#if (BUILD_INSTRUMENTOR == 1)
    return Instrumentor::getState(*this);
#else
    return Thread::Terminated;
#endif
}



/**
 * Test our state.
 *
 * Compares this thread's current state against the passed value. Returns a
 * boolean value indicating if the thread was in that state or not. Really just
 * a convenience function, built directly on getState(), for use when simpler
 * conditional statements are desired.
 *
 * @param state    State to compare against.
 * @return         Boolean "true" if this thread was in the passed state,
 *                 "false" otherwise.
 */
bool Thread::isState(const State& state) const
{
    return getState() == state;
}



/**
 * Change our state.
 *
 * Changes the current state of this thread to the passed value. Used to, for
 * example, suspend a thread that was previously running. This function does not
 * wait until the thread has actually completed the state change, and calling
 * getState() immediately following changeState() will not reflect the new state
 * until the change has actually completed.
 *
 * @param state    Change to this state.
 */
void Thread::changeState(const State& state) const
{
    ThreadGroup threads;
    threads.insert(*this);
    Instrumentor::changeState(threads, state);
}



/**
 * Get our MPI Implementation.
 *
 * Returns the MPI Implementation for this thread. If this thread wasn't
 * created directly by the framework, the first value in the pair returned 
 * will be "false".
 *
 * @return    Pair containing MPI Implementation used to create this thread.
 */
std::pair<bool, std::string> Thread::getMPIImplementation() const
{
    std::pair<bool, std::string> mpi_impl(false, "");

    // Find our MPI Implementation
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT mpi_impl FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    mpi_impl = std::make_pair(true,
				     dm_database->getResultAsString(1));
    END_TRANSACTION(dm_database);

    // Return the MPI Implementation to the caller
    return mpi_impl;
}

/**
 *
 * Returns the command used to create this thread. If this thread wasn't
 * created directly by the framework, the first value in the pair returned 
 * will be "false".
 *
 * @return    Pair containing command used to create this thread.
 */
std::pair<bool, std::string> Thread::getCommand() const
{
    std::pair<bool, std::string> command(false, "");

    // Find our command
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT command FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    command = std::make_pair(true,
				     dm_database->getResultAsString(1));
    END_TRANSACTION(dm_database);

    // Return the command to the caller
    return command;
}



/**
 * Get our host name.
 *
 * Returns the name of the host on which this thread is located.
 *
 * @return    Name of host on which this thread is located.
 */
std::string Thread::getHost() const
{
    std::string host;

    // Find our host name
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT host FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	host = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the host name to the caller
    return host;
}



/**
 * Get our process identifier.
 *
 * Returns the identifier of the process containing this thread.
 *
 * @return    Identifier of process containing this thread.
 */
pid_t Thread::getProcessId() const
{
    pid_t pid;

    // Find our process identifier
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT pid FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	pid = static_cast<pid_t>(dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the process identifier to the caller
    return pid;
}



/**
 * Get our POSIX thread identifier.
 *
 * Returns the POSIX identifier of this thread. If this thread isn't a POSIX
 * thread, the first value in the pair returned will be "false".
 *
 * @return    Pair containing identifier of this thread.
 */
std::pair<bool, pthread_t> Thread::getPosixThreadId() const
{
    std::pair<bool, pthread_t> tid(false, 0);
    
    // Find our POSIX thread identifier	
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT posix_tid FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    tid = std::make_pair(true, 
				 dm_database->getResultAsPosixThreadId(1));
    END_TRANSACTION(dm_database);
    
    // Return the POSIX thread identifier to the caller
    return tid;
}



/**
 * Get our OpenMP thread identifier.
 *
 * Returns the OpenMP identifier of this thread. If this thread isn't an OpenMP
 * thread, the first value in the pair returned will be "false".
 *
 * @return    Pair containing identifier of this thread.
 */
std::pair<bool, int> Thread::getOpenMPThreadId() const
{
    std::pair<bool, int> tid(false, 0);
    
    // Find our OpenMP thread identifier	
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT openmp_tid FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    tid = std::make_pair(true, dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the OpenMP thread identifier to the caller
    return tid;
}



/**
 * Get our MPI rank.
 *
 * Returns the MPI rank of this thread within the application's world
 * communicator. If this thread isn't part of an MPI application, the
 * first value in the pair returned will be "false".
 *
 * @return    Pair containing rank of this thread.
 */
std::pair<bool, int> Thread::getMPIRank() const
{
    std::pair<bool, int> rank(false, 0);
    
    // Find our MPI rank
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT mpi_rank FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    rank = std::make_pair(true, dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the MPI rank to the caller
    return rank;
}



/**
 * Get our linked objects.
 *
 * Returns the linked objects contained within this thread. An empty set is
 * returned if no linked objects are found.
 *
 * @return    Linked objects contained within this thread.
 */
std::set<LinkedObject> Thread::getLinkedObjects() const
{
    std::set<LinkedObject> linked_objects;
    
    // Find our linked objects
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM AddressSpaces WHERE thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	linked_objects.insert(LinkedObject(dm_database,
					   dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the linked objects to the caller
    return linked_objects;
}



/**
 * Get our functions.
 *
 * Returns the functions contained within this thread. An empty set is returned
 * if no functions are found.
 *
 * @return    Functions contained within this thread.
 */
std::set<Function> Thread::getFunctions() const
{
    std::set<Function> functions;

    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT Functions.id
    //       FROM Functions
    //         JOIN AddressSpaces
    //       ON Functions.linked_object = AddressSpaces.linked_object
    //       WHERE AddressSpaces.thread = <dm_entry>
    //
    //       However the implementation below was found to be quite a bit
    //       faster.

    // Find our linked objects
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM AddressSpaces WHERE thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	int linked_object = dm_database->getResultAsInteger(1);
	
	// Find all the functions in this linked object
	dm_database->prepareStatement(
	    "SELECT id FROM Functions WHERE linked_object = ?;"
	    );
	dm_database->bindArgument(1, linked_object);
	while(dm_database->executeStatement())
	    functions.insert(Function(dm_database,
				      dm_database->getResultAsInteger(1)));

    }
    END_TRANSACTION(dm_database);

    // Return the functions to the caller
    return functions;
}



/**
 * Get our loops.
 *
 * Returns the loops contained within this thread. An empty set is returned if
 * no loops are found.
 *
 * @return    Loops contained within this thread.
 */
std::set<Loop> Thread::getLoops() const
{
    std::set<Loop> loops;

    // Find our linked objects
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM AddressSpaces WHERE thread = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
        
        int linked_object = dm_database->getResultAsInteger(1);
        
        // Find all the loops in this linked object
        dm_database->prepareStatement(
            "SELECT id FROM Loops WHERE linked_object = ?;"
            );
        dm_database->bindArgument(1, linked_object);
        while(dm_database->executeStatement())
            loops.insert(Loop(dm_database, dm_database->getResultAsInteger(1)));
        
    }
    END_TRANSACTION(dm_database);

    //std::cerr << "DEBUG FRAMEWORK: Exit std::set<Loop> Thread::getLoops() const, loops.size()=" << loops.size() << std::endl;
    
    // Return the loops to the caller
    return loops;
}



/**
 * Get our statements.
 *
 * Returns the statements contained within this thread. An empty set is returned
 * if no statements are found.
 *
 * @return    Statements contained within this thread.
 */
std::set<Statement> Thread::getStatements() const
{
    std::set<Statement> statements;

    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT Statements.id
    //       FROM Statements
    //         JOIN AddressSpaces
    //       ON Statements.linked_object = AddressSpaces.linked_object
    //       WHERE AddressSpaces.thread = <dm_entry>
    //
    //       However the implementation below was found to be quite a bit
    //       faster.

    // Find our linked objects
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM AddressSpaces WHERE thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	int linked_object = dm_database->getResultAsInteger(1);
	
	// Find all the statements in this linked object
	dm_database->prepareStatement(
	    "SELECT id FROM Statements WHERE linked_object = ?;"
	    );
	dm_database->bindArgument(1, linked_object);
	while(dm_database->executeStatement())
	    statements.insert(Statement(dm_database,
					dm_database->getResultAsInteger(1)));
	
    }
    END_TRANSACTION(dm_database);

    // Return the statements to the caller
    return statements;
}



/**
 * Get the linked object at an address.
 *
 * Returns the linked object containing the specified address at a particular
 * moment in time. If a linked object cannot be found, the first value in the
 * pair returned will be "false".
 *
 * @param address    Address to be found.
 * @param time       Time at which to find this address.
 * @return           Linked object containing this address.
 */
std::pair<bool, LinkedObject> 
Thread::getLinkedObjectAt(const Address& address, const Time& time) const
{
    std::pair<bool, LinkedObject> linked_object(false, LinkedObject());

    // Find the linked object containing the requested address/time
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object "
	"FROM AddressSpaces "
	"WHERE thread = ? "
	"  AND ? >= time_begin "
	"  AND ? < time_end "
	"  AND ? >= addr_begin "
	"  AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);	
    while(dm_database->executeStatement()) {
	if(linked_object.first)
	    throw Exception(Exception::EntryOverlapping, "AddressSpaces");
	linked_object.first = true;
	linked_object.second = LinkedObject(dm_database,
					    dm_database->getResultAsInteger(1));
    }
    END_TRANSACTION(dm_database);
    
    // Return the linked object to the caller
    return linked_object;
}



/**
 * Get the function at an address.
 *
 * Returns the function containing the specified address at a particular moment
 * in time. If a function cannot be found, the first value in the pair returned
 * will be "false".
 *
 * @param address    Address to be found.
 * @param time       Time at which to find this address.
 * @return           Function containing this address.
 */
std::pair<bool, Function> 
Thread::getFunctionAt(const Address& address, const Time& time) const
{
    std::pair<bool, Function> function(false, Function());

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate();
    
    // Find the linked object containing the requested address/time
    bool found_linked_object = false; 
    int linked_object = 0;
    Address addr_begin = 0;    
    dm_database->prepareStatement(
	"SELECT linked_object, "
	"       addr_begin "
	"FROM AddressSpaces "
	"WHERE thread = ? "
	"  AND ? >= time_begin "
	"  AND ? < time_end "
	"  AND ? >= addr_begin "
	"  AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);	
    while(dm_database->executeStatement()) {
#if 0
	if(found_linked_object)
	    throw Exception(Exception::EntryOverlapping, "AddressSpaces");
#else
	if(found_linked_object) break;
#endif
	found_linked_object = true;
	linked_object = dm_database->getResultAsInteger(1);
	addr_begin = dm_database->getResultAsAddress(2);
    }

    // Did we find the linked object?
    if(found_linked_object) {

	// Find the function containing the requested address    
	dm_database->prepareStatement(
	    "SELECT Functions.id, "
	    "       FunctionRanges.addr_begin, "
	    "       FunctionRanges.addr_end, "
	    "       FunctionRanges.valid_bitmap "
	    "FROM FunctionRanges "
	    "  JOIN Functions "
	    "ON FunctionRanges.function = Functions.id "
	    "WHERE Functions.linked_object = ? "
	    "  AND ? >= FunctionRanges.addr_begin "
	    "  AND ? < FunctionRanges.addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - addr_begin));
	dm_database->bindArgument(3, Address(address - addr_begin));
	while(dm_database->executeStatement()) {

	    AddressBitmap bitmap(
		AddressRange(dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(3)),
		dm_database->getResultAsBlob(4)
		);
	    
	    if(bitmap.getValue(address - addr_begin)) {
// This is causing failures with some offline database views
// (calltrees,butterfly). Disabling the exception until we can
// figure out why some offline functions have overlapping entries.
#if 0
		if(function.first)
		    throw Exception(Exception::EntryOverlapping, "Functions");
#endif
		function.first = true;
		function.second = Function(dm_database,
					   dm_database->getResultAsInteger(1));
	    }
	    
	}
	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Return the function to the caller
    return function;
}



/**
 * Get the loops at an address.
 *
 * Returns the loops containing the specified address at a particular moment
 * in time. An empty set is returned if no loops are found.
 *
 * @param address    Address to be found.
 * @param time       Time at which to find this address.
 * @return           Loops containing this address.
 */
std::set<Loop> Thread::getLoopsAt(const Address& address,
                                  const Time& time) const
{
    std::set<Loop> loops;
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate();
    
    // Find the linked object containing the requested address/time
    bool found_linked_object = false; 
    int linked_object;
    Address addr_begin;    
    dm_database->prepareStatement(
        "SELECT linked_object, "
        "       addr_begin "
        "FROM AddressSpaces "
        "WHERE thread = ? "
        "  AND ? >= time_begin "
        "  AND ? < time_end "
        "  AND ? >= addr_begin "
        "  AND ? < addr_end;"
        );
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);	
    while(dm_database->executeStatement()) {
        if(found_linked_object)
            throw Exception(Exception::EntryOverlapping, "AddressSpaces");
        found_linked_object = true;
        linked_object = dm_database->getResultAsInteger(1);
        addr_begin = dm_database->getResultAsAddress(2);
    }
    
    // Did we find the linked object?
    if(found_linked_object) {
        
        // Find the loops containing the requested address    
        dm_database->prepareStatement(
            "SELECT Loops.id, "
            "       LoopRanges.addr_begin, "
            "       LoopRanges.addr_end, "
            "       LoopRanges.valid_bitmap "
            "FROM LoopRanges "
            "  JOIN Loops "
            "ON LoopRanges.loop = Loops.id "
            "WHERE Loops.linked_object = ? "
            "  AND ? >= LoopRanges.addr_begin "
            "  AND ? < LoopRanges.addr_end;"
            );
        dm_database->bindArgument(1, linked_object);
        dm_database->bindArgument(2, Address(address - addr_begin));
        dm_database->bindArgument(3, Address(address - addr_begin));
        while(dm_database->executeStatement()) {
            
            AddressBitmap bitmap(
                AddressRange(dm_database->getResultAsAddress(2),
                             dm_database->getResultAsAddress(3)),
                dm_database->getResultAsBlob(4)
                );
            
            if(bitmap.getValue(address - addr_begin))
                loops.insert(
                    Loop(dm_database, dm_database->getResultAsInteger(1))
                    );
            
        }
        
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
    
    // Return the loops to the caller
    return loops;
}



/**
 * Get the statements at an address.
 *
 * Returns the statements containing the specified address at a particular
 * moment in time. An empty set is returned if no statements are found.
 *
 * @param address    Address to be found.
 * @param time       Time at which to find this address.
 * @return           Statements containing this address.
 */
std::set<Statement>
Thread::getStatementsAt(const Address& address, const Time& time) const
{
    std::set<Statement> statements;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate();
    
    // Find the linked object containing the requested address/time
    bool found_linked_object = false; 
    int linked_object;
    Address addr_begin;    
    dm_database->prepareStatement(
	"SELECT linked_object, "
	"       addr_begin "
	"FROM AddressSpaces "
	"WHERE thread = ? "
	"  AND ? >= time_begin "
	"  AND ? < time_end "
	"  AND ? >= addr_begin "
	"  AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);	
    while(dm_database->executeStatement()) {
#if 0
	if(found_linked_object)
	    throw Exception(Exception::EntryOverlapping, "AddressSpaces");
#else
	if(found_linked_object) break;
#endif
	found_linked_object = true;
	linked_object = dm_database->getResultAsInteger(1);
	addr_begin = dm_database->getResultAsAddress(2);
    }

    // Did we find the linked object?
    if(found_linked_object) {

	// Find the statements containing the requested address    
	dm_database->prepareStatement(
	    "SELECT Statements.id, "
	    "       StatementRanges.addr_begin, "
	    "       StatementRanges.addr_end, "
	    "       StatementRanges.valid_bitmap "
	    "FROM StatementRanges "
	    "  JOIN Statements "
	    "ON StatementRanges.statement = Statements.id "
	    "WHERE Statements.linked_object = ? "
	    "  AND ? >= StatementRanges.addr_begin "
	    "  AND ? < StatementRanges.addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - addr_begin));
	dm_database->bindArgument(3, Address(address - addr_begin));
	while(dm_database->executeStatement()) {
	    
	    AddressBitmap bitmap(
		AddressRange(dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(3)),
		dm_database->getResultAsBlob(4)
		);
	    
	    if(bitmap.getValue(address - addr_begin))
		statements.insert(
		    Statement(dm_database,
			      dm_database->getResultAsInteger(1))
		    );
	    
	}

    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Return the statements to the caller
    return statements;
}



//
// LOOP TODO: getFunctionandStatementsAt() might need to be updated...
// 

// Used by Experiment::compressDB to prune an OpenSpeedShop database of
// any entries not found in the experiments sampled addresses.
std::pair<std::pair<bool, Function>, std::set<Statement> >
Thread::getFunctionAndStatementsAt(const Address& address) const
{
    std::pair<bool, Function> function(false, Function());
    std::set<Statement> statements;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate();
    
    // Find the linked object containing the requested address/time
    bool found_linked_object = false; 
    int linked_object = 0;
    Address addr_begin = 0;    
    dm_database->prepareStatement(
	"SELECT linked_object, "
	"       addr_begin "
	"FROM AddressSpaces "
	"WHERE thread = ? "
	"  AND ? >= addr_begin "
	"  AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, address);
    dm_database->bindArgument(3, address);	
    while(dm_database->executeStatement()) {
	if(found_linked_object)
	    throw Exception(Exception::EntryOverlapping, "AddressSpaces");
	found_linked_object = true;
	linked_object = dm_database->getResultAsInteger(1);
	addr_begin = dm_database->getResultAsAddress(2);
    }

    // Did we find the linked object?
    if(found_linked_object) {

	// Find the function containing the requested address    
	dm_database->prepareStatement(
	    "SELECT Functions.id, "
	    "       FunctionRanges.addr_begin, "
	    "       FunctionRanges.addr_end, "
	    "       FunctionRanges.valid_bitmap "
	    "FROM FunctionRanges "
	    "  JOIN Functions "
	    "ON FunctionRanges.function = Functions.id "
	    "WHERE Functions.linked_object = ? "
	    "  AND ? >= FunctionRanges.addr_begin "
	    "  AND ? < FunctionRanges.addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - addr_begin));
	dm_database->bindArgument(3, Address(address - addr_begin));
	while(dm_database->executeStatement()) {

	    AddressBitmap bitmap(
		AddressRange(dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(3)),
		dm_database->getResultAsBlob(4)
		);
	    
	    if(bitmap.getValue(address - addr_begin)) {
		if(function.first)
		    throw Exception(Exception::EntryOverlapping, "Functions");
		function.first = true;
		function.second = Function(dm_database,
					   dm_database->getResultAsInteger(1));
	    }
	    
	}
	
	dm_database->prepareStatement(
	    "SELECT Statements.id, "
	    "       StatementRanges.addr_begin, "
	    "       StatementRanges.addr_end, "
	    "       StatementRanges.valid_bitmap "
	    "FROM StatementRanges "
	    "  JOIN Statements "
	    "ON StatementRanges.statement = Statements.id "
	    "WHERE Statements.linked_object = ? "
	    "  AND ? >= StatementRanges.addr_begin "
	    "  AND ? < StatementRanges.addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - addr_begin));
	dm_database->bindArgument(3, Address(address - addr_begin));
	while(dm_database->executeStatement()) {
	    
	    AddressBitmap bitmap(
		AddressRange(dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(3)),
		dm_database->getResultAsBlob(4)
		);
	    
	    if(bitmap.getValue(address - addr_begin))
		statements.insert(
		    Statement(dm_database,
			      dm_database->getResultAsInteger(1))
		    );

	}

    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Return the function and statements to the caller
    return std::make_pair(function, statements);
}



/**
 * Get our executable.
 *
 * Returns the linked object which is this thread's executable at a particular
 * moment in time. If the executable can't be found, the first value in the pair
 * returned will be "false".
 *
 * @param time    Time at which to find the thread's executable.
 * @return        Pair containing executable of this thread.
 */
std::pair<bool, LinkedObject> Thread::getExecutable(const Time& time) const
{
    std::pair<bool, LinkedObject> executable(false, LinkedObject());

    // Find our executable at the specified time
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT LinkedObjects.id "
	"FROM AddressSpaces "
	"  JOIN LinkedObjects "
	"ON AddressSpaces.linked_object = LinkedObjects.id "
	"WHERE AddressSpaces.thread = ? "
	"  AND ? >= AddressSpaces.time_begin "
	"  AND ? < AddressSpaces.time_end "
	"  AND LinkedObjects.is_executable = 1;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    while(dm_database->executeStatement()) {
	if(executable.first) {
#if defined(OPENSS_STRICT_EXCEPTIONS)
	    throw Exception(Exception::EntryNotUnique, "LinkedObjects",
                            "<Threads-Referenced-Executable>");
#else
	    break;
#endif
	}
	executable = std::make_pair(
	    true, 
	    LinkedObject(dm_database, dm_database->getResultAsInteger(1))
	    );
    }
    END_TRANSACTION(dm_database);
    
    // Return the executable to the caller
    return executable;
}



/**
 * Get a function by name.
 *
 * Returns the function with the passed name. If the function cannot be found,
 * the first value in the pair returned will be "false". If more than one
 * function is found, the first one encountered is returned.
 *
 * @param name    Name of the function to find.
 * @return        Pair containing function with this name.
 */
std::pair<bool, Function>
Thread::getFunctionByName(const std::string& name) const
{
    std::pair<bool, Function> function(false, Function());
    
    // Find the function with this name
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT Functions.id "
	"FROM Functions "
	"  JOIN AddressSpaces "
	"ON Functions.linked_object = AddressSpaces.linked_object "
	"WHERE AddressSpaces.thread = ? "
	"  AND Functions.name = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, name);
    while(dm_database->executeStatement())
	if(!function.first)
	    function = std::make_pair(
		true,
		Function(dm_database, dm_database->getResultAsInteger(1))
		);
    END_TRANSACTION(dm_database);
    
    // Return the function to the caller
    return function;
}



/**
 * Get loops by source file.
 *
 * Returns the loops in the passed source file. An empty set is returned if the
 * source file cannot be found.
 *
 * @param path    Source file for which to obtain loops.
 * @return        Loops in this source file.
 */
std::set<Loop> Thread::getLoopsBySourceFile(const Path& file) const
{
    std::set<Loop> loops;

    // Find our linked objects
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT linked_object FROM AddressSpaces WHERE thread = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {        
        int linked_object = dm_database->getResultAsInteger(1);
        
        // Find all the loops in this linked object
        dm_database->prepareStatement(
            "SELECT id, addr_head FROM Loops WHERE linked_object = ?;"
            );
        dm_database->bindArgument(1, linked_object);
        while(dm_database->executeStatement()) {
            int loop = dm_database->getResultAsInteger(1);
            Address addr_head = dm_database->getResultAsAddress(2);

            // Find the statements containing this loop's head address
            dm_database->prepareStatement(
                "SELECT StatementRanges.addr_begin, "
                "       StatementRanges.addr_end, "
                "       StatementRanges.valid_bitmap "
                "FROM StatementRanges "
                "  JOIN Statements "
                "  JOIN Files "
                "ON StatementRanges.statement = Statements.id "
                "  AND Statements.file = Files.id"
                "WHERE Statements.linked_object = ? "
                "  AND Files.path = ? "
                "  AND ? >= StatementRanges.addr_begin "
                "  AND ? < StatementRanges.addr_end;"
                );
            dm_database->bindArgument(1, linked_object);
            dm_database->bindArgument(2, file);
            dm_database->bindArgument(3, addr_head);
            dm_database->bindArgument(4, addr_head);
            while(dm_database->executeStatement()) {
                
                AddressBitmap bitmap(
                    AddressRange(dm_database->getResultAsAddress(1),
                                 dm_database->getResultAsAddress(2)),
                    dm_database->getResultAsBlob(3)
                    );
                
                if(bitmap.getValue(addr_head))
                    loops.insert(Loop(dm_database, loop));
            }
            
        }
        
    }
    END_TRANSACTION(dm_database);
    
    // Return the loops to the caller
    return loops;
}



/**
 * Get statements by source file.
 *
 * Returns the statements in the passed source file. An empty set is returned
 * if the source file cannot be found.
 *
 * @param path    Source file for which to obtain statements.
 * @return        Statements in this source file.
 */
std::set<Statement>
Thread::getStatementsBySourceFile(const Path& file) const
{
    std::set<Statement> statements;
    
    // Find the statements contained within this source file
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT Statements.id "
	"FROM Files "
	"  JOIN Statements "
	"  JOIN AddressSpaces "
	"ON Files.id = Statements.file "
	"  AND Statements.linked_object = AddressSpaces.linked_object "	
	"WHERE AddressSpaces.thread = ? "
	"  AND Files.path = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, file);
    while(dm_database->executeStatement())
	statements.insert(Statement(dm_database,
				    dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the statements to the caller
    return statements;
}



/**
 * Get our collectors.
 *
 * Returns all the collectors currently collecting performance data for this
 * thread. An empty collector group is returned if no collectors are collecting
 * performance data for this thread.
 *
 * @return    Collectors which are collecting performance data for this thread.
 */
CollectorGroup Thread::getCollectors() const
{
    CollectorGroup collectors;
    
    // Find the collectors which are collecting for us
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT collector "
	"FROM Collecting "
	"WHERE thread = ? "
	"  AND is_postponed = 0;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        collectors.insert(Collector(dm_database,
				    dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the collectors to the caller
    return collectors;
}



/**
 * Get our postponed collectors.
 *
 * Returns all the collectors currently postponing collection of performance
 * data for this thread. An empty collector group is returned if no collectors
 * are postponing collection of performance data for this thread.
 *
 * @return    Collectors which are postponing collection of performance data
 *            for this thread.
 */
CollectorGroup Thread::getPostponedCollectors() const
{
    CollectorGroup collectors;

    // Find the collectors which are postponing collection for us
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT collector "
	"FROM Collecting "
	"WHERE thread = ? "
	"  AND is_postponed = 1;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        collectors.insert(Collector(dm_database,
				    dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the collectors to the caller
    return collectors;   
}



/**
 * Default constructor.
 *
 * Constructs a Thread that refers to a non-existent thread. Any use of a
 * member function on an object constructed in this way will result in an
 * assertion failure.
 */
Thread::Thread() :
    Entry()
{
}



/**
 * Constructor from a thread entry.
 *
 * Constructs a new Thread for the specified thread entry.
 *
 * @param database    Database containing this thread.
 * @param entry       Identifier for this thread.
 */
Thread::Thread(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, Entry::Threads, entry)
{
}



/**
 * Does a sibling exist?
 *
 * Searches the database containing this thread for a sibling of this thread.
 * Returns a boolean flag indiciating if the sibling exists or not.
 *
 * @note    The term "sibling" here means another thread that is contained
 *          within the same process as this thread.
 *
 * @param tid    Identifier of the sibling to find.
 * @return       Boolean "true" if the sibling exists, "false" otherwise.
 */
bool Thread::doesSiblingExist(const pthread_t& tid) const
{
    bool exists = false;

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate();

    // Find our host and process identifier
    std::string host;
    pid_t pid;
    dm_database->prepareStatement(
	"SELECT host, pid FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	host = dm_database->getResultAsString(1);
	pid = static_cast<pid_t>(dm_database->getResultAsInteger(2));
    }	

    // Find the sibiling
    dm_database->prepareStatement(
	"SELECT id FROM Threads WHERE host = ? AND pid = ? AND posix_tid = ?;"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    dm_database->bindArgument(3, tid);
    while(dm_database->executeStatement())
	exists = true;

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
    
    // Return the existence flag to the caller
    return exists;
}




/**
 * Set our POSIX thread identifier.
 *
 * Sets the POSIX identifier of this thread.
 *
 * @param tid    Identifier of this thread.
 */
void Thread::setPosixThreadId(const pthread_t& tid) const
{
    // Set our POSIX thread identifier
    BEGIN_WRITE_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"UPDATE Threads SET posix_tid = ? WHERE id = ?;"
	);
    dm_database->bindArgument(1, tid);
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement());
    END_TRANSACTION(dm_database);
}


/**
 * Set our new command for the thread.
 *
 * Sets the command of this thread.
 *
 * @param tid    Command to be executed by this thread.
 */
void Thread::setCommand(const char *command) const
{
    // Set our application execute command
    BEGIN_WRITE_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "UPDATE Threads SET command = ? WHERE id = ?;"
        );
    dm_database->bindArgument(1, command);
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement());
    END_TRANSACTION(dm_database);
}



/**
 * Set our new MPI Implementation for the thread.
 *
 * Sets the MPI Implementation of this thread.
 *
 * @param tid    Command to be executed by this thread.
 */
void Thread::setMPIImplementation(std::string mpi_impl) const
{
    // Set our application execute MPI Implementation 
    BEGIN_WRITE_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"UPDATE Threads SET mpi_impl = ? WHERE id = ?;"
	);
    dm_database->bindArgument(1, mpi_impl);
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement());
    END_TRANSACTION(dm_database);
}



/**
 * Copy ourselves.
 * 
 * Constructs a copy of this thread within the database and returns a Thread
 * object for that copy.
 *
 * @return    Copy of this thread.
 */
Thread Thread::createCopy() const
{
    Thread copy;

    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(dm_database);
    validate();

    // Find our host, process identifier, etc.
    std::string host("");
    pid_t pid = 0;
    std::pair<bool, pthread_t> posix_tid(false, 0);
    std::pair<bool, int> openmp_tid(false, 0);
    std::pair<bool, int> mpi_rank(false, 0);
    dm_database->prepareStatement(
	"SELECT host, "
	"       pid, "
	"       posix_tid, "
	"       openmp_tid, "
	"       mpi_rank "
	"FROM Threads "
	"WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {
	host = dm_database->getResultAsString(1);
	pid = static_cast<pid_t>(dm_database->getResultAsInteger(2));
	if(!dm_database->getResultIsNull(3))
            posix_tid = 
		std::make_pair(true, dm_database->getResultAsPosixThreadId(3));
	if(!dm_database->getResultIsNull(4))
            openmp_tid = 
		std::make_pair(true, dm_database->getResultAsInteger(4));
	if(!dm_database->getResultIsNull(5))
            mpi_rank = std::make_pair(true, dm_database->getResultAsInteger(5));
    }

    // Create a copy of this thread
    dm_database->prepareStatement(
	"INSERT INTO Threads (host, pid) VALUES (?, ?);"
	);
    dm_database->bindArgument(1, host);
    dm_database->bindArgument(2, pid);
    while(dm_database->executeStatement());	
    copy = Thread(dm_database, dm_database->getLastInsertedUID());
    
    if(posix_tid.first) {
	dm_database->prepareStatement(
	    "UPDATE Threads SET posix_tid = ? WHERE id = ?;"
	    );
	dm_database->bindArgument(1, posix_tid.second);
	dm_database->bindArgument(2, copy.dm_entry);
	while(dm_database->executeStatement());	
    }
    
    if(openmp_tid.first) {
	dm_database->prepareStatement(
	    "UPDATE Threads SET openmp_tid = ? WHERE id = ?;"
	    );
	dm_database->bindArgument(1, openmp_tid.second);
	dm_database->bindArgument(2, copy.dm_entry);
	while(dm_database->executeStatement());	
    }

    if(mpi_rank.first) {
	dm_database->prepareStatement(
	    "UPDATE Threads SET mpi_rank = ? WHERE id = ?;"
	    );
	dm_database->bindArgument(1, mpi_rank.second);
	dm_database->bindArgument(2, copy.dm_entry);
	while(dm_database->executeStatement());	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
        
    // Return the copy to the caller
    return copy;
}



/**
 * Conversion from Thread::State to std::string.
 *
 * Returns the conversion of a Thread::State into a std::string. Simply returns
 * the string containing the textual representation of the passed thread state
 * enumeration value (e.g. "Running" for Thread::Running).
 *
 * @note    This cannot be implemented as a regular C++ type conversion
 *          operator because Thread::State is an enumeration rather than
 *          a class or structure.
 *
 * @param state    Thread state to be converted.
 * @return         String conversion of that thread state.
 */
std::string OpenSpeedShop::Framework::toString(const Thread::State& state)
{
    switch(state) {
    case Thread::Disconnected:
	return "Disconnected";
    case Thread::Connecting:
	return "Connecting";
    case Thread::Nonexistent:
	return "Nonexistent";
    case Thread::Running:
	return "Running";
    case Thread::Suspended:
	return "Suspended";
    case Thread::Terminated:
	return "Terminated";
    default:
	return "?";
    }
}
