////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2009 Krell Institute. All Rights Reserved.
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

    /** Identifier of the monitor thread. */
pthread_t fileIOmonitor_tid;

    /** Access-controlled flag used to request that the monitor thread exit. */
struct {
  bool flag;		       /**< Actual flag value. */
  pthread_mutex_t lock;	       /**< Mutual exclusion lock for this flag. */
} monitor_request_exit = { false, PTHREAD_MUTEX_INITIALIZER};

    /** Access-controlled flag used for controling access to the rawdata scan data structures. */
static pthread_mutex_t RawDataScan_Lock = PTHREAD_MUTEX_INITIALIZER;

#ifndef NDEBUG
    /** Flag indicating if debugging for the backend is enabled. */
bool is_backend_debug_enabled = false;
#endif


  /**
    * Function: acquireScanLock
    * This function acquires a lock on the rawdata file scan data structures
    */
void
OpenSpeedShop::Watcher::acquireScanLock ()
{
  if (isDebugEnabled ()) {
    std:: cout << "[TID " << pthread_self () << " OpenSpeedShop::Watcher::acquireScanLock() ENTERED"  << std::endl;
  }

  Assert(pthread_mutex_lock(&RawDataScan_Lock) == 0);

  if (isDebugEnabled ()) {
    std:: cout << "[TID " << pthread_self () << " OpenSpeedShop::Watcher::acquireScanLock() EXITED"  << std::endl;
  }

}

  /**
    * Function: releaseScanLock
    * This function releases the lock on the rawdata file scan data structures
    */
void
OpenSpeedShop::Watcher::releaseScanLock ()
{
  if (isDebugEnabled ()) {
    std:: cout << "[TID " << pthread_self () << " OpenSpeedShop::Watcher::releaseScanLock() ENTERED"  << std::endl;
  }

  Assert(pthread_mutex_unlock(&RawDataScan_Lock) == 0);

  if (isDebugEnabled ()) {
    std:: cout << "[TID " << pthread_self () << " OpenSpeedShop::Watcher::releaseScanLock() EXITED"  << std::endl;
  }
}

  /**
    * Function: getTidFromFilename
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
    * Function: getPidFromFilename
    * Take a file name of the form: mutatee-11657-47381046878688.openss-data
    * and parse out the tid portion (47381046878688) in this case.
    * and parse out the pid portion (11657) in this case.
    */
pid_t
getPidFromFilename (std::string filename)
{
  pid_t retval = 0;

  int filenameSize = filename.length ();

  if (isDebugEnabled ()) {
    std:: cout << "[TID " << pthread_self () << " OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " filenameSize=" << filenameSize 
               << std::endl;
  }

  int suffixDx = filename.rfind (".openss-data", filenameSize);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " suffixDx=" << suffixDx 
               << std::endl;
  }

  filename.erase (suffixDx, 12);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " suffixDx=" << suffixDx 
               << std::endl;
  }

  filenameSize = filename.length ();

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " filenameSize=" << filenameSize 
               << std::endl;
  }

  suffixDx = filename.rfind ("-", filenameSize);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " suffixDx=" << suffixDx 
               << std::endl;
  }

  std::string tidString = filename.substr (suffixDx + 1);
  filename.erase (suffixDx, filenameSize - suffixDx);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " tidString=" << tidString 
               << " suffixDx=" << suffixDx 
               << std::endl;
  }

  filenameSize = filename.length ();

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " filenameSize=" << filenameSize 
               << " suffixDx=" << suffixDx 
               << std::endl;
  }

  suffixDx = filename.rfind ("-", filenameSize);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " filenameSize=" << filenameSize 
               << " after rfind - suffixDx=" << suffixDx 
               << std::endl;
  }

  std::string pidString = filename.substr (suffixDx + 1);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " pidString=" << pidString 
               << " after substr, suffixDx=" << suffixDx 
               << std::endl;
  }

  sscanf (pidString.c_str (), "%d", &retval);

  if (isDebugEnabled ()) {
    std:: cout << "OpenSpeedShop::Watcher::getPidFromFilename() filename=" << filename 
               << " pidString=" << pidString 
               << " after sscanf, retval=" << retval 
               << std::endl;
  }

  return (retval);

}

void Watcher::Watcher ()		// : Lockable()
{
#ifndef NDEBUG
  if (isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self () <<
	"] OpenSpeedShop::Watcher::Watcher()" << " Enter." << std::endl;
      std::cerr << output.str ();
    }
#endif
  WatcherThreadTable ();
}


