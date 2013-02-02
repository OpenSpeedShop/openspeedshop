////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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
 

#ifndef PLUGIN_INFO_H
#define PLUGIN_INFO_H

#include <stddef.h>
#include <qobject.h>
#include "SlotInfo.hxx"

class Panel;
class ArgumentObject;

//! Defines all the fields required for a plugin definition.
class PluginInfo {
  public:
    //! True if panel is toplevel and is shown at initialization.
    int show_immediate;

    //! A description of the plugin.
    char *plugin_description;

    //! A short description of the plugin.
    char *plugin_short_description;
 
    //! The name of the plugin executable
    char *plugin_name;

    //! The location directory of the plugin executable.
    char *plugin_location;

    //! The plugin (dso) entry point
    char *plugin_entry_point;

    //! Hierarch stuff.  Which group should this parented under
    char *grouping;

    //! The name of which menu to add this entry to.
    char *menu_heading;

    //! The name of which sub_menu to add this entry to.
    char *sub_menu_heading;

    //! The actual "words used" in the menu entry.
    char *menu_label;

    //! The actual accelerator used in the menu entry.
    char *menu_accel;

    //! The string representing the panel type.  i.e. "Source Panel"
    char *panel_type;

    /*! This gets called on the main menu callback to create
        and add each type of panel. */
    Panel * (*dl_create_and_add_panel)(void *, void *, ArgumentObject *, const char *);

    //! The plugin preference category listing.
    char *preference_category;

    //! The plugin initialize preferences entry point.
    char *initialize_preferences_entry_point;

    //! The plugin save preferences entry point.
    char *save_preferences_entry_point;

    //! This will handle the menu callback.
    SlotInfo *slotInfo;

    //! void * pointer to PerformanceLeader NOTE: cast before using
    void *pl;

    //! void * pointer to top PanelContainer NOTE: cast before using
    void *masterPC;


    //! PluginInfo() -  A default constructor - not called.
    PluginInfo();

    //! The work constructor 
    PluginInfo(QObject *parent);

    //! Destructor.
    ~PluginInfo();

    //! When debugging, the will print out the PluginInfo information.
    void Print();

    //! This initializes the member data.
    void initialize();
};
#endif // PLUGIN_INFO_H
