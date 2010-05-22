////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008,2009 The Krell Institute. All Rights Reserved.
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
 * Declaration of the OfflineExperiment class.
 *
 */

#include "EntrySpy.hxx"
#include "ToolAPI.hxx"
#include "AddressSpace.hxx"
#include "AddressRange.hxx"
#include "DataQueues.hxx"
#include "OfflineExperiment.hxx"
#include "SymbolTable.hxx"
#include "Instrumentor.hxx"
#include "Blob.hxx"
#include "ThreadName.hxx"

#if defined(OPENSS_USE_SYMTABAPI)
#include "SymtabAPISymbols.hxx"
#include "Symtab.h"
#else
#include "BFDSymbols.hxx"
#endif

#include <algorithm>

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "OpenSS_DataHeader.h"
#include "offline/offline.h"

#include "OpenSSPath.cxx"

using namespace OpenSpeedShop::Framework;

namespace {
    struct DsoEntry
    {

    /** Full path name of this linked object. */
    Path dso_path;

    /** Flag indicating if this linked object is an executable. */
    std::string dso_host;
    int  dso_pid;
    uint64_t dso_tid;
    AddressRange dso_range;
    TimeInterval dso_time;
    int dso_dlopen;

    /** Constructor from fields. */
    DsoEntry(const Path& path, const std::string host, const int& pid,
	     const uint64_t& tid, const AddressRange& range,
	     const TimeInterval& time, const int& is_dlopen) :
	dso_path(path),
	dso_host(host),
	dso_pid(pid),
	dso_tid(tid),
	dso_range(range),
	dso_time(time), 
	dso_dlopen(is_dlopen)
	{ }
    };

    typedef  std::vector<DsoEntry> DsoVec;
    static   PCBuffer data_addr_buffer;
    DsoVec dsoVec;
};

#ifndef NDEBUG
/** Flag indicating if debuging for MPI jobs is enabled. */
bool OfflineExperiment::is_debug_offlinesymbols_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS") != NULL);

/** Flag indicating if debuging for offline experiments is enabled. */
bool OfflineExperiment::is_debug_offline_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE") != NULL);
#endif

/**
 * Utility: setparam()
 * 
 * Taken from cli and modified for OfflineExperiment.
 * .
 *     
 * @param   .
 *
 * @return  bool
 *
 * @todo    Error handling.
 *
 */
static bool setparam(Collector C, std::string pname,
		     std::vector<OfflineParamVal> *value_list)
{

  std::set<Metadata>::const_iterator mi;
  std::set<Metadata> md = C.getParameters();
  for (mi = md.begin(); mi != md.end(); mi++) {
    Metadata m = *mi;
    if (m.getUniqueId() != pname) {
     // Not the one we want - keep looking.
      continue;
    }

    if ( m.isType(typeid(std::map<std::string, bool>)) ) {
     // Set strings in the value_list to true.
      std::map<std::string, bool> Value;
      C.getParameterValue(pname, Value);

     // Set all the booleans to true, if the corresponding name is in the list,
     // and false otherwise.
      for (std::map<std::string, bool>::iterator
                 im = Value.begin(); im != Value.end(); im++) {
        bool name_in_list = false;
        for (std::vector<OfflineParamVal>::iterator
                iv = value_list->begin(); iv != value_list->end(); iv++) {
          Assert (iv->getValType() == PARAM_VAL_STRING);
          if (!strcasecmp( im->first.c_str(), iv->getSVal() )) {
            name_in_list = true;
            break;
	  }
        }
        im->second = name_in_list;

      }

      C.setParameterValue(pname,Value);
    } else {
      OfflineParamVal pvalue = (*value_list)[0];

      if( m.isType(typeid(int)) ) {
        int ival;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%d", &ival);
        } else {
          ival = (int)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(int)ival);
      } else if( m.isType(typeid(int64_t)) ) {
        int64_t i64val;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%lld", &i64val);
        } else {
          i64val = (int64_t)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(int64_t)i64val);
      } else if( m.isType(typeid(uint)) ) {
        uint uval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%d", &uval);
        } else {
          uval = (uint)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(uint)uval);
      } else if( m.isType(typeid(uint64_t)) ) {
        uint64_t u64val;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%lld", &u64val);
        } else {
          u64val = (uint64_t)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(uint64_t)u64val);
      } else if( m.isType(typeid(float)) ) {
        float fval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%f", &fval);
        } else {
          fval = (float)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(float)fval);
      } else if( m.isType(typeid(double)) ) {
        double dval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%llf", &dval);
        } else {
          dval = (double)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(double)dval);
      } else if( m.isType(typeid(string)) ) {
        std::string sval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sval = std::string(pvalue.getSVal());
        } else {
          char cval[20];
          sprintf( cval, "%d", pvalue.getIVal());
          sval = std::string(&cval[0]);
        }
        C.setParameterValue(pname,(std::string)sval);
      }
    }

    return true;
  }

  // We didn't find the named parameter in this collector.
  return false;
}

