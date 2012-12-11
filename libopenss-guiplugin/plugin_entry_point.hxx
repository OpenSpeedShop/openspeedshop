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
 
// Debug Flag
//#define DEBUG_PI 1
//

#include "openspeedshop.hxx"
#include "PluginInfo.hxx"
#include <qaction.h>
#include <qobject.h>

class ArgumentObject;

//#define DEBUG_OUTPUT_REQUESTED 1
#include "debug.hxx"

//! The default entry point for the dynamic library.
/*! This routine is called before the 'Open|SpeedShop' entry point, panel_init().
 */
int
_init()
{
  dprintf("Hello from _init()\n");
 
  return(1);
}

extern "C" 
{
  //! The Open|SpeedShop entry point for the dynamic library.
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
#ifdef DEBUG_PI
    printf("plugin_info_init() entered just fine.\n");
#endif
    PluginInfo *plugin_entry = (PluginInfo *)pluginInfoArg;

    if( !plugin_entry )
    {
      fprintf(stderr, "Unable to load plugin header information.\n");
#ifdef DEBUG_PI
      printf("plugin_info_init() return 0 - first Unable to load plugin header information.\n");
#endif
      return 0;
    }

    #include "local_plugin_info.hxx"
    plugin_entry->masterPC = topPCArg;

#ifdef DEBUG_PI
    // printf type output
//    plugin_entry->Print();
    printf("plugin_info_init() return 1 - first\n");
#endif
    return 1;
  }

  //! This routine is called to hook up the plugin.   
  /*! It takes the plugin info (PluginInfo) as input and uses that 
      information to build a menu structure, hook up a SIGNAL/SLOT
      callback, and shows the panel if the plugin info is show immediate.
      \param pl_arg   This void pointer is immediately cast to type
                      OpenSpeedshop * which is the pointer to the 
                      main window information.
      \param pluginInfoArg is a void * that is immediately cast to type
                          PluginInfo *, which contains the header information
                          of the dynamic library.
   */
  int
  panel_init(void *pl_arg, void *pluginInfoArg)
  {
dprintf("  panel_init() entered\n");
    QPopupMenu *menu = NULL;
    QPopupMenu *new_menu = NULL;
    OpenSpeedshop *pl = (OpenSpeedshop *)pl_arg;
    QMenuBar *menubar = pl->menubar;

    dprintf("Hello from panel_init()\n");
#ifdef DEBUG_PI
    printf("Hello from panel_init()\n");
#endif

    PluginInfo *pluginInfo = (PluginInfo *)pluginInfoArg;


    if( pluginInfo->menu_heading == NULL ||
        strcmp(pluginInfo->menu_heading, "")  == 0 )
    {
      dprintf("this is a hidden entry.   Load it, but there's no menu.  It will be called directly.\n");
#ifdef DEBUG_PI
      printf("panel_init, this is a hidden entry.   Load it, but there's no menu.  It will be called directly.\n");
#endif
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

    dprintf("pluginInfo->panel_type=(%s) menubar->count()=%d\n", pluginInfo->panel_type, menubar->count() );
#ifdef DEBUG_PI
    printf("panel_init, pluginInfo->panel_type=(%s) menubar->count()=%d\n", pluginInfo->panel_type, menubar->count() );
#endif

    // Menu's off the menubar are 1 based
    for( i=1;i<=count;i++ )
    {
      QMenuItem *item = menubar->findItem(i);
      if( item )
      {
        menu_text = item->text();
      }

      if( menu_text.isEmpty() )
      {
        dprintf("no menu_text at position %d\n", i );
#ifdef DEBUG_PI
        printf("no menu_text at position %d\n", i );
#endif
      } else
      {
        dprintf("menu_text(%d)=%s\n", i, menu_text.ascii() );
#ifdef DEBUG_PI
        printf("panel_init, menu_text(%d)=%s\n", i, menu_text.ascii() );
#endif
      }

        dprintf("menu_text(%d)=(%s) menu_heading=(%s)\n", i, menu_text.ascii(), pluginInfo->menu_heading );
#ifdef DEBUG_PI
        printf("panel_init, menu_text(%d)=(%s) menu_heading=(%s)\n", i, menu_text.ascii(), pluginInfo->menu_heading );
#endif
      if( menu_text == pluginInfo->menu_heading )
      {
        dprintf("Found %s at %d for %s\n", pluginInfo->menu_heading, i, pluginInfo->panel_type );
#ifdef DEBUG_PI
        printf("panel_init, Found %s at %d for %s\n", pluginInfo->menu_heading, i, pluginInfo->panel_type );
        // printf type output
//        pluginInfo->Print();
#endif

        found = TRUE;
        if(  pluginInfo->sub_menu_heading == NULL )
        {
          if( item )
          {
            menu = item->popup();
          }
          break;
        } else
        {
          int si = 1;
dprintf("Found %s now look for sub_menu_heading %s\n", pluginInfo->menu_heading, pluginInfo->sub_menu_heading );
#ifdef DEBUG_PI
printf("panel_init, Found %s now look for sub_menu_heading %s\n", pluginInfo->menu_heading, pluginInfo->sub_menu_heading );
#endif
          QPopupMenu *sub_menu = NULL;
dprintf("item =0x%x\n", item );
#ifdef DEBUG_PI
printf("panel_init, item =0x%x\n", item );
#endif
          if( item )
          {
            sub_menu = item->popup();
            int sub_count = sub_menu->count();
dprintf("sub_menu->count()=%d\n", sub_menu->count() );
#ifdef DEBUG_PI
printf("panel_init, sub_menu->count()=%d\n", sub_menu->count() );
#endif
dprintf("item->text()=%s\n", item->text().ascii() );
#ifdef DEBUG_PI
printf("panel_init, item->text()=%s\n", item->text().ascii() );
#endif
            found = FALSE;
            for(;si<=sub_count;si++ )
            {
              QMenuItem *sub_menu_item = sub_menu->findItem(si);
              menu = sub_menu;
dprintf("sub_menu_item[%d]=0x%x\n", si, sub_menu_item );
#ifdef DEBUG_PI
printf("panel_init, sub_menu_item[%d]=0x%x\n", si, sub_menu_item );
#endif
              QString sub_menu_text = sub_menu->text(si);
              if( sub_menu_text.isEmpty() )
              {
dprintf("sub_menu_text is empty.  ;-( \n");
#ifdef DEBUG_PI
printf("panel_init, sub_menu_text is empty.  ;-( \n");
#endif
              } else if( sub_menu_text == pluginInfo->sub_menu_heading )
              {
dprintf("FOUND: sub_menu_text=%s\n", sub_menu_text.ascii() );
#ifdef DEBUG_PI
printf("panel_init, FOUND: sub_menu_text=%s\n", sub_menu_text.ascii() );
#endif
                found = TRUE;
                menu = sub_menu_item->popup();
dprintf("menu (sub menu really) =0x%x\n", menu);
#ifdef DEBUG_PI
printf("panel_init, menu (sub menu really) =0x%x\n", menu);
#endif
                break;
              }
            }
dprintf("A: Create a new submenu???? found=%d menu=0x%x\n", found, menu);
#ifdef DEBUG_PI
printf("panel_init, A: Create a new submenu???? found=%d menu=0x%x\n", found, menu);
#endif
            if( !found && menu)
            {
dprintf("A: Create a new submenu!!!! si=%d pl=0x%x\n", si, pl);
#ifdef DEBUG_PI
printf("panel_init, A: Create a new submenu!!!! si=%d pl=0x%x\n", si, pl);
#endif
              new_menu = new QPopupMenu(pl);
dprintf("sub_menu=0x%x item->popup()=0x%x\n", sub_menu, item->popup() );
#ifdef DEBUG_PI
printf("panel_init, sub_menu=0x%x item->popup()=0x%x\n", sub_menu, item->popup() );
#endif
              sub_menu->insertItem(pluginInfo->sub_menu_heading, new_menu, si, si-7);
              menu = new_menu;
              found = TRUE;
            }
            break;
          } else
          {
fprintf(stderr, "Warning: You shouldn't get here.\n");
          }
        }
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
    if( pluginInfo->plugin_short_description )
    {
      action->setStatusTip(QObject::tr(pluginInfo->plugin_short_description) );
    }
  
    pluginInfo->slotInfo->connect( action,
      SIGNAL( activated() ), pluginInfo->slotInfo,
        SLOT( dynamicMenuCallback() ));

    // If the panels are toplevel panels and are marked to show
    // create and show them right now.
    if( pluginInfo->show_immediate == 1 )
    {
      dprintf("This panel is a top level grouping panel.\n");
      dprintf("Initialize it right away, then continue the menu addition...\n");
      extern Panel * create_and_add_panel(void *, void *, ArgumentObject *, const char *);
      create_and_add_panel(pluginInfoArg, (void *)pluginInfo->masterPC, NULL, (const char *)NULL);
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
  create_and_add_panel(void *pluginInfoArg, void *tPC, ArgumentObject *ao, const char *collector_names)
  {
    PanelContainer *targetPC = (PanelContainer *)tPC;
    PluginInfo *pluginInfo = (PluginInfo *)pluginInfoArg;
    char *name = pluginInfo->menu_label;

    PanelContainer *local_masterPC = (PanelContainer *)pluginInfo->masterPC;


    dprintf("Don't forget to add a preference and check it!\n");
    dprintf("create_and_add_panel::: name=(%s)\n", name );
    if( strcmp(name, "&Command Panel") == 0 )
    {
      Panel *p = local_masterPC->findNamedPanel(local_masterPC, name);
      if( p )
      {
        p = p->getPanelContainer()->raiseNamedPanel(name);
      }
      if( p )
      {
        return p;
      }
    } else if( strcmp(name, "Intro Wizard") == 0 )
    {
      dprintf("name = (%s)\n", name );
      Panel *p = local_masterPC->findNamedPanel(local_masterPC, name);
      if( p )
      {
        p = p->getPanelContainer()->raiseNamedPanel(name);
      }
      if( p )
      {
        dprintf("Found a hidden Intro Wizard Panel!  raise it!\n");
        return p;
      }
      dprintf("Bzzzt.  No panel around!\n");
    }


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
    if( !name )
    {
      name = pluginInfo->panel_type;
    }
    if( !name )
    {
      name = (char *) "Warning: Unamed Panel: No menu_label or panel_type.";
    }

    dprintf("All panels start here!   name=(%s)\n", name);

    plugin_panel = new PANEL_CLASS_NAME(targetPC, name, ao);
    plugin_panel->pluginInfo = pluginInfo;
    targetPC->addPanel((Panel *)plugin_panel, targetPC, name);
    
    return(plugin_panel);
  }
}
