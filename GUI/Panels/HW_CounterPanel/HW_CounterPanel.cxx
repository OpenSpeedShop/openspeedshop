#include "HW_CounterPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


/*! HW_CounterPanel Class is intended to be used as a starting point to create
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


    An exmple would be to cd to this HW_CounterPanel directory and issue the
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
HW_CounterPanel::HW_CounterPanel()
{ // Unused... Here for completeness...
  fprintf(stderr, "HW_CounterPanel::HW_CounterPanel() should not be called.\n");
  fprintf(stderr, "see: HW_CounterPanel::HW_CounterPanel(PanelContainer *pc, const char *n)\n");
}


/*! This constructor is the work constructor.   It is called to
    create the new Panel and attach it to a PanelContainer.
    \param pc is a pointer to PanelContainer
      the Panel will be initially attached.
    \param name is the name give to the Panel.
      This is where the user would create the panel specific Qt code
      to do whatever functionality the user wanted the panel to perform.
 */
HW_CounterPanel::HW_CounterPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  setCaption("HW_CounterPanel");
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
  PanelContainer *topPPL = createPanelContainer( namedPanelContainerWidget, "", NULL );
  frameLayout->addWidget( namedPanelContainerWidget );
  
  namedPanelContainerWidget->show();
  topPPL->show();
  topPPL->topLevel = TRUE;
*/

  baseWidgetFrame->setCaption("HW_CounterPanelBaseWidget");
}


/*! The only thing that needs to be cleaned is anything allocated in this
    class.  By default that is nothing.
 */
HW_CounterPanel::~HW_CounterPanel()
{
  // Delete anything you new'd from the constructor.
}

void
HW_CounterPanel::languageChange()
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
HW_CounterPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("HW_CounterPanel::menu() requested.\n");

  return( FALSE );
}

/*! If the user panel save functionality, their function
     should provide the saving.
 */
void 
HW_CounterPanel::save()
{
  dprintf("HW_CounterPanel::save() requested.\n");
}

/*! If the user panel provides save to functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
HW_CounterPanel::saveAs()
{
  dprintf("HW_CounterPanel::saveAs() requested.\n");
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
HW_CounterPanel::listener(void *msg)
{
  dprintf("HW_CounterPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! If the user wants to override the Panel::broadcast() functionality, put
    that logic here.  Otherwise, based on the broadcast type, send out the
    message.
 */
int 
HW_CounterPanel::broadcast(char *msg)
{
  dprintf("HW_CounterPanel::broadcast() requested.\n");
  return 0;
}