void Watcher::scanForRawPerformanceData(pid_t pid_to_monitor, std::string host_to_monitor,
					pthread_t tid_to_monitor)
{
  struct stat statbuf;
  char directoryName[1024];
  struct dirent * direntry;
  struct dirent * perfdata_direntry;
  char data_dirname[PATH_MAX];
  char openssDataDirName[1024];
  XDR xdrs;
  std::string host = "";
  unsigned int blobsize = 0;
  uint64_t prevSize = 0;
  uint64_t prevPos = 0;

  WatcherThreadTable::FileInfoEntry currentFileEntryInfo;

  if (getenv("OPENSS_RAWDATA_DIR") != NULL) {
     sprintf(data_dirname,"%s",getenv("OPENSS_RAWDATA_DIR"));
  } else {
     sprintf(data_dirname,"%s","/tmp");
  }

  DIR * perfdata_dirhandle = opendir (data_dirname);

  // Create the name of the raw data directory with pid included
  sprintf (openssDataDirName, "openss-rawdata-%s-%d", host_to_monitor.c_str(), pid_to_monitor);

#ifndef NDEBUG
  if (isDebugEnabled ()) {
      std::stringstream output;
      output << "[TID " << pthread_self () << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()" <<
		" perfdata_dirhandle=" << perfdata_dirhandle << 
		" data_dirname=" << data_dirname << 
                " openssDataDirName=" << openssDataDirName << std::endl;
      std::cerr << output.str ();
  }
#endif

  // If the main (upper level) directory exists continue the search for rawdata associated with pid == pid_to_monitor
  
  if (perfdata_dirhandle) {

	  while ((perfdata_direntry = readdir (perfdata_dirhandle)) != NULL) {

              // Look at the directories in the upper level directory until we find the one associated with the pid_to_monitor
              // Then start processing the data
	      if (strstr (perfdata_direntry->d_name, openssDataDirName)) {

		  sprintf (directoryName, "%s/%s", data_dirname, perfdata_direntry->d_name);

		  DIR * rawdata_dirhandle = opendir (directoryName);

#ifndef NDEBUG
		  if (isDebugEnabled ()) {
		      std::stringstream output;
		      output << "[TID " << pthread_self () << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()" <<
			" Examining directories, looking at directoryName=" << directoryName << 
			"  openssDataDirName=" << openssDataDirName <<
			"  perfdata_direntry->d_name=" << perfdata_direntry->d_name <<
			"  rawdata_dirhandle=" << rawdata_dirhandle << std::endl;
		      std::cerr << output.str ();
                  }
#endif
		  if (rawdata_dirhandle) {

		      while ((direntry = readdir (rawdata_dirhandle)) != NULL) {

#ifndef NDEBUG
			  if (isDebugEnabled ()) {
			      std::stringstream output;
			      output << "[TID " << pthread_self () <<
				"] OpenSpeedShop::Watcher::scanForRawPerformanceData()" <<
				" Examining files, looking at direntry->d_name=" << direntry->d_name << std::endl;
			      std::cerr << output.str ();
			    }
#endif

			  if (strstr (direntry->d_name, ".openss-data")) {

			      // ******************* Initialize working copy of the file being processed
			      // FileInfoEntry entry record.

			      currentFileEntryInfo.filePtr = NULL;
			      currentFileEntryInfo.fileName = "";
			      currentFileEntryInfo.readPosition = 0;
			      currentFileEntryInfo.prevSize = 0;
#ifndef NDEBUG
			      if (isDebugEnabled ()) {
				  std::stringstream output;
				  output << "[TID " << pthread_self () <<
				    "] OpenSpeedShop::Watcher::scanForRawPerformanceData()" <<
				    " FOUND openss-data filename, direntry->d_name=" << direntry->d_name << std::endl;
				  std::cerr << output.str ();
				}
#endif

  		              char dataFilename[PATH_MAX];
			      char openssDataFilename[PATH_MAX];
			      sprintf (dataFilename, "%s/%s", directoryName, direntry->d_name);
			      sprintf (openssDataFilename, "%s", direntry->d_name);

			      if (isDebugEnabled ()) {
  			         std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() openssDataFilename=" << openssDataFilename 
                                            << " directoryName=" << directoryName 
                                            << " dataFilename=" << dataFilename
				            << std::endl;
                              }

			      // *********** We have found a filename that matches our criteria
			      // Save the filename into our file information record
			      currentFileEntryInfo.fileName = openssDataFilename;

			      pid_t pid = getPidFromFilename (openssDataFilename);
#ifndef NDEBUG
			      if (isDebugEnabled ()) {
				  std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() pid=" << pid << std::endl;
                              }
#endif

			      pthread_t tid = getTidFromFilename (openssDataFilename);

#ifndef NDEBUG
			      if (isDebugEnabled ()) {
				  std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() pid=" << pid << " tid=" << tid << std::endl;
				  std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() pid_to_monitor=" << pid_to_monitor << " tid_to_monitor=" << tid_to_monitor << std::endl;
                              }
#endif
                              if (pid != pid_to_monitor) {
                                // skip this directory
			        if (isDebugEnabled ()) {
				  std:: cout << "BREAK BECAUSE PID and PID_TO_MONITOR DO NOT MATCH" << std::endl;
                                }
                                break;
                              }

                              // Add this special case for the case where the application is not-pthreaded but when the data file is created
                              // the master thread id is used to create the name.  Here we detect this case as dyninst will report 0 tid value
                              // (tid_to_monitor here), the tid value here is the tid value derived from the openss-data file name.
                              // We check to see if this is the case and then set tid to zero so we process this file with the tid value from 
                              // the named file even if the tid to monitor is 0.   Basically, we ignore the fact the tids don't match, but only
                              // when tid_to_monitor is a zero, which means the application was not threaded, so this should be relatively safe.
                              if (tid != 0 && tid_to_monitor == 0) {
                                 tid = 0;
                              } else if (tid != tid_to_monitor) {
				  if (isDebugEnabled ()) {
				     std:: cout << "CONTINUE BECAUSE TID and TID_TO_MONITOR DO NOT MATCH" << std::endl;
                                  }
				  continue;
			      }

			      // ************** Make legality checks on pid and tid
			      // More here?

                              // See comments above about master thread etc...
			      if (pid > 0 && (tid > 0 || tid == 0 && tid_to_monitor == 0)) {
				  if (WatcherThreadTable::TheTable.getThreadAlreadyPresent (pid, tid)) {

				      currentFileEntryInfo = WatcherThreadTable::TheTable.getEntry (pid, tid);
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
					  std:: cout << "Call to WatcherThreadTable::getThreadAlreadyPresent is true"
					             << std::endl;
                                      }
#endif
				    } else {

#ifndef NDEBUG
				      if (isDebugEnabled ()) {
					  std::cout << "Call to WatcherThreadTable::getThreadAlreadyPresent is false" << std::endl;

					  std::cout << "Calling WatcherThreadTable::addThread"
                                                    << pid << tid << currentFileEntryInfo << std::endl;
					}
#endif
				      WatcherThreadTable::TheTable.addThread (pid, tid, currentFileEntryInfo);
				    }
				}

			      int status = stat (dataFilename, &statbuf);
			      if (isDebugEnabled ()) {
			        std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() dataFilename stat value=status=" 
                                               << status << ", dataFilename=" << dataFilename << std::endl;
				}
			      if (status == (-1)) {
				  std::cerr << "OpenSpeedShop::Watcher::scanForRawPerformanceData() failed to stat file: "
				    	    << dataFilename << std::endl;
				  break;
                              }

			      // **************** Get the file's current size in bytes
			      //
			      uint64_t currentFileSize = statbuf.st_size;

			      FILE * f = fopen (dataFilename, "r");
			      if (f == NULL) {
				  std::cerr << "OpenSpeedShop::Watcher::scanForRawPerformanceData() failed to FOPEN file: "
				            << "(" << dataFilename << ")" << std::endl;
				  break;
				} else {
			          if (isDebugEnabled ()) {
			            std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() successfully FOPEN dataFilename=" 
                                               << dataFilename << std::endl;
			    	    }
                                }

			      // ************ We have the file pointer to the openss-data file
			      // Save the pointer to the file information record
			      currentFileEntryInfo.filePtr = f;
			      WatcherThreadTable::TheTable.setEntry (pid, tid, currentFileEntryInfo);

				  uint64_t currentPos = ftell (f);

				  currentFileEntryInfo = WatcherThreadTable::TheTable.getEntry (pid, tid);
				  prevSize = currentFileEntryInfo.prevSize;

#ifndef NDEBUG
			          if (isDebugEnabled ()) {
				    std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() prevSize=" 
                                               << prevSize << ", currentPos=" << currentPos << ", currentFileSize=" << currentFileSize
				               << std::endl;
				  }
#endif
                                  // If this is the case break out of the main loop because there isn't any new data
				  if (prevSize == currentFileSize) {
				      // Skip processing this file, it is the same size it was before
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
					  std::stringstream output;
					  output << "[TID " << pthread_self ()
					    << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
					    << " file is same size as the last time file was saved="
					    << currentFileSize << std::endl;
					  std::cerr << output.str ();
				          std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() BREAK; prevSize=" 
                                                     << prevSize << " currentFileSize=" << currentFileSize
					             << "  FCLOSING FILE dataFilename" << dataFilename 
				                     << std::endl;
					} // end debug
#endif
                                      fclose(f);
				      break;
				    }

			      bool continue_checking_for_data = true;
			      while (continue_checking_for_data) {

				  // ********** Have we seen this file before and read from it?
				  // Seek to the position we need to be at in order to read the next blob size and blob
				  prevPos = currentFileEntryInfo.readPosition;
#ifndef NDEBUG
				  if (isDebugEnabled ()) {
				     std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() prevPos=" 
                                                << prevPos << std::endl;
			       	  } // end debug
#endif
				  if (prevPos != 0) {
				      uint64_t fseekPos = fseek (f, prevPos, SEEK_SET);
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
				          std::stringstream output;
				          output << "[TID " << pthread_self () 
					    << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
					    << " current position in file(currentPos)=" << currentPos 
					    << " fseek position in file(fseekPos)=" << fseekPos 
					    << " previous position in file(prevPos)=" << prevPos << std::endl;
				          std::cerr << output.str ();
				        } //end debug
#endif
				    } // end: if prevPos != 0

				  xdrstdio_create (&xdrs, f, XDR_DECODE);

				  if (!xdr_u_int (&xdrs, &blobsize)) {
				      continue_checking_for_data = false;
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
				        std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() BREAK; !xdr_u_int (&xdrs, &blobsize)" 
					           << "  FCLOSING FILE dataFilename" << dataFilename 
                                                   << std::endl;
				      } //end debug
#endif
                                      fclose(f);
				      break;
				    } else {
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
					  std::stringstream output;
					  output << "[TID " << pthread_self () <<
					    "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
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

				  if (bytesRead == 0 || bytesRead < blobsize) {
				      continue_checking_for_data = false;
#ifndef NDEBUG
				      if (isDebugEnabled ()) {
				        std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() BREAK ; bytesRead=" 
                                                   << bytesRead << " blobsize=" << blobsize
					           << "  FCLOSING FILE dataFilename" << dataFilename 
				                   << std::endl;
				      } //end debug
#endif
                                      fclose(f);
				      break;
				    }


				  uint64_t lastPos = ftell (f);
#ifndef NDEBUG
				  if (isDebugEnabled ()) {
				      std::stringstream output;
				      output << "[TID " << pthread_self ()
                                             << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
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
				  if (isDebugEnabled ()) {
				      std::stringstream output;
				      output << "[TID " << pthread_self () 
                                             << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
					     << " BEFORE SENDING BLOB, Sending blob to performanceData, size="
					     << blobsize << std::endl;
				      std::cerr << output.str ();
				    } // end debug
#endif

				  // **************** Send the performance data blob upstream to be sent back to 
				  // the client tool.

				  OpenSpeedShop::Framework::Senders::performanceData (Blob (blobsize, blobbuff));
#ifndef NDEBUG
				  if (isDebugEnabled ()) {
				      std::stringstream output;
				      output << "[TID " << pthread_self ()
					     << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
					     << " AFTER SENDING BLOB, Sending blob to performanceData, size="
					     << blobsize << std::endl;
				      std::cerr << output.str ();
				    } // end debug
#endif

				}	// end while data in this file

                             // close file for now, we will reopen it later
#ifndef NDEBUG
			  if (isDebugEnabled ()) {
				      std::stringstream output;
				      output << "[TID " << pthread_self ()
					     << "] OpenSpeedShop::Watcher::scanForRawPerformanceData()"
					     << " WOULD HAVE BEEN (COMMENTED OUT) FCLOSING FILE dataFilename" << dataFilename << std::endl;
				      std::cerr << output.str ();
				    } // end debug
#endif
                             // fclose(f);

#ifndef NDEBUG
		             if (isDebugEnabled ()) {
			        std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() " 
                                           << " Falling out,read blob loop for file, direntry->d_name=" << direntry->d_name 
			                   << std::endl;
			     } // end debug
#endif
			  } else {
#ifndef NDEBUG
		             if (isDebugEnabled ()) {
			        std:: cout << "OpenSpeedShop::Watcher::scanForRawPerformanceData() " 
                                           << " Did not find openss-data filename, direntry->d_name=" << direntry->d_name 
			                   << std::endl;
			     } // end debug
#endif
		          } // end else
			}
		      closedir (rawdata_dirhandle);
	            // end if rawdata_dirhandle
		    } else {
                       perror("rawdata_dirhandle is NULL, failed to open /tmp/rawdata_dir or OPENSS_RAWDATA_DIR/rawdata_dir");
                    }

		} else {
                  // There was no match on the directory name created with the input pid (pid_to_monitor)
                  // break;
                }		
	    }			// while perfdata_dirhandle
          closedir (perfdata_dirhandle);
   // end if perfdata_dirhandle
   } else {
    // perfdata_dirhandle is NULL
    perror("perfdata_dirhandle is NULL, failed to open /tmp or OPENSS_RAWDATA_DIR");
   }
   return;
} // end scan for directory and file

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
#if 1
  char data_dirname[PATH_MAX];
