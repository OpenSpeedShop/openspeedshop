////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the OfflineExperiment class.
 *
 */

#include "EntrySpy.hxx"
#include "ToolAPI.hxx"
#include "AddressBitmap.hxx"
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
#if defined(HAVE_DYNINST)
#include "DyninstSymbols.hxx"
#endif
#else
#include "BFDSymbols.hxx"
#endif

#include <algorithm>

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "OpenSS_DataHeader.h"
#include "offline.h"

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
    std::set<Address> unique_addresses;
    DsoVec dsoVec;
};

#ifndef NDEBUG
/** Flag indicating if debuging for offline symbols is enabled. */
bool OfflineExperiment::is_debug_offlinesymbols_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS") != NULL);

/** Flag indicating if debuging for offline symbols is enabled. */
bool OfflineExperiment::is_debug_offlinesymbols_detailed_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS_DETAILS") != NULL);

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
          sscanf ( pvalue.getSVal(), "%lf", &dval);
        } else {
          dval = (double)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(double)dval);
      } else if( m.isType(typeid(std::string)) ) {
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

    std::string::size_type basedirpos =
		rawdirs[0].find("/openss-rawdata-", 0);
    std::string basedir = rawdirs[0].substr(0,basedirpos);

    std::string dsosuffix("openss-dsos");
    std::string datasuffix("openss-data");
    std::string infosuffix("openss-info");
    if (rawdirs.size() == 0 ) {
	basedirpos = rawdirs[0].find("/cbtf-rawdata-", 0);
	basedir = rawdirs[0].substr(0,basedirpos);
    } else {
	// must be data from oss offline collectors.
	is_cbtf_data = false;
    }

    if (rawdirs.size() == 0 ) {
	std::cerr << "RETURNING DUE TO NO RAW DATA" << std::endl;
	return -1;
    } else {
	// must be data from cbtf collectors.
	is_cbtf_data = true;
    }

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
                rawfile.find(datasuffix) != std::string::npos ||
		rawfile.find(infosuffix) != std::string::npos ||
		rawfile.find(dsosuffix) != std::string::npos ) {

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
	    if (rawfiles[i].find(infosuffix) != std::string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];
		struct stat stat_record;
		stat(rawname.c_str(), &stat_record);
		if (stat_record.st_size) {
		    infoList.insert(rawname);
		}
	    }

	    // create the master list of data files.
	    // update list of unique executables found.
	    if (rawfiles[i].find(datasuffix) != std::string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];

		std::string::size_type pos = rawfiles[i].find_first_of("-", 0);
		std::string temp = rawfiles[i].substr(0,pos);

#if 0
		std::ifstream dfile(rawname.c_str());
		if (dfile.peek() == std::ifstream::traits_type::eof()) {
		    std::cerr << "EMPTY DATA FILE for " << temp << std::endl;
		}
#endif

		
		struct stat stat_record;
		stat(rawname.c_str(), &stat_record);
		if (stat_record.st_size) {
		    executables_used.insert(temp);
		    dataList.insert(rawname);
		}
	    }

	    // create the master list of dsos files.
	    if (rawfiles[i].find(dsosuffix) != std::string::npos) {
		rawname = rawdatadir + "/" + rawfiles[i];
		struct stat stat_record;
		stat(rawname.c_str(), &stat_record);
		if (stat_record.st_size) {
		    dsosList.insert(rawname);
		}
	    }
	}

	rawfiles.clear();
	closedir(dpsub);
    }

    std::set<std::string>::iterator ssi,ssii, temp;
    for( ssi = executables_used.begin(); ssi != executables_used.end(); ++ssi) {
	std::cerr << "Processing raw data for " << (*ssi) << " ..." << std::endl;
        for( ssii = dataList.begin(); ssii != dataList.end(); ++ssii) {
	    // Need to base the test for existence on the basename
	    // of the dataList file.  Who knows what may be in the
	    // full path name that MAY match up.  e.g. the executable
	    // "wc" matched a directory named "hwctime".
	    std::string tname;
	    size_t pos = (*ssii).find_last_of("/");
	    if(pos != std::string::npos) {
		tname.assign((*ssii).begin() + pos + 1, (*ssii).end());
	    } else {
		tname = *ssii;
	    }

	    if( tname.find((*ssi)) != std::string::npos) {
		    rawfiles.push_back((*ssii));
	    }
	}


#if !defined(BUILD_CBTF)
        for( ssii = infoList.begin(); ssii != infoList.end(); ++ssii) {
	    //std::cerr << "INFO find " << *ssi << " in " << *ssii << std::endl;
	    if( (*ssii).find((*ssi)) != std::string::npos) {
		// restrict list to only those for which a matching
		// openss-data file exists.
		std::string::size_type pos = (*ssii).find(".openss-info", 0);
		std::string ts = (*ssii).substr(0,pos) + ".openss-data";
		for(temp = dataList.begin(); temp != dataList.end();
							++temp) {
		    if ((*temp).find(ts) != std::string::npos) {
			rawfiles.push_back((*ssii));
			// remove it from master list now that it will
			// be processed.
		        //infoList.erase(ssii);
		    }
		}
	    }
	}

        for( ssii = dsosList.begin(); ssii != dsosList.end(); ++ssii) {
	    //std::cerr << "DSOS find " << *ssi << " in " << *ssii << std::endl;
	    if( (*ssii).find((*ssi)) != std::string::npos) {
		// restrict list to only those for which a matching
		// openss-data file exists.
		std::string::size_type pos = (*ssii).find(".openss-dsos", 0);
		std::string ts = (*ssii).substr(0,pos) + ".openss-data";
		for(temp = dataList.begin(); temp != dataList.end();
							++temp) {
		    if ((*temp).find(ts) != std::string::npos) {
			rawfiles.push_back((*ssii));
			// remove both from master list now that it will
			// be processed.
		        //dsosList.erase(ssii);
		        dataList.erase(temp);
		    }
		}
	    }
	}
#endif // !BUILD_CBTF

        convertToOpenSSDB();
        createOfflineSymbolTable();
	finalizeDB();
	rawfiles.clear();
	threads_processed.clear();
    }

    if (dp) {
	free(dp);
    }
    return 0;
}

