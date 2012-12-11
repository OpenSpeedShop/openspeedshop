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
 

/*!  \class SlotInfo

    This class is responsible for managing menu callbacks to create and 
    add new Panels defined by the plugins.


 */

#include "SlotInfo.hxx"
#include "PluginInfo.hxx"

#include "debug.hxx"

/*! This constructor is not called and is only here for completeness. (unused)*/
SlotInfo::SlotInfo()
{
  fprintf(stderr, "SlotInfo::SlotInfo() This constructor should not be called.\n");
  fprintf(stderr, "see: SlotInfo::SlotInfo(QObject *parent, const char *name, PluginInfo *pi);\n");
}

/*! This constructor is the work constructor.   It is called with:
    \param parent is the QtObject the SlotInfo is connected to.
    \param name the name of the QtObject that will handle the menu event.
    \param pi   The PluginInfo.   It cantains the pointer to the 
    plugin entry point which is called with pluginInfo.
 */
SlotInfo::SlotInfo(QObject *parent, const char *name, PluginInfo *pi) : QObject(parent, name)
{
  pluginInfo = pi;
  dprintf("SlotInfo::SlotInfo() entered\n");
}

/*! There's been nothing allocated.  Nothing needs to be deleted. */
SlotInfo::~SlotInfo()
{
}

/*!  This is the callback from the main window's toolbar menu item.
     This will call the dynamic libraries, dl_create_and_add_panel() 
     entry.   The panel should be created and then added to the best 
     panel container available. 
 */
void
SlotInfo::dynamicMenuCallback()
{
  dprintf("SlotInfo::dynamicMenuCallback() entered\n");

  if( pluginInfo == NULL )
  {
    fprintf(stderr, "No pluginInfo, unable to complete dynamic creation of panel.\n");
    return;
  }
  if( pluginInfo->dl_create_and_add_panel == NULL )
  {
    fprintf(stderr, "No create and add panel function to call.  Unable to complete dynamic creation of panel.\n");
    return;
  }
  Panel *p = (*(pluginInfo->dl_create_and_add_panel))((void *)pluginInfo, NULL, NULL, (const char *)NULL);

  if( p == NULL )
  {
    fprintf(stderr, "Failed to start the dynamic panel...\n");
  }
}
