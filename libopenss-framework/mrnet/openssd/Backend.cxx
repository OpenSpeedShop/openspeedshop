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
 * Definition of the Backend namespace.
 *
 */

#include "Backend.hxx"
#include "Blob.hxx"
#include "DyninstCallbacks.hxx"
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

    /** Flag indicating if symbols debugging is enabled. */
    bool is_symbols_debug_enabled = false;
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
	// Note: Ideally the following Dyninst bpatch singleton object would
	//       be declared inside the unnamed namespace of "openssd.cxx" and
	//       the Dyninst callback registrations would take place inside
	//       main(). Per Matt Legendre on JAN-09-2008, only one thread is
	//       allowed to access the Dyninst API. Since the monitor thread
	//       is the one that does all the real Dyninst work in response
	//       to messages from the frontend, this stuff needs to be here
	//       instead.

	// Singleton Dyninst bpatch object
	BPatch TheBPatch;
	
	// Register callbacks with Dyninst
	BPatch* bpatch = BPatch::getBPatch();
	Assert(bpatch != NULL);
	bpatch->registerDynLibraryCallback(DyninstCallbacks::dynLibrary);
	bpatch->registerErrorCallback(DyninstCallbacks::error);
	bpatch->registerExecCallback(DyninstCallbacks::exec);
	bpatch->registerExitCallback(DyninstCallbacks::exit);
	bpatch->registerPostForkCallback(DyninstCallbacks::postFork);
	bpatch->registerThreadEventCallback(BPatch_threadCreateEvent,
					    DyninstCallbacks::threadCreate);
	bpatch->registerThreadEventCallback(BPatch_threadDestroyEvent,
					    DyninstCallbacks::threadDestroy);
	
	// TODO: We need to register some sort of callback to pickup when a
	//       process under our control stops at a breakpoint. Per Matt
	//       Legendre on DEC-31-2007, Dyninst does not currently provide
	//       any sort of callback for this. The only way to do this is
	//       to track process state inside the daemon and have a thread
	//       do a Dyninst waitForStatusChange() and watch for which
	//       process has changed state, and if it has stopped.

	// Instruct Dyninst to give source statement info with full path names
	bpatch->truncateLineInfoFilenames(false);

	// Note: End of code that would normally be in "openssd.cxx".

	// Get the Dyninst and MRNet file descriptors
	int bpatch_fd = bpatch->getNotificationFD();
	int mrnet_fd = network->get_SocketFd();
	
	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Initialize the set of incoming file descriptors
	    int nfds = 0;
	    fd_set readfds;
	    FD_ZERO(&readfds);
	    nfds = std::max(nfds, bpatch_fd + 1);
	    FD_SET(bpatch_fd, &readfds);
	    nfds = std::max(nfds, mrnet_fd + 1);
	    FD_SET(mrnet_fd, &readfds);
	    
	    // Initialize a one second timeout
	    struct timeval timeout;
	    timeout.tv_sec = 1;
	    timeout.tv_usec = 0;
	    
 	    // Wait for file descriptor activity or timeout expiration
	    int retval = select(nfds, &readfds, NULL, NULL, &timeout);

	    // Is Dyninst indicating there has been a status change?
	    if((retval > 0) && (FD_ISSET(bpatch_fd, &readfds))) {

		// Give Dyninst the opportunity to poll for any status changes
		bpatch->pollForStatusChange();
		
	    }

	    // Receive all available messages from the frontend
	    else while(retval > 0) {

		// Receive the next available message from the frontend
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
	if(std::string(argv[i]) == std::string("--debug"))
	    is_backend_debug_enabled = true;
	else if(std::string(argv[i]) == std::string("--symbols-debug"))
	    is_symbols_debug_enabled = true;
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



/**
 * Get symbols debugging flag.
 *
 * Returns a flag indicating if symbols debugging is enabled.
 *
 * @return    Boolean "true" if debugging for symbols is enabled,
 *            "false" otherwise.
 */
bool Backend::isSymbolsDebugEnabled()
{
    return is_symbols_debug_enabled;
}
#endif
