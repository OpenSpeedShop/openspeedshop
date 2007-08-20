////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 Krell Institute All Rights Reserved.
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


  plugin_entry->grouping = "TOPLEVEL";
  plugin_entry->show_immediate = 0;
  plugin_entry->plugin_description = "PLUGIN_DESCRIPTION";
  plugin_entry->plugin_short_description = "Manages the MPIOTF experiment.";
  plugin_entry->plugin_name =  "MPIOTFPanel.so";
  plugin_entry->plugin_location = "$OPENSS_PLUGIN_PATH";
  plugin_entry->plugin_entry_point = "panel_init";
  plugin_entry->menu_heading =  "&File";
  plugin_entry->sub_menu_heading =  "E&xperiments";
  plugin_entry->menu_label =  "MPIOTF";
  plugin_entry->menu_accel =  "Alt+M";
  plugin_entry->panel_type =  "MPIOTF";