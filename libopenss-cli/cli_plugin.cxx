#include <stdio.h> 

#include <ltdl.h>
#define PLUGIN_INFO_ENTRY_POINT "cli_plugin_init"
#define PLUGIN_DO_CMD_ENTRY_POINT "dl_run_cmd"
#include "cli_PluginInfo.hxx"
#include "CmdClass.hxx"
std::list<CmdClass *> pluginRegistryList;
std::list<cli_PluginInfo *> pluginInfoRegistryList;

int 
run_cli_plugin( const char *plugin_file, char *command_line )
{
// printf("Attempting to open %s\n", plugin_file );

  // We got a plugin location directory and an absoluted path to a plugin.
  // Let's try to load the cli_PluginInfo information.

  lt_dlhandle dl_object = lt_dlopenext((const char *)plugin_file);
  if( dl_object == NULL )
  {
//NOTE: THIS IS THE IMPORTANT DEBUG PRINT
// fprintf(stderr, "DLOPEN: (%s): lt_dlerror()=%s\n", plugin_file, lt_dlerror() );
    return(0);
  }

// printf("We seemed to open the dynamic library (%s) just fine...\n", plugin_file);

// printf("Try to lookup the entry_point (%s) in (%s)\n", PLUGIN_INFO_ENTRY_POINT, plugin_file);

  // We've managed to open the dso.  Can we find the cli_PluginInfo entry point?
  lt_ptr (*dl_run_cmd)(char *);

  dl_run_cmd = (lt_ptr (*)(char *))lt_dlsym(dl_object, PLUGIN_DO_CMD_ENTRY_POINT);
  if( dl_run_cmd == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", PLUGIN_INFO_ENTRY_POINT, plugin_file, lt_dlerror() );
//    fprintf(stderr, "This is not a gui panel plugin... ignore it.\n");
    lt_dlclose(dl_object);
    return(0);
  } else
  {
// printf("\n\n\nFOUND A CLI PLUGIN TO RUN\n\n\n\n");
  }


  lt_ptr i = NULL;
// printf("Run the command!\n");
  i = (*dl_run_cmd)(command_line);


lt_dlclose(dl_object);
  return(0);
}


int
register_cli_plugin(const char *plugin_file)
{
// printf("Attempting to open and register %s\n", plugin_file );

  // We got a plugin location directory and an absoluted path to a plugin.
  // Let's try to load the cli_PluginInfo information.


  lt_dlhandle dl_object = lt_dlopenext((const char *)plugin_file);
  if( dl_object == NULL )
  {
//NOTE: THIS IS THE IMPORTANT DEBUG PRINT
// fprintf(stderr, "DLOPEN: (%s): lt_dlerror()=%s\n", plugin_file, lt_dlerror() );
    return(0);
  }

// printf("We seemed to open the dynamic library (%s) just fine...\n", plugin_file);

// printf("Try to lookup the entry_point (%s) in (%s)\n", PLUGIN_INFO_ENTRY_POINT, plugin_file);

  // We've managed to open the dso.  Can we find the cli_PluginInfo entry point?
  lt_ptr (*dl_cli_plugin_info_init_routine)(void *, std::list<CmdClass *>);

  dl_cli_plugin_info_init_routine = (lt_ptr (*)(void *, std::list<CmdClass *>))lt_dlsym(dl_object, PLUGIN_INFO_ENTRY_POINT);
  if( dl_cli_plugin_info_init_routine == NULL )
  {
//    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", PLUGIN_INFO_ENTRY_POINT, plugin_file, lt_dlerror() );
// fprintf(stderr, "This is not a cli plugin... ignore it.\n");
    lt_dlclose(dl_object);
    return(0);
  } else
  {
// printf("\n\n\nFOUND A CLI PLUGIN! plugin_file=%s\n\n\n\n", plugin_file);
  }



  // Well, we found the plugin entry point, which is suppose to contain
  // information about this plugin.   Let's call it and fill up our 
  // cli_PluginInfo structure with the pertinent information.
  cli_PluginInfo *pluginInfo = new cli_PluginInfo();

  lt_ptr i = NULL;
  i = (*dl_cli_plugin_info_init_routine)((void *)pluginInfo, (std::list<CmdClass *>)pluginRegistryList);

// printf("%s returned %d\n", PLUGIN_INFO_ENTRY_POINT, i);
// pluginInfo->Print();

// printf("Push back the plugInInfoRegistry entry.\n");
  pluginInfoRegistryList.push_back(pluginInfo);


// pluginInfo->Print();


// This is really screwy... If I comment this next line out, I look up the 
// dso's correctly, but the run fails.  If I leave it in, the look up fails, but
// I'm able to run (at least) one of the plugins.
// lt_dlclose(dl_object);

  return(0);
}


int
foreachCLICallback(const char *filename, lt_ptr data)
{
// printf("foreachCLICallback() called for (%s)\n", filename );

// lt_dlinit();
  register_cli_plugin(filename);
// lt_dlexit();

  // Always return zero to insure we keep searching
  return 0;
}


void initialize_cli_plugins()
{
printf("initialize_cli_Plugins() entered\n");

pluginRegistryList.clear();
pluginInfoRegistryList.clear();

  // Insure the libltdl user-defined library search path has been set
//  assert(lt_dlgetsearchpath() != NULL);
  // Now search for panel plugins in this path

  lt_dlforeachfile(lt_dlgetsearchpath(), foreachCLICallback, 0);

}
