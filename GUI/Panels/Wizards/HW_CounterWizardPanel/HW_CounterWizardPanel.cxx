#include "HW_CounterWizardPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include "ProcessListObject.hxx"  // For getting pid list off a host...

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>


/*!  \class HW_CounterWizardPanel

   Create a prototype fpe tracing wizard
 */


/*! The default constructor.   Unused. */
HW_CounterWizardPanel::HW_CounterWizardPanel()
{ // Unused... Here for completeness...
}


/*! Constructs a new HW_CounterPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
HW_CounterWizardPanel::HW_CounterWizardPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf("HW_CounterWizardPanel::HW_CounterWizardPanel() constructor called\n");
  frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

    if ( !name )
	setName( "HW_CounterWizardPanel" );

    QWidget* privateLayoutWidget = new QWidget( getBaseWidgetFrame(), "layout16" );
    privateLayoutWidget->setGeometry( QRect( 0, 0, 604, 482 ) );
    layout16 = new QVBoxLayout( privateLayoutWidget, 11, 6, "layout16"); 

    frame3 = new QFrame( privateLayoutWidget, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QVBoxLayout( frame3, 11, 6, "frame3Layout"); 

    userTimeWizardStack = new QWidgetStack( frame3, "userTimeWizardStack" );

    WStackVPage0 = new QWidget( userTimeWizardStack, "WStackVPage0" );
    WStackVPage0Layout = new QVBoxLayout( WStackVPage0, 11, 6, "WStackVPage0Layout"); 

    vWizardTitleLabel = new QLabel( WStackVPage0, "vWizardTitleLabel" );
    vWizardTitleLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, vWizardTitleLabel->sizePolicy().hasHeightForWidth() ) );
    vWizardTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackVPage0Layout->addWidget( vWizardTitleLabel );

    vWizardDescription = new QTextEdit( WStackVPage0, "vWizardDescription" );
    vWizardDescription->setEnabled( TRUE );
    vWizardDescription->setWordWrap( QTextEdit::WidgetWidth );
    vWizardDescription->setReadOnly( TRUE );
    WStackVPage0Layout->addWidget( vWizardDescription );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 
    spacer4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer4 );

    vpage0HideWizardCheckBox = new QCheckBox( WStackVPage0, "vpage0HideWizardCheckBox" );
    layout7->addWidget( vpage0HideWizardCheckBox );
    spacer4_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer4_2 );
    WStackVPage0Layout->addLayout( layout7 );

    layout17 = new QHBoxLayout( 0, 0, 6, "layout17"); 

    vpage0StartButton = new QPushButton( WStackVPage0, "vpage0StartButton" );
    vpage0StartButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage0StartButton->sizePolicy().hasHeightForWidth() ) );
    layout17->addWidget( vpage0StartButton );
    spacer20_2 = new QSpacerItem( 251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout17->addItem( spacer20_2 );

    vpage0NextButton = new QPushButton( WStackVPage0, "vpage0NextButton" );
    vpage0NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage0NextButton->sizePolicy().hasHeightForWidth() ) );
    layout17->addWidget( vpage0NextButton );
    WStackVPage0Layout->addLayout( layout17 );
    userTimeWizardStack->addWidget( WStackVPage0, 0 );

    WStackVPage1 = new QWidget( userTimeWizardStack, "WStackVPage1" );
    WStackVPage1Layout = new QVBoxLayout( WStackVPage1, 11, 6, "WStackVPage1Layout"); 

    vWizardOptionsLabel = new QLabel( WStackVPage1, "vWizardOptionsLabel" );
    vWizardOptionsLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, vWizardOptionsLabel->sizePolicy().hasHeightForWidth() ) );
    WStackVPage1Layout->addWidget( vWizardOptionsLabel );

    line8 = new QFrame( WStackVPage1, "line8" );
    line8->setFrameShape( QFrame::HLine );
    line8->setFrameShadow( QFrame::Sunken );
    line8->setFrameShape( QFrame::HLine );
    WStackVPage1Layout->addWidget( line8 );

    layout9 = new QVBoxLayout( 0, 0, 6, "layout9"); 

    rateHeaderLabel = new QLabel( WStackVPage1, "rateHeaderLabel" );
    rateHeaderLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, rateHeaderLabel->sizePolicy().hasHeightForWidth() ) );
    layout9->addWidget( rateHeaderLabel );

    layout1_2 = new QHBoxLayout( 0, 0, 6, "layout1_2"); 

    rateLabel = new QLabel( WStackVPage1, "rateLabel" );
    layout1_2->addWidget( rateLabel );

    vpage1SampleRateText = new QLineEdit( WStackVPage1, "vpage1SampleRateText" );
    layout1_2->addWidget( vpage1SampleRateText );
    layout9->addLayout( layout1_2 );
    WStackVPage1Layout->addLayout( layout9 );
    spacer7 = new QSpacerItem( 20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding );
    WStackVPage1Layout->addItem( spacer7 );

    layout31 = new QHBoxLayout( 0, 0, 6, "layout31"); 

    vpage1BackButton = new QPushButton( WStackVPage1, "vpage1BackButton" );
    vpage1BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1BackButton->sizePolicy().hasHeightForWidth() ) );
    layout31->addWidget( vpage1BackButton );

    vpage1ResetButton = new QPushButton( WStackVPage1, "vpage1ResetButton" );
    vpage1ResetButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1ResetButton->sizePolicy().hasHeightForWidth() ) );
    layout31->addWidget( vpage1ResetButton );

    vpage1NextButton = new QPushButton( WStackVPage1, "vpage1NextButton" );
    vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
    layout31->addWidget( vpage1NextButton );
    WStackVPage1Layout->addLayout( layout31 );
    userTimeWizardStack->addWidget( WStackVPage1, 1 );

    WStackVPage2 = new QWidget( userTimeWizardStack, "WStackVPage2" );
    WStackVPage2Layout = new QVBoxLayout( WStackVPage2, 11, 6, "WStackVPage2Layout"); 

    layout25 = new QVBoxLayout( 0, 0, 6, "layout25"); 
    spacer20 = new QSpacerItem( 20, 1, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout25->addItem( spacer20 );

    vFinishLabel = new QLabel( WStackVPage2, "vFinishLabel" );
    vFinishLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, vFinishLabel->sizePolicy().hasHeightForWidth() ) );
    layout25->addWidget( vFinishLabel );
    WStackVPage2Layout->addLayout( layout25 );

    layout7_2 = new QHBoxLayout( 0, 0, 6, "layout7_2"); 

    vpage2BackButton = new QPushButton( WStackVPage2, "vpage2BackButton" );
    vpage2BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage2BackButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2->addWidget( vpage2BackButton );

    vpage2FinishButton = new QPushButton( WStackVPage2, "vpage2FinishButton" );
    vpage2FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage2FinishButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2->addWidget( vpage2FinishButton );
    WStackVPage2Layout->addLayout( layout7_2 );
    userTimeWizardStack->addWidget( WStackVPage2, 2 );

    WStackEPage0 = new QWidget( userTimeWizardStack, "WStackEPage0" );
    WStackEPage0Layout = new QVBoxLayout( WStackEPage0, 11, 6, "WStackEPage0Layout"); 

    eWizardTitleLabel = new QLabel( WStackEPage0, "eWizardTitleLabel" );
    eWizardTitleLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eWizardTitleLabel->sizePolicy().hasHeightForWidth() ) );
    eWizardTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackEPage0Layout->addWidget( eWizardTitleLabel );

    eWizardDescription = new QTextEdit( WStackEPage0, "eWizardDescription" );
    eWizardDescription->setEnabled( TRUE );
    eWizardDescription->setWordWrap( QTextEdit::WidgetWidth );
    eWizardDescription->setReadOnly( TRUE );
    WStackEPage0Layout->addWidget( eWizardDescription );

    layout15 = new QHBoxLayout( 0, 0, 6, "layout15"); 
    spacer9 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout15->addItem( spacer9 );

    epage0HideWizardCheckBox = new QCheckBox( WStackEPage0, "epage0HideWizardCheckBox" );
    layout15->addWidget( epage0HideWizardCheckBox );
    WStackEPage0Layout->addLayout( layout15 );

    layout17_2 = new QHBoxLayout( 0, 0, 6, "layout17_2"); 

    epage0StartButton = new QPushButton( WStackEPage0, "epage0StartButton" );
    epage0StartButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage0StartButton->sizePolicy().hasHeightForWidth() ) );
    layout17_2->addWidget( epage0StartButton );
    spacer20_2_2 = new QSpacerItem( 251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout17_2->addItem( spacer20_2_2 );

    epage0NextButton = new QPushButton( WStackEPage0, "epage0NextButton" );
    epage0NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage0NextButton->sizePolicy().hasHeightForWidth() ) );
    layout17_2->addWidget( epage0NextButton );
    WStackEPage0Layout->addLayout( layout17_2 );
    userTimeWizardStack->addWidget( WStackEPage0, 3 );

    WStackEPage1 = new QWidget( userTimeWizardStack, "WStackEPage1" );
    WStackEPage1Layout = new QVBoxLayout( WStackEPage1, 11, 6, "WStackEPage1Layout"); 

    eWizardOptionsLabel = new QLabel( WStackEPage1, "eWizardOptionsLabel" );
    eWizardOptionsLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, eWizardOptionsLabel->sizePolicy().hasHeightForWidth() ) );
    WStackEPage1Layout->addWidget( eWizardOptionsLabel );

    line8_2 = new QFrame( WStackEPage1, "line8_2" );
    line8_2->setFrameShape( QFrame::HLine );
    line8_2->setFrameShadow( QFrame::Sunken );
    line8_2->setFrameShape( QFrame::HLine );
    WStackEPage1Layout->addWidget( line8_2 );

    layout9_2 = new QVBoxLayout( 0, 0, 6, "layout9_2"); 

    rateHeaderLabel_2 = new QLabel( WStackEPage1, "rateHeaderLabel_2" );
    rateHeaderLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, rateHeaderLabel_2->sizePolicy().hasHeightForWidth() ) );
    layout9_2->addWidget( rateHeaderLabel_2 );

    layout1_2_2 = new QHBoxLayout( 0, 0, 6, "layout1_2_2"); 

    rateLabel_2 = new QLabel( WStackEPage1, "rateLabel_2" );
    layout1_2_2->addWidget( rateLabel_2 );

    epage1SampleRateText = new QLineEdit( WStackEPage1, "epage1SampleRateText" );
    layout1_2_2->addWidget( epage1SampleRateText );
    layout9_2->addLayout( layout1_2_2 );
    WStackEPage1Layout->addLayout( layout9_2 );
    spacer7_2 = new QSpacerItem( 20, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
    WStackEPage1Layout->addItem( spacer7_2 );

    layout31_2 = new QHBoxLayout( 0, 0, 6, "layout31_2"); 

    epage1BackButton = new QPushButton( WStackEPage1, "epage1BackButton" );
    epage1BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1BackButton->sizePolicy().hasHeightForWidth() ) );
    layout31_2->addWidget( epage1BackButton );

    epage1ResetButton = new QPushButton( WStackEPage1, "epage1ResetButton" );
    epage1ResetButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1ResetButton->sizePolicy().hasHeightForWidth() ) );
    layout31_2->addWidget( epage1ResetButton );

    epage1NextButton = new QPushButton( WStackEPage1, "epage1NextButton" );
    epage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1NextButton->sizePolicy().hasHeightForWidth() ) );
    layout31_2->addWidget( epage1NextButton );
    WStackEPage1Layout->addLayout( layout31_2 );
    userTimeWizardStack->addWidget( WStackEPage1, 4 );

    WStackEPage2 = new QWidget( userTimeWizardStack, "WStackEPage2" );
    WStackEPage2Layout = new QVBoxLayout( WStackEPage2, 11, 6, "WStackEPage2Layout"); 

    eFinishLabel_2 = new QLabel( WStackEPage2, "eFinishLabel_2" );
    eFinishLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, eFinishLabel_2->sizePolicy().hasHeightForWidth() ) );
    WStackEPage2Layout->addWidget( eFinishLabel_2 );

    layout7_2_2 = new QHBoxLayout( 0, 0, 6, "layout7_2_2"); 

    epage2BackButton = new QPushButton( WStackEPage2, "epage2BackButton" );
    epage2BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage2BackButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2_2->addWidget( epage2BackButton );

    epage2FinishButton = new QPushButton( WStackEPage2, "epage2FinishButton" );
    epage2FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage2FinishButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2_2->addWidget( epage2FinishButton );
    WStackEPage2Layout->addLayout( layout7_2_2 );
    userTimeWizardStack->addWidget( WStackEPage2, 5 );
    frame3Layout->addWidget( userTimeWizardStack );
    layout16->addWidget( frame3 );

    layout15_2 = new QHBoxLayout( 0, 0, 6, "layout15_2"); 

    wizardMode = new QCheckBox( privateLayoutWidget, "wizardMode" );
    wizardMode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, wizardMode->sizePolicy().hasHeightForWidth() ) );
    wizardMode->setChecked( TRUE );
    layout15_2->addWidget( wizardMode );
    spacer15 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout15_2->addItem( spacer15 );

    broughtToYouByLabel = new QLabel( privateLayoutWidget, "broughtToYouByLabel" );
    broughtToYouByLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, broughtToYouByLabel->sizePolicy().hasHeightForWidth() ) );
    layout15_2->addWidget( broughtToYouByLabel );
    layout16->addLayout( layout15_2 );
    languageChange();
//    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( epage0HideWizardCheckBox, SIGNAL( clicked() ), this, SLOT( epage0HideWizardCheckBoxSelected() ) );
    connect( epage0StartButton, SIGNAL( clicked() ), this, SLOT( epage0StartButtonSelected() ) );
    connect( epage0NextButton, SIGNAL( clicked() ), this, SLOT( epage0NextButtonSelected() ) );
    connect( epage1BackButton, SIGNAL( clicked() ), this, SLOT( epage1BackButtonSelected() ) );
    connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( epage1NextButtonSelected() ) );
    connect( epage1ResetButton, SIGNAL( clicked() ), this, SLOT( epage1ResetButtonSelected() ) );
    connect( epage1SampleRateText, SIGNAL( returnPressed() ), this, SLOT( epage1SampleRateTextSelected() ) );
    connect( epage2BackButton, SIGNAL( clicked() ), this, SLOT( epage2BackButtonSelected() ) );
    connect( epage2FinishButton, SIGNAL( clicked() ), this, SLOT( epage2FinishButtonSelected() ) );
    connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
    connect( vpage0HideWizardCheckBox, SIGNAL( clicked() ), this, SLOT( vpage0HideWizardCheckBoxSelected() ) );
    connect( vpage0NextButton, SIGNAL( clicked() ), this, SLOT( vpage0NextButtonSelected() ) );
    connect( vpage0StartButton, SIGNAL( clicked() ), this, SLOT( vpage0StartButtonSelected() ) );
    connect( vpage1BackButton, SIGNAL( clicked() ), this, SLOT( vpage1BackButtonSelected() ) );
    connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
    connect( vpage1ResetButton, SIGNAL( clicked() ), this, SLOT( vpage1ResetButtonSelected() ) );
    connect( vpage1SampleRateText, SIGNAL( returnPressed() ), this, SLOT( vpage1SampleRateTextSelected() ) );
    connect( vpage2BackButton, SIGNAL( clicked() ), this, SLOT( vpage2BackButtonSelected() ) );
    connect( vpage2FinishButton, SIGNAL( clicked() ), this, SLOT( vpage2FinishButtonSelected() ) );

  // This next line makes it all magically appear and resize correctly.
  frameLayout->addWidget(privateLayoutWidget);
}


/*! Destroys the object and frees any allocated resources
 */
