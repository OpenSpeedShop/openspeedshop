////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the Senders namespace.
 *
 */

#include "Backend.hxx"
#include "Blob.hxx"
#include "Collector.hxx"
#include "ExperimentGroup.hxx"
#include "FileName.hxx"
#include "Protocol.h"
#include "Senders.hxx"
#include "SymbolTable.hxx"
#include "ThreadName.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"
#include "Time.hxx"
#include "Utility.hxx"

#include <algorithm>

using namespace OpenSpeedShop::Framework;



// This should be namespace, not namespace anonymous but gcc can not handle that
// With dyninst-8.0.0 adding more ::convert methods gcc complains with an error.
namespace anonymous {



    /**
     * Convert job for protocol use.
     *
     * Converts the specified framework job object to the structure used in
     * protocol messages.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      Job to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const Job& in, OpenSS_Protocol_Job& out)
    {
	// Allocate an appropriately sized array of job entries
	out.entries.entries_len = in.size();
	out.entries.entries_val = 
	    reinterpret_cast<OpenSS_Protocol_JobEntry*>(malloc(
		std::max(static_cast<Job::size_type>(1), in.size()) *
		sizeof(OpenSS_Protocol_JobEntry)
		));

	// Iterate over each entry of this job
	OpenSS_Protocol_JobEntry* ptr = out.entries.entries_val;
	for(Job::const_iterator
		i = in.begin(); i != in.end(); ++i, ++ptr) {
	    OpenSpeedShop::Framework::convert(i->first, ptr->host);
	    ptr->pid = i->second;
	}
    }
    
    
    
}



/**
 * Attached to a thread.
 *
 * Issue a message to the frontend to indicate the specified threads were
 * attached.
 *
 * @param threads    Threads that were attached.
 */
void Senders::attachedToThreads(const ThreadNameGroup& threads)
{
    // Assemble the request into a message
    OpenSS_Protocol_AttachedToThreads message;
    message.threads = threads;

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachedToThreads),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_ATTACHED_TO_THREADS, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachedToThreads),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Created a process.
 *
 * Issue a message to the frontend to indicate the specified process was
 * created.
 *
 * @param original_thread    Original thread as specified in the
 *                           CreateProcess requesdt.
 * @param created_thread     Process that was created.
 */
void Senders::createdProcess(const ThreadName& original_thread,
			     const ThreadName& created_thread)
{
    // Assemble the request into a message
    OpenSS_Protocol_CreatedProcess message;
    message.original_thread = original_thread;
    message.created_thread = created_thread;

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreatedProcess),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_CREATED_PROCESS, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreatedProcess),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Value of an integer global variable.
 *
 * Issue a message to the frontend to return the current value of a signed
 * integer global variable within a thread.
 *
 * @param thread    Thread from which the global variable value was retrieved.
 * @param global    Name of global variable whose value is being returned.
 * @param found     Boolean "true" if that global variable was found, or
 *                  "false" otherwise.
 * @param value     Current value of that variable.
 */
void Senders::globalIntegerValue(const ThreadName& thread,
				 const std::string& global,
				 const bool& found,
				 const int64_t& value)
{
    // Assemble the request into a message
    OpenSS_Protocol_GlobalIntegerValue message;
    message.thread = thread;
    convert(global, message.global);
    message.found = found;
    message.value = value;

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalIntegerValue),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalIntegerValue),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Value of a job description variable.
 *
 * Issue a message to the frontend to return the current value of a job
 * description global variable within a thread.
 *
 * @param thread    Thread from which the global variable value was retrieved.
 * @param global    Name of global variable whose value is being returned.
 * @param found     Boolean "true" if that global variable was found, or
 *                  "false" otherwise.
 * @param job       Current value of that variable.
 */
