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
// Cut-n-paste from here...
    if ( !getName() )
	setName( "IntroWizardForm" );

    // Create a QScrollView to put all the widgets into.  This keeps us 
    // from messing up the Panel resize logic.  i.e. Sometimes other 
    // Panels in this same PanelContainer don't seem to resize properly.
    // It's because one of the Panels, isn't behaving well and won't
    // resize as small as the PanelContainer clip.
    sv = new QScrollView(getBaseWidgetFrame(), "scrollview");
    sv->setResizePolicy( QScrollView::Manual );

    iwpFrame = new QFrame(sv->viewport(), "iwpFrame");
iwpFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );

//    sv->setVScrollBarMode( QScrollView::AlwaysOn );
//    sv->setHScrollBarMode( QScrollView::AlwaysOn );

    IntroWizardFormLayout = new QVBoxLayout( iwpFrame, 0, 0, "IntroWizardFormLayout"); 

    mainFrame = new QFrame( iwpFrame, "mainFrame" );
mainFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, FALSE ) );
    mainFrame->setFrameShape( QFrame::StyledPanel );
    mainFrame->setFrameShadow( QFrame::Raised );
    mainFrameLayout = new QVBoxLayout( mainFrame, 1, 1, "mainFrameLayout"); 


    widgetStack5 = new QWidgetStack( mainFrame, "widgetStack5" );

    WStackPage = new QWidget( widgetStack5, "WStackPage" );
    vWStackPageLayout = new QVBoxLayout( WStackPage, 11, 6, "vWStackPageLayout"); 

    vWelcomeHeader = new QLabel( WStackPage, "vWelcomeHeader" );
    vWelcomeHeader->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, vWelcomeHeader->sizePolicy().hasHeightForWidth() ) );
    vWelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignHCenter  | QLabel::AlignTop ) );
    vWStackPageLayout->addWidget( vWelcomeHeader );

    vHelpfulLabel = new QLabel( WStackPage, "vHelpfulLabel" );
    vHelpfulLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, vHelpfulLabel->sizePolicy().hasHeightForWidth() ) );
    vHelpfulLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
    vWStackPageLayout->addWidget( vHelpfulLabel );

    vpage1LoadExperimentCheckBox = new QCheckBox( WStackPage, "vpage1LoadExperimentCheckBox" );
    vWStackPageLayout->addWidget( vpage1LoadExperimentCheckBox );

    line3 = new QFrame( WStackPage, "line3" );
    line3->setFrameShape( QFrame::HLine );
    line3->setFrameShadow( QFrame::Sunken );
    line3->setFrameShape( QFrame::HLine );
    vWStackPageLayout->addWidget( line3 );

// Indent the toggle boxes
    pcSampleHLayout = new QHBoxLayout( 0, 0, 6, "pcSampleHLayout"); 
    spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
    pcSampleHLayout->addItem( spacer5 );

    vpage1pcSampleRB = new QRadioButton( WStackPage, "vpage1pcSampleRB" );
    vpage1pcSampleRB->setChecked( TRUE );
    pcSampleHLayout->addWidget( vpage1pcSampleRB );
    vWStackPageLayout->addLayout( pcSampleHLayout );


    userTimeHLayout = new QHBoxLayout( 0, 0, 6, "userTimeHLayout"); 
    spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    userTimeHLayout->addItem( spacer6 );

    vpage1UserTimeRB = new QRadioButton( WStackPage, "vpage1UserTimeRB" );
    userTimeHLayout->addWidget( vpage1UserTimeRB );
    vWStackPageLayout->addLayout( userTimeHLayout );

    vHardwareCounterHLayout = new QHBoxLayout( 0, 0, 6, "vHardwareCounterHLayout"); 
    spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vHardwareCounterHLayout->addItem( spacer7 );

    vpage1HardwareCounterRB = new QRadioButton( WStackPage, "vpage1HardwareCounterRB" );
    vHardwareCounterHLayout->addWidget( vpage1HardwareCounterRB );
    vWStackPageLayout->addLayout( vHardwareCounterHLayout );

    vFloatingPointHLayout = new QHBoxLayout( 0, 0, 6, "vFloatingPointHLayout"); 
    spacer7_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vFloatingPointHLayout->addItem( spacer7_3 );

    vpage1FloatingPointRB = new QRadioButton( WStackPage, "vpage1FloatingPointRB" );
    vFloatingPointHLayout->addWidget( vpage1FloatingPointRB );
    vWStackPageLayout->addLayout( vFloatingPointHLayout );

    vInputOutputHLayout = new QHBoxLayout( 0, 0, 6, "vInputOutputHLayout"); 
    spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vInputOutputHLayout->addItem( spacer7_4 );

    vpage1InputOutputRB = new QRadioButton( WStackPage, "vpage1InputOutputRB" );
    vInputOutputHLayout->addWidget( vpage1InputOutputRB );
    vWStackPageLayout->addLayout( vInputOutputHLayout );

    vOtherHLayout = new QHBoxLayout( 0, 0, 6, "vOtherHLayout"); 
    spacer7_5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    vOtherHLayout->addItem( spacer7_5 );

    vOtherRB = new QRadioButton( WStackPage, "vOtherRB" );
    vOtherHLayout->addWidget( vOtherRB );
    vWStackPageLayout->addLayout( vOtherHLayout );

    vNextHLayout = new QHBoxLayout( 0, 0, 6, "vNextHLayout"); 
