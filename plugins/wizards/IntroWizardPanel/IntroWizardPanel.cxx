////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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
// Please uncomment the define below if you want to 
// see debug output when running the introduction wizard.
//#define DEBUG_INTRO 1
// End debug introduction wizard explanation

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
#if DEBUG_INTRO
  printf("IntroWizardPanel::IntroWizardPanel() constructor called.\n");
#endif

  if ( !getName() )
  {
	setName( "IntroWizardForm" );
  }


//  IntroWizardFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 11, 6, "IntroWizardFormLayout"); 
  IntroWizardFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 0, 0, "IntroWizardFormLayout"); 

  mainFrame = new QFrame( getBaseWidgetFrame(), "mainFrame" );
  mainFrame->setMinimumSize( QSize(10,10) );
  mainFrame->setFrameShape( QFrame::StyledPanel );
  mainFrame->setFrameShadow( QFrame::Raised );
  mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout");

  mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );
//  mainWidgetStack->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum, 0, 0, FALSE ) );
  mainWidgetStack->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vWStackPage0 = new QWidget( mainWidgetStack, "vWStackPage0" );
  vWStackPage0->setMinimumSize( QSize(10,10) );
  vWStackPage0Layout = new QVBoxLayout( vWStackPage0, 11, 6, "vWStackPage0Layout"); 

  vpage0sv = new QScrollView( vWStackPage0, "vpage0sv" );
//  vpage0sv->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum, 0, 0, FALSE ) );
  vpage0sv->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vpage0big_box = new QVBox(vpage0sv->viewport(), "vpage0big_box" );
  vpage0big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );
//  vpage0big_box->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vpage0sv->addChild(vpage0big_box);

  //
  // PAGE 0 vWStackPage Build starts here
  //


  vpage0WelcomeHeader = new QLabel( vWStackPage0, "vpage0WelcomeHeader" );
  vpage0WelcomeHeader->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vpage0WelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  vpage0WelcomeHeader->setMinimumSize( QSize(10,10) );
  const QColor vpage0color = vpage0WelcomeHeader->paletteBackgroundColor();
  vWStackPage0Layout->addWidget( vpage0WelcomeHeader);

  vpage0sv->viewport()->setBackgroundColor(vpage0color);
  vpage0sv->viewport()->setPaletteBackgroundColor(vpage0color);
//  vpage0sv->viewport()->setBackgroundColor("Yellow");


//jeg  vpage0SpacerItem1 = new QLabel( vpage0big_box, "vpage0SpacerItem1" );
  // Add the "Please select .... message
  vpage0HelpfulLabel = new QLabel( vpage0big_box, "vpage0HelpfulLabel" );


  vpage0SpacerItem2 = new QLabel( vpage0big_box, "vpage0SpacerItem2" );
  vpage0CreateLoadExpDataRB = new QRadioButton( vpage0big_box, "vpage0CreateLoadExpDataRB" );
  vpage0CreateLoadExpDataRB->setChecked( TRUE );
  // Add the Load Single Saved Experiment Data Radio Button 

  vpage0SpacerItem3 = new QLabel( vpage0big_box, "vpage0SpacerItem3" );
  vpage0SavedExpDataRB = new QRadioButton( vpage0big_box, "vpage0SavedExpDataRB" );
  vpage0SavedExpDataRB->setChecked( FALSE );

  // Compare Two Saved Experiment Data Files Radio Button add

  vpage0SpacerItem4 = new QLabel( vpage0big_box, "vpage0SpacerItem4" );
  vpage0SavedExpCompareDataRB = new QRadioButton( vpage0big_box, "vpage0SavedExpCompareDataRB" );
  vpage0SavedExpCompareDataRB->setChecked( FALSE );
  vpage0SpacerItem5 = new QLabel( vpage0big_box, "vpage0SpacerItem5" );

  vWStackPage0Layout->addWidget( vpage0sv);

  // -----------------------------
  // Page 0 Verbose Button Layout
  // -----------------------------

  vNextButtonPage0Layout = new QHBoxLayout( 0, 0, 6, "vNextButtonPage0Layout");

  vpage0wizardMode = new QCheckBox( vWStackPage0,  "vpage0wizardMode" );
//  vpage0wizardMode = new QCheckBox( getBaseWidgetFrame(),  "vpage0wizardMode" );
  vpage0wizardMode->setMinimumSize( QSize(10,10) );
  vpage0wizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vpage0wizardMode->setChecked( TRUE );
  vNextButtonPage0Layout->addWidget( vpage0wizardMode );

  vNextButtonPage0ButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  vNextButtonPage0Layout->addItem( vNextButtonPage0ButtonSpacer );

  vpage0IntroButton = new QPushButton( vWStackPage0, "vpage0IntroButton" );
//  vpage0IntroButton = new QPushButton( getBaseWidgetFrame(), "vpage0IntroButton" );
  vpage0IntroButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage0IntroButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage0Layout->addWidget( vpage0IntroButton );

  vpage0NextButton = new QPushButton( vWStackPage0, "vpage0NextButton" );
//  vpage0NextButton = new QPushButton( getBaseWidgetFrame(), "vpage0NextButton" );
  vpage0NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage0NextButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage0Layout->addWidget( vpage0NextButton );

  vpage0FinishButton = new QPushButton( vWStackPage0, "vpage0FinishButton" );
//  vpage0FinishButton = new QPushButton( getBaseWidgetFrame(), "vpage0FinishButton" );
  vpage0FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage0FinishButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage0Layout->addWidget( vpage0FinishButton );

//  IntroWizardFormLayout->addLayout( vNextButtonPage0Layout );
//  mainFrameLayout->addLayout( vNextButtonPage0Layout );
  vWStackPage0Layout->addLayout( vNextButtonPage0Layout );

  mainWidgetStack->addWidget( vWStackPage0, 0 );


  // -----------------------------
  // Page 0 Expert ScrollView setup 
  // -----------------------------

  eWStackPage0 = new QWidget( mainWidgetStack, "eWStackPage0" );
  eWStackPage0->setMinimumSize( QSize(10,10) );
  eWStackPage0Layout = new QVBoxLayout( eWStackPage0, 11, 6, "eWStackPage0Layout"); 

  epage0sv = new QScrollView( eWStackPage0, "epage0sw" );
  epage0sv->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  epage0big_box = new QVBox(epage0sv->viewport(), "epage0big_box" );
  epage0big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  epage0sv->addChild(epage0big_box);

  // Page 0 Radio Button Choice Setup

  epage0WelcomeHeader = new QLabel( eWStackPage0, "epage0WelcomeHeader" );
  epage0WelcomeHeader->setMinimumSize( QSize(10,10) );
  epage0WelcomeHeader->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, FALSE ) );
  epage0WelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  const QColor epage0color = epage0WelcomeHeader->paletteBackgroundColor();

  epage0sv->viewport()->setBackgroundColor(epage0color);
  epage0sv->viewport()->setPaletteBackgroundColor(epage0color);

  eWStackPage0Layout->addWidget( epage0WelcomeHeader);

//jeg  epage0SpacerItem1 = new QLabel( epage0big_box, "epage0SpacerItem1" );

  epage0HelpfulLabel = new QLabel( epage0big_box, "epage0HelpfulLabel" );
