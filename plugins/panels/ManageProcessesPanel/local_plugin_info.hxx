////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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


  plugin_entry->grouping = (char *) "";
  plugin_entry->show_immediate = 0;
  plugin_entry->plugin_description = (char *) "Shows processes/threads/collectors known to experiment.";
  plugin_entry->plugin_short_description = (char *) "Shows processes/threads/collectors known to experiment.  Use\na left mouse click on the menu icon or use right mouse button\ndown on tab for menu options.";
  plugin_entry->plugin_name = (char *) "ManageProcessesPanel.so";
  plugin_entry->plugin_location = (char *) "$OPENSS_PLUGIN_PATH";
  plugin_entry->plugin_entry_point = (char *) "panel_init";
  plugin_entry->menu_heading = (char *) "";
  plugin_entry->sub_menu_heading = (char *) "";
  plugin_entry->menu_label = (char *) "ManageProcessesPanel";
  plugin_entry->menu_accel = (char *) " ";
  plugin_entry->panel_type = (char *) "ManageProcessesPanel";
  plugin_entry->preference_category = (char *) "ManageProcessesPanel";
  plugin_entry->initialize_preferences_entry_point = (char *) "initialize_preferences_entry_point";
  plugin_entry->save_preferences_entry_point = (char *) "save_preferences_entry_point";
