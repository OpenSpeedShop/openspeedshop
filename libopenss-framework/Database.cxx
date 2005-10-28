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
 * Definition of the Database class.
 *
 */

#include "Address.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "Exception.hxx"
#include "Guard.hxx"
#include "Path.hxx"
#include "Time.hxx"

#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <limits>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Offset used to convert 64-bit unsigned integers to 64-bit signed. */
    const int64_t signedOffset = std::numeric_limits<int64_t>::max() + 1;
    
    

    /**
     * Database busy handler.
     *
     * Callback function used by SQLite when an attempt is made to access
     * a database that is currently locked by another process or thread. 
     * Temporarily suspends the calling thread before instructing SQLite to
     * retry the operation. Currently the thread is suspended for 1 mS.
     *     
     * @return    Always "1" indicating SQLite should retry the operation.
     */
    int busyHandler(void*, int)
    {
	// Setup to wait for 1 mS
	struct timespec wait;
	wait.tv_sec = 0;
	wait.tv_nsec = 1 * 1000 * 1000;
	
	// Suspend ourselves temporarily
	nanosleep(&wait, NULL);
	
	// Instruct SQLite to retry the query
	return 1;
    }


    
}



/**
 * Test accessibility of a database.
 *
 * Returns a boolean value indicating if the specified database is accessible.
 * Simply confirms that the database exists and is accessible by the database
 * engine. It does not examine or check the actual contents of the database.
 *
 * @param name    Name of the database to be tested.
 * @return        Boolean "true" if the database is accessible,
 *                "false" otherwise.
 */
bool Database::isAccessible(const std::string& name)
{
    // Verify a file with this name exists
    int fd;
    if((fd = open(name.c_str(), O_RDONLY)) == -1)
	return false;
    Assert(close(fd) == 0);
    
    // Open this file as a database
    sqlite3* handle = NULL;
    int retval = sqlite3_open(name.c_str(), &handle);
    if(retval == SQLITE_CANTOPEN)
	return false;
    Assert(retval == SQLITE_OK);
    Assert(handle != NULL);
    
    // Check that we reallly have a database
    retval = sqlite3_exec(handle,
			  "SELECT * FROM sqlite_master WHERE type='table';",
			  NULL, NULL, NULL);
    if(retval == SQLITE_NOTADB) {
	Assert(sqlite3_close(handle) == SQLITE_OK); 
	return false;
    }
    Assert(retval == SQLITE_OK);
    
    // Close the database
    Assert(sqlite3_close(handle) == SQLITE_OK); 
    
    // Return that the database is accessible
    return true;
}



/**
 * Create a database.
 *
 * Creates a new, empty, database with the specified name. Operations can
 * be performed on the new database by creating a Database object with this
 * database's name and then executing SQL statements.
 *
 * @note    A DatabaseExists or DatabaseCannotCreate exception is thrown if the
 *          database cannot be created for any reason (including pre-existence
 *          of the named database in the former case).
 *
 * @param name    Name of the database to be created.
 */
void Database::create(const std::string& name)
{
    // Verify the database is not already accessible
    if(isAccessible(name))
	throw Exception(Exception::DatabaseExists, name);
    
    // Open this file as a database
    sqlite3* handle = NULL;
    int retval = sqlite3_open(name.c_str(), &handle);
    if(retval == SQLITE_CANTOPEN)
	throw Exception(Exception::DatabaseCannotCreate, name);
    Assert(retval == SQLITE_OK);
    Assert(handle != NULL);
    
    // Close the database
    Assert(sqlite3_close(handle) == SQLITE_OK); 
}



/**
 * Remove a database.
 *
 * Removes the database with the specified name.
 *
 * @note    A DatabaseDoesNotExist or DatabaseCannotRemove exception is thrown
 *          if the database cannot be removed for any reason (including the non-
 *          existence of the named database in the former case).
 *
 * @param name    Name of the database to be removed.
 */
void Database::remove(const std::string& name)
{
    // Verify the database is accessible
    if(!isAccessible(name))
	throw Exception(Exception::DatabaseDoesNotExist, name);
    
    // Attempt to remove the database file
    if(::remove(name.c_str()) == -1)
	throw Exception(Exception::DatabaseCannotRemove, name, strerror(errno));
}



