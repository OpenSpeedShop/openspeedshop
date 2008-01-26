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
    MRN::Network* network = NULL;

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

	// Get the file descriptors used by the incoming backend connections
	int* mrnet_fds = NULL;
	unsigned int num_mrnet_fds = 0;
	Assert(network->get_SocketFd(&mrnet_fds, &num_mrnet_fds) == 0);
	
	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Initialize the set of incoming file descriptors
	    int nfds = 0;	
	    fd_set readfds;
	    FD_ZERO(&readfds);
	    for(unsigned int i = 0; i < num_mrnet_fds; ++i) {
		nfds = std::max(nfds, mrnet_fds[i] + 1);
		FD_SET(mrnet_fds[i], &readfds);
	    }
	    
	    // Initialize a one second timeout
	    struct timeval timeout;
	    timeout.tv_sec = 1;
	    timeout.tv_usec = 0;

	    // Wait for file descriptor activity or timeout expiration
	    int retval = select(nfds, &readfds, NULL, NULL, &timeout);
	    
	    // Receive all available messages from the backends
	    while(retval > 0) {
		
		// Receive the next available message
		int tag = -1;
		MRN::Packet* packet = NULL;
		retval = upstream->recv(&tag, &packet, false);
		Assert(retval != -1);
		if(retval == 1) {
		    Assert(packet != NULL);
		    
		    // Decode the packet containing the message
		    void* contents = NULL;
		    unsigned size = 0;
		    Assert(MRN::Stream::unpack(packet, "%auc",
					       &contents, &size) == 0);
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

	// Destroy the array of file descriptors
	delete [] mrnet_fds;
	
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
    static const char* DebugArgs[2] = { 
	"--debug", NULL 
    };
    static const char* SymbolsDebugArgs[3] = { 
	"--debug", "--symbols-debug", NULL 
    };

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
    is_symbols_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_SYMBOLS") != NULL);
    
    // Initialize MRNet (participating as the frontend)
    network = new MRN::Network(topology_file.getNormalized().c_str(), "openssd",
			       is_backend_debug_enabled ? 
			       (is_symbols_debug_enabled ? 
				SymbolsDebugArgs : DebugArgs) :
			       NULL);
    if(network->fail())
	throw std::runtime_error("Unable to initialize MRNet.");
    
    // Create the stream used by backends to pass data to the frontend.
    upstream = network->new_Stream(network->get_BroadcastCommunicator(),
	 			   MRN::TFILTER_NULL,
				   MRN::SFILTER_DONTWAIT,
				   MRN::TFILTER_NULL);
    Assert(upstream != NULL);
    Assert(upstream->send(OPENSS_PROTOCOL_TAG_ESTABLISH_UPSTREAM, "") == 0);
    Assert(upstream->flush() == 0);
    
    // Create the monitor thread
    Assert(pthread_create(&monitor_tid, NULL, monitorThread, NULL) == 0);
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
    
    // Destroy the stream used by backends to pass data to the frontend
    delete upstream;
    
    // Finalize MRNet
    delete network;
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
    Assert(network != NULL);

    // Find all the unique host names in the specified thread group
    std::set<std::string> hosts;
    for(int i = 0; i < threads.names.names_len; ++i)
	if(threads.names.names_val[i].host != NULL)
	    hosts.insert(threads.names.names_val[i].host);

    // Find all the endpoints corresponding to those hosts
    std::vector<MRN::EndPoint*> endpoints;
    const std::vector<MRN::EndPoint*>& all_endpoints = 
	network->get_BroadcastCommunicator()->get_EndPoints();
    for(std::vector<MRN::EndPoint*>::const_iterator
	    i = all_endpoints.begin(); i != all_endpoints.end(); ++i)
	if(hosts.find(Experiment::getCanonicalName((*i)->get_HostName())) != 
	   hosts.end())
	    endpoints.push_back(*i);
    
    // Create a communicator and stream for communicating with those endpoints
    MRN::Communicator* communicator = network->new_Communicator(endpoints);
    Assert(communicator != NULL);
    MRN::Stream* stream = network->new_Stream(communicator,
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
