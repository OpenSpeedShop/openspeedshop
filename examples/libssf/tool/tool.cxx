////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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

#include "ToolAPI.hxx"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
 
using namespace OpenSpeedShop::Framework;

int main(int argc, char* argv[])
{
    // Display usage information when necessary
    if(argc < 3) {
        std::cout << "Usage: "
                  << ((argc > 0) ? argv[0] : "???")
                  << " <function> <a.out> <args>" << std::endl;
	return 1;
    }
    
    // Build a command string from the passed arguments
    std::string command;
    for(int i = 2; i < argc; ++i) {
        if(i > 2)
            command += " ";
        command += argv[i];
    }
    
    try {
	
	// Create and open an experiment
	std::string name = std::string(argv[2]) + ".openss";
	Experiment::create(name);
	Experiment experiment(name);
	
	// Create a process for the command in the suspended state
	Thread thread = experiment.createProcess(command);

	// Find the requested function within this thread
	Optional<Function> function = thread.getFunctionByName(argv[1]);
	if(!function.hasValue())
	    throw std::runtime_error("Cannot find function " + 
				     std::string(argv[1]) +
				     "() in this process.");
	
	// Create the example collector and set its parameter
	Collector collector = experiment.createCollector("example");
	collector.setParameterValue("function", function.getValue());
	
	// Attach this process to the collector and start collecting data
	collector.attachThread(thread);
	collector.startCollecting();
	
	// Resume all threads and wait for them to terminate
	experiment.getThreads().changeState(Thread::Running);
	while(!experiment.getThreads().areAllState(Thread::Terminated))
	    sleep(1);
	
	// Evaluate the time metric for this function
	double t;
	collector.getMetricValue("time", thread,
				 function.getValue().getAddressRange(),
				 TimeInterval(Time::TheBeginning(),
					      Time::TheEnd()),
				 t);
	
	// Show the amount of time spent in this function
	std::cout << std::endl
		  << "Spent " << t << " seconds executing "
		  << function.getValue().getName() << "()." << std::endl
		  << std::endl;
	
    }
    catch(const std::exception& error) {
	std::cerr
	    << std::endl
	    << "Error: "
	    << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
		"Unknown runtime error." : error.what()) << std::endl
	    << std::endl;
	return 1;
    }

    // Indicate success to the shell
    return 0;
}
