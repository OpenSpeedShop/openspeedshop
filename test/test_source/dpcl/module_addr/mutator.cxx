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


#include <assert.h>
#include <dpcl.h>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX  256
#endif


void dummyCallback(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType)
{
}

int main(int argc, char* argv[])
{
    AisStatus retval;

    // Display usage information when necessary
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <a.out> <args>" << std::endl;
        return 1;
    }
    
    // Report progress
    std::cout << std::endl << "Initializing DPCL..." << std::endl;
    
    // Initialize DPCL
    Ais_initialize();
    if(getenv("DEBUG_DPCLD") != NULL) {
        char hostname[HOST_NAME_MAX];
        assert(gethostname(hostname, sizeof(hostname)) == 0);
        Ais_blog_on(hostname, LGL_detail, LGD_daemon, NULL, 0);
    }
    
    // Report progress
    std::cout << "Creating process..." << std::endl;

    // Create the (suspended) process
    Process process;
    extern char** environ;
    retval = process.bcreate(NULL, argv[1], (const char**)&(argv[1]), ::environ,
                             dummyCallback, NULL, dummyCallback, NULL);
    assert(retval.status() == ASC_success);

    // Get the program object associated with this process
    SourceObj program = process.get_program_object();

    // Iterate over each module in this program
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);

	// Get the name of this module
	char name[module.module_name_length() + 1];
	module.module_name(name, module.module_name_length() + 1);	

	if (module.address_start() == 0)
		std::cout << std::hex 
			  << std::setfill('0') 
			  << std::setw(sizeof(void*) * 2)
			  << "FAILURE Module \"" << name << "\" at [ " 
			  << "0x" << module.address_start() << ", "
			  << "0x" << module.address_end() << " )"
			  << std::endl;
	else if (module.address_end() == 0)
		std::cout << std::hex 
			  << std::setfill('0') 
			  << std::setw(sizeof(void*) * 2)
			  << "FAILURE Module \"" << name << "\" at [ " 
			  << "0x" << module.address_start() << ", "
			  << "0x" << module.address_end() << " )"
			  << std::endl;
	else // Report progress
		std::cout << std::hex 
			  << std::setfill('0') 
			  << std::setw(sizeof(void*) * 2)
			  << "Success Module \"" << name <<  " "
			  << "module.address_start() is nonzero" <<  ", "
			  << "module.address_end() is nonzero" <<  " )"
			  << std::endl;
	
    }

    // Report progress
    std::cout << "Destroying process..." << std::endl;

    // Destroy the process
    retval = process.bdestroy();
    assert(retval.status() == ASC_success);
    
    // Success!
    return 0;
}
