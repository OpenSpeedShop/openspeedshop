////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2015  The Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the Callbacks namespace.
 *
 */


#include "Address.hxx"
#include "AddressBitmap.hxx"
#include "AddressRange.hxx"
#include "Blob.hxx"
#include "Callbacks.hxx"
#include "Database.hxx"
#include "DataQueues.hxx"
#include "DyninstSymbols.hxx"
#include "Extent.hxx"
#include "EntrySpy.hxx"
#include "Frontend.hxx"
#include "LinkedObject.hxx"
#include "SmartPtr.hxx"
#include "Path.hxx"
#include "PCBuffer.hxx"
#include "SymbolTable.hxx"
#include "SymtabAPISymbols.hxx"
#include "ThreadGroup.hxx"
#include "ThreadTable.hxx"
#include "Time.hxx"
#include "Utility.hxx"

#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/SignalAdapter.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>
#include <map>
#include <string>

// FIXME: Until we understand why the symboltables created by
// cbtf components and passed to the cbtf instrumentor do not
// work with Thread::getFunctionAt for dsos with a base offset of 0
// we will use the symbol resolver in these callbacks.
// Another issue is that resolving loops may be better done once
// in this FE rather than N times in the leaf CP component nodes. 
#define OPENSS_CBTF_FE_RESOLVE_SYMBOLS 1

using namespace OpenSpeedShop::Framework;
using namespace KrellInstitute::Core;

// mapping of function to thread and value.
typedef std::map<std::string,std::pair<ThreadName,uint64_t> > FunctionThreadCount;

// mapping of function to total value and total number of threads.
typedef std::map<std::string,std::pair<uint64_t,uint64_t> > FunctionAvgMap;

namespace OpenSpeedShop { namespace Framework {
    typedef std::map<AddressRange, std::pair<SymbolTable, std::set<LinkedObject> > > SymbolTableMap;
    static std::map<AddressRange, std::string> allfuncs;
//    std::vector<OpenSpeedShop::Framework::Blob> pending_data;
}}

namespace {

    void convert(const ThreadName& in, CBTF_Protocol_ThreadName& out)
    {
        out.experiment = 0;
        OpenSpeedShop::Framework::convert(in.getHost(), out.host);
        out.pid = in.getPid();
        std::pair<bool, pthread_t> posix_tid = in.getPosixThreadId();
        out.has_posix_tid = posix_tid.first;
        if(posix_tid.first)
            out.posix_tid = posix_tid.second;
        out.rank = in.getMPIRank();
        out.omp_tid = in.getOmpTid();
    }



    /**
     * Get a linked object's identifier.
     *
     * Returns the identifier for the specified linked object's file name
     * in the passed database. An invalid identifier (-1) is returned if the
     * linked object does not exist.
     *
     * @param database         Database containing the linked object.
     * @param linked_object    Name of the linked object's file.
     * @return                 Identifier of the linked object or an invalid
     *                         identifier (-1) if no such linked object is
     *                         found.            
     */
    int getLinkedObjectIdentifier(SmartPtr<Database>& database,
				  const CBTF_Protocol_FileName& linked_object)
    {
	int identifier = -1;

	// Find the identifier of the specified linked object's file name
	BEGIN_TRANSACTION(database);
	database->prepareStatement(
	    "SELECT LinkedObjects.id "
	    "FROM LinkedObjects "
	    "    JOIN Files "
	    "ON LinkedObjects.file = Files.id "
	    "WHERE Files.path = ?;"
	    );
	database->bindArgument(1, linked_object.path);

	// TODO: compare the checksum
	
	while(database->executeStatement())
	    identifier = database->getResultAsInteger(1);
	END_TRANSACTION(database);
	
	// Return the identifier to the caller
	return identifier;
    }



    /**
     * Get a thread's identifier.
     *
     * Returns the identifier for the specified thread name in the passed
     * database. An invalid identifier (-1) is returned if the thread does
     * not exist.
     *
     * @param database    Database containing the thread.
     * @param thread      Name of the thread.
     * @return            Identifier of the thread or an invalid identifier
     *                    (-1) if no such thread is found.
     */
    int getThreadIdentifier(SmartPtr<Database>& database,
			    const CBTF_Protocol_ThreadName& thread)
			    
    {
	int identifier = -1;

	// Find the identifier of the specified thread name
	BEGIN_TRANSACTION(database);
	if(thread.has_posix_tid)
	    database->prepareStatement(
	        "SELECT id "
		"FROM Threads "
		"WHERE host = ? "
		"  AND pid = ? "
		"  AND posix_tid = ?;"
		);
	else
	    database->prepareStatement(
	        "SELECT id FROM Threads WHERE host = ? AND pid = ?;"
		);
	database->bindArgument(1, thread.host);
	database->bindArgument(2, static_cast<int>(thread.pid));
	if(thread.has_posix_tid)
	    database->bindArgument(3, static_cast<pthread_t>(thread.posix_tid));
	while(database->executeStatement())
	    identifier = database->getResultAsInteger(1);
	END_TRANSACTION(database);

	// Return the identifier to the caller
	return identifier;
    }

    ThreadGroup getThreads(SmartPtr<Database>& database)
    {
	ThreadGroup threads;
	BEGIN_TRANSACTION(database);
	database->prepareStatement("SELECT id FROM Threads;");
	while(database->executeStatement())
            threads.insert(Thread(database, database->getResultAsInteger(1)));
	END_TRANSACTION(database);
	return threads;
    }


    /**
     * Store a view to the database that represents the view_data of the pass view_cmd.
     * This is essentially a simple insert since no views will be present
     * in the views table for the just created database.
     *
     * TODO:  The view data for a max and min contain a thread descriptor.
     * At the time this view is added to the database there is a thread
     * table. The thread discriptor could be used as a lookup into the
     * thread table and replaced with the table id in the view data. This
     * would then be used to lookup the thread in any view code from the
     * thread table rather than using the string descriptor of the thread.
     * The function names in the viewdata could be resolved to their table
     * id in the functions table as well (as long as they are already present
     * in the functions table).
     *
     * @return   The success/failure return value.
     */

    bool addView(SmartPtr<Database>& database, std::string& viewcommand, std::string& viewdata )
    {
#ifndef NDEBUG
	std::stringstream output;
	if(Frontend::isMetricDebugEnabled()) {
	 std::stringstream output;
	 output << "addView INSERT view viewcommand:" << viewcommand 
		<< " data:" << viewdata
		<< " data size:" << viewdata.size()
		<< std::endl;
	 std::cerr << output.str();
	}
#endif

	int viewid = -1;

	BEGIN_WRITE_TRANSACTION(database);

	database->prepareStatement(
	      "INSERT INTO Views "
			"  (view_cmd, view_data) "
			"VALUES (?, ?);"
	);
	database->bindArgument(1, viewcommand);
	database->bindArgument(2, viewdata);
	while(database->executeStatement());
	viewid = database->getLastInsertedUID();
	
	END_TRANSACTION(database);

#ifndef NDEBUG
	if(Frontend::isMetricDebugEnabled()) {
	 std::stringstream output;
	 output << "EXIT addView"
		<< " view command:" << viewcommand
		<< " view id:" << viewid
		<< std::endl;
	 std::cerr << output.str();
	}
#endif

	return true;

    }

    LinkedObjectEntryVec linkedobjectvec;
    std::vector<ThreadName> threadvec;
    std::vector<CBTF_Protocol_SymbolTable> symvec;
    std::set<OpenSpeedShop::Framework::Address> addresses;
    AddressBuffer abuffer;

    int pending_blobs = 0;
    int num_linkedobjectgroups = 0;
    int num_attachedthreads = 0;
}

void process_symvec(const CBTF_Protocol_SymbolTable& message) {
#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {

	std::cerr << "TIME " << KrellInstitute::Core::Time::Now()
	<< " CLIENT:" << getpid()
	<< " Callbacks process_symvec entered"
	<< std::endl;
    }
