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

#include <map>
#include <pthread.h>
#include <string>

struct sqlite3;
struct sqlite3_stmt;



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Blob;
    class Path;
    class Time;

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
     *          64-bit quantities (such as addresses and times) are "shifted" in
     *          order to convert them into signed 64-bit quantities instead. The
     *          mapping [ UINT64_MIN, UINT64_MAX ] <--> [ INT64_MIN, INT64_MAX ]
     *          is applied in order to do this.
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
	~Database();

	void renameTo(const std::string&);
	void copyTo(const std::string&);
	
	std::string getName() const;

	void beginTransaction();
	void prepareStatement(const std::string&);
	
	void bindArgument(const unsigned&, const std::string&);
	void bindArgument(const unsigned&, const int&);
	void bindArgument(const unsigned&, const double&);
	void bindArgument(const unsigned&, const Blob&);
	void bindArgument(const unsigned&, const Address&);
	void bindArgument(const unsigned&, const Time&);
	
	bool executeStatement();

	bool getResultIsNull(const unsigned&);
	std::string getResultAsString(const unsigned&);
	int getResultAsInteger(const unsigned&);
	double getResultAsReal(const unsigned&);
	Blob getResultAsBlob(const unsigned&);
	Address getResultAsAddress(const unsigned&);
	Time getResultAsTime(const unsigned&);

	int getLastInsertedUID();
	
	void commitTransaction();
	void rollbackTransaction();
	
    private:

	static void copyFile(const Path&, const Path&);

	/** Name of this database. */
	std::string dm_name;
	
	/** Lock indicating when transactions are in-progress. */
	pthread_rwlock_t dm_transaction_lock;	
	
	/**
	 * Database handle.
	 *
	 * Structure for a per-thread database handle. Contains an SQLite
	 * handle for the database, a cache of prepared statements, and various
	 * information about the statement currently being executed by the
	 * thread (if any).
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
	    std::map<std::string, sqlite3_stmt*> dm_cache;

	    /** Currently executing statement. */
	    sqlite3_stmt* dm_statement;
	    	    
	    /** Current transaction nesting level. */
	    unsigned dm_nesting_level;
	    
	    /** Flag indicating if commit can occur. */
	    bool dm_is_committable;

	    /** Default constructor. */
	    Handle() :
		dm_database(NULL),
		dm_cache(),
		dm_statement(NULL),
		dm_nesting_level(0),
		dm_is_committable(false)
	    {
	    }

	};

	/** Map threads to their database handle. */	
	std::map<pthread_t, Handle> dm_handles;

	Handle& getHandle();
	void releaseAllHandles();

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
 * End a transaction.
 *
 * Convenience wrapper for ending a database transaction inside a try/catch
 * clause. Meant to be used in conjunction with BEGIN_TRANSACTION.
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
