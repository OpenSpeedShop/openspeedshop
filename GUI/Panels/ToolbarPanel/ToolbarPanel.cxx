#include "ToolbarPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

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

/*!  ToolbarPanel Class
     This class is used by the script mknewpanel to create a new work area
     for the panel creator to design a new panel.


     Autor: Al Stipek (stipek@sgi.com)
 */


/*! The default constructor.   Unused. */
ToolbarPanel::ToolbarPanel()
{ // Unused... Here for completeness...
}


/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
ToolbarPanel::ToolbarPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf("ToolbarPanel::ToolbarPanel() constructor called.\n");
printf("ToolbarPanel::ToolbarPanel() obsoleted class.\n");
  QHBoxLayout * frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  QButtonGroup *buttonGroup = new QButtonGroup( getBaseWidgetFrame(), "buttonGroup" );
  buttonGroup->setColumnLayout(0, Qt::Vertical );
  buttonGroup->layout()->setSpacing( 6 );
  buttonGroup->layout()->setMargin( 11 );
  QHBoxLayout *buttonGroupLayout = new QHBoxLayout( buttonGroup->layout() );
  buttonGroupLayout->setAlignment( Qt::AlignTop );
  buttonGroup->setTitle( tr( "Process Control" ) );

  frameLayout->addWidget( buttonGroup );

  attachButton = new QPushButton( buttonGroup, "attachButton" );
  attachButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, attachButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(attach_xpm);
  pm.setMask(pm.createHeuristicMask());
  attachButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( attachButton );
  attachButton->setText( QString::null );
  QToolTip::add( attachButton, tr( "Load or attach to a process." ) );

  detachButton = new QPushButton( buttonGroup, "detachButton" );
  detachButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, detachButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(detach_xpm);
  pm.setMask(pm.createHeuristicMask());
  detachButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( detachButton );
  detachButton->setText( QString::null );
  QToolTip::add( detachButton, tr( "Detach the experiment from the process(es)." ) );


{
QSpacerItem *spacer = new QSpacerItem( 5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
buttonGroupLayout->addItem(spacer);
}

  runButton = new QPushButton( buttonGroup, "runButton" );
  runButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, runButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(run_xpm);
  pm.setMask(pm.createHeuristicMask());
  runButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( runButton );
  runButton->setText( QString::null );
  QToolTip::add( runButton, tr( "Run the experiment." ) );

{
QSpacerItem *spacer = new QSpacerItem( 10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
buttonGroupLayout->addItem(spacer);
}


  pauseButton = new QPushButton( buttonGroup, "pauseButton" );
  pauseButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pauseButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(pause_xpm);
  pm.setMask(pm.createHeuristicMask());
  pauseButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( pauseButton );
  pauseButton->setText( QString::null );
  QToolTip::add( pauseButton, tr( "Temporarily pause the experiment." ) );

  continueButton = new QPushButton( buttonGroup, "continueButton" );
  continueButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, continueButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(cont_xpm);
  pm.setMask(pm.createHeuristicMask());
  continueButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( continueButton );
  continueButton->setText( QString::null );
  QToolTip::add( continueButton, tr( "Continue the experiment from current location." ) );

  updateButton = new QPushButton( buttonGroup, "updateButton" );
  updateButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, updateButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(update_xpm);
  pm.setMask(pm.createHeuristicMask());
  updateButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( updateButton );
  updateButton->setText( QString::null );
  QToolTip::add( updateButton, tr( "Update the display with the current information." ) );

{
QSpacerItem *spacer = new QSpacerItem( 10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
buttonGroupLayout->addItem(spacer);
}

  interruptButton = new QPushButton( buttonGroup, "interruptButton" );
  interruptButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, interruptButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(interrupt_xpm);
  pm.setMask(pm.createHeuristicMask());
  interruptButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( interruptButton );
  interruptButton->setText( QString::null );
  QToolTip::add( interruptButton, tr( "Interrupt the current action." ) );

{
QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum );
buttonGroupLayout->addItem(spacer);
}

  terminateButton = new QPushButton( buttonGroup, "terminateButton" );
  terminateButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, terminateButton->sizePolicy().hasHeightForWidth() ) );
{
  QPixmap pm(terminate_xpm);
  pm.setMask(pm.createHeuristicMask());
  terminateButton->setPixmap( pm );
}
  buttonGroupLayout->addWidget( terminateButton );
  terminateButton->setText( QString::null );
  QToolTip::add( terminateButton, tr( "Interrupt the current action." ) );

// signals and slots connections
  connect( attachButton, SIGNAL( clicked() ), this, SLOT( attachButtonSlot() ) );
  connect( detachButton, SIGNAL( clicked() ), this, SLOT( detachButtonSlot() ) );
  connect( runButton, SIGNAL( clicked() ), this, SLOT( runButtonSlot() ) );
  connect( continueButton, SIGNAL( clicked() ), this, SLOT( continueButtonSlot() ) );
  connect( interruptButton, SIGNAL( clicked() ), this, SLOT( interruptButtonSlot() ) );
  connect( pauseButton, SIGNAL( clicked() ), this, SLOT( pauseButtonSlot() ) );   
  connect( terminateButton, SIGNAL( clicked() ), this, SLOT( terminateButtonSlot() ) );
  connect( updateButton, SIGNAL( clicked() ), this, SLOT( updateButtonSlot() ) );


}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
ToolbarPanel::~ToolbarPanel()
{
  printf("  ToolbarPanel::~ToolbarPanel() destructor called.\n");
}

//! Add user panel specific menu items if they have any.
bool
ToolbarPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("ToolbarPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
ToolbarPanel::save()
{
  dprintf("ToolbarPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
ToolbarPanel::saveAs()
{
  dprintf("ToolbarPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
ToolbarPanel::listener(char *msg)
{
  dprintf("ToolbarPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
ToolbarPanel::broadcast(char *msg)
{
  dprintf("ToolbarPanel::broadcast() requested.\n");
  return 0;
}


void 
ToolbarPanel::attachButtonSlot()
{
  printf("Attach\n");
}


void 
ToolbarPanel::detachButtonSlot()
{
  printf("Detach\n");
}


void 
ToolbarPanel::runButtonSlot()
{
  printf("Run\n"); }


void 
ToolbarPanel::pauseButtonSlot()
{
  printf("Pause\n");
}


void 
ToolbarPanel::continueButtonSlot()
{
  printf("Continue\n");
}


void 
ToolbarPanel::updateButtonSlot()
{
  printf("Update\n");
}


void 
ToolbarPanel::interruptButtonSlot()
{
  printf("Interrupt\n");
}


void 
ToolbarPanel::terminateButtonSlot()
{
  printf("Terminate\n");
}