#endif
	// Begin a transaction on this experiment's database
        SmartPtr<Database> database = DataQueues::getDatabase(0);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isSymbolsDebugEnabled()) {
		std::cerr << "CLIENT:" << getpid()
		<< " Callbacks::process_symvec Experiment "
		<< " no longer exists."
		<< std::endl;
	    }
#endif

	}
	BEGIN_WRITE_TRANSACTION(database);
	
	// Find the existing linked object in the database
	int linked_object = 
	    getLinkedObjectIdentifier(database, message.linked_object);
#ifndef NDEBUG
	if(linked_object == -1) {
	    if(Frontend::isSymbolsDebugEnabled()) {
		std::cerr << "CLIENT:" << getpid()
		<< " Callbacks::process_symvec Linked Object "
		<< toString(message.linked_object) 
		<< " no longer exists."
		<< std::endl;
	    }
	} else {
	    if(Frontend::isSymbolsDebugEnabled()) {
		std::cerr << "CLIENT:" << getpid()
		<< " Callbacks::process_symvec Linked Object "
		<< message.linked_object.path << " processed for symbols."
		<< std::endl;
	    }
	}
#endif

	// Only proceed if the linked object was found
	if(linked_object != -1) {

	    // Are there functions in the database for this linked object?
	    bool has_functions = false;
	    database->prepareStatement(
	        "SELECT COUNT(*) FROM Functions WHERE linked_object = ?;"
	        );
	    database->bindArgument(1, linked_object);
	    while(database->executeStatement())
		if(database->getResultAsInteger(1) > 0)
		    has_functions = true;

	    // Skip adding these functions if they are already present
	    if(!has_functions) {
	  
		// Iterate over each function entry
		for(int j = 0; j < message.functions.functions_len; ++j) {
		    const CBTF_Protocol_FunctionEntry& msg_function = 
			message.functions.functions_val[j];

#ifndef NDEBUG
		    if(Frontend::isSymbolsDebugEnabled()) {
		    std::cerr << "CLIENT:" << getpid()
		    << " Callbacks::process_symvec INSERTS FUNCTION " << msg_function.name
		    << std::endl;
		    }
#endif

		    // Create the function entry
		    database->prepareStatement(
		        "INSERT INTO Functions "
			"  (linked_object, name) "
			"VALUES (?, ?);"
		        );
		    database->bindArgument(1, linked_object);
		    database->bindArgument(2, msg_function.name);
		    while(database->executeStatement());
		    int function = database->getLastInsertedUID();
	    
		    // Iterate over each bitmap for this function
		    for(int k = 0; k < msg_function.bitmaps.bitmaps_len; ++k) {
			const CBTF_Protocol_AddressBitmap& msg_bitmap =
			    msg_function.bitmaps.bitmaps_val[k];

#ifndef NDEBUG
			if(Frontend::isSymbolsDebugEnabled()) {
			std::cerr << "CLIENT:" << getpid()
			<< " Callbacks::process_symvec INSERTS FUNCTIONRANGES " << msg_function.name
			<< " range:" << OpenSpeedShop::Framework::AddressRange(msg_bitmap.range.begin,msg_bitmap.range.end)
			<< " valid_bitmap data len:" << msg_bitmap.bitmap.data.data_len
		        << std::endl;
			}
#endif

			// Create the function ranges entry
			database->prepareStatement(
		            "INSERT INTO FunctionRanges "
			    "  (function, addr_begin, addr_end, valid_bitmap) "
			    "VALUES (?, ?, ?, ?);"
			    );
			database->bindArgument(1, function);
			database->bindArgument(2,
			    OpenSpeedShop::Framework::Address(msg_bitmap.range.begin)
			    );
			database->bindArgument(3,
			    OpenSpeedShop::Framework::Address(msg_bitmap.range.end)
			    );
			database->bindArgument(4, 
			    Blob(msg_bitmap.bitmap.data.data_len,
				 msg_bitmap.bitmap.data.data_val)
			    );
			while(database->executeStatement());
			
		    }
		    
		}
		
	    } else {
#ifndef NDEBUG
		if(Frontend::isSymbolsDebugEnabled()) {
		    std::cerr << "CLIENT:" << getpid()
		    << " Callbacks::process_symvec() Experiment "
		    << " DB already has these functions!." << std::endl;
		}
#endif
	    }
	    
	    // Are there statements in the database for this linked object?
	    bool has_statements = false;
	    database->prepareStatement(
                "SELECT COUNT(*) FROM Statements WHERE linked_object = ?;"
	        );
	    database->bindArgument(1, linked_object);
	    while(database->executeStatement())
		if(database->getResultAsInteger(1) > 0)
		    has_statements = true;

	    // Skip adding these statements if they are already present
	    if(!has_statements) {
#ifndef NDEBUG
		if(Frontend::isSymbolsDebugEnabled()) {
		    std::cerr << "CLIENT:" << getpid()
		    << " Callbacks::process_symvec message.statements.statements_len "
		    << message.statements.statements_len
		    << std::endl;
		}
#endif

		// Iterate over each statement entry
		for(int j = 0; j < message.statements.statements_len; ++j) {
		    const CBTF_Protocol_StatementEntry& msg_statement =
			message.statements.statements_val[j];

#ifndef NDEBUG
		    if(Frontend::isSymbolsDebugEnabled()) {
			std::cerr << "CLIENT:" << getpid()
			<< " Callbacks::process_symvec"
			<< " FILE " << msg_statement.path.path
			<< " LINE " << msg_statement.line
			<< " COL " << msg_statement.column
			<< std::endl;
		    }
#endif
		    
		    // Is there an existing file in the database?
		    int file = -1;
		    database->prepareStatement(
		        "SELECT id FROM Files WHERE path = ?;"
		        );
		    database->bindArgument(1, msg_statement.path.path);

		    // TODO: compare the checksum
		    
		    while(database->executeStatement())
			file = database->getResultAsInteger(1);

		    // Create the file entry if it wasn't present
		    if(file == -1) {
#ifndef NDEBUG
		    if(Frontend::isSymbolsDebugEnabled()) {
			std::cerr << "CLIENT:" << getpid()
			<< " Callbacks::process_symvec INSERTS FILE " << msg_statement.path.path
			<< std::endl;
		    }
#endif

			database->prepareStatement(
		            "INSERT INTO Files (path) VALUES (?);"
			    );
			database->bindArgument(1, msg_statement.path.path);

			// TODO: insert the checksum
			
			while(database->executeStatement());
			file = database->getLastInsertedUID();
		    }

		    // Create the statement entry
#ifndef NDEBUG
		    if(Frontend::isSymbolsDebugEnabled()) {
		    std::cerr << "CLIENT:" << getpid()
		    << " Callbacks::process_symvec INSERTS STATEMENT " << file
		    << ":" << msg_statement.line << ":" << msg_statement.column
		    << std::endl;
		    }
#endif

		    database->prepareStatement(
	                "INSERT INTO Statements "
		        "  (linked_object, file, line, \"column\") "
		        "VALUES (?, ?, ?, ?);"
		        );
		    database->bindArgument(1, linked_object);
		    database->bindArgument(2, file);
		    database->bindArgument(3, msg_statement.line);
		    database->bindArgument(4, msg_statement.column);
		    while(database->executeStatement());
		    int statement = database->getLastInsertedUID();

		    // Iterate over each bitmap for this statement
		    for(int k = 0; k < msg_statement.bitmaps.bitmaps_len; ++k) {
			const CBTF_Protocol_AddressBitmap& msg_bitmap =
			    msg_statement.bitmaps.bitmaps_val[k];
		
			// Create the statement ranges entry
			database->prepareStatement(
		            "INSERT INTO StatementRanges "
			    "  (statement, addr_begin, addr_end, valid_bitmap) "
			    "VALUES (?, ?, ?, ?);"
			    );
			database->bindArgument(1, statement);
			database->bindArgument(2,
			    OpenSpeedShop::Framework::Address(msg_bitmap.range.begin)
			    );
			database->bindArgument(3,
			    OpenSpeedShop::Framework::Address(msg_bitmap.range.end)
			    );
			database->bindArgument(4, 
		            Blob(msg_bitmap.bitmap.data.data_len,
				 msg_bitmap.bitmap.data.data_val)
			    );
			while(database->executeStatement());
		    
		    }
		    
		}
		
	    } else {
#ifndef NDEBUG
		if(Frontend::isSymbolsDebugEnabled()) {
		    std::cerr << "CLIENT:" << getpid()
		    << " Callbacks::process_symvec Experiment "
		    << " DB already has these statements!." << std::endl;
		}
#endif
	    }

	}
	
	// End the transaction on this thread's database
	END_TRANSACTION(database);
	