// Keep this one "Expanding so the Next button stays to the right.
    spacer44 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    vNextHLayout->addItem( spacer44 );

    vpage1NextButton = new QPushButton( WStackPage, "vpage1NextButton" );
    vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
    vNextHLayout->addWidget( vpage1NextButton );
    vWStackPageLayout->addLayout( vNextHLayout );
    widgetStack5->addWidget( WStackPage, 0 );




    WStackPage_2 = new QWidget( widgetStack5, "WStackPage_2" );
    eWStackPageLayout = new QVBoxLayout( WStackPage_2, 11, 6, "eWStackPageLayout"); 

    eWelcomeHeader = new QLabel( WStackPage_2, "eWelcomeHeader" );
    eWelcomeHeader->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eWelcomeHeader->sizePolicy().hasHeightForWidth() ) );
    eWelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignHCenter  | QLabel::AlignTop ) );
    eWStackPageLayout->addWidget( eWelcomeHeader );

    eHelpfulLabel = new QLabel( WStackPage_2, "eHelpfulLabel" );
    eHelpfulLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eHelpfulLabel->sizePolicy().hasHeightForWidth() ) );
    eHelpfulLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
    eWStackPageLayout->addWidget( eHelpfulLabel );

    eLoadExperimentHLayout = new QHBoxLayout( 0, 0, 6, "eLoadExperimentHLayout"); 
    spacer5_2_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eLoadExperimentHLayout->addItem( spacer5_2_2 );

    epage1LoadExperimentCheckBox = new QCheckBox( WStackPage_2, "epage1LoadExperimentCheckBox" );
    eLoadExperimentHLayout->addWidget( epage1LoadExperimentCheckBox );
    eWStackPageLayout->addLayout( eLoadExperimentHLayout );

    line2 = new QFrame( WStackPage_2, "line2" );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );
    eWStackPageLayout->addWidget( line2 );

    epcSampleHLayout = new QHBoxLayout( 0, 0, 6, "epcSampleHLayout"); 
    spacer5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    epcSampleHLayout->addItem( spacer5_2 );

    epage1pcSampleRB = new QRadioButton( WStackPage_2, "epage1pcSampleRB" );
    epage1pcSampleRB->setChecked( TRUE );
    epcSampleHLayout->addWidget( epage1pcSampleRB );
    eWStackPageLayout->addLayout( epcSampleHLayout );

    eUserTimeHLayout = new QHBoxLayout( 0, 0, 6, "eUserTimeHLayout"); 
    spacer6_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eUserTimeHLayout->addItem( spacer6_2 );

    epage1UserTimeRB = new QRadioButton( WStackPage_2, "epage1UserTimeRB" );
    eUserTimeHLayout->addWidget( epage1UserTimeRB );
    eWStackPageLayout->addLayout( eUserTimeHLayout );

    eHardwareCounterHLayout = new QHBoxLayout( 0, 0, 6, "eHardwareCounterHLayout"); 
    spacer7_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eHardwareCounterHLayout->addItem( spacer7_2 );

    epage1HardwareCounterRB = new QRadioButton( WStackPage_2, "epage1HardwareCounterRB" );
    eHardwareCounterHLayout->addWidget( epage1HardwareCounterRB );
    eWStackPageLayout->addLayout( eHardwareCounterHLayout );

    eFLoatingPointHLayout = new QHBoxLayout( 0, 0, 6, "eFLoatingPointHLayout"); 
    spacer7_3_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eFLoatingPointHLayout->addItem( spacer7_3_2 );

    epage1FloatingPointRB = new QRadioButton( WStackPage_2, "epage1FloatingPointRB" );
    eFLoatingPointHLayout->addWidget( epage1FloatingPointRB );
    eWStackPageLayout->addLayout( eFLoatingPointHLayout );

    eInputOutputHLayout = new QHBoxLayout( 0, 0, 6, "eInputOutputHLayout"); 
    spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eInputOutputHLayout->addItem( spacer7_4_2 );

    epage1InputOutputRB = new QRadioButton( WStackPage_2, "epage1InputOutputRB" );
    eInputOutputHLayout->addWidget( epage1InputOutputRB );
    eWStackPageLayout->addLayout( eInputOutputHLayout );

    eOtherHLayout = new QHBoxLayout( 0, 0, 6, "eOtherHLayout"); 
    spacer7_5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    eOtherHLayout->addItem( spacer7_5_2 );

    eOtherRB = new QRadioButton( WStackPage_2, "eOtherRB" );
    eOtherHLayout->addWidget( eOtherRB );
    eWStackPageLayout->addLayout( eOtherHLayout );

    eNextHLayout = new QHBoxLayout( 0, 0, 6, "eNextHLayout"); 
