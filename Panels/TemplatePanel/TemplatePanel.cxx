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


/*! TemplatePanel Class is intended to be used as a starting point to create
    user defined panels.   There's a script: mknewpanel that takes this
    template panel and creates a panel for the user to work with.    (See:
    mknewpanel in this directory.  i.e. type: mknewpanel --help)

    $ mknewpanel
    usage: mknewpanel directory panelname "menu header" "menu label" "show immediate" "grouping"
    where:
      directory:  Is the path to the directory to put the new panel code.
      panelname:  Is the name of the new panel.
      menu header: Is the Menu named to be put on the menu bar.
      menu label: Is the menu label under the menu header.
      show immediate: Default is 0.  Setting this to 1 will display the panel upon initialization.
      grouping: Which named panel container should this menu item drop this panel by default.


    An exmple would be to cd to this TemplatePanel directory and issue the
    following command:
    mknewpanel ../NewPanelName "NewPanelName" "New Panel Menu Heading" "New Panel Label" 0 "Performance"

    That command would create a new panel directory, with the necessary
    structure for the user to create a new panel.   The user's new panel would
    be in the NewPanelName directory.   The future panel would be called,
    "NewPanelName".   A toplevel menu heading will be created called "New 
    Panel Menu Heading".   An entry under that topleve menu would read "New
    Panel Label".    The panel would not be displayed upon initialization of
    the tool, but only upon menu selection.    The final argument hints to the 
    tool that this panel belongs to the group of other Performance related 
    panels.
*/


/*! The default constructor.   Unused. */
TemplatePanel::TemplatePanel()
{ // Unused... Here for completeness...
  fprintf(stderr, "TemplatePanel::TemplatePanel() should not be called.\n");
  fprintf(stderr, "see: TemplatePanel::TemplatePanel(PanelContainer *pc, const char *n)\n");
}


/*! This constructor is the work constructor.   It is called to
    create the new Panel and attach it to a PanelContainer.
    \param pc is a pointer to PanelContainer
      the Panel will be initially attached.
    \param name is the name give to the Panel.
      This is where the user would create the panel specific Qt code
      to do whatever functionality the user wanted the panel to perform.
 */
TemplatePanel::TemplatePanel(PanelContainer *pc, const char *n, char *argument) : Panel(pc, n)
{
  setCaption("TemplatePanel");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

/* Here's an example of adding a class you created with QtDesigner...
  ProfileClass *ppc = new ProfileClass(getBaseWidgetFrame(), getName() );
  frameLayout->addWidget( ppc );
  ppc->show();
*/

/*
// Here's an example of a putting a name toplevel panel container in the
// panel.
  QWidget *namedPanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "namedPanelContainerWidget" );
  PanelContainer *topPPL = createPanelContainer( namedPanelContainerWidget, "PLUGIN_GROUPING", NULL, pc->getMasterPCList() );
  frameLayout->addWidget( namedPanelContainerWidget );
  
  namedPanelContainerWidget->show();
  topPPL->show();
  topPPL->topLevel = TRUE;
*/

  getBaseWidgetFrame()->setCaption("TemplatePanelBaseWidget");
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
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
  dprintf("TemplatePanel::menu() requested.\n");

  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
TemplatePanel::save()
{
  dprintf("TemplatePanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
TemplatePanel::saveAs()
{
  dprintf("TemplatePanel::saveAs() requested.\n");
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
  dprintf("TemplatePanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
TemplatePanel::broadcast(char *msg)
{
  dprintf("TemplatePanel::broadcast() requested.\n");
  return 0;
}
