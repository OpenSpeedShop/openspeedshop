////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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
#include "BFDSymbols.hxx"
#include "ThreadName.hxx"
#include <algorithm>

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
static   PCBuffer data_addr_buffer;
};

std::map<AddressRange, std::set<LinkedObject> > global_needed;

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
		     std::vector<OfflineParamVal> *value_list) {


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
		if (subdir.find("..") == string::npos &&
		    subdir.find(".") == string::npos ) {

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

	    if (rawfile.find(".openss-data") != string::npos ||
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
            theExperiment->updateThreads(expPid,expPosixTid,expHost);
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

	    if (mm.getUniqueId() == "sampling_rate") {
	       //std::cerr << "sampling_rate is: " << expRate << std::endl;
	       o_param.pushVal((int64_t) expRate);
	    } else if (mm.getUniqueId() == "traced_functions") {
		std::map<std::string, bool> traced;
		c.getParameterValue("traced_functions", traced);
	    } else if (mm.getUniqueId() == "event") {
	    }

	    (void) setparam(c, param_name, value_list);
	}
    }

    // Process data first so we can find the unique pc values
    // found in this experiments performance date.  These pc values
    // will be used to restrict the symbol lookup to just those
    // linked objects, addressspaces, files, functions, statements
    // where performance data was collected.

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
 
    // Process the list of dsos and address ranges for this experiment.
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
 * For an experiment info blob, this the usual header and an
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

// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	std::cout << "info..." << std::endl
		  << "collector name: " << info.collector << std::endl
		  << "host name:      " << infoheader.host << std::endl
		  << "executable :    " << info.exename << std::endl
		  << "pid:            " << infoheader.pid << std::endl
		  << "tid:            " << infoheader.posix_tid << std::endl;

	std::cout << "infoheader..." << std::endl
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

	char *theData;
	theData = (char *) malloc (blobsize+4);
	if (theData == 0) {
	    std::cerr << "Could Not allocate memory for data!" << std::endl;
	    fclose(f);
	    abort ();
	}

	memset(theData, 0, blobsize);
	int bytesRead = fread(theData, 1, blobsize, f);
	if (bytesRead == 0 || bytesRead < blobsize) {
	    // FIXME: What does this mean?
	    //std::cerr << "Could Not read any more data..." << std::endl;
	}

	// For offline collection we really maintain one dataqueue.
	// So the collector runtimes need to set the header.experiment to 0.
	// This is the first index into the DataQueue.
	Blob datablob(blobsize, theData);
	DataQueues::enqueuePerformanceData(datablob);
	theExperiment->flushPerformanceData();

	if (theData) {
	    free(theData);
	}

    } // while

    fclose(f);
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

	// dso or executable blob.
	openss_objects objs;
	memset(&objs, 0, sizeof(objs));

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

	bool_t objcall = xdr_openss_objects(&xdrs, &objs);

	if (objcall) {

	    // do not include entries with no name or [vdso].
	    std::string objname = objs.objname;
            if (objname.empty()) {
		continue;
	    }

	    uint64_t endaddr = objs.addr_end;
	    Address b_addr(objs.addr_begin);

	    Address e_addr(endaddr);
	    if (objs.addr_end <= objs.addr_begin) {
		// have not seen this happen ever. still, lets be safe.
		e_addr = Address(objs.addr_begin + 1);
	    }
	    AddressRange range(b_addr,e_addr);

            Time tbegin(objsheader.time_begin);
            Time tend(objsheader.time_end);

	    if (objsheader.time_end <= objsheader.time_begin) {
		// This may happen on very rare occasions on multi-core cpus.
		// If we get a corrupted time end, just default 
		// to the default largest time defined by the framework
		tend = Time::TheEnd();
	    }

            TimeInterval time_interval(tbegin,tend);

	    // Skip any vdso or vsyscall entry from /proc/self/maps.
            if ( objname.find("[vdso]") == string::npos &&
                 objname.find("[vsyscall]") == string::npos &&
		 objname.compare("unknown") != 0 ) {
                names_to_range.push_back(std::make_pair(objname,range));
                names_to_time.push_back(std::make_pair(objname,time_interval));
            }

//DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
		std::cout << "range for " << objs.objname << " is"
		  << range <<  " is dlopen " << objs.is_open << std::endl;

		std::cout << "objectsheader..." << std::endl
		  << "experiment id:  " << objsheader.experiment << std::endl
		  << "collector id:   " << objsheader.collector << std::endl
		  << "host:           " << objsheader.host << std::endl
		  << "pid:            " << objsheader.pid << std::endl
		  << "posix_tid:      " << objsheader.posix_tid << std::endl
		  << "time_begin:     " << objsheader.time_begin << std::endl
		  << "time_end:       " << objsheader.time_end << std::endl
		  << "addr_begin:     " << objsheader.addr_begin << std::endl
		  << "addr_end:       " << objsheader.addr_end << std::endl;
        	std::cout << std::endl;
	    }
