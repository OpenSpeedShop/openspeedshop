#include "pcSamplePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


/*!  pcSamplePanel Class
     This class is used by the script mknewpanel to create a new work area
     for the panel creator to design a new panel.


     Autor: Al Stipek (stipek@sgi.com)
 */

#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qbitmap.h>

#include "attach.xpm"
#include "detach.xpm"
#include "run.xpm"
#include "pause.xpm"
#include "cont.xpm"
#include "update.xpm"
#include "interrupt.xpm"
#include "terminate.xpm"

/*! The default constructor.   Unused. */
pcSamplePanel::pcSamplePanel()
{ // Unused... Here for completeness...
}


/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
pcSamplePanel::pcSamplePanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf( "pcSamplePanel::pcSamplePanel() constructor called\n");
  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );
 
#ifdef OLDWAY
  lpcl = new PanelContainerList();
  lpcl->clear();
#endif //

  pcSamplePanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "pcSamplePanelContainerWidget" );
  topPC = createPanelContainer( pcSamplePanelContainerWidget,
                              "PCSamplePanel_topPC", NULL,
                              pc->_masterPanelContainerList );
  frameLayout->addWidget( pcSamplePanelContainerWidget );


//  topPC->splitVertical();

  pcSamplePanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;

printf("load pcSampleCollector; (if not already loaded.)\n");
printf("# Find the \"pcsamp\" data collector\n");
printf("#   Optional<Collector> pcCollector = Collector::findCollector(\"pcsamp\");\n");
printf("#    if(!pcCollector.hasValue()) {\n");
printf("#      std::cout << \"The \"pcsamp\" collector is unavailable.\" << std::endl;\n");
printf("#        return(-1);\n");
printf("#    }\n");
printf("get the collector description\n");
printf("#  pcCollector.getDesctription() ????\n");
printf("get the collector parameters\n");
printf("#  Parameter *paramList = pcCollector.getParameters();\n");
printf("#    if( paramList.empty() ) { \n");
printf("#      return(-2);\n");
printf("#    }\n");

  topPC->dl_create_and_add_panel("pc Sample Wizard", topPC);
  topPC->dl_create_and_add_panel("pc Sample Panel Control", topPC);

  topPC->raiseNamedPanel("pc Sample Wizard");
}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
pcSamplePanel::~pcSamplePanel()
{
  printf("  pcSamplePanel::~pcSamplePanel() destructor called\n");

  delete pcSamplePanelContainerWidget;
  delete frameLayout;
  delete baseWidgetFrame;
}

//! Add user panel specific menu items if they have any.
bool
pcSamplePanel::menu(QPopupMenu* contextMenu)
{
  dprintf("pcSamplePanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
pcSamplePanel::save()
{
  dprintf("pcSamplePanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
pcSamplePanel::saveAs()
{
  dprintf("pcSamplePanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
pcSamplePanel::listener(char *msg)
{
  dprintf("pcSamplePanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
pcSamplePanel::broadcast(char *msg)
{
  dprintf("pcSamplePanel::broadcast() requested.\n");
  return 0;
}