#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::cerr << "TIME " << KrellInstitute::Core::Time::Now()
	    << " CLIENT:" << getpid() << " Callbacks process_symvec() exits"
	    << std::endl;
    }
#endif
}

// Helper function to do bulk updates of the thread table from a vector
// of ThreadNames created by the attachToThreads callback.
// Threads are queued in the threadvec and not flush to the database until
// we need to (like when a data blob needs to be added to the DataQueue).
bool updateThreads()
{
    // thread vector is empty so no need to access database.
    if (threadvec.empty()) {
#ifndef NDEBUG
      if(Frontend::isDebugEnabled()) {
	std::cerr << "EARLY EXIT FROM updateThreads -- threadvec.empty" << std::endl;
      }
#endif
	return false;
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << OpenSpeedShop::Framework::Time::Now()
	<< " CLIENT:" << getpid() << " entered updateThreads with "
	<< threadvec.size() << " threads" << std::endl;
	std::cerr << output.str();
    }
#endif

	// Begin a transaction on this thread's database
        SmartPtr<Database> database = DataQueues::getDatabase(0);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "CLIENT:" << getpid()
		<< " updateThreads(): Experiment " << 0 
		<< " no longer exists." << std::endl;
		std::cerr << output.str();
	    }
#endif

	   return false;
	}

    // This can be potentially large so do it in one transaction!
    BEGIN_WRITE_TRANSACTION(database);
    std::vector<ThreadName>::const_iterator i;
    for(i=threadvec.begin(); i != threadvec.end(); i++)
    {
        std::string host = Experiment::getCanonicalName(i->getHost());
	
	// TODO: for CBTF there is no longer an initial created process
	// message passed up. CBTF now passes one large message
	// representing all threads if possible. So the initial
	// test will not result in any entry found from the database.
	// So this logic could be removed in the future.
	//
	// Is there an existing placeholder for the entire process?
	// The initial createdprocess creates the database entry for
	// a process with the posix_tid of null. The initial thread
	// will be updated with it's posix_tid.  All new threads
	// will look for an exiting host:pid in the database and if
	// one is found, a new thread entry in the data base will
	// be created.
	int thread = -1;
	database->prepareStatement(
	    "SELECT id "
	    "FROM Threads "
	    "WHERE host = ? "
	    "  AND pid = ? "
	    "  AND posix_tid ISNULL;"
	    );
	database->bindArgument(1, host);
	database->bindArgument(2, static_cast<int>((*i).getPid()));
	while(database->executeStatement())
	    thread = database->getResultAsInteger(1);

	// Reuse the placeholder if appropriate
	if(thread != -1) {
	    if((*i).getPosixThreadId().first && (thread != -1)) {

#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    std::stringstream output;
		    output << "CLIENT::" << getpid()
		       << " updateThreads() UPDATE Threads SET "
		       << " rank:" << (*i).getMPIRank()
		       << " posix_tid:" << (*i).getPosixThreadId().second
		       << " omp_tid:" << (*i).getOmpTid()
		       << " For thread id:" << thread << std::endl;
		    std::cerr << output.str();
		}
#endif

		database->prepareStatement(
		    "UPDATE Threads SET posix_tid = ? WHERE id = ?;"
		    );
		database->bindArgument(
		    1, static_cast<pthread_t>((*i).getPosixThreadId().second)
		    );
		database->bindArgument(2, thread);
		while(database->executeStatement());

		if ((*i).getMPIRank() >= 0 ) {
		  database->prepareStatement(
		    "UPDATE Threads SET mpi_rank = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<int>((*i).getMPIRank())
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}

		if ((*i).getOmpTid() >= 0 ) {
		  database->prepareStatement(
		    "UPDATE Threads SET openmp_tid = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<int>((*i).getOmpTid())
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}
	    }
	}

	// See if there is already an entry for this host and pid.
	// Create a new thread entry for this posix_tid for this host:pid
	// and include the mpi_rank. For non-mpi jobs mpi_rank is -1 and
	// is already set to that by the collector. The openmp_tid is
	// an integer representing a thread and is used as a simple
	// identifier for pthreads as well.
	else {

	thread = -1;
	database->prepareStatement(
	    "SELECT id "
	    "FROM Threads "
	    "WHERE host = ? "
	    "  AND pid = ? "
	    "  AND posix_tid = ? "
	    "  AND mpi_rank = ? "
	    "  AND openmp_tid = ? "
	    );

	database->bindArgument(1, host);
	database->bindArgument(2, static_cast<int>((*i).getPid()));
	database->bindArgument(3, static_cast<pthread_t>((*i).getPosixThreadId().second));
	database->bindArgument(4, static_cast<int>((*i).getMPIRank()));
	database->bindArgument(5, static_cast<int>((*i).getOmpTid()));

	while(database->executeStatement())
	    thread = database->getResultAsInteger(1);

	    if( thread != -1) {
#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    std::stringstream output;
		    output << "CLIENT:" << getpid()
		       << " updateThreads: FOUND existing thread in database"
		       << " " << host
		       << ":" << (*i).getPid()
		       << ":" << (*i).getPosixThreadId().second
		       << ":" << (*i).getMPIRank()
		       << ":" << (*i).getOmpTid()
			<< std::endl;
		    std::cerr << output.str();
		}
#endif
	    } else if (thread == -1 ) {
#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    std::stringstream output;
		    output << "CLIENT:" << getpid()
		       << " updateThreads: INSERT INTO Threads"
		       << " " << host
		       << ":" << (*i).getPid()
		       << ":" << (*i).getPosixThreadId().second
		       << ":" << (*i).getMPIRank()
		       << ":" << (*i).getOmpTid()
			<< std::endl;
		    std::cerr << output.str();
		}
#endif
		database->prepareStatement(
		    "INSERT INTO Threads (host, pid, posix_tid, openmp_tid) VALUES (?, ?, ?, ?);"
		    );
	        database->bindArgument(1, host);
	        database->bindArgument(2, static_cast<int>((*i).getPid()));
		database->bindArgument(3, static_cast<pthread_t>((*i).getPosixThreadId().second));
	        database->bindArgument(4, static_cast<int>((*i).getOmpTid()));
	        while(database->executeStatement());

	        int thread = database->getLastInsertedUID();
	        ThreadTable::TheTable.addThread(Thread(database, thread));

		if ((*i).getMPIRank() >= 0) {
		  database->prepareStatement(
		    "UPDATE Threads SET mpi_rank = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<int>((*i).getMPIRank())
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}

	    } else {
		// Must be a new host pid.  Add it to the thread table.
		// For CBTF, we likely will not enter this case.
#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    std::stringstream output;
		    output << "CLIENT " << getpid()
		       << " updateThreads(): New Thread - INSERTING"
		       << " " << host
		       << ":" << (*i).getPid()
		       << ":" << (*i).getMPIRank()
		       << ":" << static_cast<pthread_t>((*i).getPosixThreadId().second)
		       << ":" << (*i).getOmpTid()
		       << std::endl;
		    std::cerr << output.str();
		}
#endif
		database->prepareStatement(
		    "INSERT INTO Threads (host, pid) VALUES (?, ?);"
		    );

	        database->bindArgument(1, host);
	        database->bindArgument(2, static_cast<int>((*i).getPid()));
	        while(database->executeStatement());

	        int thread = database->getLastInsertedUID();
	        ThreadTable::TheTable.addThread(Thread(database, thread));

		if ((*i).getPosixThreadId().first ) {
		  database->prepareStatement(
		    "UPDATE Threads SET posix_tid = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<pthread_t>((*i).getPosixThreadId().second)
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}

		if ((*i).getMPIRank() >= 0) {
		  database->prepareStatement(
		    "UPDATE Threads SET mpi_rank = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<int>((*i).getMPIRank())
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}

		if ((*i).getOmpTid() >= 0 ) {
		  database->prepareStatement(
		    "UPDATE Threads SET openmp_tid = ? WHERE id = ?;"
		    );
		  database->bindArgument(
		    1, static_cast<int>((*i).getOmpTid())
		    );
		  database->bindArgument(2, thread);
		  while(database->executeStatement());
		}
	    }
	}
	// End the transaction on this thread's database
    }
    END_TRANSACTION(database);

    // clear for any potential new threads
    threadvec.clear();
    return true;
}