//  epage0HelpfulLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );

  epage0SpacerItem2 = new QLabel( epage0big_box, "epage0SpacerItem2" );

  epage0CreateLoadExpDataRB = new QRadioButton( epage0big_box, "epage0CreateLoadExpDataRB" );
  epage0CreateLoadExpDataRB->setChecked( TRUE );

  epage0SpacerItem3 = new QLabel( epage0big_box, "epage0SpacerItem3" );

  epage0SavedExpDataRB = new QRadioButton( epage0big_box, "epage0SavedExpDataRB" );
  epage0SavedExpDataRB->setChecked( FALSE );

  epage0SpacerItem4 = new QLabel( epage0big_box, "epage0SpacerItem4" );

  epage0SavedExpCompareDataRB = new QRadioButton( epage0big_box, "epage0SavedExpCompareDataRB" );
  epage0SavedExpCompareDataRB->setChecked( FALSE );

  epage0SpacerItem5 = new QLabel( epage0big_box, "epage0SpacerItem5" );

  eWStackPage0Layout->addWidget( epage0sv);

  // ----------------------------------------------------------------
  // Start the flow control button layout for the Expert Page 0 
  // ----------------------------------------------------------------

  eNextButtonPage0Layout = new QHBoxLayout( 0, 0, 6, "eNextButtonPage0Layout");

  epage0wizardMode = new QCheckBox( eWStackPage0,  "epage0wizardMode" );
  epage0wizardMode->setMinimumSize( QSize(10,10) );
  epage0wizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  epage0wizardMode->setChecked( TRUE );

  eNextButtonPage0Layout->addWidget( epage0wizardMode );

  eNextButtonPage0ButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  eNextButtonPage0Layout->addItem( eNextButtonPage0ButtonSpacer );

  epage0IntroButton = new QPushButton( eWStackPage0, "epage0IntroButton" );
  epage0IntroButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage0IntroButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage0Layout->addWidget( epage0IntroButton );

  epage0NextButton = new QPushButton( eWStackPage0, "epage0NextButton" );
  epage0NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage0NextButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage0Layout->addWidget( epage0NextButton );

  epage0FinishButton = new QPushButton( eWStackPage0, "epage0FinishButton" );
  epage0FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage0FinishButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage0Layout->addWidget( epage0FinishButton );

  eWStackPage0Layout->addLayout( eNextButtonPage0Layout );

  // 
  // PAGE 1 vWStackPage1 Build starts here
  //

  vWStackPage1 = new QWidget( mainWidgetStack, "vWStackPage1" );
  vWStackPage1->setMinimumSize( QSize(10,10) );
  vWStackPage1Layout = new QVBoxLayout( vWStackPage1, 11, 6, "vWStackPage1Layout"); 

  vpage1sv = new QScrollView( vWStackPage1, "vpage1sv" );
  vpage1sv->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vpage1big_box = new QVBox(vpage1sv->viewport(), "vpage1big_box" );
  vpage1big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  vpage1sv->addChild(vpage1big_box);

  vpage1WelcomeHeader = new QLabel( vWStackPage1, "vpage1WelcomeHeader" );
  vpage1WelcomeHeader->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vpage1WelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  vpage1WelcomeHeader->setMinimumSize( QSize(10,10) );
  const QColor vpage1_color = vpage1WelcomeHeader->paletteBackgroundColor();
  vWStackPage1Layout->addWidget( vpage1WelcomeHeader);

  vpage1sv->viewport()->setBackgroundColor(vpage1_color);
  vpage1sv->viewport()->setPaletteBackgroundColor(vpage1_color);

  // Add the "Please select .... message
  vpage1HelpfulLabel = new QLabel( vpage1big_box, "vpage1HelpfulLabel" );

  vpage1SpacerItem2 = new QLabel( vpage1big_box, "vpage1SpacerItem2" );

  vpage1pcSampleRB = new QRadioButton( vpage1big_box, "vpage1pcSampleRB" );
  vpage1pcSampleRB->setChecked( TRUE );

  vpage1SpacerItem3 = new QLabel( vpage1big_box, "vpage1SpacerItem3" );

  vpage1UserTimeRB = new QRadioButton( vpage1big_box, "vpage1UserTimeRB" );
  vpage1UserTimeRB->setChecked(FALSE);

  vpage1SpacerItem4 = new QLabel( vpage1big_box, "vpage1SpacerItem4" );

  vpage1HardwareCounterRB = new QRadioButton( vpage1big_box, "vpage1HardwareCounterRB" );
  vpage1HardwareCounterRB->setChecked(FALSE);

  vpage1SpacerItem4b = new QLabel( vpage1big_box, "vpage1SpacerItem4b" );

  vpage1HardwareCounterSampRB = new QRadioButton( vpage1big_box, "vpage1HardwareCounterSampRB" );
  vpage1HardwareCounterSampRB->setChecked(FALSE);

  vpage1SpacerItem5 = new QLabel( vpage1big_box, "vpage1SpacerItem5" );

  vpage1FloatingPointRB = new QRadioButton( vpage1big_box, "vpage1FloatingPointRB" );
  vpage1FloatingPointRB->setChecked(FALSE);

  vpage1SpacerItem6 = new QLabel( vpage1big_box, "vpage1SpacerItem6" );

  vpage1InputOutputRB = new QRadioButton( vpage1big_box, "vpage1InputOutputRB" );
  vpage1InputOutputRB->setChecked(FALSE);

  vpage1SpacerItem7 = new QLabel( vpage1big_box, "vpage1SpacerItem7" );

  vpage1MPIRB = new QRadioButton( vpage1big_box, "vpage1MPIRB" );
  vpage1MPIRB->setChecked(FALSE);

  vpage1SpacerItem8 = new QLabel( vpage1big_box, "vpage1SpacerItem8" );

  vWStackPage1Layout->addWidget( vpage1sv);

  // Handle the buttons for Page 1 verbose

  vNextButtonPage1Layout = new QHBoxLayout( 0, 0, 6, "vNextButtonPage1Layout");

  vpage1wizardMode = new QCheckBox( vWStackPage1,  "vpage1wizardMode" );
  vpage1wizardMode->setMinimumSize( QSize(10,10) );
  vpage1wizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vpage1wizardMode->setChecked( TRUE );
  vNextButtonPage1Layout->addWidget( vpage1wizardMode );

  vNextButtonPage1ButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  vNextButtonPage1Layout->addItem( vNextButtonPage1ButtonSpacer );

  vpage1BackButton = new QPushButton( vWStackPage1, "vpage1BackButton" );
  vpage1BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1BackButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage1Layout->addWidget( vpage1BackButton );

  vpage1NextButton = new QPushButton( vWStackPage1, "vpage1NextButton" );
  vpage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1NextButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage1Layout->addWidget( vpage1NextButton );

  vpage1FinishButton = new QPushButton( vWStackPage1, "vpage1FinishButton" );
  vpage1FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vpage1FinishButton->sizePolicy().hasHeightForWidth() ) );
  vNextButtonPage1Layout->addWidget( vpage1FinishButton );

  vWStackPage1Layout->addLayout( vNextButtonPage1Layout );

  mainWidgetStack->addWidget( vWStackPage1, 0 );


  // Expert layout for Page 1

  eWStackPage1 = new QWidget( mainWidgetStack, "eWStackPage1" );
  eWStackPage1->setMinimumSize( QSize(10,10) );
  eWStackPage1Layout = new QVBoxLayout( eWStackPage1, 11, 6, "eWStackPage1Layout"); 

  epage1sv = new QScrollView( eWStackPage1, "epage1sv" );
  epage1sv->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  epage1big_box = new QVBox(epage1sv->viewport(), "epage1big_box" );
  epage1big_box->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum, 0, 0, FALSE ) );

  epage1sv->addChild(epage1big_box);

  epage1WelcomeHeader = new QLabel( eWStackPage1, "epage1WelcomeHeader" );
  epage1WelcomeHeader->setMinimumSize( QSize(10,10) );
  epage1WelcomeHeader->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, 0, 0, FALSE ) );
  epage1WelcomeHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  const QColor epage1_color = epage1WelcomeHeader->paletteBackgroundColor();

  eWStackPage1Layout->addWidget( epage1WelcomeHeader);

  epage1sv->viewport()->setBackgroundColor(epage1_color);
  epage1sv->viewport()->setPaletteBackgroundColor(epage1_color);


  // Add the "Please select .... message
  epage1HelpfulLabel = new QLabel( epage1big_box, "epage1HelpfulLabel" );

  epage1SpacerItem2 = new QLabel( epage1big_box, "epage1SpacerItem2" );

  epage1pcSampleRB = new QRadioButton( epage1big_box, "epage1pcSampleRB" );
  epage1pcSampleRB->setChecked( TRUE );

  epage1SpacerItem3 = new QLabel( epage1big_box, "epage1SpacerItem3" );

  epage1UserTimeRB = new QRadioButton( epage1big_box, "epage1UserTimeRB" );
  epage1UserTimeRB->setChecked(FALSE);

  epage1SpacerItem4 = new QLabel( epage1big_box, "epage1SpacerItem4" );

  epage1HardwareCounterRB = new QRadioButton( epage1big_box, "epage1HardwareCounterRB" );
  epage1HardwareCounterRB->setChecked(FALSE);

  epage1SpacerItem4b = new QLabel( epage1big_box, "epage1SpacerItem4b" );

  epage1HardwareCounterSampRB = new QRadioButton( epage1big_box, "epage1HardwareCounterSampRB" );
  epage1HardwareCounterSampRB->setChecked(FALSE);

  epage1SpacerItem5 = new QLabel( epage1big_box, "epage1SpacerItem5" );

  epage1FloatingPointRB = new QRadioButton( epage1big_box, "epage1FloatingPointRB" );
  epage1FloatingPointRB->setChecked(FALSE);

  epage1SpacerItem6 = new QLabel( epage1big_box, "epage1SpacerItem6" );

  epage1InputOutputRB = new QRadioButton( epage1big_box, "epage1InputOutputRB" );
  epage1InputOutputRB->setChecked(FALSE);

  epage1SpacerItem7 = new QLabel( epage1big_box, "epage1SpacerItem7" );

  epage1MPIRB = new QRadioButton( epage1big_box, "epage1MPIRB" );
  epage1MPIRB->setChecked(FALSE);

  epage1SpacerItem8 = new QLabel( epage1big_box, "epage1SpacerItem8" );

  eWStackPage1Layout->addWidget( epage1sv);

  // -------------------------------------
  // Handle the buttons for Page 1 verbose
  // -------------------------------------

  eNextButtonPage1Layout = new QHBoxLayout( 0, 0, 6, "eNextButtonPage1Layout");

  epage1wizardMode = new QCheckBox( eWStackPage1,  "epage1wizardMode" );
  epage1wizardMode->setMinimumSize( QSize(10,10) );
  epage1wizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  epage1wizardMode->setChecked( TRUE );
  eNextButtonPage1Layout->addWidget( epage1wizardMode );

  eNextButtonPage1ButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  eNextButtonPage1Layout->addItem( eNextButtonPage1ButtonSpacer );

  epage1BackButton = new QPushButton( eWStackPage1, "epage1BackButton" );
  epage1BackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1BackButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage1Layout->addWidget( epage1BackButton );

  epage1NextButton = new QPushButton( eWStackPage1, "epage1NextButton" );
  epage1NextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1NextButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage1Layout->addWidget( epage1NextButton );

  epage1FinishButton = new QPushButton( eWStackPage1, "epage1FinishButton" );
  epage1FinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, epage1FinishButton->sizePolicy().hasHeightForWidth() ) );
  eNextButtonPage1Layout->addWidget( epage1FinishButton );

  eWStackPage1Layout->addLayout( eNextButtonPage1Layout );

