/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include "SS_Input_Manager.hxx"

#include "Python.h"

#include <ltdl.h>
#include <stdlib.h>

using namespace std;
using namespace OpenSpeedShop::cli;

    /** Mutual exclusion lock for libltdl. */
    static pthread_mutex_t libltdl_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

    /** Thread-local storage for libltdl error message. */
    static __thread const char* libltdl_error = NULL;



    /**
     * Lock libltdl.
     *
     * Acquire exclusive access to libltdl. Called by libltdl whenever a thread
     * needs to gain exclusive access to libltdl before proceeding further.
     */
    static void libltdl_lock()
    {
        Assert(pthread_mutex_lock(&libltdl_mutex) == 0);
    }


    /**
     * Unlock libltdl.
     *
     * Release exclusive access to libltdl. Called by libltdl whenever a thread
     * is relinquishing exclusive access to libltdl.
     */
    static void libltdl_unlock()
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
    static void libltdl_seterror(const char* error)
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
    static const char* libltdl_geterror()
    {
        return libltdl_error;
    }





    class ViewPluginTable {

    public:

        ViewPluginTable();
        ~ViewPluginTable();

        void foreachCallback(const std::string&);

    private:

        /**
         * View plugin table entry.
         *
         * Structure for an entry in the View plugin table describing a
         * single View plugin. Contains the path and handle for the plugin.
         */
        struct Entry {

            /** Path of this plugin. */
            Path path;

            /** Handle to this plugin. */
            void* handle;

            /** Default constructor. */
            Entry() :
                path(""),
                handle(NULL)
             {
             }

        };

        /** Keep a list of view plugins. */
        std::list<Entry> dm_entry;

    };

namespace {



    /**
     * View plugin search callback.
     *
     * Simply re-routes callbacks made during a collector plugin search to the
     * appropriate CollectorPluginTable object.
     *
     * @param filename    File name of potential collector plugin.
     * @param data        Untyped pointer to the CollectorPluginTable.
     * @return            Always returns zero to insure search continues.
     */
    int foreachCallback(const char* filename, lt_ptr data)
    {
        // Re-route the callback to the actual PluginTable object
        ViewPluginTable* table =
            reinterpret_cast<ViewPluginTable*>(data);
        table->foreachCallback(filename);

        // Always return zero to insure we keep searching
        return 0;
    }



}

/**
 * View plugin search callback.
 *
 * Callback function called (indirectly via ::foreachCallback) by libltdl for
 * each potential view plugin candidate found in our specified search path.
 * The candidate is examined to determine if it is truly a view plugin and,
 * if so, is added to our table of available view plugins.
 *
 * @param filename    File name of potential view plugin.
 */
void ViewPluginTable::foreachCallback(const std::string& filename)
{   
    // Create an entry for this possible view plugin
    Entry entry;
    entry.path = filename;
            
    // Can we open this file as a libltdl module?
    lt_dlhandle handle = lt_dlopenext(entry.path.c_str());
    handle = lt_dlopenext(entry.path.c_str());
    if(handle == NULL) {
        return;
    }
    // Is there a view factory method in this module?
    void (*factory)() = (void (*)()) lt_dlsym(handle, "ViewFactory");
    if (factory == NULL) {
     // close an unneeded handle.
        Assert(lt_dlclose(handle) == 0);
        return;
    }

    // Call the initialization entry point.
    // Note that more than one view may be defined.
    (void) (*factory)();

    // Add this entry to the table of loaded view plugins
    // We need this list so that we can unload them on termination.
    entry.handle = handle;
    dm_entry.push_back(entry);
}

/**
 * Default constructor.
 *
 * Sets up an empty view plugin table and initializes libltdl.
 * Builds the table of available view plugins. If libltdl's user-defined
 * library search path has not been set elsewhere (e.g. by the tool), it is set
 * to the value of the environment variable OPENSS_PLUGIN_PATH. In either case,
 * that path is then searched for plugin and any valid ones are loaded into
 * memory and the initialization entrypoints are called.
 */