/**
 * Constructor from database name.
 *
 * Constructs an object for accessing the existing named database. Operations
 * can then be performed on this database by executing SQL statements using the
 * member functions of this class.
 *
 * @note    A DatabaseDoesNotExist exception is thrown if the database cannot
 *          be opened for any reason.
 *
 * @param name    Name of database to be accessed.
 */
Database::Database(const std::string& name) :
    Lockable(),
    dm_name(name),
    dm_transaction_lock(),
    dm_handles()
{
    // Initialize the transaction lock
    Assert(pthread_rwlock_init(&dm_transaction_lock, NULL) == 0);
    
    // Verify the database is accessible
    if(!isAccessible(name))
	throw Exception(Exception::DatabaseDoesNotExist, name);
}



/**
 * Destructor.
 *
 * Closes this database. All per-thread database handles are first released.
 */
Database::~Database()
{
    // Release all per-thread database handles for this database
    releaseAllHandles();    
}



/**
 * Rename this database.
 *
 * Renames this database to the specified name. The database is unmodified and
 * still accessible via this object.
 *
 * @param name    New name of this database.
 */
void Database::renameTo(const std::string& name)
{
    //
    // Acquire write access for the transaction lock
    //     (indicate no in-progress transactions beyond this point)
    //
    Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
    
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Begin an exclusive transaction to prevent mid-copy changes
    Assert(sqlite3_exec(handle.dm_database, "BEGIN EXCLUSIVE TRANSACTION;",
			NULL, NULL, NULL) == SQLITE_OK);

    // Perform the rename
    try {
	
	// Create a new database with the new name
	create(name);

	// Copy the original database to the new database
	copyFile(Path(dm_name), Path(name));
	
	// Remove the original database
	remove(dm_name);
	
    }
    catch(...) {
	
	// Commit the transaction to allow changes to now proceed
	Assert(sqlite3_exec(handle.dm_database, "COMMIT TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);

	//
	// Release write access for the transaction lock
	//     (indicate transactions can once again proceed)
	//
	Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
	
	// Re-throw exception upwards
	throw;
    }
    
    // Release all per-thread database handles for this database
    releaseAllHandles();    
    
    // Switch to the new database
    dm_name = name;
    
    // Commit the transaction to allow changes to now proceed
    Assert(sqlite3_exec(handle.dm_database, "COMMIT TRANSACTION;",
			NULL, NULL, NULL) == SQLITE_OK);
    
    //
    // Release write access for the transaction lock
    //     (indicate transactions can once again proceed)
    //
    Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
}



/**
 * Copy this database.
 *
 * Copies this database to the specified name. The original database remains
 * unmodified and still accessible via this object. Accessing the copy involves
 * creating a new Database object with the copy's name.
 *
 * @param name    Name of the created copy.
 */
void Database::copyTo(const std::string& name)
{
    //
    // Acquire write access for the transaction lock
    //     (indicate no in-progress transactions beyond this point)
    //
    Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
    
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Begin an exclusive transaction to prevent mid-copy changes
    Assert(sqlite3_exec(handle.dm_database, "BEGIN EXCLUSIVE TRANSACTION;",
			NULL, NULL, NULL) == SQLITE_OK);

    // Perform the rename
    try {

	// Create a new database with the new name
	create(name);

	// Copy the original database to the new database
	copyFile(dm_name, name);

    }
    catch(...) {

	// Commit the transaction to allow changes to now proceed
	Assert(sqlite3_exec(handle.dm_database, "COMMIT TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);
	
	//
	// Release write access for the transaction lock
	//     (indicate transactions can once again proceed)
	//
	Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
	
	// Re-throw exception upwards
	throw;
    }

    // Commit the transaction to allow changes to now proceed
    Assert(sqlite3_exec(handle.dm_database, "COMMIT TRANSACTION;",
			NULL, NULL, NULL) == SQLITE_OK);

    //
    // Release write access for the transaction lock
    //     (indicate transactions can once again proceed)
    //
    Assert(pthread_rwlock_wrlock(&dm_transaction_lock) == 0);
}



/**
 * Get our name.
 *
 * Returns the name of this database.
 *
 * @return    Name of this database.
 */
std::string Database::getName() const
{
    Guard guard_myself(this);

    // Return the database name to the caller
    return dm_name;
}



/**
 * Begin a new transaction.
 *
 * Begins a new SQL transaction on this database.
 *
 * @note    Nested transactions (of a sort) are implemented. A given thread may
 *          begin additional transactions before completing the previous one.
 */
void Database::beginTransaction()
{
    //
    // Acquire read access for the transaction lock
    //     (indicate there is an in-progress transaction)
    //
    Assert(pthread_rwlock_rdlock(&dm_transaction_lock) == 0);
    
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_database != NULL);
    
    // Is this an outer transaction?
    if(handle.dm_nesting_level == 0) {
	
	// Execute a SQL query to begin a new transaction
	Assert(sqlite3_exec(handle.dm_database, "BEGIN DEFERRED TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);
	
	// Indicate the transaction can be committed
	handle.dm_is_committable = true;
	
    }
    
    // Increment the transaction nesting level
    handle.dm_nesting_level++;
}



/**
 * Prepare a SQL statement.
 *
 * Prepares the passed SQL statement for execution by parsing it. After parsing,
 * any arguments to the statement are cleared by binding them to null values.
 * Performance is improved by automatically caching parsed prepared statements
 * so that subsequent executions of the same query will not require parsing.
 *
 * @note    Statements may only be prepared within the context of a transaction.
 *          Any attempt to prepare a statement before beginning a transaction
 *          will result in an assertion failure.
 * 
 * @note    A statement that has been prepared must be executed to completion
 *          via calls to executeStatement(). Any attempt to prepare a statement
 *          before a previously prepared statement has completed will result in
 *          an assertion failure.
 *
 * @note    Preparing multiple SQL statements (separated by ";") via a single
 *          call to this function is not supported. Each statement must be
 *          prepared and executed separately. Any attempt to prepare multiple
 *          statements at once will result in an assertion failure.
 *
 * @note    An assertion failure occurs if the SQL statement contains a syntax
 *          error. A DatabaseInvalid exception is thrown if the statement cannot
 *          be prepared for any other reason (e.g. a query against a table that
 *          doesn't exist).
 *
 * @param statement    SQL statement to be executed.
 */
void Database::prepareStatement(const std::string& statement)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();
    
    // Check assertions
    Assert(handle.dm_database != NULL);
    Assert(handle.dm_nesting_level > 0);
    Assert(handle.dm_statement == NULL);

    // Determine if this statement has already been cached
    std::map<std::string, sqlite3_stmt*>::const_iterator i =
	handle.dm_cache.find(statement);
    if(i != handle.dm_cache.end()) {
	
	// Use the cached prepared statement
	handle.dm_statement = i->second;
	
	// Reset this statement for re-execution
	Assert(sqlite3_reset(handle.dm_statement) == SQLITE_OK);
	
    }
    else {
	
	// Prepare this statement for execution
	const char* tail = NULL;	
	int retval = sqlite3_prepare(handle.dm_database, 
				     statement.c_str(), statement.size(),
				     &handle.dm_statement, &tail);
	if(retval == SQLITE_ERROR) {
	    std::string errmsg = sqlite3_errmsg(handle.dm_database);
	    if(errmsg.find("syntax error") == std::string::npos)
		throw Exception(Exception::DatabaseInvalid, getName(), errmsg);
	}
	Assert(retval == SQLITE_OK);
	Assert((tail != NULL) && (*tail == '\0'));
	
	// Cache this prepared statement for future use
	handle.dm_cache[statement] = handle.dm_statement;
	
    }
    
    // Check assertions
    Assert(handle.dm_statement != NULL);
    
    // Bind NULL values to all arguments of this statement
    for(int i = 1; i <= sqlite3_bind_parameter_count(handle.dm_statement); ++i)
	Assert(sqlite3_bind_null(handle.dm_statement, i) == SQLITE_OK);
}



/**
 * Bind a string to an argument.
 *
 * Binds the passed string value to the specified numbered argument of the
 * currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const std::string& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_text(handle.dm_statement, index,
			     value.c_str(), value.size(),
			     SQLITE_TRANSIENT) == SQLITE_OK);
}



/**
 * Bind a signed integer to an argument.
 *
 * Binds the passed signed integer value to the specified numbered argument of
 * the currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const int& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_int(handle.dm_statement, index, value) == SQLITE_OK);
}



/**
 * Bind a real to an argument.
 *
 * Binds the passed real value to the specified numbered argument of the
 * currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const double& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_double(handle.dm_statement, index, value) == SQLITE_OK);
}



/**
 * Bind a blob to an argument.
 *
 * Binds the passed blob value to the specified numbered argument of the
 * currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const Blob& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_blob(handle.dm_statement, index, 
			     value.getContents(), value.getSize(), 
			     SQLITE_TRANSIENT) == SQLITE_OK);
}



/**
 * Bind an address to an argument.
 *
 * Binds the passed address value to the specified numbered argument of the
 * currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const Address& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));

    // Bind the argument
    Assert(sqlite3_bind_int64(handle.dm_statement, index, 
			      static_cast<int64_t>(value.getValue()) -
			      signedOffset) == SQLITE_OK);
}



/**
 * Bind a time to an argument.
 *
 * Binds the passed time value to the specified numbered argument of the
 * currently prepared statement. Arguments in a SQL statement are indicated
 * by wildcard ('?') characters. For example, "INSERT INTO tbl VALUES (?,?);"
 * has two arguments numbered "1" and "2".
 *
 * @note    Arguments may only be bound after a statement has previously been
 *          prepared via a call to prepareStatement(). Any attempt to bind an
 *          argument before preparing a statement will result in an assertion
 *          failure.
 *
 * @note    Attempting to bind an argument index less than one or greater than
 *          the number of bindable arguments in the currently prepared statement
 *          will result in an assertion failure.
 *
 * @param index    Index (number) of argument to be bound.
 * @param value    Value to be bound to this argument.
 */
void Database::bindArgument(const unsigned& index, const Time& value)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(handle.dm_statement)));

    // Bind the argument
    Assert(sqlite3_bind_int64(handle.dm_statement, index, 
			      static_cast<int64_t>(value.getValue()) -
			      signedOffset) == SQLITE_OK);
}



/**
 * Execute a SQL statement.
 *
 * Called one or more times to execute the previously prepared SQL statement.
 * Statements producing no results will executed completely with a single call.
 * Statements producing results will, on the other hand, produce a single row
 * for each call to this function. It is intended that this function be called
 * using a loop such as "while(db.executeStatement()) ...;" where "..." is code
 * for accessing a single row of result data.
 *
 * @note    Statements may only be executed after previously being prepared via
 *          a call to prepareStatement(). Any attempt to execute a statement
 *          before it is prepared will result in an assertion failure.
 *
 * @return    Boolean "true" if execution generated a result row and has not
 *            been completed, "false" if execution has completed.
 */
bool Database::executeStatement()
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    
    // Execute the next step of the current statement
    int retval = sqlite3_step(handle.dm_statement);
    Assert((retval == SQLITE_ROW) || (retval == SQLITE_DONE));
    
    // Handle a completed statement
    if(retval == SQLITE_DONE) {
	
	// Indicate there is no longer a current statement
	handle.dm_statement = NULL;
	
    }
    
    // Indicate to the caller whether or not there are results
    return retval == SQLITE_ROW;
}