/**
 * Attached to threads.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates threads have been attached is received. Updates the experiment
 * databases as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::attachedToThreads(const boost::shared_ptr<CBTF_Protocol_AttachedToThreads> & in)
{
    // Decode the message
    CBTF_Protocol_AttachedToThreads message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_AttachedToThreads)); 

    num_attachedthreads += message.threads.names.names_len;

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now()
		<< " CLIENT:" << getpid() << " Callbacks::attachedToThreads"
		<< " ENTERED message threads:" << message.threads.names.names_len
		<< " num_attachedthreads:" << num_attachedthreads
		<< std::endl;
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread attached in this message
    // and add it to vector for later updating of database.
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	threadvec.push_back(message.threads.names.names_val[i]);
    }

    updateThreads();

}

/**
 * Created a process.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a process was created is received. Updates the experiment databases
 * as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::createdProcess(const boost::shared_ptr<CBTF_Protocol_CreatedProcess> & in)
{


    // This is a holdover from online instrumentation and in particular
    // is not needed for offline type instrumentation like cbtf collectors use.
    // So we will return early here.  Likely we do not need to send
    // this message for cbtf/offline collection as it is redundant and
    // causes uneeded message traffic and database updates.  all the info
    // we need to update the thread table in the database comes from the
    // attachedToThreads messages.
    return;

    // Decode the message
    CBTF_Protocol_CreatedProcess message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_CreatedProcess)); 

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid() << " Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Update the thread with its real process identifier
    SmartPtr<Database> database = 
	DataQueues::getDatabase(message.original_thread.experiment);
    if(database.isNull()) {
	
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "CLIENT:" << getpid() << " Callbacks::"
		   << "createdProcess(): Experiment " 
		   << message.original_thread.experiment 
		   << " no longer exists." << std::endl;
	    std::cerr << output.str();
	}
#endif

	return;
    }

    // Is there an existing placeholder for the entire process?
    BEGIN_WRITE_TRANSACTION(database);
        database->prepareStatement(
            "INSERT INTO Threads (host,pid) VALUES (?,?);"
            );
        database->bindArgument(1, (message.created_thread.host));
        database->bindArgument(2, static_cast<int>(message.created_thread.pid));
	while(database->executeStatement());

    END_TRANSACTION(database);
}

/**
 * When all threads have terminated, this address buffer is sent,
 *
 * @param in    address buffer containing the unique addresses with counts.
 */
void Callbacks::addressBuffer(const AddressBuffer& in)
{

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::cerr << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::addressBuffer entered"
	<< " with " << in.addresscounts.size() << " addresses"
	 << std::endl;
    }
#endif

    // Update thread table for last time.
    updateThreads();

    // This message come after all threads have finished and therefore
    // all datablobs have been sent and are queued.
    // Flush the dataqueues now for last time.
    DataQueues::flushPerformanceData();


    // Create a set of addresses in the passed buffer.
    // We do this so we can use the OSS symbol resolution functions.
    AddressCounts::const_iterator aci;
    AddressCounts ac = in.addresscounts;
    abuffer = in;
 
    // create the set of addresses used to restrict entries into the database
    // to those that contain a valid sample or sample related address.
    //std::set<OpenSpeedShop::Framework::Address> addresses;
    for (aci = ac.begin(); aci != ac.end(); ++aci) {
	addresses.insert(aci->first.getValue());
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::cerr << "TIME " << Time::Now() << " CLIENT:" << getpid()
	       << " Callbacks::addressBuffer exits" << std::endl;
    }
#endif
}

void Callbacks::finalize() {
    // this updates the addresspace table.
    process_addressspace(addresses);


    // For our purposes there is only the one experiment (0).
    SmartPtr<Database> database = DataQueues::getDatabase(0);

    OpenSpeedShop::Framework::SymbolTableMap symtabmap;

    // at this point we should have recorded threads into the
    // database. So we will just get the list of threads from
    // the database.  This should be safe since at the end of
    // an experiment all threads will already be recorded into
    // the database and all individual threads will dump
    // any remaining performance data blobs prior to exiting.
    ThreadGroup threads;

    BEGIN_TRANSACTION(database);
	database->prepareStatement("SELECT id FROM Threads;");
	while(database->executeStatement())
	    threads.insert(Thread(database, database->getResultAsInteger(1)));
    END_TRANSACTION(database);

    // Now get the currently recorded linkedobjects for these threads
    // from the database. 
    std::set<LinkedObject> ttgrp_lo = threads.getLinkedObjects();

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::cerr << "TIME " << Time::Now() << " CLIENT:" << getpid()
	       << " Callbacks::finalize find symbol tables for "
	       << ttgrp_lo.size() << " linked objects from  "
	       << threads.size() << " threads."
	       << std::endl;
    }
#endif

    // loop through linkedobjects looking for those with addresseranges
    // that contain an address from the passed addressbuffer.
#ifndef NDEBUG
    std::stringstream output;
#endif

    std::set<std::string> linkedobjs;
    for(std::set<LinkedObject>::const_iterator li = ttgrp_lo.begin();
	li != ttgrp_lo.end(); ++li) {

	// loop through any addressrange for the linkedobject.
	std::set<AddressRange> addr_range(li->getAddressRange());
	for(std::set<AddressRange>::const_iterator ar = addr_range.begin();
            ar != addr_range.end(); ++ar) {

	    // now see if there is a sample address in this addressrange.
	    bool foundaddress = false;
	    AddressCounts::const_iterator aci;
	    //for (aci = in.addresscounts.begin(); aci != in.addresscounts.end(); ++aci)
	    for (aci = abuffer.addresscounts.begin(); aci != abuffer.addresscounts.end(); ++aci)
	    {

	      if (!foundaddress && ar->doesContain(aci->first.getValue()) ) {
		// found an address. create a symtab for it if object path
		// is not already present in symtab.
		std::set<LinkedObject> stlo;
		std::pair<std::set<std::string>::iterator,bool> ret = linkedobjs.insert((*li).getPath());
		if (ret.second) {

#ifndef NDEBUG
		    if(Frontend::isTimingDebugEnabled()) {
			output << "TIME " << Time::Now() << " CLIENT:" << getpid()
			    << " Callbacks::finalize insert symtab " <<
			    (*li).getPath() << std::endl;
		    }
#endif
		    stlo.insert(*li);
		    symtabmap.insert(std::make_pair(*ar,
				std::make_pair(SymbolTable(*ar), stlo)
                                ));
		}

		foundaddress = true;
		break;

	      }

	      if (foundaddress) break;

	    }
        }

    }
