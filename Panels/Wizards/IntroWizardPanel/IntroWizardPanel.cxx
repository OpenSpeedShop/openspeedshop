/*! \class IntroWizardPanel
     This defines the starting point for entry level users.
     asks the highest level questions attempting to take 
     the user to a more specific wizard panel (i.e. pc sampline
     wizrard)

     This is only prototype code.   It was created using
     Qt Designer, then the code was pulled into this class.

     Autor: Al Stipek (stipek@sgi.com)
 */

#include "IntroWizardPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove


#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qfile.h>   // For the file dialog box.
#include <qfiledialog.h>  // For the file dialog box.
#include <qmessagebox.h>
#include "qscrollview.h"



/*! The default constructor.   Unused. */
IntroWizardPanel::IntroWizardPanel()
{ // Unused... Here for completeness...
}


/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
IntroWizardPanel::IntroWizardPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  nprintf( DEBUG_CONST_DESTRUCT) ( "IntroWizardPanel::IntroWizardPanel() constructor called.\n");

    if ( !getName() )
	setName( "IntroWizardForm" );

    sv = new QScrollView(getBaseWidgetFrame(), "scrollview");
    sv->setResizePolicy( QScrollView::Manual );
    // I'm not calculating this, but rather just setting a "reasonable"
    // size.   Eventually this should be calculated.
    sv->resize(750,400);
    sv->resizeContents(800,450);

    IntroWizardFormLayout = new QVBoxLayout( sv->viewport(), 11, 6, "IntroWizardFormLayout"); 

    mainFrame = new QFrame( sv->viewport(), "mainFrame" );
    mainFrame->setFrameShape( QFrame::StyledPanel );
    mainFrame->setFrameShadow( QFrame::Raised );
    mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout"); 

    mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );

    vWStackPage = new QWidget( mainWidgetStack, "vWStackPage" );
    vWStackPageLayout = new QVBoxLayout( vWStackPage, 11, 6, "vWStackPageLayout"); 

    vWelcomeHeader = new QLabel( vWStackPage, "vWelcomeHeader" );
    vWelcomeHeader->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, vWelcomeHeader->sizePolicy().hasHeightForWidth() ) );
    vWelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    vWStackPageLayout->addWidget( vWelcomeHeader );

    vRBLayout = new QVBoxLayout( 0, 0, 6, "vRBLayout"); 

    vHelpfulLabel = new QLabel( vWStackPage, "vHelpfulLabel" );
    vHelpfulLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, vHelpfulLabel->sizePolicy().hasHeightForWidth() ) );
    vRBLayout->addWidget( vHelpfulLabel );

    vLoadExperimentLayout = new QHBoxLayout( 0, 0, 6, "vLoadExperimentLayout"); 
    spacer5_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vLoadExperimentLayout->addItem( spacer5_3 );

    vpage1LoadExperimentCheckBox = new QCheckBox( vWStackPage, "vpage1LoadExperimentCheckBox" );
    vLoadExperimentLayout->addWidget( vpage1LoadExperimentCheckBox );
    vRBLayout->addLayout( vLoadExperimentLayout );

    line3 = new QFrame( vWStackPage, "line3" );
    line3->setFrameShape( QFrame::HLine );
    line3->setFrameShadow( QFrame::Sunken );
    line3->setFrameShape( QFrame::HLine );
    vRBLayout->addWidget( line3 );

    vpcSampleRBLayout = new QHBoxLayout( 0, 0, 6, "vpcSampleRBLayout"); 
    spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vpcSampleRBLayout->addItem( spacer5 );

    vpage1pcSampleRB = new QRadioButton( vWStackPage, "vpage1pcSampleRB" );
    vpage1pcSampleRB->setChecked( TRUE );
    vpcSampleRBLayout->addWidget( vpage1pcSampleRB );
    vRBLayout->addLayout( vpcSampleRBLayout );

    vUserTimeRBLayout = new QHBoxLayout( 0, 0, 6, "vUserTimeRBLayout"); 
    spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vUserTimeRBLayout->addItem( spacer6 );

    vpage1UserTimeRB = new QRadioButton( vWStackPage, "vpage1UserTimeRB" );
    vUserTimeRBLayout->addWidget( vpage1UserTimeRB );
    vRBLayout->addLayout( vUserTimeRBLayout );

    vHWCounterRBLayout = new QHBoxLayout( 0, 0, 6, "vHWCounterRBLayout"); 
    spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vHWCounterRBLayout->addItem( spacer7 );

    vpage1HardwareCounterRB = new QRadioButton( vWStackPage, "vpage1HardwareCounterRB" );
    vHWCounterRBLayout->addWidget( vpage1HardwareCounterRB );
    vRBLayout->addLayout( vHWCounterRBLayout );

    vFloatingPointRBLayout = new QHBoxLayout( 0, 0, 6, "vFloatingPointRBLayout"); 
    spacer7_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vFloatingPointRBLayout->addItem( spacer7_3 );

    vpage1FloatingPointRB = new QRadioButton( vWStackPage, "vpage1FloatingPointRB" );
    vFloatingPointRBLayout->addWidget( vpage1FloatingPointRB );
    vRBLayout->addLayout( vFloatingPointRBLayout );

    vInputOutputRBLayout = new QHBoxLayout( 0, 0, 6, "vInputOutputRBLayout"); 
    spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vInputOutputRBLayout->addItem( spacer7_4 );

    vpage1InputOutputRB = new QRadioButton( vWStackPage, "vpage1InputOutputRB" );
    vInputOutputRBLayout->addWidget( vpage1InputOutputRB );
    vRBLayout->addLayout( vInputOutputRBLayout );

    vMPIRBLayout = new QHBoxLayout( 0, 0, 6, "vMPIRBLayout"); 
    spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vMPIRBLayout->addItem( spacer7_4 );

    vpage1MPIRB = new QRadioButton( vWStackPage, "vpage1MPIRB" );
    vMPIRBLayout->addWidget( vpage1MPIRB );
    vRBLayout->addLayout( vMPIRBLayout );

    vWStackPageLayout->addLayout( vRBLayout );

    vNextButtonLayout = new QHBoxLayout( 0, 0, 6, "vNextButtonLayout"); 
    spacer44 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    vNextButtonLayout->addItem( spacer44 );

    vpage1NextButton = new QPushButton( vWStackPage, "vpage1NextButton" );
    vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
    vNextButtonLayout->addWidget( vpage1NextButton );
    vWStackPageLayout->addLayout( vNextButtonLayout );
    mainWidgetStack->addWidget( vWStackPage, 0 );

    eWStackPage = new QWidget( mainWidgetStack, "eWStackPage" );
    WStackPageLayout_2 = new QVBoxLayout( eWStackPage, 11, 6, "WStackPageLayout_2"); 

    eWelcomeHeader = new QLabel( eWStackPage, "eWelcomeHeader" );
    eWelcomeHeader->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eWelcomeHeader->sizePolicy().hasHeightForWidth() ) );
    eWelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackPageLayout_2->addWidget( eWelcomeHeader );

    eRBLayout = new QVBoxLayout( 0, 0, 6, "eRBLayout"); 

    eHelpfulLabel = new QLabel( eWStackPage, "eHelpfulLabel" );
    eHelpfulLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eHelpfulLabel->sizePolicy().hasHeightForWidth() ) );
    eRBLayout->addWidget( eHelpfulLabel );

    eLoadExperimentRBLayout = new QHBoxLayout( 0, 0, 6, "eLoadExperimentRBLayout"); 
    spacer5_2_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eLoadExperimentRBLayout->addItem( spacer5_2_2 );

    epage1LoadExperimentCheckBox = new QCheckBox( eWStackPage, "epage1LoadExperimentCheckBox" );
    eLoadExperimentRBLayout->addWidget( epage1LoadExperimentCheckBox );
    eRBLayout->addLayout( eLoadExperimentRBLayout );

    line2 = new QFrame( eWStackPage, "line2" );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );
    eRBLayout->addWidget( line2 );

    epcSampleRBLayout = new QHBoxLayout( 0, 0, 6, "epcSampleRBLayout"); 
    spacer5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    epcSampleRBLayout->addItem( spacer5_2 );

    epage1pcSampleRB = new QRadioButton( eWStackPage, "epage1pcSampleRB" );
    epage1pcSampleRB->setChecked( TRUE );
    epcSampleRBLayout->addWidget( epage1pcSampleRB );
    eRBLayout->addLayout( epcSampleRBLayout );

    eUserTimeRBLayout = new QHBoxLayout( 0, 0, 6, "eUserTimeRBLayout"); 
    spacer6_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eUserTimeRBLayout->addItem( spacer6_2 );

    epage1UserTimeRB = new QRadioButton( eWStackPage, "epage1UserTimeRB" );
    eUserTimeRBLayout->addWidget( epage1UserTimeRB );
    eRBLayout->addLayout( eUserTimeRBLayout );

    eHWCounterRBLayout = new QHBoxLayout( 0, 0, 6, "eHWCounterRBLayout"); 
    spacer7_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eHWCounterRBLayout->addItem( spacer7_2 );

    epage1HardwareCounterRB = new QRadioButton( eWStackPage, "epage1HardwareCounterRB" );
    eHWCounterRBLayout->addWidget( epage1HardwareCounterRB );
    eRBLayout->addLayout( eHWCounterRBLayout );

    eFloatingPointRBLayout = new QHBoxLayout( 0, 0, 6, "eFloatingPointRBLayout"); 
    spacer7_3_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eFloatingPointRBLayout->addItem( spacer7_3_2 );

    epage1FloatingPointRB = new QRadioButton( eWStackPage, "epage1FloatingPointRB" );
    eFloatingPointRBLayout->addWidget( epage1FloatingPointRB );
    eRBLayout->addLayout( eFloatingPointRBLayout );

    eInputOutputRBLayout = new QHBoxLayout( 0, 0, 6, "eInputOutputRBLayout"); 
    spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eInputOutputRBLayout->addItem( spacer7_4_2 );

    epage1InputOutputRB = new QRadioButton( eWStackPage, "epage1InputOutputRB" );
    eInputOutputRBLayout->addWidget( epage1InputOutputRB );
    eRBLayout->addLayout( eInputOutputRBLayout );

    eMPIRBLayout = new QHBoxLayout( 0, 0, 6, "eMPIRBLayout"); 
    spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eMPIRBLayout->addItem( spacer7_4_2 );

    epage1MPIRB = new QRadioButton( eWStackPage, "epage1MPIRB" );
    eMPIRBLayout->addWidget( epage1MPIRB );
    eRBLayout->addLayout( eMPIRBLayout );

    WStackPageLayout_2->addLayout( eRBLayout );

    eNextButtonLayout = new QHBoxLayout( 0, 0, 6, "eNextButtonLayout"); 
    spacer37 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    eNextButtonLayout->addItem( spacer37 );

    epage1NextButton = new QPushButton( eWStackPage, "epage1NextButton" );
    epage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1NextButton->sizePolicy().hasHeightForWidth() ) );
    eNextButtonLayout->addWidget( epage1NextButton );
    WStackPageLayout_2->addLayout( eNextButtonLayout );
    mainWidgetStack->addWidget( eWStackPage, 1 );
    mainFrameLayout->addWidget( mainWidgetStack );
    IntroWizardFormLayout->addWidget( mainFrame );

    wizardModeLayout = new QHBoxLayout( 0, 0, 6, "wizardModeLayout"); 

    wizardMode = new QCheckBox( sv->viewport(), "wizardMode" );
    wizardMode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, wizardMode->sizePolicy().hasHeightForWidth() ) );
    wizardMode->setChecked( TRUE );
    wizardModeLayout->addWidget( wizardMode );
    spacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    wizardModeLayout->addItem( spacer1 );

    broughtToYouByLabel = new QLabel( sv->viewport(), "broughtToYouByLabel" );
    broughtToYouByLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, broughtToYouByLabel->sizePolicy().hasHeightForWidth() ) );
    wizardModeLayout->addWidget( broughtToYouByLabel );
    IntroWizardFormLayout->addLayout( wizardModeLayout );
    languageChange();
