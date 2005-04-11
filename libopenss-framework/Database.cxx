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
#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "Time.hxx"

#include <fcntl.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <limits>
#include <pthread.h>
#include <sqlite3.h>
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
     * retry the operation. Currently the thread is suspended for 10 mS.
     *     
     * @return    Always "1" indicating SQLite should retry the operation.
     */
    int busyHandler(void*, int)
    {
	// Setup to wait for 10 mS
	struct timespec wait;
	wait.tv_sec = 0;
	wait.tv_nsec = 10 * 1000 * 1000;
	
	// Suspend ourselves temporarily
	nanosleep(&wait, NULL);
	
	// Instruct SQLite to retry the query
	return 1;
    }



    /**
     * Address addition function.
     *
     * Function used by SQLite for adding two addresses. SQLite doesn't support
     * unsigned 64-bit values directly. So addresses are "shifted" in order to
     * convert them into signed 64-bit quantities instead. Conditionals on these
     * transformed addresses can be applied directly, but arithmetic operations
     * don't work properly because of the sign change. This function implements
     * addition of two such addresses.
     *
     * @param context    Handle to the database context in which to calculate.
     * @param argc       Number of arguments.
     * @param argv       Array of arguments.
     */
    void addressAdditionFunc(sqlite3_context* context,
			     int argc, sqlite3_value** argv)
    {
	// Check assertions
	Assert(argc == 2);
	
	// Return a null if either argument wasn't an integer
	if((sqlite3_value_type(argv[0]) != SQLITE_INTEGER) ||
	   (sqlite3_value_type(argv[1]) != SQLITE_INTEGER)) {
	    sqlite3_result_null(context);
	    return;
	}
	
	// Convert the two arguments to addresses
	Address lhs = sqlite3_value_int64(argv[0]) + signedOffset;
	Address rhs = sqlite3_value_int64(argv[1]) + signedOffset;
	
	// Calculate the addition of these two addresses
	Address result = lhs + rhs;
	
	// Return the result
	sqlite3_result_int64(context, static_cast<int64_t>(result.getValue()) -
			     signedOffset);
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
 * @note    A DatabaseCannotCreate or DatabaseExists exception is thrown if the
 *          database cannot be created for any reason (including pre-existence
 *          of the named database in the later case).
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
 * Constructor from database name.
 *
 * Constructs an object for accessing the existing named database. Operations
 * can then be performed on this database by executing SQL statements using the
 * member functions of this class.
 *
 * @note    A DatabaseCannotOpen exception is thrown if the database cannot
 *          be opened for any reason.
 *
 * @param name    Name of database to be accessed.
 */
Database::Database(const std::string& name) :
    dm_lock(),
    dm_name(name),
    dm_handle(NULL),
    dm_statements(),
    dm_nesting_level(0),
    dm_is_committable(false),
    dm_current_statement(NULL)
{
    // Initialize the mutual exclusion lock
    pthread_mutexattr_t attr;
    Assert(pthread_mutexattr_init(&attr) == 0);
    Assert(pthread_mutexattr_settype(&attr,
				     PTHREAD_MUTEX_RECURSIVE_NP) == 0);    
    Assert(pthread_mutex_init(&dm_lock, &attr) == 0);
    Assert(pthread_mutexattr_destroy(&attr) == 0);
    
    // Verify the database is accessible
    if(!isAccessible(name))
	throw Exception(Exception::DatabaseCannotOpen, name);
    
    // Open the database
    Assert(sqlite3_open(name.c_str(), &dm_handle) == SQLITE_OK);
    Assert(dm_handle != NULL);
    
    // Specify our busy handler
    Assert(sqlite3_busy_handler(dm_handle, busyHandler, this) == SQLITE_OK);
    
    // Specify the user-defined address addition function
    Assert(sqlite3_create_function(dm_handle, "addrAdd", 2,
				   SQLITE_ANY, NULL, addressAdditionFunc,
				   NULL, NULL) == SQLITE_OK);
}



/**
 * Destructor.
 *
 * Closes this database. All cached prepared statements associated with this
 * database are first released.
 */
Database::~Database()
{
    // Check assertions
    Assert(dm_handle != NULL);
    Assert(dm_nesting_level == 0);
    
    // Reset and finalize all cached prepared statements
    for(std::map<std::string, sqlite3_stmt*>::const_iterator
	    i = dm_statements.begin(); i != dm_statements.end(); ++i)
	Assert(sqlite3_finalize(i->second) == SQLITE_OK);
    
    // Close the database
    Assert(sqlite3_close(dm_handle) == SQLITE_OK);    
    
    // Destroy the mutual exclusion lock
    Assert(pthread_mutex_destroy(&dm_lock) == 0);
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
    return dm_name;
}



/**
 * Begin a new transaction.
 *
 * Begins a new SQL transaction on this database. Exclusive access to the
 * Database object is obtained and the SQL implementation is asked to begin
 * a new transaction.
 *
 * @note    Only one thread may execute a transaction against a given Database
 *          at any given time. Locking is used to insure this constraint is met.
 *          Multiple threads may, however, each create their own Database for
 *          accessing a single, shared, database. Under these circumstances the
 *          locking constraints are imposed by the SQL implementation.
 *
 * @note    Nested transactions (of a sort) are implemented. A given thread may
 *          begin additional transactions before completing the previous one.
 */
void Database::beginTransaction()
{
    // Acquire exclusive access to this object
    Assert(pthread_mutex_lock(&dm_lock) == 0);
    
    // Check assertions
    Assert(dm_handle != NULL);
    
    // Is this an outer transaction?
    if(dm_nesting_level == 0) {
	
	// Execute a SQL query to begin a new transaction
	Assert(sqlite3_exec(dm_handle, "BEGIN TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);
	
	// Indicate the transaction can be committed
	dm_is_committable = true;
	
    }
    
    // Increment the transaction nesting level
    dm_nesting_level++;
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
    // Check assertions
    Assert(dm_handle != NULL);
    Assert(dm_nesting_level > 0);
    Assert(dm_current_statement == NULL);

    // Determine if this statement has already been cached
    std::map<std::string, sqlite3_stmt*>::const_iterator i =
	dm_statements.find(statement);
    if(i != dm_statements.end()) {
	
	// Use the cached prepared statement
	dm_current_statement = i->second;
	
	// Reset this statement for re-execution
	Assert(sqlite3_reset(dm_current_statement) == SQLITE_OK);
	
    }
    else {
	
	// Prepare this statement for execution
	const char* tail = NULL;	
	int retval = sqlite3_prepare(dm_handle, statement.c_str(),
				     statement.size(), &dm_current_statement,
				     &tail);
	if(retval == SQLITE_ERROR) {
	    std::string errmsg = sqlite3_errmsg(dm_handle);
	    if(errmsg.find("syntax error") == std::string::npos)
		throw Exception(Exception::DatabaseInvalid, dm_name, errmsg);
	}
	Assert(retval == SQLITE_OK);
	Assert((tail != NULL) && (*tail == '\0'));
	
	// Cache this prepared statement for future use
	dm_statements[statement] = dm_current_statement;
	
    }
    
    // Check assertions
    Assert(dm_current_statement != NULL);
    
    // Bind NULL values to all arguments of this statement
    for(int i = 1; i <= sqlite3_bind_parameter_count(dm_current_statement); ++i)
	Assert(sqlite3_bind_null(dm_current_statement, i) == SQLITE_OK);
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));

    // Bind the argument
    Assert(sqlite3_bind_text(dm_current_statement, index,
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_int(dm_current_statement, index, value) == SQLITE_OK);
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));
    
    // Bind the argument
    Assert(sqlite3_bind_double(dm_current_statement, index,
			       value) == SQLITE_OK);
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));

    // Bind the argument
    Assert(sqlite3_bind_blob(dm_current_statement, index, value.getContents(),
			     value.getSize(), SQLITE_TRANSIENT) == SQLITE_OK);
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));

    // Bind the argument
    Assert(sqlite3_bind_int64(dm_current_statement, index, 
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) && 
	   (index <= sqlite3_bind_parameter_count(dm_current_statement)));

    // Bind the argument
    Assert(sqlite3_bind_int64(dm_current_statement, index, 
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
    // Check assertions
    Assert(dm_current_statement != NULL);
    
    // Execute the next step of the current statement
    int retval = sqlite3_step(dm_current_statement);
    Assert((retval == SQLITE_ROW) || (retval == SQLITE_DONE));
    
    // Handle a completed statement
    if(retval == SQLITE_DONE) {
	
	// Indicate there is no longer a current statement
	dm_current_statement = NULL;
	
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
bool Database::getResultIsNull(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));

    // Return the result to the caller
    return sqlite3_column_type(dm_current_statement, index - 1) == SQLITE_NULL;
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
std::string Database::getResultAsString(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));

    // Return the result to the caller
    return reinterpret_cast<const char*>
	(sqlite3_column_text(dm_current_statement, index - 1));
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
int Database::getResultAsInteger(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));

    // Return the result to the caller
    return sqlite3_column_int(dm_current_statement, index - 1);
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
double Database::getResultAsReal(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));

    // Return the result to the caller
    return sqlite3_column_double(dm_current_statement, index - 1);
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
Blob Database::getResultAsBlob(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));
    
    // Return the result to the caller
    return Blob(sqlite3_column_bytes(dm_current_statement, index - 1),
		sqlite3_column_blob(dm_current_statement, index - 1));
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
Address Database::getResultAsAddress(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));
    
    // Return the result to the caller
    return Address(sqlite3_column_int64(dm_current_statement, index - 1) +
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
Time Database::getResultAsTime(const unsigned& index) const
{
    // Check assertions
    Assert(dm_current_statement != NULL);
    Assert((index > 0) &&
	   (index <= sqlite3_column_count(dm_current_statement)));
    
    // Return the result to the caller
    return Time(sqlite3_column_int64(dm_current_statement, index - 1) +
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
int Database::getLastInsertedUID() const
{
    // Check assertions
    Assert(dm_handle != NULL);
    Assert(dm_nesting_level > 0);
    
    // Return the result to the caller
    return static_cast<int>(sqlite3_last_insert_rowid(dm_handle));
}



/**
 * Commit a transaction.
 *
 * Ends the current SQL transaction on this database by committing any changes
 * that were made. After ending the current transaction, the thread's exclusive
 * lock on the Database object is released.
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
    // Check assertions
    Assert(dm_handle != NULL);
    Assert(dm_nesting_level > 0);

    // Decrement the transaction nesting level
    dm_nesting_level--;
    
    // Commit for the outermost transaction?
    if(dm_nesting_level == 0) {
	
	// Execute a SQL query to commit (or rollback) the transaction
	Assert(sqlite3_exec(dm_handle, dm_is_committable ?
			    "COMMIT TRANSACTION;" : "ROLLBACK TRANSACTION;",
			    NULL, NULL, NULL) == SQLITE_OK);
	
    }
    
    // Release exclusive access to this object
    Assert(pthread_mutex_unlock(&dm_lock) == 0);	
}



/**
 * Rollback a transaction.
 *
 * Ends the current SQL transaction on this database by rolling back any changes
 * that were made. After ending the current transaction, the thread's exclusive
 * lock on the Database object is released.
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
    // Check assertions
    Assert(dm_handle != NULL);
    Assert(dm_nesting_level > 0);

    // Is there a current statement?
    if(dm_current_statement != NULL) {
	
	// Reset the current statement
	Assert(sqlite3_reset(dm_current_statement) == SQLITE_OK);	    
	
	// Indicate there is no longer a current statement
	dm_current_statement = NULL;	    
	
    }

    // Decrement the transaction nesting level
    dm_nesting_level--;
    
    // Rollback for the outermost transaction?
    if(dm_nesting_level == 0) {

	// Execute a SQL query to rollback the transaction
        Assert(sqlite3_exec(dm_handle, "ROLLBACK TRANSACTION;",
        		    NULL, NULL, NULL) == SQLITE_OK);
	
    }
    else {

	// Simply indicate transaction is no longer committable
	dm_is_committable = false;    
	
    }
    
    // Release exclusive access to this object
    Assert(pthread_mutex_unlock(&dm_lock) == 0);
}
