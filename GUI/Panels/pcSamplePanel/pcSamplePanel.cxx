#include "pcSamplePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qiconset.h>

#include <qbitmap.h>

#define DEMO 1
#ifdef DEMO
#include "SourcePanel.hxx" // For demo only....
#include "SourceObject.hxx" // For demo only....
#include "TopPanel.hxx" // For demo only....
#endif // DEMO



/*!  pcSamplePanel Class

     Autor: Al Stipek (stipek@sgi.com)
 */


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
  printf("pcSamplePanel::pcSamplePanel() constructor called\n");
  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  pco = new ProcessControlObject(frameLayout, getBaseWidgetFrame(), (Panel *)this );

  statusLayout = new QHBoxLayout( 0, 10, 0, "statusLayout" );
  
  statusLabel = new QLabel( getBaseWidgetFrame(), "statusLabel");
  statusLayout->addWidget( statusLabel );

  statusLabelText = new QLineEdit( getBaseWidgetFrame(), "statusLabelText");
  statusLabelText->setReadOnly(TRUE);
  statusLayout->addWidget( statusLabelText );

  frameLayout->addLayout( statusLayout );

  languageChange();

  PanelContainerList *lpcl = new PanelContainerList();
  lpcl->clear();

  QWidget *pcSampleControlPanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "pcSampleControlPanelContainerWidget" );
  topPC = createPanelContainer( pcSampleControlPanelContainerWidget,
                              "PCSamplingControlPanel_topPC", NULL,
                              pc->_masterPanelContainerList );
  frameLayout->addWidget( pcSampleControlPanelContainerWidget );

printf("Create an Application\n");
printf("# Application theApplication;\n");
printf("# //Create a process for the command in the suspended state\n");
printf("# load the executable or attach to the process.\n");
printf("# if( loadExecutable ) {\n");
printf("# theApplication.createProcess(command);\n");
printf("# } else { // attach \n");
printf("#   theApplication.attachToPrcess(pid, hostname);\n");
printf("# }\n");
printf("set the parameters.\n");
printf("# pcCollector.setParameter(param);  // obviously looping over each.\n");
printf("attach the collector to the Application.\n");
printf("# // Attach the collector to all threads in the application\n");
printf("# theApplication.attachCollector(theCollector.getValue());\n");



  topPC->splitVertical(75);
//  topPC->rightPanelContainer->splitVertical();

  pcSampleControlPanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;

//  topPC->dl_create_and_add_panel("Toolbar Panel", topPC->leftPanelContainer);
//  topPC->dl_create_and_add_panel("Top Five Panel", topPC->leftPanelContainer);
#ifdef DEMO
  SourcePanel *sp = (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", topPC->leftPanelContainer);
#else // DEMO
  topPC->dl_create_and_add_panel("Source Panel", topPC->leftPanelContainer);
#endif // DEMO
  topPC->dl_create_and_add_panel("Command Panel", topPC->rightPanelContainer);


#ifdef DEMO
{ // Begin - Just for demo sake...load a file and place the user at main()
  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
  char buffer[200];
  strcpy(buffer, plugin_directory);
  strcat(buffer, "/../../../Example.cpp");
printf("load (%s)\n", buffer);
  SourceObject *spo = new SourceObject("main", buffer, 1556, TRUE, NULL);

  if( !sp->listener((void *)spo) )
  {
    fprintf(stderr, "Unable to position at main in %s\n", buffer);
  } else
  {
fprintf(stderr, "Positioned at main in %s ????? \n", buffer);
  }
} // End - for demo sake
#endif // DEMO
  
}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
pcSamplePanel::~pcSamplePanel()
{
  printf("  pcSamplePanel::~pcSamplePanel() destructor called\n");
  delete frameLayout;
  delete baseWidgetFrame;
}

//! Add user panel specific menu items if they have any.
bool
pcSamplePanel::menu(QPopupMenu* contextMenu)
{
  printf("pcSamplePanel::menu() requested.\n");

  contextMenu->insertSeparator();
  contextMenu->insertItem("&Manage Collectors", this, SLOT(manageCollectorsSelected()), CTRL+Key_A );
  contextMenu->insertItem("&Manage Processes", this, SLOT(manageProcessesSelected()), CTRL+Key_A );
  contextMenu->insertSeparator();
  contextMenu->insertItem("&Save As ...", this, SLOT(saveAsSelected()), CTRL+Key_S ); 

  return( TRUE );
}

