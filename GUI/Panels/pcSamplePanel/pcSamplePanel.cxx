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
/*
#include "attach.xpm"
#include "detach.xpm"
*/
#include "attach_hand.xpm"
#include "attach_hand2.xpm"
#include "attach_hand3.xpm"
#include "attach_hand4.xpm"
#include "detach_hand.xpm"
#include "detach_hand2.xpm"
#include "detach_hand3.xpm"
#include "detach_hand4.xpm"
#include "detach_hand5.xpm"
#include "attach_process1.xpm"
#include "attach_process2.xpm"
#include "attach_process3.xpm"
#include "attach_process4.xpm"
#include "detach_process0.xpm"
#include "detach_process1.xpm"
#include "detach_process2.xpm"
#include "detach_process3.xpm"
#include "detach_process4.xpm"
#include "run.xpm"
#include "pause.xpm"
#include "cont.xpm"
#include "update.xpm"
#include "interrupt.xpm"
#include "terminate.xpm"



/*!  pcSamplePanel Class
     This class is used by the script mknewpanel to create a new work area
     for the panel creator to design a new panel.


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

  buttonGroup = new QButtonGroup( getBaseWidgetFrame(), "buttonGroup" );
  buttonGroup->setColumnLayout(0, Qt::Vertical );
  buttonGroup->layout()->setSpacing( 6 );
  buttonGroup->layout()->setMargin( 11 );
  QHBoxLayout *buttonGroupLayout = new QHBoxLayout( buttonGroup->layout() );
  buttonGroupLayout->setAlignment( Qt::AlignTop );

  frameLayout->addWidget( buttonGroup );

  attachProcessButton = new AnimatedQPushButton( buttonGroup, "attachProcessButton" );
  QPixmap *apm1 = new QPixmap( attach_hand_xpm );
  apm1->setMask(apm1->createHeuristicMask());
  attachProcessButton->setPixmap( *apm1 );
  attachProcessButton->push_back(apm1);
  QPixmap *apm2 = new QPixmap( attach_hand2_xpm );
  apm2->setMask(apm2->createHeuristicMask());
  attachProcessButton->push_back(apm2);
  QPixmap *apm3 = new QPixmap( attach_hand3_xpm );
  apm3->setMask(apm3->createHeuristicMask());
  attachProcessButton->push_back(apm3);
  QPixmap *apm4 = new QPixmap( attach_hand4_xpm );
  apm4->setMask(apm4->createHeuristicMask());
  attachProcessButton->push_back(apm4);
  attachProcessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, attachProcessButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( attachProcessButton );
  attachProcessButton->setText( QString::null );

//  detachProcessButton = new QPushButton( buttonGroup, "detachProcessButton" );
  detachProcessButton = new AnimatedQPushButton( buttonGroup, "detachProcessButton");
  QPixmap *dpm1 = new QPixmap( detach_hand_xpm );
  dpm1->setMask(dpm1->createHeuristicMask());
  detachProcessButton->setPixmap( *dpm1 );
  detachProcessButton->push_back(dpm1);

  QPixmap *dpm2 = new QPixmap( detach_hand2_xpm );
  dpm2->setMask(dpm2->createHeuristicMask());
  detachProcessButton->push_back( dpm2);

  QPixmap *dpm3 = new QPixmap( detach_hand3_xpm );
  dpm3->setMask(dpm3->createHeuristicMask());
  detachProcessButton->push_back( dpm3);

  QPixmap *dpm4 = new QPixmap( detach_hand4_xpm );
  dpm4->setMask(dpm4->createHeuristicMask());
  detachProcessButton->push_back( dpm4);

  QPixmap *dpm5 = new QPixmap( detach_hand5_xpm );
  dpm5->setMask(dpm5->createHeuristicMask());
  detachProcessButton->push_back( dpm5);

  detachProcessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, detachProcessButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( detachProcessButton );
  detachProcessButton->setText( QString::null );

{
  attachCollectorButton = new AnimatedQPushButton( buttonGroup, "attachCollectorButton" );
  QPixmap *apm1 = new QPixmap( attach_process1_xpm );
  apm1->setMask(apm1->createHeuristicMask());
  attachCollectorButton->setPixmap( *apm1 );
  attachCollectorButton->push_back(apm1);
  QPixmap *apm2 = new QPixmap( attach_process2_xpm );
  apm2->setMask(apm2->createHeuristicMask());
  attachCollectorButton->push_back(apm2);
  QPixmap *apm3 = new QPixmap( attach_process3_xpm );
  apm3->setMask(apm3->createHeuristicMask());
  attachCollectorButton->push_back(apm3);
  QPixmap *apm4 = new QPixmap( attach_process4_xpm );
  apm4->setMask(apm4->createHeuristicMask());
  attachCollectorButton->push_back(apm4);
  attachCollectorButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, attachCollectorButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( attachCollectorButton );
  attachCollectorButton->setText( QString::null );
}

{
  detachCollectorButton = new AnimatedQPushButton( buttonGroup, "detachCollectorButton" );
  QPixmap *apm1 = new QPixmap( detach_process4_xpm );
  apm1->setMask(apm1->createHeuristicMask());
  detachCollectorButton->setPixmap( *apm1 );
  detachCollectorButton->push_back(apm1);
  QPixmap *apm2 = new QPixmap( detach_process3_xpm );
  apm2->setMask(apm2->createHeuristicMask());
  detachCollectorButton->push_back(apm2);
  QPixmap *apm3 = new QPixmap( detach_process2_xpm );
  apm3->setMask(apm3->createHeuristicMask());
  detachCollectorButton->push_back(apm3);
  QPixmap *apm4 = new QPixmap( detach_process1_xpm );
  apm4->setMask(apm4->createHeuristicMask());
  detachCollectorButton->push_back(apm4);
  QPixmap *apm5 = new QPixmap( detach_process0_xpm );
  apm5->setMask(apm5->createHeuristicMask());
  detachCollectorButton->push_back(apm5);
  detachCollectorButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, detachCollectorButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( detachCollectorButton );
  detachCollectorButton->setText( QString::null );
}

  {
  QSpacerItem *spacer = new QSpacerItem( 5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
  buttonGroupLayout->addItem(spacer);
  }

  runButton = new AnimatedQPushButton( buttonGroup, "runButton" );
  QPixmap *pm = new QPixmap( run_xpm );
  pm->setMask(pm->createHeuristicMask());
  runButton->setPixmap( *pm );
  runButton->push_back( pm );
  runButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, runButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( runButton );
  runButton->setText( QString::null );
  {
  QSpacerItem *spacer = new QSpacerItem( 10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
  buttonGroupLayout->addItem(spacer);
  }


  {
  pauseButton = new AnimatedQPushButton( buttonGroup, "pauseButton", FALSE );
  QPixmap *pm = new QPixmap( pause_xpm );
  pm->setMask(pm->createHeuristicMask());
  pauseButton->setPixmap( *pm );
  pauseButton->push_back( pm );
  pauseButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pauseButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( pauseButton );
  pauseButton->setText( QString::null );
  }

  {
  continueButton = new AnimatedQPushButton( buttonGroup, "continueButton", FALSE );
  QPixmap *pm = new QPixmap( cont_xpm );
  pm->setMask(pm->createHeuristicMask());
  continueButton->setPixmap( *pm );
  continueButton->push_back( pm );
  continueButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, continueButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( continueButton );
  continueButton->setText( QString::null );
  }

  {
  updateButton = new AnimatedQPushButton( buttonGroup, "updateButton", FALSE );
  QPixmap *pm = new QPixmap( update_xpm );
  pm->setMask(pm->createHeuristicMask());
  updateButton->setPixmap( *pm );
  updateButton->push_back( pm );
  updateButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, updateButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( updateButton );
  updateButton->setText( QString::null );
  }

  {
  QSpacerItem *spacer = new QSpacerItem( 10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
  buttonGroupLayout->addItem(spacer);
  }

#ifdef OLDWAY
  interruptButton = new QPushButton( buttonGroup, "interruptButton" );
  interruptButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, interruptButton->sizePolicy().hasHeightForWidth() ) );
  {
  QPixmap pm(interrupt_xpm);
  pm.setMask(pm.createHeuristicMask());
  interruptButton->setPixmap( pm );
  }
  buttonGroupLayout->addWidget( interruptButton );
  interruptButton->setText( QString::null );
#else // OLDWAY
  {
  interruptButton = new AnimatedQPushButton( buttonGroup, "interruptButton", TRUE );
  QPixmap *pm = new QPixmap( interrupt_xpm );
  pm->setMask(pm->createHeuristicMask());
  interruptButton->setPixmap( *pm );
  interruptButton->push_back( pm );
  interruptButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, interruptButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( interruptButton );
  interruptButton->setText( QString::null );
  }
#endif // OLDWAY


  {
  QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum );
  buttonGroupLayout->addItem(spacer);
  }

#ifdef OLDWAY
  terminateButton = new QPushButton( buttonGroup, "terminateButton" );
  terminateButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, terminateButton->sizePolicy().hasHeightForWidth() ) );
  {
  QPixmap pm(terminate_xpm);
  pm.setMask(pm.createHeuristicMask());
  terminateButton->setPixmap( pm );
  }
  buttonGroupLayout->addWidget( terminateButton );
  terminateButton->setText( QString::null );
#else //OLDWAY
  {
  terminateButton = new AnimatedQPushButton( buttonGroup, "terminateButton", FALSE );
  QPixmap *pm = new QPixmap( terminate_xpm );
  pm->setMask(pm->createHeuristicMask());
  terminateButton->setPixmap( *pm );
  terminateButton->push_back( pm );
  terminateButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, terminateButton->sizePolicy().hasHeightForWidth() ) );
  buttonGroupLayout->addWidget( terminateButton );
  terminateButton->setText( QString::null );
  }
#endif //OLDWAY

  statusLayout = new QHBoxLayout( 0, 10, 0, "statusLayout" );
  
  statusLabel = new QLabel( getBaseWidgetFrame(), "statusLabel");
  statusLayout->addWidget( statusLabel );

  statusLabelText = new QLineEdit( getBaseWidgetFrame(), "statusLabelText");
  statusLabelText->setReadOnly(TRUE);
  statusLayout->addWidget( statusLabelText );

  frameLayout->addLayout( statusLayout );

  languageChange();

  // set button look.
  attachProcessButton->setFlat(TRUE);
  detachProcessButton->setFlat(TRUE);
  attachCollectorButton->setFlat(TRUE);
  detachCollectorButton->setFlat(TRUE);
  runButton->setFlat(TRUE);
  pauseButton->setFlat(TRUE);
  continueButton->setFlat(TRUE);
  updateButton->setFlat(TRUE);
  interruptButton->setFlat(TRUE);
  terminateButton->setFlat(TRUE);

  // set button sensitivities.
  attachProcessButton->setEnabled(TRUE);
  attachCollectorButton->setEnabled(TRUE);
  runButton->setEnabled(TRUE);
  pauseButton->setEnabled(FALSE);
  continueButton->setEnabled(FALSE);
  updateButton->setEnabled(FALSE);
  detachProcessButton->setEnabled(TRUE);
  terminateButton->setEnabled(FALSE);


// signals and slots connections
  connect( attachProcessButton, SIGNAL( clicked() ), this, SLOT( attachProcessButtonSlot() ) );
  connect( detachProcessButton, SIGNAL( clicked() ), this, SLOT( detachProcessButtonSlot() ) );
  connect( attachCollectorButton, SIGNAL( clicked() ), this, SLOT( attachCollectorButtonSlot() ) );
  connect( detachCollectorButton, SIGNAL( clicked() ), this, SLOT( detachCollectorButtonSlot() ) );
  connect( runButton, SIGNAL( clicked() ), this, SLOT( runButtonSlot() ) );
  connect( continueButton, SIGNAL( clicked() ), this, SLOT( continueButtonSlot() ) );
  connect( interruptButton, SIGNAL( clicked() ), this, SLOT( interruptButtonSlot() ) );
  connect( pauseButton, SIGNAL( clicked() ), this, SLOT( pauseButtonSlot() ) );   
  connect( terminateButton, SIGNAL( clicked() ), this, SLOT( terminateButtonSlot() ) );
  connect( updateButton, SIGNAL( clicked() ), this, SLOT( updateButtonSlot() ) );

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



  topPC->splitVertical();
//  topPC->rightPanelContainer->splitVertical();

  pcSampleControlPanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;

//  topPC->dl_create_and_add_panel("Toolbar Panel", topPC->leftPanelContainer);
//  topPC->dl_create_and_add_panel("Top Five Panel", topPC->leftPanelContainer);
  topPC->dl_create_and_add_panel("Source Panel", topPC->leftPanelContainer);
  topPC->dl_create_and_add_panel("Command Panel", topPC->rightPanelContainer);
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
  contextMenu->insertItem("&Add Collector", this, SLOT(addCollectorSelected()), CTRL+Key_A );
  contextMenu->insertItem("&Remove Collector", this, SLOT(removeCollectorSelected()), CTRL+Key_R );
  contextMenu->insertSeparator();
  contextMenu->insertItem("&Add Process", this, SLOT(addProcessSelected()), CTRL+Key_A );
  contextMenu->insertItem("&Remove Process", this, SLOT(removeProcessSelected()), CTRL+Key_R );
  contextMenu->insertSeparator();
  contextMenu->insertItem("&Save As ...", this, SLOT(saveAsSelected()), CTRL+Key_S ); 

  return( TRUE );
}

void
pcSamplePanel::addCollectorSelected()
{
  printf("pcSamplePanel::addCollectorSelected()\n");
}   

void 
pcSamplePanel::removeCollectorSelected()
{
  printf("pcSamplePanel::removeCollectorSelected() requested.\n");
}

void
pcSamplePanel::addProcessSelected()
{
  printf("pcSamplePanel::addProcessSelected()\n");
}   

void 
pcSamplePanel::removeProcessSelected()
{
  printf("pcSamplePanel::removeProcessSelected() requested.\n");
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


void 
pcSamplePanel::attachProcessButtonSlot()
{
  printf("Attach Process\n");
}

void 
pcSamplePanel::detachProcessButtonSlot()
{
  printf("Detach Process\n");
}

void 
pcSamplePanel::attachCollectorButtonSlot()
{
  printf("Attach Collector\n");
}

void 
pcSamplePanel::detachCollectorButtonSlot()
{
  printf("Detach Collector\n");
}


void 
pcSamplePanel::runButtonSlot()
{
  printf("Run button pressed\n");

printf("# theApplication.startAllCollecting();\n");
printf("# theApplication.setStatus(Thread::Running\n");

  statusLabelText->setText( tr("Process running...") );

  attachProcessButton->setEnabled(TRUE);
  attachCollectorButton->setEnabled(TRUE);
  runButton->setEnabled(FALSE);
  runButton->enabledFLAG = FALSE;
  runButton->setFlat(TRUE);
  pauseButton->setEnabled(TRUE);
pauseButton->enabledFLAG = TRUE;
  continueButton->setEnabled(FALSE);
continueButton->enabledFLAG = FALSE;
  updateButton->setEnabled(TRUE);
updateButton->enabledFLAG = TRUE;
  detachProcessButton->setEnabled(TRUE);
  detachCollectorButton->setEnabled(TRUE);
  terminateButton->setEnabled(TRUE);
terminateButton->enabledFLAG = TRUE;
}


void 
pcSamplePanel::pauseButtonSlot()
{
  printf("Pause button pressed\n");
printf("# theApplication.setStatus(Thread::Suspended)\n");;

  statusLabelText->setText( tr("Process suspended...") );

  pauseButton->setEnabled(FALSE);
pauseButton->enabledFLAG = TRUE;
pauseButton->setFlat(TRUE);
  continueButton->setEnabled(TRUE);
continueButton->enabledFLAG = TRUE;
continueButton->setFlat(TRUE);
}


void 
pcSamplePanel::continueButtonSlot()
{
  printf("Continue button pressed.\n");
printf("# theApplication.setStatus(Thread::Running\n");

  statusLabelText->setText( tr("Process running...") );

  pauseButton->setEnabled(TRUE);
pauseButton->enabledFLAG = TRUE;
pauseButton->setFlat(TRUE);
  continueButton->setEnabled(FALSE);
continueButton->enabledFLAG = TRUE;
continueButton->setFlat(TRUE);
}


void 
pcSamplePanel::updateButtonSlot()
{
  printf("Update button pressed.\n");
printf("Get some data!\n");
}



void 
pcSamplePanel::interruptButtonSlot()
{
  printf("Interrupt button pressed\n");
}


void 
pcSamplePanel::terminateButtonSlot()
{
  printf("Terminate button pressed\n");
printf("# theApplication.stopAllCollecting();???????   \n"); 
printf("prompt for saving data.\n");
printf("# if( response is yes, then called theApplication.saveAs();\n");

  statusLabelText->setText( tr("Process terminated...") );

  // set button sensitivities.
  attachProcessButton->setEnabled(TRUE);
  attachCollectorButton->setEnabled(TRUE);
  runButton->setEnabled(TRUE);
  runButton->enabledFLAG = TRUE;
pauseButton->setEnabled(FALSE);
pauseButton->enabledFLAG = FALSE;
  continueButton->setEnabled(FALSE);
continueButton->setEnabled(FALSE);
continueButton->enabledFLAG = FALSE;
  updateButton->setEnabled(FALSE);
updateButton->setEnabled(FALSE);
updateButton->enabledFLAG = FALSE;
  detachProcessButton->setEnabled(TRUE);
  detachCollectorButton->setEnabled(TRUE);
  terminateButton->setEnabled(FALSE);
terminateButton->setFlat(TRUE);
terminateButton->setEnabled(FALSE);
terminateButton->enabledFLAG = FALSE;
}

/*
*  Sets the strings of the subwidgets using the current
 *  language.
 */
void
pcSamplePanel::languageChange()
{
  buttonGroup->setTitle( tr( "Process Control" ) );
  QToolTip::add( attachProcessButton, tr( "Load or attach to a process." ) );
  QToolTip::add( detachProcessButton, tr( "Detach a process from the experiment(s)." ) );
  QToolTip::add( attachCollectorButton, tr( "Load or attach to a collector." ) );
  QToolTip::add( detachCollectorButton, tr( "Detach a collector." ) );
  QToolTip::add( runButton, tr( "Run the experiment." ) );
  QToolTip::add( pauseButton, tr( "Temporarily pause the experiment." ) );
  QToolTip::add( continueButton, tr( "Continue the experiment from current location." ) );
  QToolTip::add( updateButton, tr( "Update the display with the current information." ) );
  QToolTip::add( interruptButton, tr( "Interrupt the current action." ) );
  QToolTip::add( terminateButton, tr( "Terminate the experiment." ) );

  statusLabel->setText( tr("Status:") );
  statusLabelText->setText( tr("No status currently available.") );
}
