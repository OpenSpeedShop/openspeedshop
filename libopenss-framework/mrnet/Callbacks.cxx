////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007-2009 William Hachfeld. All Rights Reserved.
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
#include "Blob.hxx"
#include "Callbacks.hxx"
#include "Database.hxx"
#include "DataQueues.hxx"
#include "Frontend.hxx"
#include "GlobalTable.hxx"
#include "Job.hxx"
#include "Protocol.h"
#include "SmartPtr.hxx"
#include "ThreadGroup.hxx"
#include "ThreadTable.hxx"
#include "Utility.hxx"

#include <iostream>
#include <sstream>

using namespace OpenSpeedShop::Framework;



namespace {



    /**
     * Convert job from protocol use.
     *
     * Converts a framework job object from the structure used in protocol
     * messages.
     *
     * @param in      Structure to be converted.
     * @retval out    Job to hold the results.
     */
    void convert(const OpenSS_Protocol_Job& in, Job& out)
    {
	out.clear();
	for(int i = 0; i < in.entries.entries_len; ++i) {
	    out.push_back(std::make_pair(in.entries.entries_val[i].host,
					 in.entries.entries_val[i].pid));
	}
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
				  const OpenSS_Protocol_FileName& linked_object)
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
			    const OpenSS_Protocol_ThreadName& thread)
			    
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
void Callbacks::attachedToThreads(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_AttachedToThreads message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachedToThreads),
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

    // Iterate over each thread loading this linked object
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const OpenSS_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "attachedToThreads(): Experiment " 
		       << msg_thread.experiment 
		       << " no longer exists.";
		std::cerr << output.str();
	    }
#endif

	    continue;
	}
	BEGIN_WRITE_TRANSACTION(database);

	// Is there an existing placeholder for the entire process?
	int thread = -1;
	database->prepareStatement(
	    "SELECT id "
	    "FROM Threads "
	    "WHERE host = ? "
	    "  AND pid = ? "
	    "  AND posix_tid ISNULL;"
	    );
	database->bindArgument(1, msg_thread.host);
	database->bindArgument(2, static_cast<int>(msg_thread.pid));
	while(database->executeStatement())
	    thread = database->getResultAsInteger(1);

	// Reuse the placeholder if appropriate
	if(thread != -1) {
	    if(msg_thread.has_posix_tid && (thread != -1)) {
		database->prepareStatement(
		    "UPDATE Threads SET posix_tid = ? WHERE id = ?;"
		    );
		database->bindArgument(
		    1, static_cast<pthread_t>(msg_thread.posix_tid)
		    );
		database->bindArgument(2, thread);
		while(database->executeStatement());
	    }
	}

	// Otherwise create the thread entry
	else {
	    if(msg_thread.has_posix_tid)
		database->prepareStatement(
		    "INSERT INTO Threads "
		    "  (host, pid, posix_tid) "
		    "VALUES (?, ?, ?);"
		    );
	    else
		database->prepareStatement(
		    "INSERT INTO Threads (host, pid) VALUES (?, ?);"
		    );
	    database->bindArgument(1, msg_thread.host);
	    database->bindArgument(2, static_cast<int>(msg_thread.pid));
	    if(msg_thread.has_posix_tid)
		database->bindArgument(
		    3, static_cast<pthread_t>(msg_thread.posix_tid)
		    );
	    while(database->executeStatement());
	    int thread = database->getLastInsertedUID();
	    ThreadTable::TheTable.addThread(Thread(database, thread));
	}

	// End the transaction on this thread's database
	END_TRANSACTION(database);

    }
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
void Callbacks::createdProcess(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_CreatedProcess message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreatedProcess),
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

    // Update the thread with its real process identifier
    SmartPtr<Database> database = 
	DataQueues::getDatabase(message.original_thread.experiment);
    if(database.isNull()) {
	
#ifndef NDEBUG
	if(Frontend::isDebugEnabled()) {
	    std::stringstream output;
	    output << "[TID " << pthread_self() << "] Callbacks::"
		   << "createdProcess(): Experiment " 
		   << message.original_thread.experiment 
		   << " no longer exists.";
	    std::cerr << output.str();
	}
#endif

	return;
    }
    BEGIN_WRITE_TRANSACTION(database);
    database->prepareStatement("UPDATE Threads SET pid = ? WHERE id = ?;");
    database->bindArgument(1, static_cast<int>(message.created_thread.pid));
    database->bindArgument(2, - static_cast<int>(message.original_thread.pid));
    while(database->executeStatement());
    END_TRANSACTION(database);
}



