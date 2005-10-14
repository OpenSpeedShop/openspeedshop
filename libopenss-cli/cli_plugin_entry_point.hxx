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
 
#undef PLUGIN_CLASS_NAME

#include <stdio.h>
#include "cli_PluginInfo.hxx"
#include "CmdClass.hxx"


#include <unistd.h>
#include <stdio.h>
#include <list>
#include <inttypes.h>
#include <stdexcept>
#include <string>

#include <vector>
#include <iostream>

#include "CmdNameHeader.hxx"

//! The default entry point for the dynamic library.
/*! This routine is called before the 'Open|SpeedShop' entry point, panel_init().
 */
int
_init()
{
// printf("Hello from _init()\n");
 
  return(1);
}

extern "C" 
{
  int cli_plugin_init(void *pluginInfoArg, std::list<CmdClass *>blah)
  {
// fprintf(stderr, "                               cli_plugin_info_init() entered just fine.\n");
    cli_PluginInfo *plugin_entry = (cli_PluginInfo *)pluginInfoArg;

    if( !plugin_entry )
    {
      fprintf(stderr, "Unable to load plugin header information.\n");
      return 0;
    }

    #include "local_plugin_info.hxx"


   extern CmdClass *create_and_add_cmd(void *);
   create_and_add_cmd(pluginInfoArg);
   

    return 1;
  }

  CmdClass *
  create_and_add_cmd(void *pluginInfoArg)
  {
// printf("create_and_add_cmd() entrered\n");
    CmdClass *cc = new PLUGIN_CLASS_NAME();
// printf("Do a new on your plugin command (%s)\n", cc->cmd_name.c_str() );

    extern std::list<CmdClass *> pluginRegistryList;
    pluginRegistryList.push_back(cc);

    return (cc);
  }


  bool
  dl_run_cmd(char *command)
  {
// printf("dl_run_command\n");
    // look up the class name == PLUGIN_CLASS_NAME();
    // Then: (PLUGIN_CLASS_NAME)cc->parseCmd() ...
PLUGIN_CLASS_NAME *c = new PLUGIN_CLASS_NAME();
c->parseCmd((const char *)command);
c->doCmd((const char *)command);

  }
}
