#include "pcSampleWizardPanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qvariant.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>   // For the file dialog box.
#include <qfiledialog.h>  // For the file dialog box.
#include <qmessagebox.h>

#include <qbitmap.h>
#include "rightarrow.xpm"
#include "leftarrow.xpm"


/*!  pcSampleWizardPanel Class
     This class is used by the script mknewpanel to create a new work area
     for the panel creator to design a new panel.


     Autor: Al Stipek (stipek@sgi.com)
 */


/*! The default constructor.   Unused. */
pcSampleWizardPanel::pcSampleWizardPanel()
{ // Unused... Here for completeness...
}


/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
pcSampleWizardPanel::pcSampleWizardPanel(PanelContainer *pc, const char *n) : Panel(pc, n)
{
  printf("pcSampleWizardPanel::pcSampleWizardPanel() constructor called\n");
  QHBoxLayout * frameLayout = new QHBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

    if ( !name )
	setName( "pcsample" );

    QWidget* privateLayoutWidget = new QWidget( getBaseWidgetFrame(), "layout16" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 604, 482 ) );
    layout16 = new QVBoxLayout( privateLayoutWidget, 11, 6, "layout16"); 

    frame3 = new QFrame( privateLayoutWidget, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QVBoxLayout( frame3, 11, 6, "frame3Layout"); 

    pcSampleWizardPanelStack = new QWidgetStack( frame3, "pcSampleWizardPanelStack" );

    WStackVPage0 = new QWidget( pcSampleWizardPanelStack, "WStackVPage0" );
    WStackVPage0Layout = new QVBoxLayout( WStackVPage0, 11, 6, "WStackVPage0Layout"); 

    pcSampleWizardPanelTitleLabel = new QLabel( WStackVPage0, "pcSampleWizardPanelTitleLabel" );
    pcSampleWizardPanelTitleLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, pcSampleWizardPanelTitleLabel->sizePolicy().hasHeightForWidth() ) );
    pcSampleWizardPanelTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackVPage0Layout->addWidget( pcSampleWizardPanelTitleLabel );

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
    pcSampleWizardPanelStack->addWidget( WStackVPage0, 0 );

    WStackVPage1 = new QWidget( pcSampleWizardPanelStack, "WStackVPage1" );
    WStackVPage1Layout = new QVBoxLayout( WStackVPage1, 11, 6, "WStackVPage1Layout"); 

    pcSampleWizardPanelOptionsLabel_2 = new QLabel( WStackVPage1, "pcSampleWizardPanelOptionsLabel_2" );
    pcSampleWizardPanelOptionsLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pcSampleWizardPanelOptionsLabel_2->sizePolicy().hasHeightForWidth() ) );
    WStackVPage1Layout->addWidget( pcSampleWizardPanelOptionsLabel_2 );

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
    pcSampleWizardPanelStack->addWidget( WStackVPage1, 1 );

    WStackVPage2 = new QWidget( pcSampleWizardPanelStack, "WStackVPage2" );
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
    pcSampleWizardPanelStack->addWidget( WStackVPage2, 2 );

    WStackEPage0 = new QWidget( pcSampleWizardPanelStack, "WStackEPage0" );
    WStackEPage0Layout = new QVBoxLayout( WStackEPage0, 11, 6, "WStackEPage0Layout"); 

    pcSampleWizardPanelTitleLabel_2 = new QLabel( WStackEPage0, "pcSampleWizardPanelTitleLabel_2" );
    pcSampleWizardPanelTitleLabel_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, pcSampleWizardPanelTitleLabel_2->sizePolicy().hasHeightForWidth() ) );
    pcSampleWizardPanelTitleLabel_2->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    WStackEPage0Layout->addWidget( pcSampleWizardPanelTitleLabel_2 );

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
    pcSampleWizardPanelStack->addWidget( WStackEPage0, 3 );

    WStackEPage1 = new QWidget( pcSampleWizardPanelStack, "WStackEPage1" );
    WStackEPage1Layout = new QVBoxLayout( WStackEPage1, 11, 6, "WStackEPage1Layout"); 

    pcSampleWizardPanelOptionsLabel = new QLabel( WStackEPage1, "pcSampleWizardPanelOptionsLabel" );
    pcSampleWizardPanelOptionsLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pcSampleWizardPanelOptionsLabel->sizePolicy().hasHeightForWidth() ) );
    WStackEPage1Layout->addWidget( pcSampleWizardPanelOptionsLabel );

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
    pcSampleWizardPanelStack->addWidget( WStackEPage1, 4 );

    WStackEPage2 = new QWidget( pcSampleWizardPanelStack, "WStackEPage2" );
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
    pcSampleWizardPanelStack->addWidget( WStackEPage2, 5 );
    frame3Layout->addWidget( pcSampleWizardPanelStack );
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

  // This next line makes it all magically appear and resize correctly.
  frameLayout->addWidget(privateLayoutWidget);
}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
pcSampleWizardPanel::~pcSampleWizardPanel()
{
  printf("  pcSampleWizardPanel::~pcSampleWizardPanel() destructor called\n");

  delete baseWidgetFrame;
}

