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
#include "WatcherThreadTable.hxx"
#include "Protocol.h"
#include "Senders.hxx"
#include "ThreadTable.hxx"

#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <inttypes.h>
#include <vector>

using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Watcher;
using namespace OpenSpeedShop;
ThreadTable ThreadTable::TheTable;

    /** Identifier of the monitor thread. */
pthread_t fileIOmonitor_tid;

    /** Access-controlled flag used to request that the monitor thread exit. */
struct {
  bool flag;		       /**< Actual flag value. */
  pthread_mutex_t lock;	       /**< Mutual exclusion lock for this flag. */
} monitor_request_exit = { false, PTHREAD_MUTEX_INITIALIZER};

#ifndef NDEBUG
    /** Flag indicating if debugging for the backend is enabled. */
bool is_backend_debug_enabled = false;
#endif

  /**
    * Take a file name of the form: mutatee-11657-47381046878688.openss-data
    * and parse out the tid portion (47381046878688) in this case.
    */

pthread_t
getTidFromFilename (std::string filename)
{
  pthread_t retval = 0;

  int filenameSize = filename.length ();
  int suffixDx = filename.rfind (".openss-data", filenameSize);
  filename.erase (suffixDx, 12);

  filenameSize = filename.length ();
  suffixDx = filename.rfind ("-", filenameSize);
  std::string tidString = filename.substr (suffixDx + 1);

  sscanf (tidString.c_str (), "%lld", &retval);

  return (retval);

}

  /**
    * Take a file name of the form: mutatee-11657-47381046878688.openss-data
    * and parse out the tid portion (47381046878688) in this case.
    * and parse out the pid portion (11657) in this case.
    */
pid_t
getPidFromFilename (std::string filename)
{
  pid_t retval = 0;

  int filenameSize = filename.length ();
  int suffixDx = filename.rfind (".openss-data", filenameSize);
  filename.erase (suffixDx, 12);

  filenameSize = filename.length ();
  suffixDx = filename.rfind ("-", filenameSize);
  std::string tidString = filename.substr (suffixDx + 1);
  filename.erase (suffixDx, filenameSize - suffixDx);

  filenameSize = filename.length ();
  suffixDx = filename.rfind ("-", filenameSize);
  std::string pidString = filename.substr (suffixDx + 1);

  sscanf (pidString.c_str (), "%lld", &retval);

  return (retval);

}

void Watcher::Watcher ()		// : Lockable()
{
#ifndef NDEBUG
  if (Watcher::isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self () <<
	"] OpenSpeedShop::Watcher::Watcher()" << " Enter." << std::endl;
      std::cerr << output.str ();
    }
#endif
  WatcherThreadTable ();
}

  /**
    * Watcher thread routine to monitor the fileIO files for data.
    *
    * Monitor thread for fileIO to be sent back to the client tool.
    * Use a separate thread, so it is insured that the data will be
    * handled in a timely manner. The monitor simply sits in ???
    * until it is asynchronously interrupted.
    * IT handles the configuration of an interval timer that is used
    * for periodically flushing performance data
    * to the appropriate experiment databases.
    */