#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::cerr << output.str();
	output.str(std::string());
    }
#endif

#ifdef OPENSS_CBTF_FE_RESOLVE_SYMBOLS

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
#if defined(HAVE_DYNINST)
	       << " Callbacks::finalize resolving functions,statements,loops" << std::endl;
#else
	       << " Callbacks::finalize resolving functions,statements" << std::endl;
#endif
	std::cerr << output.str();
    }
#endif

    // Use symtabapi for symbols. TODO: add support for BFD???
    // Note that BFD symbol resolution is typically reserved for a
    // system where dyninst and symtabapi are not available.
    // For the BFD case, there would be no loop resolution possible.
    SymtabAPISymbols stapi_symbols;

    // cycle through the symboltables and resolve symbols.
    for(SymbolTableMap::iterator i = symtabmap.begin();
					i != symtabmap.end(); ++i) {
	std::set<LinkedObject> le = i->second.second;
        for(std::set<LinkedObject>::const_iterator li = le.begin();
		li != le.end(); ++li) {
#if defined(HAVE_DYNINST)
	    DyninstSymbols::getLoops(addresses, *li, symtabmap);
#endif
	    stapi_symbols.getSymbols(addresses,*li,symtabmap);
	}
    }

    for(SymbolTableMap::iterator i = symtabmap.begin();
                i != symtabmap.end(); ++i) {

	// This records any previously resolved function symbols.
	// We will use this later to remove them from the corresponding
	// symboltable since we do not want processAndStore to record
	// them into the database more than once.
	std::map<AddressRange, std::string> existingfunctions;

	SymbolTable st = i->second.first;
	std::map<AddressRange, std::string> stfuncs = st.getFunctions();
	for (std::map<AddressRange,
	     std::string>::iterator kk = stfuncs.begin();
	     kk != stfuncs.end(); ++kk) {

	    // allfuncs records already processed function symbols.
	    // look for the function there first.
	    std::map<AddressRange, std::string>::iterator it = allfuncs.find(kk->first);

	    if (it == allfuncs.end()) {
		// We have not seen this function before. so this function
		// will be processed for storage into the database.
		allfuncs.insert(*kk);
	    }  else {
		// need to remove this function from this symtabs dm_functions
		// before calling processandStore.
		existingfunctions.insert(*kk);
	    }
	}

	// remove any functions for which we have added to the database.
	i->second.first.removeFunctions(existingfunctions);

	// finally record functions and statements to the database.
        for(std::set<LinkedObject>::const_iterator j = i->second.second.begin();
                j != i->second.second.end(); ++j) {
            i->second.first.processAndStore(*j);
        }
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	       << " Callbacks::finalize finished resolving symbols" << std::endl;
	std::cerr << output.str();
    }
#endif

#endif  // OPENSS_CBTF_FE_RESOLVE_SYMBOLS

    // process that symboltable here...
    for(std::vector<CBTF_Protocol_SymbolTable>::const_iterator si = symvec.begin();
	si != symvec.end(); si++) {
	process_symvec(*si);
    }

    // Now clean up any remaining linkedobjects, files, addressspaces,
    // and threads that do not contain any sample date, functions or statements.
    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(database);

    // delete any thread entries where no sample data was recorded.
    database->prepareStatement(
	"DELETE FROM Threads "
	"WHERE id NOT IN (SELECT DISTINCT thread FROM Data);"
	);
    while(database->executeStatement());

    // This code has issues with the symboltables passed from the cbtf
    // component network. Until those issues are  resolved, only allow
    // this cleanup of unneeded entries in the database for the symbols
    // resolved here in the the cbtf instrumentor FE.
#ifdef OPENSS_CBTF_FE_RESOLVE_SYMBOLS
    // delete any linkedobject entries where no functions or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM LinkedObjects "
	"WHERE id NOT IN (SELECT DISTINCT linked_object FROM Functions) "
	"  AND id NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(database->executeStatement());

    // delete any file entries where no linkedobjects or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM Files "
	"WHERE id NOT IN (SELECT DISTINCT file FROM LinkedObjects) "
	"  AND id NOT IN (SELECT DISTINCT file FROM Statements);"
	);
    while(database->executeStatement());

    // delete any addresspace entries where no linkedobjects or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM AddressSpaces "
	"WHERE linked_object NOT IN (SELECT DISTINCT id FROM LinkedObjects) "
	"  AND linked_object NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(database->executeStatement());

    // delete any addresspace entries for threads where no data
    // was recorded.
    database->prepareStatement(
	"DELETE FROM AddressSpaces "
	"WHERE thread NOT IN (SELECT DISTINCT id FROM Threads);"
	);
    while(database->executeStatement());
#endif

    // TODO:  Could look at removing any thread entries for which there
    // are no performance data blobs in the data table.
    END_TRANSACTION(database);

    // Now run vacuum on the database cleanup after removing entries.
    // FIXME: Apparently some DOD systems are crashing during vacuum; 
    //database->vacuum();


#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	       << " Callbacks::finalize exits" << std::endl;
	std::cerr << output.str();
    }
#endif

}

