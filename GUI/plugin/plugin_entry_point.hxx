#include "performanceleader.hxx"
#include "PluginInfo.hxx"
#include <qaction.h>

// // #define DEBUG_OUTPUT_REQUESTED 1
#include "debug.hxx"

//! The default entry point for the dynamic library.
/*! This routine is called before the 'Open/SpeedShop' entry point, panel_init().
 */
int
_init()
{
  dprintf("Hello from _init()\n");
 
  return(1);
}

extern "C" 
{
  //! The Open/SpeedShop entry point for the dynamic library.
  /*! This routine is called to initialize the plugin information.
      It loads a structure (PlugingInfo) that contains information 
      about the specific plugin.
      \param pluginInfoArg is a void * that is immediately cast 
                           to type PluginInfo *.   The structure is
                           then assigned to the library header information.
   */
  int plugin_info_init(void *pluginInfoArg, void *topPCArg)
  {
    dprintf("plugin_info_init() entered just fine.\n");
    PluginInfo *plugin_entry = (PluginInfo *)pluginInfoArg;

    if( !plugin_entry )
    {
      fprintf(stderr, "Unable to load plugin header information.\n");
      return 0;
    }

    #include "local_plugin_info.hxx"
    plugin_entry->masterPC = topPCArg;

    return 1;
  }

  //! This routine is called to hook up the plugin.   
  /*! It takes the plugin info (PluginInfo) as input and uses that 
      information to build a menu structure, hook up a SIGNAL/SLOT
      callback, and shows the panel if the plugin info is show immediate.
      \param pl_arg   This void pointer is immediately cast to type
                      PerformanceLeader * which is the pointer to the 
                      main window information.
      \param pluginInfoArg is a void * that is immediately cast to type
                          PluginInfo *, which contains the header information
                          of the dynamic library.
   */
  int
  panel_init(void *pl_arg, void *pluginInfoArg)
  {
    QPopupMenu *menu = NULL;
    PerformanceLeader *pl = (PerformanceLeader *)pl_arg;
    QMenuBar *menubar = pl->menubar;

    dprintf("Hello from panel_init()\n");

    PluginInfo *pluginInfo = (PluginInfo *)pluginInfoArg;


    if( pluginInfo->menu_heading == NULL ||
        strcmp(pluginInfo->menu_heading, "")  == 0 )
    {
      dprintf("this is a hidden entry.   Load it, but there's no menu.  It will be called directly.\n");
      return 1;
    }

    // Loop through the menu headers, trying to find a place to drop
    // this entry.   If we find one, append the menu_label to the end
    // of the list.   Otherwise, create a new menu_header, then append 
    // the name.
    int count = menubar->count();
    QString menu_text = NULL;
    int i = 0;
    bool found=FALSE;

    dprintf("menubar->count()=%d\n", menubar->count() );

    // Menu's off the menubar are 1 based
    for( i=1;i<=count;i++ )
    {
      QMenuItem *item = menubar->findItem(i);
      if( item )
      {
        menu_text = item->text();
      }

      if( menu_text )
      {
        dprintf("menu_text(%d)=%s\n", i, menu_text.ascii() );
      } else
      {
        dprintf("no menu_text at position %d\n", i );
      }

      dprintf("menu_text(%d)=(%s) menu_heading=(%s)\n", i, menu_text.ascii(), pluginInfo->menu_heading );
      if( menu_text == pluginInfo->menu_heading )
      {
        dprintf("Found %s at %d\n", pluginInfo->menu_heading, i );
        found = TRUE;
        if( item )
        {
          menu = item->popup();
        }
        break;
      }
    }

    QAction *action = new QAction( pl, pluginInfo->menu_label);

    if( !found || !menu )
    {
      dprintf("no menu found, create a new entry.\n");
      menu = new QPopupMenu( pl );
      {
      char n[1024]; strcpy(n,"plugin menu:");strcat(n, pluginInfo->menu_label);
      menu->setCaption(n);
      }
      int id = menubar->insertItem( pluginInfo->menu_heading, menu, i);
    }

    action->addTo( menu );
//    action->setText( pluginInfo->menu_label );
    action->setMenuText( pluginInfo->menu_label );
    dprintf("assign the accelerator to %s\n", pluginInfo->menu_accel );
    action->setAccel( QString(pluginInfo->menu_accel) );
  
    pluginInfo->slotInfo->connect( action,
      SIGNAL( activated() ), pluginInfo->slotInfo,
        SLOT( dynamicMenuCallback() ));

    // If the panels are toplevel panels and are marked to show
    // create and show them right now.
    if( pluginInfo->show_immediate == 1 )
    {
      dprintf("This panel is a top level grouping panel.\n");
      dprintf("Initialize it right away, then continue the menu addition...\n");
      extern Panel * create_and_add_panel(void *, void *);
      create_and_add_panel(pluginInfoArg, (void *)pluginInfo->masterPC);
    }

    dprintf("panel_init(success) returning\n");
    return(1);
  }

  //! This is the routine that creates and adds the panel.
  /*! It us usaually called from the result of a menu callback, but can 
      be called at initialization from panel_init() as well.
      What happens is, from the plugin info information a panel container
      with the name of "grouping" is attempted to be located.   If one is
      found the new Panel will be added to that PanelContainer.   Otherwise
      the Panel will be dropped on the very top level PanelContainer.
      \param pluginInfoArg  The plugin information.   This is really of 
                            type PluginInfo * and is immediately cast to that
                            type.
   */
//  int
  Panel *
  create_and_add_panel(void *pluginInfoArg, void *tPC)
  {
    PanelContainer *targetPC = (PanelContainer *)tPC;
    PluginInfo *pluginInfo = (PluginInfo *)pluginInfoArg;

    PanelContainer *local_masterPC = (PanelContainer *)pluginInfo->masterPC;

//    printf("Hello from create_and_add_panel()\n");

    // FIX:
    // This needs to be pushed to a 
    // smarter routine that knows how to add panels without the need for 
    // a specific panel container.
    PANEL_CLASS_NAME *plugin_panel = NULL;
    dprintf("about to create the panel: pluginInfo->menu_label=(%s) plugin->grouping=(%s)\n", pluginInfo->menu_label, pluginInfo->grouping );
    if( targetPC == NULL )
    {
      if( strcmp(pluginInfo->grouping, "TOPLEVEL") == 0 )
      {
        // If we have already split the pc:0 then we need to look for a best
        // fit.
        if( local_masterPC->leftPanelContainer ||
            local_masterPC->rightPanelContainer )
        {
          targetPC = local_masterPC->findBestFitPanelContainer(local_masterPC);
        } else
        {  // Otherwise, we just drop it on top.
          targetPC = local_masterPC;
        }
      } else
      {
        targetPC =
          local_masterPC->findPanelContainerWithNamedPanel(pluginInfo->grouping);
        targetPC = local_masterPC->findBestFitPanelContainer(targetPC);
      }
      if( targetPC == NULL )
      {
        targetPC = local_masterPC;
      }
    }
    char *name = pluginInfo->menu_label;
    if( !name )
    {
      name = pluginInfo->panel_type;
    }
    if( !name )
    {
      name = "Warning: Unamed Panel: No menu_label or panel_type.";
    }
fprintf(stderr, "name=(%s)\n", name);
    plugin_panel = new PANEL_CLASS_NAME(targetPC, name);
    targetPC->addPanel((Panel *)plugin_panel, targetPC, name);
    
    return(plugin_panel);
  }
}
