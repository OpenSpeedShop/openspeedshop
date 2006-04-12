////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/*! \class IntroWizardPanel
     This defines the starting point for entry level users.
     asks the highest level questions attempting to take 
     the user to a more specific wizard panel (i.e. pc sampline
     wizrard)
 */

#include "IntroWizardPanel.hxx"
#include "PanelContainer.hxx"
#include "plugin_entry_point.hxx"

#include "MessageObject.hxx"

#include "ArgumentObject.hxx"


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

#include <qscrollview.h>
#include <qvbox.h>



/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc         The panel container the panel will initially be attached.
    \param n          The initial name of the panel container
    \param argument   Unused
 */
IntroWizardPanel::IntroWizardPanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  nprintf( DEBUG_CONST_DESTRUCT) ( "IntroWizardPanel::IntroWizardPanel() constructor called.\n");

  if ( !getName() )
  {
	setName( "IntroWizardForm" );
  }


//  IntroWizardFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 11, 6, "IntroWizardFormLayout"); 
  IntroWizardFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 0, 0, "IntroWizardFormLayout"); 


  vWelcomeHeader = new QLabel( getBaseWidgetFrame(), "vWelcomeHeader" );
  vWelcomeHeader->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vWelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  vWelcomeHeader->setMinimumSize( QSize(10,10) );

  IntroWizardFormLayout->addWidget( vWelcomeHeader);

  sv = new QScrollView( getBaseWidgetFrame(), "scrollView" );

  big_box = new QVBox(sv->viewport() );
  const QColor color = vWelcomeHeader->paletteBackgroundColor();
  sv->viewport()->setBackgroundColor(color);
// sv->viewport()->setPaletteBackgroundColor(color);
  sv->addChild(big_box);
  IntroWizardFormLayout->addWidget( sv );

  mainWidgetStack = new QWidgetStack( big_box, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );
  mainWidgetStack->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vWStackPage = new QWidget( mainWidgetStack, "vWStackPage" );
  vWStackPage->setMinimumSize( QSize(10,10) );
  vWStackPageLayout = new QVBoxLayout( vWStackPage, 11, 6, "vWStackPageLayout"); 
  vRBLayout = new QVBoxLayout( 0, 0, 6, "vRBLayout"); 


  vHelpfulLabel = new QLabel( vWStackPage, "vHelpfulLabel" );
  vRBLayout->addWidget( vHelpfulLabel );

  vLoadExperimentLayout = new QVBoxLayout( 0, 0, 6, "vLoadExperimentLayout"); 