//    resize( QSize(610, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
    connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
    connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( epage1NextButtonSelected() ) );

    connect( vpage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(vpage1pcSampleRBChanged() ) );
    connect( vpage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(vpage1UserTimeRBChanged() ) );
    connect( vpage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(vpage1HardwareCounterRBChanged() ) );
    connect( vpage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(vpage1FloatingPointRBChanged() ) );
    connect( vpage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(vpage1InputOutputRBChanged() ) );
connect( vpage1MPIRB, SIGNAL( clicked() ), this, SLOT(vpage1MPIRBChanged() ) );

    connect( epage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(epage1pcSampleRBChanged() ) );
    connect( epage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(epage1UserTimeRBChanged() ) );
    connect( epage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(epage1HardwareCounterRBChanged() ) );
    connect( epage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(epage1FloatingPointRBChanged() ) );
    connect( epage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(epage1InputOutputRBChanged() ) );
connect( epage1MPIRB, SIGNAL( clicked() ), this, SLOT(epage1MPIRBChanged() ) );

#ifdef DEBUG_WITH_COLOR
sv->viewport()->setPaletteBackgroundColor("red");
sv->viewport()->setPaletteForegroundColor("green");
#endif // DEBUG_WITH_COLOR

// sv->viewport()->setPaletteBackgroundColor(getBaseWidgetFrame()->paletteBackgroundColor() );
sv->viewport()->setBackgroundColor(getBaseWidgetFrame()->backgroundColor() );

}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
IntroWizardPanel::~IntroWizardPanel()
{
  nprintf( DEBUG_CONST_DESTRUCT) ( "  IntroWizardPanel::~IntroWizardPanel() destructor called.\n");
}

