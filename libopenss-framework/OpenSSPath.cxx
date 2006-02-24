////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

#include <iostream>
#include <ltdl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define PATH_DELIMITER ':'
#define RELATIVE_OPENSS_PLUGIN_PATH "/openspeedshop"

#ifndef LIBRARY_DIR
#define LIBRARY_DIR 0
#endif

#ifndef PLUGIN_DIR
#define PLUGIN_DIR 0
#endif

/**
 * Method: SetOpenssLibPath()
 * 
 * The sets the search path for both our basic
 * libraries and the plugin libraries.
 *
 * Upon entry if a call to lt_dlgetsearchpath() 
 * does not result in NULL it is assumed that
 * the paths have already been set. 
 *     
 * Upon return a call to lt_dlgetsearchpath() 
 * should not result in NULL.
 *     
 * These are the order of paths searched for base
 * opess libraries:
 * 
 *     1)	LD_LIBRARY_PATH
 *     2)	LIBRARY_DIR
 * 
 * These are the order of paths searched for plugin
 * libraries and the python preparser:
 * 
 *     1)	OPENSS_PLUGIN_PATH
 *     2)	LD_LIBRARY_PATH with /openspeedshop tacked 
 *     	    	on to each entry.
 *     3)	PLUGIN_DIR
 *     
 * @param   
 * @param   
 *
 * @return  void
 *
 * @todo    .
 *
 */
static void
SetOpenssLibPath()
{

    char* ld_library_path_cstr = NULL;
    
    //std::cout << "in SetOpenssLibPath() before check" << std::endl;

    if (lt_dlgetsearchpath() == NULL) {

    	//std::cout << "in SetOpenssLibPath() after check" << std::endl;

    	// Append our compile-time library directory to LD_LIBRARY_PATH
    	// for the runtime linker to find the core OpenSpeedShop libraries.
    	if (LIBRARY_DIR) {
    	    std::string ld_library_path = std::string("LD_LIBRARY_PATH=");
    
    	    if(getenv("LD_LIBRARY_PATH") != NULL)
    	    	ld_library_path += getenv("LD_LIBRARY_PATH") + std::string(":");

    	    ld_library_path += (char *)LIBRARY_DIR;

    	    // Set new LD_LIBRARY_PATH

    	    Assert((ld_library_path_cstr = strdup(ld_library_path.c_str())) != NULL);
    	    Assert(putenv(ld_library_path_cstr) == 0);    
    	}
	else {
    	    if(getenv("LD_LIBRARY_PATH") != NULL){
    	    	std::string ld_library_path = std::string(getenv("LD_LIBRARY_PATH"));
    	     	Assert((ld_library_path_cstr = strdup(ld_library_path.c_str())) != NULL);
	    }
	}

    	// Set the plugin search paths for the dynamic loading
    	//    
    	// Note: For some unknown reason, before returning, lt_dlmutex_register()
    	//       calls libltdl_unlock() - even though it never acquired the lock
    	//       in the first place. When that happens, libltdl_unlock() triggers
    	//       an assertion failure upon calling pthread_mutex_unlock(), which
    	//       returns an EPERM error code. To fix this problem, we acquire the
    	//       lock here, manually, before calling lt_dlmutex_register().
    
    	// Initialize libltdl
    	Assert(lt_dlinit() == 0); 

#ifdef USE_DL_LOCK
    	libltdl_lock();
    	Assert(lt_dlmutex_register(libltdl_lock, libltdl_unlock,
			            libltdl_seterror, libltdl_geterror) == 0);
#endif
 
    	// Start with an empty libltdl search path
    	Assert(lt_dlsetsearchpath("") == 0);
    
    	// Add the user-specified plugin path
    	if(getenv("OPENSS_PLUGIN_PATH") != NULL) {
    	    Assert(lt_dladdsearchdir(getenv("OPENSS_PLUGIN_PATH")) == 0);
	    //std::cout << "OPENSS_PLUGIN_PATH = " <<  getenv("OPENSS_PLUGIN_PATH") << std::endl;
	}

    	// Add the LD_LIBRARY_PATH relative paths.
    	char *old_ptr = getenv("LD_LIBRARY_PATH");

    	// This is stupid code since it adds duplicate paths.
	// Feel free to make it smarter without pulling in other
	// parts of the tool. This needs to remain on the fully
	// opensource side of the tool. This means, I don't think
	// we can use Path.[ch]xx.
    	while(1) {
    	    char *new_ptr = strchr(old_ptr,PATH_DELIMITER);
	
    	    if (!new_ptr) {
    	    	std::string temp_path(old_ptr);
    	    	temp_path += RELATIVE_OPENSS_PLUGIN_PATH;
		//std::cout << "temp_path = " << temp_path.c_str() << std::endl;
    	    	Assert(lt_dladdsearchdir(temp_path.c_str()) == 0);
    	    	break;
	    }
	    else {
    	    	int size = (new_ptr) - old_ptr;
    	    	char *p_buf = (char *)malloc(size+1+strlen(RELATIVE_OPENSS_PLUGIN_PATH));
    
    	    	strncpy(p_buf,old_ptr,size);
		p_buf[size] = '\0';
		strcat(p_buf,RELATIVE_OPENSS_PLUGIN_PATH);

		//std::cout << "temp_path = " << p_buf << std::endl;
    	    	Assert(lt_dladdsearchdir(p_buf) == 0);
	    
    	    	old_ptr = new_ptr+1;
	    }
    	} // while
    
    	// Add our compile-time plugin directory
	if (PLUGIN_DIR) {
    	    Assert(lt_dladdsearchdir(PLUGIN_DIR) == 0);
	}

    	//std::cout << "LD_LIBRARY_PATH = " << getenv("LD_LIBRARY_PATH") << std::endl;
	//std::cout << std::endl << "PI_PATH = " << lt_dlgetsearchpath() << std::endl;
	
    } // if (lt_dlgetsearchpath() == NULL)
}