//  spacer5_3 = new QSpacerItem( getBaseWidgetFrame()->width(), 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
  spacer5_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
  vLoadExperimentLayout->addItem( spacer5_3 );

  vpage1LoadExperimentCheckBox = new QCheckBox( vWStackPage, "vpage1LoadExperimentCheckBox" );
  vLoadExperimentLayout->addWidget( vpage1LoadExperimentCheckBox );

  vpage1CompareExperimentsCheckBox = new QCheckBox( vWStackPage, "vpage1CompareExperimentsCheckBox" );
  vLoadExperimentLayout->addWidget( vpage1CompareExperimentsCheckBox );
   vRBLayout->addLayout( vLoadExperimentLayout );

  line3 = new QFrame( vWStackPage, "line3" );
  line3->setFrameShape( QFrame::HLine );
  line3->setFrameShadow( QFrame::Sunken );
  line3->setFrameShape( QFrame::HLine );
  vRBLayout->addWidget( line3 );

  vpcSampleRBLayout = new QHBoxLayout( 0, 0, 6, "vpcSampleRBLayout"); 
  spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vpcSampleRBLayout->addItem( spacer5 );

  vpage1pcSampleRB = new QRadioButton( vWStackPage, "vpage1pcSampleRB" );
  vpage1pcSampleRB->setChecked( TRUE );
  vpcSampleRBLayout->addWidget( vpage1pcSampleRB );
  vRBLayout->addLayout( vpcSampleRBLayout );

  vUserTimeRBLayout = new QHBoxLayout( 0, 0, 6, "vUserTimeRBLayout"); 
  spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vUserTimeRBLayout->addItem( spacer6 );

  vpage1UserTimeRB = new QRadioButton( vWStackPage, "vpage1UserTimeRB" );
  vUserTimeRBLayout->addWidget( vpage1UserTimeRB );
  vRBLayout->addLayout( vUserTimeRBLayout );

  vHWCounterRBLayout = new QHBoxLayout( 0, 0, 6, "vHWCounterRBLayout"); 
  spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vHWCounterRBLayout->addItem( spacer7 );

  vpage1HardwareCounterRB = new QRadioButton( vWStackPage, "vpage1HardwareCounterRB" );
  vHWCounterRBLayout->addWidget( vpage1HardwareCounterRB );
  vRBLayout->addLayout( vHWCounterRBLayout );

  vFloatingPointRBLayout = new QHBoxLayout( 0, 0, 6, "vFloatingPointRBLayout"); 
  spacer7_3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vFloatingPointRBLayout->addItem( spacer7_3 );

  vpage1FloatingPointRB = new QRadioButton( vWStackPage, "vpage1FloatingPointRB" );
  vFloatingPointRBLayout->addWidget( vpage1FloatingPointRB );
  vRBLayout->addLayout( vFloatingPointRBLayout );

  vInputOutputRBLayout = new QHBoxLayout( 0, 0, 6, "vInputOutputRBLayout"); 
  spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vInputOutputRBLayout->addItem( spacer7_4 );

  vpage1InputOutputRB = new QRadioButton( vWStackPage, "vpage1InputOutputRB" );
  vInputOutputRBLayout->addWidget( vpage1InputOutputRB );
  vRBLayout->addLayout( vInputOutputRBLayout );

  vMPIRBLayout = new QHBoxLayout( 0, 0, 6, "vMPIRBLayout"); 
  spacer7_4 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vMPIRBLayout->addItem( spacer7_4 );

  vpage1MPIRB = new QRadioButton( vWStackPage, "vpage1MPIRB" );
  vMPIRBLayout->addWidget( vpage1MPIRB );
  vRBLayout->addLayout( vMPIRBLayout );

  QSpacerItem *bfs = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vRBLayout->addItem( bfs );

  vWStackPageLayout->addLayout( vRBLayout );


  eWStackPage = new QWidget( mainWidgetStack, "eWStackPage" );
  WStackPageLayout_2 = new QVBoxLayout( eWStackPage, 11, 6, "WStackPageLayout_2"); 

  eRBLayout = new QVBoxLayout( 0, 0, 6, "eRBLayout"); 

  eHelpfulLabel = new QLabel( eWStackPage, "eHelpfulLabel" );
eHelpfulLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eRBLayout->addWidget( eHelpfulLabel );

  eLoadExperimentRBLayout = new QVBoxLayout( 0, 0, 6, "eLoadExperimentRBLayout"); 
  spacer5_2_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  eLoadExperimentRBLayout->addItem( spacer5_2_2 );

  epage1LoadExperimentCheckBox = new QCheckBox( eWStackPage, "epage1LoadExperimentCheckBox" );
  eLoadExperimentRBLayout->addWidget( epage1LoadExperimentCheckBox );