/**
 * Linked object has been loaded. (dlopen only)
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a linked object was loaded into one or more threads is received.
 * Updates the experiment databases as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::loadedLinkedObject(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> & in)
{
    // Decode the message
    CBTF_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_LoadedLinkedObject)); 

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid() << " Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    LinkedObjectEntry entry;

#ifndef NDEBUG
    std::stringstream output;
#endif

    // Iterate over each thread loading this linked object
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const CBTF_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	ThreadName tname(msg_thread);
        entry.tname = tname;
        entry.path = message.linked_object.path;
        entry.addr_begin = message.range.begin;
        entry.addr_end = message.range.end;
        entry.is_executable = message.is_executable;
        entry.time_loaded = message.time;
        entry.time_unloaded = KrellInstitute::Core::Time::Now();

        linkedobjectvec.push_back(entry);

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);


	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		output << "CLIENT:" << getpid() << " Callbacks::"
		       << "loadedLinkedObject(): Experiment "
		       << msg_thread.experiment
		       << " no longer exists." << std::endl;
	    }
#endif

	    continue;
	}
	BEGIN_WRITE_TRANSACTION(database);

	// Find the existing thread in the database
	int thread = getThreadIdentifier(database, msg_thread);
#ifndef NDEBUG
	if(thread == -1) {
	    if(Frontend::isDebugEnabled()) {
		output << "CLIENT:" << getpid() << " Callbacks::"
		       << "loadedLinkedObject(): Thread "
		       << toString(msg_thread) 
		       << " no longer exists." << std::endl;
	    }
	}
#endif

	// Only proceed if the thread was found
	if(thread != -1) {
	    
	    // Is there an existing linked object in the database?
	    int linked_object = 
		getLinkedObjectIdentifier(database, message.linked_object);

	    // Create this linked object if it wasn't present in the database
	    if(linked_object == -1) {

		// Create the file entry
		database->prepareStatement(
	            "INSERT INTO Files (path) VALUES (?);"
		    );
		database->bindArgument(1, message.linked_object.path);

		// TODO: insert the checksum
		
		while(database->executeStatement());
		int file = database->getLastInsertedUID();
	    
		// Create the linked object entry
		database->prepareStatement(
	            "INSERT INTO LinkedObjects "
		    "  (addr_begin, addr_end, file, is_executable) "
		    "VALUES (0, ?, ?, ?);"
		    );
		database->bindArgument(1, Address(message.range.end -
						  message.range.begin));
		database->bindArgument(2, file);
		database->bindArgument(3, message.is_executable);
		while(database->executeStatement());
		linked_object = database->getLastInsertedUID();
		
	    }

	    // Create an address space entry for this load
	    database->prepareStatement(
	        "INSERT INTO AddressSpaces "
	        "  (thread, time_begin, time_end, "
	        "   addr_begin, addr_end, linked_object) "
	        "VALUES (?, ?, ?, ?, ?, ?);"
	        );
	    database->bindArgument(1, thread);
	    database->bindArgument(2, Time(message.time));
	    database->bindArgument(3, Time::TheEnd());
	    database->bindArgument(4, Address(message.range.begin));
	    database->bindArgument(5, Address(message.range.end));
	    database->bindArgument(6, linked_object);
	    while(database->executeStatement());
	    
	}

	// End the transaction on this thread's database
	END_TRANSACTION(database);

    }

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::cerr << output.str();
    }
#endif
}

/**
 * Linked object group (protocol handler) has been loaded.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a linked object was loaded into one or more threads is received.
 * Updates the experiment databases as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::linkedObjectGroup(const boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> & in)
{
    // Decode the message
    CBTF_Protocol_LinkedObjectGroup message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_LinkedObjectGroup)); 

    ++num_linkedobjectgroups;

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now()
	       << " CLIENT:" << getpid() << " Callbacks::linkedObjectGroup"
	       << " message objs:" << message.linkedobjects.linkedobjects_len
	       << " num_linkedobjectgroups:" << num_linkedobjectgroups
	       << " num_attachedthreads:" << num_attachedthreads
	       << std::endl;
	std::cerr << output.str();
    }
#endif

    const CBTF_Protocol_ThreadName& msg_thread = message.thread;
    ThreadName tname(msg_thread);

    // Begin a transaction on this thread's database
#if 1
    SmartPtr<Database> database = DataQueues::getDatabase(0);
#else
    SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
#endif


    if(database.isNull()) {
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "CLIENT:" << getpid() << " Callbacks::"
		       << "linkedObjectGroup(): Experiment "
		       << msg_thread.experiment
		       << " no longer exists." << std::endl;
		std::cerr << output.str();
	}
#endif
	return;
    }


    BEGIN_WRITE_TRANSACTION(database);
    // Find the existing thread in the database
    int thread = getThreadIdentifier(database, msg_thread);

    // Only proceed if the thread was found
    if(thread != -1) {

	for(int i = 0; i < message.linkedobjects.linkedobjects_len; ++i) {
	    const CBTF_Protocol_LinkedObject& msg_lo =
                                message.linkedobjects.linkedobjects_val[i];

	    // Is there an existing linked object in the database?
	    int linked_object = 
		getLinkedObjectIdentifier(database, msg_lo.linked_object);

	    // Create this linked object if it wasn't present in the database
	    if(linked_object == -1) {
#if 0
		std::cerr << "INSERT INTO Files:" << msg_lo.linked_object.path
		<< " INSERT INTO LinkedObjects: " << Address(msg_lo.range.end -msg_lo.range.begin)
		<< " isEXE:" << msg_lo.is_executable
		<< std::endl;
#endif
		// Create the file entry
		database->prepareStatement(
	            "INSERT INTO Files (path) VALUES (?);"
		    );
		database->bindArgument(1, msg_lo.linked_object.path);

		// TODO: insert the checksum
		
		while(database->executeStatement());
		int file = database->getLastInsertedUID();
	    
		// Create the linked object entry
		database->prepareStatement(
	            "INSERT INTO LinkedObjects "
		    "  (addr_begin, addr_end, file, is_executable) "
		    "VALUES (0, ?, ?, ?);"
		    );
		database->bindArgument(1, Address(msg_lo.range.end -
						  msg_lo.range.begin));
		database->bindArgument(2, file);
		database->bindArgument(3, msg_lo.is_executable);
		while(database->executeStatement());
		linked_object = database->getLastInsertedUID();
		
	    }

	    // Create an address space entry for this load
	    database->prepareStatement(
	        "INSERT INTO AddressSpaces "
	        "  (thread, time_begin, time_end, "
	        "   addr_begin, addr_end, linked_object) "
	        "VALUES (?, ?, ?, ?, ?, ?);"
	        );
	    database->bindArgument(1, thread);
	    database->bindArgument(2, Time(msg_lo.time_begin));
	    database->bindArgument(3, Time::TheEnd());
	    database->bindArgument(4, Address(msg_lo.range.begin));
	    database->bindArgument(5, Address(msg_lo.range.end));
	    database->bindArgument(6, linked_object);
	    while(database->executeStatement());
	}
    }

    // End the transaction on this thread's database
    END_TRANSACTION(database);

    if (num_linkedobjectgroups == num_attachedthreads) {
	finalize();
    }
}

/**
 * vector of Linked object entry initially loaded.
 *
 * @param in  vector in LinkedObjectEntry.
 */
void Callbacks::linkedObjectEntryVec(const KrellInstitute::Core::LinkedObjectEntryVec & in)
{
#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::linkedObjectEntryVec"
	<< " input vector size is " << in.size()
	<< std::endl;;
	std::cerr << output.str();
    }
#endif
    linkedobjectvec = in;
}

/**
 * Process linkobject addressspaces initially recorded by the
 * linkedObjectGroupHandler callback.
 *
 * Updates the experiment databases as necessary.
 *
 * @param addresses  set of addresses used to compute inclusion into database.
 */
void Callbacks::process_addressspace(const std::set<OpenSpeedShop::Framework::Address> addresses)
{
    // For offline ld_preloaded collection runtimes, the getDatabase argument
    // experiment ID is always 0.
    SmartPtr<Database> database = DataQueues::getDatabase(0);
    KrellInstitute::Core::LinkedObjectEntryVec in = linkedobjectvec;

#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<<  " Callbacks::process_addressspace entered addresses:"
	<< addresses.size() << std::endl;
	std::cerr << output.str();
    }
#endif


    // This can be potentially very large so do it in one transaction.
    BEGIN_WRITE_TRANSACTION(database);

    // iterate through all the linkedobjects sent and record only those for
    // which we have a sample or callstack frame address.
    LinkedObjectEntryVec::const_iterator li;
    for (li = in.begin(); li != in.end(); ++li) {

	// create a range for this linkedobjects addressspace. will search
	// this range for any sample address.
	AddressRange range((*li).addr_begin.getValue(),
			   (*li).addr_end.getValue()) ;

	// use the begin address of the linkedobject range to see if any
	// addresses in the set of unique addresses is contained in it.
	// For now this seems sufficient to determine whether to include
	// this linkedobject into the linkedobject table and create an
	// addresspace for it. The end of range is commented out in case
	// we find a case where we need it.
	std::set<Address>::const_iterator aib = addresses.equal_range(range.getBegin()).first;
	if ( !(range.doesContain(*aib)) ) {
	    continue;
	}
	
	CBTF_Protocol_ThreadName message;
	::convert((*li).tname, message);
        int thread = getThreadIdentifier(database, message);

	// Only proceed if the thread was found
	if(thread != -1) {
	    // Is there an existing linked object in the database?
	    CBTF_Protocol_FileName fmessage;
	    std::string lopath((*li).path);
	    OpenSpeedShop::Framework::convert( lopath, fmessage.path);
	    int linked_object = 
		getLinkedObjectIdentifier(database, fmessage);

	    // Create this linked object if it wasn't present in the database
	    if(linked_object == -1) {

		// Create the file entry
		database->prepareStatement(
	            "INSERT INTO Files (path) VALUES (?);"
		    );
		database->bindArgument(1, lopath);

		// TODO: insert the checksum
		
		while(database->executeStatement());
		int file = database->getLastInsertedUID();

		// Create the linked object entry
		database->prepareStatement(
	            "INSERT INTO LinkedObjects "
		    "  (addr_begin, addr_end, file, is_executable) "
		    "VALUES (0, ?, ?, ?);"
		    );
		database->bindArgument(1, Address((*li).addr_end -
						  (*li).addr_begin));
		database->bindArgument(2, file);
		database->bindArgument(3, (*li).isExecutable());
		while(database->executeStatement());
		linked_object = database->getLastInsertedUID();

#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    output << "Callbacks::process_addresspace INSERT INTO LinkedObjects "
		    << "Id:" << linked_object
		    << " range:" << AddressRange(0,Address((*li).addr_end - (*li).addr_begin))
		    << " file " << file << " is_executable " << (*li).isExecutable()
		    << std::endl;
		}
#endif
	    }

#ifndef NDEBUG
		if(Frontend::isDebugEnabled()) {
		    output << "Callbacks::process_addresspace INSERT INTO AddressSpaces "
			<< "Thread.Id:" << thread
			<< " " << AddressRange(Address((*li).addr_begin.getValue()), Address((*li).addr_end.getValue()))
			<< ":" << TimeInterval(Time((*li).time_loaded.getValue()), Time((*li).time_unloaded.getValue()))
			<< ":" << linked_object 
			<< std::endl;
		}
#endif

	    // Create an address space entry for this load
	    database->prepareStatement(
	        "INSERT INTO AddressSpaces "
	        "  (thread, time_begin, time_end, "
	        "   addr_begin, addr_end, linked_object) "
	        "VALUES (?, ?, ?, ?, ?, ?);"
	        );
	    database->bindArgument(1, thread);
	    database->bindArgument(2, Time((*li).time_loaded.getValue()) );
	    database->bindArgument(3, Time((*li).time_unloaded.getValue()) );
	    database->bindArgument(4, Address((*li).addr_begin.getValue()));
	    database->bindArgument(5, Address((*li).addr_end.getValue()));
	    database->bindArgument(6, linked_object);
	    while(database->executeStatement());
	}
    }

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::cerr << output.str();
    }
