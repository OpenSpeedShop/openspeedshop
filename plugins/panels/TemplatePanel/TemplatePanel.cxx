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


#include "TemplatePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#define SIMPLEPANEL 1
// #define TOPLEVEL 1
// #define QTCLASS 1
#ifdef TOPLEVEL
#include "ProcessControlObject.hxx"
#include "qlineedit.h"
#endif // TOPLEVEL


/*! TemplatePanel Class is intended to be used as a starting point to create
    user defined panels.   There's a script: mknewpanel that takes this
    template panel and creates a panel for the user to work with.    (See:
    mknewpanel in this directory.  i.e. type: mknewpanel)

    $ mknewpanel
    usage: mknewpanel directory panelname "menu header" "menu label" "menu accel" "show immediate" "grouping"
    where:
      directory:  Is the path to the directory to put the new panel code.
      panelname:  Is the name of the new panel.
      menu header: Is the Menu named to be put on the menu bar.
      menu label: Is the menu label under the menu header.
      menu accel: Is the menu accelerator.
      show immediate: Default is 0.  Setting this to 1 will display the panel upon initialization.
      grouping: Which named panel container should this menu item drop this panel by default.


    An exmple would be to cd to this TemplatePanel directory and issue the
    following command:
    mknewpanel ../ExamplePanel "ExamplePanel" "Experiments" "ExamplePanel" " " 0 " Performance

    That command would create a new panel directory, with the necessary
    structure for the user to create a new panel.   The user's new panel would
    be in the ../ExamplePanel directory.   The future panel would be called,
    "ExamplePanel".   A toplevel menu heading will be created called 
    "Experiments".   An entry under that topleve menu would read "ExamplePanel"
    The panel would not be displayed upon initialization of
    the tool, but only upon menu selection.    The final argument hints to the 
    tool that this panel belongs to the group of other Performance related 
    panels.
*/


#include <qlistview.h>  // For QListView
#include <qvaluelist.h>  // For QTextEdit in example below...
#include <qsplitter.h>  // For QSplitter in example below...
#include <chartform.hxx>  // For chart in example below...
TemplatePanel::TemplatePanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  setCaption("TemplatePanel");
  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

#ifdef QTCLASS
// Here's an example of  adding a class you created with QtDesigner...
// Defining QtClass (above) will enable this block.   
// This block will include a class that you designed with QtDesigner.
  YourClass *ppc = new YourClass(getBaseWidgetFrame(), getName() );
  frameLayout->addWidget( ppc );
  ppc->show();
#endif // QTCLASS


#ifdef TOPLEVEL
// Here's an example of a putting a name toplevel panel container in the
// panel.
// Defining TOPLEVEL (above) will enable this block.   
// This block will create an outline much like the pcSamplePanel and 
// UserTimePanel experiments.   
// A process control panel (run, pause, update, terminate) will be created
// and a panel container to place child panels will be created.
// See pcSamplePanel for a working example.
  ProcessControlObject *pco = new ProcessControlObject(frameLayout, getBaseWidgetFrame(), (Panel *)this );

  QHBoxLayout *statusLayout = new QHBoxLayout( 0, 10, 0, "statusLayout" );

  QLabel *statusLabel = new QLabel( getBaseWidgetFrame(), "statusLabel");
  statusLayout->addWidget( statusLabel );

  QLineEdit *statusLabelText = new QLineEdit( getBaseWidgetFrame(), "statusLabelText");
  statusLabelText->setReadOnly(TRUE);
  statusLayout->addWidget( statusLabelText );

  frameLayout->addLayout( statusLayout );

  QWidget *namedPanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "namedPanelContainerWidget" );
  PanelContainer *topPPL = createPanelContainer( namedPanelContainerWidget, "", NULL, pc->getMasterPCList() );
  frameLayout->addWidget( namedPanelContainerWidget );
  
//  namedPanelContainerWidget->show();
  topPPL->show();
  topPPL->topLevel = TRUE;
#endif // TOPLEVEL


#ifdef SIMPLEPANEL
// Here's an example of creating a simple panel.  
// Defining SIMPLEPANEL (above) will enable this block.   
// This block creates a panel with a QSplitter.  On the left side of the
// splitter there is a QListView and on the right a chart widget.
// See StatsPanel for a working example.
  QSplitter *splitter = new QSplitter(getBaseWidgetFrame(), "splitter");
  splitter->setCaption("splitter");

  splitter->setOrientation(QSplitter::Horizontal);

  // A simple start to adding simple qt widgets to a panel...
  QListView *lv = new QListView( splitter );
  lv->addColumn("Time");
  lv->addColumn("Percent");
  lv->addColumn("Description");
  ChartForm *cf = new ChartForm( splitter );
  cf->setCaption("Example Chart");


  // Add some numbers to both the list and chart.
  ChartPercentValueList cpvl;
  ChartTextValueList ctvl;
  cpvl.clear();
  ctvl.clear();

  cpvl.push_back(40);
  cpvl.push_back(30);
  cpvl.push_back(20);
  cpvl.push_back(10);

  ctvl.push_back("40");
  ctvl.push_back("30");
  ctvl.push_back("20");
  ctvl.push_back("10");

  cf->setValues(cpvl, ctvl);

  cf->show();

  new QListViewItem(lv,"40.00", "40", "Time spent in function a()" );
  new QListViewItem(lv,"30.00", "30", "Time spent in function b()" );
  new QListViewItem(lv,"20.00", "20", "Time spent in function c()" );
  new QListViewItem(lv,"10.00", "10", "Time spent in function d()" );



  lv->show();

  frameLayout->addWidget( splitter );
#endif // SIMPLEPANEL

  getBaseWidgetFrame()->setCaption("TemplatePanelBaseWidget");
}


TemplatePanel::~TemplatePanel()
{
  // Delete anything you new'd from the constructor.
}

void
TemplatePanel::languageChange()
{
  // Set language specific information here.
}


/*! This calls the user 'menu()' function
    if the user provides one.   The user can attach any specific panel
    menus to the passed argument and they will be displayed on a right
    mouse down in the panel.
    /param  contextMenu is the QPopupMenu * that use menus can be attached.
 */
bool
TemplatePanel::menu(QPopupMenu* contextMenu)
{
  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
TemplatePanel::save()
{
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
TemplatePanel::saveAs()
{
}


/*! When a message has been sent (from anyone) and the message broker is
    notifying panels that they may want to know about the message, this is the
    function the broker notifies.   The listener then needs to determine
    if it wants to handle the message.
    \param msg is the incoming message.
    \return 0 means you didn't do anything with the message.
    \return 1 means you handled the message.
 */
int 
TemplatePanel::listener(void *msg)
{
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
TemplatePanel::broadcast(char *msg)
{
  return 0;
}