//! Add user panel specific menu items if they have any.
bool
IntroWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
IntroWizardPanel::save()
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
IntroWizardPanel::saveAs()
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
IntroWizardPanel::listener(char *msg)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
IntroWizardPanel::broadcast(char *msg)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::broadcast() requested.\n");
  return 0;
}


void
IntroWizardPanel::languageChange()
{
    setCaption( tr( "IntroWizardForm" ) );
    vWelcomeHeader->setText( tr( "<h2>Welcome to Open/SpeedShop(tm)</h2>" ) );
    vHelpfulLabel->setText( tr( "Please select which of the following are true for your application:" ) );
    vpage1LoadExperimentCheckBox->setText( tr( "I already have experiment data and would like to analyze it." ) );
    vpage1pcSampleRB->setText( tr( "I'm trying to find where my program is spending most of it's time." ) );
    vpage1UserTimeRB->setText( tr( "I'd like to find out how much time is system time vs. my program's time." ) );
    vpage1HardwareCounterRB->setText( tr( "I'd like to see what the internal Hardware Counters can show me.\n"
"(Hardware Counters are an advanced feature that utilizes machine \n"
"hardware to measure certain attributes of program execution.)" ) );
    vpage1FloatingPointRB->setText( tr( "I need to measure how many times I am causing Floating Point Exceptions." ) );
    vpage1InputOutputRB->setText( tr( "My program does a lot of Input and Output and I'd like to trace that work." ) );
    vpage1MPIRB->setText( tr( "I have an MPI program and I'd like trace the mpi calls." ) );
    vpage1NextButton->setText( tr( "> Next" ) );
    eWelcomeHeader->setText( tr( "<h2>Welcome to Open/SpeedShop(tm)</h2>" ) );
    eHelpfulLabel->setText( tr( "Please select which of the following are true for your application:" ) );
    epage1LoadExperimentCheckBox->setText( tr( "Load experiment data" ) );
    epage1pcSampleRB->setText( tr( "pcSampling (profiling)" ) );
    epage1UserTimeRB->setText( tr( "User Time Experiment." ) );
    epage1HardwareCounterRB->setText( tr( "Hardware Counter Tracing." ) );
    epage1FloatingPointRB->setText( tr( "Floating Point Exceptions Tracing" ) );
    epage1InputOutputRB->setText( tr( "Input/Output Tracing" ) );
epage1MPIRB->setText( tr( "MPI Tracing" ) );
    epage1NextButton->setText( tr( "> Next" ) );
    wizardMode->setText( tr( "Verbose Wizard Mode" ) );
    broughtToYouByLabel->setText( tr( "Brought to you by SGI (SiliconGraphics)" ) );
}

