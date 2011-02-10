/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2011 Krell Institute  All Rights Reserved.
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

// Static sources for SetOpenssLibPath()
#include "OpenSSPath.cxx"

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
    // Only examine the view plugins.
    if (filename.find("_view") == std::string::npos) {
	return;
    }

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

    std::string new_cmdline;

    // Set up LD_LIBRARY_PATH and plugin dl_open paths
    // if not done already.
    SetOpenssLibPath(new_cmdline);

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
