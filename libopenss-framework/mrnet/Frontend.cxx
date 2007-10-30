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
 * Definition of the Frontend namespace.
 *
 */

#include "Blob.hxx"
#include "DataQueues.hxx"
#include "Frontend.hxx"
#include "MessageCallbackTable.hxx"
#include "Path.hxx"
#include "Protocol.h"

#include <mrnet/MRNet.h>
#include <pthread.h>
#include <stdexcept>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Singleton message callback table. */
    MessageCallbackTable message_callback_table;

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
	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Suspend ourselves for one second
	    struct timespec wait;
	    wait.tv_sec = 1;
	    wait.tv_nsec = 0;
	    nanosleep(&wait, NULL);

	    // Receive the next available message from the backends
	    int tag = -1;
	    MRN::Packet* packet = NULL;
	    int retval = upstream->recv(&tag, &packet, false);
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
		    message_callback_table.getCallbacksByTag(tag);

		// Iterate over the callbacks
		for(std::set<MessageCallback>::const_iterator
			i = callbacks.begin(); i != callbacks.end(); ++i)
		    
		    // Dispatch the message to this callback
		    (**i)(blob);

	    }

	    // Request that all performance data be flushed to databases
	    DataQueues::flushPerformanceData();
	    
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
    // Pass the request through to the singleton message callback table
    message_callback_table.addCallback(tag, callback);
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
    // Pass the request through to the singleton message callback table
    message_callback_table.removeCallback(tag, callback);
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
    // Initialize MRNet (participating as the frontend)
    network = new MRN::Network(topology_file.c_str(), "openssd", NULL);
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

    // TODO: destroy upstream and network???

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
    Assert(upstream->send(tag, "auc", blob.getContents(), blob.getSize()) == 0);
}



/**
 * Send a message to a subset of backends.
 *
 * ...
 *
 * @param tag        Tag for the message to be sent.
 * @param blob       Blob containing the message.
 * @param threads    ...
 */
void Frontend::sendToBackends(const int& tag, const Blob& blob,
			      const OpenSS_Protocol_ThreadNameGroup& threads)
{
}
