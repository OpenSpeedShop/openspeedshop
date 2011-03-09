////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 The Krell Institute All Rights Reserved.
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


  plugin_entry->grouping = ( char *) "Performance";
  plugin_entry->show_immediate = 0;
  plugin_entry->plugin_description = ( char *) "PLUGIN_DESCRIPTION";
  plugin_entry->plugin_short_description = ( char *) "Enter interactive commands in this window.\nType \"help\" for general help and \n\"help commands\" for available commands.\n\nThis panel cannot be deleted.\nAttempting to delete this panel\nwill result in it being placed in\nthe main window's panel.";
  plugin_entry->plugin_name = (char *) "CmdPanel.so";
  plugin_entry->plugin_location = ( char *) "$OPENSS_PLUGIN_PATH";
  plugin_entry->plugin_entry_point = ( char *) "panel_init";
  plugin_entry->menu_heading = (char *) "&Tools";
  plugin_entry->menu_label = (char *) "&Command Panel";
  plugin_entry->menu_accel = (char *) "Alt+P";
  plugin_entry->panel_type = (char *) "Command Panel";