//  Done above -->  mainWidgetStack->addWidget( vWStackPage0, 0 );
  mainWidgetStack->addWidget( eWStackPage0, 1 );
  mainWidgetStack->addWidget( vWStackPage1, 2 );
  mainWidgetStack->addWidget( eWStackPage1, 3 );

  mainFrameLayout->addWidget( mainWidgetStack);
  IntroWizardFormLayout->addWidget( mainFrame );

  // ----------------------------------------------------
  // SIGNAL SETUP AREA
  // ---------------------------------------------------

#if DEBUG_INTRO
  printf("IntroWizardPanel::IntroWizardPanel() constructor, start connect section of code\n");
#endif
  connect( vpage0wizardMode, SIGNAL( clicked() ), this, SLOT( vpage0wizardModeSelected() ) );
  connect( epage0wizardMode, SIGNAL( clicked() ), this, SLOT( epage0wizardModeSelected() ) );
  connect( epage1wizardMode, SIGNAL( clicked() ), this, SLOT( epage1wizardModeSelected() ) );
  connect( vpage1wizardMode, SIGNAL( clicked() ), this, SLOT( vpage1wizardModeSelected() ) );

  connect( vpage0NextButton, SIGNAL( clicked() ), this, SLOT( vORepage0NextButtonSelected() ) );
  connect( epage0NextButton, SIGNAL( clicked() ), this, SLOT( vORepage0NextButtonSelected() ) );
  connect( vpage1NextButton, SIGNAL( clicked() ), this, SLOT( vORepage1NextButtonSelected() ) );
  connect( epage1NextButton, SIGNAL( clicked() ), this, SLOT( vORepage1NextButtonSelected() ) );
  connect( vpage1BackButton, SIGNAL( clicked() ), this, SLOT( vpage1BackButtonSelected() ) );
  connect( epage1BackButton, SIGNAL( clicked() ), this, SLOT( epage1BackButtonSelected() ) );
  connect( vpage0SavedExpDataRB, SIGNAL( clicked() ), this, SLOT( vpage0SavedExpDataRBChanged() ) );
  connect( vpage0SavedExpCompareDataRB, SIGNAL( clicked() ), this, SLOT( vpage0SavedExpCompareDataRBChanged() ) );
  connect( vpage0CreateLoadExpDataRB, SIGNAL( clicked() ), this, SLOT( vpage0CreateLoadExpDataRBChanged() ) );
  connect( epage0SavedExpDataRB, SIGNAL( clicked() ), this, SLOT( epage0SavedExpDataRBChanged() ) );
  connect( epage0SavedExpCompareDataRB, SIGNAL( clicked() ), this, SLOT( epage0SavedExpCompareDataRBChanged() ) );
  connect( epage0CreateLoadExpDataRB, SIGNAL( clicked() ), this, SLOT( epage0CreateLoadExpDataRBChanged() ) );

  connect( vpage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(vpage1pcSampleRBChanged() ) );
  connect( vpage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(vpage1UserTimeRBChanged() ) );
  connect( vpage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(vpage1HardwareCounterRBChanged() ) );
  connect( vpage1HardwareCounterSampRB, SIGNAL( clicked() ), this, SLOT(vpage1HardwareCounterSampRBChanged() ) );
  connect( vpage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(vpage1FloatingPointRBChanged() ) );
  connect( vpage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(vpage1InputOutputRBChanged() ) );
  connect( vpage1MPIRB, SIGNAL( clicked() ), this, SLOT(vpage1MPIRBChanged() ) );

  connect( epage1pcSampleRB, SIGNAL( clicked() ), this, SLOT(epage1pcSampleRBChanged() ) );
  connect( epage1UserTimeRB, SIGNAL( clicked() ), this, SLOT(epage1UserTimeRBChanged() ) );
  connect( epage1HardwareCounterRB, SIGNAL( clicked() ), this, SLOT(epage1HardwareCounterRBChanged() ) );
  connect( epage1HardwareCounterSampRB, SIGNAL( clicked() ), this, SLOT(epage1HardwareCounterSampRBChanged() ) );
  connect( epage1FloatingPointRB, SIGNAL( clicked() ), this, SLOT(epage1FloatingPointRBChanged() ) );
  connect( epage1InputOutputRB, SIGNAL( clicked() ), this, SLOT(epage1InputOutputRBChanged() ) );
  connect( epage1MPIRB, SIGNAL( clicked() ), this, SLOT(epage1MPIRBChanged() ) );

  languageChange();

#if DEBUG_INTRO
  printf("IntroWizardPanel::IntroWizardPanel(), before call resize initially\n");
#endif

  handleSizeEvent((QResizeEvent *)NULL);

}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
IntroWizardPanel::~IntroWizardPanel()
{
  nprintf( DEBUG_CONST_DESTRUCT) ( "  IntroWizardPanel::~IntroWizardPanel() destructor called.\n");
#if DEBUG_INTRO
  printf("  IntroWizardPanel::~IntroWizardPanel() destructor called.\n");
#endif
}

//! Add user panel specific menu items if they have any.
bool
IntroWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::menu() requested.\n");
#if DEBUG_INTRO
  printf("IntroWizardPanel::menu() requested.\n");
#endif

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
#if DEBUG_INTRO
  printf("IntroWizardPanel::save() requested.\n");
#endif
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
#if DEBUG_INTRO
  printf("IntroWizardPanel::saveAs() requested.\n");
#endif
}

//! This function listens for messages.
int 
IntroWizardPanel::listener(void *msg)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::listener() requested.\n");
#if DEBUG_INTRO
  printf("IntroWizardPanel::listener() requested.\n");