int
OfflineExperiment::getRawDataFiles (std::string dir)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cerr << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }


    struct stat file_info;
    stat(dir.c_str(), &file_info);
    bool is_dir = S_ISDIR(file_info.st_mode);

    if (is_dir) {
	while ((dirp = readdir(dp)) != NULL) {
// DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
	        std::cerr << "OfflineExperiment::getRawDataFiles testing "
		<< dirp->d_name << std::endl;
	    }
#endif
	    struct stat file_info1;
	    std::string fname(dir+"/"+dirp->d_name);
	    stat(fname.c_str(), &file_info1);
	    bool is_subdir = S_ISDIR(file_info1.st_mode);
	    bool is_regfile = S_ISREG(file_info1.st_mode);

	    if (is_subdir) {
		std::string subdir(dirp->d_name);
		if (subdir.compare("..") != 0 && subdir.compare(".") != 0) {
		    rawdirs.push_back(dir+"/"+subdir);
		}
	    }
	    else {
	    }
	}

	// no subdirs.  must be a single raw data dir.
	if (rawdirs.size() == 0 ) {
	    rawdirs.push_back(dir);
	}
    }


    DIR *dpsub;
    struct dirent *dirpsub;

    std::set<std::string> infoList;
    std::set<std::string> dataList;
    std::set<std::string> dsosList;
    std::set<std::string> executables_used;

    string::size_type basedirpos =
		rawdirs[0].find("/openss-rawdata-", 0);
    std::string basedir = rawdirs[0].substr(0,basedirpos);


    for (unsigned int i = 0;i < rawdirs.size();i++) {

	if((dpsub  = opendir(rawdirs[i].c_str())) == NULL) {
            std::cerr << "Error(" << errno << ") opening "
		<< rawdirs[i] << std::endl;
            return errno;
	}


	while ((dirpsub = readdir(dpsub)) != NULL) {

	    std::string rawfile(dirpsub->d_name);
	    stat(rawfile.c_str(), &file_info);
	    bool is_dir = S_ISDIR(file_info.st_mode);

	    if (rawfile.find("-") != 0 && 
                rawfile.find(".openss-data") != string::npos ||
		rawfile.find(".openss-info") != string::npos ||
		rawfile.find(".openss-dsos") != string::npos ) {

		rawfiles.push_back(std::string(rawfile));

	    }
	}

	if (rawfiles.size() > 0 ) {
	    rawdatadir = rawdirs[i];
	}

	// Sort our raw data files into info, dsos, and data lists. 
	std::string rawname;
	for (unsigned int i = 0;i < rawfiles.size();++i) {

	    // create the master list of info files.
	    if (rawfiles[i].find(".openss-info") != string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];
		infoList.insert(rawname);
	    }

	    // create the master list of data files.
	    // update list of unique executables found.
	    if (rawfiles[i].find(".openss-data") != string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];

		string::size_type pos = rawfiles[i].find_first_of("-", 0);
		std::string temp = rawfiles[i].substr(0,pos);

		executables_used.insert(temp);
		dataList.insert(rawname);
	    }

	    // create the master list of dsos files.
	    if (rawfiles[i].find(".openss-dsos") != string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];
		dsosList.insert(rawname);
	    }
	}

	rawfiles.clear();
	closedir(dpsub);
    }

    std::set<std::string>::iterator ssi,ssii, temp;
    for( ssi = executables_used.begin(); ssi != executables_used.end(); ++ssi) {
	std::cerr << "Processing raw data for " << (*ssi) << std::endl;
        for( ssii = dataList.begin(); ssii != dataList.end(); ++ssii) {
	    if( (*ssii).find((*ssi)) != string::npos) {
		rawfiles.push_back((*ssii));
	    }
	}


        for( ssii = infoList.begin(); ssii != infoList.end(); ++ssii) {
	    if( (*ssii).find((*ssi)) != string::npos) {
		// restrict list to only those for which a matching
		// openss-data file exists.
		string::size_type pos = (*ssii).find(".openss-info", 0);
		std::string ts = (*ssii).substr(0,pos) + ".openss-data";
		for(temp = dataList.begin(); temp != dataList.end();
							++temp) {
		    if ((*temp).find(ts) != string::npos) {
			rawfiles.push_back((*ssii));
		    }
		}
	    }
	}
        for( ssii = dsosList.begin(); ssii != dsosList.end(); ++ssii) {
	    if( (*ssii).find((*ssi)) != string::npos) {
		// restrict list to only those for which a matching
		// openss-data file exists.
		string::size_type pos = (*ssii).find(".openss-dsos", 0);
		std::string ts = (*ssii).substr(0,pos) + ".openss-data";
		for(temp = dataList.begin(); temp != dataList.end();
							++temp) {
		    if ((*temp).find(ts) != string::npos) {
			rawfiles.push_back((*ssii));
		    }
		}
	    }
	}
        convertToOpenSSDB();
        createOfflineSymbolTable();
	rawfiles.clear();
    }

    if (dp) {
	free(dp);
    }
    return 0;
}

