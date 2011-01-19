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
 * Definition of the Backend namespace.
 *
 */

#include "Backend.hxx"
#include "Blob.hxx"
#include "Dyninst.hxx"
#include "MessageCallbackTable.hxx"
#include "Protocol.h"
#include "Senders.hxx"
#include "ThreadNameGroup.hxx"
#include "ThreadTable.hxx"

#include <mrnet/MRNet.h>
#include <stdexcept>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Singleton message callback table. */
    MessageCallbackTable message_callback_table;

    /** MRNet network containing this backend. */
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
    /** Flag indicating if debugging for the backend is enabled. */
    bool is_backend_debug_enabled = false;

    /** Flag indicating if performance data debugging is enabled. */
    bool is_perfdata_debug_enabled = false;

    /** Flag indicating if standard I/O debugging is enabled. */
    bool is_stdio_debug_enabled = false;

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
	bpatch->registerDynLibraryCallback(
	    OpenSpeedShop::Framework::Dyninst::dynLibrary
	    );
	bpatch->registerErrorCallback(
	    OpenSpeedShop::Framework::Dyninst::error
	    );
	bpatch->registerExecCallback(
	    OpenSpeedShop::Framework::Dyninst::exec
	    );
	bpatch->registerExitCallback(
            OpenSpeedShop::Framework::Dyninst::exit
	    );
	bpatch->registerPostForkCallback(
            OpenSpeedShop::Framework::Dyninst::postFork
	    );
	bpatch->registerThreadEventCallback(
            BPatch_threadCreateEvent,
	    OpenSpeedShop::Framework::Dyninst::threadCreate
	    );
	bpatch->registerThreadEventCallback(
            BPatch_threadDestroyEvent,
	    OpenSpeedShop::Framework::Dyninst::threadDestroy
	    );
	
	// Instruct Dyninst to give source statement info with full path names
	bpatch->truncateLineInfoFilenames(false);

	// Note: End of code that would normally be in "openssd.cxx".

	// Declare the buffer used when reading stdout/stderr streams
	static char stream_buffer[4 * 1024];

	// Get the Dyninst file descriptor
	int bpatch_fd = bpatch->getNotificationFD();

	// Get the MRNet file descriptor
#if defined(MRNET_21)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_22)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_30)
	int mrnet_fd = the_network->get_EventNotificationFd( MRN::Event::DATA_EVENT );
#else
        int mrnet_fd = the_network->get_DataNotificationFd();
#endif

	// Run the message pump until instructed to exit
	for(bool do_exit = false; !do_exit;) {

	    // Get the file descriptors used to monitor stdout/stderr streams
	    std::set<int> stdout_fds = ThreadTable::TheTable.getStdOutFDs();
	    std::set<int> stderr_fds = ThreadTable::TheTable.getStdErrFDs();

	    // Initialize the set of incoming file descriptors
	    
	    int nfds = 0;
	    fd_set readfds;
	    FD_ZERO(&readfds);
	    
	    nfds = std::max(nfds, bpatch_fd + 1);
	    FD_SET(bpatch_fd, &readfds);

	    nfds = std::max(nfds, mrnet_fd + 1);
	    FD_SET(mrnet_fd, &readfds);

	    for(std::set<int>::const_iterator
		    i = stdout_fds.begin(); i != stdout_fds.end(); ++i) {
		nfds = std::max(nfds, *i + 1);
		FD_SET(*i, &readfds);
	    }

	    for(std::set<int>::const_iterator
		    i = stderr_fds.begin(); i != stderr_fds.end(); ++i) {
		nfds = std::max(nfds, *i + 1);
		FD_SET(*i, &readfds);
	    }
	    
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
		
		// Update the frontend with any new thread state changes
		OpenSpeedShop::Framework::Dyninst::sendThreadStateUpdates();
		
	    }

	    // Is MRNet indicating there is incoming data available?
	    if((retval > 0) && (FD_ISSET(mrnet_fd, &readfds))) {

		while(true) {

		    // Receive the next available message
		    int tag = -1;
		    MRN::Stream* stream = NULL;
		    MRN::PacketPtr packet;
		    retval = the_network->recv(&tag, packet, &stream, false);
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
			message_callback_table.getCallbacksByTag(tag);
		    
		    // Iterate over the callbacks
		    for(std::set<MessageCallback>::const_iterator
			    i = callbacks.begin(); i != callbacks.end(); ++i)
			
			// Dispatch the message to this callback
			(**i)(blob);
		    
		}

		// Reset the MRNet notification descriptor
#if defined(MRNET_21)
		the_network->clear_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_22)
		the_network->clear_EventNotificationFd( MRN::DATA_EVENT );