/**
 * Test for a null result.
 *
 * Test if the specified numbered column in the active row of the currently
 * executing statement is null.
 *
 * @param index    Index (number) of column to test.
 * @return         Boolean "true" if this column is null, "false" otherwise.
 */
bool Database::getResultIsNull(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));

    // Return the result to the caller
    return sqlite3_column_type(handle.dm_statement, index - 1) == SQLITE_NULL;
}



/**
 * Get a string result.
 *
 * Obtains a string from the specified numbered column in the active row of the
 * currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         String result from this column.
 */
std::string Database::getResultAsString(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));

    // Return the result to the caller
    return reinterpret_cast<const char*>
	(sqlite3_column_text(handle.dm_statement, index - 1));
}



/**
 * Get a signed integer result.
 *
 * Obtains a signed integer from the specified numbered column in the active
 * row of the currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         Signed integer result from this column.
 */
int Database::getResultAsInteger(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));

    // Return the result to the caller
    return sqlite3_column_int(handle.dm_statement, index - 1);
}



/**
 * Get a real result.
 *
 * Obtains a real from the specified numbered column in the active row of the
 * currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         Real result from this column.
 */
double Database::getResultAsReal(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));

    // Return the result to the caller
    return sqlite3_column_double(handle.dm_statement, index - 1);
}



