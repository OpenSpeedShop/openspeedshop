#include <stdlib.h>
#include <stdio.h>

#include "PanelContainer.hxx"

#include <qwidget.h>
#include <qlayout.h>

PanelContainer *topPC = NULL;
PanelContainerList *panelContainerList;

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
    printf("hello from pc_init(QWidget *w) oooboy\n");

    topPC = NULL;

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
