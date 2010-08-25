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
 * Definition of the Frontend namespace.
 *
 */

#include "Blob.hxx"
#include "DataQueues.hxx"
#include "Experiment.hxx"
#include "Frontend.hxx"
#include "MessageCallbackTable.hxx"
#include "Path.hxx"
#include "Protocol.h"

#include <algorithm>
#include <mrnet/MRNet.h>
#include <pthread.h>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace OpenSpeedShop::Framework;



namespace {

    // Note: It would seemingly make more sense for the following to be a
    //       simple singleton object rather than a pointer that must then
    //       be checked for proper initialization in various places. The
    //       frontend initialization, however, is done inside ThreadTable's
    //       constructor and the thread table itself is a singleton. Since
    //       C++ doesn't guarantee initialization order, the callbacks
    //       could actually be registered BEFORE the table was properly
    //       initialized. This resulted in the callbacks being removed.
    //       Using a pointer is a bit of a hack that works for now...

    /** Singleton message callback table. */
    MessageCallbackTable* message_callback_table = NULL;

    /** MRNet network containing this frontend. */
    MRN::Network* the_network = NULL;

    /** MRNet stream used by backends to pass data to the frontend. */
    MRN::Stream* upstream = NULL;

    /** Identifier of the monitor thread. */
    pthread_t monitor_tid;

    /** Access-controlled flag used to request that the monitor thread exit. */
    struct {
	bool flag;             /**< Actual flag value. */
	pthread_mutex_t lock;  /**< Mutual exclusion lock for this flag. */
    } monitor_request_exit = {
	false, PTHREAD_MUTEX_INITIALIZER
    };

#ifndef NDEBUG
    /** Flag indicating if debugging for the frontend is enabled. */
    bool is_frontend_debug_enabled = false;

    /** Flag indicating if debugging for performance data is enabled. */
    bool is_perfdata_debug_enabled = false;

    /** Flag indicating if standard I/O debugging is enabled. */
    bool is_stdio_debug_enabled = false;

    /** Flag indicating if symbols debugging is enabled. */
    bool is_symbols_debug_enabled = false;
#endif
    
    

    /**
     * Monitor thread function.
     *
     * Monitor thread for executing the frontend's message pump. By executing
     * the pump in a separate thread, it is insured that incoming messages will
     * be handled in a timely manner. The monitor simply receives the incoming
     * messages and dispatches them to the appropriate callback(s) until it is
     * instructed to exit.
     *
     * @note    This thread is also used to perform the periodic (once per
     *          second) flushing of performance data to experiment databases.
     */
    void* monitorThread(void*)
    {
	// Time when performance data was last flushed to databases
	Time last_flush = Time::Now();

	// Get the MRNet file descriptor
#if defined (MRNET_30)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::Event::DATA_EVENT );
#elif defined (MRNET_22)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_21)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::DATA_EVENT );
#else
        int mrnet_fd = the_network->get_DataNotificationFd();
#endif


	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Initialize the set of incoming file descriptors

	    int nfds = 0;	
	    fd_set readfds;
	    FD_ZERO(&readfds);

	    nfds = std::max(nfds, mrnet_fd + 1);
	    FD_SET(mrnet_fd, &readfds);

	    // Initialize a one second timeout
	    struct timeval timeout;
	    timeout.tv_sec = 1;
	    timeout.tv_usec = 0;

	    // Wait for file descriptor activity or timeout expiration
	    int retval = select(nfds, &readfds, NULL, NULL, &timeout);

	    // Is MRNet indicating there is incoming data available?
	    if((retval > 0) && (FD_ISSET(mrnet_fd, &readfds))) {

		while(true) {
		    
		    // Receive the next available message
		    int tag = -1;
		    MRN::PacketPtr packet;
		    retval = upstream->recv(&tag, packet, false);
		    Assert(retval != -1);
		    if(retval == 0)
			break;
		    Assert(packet != NULL);
		    
		    // Decode the packet containing the message
		    void* contents = NULL;
		    unsigned size = 0;
		    Assert(packet->unpack("%auc", &contents, &size) == 0);
		    Blob blob(size, contents);
		    
		    // Get the proper callbacks for this message's tag
		    std::set<MessageCallback> callbacks =
			message_callback_table->getCallbacksByTag(tag);
		    
		    // Iterate over the callbacks
		    for(std::set<MessageCallback>::const_iterator
			    i = callbacks.begin(); i != callbacks.end(); ++i)
			
			// Dispatch the message to this callback
			(**i)(blob);
		    
		}

		// Reset the MRNet notification descriptor
#if defined(MRNET_30)
		the_network->clear_EventNotificationFd( MRN::Event::DATA_EVENT );
#elif defined(MRNET_22)
		the_network->clear_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_21)
		the_network->clear_EventNotificationFd( MRN::DATA_EVENT );
#else
                the_network->clear_DataNotificationFd();
#endif
		
	    }
	    
