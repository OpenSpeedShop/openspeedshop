/*! 
    This file is responsible for loading all plugins at initialization time.
    A directory pointed at the environment vairiable FUTURE_TOOL_PLUGIN_DIR
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
#include <dlfcn.h>
#include <errno.h>
#include <time.h>

#ifdef LTDL_H
#include <ltdl.h>
#endif // LTDL_H

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

#include "PanelContainer.hxx"
/*! This routine opens each plugin and looks for a predefined entry point.
    If the entry point exists, then the plugin information is loaded that 
    describes this plugin.   NOTE: This is only for the panel plugin entries.
*/
  
int
register_plugin(QWidget *pl, char *plugin_file, PanelContainer *masterPC)
{
  dprintf("Attempting to open %s\n", plugin_file );

  // We got a plugin location directory and an absoluted path to a plugin.
  // Let's try to load the PluginInfo information.

#ifdef LTDL_H
  lt_dlhandle *dl_object = lt_dlopen((const char *)plugin_file, (int)RTLD_LAZY );
#else // LTDL_H
  void *dl_object = dlopen((const char *)plugin_file, (int)RTLD_LAZY );
#endif // LTDL_H

  if( !dl_object )
  {
    fprintf(stderr, "(%s): dlerror()=%s\n", plugin_file, dlerror() );
    return(0);
  }

  dprintf("We seemed to open the dynamic library (%s) just fine...\n", plugin_file);

  dprintf("Try to lookup the entry_point (%s)\n", plugin_file);

  // We've managed to open the dso.  Can we the PluginInfo entry point?
#ifdef LTDL_H
  lt_ptr (*dl_plugin_info_init_routine)(void *, void *);

  dl_plugin_info_init_routine = (int (*)(void *, void *))lt_dlsym(dl_object, PLUGIN_INFO_ENTRY_POINT);
#else // LTDL_H
  int (*dl_plugin_info_init_routine)(void *, void *);

  dl_plugin_info_init_routine = (int (*)(void *, void *))dlsym(dl_object, PLUGIN_INFO_ENTRY_POINT);
#endif // LTDL_H
  if( dl_plugin_info_init_routine == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", PLUGIN_INFO_ENTRY_POINT, plugin_file, dlerror() );
// This is not a gui panel plugin... ignore it.
    return(0);
  }

  PluginInfo *pluginInfo = new PluginInfo((QObject *)pl);
  // Well, we found the plugin entry point, which is suppose to contain
  // information about this plugin.   Let's call it and fill up our 
  // PluginInfo structure with the pertinent information.
#ifdef LTDL_H
  lt_ptr i = (*dl_plugin_info_init_routine)((void *)pluginInfo, (void *)masterPC);
#else // LTDL_H
  int i = (*dl_plugin_info_init_routine)((void *)pluginInfo, (void *)masterPC);
#endif // LTDL_H

  dprintf("%s returned %d\n", PLUGIN_INFO_ENTRY_POINT, i);


//  pluginInfo->Print();

  // We've managed to open the dso and initialize the plugin description 
  // information, from the dso.  Now from the plugin description, can we
  // find the defined entry point?
  dprintf("Try to lookup the entry_point (%s)\n", pluginInfo->plugin_entry_point);

#ifdef LTDL_H
  lt_ptr (*dl_routine)(void *, void *);

  dl_routine = (int (*)(void *, void *))lt_dlsym(dl_object, pluginInfo->plugin_entry_point);
#else // LTDL_H
  int (*dl_routine)(void *, void *);

  dl_routine = (int (*)(void *, void *))dlsym(dl_object, pluginInfo->plugin_entry_point);
#endif // LTDL_H

  if( dl_routine == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found dlerror()=%s\n", pluginInfo->plugin_entry_point, dlerror() );
    delete pluginInfo;   // Don't forget to delete this on an early return.
    return 0;
  }

  // Well, we found the entry point and everything seems to match.  Now
  // call the entry point routine passing it a couple of void pointers (the
  // called routine will need to cast these appropriately.)
  // The first parameter is to the parent tools, parent class.   The second
  // parameter is a PluginInfo object, which contains all the know
  // information about the plugin that is being loaded.
  dprintf("\tlibdso: dynamic routine returned %d\n", i);

  i = (*dl_routine)((void *)pl, (void *)pluginInfo);
  pluginInfo->dl_create_and_add_panel = (Panel * (*)(void *, void *))dlsym(dl_object, "create_and_add_panel" );
  if( pluginInfo->dl_create_and_add_panel == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found dlerror()=%s\n", pluginInfo->plugin_entry_point, dlerror() );
    fprintf(stderr, "Will not be able to create_and_add_panel() for:\n");
    pluginInfo->Print();
    i = -1;
  }

  if( i == -1 )
  {
    return(-1);
  } else
  {
    if( masterPC->_pluginRegistryList == NULL )
    {
      masterPC->_pluginRegistryList = new PluginRegistryList;
      masterPC->_pluginRegistryList->clear();
    }
    masterPC->_pluginRegistryList->push_back(pluginInfo);
    return(0);
  }
}