void *
OpenSpeedShop::Watcher::fileIOmonitorThread (void *)
{

  struct stat statbuf;
  struct timespec wait;
  char directoryName[1024];
  struct dirent * direntry;
  struct dirent * perfdata_direntry;
  XDR xdrs;
  std::string host = "";
  unsigned int blobsize = 0;
  uint64_t prevSize = 0;
  uint64_t prevPos = 0;
  WatcherThreadTable::FileInfoEntry currentFileEntryInfo;
  std::set<pid_t> PidSet; 
  pid_t pid_to_monitor=0; 
#if 1
  char data_dirname[PATH_MAX];
#endif

#ifndef NDEBUG
  if (Watcher::isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self () 
             << "] OpenSpeedShop::Watcher::fileIOmonitorThread()" 
             << " Enter." << std::endl;
      std::cerr << output.str ();
    }
#endif

  // Run the fileIO monitoring until instructed to exit
  for (bool do_exit = false; !do_exit;)
    {

      // Suspend ourselves for two seconds

#ifndef NDEBUG
      if (Watcher::isDebugEnabled ())
	{
	  std::stringstream output;
	  output << "[TID " << pthread_self () 
	         << "] OpenSpeedShop::Watcher::fileIOmonitorThread()" 
	         << " ----- Go to sleep using nanosleep." << std::endl;
	  std::cerr << output.str ();
	}
#endif
      wait.tv_sec = 2;
      wait.tv_nsec = 0;
      nanosleep (&wait, NULL);

#ifndef NDEBUG
      if (Watcher::isDebugEnabled ())
	{
	  std::stringstream output;
	  output << "[TID " << pthread_self () <<
	    "] OpenSpeedShop::Watcher::fileIOmonitorThread()" <<
	    " ----- Wake up from nanosleep." << std::endl;
	  std::cerr << output.str ();
	}
#endif

    PidSet.clear();
    PidSet = OpenSpeedShop::Framework::ThreadTable::TheTable.getActivePids();

    if (PidSet.size() > 0) {
     for (std::set< pid_t >::iterator i = PidSet.begin(); i != PidSet.end(); i++) {

#ifndef NDEBUG
        if (Watcher::isDebugEnabled ()) {
 	    std::stringstream output;
	    output << "[TID " << pthread_self () << "] OpenSpeedShop::Watcher::fileIOmonitorThread()" <<
	    " ----- PidSet, pid=" << *i << std::endl;
	    std::cerr << output.str ();
        }
#endif
            pid_to_monitor = *i;

      // Search given directory for files with the openss-data suffix
      // When we find a openss data file, keep track of several items per file.
      // We will be reading from this file when the size of the blob and the blob bytes
      // are written.  What we read will be sent to the client tool via the Senders:performanceData
      // callback routine.

      // What we will be keeping track of for each file is:
      // a) file pointer
      // b) filename corresponding to file pointer
      // c) position within the file, as to where to start looking for data
      // d) offset of next blob


      // ******* Identify the directory that we need to search for files
      // Once found, while through the directory looking for openss-data
      // type files.


#if 1
      if (getenv("OPENSS_RAWDATA_DIR") != NULL) {
          sprintf(data_dirname,"%s",getenv("OPENSS_RAWDATA_DIR"));
      } else {
          sprintf(data_dirname,"%s","/tmp");
      }

      DIR * perfdata_dirhandle = opendir (data_dirname);
#else
      DIR * perfdata_dirhandle = opendir ("/tmp");
#endif


      if (perfdata_dirhandle)
	{

	  while ((perfdata_direntry = readdir (perfdata_dirhandle)) != NULL)
	    {

	      if (strstr (perfdata_direntry->d_name, "openss-rawdata-"))
		{

		  sprintf (directoryName, "%s/%s", data_dirname,
			   perfdata_direntry->d_name);

		  DIR * dirhandle = opendir (directoryName);

#ifndef NDEBUG
		  if (Watcher::isDebugEnabled ())
		    {
		      std::stringstream output;
		      output << "[TID " << pthread_self () <<
			"] OpenSpeedShop::Watcher::fileIOmonitorThread()" <<
			" Examining directories, looking at directoryName=" <<
			directoryName << std::endl;
		      std::cerr << output.str ();
		    }
#endif
		  if (dirhandle)
		    {

		      while ((direntry = readdir (dirhandle)) != NULL)
			{

#ifndef NDEBUG
			  if (Watcher::isDebugEnabled ())
			    {
			      std::stringstream output;
			      output << "[TID " << pthread_self () <<
				"] OpenSpeedShop::Watcher::fileIOmonitorThread()"
				<<
				" Examining files, looking at direntry->d_name="
				<< direntry->d_name << std::endl;
			      std::cerr << output.str ();
			    }
#endif

			  if (strstr (direntry->d_name, ".openss-data"))
			    {

			      // ******************* Initialize working copy of the file being processed
			      // FileInfoEntry entry record.

			      currentFileEntryInfo.filePtr = NULL;
			      currentFileEntryInfo.fileName = "";
			      currentFileEntryInfo.readPosition = 0;
			      currentFileEntryInfo.prevSize = 0;
#ifndef NDEBUG
			      if (Watcher::isDebugEnabled ())
				{
				  std::stringstream output;
				  output << "[TID " << pthread_self () <<
				    "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
				    <<
				    " FOUND openss-data filename, direntry->d_name="
				    << direntry->d_name << std::endl;
				  std::cerr << output.str ();
				}
#endif


			      char dataFilename[PATH_MAX];
			      char openssDataFilename[PATH_MAX];
			      sprintf (dataFilename, "%s/%s", directoryName, direntry->d_name);

			      sprintf (openssDataFilename, "%s", direntry->d_name);

			      // *********** We have found a filename that matches our criteria
			      // Save the filename into our file information record
			      currentFileEntryInfo.fileName = openssDataFilename;

			      pid_t pid = getPidFromFilename (openssDataFilename);
			      pthread_t tid = getTidFromFilename (openssDataFilename);
#ifndef NDEBUG
			      if (Watcher::isDebugEnabled ()) {
				  std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() pid=" << pid << " tid=" << tid
				             << std::endl;
                              }
#endif
                              if (pid != pid_to_monitor) {
                                // skip this directory
                                break;
                              }
			      // ************** Make legality checks on pid and tid
			      // More here?

			      if (pid > 0 && tid > 0)
				{
				  if (WatcherThreadTable::TheTable.getThreadAlreadyPresent (pid, tid))
				    {

				      currentFileEntryInfo = WatcherThreadTable::TheTable.getEntry (pid, tid);
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ()) {
					  std:: cout << "Call to WatcherThreadTable::getThreadAlreadyPresent is true"
					             << std::endl;
                                      }
#endif
				    } else {

#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ())
					{
					  std::cout << "Call to WatcherThreadTable::getThreadAlreadyPresent is false" << std::endl;

					  std::cout << "Calling WatcherThreadTable::addThread"
                                                    << pid << tid << currentFileEntryInfo << std::endl;
					}
#endif
				      WatcherThreadTable::TheTable.addThread (pid, tid, currentFileEntryInfo);
				    }
				}

			      int status = stat (dataFilename, &statbuf);
			      if (status == (-1))
				{
				  std::cerr << "OpenSpeedShop::Watcher::fileIOmonitorThread() failed to stat file"
				    	    << dataFilename << std::endl;
				  break;
				}

			      // **************** Get the file's current size in bytes
			      //
			      uint64_t currentFileSize = statbuf.st_size;

			      FILE * f = fopen (dataFilename, "r");
			      if (f == NULL)
				{
				  std::cerr << "OpenSpeedShop::Watcher::fileIOmonitorThread() failed to open file"
				            << dataFilename << std::endl;
				  break;
				}

			      // ************ We have the file pointer to the openss-data file
			      // Save the pointer to the file information record
			      currentFileEntryInfo.filePtr = f;
			      WatcherThreadTable::TheTable.setEntry (pid, tid, currentFileEntryInfo);

				  uint64_t currentPos = ftell (f);

				  currentFileEntryInfo = WatcherThreadTable::TheTable.getEntry (pid, tid);
				  prevSize = currentFileEntryInfo.prevSize;

#ifndef NDEBUG
			          if (Watcher::isDebugEnabled ()) {
				    std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() prevSize=" 
                                               << prevSize << ", currentPos=" << currentPos << ", currentFileSize=" << currentFileSize
				               << std::endl;
				  }
#endif
                                  // If this is the case break out of the main loop because there isn't any new data
				  if (prevSize == currentFileSize)
				    {
				      // Skip processing this file, it is the same size it was before
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ())
					{
					  std::stringstream output;
					  output << "[TID " << pthread_self ()
					    << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					    << " file is same size as the last time file was saved="
					    << currentFileSize << std::endl;
					  std::cerr << output.str ();
				          std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() BREAK; prevSize=" 
                                                     << prevSize << " currentFileSize=" << currentFileSize
				                     << std::endl;
					} // end debug
#endif
				      break;
				    }

			      bool continue_checking_for_data = true;
			      while (continue_checking_for_data)
				{

				  // ********** Have we seen this file before and read from it?
				  // Seek to the position we need to be at in order to read the next blob size and blob
				  prevPos = currentFileEntryInfo.readPosition;
#ifndef NDEBUG
				  if (Watcher::isDebugEnabled ()) {
				     std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() prevPos=" 
                                                << prevPos << std::endl;
			       	  } // end debug
#endif
				  if (prevPos != 0)
				    {
				      uint64_t fseekPos = fseek (f, prevPos, SEEK_SET);
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ())
				        {
				          std::stringstream output;
				          output << "[TID " << pthread_self () 
					    << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					    << " current position in file(currentPos)=" << currentPos 
					    << " fseek position in file(fseekPos)=" << fseekPos 
					    << " previous position in file(prevPos)=" << prevPos << std::endl;
				          std::cerr << output.str ();
				        } //end debug
#endif
				    } // end: if prevPos != 0

				  xdrstdio_create (&xdrs, f, XDR_DECODE);

				  if (!xdr_u_int (&xdrs, &blobsize))
				    {
				      continue_checking_for_data = false;
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ()) {
				        std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() BREAK; !xdr_u_int (&xdrs, &blobsize)" 
                                                   << std::endl;
				      } //end debug
#endif
				      break;
				    }
				  else
				    {
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ())
					{
					  std::stringstream output;
					  output << "[TID " << pthread_self ()
					    <<
					    "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					    << " successfully read blobsize="
					    << blobsize << std::endl;
					  std::cerr << output.str ();
					} //end debug
#endif
				    }

				  char * blobbuff;
				  blobbuff = (char *) malloc (blobsize + 4);
				  if (blobbuff == 0) {
				    abort ();
                                  }
				  memset (blobbuff, 0, blobsize);

				  int bytesRead = fread (blobbuff, 1, blobsize, f);

				  if (bytesRead == 0 || bytesRead < blobsize)
				    {
				      continue_checking_for_data = false;
#ifndef NDEBUG
				      if (Watcher::isDebugEnabled ()) {
				        std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() BREAK; bytesRead=" 
                                                   << bytesRead << " blobsize=" << blobsize
				                   << std::endl;
				      } //end debug
#endif
				      break;
				    }


				  uint64_t lastPos = ftell (f);
#ifndef NDEBUG
				  if (Watcher::isDebugEnabled ())
				    {
				      std::stringstream output;
				      output << "[TID " << pthread_self ()
                                             << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					     << " BYTESREAD, status for read of blob=" << bytesRead 
					     << " position inside file after reading the blob is(lastPos)="
					     << lastPos << std::endl;
				      std::cerr << output.str ();
				    }
#endif
				  // ****************  We have the position inside this file after reading of the blob.
				  // Save this position of fseek usage when reading this file again
				  currentFileEntryInfo.readPosition = lastPos;
				  currentFileEntryInfo.prevSize = currentFileSize;
				  WatcherThreadTable::TheTable.setEntry (pid, tid, currentFileEntryInfo);


#ifndef NDEBUG
				  if (Watcher::isDebugEnabled ())
				    {
				      std::stringstream output;
				      output << "[TID " << pthread_self () 
                                             << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					     << " BEFORE SENDING BLOB, Sending blob to performanceData, size="
					     << blobsize << std::endl;
				      std::cerr << output.str ();
				    } // end debug
#endif

				  // **************** Send the performance data blob upstream to be sent back to 
				  // the client tool.

				  OpenSpeedShop::Framework::Senders::performanceData (Blob (blobsize, blobbuff));
#ifndef NDEBUG
				  if (Watcher::isDebugEnabled ())
				    {
				      std::stringstream output;
				      output << "[TID " << pthread_self ()
					     << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
					     << " AFTER SENDING BLOB, Sending blob to performanceData, size="
					     << blobsize << std::endl;
				      std::cerr << output.str ();
				    } // end debug
#endif

				}	// end while data in this file

#ifndef NDEBUG
		             if (Watcher::isDebugEnabled ()) {
			        std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() " 
                                           << " Falling out,read blob loop for file, direntry->d_name=" << direntry->d_name 
			                   << std::endl;
			     } // end debug
#endif
			  } else {
#ifndef NDEBUG
		             if (Watcher::isDebugEnabled ()) {
			        std:: cout << "OpenSpeedShop::Watcher::fileIOmonitorThread() " 
                                           << " Did not find openss-data filename, direntry->d_name=" << direntry->d_name 
			                   << std::endl;
			     } // end debug
#endif
		          } // end else
			}
		      closedir (dirhandle);
		    }		// end if dirhandle

		}		// end if openss-rawdata- match
	    }			// while perfdata_dirhandle
          closedir (perfdata_dirhandle);
	}			// end if perfdata_dirhandle
     } // PidSet loop
    } // PidSet > 0 if