//  eRBLayout->addLayout( eLoadExperimentRBLayout );

  epage1CompareExperimentsCheckBox = new QCheckBox( eWStackPage, "epage1CompareExperimentsCheckBox" );
  eLoadExperimentRBLayout->addWidget( epage1CompareExperimentsCheckBox );
  eRBLayout->addLayout( eLoadExperimentRBLayout );

  line2 = new QFrame( eWStackPage, "line2" );
  line2->setFrameShape( QFrame::HLine );
  line2->setFrameShadow( QFrame::Sunken );
  line2->setFrameShape( QFrame::HLine );
  eRBLayout->addWidget( line2 );

  epcSampleRBLayout = new QHBoxLayout( 0, 0, 6, "epcSampleRBLayout"); 
  spacer5_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
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
  spacer7_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  eHWCounterRBLayout->addItem( spacer7_2 );

  epage1HardwareCounterRB = new QRadioButton( eWStackPage, "epage1HardwareCounterRB" );
  eHWCounterRBLayout->addWidget( epage1HardwareCounterRB );
  eRBLayout->addLayout( eHWCounterRBLayout );

  eFloatingPointRBLayout = new QHBoxLayout( 0, 0, 6, "eFloatingPointRBLayout"); 
  spacer7_3_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  eFloatingPointRBLayout->addItem( spacer7_3_2 );

  epage1FloatingPointRB = new QRadioButton( eWStackPage, "epage1FloatingPointRB" );
  eFloatingPointRBLayout->addWidget( epage1FloatingPointRB );
  eRBLayout->addLayout( eFloatingPointRBLayout );

  eInputOutputRBLayout = new QHBoxLayout( 0, 0, 6, "eInputOutputRBLayout"); 
  spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  eInputOutputRBLayout->addItem( spacer7_4_2 );

  epage1InputOutputRB = new QRadioButton( eWStackPage, "epage1InputOutputRB" );
  eInputOutputRBLayout->addWidget( epage1InputOutputRB );
  eRBLayout->addLayout( eInputOutputRBLayout );

  eMPIRBLayout = new QHBoxLayout( 0, 0, 6, "eMPIRBLayout"); 
  spacer7_4_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  eMPIRBLayout->addItem( spacer7_4_2 );

  epage1MPIRB = new QRadioButton( eWStackPage, "epage1MPIRB" );
  eMPIRBLayout->addWidget( epage1MPIRB );
  eRBLayout->addLayout( eMPIRBLayout );

  WStackPageLayout_2->addLayout( eRBLayout );

  QSpacerItem *spacer1 = new QSpacerItem( 20, 5, QSizePolicy::Fixed, QSizePolicy::Fixed );
  IntroWizardFormLayout->addItem( spacer1 );

  vNextButtonLayout = new QHBoxLayout( 0, 0, 6, "vNextButtonLayout"); 
  IntroWizardFormLayout->addLayout( vNextButtonLayout );

  QSpacerItem *spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vNextButtonLayout->addItem( spacer2 );


  wizardMode = new QCheckBox( getBaseWidgetFrame(),  "wizardMode" );
  wizardMode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, wizardMode->sizePolicy().hasHeightForWidth() ) );
  wizardMode->setChecked( TRUE );
  vNextButtonLayout->addWidget( wizardMode );
 
  spacer44 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vNextButtonLayout->addItem( spacer44 );
  broughtToYouByLabel = new QLabel( getBaseWidgetFrame(), "broughtToYouByLabel" );
  broughtToYouByLabel->setMinimumSize( QSize(1,1) );
  broughtToYouByLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vNextButtonLayout->addWidget( broughtToYouByLabel );

  spacer44 = new QSpacerItem( 1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vNextButtonLayout->addItem( spacer44 );

  vpage1NextButton = new QPushButton( getBaseWidgetFrame(), "vpage1NextButton" );
  vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonLayout->addWidget( vpage1NextButton );

  QSpacerItem *spacer3 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  vNextButtonLayout->addItem( spacer3 );


  mainWidgetStack->addWidget( vWStackPage, 0 );
  mainWidgetStack->addWidget( eWStackPage, 1 );


  QSpacerItem *fill_spacer = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed );
  IntroWizardFormLayout->addItem( fill_spacer );


  // signals and slots connections
  connect( wizardMode, SIGNAL( clicked() ), this, SLOT( wizardModeSelected() ) );
  connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vpage1NextButtonSelected() ) );
   connect( vpage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(vpage1pcSampleRBChanged() ) );
  connect( vpage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(vpage1UserTimeRBChanged() ) );
  connect( vpage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(vpage1HardwareCounterRBChanged() ) );
  connect( vpage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(vpage1FloatingPointRBChanged() ) );
  connect( vpage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(vpage1InputOutputRBChanged() ) );
  connect( vpage1MPIRB, SIGNAL( clicked() ), this, SLOT(vpage1MPIRBChanged() ) );

  connect( vpage1LoadExperimentCheckBox, SIGNAL( clicked() ), this, SLOT(page1LoadExperimentCheckBoxChanged() ) );
  connect( vpage1CompareExperimentsCheckBox, SIGNAL( clicked() ), this, SLOT(page1CompareExperimentsCheckBoxChanged() ) );
  connect( epage1LoadExperimentCheckBox, SIGNAL( clicked() ), this, SLOT(page1LoadExperimentCheckBoxChanged() ) );
  connect( epage1CompareExperimentsCheckBox, SIGNAL( clicked() ), this, SLOT(page1CompareExperimentsCheckBoxChanged() ) );


  connect( epage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(epage1pcSampleRBChanged() ) );
  connect( epage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(epage1UserTimeRBChanged() ) );
  connect( epage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(epage1HardwareCounterRBChanged() ) );
  connect( epage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(epage1FloatingPointRBChanged() ) );
  connect( epage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(epage1InputOutputRBChanged() ) );
  connect( epage1MPIRB, SIGNAL( clicked() ), this, SLOT(epage1MPIRBChanged() ) );
  languageChange();