/**
 * Get a blob result.
 *
 * Obtains a blob from the specified numbered column in the active row of the
 * currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         Blob result from this column.
 */
Blob Database::getResultAsBlob(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));
    
    // Return the result to the caller
    return Blob(sqlite3_column_bytes(handle.dm_statement, index - 1),
		sqlite3_column_blob(handle.dm_statement, index - 1));
}



/**
 * Get an address result.
 *
 * Obtains an address from the specified numbered column in the active row of 
 * the currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         Address result from this column.
 */
Address Database::getResultAsAddress(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));
    
    // Return the result to the caller
    return Address(sqlite3_column_int64(handle.dm_statement, index - 1) +
		   signedOffset);
}



/**
 * Get a time result.
 *
 * Obtains a time from the specified numbered column in the active row of the
 * currently executing statement.
 *
 * @note    Results may be obtained only after a statement has previously been
 *          executed via a call to executeStatement(). Any attempt to obtain a
 *          result from a column before executing a statement will result in an
 *          assertion failure.
 *
 * @note    Attempting to obtain a result for a column index less than one
 *          or greater than the number of columns in the currently prepared
 *          statement will result in an assertion failure.
 *
 * @param index    Index (number) of column to obtain.
 * @return         Time result from this column.
 */
Time Database::getResultAsTime(const unsigned& index)
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(handle.dm_statement)));
    
    // Return the result to the caller
    return Time(sqlite3_column_int64(handle.dm_statement, index - 1) +
		signedOffset);
}