int OfflineExperiment::convertToOpenSSDB()
{
    std::string rawname;

    // process offline info blobs first.
    std::cerr << "Processing processes and threads ..." << std::endl;
    for (unsigned int i = 0;i < rawfiles.size();++i) {
	bool_t found_infofile = false;
	if (rawfiles[i].find(".openss-info") != string::npos) {
	    //std::cerr << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_infofile = true;
	} 

        if (found_infofile) {
            bool_t rval = process_expinfo(rawname);
            if (!rval) {
    	        std::cerr << "Could not process experiment info for: "
		    << rawname << std::endl;
            }

            // add this pid and host to database.
            theExperiment->updateThreads(expPid,expPosixTid,expRank,expHost);
        } else {
	}
    }

    // Set LD_LIBRARY_PATH and plugin path so we can find the
    // collector implementation.
    std::string dummy;
    SetOpenssLibPath(dummy);

    CollectorGroup cgrp = theExperiment->getCollectors();
    std::string collector_name;
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        Metadata m = c.getMetadata();
        collector_name = m.getUniqueId();
    }

    if (expCollector != collector_name) {
        Collector c = theExperiment->createCollector(expCollector);
        Metadata m = c.getMetadata();
        collector_name = m.getUniqueId();
	Blob b;

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	    std::cerr << "COLLECTOR   : " << collector_name << std::endl;
	    std::cerr << "            : " << m.getShortName() << std::endl;
	    std::cerr << "            : " << m.getType() << std::endl;
	    std::cerr << "            : " << m.getDescription() << std::endl;
	}
#endif
	
	std::set<Metadata> md = c.getParameters();
	std::set<Metadata>::const_iterator mi;
	for (mi = md.begin(); mi != md.end(); mi++) {
	    Metadata mm = *mi;

// DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
		std::cerr << "PARAMETERS  : " << mm.getUniqueId() << std::endl;
		std::cerr << "            : " << mm.getShortName() << std::endl;
		std::cerr << "            : " << mm.getType() << std::endl;
		std::cerr << "            : " << mm.getDescription() << std::endl;
	    }