// printf("Call resize initially\n");
  handleSizeEvent((QResizeEvent *)NULL);
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
IntroWizardPanel::listener(void *msg)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::listener() requested.\n");
  MessageObject *messageObject = (MessageObject *)msg;
  if( messageObject->msgType == "&Intro Wizard" || 
      messageObject->msgType == "Intro Wizard" )
  {
    nprintf(DEBUG_MESSAGES) ("IntroWizardPanel::listener() interested!\n");
    return 1;
  }

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
  vWelcomeHeader->setText( tr( "<h2>Welcome to Open|SpeedShop(tm)</h2>" ) );

  vHelpfulLabel->setText( tr( "Please select which of the following are true for your application:" ) );
  vpage1LoadExperimentCheckBox->setText( tr( "I already have experiment data and would like to analyze it." ) );
  vpage1CompareExperimentsCheckBox->setText( tr( "I have two saved experiment data files that I'd like to compare to each other." ) );
  vpage1pcSampleRB->setText( tr( "I'm trying to find where my program is spending most of its time. (pcsamp)" ) );
  vpage1UserTimeRB->setText( tr( "I'd like to find out how much time is system time vs. my program's time. (usertime)" ) );
  vpage1HardwareCounterRB->setText( tr( "I'd like to see what the internal Hardware Counters can show me.\n"
"(Hardware Counters are an advanced feature that utilizes machine \n"
"hardware to measure certain attributes of program execution.) (hwc/hwctime)" ) );
  vpage1FloatingPointRB->setText( tr( "I need to measure how many times I am causing Floating Point Exceptions. (fpe)" ) );
  vpage1InputOutputRB->setText( tr( "My program does a lot of Input and Output and I'd like to trace that work. (io/iot)" ) );
  vpage1MPIRB->setText( tr( "I have an MPI program and I'd like measure the mpi calls. (mpi/mpit)" ) );

  eHelpfulLabel->setText( tr( "Please select which of the following are true for your application:" ) );
  epage1LoadExperimentCheckBox->setText( tr( "Load experiment data." ) );
  epage1CompareExperimentsCheckBox->setText( tr( "Compare two experiments." ) );
  epage1pcSampleRB->setText( tr( "pcSampling (profiling)" ) );
  epage1UserTimeRB->setText( tr( "User Time Experiment." ) );
  epage1HardwareCounterRB->setText( tr( "Hardware Counter Tracing." ) );
  epage1FloatingPointRB->setText( tr( "Floating Point Exceptions Tracing" ) );
  epage1InputOutputRB->setText( tr( "Input/Output Tracing" ) );
  epage1MPIRB->setText( tr( "MPI Tracing" ) );

  wizardMode->setText( tr( "Verbose Wizard Mode" ) );
  vpage1NextButton->setText( tr( "> Next" ) );

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
}