void IntroWizardPanel::wizardModeSelected()
{
  if( wizardMode->isOn() )
  {
    mainWidgetStack->raiseWidget(0);
  } else
  {
    mainWidgetStack->raiseWidget(1);
  }

  // Before we swith reposition to top...
  sv->verticalScrollBar()->setValue(0);
  sv->horizontalScrollBar()->setValue(0);

  handleSizeEvent(NULL);
}

void IntroWizardPanel::epage1NextButtonSelected()
{
  Panel *p = NULL;
  if( vpage1LoadExperimentCheckBox->isOn() )
  {
    QString fn = QString::null;
    char *cwd = get_current_dir_name();
    fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.exp)", this, "open experiment dialog", "Choose an experiment file to open");
    free(cwd);
    if( !fn.isEmpty() )
    {
      printf("fn = %s\n", fn.ascii() );
      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
    } else
    {
      fprintf(stderr, "No experiment file name given.\n");
    }
    return;
  }
  if( epage1pcSampleRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&pc Sample Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer());
    }
  }
  if( epage1UserTimeRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&User Time Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer());
    }
  }
  if( epage1HardwareCounterRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&HW Counter Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer());
    }
  }
  if( epage1FloatingPointRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&FPE Tracing Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer());
    }
  }
  if( epage1InputOutputRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("I&O Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer());
    }
  }
  if( epage1MPIRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&MPI Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer());
    }
  }
  getPanelContainer()->hidePanel((Panel *)this);
}