#endif
  MessageObject *messageObject = (MessageObject *)msg;
  if( messageObject->msgType == "&Intro Wizard" || 
      messageObject->msgType == "Intro Wizard" )
  {
    nprintf(DEBUG_MESSAGES) ("IntroWizardPanel::listener() interested!\n");
#if DEBUG_INTRO
    printf("IntroWizardPanel::listener() interested!\n");
#endif
    return 1;
  }

  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
IntroWizardPanel::broadcast(char *msg)
{
  nprintf( DEBUG_PANELS ) ("IntroWizardPanel::broadcast() requested.\n");
#if DEBUG_INTRO
  printf("IntroWizardPanel::broadcast() requested.\n");
#endif
  return 0;
}

void IntroWizardPanel::languageChange()
{

  // This set up of the text for buttons and checkboxes is for Page 0

#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::languageChange()\n");
#endif

  setCaption( tr( "IntroWizardForm" ) );
  vpage0WelcomeHeader->setText( tr( "<h3>Welcome to Open|SpeedShop(tm)</h3>\n<p>Introduction Wizard page 1 of 2</p>" ) );
  epage0WelcomeHeader->setText( tr( "<h3>Welcome to Open|SpeedShop(tm)</h3>\n<p>Introduction Wizard page 1 of 2</p>" ) );
  vpage1WelcomeHeader->setText( tr( "<h3>Welcome to Open|SpeedShop(tm)</h3>\n<p>Introduction Wizard page 2 of 2</p>" ) );
  epage1WelcomeHeader->setText( tr( "<h3>Welcome to Open|SpeedShop(tm)</h3>\n<p>Introduction Wizard page 2 of 2</p>" ) );

//jeg  vpage0SpacerItem1->setText( tr( "\n" ) );
  vpage0SpacerItem2->setText( tr( "\n" ) );
  vpage0SpacerItem3->setText( tr( "\n" ) );
  vpage0SpacerItem4->setText( tr( "\n" ) );
  vpage0SpacerItem5->setText( tr( "\n" ) );

  vpage0HelpfulLabel->setText( tr( "Please select one of the following to begin analyzing your application or your previously saved performance data file\nfor performance issues:" ) );

  vpage0SavedExpDataRB->setText( tr( "LOAD SAVED PERFORMANCE DATA: I have a saved performance experiment data file that I would like to load and analyze.\nOpen|SpeedShop saved performance experiment filenames have the prefix '.openss'" ) );

  vpage0SavedExpCompareDataRB->setText( tr( "COMPARE SAVED PERFORMANCE DATA: I have two saved performance experiment data files that I would like to load and compare.\nOpen|SpeedShop saved performance experiment filenames have the prefix '.openss'" ) );

  vpage0CreateLoadExpDataRB->setText( tr( "GENERATE NEW PERFORMANCE DATA: I would like to load or attach to an application/executable and gather new performance information on it.\nA series of wizard panels will guide you through the process of creating a performance experiment and running it." ) );

//jeg  epage0SpacerItem1->setText( tr( "\n" ) );
  epage0SpacerItem2->setText( tr( "\n" ) );
  epage0SpacerItem3->setText( tr( "\n" ) );
  epage0SpacerItem4->setText( tr( "\n" ) );
  epage0SpacerItem5->setText( tr( "\n" ) );
  epage0HelpfulLabel->setText( tr( "Please select one of the following to begin analyzing your application or your previously saved performance data file\nfor performance issues:" ) );
  epage0SavedExpDataRB->setText( tr( "LOAD SAVED PERFORMANCE DATA: Load saved experiment file." ) );
  epage0SavedExpCompareDataRB->setText( tr( "COMPARE SAVED PERFORMANCE DATA: Compare two saved experiment files." ) );
  epage0CreateLoadExpDataRB->setText( tr( "GENERATE NEW PERFORMANCE DATA: Load or attach to an executable and gather new performance data." ) );
  vpage0wizardMode->setText( tr( "Verbose Wizard Mode" ) );
  QToolTip::add( vpage0wizardMode, tr( "Clicking on this will alter the mode between Verbose and Expert. Expert mode has less text." ) );
  epage0wizardMode->setText( tr( "Expert Wizard Mode" ) );
  QToolTip::add( epage0wizardMode, tr( "Clicking on this will change the mode between Verbose and Expert. Verbose mode has more text." ) );
  vpage0NextButton->setText( tr( "> Next" ) );
  QToolTip::add( vpage0NextButton, tr( "Advance to page two (2) of the Introduction wizard." ) );
  vpage0FinishButton->setText( tr( "> Finish" ) );
  QToolTip::add( vpage0FinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vpage0IntroButton->setText( tr( "< Back" ) );
  QToolTip::add( vpage0IntroButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  epage0NextButton->setText( tr( "> Next" ) );
  QToolTip::add( epage0NextButton, tr( "Advance to page two (2) of the Introduction wizard." ) );
  epage0FinishButton->setText( tr( "> Finish" ) );
  QToolTip::add( epage0FinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  epage0IntroButton->setText( tr( "< Back" ) );
  QToolTip::add( epage0IntroButton, tr( "Not active, present in this page for consistent use of buttons." ) );

  // Set these disabled - they are present for consistency
  epage0IntroButton->setEnabled(FALSE);
  vpage0IntroButton->setEnabled(FALSE);
  epage0FinishButton->setEnabled(FALSE);
  vpage0FinishButton->setEnabled(FALSE);



  // This set up of the text for buttons and checkboxes is for Page 1

//jeg  vpage1SpacerItem1->setText( tr( "\n" ) );
  vpage1SpacerItem2->setText( tr( "\n" ) );
  vpage1SpacerItem3->setText( tr( "\n" ) );
  vpage1SpacerItem4->setText( tr( "\n" ) );
  vpage1SpacerItem4b->setText( tr( "\n" ) );
  vpage1SpacerItem5->setText( tr( "\n" ) );
  vpage1SpacerItem6->setText( tr( "\n" ) );
  vpage1SpacerItem7->setText( tr( "\n" ) );
  vpage1SpacerItem8->setText( tr( "\n" ) );
  vpage1HelpfulLabel->setText( tr( "Please select one of the following options (EXPERIMENT: description) to indicate what type of performance information you are\ninterested in gathering  Open|SpeedShop will ask about loading your application or attaching to your running application later." ) );
  vpage1pcSampleRB->setText( tr( "PCSAMP: I'm trying to find where my program is spending most of its time.  Most lightweight impact on application." ) );
  vpage1UserTimeRB->setText( tr( "USERTIME: I'd like to see information about which routines are calling other routines in addition to the inclusive/exclusive timing information." ) );
  vpage1HardwareCounterRB->setText( tr( "HWC: I'd like to see what kind of performance information the internal Hardware Counters can show me." ) );
  vpage1HardwareCounterSampRB->setText( tr( "HWCSAMP: I'd like get an overview of how many hardware counter events are occurring for my selected set of up to six (6) internal hardware counters." ) );
  vpage1FloatingPointRB->setText( tr( "FPE: I would like to know how many times my program is causing Floating Point Exceptions and where in my program they are occuring." ) );
  vpage1InputOutputRB->setText( tr( "I/O: I would like to see which Input/Output calls are being made and where most of that time is being spent." ) );
  vpage1MPIRB->setText( tr( "MPI: I would like to see what MPI calls are being made and where the MPI calls are being made in my program." ) );

//jeg  epage1SpacerItem1->setText( tr( "\n" ) );
  epage1SpacerItem2->setText( tr( "\n" ) );
  epage1SpacerItem3->setText( tr( "\n" ) );
  epage1SpacerItem4->setText( tr( "\n" ) );
  epage1SpacerItem4b->setText( tr( "\n" ) );
  epage1SpacerItem5->setText( tr( "\n" ) );
  epage1SpacerItem6->setText( tr( "\n" ) );
  epage1SpacerItem7->setText( tr( "\n" ) );
  epage1SpacerItem8->setText( tr( "\n" ) );
  epage1HelpfulLabel->setText( tr( "Please select one of the following options (EXPERIMENT: description) to indicate what type of performance information you are\ninterested in gathering.  Open|SpeedShop will ask about loading your application or attaching to your running application later." ) );

  epage1pcSampleRB->setText( tr( "PCSAMP: (Program Counter Sampling)" ) );
  epage1UserTimeRB->setText( tr( "USERTIME: (CallStack Sampling)" ) );
  epage1HardwareCounterRB->setText( tr( "HWC: (Time and Record Hardware Counter Events)" ) );
  epage1HardwareCounterSampRB->setText( tr( "HWCSAMP: (Sample up to six (6) Hardware Counter Events)" ) );
  epage1FloatingPointRB->setText( tr( "FPE: (Record Floating Point Exception Events)" ) );
  epage1InputOutputRB->setText( tr( "I/O: (Wrap/Trace Input/Output Calls and Record Info)" ) );
  epage1MPIRB->setText( tr( "MPI: (Wrap/Trace MPI Calls and Record Info) " ) );

  vpage1wizardMode->setText( tr( "Verbose Wizard Mode" ) );
  QToolTip::add( vpage1wizardMode, tr( "Clicking on this will alter the mode between Verbose and Expert. Expert mode has less text." ) );
  epage1wizardMode->setText( tr( "Expert Wizard Mode" ) );
  QToolTip::add( epage1wizardMode, tr( "Clicking on this will change the mode between Verbose and Expert. Verbose mode has more text." ) );
  vpage1NextButton->setText( tr( "> Next" ) );
  QToolTip::add( vpage1NextButton, tr( "Takes you to wizard corresponding to your selection of the type of data you wish to gather." ) );
  vpage1FinishButton->setText( tr( "> Finish" ) );
  vpage1FinishButton->setFlat(TRUE);
  QToolTip::add( vpage1FinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vpage1BackButton->setText( tr( "< Back" ) );
  QToolTip::add( vpage1BackButton, tr( "Takes you back to page 1 of the Introduction wizard." ) );
  epage1NextButton->setText( tr( "> Next" ) );
  QToolTip::add( epage1NextButton, tr( "Takes you to wizard corresponding to your selection of the type of data you wish to gather." ) );
  epage1FinishButton->setText( tr( "> Finish" ) );
  QToolTip::add( epage1FinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  epage1FinishButton->setFlat(TRUE);
  epage1BackButton->setText( tr( "< Back" ) );
  QToolTip::add( epage1BackButton, tr( "Takes you back to page 1 of the Introduction wizard." ) );

  // Set these disabled - they are present for consistency
  epage1FinishButton->setEnabled(FALSE);
  vpage1FinishButton->setEnabled(FALSE);

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::languageChange()\n");
#endif
}


void IntroWizardPanel::epage0wizardModeSelected()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage0wizardModeSelected()\n");
#endif
  wizardModeSelected();

  if( raisedWidget == eWStackPage0 ) {

#if DEBUG_INTRO
      printf("IntroWizardPanel::epage0wizardModeSelected(), raised widget is eWStackPage0, raising vWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage0);
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );

  } else if( raisedWidget ==  eWStackPage1 ) {

#if DEBUG_INTRO
      printf("IntroWizardPanel::epage0wizardModeSelected(), raised widget is eWStackPage1, raising vWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage1);
      vpage1wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( TRUE );

  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage0wizardModeSelected(), Verbose to Expert: unknown WStackPage\n");
#endif
  }
}

void IntroWizardPanel::epage1wizardModeSelected()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1wizardModeSelected()\n");
#endif
  wizardModeSelected();

  if( raisedWidget == eWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1wizardModeSelected(), raised widget is eWStackPage1, raising vWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage1);
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );

  } else if( raisedWidget ==  eWStackPage0 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1wizardModeSelected(), raised widget is eWStackPage0, raising vWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage0);
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );
  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1wizardModeSelected(), Verbose to Expert: unknown WStackPage\n");
#endif
  }
}


