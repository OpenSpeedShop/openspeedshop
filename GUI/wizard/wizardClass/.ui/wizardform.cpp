/****************************************************************************
** Form implementation generated from reading ui file 'wizardform.ui'
**
** Created: Mon Feb 23 07:00:46 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "wizardform.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/* 
 *  Constructs a WizardForm as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The wizard will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal wizard.
 */
WizardForm::WizardForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QWizard( parent, name, modal, fl )

{
    if ( !name )
	setName( "WizardForm" );

    page1 = new QWidget( this, "page1" );
    page1Layout = new QVBoxLayout( page1, 11, 6, "page1Layout"); 

    frame3 = new QFrame( page1, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QVBoxLayout( frame3, 11, 6, "frame3Layout"); 

    textLabel4 = new QLabel( frame3, "textLabel4" );
    textLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel4->sizePolicy().hasHeightForWidth() ) );
    textLabel4->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    frame3Layout->addWidget( textLabel4 );

    textLabel5 = new QLabel( frame3, "textLabel5" );
    textLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel5->sizePolicy().hasHeightForWidth() ) );
    frame3Layout->addWidget( textLabel5 );

    buttonGroup2 = new QButtonGroup( frame3, "buttonGroup2" );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QVBoxLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    instrumentCodeRadioButton = new QRadioButton( buttonGroup2, "instrumentCodeRadioButton" );
    instrumentCodeRadioButton->setChecked( TRUE );
    buttonGroup2Layout->addWidget( instrumentCodeRadioButton );

    radioButton2 = new QRadioButton( buttonGroup2, "radioButton2" );
    buttonGroup2Layout->addWidget( radioButton2 );

    radioButton3 = new QRadioButton( buttonGroup2, "radioButton3" );
    buttonGroup2Layout->addWidget( radioButton3 );

    radioButton4 = new QRadioButton( buttonGroup2, "radioButton4" );
    buttonGroup2Layout->addWidget( radioButton4 );
    frame3Layout->addWidget( buttonGroup2 );
    page1Layout->addWidget( frame3 );
    addPage( page1, "" );

    page2 = new QWidget( this, "page2" );
    page2Layout = new QVBoxLayout( page2, 11, 6, "page2Layout"); 

    frame4 = new QFrame( page2, "frame4" );
    frame4->setFrameShape( QFrame::StyledPanel );
    frame4->setFrameShadow( QFrame::Raised );
    frame4Layout = new QVBoxLayout( frame4, 11, 6, "frame4Layout"); 

    textLabel6 = new QLabel( frame4, "textLabel6" );
    textLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel6->sizePolicy().hasHeightForWidth() ) );
    frame4Layout->addWidget( textLabel6 );

    buttonGroup3 = new QButtonGroup( frame4, "buttonGroup3" );
    buttonGroup3->setColumnLayout(0, Qt::Vertical );
    buttonGroup3->layout()->setSpacing( 6 );
    buttonGroup3->layout()->setMargin( 11 );
    buttonGroup3Layout = new QVBoxLayout( buttonGroup3->layout() );
    buttonGroup3Layout->setAlignment( Qt::AlignTop );

    checkBox1 = new QCheckBox( buttonGroup3, "checkBox1" );
    checkBox1->setChecked( TRUE );
    buttonGroup3Layout->addWidget( checkBox1 );

    checkBox2 = new QCheckBox( buttonGroup3, "checkBox2" );
    buttonGroup3Layout->addWidget( checkBox2 );

    checkBox3 = new QCheckBox( buttonGroup3, "checkBox3" );
    buttonGroup3Layout->addWidget( checkBox3 );

    checkBox5 = new QCheckBox( buttonGroup3, "checkBox5" );
    buttonGroup3Layout->addWidget( checkBox5 );

    checkBox6 = new QCheckBox( buttonGroup3, "checkBox6" );
    buttonGroup3Layout->addWidget( checkBox6 );

    checkBox7 = new QCheckBox( buttonGroup3, "checkBox7" );
    buttonGroup3Layout->addWidget( checkBox7 );
    frame4Layout->addWidget( buttonGroup3 );
    page2Layout->addWidget( frame4 );
    addPage( page2, "" );

    page3 = new QWidget( this, "page3" );
    page3Layout = new QVBoxLayout( page3, 11, 6, "page3Layout"); 

    textLabel7 = new QLabel( page3, "textLabel7" );
    textLabel7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel7->sizePolicy().hasHeightForWidth() ) );
    page3Layout->addWidget( textLabel7 );

    buttonGroup4 = new QButtonGroup( page3, "buttonGroup4" );
    buttonGroup4->setColumnLayout(0, Qt::Vertical );
    buttonGroup4->layout()->setSpacing( 6 );
    buttonGroup4->layout()->setMargin( 11 );
    buttonGroup4Layout = new QVBoxLayout( buttonGroup4->layout() );
    buttonGroup4Layout->setAlignment( Qt::AlignTop );

    radioButton6 = new QRadioButton( buttonGroup4, "radioButton6" );
    radioButton6->setChecked( TRUE );
    buttonGroup4Layout->addWidget( radioButton6 );

    radioButton7 = new QRadioButton( buttonGroup4, "radioButton7" );
    buttonGroup4Layout->addWidget( radioButton7 );

    radioButton8 = new QRadioButton( buttonGroup4, "radioButton8" );
    buttonGroup4Layout->addWidget( radioButton8 );

    radioButton9_2 = new QRadioButton( buttonGroup4, "radioButton9_2" );
    buttonGroup4Layout->addWidget( radioButton9_2 );
    page3Layout->addWidget( buttonGroup4 );
    addPage( page3, "" );

    page4 = new QWidget( this, "page4" );
    page4Layout = new QVBoxLayout( page4, 11, 6, "page4Layout"); 

    textLabel8 = new QLabel( page4, "textLabel8" );
    textLabel8->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel8->sizePolicy().hasHeightForWidth() ) );
    page4Layout->addWidget( textLabel8 );

    buttonGroup6 = new QButtonGroup( page4, "buttonGroup6" );
    buttonGroup6->setColumnLayout(0, Qt::Vertical );
    buttonGroup6->layout()->setSpacing( 6 );
    buttonGroup6->layout()->setMargin( 11 );
    buttonGroup6Layout = new QVBoxLayout( buttonGroup6->layout() );
    buttonGroup6Layout->setAlignment( Qt::AlignTop );

    checkBox8 = new QCheckBox( buttonGroup6, "checkBox8" );
    buttonGroup6Layout->addWidget( checkBox8 );

    checkBox9 = new QCheckBox( buttonGroup6, "checkBox9" );
    buttonGroup6Layout->addWidget( checkBox9 );

    radioButton9 = new QRadioButton( buttonGroup6, "radioButton9" );
    radioButton9->setChecked( TRUE );
    buttonGroup6Layout->addWidget( radioButton9 );

    textLabel9 = new QLabel( buttonGroup6, "textLabel9" );
    buttonGroup6Layout->addWidget( textLabel9 );
    page4Layout->addWidget( buttonGroup6 );
    addPage( page4, "" );
    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( this, SIGNAL( selected(const QString&) ), this, SLOT( wizardPageSlot() ) );
    connect( this, SIGNAL( selected(const QString&) ), this, SLOT( wizardPageSlot() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
WizardForm::~WizardForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WizardForm::languageChange()
{
    setCaption( tr( "WizardForm" ) );
    QToolTip::add( this, tr( "Measuring can be expensive.   The more you gather, the more experiment overhead." ) );
    QWhatsThis::add( this, tr( "Give some information about the Wizard here..." ) );
    textLabel4->setText( tr( "<font size=\"+2\"><b>Welcome to \"OpenSpeedShop\"<p align=\"center\"><h1></h1></p></b></font>" ) );
    textLabel5->setText( tr( "Please select the item which most closely describes what you want to do:" ) );
    buttonGroup2->setTitle( QString::null );
    instrumentCodeRadioButton->setText( tr( "I have an executable and I want to determine it's performance." ) );
    QToolTip::add( instrumentCodeRadioButton, tr( "Select this option if you want to instrument a particular application." ) );
    QWhatsThis::add( instrumentCodeRadioButton, tr( "Give some detailed information about intrumenting a program." ) );
    radioButton2->setText( tr( "I want to attach to an already running program." ) );
    QToolTip::add( radioButton2, tr( "Select this option to attach to a local or remote process." ) );
    QWhatsThis::add( radioButton2, tr( "Give some information about attaching to running applications." ) );
    radioButton3->setText( tr( "I already have performance data gathered and would like to examine it." ) );
    QToolTip::add( radioButton3, tr( "Select this option aid in loading an existing performance file." ) );
    QWhatsThis::add( radioButton3, tr( "Give information about analyzing collected performance data." ) );
    radioButton4->setText( tr( "more options..." ) );
    QToolTip::add( radioButton4, tr( "This option presents other options." ) );
    setTitle( page1, tr( "Wizard" ) );
    textLabel6->setText( tr( "What type of performance information are you looking for?" ) );
    QWhatsThis::add( textLabel6, QString::null );
    buttonGroup3->setTitle( QString::null );
    checkBox1->setText( tr( "I want to see who's taking the most time." ) );
    QToolTip::add( checkBox1, tr( "Selecting this will narrow down where the application is spending most of it's time.  (This is the suggested starting point.)" ) );
    QWhatsThis::add( checkBox1, tr( "Give information about experiments that see who's taking the most time." ) );
    checkBox2->setText( tr( "I want to trace my I/O activity." ) );
    QToolTip::add( checkBox2, tr( "If you want to measure you're Input Output activity, select this item." ) );
    QWhatsThis::add( checkBox2, tr( "Give information about experiments that give information about I/O" ) );
    checkBox3->setText( tr( "I want to trace my MPI application." ) );
    QToolTip::add( checkBox3, tr( "This will trace your MPI routines." ) );
    QWhatsThis::add( checkBox3, tr( "Give information about MPI experiments" ) );
    checkBox5->setText( tr( "I want to find my floating point exceptions." ) );
    QToolTip::add( checkBox5, tr( "This option will report floating point exceptions." ) );
    QWhatsThis::add( checkBox5, tr( "Give information about floating point experiments" ) );
    checkBox6->setText( tr( "I want to leverage the H/W counters available." ) );
    QToolTip::add( checkBox6, tr( "This is and advanded option that will allow you to utilize the Hardware Counters available on this machine." ) );
    QWhatsThis::add( checkBox6, tr( "Give information about Hardware Counter experiments." ) );
    checkBox7->setText( tr( "more options..." ) );
    QToolTip::add( checkBox7, tr( "Select this to see more options." ) );
    setTitle( page2, tr( "What sort of data should we collect?" ) );
    textLabel7->setText( tr( "We can gather different types of information:" ) );
    buttonGroup4->setTitle( QString::null );
    radioButton6->setText( tr( "I'm just trying to find out which routines are taking the most time." ) );
    QToolTip::add( radioButton6, tr( "Select this setting if you are just starting.   (This is the suggested default.)" ) );
    QWhatsThis::add( radioButton6, tr( "Giive information about profiling experiment" ) );
    radioButton7->setText( tr( "I want to see how much time my application is spending in system time." ) );
    QToolTip::add( radioButton7, tr( "This will measure all time associated with your run." ) );
    QWhatsThis::add( radioButton7, tr( "Give information about user time experments" ) );
    radioButton8->setText( tr( "I want to ignore system time and only measure my application's time." ) );
    QToolTip::add( radioButton8, tr( "This will only measure time spent in your application." ) );
    QWhatsThis::add( radioButton8, tr( "Give more information about performance experiments that ignore system time." ) );
    radioButton9_2->setText( tr( "I want to know more about one particular function or code block." ) );
    QToolTip::add( radioButton9_2, tr( "Select if you want to watch one more more specific parts of you application." ) );
    QWhatsThis::add( radioButton9_2, tr( "Give more information about narrowing down experiments." ) );
    setTitle( page3, tr( "How do you want your data organized?" ) );
    textLabel8->setText( tr( "Specify how often you want your applicatioin sampled:" ) );
    QToolTip::add( textLabel8, tr( "This selection can fine-tune where you are spending your time." ) );
    QWhatsThis::add( textLabel8, tr( "Give more information about a profile experiment" ) );
    buttonGroup6->setTitle( QString::null );
    checkBox8->setText( tr( "Sample every 1 millisecond (rather than the defualt 10 milliseconds)" ) );
    QToolTip::add( checkBox8, tr( "Selecting this box will change the default sampling rate." ) );
    QWhatsThis::add( checkBox8, tr( "Give more detailed information about sample rates." ) );
    checkBox9->setText( tr( "Use 32-bit bins to allow billion counts" ) );
    QToolTip::add( checkBox9, tr( "32-bit counts are more accurate, but more expensive." ) );
    QWhatsThis::add( checkBox9, tr( "Give more information about granularity" ) );
    radioButton9->setText( tr( "Run after instrumentation." ) );
    QToolTip::add( radioButton9, tr( "Selecting this will run the experiment after instrumentation." ) );
    QWhatsThis::add( radioButton9, tr( "Give more information, explaining the steps about to be taken." ) );
    textLabel9->setText( tr( "Selecting Finish will instrument the executable." ) );
    setTitle( page4, tr( "How much information do you want?" ) );
}

void WizardForm::wizardPageSlot()
{
    qWarning( "WizardForm::wizardPageSlot(): Not implemented yet" );
}

void WizardForm::selected(const QString&)
{
    qWarning( "WizardForm::selected(const QString&): Not implemented yet" );
}