// Keep this Expanding so the Next button stays attached to the right.
    spacer37 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    eNextHLayout->addItem( spacer37 );

    epage1NextButton = new QPushButton( WStackPage_2, "epage1NextButton" );
    epage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1NextButton->sizePolicy().hasHeightForWidth() ) );
    eNextHLayout->addWidget( epage1NextButton );
    eWStackPageLayout->addLayout( eNextHLayout );
    widgetStack5->addWidget( WStackPage_2, 1 );
    mainFrameLayout->addWidget( widgetStack5 );
    IntroWizardFormLayout->addWidget( mainFrame );

    wizardModeHLayout = new QHBoxLayout( 0, 0, 6, "wizardModeHLayout"); 

    wizardMode = new QCheckBox( iwpFrame, "wizardMode" );
    wizardMode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, wizardMode->sizePolicy().hasHeightForWidth() ) );
    wizardMode->setChecked( TRUE );
    wizardModeHLayout->addWidget( wizardMode );
    spacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    wizardModeHLayout->addItem( spacer1 );

    broughtToYouByLabel = new QLabel( iwpFrame, "broughtToYouByLabel" );
    broughtToYouByLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, broughtToYouByLabel->sizePolicy().hasHeightForWidth() ) );
    wizardModeHLayout->addWidget( broughtToYouByLabel );
    IntroWizardFormLayout->addLayout( wizardModeHLayout );
    languageChange();
setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
//    clearWState( WState_Polished );

    // signals and slots connections
    connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
    connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
    connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( epage1NextButtonSelected() ) );

    connect( vpage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(vpage1pcSampleRBChanged() ) );
    connect( vpage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(vpage1UserTimeRBChanged() ) );
    connect( vpage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(vpage1HardwareCounterRBChanged() ) );
    connect( vpage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(vpage1FloatingPointRBChanged() ) );
    connect( vpage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(vpage1InputOutputRBChanged() ) );
    connect( vOtherRB, SIGNAL( clicked() ), this, SLOT(vOtherRBChanged() ) );

    connect( epage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(epage1pcSampleRBChanged() ) );
    connect( epage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(epage1UserTimeRBChanged() ) );
    connect( epage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(epage1HardwareCounterRBChanged() ) );
    connect( epage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(epage1FloatingPointRBChanged() ) );
    connect( epage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(epage1InputOutputRBChanged() ) );
    connect( eOtherRB, SIGNAL( clicked() ), this, SLOT(epage1InputOutputRBChanged() ) );
    connect( eOtherRB, SIGNAL( clicked() ), this, SLOT(eOtherRBChanged() ) );
// Cut-n-paste to here...