void IntroWizardPanel::vpage1wizardModeSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vpage1wizardModeSelected()\n");
#endif
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
  wizardModeSelected();

  if( raisedWidget == vWStackPage0 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage1wizardModeSelected(), raised widget is vWStackPage0, raising eWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(eWStackPage0);
      epage1wizardMode->setChecked( TRUE );
      epage0wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( FALSE );
      vpage1wizardMode->setChecked( FALSE );

  } else if( raisedWidget ==  eWStackPage0 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage1wizardModeSelected(), raised widget is eWStackPage0, raising vWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage0);
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );
      epage1wizardMode->setChecked( FALSE );
      epage0wizardMode->setChecked( FALSE );
  } else if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage1wizardModeSelected(), raised widget is eWStackPage1, raising vWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage1);
      vpage1wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( TRUE );
      epage1wizardMode->setChecked( FALSE );
      epage0wizardMode->setChecked( FALSE );
  } else if( raisedWidget == vWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage1wizardModeSelected(), raised widget is vWStackPage1, raising eWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(eWStackPage1);
      vpage1wizardMode->setChecked( FALSE );
      vpage0wizardMode->setChecked( FALSE );
      epage1wizardMode->setChecked( TRUE );
      epage0wizardMode->setChecked( TRUE );

  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage1wizardModeSelected(), Expert to Verbose: unknown WStackPage\n");
#endif
  }
}


void IntroWizardPanel::vpage0wizardModeSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vpage0wizardModeSelected()\n");
#endif
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
  wizardModeSelected();

  if( raisedWidget == vWStackPage0 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0wizardModeSelected(), raised widget is vWStackPage0, raising eWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(eWStackPage0);
      epage0wizardMode->setChecked( TRUE );
      epage1wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( FALSE );
      vpage1wizardMode->setChecked( FALSE );

  } else if( raisedWidget ==  eWStackPage0 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0wizardModeSelected(), raised widget is eWStackPage0, raising vWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage0);
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );
      epage0wizardMode->setChecked( FALSE );
      epage1wizardMode->setChecked( FALSE );
  } else if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0wizardModeSelected(), raised widget is eWStackPage1, raising vWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage1);
      vpage1wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( TRUE );
      epage0wizardMode->setChecked( FALSE );
      epage1wizardMode->setChecked( FALSE );
  } else if( raisedWidget == vWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0wizardModeSelected(), raised widget is vWStackPage1, raising eWStackPage1\n");
#endif
      mainWidgetStack->raiseWidget(eWStackPage1);
      epage0wizardMode->setChecked( TRUE );
      epage1wizardMode->setChecked( TRUE );
      vpage0wizardMode->setChecked( FALSE );
      vpage1wizardMode->setChecked( FALSE );

  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0wizardModeSelected(), Expert to Verbose: unknown WStackPage\n");
#endif
  }
}


void IntroWizardPanel::printRaisedPanel()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();

  if( raisedWidget == vWStackPage0 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::printRaisedPanel(), vWStackPage0 is visible\n");
#endif
  } else if( raisedWidget ==  eWStackPage0 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::printRaisedPanel(), eWStackPage0 is visible\n");
#endif
  } else if( raisedWidget ==  vWStackPage1 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::printRaisedPanel(), vWStackPage1 is visible\n");
#endif
  } else if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::printRaisedPanel(), eWStackPage1 is visible\n");
#endif
  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::printRaisedPanel(), Expert to Verbose: unknown WStackPage\n");
#endif
  }

}


void IntroWizardPanel::wizardModeSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::wizardModeSelected(), vpage0wizardMode->isOn()=%d\n", vpage0wizardMode->isOn());
  printf("Enter IntroWizardPanel::wizardModeSelected(), vpage1wizardMode->isOn()=%d\n", vpage1wizardMode->isOn());
  printf("Enter IntroWizardPanel::wizardModeSelected(), epage0wizardMode->isOn()=%d\n", epage0wizardMode->isOn());
  printf("Enter IntroWizardPanel::wizardModeSelected(), epage1wizardMode->isOn()=%d\n", epage1wizardMode->isOn());
#endif
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();

  if( raisedWidget == vWStackPage0 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::wizardModeSelected(), vWStackPage0 is visible\n");
#endif
  } else if( raisedWidget ==  eWStackPage0 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::wizardModeSelected(), eWStackPage0 is visible\n");
#endif
  } else if( raisedWidget ==  vWStackPage1 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::wizardModeSelected(), vWStackPage1 is visible\n");
#endif
  } else if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf(" IntroWizardPanel::wizardModeSelected(), eWStackPage1 is visible\n");
