/****************************************************************************
** Form implementation generated from reading ui file 'introwizardform.ui'
**
** Created: Mon Apr 19 13:12:26 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "introwizardform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a IntroWizardForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
IntroWizardForm::IntroWizardForm( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "IntroWizardForm" );
    IntroWizardFormLayout = new QVBoxLayout( this, 11, 6, "IntroWizardFormLayout"); 

    frame5 = new QFrame( this, "frame5" );
    frame5->setFrameShape( QFrame::StyledPanel );
    frame5->setFrameShadow( QFrame::Raised );
    frame5Layout = new QVBoxLayout( frame5, 11, 6, "frame5Layout"); 

    widgetStack5 = new QWidgetStack( frame5, "widgetStack5" );

    WStackPage = new QWidget( widgetStack5, "WStackPage" );
    WStackPageLayout = new QVBoxLayout( WStackPage, 11, 6, "WStackPageLayout"); 

    epage1Header_2 = new QLabel( WStackPage, "epage1Header_2" );
    epage1Header_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, epage1Header_2->sizePolicy().hasHeightForWidth() ) );
    epage1Header_2->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackPageLayout->addWidget( epage1Header_2 );

    layout21 = new QVBoxLayout( 0, 0, 6, "layout21"); 

    vpage1Label = new QLabel( WStackPage, "vpage1Label" );
    vpage1Label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, vpage1Label->sizePolicy().hasHeightForWidth() ) );
    layout21->addWidget( vpage1Label );

    layout20 = new QHBoxLayout( 0, 0, 6, "layout20"); 
    spacer5_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout20->addItem( spacer5_3 );

    vpage1LoadExperimentCheckBox = new QCheckBox( WStackPage, "vpage1LoadExperimentCheckBox" );
    layout20->addWidget( vpage1LoadExperimentCheckBox );
    layout21->addLayout( layout20 );

    line3 = new QFrame( WStackPage, "line3" );
    line3->setFrameShape( QFrame::HLine );
    line3->setFrameShadow( QFrame::Sunken );
    line3->setFrameShape( QFrame::HLine );
    layout21->addWidget( line3 );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 
    spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout9->addItem( spacer5 );

    vpage1pcSampleRB = new QRadioButton( WStackPage, "vpage1pcSampleRB" );
    vpage1pcSampleRB->setChecked( TRUE );
    layout9->addWidget( vpage1pcSampleRB );
    layout21->addLayout( layout9 );

    layout10 = new QHBoxLayout( 0, 0, 6, "layout10"); 
    spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout10->addItem( spacer6 );

    vpage1UserTimeRB = new QRadioButton( WStackPage, "vpage1UserTimeRB" );
    layout10->addWidget( vpage1UserTimeRB );
    layout21->addLayout( layout10 );

    layout21_2 = new QHBoxLayout( 0, 0, 6, "layout21_2"); 
    spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout21_2->addItem( spacer7 );

    vpage1HardwareCounterRB = new QRadioButton( WStackPage, "vpage1HardwareCounterRB" );
    layout21_2->addWidget( vpage1HardwareCounterRB );
    layout21->addLayout( layout21_2 );

    layout20_2 = new QHBoxLayout( 0, 0, 6, "layout20_2"); 
    spacer7_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout20_2->addItem( spacer7_3 );

    vpage1FloatingPointRB = new QRadioButton( WStackPage, "vpage1FloatingPointRB" );
    layout20_2->addWidget( vpage1FloatingPointRB );
    layout21->addLayout( layout20_2 );

    layout19 = new QHBoxLayout( 0, 0, 6, "layout19"); 
    spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout19->addItem( spacer7_4 );

    vpage1InputOutputRB = new QRadioButton( WStackPage, "vpage1InputOutputRB" );
    layout19->addWidget( vpage1InputOutputRB );
    layout21->addLayout( layout19 );

    layout18 = new QHBoxLayout( 0, 0, 6, "layout18"); 
    spacer7_5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout18->addItem( spacer7_5 );

    vpage1MpiRB = new QRadioButton( WStackPage, "vpage1MpiRB" );
    layout18->addWidget( vpage1MpiRB );
    layout21->addLayout( layout18 );
    WStackPageLayout->addLayout( layout21 );

    layout52 = new QHBoxLayout( 0, 0, 6, "layout52"); 
    spacer44 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout52->addItem( spacer44 );

    vpage1NextButton = new QPushButton( WStackPage, "vpage1NextButton" );
    vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
    layout52->addWidget( vpage1NextButton );
    WStackPageLayout->addLayout( layout52 );
    widgetStack5->addWidget( WStackPage, 0 );

    WStackPage_2 = new QWidget( widgetStack5, "WStackPage_2" );
    WStackPageLayout_2 = new QVBoxLayout( WStackPage_2, 11, 6, "WStackPageLayout_2"); 

    epage1Header = new QLabel( WStackPage_2, "epage1Header" );
    epage1Header->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, epage1Header->sizePolicy().hasHeightForWidth() ) );
    epage1Header->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackPageLayout_2->addWidget( epage1Header );

    layout20_3 = new QVBoxLayout( 0, 0, 6, "layout20_3"); 

    epage1Label = new QLabel( WStackPage_2, "epage1Label" );
    epage1Label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, epage1Label->sizePolicy().hasHeightForWidth() ) );
    layout20_3->addWidget( epage1Label );

    layout19_2 = new QHBoxLayout( 0, 0, 6, "layout19_2"); 
    spacer5_2_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout19_2->addItem( spacer5_2_2 );

    epage1LoadExperimentCheckBox = new QCheckBox( WStackPage_2, "epage1LoadExperimentCheckBox" );
    layout19_2->addWidget( epage1LoadExperimentCheckBox );
    layout20_3->addLayout( layout19_2 );

    line2 = new QFrame( WStackPage_2, "line2" );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );
    layout20_3->addWidget( line2 );

    layout9_2 = new QHBoxLayout( 0, 0, 6, "layout9_2"); 
    spacer5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout9_2->addItem( spacer5_2 );

    epage1pcSampleRB = new QRadioButton( WStackPage_2, "epage1pcSampleRB" );
    epage1pcSampleRB->setChecked( TRUE );
    layout9_2->addWidget( epage1pcSampleRB );
    layout20_3->addLayout( layout9_2 );

    layout10_2 = new QHBoxLayout( 0, 0, 6, "layout10_2"); 
    spacer6_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout10_2->addItem( spacer6_2 );

    epage1UserTimeRB = new QRadioButton( WStackPage_2, "epage1UserTimeRB" );
    layout10_2->addWidget( epage1UserTimeRB );
    layout20_3->addLayout( layout10_2 );

    layout21_2_2 = new QHBoxLayout( 0, 0, 6, "layout21_2_2"); 
    spacer7_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout21_2_2->addItem( spacer7_2 );

    epage1HardwareCounterRB = new QRadioButton( WStackPage_2, "epage1HardwareCounterRB" );
    layout21_2_2->addWidget( epage1HardwareCounterRB );
    layout20_3->addLayout( layout21_2_2 );

    layout20_2_2 = new QHBoxLayout( 0, 0, 6, "layout20_2_2"); 
    spacer7_3_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout20_2_2->addItem( spacer7_3_2 );

    epage1FloatingPointRB = new QRadioButton( WStackPage_2, "epage1FloatingPointRB" );
    layout20_2_2->addWidget( epage1FloatingPointRB );
    layout20_3->addLayout( layout20_2_2 );

    layout19_2_2 = new QHBoxLayout( 0, 0, 6, "layout19_2_2"); 
    spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout19_2_2->addItem( spacer7_4_2 );

    epage1InputOutputRB = new QRadioButton( WStackPage_2, "epage1InputOutputRB" );
    layout19_2_2->addWidget( epage1InputOutputRB );
    layout20_3->addLayout( layout19_2_2 );

    layout18_2 = new QHBoxLayout( 0, 0, 6, "layout18_2"); 
    spacer7_5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout18_2->addItem( spacer7_5_2 );

    epage1MpiRB = new QRadioButton( WStackPage_2, "epage1MpiRB" );
    layout18_2->addWidget( epage1MpiRB );
    layout20_3->addLayout( layout18_2 );
    WStackPageLayout_2->addLayout( layout20_3 );

    layout44 = new QHBoxLayout( 0, 0, 6, "layout44"); 
    spacer37 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout44->addItem( spacer37 );

    epage1NextButton = new QPushButton( WStackPage_2, "epage1NextButton" );
    epage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1NextButton->sizePolicy().hasHeightForWidth() ) );
    layout44->addWidget( epage1NextButton );
    WStackPageLayout_2->addLayout( layout44 );
    widgetStack5->addWidget( WStackPage_2, 1 );
    frame5Layout->addWidget( widgetStack5 );
    IntroWizardFormLayout->addWidget( frame5 );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    wizardMode = new QCheckBox( this, "wizardMode" );
    wizardMode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, wizardMode->sizePolicy().hasHeightForWidth() ) );
    wizardMode->setChecked( TRUE );
    layout5->addWidget( wizardMode );
    spacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer1 );

    broughtToYouByLabel = new QLabel( this, "broughtToYouByLabel" );
    broughtToYouByLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, broughtToYouByLabel->sizePolicy().hasHeightForWidth() ) );
    layout5->addWidget( broughtToYouByLabel );
    IntroWizardFormLayout->addLayout( layout5 );
    languageChange();
    resize( QSize(610, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
    connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
    connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( epage1NextButtonSelected() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
IntroWizardForm::~IntroWizardForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void IntroWizardForm::languageChange()
{
    setCaption( tr( "IntroWizardForm" ) );
    epage1Header_2->setText( tr( "<h2>Welcome to OpenSpeedShop(tm)</h2>" ) );
    vpage1Label->setText( tr( "Please select which of the following are true for your application:" ) );
    vpage1LoadExperimentCheckBox->setText( tr( "I already have experiment data and would like to analyze it." ) );
    vpage1pcSampleRB->setText( tr( "I'm simply trying to find where my program is spending most of it's time." ) );
    vpage1UserTimeRB->setText( tr( "I'd like to find out how much time is system time vs. my program's time." ) );
    vpage1HardwareCounterRB->setText( tr( "I'd like to see what the internal Hardware Counters can show me.\n"
"(Hardware Counters are an advanced feature that utilizes machine \n"
"hardware to measure certain attributes of program execution.)" ) );
    vpage1FloatingPointRB->setText( tr( "I need to measure how many times I am causing Floating Point Exceptions." ) );
    vpage1InputOutputRB->setText( tr( "My program does a lot of Input and Output and I'd like to trace that work." ) );
    vpage1MpiRB->setText( tr( "I have an MPI job that I need to measure mpi characteristics." ) );
    vpage1NextButton->setText( tr( "Next" ) );
    epage1Header->setText( tr( "<h2>Welcome to OpenSpeedShop(tm)</h2>" ) );
    epage1Label->setText( tr( "Please select which of the following are true for your application:" ) );
    epage1LoadExperimentCheckBox->setText( tr( "Load experiment data" ) );
    epage1pcSampleRB->setText( tr( "pcSampling (profiling)" ) );
    epage1UserTimeRB->setText( tr( "User Time Experiment." ) );
    epage1HardwareCounterRB->setText( tr( "Hardware Counter Tracing." ) );
    epage1FloatingPointRB->setText( tr( "Floating Point Exceptions Tracing" ) );
    epage1InputOutputRB->setText( tr( "Input/Output Tracing" ) );
    epage1MpiRB->setText( tr( "MPI Tracing" ) );
    epage1NextButton->setText( tr( "Next" ) );
    wizardMode->setText( tr( "Verbose Wizard Mode" ) );
    broughtToYouByLabel->setText( tr( "Brought to you by SGI (SiliconGraphics)" ) );
}

void IntroWizardForm::wizardModeSelected()
{
    qWarning( "IntroWizardForm::wizardModeSelected(): Not implemented yet" );
}

void IntroWizardForm::epage1NextButtonSelected()
{
    qWarning( "IntroWizardForm::epage1NextButtonSelected(): Not implemented yet" );
}

void IntroWizardForm::vpage1NextButtonSelected()
{
    qWarning( "IntroWizardForm::vpage1NextButtonSelected(): Not implemented yet" );
}

