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


#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
int
main( int argc, char ** argv ) {
  char cli_plugin_file[2048];
  char *cli_dl_name = getenv("OPENSS_CLI_RELOCATABLE_NAME");
  char *cli_entry_point = getenv("OPENSS_CLI_ENTRY_POINT");
  char *plugin_directory = getenv("OPENSS_PLUGIN_PATH");

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
