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
 * Definition of the Backend namespace.
 *
 */

#include "Backend.hxx"
#include "Blob.hxx"
#include "MessageCallbackTable.hxx"
#include "Protocol.h"

#include <mrnet/MRNet.h>
#include <stdexcept>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Singleton message callback table. */
    MessageCallbackTable message_callback_table;

    /** MRNet network containing this backend. */
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
    /** Flag indicating if debugging for the backend is enabled. */
    bool is_backend_debug_enabled = false;
#endif    



    /**
     * Monitor thread function.
     *
     * Monitor thread for executing this backend's message pump. By executing
     * the pump in a separate thread, it is insured that incoming messages will
     * be handled in a timely manner. The monitor simply receives the incoming
     * messages and dispatches them to the appropriate callback(s) until it is
     * instructed to exit.
     */
    void* monitorThread(void*)
    {
	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Suspend ourselves for one second
	    struct timespec wait;
	    wait.tv_sec = 1;
	    wait.tv_nsec = 0;
	    nanosleep(&wait, NULL);
	    
	    // Receive all available messages from the backends
	    int retval = 1;
	    while(retval == 1) {

		// Receive the next available message from the backends
		int tag = -1;
		MRN::Stream* stream = NULL;
		MRN::Packet* packet = NULL;
		retval = network->recv(&tag, &packet, &stream, false);
		Assert(retval != -1);
		if(retval == 1) {
		    Assert(packet != NULL);
		    
		    // Decode the packet containing the message
		    void* contents = NULL;
		    unsigned size = 0;
		    Assert(MRN::Stream::unpack(packet, "%auc",
					       &contents, &size) == 0);
		    Blob blob = ((size == 0) || (contents == NULL)) ?
			Blob() : Blob(size, contents);
		    
		    // Get the proper callbacks for this message's tag
		    std::set<MessageCallback> callbacks =
			message_callback_table.getCallbacksByTag(tag);
		    
		    // Iterate over the callbacks
		    for(std::set<MessageCallback>::const_iterator
			    i = callbacks.begin(); i != callbacks.end(); ++i)
			
			// Dispatch the message to this callback
			(**i)(blob);
		    
		}

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
 * specified tag arrive at this backend's message pump.
 *
 * @note    Multiple callbacks can be registered for a given message tag. In
 *          this case each callback will be called for every received message.
 *
 * @param tag         Message tag for which to register a new callback.
 * @param callback    Callback to register for that message tag.
 */
void Backend::registerCallback(const int& tag, const MessageCallback callback)
{
    // Pass the request through to the singleton message callback table
    message_callback_table.addCallback(tag, callback);
}



/**
 * Unregister a callback with the message pump.
 *
 * Requests that the specified function no longer be called when messages
 * with the specified tag arrive at this backend's message pump.
 *
 * @param tag         Message tag for which to unregister an existing callback.
 * @param callback    Callback to unregister for that message tag.
 */
void Backend::unregisterCallback(const int& tag, const MessageCallback callback)
{
    // Pass the request through to the singleton message callback table
    message_callback_table.removeCallback(tag, callback);
}



/**
 * Start this backend's message pump.
 *
 * Initializes the MRNet library as a backend, gathering necessary parameters
 * directly from the command-line arguments. Establishes the stream used by
 * backends to pass data to the frontend and then initiates a monitor thread
 * for executing this backend's message pump.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
void Backend::startMessagePump(int argc, char* argv[])
{
#ifndef NDEBUG
    // Determine if debugging for the backend should be enabled
    for(int i = 0; i < argc; ++i)
	if(std::string(argv[i]) == std::string("-debug"))
	    is_backend_debug_enabled = true;
#endif

    // Initialize MRNet (participating as a backend)
    network = new MRN::Network(argc, argv);
    if(network->fail())
	throw std::runtime_error("Unable to initialize MRNet.");
    
    // Create the stream used by backends to pass data to the frontend.
    int tag = -1;
    MRN::Packet* packet = NULL;
    Assert(network->recv(&tag, &packet, &upstream, true) == 1);
    Assert(tag == OPENSS_PROTOCOL_TAG_ESTABLISH_UPSTREAM);
    Assert(upstream != NULL);
    
    // Create the monitor thread
    Assert(pthread_create(&monitor_tid, NULL, monitorThread, NULL) == 0);
}



/**
 * Stop this backend's message pump.
 *
 * Instructs the monitor thread to exit and then waits for it to do so. Also
 * finalizes the MRNet library.
 */
void Backend::stopMessagePump()
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

    // Destroy the stream used by backends to pass data to the frontend
    delete upstream;

    // Finalize MRNet
    delete network;
}



/**
 * Send a message to the frontend.
 *
 * Sends a message to the MRNet frontend.
 *
 * @param tag     Tag for the message to be sent.
 * @param blob    Blob containing the message.
 */
void Backend::sendToFrontend(const int& tag, const Blob& blob)
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
 * Get backend debugging flag.
 *
 * Returns a flag indicating if debugging for the backend is enabled.
 *
 * @return    Boolean "true" if debugging for the backend is enabled,
 *            "false" otherwise.
 */
bool Backend::isDebugEnabled()
{
    return is_backend_debug_enabled;
}
#endif