struct stat Statbuf;
#define exists(file)           (stat(file,&Statbuf)<0 ? 0:Statbuf.st_mode)
/*! This routine checks the plugin directory ($FUTURE_TOOL_PLUGIN_DIR) for
    plugin files.   It loops over and shovels each file to a routine that
    determines if the file is a valid plugin.
*/
void initialize_plugins(QWidget *pl, PanelContainer *masterPC)
{
  //This is the base plugin directory.   In this directory there should
  // be a list of dso (.so) which are the plugins.
  plugin_directory = getenv("FUTURE_TOOL_PLUGIN_DIR");
  
  // Check for a plugin environment variable
  if( !plugin_directory )
  {
    fprintf(stderr, "There are no plug-ins.   $FUTURE_TOOL_PLUGIN_DIR not set.\n");
    exit(1);
  }

  // If no plugin list directory is available, return after printing error.
  if( !exists(plugin_directory) )
  {
    fprintf(stderr, "Plugin directory does not exist.  Check $FUTURE_TOOL_PLUGIN_DIR variable.\n");
//    exit(1);
    return;
  }

  // Go and get a list of all the plugin files.
  int glob_ret_value = 0;
  char target[1024];
  char *pattern = "*Panel.so";
  sprintf(target, "%s/%s", plugin_directory, pattern);
//  int flags = GLOB_NOSORT | GLOB_ERR | GLOB_MARK | GLOB_TILDE;
  int flags = GLOB_ERR | GLOB_MARK | GLOB_TILDE;
  glob_t *pglob = new glob_t;

  // Get all the Panel (plugin) files "*Panel.so".  
  // These are the plugin files...
  glob_ret_value = glob(target, flags, 0, pglob);
  if( glob_ret_value != 0 )
  {
    fprintf(stderr, "Error: reading list of plugins in %s\n", target);
//    exit(1);
    return;
  }

  pattern = "ft*.so";
  // Append all the internal libraries.  
  flags = GLOB_ERR | GLOB_MARK | GLOB_TILDE | GLOB_APPEND;
  glob_ret_value = glob(target, flags, 0, pglob);
  if( glob_ret_value != 0 )
  {
    fprintf(stderr, "Error: reading list of plugins in %s\n", target);
//    exit(1);
    return;
  }


  // If no plugin files are located in the path, return after printing error.
  if( pglob->gl_pathc == 0 )
  {
    fprintf(stderr, "Error: there are no plugins available in $FUTURE_TOOL_PLUGIN_DIR.\n");
//    exit(1);
    return;
  }

  // here's your list of plugins.... 
  unsigned int i = 0;
  for(i=0;i<pglob->gl_pathc;i++)
  {
// printf("processing plugin record (%s)\n", pglob->gl_pathv[i] );
    if( register_plugin(pl, pglob->gl_pathv[i], masterPC) == -1 )
    {
      fprintf(stderr, "Error processing the plugin record (%s)\n", pglob->gl_pathv[i] );
      fprintf(stderr, "Plugin record: \n");
    }
  }
  globfree(pglob);
}


#include <stdlib.h>
#include <stdio.h>

#include "PanelContainer.hxx"

#include <qwidget.h>
#include <qlayout.h>

class PanelContainer;
PanelContainerList *panelContainerList;

int
_init()
{
//  printf("Hello from _init() pc_plugin.cpp\n");

  return(1);
}

extern "C"
{
  int ph_init(QWidget *w, PanelContainer *masterPC)
  {
//    printf("hello from ph_init(QWidget *w, PanelContainer *masterPC) oooboy\n");

    initialize_plugins(w, masterPC);

    return 1;
  }
}

