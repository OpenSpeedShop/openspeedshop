////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the Function class.
 *
 */

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "CallSite.hxx"
#include "EntrySpy.hxx"
#include "Exception.hxx"
#include "ExtentGroup.hxx"
#include "Function.hxx"
#include "FunctionCache.hxx"
#include "LinkedObject.hxx"
#include "Statement.hxx"
#include "StatementCache.hxx"
#include "Thread.hxx"

// #include <demangle.h>

using namespace OpenSpeedShop::Framework;



//
// Note: Unfortunately some versions (e.g. binutils 2.15) of the <demangle.h>
//       header file contain a function declaring a parameter named "typename"
//       which is a reserved word in C++. So instead we declare the few symbols
//       we use from that header here directly. Hopefully in the future this
//       can be remedied.
//

#if !defined (DEMANGLE_H)

#define DMGL_NO_OPTS	 0		/* For readability... */
#define DMGL_PARAMS	 (1 << 0)	/* Include function args */
#define DMGL_ANSI	 (1 << 1)	/* Include const, volatile, etc */

extern "C" char* cplus_demangle(const char*, int);

#endif



/** Function cache. */
FunctionCache Function::TheCache;



/**
 * Get our threads.
 *
 * Returns the threads containing this function.
 *
 * @return    Threads containing this function.
 */
std::set<Thread> Function::getThreads() const
{
    std::set<Thread> threads;

    // Note: This query could be, and in fact used to be, implemented in a
    //       more concise manner as:
    //
    //       SELECT AddressSpaces.thread
    //       FROM AddressSpaces
    //         JOIN Functions
    //       ON AddressSpaces.linked_object = Functions.linked_object
    //       WHERE Functions.id = <dm_entry>;
    //
    //       However the implementation below, combined with an index on
    //       AddressSpaces(linked_object), was found to be quite a bit faster.
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM Functions WHERE Functions.id = ?;"
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
			"<Functions-Referenced>");
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Get our extent in a thread.
 *
 * Returns the extent of this function within the specified thread. An
 * empty extent is returned if this function isn't present within the
 * specified thread.
 *
 * @pre    The thread must be in the same experiment as the function. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the function.
 *
 * @param thread    Thread in which to find our extent.
 * @return          Extent of this function in that thread.
 */
ExtentGroup Function::getExtentIn(const Thread& thread) const
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
    //              Functions.addr_begin,
    //              Functions.addr_end
    //       FROM AddressSpaces
    //         JOIN Functions
    //       ON AddressSpaces.linked_object = Functions.linked_object
    //       WHERE AddressSpaces.thread = <thread.dm_entry>
    //         AND Functions.id = <dm_entry>
    //
    //       However the implementation below, combined with an index on
    //       AddressSpaces(linked_object, thread), was found to be quite
    //       a bit faster.
    
    // Find our linked object and address range
    BEGIN_TRANSACTION(dm_database);
    validate();
    thread.validate();
    dm_database->prepareStatement(
	"SELECT linked_object, "
	"       addr_begin, "
	"       addr_end "
	"FROM Functions "
	"WHERE Functions.id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {

	int linked_object = dm_database->getResultAsInteger(1);	
	Address addr_begin = dm_database->getResultAsAddress(2);
	Address addr_end = dm_database->getResultAsAddress(3);

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
	while(dm_database->executeStatement())
	    extent.push_back(
		Extent(TimeInterval(dm_database->getResultAsTime(1),
				    dm_database->getResultAsTime(2)),
		       AddressRange(dm_database->getResultAsAddress(3) +
				    addr_begin,
				    dm_database->getResultAsAddress(3) +
				    addr_end))
		);

    }	
    END_TRANSACTION(dm_database);

    // Return the extent to the caller    
    return extent;
}



/**
 * Get our linked object.
 *
 * Returns the linked object containing this function.
 *
 * @return    Linked object containing this function.
 */