#endif
    	     xdr_free(reinterpret_cast<xdrproc_t>(xdr_openss_objects),
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
    BFDSymbols bfd_symbols;
    FuncMap FunctionsWithData;
    StatementsVec StatementsWithData;


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

    // Create an Extent from this experiments performance data.
    Extent e = theExperiment->getPerformanceDataExtent();

    // ExtentGroup for use with getUniquePCValues.
    ExtentGroup eg;

    CollectorGroup cgrp = theExperiment->getCollectors();

    // Find the unique address values collected for this experiment
    // from all threads for the current executable being processed.
    // For mpi jobs we group threads to a specific executable.
    // That allows us to find all the sampled address from each mpi rank
    // and then search the linked objects for that  thread group
    // for functions and statements.
    //
    for(ThreadGroup::const_iterator i = threads.begin();
                                    i != threads.end(); ++i) {
        Instrumentor::retain(*i);
	std::string collector_name;
	CollectorGroup::iterator ci;
	for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
	    Collector c = *ci;
	    Metadata m = c.getMetadata();
	    collector_name = m.getUniqueId();

	    // get the Extent of this collectors data for this thread.
	    eg.push_back(c.getExtentIn(*i));

	    // add performance sample addresses to address buffer
	    // for this thread group.
	    c.getUniquePCValues(*i,eg,&data_addr_buffer);
	}
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_offlinesymbols_enabled) {
        std::cerr << "OfflineExperiment::createOfflineSymbolTable have total "
	<< data_addr_buffer.length << " UNIQUE addresses" << std::endl;
    }
#endif

    std::set<std::string> dsos_used;
    AddressSpace address_space;

    for (unsigned i = 0; i < data_addr_buffer.length; i++) {
 	Address addr = Address(data_addr_buffer.pc[i]);

	// create list of dso's for which there is an address in the performance
	// data. Update database address_space with threads and linked objects.
	// Mark the executable (is_executable = true, shared librarys = false).
	for(std::vector<std::pair<std::string, AddressRange> >::iterator
              j = names_to_range.begin(); j != names_to_range.end(); ++j) {

	    bool_t range_found = false;
	    if (j->second.doesContain(addr) && !range_found) {
		range_found = true;
		if (dsos_used.find(j->first) == dsos_used.end()) {

		    dsos_used.insert(j->first);
		    bool is_exe = false;

                    std::set<std::string>::iterator exei =
                                expExecutableName.find(j->first);
                    if(exei != expExecutableName.end()) {
                        is_exe = true;
                    }
// DEBUG
#ifndef NDEBUG
		    if(is_debug_offline_enabled) {
		      std::cerr << "OfflineExperiment::createOfflineSymbolTable:"
			<< " Added " << j->first
			<< " to linkedobjects table, is executable "
			<< is_exe << std::endl;
		    }
#endif
		    address_space.setValue(j->second, j->first,
					   /*is_executable*/ is_exe);
		}
	    }
	} 
    }

    Time when;
    for(std::vector<std::pair<std::string, TimeInterval> >::iterator
              j = names_to_time.begin(); j != names_to_time.end(); ++j) {
	when = j->second.getBegin();
	break;
    }

    // creates empty symboltable for these.
    // Apparently, all symboltable seem to have Time::TheBegining
    // as the load begin time.
    // This updateThreads call will update the addressspace, linkedobjects,
    // and files tables in the database. We do not use returned map
    // of needed symboltables since some of the existing symboltables
    // may also be used by the current executable (if more than one
    // executable contributes data for this experiment - e.g. mpi).
    std::map<AddressRange, std::set<LinkedObject> > tneeded =
    address_space.updateThreads(threads, when,
				/*update_time_interval*/ false);