void IntroWizardPanel::vpage1NextButtonSelected()
{
  Panel *p = NULL;
  if( wizardMode->isOn() )
  {
    if( vpage1LoadExperimentCheckBox->isOn() )
    {
      QString fn = QString::null;
      char *cwd = get_current_dir_name();
      fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;AnyFiles (*.*)", this, "open experiment dialog", "Choose an experiment file to open");
      free(cwd);
      if( !fn.isEmpty() )
      {
//      printf("fn = %s\n", fn.ascii() );
//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
        getPanelContainer()->getMainWindow()->executableName = QString::null;
        getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn);
      } else
      {
        fprintf(stderr, "No experiment file name given.\n");
      }
      return;
    }
    if( vpage1CompareExperimentsCheckBox->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("Compare Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Compare Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1pcSampleRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("pc Sample Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1UserTimeRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("User Time Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1HardwareCounterRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("HW Counter Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1FloatingPointRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("FPE Tracing Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1InputOutputRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("IO Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( vpage1MPIRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("MPI Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
  } else
  {
  
    Panel *p = NULL;
    if( epage1LoadExperimentCheckBox->isOn() )
    {
      QString fn = QString::null;
      char *cwd = get_current_dir_name();
      fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;AnyFiles (*.*)", this, "open experiment dialog", "Choose an experiment file to open");
      free(cwd);
      if( !fn.isEmpty() )
      {
//      printf("fn = %s\n", fn.ascii() );
//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
        getPanelContainer()->getMainWindow()->executableName = QString::null;
        getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn);
      } else
      {
        fprintf(stderr, "No experiment file name given.\n");
      }
      return;
    }
    if( epage1CompareExperimentsCheckBox->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("Compare Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Compare Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1pcSampleRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("pc Sample Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1UserTimeRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("User Time Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1HardwareCounterRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("HW Counter Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1FloatingPointRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("FPE Tracing Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1InputOutputRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("IO Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }
    if( epage1MPIRB->isOn() )
    {
      p = getPanelContainer()->raiseNamedPanel("MPI Wizard");
      if( !p )
      {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer(), ao);
        delete ao;
      } else
      {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
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

void IntroWizardPanel::page1LoadExperimentCheckBoxChanged()
{
  vSetStateChanged(NULL);
  eSetStateChanged(NULL);
  vpage1CompareExperimentsCheckBox->setChecked( FALSE );
  epage1CompareExperimentsCheckBox->setChecked( FALSE );
  if( wizardMode->isOn() )
  {
    if( !vpage1LoadExperimentCheckBox->isOn() &&
        !vpage1CompareExperimentsCheckBox->isOn() )
    {
      vpage1pcSampleRB->setChecked( TRUE );
      epage1pcSampleRB->setChecked( TRUE );
    }
  } else
  {
    if( !epage1LoadExperimentCheckBox->isOn()  &&
        !epage1CompareExperimentsCheckBox->isOn() )
    {
      vpage1pcSampleRB->setChecked( TRUE );
      epage1pcSampleRB->setChecked( TRUE );
    }
  }
}

void IntroWizardPanel::page1CompareExperimentsCheckBoxChanged()
{
  vSetStateChanged(NULL);
  eSetStateChanged(NULL);
  vpage1LoadExperimentCheckBox->setChecked( FALSE );
  epage1LoadExperimentCheckBox->setChecked( FALSE );
  if( wizardMode->isOn() )
  {
    if( !vpage1LoadExperimentCheckBox->isOn() &&
        !vpage1CompareExperimentsCheckBox->isOn() )
    {
      vpage1pcSampleRB->setChecked( TRUE );
      epage1pcSampleRB->setChecked( TRUE );
    }
  } else
  {
    if( !epage1LoadExperimentCheckBox->isOn()  &&
        !epage1CompareExperimentsCheckBox->isOn() )
    {
      vpage1pcSampleRB->setChecked( TRUE );
      epage1pcSampleRB->setChecked( TRUE );
    }
  }
}

void IntroWizardPanel::vSetStateChanged(QRadioButton *rb)
{
  vpage1pcSampleRB->setChecked( FALSE );
  vpage1UserTimeRB->setChecked( FALSE );
  vpage1HardwareCounterRB->setChecked( FALSE );
  vpage1FloatingPointRB->setChecked( FALSE );
  vpage1InputOutputRB->setChecked( FALSE );
  vpage1MPIRB->setChecked( FALSE );
  if( rb != NULL )
  {
    rb->setChecked(TRUE);
    vpage1LoadExperimentCheckBox->setChecked( FALSE );
    epage1LoadExperimentCheckBox->setChecked( FALSE );
    vpage1CompareExperimentsCheckBox->setChecked( FALSE );
    epage1CompareExperimentsCheckBox->setChecked( FALSE );
  }
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
  if( rb != NULL )
  {
    rb->setChecked(TRUE);
    vpage1LoadExperimentCheckBox->setChecked( FALSE );
    epage1LoadExperimentCheckBox->setChecked( FALSE );
    vpage1CompareExperimentsCheckBox->setChecked( FALSE );
    epage1CompareExperimentsCheckBox->setChecked( FALSE );
  }
}

void
IntroWizardPanel::handleSizeEvent(QResizeEvent *e)
{
  int calculated_height = 0;
  int SPACER_SIZE = 20;
  calculated_height += vHelpfulLabel->height();
  calculated_height += vpage1LoadExperimentCheckBox->height();
  calculated_height += vpage1CompareExperimentsCheckBox->height();
  calculated_height += line3->height();
  calculated_height += vpage1pcSampleRB->height();
  calculated_height += vpage1UserTimeRB->height();
  calculated_height += vpage1HardwareCounterRB->height();
  calculated_height += vpage1FloatingPointRB->height();
  calculated_height += vpage1InputOutputRB->height();
  calculated_height += vpage1MPIRB->height();

  // add in the 2 spacers..
  calculated_height += 2*20;

  // add in margins... around the children
  calculated_height += 10*6;


  int height = getPanelContainer()->parent->height();
  if( calculated_height > height )
  {
    height = calculated_height;
  }

  big_box->resize(vpage1UserTimeRB->width()+100, calculated_height);
}