#endif

	    char *type_name = (char*)collector_name.c_str();
	    char *param_name =  (char*)mm.getUniqueId().c_str();
	    OfflineParameters o_param(type_name, param_name);

	    std::vector<OfflineParamVal> *value_list = o_param.getValList();

	    // Parameter sampling_rate is an integer paramter.
	    // sampling_rate is used by pcsamp and usertime.  The sampling_rate
	    // parameter is aslo used by hwc and hwctime for threshold.
	    // Parameter traced_functions is string of functions separated by either
	    // a ":" or "," character.
	    // The traced_functions parameter is used by the io, iot, mpi, and mpit
	    // function tracing collectors.
	    // Parameter event is a string representing a hardware counter name
	    // or traced_fpes as a string of exceptions separated by either
	    // a ":" or ","  character.
	    // The event parameter is used by hwc and hwctime for the name of the
	    // hardware counter to sample.  The event parameter also defines the
	    // fpe execption (or exceptions) traced by the fpe collector.
	    if (mm.getUniqueId() == "sampling_rate") {
	       o_param.pushVal((int64_t) expRate);
	    } else if (mm.getUniqueId() == "traced_functions") {
		char *tfptr, *saveptr, *tf_token;
		tfptr = strdup(expTraced.c_str());
		for (int i = 1; ; i++, tfptr = NULL) {
		    tf_token = strtok_r(tfptr, ":,", &saveptr);
		    if (tf_token == NULL) {
			break;
		    } else {
			o_param.pushVal(tf_token);
		    }
		}

		if (tfptr) {
		    free(tfptr);
		}
	    } else if (mm.getUniqueId() == "event") {
		char *evptr, *saveptr, *ev_token;
		evptr = strdup(expEvent.c_str());
		for (int i = 1; ; i++, evptr = NULL) {
		    ev_token = strtok_r(evptr, ":,", &saveptr);
		    if (ev_token == NULL) {
			break;
		    } else {
			o_param.pushVal(ev_token);
		    }
		}

		if (evptr) {
		    free(evptr);
		}
	    }

	    (void) setparam(c, param_name, value_list);
	}
    }

    // Process data first so we can find the unique pc values
    // found in this experiments performance date.  These pc values
    // will be used to restrict the symbol lookup to just those
    // linked objects, addressspaces, files, functions, statements
    // where performance data was collected.

    std::cerr << "Processing performance data ..." << std::endl;
    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_datafile = false;
	if (rawfiles[i].find(".openss-data") != string::npos) {
	    //std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_datafile = true;
	} 

        if (found_datafile) {
            bool_t rval = process_data(rawname);
            if (!rval) {
	        std::cerr << "Could not process experiment info for: "
		        << rawname << std::endl;
            }
        }
    }

    theExperiment->flushPerformanceData();
 
    // Process the list of dsos and address ranges for this experiment.
    std::cerr << "Processing functions and statements ..." << std::endl;
    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_dsofile = false;
	if (rawfiles[i].find(".openss-dsos") != string::npos) {
	    //std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_dsofile = true;
	} 

        if (found_dsofile) {
            bool_t rval = process_objects(rawname);
            if (!rval) {
	        std::cerr << "Could not process experiment info for: "
	    	    << rawname << std::endl;
            }
        }
    }

    return(0);
}


/*
 * currently experiment info blob is first blob in file followed
 * by the either a string of data blobs or dso blobs and data blobs
 * the are intermixed.
 * Each blob will have a header and the actual blob data.
 * For an experiment info blob, this is the usual header and an
 * expinfo blob as described by the runtime's offline.x file.
 */

