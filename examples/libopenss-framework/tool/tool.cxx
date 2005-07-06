////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
#include <stdlib.h>
#include <string>
#include <unistd.h>
 
using namespace OpenSpeedShop::Framework;



static void Run(int argc, char* argv[])
{
    // Create and open a new experiment
    std::string name = std::string(Path(argv[0]).getBaseName()) + ".openss";
    if(Experiment::isAccessible(name))
	Experiment::remove(name);
    Experiment::create(name);	
    Experiment experiment(name);

    // Was the first argument potentially a process identifier?
    pid_t pid = strtol(argv[0], NULL, 10);
    if(pid > 0) {
	
	// Attach to the specified process and put it into the suspended state
	experiment.attachProcess(pid);
	
    }
    else {

	// Build a command string from the passed arguments
	std::string command;
	for(int i = 0; i < argc; ++i) {
	    if(i > 0)
		command += " ";
	    command += argv[i];
	}
	
	// Create a process for the command in the suspended state
	experiment.createProcess(command);
	
    }

    // Wait until we've finished connecting the process
    while(experiment.getThreads().isAnyState(Thread::Connecting))
	sleep(1);
    
    // Create and start the PC sampling collector
    Collector collector = experiment.createCollector("pcsamp");
    collector.setParameterValue("sampling_rate", (unsigned)100);
    experiment.getThreads().startCollecting(collector);
    
    // Resume all threads and wait for them to terminate
    experiment.getThreads().changeState(Thread::Running);
    while(!experiment.getThreads().areAllState(Thread::Terminated))
	sleep(1);
}



static void Show(const std::string& name)
{
    // Open the existing experiment
    Experiment experiment(name);

    // Find the first collector and thread in this experiment
    Collector collector = *(experiment.getCollectors().begin());
    Thread thread = *(experiment.getThreads().begin());

    // Evaluate the collector's time metric for all functions in thread
    SmartPtr<std::map<Function, double> > data;
    Queries::GetMetricByFunctionInThread(collector, "time", thread, data);
    
    // Sort the results
    std::multimap<double, Function> sorted;
    for(std::map<Function, double>::const_iterator
	    i = data->begin(); i != data->end(); ++i)
	sorted.insert(std::make_pair(i->second, i->first));	
    
    // Display the results
    
    std::cout << std::endl << std::endl
	      << std::setw(10) << "Time"
	      << "    "
	      << "Function" << std::endl
	      << std::endl;
    
    for(std::multimap<double, Function>::reverse_iterator
	    i = sorted.rbegin(); i != sorted.rend(); ++i) {
	
	std::cout << std::setw(10) << std::fixed << std::setprecision(3)
		  << i->first
		  << "    "
		  << i->second.getName();
	
	std::set<Statement> definitions = i->second.getDefinitions();
	for(std::set<Statement>::const_iterator
		i = definitions.begin(); i != definitions.end(); ++i)
	    std::cout << " (" << i->getPath().getBaseName()
		      << ", " << i->getLine() << ")";
	
	std::cout << std::endl;
	
    }
    
    std::cout << std::endl << std::endl;    
}



int main(int argc, char* argv[])
{
    // Display usage information when necessary
    if(argc < 2) {
        std::cout << "Usage: "
                  << ((argc > 0) ? argv[0] : "???")
                  << " [<a.out> <args>] | [<exp-dbase>]" << std::endl;
	return 1;
    }
    
    // Otherwise get things rolling
    try {
	
	// Is argv[1] an existing experiment?
	if((argc == 2) && Experiment::isAccessible(argv[1]))
	    Show(argv[1]);

	// Otherwise run the experiment
	else
	    Run(argc - 1, &(argv[1]));
	
    }
    catch(const Exception& error) {
	std::cerr
	    << std::endl
	    << "Error: " << error.getDescription() << std::endl
	    << std::endl;
    }
    
    // Indicate success to the shell
    return 0;
}
