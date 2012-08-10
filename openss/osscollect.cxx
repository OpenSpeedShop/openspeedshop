////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-12 Krell Institute. All Rights Reserved.
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
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include "Collector.hxx"
#include "Experiment.hxx"
#include "FEThread.hxx"
#include "Path.hxx"
#include "ThreadGroup.hxx"
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"

using namespace boost;
using namespace KrellInstitute::Core;
using namespace OpenSpeedShop::Framework;


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
    std::string topology, connections, collector, program, mpiexecutable, cbtfrunpath;

    // create a default for topology file.
    char const* home = getenv("HOME");
    std::string default_topology(home);
    default_topology += "/.cbtf/cbtf_topology";
    // create a default for connections file.
    std::string default_connections(home);
    default_connections += "/.cbtf/attachBE_connections";
    // create a default for the collection type.
    std::string default_collector("pcsamp");

    boost::program_options::options_description desc("pcsampDemo options");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("numBE", boost::program_options::value<unsigned int>(&numBE)->default_value(1),
	    "Number of lightweight mrnet backends. Default is 1, For an mpi job this must match the number of mpi ranks specififed in the mpi launcher arguments.")
        ("topology",
	    boost::program_options::value<std::string>(&topology)->default_value(default_topology),
	    "Path name to a valid mrnet topology file. (i.e. from mrnet_topgen),")
        ("connections",
	    boost::program_options::value<std::string>(&connections)->default_value(default_connections),
	    "Path name to a valid backend connections file. The connections file is created by the mrnet backends based on the mrnet topology file. The default is sufficient for most cases.")
        ("collector",
	    boost::program_options::value<std::string>(&collector)->default_value(default_collector),
	    "Name of collector to use [pcsamp | usertime]. Default is pcsamp.")
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

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    bool finished = false;

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
    std::string dbprefix = prg + "-" + collector;
    // get a dbname that does not conflict with an existing dbname.
    std::string dbname = createDBName(dbprefix);

    // create the actual database and create an experiment object.
    OpenSpeedShop::Framework::Experiment::create (dbname);
    OpenSpeedShop::Framework::Experiment *FW_Experiment = new OpenSpeedShop::Framework::Experiment (dbname);

    // setup the experiment to run with cbtf.
    FW_Experiment->setBEprocCount( numBE );
    Collector mycollector = FW_Experiment->createCollector( collector );
    ThreadGroup tg = FW_Experiment->createProcess("FOO", "localhost", numBE,
                                                     OutputCallback(NULL,NULL),
                                                     OutputCallback(NULL,NULL)   );

    // From this point on we run the application with cbtf and specified collector.
    // verify valid numBE.
    if (numBE == 0) {
        std::cout << desc << std::endl;
        return 1;
    } else if (program == "" && numBE > 0) {
	// this allows us to start the mrnet client FE
	// and then start the program with collector in
	// a separate window using cbtfrun.
	FEThread fethread;
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


    // TODO: need to cleanly terminate mrnet.
    FEThread fethread;
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

	    size_t pos = program.find(mpiexecutable);
            SymtabAPISymbols stapi_symbols;

            // Determine if libmpi is present in the application in order to call out the proper
            // collector runtime library.
            bool found_mpi_lib = stapi_symbols.foundLibrary(mpiexecutable,"libmpi");

            if (found_mpi_lib) {

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
