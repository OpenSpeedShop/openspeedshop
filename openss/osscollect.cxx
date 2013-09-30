////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2013 Krell Institute. All Rights Reserved.
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

#include <sys/stat.h>
#include <errno.h>

#include <sys/param.h>
#include <sys/wait.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>

#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "Collector.hxx"
#include "Experiment.hxx"
#include "FEThread.hxx"
#include "Path.hxx"
#include "ThreadGroup.hxx"
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace boost;
using namespace KrellInstitute::Core;
using namespace OpenSpeedShop::Framework;

enum exe_class_types { MPI_exe_type, SEQ_RunAs_MPI_exe_type, SEQ_exe_type };

// Experiment Utilities.

// Function that returns the number of BE processes that are required for LW MRNet BEs.
// The function tokenizes the program command and searches for -np or -n.
static int getBEcountFromCommand(std::string command) {

    int retval = 1;
    bool is_debug_startup_enabled = (getenv("OPENSS_DEBUG_STARTUP") != NULL);

    if (is_debug_startup_enabled) {
      std::cerr << "DEBUG: osscollect, Enter getBEcountFromCommand, command=" << command << std::endl;
    }

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);
    std::string S = "";

    bool found_be_count = false;

    BOOST_FOREACH (const std::string& t, btokens) {
	S = t;
	if (found_be_count) {
	    S = t;
	    retval = boost::lexical_cast<int>(S);
            if (is_debug_startup_enabled) {
              std::cerr << "DEBUG: osscollect, IN getBEcountFromCommand, found_be_count set retval=" << retval << " S=" << S << std::endl;
            }
	    break;
	} else if (!strcmp( S.c_str(), std::string("-np").c_str())) {
            if (is_debug_startup_enabled) {
              std::cerr << "DEBUG: osscollect, IN getBEcountFromCommand, found -np value" << std::endl;
            }
	    found_be_count = true;
	} else if (!strcmp(S.c_str(), std::string("-n").c_str())) {
            if (is_debug_startup_enabled) {
              std::cerr << "DEBUG: osscollect, IN getBEcountFromCommand, found -n value" << std::endl;
            }
	    found_be_count = true;
	}
    } // end foreach

    if (is_debug_startup_enabled) {
      std::cerr << "DEBUG: osscollect, Exit getBEcountFromCommand, retval=" << retval << std::endl;
    }
    return retval;
}

// 
// Determine if libmpi is present in this executable.
//
static bool isMpiExe(const std::string exe) {
    SymtabAPISymbols stapi_symbols;
    bool found_libmpi = stapi_symbols.foundLibrary(exe,"libmpi");
    return found_libmpi;
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

    bool is_debug_startup_enabled = (getenv("OPENSS_DEBUG_STARTUP") != NULL);

    if (is_debug_startup_enabled) {
      std::cerr << "DEBUG: osscollect, ENTER getMPIExecutableFromCommand, command=" << command << std::endl;
    }

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);

    BOOST_FOREACH (const std::string& t, btokens) {
      if (is_debug_startup_enabled) {
        std::cerr << "DEBUG: osscollect, IN getMPIExecutableFromCommand, top of if is_executable, t=" << t << std::endl;
      }
      if (is_executable( t )) {
         exe_class_types local_exe_type = typeOfExecutable(command, t);
         if (is_debug_startup_enabled) {
           std::cerr << "DEBUG: osscollect, IN getMPIExecutableFromCommand, inside checks base on if is_executable, t=" 
                     << t << " local_exe_type=" << local_exe_type << std::endl;
         }
         if (local_exe_type == MPI_exe_type || local_exe_type == SEQ_RunAs_MPI_exe_type ) {
           if (is_debug_startup_enabled) {
             std::cerr << "DEBUG: osscollect, EXIT getMPIExecutableFromCommand, early return t=" << t << std::endl;
           }
           return t;
         }
      }
    } // end foreach

  if (is_debug_startup_enabled) {
    std::cerr << "DEBUG: osscollect, EXIT getMPIExecutableFromCommand, bottom of routine return retval=" << retval << std::endl;
  }
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
	LocalDataFileName = ".";
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