// FUTURE call in AddressSpace class to set time_begin for
// addressspaces.  The offline rawdata conversion tool will
// use this to ensure that the time stamp when linked object
// is loaded will be set correctly in the database.
// Currently updateThreads does not do this for offline.
#if 0
    address_space.updateThreadsTimeBegin(threads, when);
#endif

    // For the conventional dpcl/mrnet dynamic instrumentors the
    // AddressSpace::updateThreads call will return only those
    // symboltables that where not previously built.  This is
    // expected for dpcl/mrnet since ALL symbols are parsed and
    // added to the database when first see a linked object.
    //
    // For offline experiments, we post process symbols according
    // to performance data sample addresses. Therfore we can not
    // assume any previously seen symboltables where completey built.
    // In the case of openmpi, the orterun (mpirun) process may
    // be first processed for symbols and creates entries in the
    // database for any number of linked objects.  When we parse the
    // mpi application later, AddressSpace::updateThreads assumes
    // we have already gotten symbols for dsos like libc.so which
    // was parsed previously by orterun and therefore would skip
    // such dsos even though we may have new function addresses
    // from the mpi aplication with in a previously see dso.
    //
    // The following code creates a map of address ranges and
    // linkedobjects based on performance data sample addresses.
    // We still need to call AddressSpace::updateThreads prior to
    // this to update the database addressspace, file, linkedobects
    // tables.
    std::map<AddressRange, std::set<LinkedObject> > needed; 

    // Find all linkedobjects for the current threadgroup.
    std::set<LinkedObject> objects = threads.getLinkedObjects();
    for(std::set<LinkedObject>::const_iterator li = objects.begin();
				    li != objects.end(); ++li) {


	// Find addressranges for this linked object.
	std::set<AddressRange> obj_range = (*li).getAddressRange();
	for(std::set<AddressRange>::const_iterator obj_ar = obj_range.begin();
		obj_ar != obj_range.end(); ++obj_ar) {

	    for(std::vector<std::pair<std::string, AddressRange> >::iterator
           		nr = names_to_range.begin();
			nr != names_to_range.end();
			++nr) {

		// See if this linked object is found in our list of
		// dsos and addressranges for the current executable,
		if ((*nr).second.doesContain(*obj_ar) &&
			(*nr).first == (*li).getPath() ) {

// DEBUG
#ifndef NDEBUG
		    if(is_debug_offline_enabled) {
			std::cerr
			<< "OfflineExperiment::createOfflineSymbolTable "
			<< " linked object " << (*li).getPath()
			<< " contains " << (*obj_ar) << std::endl;
		    }
#endif

		    std::map<AddressRange, std::set<LinkedObject> >::iterator
			ni = needed.insert( std::make_pair( (*obj_ar),
					    std::set<LinkedObject>())).first;

		    ni->second.insert(*li);
		    // FOUND IT.  stop looking.
		    break;
		}
	    }
	}
    }

    // Iterate over each needed symbol table for this address space
    for(std::map<AddressRange, std::set<LinkedObject> >::const_iterator
	    t = needed.begin(); t != needed.end(); ++t) {

// DEBUG
#ifndef NDEBUG
	if(is_debug_offlinesymbols_enabled) {
	    for (std::set<LinkedObject>::const_iterator
		    l = t->second.begin(); l != t->second.end(); ++l) {
	        std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	        << " INSERT SYMTAB for " <<  (*l).getPath()
	        << " RANGE " << t->first
	        << " AT TIME " << when
	        << std::endl;
	    }
	}
#endif

	// Add an empty symbol table to the state for this address range
	symtabmap.insert( std::make_pair(t->first,
			  std::make_pair(SymbolTable(t->first), t->second))
			);
    }

    int addresses_found =-1;
 
    // TODO: Make this configurable for use with symtabAPI from dyninst.
    // Using bfd to find symbols.
    // Foreach linked object we find that has pc data,
    // pass the buffer of sampled addresses to getBFDFunctionStatements
    // and resolve function and statement symbols.
    std::set<LinkedObject> tgrp_lo = threads.getLinkedObjects();
    for(std::set<LinkedObject>::const_iterator j = tgrp_lo.begin();
		j != tgrp_lo.end(); ++j) {
	    LinkedObject lo = (*j);
            addresses_found =
		bfd_symbols.getBFDFunctionStatements(&data_addr_buffer, &lo);
    }

    FunctionsWithData = bfd_symbols.bfd_functions;
    StatementsWithData =  bfd_symbols.bfd_statements;