bool
OfflineExperiment::process_expinfo(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";
    long where = 0;

    FILE *f = fopen(rawfilename.c_str(),"r");

    xdrstdio_create(&xdrs, f, XDR_DECODE);

    unsigned int blobsize;
    if (!xdr_u_int(&xdrs, &blobsize)) {
	std::cerr << "process_expinfo fails to find blobsize" << std::endl;
    }

    /* info blob */
    openss_expinfo info;
    memset(&info, 0, sizeof(info));

    OpenSS_DataHeader infoheader;
    memset(&infoheader, 0, sizeof(infoheader));

    bool_t infoheadercall = xdr_OpenSS_DataHeader(&xdrs, &infoheader);
    if(!infoheadercall) {
	std::cerr << "Could Not find a valid data header in raw file "
		<< rawfilename << std::endl;
	fclose(f);
	return false;
    }

    bool_t infocall = xdr_openss_expinfo(&xdrs, &info);

    if(infocall) {
	std::string command = "empty command";
	expHost = infoheader.host;
	expExecutableName.insert(info.exename);
	expCollector = info.collector;
	expPosixTid = infoheader.posix_tid;
	expPid = infoheader.pid;
	expRate = info.rate;
	expColId = infoheader.collector;
	expExpId = infoheader.experiment;
	expEvent = std::string(info.event);
	expTraced = std::string(info.traced);
	expRank = info.rank;

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cout << "\ninfo..." << std::endl
		  << "collector name: " << info.collector << std::endl
		  << "executable :    " << info.exename << std::endl
		  << "mpi rank:       " << info.rank << std::endl;

	std::cout << "\ninfoheader..." << std::endl
		  << "host name:      " << infoheader.host << std::endl
		  << "experiment id:  " << infoheader.experiment << std::endl
		  << "collector id:   " << infoheader.collector << std::endl
		  << "host:           " << infoheader.host << std::endl
		  << "pid:            " << infoheader.pid << std::endl
		  << "posix_tid:      " << infoheader.posix_tid << std::endl
		  << "time_begin:     " << infoheader.time_begin << std::endl
		  << "time_end:       " << infoheader.time_end << std::endl
		  << "addr_begin:     " << infoheader.addr_begin << std::endl
		  << "addr_end:       " << infoheader.addr_end << std::endl;
	}
#endif

	where = ftell(f);
    	xdr_free(reinterpret_cast<xdrproc_t>(xdr_openss_expinfo),
		      reinterpret_cast<char*>(&info));
    } else {
	std::cerr << "process_expinfo info call FAILED" << std::endl;
	return false;
    }

    fclose(f);
    return true;
}

/*
 *
 * We handle specific xdr routines for the collector named
 * in expCollector by instantiating the collector implementation
 * in the same way that openss does.
 *
 */
bool
OfflineExperiment::process_data(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";

    FILE *f = fopen(rawfilename.c_str(),"r");
    xdrstdio_create(&xdrs, f, XDR_DECODE);

    bool_t done = false;

    while (!done) {
	unsigned int blobsize;
	if (!xdr_u_int(&xdrs, &blobsize)) {
	    //  no more entries. done.
	    break;
	}

	// FIXME: At large scales (1024 pe) usertime data blobs can
	// be written to the database of size 0 or 1. These are not
	// valid and will cause an assert in the DataQueue.
	// Problem found with smg2000 -n 40 40 40 using intel compilers
	// on a 128 node parition. Why did such a small blob
	// get written to the openss-data file?
	if (blobsize < 1024 ) {
	    std::cerr << "Data blob possibly corrupt from " << rawfilename
		<< " blobsize is " << blobsize << std::endl;
	    continue;
	}

	char* theData = (char*) malloc(blobsize+4);

	if (theData == 0) {
	    std::cerr << "Could Not allocate memory for data!" << std::endl;
	    fclose(f);
	    abort ();
	}

	memset(theData, 0, blobsize);
	int bytesRead = fread(theData, 1, blobsize, f);
	if (bytesRead == 0 || bytesRead != blobsize) {
	    // FIXME: On some nodes of hyperion there are
	    // bad writes to the offline-data files.
	    std::cerr << "Bad read of data for " << rawfilename
		<< " expected: " << blobsize << " got:" << bytesRead << std::endl;
	    if (theData) free(theData);
	    continue;
	}

	// For offline collection we really maintain one dataqueue.
	// So the collector runtimes need to set the header.experiment to 0.
	// This is the first index into the DataQueue.
	Blob datablob(blobsize, theData);
	DataQueues::enqueuePerformanceData(datablob);
	if (theData) free(theData);

    } // while

    fclose(f);
    // experimental code to remove raw openss-data file
    // once it is copied to the openss database.
    //std::remove(rawfilename.c_str());
    return true;
}

