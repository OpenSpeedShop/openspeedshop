#include "ProcessControlObject.hxx"

#include "ControlObject.hxx"


#include <qapplication.h>
#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qiconset.h>

#include <qbitmap.h>
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



/*! The default constructor.   Unused. */
ProcessControlObject::ProcessControlObject(QVBoxLayout *frameLayout, QWidget *baseWidget, Panel *p)
{ // Unused... Here for completeness...
  printf("ProcessControlObject::ProcessControlObject() constructor called\n");

  panel = p;

  buttonGroup = new QButtonGroup( baseWidget, "buttonGroup" );
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

  {
  QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum );
  buttonGroupLayout->addItem(spacer);
  }

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

  languageChange();
}


ProcessControlObject::~ProcessControlObject()
{
//  printf("  ProcessControlObject::~ProcessControlObject() destructor called\n");
//  delete frameLayout;
//  delete baseWidgetFrame;
}

void 
ProcessControlObject::attachProcessButtonSlot()
{
  printf("PCO: Attach Process\n");

  ControlObject *co = new ControlObject(ATTACH_PROCESS_T);
  panel->listener((void *)co);
  delete co;
}

void 
ProcessControlObject::detachProcessButtonSlot()
{
  printf("PCO: Detach Process\n");

  ControlObject *co = new ControlObject(DETACH_PROCESS_T);
  panel->listener((void *)co);
  delete co;
}

void 
ProcessControlObject::attachCollectorButtonSlot()
{
  printf("PCO: Attach Collector\n");

  ControlObject *co = new ControlObject(ATTACH_COLLECTOR_T);
  panel->listener((void *)co);
  delete co;
}

void 
ProcessControlObject::detachCollectorButtonSlot()
{
  printf("PCO: Detach Collector\n");

  ControlObject *co = new ControlObject(REMOVE_COLLECTOR_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::runButtonSlot()
{
  printf("PCO: Run button pressed\n");

printf("# theApplication.startAllCollecting();\n");
printf("# theApplication.setStatus(Thread::Running\n");

//  statusLabelText->setText( tr("Process running...") );

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

  ControlObject *co = new ControlObject(RUN_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::pauseButtonSlot()
{
  printf("PCO: Pause button pressed\n");
printf("# theApplication.setStatus(Thread::Suspended)\n");;

//  statusLabelText->setText( tr("Process suspended...") );

  pauseButton->setEnabled(FALSE);
  pauseButton->enabledFLAG = FALSE;
  pauseButton->setFlat(TRUE);
  continueButton->setEnabled(TRUE);
  continueButton->enabledFLAG = TRUE;
  continueButton->setFlat(TRUE);

  ControlObject *co = new ControlObject(PAUSE_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::continueButtonSlot()
{
  printf("PCO: Continue button pressed.\n");
printf("# theApplication.setStatus(Thread::Running\n");

//  statusLabelText->setText( tr("Process running...") );

  pauseButton->setEnabled(TRUE);
  pauseButton->enabledFLAG = TRUE;
  pauseButton->setFlat(TRUE);
  continueButton->setEnabled(FALSE);
  continueButton->enabledFLAG = TRUE;
  continueButton->setFlat(TRUE);

  ControlObject *co = new ControlObject(CONT_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::updateButtonSlot()
{
  printf("PCO: Update button pressed.\n");
printf("Get some data!\n");

  ControlObject *co = new ControlObject(UPDATE_T);
  panel->listener((void *)co);
  delete co;
}



void 
ProcessControlObject::interruptButtonSlot()
{
  printf("PCO: Interrupt button pressed\n");

  ControlObject *co = new ControlObject(INTERRUPT_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::terminateButtonSlot()
{
  printf("PCO: Terminate button pressed\n");
printf("# theApplication.stopAllCollecting();???????   \n"); 
printf("prompt for saving data.\n");
printf("# if( response is yes, then called theApplication.saveAs();\n");

//  statusLabelText->setText( tr("Process terminated...") );

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

  ControlObject *co = new ControlObject(TERMINATE_T);
  panel->listener((void *)co);
  delete co;
}

/*
*  Sets the strings of the subwidgets using the current
 *  language.
 */
void
ProcessControlObject::languageChange()
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

//  statusLabel->setText( tr("Status:") );
//  statusLabelText->setText( tr("No status currently available.") );
}
