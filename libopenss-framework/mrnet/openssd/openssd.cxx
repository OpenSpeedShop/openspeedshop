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

#include "Assert.hxx"
#include "Backend.hxx"
#include "Callbacks.hxx"
#include "Path.hxx"
#include "Protocol.h"
#include "Time.hxx"
#include "Watcher.hxx"

#include <BPatch.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Access-controlled flag indicating when the daemon should exit. */
    struct {
	bool flag;             /**< Actual flag value. */
	pthread_mutex_t lock;  /**< Mutal exclusion lock for this flag. */
	pthread_cond_t cv;     /**< Condition variable for this flag. */
    } daemon_request_exit = {
	false, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
    };



    /**
     * Shutdown backends.
     *
     * Callback function called by the backend message pump when a request to
     * shutdown the backends is received. Signals that the daemon should exit.
     *
     * @param blob    Blob containing the message.
     */
    void shutdownBackends(const Blob& blob)
    {
	Assert(pthread_mutex_lock(&daemon_request_exit.lock) == 0);
	daemon_request_exit.flag = true;
	Assert(pthread_cond_signal(&daemon_request_exit.cv) == 0);
	Assert(pthread_mutex_unlock(&daemon_request_exit.lock) == 0);
    }
    


}



/**
 * Main entry point.
 *
 * Main entry point for the daemon. Starts the MRNet backend message pump
 * after registering callbacks, waits until the daemon is instructed to exit,
 * then stops the MRNet backend message pump.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{
    // Redirect the stdout and stderr streams to a log file
    std::ostringstream pid;
    pid << getpid();
    Path log_filename(std::string(Path("/tmp") + Path(argv[0]).getBaseName()) +
		      std::string(".") + pid.str());
    FILE* log_file = fopen(log_filename.c_str(), "w");
    if(log_file != NULL) {
	dup2(fileno(log_file), STDOUT_FILENO);
	dup2(fileno(log_file), STDERR_FILENO);
    }

    // Display a startup message
    std::cout << "Started Open|SS MRNet Daemon: " << Time::Now() << std::endl;
    std::cout << std::endl;
    for(int i = 0; environ[i] != NULL; ++i)
	std::cout << environ[i] << std::endl;
    std::cout << std::endl;
    for(int i = 0; i < argc; ++i)
	std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    std::cout << std::endl;
    std::cout.flush();

    // Register callbacks with the backend
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_SHUTDOWN_BACKENDS,
			      shutdownBackends);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_ATTACH_TO_THREADS,
			      Callbacks::attachToThreads);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CHANGE_THREADS_STATE,
			      Callbacks::changeThreadsState);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CREATE_PROCESS,
			      Callbacks::createProcess);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_DETACH_FROM_THREADS,
			      Callbacks::detachFromThreads);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_NOW,
			      Callbacks::executeNow);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT,
			      Callbacks::executeAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF,
			      Callbacks::executeInPlaceOf);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
			      Callbacks::getGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
			      Callbacks::getGlobalString);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
			      Callbacks::getMPICHProcTable);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
			      Callbacks::setGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
			      Callbacks::stopAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
			      Callbacks::uninstrument);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_MPI_STARTUP,
			      Callbacks::MPIStartup);
    
    // Start the backend's message pump
    Backend::startMessagePump(argc, argv);

    // Start watching for incoming performance data
    OpenSpeedShop::Watcher::startWatching();

    // Wait until the daemon is instructed to exit
    Assert(pthread_mutex_lock(&daemon_request_exit.lock) == 0);
    while(daemon_request_exit.flag == false)
	Assert(pthread_cond_wait(&daemon_request_exit.cv,
				 &daemon_request_exit.lock) == 0);
    Assert(pthread_mutex_unlock(&daemon_request_exit.lock) == 0);

    // Stop watching for incoming performance data
    OpenSpeedShop::Watcher::stopWatching();

    // Let the frontend know that this daemon is ready to shutdown
    Backend::sendToFrontend(OPENSS_PROTOCOL_TAG_READY_TO_SHUTDOWN, Blob());
    
    // Stop the backend's message pump
    Backend::stopMessagePump();

    // Unregister callbacks with the backend
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_SHUTDOWN_BACKENDS,
				shutdownBackends);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_ATTACH_TO_THREADS,
				Callbacks::attachToThreads);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_CHANGE_THREADS_STATE,
				Callbacks::changeThreadsState);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_CREATE_PROCESS,
				Callbacks::createProcess);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_DETACH_FROM_THREADS,
				Callbacks::detachFromThreads);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_EXECUTE_NOW,
				Callbacks::executeNow);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT,
				Callbacks::executeAtEntryOrExit);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF,
				Callbacks::executeInPlaceOf);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
				Callbacks::getGlobalInteger);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
				Callbacks::getGlobalString);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
				Callbacks::getMPICHProcTable);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
				Callbacks::setGlobalInteger);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
				Callbacks::stopAtEntryOrExit);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
				Callbacks::uninstrument);
    Backend::unregisterCallback(OPENSS_PROTOCOL_TAG_MPI_STARTUP,
			        Callbacks::MPIStartup);
    
    // Display a shutdown message
    std::cout << std::endl;
    std::cout << "Stopped Open|SS MRNet Daemon: " << Time::Now() << std::endl;
    std::cout.flush();

    // Close the log file containing the redirected stdout and stderr streams
    if(log_file != NULL)
	fclose(log_file);
    
    // Indicate success to the shell
    return 0;
}