HW_CounterWizardPanel::~HW_CounterWizardPanel()
{
  printf("  HW_CounterWizardPanel::~HW_CounterWizardPanel() destructor called\n");
  delete frameLayout;

  delete baseWidgetFrame;
}


/*!
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void HW_CounterWizardPanel::languageChange()
{
    setCaption( tr( "HW_CounterWizardPanel" ) );
    vWizardTitleLabel->setText( tr( "<h1>HW Counter Wizard</h1>" ) );
    vWizardDescription->setText( tr( "Insert the description of a HW Counter Experiment." ) );
    vpage0HideWizardCheckBox->setText( tr( "Hide HW Counter Wizard next time HW Counter Experiment is selected.\n"
"(Note: You can change this back by going to the HW Counter local menu.)" ) );
    vpage0StartButton->setText( tr( "Start" ) );
    vpage0NextButton->setText( tr( "Next" ) );
    QToolTip::add( vpage0NextButton, tr( "Advance to the next wizard page." ) );
    vWizardOptionsLabel->setText( tr( "The following options (paramaters) are available to adjust.   These are the options the collector has exported.<br><br>\n"
"The smaller the number used for the sampling rate, the more\n"
"sampling detail will be show.   However, the trade off will be slower\n"
"performance and a larger data file.<br><br>\n"
"It may take a little expermenting to find the right setting for your \n"
"particular executable.   We suggest starting with the default setting\n"
"of 10." ) );
    rateHeaderLabel->setText( tr( "You can set the following option(s):" ) );
    rateLabel->setText( tr( "Sampling rate:" ) );
    vpage1SampleRateText->setText( tr( "30" ) );
    QToolTip::add( vpage1SampleRateText, tr( "The rate to sample.   (Default 30 milliseconds.)" ) );
    vpage1BackButton->setText( tr( "Back" ) );
    QToolTip::add( vpage1BackButton, tr( "Takes you back one page." ) );
    vpage1ResetButton->setText( tr( "Reset" ) );
    QToolTip::add( vpage1ResetButton, tr( "Reset the values to the default setings." ) );
    vpage1NextButton->setText( tr( "Next" ) );
    QToolTip::add( vpage1NextButton, tr( "Advance to the next wizard page." ) );
    vFinishLabel->setText( tr( "<p align=\"left\">(Verbose)<br>\n"
"Summare the HW Counter Experiment settings</p>" ) );
    vpage2BackButton->setText( tr( "Back" ) );
    QToolTip::add( vpage2BackButton, tr( "Takes you back one page." ) );
    vpage2FinishButton->setText( tr( "Finish..." ) );
    QToolTip::add( vpage2FinishButton, tr( "Finishes loading the wizard information and brings up a \"Control\" panel" ) );
    eWizardTitleLabel->setText( tr( "<h1>HW Counter Wizard</h1>" ) );
eWizardDescription->setText( tr( "The usertime experiment returns CPU time for each function while your program runs.\n"
"\n"
"This experiment uses statistical call stack profiling to measure inclusive and exclusive fpe tracing.\n"
"\n"
"The usertime experiment should incur a program execution slowdown of no more than 15%. Data from a usertime experiment is statistical in nature and shows some variance from run to run." ) );
    epage0HideWizardCheckBox->setText( tr( "Hide HW Counter Wizard next time HW Counter Experiment is selected.\n"
"(Note: You can change this back by going to the HW Counter local menu.)" ) );
    epage0StartButton->setText( tr( "Start" ) );
    epage0NextButton->setText( tr( "Next" ) );
    QToolTip::add( epage0NextButton, tr( "Advance to the next wizard page." ) );
    eWizardOptionsLabel->setText( tr( "(Expert) The following options (paramaters) are available to adjust.     <br>These are the options the collector has exported." ) );
    rateHeaderLabel_2->setText( tr( "You can set the following option(s):" ) );
    rateLabel_2->setText( tr( "Sampling rate:" ) );
    epage1SampleRateText->setText( tr( "30" ) );
    QToolTip::add( epage1SampleRateText, tr( "The rate to sample.   (Default 30 milliseconds.)" ) );
    epage1BackButton->setText( tr( "Back" ) );
    QToolTip::add( epage1BackButton, tr( "Takes you back one page." ) );
    epage1ResetButton->setText( tr( "Reset" ) );
    QToolTip::add( epage1ResetButton, tr( "Reset the values to the default setings." ) );
    epage1NextButton->setText( tr( "Next" ) );
    QToolTip::add( epage1NextButton, tr( "Advance to the next wizard page." ) );
    eFinishLabel_2->setText( tr( "<p align=\"left\">(Expert)<br><br>\n"
"You've selected a HW Counter experiment for executable \"%s\" to be run on host \"%s\".  Futher you've chosed a sample rate of \"%d\" milliseconds.<br><br>To complete the exeriment setup select the \"Finish\" button.<br><br>Upon selection of the \"Finish\" button an experiment \"Control\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>" ) );
    epage2BackButton->setText( tr( "Back" ) );
    QToolTip::add( epage2BackButton, tr( "Takes you back one page." ) );
    epage2FinishButton->setText( tr( "Finish..." ) );
    QToolTip::add( epage2FinishButton, tr( "Finishes loading the wizard information and brings up a \"Control\" panel" ) );
    wizardMode->setText( tr( "Verbose Wizard Mode" ) );
    broughtToYouByLabel->setText( tr( "Brought to you by SGI (SiliconGraphics)" ) );
}

/*! Add fpe tracing panel specific menu items if they have any. */
bool
HW_CounterWizardPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("HW_CounterWizardPanel::menu() requested.\n");

  return( FALSE );
}

