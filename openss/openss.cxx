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

#include <iostream>
#include <ltdl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>

extern char **environ;

// jeg - commmenting this out.
// this define cause the code in OpenSSPath.cxx
// in libopenss-framework and inlined here (below)
// to do a hard call to ltdl_lock.  I believe this
// was a workaround for libtool/libltdl problems 
// in a previous version.  What is happening now
// with libtool/libltdl 1.5.24 is that this code
// is causing a deadlock.  So, removing as it appears
// to work fine without this code on other versions
// of libltdl, older than 1.5.24.
/*
#define USE_DL_LOCK
*/

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

        if (error == NULL) return;

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

// Static sources for SetOpenssLibPath()
#include "OpenSSPath.cxx"

#define RE_EXEC_FLAG "-OSS_NEW_PATH"

/**
 * Function s_recall_openss.
 *
 * If this is the first call to openss, check if the 
 * LD_LIBRARY_PATH is redone. If so, reexec openss with
 * the new environment.
 *
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 * @param new_cmdline	command-line to be built.
 */

/* no more than 100 args to any program being called */
#define MAXARGS 100

static int 
s_recall_openss(int argc, char* argv[], std::string& new_cmdline)
{

    // Set up LD_LIBRARY_PATH and plugin dl_open paths.
    SetOpenssLibPath(new_cmdline);
    
    // This is for gdb because the execv() call seems to throw it
    // for a loop
    if ( (getenv("OPENSS_DEBUG_OPENSS") != NULL)) {
        return argc;
    }

    // Check to see if we need to redo the search path.
    //
    // We look at the last argument for the defined 
    // string for RE_EXEC_FLAG.
    //
    // If it doesn't match it means that openss is
    // exec'ed directly from the user. We need to 
    // look up the current path for this execution of
    // openss and from that  guess the path to the openss
    // libraries and set LD_LIBRARY_PATH. Then we reinvoke
    // openss with the RE_EXEC_FLAG added to the argument
    // list.
    //
    // If it does match we decrement argc and move on.
    //
    if (((strcmp(argv[argc -1], RE_EXEC_FLAG)) != 0)) {
	
	char *t_path = getenv("LD_LIBRARY_PATH");
	if (t_path != NULL) {
    	    char *  args[MAXARGS];
    	    int     i;

	    //cout << "My path: " << t_path << endl;
	    //cout << "My exe: " << new_cmdline << endl;

    	    // Start with the executable name
	    args[0] = (char *)malloc(new_cmdline.length()+1);
	    strcpy(args[0],new_cmdline.c_str());

	    // Add the old arguments
    	    for (i=1;i<argc;++i) {
	    	args[i] = argv[i];
	    }
	    // Add our sentinel flag
	    args[i++] = (char *) RE_EXEC_FLAG;
	    args[i] = NULL;
	    
	    // Execute the commandline again
	    execv(new_cmdline.c_str(),args);
	    //cout << "AFTER execv() " << new_cmdline.c_str() << endl;
	}
	else {
	    //cout << "My path is NULL!" << endl;
	    return argc;
    	}
    }
    else {
    	// Pretend the last argument is not there.
	return --argc;
    }

    return argc;
}

/**
 * Main entry point.
 *
 * Main entry point for the application. Sets up library and plugin paths, then
 * loads the CLI library and calls its entry point.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{

    std::string new_cmdline;

    // Set up LD_LIBRARY_PATH and plugin dl_open paths.
    argc = s_recall_openss(argc, argv, new_cmdline);

    // Attempt to open the CLI library
    lt_dlhandle handle = lt_dlopenext("libopenss-cli");
    if(handle == NULL) {
	const char* error = lt_dlerror();
	Assert(error != NULL);
	std::cerr << std::endl << error << std::endl << std::endl;
	Assert(lt_dlexit() == 0);
	return 1;
    }

    // Attempt to locate the CLI library's entry point
    void (*entry)(int, char*[]) = (void (*)(int, char*[]))
	lt_dlsym(handle, "cli_init");
    if(entry == NULL) {
	const char* error = lt_dlerror();
	Assert(error != NULL);
	std::cerr << std::endl << error << std::endl << std::endl;
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