/**
 * Get unique ID of last inserted row.
 *
 * Obtains the unique ID of the last row inserted into this database. This
 * is the integer primary key value of the row if the table into which it was
 * inserted has such a column.
 *
 * @note    Unique IDs may be obtained within the context of a transaction. Any
 *          attempt to obtain a unique ID before beginning a transaction will
 *          result in an assertion failure.
 *
 * @return    Unique ID of last inserted row.
 */ 
int Database::getLastInsertedUID()
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_database != NULL);
    Assert(handle.dm_nesting_level > 0);
    
    // Return the result to the caller
    return static_cast<int>(sqlite3_last_insert_rowid(handle.dm_database));
}



/**
 * Commit a transaction.
 *
 * Ends the current SQL transaction on this database by committing any changes
 * that were made.
 *
 * @note    Any attempt to commit a transaction that was not first begun with a
 *          call to beginTransaction() will result in an assertion failure.
 *
 * @note    A commit within a nest transaction does not actually occur until
 *          the outermost transaction is completed. The commit is negated if
 *          any other nested transaction within our outermost transaction is
 *          rolled back.
 */
void Database::commitTransaction()
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_database != NULL);
    Assert(handle.dm_nesting_level > 0);

    // Decrement the transaction nesting level
    handle.dm_nesting_level--;
    
    // Commit for the outermost transaction?
    if(handle.dm_nesting_level == 0) {
	
	// Execute a SQL query to commit (or rollback) the transaction
	Assert(sqlite3_exec(handle.dm_database, handle.dm_is_committable ?
			    "COMMIT TRANSACTION;" : "ROLLBACK TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);
	
    }

    //
    // Release read access for the transaction lock
    //     (indicate the in-progress transaction has completed)
    //
    Assert(pthread_rwlock_unlock(&dm_transaction_lock) == 0);
}



/**
 * Rollback a transaction.
 *
 * Ends the current SQL transaction on this database by rolling back any changes
 * that were made.
 *
 * @note    Any attempt to rollback a transaction that was not first begun with
 *          a call to beginTransaction() will result in an assertion failure.
 *
 * @note    A rollback within a nested transaction will cause <em>all</em>
 *          changes made within the context of the outermost transaction to
 *          be rolled back. This does not occur, however, until the outermost
 *          transaction has completed.
 */
void Database::rollbackTransaction()
{
    // Get our per-thread database handle
    Handle& handle = getHandle();

    // Check assertions
    Assert(handle.dm_database != NULL);
    Assert(handle.dm_nesting_level > 0);

    // Is there a current statement?
    if(handle.dm_statement != NULL) {
	
	// Reset the current statement
	Assert(sqlite3_reset(handle.dm_statement) == SQLITE_OK);	    
	
	// Indicate there is no longer a current statement
	handle.dm_statement = NULL;	    
	
    }

    // Decrement the transaction nesting level
    handle.dm_nesting_level--;
    
    // Rollback for the outermost transaction?
    if(handle.dm_nesting_level == 0) {

	// Execute a SQL query to rollback the transaction
        Assert(sqlite3_exec(handle.dm_database, "ROLLBACK TRANSACTION;",
        		    NULL, NULL, NULL) == SQLITE_OK);
	
    }
    else {

	// Simply indicate transaction is no longer committable
	handle.dm_is_committable = false;    
	
    }

    //
    // Release read access for the transaction lock
    //     (indicate the in-progress transaction has completed)
    //
    Assert(pthread_rwlock_unlock(&dm_transaction_lock) == 0);
}



/**
 * Copy a file.
 *
 * Copies the contents of one file over the contents of another. The source and
 * destination file must both already exist and the original contents of the
 * destination file are completely destroyed.
 *
 * @param source         File to be copied.
 * @param destination    File to be overwritten.
 */
void Database::copyFile(const Path& source, const Path& destination)
{
    const int copyBufferSize = 65536;
    
    // Open the source file for read-only access
    int source_fd = open(source.c_str(), O_RDONLY);
    Assert(source_fd != -1);
    
    // Open the destination file for write-only access
    int destination_fd = open(destination.c_str(), O_WRONLY | O_TRUNC);
    Assert(destination_fd != -1);
    
    // Allocate a buffer for performing the copy
    char* buffer = new char[copyBufferSize];
    
    // Perform the copy
    for(int num = 1; num > 0;) {
	
	// Read bytes from the source file
	num = read(source_fd, buffer, copyBufferSize);
	Assert((num >= 0) || ((num == -1) && (errno == EINTR)));
	
	// Write bytes until none remain
	if(num > 0)
	    for(int i = 0; i < num;) {
		
		// Write bytes to the destination file
		int written = write(destination_fd, &(buffer[i]), num - i);
		Assert((written >= 0) || ((written == -1) && (errno == EINTR)));
		
		// Update the number of bytes remaining
		if(written > 0)
		    i+= written;
		
	    }
	
    }
    
    // Destroy the copy buffer
    delete [] buffer;
    
    // Close the two files
    Assert(close(source_fd) == 0);
    Assert(close(destination_fd) == 0);    
}



/**
 * Get our per-thread database handle.
 *
 * Returns the per-thread database handle for the executing thread. A new handle
 * is created if the executing thread didn't already have a handle.
 *
 * @return    Database handle for the executing thread.
 */
Database::Handle& Database::getHandle()
{
    Guard guard_myself(this);
    
    // Find the per-thread database handle (if any) for the executing thread
    std::map<pthread_t, Handle>::iterator i =
	dm_handles.find(pthread_self());
    
    // Is there no existing handle for this thread?
    if(i == dm_handles.end()) {
	
	// Create a new per-thread database handle
	Handle handle;
	
	// Open the database
	Assert(sqlite3_open(dm_name.c_str(), &handle.dm_database) == SQLITE_OK);
	Assert(handle.dm_database != NULL);
	
	// Specify our busy handler
	Assert(sqlite3_busy_handler(handle.dm_database,
				    busyHandler, NULL) == SQLITE_OK);
	
	// Save this handle for future re-use
	i = dm_handles.insert(std::make_pair(pthread_self(), handle)).first;
	
    }
    
    // Check assertions
    Assert(i != dm_handles.end());
    
    // Return the per-thread database handle to the caller
    return i->second;    
}



/**
 * Remove all per-thread database handles.
 * 
 * Removes all per-thread database handles for this database. The prepared
 * statement caches associated with this database are also released.
 */
void Database::releaseAllHandles()
{
    Guard guard_myself(this);

    // Iterate over each per-thread database handle
    for(std::map<pthread_t, Handle>::iterator
	    i = dm_handles.begin(); i != dm_handles.end(); ++i) {
	
	// Check assertions
	Assert(i->second.dm_database != NULL);
	Assert(i->second.dm_nesting_level == 0);
	
	// Finalize all cached prepared statements
	for(std::map<std::string, sqlite3_stmt*>::const_iterator
		j = i->second.dm_cache.begin();
	    j != i->second.dm_cache.end();
	    ++j)
	    Assert(sqlite3_finalize(j->second) == SQLITE_OK);
	
	// Clear the prepared statement cache
	i->second.dm_cache.clear();

	// Close the SQLite handle for this database
	Assert(sqlite3_close(i->second.dm_database) == SQLITE_OK);    
	
    }

    // Clear the per-thread database handles
    dm_handles.clear();
}