#endif

    // End the transaction on this thread's database
    END_TRANSACTION(database);

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	       <<  " Callbacks::process_addresspace exits" << std::endl;
	std::cerr << output.str();
    }
#endif

}


// These are no longer emitted from the cbtf component network.
#if 0
/**
 * Thread's state has changed.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a state change occured in one or more threads is received. Updates
 * the thread table entries as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::threadsStateChanged(const Blob& blob)
{
    // Decode the message
    CBTF_Protocol_ThreadsStateChanged message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_CBTF_Protocol_ThreadsStateChanged),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Convert from CBTF_Protocol_ThreadState to Thread::State
    Thread::State state;
    switch(message.state) {
    case Disconnected:
	state = Thread::Disconnected;
	break;
    case Connecting:
	state = Thread::Connecting;
	break;
    case Nonexistent:
	state = Thread::Nonexistent;
	break;
    case Running:
	state = Thread::Running;
	break;
    case Suspended:
	state = Thread::Suspended;
	break;
    case Terminated:
	state = Thread::Terminated;
	break;
    default:
	state = Thread::Disconnected;
	break;
    }

    // Iterate over each thread changing state
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const CBTF_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	
	// Get the threads matching this thread name
	ThreadGroup threads = ThreadTable::TheTable.
	    getThreads(msg_thread.host, msg_thread.pid,
		       std::make_pair(msg_thread.has_posix_tid,
				      msg_thread.posix_tid)
		       );
	
	// Update these threads with their new state
	for(ThreadGroup::const_iterator
		i = threads.begin(); i != threads.end(); ++i) {
	    ThreadTable::TheTable.setThreadState(*i, state);
	}
    }
}



/**
 * Linked object has been unloaded.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a linked object was unloaded from one or more threads is received.
 * Updates the experiment databases as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::unloadedLinkedObject(const Blob& blob)
{
    // Decode the message
    CBTF_Protocol_UnloadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_CBTF_Protocol_UnloadedLinkedObject),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::" << toString(message);
	std::cerr << output.str();
    }
#endif

    // Iterate over each thread unloading this linked object
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const CBTF_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "CLIENT:" << getpid()
		<< " Callbacks::unloadedLinkedObject(): Experiment "
		<< msg_thread.experiment << " no longer exists."
		<< std::endl;
		std::cerr << output.str();
	    }
#endif

	    continue;
	}
	BEGIN_WRITE_TRANSACTION(database);

	// Find the existing thread in the database
	int thread = getThreadIdentifier(database, msg_thread);
#ifndef NDEBUG
	if(thread == -1) {
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "CLIENT:" << getpid()
		<< " Callbacks::unloadedLinkedObject(): Thread "
		<< toString(msg_thread) << " no longer exists."
		<< std::endl;
		std::cerr << output.str();
	    }
	}
#endif

	// Find the existing linked object in the database
	int linked_object = 
	    getLinkedObjectIdentifier(database, message.linked_object);
#ifndef NDEBUG
	if(linked_object == -1) {
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "CLIENT:" << getpid()
		<< " Callbacks::unloadedLinkedObject(): Linked Object "
		<< toString(message.linked_object) << " no longer exists."
		<< std::endl;
		std::cerr << output.str();
	    }
	}
#endif
	
	// Only proceed if the thread and linked object were found
	if((thread != -1) && (linked_object != -1)) {
	    
	    // Update the correct address space entry for this unload
	    database->prepareStatement(
	        "UPDATE AddressSpaces "
		"SET time_end = ? "
		"WHERE thread = ? "
		"  AND time_end = ? "
		"  AND linked_object = ?;"
	        );
	    database->bindArgument(1, Time(message.time));
	    database->bindArgument(2, thread);
	    database->bindArgument(3, Time::TheEnd());
	    database->bindArgument(4, linked_object);	
	    while(database->executeStatement());	    
	    
	}

	// End the transaction on this thread's database
	END_TRANSACTION(database);

    }
}

#endif


/**
 * Performance data.
 *
 * Callback function called by the frontend message pump when performance
 * data is received. Simply enqueues this performance data for later storage
 * in an experiment database.
 *
 * @param blob    Blob containing the performance data.
 */
void Callbacks::performanceData(const boost::shared_ptr<CBTF_Protocol_Blob> & in)
{
    // Decode the message
    CBTF_Protocol_Blob message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_Blob)); 

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {

	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::performanceData entered with data blob size:"
	<< message.data.data_len
	<< std::endl;
	std::cerr << output.str();
    }
#endif
 
    
    // Enqueue this performance data.  For now we will not flush
    // the dataqueue per blob as that is going to thrash the database.
    OpenSpeedShop::Framework::Blob blob(message.data.data_len,message.data.data_val);
#if 0
    //std::cerr << "Callbacks::performanceData calls  updateThreads" << std::endl;
    bool found_threads = updateThreads();
    if (!found_threads) {
	pending_data.push_back(blob);	
    std::cerr << "Callbacks::performanceData pending data blobs " << pending_data.size() << std::endl;
	return;
    } else {
	DataQueues::enqueuePerformanceData(blob);
    }
    // used to flush queue here.
    if (found_threads && pending_blobs > 1000) {
	DataQueues::flushPerformanceData();
	pending_blobs = 0;
    }
    pending_blobs++;
#else
    DataQueues::enqueuePerformanceData(blob);
#endif


#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {

	std::stringstream output;
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::performanceData exits"
	<< std::endl;
	std::cerr << output.str();
    }
#endif
}

void Callbacks::symbolTable(const boost::shared_ptr<CBTF_Protocol_SymbolTable> & in)
{
#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {

	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::symbolTable entered" << std::endl;
	std::cerr << output.str();
    }
#endif

    // Decode the message
    CBTF_Protocol_SymbolTable message;
    memset(&message, 0, sizeof(message));
    memcpy(&message, in.get(),sizeof(CBTF_Protocol_SymbolTable)); 

    process_symvec(message);

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	output << "TIME " << Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::symbolTable exits" << std::endl;
    }
    std::cerr << output.str();
#endif
}

// The following call in the Experiment class will add a view to the VIEWS
// table.  
// addView(SmartPtr<Database>& database, std::string& viewcommand, std::string& viewdata )
void Callbacks::maxFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> & in)
{
#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {

	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::maxFunctionValues entered" << std::endl;
	std::cerr << output.str();
    }
