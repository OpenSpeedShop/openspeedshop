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
 * Declaration of the Database class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Database_
#define _OpenSpeedShop_Framework_Database_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "NonCopyable.hxx"

#include <map>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <utility>

struct sqlite3;
struct sqlite3_stmt;



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Time;

    /**
     * SQL database.
     *
     * Class abstracting access to an SQL database. The database is opened or
     * created with read/write access upon object construction and closed upon
     * object destruction. Actual database access is accomplished via one or
     * more SQL statements executed within atomic transactions.
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
	private NonCopyable
    {

    public:

	/**
	 * Database corrupted error.
	 *
	 * Exception class that is thrown whenever a database file is found,
	 * for any reason, to be corrupted.
	 */
	class Corrupted :
	    public std::runtime_error
	{
	    
	public:

	    /** Constructor from database and details. */
	    Corrupted(const Database& database, const std::string& what) :
		std::runtime_error("Database \"" + database.getName() +
				   "\" is corrupt (" + what + ").")
	    {
	    }
  
	};

    public:
	
	explicit Database(const std::string&);
	~Database();
	
	std::string getName() const;

	void beginTransaction();
	void prepareStatement(const std::string&);
	
	void bindArgument(const unsigned&, const void*, const unsigned&);
	void bindArgument(const unsigned&, const std::string&);
	void bindArgument(const unsigned&, const int&);
	void bindArgument(const unsigned&, const double&);	
	void bindArgument(const unsigned&, const Address&);
	void bindArgument(const unsigned&, const Time&);
	
	bool executeStatement();

	bool getResultIsNull(const unsigned&) const;
	std::pair<unsigned, const void*> getResultAsBlob(const unsigned&) const;
	std::string getResultAsString(const unsigned&) const;
	int getResultAsInteger(const unsigned&) const;
	double getResultAsReal(const unsigned&) const;
	Address getResultAsAddress(const unsigned&) const;
	Time getResultAsTime(const unsigned&) const;

	int getLastInsertedUID() const;
	
	void commitTransaction();
	void rollbackTransaction();
	
    private:

	/** Mutual exclusion lock. */
	pthread_mutex_t dm_lock;
	
	/** Name of this database. */
	std::string dm_name;
	
	/** Handle for this database. */
	sqlite3* dm_handle;
	
	/** Map query strings to their prepared statement. */
	std::map<std::string, sqlite3_stmt*> dm_statements;

	/** Current transaction nesting level. */
	unsigned dm_nesting_level;

	/** Flag indicating if commit can occur. */
	bool dm_is_committable;
	
	/** Currently executing statement. */
	sqlite3_stmt* dm_current_statement;
	
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
#define BEGIN_TRANSACTION(db)    \
    try {                        \
	db->beginTransaction();  \
	if(0)



/**
 * End a transaction.
 *
 * Convenience wrapper for ending a database transaction inside a try/catch
 * caluse. Meant to be used in conjunction with BEGIN_TRANSACTION.
 *
 * @param db    Pointer to database being accessed.
 */
#define END_TRANSACTION(db)	       \
        db->commitTransaction();       \
    }				       \
    catch(...) {		       \
	db->rollbackTransaction();     \
	throw; \
    } \
    if(0)



#endif