#endif
  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::wizardModeSelected(), Expert to Verbose: unknown WStackPage\n");
#endif
  }

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::wizardModeSelected(), vpage0wizardMode->isOn()=%d\n", vpage0wizardMode->isOn());
  printf("Exit IntroWizardPanel::wizardModeSelected(), vpage1wizardMode->isOn()=%d\n", vpage1wizardMode->isOn());
  printf("Exit IntroWizardPanel::wizardModeSelected(), epage0wizardMode->isOn()=%d\n", epage0wizardMode->isOn());
  printf("Exit IntroWizardPanel::wizardModeSelected(), epage1wizardMode->isOn()=%d\n", epage1wizardMode->isOn());
#endif
}


void IntroWizardPanel::epage1BackButtonSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1BackButtonSelected()\n");
#endif

  QWidget *raisedWidget = mainWidgetStack->visibleWidget();

  if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1BackButtonSelected(), raised widget is eWStackPage1, raising eWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(eWStackPage0);
  } else if( raisedWidget == vWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1BackButtonSelected(), raised widget is vWStackPage1, raising vWStackPage0\n");
#endif
      mainWidgetStack->raiseWidget(vWStackPage0);

  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::epage1BackButtonSelected(), wrong page initiated this request\n");
#endif
  }
}

void IntroWizardPanel::vpage1BackButtonSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vpage1BackButtonSelected()\n");
#endif

  QWidget *raisedWidget = mainWidgetStack->visibleWidget();

  if( raisedWidget ==  eWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0BackButtonSelected(), raised widget is eWStackPage1, raising eWStackPage0\n");
#endif
      epage0wizardMode->setChecked( TRUE );
      epage1wizardMode->setChecked( TRUE );
      mainWidgetStack->raiseWidget(eWStackPage0);
  } else if( raisedWidget == vWStackPage1 ) {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0BackButtonSelected(), raised widget is vWStackPage1, raising vWStackPage0\n");
#endif
      vpage0wizardMode->setChecked( TRUE );
      vpage1wizardMode->setChecked( TRUE );
      mainWidgetStack->raiseWidget(vWStackPage0);

  } else {
#if DEBUG_INTRO
      printf("IntroWizardPanel::vpage0BackButtonSelected(), wrong page initiated this request\n");
#endif
  }
}


void IntroWizardPanel::vORepage0NextButtonSelected()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vORepage0NextButtonSelected(), vpage0wizardMode->isOn()=%d\n", vpage0wizardMode->isOn());
#endif
  printRaisedPanel();

  Panel *p = NULL;

  if( vpage0wizardMode->isOn() ) {

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
#endif

    if( vpage0SavedExpDataRB->isOn() ) {

    // ---------------------------------------------------------------------------
    // Process The Load A Saved Experiment File Option
    // ---------------------------------------------------------------------------

#if DEBUG_INTRO
      printf(" IntroWizardPanel::vORepage0NextButtonSelected(), IN vpage0SavedExpDataRB code\n");
#endif

      QString fn = QString::null;
      char *cwd = get_current_dir_name();
      fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;AnyFiles (*.*)", 
                                         this, "open experiment dialog", "Choose an experiment file to open");
      free(cwd);
      if( !fn.isEmpty() ) {
//      printf("fn = %s\n", fn.ascii() );
//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
        getPanelContainer()->getMainWindow()->executableName = QString::null;
        getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn);
      } else {
        fprintf(stderr, "No experiment file name given.\n");
      }
      return;
    }

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), vpage0SavedExpCompareDataRB->isOn()=%d\n", 
            vpage0SavedExpCompareDataRB->isOn());
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0SavedExpCompareDataRB->isOn()=%d\n", 
            epage0SavedExpCompareDataRB->isOn());
#endif

    if( vpage0SavedExpCompareDataRB->isOn() ) {

    // ---------------------------------------------------------------------------
    // Process The Compare Two Saved Experiment Files Option
    // ---------------------------------------------------------------------------

#if DEBUG_INTRO
      printf(" IntroWizardPanel::vORepage0NextButtonSelected(), IN vpage0SavedExpCompareDataRB code\n");
#endif
      p = getPanelContainer()->raiseNamedPanel((char *) "Compare Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Compare Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), vpage0CreateLoadExpDataRB->isOn()=%d\n", 
           vpage0CreateLoadExpDataRB->isOn());
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0CreateLoadExpDataRB->isOn()=%d\n", 
           epage0CreateLoadExpDataRB->isOn());
#endif

    if( vpage0CreateLoadExpDataRB->isOn() ) {

    // ---------------------------------------------------------------------------
    // Process The Create New Performance Data Option 
    // ---------------------------------------------------------------------------

#if DEBUG_INTRO
       printf("IntroWizardPanel::vORepage0NextButtonSelected(), IN vpage0CreateLoadExpDataRB code, before raiseWidget(2)\n");
#endif
       mainWidgetStack->raiseWidget(2);
#if DEBUG_INTRO
       printf("IntroWizardPanel::vORepage0NextButtonSelected(), IN vpage0CreateLoadExpDataRB code, after raiseWidget(2)\n");
#endif
    }

  } else {

    Panel *p = NULL;

    // Load one saved file

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
#endif
    if( epage0SavedExpDataRB->isOn() ) {

      QString fn = QString::null;
      char *cwd = get_current_dir_name();
      fn = QFileDialog::getOpenFileName( cwd, "Experiment Files (*.openss);;AnyFiles (*.*)", this, "open experiment dialog", "Choose an experiment file to open");
      free(cwd);
      if( !fn.isEmpty() ) {
//      printf("fn = %s\n", fn.ascii() );
//      fprintf(stderr, "Determine which panel to bring up base on experiment file %s\n", fn.ascii() );
        getPanelContainer()->getMainWindow()->executableName = QString::null;
        getPanelContainer()->getMainWindow()->fileOpenSavedExperiment(fn);
      } else {
        fprintf(stderr, "No experiment file name given.\n");
      }
      return;
    }

    // Compare two saved files

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
#endif

    if( epage0SavedExpCompareDataRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "Compare Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Compare Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf(" IntroWizardPanel::vORepage0NextButtonSelected(), epage0CreateLoadExpDataRB->isOn()=%d\n", epage0CreateLoadExpDataRB->isOn());
#endif

    if( epage0CreateLoadExpDataRB->isOn() ) {
#if DEBUG_INTRO
       printf("IntroWizardPanel::vORepage0NextButtonSelected(), IN epage0CreateLoadExpDataRB code, before raiseWidget(3)\n");
#endif
      
       epage1wizardMode->setChecked( TRUE );
       epage0wizardMode->setChecked( TRUE );
       mainWidgetStack->raiseWidget(3);
#if DEBUG_INTRO
       printf("IntroWizardPanel::vORepage0NextButtonSelected(), IN epage0CreateLoadExpDataRB code, after raiseWidget(3)\n");
#endif
    }


  }
}