// DEBUG
#ifndef NDEBUG
    if(is_debug_offline_enabled) {
      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " getBFDFunctionStatements found functions for " << addresses_found
	<< " sample addresses from total "
	<< data_addr_buffer.length << std::endl;

      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " Functions with DATA: " << FunctionsWithData.size()
	<< std::endl;
      std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
	<< " StatementsWithData with DATA: " << StatementsWithData.size()
	<< std::endl;
    }
#endif

    // Add Functions
    // RESTRICT functions to only those with sampled addresses.
    int functionsadded = 0;
    for(FuncMap::const_iterator ic = FunctionsWithData.begin();
				ic != FunctionsWithData.end(); ++ic) {
	AddressRange frange(ic->second.func_begin,ic->second.func_end);
        if (symtabmap.find(frange) != symtabmap.end()) {
	    SymbolTable& symbol_table =  symtabmap.find(frange)->second.first;
	    Address start = ic->second.func_begin;
	    Address end = ic->second.func_end;
// DEBUG
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
              std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
		<< "ADDING FUNCTION for " << ic->first
		<< " with range " << frange << std::endl;
	    }
#endif
	    symbol_table.addFunction(start, end, ic->first);
	    functionsadded++;
	} else {
              std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
		<< "FAILED FUNCTION for " << ic->first
		<< " with range " << frange << std::endl;
	}
    }

    // Add Statements
    // RESTRICT statement to only those with sampled addresses.
    int statementsadded = 0;
    for(StatementsVec::iterator objsyms = StatementsWithData.begin() ;
	objsyms != StatementsWithData.end(); ++objsyms) {
      AddressRange frange(objsyms->pc,objsyms->pc+1);
      if (symtabmap.find(frange) != symtabmap.end()) {
	SymbolTable& symbol_table =  symtabmap.find(frange)->second.first;
	Address s_begin = objsyms->pc;
	Address s_end = objsyms->pc + 1;
	std::string path = objsyms->file_name;
	unsigned int line = objsyms->lineno;
	unsigned int col = 0;
// DEBUG
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
            std::cerr << "OfflineExperiment::createOfflineSymbolTable"
		<< " ADDING STATEMENT for " << Address(objsyms->pc)
		<< " with path " << path
		<< " line " << line
		<< std::endl;
	}
#endif
	if (path.size() != 0) {
	    symbol_table.addStatement(s_begin,s_end,path,line,col);
	    statementsadded++;
	} else {
	}
      } else {
              std::cerr << "OfflineExperiment::createOfflineSymbolTable: "
		<< "FAILED STATEMENT for " << Address(objsyms->pc)
		<< " path " << objsyms->file_name
		<< " line " << objsyms->lineno
		<< std::endl;
      }
    }

    std::cerr << "Added " << functionsadded
	<< " functions to symboltable" << std::endl;
    std::cerr << "Added " << statementsadded
	<< " statements to symboltable" << std::endl;

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
    names_to_range.clear();
    names_to_time.clear();
}