void Senders::globalJobValue(const ThreadName& thread,
			     const std::string& global,
			     const bool& found,
			     const Job& job)
{
    // Assemble the request into a message
    OpenSS_Protocol_GlobalJobValue message;
    message.thread = thread;
    convert(global, message.global);
    message.found = found;
    // This should be :: not anonymous:: but gcc can not handle that
    // With dyninst-8.0.0 adding more ::convert methods gcc complains with an error.
    anonymous::convert(job, message.value);
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif
    
    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalJobValue),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_GLOBAL_JOB_VALUE, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalJobValue),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Value of a string global variable.
 *
 * Issue a message to the frontend to return the current value of a character
 * string global variable within a thread.
 *
 * @param thread    Thread from which the global variable value was retrieved.
 * @param global    Name of global variable whose value is being returned.
 * @param found     Boolean "true" if that global variable was found, or
 *                  "false" otherwise.
 * @param value     Current value of that variable.
 */
void Senders::globalStringValue(const ThreadName& thread,
				const std::string& global,
				const bool& found,
				const std::string& value)
{
    // Assemble the request into a message
    OpenSS_Protocol_GlobalStringValue message;
    message.thread = thread;
    convert(global, message.global);
    message.found = found;
    convert(value, message.value);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalStringValue),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_GLOBAL_STRING_VALUE, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalStringValue),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Threads have been instrumented.
 *
 * Issue a message to the frontend to indicate that the specified collector
 * has placed instrumentation in the specified threads.
 *
 * @param threads      Threads to which instrumentation was applied.
 * @param collector    Collector which is instrumenting.
 */
void Senders::instrumented(const ThreadNameGroup& threads,
			   const Collector& collector)
{
    // Assemble the request into a message
    OpenSS_Protocol_Instrumented message;
    message.threads = threads;
    message.collector = collector;

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Instrumented),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_INSTRUMENTED, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Instrumented),
	reinterpret_cast<char*>(&message)
	);
}



/**
 * Linked object has been loaded.
 *
 * Issue a message to the frontend to indicate that the specified linked object
 * has been loaded into the address space of the specified threads. Includes the
 * time at which the load occured as well as a description of what was loaded.
 *
 * @param threads          Threads which loaded the linked object.
 * @param time             Time at which the linked object was loaded.
 * @param range            Address range at which this linked object was loaded.
 * @param linked_object    Name of the linked object's file.
 * @param is_executable    Boolean "true" if this linked object is an
 *                         executable, or "false" otherwise.
 */
void Senders::loadedLinkedObject(const ThreadNameGroup& threads,
				 const Time& time,
				 const AddressRange& range,
				 const FileName& linked_object,
				 const bool& is_executable)
{
    // Assemble the request into a message
    OpenSS_Protocol_LoadedLinkedObject message;
    message.threads = threads;
    message.time = time.getValue();
    message.range.begin = range.getBegin().getValue();
    message.range.end = range.getEnd().getValue();
    message.linked_object = linked_object;
    message.is_executable = is_executable;
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_LoadedLinkedObject),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_LOADED_LINKED_OBJECT, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_LoadedLinkedObject),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Report an error.
 *
 * Issue a message to the frontend to indicate that an error has occured.
 *
 * @param text    Text describing the error that has occured.
 */
void Senders::reportError(const std::string& text)
{
    // Assemble the request into a message
    OpenSS_Protocol_ReportError message;
    convert(text, message.text);
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ReportError),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_REPORT_ERROR, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ReportError),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Standard error stream.
 *
 * Issue a request to the backends that data be written to the standard error
 * stream of a created process.
 *
 * @param thread    Thread which generated the data on its standard error
 *                  stream.
 * @param data      Data that was generated.
 */
void Senders::stdErr(const ThreadName& thread, const Blob& data)
{
    // Assemble the request into a message
    OpenSS_Protocol_StdErr message;
    message.thread = thread;
    OpenSpeedShop::Framework::convert(data, message.data);
    
#ifndef NDEBUG
    if(Backend::isStdioDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdErr),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_STDERR, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdErr),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Standard output stream.
 *
 * Issue a request to the backends that data be written to the standard output
 * stream of a created process.
 *
 * @param thread    Thread which generated the data on its standard output
 *                  stream.
 * @param data      Data that was generated.
 */