#elif defined(MRNET_30)
		the_network->clear_EventNotificationFd( MRN::Event::DATA_EVENT );
#else
                the_network->clear_DataNotificationFd();
#endif
		
	    }

	    // Handle any data available on incoming stdout streams
	    for(std::set<int>::const_iterator
		    i = stdout_fds.begin(); i != stdout_fds.end(); ++i)
		if((retval > 0) && FD_ISSET(*i, &readfds)) {

		    // Get the thread names for this stdout stream
		    ThreadNameGroup names = ThreadTable::TheTable.getNames(*i);
		    
		    // Read from the stream into the stream buffer
		    int retval = read(*i, stream_buffer, sizeof(stream_buffer));
		    Assert((retval >= 0) || 
			   ((retval == -1) && (errno == EINTR)));
		    
		    // Send contents (if any) to the frontend
		    if((retval > 0) && (!names.empty()))
			Senders::stdOut(*(names.begin()),
					Blob(retval, stream_buffer));
		    
		}

	    // Handle any data available on incoming stderr streams
	    for(std::set<int>::const_iterator
		    i = stderr_fds.begin(); i != stderr_fds.end(); ++i)
		if((retval > 0) && FD_ISSET(*i, &readfds)) {

		    // Get the thread names for this stderr stream
		    ThreadNameGroup names = ThreadTable::TheTable.getNames(*i);
		    
		    // Read from the stream into the stream buffer
		    int retval = read(*i, stream_buffer, sizeof(stream_buffer));
		    Assert((retval >= 0) || 
			   ((retval == -1) && (errno == EINTR)));
		    
		    // Send contents (if any) to the frontend
		    if((retval > 0) && (!names.empty()))
			Senders::stdErr(*(names.begin()),
					Blob(retval, stream_buffer));

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
	else if(std::string(argv[i]) == std::string("--perfdata-debug"))
	    is_perfdata_debug_enabled = true;
	else if(std::string(argv[i]) == std::string("--symbols-debug"))
	    is_symbols_debug_enabled = true;
	else if(std::string(argv[i]) == std::string("--stdio-debug"))
	    is_stdio_debug_enabled = true;
#endif

    // Initialize MRNet (participating as a backend)
#if defined(MRNET_30)
    the_network = MRN::Network::CreateNetworkBE(argc, argv);
#elif defined(MRNET_22)
    the_network = MRN::Network::CreateNetworkBE(argc, argv);
#else
    the_network = new MRN::Network(argc, argv);
#endif
    if(the_network->has_Error())
	throw std::runtime_error("Unable to initialize MRNet.");

#if defined(MRNET_301) 
#else
    the_network->set_TerminateBackEndsOnShutdown(false);
#endif
    
    // Create the stream used by backends to pass data to the frontend.
    int tag = -1;
    MRN::PacketPtr packet;
    Assert(the_network->recv(&tag, packet, &upstream, true) == 1);
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
    delete the_network;
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
 * Get performance data debugging flag.
 *
 * Returns a flag indicating if performance data debugging is enabled.
 *
 * @return    Boolean "true" if debugging for performance data is enabled,
 *            "false" otherwise.
 */
bool Backend::isPerfDataDebugEnabled()
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
bool Backend::isStdioDebugEnabled()
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
bool Backend::isSymbolsDebugEnabled()
{
    return is_symbols_debug_enabled;
}
#endif