#endif
  XDR xdrs;
  std::string host = "";
  unsigned int blobsize = 0;
  uint64_t prevSize = 0;
  uint64_t prevPos = 0;
  WatcherThreadTable::FileInfoEntry currentFileEntryInfo;
  std::set<pid_t > PidSet; 
  std::vector<std::pair<pid_t, std::pair< std::pair< bool, pthread_t>, std::string> > > PidsAndHostsVector; 
  pid_t pid_to_monitor=0; 
  std::string host_to_monitor = ""; 

#ifndef NDEBUG
  if (isDebugEnabled ())
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

#ifndef NDEBUG
      if (isDebugEnabled ()) {
	  std::stringstream output;
	  output << "[TID " << pthread_self () << "] OpenSpeedShop::Watcher::fileIOmonitorThread()" 
	         << " ----- Go to sleep using nanosleep." << std::endl;
	  std::cerr << output.str ();
	}
#endif

      // Suspend ourselves for two seconds
      wait.tv_sec = 2;
      wait.tv_nsec = 0;
      nanosleep (&wait, NULL);

      // Acquire lock for the scan for rawdata files
      // This lock is competing with similiar code in watchProcess()
      OpenSpeedShop::Watcher::acquireScanLock();

#ifndef NDEBUG
      if (isDebugEnabled ()) {
	  std::stringstream output;
	  output << "[TID " << pthread_self () << "] OpenSpeedShop::Watcher::fileIOmonitorThread()" <<
	    " ----- Wake up from nanosleep." << std::endl;
	  std::cerr << output.str ();
	}
