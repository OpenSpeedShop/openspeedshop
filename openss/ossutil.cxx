////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2012 The Krell Institute. All Rights Reserved.
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
#include "OfflineExperiment.hxx"

#include <string>
#include <fcntl.h>
#include <unistd.h>

using namespace OpenSpeedShop::Framework;

int main(int argc, char* argv[])
{
    // Display usage information when necessary
    // We need to handle multiple files for threaded jobs
    // and mpi jobs where data will be collected for multiple
    // processes(and threads) on more than one host.
    // Therefore we need to consider where such raw data files
    // will be located. i.e. this tool will need to be able
    // to append raw data to one global openss database from
    // multiple openss-raw files.

    std::string DBname;

    if(argc < 2) {
        std::cout << "Usage: "
            << ((argc > 0) ? argv[0] : "???")
            << "\n [<exp-raw-directory>] directory of openss raw data files\n" 
            << " or directory with openss raw data sub-directories from an"
	    << " mpi experiment."
	    << std::endl;
	return 1;
    }
    
    // Is argv[1] an existing experiment?
    //if((argc == 3) && Experiment::isAccessible(argv[2]))
    if((argc == 3) ) {
	DBname = argv[1];
	std::string NewDBname = argv[2];
        Experiment prunedExperiment(DBname);
	prunedExperiment.compressDB();
    } else if ((argc == 2) ) {
	char base[100];
	int64_t cnt = 0;
	for (cnt = 0; cnt < 1000; cnt++) {
	    snprintf(base, 100, "./X.%lld.openss",cnt);
	    int fd;
	    if ((fd = open(base, O_RDONLY)) != -1) {
		Assert(close(fd) == 0);
		continue;
	    }
	    DBname = std::string(base);
	    break;
	}

	std::string rawdatadir = argv[1];
	OfflineExperiment myOffExp(DBname,rawdatadir);
	myOffExp.getRawDataFiles(rawdatadir);
   }

   exit(0);
}