void IntroWizardPanel::vORepage1NextButtonSelected()
{
  Panel *p = NULL;
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1wizardMode->isOn()=%d\n", vpage1wizardMode->isOn() );
  printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1wizardMode->isOn()=%d\n", epage1wizardMode->isOn() );
#endif

  if( vpage1wizardMode->isOn() ) {

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1pcSampleRB->isOn()=%d\n", vpage1pcSampleRB->isOn() );
#endif

    if( vpage1pcSampleRB->isOn() ) {

      p = getPanelContainer()->raiseNamedPanel((char *) "pc Sample Wizard");

#if DEBUG_INTRO
      printf("In IntroWizardPanel::vORepage1NextButtonSelected(), pcsamp, after getPanelContainer()->raiseNamedPanel, p=%d\n", p );
#endif

      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1UserTimeRB->isOn()=%d\n", vpage1UserTimeRB->isOn() );
#endif

    if( vpage1UserTimeRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "User Time Wizard");
#if DEBUG_INTRO
      printf("In IntroWizardPanel::vORepage1NextButtonSelected(), usertime, after getPanelContainer()->raiseNamedPanel, p=%d\n", p );
#endif
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1HardwareCounterRB->isOn()=%d\n", vpage1HardwareCounterRB->isOn() );
#endif

    if( vpage1HardwareCounterRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "HW Counter Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1HardwareCounterSampRB->isOn()=%d\n", vpage1HardwareCounterSampRB->isOn() );
#endif

    if( vpage1HardwareCounterSampRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "HW Counter Samp Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Samp Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1FloatingPointRB->isOn()=%d\n", vpage1FloatingPointRB->isOn() );
#endif

    if( vpage1FloatingPointRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "FPE Tracing Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1InputOutputRB->isOn()=%d\n", vpage1InputOutputRB->isOn() );
#endif

    if( vpage1InputOutputRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "IO Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer(), ao);
        delete ao;
       } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), vpage1MPIRB->isOn()=%d\n", vpage1MPIRB->isOn() );
#endif

    if( vpage1MPIRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "MPI Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

  } else {
  
    Panel *p = NULL;


#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1pcSampleRB->isOn()=%d\n", epage1pcSampleRB->isOn() );
#endif

    if( epage1pcSampleRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "pc Sample Wizard");
#if DEBUG_INTRO
      printf("In IntroWizardPanel::vORepage1NextButtonSelected(), e-pcsamp, after getPanelContainer()->raiseNamedPanel, p=%d\n", p );
#endif

      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Sample Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1UserTimeRB->isOn()=%d\n", epage1UserTimeRB->isOn() );
#endif

    if( epage1UserTimeRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "User Time Wizard");
#if DEBUG_INTRO
      printf("In IntroWizardPanel::vORepage1NextButtonSelected(), e-usertime, after getPanelContainer()->raiseNamedPanel, p=%d\n", p );
#endif

      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("User Time Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1HardwareCounterRB->isOn()=%d\n", epage1HardwareCounterRB->isOn() );
#endif

    if( epage1HardwareCounterRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "HW Counter Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1HardwareCounterSampRB->isOn()=%d\n", epage1HardwareCounterSampRB->isOn() );
#endif

    if( epage1HardwareCounterSampRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "HW Counter Samp Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HW Counter Samp Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1FloatingPointRB->isOn()=%d\n", epage1FloatingPointRB->isOn() );
#endif

    if( epage1FloatingPointRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "FPE Tracing Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1InputOutputRB->isOn()=%d\n", epage1InputOutputRB->isOn() );
#endif

    if( epage1InputOutputRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "IO Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("IO Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
        MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
        p->listener((void *)msg);
        delete msg;
      }
    }

#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vORepage1NextButtonSelected(), epage1MPIRB->isOn()=%d\n", epage1MPIRB->isOn() );
#endif

    if( epage1MPIRB->isOn() ) {
      p = getPanelContainer()->raiseNamedPanel((char *) "MPI Wizard");
      if( !p ) {
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", 1);
        getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI Wizard", getPanelContainer(), ao);
        delete ao;
      } else {
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
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1pcSampleRBChanged(), vpage1pcSampleRB->isOn()=%d\n", vpage1pcSampleRB->isOn());
#endif
  vpage1SetStateChanged(vpage1pcSampleRB);
}

void IntroWizardPanel::vpage1UserTimeRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1UserTimeRBChanged(), vpage1UserTimeRB->isOn()=%d\n", vpage1UserTimeRB->isOn());
#endif
  vpage1SetStateChanged(vpage1UserTimeRB);
}

void IntroWizardPanel::vpage1HardwareCounterRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1HardwareCounterRBChanged(), vpage1HardwareCounterRB->isOn()=%d\n", vpage1HardwareCounterRB->isOn());
#endif
  vpage1SetStateChanged(vpage1HardwareCounterRB);
}

void IntroWizardPanel::vpage1HardwareCounterSampRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1HardwareCounterSampRBChanged(), vpage1HardwareCounterSampRB->isOn()=%d\n", vpage1HardwareCounterSampRB->isOn());
#endif
  vpage1SetStateChanged(vpage1HardwareCounterSampRB);
}

void IntroWizardPanel::vpage1FloatingPointRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1FloatingPointRBChanged(), vpage1FloatingPointRB->isOn()=%d\n", vpage1FloatingPointRB->isOn());
#endif
  vpage1SetStateChanged(vpage1FloatingPointRB);
}

void IntroWizardPanel::vpage1InputOutputRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1InputOutputRBChanged(), vpage1InputOutputRB->isOn()=%d\n", vpage1InputOutputRB->isOn());
#endif
  vpage1SetStateChanged(vpage1InputOutputRB);
}

void IntroWizardPanel::vpage1MPIRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage1MPIRBChanged(), vpage1MPIRB->isOn()=%d\n", vpage1MPIRB->isOn());
#endif
  vpage1SetStateChanged(vpage1MPIRB);
}

void IntroWizardPanel::epage0CreateLoadExpDataRBChanged()
{
#if DEBUG_INTRO
  printf(" IntroWizardPanel::epage0CreateLoadExpDataRBChanged(), isOn()=%d\n", epage0SavedExpDataRB->isOn());
#endif
  eSetStateChanged(epage0CreateLoadExpDataRB);
}

void IntroWizardPanel::vpage0CreateLoadExpDataRBChanged()
{
#if DEBUG_INTRO
  printf(" IntroWizardPanel::vpage0CreateLoadExpDataRBChanged(), isOn()=%d\n", vpage0SavedExpDataRB->isOn());
#endif
  vSetStateChanged(vpage0CreateLoadExpDataRB);
}

void IntroWizardPanel::epage0SavedExpDataRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::epage0SavedExpDataRBChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpDataRBChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpDataRBChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpDataRBChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
#endif

  eSetStateChanged(this->epage0SavedExpDataRB);
}

void IntroWizardPanel::vpage0SavedExpDataRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage0SavedExpDataRBChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpDataRBChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpDataRBChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpDataRBChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
#endif

  vSetStateChanged(this->vpage0SavedExpDataRB);
}


void IntroWizardPanel::epage0SavedExpCompareDataRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::epage0SavedExpCompareDataRBChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpCompareDataRBChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpCompareDataRBChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("IntroWizardPanel::epage0SavedExpCompareDataRBChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
#endif

  eSetStateChanged(this->epage0SavedExpCompareDataRB);
}

void IntroWizardPanel::vpage0SavedExpCompareDataRBChanged()
{
#if DEBUG_INTRO
  printf("IntroWizardPanel::vpage0SavedExpCompareDataRBChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpCompareDataRBChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpCompareDataRBChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("IntroWizardPanel::vpage0SavedExpCompareDataRBChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
#endif

  vSetStateChanged(this->vpage0SavedExpCompareDataRB);
}

void IntroWizardPanel::vpage1SetStateChanged(QRadioButton *rb)
{

#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1pcSampleRB->isOn()=%d\n", vpage1pcSampleRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1UserTimeRB->isOn()=%d\n", vpage1UserTimeRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1HardwareCounterRB->isOn()=%d\n", vpage1HardwareCounterRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1HardwareCounterSampRB->isOn()=%d\n", vpage1HardwareCounterSampRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1FloatingPointRB->isOn()=%d\n", vpage1FloatingPointRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1InputOutputRB->isOn()=%d\n", vpage1InputOutputRB->isOn());
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, vpage1MPIRB->isOn()=%d\n", vpage1MPIRB->isOn());
 
  printf("Enter IntroWizardPanel::vpage1SetStateChanged, rb=%d\n", rb);
#endif
  if ( rb != NULL) {
#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vpage1SetStateChanged, rb->isOn()=%d\n", rb->isOn());
#endif
  }

  vpage1pcSampleRB->setChecked( FALSE );
  vpage1UserTimeRB->setChecked( FALSE );
  vpage1HardwareCounterRB->setChecked( FALSE );
  vpage1HardwareCounterSampRB->setChecked( FALSE );
  vpage1FloatingPointRB->setChecked( FALSE );
  vpage1InputOutputRB->setChecked( FALSE );
  vpage1MPIRB->setChecked( FALSE );
  if( rb != NULL ) {
    rb->setChecked(TRUE);
  }

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1pcSampleRB->isOn()=%d\n", vpage1pcSampleRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1UserTimeRB->isOn()=%d\n", vpage1UserTimeRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1HardwareCounterRB->isOn()=%d\n", vpage1HardwareCounterRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1HardwareCounterSampRB->isOn()=%d\n", vpage1HardwareCounterSampRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1FloatingPointRB->isOn()=%d\n", vpage1FloatingPointRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1InputOutputRB->isOn()=%d\n", vpage1InputOutputRB->isOn());
  printf("Exit IntroWizardPanel::vpage1SetStateChanged, vpage1MPIRB->isOn()=%d\n", vpage1MPIRB->isOn());
#endif
}

