////////////////////////////////////////////////////////////////////////////////
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
 * Definition of the Thread class.
 *
 */

#include "Address.hxx"
#include "Assert.hxx"
#include "Function.hxx"
#include "Guard.hxx"
#include "LinkedObject.hxx"
#include "Process.hxx"
#include "Statement.hxx"
#include "SymbolTable.hxx"
#include "Thread.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our state.
 *
 * Returns the caller the current state of the thread. Since this state changes
 * asynchronously and must be updated across a network, there is a lag between
 * when the actual thread's state changes and when that is reflected here.
 *
 * @return    Current state of this thread.
 */
Thread::State Thread::getState() const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);

    // Return our current state to the caller
    return dm_process->getState();
}



/**
 * Test our state.
 *
 * Compares the thread's current state against the passed value. Returns a
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
 * @todo    Currently DPCL only provides the ability to change the state of an
 *          entire process - not that of a single thread. For now, if a thread's
 *          state is changed, ALL threads in the process containing that thread
 *          will have their state changed.
 *
 * @param state    Change to this state.
 */
void Thread::changeState(const State& state)
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);

    // Request the underlying process perform a state change
    dm_process->changeState(state);
}



/**
 * Get our host name.
 *
 * Returns to the caller the name of the host on which the thread is located.
 *
 * @return    Name of host on which this thread is located.
 */
std::string Thread::getHost() const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);
    
    // Return the host name to the caller
    return dm_process->getHost();
}



/**
 * Get our process identifier.
 *
 * Returns to the caller the identifier of the process containing the thread.
 *
 * @return    Identifier of process containing this thread.
 */
pid_t Thread::getProcessId() const
{
    Guard guard_myself(this);
    
    // Check assertions
    Assert(dm_process != NULL);
    
    // Return the process identifier to the caller
    return dm_process->getProcessId();
}



/**
 * Get our POSIX thread identifier.
 *
 * Returns to the caller the POSIX identifier of the thread. If the thread isn't
 * a POSIX thread, the Optional returned will not have a value.
 *
 * @return    Optional identifier of this thread.
 */
Optional<pthread_t> Thread::getPosixThreadId() const
{
    // TODO: implement
    return Optional<pthread_t>();
}



#ifdef HAVE_OPENMP
/**
 * Get our OpenMP thread identifier.
 *
 * Returns to the caller the OpenMP identifier of the thread. If the thread
 * isn't an OpenMP thread, the Optional returned will not have a value.
 *
 * @return    Optional identifier of this thread.
 */
Optional<int> Thread::getOmpThreadId() const
{
    // TODO: implement
    return Optional<int>();
}
#endif



#ifdef HAVE_MPI
/**
 * Get our MPI rank.
 *
 * Returns to the caller the MPI rank of the thread within the application's
 * world communicator. If the thread isn't part of an MPI application, the
 * Optional returned will not have a value.
 *
 * @return    Optional rank of this thread.
 */
Optional<int> Thread::getMpiRank() const
{
    // TODO: implement
    return Optional<int>();
}
#endif



#ifdef HAVE_ARRAY_SERVICES
/**
 * Get our array session handle.
 *
 * Returns to the caller the array session handle containing the thread. If the
 * thread isn't part of an array session, the Optional returned will not have a
 * value.
 *
 * @return    Optional array session handle containing this thread.
 */
Optional<ash_t> Thread::getArraySessionHandle() const
{
    // TODO: implement
    return Optional<ash_t>();
}
#endif



/**
 * Find all linked objects.
 *
 * Finds and returns all the linked objects contained within this thread. An
 * empty list is returned if no linked objects are found within this thread
 * (unlikely).
 *
 * @param time    Query time.
 * @return        All linked objects in this thread.
 */
std::vector<LinkedObject> Thread::findAllLinkedObjects(const Time& time) const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);
    
    // Find the address space for our process at the query time
    const AddressSpace* address_space = dm_process->findAddressSpace(time);
    Assert(address_space != NULL);

    // Find all symbol tables in that address space
    std::vector<const SymbolTable*> symbol_tables =
	address_space->findSymbolTables();

    // Create the linked objects and return them to the caller
    std::vector<LinkedObject> result;
    for(std::vector<const SymbolTable*>::const_iterator
	    i = symbol_tables.begin(); i != symbol_tables.end(); ++i)
	result.push_back(LinkedObject(address_space, *i,
				      (*i)->getLinkedObjectEntry()));
    return result;
}



/**
 * Find all functions.
 *
 * Finds and returns all the functions contained within this thread. An empty
 * list is returned if no functions are found within this thread (unlikely).
 *
 * @note    This is an extremely high cost operation in terms of time and memory
 *          used. Avoid using this member function if at all possible.
 *
 * @param time    Query time.
 * @return        All functions in this thread.
 */