void
pcSamplePanel::manageCollectorsSelected()
{
  printf("pcSamplePanel::manageCollectorsSelected()\n");
}   

void
pcSamplePanel::manageProcessesSelected()
{
  printf("pcSamplePanel::managerProcessesSelected()\n");
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
  printf("pcSamplePanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
pcSamplePanel::listener(void *msg)
{
  printf("pcSamplePanel::listener() requested.\n");
  int ret_val = 0; // zero means we didn't handle the message.

  ControlObject *co = (ControlObject *)msg;

  if( !co )
  {
     return 0; // 0 means, did not act on message
  }

  // Check the message type to make sure it's our type...
  if( co->msgType != "ControlObject" )
  {
    nprintf(DEBUG_PANELS) ("psSamplePanel::listener() Not a ControlObject.\n");
    return 0; // o means, did not act on message.
  }

  co->print();

  switch( (int)co->cot )
  {
    case  ATTACH_PROCESS_T:
      printf("Attach to a process\n");
      break;
    case  DETACH_PROCESS_T:
      printf("Detach from a process\n");
      ret_val = 1;
      break;
    case  ATTACH_COLLECTOR_T:
      printf("Attach to a collector\n");
      ret_val = 1;
      break;
    case  REMOVE_COLLECTOR_T:
      printf("Remove a collector\n");
      ret_val = 1;
      break;
    case  RUN_T:
      printf("Run\n");
statusLabelText->setText( tr("Process running...") );
#ifdef DEMO
{
qApp->processEvents(500);
sleep(5);
statusLabelText->setText( tr("Performance data available.  Click update arrow for a report...") );
qApp->processEvents(500);
sleep(5);
statusLabelText->setText( tr("Process completed...") );
qApp->processEvents(500);
pco->attachProcessButton->setEnabled(TRUE);
pco->attachCollectorButton->setEnabled(TRUE);
pco->runButton->setEnabled(TRUE);
pco->runButton->enabledFLAG = TRUE;
pco->pauseButton->setEnabled(FALSE);
pco->pauseButton->enabledFLAG = FALSE;
pco->continueButton->setEnabled(FALSE);
pco->continueButton->setEnabled(FALSE);
pco->continueButton->enabledFLAG = FALSE;
pco->updateButton->setEnabled(FALSE);
pco->updateButton->setEnabled(FALSE);
pco->updateButton->enabledFLAG = FALSE;
pco->detachProcessButton->setEnabled(TRUE);
pco->detachCollectorButton->setEnabled(TRUE);
pco->terminateButton->setEnabled(FALSE);
pco->terminateButton->setFlat(TRUE);
pco->terminateButton->setEnabled(FALSE);

// TopPanel *tp = (TopPanel *)topPC->dl_create_and_add_panel("Top Panel", topPC->leftPanelContainer);
TopPanel *tp = (TopPanel *)topPC->dl_create_and_add_panel("Top Panel", NULL); 
// Uncomment the next line if you want the TopPanel to position the 
// source automatically.
// tp->listener((void *)NULL);
}
#endif // DEMO
      ret_val = 1;
      break;
    case  PAUSE_T:
      printf("Pause\n");
statusLabelText->setText( tr("Process suspended...") );
      ret_val = 1;
      break;
    case  CONT_T:
      printf("Continue\n");
statusLabelText->setText( tr("Process continued...") );
sleep(1);
statusLabelText->setText( tr("Process running...") );
      ret_val = 1;
      break;
    case  UPDATE_T:
      printf("Update\n");
      ret_val = 1;
      break;
    case  INTERRUPT_T:
      printf("Interrupt\n");
      ret_val = 1;
      break;
    case  TERMINATE_T:
statusLabelText->setText( tr("Process terminated...") );
      ret_val = 1;
      printf("Terminate\n");
      break;
    default:
      break;
  }
  return ret_val;  // 0 means, did not want this message and did not act on anything.
}

/*
*  Sets the strings of the subwidgets using the current
 *  language.
 */
void
pcSamplePanel::languageChange()
{
  statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("No status currently available.") );
}
