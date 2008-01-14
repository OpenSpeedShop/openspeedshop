////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the Watcher namespace.
 *
 */

#include "Assert.hxx"
#include "Blob.hxx"
#include "Backend.hxx"
#include "Watcher.hxx"
//#include "../Senders.hxx"
#include "Watcher_FileIO.hxx"

#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <sstream>


using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Watcher;
using namespace OpenSpeedShop;


/**
  * Check the fileIO files for data to package for sending to the client
  *
  * Called when our real-time interval timer cause an
  * alarm signal to be sent. Checks for performance data written
  * into the fileIO intermediate files to see if it can be taken out of
  * the files and packaged for the client.
  */
 int checkForData(int)
 {
   // Look into the fileIO intermediate openspeedshop files


   // Check in the named /tmp directory for files

   // ??
 }

    /** Identifier of the monitor thread. */
    pthread_t fileIOmonitor_tid;

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
    * Monitor fileIO function.
    *
    * Monitor thread for fileIO to be sent back to the client tool.
    * Use a separate thread, so it is insured that the data will be
    * handled in a timely manner. The monitor simply sits in ???
    * until it is asynchronously interrupted.
    * IT handles the configuration of an interval timer that is used
    * for periodically flushing performance data
    * to the appropriate experiment databases.
    */
    void* OpenSpeedShop::Watcher::fileIOmonitorThread(void*)
    {
#ifndef NDEBUG
            if(Watcher::isDebugEnabled()) {
               std::stringstream output;
               output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
                      << " Enter." <<  std::endl;
               std::cerr << output.str();
            }
#endif
        // Run the fileIO monitoring until instructed to exit
        for(bool do_exit = false; !do_exit;) {

            // Suspend ourselves for one second
            struct timespec wait;
            wait.tv_sec = 1;
            wait.tv_nsec = 0;
            nanosleep(&wait, NULL);

#ifndef NDEBUG
            if(Watcher::isDebugEnabled()) {
               std::stringstream output;
               output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
                      << " Wake up from nanosleep." <<  std::endl;
               std::cerr << output.str();
            }
#endif
            std::string collectorStr = "pcsamp";
            std::string dirPath = "";

            // Pass the collector and pid to create the directory path to the location
            // of the files to monitor.  This is a dummy pid example.
            dirPath = Watcher_OpenSS_GetFilePrefix(collectorStr.c_str(), 5550);

#ifndef NDEBUG
            if(Watcher::isDebugEnabled()) {
               std::stringstream output;
               output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
                      << " collectorStr=" <<  collectorStr << " dirPath=" << dirPath << std::endl;
               std::cerr << output.str();
            }
#endif
            

//            Check if this is the first time through?
//               Are there any file lists and pointers into the files
//                 If not need to create the initial file lists and create pointers
//                 once the data is read up.
//            Find the threads/processes we are dealing with and 
//            search for their corresponding directories and files in /tmp


//            Then read their performance data out of the files and call 
//            performanceData with the blobs of data
//               const Blob dummyBlob = Blob();
//               OpenSpeedShop::Framework::performanceData(dummyBlob);

//            Update the list of files and the pointers into the file indicating
//            where to look for the next blobs of data.


#ifndef NDEBUG
            if(Watcher::isDebugEnabled()) {
               std::stringstream output;
               output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
                      << " Exit from monitor thread." <<  std::endl;
               std::cerr << output.str();
            }
#endif

            // Exit monitor thread if instructed to do so
            Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);

//
//          Clean up the file tables and pointers into the files
//          before we exit.

            do_exit = monitor_request_exit.flag;
            Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);

        }

        // Empty, unused, return value from this thread
        return NULL;
    }


//needthis void OpenSpeedShop::Watcher::startWatching(const OpenSpeedShop::Watcher::BlobCallback callback) {
void OpenSpeedShop::Watcher::startWatching() {

#ifndef NDEBUG
    if(Watcher::isDebugEnabled()) {
       std::stringstream output;
       output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::startWatching()"
              << " Entered " <<  std::endl;
       std::cerr << output.str();
    }
#endif

 // Create the monitor/watcher thread
 Assert(pthread_create(&OpenSpeedShop::Watcher::fileIOmonitor_tid, 
                       NULL, 
                       OpenSpeedShop::Watcher::fileIOmonitorThread, NULL) == 0);

}

void OpenSpeedShop::Watcher::stopWatching() {

#ifndef NDEBUG
    if(Watcher::isDebugEnabled()) {
       std::stringstream output;
       output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::stopWatching()"
              << " Entered " <<  std::endl;
       std::cerr << output.str();
    }
#endif

    // Instruct the monitor thread to exit
    Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);
    monitor_request_exit.flag = true;
    Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);

    // Wait for the monitor thread to actually exit
    Assert(pthread_join(fileIOmonitor_tid, NULL) == 0);

    // Cleanup the exit request flag
    Assert(pthread_mutex_lock(&monitor_request_exit.lock) == 0);
    monitor_request_exit.flag = false;
    Assert(pthread_mutex_unlock(&monitor_request_exit.lock) == 0);

}

void OpenSpeedShop::Watcher::addThread(const pid_t& pid, const pthread_t& tid)
{
#ifndef NDEBUG
    if(Watcher::isDebugEnabled()) {
       std::stringstream output;
       output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::addThread()"
              << " Entered " <<  std::endl;
       std::cerr << output.str();
    }
#endif

}

void OpenSpeedShop::Watcher::removeThread(const pid_t& pid, const pthread_t& tid)
{

#ifndef NDEBUG
    if(Watcher::isDebugEnabled()) {
       std::stringstream output;
       output << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::removeThread()"
              << " Entered " <<  std::endl;
       std::cerr << output.str();
    }
#endif

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
bool Watcher::isDebugEnabled()
{
    bool is_backend_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_WATCHER") != NULL) ;
    return is_backend_debug_enabled;

}
#endif