bool OfflineExperiment::process_objects(const std::string rawfilename)
{
    XDR xdrs;
    std::string host = "";

    FILE *f = fopen(rawfilename.c_str(),"r");
    xdrstdio_create(&xdrs, f, XDR_DECODE);

    bool_t done = false;
    while (!done) {
	unsigned int blobsize;
	if (!xdr_u_int(&xdrs, &blobsize)) {
	    // just means no more items in this file...
	    break;
	}

	OpenSS_DataHeader objsheader;
	memset(&objsheader, 0, sizeof(objsheader));
	bool_t objsheadercall = xdr_OpenSS_DataHeader(&xdrs, &objsheader);

	if(!objsheadercall) {
	    // just means no more items in this file...
	    break;
	}

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cerr << "OfflineExperiment::process_objects: "
	    << " OBJECT loaded at TIME: begin " << Time(objsheader.time_begin)
	    << " (" << objsheader.time_begin << ")"
	    << " end " << Time(objsheader.time_end)
	    << std::endl;
	}
#endif

	// dso or executable blob.
	offline_data objs;
	memset(&objs, 0, sizeof(objs));
	bool_t objcall = xdr_offline_data(&xdrs, &objs);

	if (objcall) {

	    for(int i = 0; i < objs.objs.objs_len; i++) {

	        std::string objname = objs.objs.objs_val[i].objname;
		if (objname.empty()) {
		    continue;
		}

		AddressRange range(Address(objs.objs.objs_val[i].addr_begin),
				   Address(objs.objs.objs_val[i].addr_end)) ;
		TimeInterval time_interval(Time(objs.objs.objs_val[i].time_begin),
					   Time(objs.objs.objs_val[i].time_end));

// DEBUG
#ifndef NDEBUG
		if(is_debug_offline_enabled) {
		  std::cerr << "DSONAME " << objname << std::endl;
		  std::cerr << "ADDR " << range << std::endl;
		  std::cerr << "TIME " << time_interval << std::endl;
		  std::cerr << "DLOPEN " << (int) objs.objs.objs_val[i].is_open  << std::endl;
		  std::cerr << std::endl;
		}
#endif

		if (objname.find("[vdso]") == string::npos &&
		    objname.find("[vsyscall]") == string::npos &&
		    objname.find("[stack]") == string::npos &&
		    objname.find("[heap]") == string::npos &&
		    objname.compare("unknown") != 0 ) {

		    DsoEntry e(objname,objsheader.host, objsheader.pid,
				objsheader.posix_tid, range,time_interval,
				objs.objs.objs_val[i].is_open);
		    dsoVec.push_back(e);
//DEBUG
#ifndef NDEBUG
		    if(is_debug_offline_enabled) {
			std::cout << "dsoVec inserts " << objname << ", " << range
			<< ", " << objsheader.host << ":"
			<< objsheader.pid << ":" << objsheader.posix_tid << std::endl;
		    }
#endif
		}

	    }

	    xdr_free(reinterpret_cast<xdrproc_t>(xdr_offline_data),
		      reinterpret_cast<char*>(&objs));
	}
    } // end while

    fclose(f);
    return true;
}


/**
 * Create the symbol table for the experiment database.
 * Currently bfd based...
 */