#endif

    PidsAndHostsVector.clear();
    PidsAndHostsVector = OpenSpeedShop::Framework::ThreadTable::TheTable.getActivePidsAndHosts();

    if (PidsAndHostsVector.size() > 0) {
      for (std::vector<std::pair<pid_t, std::pair< std::pair< bool, pthread_t>, std::string> > >::iterator ph = PidsAndHostsVector.begin(); 
                                                                ph != PidsAndHostsVector.end(); ph++) {
#ifndef NDEBUG
         if (isDebugEnabled ()) {
	    std::stringstream output;
            output << " OpenSpeedShop::Watcher::fileIOmonitorThread(), ph->first=" 
                      << ph->first << " ph->second.first.second=" << ph->second.first.second << std::endl;
	    std::cerr << output.str ();
         }
#endif

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
         //

         // Kind of Kludge - the database names are created from gethostname (non-canonical) but getHost returns canonical.
         // So, here we look for the first '.' and take the characters prior to it as the non-canonical name
         // Use this until we process host names uniformly across the OpenSpeedShop tool set.
         //
         std::string tmp_str =  ph->second.second;
         int dot_loc = ph->second.second.find('.');
         std::string new_tmp_str = tmp_str.substr (0,dot_loc);
       
         // First scan the main directory for sub-directories using the canonical name
         //
         scanForRawPerformanceData(ph->first, ph->second.second, ph->second.first.second);

         // Now scan the main directory for sub-directories using the truncated canonical name
         //
         scanForRawPerformanceData(ph->first, new_tmp_str.c_str(), ph->second.first.second);

      }

    }


    // Release the scan lock so the code in watchProcess() can have it, if needed
    //
    OpenSpeedShop::Watcher::releaseScanLock();

