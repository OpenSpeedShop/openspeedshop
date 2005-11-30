////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
#include "Path.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

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
    return Instrumentor::getState(*this);
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
    Instrumentor::changeState(*this, state);
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
	    tid = std::make_pair(true, static_cast<pthread_t>
				 (dm_database->getResultAsInteger(1)));
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

    // Find our functions
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT Functions.id "
	"FROM Functions "
	"  JOIN AddressSpaces "
	"ON Functions.linked_object = AddressSpaces.linked_object "
	"WHERE AddressSpaces.thread = ?;"
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

    // Find our statements
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT Statements.id "
	"FROM Statements "
	"  JOIN AddressSpaces "
	"ON Statements.linked_object = AddressSpaces.linked_object "
	"WHERE AddressSpaces.thread = ?;"
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

	// Find the function containing the requested address
	dm_database->prepareStatement(
	    "SELECT Functions.id "
	    "FROM Functions "
	    "WHERE Functions.linked_object = ? "
	    "  AND ? >= Functions.addr_begin "
	    "  AND ? < Functions.addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - addr_begin));
	dm_database->bindArgument(3, Address(address - addr_begin));
	while(dm_database->executeStatement()) {
	    if(function.first)
		throw Exception(Exception::EntryOverlapping, "Functions");
	    function.first = true;
	    function.second = Function(dm_database,
				       dm_database->getResultAsInteger(1));
	}
	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);

    // Return the function to the caller
    return function;
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
	if(found_linked_object)
	    throw Exception(Exception::EntryOverlapping, "AddressSpaces");
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
	    "ON StatementRanges.statement = Statments.id "
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
	if(executable.first)
	    throw Exception(Exception::EntryNotUnique, "LinkedObjects",
                            "<Threads-Referenced-Executable>");
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