	    // Has at least one second past since the last flush?
	    Time now = Time::Now();
	    if((now - last_flush) >= 1000000000 /* 10^9 nS = 1 sec */) {
		last_flush = now;
		
		// Request that all performance data be flushed to databases
		DataQueues::flushPerformanceData();
		
	    }
	    
	    // Exit monitor thread if instructed to do so
	    Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);
	    do_exit = monitor_request_exit.flag;
	    Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);
	    
	}
	
	// Empty, unused, return value from this thread
	return NULL;
    }


    
}



/**
 * Register a callback with the message pump.
 *
 * Requests that the specified function be called when messages with the
 * specified tag arrive at the frontend's message pump.
 *
 * @note    Multiple callbacks can be registered for a given message tag. In
 *          this case each callback will be called for every received message.
 *
 * @param tag         Message tag for which to register a new callback.
 * @param callback    Callback to register for that message tag.
 */
void Frontend::registerCallback(const int& tag, const MessageCallback callback)
{
    // Allocate the message callback table when necessary
    if(message_callback_table == NULL)
	message_callback_table = new MessageCallbackTable();

    // Pass the request through to the singleton message callback table
    message_callback_table->addCallback(tag, callback);
}



/**
 * Unregister a callback with the message pump.
 *
 * Requests that the specified function no longer be called when messages
 * with the specified tag arrive at the frontend's message pump.
 *
 * @param tag         Message tag for which to unregister an existing callback.
 * @param callback    Callback to unregister for that message tag.
 */
void Frontend::unregisterCallback(const int& tag,
				  const MessageCallback callback)
{
    // Allocate the message callback table when necessary
    if(message_callback_table == NULL)
	message_callback_table = new MessageCallbackTable();

    // Pass the request through to the singleton message callback table
    message_callback_table->removeCallback(tag, callback);
}



/**
 * Start the frontend's message pump.
 *
 * Initializes the MRNet library as the frontend, using the specified MRNet
 * topology file for creating the MRNet process tree, and using the "openssd"
 * daemon as the backend process. Establishes the stream used by backends to
 * pass data to the frontend and then initiates a monitor thread for executing
 * the frontend's message pump.
 *
 * @param topology_file    Path of the MRNet topology file.
 */
void Frontend::startMessagePump(const Path& topology_file)
{
    // Allocate the message callback table when necessary
    if(message_callback_table == NULL)
	message_callback_table = new MessageCallbackTable();
    
    // Determine the type of debugging that should be enabled
    is_frontend_debug_enabled = 
	((getenv("OPENSS_DEBUG_MRNET") != NULL) ||
	 (getenv("OPENSS_DEBUG_MRNET_FRONTEND") != NULL));
    bool is_backend_debug_enabled = 
        ((getenv("OPENSS_DEBUG_MRNET") != NULL) ||
	 (getenv("OPENSS_DEBUG_MRNET_BACKEND") != NULL));
    is_perfdata_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_PERFDATA") != NULL);
    is_stdio_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_STDIO") != NULL);
    is_symbols_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_SYMBOLS") != NULL);
    bool is_tracing_debug_enabled = 
	(getenv("OPENSS_DEBUG_MRNET_TRACING") != NULL);
    bool is_valgrind_debug_enabled = 
	(getenv("OPENSS_DEBUG_MRNET_VALGRIND") != NULL);

    // Construct the arguments to the MRNet backend
    std::string executable =
	is_valgrind_debug_enabled ? "valgrind" : "openssd";
    std::vector<std::string> args;
    if(is_valgrind_debug_enabled) {
	args.push_back("--log-file=/tmp/openssd-valgrind");
	args.push_back("openssd");
    }
    if(is_backend_debug_enabled)
	args.push_back("--debug");
    if(is_perfdata_debug_enabled)
	args.push_back("--perfdata-debug");
    if(is_stdio_debug_enabled)
        args.push_back("--stdio-debug");
    if(is_symbols_debug_enabled)
	args.push_back("--symbols-debug");
    
    // Translate the arguments into an argv-style argument list
    const char** argv = new const char*[args.size() + 1];
    for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
	argv[i] = args[i].c_str();
    argv[args.size()] = NULL;

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
        std::stringstream output;
        output << "[TID " << pthread_self() << "] Frontend::"
	       << "startMessagePump() using MRNet topology file \""
	       << topology_file.getNormalized() << "\"." << std::endl;
        std::cerr << output.str();
    }