void IntroWizardPanel::vSetStateChanged(QRadioButton *rb)
{

#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::vSetStateChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("Enter IntroWizardPanel::vSetStateChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("Enter IntroWizardPanel::vSetStateChanged, vpage0CreateLoadExpDataRB->isOn()=%d\n", vpage0CreateLoadExpDataRB->isOn());
  printf("Enter IntroWizardPanel::vSetStateChanged, rb=%d\n", rb);
#endif
  if ( rb != NULL) {
#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::vSetStateChanged, rb->isOn()=%d\n", rb->isOn());
#endif
  }

  vpage0SavedExpDataRB->setChecked( FALSE );
  vpage0SavedExpCompareDataRB->setChecked( FALSE );
  vpage0CreateLoadExpDataRB->setChecked( FALSE );
  if( rb != NULL ) {
    rb->setChecked(TRUE);
  }

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::vSetStateChanged, vpage0SavedExpDataRB->isOn()=%d\n", vpage0SavedExpDataRB->isOn());
  printf("Exit IntroWizardPanel::vSetStateChanged, vpage0SavedExpCompareDataRB->isOn()=%d\n", vpage0SavedExpCompareDataRB->isOn());
  printf("Exit IntroWizardPanel::vSetStateChanged, vpage0CreateLoadExpDataRB->isOn()=%d\n", vpage0CreateLoadExpDataRB->isOn());
#endif
}


void IntroWizardPanel::epage1pcSampleRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1pcSampleRBChanged, epage1pcSampleRB->isOn()=%d\n", epage1pcSampleRB->isOn());
#endif
  epage1SetStateChanged(epage1pcSampleRB);
}

void IntroWizardPanel::epage1UserTimeRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1UserTimeRBChanged, epage1UserTimeRB->isOn()=%d\n", epage1UserTimeRB->isOn());
#endif
  epage1SetStateChanged(epage1UserTimeRB);
}

void IntroWizardPanel::epage1HardwareCounterRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1HardwareCounterRBChanged, epage1HardwareCounterRB->isOn()=%d\n", epage1HardwareCounterRB->isOn());
#endif
  epage1SetStateChanged(epage1HardwareCounterRB);
}

void IntroWizardPanel::epage1HardwareCounterSampRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1HardwareCounterSampRBChanged, epage1HardwareCounterSampRB->isOn()=%d\n", epage1HardwareCounterSampRB->isOn());
#endif
  epage1SetStateChanged(epage1HardwareCounterSampRB);
}

void IntroWizardPanel::epage1FloatingPointRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1FloatingPointRBChanged, epage1FloatingPointRB->isOn()=%d\n", epage1FloatingPointRB->isOn());
#endif
  epage1SetStateChanged(epage1FloatingPointRB);
}

void IntroWizardPanel::epage1InputOutputRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1InputOutputRBChanged, epage1InputOutputRB->isOn()=%d\n", epage1InputOutputRB->isOn());
#endif
  epage1SetStateChanged(epage1InputOutputRB);
}

void IntroWizardPanel::epage1MPIRBChanged()
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1MPIRBChanged, epage1MPIRB->isOn()=%d\n", epage1MPIRB->isOn());
#endif
  epage1SetStateChanged(epage1MPIRB);
}

void IntroWizardPanel::epage1SetStateChanged(QRadioButton *rb)
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1pcSampleRB->isOn()=%d\n", epage1pcSampleRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1UserTimeRB->isOn()=%d\n", epage1UserTimeRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1HardwareCounterRB->isOn()=%d\n", epage1HardwareCounterRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1HardwareCounterSampRB->isOn()=%d\n", epage1HardwareCounterSampRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1FloatingPointRB->isOn()=%d\n", epage1FloatingPointRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1InputOutputRB->isOn()=%d\n", epage1InputOutputRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, epage1MPIRB->isOn()=%d\n", epage1MPIRB->isOn());
  printf("Enter IntroWizardPanel::epage1SetStateChanged, rb=%d\n", rb);
#endif
  if ( rb != NULL) {
#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::epage1SetStateChanged, rb->isOn()=%d\n", rb->isOn());
#endif
  }

  epage1pcSampleRB->setChecked( FALSE );
  epage1UserTimeRB->setChecked( FALSE );
  epage1HardwareCounterRB->setChecked( FALSE );
  epage1HardwareCounterSampRB->setChecked( FALSE );
  epage1FloatingPointRB->setChecked( FALSE );
  epage1InputOutputRB->setChecked( FALSE );
  epage1MPIRB->setChecked( FALSE );
  if( rb != NULL ) {
    rb->setChecked(TRUE);
  }

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1pcSampleRB->isOn()=%d\n", epage1pcSampleRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1UserTimeRB->isOn()=%d\n", epage1UserTimeRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1HardwareCounterRB->isOn()=%d\n", epage1HardwareCounterRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1HardwareCounterSampRB->isOn()=%d\n", epage1HardwareCounterSampRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1FloatingPointRB->isOn()=%d\n", epage1FloatingPointRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1InputOutputRB->isOn()=%d\n", epage1InputOutputRB->isOn());
  printf("Exit IntroWizardPanel::epage1SetStateChanged, epage1MPIRB->isOn()=%d\n", epage1MPIRB->isOn());
#endif
}

void IntroWizardPanel::eSetStateChanged(QRadioButton *rb)
{
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::eSetStateChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("Enter IntroWizardPanel::eSetStateChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
  printf("Enter IntroWizardPanel::eSetStateChanged, epage0CreateLoadExpDataRB->isOn()=%d\n", epage0CreateLoadExpDataRB->isOn());
  printf("Enter IntroWizardPanel::eSetStateChanged, rb=%d\n", rb);
#endif
  if ( rb != NULL) {
#if DEBUG_INTRO
    printf("Enter IntroWizardPanel::eSetStateChanged, rb->isOn()=%d\n", rb->isOn());
#endif
  }

  epage0SavedExpDataRB->setChecked( FALSE );
  epage0SavedExpCompareDataRB->setChecked( FALSE );
  epage0CreateLoadExpDataRB->setChecked( FALSE );

  if( rb != NULL ) {
    rb->setChecked(TRUE);
  }

#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::eSetStateChanged, epage0SavedExpDataRB->isOn()=%d\n", epage0SavedExpDataRB->isOn());
  printf("Exit IntroWizardPanel::eSetStateChanged, epage0SavedExpCompareDataRB->isOn()=%d\n", epage0SavedExpCompareDataRB->isOn());
  printf("Exit IntroWizardPanel::eSetStateChanged, epage0CreateLoadExpDataRB->isOn()=%d\n", epage0CreateLoadExpDataRB->isOn());
#endif

}

void
IntroWizardPanel::handleSizeEvent(QResizeEvent *e)
{
  int calculated_height = 0;
  int SPACER_SIZE = 20;
#if DEBUG_INTRO
  printf("Enter IntroWizardPanel::handleSizeEvent, calculated_height=%d\n", calculated_height);
#endif
  calculated_height += vpage0HelpfulLabel->height();
  calculated_height += vpage0SavedExpDataRB->height();
  calculated_height += vpage0SavedExpCompareDataRB->height();
  calculated_height += vpage0CreateLoadExpDataRB->height();

  // add in the 2 spacers..
  calculated_height += 2*20;

  // add in margins... around the children
  calculated_height += 10*6;

  int height = getPanelContainer()->parent->height();
  if( calculated_height > height )
  {
    height = calculated_height;
  }


#if DEBUG_INTRO
  printf("Exit IntroWizardPanel::handleSizeEvent, final calculated_height=%d\n", calculated_height);
#endif
}

#if 0
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
  calculated_height += vpage1HardwareCounterSampRB->height();
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
#endif

