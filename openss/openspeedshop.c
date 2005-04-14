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


#include <ltdl.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int
main( int argc, char ** argv ) {

  char *cli_dl_name = getenv("OPENSS_CLI_RELOCATABLE_NAME");
  char *cli_entry_point = getenv("OPENSS_CLI_ENTRY_POINT");
  if( !cli_dl_name ) cli_dl_name = "libopenss-cli";
  if( !cli_entry_point ) cli_entry_point = "cli_init";

  assert(lt_dlinit() == 0);
  // Start with an empty user-defined search path
  assert(lt_dlsetsearchpath("") == 0);
  // Add the user-specified plugin path
  if(getenv("OPENSS_PLUGIN_PATH") != NULL)
  {
    char *user_specified_path = getenv("OPENSS_PLUGIN_PATH");
    const char *currrent_search_path = lt_dlgetsearchpath();
    assert(lt_dladdsearchdir(user_specified_path) == 0);
  }

  // Add the install plugin path
  char *openss_install_dir = getenv("OPENSS_INSTALL_DIR");
  if( openss_install_dir != NULL)
  {
    char *install_path = (char *)calloc(strlen(openss_install_dir)+
                                        strlen("/lib/openspeedshop")+1,
                                        sizeof(char *) );
    strcpy(install_path, openss_install_dir);
    strcat(install_path, "/lib/openspeedshop");
    const char *currrent_search_path = lt_dlgetsearchpath();
    assert(lt_dladdsearchdir(install_path) == 0);
  }

  // Add the compile-time plugin path
  assert(lt_dladdsearchdir(PLUGIN_PATH) == 0);

  // Now search for cli library in these paths
  lt_dlhandle dl_cli_object = lt_dlopenext((const char *)cli_dl_name);
  if( dl_cli_object == NULL ) {
    fprintf(stderr, "cannot: access %s in %s : %s\n", cli_dl_name, lt_dlgetsearchpath(), lt_dlerror() );
    exit(EXIT_FAILURE);
  }

  lt_ptr (*dl_cli_init_routine)(int, char **);
  dl_cli_init_routine = (lt_ptr (*)(int, char **))lt_dlsym(dl_cli_object, cli_entry_point);
  if( dl_cli_init_routine == NULL )
  {
    fprintf(stderr, "Unable to call %s : %s\n", cli_entry_point, lt_dlerror() );
    exit(EXIT_FAILURE);
  }

// printf("calling cli_entry point \n");
  (*dl_cli_init_routine)(argc, argv);

}

