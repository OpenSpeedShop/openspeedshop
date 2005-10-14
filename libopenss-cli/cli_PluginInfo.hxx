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
 

#ifndef cli_PLUGIN_INFO_H
#define cli_PLUGIN_INFO_H

//#include <qobject.h>
// #include "SlotInfo.hxx"

// class CmdClass;
class Panel;
class ArgumentObject;

//! Defines all the fields required for a plugin definition.
class cli_PluginInfo {
  public:
    //! The name of the plugin executable
    char *plugin_name;

    //! The name of the plugin command
    char *plugin_command_name;

    //! The location directory of the plugin executable.
    char *plugin_location;

    //! The plugin (dso) entry point
    char *plugin_entry_point;

    //! The plugin (dso) worker point
    char *plugin_do_cmd_entry_point;

    /*! This gets called on the main menu callback to create
        and add each type of panel. */
//    Panel * (*dl_create_and_add_panel)(void *, void *, ArgumentObject *);

/*! This gets called when the command is to be parsed and run. */
// CmdClass * (*dl_run_cmd)(void *, void *, ArgumentObject *);
bool (*dl_run_cmd)(char *);

    //! The work constructor 
    cli_PluginInfo();

    //! Destructor.
    ~cli_PluginInfo();

    //! When debugging, the will print out the cli_PluginInfo information.
    void Print();

    //! This initializes the member data.
    void initialize();
};
#endif // cli_PLUGIN_INFO_H