#endif

    // Initialize MRNet (participating as the frontend)
    if(is_tracing_debug_enabled)
	MRN::set_OutputLevel(5);
#if defined(MRNET_30)
    the_network = MRN::Network::CreateNetworkFE(topology_file.getNormalized().c_str(),
				   executable.c_str(), argv);
#elif defined(MRNET_22)
    the_network = MRN::Network::CreateNetworkFE(topology_file.getNormalized().c_str(),
				   executable.c_str(), argv);
#else
    the_network = new MRN::Network(topology_file.getNormalized().c_str(),
				   executable.c_str(), argv);
#endif

    if(the_network->has_Error())
	throw std::runtime_error("Unable to initialize MRNet.");

    // Destroy the argv-style argument list
    delete [] argv;
    
    // Create the stream used by backends to pass data to the frontend.
#if defined(MRNET_30)
    upstream = the_network->new_Stream(the_network->get_BroadcastCommunicator(),
#elif defined(MRNET_22)
    upstream = the_network->new_Stream(the_network->get_BroadcastCommunicator(),
#elif defined(MRNET_21)
    upstream = the_network->new_Stream(the_network->get_BroadcastCommunicator(),
#else
    upstream = network->new_Stream(the_network->get_BroadcastCommunicator(),
#endif
	 			   MRN::TFILTER_NULL,
				   MRN::SFILTER_DONTWAIT,
				   MRN::TFILTER_NULL);
    Assert(upstream != NULL);
    Assert(upstream->send(OPENSS_PROTOCOL_TAG_ESTABLISH_UPSTREAM, "") == 0);
    Assert(upstream->flush() == 0);
    
    // Create the monitor thread
    Assert(pthread_create(&monitor_tid, NULL, monitorThread, NULL) == 0);

#ifndef WDH_TEMPORARY_HACK
    //
    // TODO: Temporarily sleep for five seconds to get around what appears to
    //       be a race condition in MRNet 2.0.1b that causes the daemon to sit
    //       around waiting for messages from the frontend that never arrive.
    //
    sleep(5);
#endif  // WDH_TEMPORARY_HACK
}



/**
 * Stop the frontend's message pump.
 *
 * Instructs the monitor thread to exit and then waits for it to do so. Also
 * finalizes the MRNet library.
 */
void Frontend::stopMessagePump()
{
    // Instruct the monitor thread to exit
    Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);
    monitor_request_exit.flag = true;
    Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);

    // Wait for the monitor thread to actually exit
    Assert(pthread_join(monitor_tid, NULL) == 0);

    // Cleanup the exit request flag
    Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);
    monitor_request_exit.flag = false;
    Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);

    // Instruct the backends to shutdown
    sendToAllBackends(OPENSS_PROTOCOL_TAG_SHUTDOWN_BACKENDS, Blob());

    // Iterate until all backends are ready to shutdown
    for(int remaining_endpoints =
	    the_network->get_BroadcastCommunicator()->get_EndPoints().size();
	remaining_endpoints > 0;) {

	// Receive the next available message
	int tag = -1;
	MRN::PacketPtr packet;
	int retval = upstream->recv(&tag, packet, false);
	Assert(retval != -1);
	if(retval == 0)
	    continue;
	Assert(packet != NULL);

	// Was this a backend reporting its readiness to shutdown?
	if (tag == OPENSS_PROTOCOL_TAG_READY_TO_SHUTDOWN)
	    remaining_endpoints--;
	
    }
    
    // Destroy the stream used by backends to pass data to the frontend
    delete upstream;
    
    // Finalize MRNet
    delete the_network;
}



/**
 * Send a message to the backends.
 *
 * Sends a message to the MRNet backends. The passed thread group specifies
 * the subset of backends that must receive the message and the distribution
 * of the message is restricted to that subset of backends.
 *
 * @param tag        Tag for the message to be sent.
 * @param blob       Blob containing the message.
 * @param threads    Threads referenced by this message. Used to determine
 *                   the subset of backends that must receive the message.
 */