//! Add user panel specific menu items if they have any.
bool
pcSampleWizardPanel::menu(QPopupMenu* contextMenu)
{
  dprintf("pcSampleWizardPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
pcSampleWizardPanel::save()
{
  dprintf("pcSampleWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
pcSampleWizardPanel::saveAs()
{
  dprintf("pcSampleWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
pcSampleWizardPanel::listener(char *msg)
{
  dprintf("pcSampleWizardPanel::listener() requested.\n");
  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
pcSampleWizardPanel::broadcast(char *msg)
{
  dprintf("pcSampleWizardPanel::broadcast() requested.\n");
  return 0;
}

#ifdef PULL
void
pcSampleWizardPanel::page1NextSelected()
{
  QString fn = QString::null;

  if( loadExecutableRB->isChecked() || attachProcessRB->isChecked() )
  {
printf("GUI ONLY?: get a list of executables to attach to.\n");
    pcSampleWizardPanelStack->raiseWidget(1);
  } else if( viewDataRB->isChecked() )
  {
printf("GUI ONLY?: get a list of experiment files to load.\n");
    char *cwd = get_current_dir_name();
    fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.exp)", this, "open experiment dialog", "Choose an experiment file to open");
    free(cwd);
    if( !fn.isEmpty() )
    {
      printf("fn = %s\n", fn.ascii() );
      page3FinishSelected();
    } else
    {
      fprintf(stderr, "No experiment file name given.\n");
      return;
    }
  } else
  {
    fprintf(stderr, "Confusion on page1 \"Next\" button.\n");
    return;
  }
}

void
pcSampleWizardPanel::page2BackSelected()
{
  pcSampleWizardPanelStack->raiseWidget(0);
}

void
pcSampleWizardPanel::page2ResetSelected()
{
  printf("Reset selected.\n");
}

void
pcSampleWizardPanel::page2NextSelected()
{
QString fn = QString::null;
  if( loadExecutableRB->isChecked() )
  {
printf("GUI ONLY?: get an executable to open.\n");
    char *cwd = get_current_dir_name();
    fn = QFileDialog::getOpenFileName( cwd, "All Files (*)", this, "open executable dialog", "Choose an executable to open");
    free(cwd);
    if( !fn.isEmpty() )
    {
      printf("fn = %s\n", fn.ascii() );
    } else
    {
      fprintf(stderr, "No executable name given.\n");
      return;
    }
  } else if( attachProcessRB->isChecked() )
  {
    QString msg = QString("No PID specified.  Please select one.\n");
    QMessageBox::information( (QWidget *)this, "PID Needed...",
                               msg, QMessageBox::Ok );
fn = QString("12345");
  } else
  {
    // error...
    return;
  }

char buffer[2048];
sprintf(buffer, "<p align=\"left\">You've selected a pc Sampling experiment for executable \"%s\" to be run on host \"%s\".  Futher you've chosed a sample rate of \"%s\" milliseconds.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>Upon selection of the \"Finish\" button an experiment \"Control\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", fn.ascii(), "localhost", pcSampleRateText->text().ascii() );
  psSampleFinishLabel->setText( tr( buffer ) );
  pcSampleWizardPanelStack->raiseWidget(2);
}

void
pcSampleWizardPanel::page3BackSelected()
{
  pcSampleWizardPanelStack->raiseWidget(1);
}

void
pcSampleWizardPanel::setLoadExecutableRB()
{
//  printf("setLoadExecutableRB() selected.\n");

printf("CLI: set the executable\n");
#ifdef LATER
  // Make sure this one is set on...
  loadExecutableRB->setChecked(TRUE);
  // Set the other 2 to off....
  attachProcessRB->setChecked(FALSE);
  viewDataRB->setChecked(FALSE);
#endif // LATER
}

void
pcSampleWizardPanel::setAttachProcessRB()
{
//  printf("setAttachProcessRB() selected.\n");
printf("CLI: attach to the process\n");
#ifdef LATER
  // Make sure this one is set on...
  attachProcessRB->setChecked(TRUE);
  // Set the other 2 to off....
  loadExecutableRB->setChecked(FALSE);
  viewDataRB->setChecked(FALSE);
#endif // LATER
}

void
pcSampleWizardPanel::setViewDataRB()
{
//  printf("setViewDataRB() selected.\n");
printf("CLI: load the experiment file.\n");
#ifdef LATER
  // Make sure this one is set on...
  viewDataRB->setChecked(TRUE);
  // Set the other 2 to off....
  loadExecutableRB->setChecked(FALSE);
  attachProcessRB->setChecked(FALSE);
#endif // LATER
}


void
pcSampleWizardPanel::hideWizardCheck()
{
  printf("hideWizardCheck selected.\n");
  printf("turn on/off this wizard next time in...\n");
}

void
pcSampleWizardPanel::page3FinishSelected()
{
  printf("Finish selected.\n");

  _masterPC->dl_create_and_add_panel("pc Sample Panel Control", _masterPC);
//  Panel *p = panelContainer->raiseNamedPanel("pc Sample Panel Control");
}
#else // PULL

void pcSampleWizardPanel::wizardModeSelected()
{
  QWidget *raisedWidget = pcSampleWizardPanelStack->visibleWidget();
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
        pcSampleWizardPanelStack->raiseWidget(WStackVPage0);
    } else if( raisedWidget == WStackEPage1 )
    {
        printf("WStackEPage1\n");
        pcSampleWizardPanelStack->raiseWidget(WStackVPage1);
    } else if( raisedWidget == WStackEPage2 )
    {
        printf("WStackEPage2\n");
        pcSampleWizardPanelStack->raiseWidget(WStackVPage2);
    } else
    {
        printf("Verbose to Expert: unknown WStackPage\n");
    }
  } else
  {
    if( raisedWidget == WStackVPage0 )
    {
        printf("WStackVPage0\n");
        pcSampleWizardPanelStack->raiseWidget(WStackEPage0);
    } else if( raisedWidget ==  WStackVPage1 )
    {
        printf("WStackVPage1\n");
        pcSampleWizardPanelStack->raiseWidget(WStackEPage1);
    } else if( raisedWidget == WStackVPage2 )
    {
        printf("WStackVPage2\n");
        pcSampleWizardPanelStack->raiseWidget(WStackEPage2);
    } else
    {
        printf("Expert to Verbose: unknown WStackPage\n");
    }
  }
}


void pcSampleWizardPanel::epage0HideWizardCheckBoxSelected()
{
}

#include "ladDialog.hxx"
void pcSampleWizardPanel::epage0NextButtonSelected()
{
printf("epage0NextButtonSelected() \n");
    pcSampleWizardPanelStack->raiseWidget(WStackEPage1);
}

void pcSampleWizardPanel::epage0StartButtonSelected()
{
printf("epage0StartButtonSelected() \n");
    Panel *p = panelContainer->raiseNamedPanel("Intro Wizard");
    if( !p )
    {
      panelContainer->_masterPC->dl_create_and_add_panel("Intro Wizard", panelContainer);
    }
}

void pcSampleWizardPanel::epage1BackButtonSelected()
{
printf("epage1BackButtonSelected() \n");
    pcSampleWizardPanelStack->raiseWidget(WStackEPage0);
}

void pcSampleWizardPanel::epage1NextButtonSelected()
{
printf("epage1NextButtonSelected() \n");

 MyDialog1 *w = new MyDialog1(this, "ladDialog", TRUE);
 w->updateAttachableProcessList();
 w->show();


    pcSampleWizardPanelStack->raiseWidget(WStackEPage2);
}

void pcSampleWizardPanel::epage1ResetButtonSelected()
{
printf("epage1ResetButtonSelected() \n");
}

void pcSampleWizardPanel::epage2BackButtonSelected()
{
printf("epage2BackButtonSelected() \n");
    pcSampleWizardPanelStack->raiseWidget(WStackEPage1);
}

void pcSampleWizardPanel::epage2FinishButtonSelected()
{
printf("epage2FinishButtonSelected() \n");

//  Panel *p = panelContainer->raiseNamedPanel("pc Sample Panel Control");
//  if( !p )
  {
    panelContainer->_masterPC->dl_create_and_add_panel("pc Sample Panel Control", panelContainer);
  }
}

void pcSampleWizardPanel::vpage0HideWizardCheckBoxSelected()
{
printf("vpage1HideWizardCheckBoxSelected() \n");
}

void pcSampleWizardPanel::vpage0NextButtonSelected()
{
printf("vpage0NextButtonSelected() \n");

    pcSampleWizardPanelStack->raiseWidget(WStackVPage1);
}

void pcSampleWizardPanel::vpage0StartButtonSelected()
{
printf("vpage0StartButtonSelected() \n");
    Panel *p = panelContainer->raiseNamedPanel("Intro Wizard");
    if( !p )
    {
      panelContainer->_masterPC->dl_create_and_add_panel("Intro Wizard", panelContainer);
    }
}

void pcSampleWizardPanel::vpage1pcSampleRateTextReturnPressed()
{
printf("vpage1pcSampleRateTextReturnPressed() \n");
}

void pcSampleWizardPanel::epage1pcSampleRateTextReturnPressed()
{
printf("epage1pcSampleRateTextReturnPressed() \n");
}

void pcSampleWizardPanel::vpage1BackButtonSelected()
{
printf("vpage1BackButtonSelected() \n");
    pcSampleWizardPanelStack->raiseWidget(WStackVPage0);
}

void pcSampleWizardPanel::vpage1NextButtonSelected()
{
printf("vpage1NextButtonSelected() \n");

 MyDialog1 *w = new MyDialog1(this, "ladDialog", TRUE);
 w->updateAttachableProcessList();
 w->show();

    pcSampleWizardPanelStack->raiseWidget(WStackVPage2);
}

void pcSampleWizardPanel::vpage1ResetButtonSelected()
{
printf("vpage1ResetButtonSelected() \n");
}

void pcSampleWizardPanel::vpage2BackButtonSelected()
{
printf("vpage2BackButtonSelected() \n");
    pcSampleWizardPanelStack->raiseWidget(WStackVPage1);
}

void pcSampleWizardPanel::vpage2FinishButtonSelected()
{
printf("vpage2FinishButtonSelected() \n");

//  Panel *p = panelContainer->raiseNamedPanel("pc Sample Panel Control");
//  if( !p )
  {
    panelContainer->_masterPC->dl_create_and_add_panel("pc Sample Panel Control", panelContainer);
  }
}

#endif // PULL

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
pcSampleWizardPanel::languageChange()
{
    setCaption( tr( "Form1" ) );
    pcSampleWizardPanelTitleLabel->setText( tr( "<h1>pc Sampling Wizard</h1>" ) );
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
    pcSampleWizardPanelOptionsLabel_2->setText( tr( "The following options (paramaters) are available to adjust.   These are the options the collector has exported.<br><br>\n"
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
    pcSampleWizardPanelTitleLabel_2->setText( tr( "<h1>pc Sampling Wizard</h1>" ) );
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
    pcSampleWizardPanelOptionsLabel->setText( tr( "The following options (paramaters) are available to adjust.     <br>These are the options the collector has exported." ) );
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
