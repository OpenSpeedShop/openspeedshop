#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
int
main( int argc, char ** argv ) {
  char gui_plugin_file[2048];
  char *gui_dl_name = getenv("OPENSPEEDSHOP_RELOCATABLE_NAME");
  char *gui_entry_point = getenv("OPENSPEEDSHOP_ENTRY_POINT");
  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");

  if( !plugin_directory ) exit(EXIT_FAILURE);

  sprintf(gui_plugin_file, "%s/%s", plugin_directory, gui_dl_name);
  void *dl_gui_object = dlopen((const char *)gui_plugin_file, (int)RTLD_LAZY );
  if( !dl_gui_object ) {
    fprintf(stderr, "%s\n", dlerror() );
    exit(EXIT_FAILURE);
  }

  int (*dl_gui_init_routine)(int, char **, int);
  dl_gui_init_routine = (int (*)(int, char **, int))dlsym(dl_gui_object, gui_entry_point);
  if( dl_gui_init_routine == NULL )
  {
    fprintf(stderr, "%s\n", dlerror() );
    exit(EXIT_FAILURE);
  }

  (*dl_gui_init_routine)(argc, argv, 0);
}
