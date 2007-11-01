////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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


  plugin_entry->grouping = "";
  plugin_entry->show_immediate = 0;
  plugin_entry->plugin_description = "PLUGIN_DESCRIPTION";
  plugin_entry->plugin_short_description = "Displays source files. Use\na left mouse click on the menu icon or use right mouse button\ndown on tab for menu options.";
  plugin_entry->plugin_name =  "SourcePanel.so";
//unused  plugin_entry->plugin_location = "$OPENSS_PLUGIN_PATH";
  plugin_entry->plugin_entry_point = "panel_init";
  plugin_entry->menu_heading =  "&Tools";
  plugin_entry->menu_label =  "Source Panel";
  plugin_entry->menu_accel =  "Alt+J";
  plugin_entry->panel_type =  "Source Panel";
  plugin_entry->preference_category =  "Source Panel";
  plugin_entry->initialize_preferences_entry_point =  "initialize_preferences_entry_point";
  plugin_entry->save_preferences_entry_point =  "save_preferences_entry_point";