void OfflineExperiment::createOfflineSymbolTable()
{
    // reset the buffer of unique sampled addresses.
    data_addr_buffer.length=0;
    memset(&data_addr_buffer,0,sizeof(data_addr_buffer));

    SymbolTableMap symtabmap;

    // Find current threads used in this experiment.
    ThreadGroup threads;
    ThreadGroup original = theExperiment->getThreads();

    // Find any new threads.
    insert_iterator< ThreadGroup > ii( threads, threads.begin() );
    std::set_difference(original.begin(), original.end(),
			threads_processed.begin(), threads_processed.end(),
			ii);

// DEBUG
#ifndef NDEBUG
    if(is_debug_offlinesymbols_enabled) {
        std::cerr << "OfflineExperiment::createOfflineSymbolTable have total "
	<< original.size() << " threads" << std::endl;

        for(ThreadGroup::const_iterator ni = threads.begin();
				    ni != threads.end(); ++ni) {
	    std::cerr << "OfflineExperiment::createOfflineSymbolTable "
	    << " NEW THREAD: " << EntrySpy(*ni).getEntry() << std::endl;

        }
    }
#endif

    // add any new threads to threads_processed group.
    // the new threads will be processed below.
    insert_iterator< ThreadGroup > iu( threads_processed,
					threads_processed.begin() );
    std::set_union(original.begin(), original.end(),
			threads.begin(), threads.end(),
			iu);

    if (threads.size() == 0) {
	return;
    }

    // ExtentGroup for use with getUniquePCValues.
    ExtentGroup eg;
    eg.push_back(theExperiment->getPerformanceDataExtent());

    // Find the unique address values collected for this experiment
    // from all threads for the current executable being processed.
    // For mpi jobs we group threads to a specific executable.
    // That allows us to find all the sampled address from each mpi rank
    // and then search the linked objects for that  thread group
    // for functions and statements.
    //

#if defined(OPENSS_USE_SYMTABAPI)
    SymtabAPISymbols stapi_symbols;
#else
    BFDSymbols bfd_symbols;
#endif

    CollectorGroup cgrp = theExperiment->getCollectors();
    CollectorGroup::iterator ci = cgrp.begin();
    Collector c = *ci;
    Metadata m = c.getMetadata();

    std::map<AddressRange, std::set<LinkedObject> > tneeded;

    for(ThreadGroup::const_iterator i = threads.begin();
                                    i != threads.end(); ++i) {
	AddressSpace taddress_space;
        Instrumentor::retain(*i);

	// add performance sample addresses to address buffer
	// for this thread group.
	c.getUniquePCValues(*i,eg,&data_addr_buffer);

// DEBUG
#ifndef NDEBUG
	if(is_debug_offlinesymbols_enabled) {
	    std::cerr << "OfflineExperiment::createOfflineSymbolTable "
	     << "set address space for " << (*i).getProcessId() << std::endl;
	}
#endif

	pid_t d_pid = (*i).getProcessId();
	for(DsoVec::const_iterator d=dsoVec.begin(); d != dsoVec.end(); ++d) {
	    if (d_pid == d->dso_pid ) {
		bool is_exe = false;

		std::set<std::string>::iterator exei =
					expExecutableName.find(d->dso_path);
		if(exei != expExecutableName.end()) {
		    is_exe = true;
		}

		// Create an addressspace only for dso's with sample data.
		for (unsigned ii = 0; ii < data_addr_buffer.length; ii++) {
		    if (d->dso_range.doesContain(Address(data_addr_buffer.pc[ii]))) {
// DEBUG
#ifndef NDEBUG
			if(is_debug_offlinesymbols_enabled) {
		            std::cerr << "OfflineExperiment::createOfflineSymbolTable "
		            << "addressspace setValue for " <<  d->dso_path
		            << ":" << d->dso_range << ":" << d->dso_time << std::endl;
			}
#endif
			taddress_space.setValue(d->dso_range, d->dso_path,
					/*is_executable*/ is_exe);
			break;
		    }
		} // end for data_addr_buffer
	    }
	} // end for dsoVec

	// The AddressSpace class now supports an update specific
	// to the offline method of creating the AddressSPace table
	// in the database.  
	tneeded = taddress_space.updateThread(*i);
    } // end for threads

    for(std::map<AddressRange, std::set<LinkedObject> >::const_iterator
		t = tneeded.begin(); t != tneeded.end(); ++t) {
// DEBUG
#ifndef NDEBUG
	if(is_debug_offlinesymbols_enabled) {
	    std::cerr << "OfflineExperiment::createOfflineSymbolTable "
		<< " insert  symtab for " << t->first
		<< std::endl;
	}
#endif
	// Add an empty symbol table to the state for this address range.
	// Any samples taken for this linked object in an addresspace
	// other than the addressrange used for the symboltable will need
	// to compute an offset from the symboltable addressrange.
	symtabmap.insert(std::make_pair(t->first,
			 std::make_pair(SymbolTable(t->first), t->second))
			);
    }


    std::set<LinkedObject> ttgrp_lo = threads.getLinkedObjects();
    for(std::set<LinkedObject>::const_iterator j = ttgrp_lo.begin();
					       j != ttgrp_lo.end(); ++j) {
	LinkedObject lo = (*j);

#if defined(OPENSS_USE_SYMTABAPI)
	stapi_symbols.getSymbols(lo,symtabmap);
#else
	bfd_symbols.getSymbols(&data_addr_buffer,lo,symtabmap);
#endif

    } // end for threads linkedobjects

    // Now update the database with all our functions and statements...
    for(SymbolTableMap::iterator i = symtabmap.begin();
        i != symtabmap.end();
        ++i) {
        for(std::set<LinkedObject>::const_iterator j = i->second.second.begin();
            j != i->second.second.end();
            ++j) {
            i->second.first.processAndStore(*j);
        }
    }

    // clear names to range for our next linked object.
    dsoVec.clear();
}