/*! If the fpe tracing panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
HW_CounterWizardPanel::save()
{
  dprintf("HW_CounterWizardPanel::save() requested.\n");
}

/*! If the fpe tracing panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
HW_CounterWizardPanel::saveAs()
{
  dprintf("HW_CounterWizardPanel::saveAs() requested.\n");
}

/*! This function listens for messages. */
int 
HW_CounterWizardPanel::listener(char *msg)
{
  dprintf("HW_CounterWizardPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


/*! This function broadcasts messages. */
int 
HW_CounterWizardPanel::broadcast(char *msg)
{
  dprintf("HW_CounterWizardPanel::broadcast() requested.\n");
  return 0;
}


/*! Unimplemented */
void HW_CounterWizardPanel::epage0HideWizardCheckBoxSelected()
{
    qWarning( "HW_CounterWizardPanel::epage0HideWizardCheckBoxSelected(): Not implemented yet" );
}

/*! Goes way back and raises the Intro Wizard. */
void HW_CounterWizardPanel::epage0StartButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage0StartButtonSelected(): Not implemented yet" );

    Panel *p = panelContainer->raiseNamedPanel("Intro Wizard");
    if( !p )
    {
      panelContainer->_masterPC->dl_create_and_add_panel("Intro Wizard", panelContainer);
    }
}

/*! Takes you to the next page. */
void HW_CounterWizardPanel::epage0NextButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage0NextButtonSelected(): Not implemented yet" );

    ProcessListObject *plo = new ProcessListObject("localhost");
    plo->print();

    userTimeWizardStack->raiseWidget(WStackEPage1);
}

