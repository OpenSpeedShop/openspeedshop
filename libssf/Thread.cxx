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

#include "Address.hxx"
#include "Database.hxx"
#include "Function.hxx"
#include "Instrumentor.hxx"
#include "LinkedObject.hxx"
#include "Statement.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our state.
 *
 * Returns the caller the current state of this thread. Since this state changes
 * asynchronously and may be updated across a network, there is a lag between
 * when the actual thread's state changes and when that is reflected here.
 *
 * @return    Current state of this thread.
 */
Thread::State Thread::getState() const
{
    return Instrumentor::getThreadState(*this);
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
 * @note    Some transitions are disallowed because they do not make sense or
 *          cannot be implemented. For example, a terminated thread cannot be
 *          set to a running thread. An exception of type std::logic_error is
 *          thrown when such an invalid transition is requested.
 *
 * @note    Only one in-progress state change is allowed per thread at any
 *          given time. For example, if you request that a thread be suspended,
 *          you cannot request that it be terminated before the suspension is
 *          completed. An exception of type std::logic_error is thrown when
 *          multiple in-progress changes are requested.
 *
 * @param state    Change to this state.
 */
void Thread::changeState(const State& state) const
{
    Instrumentor::changeThreadState(*this, state);
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
    validateEntry();
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
    validateEntry();
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
 * thread, the Optional returned will not have a value.
 *
 * @return    Optional identifier of this thread.
 */
Optional<pthread_t> Thread::getPosixThreadId() const
{
    Optional<pthread_t> tid;

    // Find our POSIX thread identifier	
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT posix_tid FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    tid = static_cast<pthread_t>
		(dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the POSIX thread identifier to the caller
    return tid;
}



#ifdef HAVE_OPENMP
/**
 * Get our OpenMP thread identifier.
 *
 * Returns the OpenMP identifier of this thread. If this thread isn't an OpenMP
 * thread, the Optional returned will not have a value.
 *
 * @return    Optional identifier of this thread.
 */
Optional<int> Thread::getOmpThreadId() const
{
    Optional<int> tid;

    // Find our OpenMP thread identifier	
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement("SELECT omp_tid FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    tid = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);

    // Return the OpenMP thread identifier to the caller
    return tid;
}
#endif



#ifdef HAVE_MPI
/**
 * Get our MPI rank.
 *
 * Returns the MPI rank of this thread within the application's world
 * communicator. If this thread isn't part of an MPI application, the
 * Optional returned will not have a value.
 *
 * @return    Optional rank of this thread.
 */
Optional<int> Thread::getMpiRank() const
{
    Optional<int> rank;
    
    // Find our MPI rank
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement("SELECT mpi_rank FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    rank = dm_database->getResultAsInteger(1);
    END_TRANSACTION(dm_database);
    
    // Return the MPI rank to the caller
    return rank;
}
#endif



#ifdef HAVE_ARRAY_SERVICES
/**
 * Get our array session handle.
 *
 * Returns the array session handle containing this thread. If this thread
 * isn't part of an array session, the Optional returned will not have a value.
 *
 * @return    Optional array session handle containing this thread.
 */
Optional<ash_t> Thread::getArraySessionHandle() const
{
    Optional<ash_t> ash;

    // Find our array session handle
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement("SELECT ash FROM Threads WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	if(!dm_database->getResultIsNull(1))
	    ash = static_cast<ash_t>(dm_database->getResultAsInteger(1));
    END_TRANSACTION(dm_database);
    
    // Return the array session handle to the caller
    return ash;
}
#endif



/**
 * Get our linked objects.
 *
 * Returns the linked objects contained within this thread at a particular
 * moment in time. If no linked objects can be found, the Optional retruned
 * will not have a value.
 *
 * @param time    Query time.
 * @return        Linked objects contained within this thread.
 */
Optional<std::vector<LinkedObject> >
Thread::getLinkedObjects(const Time& time) const
{
    std::vector<LinkedObject> linked_objects;

    // Find our address space entries containing the query time
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT id, linked_object FROM AddressSpaces"
	" WHERE thread = ? AND ? >= time_begin AND ? < time_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    while(dm_database->executeStatement())
	linked_objects.push_back(
	    LinkedObject(dm_database,
			 dm_database->getResultAsInteger(2),
			 dm_database->getResultAsInteger(1)));        
    END_TRANSACTION(dm_database);
   
    // Return the linked objects to the caller
    return linked_objects.empty() ?
	Optional<std::vector<LinkedObject> >() :
	Optional<std::vector<LinkedObject> >(linked_objects);
}



/**
 * Get our functions.
 *
 * Returns the functions contained within this thread at a particular moment
 * in time. If no functions can be found, the Optional returned will not have
 * a value.
 *
 * @note    This is an extremely high cost operation in terms of time and memory
 *          used. Avoid using this member function if at all possible.
 *
 * @param time    Query time.
 * @return        Functions contained within this thread.
 */
Optional<std::vector<Function> >
Thread::getFunctions(const Time& time) const
{
    std::vector<Function> functions;

    // Find our functions and their contexts containing the query time
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT Functions.id, AddressSpaces.id"
	" FROM Functions JOIN AddressSpaces"
	" ON Functions.linked_object = AddressSpaces.linked_object"
	" WHERE AddressSpaces.thread = ?"
	"   AND ? >= AddressSpaces.time_begin AND ? < AddressSpaces.time_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    while(dm_database->executeStatement())
	functions.push_back(Function(dm_database,
				     dm_database->getResultAsInteger(1),
				     dm_database->getResultAsInteger(2)));    
    END_TRANSACTION(dm_database);

    // Return the functions to the caller
    return functions.empty() ?
	Optional<std::vector<Function> >() :
	Optional<std::vector<Function> >(functions);
}



/**
 * Get the linked object at an address.
 *
 * Returns the linked object containing the passed query address at a
 * particular moment in time. If a linked object cannot be found, the
 * Optional returned will not have a value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional linked object containing the query address.
 */
Optional<LinkedObject> Thread::getLinkedObjectAt(const Address& address,
						 const Time& time) const
{
    Optional<LinkedObject> linked_object;

    // Find the address space entry containing the query time/address
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT id, linked_object FROM AddressSpaces"
	" WHERE thread = ? AND ? >= time_begin AND ? < time_end"
	"   AND ? >= addr_begin AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);	
    while(dm_database->executeStatement()) {
	if(linked_object.hasValue())
	    throw Database::Corrupted(*dm_database, 
				      "overlapping address space entries");
	linked_object = LinkedObject(dm_database,
				     dm_database->getResultAsInteger(2),
				     dm_database->getResultAsInteger(1));
    }    
    END_TRANSACTION(dm_database);
    
    // Return the linked object to the caller
    return linked_object;
}



/**
 * Get the function at an address.
 *
 * Returns the function containing the passed query address at a particular
 * moment in time. If a function cannot be found, the Optional returned will
 * not have a value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional function containing the query address.
 */
Optional<Function> Thread::getFunctionAt(const Address& address,
					 const Time& time) const
{
    Optional<Function> function;
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();

    // Find the address space entry containing the query time/address
    Optional<int> context;
    Optional<int> linked_object;
    Optional<Address> base_address;
    dm_database->beginTransaction();
    dm_database->prepareStatement(
	"SELECT id, linked_object, addr_begin FROM AddressSpaces"
	" WHERE thread = ? AND ? >= time_begin AND ? < time_end"
	"   AND ? >= addr_begin AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);		
    while(dm_database->executeStatement()) {
	if(context.hasValue())
	    throw Database::Corrupted(*dm_database,
				      "overlapping address space entries");
	context = dm_database->getResultAsInteger(1);
	linked_object = dm_database->getResultAsInteger(2);
	base_address = dm_database->getResultAsAddress(3);
    }
    
    // Find the function containing the query address
    if(context.hasValue() &&
       linked_object.hasValue() &&
       base_address.hasValue()) {

	dm_database->prepareStatement(
	    "SELECT id FROM Functions"
	    " WHERE linked_object = ? AND ? >= addr_begin AND ? < addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - base_address));
	dm_database->bindArgument(3, Address(address - base_address));
	while(dm_database->executeStatement()) {
	    Assert(!function.hasValue());
	    function = Function(dm_database,
				dm_database->getResultAsInteger(1),
				context);
	}
	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);    
    
    // Return the function to the caller
    return function;
}



/**
 * Get the statement at an address.
 *
 * Returns the statement containing the passed query address at a particular
 * moment in time. If a statement cannot be found, or if no source statement
 * information is available, the Optional returned will not have a value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional statement containing the query address.
 */
Optional<Statement> Thread::getStatementAt(const Address& address,
					   const Time& time) const
{
    Optional<Statement> statement;
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    
    // Find the address space entry containing the query time/address
    Optional<int> context;
    Optional<int> linked_object;
    Optional<Address> base_address;
    dm_database->beginTransaction();
    dm_database->prepareStatement(
	"SELECT id, linked_object, addr_begin FROM AddressSpaces"
	" WHERE thread = ? AND ? >= time_begin AND ? < time_end"
	"   AND ? >= addr_begin AND ? < addr_end;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, address);
    dm_database->bindArgument(5, address);		
    while(dm_database->executeStatement()) {
	if(context.hasValue())
	    throw Database::Corrupted(*dm_database, 
				      "overlapping address space entries");
	context = dm_database->getResultAsInteger(1);
	linked_object = dm_database->getResultAsInteger(2);
	base_address = dm_database->getResultAsAddress(3);
    }
    
    // Find the statement containing the query address
    if(context.hasValue() &&
       linked_object.hasValue() &&
       base_address.hasValue()) {
	
	dm_database->prepareStatement(
	    "SELECT statement FROM StatementRanges"
	    " WHERE linked_object = ? AND ? >= addr_begin AND ? < addr_end;"
	    );
	dm_database->bindArgument(1, linked_object);
	dm_database->bindArgument(2, Address(address - base_address));
	dm_database->bindArgument(3, Address(address - base_address));
	while(dm_database->executeStatement()) {
	    Assert(!statement.hasValue());
	    statement = Statement(dm_database,
				  dm_database->getResultAsInteger(1),
				  context);
	}
	
    }

    // End this multi-statement transaction
    END_TRANSACTION(dm_database);    

    // Return the statement to the caller
    return statement;
}



/**
 * Get a function by name.
 *
 * Returns the function with the passed query name at a particular moment in
 * time. If the function cannot be found, the Optional returned will not have
 * a value. If more than one function is found, the first function found is
 * returned.
 *
 * @param name    Query name.
 * @param time    Query time.
 * @return        Optional function with the query name.
 */
Optional<Function> Thread::getFunctionByName(const std::string& name,
					     const Time& time) const
{
    Optional<Function> function;

    // Find the function and its context containing the query time
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
	"SELECT Functions.id, AddressSpaces.id"
	" FROM Functions JOIN AddressSpaces"
	" ON Functions.linked_object = AddressSpaces.linked_object"
	" WHERE AddressSpaces.thread = ?"
	"   AND ? >= AddressSpaces.time_begin AND ? < AddressSpaces.time_end"
	"   AND Functions.name = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, time);
    dm_database->bindArgument(3, time);
    dm_database->bindArgument(4, name);
    while(dm_database->executeStatement())
	if(!function.hasValue())
	    function = Function(dm_database,
				dm_database->getResultAsInteger(1),
				dm_database->getResultAsInteger(2));    
    END_TRANSACTION(dm_database);
    
    // Return the function to the caller
    return function;
}



/**
 * Default constructor.
 *
 * Constructs a Thread that refers to a non-existent thread. Any use of a
 * member function on an object constructed in this way will result in an
 * assertion failure. The only reason this default constructor exists is to
 * allow Optional<Thread> to create an empty optional value and to allow
 * Experiment to temporarily create "empty" threads.
 */
Thread::Thread() :
    dm_database(NULL),
    dm_entry(0)
{
}



/**
 * Constructor from a thread entry.
 *
 * Constructs a new Thread for the specified thread entry, contained within the
 * passed database.
 *
 * @param database         Database containing the thread.
 * @param entry            Entry (id) for the thread.
 */
Thread::Thread(const SmartPtr<Database>& database, const int& entry):
    dm_database(database),
    dm_entry(entry)
{
}



/**
 * Validate our entry.
 *
 * Validates the existence and uniqueness of our entry within our database. If
 * our entry is found and is unique, this function simply returns. Otherwise
 * an exception of type Database::Corrupted is thrown.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 */
void Thread::validateEntry() const
{
    // Find the number of rows matching our entry
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Threads WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate
    if(rows == 0)
	throw Database::Corrupted(*dm_database,
				  "thread entry no longer exists");
    else if(rows > 1)
	throw Database::Corrupted(*dm_database,
				  "thread entry is not unique");
}