#ifndef NDEBUG
      if (Watcher::isDebugEnabled ())
	{
	  std::stringstream output;
	  output << "[TID " << pthread_self ()
	         << "] OpenSpeedShop::Watcher::fileIOmonitorThread()"
	         << " Exit from monitor thread." << std::endl;
	  std::cerr << output.str ();
	}
#endif

      // Exit monitor thread if instructed to do so
      Assert (pthread_mutex_lock (&monitor_request_exit.lock) == 0);

      //
      // Clean up the file tables and pointers into the files
      // before we exit.

      do_exit = monitor_request_exit.flag;
      Assert (pthread_mutex_unlock (&monitor_request_exit.lock) == 0);

    }

  // Empty, unused, return value from this thread
  return NULL;
}


void
OpenSpeedShop::Watcher::startWatching ()
{


  OpenSpeedShop::Watcher::Watcher ();

#ifndef NDEBUG
  if (Watcher::isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self () 
	     << "] OpenSpeedShop::Watcher::startWatching()" 
             << " Entered " << std::endl;
      std::cerr << output.str ();
    }
#endif

  // Create the monitor/watcher thread
  Assert (pthread_create (&OpenSpeedShop::Watcher::fileIOmonitor_tid,
			  NULL,
			  OpenSpeedShop::Watcher::fileIOmonitorThread,
			  NULL) == 0);

}

void
OpenSpeedShop::Watcher::stopWatching ()
{

#ifndef NDEBUG
  if (Watcher::isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self ()
	     << "] OpenSpeedShop::Watcher::stopWatching()" 
             << " Entered " << std::endl;
      std::cerr << output.str ();
    }
#endif

  // Instruct the monitor thread to exit
  Assert (pthread_mutex_lock (&monitor_request_exit.lock) == 0);
  monitor_request_exit.flag = true;
  Assert (pthread_mutex_unlock (&monitor_request_exit.lock) == 0);

  // Wait for the monitor thread to actually exit
  Assert (pthread_join (fileIOmonitor_tid, NULL) == 0);

  // Cleanup the exit request flag
  Assert (pthread_mutex_lock (&monitor_request_exit.lock) == 0);
  monitor_request_exit.flag = false;
  Assert (pthread_mutex_unlock (&monitor_request_exit.lock) == 0);

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
bool
Watcher::isDebugEnabled ()
{
  bool is_backend_debug_enabled =
    (getenv ("OPENSS_DEBUG_MRNET_WATCHER") != NULL);
  return is_backend_debug_enabled;

}
#endif