/**
 * Value of an integer global variable.
 *
 * Callback function called by the frontend message pump when a message that
 * contains the value of an integer global variable is received. Provides the
 * value to the appropriate waiting thread.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalIntegerValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalIntegerValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalIntegerValue),
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

    // Get the threads matching this thread name
    ThreadGroup threads = ThreadTable::TheTable.
	getThreads(message.thread.host, message.thread.pid,
		   std::make_pair(message.thread.has_posix_tid,
				  message.thread.posix_tid)
		   );
    Assert(threads.size() == 1);

    // Provide the value of this global variable.
    GlobalTable::TheTable.provideValue(
        *threads.begin(), std::string(message.global),
	std::make_pair(static_cast<bool>(message.found), message.value)
	);
}



/**
 * Value of a job description global variable.
 *
 * Callback function called by the frontend message pump when a message that
 * contains the value of a job description global variable is received. Provides
 * the value to the appropriate waiting thread.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalJobValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalJobValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalJobValue),
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

    // Get the threads matching this thread name
    ThreadGroup threads = ThreadTable::TheTable.
	getThreads(message.thread.host, message.thread.pid,
		   std::make_pair(message.thread.has_posix_tid,
				  message.thread.posix_tid)
		   );
    Assert(threads.size() == 1);

    // Provide the value of this global variable.
    Job job;
    ::convert(message.value, job);
    GlobalTable::TheTable.provideValue(
        *threads.begin(), std::string(message.global),
	std::make_pair(static_cast<bool>(message.found), job)
	);
}



/**
 * Value of a string global variable.
 *
 * Callback function called by the frontend message pump when a message that
 * contains the value of a string global variable is received. Provides the
 * value to the appropriate waiting thread.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalStringValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalStringValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalStringValue),
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

    // Get the threads matching this thread name
    ThreadGroup threads = ThreadTable::TheTable.
	getThreads(message.thread.host, message.thread.pid,
		   std::make_pair(message.thread.has_posix_tid,
				  message.thread.posix_tid)
		   );
    Assert(threads.size() == 1);

    // Provide the value of this global variable.
    GlobalTable::TheTable.provideValue(
        *threads.begin(), std::string(message.global),
	std::make_pair(
	    static_cast<bool>(message.found),
	    std::string((message.value == NULL) ? message.value : "")
	    )
        );
}



/**
 * Threads have been instrumented.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a collector has instrumented one or more threads is received.
 * Updates the experiment database as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::instrumented(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_Instrumented message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Instrumented),
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

    // Iterate over each thread being instrumented
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const OpenSS_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	// Check preconditions
	Assert(msg_thread.experiment == message.collector.experiment);

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "instrumented(): Experiment "
		       << msg_thread.experiment 
		       << " no longer exists.";
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
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "instrumented(): Thread " << toString(msg_thread) 
		       << " no longer exists.";
		std::cerr << output.str();
	    }
	}
#endif

	// Find the existing collector in the database
	bool has_collector = false;
	database->prepareStatement(
	    "SELECT COUNT(*) FROM Collectors WHERE id = ?;"
	    );
	database->bindArgument(1, message.collector.collector);
	while(database->executeStatement())
	    if(database->getResultAsInteger(1) > 0)
		has_collector = true;
#ifndef NDEBUG
	if(!has_collector) {
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "instrumented(): Collector " 
		       << message.collector.collector 
		       << " no longer exists.";
		std::cerr << output.str();
	    }
	}
#endif
	
	// Only proceed if the thread and collector were found
	if((thread != -1) && has_collector) {

	    // Create the collecting entry for this instrumentation
	    database->prepareStatement(
		"INSERT INTO Collecting "
		"  (collector, thread, is_postponed) "
		"VALUES (?, ?, 0);"
		);
	    database->bindArgument(1, message.collector.collector);
	    database->bindArgument(2, thread);
	    while(database->executeStatement());
	    
	}
	
	// End the transaction on this thread's database
	END_TRANSACTION(database);

    }
}



/**
 * Linked object has been loaded.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates a linked object was loaded into one or more threads is received.
 * Updates the experiment databases as necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::loadedLinkedObject(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_LoadedLinkedObject),
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

    // Iterate over each thread loading this linked object
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const OpenSS_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "loadedLinkedObject(): Experiment "
		       << msg_thread.experiment
		       << " no longer exists.";
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
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "loadedLinkedObject(): Thread "
		       << toString(msg_thread) 
		       << " no longer exists.";
		std::cerr << output.str();
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
}



/**
 * Report error.
 *
 * Callback function called by the frontend message pump when a message that
 * indicates an error is received. Simply displays the message to the stderr
 * stream.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::reportError(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ReportError message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ReportError),
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

    // Display the message to the stderr stream
    std::cerr << "openssd: " << message.text << std::endl;
}



/**
 * Standard error stream.
 *
 * Callback function called by the frontend message pump when a message that
 * contains standard error stream data is received. Passes the data on to the
 * appropriate output callback.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::stdErr(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_StdErr message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdErr),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isStdioDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Get the threads matching this thread name
    ThreadGroup threads = ThreadTable::TheTable.
	getThreads(message.thread.host, message.thread.pid,
		   std::make_pair(message.thread.has_posix_tid,
				  message.thread.posix_tid)
		   );

    // Iterate over each of these threads
    for(ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Get the standard error stream callback for this thread
	OutputCallback callback = ThreadTable::TheTable.getStderrCallback(*i);
	
	// Pass the data on to the callback (if any)
	if(callback.first != NULL)
	    (*(callback.first))(
	        reinterpret_cast<char*>(message.data.data.data_val),
		message.data.data.data_len,
		callback.second
		);
	
    }
}



/**
 * Standard output stream.
 *
 * Callback function called by the frontend message pump when a message that
 * contains standard output stream data is received. Passes the data on to the
 * appropriate output callback.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::stdOut(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_StdOut message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdOut),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isStdioDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Get the threads matching this thread name
    ThreadGroup threads = ThreadTable::TheTable.
	getThreads(message.thread.host, message.thread.pid,
		   std::make_pair(message.thread.has_posix_tid,
				  message.thread.posix_tid)
		   );

    // Iterate over each of these threads
    for(ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	
	// Get the standard output stream callback for this thread
	OutputCallback callback = ThreadTable::TheTable.getStdoutCallback(*i);
	
	// Pass the data on to the callback (if any)
	if(callback.first != NULL)
	    (*(callback.first))(
	        reinterpret_cast<char*>(message.data.data.data_val),
		message.data.data.data_len,
		callback.second
		);
	
    }
}



/**
 * Symbol table.
 *
 * Callback function called by the frontend message pump when a message that
 * contains a symbol table is received. Updates the experiment databases as
 * necessary.
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::symbolTable(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_SymbolTable message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SymbolTable),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message, Frontend::isSymbolsDebugEnabled());
	std::cerr << output.str();
    }
#endif

    // Iterate over each experiment adding this symbol table
    for(int i = 0; i < message.experiments.experiments.experiments_len; ++i) {
	const OpenSS_Protocol_Experiment& msg_experiment =
	    message.experiments.experiments.experiments_val[i];
	
	// Begin a transaction on this experiment's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_experiment.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "symbolTable(): Experiment "
		       << msg_experiment.experiment 
		       << " no longer exists.";
		std::cerr << output.str();
	    }
#endif

	    continue;
	}
	BEGIN_WRITE_TRANSACTION(database);
	
	// Find the existing linked object in the database
	int linked_object = 
	    getLinkedObjectIdentifier(database, message.linked_object);
#ifndef NDEBUG
	if(linked_object == -1) {
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "symbolTable(): Linked Object "
		       << toString(message.linked_object) 
		       << " no longer exists.";
		std::cerr << output.str();
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
		    const OpenSS_Protocol_FunctionEntry& msg_function = 
			message.functions.functions_val[j];

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
			const OpenSS_Protocol_AddressBitmap& msg_bitmap =
			    msg_function.bitmaps.bitmaps_val[k];

			// Create the function ranges entry
			database->prepareStatement(
		            "INSERT INTO FunctionRanges "
			    "  (function, addr_begin, addr_end, valid_bitmap) "
			    "VALUES (?, ?, ?, ?);"
			    );
			database->bindArgument(1, function);
			database->bindArgument(2,
			    Address(msg_bitmap.range.begin)
			    );
			database->bindArgument(3,
			    Address(msg_bitmap.range.end)
			    );
			database->bindArgument(4, 
			    Blob(msg_bitmap.bitmap.data.data_len,
				 msg_bitmap.bitmap.data.data_val)
			    );
			while(database->executeStatement());
			
		    }
		    
		}
		
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

		// Iterate over each statement entry
		for(int j = 0; j < message.statements.statements_len; ++j) {
		    const OpenSS_Protocol_StatementEntry& msg_statement =
			message.statements.statements_val[j];
		    
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
			database->prepareStatement(
		            "INSERT INTO Files (path) VALUES (?);"
			    );
			database->bindArgument(1, msg_statement.path.path);

			// TODO: insert the checksum
			
			while(database->executeStatement());
			file = database->getLastInsertedUID();
		    }

		    // Create the statement entry
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
			const OpenSS_Protocol_AddressBitmap& msg_bitmap =
			    msg_statement.bitmaps.bitmaps_val[k];
		
			// Create the statement ranges entry
			database->prepareStatement(
		            "INSERT INTO StatementRanges "
			    "  (statement, addr_begin, addr_end, valid_bitmap) "
			    "VALUES (?, ?, ?, ?);"
			    );
			database->bindArgument(1, statement);
			database->bindArgument(2,
			    Address(msg_bitmap.range.begin)
			    );
			database->bindArgument(3,
			    Address(msg_bitmap.range.end)
			    );
			database->bindArgument(4, 
		            Blob(msg_bitmap.bitmap.data.data_len,
				 msg_bitmap.bitmap.data.data_val)
			    );
			while(database->executeStatement());
		    
		    }
		    
		}
		
	    }

	}
	
	// End the transaction on this thread's database
	END_TRANSACTION(database);
	
    }
}



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
    OpenSS_Protocol_ThreadsStateChanged message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ThreadsStateChanged),
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

    // Convert from OpenSS_Protocol_ThreadState to Thread::State
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
	const OpenSS_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];
	
	// Get the threads matching this thread name
	ThreadGroup threads = ThreadTable::TheTable.
	    getThreads(msg_thread.host, msg_thread.pid,
		       std::make_pair(msg_thread.has_posix_tid,
				      msg_thread.posix_tid)
		       );
	
	// Update these threads with their new state
	for(ThreadGroup::const_iterator
		i = threads.begin(); i != threads.end(); ++i)
	    ThreadTable::TheTable.setThreadState(*i, state);
	
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
    OpenSS_Protocol_UnloadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_UnloadedLinkedObject),
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

    // Iterate over each thread unloading this linked object
    for(int i = 0; i < message.threads.names.names_len; ++i) {
	const OpenSS_Protocol_ThreadName& msg_thread =
	    message.threads.names.names_val[i];

	// Begin a transaction on this thread's database
	SmartPtr<Database> database = 
	    DataQueues::getDatabase(msg_thread.experiment);
	if(database.isNull()) {

#ifndef NDEBUG
	    if(Frontend::isDebugEnabled()) {
		std::stringstream output;
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "unloadedLinkedObject(): Experiment "
		       << msg_thread.experiment 
		       << " no longer exists.";
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
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "unloadedLinkedObject(): Thread "
		       << toString(msg_thread) 
		       << " no longer exists.";
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
		output << "[TID " << pthread_self() << "] Callbacks::"
		       << "unloadedLinkedObject(): Linked Object "
		       << toString(message.linked_object) 
		       << " no longer exists.";
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



/**
 * Performance data.
 *
 * Callback function called by the frontend message pump when performance
 * data is received. Simply enqueues this performance data for later storage
 * in an experiment database.
 *
 * @param blob    Blob containing the performance data.
 */
void Callbacks::performanceData(const Blob& blob)
{
#ifndef NDEBUG
    if(Frontend::isPerfDataDebugEnabled()) {
	OpenSS_Protocol_Blob perfdata;
	perfdata.data.data_len = blob.getSize();
	perfdata.data.data_val = 
	    reinterpret_cast<uint8_t*>(const_cast<void*>(blob.getContents()));
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::performanceData("
	       << std::endl << toString(perfdata) << ")" << std::endl;
	std::cerr << output.str();
    }
#endif

    // Enqueue this performance data
    DataQueues::enqueuePerformanceData(blob);
}