LinkedObject Function::getLinkedObject() const
{
    LinkedObject linked_object;
    
    // Find our linked object
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object FROM Functions WHERE id = ?;"
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
 * Get our name.
 *
 * Returns the name of this function. Equivalent to getDemangledName(false).
 *
 * @return    Name of this function.
 */
std::string Function::getName() const
{
    return getDemangledName(false);
}



/**
 * Get our mangled name.
 *
 * Returns the mangled name of this function.
 *
 * @return    Mangled name of this function.
 */
std::string Function::getMangledName() const
{
    std::string name;

    // Find our mangled name
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement("SELECT name FROM Functions WHERE id = ?;");
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	name = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the mangled name to the caller
    return name;    
}



/**
 * Get our demangled name.
 *
 * Returns the demangled name of this funciton. An optional boolean flag is used
 * to specify if all available information (including const, volatile, function
 * arguments, etc.) should be included in the demangled name or not.
 *
 * @param all    Boolean "true" if all available information should be included
 *               in the demangled name, "false" otherwise.
 * @return       Demangled name of this function.
 */
std::string Function::getDemangledName(const bool& all) const
{
    // Get our mangled name
    std::string mangled = getMangledName();

    // Demangle the mangled name
    char* demangled = 
	cplus_demangle(mangled.c_str(), 
		       all ? (DMGL_ANSI | DMGL_PARAMS) : DMGL_NO_OPTS);

    // Return the demangled name to the caller
    return demangled ? demangled : mangled;
}



/**
 * Get our definitions.
 *
 * Returns the definitions of this function. An empty set is returned if no
 * definitions of this function are found.
 *
 * @return    Definitions of this function.
 */
std::set<Statement> Function::getDefinitions() const
{
    LinkedObject linked_object;
    ExtentGroup extent;

    // Note: This query could be, and in fact used to be, implemented in a
    //       straightforward manner as:
    //
    //       SELECT Functions.addr_begin,
    //              Statements.id,
    //              StatementRanges.addr_begin,
    //              StatementRanges.addr_end,
    //              StatementRanges.valid_bitmap
    //       FROM StatementRanges
    //         JOIN Statements
    //         JOIN Functions
    //       ON StatementRanges.statement = Statements.id
    //         AND Statements.linked_object = Functions.linked_object
    //       WHERE Functions.id = ?
    //         AND Functions.addr_begin >= StatementRanges.addr_begin
    //         AND Functions.addr_begin < StatementRanges.addr_end;
    //
    //       with a subsequent check if the statement bitmap contains the
    //       function's beginning address. However the performance of this
    //       direct query was found to be lacking. A statement cache was
    //       introduced to accelerate such queries and is now used below.

    // Find our linked object and beginning address
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object, addr_begin FROM Functions WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {

	linked_object = LinkedObject(dm_database,
				     dm_database->getResultAsInteger(1));

	extent.push_back(
	    Extent(
		TimeInterval(Time::TheBeginning(),
			     Time::TheEnd()),
		AddressRange(dm_database->getResultAsAddress(2))
		)
	    );

    }
    END_TRANSACTION(dm_database);

    // Use the statement cache to find our definitions
    std::set<Statement> definitions =
	Statement::TheCache.getStatements(linked_object, extent);

    // Return the definitions to the caller
    return definitions;
}



/**
 * Get our statements.
 *
 * Returns the statements associated with this function. An empty set is
 * returned if no statements are associated with this function.
 *
 * @return    Statements associated with this function.
 */
std::set<Statement> Function::getStatements() const
{
    LinkedObject linked_object;
    ExtentGroup extent;

    // Note: This query could be, and in fact used to be, implemented in a
    //       straightforward manner as:
    //
    //       SELECT Functions.addr_begin,
    //              Functions.addr_end,
    //              Statements.id,
    //              StatementRanges.addr_begin,
    //              StatementRanges.addr_end,
    //              StatementRanges.valid_bitmap
    //       FROM StatementRanges
    //         JOIN Statements
    //         JOIN Functions
    //       ON StatementRanges.statement = Statements.id
    //         AND Statements.linked_object = Functions.linked_object
    //       WHERE Functions.id = ?
    //         AND Functions.addr_end >= StatementRanges.addr_begin
    //         AND Functions.addr_begin < StatementRanges.addr_end;
    //
    //       with a subsequent check if the statement bitmap contains an
    //       address in the function. However the performance of this direct
    //       query was found to be lacking. A statement cache was introduced
    //       to accelerate such queries and is now used below.

    // Find our linked object and address range
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
	"SELECT linked_object, "
	"       addr_begin, "
	"       addr_end "
	"FROM Functions "
	"WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement()) {

	linked_object = LinkedObject(dm_database,
				     dm_database->getResultAsInteger(1));

	extent.push_back(
	    Extent(
		TimeInterval(Time::TheBeginning(),
			     Time::TheEnd()),
		AddressRange(dm_database->getResultAsAddress(2),
			     dm_database->getResultAsAddress(3))
		)
	    );

    }
    END_TRANSACTION(dm_database);

    // Use the statement cache to find our statements
    std::set<Statement> statements =
	Statement::TheCache.getStatements(linked_object, extent);

    // Return the statements to the caller
    return statements;
}



/**
 * Get our callees.
 *
 * Returns the known callees of this function. An empty set is returned if no
 * known callees are found.
 *
 * @todo    Needs to be implemented.
 *
 * @return    All known callees of this function.
 */
std::set<CallSite> Function::getCallees() const
{
    return std::set<CallSite>();
}



/**
 * Get our callers.
 *
 * Returns the known callers of this function. An empty set is returned if no
 * known callers are found.
 *
 * @todo    Needs to be implemented.
 * 
 * @return    All known callers of this function.
 */
std::set<CallSite> Function::getCallers() const
{
    return std::set<CallSite>();
}



/**
 * Default constructor.
 *
 * Constructs a Function that refers to a non-existent function. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Function::Function() :
    Entry()
{
}



/**
 * Constructor from a function entry.
 *
 * Constructs a new Function for the specified function entry.
 *
 * @param database    Database containing this function.
 * @param entry       Identifier for this function.
 */
Function::Function(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, Entry::Functions, entry)
{
}
