////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 

/*! 
    This file is responsible for loading all plugins at initialization time.
    A directory pointed at the environment vairiable OPENSS_PLUGIN_PATH
    is interrogated to load plugins (from this file).   Each plugin file
    is openned to get information about it.   Information such as what
    group function does it belong to, what menu (if any) does it need
    added to the menu bar, and what are it's entry points.


    After the plugins are all loaded control is returned back to the 
    event loop to wait for menu requests to actually create the user panels.

    Author: Al Stipek   (stipek@sgi.com)
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
// #include <dlfcn.h>
#include <errno.h>
#include <time.h>

#include <assert.h>
#include <ltdl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <glob.h>

#include <sys/wait.h>

#include <string.h>

// #define DEBUG_OUTPUT_REQUESTED 1
#include "debug.hxx"  // This includes the definition of dprintf

#include "plugin_handler.hxx"
#include "PluginInfo.hxx"

#define PLUGIN_INFO_ENTRY_POINT "plugin_info_init"
static char *plugin_directory = NULL;

#include <stdlib.h>

#include "PanelContainer.hxx"

#include <qwidget.h>
#include <qlayout.h>

class PanelContainer;
PanelContainerList *panelContainerList;
static PanelContainer *masterPC;
static QWidget *pl;

#include "PanelContainer.hxx"
/*! This routine opens each plugin and looks for a predefined entry point.
    If the entry point exists, then the plugin information is loaded that 
    describes this plugin.   NOTE: This is only for the panel plugin entries.
*/
  
int
register_plugin(const char *plugin_file)
{
  printf("Attempting to open %s\n", plugin_file );

  // We got a plugin location directory and an absoluted path to a plugin.
  // Let's try to load the PluginInfo information.

  lt_dlhandle dl_object = lt_dlopenext((const char *)plugin_file);
  if( dl_object == NULL )
  {
//NOTE: THIS IS THE IMPORTANT DEBUG PRINT
//    fprintf(stderr, "(%s): lt_dlerror()=%s\n", plugin_file, lt_dlerror() );
    return(0);
  }

  dprintf("We seemed to open the dynamic library (%s) just fine...\n", plugin_file);

  dprintf("Try to lookup the entry_point (%s)\n", plugin_file);

  // We've managed to open the dso.  Can we the PluginInfo entry point?
  lt_ptr (*dl_plugin_info_init_routine)(void *, void *);

  dl_plugin_info_init_routine = (lt_ptr (*)(void *, void *))lt_dlsym(dl_object, PLUGIN_INFO_ENTRY_POINT);
  if( dl_plugin_info_init_routine == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", PLUGIN_INFO_ENTRY_POINT, plugin_file, lt_dlerror() );
//    fprintf(stderr, "This is not a gui panel plugin... ignore it.\n");
    lt_dlclose(dl_object);
    return(0);
  }

  PluginInfo *pluginInfo = new PluginInfo((QObject *)pl);
  // Well, we found the plugin entry point, which is suppose to contain
  // information about this plugin.   Let's call it and fill up our 
  // PluginInfo structure with the pertinent information.
  lt_ptr i = (*dl_plugin_info_init_routine)((void *)pluginInfo, (void *)masterPC);
  dprintf("%s returned %d\n", PLUGIN_INFO_ENTRY_POINT, i);


//  pluginInfo->Print();

  // We've managed to open the dso and initialize the plugin description 
  // information, from the dso.  Now from the plugin description, can we
  // find the defined entry point?
  dprintf("Try to lookup the entry_point (%s)\n", pluginInfo->plugin_entry_point);

  lt_ptr (*dl_routine)(void *, void *);

  dl_routine = (lt_ptr (*)(void *, void *))lt_dlsym(dl_object, pluginInfo->plugin_entry_point);

  if( dl_routine == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found dlerror()=%s\n", pluginInfo->plugin_entry_point, lt_dlerror() );
    delete pluginInfo;   // Don't forget to delete this on an early return.
    return 0;
  }

  // Well, we found the entry point and everything seems to match.  Now
  // call the entry point routine passing it a couple of void pointers (the
  // called routine will need to cast these appropriately.)
  // The first parameter is to the parent tools, parent class.   The second
  // parameter is a PluginInfo object, which contains all the known
  // information about the plugin that is being loaded.
  dprintf("\tlibdso: dynamic routine returned %d\n", i);

  i = (*dl_routine)((void *)pl, (void *)pluginInfo);
  pluginInfo->dl_create_and_add_panel = (Panel * (*)(void *, void *, void *))lt_dlsym(dl_object, "create_and_add_panel" );
  if( pluginInfo->dl_create_and_add_panel == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found dlerror()=%s\n", pluginInfo->plugin_entry_point, lt_dlerror() );
//    fprintf(stderr, "Will not be able to create_and_add_panel() for:\n");
    pluginInfo->Print();
    return(-1);
  }

  if( masterPC->_pluginRegistryList == NULL )
  {
    masterPC->_pluginRegistryList = new PluginRegistryList;
    masterPC->_pluginRegistryList->clear();
  }
  masterPC->_pluginRegistryList->push_back(pluginInfo);
  return(0);
}

int
foreachCallback(const char *filename, lt_ptr data)
{
  dprintf("foreachCallback() called for (%s)\n", filename );

  register_plugin(filename);

  // Always return zero to insure we keep searching
  return 0;
}

/*! This routine checks the plugin directories for
    plugin files.   It loops over and shovels each file to a routine that
    determines if the file is a valid plugin.
*/
void initialize_plugins()
{
  // Initialize libltdl
  assert(lt_dlinit() == 0);

  // Start with an empty user-defined search path
  assert(lt_dlsetsearchpath("") == 0);

  // Add the compile-time plugin path
  assert(lt_dladdsearchdir(PLUGIN_PATH) == 0);

  // Add the install plugin path
  if(getenv("OPENSS_INSTALL_DIR") != NULL)
  {
    dprintf("OPENSS_INSTALL_DIR set\n");
    QString install_path = QString(getenv("OPENSS_INSTALL_DIR")) +
            QString("/lib/openspeedshop");
    const char *currrent_search_path = lt_dlgetsearchpath();
    if( QString(currrent_search_path).contains(install_path) == 0 )
    {
      assert(lt_dladdsearchdir(install_path.ascii()) == 0);
    }
  }

  // Add the user-specified plugin path
  if(getenv("OPENSS_PLUGIN_PATH") != NULL)
  {
    dprintf("OPENSS_PLUGIN_PATH set\n");
    QString user_specified_path = QString(getenv("OPENSS_PLUGIN_PATH"));
    const char *currrent_search_path = lt_dlgetsearchpath();
    if( QString(currrent_search_path).contains(user_specified_path) == 0 )
    {
      assert(lt_dladdsearchdir(user_specified_path.ascii()) == 0);
    }
  }

  // Now search for collector plugins in all these paths
  lt_dlforeachfile(lt_dlgetsearchpath(), foreachCallback, 0);
}


int
_init()
{
//  printf("Hello from _init() pc_plugin.cpp\n");

  return(1);
}

extern "C"
{
  int ph_init(QWidget *w, PanelContainer *mPC)
  {
    pl = w;
    masterPC = mPC;
    dprintf("hello from ph_init(QWidget *w, PanelContainer *masterPC) oooboy\n");

    initialize_plugins();

    return 1;
  }
}

