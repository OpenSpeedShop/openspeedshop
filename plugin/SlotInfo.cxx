/*!  \class SlotInfo

    This class is responsible for managing menu callbacks to create and 
    add new Panels defined by the plugins.


    Author: Al Stipek   (stipek@sgi.com)
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
  Panel *p = (*(pluginInfo->dl_create_and_add_panel))((void *)pluginInfo);

  if( p == NULL )
  {
    fprintf(stderr, "Failed to start the dynamic panel...\n");
  }
}