int main(int argc, char** argv)
{
    unsigned int numBE;
    bool isMPI;
    std::string topology, arch, connections, collector, program, mpiexecutable, cbtfrunpath;

    bool is_debug_startup_enabled = (getenv("OPENSS_DEBUG_STARTUP") != NULL);

    // create a default for topology file.
    char const* cur_dir = getenv("PWD");
    std::string default_topology(cur_dir);
    std::string cbtf_path(cur_dir);

    default_topology += "/cbtf_topology";

    // create a default for connections file.
    std::string default_connections(cur_dir);
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
            boost::program_options::value<std::string>(&cbtfrunpath)->default_value(""),
            "Path to cbtfrun to collect data from, If target is cray or bluegene, use this to point to the targeted client.")
        ("mpiexecutable",
	    boost::program_options::value<std::string>(&mpiexecutable)->default_value(""),
	    "Name of the mpi executable. This must match the name of the mpi exectuable used in the program argument and implies the collection is being done on an mpi job if it is set.")
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

    // Generate the --mpiexecutable argument value if it is not set
    if (program != "" && mpiexecutable == "") {

      // Find out if there is an mpi driver to key off of
      // Then match the mpiexecutable value to the program name
      mpiexecutable = getMPIExecutableFromCommand(program);
      if (is_debug_startup_enabled) {
        std::cerr << "DEBUG: osscollect, program=" << program << " mpiexecutable=" << mpiexecutable << std::endl;
      }

    }

    if (mpiexecutable != "") {
         numBE = getBEcountFromCommand(program);
         if (is_debug_startup_enabled) {
           std::cerr << "DEBUG: osscollect, numBE=" << numBE << " mpiexecutable=" << mpiexecutable << std::endl;
         }
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    bool finished = false;

    // TODO: pass numBE to CBTFTopology and record as the number
    // of application processes.
    CBTFTopology cbtftopology;
    std::string fenodename;
    if (topology.empty()) {
      if (arch == "cray") {
          cbtftopology.autoCreateTopology(BE_CRAY_ATTACH);
      } else {
          cbtftopology.autoCreateTopology(BE_ATTACH);
      }
      topology = cbtftopology.getTopologyFileName();
      fenodename =  cbtftopology.getFENodeStr();
      std::cerr << "Generated topology file: " << topology << std::endl;
    } else {
      //topology = default_topology;
      fenodename =  "localhost";
    }
    if (is_debug_startup_enabled) {
      std::cerr << "DEBUG: osscollect, fenodename=" << fenodename << " topology=" << topology << std::endl;
    }

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

    Collector mycollector = FW_Experiment->createCollector(
        (collector == "cuda") ? "io" : collector
        );

    ThreadGroup tg = FW_Experiment->createProcess(program, fenodename, numBE,
                                                     OutputCallback(NULL,NULL),
                                                     OutputCallback(NULL,NULL)   );

    FEThread fethread;

    // From this point on we run the application with cbtf and specified collector.
    // verify valid numBE.
    if (numBE == 0) {
        std::cout << desc << std::endl;
        return 1;
    } else if (program == "" && numBE > 0) {
	// this allows us to start the mrnet client FE
	// and then start the program with collector in
	// a separate window using cbtfrun.
	fethread.start(topology,connections,collector,numBE,finished);
	std::cout << "Running Frontend for " << collector << " collector."
	  << "\nCreating openss database: " << dbname
	  << "\nNumber of mrnet backends: "  << numBE
          << "\nTopology file used: " << topology << std::endl;
	std::cout << "Start mrnet backends now..." << std::endl;
	// ctrl-c to exit.  need cbtfdemo to notify us when all threads
	// have finised.
	while(true);
        fethread.join();
	exit(0);
    } else {
        std::cout << "Running " << collector << " collector."
	  << "\nProgram: " << program
	  << "\nCreating openss database: " << dbname
	  << "\nNumber of mrnet backends: "  << numBE
          << "\nTopology file used: " << topology << std::endl;
    }

    // TODO: does cbtf cleanly terminate mrnet.
    fethread.start(topology,connections,collector,numBE,finished);
    sleep(3);

    // simple fork of process to run the program with collector.
    pid_t child,w;
    int status;

    child = fork();
    if(child < 0){
        std::cout << "fork failed";
    } else if(child == 0){

        if (!mpiexecutable.empty()) {

#if 0
            size_t pos;
#else
            int64_t pos;
#endif
            if (cbtftopology.getIsCray()) {
                if (std::string::npos != program.find("aprun")) {
                    // Add in the -L list of nodes if aprun is present 
                    // and we are not co-locating
                    std::list<std::string> nodes = cbtftopology.getAppNodeList();
                    std::string appNodesForAprun = "-L " + cbtftopology.createRangeCSVstring(nodes) + " ";
                    pos = program.find("aprun ") + 6;
                    program.insert(pos, appNodesForAprun);

                    if (is_debug_startup_enabled) {
                      std::cerr << "DEBUG: osscollect, program=" << program << " mpiexecutable=" << mpiexecutable << std::endl;
                      std::cerr << "DEBUG: osscollect, appNodesForAprun=" << appNodesForAprun << " pos=" << pos << std::endl;
                    }
                }
            }

	    pos = program.find(mpiexecutable);
            exe_class_types appl_type =  typeOfExecutable(program, mpiexecutable);

            if (is_debug_startup_enabled) {
              std::cerr << "DEBUG: osscollect, appl_type=" << appl_type << " pos=" << pos << std::endl;
            }

            if (appl_type == MPI_exe_type) {
              
              if (!cbtfrunpath.empty()) {
                program.insert(pos, " " + cbtfrunpath + " --mrnet --mpi -c " + collector + " \"");
              } else {
                program.insert(pos, " cbtfrun --mrnet --mpi -c " + collector + " \"");
              }

            } else {

              if (!cbtfrunpath.empty()) {
                program.insert(pos, " " + cbtfrunpath + " --mrnet -c " + collector + " \"");
              } else {
                program.insert(pos, " cbtfrun --mrnet -c " + collector + " \"");
              }

            }
            program.append("\"");
            std::cerr << "executing mpi program: " << program << std::endl;
	    
            ::system(program.c_str());

	} else {
    	    const char * command = "cbtfrun";
            if (!cbtfrunpath.empty()) {
                command = cbtfrunpath.c_str() ;
            } 

            std::cerr << "executing sequential program: "
		<< command << " -m -c " << collector << " " << program << std::endl;

            execlp(command,"-m", "-c", collector.c_str(), program.c_str(), NULL);
	}
    } else {

	do {
	    w = waitpid(child, &status, WUNTRACED | WCONTINUED);
	    if (w == -1) {
		perror("waitpid");
		exit(EXIT_FAILURE);
	    }
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

        fethread.join();
    }

}
