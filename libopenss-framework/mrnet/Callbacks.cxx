////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
	BEGIN_WRITE_TRANSACTION(database);

	// Is there an existing placeholder for the entire process?
	int thread = -1;
	database->prepareStatement(
	    "SELECT id "
	    "FROM Threads "
	    "WHERE host = ? "
	    "  AND pid = ? "
	    "  AND posix_tid = NULL;"
	    );
	database->bindArgument(1, msg_thread.host);
	database->bindArgument(2, static_cast<int>(msg_thread.pid));
	while(database->executeStatement())
	    thread = database->getResultAsInteger(1);

	// Reuse the placeholder if appropriate
	if(msg_thread.has_posix_tid && (thread != -1)) {
	    database->prepareStatement(
	        "UPDATE Threads SET posix_tid = ? WHERE thread = ?;"
		);
	    database->bindArgument(
	        1, static_cast<pthread_t>(msg_thread.posix_tid)
		);
	    database->bindArgument(2, thread);
	    while(database->executeStatement());
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
	}

	// End the transaction on this thread's database
	END_TRANSACTION(database);

    }
}



/**
 * Value of an integer global variable.
 *
 * ...
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

    // TODO: implement!
}



/**
 * Value of a job description global variable.
 *
 * ...
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

    // TODO: implement!
}



/**
 * Value of a string global variable.
 *
 * ...
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

    // TODO: implement!
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
	BEGIN_WRITE_TRANSACTION(database);

	// Find the existing thread in the database
	int thread = getThreadIdentifier(database, msg_thread);
	Assert(thread != -1);
	
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
	BEGIN_WRITE_TRANSACTION(database);
	
	// Find the existing linked object in the database
	int linked_object = 
	    getLinkedObjectIdentifier(database, message.linked_object);
	Assert(linked_object != -1);

	// Iterate over each function entry
	for(int j = 0; j < message.functions.functions_len; ++j) {
	    const OpenSS_Protocol_FunctionEntry& msg_function = 
		message.functions.functions_val[j];
	    
	    // Iterate over each bitmap for this function
	    for(int k = 0; k < msg_function.bitmaps.bitmaps_len; ++k) {
		const OpenSS_Protocol_AddressBitmap& msg_bitmap =
		    msg_function.bitmaps.bitmaps_val[k];

		// TODO: Handle discontiguous ranges of functions. Right now
		//       the daemon doesn't produce these, but it will soon.
		//       For now simply take the address range of the first
		//       bitmap as the address range of the function.
		if(k > 0)
		    break;

		// Create the function entry
		database->prepareStatement(
		    "INSERT INTO Functions "
		    "  (linked_object, addr_begin, addr_end, name) "
		    "VALUES (?, ?, ?, ?);"
		    );
		database->bindArgument(1, linked_object);
		database->bindArgument(2, Address(msg_bitmap.range.begin));
		database->bindArgument(3, Address(msg_bitmap.range.end));
		database->bindArgument(4, msg_function.name);
		while(database->executeStatement());
		
	    }
	    
	}

	// Iterate over each statement entry
	for(int j = 0; j < message.statements.statements_len; ++j) {
	    const OpenSS_Protocol_StatementEntry& msg_statement =
		message.statements.statements_val[j];

	    // Is there an existing file in the database?
	    int file = -1;
	    database->prepareStatement("SELECT id FROM Files WHERE path = ?;");
	    database->bindArgument(1, msg_statement.path.path);

	    // TODO: compare the checksum

	    while(database->executeStatement())
		file = database->getResultAsInteger(1);

	    // Create the file entry if it wasn't present in the database
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
		
		// Create the StatementRanges entry
		database->prepareStatement(
		    "INSERT INTO StatementRanges "
		    "  (statement, addr_begin, addr_end, valid_bitmap) "
		    "VALUES (?, ?, ?, ?);"
		    );
		database->bindArgument(1, statement);
		database->bindArgument(2, Address(msg_bitmap.range.begin));
		database->bindArgument(3, Address(msg_bitmap.range.end));
		database->bindArgument(4, 
				       Blob(msg_bitmap.bitmap.data.data_len,
					    msg_bitmap.bitmap.data.data_val));
		while(database->executeStatement());
		
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
	BEGIN_WRITE_TRANSACTION(database);

	// Find the existing thread in the database
	int thread = getThreadIdentifier(database, msg_thread);
	Assert(thread != -1);

	// Find the existing linked object in the database
	int linked_object = 
	    getLinkedObjectIdentifier(database, message.linked_object);
	Assert(linked_object != -1);
	
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
    // Enqueue this performance data
    DataQueues::enqueuePerformanceData(blob);
}
