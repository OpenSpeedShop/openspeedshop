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
 * Declaration of the Database class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Database_
#define _OpenSpeedShop_Framework_Database_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "Time.hxx"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <map>
#include <pthread.h>
#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Blob;
    class Path;

    /**
     * SQL database.
     *
     * Class abstracting access to an SQL database. The database is opened
     * for read/write access upon object construction and closed upon object
     * destruction. Actual database access is accomplished via one or more
     * SQL statements executed within atomic transactions.
     *
     * @note    The interface of this class is designed to be independent of the
     *          particular SQL implementation that is used. Currently this class
     *          is implemented using SQLite. In the future this could be changed
     *          to, for example, MySQL, if deeemed appropriate or necessary.
     *
     * @note    SQLite doesn't support unsigned 64-bit values directly. In order
     *          to take full advantage of SQL queries against the data, unsigned
     *          64-bit quantities (such as addresses) are "shifted" in order to
     *          convert them into signed 64-bit quantities instead. The mapping
     *          [ UINT64_MIN, UINT64_MAX ] <--> [ INT64_MIN, INT64_MAX ] is 
     *          applied in order to do this.
     *
     * @sa    http://www.sql-tutorial.net/
     * @sa    http://www.sqlite.org/
     *
     * @ingroup Utility
     */
    class Database :
	private Lockable
    {

    public:

	static bool isAccessible(const std::string&);
	static void create(const std::string&);
	static void remove(const std::string&);
	
	explicit Database(const std::string&);
	virtual ~Database();

	void renameTo(const std::string&);
	void copyTo(const std::string&);
	
	std::string getName() const;

	void beginTransaction(const bool& = false);
	void prepareStatement(const std::string&);
	
	void bindArgument(const unsigned&, const std::string&);
	void bindArgument(const unsigned&, const int&);
	void bindArgument(const unsigned&, const double&);
	void bindArgument(const unsigned&, const Blob&);
	void bindArgument(const unsigned&, const Address&);
	void bindArgument(const unsigned&, const Time&);
	void bindArgument(const unsigned&, const pthread_t&);
	
	bool executeStatement();

	bool getResultIsNull(const unsigned&);
	std::string getResultAsString(const unsigned&);
	int getResultAsInteger(const unsigned&);
	double getResultAsReal(const unsigned&);
	Blob getResultAsBlob(const unsigned&);
	Address getResultAsAddress(const unsigned&);
	Time getResultAsTime(const unsigned&);
	pthread_t getResultAsPosixThreadId(const unsigned&);

	int getLastInsertedUID();
	
	void commitTransaction();
	void rollbackTransaction();

	void vacuum();
	
    private:

#ifndef NDEBUG
	/** Map SQL statements to their total execution count and time. */
	std::map<std::string, std::pair<uint64_t, uint64_t> > dm_debug_stats;
#endif

	static void copyFile(const Path&, const Path&);

	/** Name of this database. */
	std::string dm_name;
	
	/** Lock indicating when transactions are in-progress. */
	pthread_rwlock_t dm_transaction_lock;	
	
	/**
	 * Database handle.
	 *
	 * Structure for a per-thread database handle. Contains an SQLite
	 * handle for the database, a cache of prepared statements, and
	 * information about the transaction currently being executed by
	 * the thread (if any).
	 *
	 * @note    Database handles are maintained on a per-thread basis mainly
	 *          because SQLite does not allow a database handle created via
	 *          sqlite_open() to be passed between threads. SQLite versions
	 *          3.2.5 and above actually check for this and return an error
	 *          (SQLITE_MISUSE) if a thread tries to use a handle that was
	 *          created by a different thread.
	 *
	 * @sa    http://www.hwaci.com/sw/sqlite/faq.html#q8
	 * @sa    http://www.sqlite.org/cvstrac/chngview?cn=2517
	 */
	struct Handle
	{

	    /** SQLite handle for this database. */
	    sqlite3* dm_database;
	    
	    /** Map query strings to their cached prepared statement. */
	    std::multimap<std::string, sqlite3_stmt*> dm_cache;
	    
	    /**
	     * Transaction stack.
	     *
	     * Stack for tracking the currently executing transaction. If the
	     * stack is empty, no transaction is in progress. When an initial,
	     * outer-most, transaction is started, a null statement is pushed
	     * onto this stack. Each statement is pushed onto this stack as it
	     * is prepared for execution, stays on the stack while it executes,
	     * and is popped after its execution completes. Additional nested
	     * transactions are indicated by pushing additional null statements
	     * onto the stack.
	     */
	    std::vector<sqlite3_stmt*> dm_transaction;

#ifndef NDEBUG
	    /** Start times of the statements on the transaction stack. */
	    std::vector<Time> dm_debug_start;
	    
	    /** Map SQL statements to their total execution count and time. */
	    std::map<sqlite3_stmt*, 
		     std::pair<uint64_t, uint64_t> > dm_debug_stats;
#endif

	    /** Flag indicating if outer-most transaction is commitable. */
	    bool dm_is_committable;
	    
	    /** Default constructor. */
	    Handle() :
		dm_database(NULL),
		dm_cache(),
		dm_transaction(),
#ifndef NDEBUG
		dm_debug_start(),
		dm_debug_stats(),
#endif
		dm_is_committable(false)
	    {
	    }

	};

	/** Map threads to their database handle. */	
	std::map<pthread_t, Handle> dm_handles;

	Handle& getHandle();
	void releaseAllHandles();

#ifndef NDEBUG
	static bool is_debug_enabled;
	
	void debugUpdatePerThread(Handle&);
	void debugUpdatePerDatabase(Handle&);
	void debugPerformanceStatistics();
#endif

    };
    
} }



/**
 * Begin a transaction.
 *
 * Convenience wrapper for beginning a database transaction inside a try/catch
 * clause. Meant to be used in conjunction with END_TRANSACTION.
 *
 * @param db    Pointer to database being accessed.
 */
#define BEGIN_TRANSACTION(db)   \
    try {                       \
	db->beginTransaction(); \
	if(0)



/**
 * Begin a write transaction.
 *
 * Convenience wrapper for beginning a database transaction inside a try/catch
 * clause that will be modifying the database. Meant to be used in conjunction
 * with END_TRANSACTION.
 *
 * @param db    Pointer to database being accessed.
 */
#define BEGIN_WRITE_TRANSACTION(db) \
    try {                           \
	db->beginTransaction(true); \
	if(0)



/**
 * End a transaction.
 *
 * Convenience wrapper for ending a database transaction inside a try/catch
 * clause. Meant to be used in conjunction with either BEGIN_TRANSACTION or
 * BEGIN_WRITE_TRANSACTION.
 *
 * @param db    Pointer to database being accessed.
 */
#define END_TRANSACTION(db)	   \
        db->commitTransaction();   \
    }				   \
    catch(...) {		   \
	db->rollbackTransaction(); \
	throw;                     \
    }                              \
    if(0)



#endif
