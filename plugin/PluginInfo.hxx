#ifndef PLUGIN_INFO_H
#define PLUGIN_INFO_H

#include <qobject.h>
#include "SlotInfo.hxx"

class Panel;

//! Defines all the fields required for a plugin definition.
class PluginInfo {
  public:
    //! True if panel is toplevel and is shown at initialization.
    int show_immediate;

    //! A short description of the plugin.
    char *plugin_description;
 
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

    //! The actual "words used" in the menu entry.
    char *menu_label;

    //! The actual accelerator used in the menu entry.
    char *menu_accel;

    //! The string representing the panel type.  i.e. "Source Panel"
    char *panel_type;

    /*! This gets called on the main menu callback to create
        and add each type of panel. */
    Panel * (*dl_create_and_add_panel)(void *, void *targetPC=NULL, void *arguments=NULL);

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
