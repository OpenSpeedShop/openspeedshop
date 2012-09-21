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


#include <stdlib.h>
#include <stdio.h>

#include "PanelContainer.hxx"

#include <qwidget.h>
#include <qlayout.h>

PanelContainerList *panelContainerList;

/*! This file contains the entry point for loading the PanelContainer
    dynamic library.   It is called from gui.so (openspeedshop.ui.h)
 */

int
_init()
{
  printf("Hello from _init() plugin_panel_container_entry_point.h\n");
 
  return(1);
}

extern "C"
{
  PanelContainer * pc_init(QWidget *w, QVBoxLayout *layout)
  {
//    printf("hello from pc_init(QWidget *w) oooboy\n");

    PanelContainer *topPC = NULL;

    panelContainerList = new PanelContainerList;
    panelContainerList->clear();

    QWidget *panelContainerWidget = new QWidget( w,
                                        "mainWindowPanelContainerWidget" );
    panelContainerWidget->setCaption("mainWindowPanelContainerWidget");

    topPC = createPanelContainer( panelContainerWidget,
                                  "masterPC", NULL, panelContainerList);

    topPC->topLevel = TRUE;

    topPC->_pluginRegistryList = new PluginRegistryList;
    topPC->_pluginRegistryList->clear();

    topPC->panelList.clear();

    layout->addWidget(panelContainerWidget);

    return topPC;
  }
}
