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

#include <errno.h>
#include <iostream>
#include <ltdl.h>
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>



/**
 * Check a runtime assertion.
 *
 * Asserts (i.e. checks at runtime) that a particular expression is true. Does
 * so in a manner almost identical to that of the standard C assert() function.
 * The only real difference is that this function is NOT disabled by defining
 * the NDEBUG macro.
 *
 * @param assertion    Assertion expression to be checked.
 */
#define Assert(assertion)			    		 \
    if(!(assertion)) {						 \
	fprintf(stderr, "Assertion \"%s\" failed at line %d.\n", \
		# assertion, __LINE__);			         \
	fflush(stderr);                                          \
	abort();						 \
    }



namespace {

    /** Mutual exclusion lock for libltdl. */
    pthread_mutex_t libltdl_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

    /** Thread-local storage for libltdl error message. */
    __thread const char* libltdl_error = NULL;



    /**       
     * Lock libltdl.
     *
     * Acquire exclusive access to libltdl. Called by libltdl whenever a thread
     * needs to gain exclusive access to libltdl before proceeding further.
     */
    void libltdl_lock()
    {
	Assert(pthread_mutex_lock(&libltdl_mutex) == 0);
    }
    

    /**
     * Unlock libltdl.
     *
     * Release exclusive access to libltdl. Called by libltdl whenever a thread
     * is relinquishing exclusive access to libltdl.
     */
    void libltdl_unlock()
    {
	Assert(pthread_mutex_unlock(&libltdl_mutex) == 0);
    }
    
    
    
    /**
     * Report libltdl error.
     *
     * Called by libltdl to report an error has occured in the thread executing
     * this function. The error message is stored in thread-local storage for
     * later (possible) retrieval by libltdl_geterror().
     *
     * @param error    Error being reported by libltld for this thread.
     */
    void libltdl_seterror(const char* error)
    {
	if(libltdl_error != NULL)
	    free((void*)libltdl_error);
	libltdl_error = strdup(error);
    }



    /**
     * Get libltdl error.
     *
     * Returns the most recently reported libltdl error for this thread. This
     * message is retrieved from thread-local storage as already set by a call
     * to libltdl_seterror().
     *
     * @return    Most recent error reported by libltdl in this thread.
     */
    const char* libltdl_geterror()
    {
	return libltdl_error;
    }



}



/**
 * Main entry point.
 *
 * Main entry point for the application. Locates the full path name of this
 * process' executable, uses that path to setup library and plugin paths, then
 * loads the CLI library and calls its entry point.
 *
 * @note    The current implementation uses the Linux /proc/<pid>/exe symbolic
 *          link to locate the executable and thus is currently Linux-specific.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{
    // Form the name of this process' /proc/<pid>/exe symbolic link
    std::ostringstream executable_link;
    executable_link << "/proc/" << getpid() << "/exe";
    
    // Resolve this symbolic link into the full path name of our executable
    char executable[PATH_MAX];
    Assert(readlink(executable_link.str().c_str(),
		    executable, sizeof(executable)) != -1);
    
    // Strip off executable name to form our base directory
    std::string base = executable;
    base = (base.rfind('/') != std::string::npos) ?
	std::string(base, 0, base.rfind('/') + 1) : "/";

    // Construct the name of our library and plugin directories from the base
    char* libraries = strdup((base + "../lib").c_str());
    char* plugins = strdup((base + "../lib/openspeedshop").c_str());
    
    // Prepend our library directory to LD_LIBRARY_PATH
    std::string ld_library_path = std::string("LD_LIBRARY_PATH=") + libraries;
    if(getenv("LD_LIBRARY_PATH") != NULL)
	ld_library_path += std::string(":") + getenv("LD_LIBRARY_PATH");
    char* ld_library_path_cstr = NULL;
    Assert((ld_library_path_cstr = strdup(ld_library_path.c_str())) != NULL);
    Assert(putenv(ld_library_path_cstr) == 0);

    // Note: For some unknown reason, before returning, lt_dlmutex_register()
    //       calls libltdl_unlock() - even though it never acquired the lock
    //       in the first place. When that happens, libltdl_unlock() triggers
    //       an assertion failure upon calling pthread_mutex_unlock(), which
    //       returns an EPERM error code. To fix this problem, we acquire the
    //       lock here, manually, before calling lt_dlmutex_register().
    
    // Initialize libltdl
    Assert(lt_dlinit() == 0); 
    libltdl_lock();
    Assert(lt_dlmutex_register(libltdl_lock, libltdl_unlock,
			       libltdl_seterror, libltdl_geterror) == 0);
    
    // Start with an empty libltdl search path
    Assert(lt_dlsetsearchpath("") == 0);
    
    // Add the user-specified plugin path
    if(getenv("OPENSS_PLUGIN_PATH") != NULL)
	Assert(lt_dladdsearchdir(getenv("OPENSS_PLUGIN_PATH")) == 0);

    // Add our plugin directory
    Assert(lt_dladdsearchdir(plugins) == 0);

    // Attempt to open the CLI library
    const char* cli_library = "libopenss-cli";
    lt_dlhandle handle = lt_dlopenext(cli_library);
    if(handle == NULL) {
	std::cerr << std::endl
		  << "Unable to locate Open|SpeedShop command-line library \""
		  << cli_library << "\"." << std::endl
		  << std::endl;
	Assert(lt_dlexit() == 0);
	return 1;
    }
    
    // Attempt to locate the CLI library's entry point
    const char* cli_entry = "cli_init";
    void (*entry)(int, char*[]) = (void (*)(int, char*[]))
	lt_dlsym(handle, cli_entry);
    if(entry == NULL) {
	std::cerr << std::endl
		  << "Unable to locate entry point " << cli_entry
		  << "() in Open|SpeedShop command-line library \""
		  << cli_library << "\"." << std::endl
		  << std::endl;
	Assert(lt_dlclose(handle) == 0);
	Assert(lt_dlexit() == 0);
	return 1;
    }
    
    // Call the CLI entry point
    (*entry)(argc, argv);
    
    // Exit libltdl
    Assert(lt_dlexit() == 0);
    
    // Indicate success to the shell
    return 0;
}
