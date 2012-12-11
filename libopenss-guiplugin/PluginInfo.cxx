////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


/*!  \class PluginInfo

    This class is responsible for managing the plugins.   Plugins are 
    loaded and this class is populated with pertinent plugin information.

    It also has a pointer to the SlotInfo class to hook up dynamic runtime
    menus.


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

#include "PluginInfo.hxx"


/*! This routine simply initializes all the member variables. */
void
PluginInfo::initialize()
{
  pl=NULL;
  show_immediate=0;
  plugin_description=NULL;
  plugin_short_description=NULL;
  plugin_name=NULL;
  plugin_location=NULL;
  plugin_entry_point=NULL;
  initialize_preferences_entry_point=NULL;

  grouping=NULL;
  menu_heading=NULL;
  sub_menu_heading=NULL;
  menu_label=NULL;
  menu_accel=NULL;
  panel_type=NULL;
  slotInfo = NULL;

  preference_category=NULL;
  initialize_preferences_entry_point=NULL;
  save_preferences_entry_point=NULL;

  dl_create_and_add_panel = NULL;
}

/*! The default constructor. */
PluginInfo::PluginInfo()
{
  initialize();
}

/*! The work constructor.
    This constructor initializes the member variables, then creates a 
    new slot for the menu hooks. */
PluginInfo::PluginInfo(QObject *parent)
{
  initialize();
  slotInfo = new SlotInfo(parent, "slotInfo", this);
  pl = (void *)parent;
     
}

/*! The default destructor */
PluginInfo::~PluginInfo()
{
/* 
  if( plugin_description ) free( plugin_description );
  if( plugin_short_description ) free( plugin_short_description );
  if( plugin_name ) free( plugin_name );
  if( plugin_location ) free( plugin_location );
  if( plugin_entry_point ) free( plugin_entry_point );
  if( grouping ) free( grouping );
  if( menu_heading ) free( menu_heading );
  if( sub_menu_heading ) free( sub_menu_heading );
  if( menu_label ) free( menu_label );
  if( menu_accel ) free( menu_accel );
  if( panel_type ) free( panel_type );
  if( preference_category ) free( preference_category );
  if( initialize_preferences_entry_point ) free( initialize_preferences_entry_point );
  if( save_preferences_entry_point ) free( save_preferences_entry_point );
*/
}

/*! A debug routine to dump out the class' information. */
void
PluginInfo::Print()
{
  printf("plugin_description=%s\n", plugin_description);
  printf("plugin_short_description=%s\n", plugin_short_description);
  printf("plugin_name=%s\n", plugin_name );
  printf("plugin_location=%s\n", plugin_location );
  printf("plugin_entry_point=%s\n", plugin_entry_point );
  printf("grouping=%s\n", grouping );
  printf("menu_heading=%s\n", menu_heading );
  printf("sub_menu_heading=%s\n", sub_menu_heading );
  printf("menu_label=%s\n", menu_label );
  printf("menu_accel=%s\n", menu_accel );
  printf("panel_type=%s\n", panel_type );
  printf("preference_category=%s\n", preference_category );
  printf("initialize_preferences_entry_point=%s\n", initialize_preferences_entry_point );
  printf("save_preferences_entry_point=%s\n", save_preferences_entry_point );
}
