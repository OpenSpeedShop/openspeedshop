#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
int
main( int argc, char ** argv ) {
  char cli_plugin_file[2048];
  char *cli_dl_name = getenv("OPENSPEEDSHOP_CLI_RELOCATABLE_NAME");
  char *cli_entry_point = getenv("OPENSPEEDSHOP_CLI_ENTRY_POINT");
  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");

  if( !plugin_directory ) exit(EXIT_FAILURE);
  if( !cli_dl_name ) cli_dl_name = "posscli.so";
  if( !cli_entry_point ) cli_entry_point = "cli_init";

  sprintf(cli_plugin_file, "%s/%s", plugin_directory, cli_dl_name);
  void *dl_cli_object = dlopen((const char *)cli_plugin_file, (int)RTLD_LAZY );
  if( !dl_cli_object ) {
    fprintf(stderr, "%s\n", dlerror() );
    exit(EXIT_FAILURE);
  }

  int (*dl_cli_init_routine)(int, char **);
  dl_cli_init_routine = (int (*)(int, char **))dlsym(dl_cli_object, cli_entry_point);
  if( dl_cli_init_routine == NULL )
  {
    fprintf(stderr, "%s\n", dlerror() );
    exit(EXIT_FAILURE);
  }

  (*dl_cli_init_routine)(argc, argv);
}
