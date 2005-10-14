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


/*!  \class cli_PluginInfo

    This class is responsible for managing the plugins.   Plugins are 
    loaded and this class is populated with pertinent plugin information.

    It also has a pointer to the SlotInfo class to hook up dynamic runtime
    menus.


    Author: Al Stipek   (stipek@sgi.com)
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>

#include "debug.hxx"  // This includes the definition of dprintf

#include "cli_PluginInfo.hxx"


/*! This routine simply initializes all the member variables. */
void
cli_PluginInfo::initialize()
{
  plugin_name=NULL;
  plugin_command_name=NULL;
  plugin_location=NULL;
  plugin_entry_point=NULL;

//  dl_create_and_add_panel = NULL;
  dl_run_cmd = NULL;
}

/*! The default constructor. */
cli_PluginInfo::cli_PluginInfo()
{
  initialize();
}

/*! The default destructor */
cli_PluginInfo::~cli_PluginInfo()
{
  if( plugin_name ) free( plugin_name );
  if( plugin_command_name ) free( plugin_command_name );
  if( plugin_location ) free( plugin_location );
  if( plugin_entry_point ) free( plugin_entry_point );
}

/*! A debug routine to dump out the class' information. */
void
cli_PluginInfo::Print()
{
  printf("plugin_name=%s\n", plugin_name ? plugin_name : "NO NAME");
  printf("plugin_command_name=%s\n", plugin_command_name ? plugin_command_name : "NO COMMAND NAME");
  printf("plugin_location=%s\n", plugin_location ? plugin_location : "NO LOCATION");
  printf("plugin_entry_point=%s\n", plugin_entry_point ? plugin_entry_point : "NO ENTRY POINT" );
}