void IntroWizardPanel::vpage1NextButtonSelected()
{
  Panel *p = NULL;
  if( vpage1LoadExperimentCheckBox->isOn() )
  {
    QString fn = QString::null;
    char *cwd = get_current_dir_name();
    fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.exp)", this, "open experiment dialog", "Choose an experiment file to open");
    free(cwd);
    if( !fn.isEmpty() )
    {
      printf("fn = %s\n", fn.ascii() );
      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
    } else
    {
      fprintf(stderr, "No experiment file name given.\n");
    }
    return;
  }
  if( vpage1pcSampleRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&pc Sample Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer());
    }
  }
  if( vpage1UserTimeRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&User Time Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer());
    }
  }
  if( vpage1HardwareCounterRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&HW Counter Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer());
    }
  }
  if( vpage1FloatingPointRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&FPE Tracing Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer());
    }
  }
  if( vpage1InputOutputRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("I&O Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer());
    }
  }
  if( vpage1MPIRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("&MPI Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer());
    }
  }

  getPanelContainer()->hidePanel((Panel *)this);
}

void IntroWizardPanel::vpage1pcSampleRBChanged()
{
  vSetStateChanged(vpage1pcSampleRB);
}

void IntroWizardPanel::vpage1UserTimeRBChanged()
{
  vSetStateChanged(vpage1UserTimeRB);
}

void IntroWizardPanel::vpage1HardwareCounterRBChanged()
{
  vSetStateChanged(vpage1HardwareCounterRB);
}

void IntroWizardPanel::vpage1FloatingPointRBChanged()
{
  vSetStateChanged(vpage1FloatingPointRB);
}

void IntroWizardPanel::vpage1InputOutputRBChanged()
{
  vSetStateChanged(vpage1InputOutputRB);
}

void IntroWizardPanel::vpage1MPIRBChanged()
{
  vSetStateChanged(vpage1MPIRB);
}

void IntroWizardPanel::vSetStateChanged(QRadioButton *rb)
{
  vpage1pcSampleRB->setChecked( FALSE );
  vpage1UserTimeRB->setChecked( FALSE );
  vpage1HardwareCounterRB->setChecked( FALSE );
  vpage1FloatingPointRB->setChecked( FALSE );
  vpage1InputOutputRB->setChecked( FALSE );
  vpage1MPIRB->setChecked( FALSE );
  rb->setChecked(TRUE);
}


void IntroWizardPanel::epage1pcSampleRBChanged()
{
  eSetStateChanged(epage1pcSampleRB);
}

void IntroWizardPanel::epage1UserTimeRBChanged()
{
  eSetStateChanged(epage1UserTimeRB);
}

void IntroWizardPanel::epage1HardwareCounterRBChanged()
{
  eSetStateChanged(epage1HardwareCounterRB);
}

void IntroWizardPanel::epage1FloatingPointRBChanged()
{
  eSetStateChanged(epage1FloatingPointRB);
}

void IntroWizardPanel::epage1InputOutputRBChanged()
{
  eSetStateChanged(epage1InputOutputRB);
}

void IntroWizardPanel::epage1MPIRBChanged()
{
  eSetStateChanged(epage1MPIRB);
}

void IntroWizardPanel::eSetStateChanged(QRadioButton *rb)
{
  epage1pcSampleRB->setChecked( FALSE );
  epage1UserTimeRB->setChecked( FALSE );
  epage1HardwareCounterRB->setChecked( FALSE );
  epage1FloatingPointRB->setChecked( FALSE );
  epage1InputOutputRB->setChecked( FALSE );
  epage1MPIRB->setChecked( FALSE );
  rb->setChecked(TRUE);
}

void IntroWizardPanel::handleSizeEvent( QResizeEvent *e )
{

  int width=100;
  int height=100;



  width=getBaseWidgetFrame()->width();
  height=getBaseWidgetFrame()->height();

  // We've got to reposition this on a resize or all alignment goes amuck...
  sv->verticalScrollBar()->setValue(0);

  sv->resize(width, height);
}