#endif


    CBTF_Protocol_FunctionThreadValues *message = in.get();

    FunctionThreadCount maxvals;
    for(int i=0; i<message->values.values_len; ++i) {
	std::string f(message->values.values_val[i].function);
	std::pair<ThreadName,uint64_t> fts =
		std::make_pair(ThreadName(message->values.values_val[i].thread),
				       message->values.values_val[i].value);
	FunctionThreadCount::iterator it = maxvals.find(f);
	if ( it == maxvals.end() ) {
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::maxFunctionThreadValues: NEW max for " << f
		<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
	    }
#endif
	    maxvals.insert(std::make_pair(f,fts));
	} else if ( f == (*it).first && fts.second > (*it).second.second ) {
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::maxFunctionThreadValues: UPDATE max for " << f
		<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
	    }
#endif
	    (*it).second.first = fts.first;
	    (*it).second.second = fts.second;
	}
    }

    std::stringstream metric_output;
    metric_output << std::left << std::setw(20) << "Max"
	<< std::setw(20) << "Thread"
	<< std::setw(20) << "Function"
	<< std::endl;
    for(FunctionThreadCount::const_iterator it = maxvals.begin(); it != maxvals.end(); ++it) {
	std::stringstream tmp;
	tmp << (*it).second.first.getMPIRank() << ":"  << (*it).second.first.getOmpTid();
	metric_output << std::left << std::setw(20) << (*it).second.second
	<< std::setw(20) << tmp.str()
	<< std::setw(20) << (*it).first
	<< std::endl;
    }
#ifndef NDEBUG
    if(Frontend::isShowLoadbalanceEnabled()) {
    std::cout << metric_output.str() << std::endl;
    }
#endif

    // Begin a transaction on this thread's database
    SmartPtr<Database> database = DataQueues::getDatabase(0);
    if(database.isNull()) {
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	    output << "CLIENT:" << getpid()
	    << " Callbacks::maxFunctionValues(): Experiment "
	    << 0 << " no longer exists." << std::endl;
	    std::cerr << output.str();
	}
#endif
    } else {
	std::string viewcmd("expview MAX");
	std::string viewdata(metric_output.str());
	addView(database, viewcmd, viewdata);
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::maxFunctionValues exits" << std::endl;
    }
    std::cerr << output.str();
#endif
}

void Callbacks::minFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> & in)
{
#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {

	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::minFunctionValues entered" << std::endl;
	std::cerr << output.str();
    }
#endif

    CBTF_Protocol_FunctionThreadValues *message = in.get();

    FunctionThreadCount minvals;
    for(int i=0; i<message->values.values_len; ++i) {
	std::string f(message->values.values_val[i].function);
	std::pair<ThreadName,uint64_t> fts =
		std::make_pair(ThreadName(message->values.values_val[i].thread),
				       message->values.values_val[i].value);
	FunctionThreadCount::iterator it = minvals.find(f);
	if ( it == minvals.end() ) {
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::minFunctionThreadValues: NEW min for " << f
		<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
	    }
#endif
	    minvals.insert(std::make_pair(f,fts));
	} else if ( f == (*it).first && fts.second < (*it).second.second ) {
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::minFunctionThreadValues: UPDATE min for " << f
		<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
	    }
#endif
	    (*it).second.first = fts.first;
	    (*it).second.second = fts.second;
	}
    }

    std::stringstream metric_output;
    metric_output << std::left << std::setw(20) << "Min"
	<< std::setw(20) << "Thread"
	<< std::setw(20) << "Function"
	<< std::endl;
    for(FunctionThreadCount::const_iterator it = minvals.begin(); it != minvals.end(); ++it) {
	std::stringstream tmp;
	tmp << (*it).second.first.getMPIRank() << ":"  << (*it).second.first.getOmpTid();
	metric_output << std::left << std::setw(20) << (*it).second.second
	<< std::setw(20) << tmp.str()
	<< std::setw(20) << (*it).first
	<< std::endl;
    }

#ifndef NDEBUG
    if(Frontend::isShowLoadbalanceEnabled()) {
    std::cout << metric_output.str() << std::endl;;
    }
#endif

    // Begin a transaction on this thread's database
    SmartPtr<Database> database = DataQueues::getDatabase(0);
    if(database.isNull()) {
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	    output << "CLIENT:" << getpid()
	    << " Callbacks::maxFunctionValues(): Experiment "
	    << 0 << " no longer exists." << std::endl;
	    std::cerr << output.str();
	}
#endif
    } else {
	std::string viewcmd("expview MIN");
	std::string viewdata(metric_output.str());
	addView(database, viewcmd, viewdata);
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::minFunctionValues exits" << std::endl;
    }
    std::cerr << output.str();
#endif
}


void Callbacks::avgFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> & in)
{
#ifndef NDEBUG
    std::stringstream output;
    if(Frontend::isTimingDebugEnabled()) {

	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::avgFunctionValues entered" << std::endl;
	std::cerr << output.str();
    }
#endif

    CBTF_Protocol_FunctionAvgValues *message = in.get();
    FunctionAvgMap avgvals;

    for(int i=0; i<message->values.values_len; ++i) {
	std::string f(message->values.values_val[i].function);
	std::pair<uint64_t,uint64_t> fts =
		std::make_pair(message->values.values_val[i].value,
			       message->values.values_val[i].num);
	FunctionAvgMap::iterator it = avgvals.find(f);
	if ( it == avgvals.end() ) {
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::avgFunctionValues: NEW avg for " << f
		<< " value:" << fts.first << " count:" << fts.second << std::endl; 
	    }
#endif
	    avgvals.insert(std::make_pair(f,fts));

	} else if ( f == (*it).first) {
	    (*it).second.first += fts.first;
	    (*it).second.second += fts.second;
#ifndef NDEBUG
	    if (Frontend::isMetricDebugEnabled()) {
		output << "Callbacks::avgFunctionValues: UPDATE avg for " << f
		<< " value:" << fts.first << " count:" << fts.second << std::endl; 
	    }
#endif
	}
    }

    // This output is for demo purposes.
    std::stringstream metric_output;
    metric_output << std::left
	<< std::setfill(' ') << std::setw(20) << "Average"
	<< std::setfill(' ') << std::setw(20) << "Total"
	<< std::setfill(' ') << std::setw(10) << "Threads"
	<< std::setfill(' ') << std::setw(20) << "Function"
	<< std::endl;
    for(FunctionAvgMap::const_iterator it = avgvals.begin(); it != avgvals.end(); ++it) {
	if ((*it).second.first > 0) {
	    metric_output << std::left
	    << std::setfill(' ') << std::setw(20) << (*it).second.first / (*it).second.second
	    << std::setfill(' ') << std::setw(20) << (*it).second.first
	    << std::setfill(' ') << std::setw(10) << (*it).second.second
	    << std::setfill(' ') << std::setw(20) << (*it).first
	    << std::endl;
	}
    }
#ifndef NDEBUG
    if(Frontend::isShowLoadbalanceEnabled()) {
    std::cout << metric_output.str();
    }
#endif

    // Begin a transaction on this thread's database
    SmartPtr<Database> database = DataQueues::getDatabase(0);
    if(database.isNull()) {
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	    output << "CLIENT:" << getpid()
	    << " Callbacks::maxFunctionValues(): Experiment "
	    << 0 << " no longer exists." << std::endl;
	    std::cerr << output.str();
	}
#endif
    } else {
	std::string viewcmd("expview AVG");
	std::string viewdata(metric_output.str());
	addView(database, viewcmd, viewdata);
    }

#ifndef NDEBUG
    if(Frontend::isTimingDebugEnabled()) {
	output << "TIME " << KrellInstitute::Core::Time::Now() << " CLIENT:" << getpid()
	<< " Callbacks::avgFunctionValues exits" << std::endl;
    }
    std::cerr << output.str();
#endif
}
