#include "UserTimePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


/*!  \class UserTimePanel

    A simple prototype with nothing in it.
 */


/*! The default constructor.   Unused. */
UserTimePanel::UserTimePanel()
{ // Unused... Here for completeness...
}


/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
UserTimePanel::UserTimePanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf( "UserTimePanel::UserTimePanel() constructor called\n");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );
}


/*! Destroys the object and frees any allocated resources
    The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
UserTimePanel::~UserTimePanel()
{
  printf("  UserTimePanel::~UserTimePanel() destructor called\n");

  delete frameLayout;

  delete baseWidgetFrame;
}

/*! Add user panel specific menu items if they have any. */
bool
UserTimePanel::menu(QPopupMenu* contextMenu)
{
  dprintf("UserTimePanel::menu() requested.\n");

  return( FALSE );
}

/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
UserTimePanel::save()
{
  dprintf("UserTimePanel::save() requested.\n");
}

/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
UserTimePanel::saveAs()
{
  dprintf("UserTimePanel::saveAs() requested.\n");
}

/*! This function listens for messages. */
int 
UserTimePanel::listener(void *msg)
{
  dprintf("UserTimePanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! This function broadcasts messages. */
int 
UserTimePanel::broadcast(char *msg)
{
  dprintf("UserTimePanel::broadcast() requested.\n");
  return 0;
}