std::vector<Function> Thread::findAllFunctions(const Time& time) const
{
    Guard guard_myself(this);
    
    // Check assertions
    Assert(dm_process != NULL);
    
    // Find the address space for our process at the query time
    const AddressSpace* address_space = dm_process->findAddressSpace(time);
    Assert(address_space != NULL);

    // Find all symbol tables in that address space
    std::vector<const SymbolTable*> symbol_tables =
	address_space->findSymbolTables();

    // Create the functions and return them to the caller
    std::vector<Function> result;
    for(std::vector<const SymbolTable*>::const_iterator
	    i = symbol_tables.begin(); i != symbol_tables.end(); ++i) {
	std::vector<const FunctionEntry*> functions = (*i)->findAllFunctions();
	for(std::vector<const FunctionEntry*>::const_iterator
		j = functions.begin(); j != functions.end(); ++j)
	    result.push_back(Function(address_space, *i, *j));
    }
    return result;
}



/**
 * Find the linked object at an address.
 *
 * Finds the linked object containing the instruction at the passed query
 * address and returns the corresponding LinkedObject object.  If no linked
 * object can be found that contains this instruction, the Optional returned
 * will not have a value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional linked object at the query address.
 */
Optional<LinkedObject> Thread::findLinkedObjectAt(const Address& address,
						  const Time& time) const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);
    
    // Find the address space for our process at the query time
    const AddressSpace* address_space = dm_process->findAddressSpace(time);
    Assert(address_space != NULL);
    
    // Find the symbol table in that address space at the query address
    const SymbolTable* symbol_table = 
	address_space->findSymbolTableByAddress(address);
    
    // Return an empty optional if no symbol table was found
    if(symbol_table == NULL)
	return Optional<LinkedObject>();

    // Create the linked object and return it to the caller
    return Optional<LinkedObject>(LinkedObject(
				      address_space, symbol_table,
				      symbol_table->getLinkedObjectEntry()));
}



/**
 * Find the source function at an address.
 *
 * Finds the source function containing the instruction at the passed query
 * address and returns the corresponding Function object. If no function can be
 * found that contains this instruction, the Optional returned will not have a
 * value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional function at the query address.
 */
Optional<Function> Thread::findFunctionAt(const Address& address,
					  const Time& time) const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);
    
    // Find the address space for our process at the query time
    const AddressSpace* address_space = dm_process->findAddressSpace(time);
    Assert(address_space != NULL);

    // Find the symbol table in that address space at the query address
    const SymbolTable* symbol_table = 
	address_space->findSymbolTableByAddress(address);

    // Return an empty optional if no symbol table was found
    if(symbol_table == NULL)
	return Optional<Function>();
    
    // Find the base address of this symbol table within its address space
    Address base_address = 
	address_space->findBaseAddressBySymbolTable(symbol_table);

    // Find the function in that symbol table at the query address
    const FunctionEntry* entry = 
	symbol_table->findFunctionAt(address - base_address);
    
    // Return an empty optional if no function was found
    if(entry == NULL)
	return Optional<Function>();
    
    // Create the function and return it to the caller
    return Optional<Function>(Function(address_space, symbol_table, entry));
}



/**
 * Find the source statement at an address.
 *
 * Finds the source statement containing the instruction at the passed query
 * address and returns the corresponding Statement object. If no source
 * statement information is available, or if no statement can be found that
 * contains this instruction, the Optional returned will not have a value.
 *
 * @param address    Query address.
 * @param time       Query time.
 * @return           Optional statement at the query address.
 */
Optional<Statement> Thread::findStatementAt(const Address& address,
					    const Time& time) const
{
    Guard guard_myself(this);

    // Check assertions
    Assert(dm_process != NULL);
    
    // Find the address space for our process at the query time
    const AddressSpace* address_space = dm_process->findAddressSpace(time);
    Assert(address_space != NULL);

    // Find the symbol table in that address space at the query address
    const SymbolTable* symbol_table = 
	address_space->findSymbolTableByAddress(address);

    // Return an empty optional if no symbol table was found
    if(symbol_table == NULL)
	return Optional<Statement>();
    
    // Find the base address of this symbol table within its address space
    Address base_address = 
	address_space->findBaseAddressBySymbolTable(symbol_table);

    // Find the statement in that symbol table at the query address
    const StatementEntry* entry = 
	symbol_table->findStatementAt(address - base_address);
    
    // Return an empty optional if no statement was found
    if(entry == NULL)
	return Optional<Statement>();
    
    // Create the statement and return it to the caller
    return Optional<Statement>(Statement(address_space, symbol_table, entry));
}



/**
 * Constructor from a process pointer.
 *
 * Creates a new thread within the specified process. Simply squirrels away the
 * process pointer for future use.
 *
 * @param process    Process containing this thread.
 */
Thread::Thread(Process* process) :
    dm_process(process)
{
    // Check assertions
    Assert(dm_process != NULL);
}



/**
 * Destructor.
 *
 * Does nothing. Defined here only so that the destructor can be made private
 * and thus prevent anyone other than Application from destroying a thread. 
 */
Thread::~Thread()
{  
    Guard guard_myself(this);
}
