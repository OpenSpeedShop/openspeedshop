/****************************************************************************
** Form implementation generated from reading ui file 'pcsample.ui'
**
** Created: Mon Apr 19 11:32:42 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "pcsample.h"

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

#include "../pcsample.ui.h"
/*
 *  Constructs a pcsample as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
pcsample::pcsample( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "pcsample" );

    QWidget* privateLayoutWidget = new QWidget( this, "layout16" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 604, 482 ) );
    layout16 = new QVBoxLayout( privateLayoutWidget, 11, 6, "layout16"); 

    frame3 = new QFrame( privateLayoutWidget, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QVBoxLayout( frame3, 11, 6, "frame3Layout"); 

    pcSampleWizardStack = new QWidgetStack( frame3, "pcSampleWizardStack" );

    WStackVPage0 = new QWidget( pcSampleWizardStack, "WStackVPage0" );
    WStackVPage0Layout = new QVBoxLayout( WStackVPage0, 11, 6, "WStackVPage0Layout"); 

    pcSampleWizardTitleLabel = new QLabel( WStackVPage0, "pcSampleWizardTitleLabel" );
    pcSampleWizardTitleLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, pcSampleWizardTitleLabel->sizePolicy().hasHeightForWidth() ) );
    pcSampleWizardTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackVPage0Layout->addWidget( pcSampleWizardTitleLabel );

    textEdit1 = new QTextEdit( WStackVPage0, "textEdit1" );
    textEdit1->setWordWrap( QTextEdit::WidgetWidth );
    WStackVPage0Layout->addWidget( textEdit1 );

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
    pcSampleWizardStack->addWidget( WStackVPage0, 0 );

    WStackVPage1 = new QWidget( pcSampleWizardStack, "WStackVPage1" );
    WStackVPage1Layout = new QVBoxLayout( WStackVPage1, 11, 6, "WStackVPage1Layout"); 

    pcSampleWizardOptionsLabel_2 = new QLabel( WStackVPage1, "pcSampleWizardOptionsLabel_2" );
    pcSampleWizardOptionsLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pcSampleWizardOptionsLabel_2->sizePolicy().hasHeightForWidth() ) );
    WStackVPage1Layout->addWidget( pcSampleWizardOptionsLabel_2 );

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

    vpage1pcSampleRateText = new QLineEdit( WStackVPage1, "vpage1pcSampleRateText" );
    layout1_2->addWidget( vpage1pcSampleRateText );
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
    pcSampleWizardStack->addWidget( WStackVPage1, 1 );

    WStackVPage2 = new QWidget( pcSampleWizardStack, "WStackVPage2" );
    WStackVPage2Layout = new QVBoxLayout( WStackVPage2, 11, 6, "WStackVPage2Layout"); 

    layout25 = new QVBoxLayout( 0, 0, 6, "layout25"); 
    spacer20 = new QSpacerItem( 20, 1, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout25->addItem( spacer20 );

    psSampleFinishLabel = new QLabel( WStackVPage2, "psSampleFinishLabel" );
    psSampleFinishLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, psSampleFinishLabel->sizePolicy().hasHeightForWidth() ) );
    layout25->addWidget( psSampleFinishLabel );
    WStackVPage2Layout->addLayout( layout25 );

    layout7_2 = new QHBoxLayout( 0, 0, 6, "layout7_2"); 

    vpage2BackButton = new QPushButton( WStackVPage2, "vpage2BackButton" );
    vpage2BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage2BackButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2->addWidget( vpage2BackButton );

    vpage2FinishButton = new QPushButton( WStackVPage2, "vpage2FinishButton" );
    vpage2FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage2FinishButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2->addWidget( vpage2FinishButton );
    WStackVPage2Layout->addLayout( layout7_2 );
    pcSampleWizardStack->addWidget( WStackVPage2, 2 );

    WStackEPage0 = new QWidget( pcSampleWizardStack, "WStackEPage0" );
    WStackEPage0Layout = new QVBoxLayout( WStackEPage0, 11, 6, "WStackEPage0Layout"); 

    pcSampleWizardTitleLabel_2 = new QLabel( WStackEPage0, "pcSampleWizardTitleLabel_2" );
    pcSampleWizardTitleLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, pcSampleWizardTitleLabel_2->sizePolicy().hasHeightForWidth() ) );
    pcSampleWizardTitleLabel_2->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackEPage0Layout->addWidget( pcSampleWizardTitleLabel_2 );

    wizardDescriptionLabel = new QLabel( WStackEPage0, "wizardDescriptionLabel" );
    WStackEPage0Layout->addWidget( wizardDescriptionLabel );

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
    pcSampleWizardStack->addWidget( WStackEPage0, 3 );

    WStackEPage1 = new QWidget( pcSampleWizardStack, "WStackEPage1" );
    WStackEPage1Layout = new QVBoxLayout( WStackEPage1, 11, 6, "WStackEPage1Layout"); 

    pcSampleWizardOptionsLabel = new QLabel( WStackEPage1, "pcSampleWizardOptionsLabel" );
    pcSampleWizardOptionsLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pcSampleWizardOptionsLabel->sizePolicy().hasHeightForWidth() ) );
    WStackEPage1Layout->addWidget( pcSampleWizardOptionsLabel );

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

    epage1pcSampleRateText = new QLineEdit( WStackEPage1, "epage1pcSampleRateText" );
    layout1_2_2->addWidget( epage1pcSampleRateText );
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
    pcSampleWizardStack->addWidget( WStackEPage1, 4 );

    WStackEPage2 = new QWidget( pcSampleWizardStack, "WStackEPage2" );
    WStackEPage2Layout = new QVBoxLayout( WStackEPage2, 11, 6, "WStackEPage2Layout"); 

    psSampleFinishLabel_2 = new QLabel( WStackEPage2, "psSampleFinishLabel_2" );
    psSampleFinishLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, psSampleFinishLabel_2->sizePolicy().hasHeightForWidth() ) );
    WStackEPage2Layout->addWidget( psSampleFinishLabel_2 );

    layout7_2_2 = new QHBoxLayout( 0, 0, 6, "layout7_2_2"); 

    epage2BackButton = new QPushButton( WStackEPage2, "epage2BackButton" );
    epage2BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage2BackButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2_2->addWidget( epage2BackButton );

    epage2FinishButton = new QPushButton( WStackEPage2, "epage2FinishButton" );
    epage2FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage2FinishButton->sizePolicy().hasHeightForWidth() ) );
    layout7_2_2->addWidget( epage2FinishButton );
    WStackEPage2Layout->addLayout( layout7_2_2 );
    pcSampleWizardStack->addWidget( WStackEPage2, 5 );
    frame3Layout->addWidget( pcSampleWizardStack );
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
    resize( QSize(631, 508).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( epage0HideWizardCheckBox, SIGNAL( clicked() ), this, SLOT( epage0HideWizardCheckBoxSelected() ) );
    connect( epage0NextButton, SIGNAL( clicked() ), this, SLOT( epage0NextButtonSelected() ) );
    connect( epage0StartButton, SIGNAL( clicked() ), this, SLOT( epage0StartButtonSelected() ) );
    connect( epage1BackButton, SIGNAL( clicked() ), this, SLOT( epage1BackButtonSelected() ) );
    connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( epage1NextButtonSelected() ) );
    connect( epage1ResetButton, SIGNAL( clicked() ), this, SLOT( epage1ResetButtonSelected() ) );
    connect( epage2BackButton, SIGNAL( clicked() ), this, SLOT( epage2BackButtonSelected() ) );
    connect( epage2FinishButton, SIGNAL( clicked() ), this, SLOT( epage2FinishButtonSelected() ) );
    connect( vpage0HideWizardCheckBox, SIGNAL( clicked() ), this, SLOT( vpage0HideWizardCheckBoxSelected() ) );
    connect( vpage0NextButton, SIGNAL( clicked() ), this, SLOT( vpage0NextButtonSelected() ) );
    connect( vpage0StartButton, SIGNAL( clicked() ), this, SLOT( vpage0StartButtonSelected() ) );
    connect( vpage1pcSampleRateText, SIGNAL( returnPressed() ), this, SLOT( vpage1pcSampleRateTextReturnPressed() ) );
    connect( vpage1BackButton, SIGNAL( clicked() ), this, SLOT( vpage1BackButtonSelected() ) );
    connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
    connect( vpage1ResetButton, SIGNAL( clicked() ), this, SLOT( vpage1ResetButtonSelected() ) );
    connect( vpage2BackButton, SIGNAL( clicked() ), this, SLOT( vpage2BackButtonSelected() ) );
    connect( vpage2FinishButton, SIGNAL( clicked() ), this, SLOT( vpage2FinishButtonSelected() ) );
    connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
    connect( epage1pcSampleRateText, SIGNAL( returnPressed() ), this, SLOT( epage1pcSampleRateTextReturnPressed() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
pcsample::~pcsample()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void pcsample::languageChange()
{
    setCaption( tr( "Form1" ) );
    pcSampleWizardTitleLabel->setText( tr( "<h1>pc Sampling Wizard</h1>" ) );
    textEdit1->setText( tr( "The pcsamp experiment estimates the actual CPU time for each source code line, machine code line, and function in your program. The report listing of this experiment shows exclusive PC sampling time. This experiment is a lightweight, high-speed operation that makes use of the operating system.\n"
"\n"
"CPU time is calculated by multiplying the number of times an instruction or function appears in the PC by the interval specified for the experiment (either 1 or 10 milliseconds).\n"
"\n"
"To collect the data, the operating system regularly stops the process, increments a counter corresponding to the current value of the PC, and resumes the process. The default sample interval is 10 millisecond.\n"
"\n"
"PC sampling runs should slow the execution time of the program down no more than 5 percent. The measurements are statistical in nature, meaning they exhibit variance inversely proportional to the running time." ) );
    vpage0HideWizardCheckBox->setText( tr( "Hide pc Sample Wizard next time pc Sample Experiment is selected.\n"
"(Note: You can change this back by going to the pc Sampling local menu.)" ) );
    vpage0StartButton->setText( tr( "Start" ) );
    vpage0NextButton->setText( tr( "Next" ) );
    QToolTip::add( vpage0NextButton, tr( "Advance to the next wizard page." ) );
    pcSampleWizardOptionsLabel_2->setText( tr( "The following options (paramaters) are available to adjust.   These are the options the collector has exported.<br><br>\n"
"The smaller the number used for the sampling rate, the more\n"
"pcSampling detail will be show.   However, the trade off will be slower\n"
"performance and a larger data file.<br><br>\n"
"It may take a little expermenting to find the right setting for your \n"
"particular executable.   We suggest starting with the default setting\n"
"of 10." ) );
    rateHeaderLabel->setText( tr( "You can set the following option(s):" ) );
    rateLabel->setText( tr( "pc Sampling rate:" ) );
    vpage1pcSampleRateText->setText( tr( "10" ) );
    QToolTip::add( vpage1pcSampleRateText, tr( "The rate to sample.   (Default 10 milliseconds.)" ) );
    vpage1BackButton->setText( tr( "Back" ) );
    QToolTip::add( vpage1BackButton, tr( "Takes you back one page." ) );
    vpage1ResetButton->setText( tr( "Reset" ) );
    QToolTip::add( vpage1ResetButton, tr( "Reset the values to the default setings." ) );
    vpage1NextButton->setText( tr( "Next" ) );
    QToolTip::add( vpage1NextButton, tr( "Advance to the next wizard page." ) );
    psSampleFinishLabel->setText( tr( "<p align=\"left\">\n"
"You've selected a pc Sampling experiment for executable \"%s\" to be run on host \"%s\".  Futher you've chosed a sample rate of \"%d\" milliseconds.<br><br>To complete the exeriment setup select the \"Finish\" button.<br><br>Upon selection of the \"Finish\" button an experiment \"Control\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>" ) );
    vpage2BackButton->setText( tr( "Back" ) );
    QToolTip::add( vpage2BackButton, tr( "Takes you back one page." ) );
    vpage2FinishButton->setText( tr( "Finish..." ) );
    QToolTip::add( vpage2FinishButton, tr( "Finishes loading the wizard information and brings up a \"Control\" panel" ) );
    pcSampleWizardTitleLabel_2->setText( tr( "<h1>pc Sampling Wizard</h1>" ) );
    wizardDescriptionLabel->setText( tr( "<p align=\"center\"><p align=\"left\">\n"
"Program counter (pc) sampling reveals the amount of execution time \n"
"spent in various parts of a program. The count includes:  <br>\n"
" * CPU time and memory access time <br>\n"
" * Time spent in user routines<br><br>\n"
"The pc sampling does not count time spent swapping or time spent accessing external resources.</p></p>" ) );
    epage0HideWizardCheckBox->setText( tr( "Hide pc Sample Wizard next time pc Sample Experiment is selected.\n"
"(Note: You can change this back by going to the pc Sampling local menu.)" ) );
    epage0StartButton->setText( tr( "Start" ) );
    epage0NextButton->setText( tr( "Next" ) );
    QToolTip::add( epage0NextButton, tr( "Advance to the next wizard page." ) );
    pcSampleWizardOptionsLabel->setText( tr( "The following options (paramaters) are available to adjust.     <br>These are the options the collector has exported." ) );
    rateHeaderLabel_2->setText( tr( "You can set the following option(s):" ) );
    rateLabel_2->setText( tr( "pc Sampling rate:" ) );
    epage1pcSampleRateText->setText( tr( "10" ) );
    QToolTip::add( epage1pcSampleRateText, tr( "The rate to sample.   (Default 10 milliseconds.)" ) );
    epage1BackButton->setText( tr( "Back" ) );
    QToolTip::add( epage1BackButton, tr( "Takes you back one page." ) );
    epage1ResetButton->setText( tr( "Reset" ) );
    QToolTip::add( epage1ResetButton, tr( "Reset the values to the default setings." ) );
    epage1NextButton->setText( tr( "Next" ) );
    QToolTip::add( epage1NextButton, tr( "Advance to the next wizard page." ) );
    psSampleFinishLabel_2->setText( tr( "<p align=\"left\">\n"
"You've selected a pc Sampling experiment for executable \"%s\" to be run on host \"%s\".  Futher you've chosed a sample rate of \"%d\" milliseconds.<br><br></p>" ) );
    epage2BackButton->setText( tr( "Back" ) );
    QToolTip::add( epage2BackButton, tr( "Takes you back one page." ) );
    epage2FinishButton->setText( tr( "Finish..." ) );
    QToolTip::add( epage2FinishButton, tr( "Finishes loading the wizard information and brings up a \"Control\" panel" ) );
    wizardMode->setText( tr( "Verbose Wizard Mode" ) );
    broughtToYouByLabel->setText( tr( "Brought to you by SGI (SiliconGraphics)" ) );
}