#ifdef DEBUG_WITH_COLOR
getBaseWidgetFrame()->setPaletteBackgroundColor("blue");
mainFrame->setPaletteBackgroundColor("pink");
iwpFrame->setPaletteBackgroundColor("orange");
sv->viewport()->setPaletteBackgroundColor("red");
sv->viewport()->setPaletteForegroundColor("green");
#endif // DEBUG_WITH_COLOR
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
    vOtherRB->setText( tr( "I'm looking for something else.  Show me more options." ) );
    vpage1NextButton->setText( tr( "> Next" ) );
    eWelcomeHeader->setText( tr( "<h2>Welcome to Open/SpeedShop(tm)</h2>" ) );
    eHelpfulLabel->setText( tr( "Please select which of the following are true for your application:" ) );
    epage1LoadExperimentCheckBox->setText( tr( "Load experiment data" ) );
    epage1pcSampleRB->setText( tr( "pcSampling (profiling)" ) );
    epage1UserTimeRB->setText( tr( "User Time Experiment." ) );
    epage1HardwareCounterRB->setText( tr( "Hardware Counter Tracing." ) );
    epage1FloatingPointRB->setText( tr( "Floating Point Exceptions Tracing" ) );
    epage1InputOutputRB->setText( tr( "Input/Output Tracing" ) );
    eOtherRB->setText( tr( "Other ..." ) );
    epage1NextButton->setText( tr( "> Next" ) );
    wizardMode->setText( tr( "Verbose Wizard Mode" ) );
    broughtToYouByLabel->setText( tr( "Brought to you by SGI (SiliconGraphics)" ) );
}

void IntroWizardPanel::wizardModeSelected()
{
  if( wizardMode->isOn() )
  {
    widgetStack5->raiseWidget(0);
  } else
  {
    widgetStack5->raiseWidget(1);
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
    p = getPanelContainer()->raiseNamedPanel("pc Sample Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer());
    }
  }
  if( epage1UserTimeRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("User Time Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer());
    }
  }
  if( epage1HardwareCounterRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("HW Counter Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer());
    }
  }
  if( epage1FloatingPointRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("FPE Tracing Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer());
    }
  }
  if( epage1InputOutputRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("IO Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer());
    }
  }
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
    p = getPanelContainer()->raiseNamedPanel("pc Sample Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer());
    }
  }
  if( vpage1UserTimeRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("User Time Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer());
    }
  }
  if( vpage1HardwareCounterRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("HW Counter Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer());
    }
  }
  if( vpage1FloatingPointRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("FPE Tracing Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer());
    }
  }
  if( vpage1InputOutputRB->isOn() )
  {
    p = getPanelContainer()->raiseNamedPanel("IO Wizard");
    if( !p )
    {
      getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer());
    }
  }
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

void IntroWizardPanel::vOtherRBChanged()
{
  vSetStateChanged(vOtherRB);
}

void IntroWizardPanel::vSetStateChanged(QRadioButton *rb)
{
  vpage1pcSampleRB->setChecked( FALSE );
  vpage1UserTimeRB->setChecked( FALSE );
  vpage1HardwareCounterRB->setChecked( FALSE );
  vpage1FloatingPointRB->setChecked( FALSE );
  vpage1InputOutputRB->setChecked( FALSE );
  vOtherRB->setChecked( FALSE );
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

void IntroWizardPanel::eOtherRBChanged()
{
  eSetStateChanged(eOtherRB);
}

void IntroWizardPanel::eSetStateChanged(QRadioButton *rb)
{
  epage1pcSampleRB->setChecked( FALSE );
  epage1UserTimeRB->setChecked( FALSE );
  epage1HardwareCounterRB->setChecked( FALSE );
  epage1FloatingPointRB->setChecked( FALSE );
  epage1InputOutputRB->setChecked( FALSE );
  eOtherRB->setChecked( FALSE );
  rb->setChecked(TRUE);
}

void IntroWizardPanel::handleSizeEvent( QResizeEvent *e )
{

  int width=100;
  int height=100;



  width=getBaseWidgetFrame()->width();
  height=getBaseWidgetFrame()->height();

//  printf("IntroWizardPanel::viewportResizeEvent(%d,%d) entered\n", width, height);
// This is the actual size of the realestate of the created widgets.
// I'm not sure (yet) just how to calculate the exact size... 
// At somepoint, we'll want to calculate this based on all widgets in 
// the collection.
// printf("width()=%d height()=%d\n", width, height );
// printf("iwpFrame->width()=%d iwpFrame->height()=%d\n", iwpFrame->width(), iwpFrame->height() );
  sv->resizeContents(iwpFrame->width(),iwpFrame->height());

  sv->resize(width, height);

// This is the size that really needs to be the size of width/height.
// It's the size of the view (i.e. Like a house window.) of the data
// behind it.
int h = height > 450 ? height : 450;
  iwpFrame->resize(width-20, h);
}