void OfflineExperiment::findUniqueAddresses()
{
    // Find current threads used in this experiment.
    ThreadGroup threads;
    ThreadGroup original = theExperiment->getThreads();
    // Find any new threads.
    std::insert_iterator< ThreadGroup > ii( threads, threads.begin() );
    std::set_difference(original.begin(), original.end(),
                        threads_processed.begin(), threads_processed.end(),
                        ii);

#ifndef NDEBUG
    if(is_debug_offlinesymbols_enabled) {
        std::cerr << "OfflineExperiment::findUniqueAddresses have total "
        << original.size() << " threads" << std::endl;

        for(ThreadGroup::const_iterator ni = threads.begin();
                                    ni != threads.end(); ++ni) {
            std::cerr << "OfflineExperiment::findUniqueAddresses "
            << " NEW THREAD: " << EntrySpy(*ni).getEntry() << std::endl;

        }
    }
#endif

    // add any new threads to threads_processed group.
    // the new threads will be processed below.
    std::insert_iterator< ThreadGroup > iu( threads_processed,
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

    CollectorGroup cgrp = theExperiment->getCollectors();
    CollectorGroup::iterator ci = cgrp.begin();
    Collector c = *ci;
    Metadata m = c.getMetadata();

    std::map<AddressRange, std::set<LinkedObject> > tneeded;

    for(ThreadGroup::const_iterator i = threads.begin();
                                    i != threads.end(); ++i) {
	Instrumentor::retain(*i);

	// add performance sample addresses to address buffer
	// for this thread group.
	c.getUniquePCValues(*i,eg,unique_addresses);
    }

#ifndef NDEBUG
    if(is_debug_offlinesymbols_enabled) {
	std::cerr << "findUniqueAddresses: total unique addresses is " << unique_addresses.size() << std::endl;
    }
#endif
}

int OfflineExperiment::convertToOpenSSDB()
{
    std::string rawname;

    // process offline info blobs first.
    std::cerr << "Processing processes and threads ..." << std::endl;
#if !defined(BUILD_CBTF)
	    // cbtf offline collections does not drop openss-info files
    for (unsigned int i = 0;i < rawfiles.size();++i) {
	bool_t found_infofile = false;
	if (rawfiles[i].find(".openss-info") != std::string::npos) {
	    //std::cerr << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_infofile = true;
	} 

        if (found_infofile) {
            bool_t rval;
	    rval = process_expinfo(rawname);
	    
            if (!rval) {
    	        std::cerr << "Could not process experiment info for: "
		    << rawname << std::endl;
            }

            // add this pid and host to database.
            theExperiment->updateThreads(expPid,expPosixTid,expRank,expHost);
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
		if (expCollector == "hwcsamp") {
			o_param.pushVal((char*)expEvent.c_str());
		} else if (expCollector == "hwc") {
			o_param.pushVal((char*)expEvent.c_str());
		} else if (expCollector == "hwctime") {
			o_param.pushVal((char*)expEvent.c_str());
		} else {
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
	    }

	    (void) setparam(c, param_name, value_list);
	}
    }
#endif // !BUILD_CBTF

    // Process data first so we can find the unique pc values
    // found in this experiments performance date.  These pc values
    // will be used to restrict the symbol lookup to just those
    // linked objects, addressspaces, files, functions, statements
    // where performance data was collected.

    std::cerr << "Processing performance data ..." << std::endl;
    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_datafile = false;
	if (rawfiles[i].find(".openss-data") != std::string::npos) {
	    //std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_datafile = true;
	} 

        if (found_datafile) {
          bool_t rval = process_data(rawname);
#if defined(BUILD_CBTF)
            if (!rval && rawfiles[i].find(".openss-dsos") != std::string::npos) {
// DEBUG
#ifndef NDEBUG
		if(is_debug_offline_enabled) {
	            std::cerr << "Could not process experiment data for: "
		        << rawname << std::endl;
		}
#endif
	    }
#else // BUILD_CBTF
            if (!rval &&
		(rawfiles[i].find(".openss-info") != std::string::npos ||
		 rawfiles[i].find(".openss-dsos") != std::string::npos)) {
// DEBUG
#ifndef NDEBUG
		if(is_debug_offline_enabled) {
	            std::cerr << "Could not process experiment data for: "
		        << rawname << std::endl;
		}
#endif
	    }
#endif // BUILD_CBTF
        }
    }

#if defined(BUILD_CBTF)
    DataQueues::flushPerformanceData();
#else
    theExperiment->flushPerformanceData();
#endif

    // create unique addresses here. 51013.
    // this allows us to exclude unneeded entries in DsoVec.
    findUniqueAddresses();
 
    // Process the list of dsos and address ranges for this experiment.
    std::cerr << "Processing symbols ..." << std::endl;
    for (unsigned int i = 0;i < rawfiles.size();i++) {
	bool_t found_dsofile = false;
#if defined(BUILD_CBTF)
	if (rawfiles[i].find(".openss-data") != std::string::npos) {
#else
	if (rawfiles[i].find(".openss-dsos") != std::string::npos) {
#endif
	    //std::cout << "processing " << rawfiles[i] << std::endl;
	    rawname = rawfiles[i];
	    found_dsofile = true;
	} 

        if (found_dsofile) {
            bool_t rval;
#if defined(BUILD_CBTF)
	    rval = process_cbtf_objects(rawname);
#else
	    rval = process_objects(rawname);
#endif
            if (!rval) {
	        std::cerr << "Could not process experiment dsos for: "
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
		  << "mpi rank:       " << info.rank << std::endl
		  << "info.event:     " << expEvent << std::endl
		  << "info.traced:     " << expTraced << std::endl;

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

    while (1) {
	unsigned int blobsize;
	if (!xdr_u_int(&xdrs, &blobsize)) {
	    //  no more entries. done.
	    break;
	}

	// NOTE: At large scales (1024 pe) usertime data blobs can
	// be written to the database of size 0 or 1. These are not
	// valid and will cause an assert in the DataQueue.
	// Problem found with smg2000 -n 40 40 40 using intel compilers
	// on a 128 node parition. Why did such a small blob
	// get written to the openss-data file?
	// A typical datablob header is greater than 1024.
	if (blobsize < 1024 ) {
	    std::cerr << "[ossutil] Warning: Data blob possibly corrupt from " << rawfilename
		<< ". Unexpected blobsize is " << blobsize << std::endl;
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
	    std::cerr << "[ossutil] Warning: Bad read of data blob from " << rawfilename
		<< " expected: " << blobsize << " got:" << bytesRead << std::endl;
	    if (theData) free(theData);
	    continue;
	}

	// For offline collection we only maintain one dataqueue.
	// So the collector runtimes need to set the header.experiment to 0.
	// This is the first index into the DataQueue.
	Blob datablob(blobsize, theData);

#if defined(BUILD_CBTF)
	// The openss-data file for cbtf-krell based offline collectors
	// writes the performance data blobs into the raw data file
	// first and then writes the linkedobject group blob as the
	// last entry.  Therefore there could be multiple performance
	// data blobs to enque into the DataQueue before we see the
	// linkedobject group blob.
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = datablob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

	// The CBTF_DataHeader contains more than thread info and
	// includes this "id" entry which is the collector name.
	// This works to identify the performance data blobs.
	// It is assumed that any remaining blobs (typically 1) are
	// the linked object group blobs and are stored into a
	// vector of blobs that represent all the linked object groups
	// recorded for each thread of execution.
	if (std::string(header.id) != "") {
	    // This is a performance data blob.  Use its header to
	    // update the thread table.  This eliminates the old
	    // .openss-info files that performed this task before
	    // (in addition to updating the parameters which is
	    // no handled by the osscollect client).
	    expPid = header.pid;
	    expRank = header.rank;
	    expPosixTid = header.posix_tid;
	    expHost = header.host;
	    expOmpTid = header.omp_tid;
#ifndef NDEBUG
	    if(is_debug_offline_enabled) {
	        std::cerr << "OfflineExperiment::process_data updates threads for "
		<< expHost << ":" << expPid << ":" << expRank << ":"
		<< expPosixTid << ":" << expOmpTid << std::endl;
	        std::cerr << "OfflineExperiment::process_data enquing datablob size:"
		<< blobsize << " bytesRead:" << bytesRead << " from " << rawfilename
		<< std::endl;
	    }
#endif
	    theExperiment->updateThreads(expPid,expPosixTid,expOmpTid,expRank,expHost);
	    DataQueues::enqueuePerformanceData(datablob);
	} else {
	    // There will typically be one linkedobject group blob per thread.
	    // We will store these into a vector of blobs and process them
	    // later via process_cbtf_objects.
	    cbtf_objs_blobs.push_back(datablob);
	}
#else
	// The openspeedshop collector runtime generated performance data blobs are queued here.
#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	    std::cerr << "OfflineExperiment::process_data enquing datablob size:"
		<< blobsize << " bytesRead:" << bytesRead
		<< " from " << rawfilename << std::endl;
	}
#endif
	DataQueues::enqueuePerformanceData(datablob);
#endif
	if (theData) free(theData);
	done = true;

    } // while

    fclose(f);
    // experimental code to remove raw openss-data file
    // once it is copied to the openss database.
    //std::remove(rawfilename.c_str());
    return done;
}

// The processing of objects for the OSS offline files generated
// by the OSS collector runtimes.
bool OfflineExperiment::process_objects(const std::string rawfilename)
{

    if (unique_addresses.size() == 0) {
	std::cerr << "Warning: No samples recorded in this experiment!" << std::endl;
	return false;
    }

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
	    << " OBJECT header TIME interval: begin "
	    << TimeInterval(Time(objsheader.time_begin), Time(objsheader.time_end))
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


		// The code below is limiting entries into the AddressSpace
		// table of the database to linkedobjects for which a sample
		// address is present and the linkedobject time was long enough
		// to sample and address or event.
		//
		// range is the address range of this linkedobject.
		// itlow and itup are iterators to the unique address set
		// that are used see if an sample address is in the range
		// of the linked object. If so, the linked object is included
		// in the database.
		AddressRange range(Address(objs.objs.objs_val[i].addr_begin),
				   Address(objs.objs.objs_val[i].addr_end)) ;
		//std::cerr << "OBJNAME: " << objname << " range " << range << std::endl;
		std::set<Address>::iterator it,itlow,itup;
		itlow = unique_addresses.lower_bound (range.getBegin()); itlow--;
		itup = unique_addresses.upper_bound (range.getEnd()); itup--;
		if ( !(range.doesContain(*itlow) || range.doesContain(*itup)) ) {
		    //std::cerr << "range " << range << " DOES NOT contain " << *itlow << " Or " << *itup << std::endl;
		    continue;
		}

		// In some codes like openss itself, a plugin table is created by opening
		// and closing a dso very quickly.  ie. no samples are taken.  This leads
		// to overlapping addresspace entries, multiple addressspace entries where
		// no work was recorded but we need to instersect with in metric views, and
		// in general, polluting the addresspace table wih entries.  This prevents
		// recording such entries.
		TimeInterval time_interval(Time(objs.objs.objs_val[i].time_begin),
					   Time(objs.objs.objs_val[i].time_end));
		uint64_t ttime = objs.objs.objs_val[i].time_end - objs.objs.objs_val[i].time_begin;
		if (ttime < 1000000) {
		    std::cerr << "Time interval to small for meaningful sample " << ttime << std::endl;
		    continue;
		}

// DEBUG
#ifndef NDEBUG
		if(is_debug_offline_enabled) {
		  std::cerr << std::endl;
		  std::cerr << "DSONAME " << objname << std::endl;
		  std::cerr << "ADDR " << range << std::endl;
		  std::cerr << "TIME " << time_interval << std::endl;
		  std::cerr << "DLOPEN " << (int) objs.objs.objs_val[i].is_open  << std::endl;
		  std::cerr << std::endl;
		}
#endif

		if (objname.find("[vdso]") == std::string::npos &&
		    objname.find("[vsyscall]") == std::string::npos &&
		    objname.find("[stack]") == std::string::npos &&
		    objname.find("[heap]") == std::string::npos &&
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


#if defined(BUILD_CBTF)
// The cbtf-krell collector generated offline data consists of one file
// name with the extention .openss-data.  The last entry in the file
// is essentially the addressspace of the thread in question.
bool OfflineExperiment::process_cbtf_objects(const std::string rawfilename)
{

    // If the datablobs did not contain any valid sample data then
    // return early.
    if (unique_addresses.size() == 0) {
	std::cerr << "Warning: No samples recorded in this experiment!" << std::endl;
	return false;
    }

    // This handles a group of linkedobjects loaded into a thread.
    // cbtf uses: CBTF_Protocol_LinkedObjectGroup which is an
    // arrary of CBTF_Protocol_LinkedObject. This matches offline_objects
    // except address begin,end is a a range.
    // TODO: Currently using CBTF_Protocol_Offline_LinkedObjectGroup.
    // This could be changed to use the same group as cbtf. That requires
    // changes in the cbtf-krell collector code.
    for(std::vector<Blob>::const_iterator i = cbtf_objs_blobs.begin();
					  i != cbtf_objs_blobs.end(); ++i) {
	CBTF_EventHeader objsheader;
	memset(&objsheader, 0, sizeof(objsheader));
	unsigned objsheader_size = (*i).getXDRDecoding(
	    reinterpret_cast<xdrproc_t>(xdr_CBTF_EventHeader), &objsheader);
	unsigned objs_size = (*i).getSize() - objsheader_size;
	const void* objs_ptr =
	    &(reinterpret_cast<const char*>((*i).getContents())[objsheader_size]);


	Blob objsblob(objs_size,objs_ptr);
	CBTF_Protocol_Offline_LinkedObjectGroup objs;
	memset(&objs, 0, sizeof(objs));
	objsblob.getXDRDecoding(
	    reinterpret_cast<xdrproc_t>(xdr_CBTF_Protocol_Offline_LinkedObjectGroup),
	    &objs);

#ifndef NDEBUG
	if(is_debug_offline_enabled) {
	    std::cerr << "OfflineExperiment::process_cbtf_objects: objsheader size:"
	    << objsheader_size << " objs_size:" << objs_size
	    << " objs length:" << objs.linkedobjects.linkedobjects_len
	    << " from " << rawfilename << std::endl;
	}
#endif

	if (objs.linkedobjects.linkedobjects_len > 0) {
	    for(int i = 0; i < objs.linkedobjects.linkedobjects_len; i++) {

	        std::string objname = objs.linkedobjects.linkedobjects_val[i].objname;
		if (objname.empty()) {
		    continue;
		}


		// The code below is limiting entries into the AddressSpace
		// table of the database to linkedobjects for which a sample
		// address is present and the linkedobject time was long enough
		// to sample and address or event.
		//
		// range is the address range of this linkedobject.
		// itlow and itup are iterators to the unique address set
		// that are used see if an sample address is in the range
		// of the linked object. If so, the linked object is included
		// in the database.
		AddressRange range(objs.linkedobjects.linkedobjects_val[i].addr_begin,
				   objs.linkedobjects.linkedobjects_val[i].addr_end);

		//std::cerr << "OBJNAME: " << objname << " range " << range << std::endl;
		std::set<Address>::iterator it,itlow,itup;
		itlow = unique_addresses.lower_bound (range.getBegin()); itlow--;
		itup = unique_addresses.upper_bound (range.getEnd()); itup--;
		if ( !(range.doesContain(*itlow) || range.doesContain(*itup)) ) {
		    //std::cerr << "range " << range << " DOES NOT contain " << *itlow << " Or " << *itup << std::endl;
		    continue;
		}

		// In some codes like openss itself, a plugin table is created by opening
		// and closing a dso very quickly.  ie. no samples are taken.  This leads
		// to overlapping addresspace entries, multiple addressspace entries where
		// no work was recorded but we need to instersect with in metric views, and
		// in general, polluting the addresspace table wih entries.  This prevents
		// recording such entries.
		TimeInterval time_interval(Time(objs.linkedobjects.linkedobjects_val[i].time_begin),
					   Time(objs.linkedobjects.linkedobjects_val[i].time_end));
		uint64_t ttime = objs.linkedobjects.linkedobjects_val[i].time_end - objs.linkedobjects.linkedobjects_val[i].time_begin;
		if (ttime < 1000000) {
		    std::cerr << "Time interval to small for meaningful sample " << ttime << std::endl;
		    continue;
		}

// DEBUG
#ifndef NDEBUG
		if(is_debug_offline_enabled) {
		  std::cerr << std::endl;
		  std::cerr << "DSONAME " << objname << std::endl;
		  std::cerr << "ADDR " << range << std::endl;
		  std::cerr << "TIME " << time_interval << std::endl;
		  std::cerr << "DLOPEN " << (int) objs.linkedobjects.linkedobjects_val[i].is_open  << std::endl;
		  std::cerr << std::endl;
		}
#endif

		if(objs.linkedobjects.linkedobjects_val[i].is_executable) {
		    expExecutableName.insert(objname);
		}

		if (objname.find("[vdso]") == std::string::npos &&
		    objname.find("[vsyscall]") == std::string::npos &&
		    objname.find("[stack]") == std::string::npos &&
		    objname.find("[heap]") == std::string::npos &&
		    objname.compare("unknown") != 0 ) {

		    DsoEntry e(objname,objsheader.host, objsheader.pid,
				objsheader.posix_tid, range,time_interval,
				objs.linkedobjects.linkedobjects_val[i].is_open);
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

	    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_Protocol_Offline_LinkedObjectGroup),
		      reinterpret_cast<char*>(&objs));
	}
    } // for linkedobject in group.
    return true;
}
#endif // defined BUILD_CBTF

/**
 * Create the symbol table for the experiment database.
 * Currently bfd based...
 */
void OfflineExperiment::createOfflineSymbolTable()
{
    SymbolTableMap symtabmap;

    // Find current threads used in this experiment.
    ThreadGroup threads = threads_processed;

    if (threads.size() == 0) {
	return;
    }

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

    std::map<AddressRange, std::set<LinkedObject> > tneeded;

    for(ThreadGroup::const_iterator i = threads.begin();
                                    i != threads.end(); ++i) {
	AddressSpace taddress_space;
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

// DEBUG
#ifndef NDEBUG
		if(is_debug_offlinesymbols_enabled) {
	            std::cerr << "OfflineExperiment::createOfflineSymbolTable "
		    << "pid: " <<  d->dso_pid
	            << " addressspace setValue for " <<  d->dso_path
	            << ":" << d->dso_range << ":" << d->dso_time << std::endl;
		}
#endif
		taddress_space.setValue(d->dso_range, d->dso_path,
					/*is_executable*/ is_exe);
	    }
	} // end for dsoVec

	// The AddressSpace class now supports an update specific
	// to the offline method of creating the AddressSpace table
	// in the database.  
	tneeded = taddress_space.updateThread(*i);
    } // end for threads

    std::set<LinkedObject> ttgrp_lo = threads.getLinkedObjects();

    std::set<Address>::const_iterator aci;
#if defined(OPENSS_USE_SYMTABAPI)
    // SYMTABPAI_SYMBOLS
    std::set<std::string> linkedobjs;
    for(std::set<LinkedObject>::const_iterator li = ttgrp_lo.begin();
        li != ttgrp_lo.end(); ++li) {
	std::set<AddressRange> addr_range(li->getAddressRange());
	for(std::set<AddressRange>::const_iterator ar = addr_range.begin();
	    ar != addr_range.end(); ++ar) {
	    std::set<Address>::iterator itlow,itup;
	    itlow = unique_addresses.lower_bound (ar->getBegin()); --itlow;
	    itup = unique_addresses.upper_bound (ar->getEnd()); --itup;
	    if (ar->doesContain(*itlow) || ar->doesContain(*itup)) {
		    std::set<LinkedObject> stlo;
		    std::pair<std::set<std::string>::iterator,bool> ret = linkedobjs.insert((*li).getPath());
		    if (ret.second) {
// DEBUG
#ifndef NDEBUG
			if(is_debug_offlinesymbols_enabled) {
			    std::cerr << "OfflineExperiment::createOfflineSymbolTable "
				<< " insert  symtab for " << (*li).getPath()
				<< std::endl;
			}
#endif
		        stlo.insert(*li);
		        symtabmap.insert(std::make_pair(*ar,
				     std::make_pair(SymbolTable(*ar), stlo)
				    ));
		    }
	    }
	}
    }
    // SYMTABPAI_SYMBOLS
#else
    // BFD SYMBOLS
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
    // BFD_SYMBOLS
#endif


    for(std::set<LinkedObject>::const_iterator j = ttgrp_lo.begin();
					       j != ttgrp_lo.end(); ++j) {
	LinkedObject lo = (*j);
	std::cerr << "Resolving symbols for " << lo.getPath() << std::endl;
#if defined(OPENSS_USE_SYMTABAPI)
    // Note that DyninstSymbols::getLoops() must be called before calling
    // SymtabAPISymbols::getSymbols(). This is because getLoops() updates
    // unique_addresses with the head address of every loop so that later
    // getSymbols() will get the statement containing these addresses.
#if defined(HAVE_DYNINST)
	//std::cerr << Time::Now() << " Resolve loop addresses" << std::endl;
	DyninstSymbols::getLoops(unique_addresses, lo, symtabmap);
	//std::cerr << Time::Now() << " Done with loops addresses" << std::endl;
#endif
	//std::cerr << Time::Now() << " Resolve symtabapi symbols" << std::endl;
	stapi_symbols.getSymbols(unique_addresses,lo,symtabmap);
	//std::cerr << Time::Now() << " Done with symbols" << std::endl;
#else
	// If this is ever reused, it needs to use unique_addresses...
	//std::cerr << Time::Now() << " Resolve bfd symbols" << std::endl;
	bfd_symbols.getSymbols(unique_addresses,lo,symtabmap);
	//std::cerr << Time::Now() << " Done with symbols" << std::endl;
#endif

    } // end for threads linkedobjects

    std::cerr << "Updating database with symbols ... " << std::endl;

    // Now update the database with all our functions and statements...
    std::map<AddressRange, std::string> allfuncs; // used to verify symbols.
    for(SymbolTableMap::iterator i = symtabmap.begin();
		i != symtabmap.end(); ++i) {


	// This records any previously resolved function symbols.
	// We will use this later to remove them from the corresponding
	// symboltable since we do not want processAndStore to record
	// them into the database more than once.
	std::map<AddressRange, std::string> existingfunctions;

	SymbolTable st = i->second.first;
	std::map<AddressRange, std::string> stfuncs = st.getFunctions();
	for (std::map<AddressRange,
	     std::string>::iterator kk = stfuncs.begin();
	     kk != stfuncs.end(); ++kk) {
	    // allfuncs records already processed function symbols.
	    // look for the function there first.
	    std::map<AddressRange, std::string>::iterator it = allfuncs.find(kk->first);

	    if (it == allfuncs.end()) {
		// We have not seen this function before. so this function
		// will be processed for storage into the database.
		allfuncs.insert(*kk);
	    }  else {
		// need to remove this function from this symtabs dm_functions
		// before calling processandStore.
		existingfunctions.insert(*kk);
	    }
	}

	// remove any functions for which we have added to the database.
	i->second.first.removeFunctions(existingfunctions);

        for(std::set<LinkedObject>::const_iterator j = i->second.second.begin();
		j != i->second.second.end(); ++j) {
	    i->second.first.processAndStore(*j);
        }

    }

    // clear names to range for our next linked object.
    dsoVec.clear();
    std::cerr << "Finished ... " << std::endl;

#ifndef NDEBUG
    // Show the raw symbol information in the database if requested
    if (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS_DATABASE") != NULL)
    {
        SmartPtr<Database> db(new Database(theExperiment->getName()));

        BEGIN_TRANSACTION(db);
        db->prepareStatement(
            "SELECT LinkedObjects.id, "
            "       Files.path "
            "FROM LinkedObjects "
            "JOIN Files ON Files.id = LinkedObjects.file;"
            );
        std::cout << std::endl;
        while (db->executeStatement())
        {
            int id = db->getResultAsInteger(1);
            Path path = Path(db->getResultAsString(2));
            
            std::cout << std::setw(3) << std::setfill('0') << id << "  "
                      << path << std::endl;
        }
        END_TRANSACTION(db);

        BEGIN_TRANSACTION(db);
        db->prepareStatement(
            "SELECT Functions.id, "
            "       Functions.linked_object, "
            "       Functions.name, "
            "       FunctionRanges.addr_begin, "
            "       FunctionRanges.addr_end, "
            "       FunctionRanges.valid_bitmap "
            "FROM Functions "
            "JOIN FunctionRanges ON FunctionRanges.function = Functions.id;"
            );
        std::cout << std::endl;
        while (db->executeStatement())
        {
            int id = db->getResultAsInteger(1);
            int linked_object = db->getResultAsInteger(2);
            std::string name = db->getResultAsString(3);
            
            std::set<AddressRange> ranges =
                AddressBitmap(AddressRange(db->getResultAsAddress(4),
                                           db->getResultAsAddress(5)),
                              db->getResultAsBlob(6)).
                getContiguousRanges(true);
            
            for (std::set<AddressRange>::const_iterator
                     r = ranges.begin(); r != ranges.end(); ++r)
            {
                std::cout << std::setw(3) << std::setfill('0') << id << "  "
                          << std::setw(3) << std::setfill('0') << linked_object
                          << "  " << *r << "  " << name << std::endl;
            }
        }
        END_TRANSACTION(db);

        BEGIN_TRANSACTION(db);
        db->prepareStatement(
            "SELECT Statements.id, "
            "       Statements.linked_object, "
            "       Files.path, "
            "       Statements.line, "
            "       StatementRanges.addr_begin, "
            "       StatementRanges.addr_end, "
            "       StatementRanges.valid_bitmap "
            "FROM Statements "
            "JOIN Files ON Files.id = Statements.file "
            "JOIN StatementRanges ON StatementRanges.statement = Statements.id;"
            );
        std::cout << std::endl;
        while (db->executeStatement())
        {
            int id = db->getResultAsInteger(1);
            int linked_object = db->getResultAsInteger(2);
            Path path = Path(db->getResultAsString(3));
            int line = db->getResultAsInteger(4);
            
            std::set<AddressRange> ranges =
                AddressBitmap(AddressRange(db->getResultAsAddress(5),
                                           db->getResultAsAddress(6)),
                              db->getResultAsBlob(7)).
                getContiguousRanges(true);
            
            for (std::set<AddressRange>::const_iterator
                     r = ranges.begin(); r != ranges.end(); ++r)
            {
                std::cout << std::setw(3) << std::setfill('0') << id << "  "
                          << std::setw(3) << std::setfill('0') << linked_object
                          << "  " << *r << "  " << path.getBaseName()
                          << ", " << line << std::endl;
            }
        }
        END_TRANSACTION(db);

        BEGIN_TRANSACTION(db);
        db->prepareStatement(
            "SELECT Loops.id, "
            "       Loops.linked_object, "
            "       Loops.addr_head, "
            "       LoopRanges.addr_begin, "
            "       LoopRanges.addr_end, "
            "       LoopRanges.valid_bitmap "
            "FROM Loops "
            "JOIN LoopRanges ON LoopRanges.loop = Loops.id;"
            );
        std::cout << std::endl;
        while (db->executeStatement())
        {
            int id = db->getResultAsInteger(1);
            int linked_object = db->getResultAsInteger(2);
            Address head = Address(db->getResultAsAddress(3));
            
            std::set<AddressRange> ranges =
                AddressBitmap(AddressRange(db->getResultAsAddress(4),
                                           db->getResultAsAddress(5)),
                              db->getResultAsBlob(6)).
                getContiguousRanges(true);
            
            for (std::set<AddressRange>::const_iterator
                     r = ranges.begin(); r != ranges.end(); ++r)
            {
                std::cout << std::setw(3) << std::setfill('0') << id << "  "
                          << std::setw(3) << std::setfill('0') << linked_object
                          << "  " << *r << "  " << head << std::endl;
            }
        }
        END_TRANSACTION(db);
    }

    // Show the loop information (per function) in the database if requested
    if (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS_LOOPS") != NULL)
    {
        ThreadGroup threads = theExperiment->getThreads();
        
        std::set<Function> functions = threads.getFunctions();
        for (std::set<Function>::const_iterator
                 f = functions.begin(); f != functions.end(); ++f)
        {
            std::cout << std::endl << "Function: " << f->getDemangledName()
                      << std::endl;
            
            std::set<Loop> loops = f->getLoops();
            for (std::set<Loop>::const_iterator
                     l = loops.begin(); l != loops.end(); ++l)
            {
                std::cout << "    Loop" << std::endl;
                
                std::set<Statement> definitions = l->getDefinitions();
                for (std::set<Statement>::const_iterator
                         s = definitions.begin(); s != definitions.end(); ++s)
                {
                    std::cout << "        Definition: "
                              << s->getPath().getBaseName() << ", Line #"
                              << s->getLine() << std::endl;
                } // s
                
                std::set<Statement> statements = l->getStatements();
                for (std::set<Statement>::const_iterator
                         s = statements.begin(); s != statements.end(); ++s)
                {
                    std::cout << "        Statement: "
                              << s->getPath().getBaseName() << ", Line #"
                              << s->getLine() << std::endl;
                } // s
                
            } // l
        } // f
    }
#endif
}



void OfflineExperiment::finalizeDB()
{

    SmartPtr<Database> database(new Database(theExperiment->getName()));

    // Now clean up any remaining linkedobjects, files, addressspaces,
    // and threads that do not contain any sample date, functions or statements.
    // Begin a multi-statement transaction
    BEGIN_WRITE_TRANSACTION(database);

    // delete any thread entries where no sample data was recorded.
    database->prepareStatement(
	"DELETE FROM Threads "
	"WHERE id NOT IN (SELECT DISTINCT thread FROM Data);"
	);
    while(database->executeStatement());

    // This code has issues with the symboltables passed from the cbtf
    // component network. Until those issues are  resolved, only allow
    // this cleanup of unneeded entries in the database for the symbols
    // resolved here in the the cbtf instrumentor FE.
#ifdef OPENSS_CBTF_FE_RESOLVE_SYMBOLS
    // delete any linkedobject entries where no functions or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM LinkedObjects "
	"WHERE id NOT IN (SELECT DISTINCT linked_object FROM Functions) "
	"  AND id NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(database->executeStatement());

    // delete any file entries where no linkedobjects or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM Files "
	"WHERE id NOT IN (SELECT DISTINCT file FROM LinkedObjects) "
	"  AND id NOT IN (SELECT DISTINCT file FROM Statements);"
	);
    while(database->executeStatement());

    // delete any addresspace entries where no linkedobjects or statements
    // where recorded.
    database->prepareStatement(
	"DELETE FROM AddressSpaces "
	"WHERE linked_object NOT IN (SELECT DISTINCT id FROM LinkedObjects) "
	"  AND linked_object NOT IN (SELECT DISTINCT linked_object FROM Statements);"
	);
    while(database->executeStatement());

    // delete any addresspace entries for threads where no data
    // was recorded.
    database->prepareStatement(
	"DELETE FROM AddressSpaces "
	"WHERE thread NOT IN (SELECT DISTINCT id FROM Threads);"
	);
    while(database->executeStatement());
#endif

    // TODO:  Could look at removing any thread entries for which there
    // are no performance data blobs in the data table.
    END_TRANSACTION(database);
}