void Frontend::sendToBackends(const int& tag, const Blob& blob,
			      const OpenSS_Protocol_ThreadNameGroup& threads)
{
    // Check assertions
    Assert(the_network != NULL);

    // Find all the unique host names in the specified thread group
    std::set<std::string> hosts;
    for(int i = 0; i < threads.names.names_len; ++i)
	if(threads.names.names_val[i].host != NULL)
	    hosts.insert(threads.names.names_val[i].host);

    // Find all the endpoints corresponding to those hosts
    std::set<MRN::CommunicationNode*> endpoints;
    const std::set<MRN::CommunicationNode*>& all_endpoints = 
	the_network->get_BroadcastCommunicator()->get_EndPoints();
    for(std::set<MRN::CommunicationNode*>::const_iterator
	    i = all_endpoints.begin(); i != all_endpoints.end(); ++i)
	if(hosts.find(Experiment::getCanonicalName((*i)->get_HostName())) != 
	   hosts.end())
	    endpoints.insert(*i);

    // Create a communicator and stream for communicating with those endpoints
    MRN::Communicator* communicator = the_network->new_Communicator(endpoints);
    Assert(communicator != NULL);
    MRN::Stream* stream = the_network->new_Stream(communicator,
						  MRN::TFILTER_NULL,
						  MRN::SFILTER_DONTWAIT,
						  MRN::TFILTER_NULL);
    Assert(stream != NULL);

    // Send the message
    Assert(stream->send(tag, "%auc", blob.getContents(), blob.getSize()) == 0);
    Assert(stream->flush() == 0);

    // Destroy the stream and communicator
    delete stream;
    delete communicator;
}



/**
 * Send a message to all the backends.
 *
 * Sends a message to all of the MRNet backends.
 *
 * @param tag     Tag for the message to be sent.
 * @param blob    Blob containing the message.
 */
void Frontend::sendToAllBackends(const int& tag, const Blob& blob)
{
    // Check asserions
    Assert(upstream != NULL);

    // Send the message
    Assert(upstream->send(tag, "%auc",
			  blob.getContents(), blob.getSize()) == 0);
    Assert(upstream->flush() == 0);
}



/**
 * Test if a host has a backend.
 *
 * Returns a boolean value indicating if this host has a MRNet backend.
 *
 * @param host   Host to test.
 * @return       Boolean "true" if this host has a MRNet backend,
 *               "false" otherwise.
 */
bool Frontend::hasBackend(const std::string& host)
{
    // Check assertions
    Assert(the_network != NULL);

    // Iterate over all the MRNet endpoints
    const std::set<MRN::CommunicationNode*>& all_endpoints = 
	the_network->get_BroadcastCommunicator()->get_EndPoints();
    for(std::set<MRN::CommunicationNode*>::const_iterator
	    i = all_endpoints.begin(); i != all_endpoints.end(); ++i)

	// Host has an MRNet backend if it matches the host of this endpoint
	if(Experiment::getCanonicalName((*i)->get_HostName()) ==
	   Experiment::getCanonicalName(host))
	    return true;

    // Otherwise this host has no MRNet backend
    return false;
}



#ifndef NDEBUG
/**
 * Get frontend debugging flag.
 *
 * Returns a flag indicating if debugging for the frontend is enabled.
 *
 * @return    Boolean "true" if debugging for the frontend is enabled,
 *            "false" otherwise.
 */
bool Frontend::isDebugEnabled()
{
    return is_frontend_debug_enabled;
}



/**
 * Get performance data debugging flag.
 *
 * Returns a flag indicating if performance data debugging is enabled.
 *
 * @return    Boolean "true" if debugging for performance data is enabled,
 *            "false" otherwise.
 */
bool Frontend::isPerfDataDebugEnabled()
{
    return is_perfdata_debug_enabled;
}



/**
 * Get standard I/O debugging flag.
 *
 * Returns a flag indicating if standard I/O debugging is enabled.
 *
 * @return    Boolean "true" if debugging for standard I/O is enabled,
 *            "false" otherwise.
 */
bool Frontend::isStdioDebugEnabled()
{
    return is_stdio_debug_enabled;
}



/**
 * Get symbols debugging flag.
 *
 * Returns a flag indicating if symbols debugging is enabled.
 *
 * @return    Boolean "true" if debugging for symbols is enabled,
 *            "false" otherwise.
 */
bool Frontend::isSymbolsDebugEnabled()
{
    return is_symbols_debug_enabled;
}
#endif    
