/*!  \class PluginInfo

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

#include "PluginInfo.hxx"


/*! This routine simply initializes all the member variables. */
void
PluginInfo::initialize()
{
  pl=NULL;
  show_immediate=0;
  plugin_description=NULL;
  plugin_name=NULL;
  plugin_location=NULL;
  plugin_entry_point=NULL;
  grouping=NULL;
  menu_heading=NULL;
  menu_label=NULL;
  menu_accel=NULL;
  panel_type=NULL;
  slotInfo = NULL;
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
  if( plugin_name ) free( plugin_name );
  if( plugin_location ) free( plugin_location );
  if( plugin_entry_point ) free( plugin_entry_point );
  if( grouping ) free( grouping );
  if( menu_heading ) free( menu_heading );
  if( menu_label ) free( menu_label );
  if( menu_accel ) free( menu_accel );
  if( panel_type ) free( panel_type );
*/
}

/*! A debug routine to dump out the class' information. */
void
PluginInfo::Print()
{
  printf("plugin_description=%s\n", plugin_description);
  printf("plugin_name=%s\n", plugin_name );
  printf("plugin_location=%s\n", plugin_location );
  printf("plugin_entry_point=%s\n", plugin_entry_point );
  printf("grouping=%s\n", grouping );
  printf("menu_heading=%s\n", menu_heading );
  printf("menu_label=%s\n", menu_label );
  printf("menu_accel=%s\n", menu_accel );
  printf("panel_type=%s\n", panel_type );
}