ViewPluginTable::ViewPluginTable()
{

  if (lt_dlgetsearchpath() == NULL) {

    // Is the following code needed?? 
    //   It seems so.  We need to find the built-in plugins and need
    //   this setup sequence to assure that we know the right path,
    //   since we can not count on the user setting OPENSS_PLUGIN_PATH
    //   to point us towards the openss install directories.

    // Prepend our compile-time library directory to LD_LIBRARY_PATH
    std::string ld_library_path = std::string("LD_LIBRARY_PATH=") + LIBRARY_DIR;
    if(getenv("LD_LIBRARY_PATH") != NULL) {
        ld_library_path += std::string(":") + getenv("LD_LIBRARY_PATH");
    }
    char* ld_library_path_cstr = NULL;
    Assert((ld_library_path_cstr = strdup(ld_library_path.c_str())) != NULL);
    Assert(putenv(ld_library_path_cstr) == 0);

    // Initialize libltdl 
    Assert(lt_dlinit() == 0);

    // Note: For some unknown reason, before returning, lt_dlmutex_register()
    //       calls libltdl_unlock() - even though it never acquired the lock
    //       in the first place. When that happens, libltdl_unlock() triggers
    //       an assertion failure upon calling pthread_mutex_unlock(), which
    //       returns an EPERM error code. To fix this problem, we acquire the
    //       lock here, manually, before calling lt_dlmutex_register().

/* There appear to be several bugs in the locking code.  Don't use it! dew 25 Oct 2005 */
   // libltdl_lock();
   // Assert(lt_dlmutex_register(libltdl_lock, libltdl_unlock,
   //                            libltdl_seterror, libltdl_geterror) == 0);

    // Start with an empty libltdl search path
    Assert(lt_dlsetsearchpath("") == 0);

    // Only set the libltdl user-defined search path if it isn't already
    if(getenv("OPENSS_PLUGIN_PATH") != NULL)
        Assert(lt_dlsetsearchpath(getenv("OPENSS_PLUGIN_PATH")) == 0);

    // Add our compile-time plugin directory
    Assert(lt_dladdsearchdir(PLUGIN_DIR) == 0);

    // Attempt to open the CLI library
    lt_dlhandle handle = lt_dlopenext("libopenss-cli");
    if(handle == NULL) {
        const char* error = lt_dlerror();
        Assert(error != NULL);
        std::cerr << std::endl << error << std::endl << std::endl;
        Assert(lt_dlexit() == 0);
        return;
    }
  }

    // Search for view plugins in the libltdl user-defined search path
    if(lt_dlgetsearchpath() != NULL)
        lt_dlforeachfile(lt_dlgetsearchpath(), ::foreachCallback, this);

}



/**
 * Destructor.
 * 
 * Destroys the view plugin table, removes them from memory and exits libltdl.
 *
 */
ViewPluginTable::~ViewPluginTable()
{
    // Check preconditions
    for(std::list<Entry>::iterator i = dm_entry.begin();
        i != dm_entry.end();
        ++i) {

            // Unload the plugin from memory
            lt_dlhandle handle =
                reinterpret_cast<lt_dlhandle>(i->handle);
            Assert(i->handle != NULL);
            Assert(lt_dlclose(handle) == 0);
            i->handle = NULL;

    }

    // Exit libltdl
     Assert(lt_dlexit() == 0);
}


// Hide the plugin tableC
static ViewPluginTable *TheTable = NULL;

// Find and initialize user define views.
void SS_Load_View_plugins () {
  if (TheTable != NULL) delete TheTable;
  TheTable = new ViewPluginTable;
}

// Remove user define views.
void SS_Remove_View_plugins () {
  if (TheTable != NULL) delete TheTable;
  TheTable = NULL;
}