/*! Takes you to the previous page. */
void HW_CounterWizardPanel::epage1BackButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage1BackButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackEPage0);
}

/*! Takes you to the next page. */
void HW_CounterWizardPanel::epage1NextButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage1NextButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackEPage2);
}

void HW_CounterWizardPanel::epage1ResetButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage1ResetButtonSelected(): Not implemented yet" );
}

void HW_CounterWizardPanel::epage1SampleRateTextSelected()
{
    qWarning( "HW_CounterWizardPanel::epage1SampleRateTextSelected(): Not implemented yet" );
}

/*! Takes you to the previous page. */
void HW_CounterWizardPanel::epage2BackButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage2BackButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackEPage1);
}

void HW_CounterWizardPanel::epage2FinishButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::epage2FinishButtonSelected(): Not implemented yet" );
     panelContainer->_masterPC->dl_create_and_add_panel("HW Counter", panelContainer);
}

void HW_CounterWizardPanel::wizardModeSelected()
{
    qWarning( "HW_CounterWizardPanel::wizardModeSelected(): Not implemented yet" );

  QWidget *raisedWidget = userTimeWizardStack->visibleWidget();
if( raisedWidget == WStackVPage0 )
{
    printf("WStackVPage0\n");
} else if( raisedWidget ==  WStackVPage1 )
{
    printf("WStackVPage1\n");
} else if( raisedWidget == WStackVPage2 )
{
    printf("WStackVPage2\n");
} else if( raisedWidget  == WStackEPage0 )
{
    printf("WStackEPage0\n");
} else if( raisedWidget == WStackEPage1 )
{
    printf("WStackEPage1\n");
} else if( raisedWidget == WStackEPage2 )
{
    printf("WStackEPage2\n");
}

  if( wizardMode->isOn() )
  {
    if( raisedWidget  == WStackEPage0 )
    {
        printf("WStackEPage0\n");
        userTimeWizardStack->raiseWidget(WStackVPage0);
    } else if( raisedWidget == WStackEPage1 )
    {
        printf("WStackEPage1\n");
        userTimeWizardStack->raiseWidget(WStackVPage1);
    } else if( raisedWidget == WStackEPage2 )
    {
        printf("WStackEPage2\n");
        userTimeWizardStack->raiseWidget(WStackVPage2);
    } else
    {
        printf("Verbose to Expert: unknown WStackPage\n");
    }
  } else
  {
    if( raisedWidget == WStackVPage0 )
    {
        printf("WStackVPage0\n");
        userTimeWizardStack->raiseWidget(WStackEPage0);
    } else if( raisedWidget ==  WStackVPage1 )
    {
        printf("WStackVPage1\n");
        userTimeWizardStack->raiseWidget(WStackEPage1);
    } else if( raisedWidget == WStackVPage2 )
    {
        printf("WStackVPage2\n");
        userTimeWizardStack->raiseWidget(WStackEPage2);
    } else
    {
        printf("Expert to Verbose: unknown WStackPage\n");
    }
  }
}

