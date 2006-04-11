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


  plugin_entry->grouping = "";
  plugin_entry->show_immediate = 0;
  plugin_entry->plugin_description = "This panel allows the user to interface to the \"Customize\" feature of Open|SpeedShop.\nThis panel allows the user to select 2 experiments to compare results (from one run to another).  It also allows the user to compare groups of process/threads from within one executable.";
  plugin_entry->plugin_short_description = "Allows the user to fully customize what's displayed in the StatsPanel.";
  plugin_entry->plugin_name =  "CustomizeStatsPanel.so";
  plugin_entry->plugin_location = "$OPENSS_PLUGIN_PATH";
  plugin_entry->plugin_entry_point = "panel_init";
  plugin_entry->menu_heading =  "";
  plugin_entry->sub_menu_heading =  "";
  plugin_entry->menu_label =  "CustomizeStatsPanel";
  plugin_entry->menu_accel =  " ";
  plugin_entry->panel_type =  "CustomizeStatsPanel";
  plugin_entry->preference_category =  "CustomizeStatsPanel";
  plugin_entry->initialize_preferences_entry_point =  "initialize_preferences_entry_point";
  plugin_entry->save_preferences_entry_point =  "save_preferences_entry_point";