#ifndef NDEBUG
      if (isDebugEnabled ())
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
  if (isDebugEnabled ())
    {
      std::stringstream output;
      output << "[TID " << pthread_self () 
	     << "] OpenSpeedShop::Watcher::startWatching()" 
             << " Entered " << std::endl;
      std::cerr << output.str ();
    }
#endif

  // Create the monitor/watcher thread
  Assert (pthread_create (&fileIOmonitor_tid,
			  NULL,
			  OpenSpeedShop::Watcher::fileIOmonitorThread,
			  NULL) == 0);

}

void
OpenSpeedShop::Watcher::watchProcess(ThreadNameGroup threads) 
{

#ifndef NDEBUG
  if (isDebugEnabled ()) {
      std::stringstream output;
      output << "[TID " << pthread_self ()
	     << "] OpenSpeedShop::Watcher::watchProcess()" 
             << " Entered FINAL CHECK - FLUSH DATA IF ANY " << std::endl;
      std::cerr << output.str ();
    }
#endif
    
    // Acquire lock for the scan for rawdata files
    // This lock is competing with similiar code in fileIOmonitorThread()
    OpenSpeedShop::Watcher::acquireScanLock();

    for(ThreadNameGroup::const_iterator i = threads.begin(); i != threads.end(); ++i) {

#ifndef NDEBUG
       if (isDebugEnabled ()) {
           std::stringstream output;
           output << "[TID " << pthread_self ()
	          << "] OpenSpeedShop::Watcher::watchProcess()" 
                  << " i->getProcessId()= " << i->getProcessId() 
                  << " i->getHost()= " << i->getHost() << std::endl;
           std::cerr << output.str ();
         }
#endif
       // Call to flush data for this pid and or tid  before the completion
       // of the termination processing for this process.


       std::pair<bool, pthread_t> tid = i->getPosixThreadId();

       std::string tmp_str2 =  i->getHost();
       int dot_loc2 = i->getHost().find('.');
       std::string new_tmp_str2 = tmp_str2.substr (0,dot_loc2);

       if(tid.first) {
//           std::cout << "[TID " << pthread_self() << "] OpenSpeedShop::Watcher::watchProcess(), tid.second=  " << tid.second << std::endl;
#ifndef NDEBUG
          if (isDebugEnabled ()) {
              std::stringstream output;
              output << "[TID " << pthread_self ()
                     << "] OpenSpeedShop::Watcher::watchProcess()" 
                     << " tid.second= " << tid.second << std::endl;
              std::cerr << output.str ();
         }
#endif
           // First scan the main directory for sub-directories using the canonical name
           //
           scanForRawPerformanceData(i->getProcessId(), i->getHost(), tid.second);

           // Now scan the main directory for sub-directories using the truncated canonical name
           //
           scanForRawPerformanceData(i->getProcessId(), new_tmp_str2.c_str(), tid.second);

         } else {

#ifndef NDEBUG
           if (isDebugEnabled ()) {
              std::stringstream output;
              output << "[TID " << pthread_self ()
	             << "] OpenSpeedShop::Watcher::watchProcess()" 
                     << " tid.first= " << tid.first << std::endl;
              std::cerr << output.str ();
          }
#endif
           // First scan the main directory for sub-directories using the canonical name
           //
           scanForRawPerformanceData(i->getProcessId(), i->getHost(), 0);

           // Now scan the main directory for sub-directories using the truncated canonical name
           //
           scanForRawPerformanceData(i->getProcessId(), new_tmp_str2.c_str(), 0);

         } // end else clause where tid.first was not true, so therefore there is no thread_id

     }  // end loop through threads

    // Release the scan lock so the code in fileIOmonitorThread() can have it, if needed
    //
    OpenSpeedShop::Watcher::releaseScanLock();
}

void
OpenSpeedShop::Watcher::stopWatching ()
{

#ifndef NDEBUG
  if (isDebugEnabled ())
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