void HW_CounterWizardPanel::vpage0HideWizardCheckBoxSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage0HideWizardCheckBoxSelected(): Not implemented yet" );
}

/*! Takes you to the next page. */
void HW_CounterWizardPanel::vpage0NextButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage0NextButtonSelected(): Not implemented yet" );

    ProcessListObject *plo = new ProcessListObject("localhost");
    plo->print();
 
    userTimeWizardStack->raiseWidget(WStackVPage1);
}

void HW_CounterWizardPanel::vpage0StartButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage0StartButtonSelected(): Not implemented yet" );

    Panel *p = panelContainer->raiseNamedPanel("Intro Wizard");
    if( !p )
    {
      panelContainer->_masterPC->dl_create_and_add_panel("Intro Wizard", panelContainer);
    }
}

/*! Takes you to the previous page. */
void HW_CounterWizardPanel::vpage1BackButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage1BackButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackVPage0);
}

/*! Takes you to the next page. */
void HW_CounterWizardPanel::vpage1NextButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage1NextButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackVPage2);
}

void HW_CounterWizardPanel::vpage1ResetButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage1ResetButtonSelected(): Not implemented yet" );
}

/*! Takes you to the previous page. */
void HW_CounterWizardPanel::vpage2BackButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage2BackButtonSelected(): Not implemented yet" );
    userTimeWizardStack->raiseWidget(WStackVPage1);
}

void HW_CounterWizardPanel::vpage2FinishButtonSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage2FinishButtonSelected(): Not implemented yet" );
     panelContainer->_masterPC->dl_create_and_add_panel("HW Counter", panelContainer);
}

void HW_CounterWizardPanel::vpage1SampleRateTextSelected()
{
    qWarning( "HW_CounterWizardPanel::vpage1SampleRateTextSelected(): Not implemented yet" );
}

