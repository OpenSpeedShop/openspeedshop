#include "ProcessControlObject.hxx"

#include "ControlObject.hxx"


#include <qapplication.h>
#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qiconset.h>

#include <qbitmap.h>
#include "run.xpm"
#include "pause.xpm"
#include "cont.xpm"
#include "update.xpm"
#include "interrupt.xpm"
#include "terminate.xpm"

#include "debug.hxx"



/*! The default constructor.   Unused. */
ProcessControlObject::ProcessControlObject(QVBoxLayout *frameLayout, QWidget *baseWidget, Panel *p)
{ // Unused... Here for completeness...
  nprintf( DEBUG_CONST_DESTRUCT ) ("ProcessControlObject::ProcessControlObject() constructor called\n");

  panel = p;

  buttonGroup = new QButtonGroup( baseWidget, "buttonGroup" );
  buttonGroup->setColumnLayout(0, Qt::Vertical );
  buttonGroup->layout()->setSpacing( 6 );
  buttonGroup->layout()->setMargin( 11 );
  // We want to keep the buttonGroup from resizing vertically.
  buttonGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, buttonGroup->sizePolicy().hasHeightForWidth() ) );
  QHBoxLayout *buttonGroupLayout = new QHBoxLayout( buttonGroup->layout() );
  buttonGroupLayout->setAlignment( Qt::AlignTop );

  frameLayout->addWidget( buttonGroup );

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
  runButton->setFlat(TRUE);
  pauseButton->setFlat(TRUE);
  continueButton->setFlat(TRUE);
  updateButton->setFlat(TRUE);
  interruptButton->setFlat(TRUE);
  terminateButton->setFlat(TRUE);

  // set button sensitivities.
  runButton->setEnabled(TRUE);
  pauseButton->setEnabled(FALSE);
  continueButton->setEnabled(FALSE);
  updateButton->setEnabled(FALSE);
  terminateButton->setEnabled(FALSE);


// signals and slots connections
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
//  nprintf( DEBUG_CONST_DESTRUCT ) ("  ProcessControlObject::~ProcessControlObject() destructor called\n");
//  delete frameLayout;
//  delete baseWidgetFrame;
}

void 
ProcessControlObject::runButtonSlot()
{
  nprintf( DEBUG_PANELS ) ("PCO: Run button pressed\n");

//  statusLabelText->setText( tr("Process running...") );

  runButton->setEnabled(FALSE);
  runButton->enabledFLAG = FALSE;
  runButton->setFlat(TRUE);
  pauseButton->setEnabled(TRUE);
  pauseButton->enabledFLAG = TRUE;
  continueButton->setEnabled(FALSE);
  continueButton->enabledFLAG = FALSE;
  updateButton->setEnabled(TRUE);
  updateButton->enabledFLAG = TRUE;
  terminateButton->setEnabled(TRUE);
  terminateButton->enabledFLAG = TRUE;

  ControlObject *co = new ControlObject(RUN_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::pauseButtonSlot()
{
  nprintf( DEBUG_PANELS ) ("PCO: Pause button pressed\n");

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
  nprintf( DEBUG_PANELS ) ("PCO: Continue button pressed.\n");

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
  nprintf( DEBUG_PANELS ) ("PCO: Update button pressed.\n");
printf("Get some data!\n");

  ControlObject *co = new ControlObject(UPDATE_T);
  panel->listener((void *)co);
  delete co;
}



void 
ProcessControlObject::interruptButtonSlot()
{
  nprintf( DEBUG_PANELS ) ("PCO: Interrupt button pressed\n");

  ControlObject *co = new ControlObject(INTERRUPT_T);
  panel->listener((void *)co);
  delete co;
}


void 
ProcessControlObject::terminateButtonSlot()
{
  nprintf( DEBUG_PANELS ) ("PCO: Terminate button pressed\n");

//  statusLabelText->setText( tr("Process terminated...") );

  // set button sensitivities.
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
  QToolTip::add( runButton, tr( "Run the experiment." ) );
  QToolTip::add( pauseButton, tr( "Temporarily pause the experiment." ) );
  QToolTip::add( continueButton, tr( "Continue the experiment from current location." ) );
  QToolTip::add( updateButton, tr( "Update the display with the current information." ) );
  QToolTip::add( interruptButton, tr( "Interrupt the current action." ) );
  QToolTip::add( terminateButton, tr( "Terminate the experiment." ) );

//  statusLabel->setText( tr("Status:") );
//  statusLabelText->setText( tr("No status currently available.") );
}
