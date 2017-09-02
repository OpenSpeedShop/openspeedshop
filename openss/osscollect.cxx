////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2016 Krell Institute. All Rights Reserved.
// Copyright (c) 2015-2016 Argo Navis Technologies. All Rights Reserved.
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

/** @file simple collection tool frontend. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <errno.h>
#include <sys/wait.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include "ToolAPI.hxx"
#include "FEThread.hxx"
#include "OfflineExperiment.hxx"
#include "OfflineParameters.hxx"
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace KrellInstitute::Core;
using namespace OpenSpeedShop::Framework;

enum exe_class_types { MPI_exe_type, SEQ_RunAs_MPI_exe_type, SEQ_exe_type };

namespace {
    void suspend()
    {
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = 500 * 1000 * 1000;
        while(nanosleep(&wait, &wait));
    }

    bool is_debug_timing_enabled = (getenv("CBTF_TIME_CLIENT_EVENTS") != NULL);
    bool is_debug_client_enabled = (getenv("CBTF_DEBUG_CLIENT") != NULL);
    bool defer_view = (getenv("OPENSS_DEFER_VIEW") != NULL);
}

// Client Utilities.

// Function that returns the number of BE processes that are required for LW MRNet BEs.
// The function tokenizes the program command and searches for -np or -n.
static int getBEcountFromCommand(std::string command) {

    int retval = 1;

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);
    std::string S = "";
    std::string n_token = "-n";
    std::string np_token = "-np";

    bool found_be_count = false;

    BOOST_FOREACH (const std::string& t, btokens) {
	S = t;

	// This handles clients that allow both -n2 on the command line.
	// For openmpi, -n2 or -np2 is in fact rejected by the openmpi mpirun command.
	// If we find the -n case with no space, get the value just after -n and
	// return it as an int while terminating loop.
	std::string::size_type n_token_pos = S.find(n_token);
	if (S.find(np_token) == std::string::npos &&
	    n_token_pos != std::string::npos) {

	    std::string::size_type token_size = n_token.length();
	    if (S.substr( n_token_pos+token_size, std::string::npos ).size() > 0) {
	        retval = boost::lexical_cast<int>(S.substr( n_token_pos+token_size, std::string::npos ));
		break;
	    }
	}

	// This handles the cases where there is a space after the -n or -np.
	// In fact, openmpi's mpirun requires the space.
	if (found_be_count) {
	    S = t;
	    retval = boost::lexical_cast<int>(S);
	    break;
	} else if (!strcmp( S.c_str(), std::string("-np").c_str())) {
	    found_be_count = true;
	} else if (!strcmp(S.c_str(), std::string("-n").c_str())) {
	    found_be_count = true;
	}
    } // end foreach

    return retval;
}


// Below is the original code to find if an executable a MPI executable
// It is maintained here for a possible fallback and may be removed
// in the future.  The previous version (below) depends on Dyninst's
// symtabAPI interface, but it did not find libmpi.so in some applications,
// like simpleFoam from OpenFOAM.
// Dyninst sets the list of dependencies from the DT_NEEDED entries in the
// binary. Dyninst doen't follow transitive dependencies, which ldd does.
// If you run readelf -d, you should see NEEDED entries that correspond to
// the dependencies Dyninst finds; the Symtab model is (at present) to represent
// what's present in the binary, not anything that's contingent on where it runs.
//
#if 0
// 
// Determine if libmpi is present in this executable.
//
static bool isMpiExe(const std::string exe) {
    SymtabAPISymbols stapi_symbols;
    bool found_libmpi = stapi_symbols.foundLibrary(exe,"libmpi");
    return found_libmpi;
}
//
// Determine if openMP runtime library is present in this executable.
//
static bool isOpenMPExe(const std::string exe) {
    SymtabAPISymbols stapi_symbols;
    bool found_openmp = stapi_symbols.foundLibrary(exe,"libiomp5");
    if (!found_openmp) {
	found_openmp = stapi_symbols.foundLibrary(exe,"libgomp");
    }
    return found_openmp;
}
#endif

//
// This routine runs the ldd command on the executable represented by the passed in executable name
// and looks to see if the passed in library name (libname) is found in the output.
//
static bool foundLibraryFromLdd(const std::string& exename, const std::string& libname)
{
    // is there any chance that ldd is not installed or in the default path?
    std::string command = "ldd ";

    // create our ldd command string with passed libname.
    command.append(exename.c_str());

    // popen ldd so we can process output.
    // make sure popen succeeds.? error checking?
    FILE *lddOutFile = popen(command.c_str(), "r");

    // now find is libname is anywhere in ldd.  More precise find
    // would use strings like "/libmpi.so".  However, there are versions
    // of libmpi.so, like libmpi_dbg.so that we would miss.
    if (lddOutFile != NULL) {
	char buffer[BUFSIZ];
	memset(&buffer, 0, sizeof(buffer));

	while (fgets(buffer, sizeof(buffer), lddOutFile)) {
	    std::string line(buffer);

	    if (!line.empty()) {
		if (line.find(libname) != std::string::npos) {
		    //std::cerr << "FOUND " << libname << " line=" << line << std::endl;
		    return true;
		}
	    }
	}
	pclose(lddOutFile);
	return false;
    } else {
	std::cerr << "WARNING: ldd on " << exename << " failed. Looking for " << libname  << std::endl;
	return false;
    }
}

//
// Determine if libmpi is present in this executable.
// libmpi should appear as a substring "/libmpi.so" in ldd output.
// We would like use strings like "/libmpi.so" for the search. However,
// there are versions of libmpi.so, like libmpi_dbg.so that we would miss.
// So, we are using "/libmpi" instead.
//
static bool isMpiExe(const std::string exe) {
    bool found_libmpi = foundLibraryFromLdd(exe,"/libmpi");
    return found_libmpi;
}

//
// Determine if openMP runtime library is present in this executable.
//
static bool isOpenMPExe(const std::string exe) {
    SymtabAPISymbols stapi_symbols;
    bool found_openmp = foundLibraryFromLdd(exe,"/libiomp5");
    if (!found_openmp) {
        found_openmp = foundLibraryFromLdd(exe,"/libgomp");
    }
    return found_openmp;
}

//
// Determine what type of executable situation we have for running with cbtfrun.
// Is this a pure MPI executable or are we running a sequential executable with a mpi driver?
// We catagorize these into three types: mpi, seq runing under mpi driver, and sequential
//
static exe_class_types typeOfExecutable ( std::string program, const std::string exe ) {
    exe_class_types tmp_exe_type;
    if ( isMpiExe(exe) ) { 
	tmp_exe_type = MPI_exe_type;
    } else {
	if ( std::string::npos != program.find("aprun")) {
	    tmp_exe_type = SEQ_RunAs_MPI_exe_type;
	} else {
	    tmp_exe_type = SEQ_exe_type;
	}
    }
    return tmp_exe_type;
}

// Function that returns whether the filename is an executable file.
// Uses stat to obtain the mode of the filename and if it executable returns true.
static bool is_executable(std::string file)
{
    struct stat status_buffer;

    // Call stat with filename which will fill status_buffer
    if (stat(file.c_str(), &status_buffer) < 0)
	return false;

    // Examine for executable status
    if ((status_buffer.st_mode & S_IEXEC) != 0 && S_ISREG(status_buffer.st_mode))
	return true;

    return false;
}

// Function that returns the filename of the executable file found in the "command".
// It tokenizes the command and runs through it backwards looking for the first file that is executable.
// That might not be sufficient in all cases.
static std::string getMPIExecutableFromCommand(std::string command) {

    std::string retval = "";


    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);

    BOOST_FOREACH (const std::string& t, btokens) {
	if (is_executable( t )) {
	    exe_class_types local_exe_type = typeOfExecutable(command, t);
	    if (local_exe_type == MPI_exe_type || local_exe_type == SEQ_RunAs_MPI_exe_type ) {
		return t;
	    }
	}
    } // end foreach

    return retval;
}


// Function that returns the filename of the executable file found in the "command".
// It tokenizes the command and runs through it backwards looking for the first file that is executable.
// That might not be sufficient in all cases.
static std::string getSeqExecutableFromCommand(std::string command) {

    std::string retval = "";

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);

    BOOST_FOREACH (const std::string& t, btokens) {
	if (is_executable( t )) {
	    exe_class_types local_exe_type = typeOfExecutable(command, t);
	    if (local_exe_type == SEQ_exe_type ) {
		return t;
	    }
	}
    } // end foreach

    return retval;
}


static std::string createDBName(std::string dbprefix)
{
     std::string LocalDataFileName;

     char *database_directory = getenv("OPENSS_DB_DIR");
     char tmp_tName[1024];

     if (database_directory) {
	LocalDataFileName = database_directory;
     } else {
	// Was simply . for current directory. Not really a good choice
	// if other scripts or code wish to use the path to the database.
	//LocalDataFileName = ".";
	boost::filesystem::path full_path( boost::filesystem::current_path() );
	LocalDataFileName = full_path.c_str();
     }

     int cnt = 0;
     for (cnt = 0; cnt < 1000; cnt++) {
         snprintf(tmp_tName, 512, "%s/%s-%d.openss",LocalDataFileName.c_str(), dbprefix.c_str(), cnt);
         Assert(tmp_tName != NULL);

         int fd;
         if ((fd = open(tmp_tName, O_RDONLY)) != -1) {
          // File name is already used!
           Assert(close(fd) == 0);
           continue;
         }
         LocalDataFileName = std::string(tmp_tName);
         break;
     }

     return LocalDataFileName;
}


/**
 * Utility: setparam()
 * 
 * Taken from cli and modified for OfflineExperiment.
 * OfflineParamVal is defined in libopens-framework/OfflineParameters.hxx.
 * This is specific to collector experiment parameters as
 * expressed by environment variables. Therefore the only
 * param types in use here are char* (std::string) and int64_t (uint).
 * We need to check the passed values to ensure that if they
 * are not set, we do not change the parameter and leave
 * the defaults set by the collector. eg. no 0 sampling_rate
 * or empty event or traced list are allowed to be set as params.
 * @param   .
 * @return  bool
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
      bool found_name = false;
      for (std::map<std::string, bool>::iterator
                 im = Value.begin(); im != Value.end(); im++) {
         bool name_in_list = false;
        for (std::vector<OfflineParamVal>::iterator
                iv = value_list->begin(); iv != value_list->end(); iv++) {
          Assert (iv->getValType() == PARAM_VAL_STRING);
          if (!strcasecmp( im->first.c_str(), iv->getSVal() )) {
            name_in_list = true;
            found_name = true;
            break;
	  }
        }
        im->second = name_in_list;
      }
      if (found_name) {
        C.setParameterValue(pname,Value);
      }
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
	if (i64val > 0) {
            C.setParameterValue(pname,(int64_t)i64val);
	}
      } else if( m.isType(typeid(uint)) ) {
        uint uval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%d", &uval);
        } else {
          uval = (uint)(pvalue.getIVal());
        }
	if (uval != 0) {
           C.setParameterValue(pname,(uint)uval);
	}
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
	if (!sval.empty()) {
            C.setParameterValue(pname,(std::string)sval);
	}
      }
    }
    return true;
  }

  // We didn't find the named parameter in this collector.
  return false;
}

// Helper function to update the database values for parameters
// that may be changed at runtime by setting environment variables.
// The values used in the collector runtimes must match those in
// use here
static void updateCollectorParameters(Collector &c, std::string &cname)
{
    int expRate = 0;
    std::string expEvent,expTraced;

    if (cname == "pcsamp") {
	const char* sampling_rate = getenv("CBTF_PCSAMP_RATE");
	if (sampling_rate != NULL) {
	    expRate = atoi(sampling_rate);
	}
    } else if (cname == "usertime") {
	const char* sampling_rate = getenv("CBTF_USERTIME_RATE");
	if (sampling_rate != NULL) {
	    expRate = atoi(sampling_rate);
	}
    } else if (cname == "hwc") {
	const char* sampling_rate = getenv("CBTF_HWC_THRESHOLD");
	if (sampling_rate != NULL) {
	    expRate = atoi(sampling_rate);
	}
	char* event_param = getenv("CBTF_HWC_EVENT");
	if (event_param != NULL) {
	    expEvent=event_param;
	}
    } else if (cname == "hwctime") {
	const char* sampling_rate = getenv("CBTF_HWCTIME_THRESHOLD");
	if (sampling_rate != NULL) {
	    expRate = atoi(sampling_rate);
	}
	char* event_param = getenv("CBTF_HWCTIME_EVENT");
	if (event_param != NULL) {
	    expEvent=event_param;
	}
    } else if (cname == "hwcsamp"){
	const char* sampling_rate = getenv("CBTF_HWCSAMP_RATE");
	if (sampling_rate != NULL) {
	    expRate = atoi(sampling_rate);
	}
	char* event_param = getenv("CBTF_HWCSAMP_EVENTS");
	if (event_param != NULL) {
	    expEvent=event_param;
	}
    } else if (cname.compare(0,2,"io") == 0) {
	// cbtf-krell collectors use CBTF_IO_TRACED for all io collectors.
	// Therefore the ossdriver script must set CBTF_IO_TRACED for
	// io,iot,iop collection if the user overrides the defaults.
	char* traced_param = getenv("CBTF_IO_TRACED");
	if (traced_param != NULL) {
	    expTraced=traced_param;
	}
    } else if (cname.compare(0,3,"mpi") == 0) {
	// cbtf-krell collectors use CBTF_MPI_TRACED for all mpi collectors.
	// Therefore the ossdriver script must set CBTF_MPI_TRACED for
	// mpi,mpit,mpip collection if the user overrides the defaults.
	char* traced_param = getenv("CBTF_MPI_TRACED");
	if (traced_param != NULL) {
	    expTraced=traced_param;
	}
    } else if (cname == "mem"){
	char* traced_param = getenv("CBTF_MEM_TRACED");
	if (traced_param != NULL) {
	    expTraced=traced_param;
	}
    } else if (cname == "pthreads"){
	char* traced_param = getenv("CBTF_PTHREAD_TRACED");
	if (traced_param != NULL) {
	    expTraced=traced_param;
	}
    } else {
    }

    std::set<Metadata> md = c.getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
	Metadata mm = *mi;
	OfflineParameters o_param((char*)cname.c_str(), (char*)mm.getUniqueId().c_str());
	std::vector<OfflineParamVal> *value_list = o_param.getValList();


	// Parameter sampling_rate is an integer paramter used by pcsamp,hwcsamp and
	// usertime for timer rate and used by hwc and hwctime for threshold.
	// Parameter traced_functions is string of functions separated by either
	// a ":" or "," character and is used by the function tracing collectors.
	// Parameter event is a string representing a hardware counter name(s).
	// Multiple names if supported are delimited by a ":" or ","  character.
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
	    if (cname == "hwcsamp") {
		o_param.pushVal((char*)expEvent.c_str());
	    } else if (cname == "hwc") {
		o_param.pushVal((char*)expEvent.c_str());
	    } else if (cname == "hwctime") {
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

	(void) setparam(c, (char*)mm.getUniqueId().c_str(), value_list);
    }
}

int main(int argc, char** argv)
{
#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << OpenSpeedShop::Framework::Time::Now() << " osscollect client started." << std::endl;
    }
#endif

    unsigned int numBE;
    bool offline_mode = false;
    bool isMPI;
    std::string topology, arch, connections, collector, program, mpiexecutable,
		cbtfrunpath, seqexecutable;


    // create a default for topology file.
    char const* curr_dir = getenv("PWD");

    std::string cbtf_path(curr_dir);


    std::string default_topology(curr_dir);
    default_topology += "/cbtfAutoTopology";

    // create a default for connections file.
    std::string default_connections(curr_dir);
    default_connections += "/attachBE_connections";

    // create a default for the collection type.
    std::string default_collector("pcsamp");

    boost::program_options::options_description desc("osscollect options");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("numBE", boost::program_options::value<unsigned int>(&numBE)->default_value(1),
	    "Number of lightweight mrnet backends. Default is 1, For an mpi job, the number of ranks specified to the launcher will be used.")
        ("arch",
	    boost::program_options::value<std::string>(&arch)->default_value(""),
	    "automatic topology type defaults to a standard cluster.  These options are specific to a Cray or BlueGene. [cray | bluegene]")
        ("topology",
	    boost::program_options::value<std::string>(&topology)->default_value(""),
	    "By default the tool will create a topology for you.  Use this option to pass a path name to a valid mrnet topology file. (i.e. from mrnet_topgen). Use this options with care.")
        ("connections",
	    boost::program_options::value<std::string>(&connections)->default_value(default_connections),
	    "Path name to a valid backend connections file. The connections file is created by the mrnet backends based on the mrnet topology file. The default is sufficient for most cases.")
        ("collector",
	    boost::program_options::value<std::string>(&collector)->default_value(default_collector),
	    "Name of collector to use [pcsamp | usertime | hwc]. Default is pcsamp.")
        ("program",
	    boost::program_options::value<std::string>(&program)->default_value(""),
	    "Program to collect data from, Program with arguments needs double quotes.  If program is not specified this client will start the mrnet tree and wait for the user to manually attach backends in another window via cbtfrun.")
        ("cbtfrunpath",
            boost::program_options::value<std::string>(&cbtfrunpath)->default_value("cbtfrun"),
            "Path to cbtfrun to collect data from, If target is cray or bluegene, use this to point to the targeted client.")
        ("mpiexecutable",
	    boost::program_options::value<std::string>(&mpiexecutable)->default_value(""),
	    "Name of the mpi executable. This must match the name of the mpi exectuable used in the program argument and implies the collection is being done on an mpi job if it is set.")
        ("offline", boost::program_options::bool_switch()->default_value(false),
	    "Use offline mode. Default is false.")
        ;

    boost::program_options::variables_map vm;

    // handle any regular options
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);    

    // handle any positional options
    boost::program_options::positional_options_description p;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
				  options(desc).positional(p).run(), vm);
    boost::program_options::notify(vm);

    bool use_offline_mode = vm["offline"].as<bool>();

    // Generate the --mpiexecutable argument value if it is not set
    if (program != "" && mpiexecutable == "") {
	// Find out if there is an mpi driver to key off of
	// Then match the mpiexecutable value to the program name
	mpiexecutable = getMPIExecutableFromCommand(program);
	seqexecutable = getSeqExecutableFromCommand(program);
    }

    if (mpiexecutable == "" && seqexecutable == "") {
        std::cerr << "Could not find executable to run from the specified input run command:" << std::endl;
        std::cerr << program << std::endl;
        return 1;
    }

    if (mpiexecutable != "") {
	numBE = getBEcountFromCommand(program);
    }

    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    bool finished = false;
    std::string aprunLlist ="";

    if (numBE == 0) {
	std::cout << desc << std::endl;
	return 1;
    }

    // start with a fresh connections file.
    // FIXME: this likely would remove any connections file passed
    // on the command line. Should we allow that any more...
    bool connections_exists = boost::filesystem::exists(connections);
    if (connections_exists) {
	boost::filesystem::remove(connections);
    }

    // TODO: pass numBE to CBTFTopology and record as the number
    // of application processes.
    CBTFTopology cbtftopology;
    std::string fenodename;
    if (!use_offline_mode) {
     if (topology.empty()) {
      if (arch == "cray") {
          cbtftopology.autoCreateTopology(BE_CRAY_ATTACH,numBE);
      } else {
          cbtftopology.autoCreateTopology(BE_ATTACH,numBE);
      }
      topology = cbtftopology.getTopologyFileName();
      fenodename =  cbtftopology.getFENodeStr();
      std::cerr << "Generated topology file: " << topology << std::endl;
     } else {
      fenodename =  "localhost";
     }
    }

    // OpenSpeedShop client specific.
    // find name of application and strip any path.
    OpenSpeedShop::Framework::Path prg(mpiexecutable);
    if (prg.empty()) {
	prg += program;
    }
    prg = prg.getBaseName();

    // application may have arguments. remove them.
    std::vector<std::string> strs;
    boost::split(strs, prg, boost::is_any_of("\t "));
    prg = strs[0];

    // create a database prefix based on application name and collector.
    std::string dbprefix(prg);
    dbprefix += "-";
    dbprefix += collector;
    // get a dbname that does not conflict with an existing dbname.
    std::string dbname = createDBName(dbprefix);

    // create the actual database and create an experiment object.
    OpenSpeedShop::Framework::Experiment::create (dbname);
    OpenSpeedShop::Framework::Experiment *FW_Experiment = new OpenSpeedShop::Framework::Experiment (dbname);

    // setup the experiment to run with cbtf.
    FW_Experiment->setBEprocCount( numBE );
    FW_Experiment->setInstrumentorUsesCBTF( false );

    Collector FWcollector = FW_Experiment->createCollector(collector);

    ThreadGroup tg = FW_Experiment->createProcess(program, fenodename, numBE,
                                                     OutputCallback((void (*)(const char*, const int&, void*))NULL,(void *)NULL),
                                                     OutputCallback((void (*)(const char*, const int&, void*))NULL,(void *)NULL)   );

    updateCollectorParameters(FWcollector, collector);

    FEThread fethread;

    // From this point on we run the application with cbtf and specified collector.
    // verify valid numBE.
    if (numBE == 0 && !use_offline_mode) {
	std::cout << desc << std::endl;
	return 1;
    } else if (!use_offline_mode && program == "" && numBE > 0) {
	// this allows us to start the mrnet client FE
	// and then start the program with collector in
	// a separate window using cbtfrun.
	fethread.start(topology,connections,collector,numBE,finished);
	std::cout << "Running Frontend for " << collector << " collector."
	    << "\nNumber of mrnet backends: "  << numBE
	    << "\nTopology file used: " << topology << std::endl;
	std::cout << "Start mrnet backends now..." << std::endl;
	// ctrl-c to exit.  need cbtfdemo to notify us when all threads
	// have finised.
	while(true);
	fethread.join();
	exit(0);
    } else {
	if (use_offline_mode) {
	    std::cout << "Running offline " << collector << " collector."
	    << "\nProgram: " << program << std::endl;
	} else {
	    std::cout << "Running " << collector << " collector."
	    << "\nProgram: " << program
	    << "\nNumber of mrnet backends: "  << numBE
            << "\nTopology file used: " << topology << std::endl;

	    // TODO: need to cleanly terminate mrnet.
	    fethread.start(topology,connections,collector,numBE,finished);

	    // sleep was not sufficient to ensure we have a connections file
	    // written by the fethread.  Without the connections file the
	    // ltwt mrnet BE's cannot connect to the netowrk.
	    // Wait for the connections file to be written before proceeding
	    // to stat the mpi job and allowing the ltwt BEs to connect to
	    // the component network instantiated by the fethread.
	    bool connections_written = boost::filesystem::exists(connections);
	    while (!connections_written) {
		connections_written = boost::filesystem::exists(connections);
	    }

	}

        bool exe_has_openmp = false;

	if (!mpiexecutable.empty()) {
	    size_t pos;
	    if (cbtftopology.getIsCray()) {
		if (std::string::npos != program.find("aprun")) {
		    // Add in the -L list of nodes if aprun is present 
		    // and we are not co-locating
		    std::list<std::string> nodes = cbtftopology.getAppNodeList();
		    std::string appNodesForAprun = "-L " + cbtftopology.createRangeCSVstring(nodes) + " "; 
		    pos = program.find("aprun ") + 6;
		    program.insert(pos, appNodesForAprun);
		}
	    }

	    exe_has_openmp = isOpenMPExe(mpiexecutable);
	    exe_class_types appl_type =  typeOfExecutable(program, mpiexecutable);

	    // build the needed options for cbtfrun.
	    std::string cbtfrun_opts;
 
	    // is this an MPI program?
	    if (appl_type == MPI_exe_type) {
	        cbtfrun_opts.append(" --mpi ");
	    }

	    // does the program use OpenMP?
	    if (exe_has_openmp) {
		cbtfrun_opts.append(" --openmp");
	    }

	    // Does the user wish to run the offline version?
	    if (use_offline_mode) {
	        cbtfrun_opts.append(" --fileio ");
	    } else {
	        cbtfrun_opts.append(" --mrnet ");
	    }

	    // add the collector as last option.
	    cbtfrun_opts.append(" -c " + collector);
	    cbtfrun_opts.append(" ");

	    // now insert the cbtfrun command and it's options before the
	    // mpi executable.
	    pos = program.find(mpiexecutable);
	    program.insert(pos, " " + cbtfrunpath + " " + cbtfrun_opts);
	    
	    std::cout << "executing mpi program: " << program << std::endl;

	    ::system(program.c_str());

	} else {

	    seqexecutable = getSeqExecutableFromCommand(program);
	    exe_has_openmp = isOpenMPExe(seqexecutable);

	    std::string cmdtorun;
	    cmdtorun.append(cbtfrunpath + " -c " + collector);

	    // Does the user wish to run the offline version?
	    if (use_offline_mode) {
	        cmdtorun.append(" --fileio ");
	    } else {
	        cmdtorun.append(" --mrnet ");
	    }

	    if (exe_has_openmp) {
		cmdtorun.append(" --openmp ");
	    } else {
		cmdtorun.append(" ");

	    } 


	    cmdtorun.append(program);
	    std::cerr << "executing sequential program: " << cmdtorun << std::endl;
	    ::system(cmdtorun.c_str());
	}

	if (!use_offline_mode) {
	    fethread.join();
	}
    }

    if (use_offline_mode) {

	// NOTE: Due to documentation concerns it is advisable to
	// continue using the published environment variable name
	// (OPENSS_RAWDATA_DIR) for setting the location of rawdata
	// files written by cbtf-krell collectors. The ossdriver
	// script is responsible for handling the creation of this
	// directory and always appends "/offline-cbtf" for cbtf-krell
	// collection.  Additionaly the cbtf-krell collection code
	// internally uses CBTF_RAWDATA_DIR. The ossdriver script also
	// sets that for the benefit of cbtf-krell collectors.
	// The default is always /tmp/$USER/ + /offline-cbtf.
	std::string rawdatadir;
	char *openss_raw_dir = getenv("OPENSS_RAWDATA_DIR");
	char *cbtf_raw_dir = getenv("CBTF_RAWDATA_DIR");

	// always test the documented OSS name first. In all cases
	// where the experiment is run via a convenience script via
	// ossdriver, OPENSS_RAWDATA_DIR will be set.
	if (openss_raw_dir) {
	    rawdatadir = openss_raw_dir;
	} else if (cbtf_raw_dir) {
	    rawdatadir = cbtf_raw_dir;
	} else {
	    char *user_name = getenv("USER");
	    rawdatadir = "/tmp/" + std::string(user_name) + "/offline-cbtf";
	}

        OfflineExperiment myOffExp(dbname,rawdatadir);
        myOffExp.getRawDataFiles(rawdatadir);
    }

    if (!defer_view) {
	std::string viewcmd;
	viewcmd.append("openss -batch -f " + dbname);
	//viewcmd.append(dbname);
	std::cerr << "default view for " << dbname << std::endl;
	::system(viewcmd.c_str());
    }

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << OpenSpeedShop::Framework::Time::Now() << " osscollect client exits." << std::endl;
    }
#endif
}