void Senders::stdOut(const ThreadName& thread, const Blob& data)
{
    // Assemble the request into a message
    OpenSS_Protocol_StdOut message;
    message.thread = thread;
    OpenSpeedShop::Framework::convert(data, message.data);
    
#ifndef NDEBUG
    if(Backend::isStdioDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdOut),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_STDOUT, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StdOut),
	reinterpret_cast<char*>(&message)
	);    
}



/**
 * Symbol table.
 *
 * Issue a message to the frontend to provide the symbol table for a single
 * linked object.
 *
 * @param experiments      Experiments referencing the linked object.
 * @param linked_object    Name of the linked object's file.
 * @param symbol_table     Symbol table for this linked object.
 */
void Senders::symbolTable(const ExperimentGroup& experiments,
			  const FileName& linked_object,
			  const SymbolTable& symbol_table)
{
    // Assemble the request into a message
    OpenSS_Protocol_SymbolTable message = symbol_table;
    message.experiments = experiments;
    message.linked_object = linked_object;
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message, Backend::isSymbolsDebugEnabled());
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SymbolTable),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_SYMBOL_TABLE, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SymbolTable),
	reinterpret_cast<char*>(&message)
	);
}



/**
 * Thread's state has changed.
 *
 * Issue a message to the frontend to indicate that the current state of
 * every thread in the specified group has changed to the specified value.
 *
 * @param threads    Threads whose state has changed.
 * @param state      State to which these threads have changed.
 */
void Senders::threadsStateChanged(const ThreadNameGroup& threads,
				  const OpenSS_Protocol_ThreadState& state)
{
    // Assemble the request into a message
    OpenSS_Protocol_ThreadsStateChanged message;
    message.threads = threads;
    message.state = state;
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ThreadsStateChanged),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_THREADS_STATE_CHANGED, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ThreadsStateChanged),
	reinterpret_cast<char*>(&message)
	);

    //
    // Update the state of these threads in the thread table. This information
    // is used by Dyninst::sendThreadStateChanges() to insure it sends the
    // minimal possible set of ThreadsStateChanged messages to the frontend.
    //
    ThreadTable::TheTable.setThreadState(threads, state);
}



/**
 * Linked object has been unloaded.
 *
 * Issue a message to the frontend to indicate that the specified linked object
 * has been unloaded from the address space of the specified threads. Includes
 * the time at which the unload occured as well as a description of what was
 * unloaded.
 *
 * @param threads          Threads which unloaded the linked object.
 * @param time             Time at which the linked object was unloaded.
 * @param linked_object    Name of the linked object's file.
 */
void Senders::unloadedLinkedObject(const ThreadNameGroup& threads,
				   const Time& time,
				   const FileName& linked_object)
{
    // Assemble the request into a message
    OpenSS_Protocol_UnloadedLinkedObject message;
    message.threads = threads;
    message.time = time.getValue();
    message.linked_object = linked_object;
    
#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // Encode the message into a blob
    Blob blob(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_UnloadedLinkedObject),
	&message
	);

    // Send the encoded message to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT, blob);

    // Destroy the message
    xdr_free(
        reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_UnloadedLinkedObject),
	reinterpret_cast<char*>(&message)
	);
}



/**
 * Performance data.
 *
 * Issue a message to the frontend containing performance data. The actual
 * contents of the performance data blob is entirely up to the collector that
 * gathered the data.
 *
 * @param blob    Blob containing performance data.
 */
void Senders::performanceData(const Blob& blob)
{
#ifndef NDEBUG
    if(Backend::isPerfDataDebugEnabled()) {
	OpenSS_Protocol_Blob perfdata;
	perfdata.data.data_len = blob.getSize();
	perfdata.data.data_val = 
	    reinterpret_cast<uint8_t*>(const_cast<void*>(blob.getContents()));
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Senders::performanceData("
	       << std::endl << toString(perfdata) << ")" << std::endl;
	std::cerr << output.str();
    }
#endif

    // Send the blob to the frontend
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_PERFORMANCE_DATA, blob);
}
