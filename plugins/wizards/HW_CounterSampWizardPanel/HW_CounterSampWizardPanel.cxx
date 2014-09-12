////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2014 Krell Institute All Rights Reserved.
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

//
// Debug Flag
//#define DEBUG_HWCSampWizard 1
//

// This comments out the restrictions for using offline in the GUI Wizards - leave this set
#define WHEN_OFFLINE_READY 1



#include <stdio.h>
#include "HW_CounterSampWizardPanel.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "AttachProcessDialog.hxx"

#include <qapplication.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>

#include <qwidgetstack.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcombobox.h>
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
#include <qscrollview.h>

#include <qbitmap.h>
#include "rightarrow.xpm"
#include "leftarrow.xpm"

#include "LoadAttachObject.hxx"

#include "HW_CounterSampDescription.hxx"

#include "SS_Input_Manager.hxx"
#include "OpenSS_Papi_Events.h"

// Temporary default value for instrumentor is offline preference
// We need to set up a class for the preference defaults to access them across the GUI panels and wizards.
bool defaultValue_instrumentorIsOffline = TRUE;

using namespace OpenSpeedShop::Framework;

typedef std::pair<std::string, std::string> papi_preset_event;

HW_CounterSampWizardPanel::HW_CounterSampWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ("HW_CounterSampWizardPanel::HW_CounterSampWizardPanel() constructor called\n");

  original_sampling_rate = 100;
  original_papi_str = QString::null;
  if ( !getName() )
  {
	setName( "HW Counter Samp" );
  }

  // Clear out the containers for executables and pids
  if( getPanelContainer()->getMainWindow() )
  {
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw )
    {
      mw->executableName = QString::null;
      mw->pidStr = QString::null;
      mw->parallelPrefixCommandStr = QString::null;
    }
  }

  // Initialize the settings for offline before setting with actual values
  setGlobalToolInstrumentorIsOffline(FALSE);
  setThisWizardsInstrumentorIsOffline(FALSE);
  setThisWizardsPreviousInstrumentorIsOffline(FALSE);

  hwCounterSampFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  initPapiTypes();

  mainFrame = new QFrame( getBaseWidgetFrame(), "mainFrame" );
  mainFrame->setMinimumSize( QSize(10,10) );
  mainFrame->setFrameShape( QFrame::StyledPanel );
  mainFrame->setFrameShadow( QFrame::Raised );
  mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout"); 

  mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );

#if 1
  // Initialize the settings for offline before setting with actual values
  setGlobalToolInstrumentorIsOffline(defaultValue_instrumentorIsOffline);
  setThisWizardsInstrumentorIsOffline(defaultValue_instrumentorIsOffline);
  setThisWizardsPreviousInstrumentorIsOffline(defaultValue_instrumentorIsOffline);

  QSettings *settings = new QSettings();
  bool boolOK = false;
  bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline", defaultValue_instrumentorIsOffline, &boolOK);
  setGlobalToolInstrumentorIsOffline(temp_instrumentorIsOffline);

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizard setup: /openspeedshop/general/instrumentorIsOffline=(%d), boolOK=%d\n", temp_instrumentorIsOffline, boolOK );
#endif

  delete settings;

#endif

// Begin: verbose description page
  vDescriptionPageWidget = new QWidget( mainWidgetStack, "vDescriptionPageWidget" );
  vDescriptionPageLayout = new QVBoxLayout( vDescriptionPageWidget, 11, 6, "vDescriptionPageLayout"); 

  vDescriptionPageTitleLabel = new QLabel( vDescriptionPageWidget, "vDescriptionPageTitleLabel" );
  vDescriptionPageTitleLabel->setMinimumSize( QSize(0,0) );
  vDescriptionPageTitleLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vDescriptionPageTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  vDescriptionPageLayout->addWidget( vDescriptionPageTitleLabel );

  vDescriptionPageText = new QTextEdit( vDescriptionPageWidget, "vDescriptionPageText" );
  vDescriptionPageText->setReadOnly(TRUE);
  vDescriptionPageText->setMinimumSize( QSize(10,10) );
  vDescriptionPageText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vDescriptionPageText->setMinimumSize( QSize(10,10) );
  vDescriptionPageText->setWordWrap( QTextEdit::WidgetWidth );
  vDescriptionPageLayout->addWidget( vDescriptionPageText );

  vDescriptionPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vDescriptionPageButtonLayout"); 

  vwizardMode = new QCheckBox( vDescriptionPageWidget, "vwizardMode" );
  vwizardMode->setMinimumSize( QSize(10,10) );
  vwizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vwizardMode->setChecked( TRUE );
  vDescriptionPageButtonLayout->addWidget( vwizardMode );

#if 1
  // Create an exclusive button group
  QButtonGroup *vExclusiveBG = new QButtonGroup( 1, QGroupBox::Horizontal, "Instrumentation Choice", vDescriptionPageWidget);
  vDescriptionPageButtonLayout->addWidget( vExclusiveBG );
  vExclusiveBG->setExclusive( TRUE );

  // insert 2 radiobuttons
  vOnlineRB = new QRadioButton( "Use Online/Dynamic", vExclusiveBG );
  vOfflineRB = new QRadioButton( "Use Offline", vExclusiveBG );
  // Use the global preferences for the initial setting
  bool vGlobalInstrumentorIsOffline = getGlobalToolInstrumentorIsOffline();
  bool vLocalInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();

#ifdef DEBUG_HWCSampWizard
  printf("Initial Setup values for offline RADIO BUTTONS: globalInstrumentorIsOffline=(%d), localInstrumentorIsOffline=(%d)\n", vGlobalInstrumentorIsOffline, vLocalInstrumentorIsOffline );
#endif

  vOnlineRB->setChecked( !vGlobalInstrumentorIsOffline );
  vOfflineRB->setChecked( vGlobalInstrumentorIsOffline );

  // Set these as the initial values, until the Radio button checkboxes are clicked
  setThisWizardsInstrumentorIsOffline(vGlobalInstrumentorIsOffline);
  setThisWizardsPreviousInstrumentorIsOffline(vGlobalInstrumentorIsOffline);
#endif


  vDescriptionPageButtonSpacer = new QSpacerItem( 1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed );
  vDescriptionPageButtonLayout->addItem( vDescriptionPageButtonSpacer );

  vDescriptionPageIntroButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageIntroButton" );
  vDescriptionPageIntroButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageIntroButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageIntroButton );

  vDescriptionPageNextButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageNextButton" );
  vDescriptionPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageNextButton );

  vDescriptionPageFinishButton = new QPushButton( vDescriptionPageWidget, "vDescriptionPageFinishButton" );
  vDescriptionPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vDescriptionPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vDescriptionPageButtonLayout->addWidget( vDescriptionPageFinishButton );

  vDescriptionPageLayout->addLayout( vDescriptionPageButtonLayout );
  mainWidgetStack->addWidget( vDescriptionPageWidget, 0 );
// End: verbose description page

// Begin: verbose parameter page
  vParameterPageWidget = new QWidget( mainWidgetStack, "vParameterPageWidget" );
  vParameterPageWidget->setMinimumSize( QSize(10,10) );
  vParameterPageLayout = new QVBoxLayout( vParameterPageWidget, 11, 6, "vParameterPageLayout"); 

  vParameterPageDescriptionText = new QTextEdit( vParameterPageWidget, "vParameterPageDescriptionText" );
  vParameterPageDescriptionText->setReadOnly(TRUE);
//  vParameterPageDescriptionText->setMinimumSize( QSize(10,10) );

  vParameterPageDescriptionText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vParameterPageDescriptionText->setMinimumSize( QSize(10,100) );
  vParameterPageDescriptionText->setWordWrap( QTextEdit::WidgetWidth );
  vParameterPageLayout->addWidget( vParameterPageDescriptionText );

  vParameterPageLine = new QFrame( vParameterPageWidget, "vParameterPageLine" );
  vParameterPageLine->setMinimumSize( QSize(10,10) );
  vParameterPageLine->setFrameShape( QFrame::HLine );
  vParameterPageLine->setFrameShadow( QFrame::Sunken );
  vParameterPageLine->setFrameShape( QFrame::HLine );
  vParameterPageLayout->addWidget( vParameterPageLine );

  vParameterPageParameterLayout = new QVBoxLayout( 0, 0, 6, "vParameterPageParameterLayout"); 

  vParameterPageSampleRateHeaderLabel = new QLabel( vParameterPageWidget, "vParameterPageSampleRateHeaderLabel" );
//  vParameterPageSampleRateHeaderLabel->setMinimumSize( QSize(10,10) );
  vParameterPageSampleRateHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vParameterPageParameterLayout->addWidget( vParameterPageSampleRateHeaderLabel );

  vParameterPageSampleRateLayout = new QHBoxLayout( 0, 0, 6, "vParameterPageSampleRateLayout"); 

  vParameterPageSampleRateLabel = new QLabel( vParameterPageWidget, "vParameterPageSampleRateLabel" );
  vParameterPageSampleRateLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vParameterPageSampleRateLayout->addWidget( vParameterPageSampleRateLabel );

  vParameterPageSampleRateText = new QLineEdit( vParameterPageWidget, "vParameterPageSampleRateText" );
  vParameterPageSampleRateText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );

  vParameterPageSampleRateLayout->addWidget( vParameterPageSampleRateText );

  vParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Fixed );
  vParameterPageSampleRateLayout->addItem( vParameterPageSpacer );

vParameterPagePAPIDescriptionLayout = new QHBoxLayout( 0, 0, 6, "vParameterPagePAPIDescriptionLayout"); 
vParamaterPagePAPIDescriptionLabel = new QLabel( vParameterPageWidget, "vParamaterPagePAPIDescriptionLabel" );
vParamaterPagePAPIDescriptionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
vParameterPagePAPIDescriptionLayout->addWidget( vParamaterPagePAPIDescriptionLabel );

//vParameterPagePAPIDescriptionText = new QComboBox( TRUE, vParameterPageWidget, "vParameterPagePAPIDescriptionText" );
vParameterPagePAPIDescriptionText = new QListBox( vParameterPageWidget, "vParameterPagePAPIDescriptionText" );
vParameterPagePAPIDescriptionText->clearSelection();
vParameterPagePAPIDescriptionText->setSelectionMode( QListBox::Extended );

vParameterPagePAPIDescriptionText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
vParameterPagePAPIDescriptionText->setMinimumWidth( 500 );
vParameterPagePAPIDescriptionText->setMinimumHeight( 40 );
vParameterPagePAPIDescriptionLayout->addWidget( vParameterPagePAPIDescriptionText );


vParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Expanding );

//vParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Fixed );
vParameterPagePAPIDescriptionLayout->addItem( vParameterPageSpacer );

  vParameterPageParameterLayout->addLayout( vParameterPageSampleRateLayout );
  vParameterPageParameterLayout->addLayout( vParameterPagePAPIDescriptionLayout );
  vParameterPageLayout->addLayout( vParameterPageParameterLayout );


  vParameterPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vParameterPageButtonLayout"); 

  vParameterPageButtonSpacer = new QSpacerItem( 251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vParameterPageButtonLayout->addItem( vParameterPageButtonSpacer );

  vParameterPageResetButton = new QPushButton( vParameterPageWidget, "vParameterPageResetButton" );
  vParameterPageResetButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vParameterPageResetButton->sizePolicy().hasHeightForWidth() ) );
  vParameterPageButtonLayout->addWidget( vParameterPageResetButton );

  vParameterPageBackButton = new QPushButton( vParameterPageWidget, "vParameterPageBackButton" );
  vParameterPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vParameterPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vParameterPageButtonLayout->addWidget( vParameterPageBackButton );

  vParameterPageNextButton = new QPushButton( vParameterPageWidget, "vParameterPageNextButton" );
  vParameterPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vParameterPageNextButton->sizePolicy().hasHeightForWidth() ) );
  vParameterPageButtonLayout->addWidget( vParameterPageNextButton );

  vParameterPageFinishButton = new QPushButton( vParameterPageWidget, "vParameterPageFinishButton" );
  vParameterPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vParameterPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vParameterPageButtonLayout->addWidget( vParameterPageFinishButton );

  vParameterPageLayout->addLayout( vParameterPageButtonLayout );
  mainWidgetStack->addWidget( vParameterPageWidget, 1 );
// End: verbose parameter page

// Begin: verbose summary page
  vSummaryPageWidget = new QWidget( mainWidgetStack, "vSummaryPageWidget" );
  vSummaryPageWidget->setMinimumSize( QSize(10,10) );
  vSummaryPageLayout = new QVBoxLayout( vSummaryPageWidget, 11, 6, "vSummaryPageLayout"); 

  vSummaryPageLabelLayout = new QVBoxLayout( 0, 0, 6, "vSummaryPageLabelLayout"); 
  vSummaryPageFinishLabel = new QTextEdit( vSummaryPageWidget, "vSummaryPageFinishLabel" );
  vSummaryPageFinishLabel->setReadOnly(TRUE);
  vSummaryPageFinishLabel->setMinimumSize( QSize(10,10) );
  vSummaryPageFinishLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred, 0, 0, FALSE ) );
  vSummaryPageLabelLayout->addWidget( vSummaryPageFinishLabel );
  vSummaryPageLayout->addLayout( vSummaryPageLabelLayout );

  vSummaryPageButtonLayout = new QHBoxLayout( 0, 0, 6, "vSummaryPageButtonLayout"); 

  vSummaryPageButtonSpacer = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  vSummaryPageButtonLayout->addItem( vSummaryPageButtonSpacer );
  vSummaryPageBackButton = new QPushButton( vSummaryPageWidget, "vSummaryPageBackButton" );
  vSummaryPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vSummaryPageBackButton->sizePolicy().hasHeightForWidth() ) );
  vSummaryPageButtonLayout->addWidget( vSummaryPageBackButton );

  vSummaryPageFinishButton = new QPushButton( vSummaryPageWidget, "vSummaryPageFinishButton" );
  vSummaryPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, vSummaryPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  vSummaryPageButtonLayout->addWidget( vSummaryPageFinishButton );
  vSummaryPageLayout->addLayout( vSummaryPageButtonLayout );
  mainWidgetStack->addWidget( vSummaryPageWidget, 3 );
// End: verbose summary page

// The advanced (expert) wording starts here....
// Begin: advance (expert) description page
  eDescriptionPageWidget = new QWidget( mainWidgetStack, "eDescriptionPageWidget" );
  eDescriptionPageLayout = new QVBoxLayout( eDescriptionPageWidget, 11, 6, "eDescriptionPageLayout"); 

  eDescriptionPageTitleLabel = new QLabel( eDescriptionPageWidget, "eDescriptionPageTitleLabel" );
  eDescriptionPageTitleLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, eDescriptionPageTitleLabel->sizePolicy().hasHeightForWidth() ) );
  eDescriptionPageTitleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
  eDescriptionPageLayout->addWidget( eDescriptionPageTitleLabel );

  eDescriptionPageText = new QLabel( eDescriptionPageWidget, "eDescriptionPageText" );
  eDescriptionPageText->setMinimumSize( QSize(10,10) );
  eDescriptionPageLayout->addWidget( eDescriptionPageText );
  eDescriptionPageButtonLayout = new QHBoxLayout( 0, 0, 6, "eDescriptionPageButtonLayout"); 

  ewizardMode = new QCheckBox( eDescriptionPageWidget, "ewizardMode" );
  ewizardMode->setMinimumSize( QSize(10,10) );
  ewizardMode->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  ewizardMode->setChecked( FALSE );
  eDescriptionPageButtonLayout->addWidget( ewizardMode );

#if 1
  // Create an exclusive button group
  QButtonGroup *eExclusiveBG = new QButtonGroup( 1, QGroupBox::Horizontal, "Instrumentation Choice", eDescriptionPageWidget);
  eDescriptionPageButtonLayout->addWidget( eExclusiveBG );
  eExclusiveBG->setExclusive( TRUE );

  // insert 2 radiobuttons
  eOnlineRB = new QRadioButton( "Use Online/Dynamic", eExclusiveBG );
  eOfflineRB = new QRadioButton( "Use Offline", eExclusiveBG );
  // Use the global preferences for the initial setting
  bool eGlobalInstrumentorIsOffline = getGlobalToolInstrumentorIsOffline();
  bool eLocalInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();

#ifdef DEBUG_HWWizard
  printf("eDescriptionPageButtonLayout, Initial Setup values for offline RADIO BUTTONS: eGlobalInstrumentorIsOffline=(%d), eLocalInstrumentorIsOffline=(%d)\n", eGlobalInstrumentorIsOffline, eLocalInstrumentorIsOffline );
#endif

  eOnlineRB->setChecked( !eGlobalInstrumentorIsOffline );
  eOfflineRB->setChecked( eGlobalInstrumentorIsOffline );

  // Set these as the initial values, until the Radio button checkboxes are clicked
  setThisWizardsInstrumentorIsOffline(eGlobalInstrumentorIsOffline);
  setThisWizardsPreviousInstrumentorIsOffline(eGlobalInstrumentorIsOffline);
#endif

  eDescriptionPageSpacer = new QSpacerItem( 1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  eDescriptionPageButtonLayout->addItem( eDescriptionPageSpacer );
  eDescriptionPageIntroButton = new QPushButton( eDescriptionPageWidget, "eDescriptionPageIntroButton" );
  eDescriptionPageIntroButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eDescriptionPageIntroButton->sizePolicy().hasHeightForWidth() ) );
  eDescriptionPageButtonLayout->addWidget( eDescriptionPageIntroButton );

  eDescriptionPageNextButton = new QPushButton( eDescriptionPageWidget, "eDescriptionPageNextButton" );
  eDescriptionPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eDescriptionPageNextButton->sizePolicy().hasHeightForWidth() ) );
  eDescriptionPageButtonLayout->addWidget( eDescriptionPageNextButton );

  eDescriptionPageFinishButton = new QPushButton( eDescriptionPageWidget, "eDescriptionPageFinishButton" );
  eDescriptionPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eDescriptionPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  eDescriptionPageButtonLayout->addWidget( eDescriptionPageFinishButton );

  eDescriptionPageLayout->addLayout( eDescriptionPageButtonLayout );
  mainWidgetStack->addWidget( eDescriptionPageWidget, 4 );
// End: advance (expert) description page

// Begin: advance (expert) parameter page
  eParameterPageWidget = new QWidget( mainWidgetStack, "eParameterPageWidget" );
  eParameterPageWidget->setMinimumSize( QSize(10,10) );
  eParameterPageLayout = new QVBoxLayout( eParameterPageWidget, 11, 6, "eParameterPageLayout"); 

  eParameterPageDescriptionLabel = new QTextEdit( eParameterPageWidget, "eParameterPageDescriptionLabel" );
  eParameterPageDescriptionLabel->setReadOnly(TRUE);
  eParameterPageDescriptionLabel->setMinimumSize( QSize(10,10) );
  eParameterPageDescriptionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  eParameterPageLayout->addWidget( eParameterPageDescriptionLabel );
  eParameterPageLine = new QFrame( eParameterPageWidget, "eParameterPageLine" );
  eParameterPageLine->setMinimumSize( QSize(10,10) );
  eParameterPageLine->setFrameShape( QFrame::HLine );
  eParameterPageLine->setFrameShadow( QFrame::Sunken );
  eParameterPageLine->setFrameShape( QFrame::HLine );
  eParameterPageLayout->addWidget( eParameterPageLine );

  eParameterPageParameterLayout = new QVBoxLayout( 0, 0, 6, "eParameterPageParameterLayout"); 


  eParameterPageSampleRateHeaderLabel = new QLabel( eParameterPageWidget, "eParameterPageSampleRateHeaderLabel" );
//  eParameterPageSampleRateHeaderLabel->setMinimumSize( QSize(10,10) );
  eParameterPageSampleRateHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eParameterPageParameterLayout->addWidget( eParameterPageSampleRateHeaderLabel );

  eParameterPageSampleRateLayout = new QHBoxLayout( 0, 0, 6, "eParameterPageSampleRateLayout"); 

  eParameterPageSampleRateLabel = new QLabel( eParameterPageWidget, "eParameterPageSampleRateLabel" );
  eParameterPageSampleRateLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eParameterPageSampleRateLayout->addWidget( eParameterPageSampleRateLabel );

  eParameterPageSampleRateText = new QLineEdit( eParameterPageWidget, "eParameterPageSampleRateText" );
  eParameterPageSampleRateText->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eParameterPageSampleRateLayout->addWidget( eParameterPageSampleRateText );

  eParameterPageSpacer = new QSpacerItem( 20, 1, QSizePolicy::Preferred, QSizePolicy::Fixed );
  eParameterPageSampleRateLayout->addItem( eParameterPageSpacer );

eParameterPagePAPIDescriptionLayout = new QHBoxLayout( 0, 0, 6, "eParameterPagePAPIDescriptionLayout"); 
eParamaterPagePAPIDescriptionLabel = new QLabel( eParameterPageWidget, "eParamaterPagePAPIDescriptionLabel" );
eParamaterPagePAPIDescriptionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
eParameterPagePAPIDescriptionLayout->addWidget( eParamaterPagePAPIDescriptionLabel );

//eParameterPagePAPIDescriptionText = new QComboBox( TRUE, eParameterPageWidget, "eParameterPagePAPIDescriptionText" );
eParameterPagePAPIDescriptionText = new QListBox( eParameterPageWidget, "eParameterPagePAPIDescriptionText" );
eParameterPagePAPIDescriptionText->clearSelection();
eParameterPagePAPIDescriptionText->setSelectionMode( QListBox::Extended );
eParameterPagePAPIDescriptionText->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
eParameterPagePAPIDescriptionLayout->addWidget( eParameterPagePAPIDescriptionText );

eParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Fixed );
eParameterPagePAPIDescriptionLayout->addItem( eParameterPageSpacer );

  eParameterPageParameterLayout->addLayout( eParameterPageSampleRateLayout );
eParameterPageParameterLayout->addLayout( eParameterPagePAPIDescriptionLayout );
  eParameterPageLayout->addLayout( eParameterPageParameterLayout );
  eParameterPageButtonLayout = new QHBoxLayout( 0, 0, 6, "eParameterPageButtonLayout"); 

  eParameterPageButtonSpacer = new QSpacerItem( 251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  eParameterPageButtonLayout->addItem( eParameterPageButtonSpacer );

  eParameterPageResetButton = new QPushButton( eParameterPageWidget, "eParameterPageResetButton" );
  eParameterPageResetButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eParameterPageResetButton->sizePolicy().hasHeightForWidth() ) );
  eParameterPageButtonLayout->addWidget( eParameterPageResetButton );

  eParameterPageBackButton = new QPushButton( eParameterPageWidget, "eParameterPageBackButton" );
  eParameterPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eParameterPageBackButton->sizePolicy().hasHeightForWidth() ) );
  eParameterPageButtonLayout->addWidget( eParameterPageBackButton );

  eParameterPageNextButton = new QPushButton( eParameterPageWidget, "eParameterPageNextButton" );
  eParameterPageNextButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eParameterPageNextButton->sizePolicy().hasHeightForWidth() ) );
  eParameterPageButtonLayout->addWidget( eParameterPageNextButton );

  eParameterPageFinishButton = new QPushButton( eParameterPageWidget, "eParameterPageFinishButton" );
  eParameterPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eParameterPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  eParameterPageButtonLayout->addWidget( eParameterPageFinishButton );

  eParameterPageLayout->addLayout( eParameterPageButtonLayout );
  mainWidgetStack->addWidget( eParameterPageWidget, 5 );
// End: advanced (exper) parameter page

// Begin: advance (expert) summary page
  eSummaryPageWidget = new QWidget( mainWidgetStack, "eSummaryPageWidget" );
  eSummaryPageWidget->setMinimumSize( QSize(10,10) );
  eSummaryPageLayout = new QVBoxLayout( eSummaryPageWidget, 11, 6, "eSummaryPageLayout"); 

  eSummaryPageFinishLabel = new QTextEdit( eSummaryPageWidget, "eSummaryPageFinishLabel" );
  eSummaryPageFinishLabel->setReadOnly(TRUE);
  eSummaryPageFinishLabel->setMinimumSize( QSize(10,10) );
  eSummaryPageFinishLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  eSummaryPageLayout->addWidget( eSummaryPageFinishLabel );

  eSummaryPageButtonLayout = new QHBoxLayout( 0, 0, 6, "eSummaryPageButtonLayout"); 

  eSummaryPageButtonSpacer = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  eSummaryPageButtonLayout->addItem( eSummaryPageButtonSpacer );
  eSummaryPageBackButton = new QPushButton( eSummaryPageWidget, "eSummaryPageBackButton" );
  eSummaryPageBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eSummaryPageBackButton->sizePolicy().hasHeightForWidth() ) );
  eSummaryPageButtonLayout->addWidget( eSummaryPageBackButton );

  eSummaryPageFinishButton = new QPushButton( eSummaryPageWidget, "eSummaryPageFinishButton" );
  eSummaryPageFinishButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, eSummaryPageFinishButton->sizePolicy().hasHeightForWidth() ) );
  eSummaryPageButtonLayout->addWidget( eSummaryPageFinishButton );
  eSummaryPageLayout->addLayout( eSummaryPageButtonLayout );
  mainWidgetStack->addWidget( eSummaryPageWidget, 7 );
  mainFrameLayout->addWidget( mainWidgetStack );
  hwCounterSampFormLayout->addWidget( mainFrame );
// End: advance (expert) summary page


  languageChange();
  resize( QSize(631, 508).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

    // signals and slots connections
  connect( eDescriptionPageNextButton, SIGNAL( clicked() ), this,
           SLOT( eDescriptionPageNextButtonSelected() ) );
  connect( eDescriptionPageIntroButton, SIGNAL( clicked() ), this,
           SLOT( eDescriptionPageIntroButtonSelected() ) );
  connect( eParameterPageBackButton, SIGNAL( clicked() ), this,
           SLOT( eParameterPageBackButtonSelected() ) );
  connect( eParameterPageNextButton, SIGNAL( clicked() ), this,
           SLOT( eParameterPageNextButtonSelected() ) );
  connect( eParameterPageResetButton, SIGNAL( clicked() ), this,
           SLOT( eParameterPageResetButtonSelected() ) );
  connect( eParameterPageSampleRateText, SIGNAL( returnPressed() ), this,
           SLOT( eParameterPageSampleRateTextReturnPressed() ) );

  connect( eSummaryPageBackButton, SIGNAL( clicked() ), this,
           SLOT( eSummaryPageBackButtonSelected() ) );
  connect( eSummaryPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( eSummaryPageFinishButtonSelected() ) );
  connect( vDescriptionPageNextButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageNextButtonSelected() ) );
  connect( vDescriptionPageIntroButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageIntroButtonSelected() ) );
  connect( vParameterPageSampleRateText, SIGNAL( returnPressed() ), this,
           SLOT( vParameterPageSampleRateTextReturnPressed() ) );
  connect( vParameterPageBackButton, SIGNAL( clicked() ), this,
           SLOT( vParameterPageBackButtonSelected() ) );
  connect( vParameterPageResetButton, SIGNAL( clicked() ), this,
           SLOT( vParameterPageResetButtonSelected() ) );
  connect( vParameterPageNextButton, SIGNAL( clicked() ), this,
           SLOT( vParameterPageNextButtonSelected() ) );

  connect( vSummaryPageBackButton, SIGNAL( clicked() ), this,
           SLOT( vSummaryPageBackButtonSelected() ) );
  connect( vSummaryPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( vSummaryPageFinishButtonSelected() ) );
  connect( vwizardMode, SIGNAL( clicked() ), this,
           SLOT( vwizardModeSelected() ) );
  connect( ewizardMode, SIGNAL( clicked() ), this,
           SLOT( ewizardModeSelected() ) );

#if 1
  connect( vOfflineRB, SIGNAL( clicked() ), this,
           SLOT( vOfflineRBSelected() ) );
  connect( vOnlineRB, SIGNAL( clicked() ), this,
           SLOT( vOnlineRBSelected() ) );

  connect( eOfflineRB, SIGNAL( clicked() ), this,
           SLOT( eOfflineRBSelected() ) );
  connect( eOnlineRB, SIGNAL( clicked() ), this,
           SLOT( eOnlineRBSelected() ) );
#endif


  connect( eDescriptionPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );
  connect( eParameterPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );

  connect( vDescriptionPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );
  connect( vParameterPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( finishButtonSelected() ) );

  // Set this wizards load panel to NULL.  It will be set
  // when created and used when needed during the wizard execution.
  setThisWizardsLoadPanel(NULL);

  if( ao && ao->int_data == 0 )
  {
    // This wizard panel was brought up explicitly.   Don't
    // enable the hook to go back to the IntroWizardPanel.
    vDescriptionPageIntroButton->hide();
    eDescriptionPageIntroButton->hide();
  }

// This is way ugly and only a temporary hack to get a handle on the 
// parent hwCounterSampPanel's hook.    
// This should only be > 1 when we're calling this wizard from within
// a hwCounterSampPanel session to help the user load an executable.
  hwCounterSampPanel = NULL;
//  if( (int)argument > 1 )
  if( ao && ao->panel_data != NULL )
  {
    hwCounterSampPanel = (Panel *)ao->panel_data;
  }
}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
HW_CounterSampWizardPanel::~HW_CounterSampWizardPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  HW_CounterSampWizardPanel::~HW_CounterSampWizardPanel() destructor called\n");
}

//! Add user panel specific menu items if they have any.
bool
HW_CounterSampWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::menu() requested.\n");

  return( FALSE );
}

static void createTokens(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ")
{
    /* Skip delimiters at beginning. */
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    /* Find first "non-delimiter". */
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        /* Found a token, add it to the vector. */
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        /* Skip delimiters.  Note the "not_of" */
        lastPos = str.find_first_not_of(delimiters, pos);

        /* Find next "non-delimiter" */
        pos = str.find_first_of(delimiters, lastPos);

#ifdef DEBUG_CLI
        printf("createTokens, in while, str.c_str()=%s, lastPos = %d, pos = %d\n", str.c_str(), lastPos, pos);
#endif

    }
}


//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
HW_CounterSampWizardPanel::save()
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
HW_CounterSampWizardPanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
HW_CounterSampWizardPanel::listener(void *msg)
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::listener() requested.\n");
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::listener() requested.\n");
#endif

  MessageObject *messageObject = (MessageObject *)msg;
  nprintf(DEBUG_PANELS) ("  HW_CounterSampWizardPanel::listener, messageObject->msgType = %s\n", messageObject->msgType.ascii() );
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::listener,  messageObject->msgType = %s\n", messageObject->msgType.ascii() );
#endif
  if( messageObject->msgType == getName() )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    eSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageBackButton->setEnabled(TRUE);
    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_LoadPanel_Back_Page" )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    eSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageBackButton->setEnabled(TRUE);
    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    nprintf(DEBUG_PANELS) ("vDescriptionPageWidget\n");
    if( vwizardMode->isOn() && !ewizardMode->isOn() )
    {// is it verbose?
      mainWidgetStack->raiseWidget(vParameterPageWidget);
    } else {
      mainWidgetStack->raiseWidget(eParameterPageWidget);
    }

    // Hide the load panel as we just came back from it.
    // It may be less confusing to hide it than to leave it up.
    // Another usability study may provide feedback

    QString name = "loadPanel";

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, try to find panel (%s)\n", name.ascii() );
#endif

//    Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    Panel *loadPanel = getThisWizardsLoadPanel();

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, loadPanel=0x%x\n", loadPanel);
#endif

    if( loadPanel ) {

#ifdef DEBUG_HWCSampWizard
      printf("HW_CounterSampWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, Found the loadPanel... Try to hide it.\n");
#endif

      loadPanel->getPanelContainer()->hidePanel(loadPanel);
    }

    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_First_Page" )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    eSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageBackButton->setEnabled(TRUE);
    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    nprintf(DEBUG_PANELS) ("vDescriptionPageWidget\n");
    if( vwizardMode->isOn() && !ewizardMode->isOn() )
    {// is it verbose?
      mainWidgetStack->raiseWidget(vDescriptionPageWidget);
    } else
    {
      mainWidgetStack->raiseWidget(eDescriptionPageWidget);
    }
    return 1;
  }
  if( messageObject->msgType == "Wizard_Raise_Summary_Page" )
  {
    vSummaryPageFinishButton->setEnabled(TRUE);
    eSummaryPageFinishButton->setEnabled(TRUE);
    vSummaryPageBackButton->setEnabled(TRUE);
    eSummaryPageBackButton->setEnabled(TRUE);
    qApp->flushX();
    
#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::listener, Wizard_Raise_Summary_Page if block \n");
#endif
    vPrepareForSummaryPage();
    return 1;
  }

  if( messageObject->msgType == "PreferencesChangedObject" ) {

   bool temp_instrumentorIsOffline = getToolPreferenceInstrumentorIsOffline();
#ifdef DEBUG_HWCSampWizard
   printf("HWCWizard::listener, PREFERENCE-CHANGED-OBJECT temp_instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
    return 1;

 }

  return 0;  // 0 means, did not want this message and did not act on anything.
}


//! This function broadcasts messages.
int 
HW_CounterSampWizardPanel::broadcast(char *msg)
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::broadcast() requested.\n");
  return 0;
}


bool HW_CounterSampWizardPanel::getToolPreferenceInstrumentorIsOffline()
{
  QSettings *settings = new QSettings();
  bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline");
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::getToolPreferenceInstrumentorIsOffline, /openspeedshop/general/instrumentorIsOffline == instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
  delete settings;
}

void HW_CounterSampWizardPanel::vOfflineRBSelected()
{

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = vOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
#endif

#if WHEN_OFFLINE_READY
   // The else won't be here when offline is ready to be run inside the GUI
#else
   if (offlineCheckBoxValue) {

      int answer_val =  QMessageBox::Ok;

      answer_val =  QMessageBox::question( this, tr("Feature In Progress"), tr("The ability to run offline experiments in the GUI is under construction.\n\nPlease run offline experiments using the:\n    openss -offline -f <executable> experiment_type\nsyntax, outside of the GUI.\n\nYou may use the online feature to create your experiment inside the GUI.\nSorry for the inconveinence.\n "),
       QMessageBox::Ok );

       if( answer_val ==  QMessageBox::Ok ) {
          setThisWizardsInstrumentorIsOffline(false);
          vOnlineRB->setChecked( true );
          vOfflineRB->setChecked( false );
//          printf("in CustomExperimentPanel::listener(), offlineCheckBoxValue=%d\n", offlineCheckBoxValue);
        }

    }
#endif

#if WHEN_OFFLINE_READY
  setThisWizardsInstrumentorIsOffline(offlineCheckBoxValue);
#else
  setThisWizardsInstrumentorIsOffline(false);
#endif

}

void HW_CounterSampWizardPanel::vOnlineRBSelected()
{
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = vOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}


void HW_CounterSampWizardPanel::eOfflineRBSelected()
{
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = eOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
#endif
#if WHEN_OFFLINE_READY
   // The else won't be here when offline is ready to be run inside the GUI
#else
   if (offlineCheckBoxValue) {

      int answer_val =  QMessageBox::Ok;
  
      answer_val =  QMessageBox::question( this, tr("Feature In Progress"), tr("The ability to run offline experiments in the GUI is under construction.\n\nPlease run offline experiments using the:\n    openss -offline -f <executable> experiment_type\nsyntax, outside of the GUI.\n\nYou may use the online feature to create your experiment inside the GUI.\nSorry for the inconveinence.\n "),
       QMessageBox::Ok );

       if( answer_val ==  QMessageBox::Ok ) {
          setThisWizardsInstrumentorIsOffline(false);
          vOnlineRB->setChecked( true );
          vOfflineRB->setChecked( false );
//          printf("in CustomExperimentPanel::listener(), offlineCheckBoxValue=%d\n", offlineCheckBoxValue);
        }

    }
#endif

#if WHEN_OFFLINE_READY
  setThisWizardsInstrumentorIsOffline(offlineCheckBoxValue);
#else
  setThisWizardsInstrumentorIsOffline(false);
#endif

}

void HW_CounterSampWizardPanel::eOnlineRBSelected()
{
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = eOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}

#if 0
void HW_CounterSampWizardPanel::instrumentorIsOfflineModeSelected()
{
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::instrumentorIsOfflineModeSelected() entered.\n");
#endif
  bool checkBoxValue = instrumentorIsOfflineMode->isOn();
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::instrumentorIsOfflineModeSelected() checkBoxValue=(%d)\n", checkBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(checkBoxValue);
}
#endif



void HW_CounterSampWizardPanel::vwizardModeSelected()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
  wizardModeSelected();
  if( raisedWidget == vDescriptionPageWidget )
  {
      nprintf(DEBUG_PANELS) ("vDescriptionPageWidget\n");
      mainWidgetStack->raiseWidget(eDescriptionPageWidget);
  } else if( raisedWidget ==  vParameterPageWidget )
  {
      nprintf(DEBUG_PANELS) ("vParameterPageWidget\n");
      mainWidgetStack->raiseWidget(eParameterPageWidget);
  } else if( raisedWidget == vSummaryPageWidget )
  {
      nprintf(DEBUG_PANELS) ("vSummaryPageWidget\n");
      mainWidgetStack->raiseWidget(eSummaryPageWidget);
  } else
  {
      nprintf(DEBUG_PANELS) ("Expert to Verbose: unknown WStackPage\n");
  }
  vwizardMode->setChecked( FALSE );
}

void HW_CounterSampWizardPanel::ewizardModeSelected()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
  wizardModeSelected();
  if( raisedWidget  == eDescriptionPageWidget )
  {
      nprintf(DEBUG_PANELS) ("eDescriptionPageWidget\n");
      mainWidgetStack->raiseWidget(vDescriptionPageWidget);
  } else if( raisedWidget == eParameterPageWidget )
  {
      nprintf(DEBUG_PANELS) ("eParameterPageWidget\n");
      mainWidgetStack->raiseWidget(vParameterPageWidget);
  } else if( raisedWidget == eSummaryPageWidget )
  {
      nprintf(DEBUG_PANELS) ("eSummaryPageWidget\n");
      mainWidgetStack->raiseWidget(vSummaryPageWidget);
  } else
  {
      nprintf(DEBUG_PANELS) ("Verbose to Expert: unknown WStackPage\n");
  }
}


void HW_CounterSampWizardPanel::wizardModeSelected()
{
  QWidget *raisedWidget = mainWidgetStack->visibleWidget();
  if( raisedWidget == vDescriptionPageWidget )
  {
    nprintf(DEBUG_PANELS) ("vDescriptionPageWidget\n");
  } else if( raisedWidget ==  vParameterPageWidget )
  {
    nprintf(DEBUG_PANELS) ("vParameterPageWidget\n");
  } else if( raisedWidget == vSummaryPageWidget )
  {
    nprintf(DEBUG_PANELS) ("vSummaryPageWidget\n");
  } else if( raisedWidget  == eDescriptionPageWidget )
  {
    nprintf(DEBUG_PANELS) ("eDescriptionPageWidget\n");
  } else if( raisedWidget == eParameterPageWidget )
  {
    nprintf(DEBUG_PANELS) ("eParameterPageWidget\n");
  } else if( raisedWidget == eSummaryPageWidget )
  {
    nprintf(DEBUG_PANELS) ("eSummaryPageWidget\n");
  }

  ewizardMode->setChecked( FALSE );
}

Panel* HW_CounterSampWizardPanel::findAndRaiseLoadPanel()
{
  // Try to raise the load panel if there is one hidden

#if 1

  Panel *p = getThisWizardsLoadPanel();

  if (getThisWizardsInstrumentorIsOffline() == getThisWizardsPreviousInstrumentorIsOffline() ) {

#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
  } else {
#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, SET P NULL, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
     // create a new loadPanel
     p = NULL;
  }

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, found thisWizardsLoadPanel - now raising, p=0x%x\n", p);
#endif
  if (p) {
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }

#else

  QString name = QString("loadPanel");

  Panel *p = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if (p) {

#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, found loadPanel - now raising, p=%x\n", p);
#endif

     p->getPanelContainer()->raisePanel(p);

  } else {

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif

  }

#endif

  return p;
}

void HW_CounterSampWizardPanel::eDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eDescriptionPageNextButtonSelected() \n");
  mainWidgetStack->raiseWidget(eParameterPageWidget);
}

void HW_CounterSampWizardPanel::eDescriptionPageIntroButtonSelected()
{
  getPanelContainer()->hidePanel((Panel *)this);

  nprintf(DEBUG_PANELS) ("eDescriptionPageIntroButtonSelected() \n");

  Panel *p = getPanelContainer()->raiseNamedPanel((char *) "Intro Wizard");
  if( !p )
  {
    getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Intro Wizard", getPanelContainer(), NULL );
  }
}

void HW_CounterSampWizardPanel::eParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageBackButtonSelected() \n");
  mainWidgetStack->raiseWidget(eDescriptionPageWidget);
}

void HW_CounterSampWizardPanel::eParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageNextButtonSelected() \n");

  samplingRate = eParameterPageSampleRateText->text();
  vParameterPageSampleRateText->setText(eParameterPageSampleRateText->text());

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;

  } else {

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel, calling loadNewProgramPanel()\n");
#endif
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw ) {
     mw->executableName = QString((const char *)0);
     mw->argsStr = QString((const char *)0);
     mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel calling mw->loadNewProgramPanel, this=0x%x\n", this );
#endif

#ifdef DEBUG_HWCSampWizard
     printf("HWCWizardPanel calling mw->loadNewProgramPanel, this=0x%x, getThisWizardsInstrumentorIsOffline()=%d \n", 
             this, getThisWizardsInstrumentorIsOffline() );
#endif

     Panel* p = mw->loadNewProgramPanel(getPanelContainer(), 
                                        getPanelContainer()->getMasterPC(), 
                                        /* expID */-1, (Panel *) 
                                        this, 
                                        getThisWizardsInstrumentorIsOffline());
     setThisWizardsLoadPanel(p);

     QString executableNameStr = mw->executableName;
     if( !mw->executableName.isEmpty() ) {
#ifdef DEBUG_HWCSampWizard
      printf("HW_CounterSampWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif
     } else {
#ifdef DEBUG_HWCSampWizard
      printf("HW_CounterSampWizardPanel, executableName is empty\n" );
#endif
     } // end if clause for empty executable name
    } // end if clause for mw
  } // end else clause for create new load panel
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);

}

void HW_CounterSampWizardPanel::eParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageResetButtonSelected() \n");
  vParameterPageSampleRateText->setText(QString("%1").arg(original_sampling_rate));
  eParameterPageSampleRateText->setText(QString("%1").arg(original_sampling_rate));
  //vParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);
  //jeg int i = 0;
  //jeg while( ++i <= 25 )
  //jeg   vParameterPagePAPIDescriptionText->insertItem( QString::fromLatin1( "Item " ) + QString::number( i ), i );

  //jeg eParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);
}

void HW_CounterSampWizardPanel::eSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageBackButtonSelected() \n");

// JUST RAISE EXISTING PANEL if one is there.

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::eSummaryPageBackButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::eSummaryPageBackButtonSelected, creating loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);

    if (p) {

#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::eSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }

}

void HW_CounterSampWizardPanel::eSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageFinishButtonSelected() \n");

  vSummaryPageFinishButtonSelected();

}

void HW_CounterSampWizardPanel::vDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageNextButtonSelected() \n");

  mainWidgetStack->raiseWidget(vParameterPageWidget);
}

void HW_CounterSampWizardPanel::vDescriptionPageIntroButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageIntroButtonSelected() \n");

  getPanelContainer()->hidePanel((Panel *)this);

  Panel *p = getPanelContainer()->raiseNamedPanel((char *) "Intro Wizard");
  if( !p )
  {
    nprintf(DEBUG_PANELS) ("vDescriptionPageIntroButtonSelected() create a new one!\n");
    getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Intro Wizard", getPanelContainer(), NULL);
  }
}

void HW_CounterSampWizardPanel::vParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("vParameterPageSampleRateTextReturnPressed() \n");
}

void HW_CounterSampWizardPanel::eParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("eParameterPageSampleRateTextReturnPressed() \n");
}

void HW_CounterSampWizardPanel::vParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageBackButtonSelected() \n");

  mainWidgetStack->raiseWidget(vDescriptionPageWidget);
}

void HW_CounterSampWizardPanel::vParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageNextButtonSelected() \n");

  samplingRate = vParameterPageSampleRateText->text();
  vParameterPageSampleRateText->setText(vParameterPageSampleRateText->text());

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_HWCSampWizard
   printf("HW_CounterSampWizardPanel, calling loadNewProgramPanel()\n");
#endif
   OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
   if( mw ) {
    mw->executableName = QString((const char *)0);
    mw->argsStr = QString((const char *)0);
    mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel -2- calling mw->loadNewProgramPanel, this=0x%x, getThisWizardsInstrumentorIsOffline()=%d \n", this, getThisWizardsInstrumentorIsOffline()  );
#endif

    Panel* p = mw->loadNewProgramPanel(getPanelContainer(), 
                                       getPanelContainer()->getMasterPC(), 
                                       /* expID */-1, 
                                       (Panel *) this, 
                                       getThisWizardsInstrumentorIsOffline() );
    setThisWizardsLoadPanel(p);
    QString executableNameStr = mw->executableName;

    if( !mw->executableName.isEmpty() ) {

#ifdef DEBUG_HWCSampWizard
      printf("HW_CounterSampWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif

    } else {

#ifdef DEBUG_HWCSampWizard
      printf("HW_CounterSampWizardPanel, executableName is empty\n" );
#endif

    }
   }
  }
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);
}

void HW_CounterSampWizardPanel::vParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageResetButtonSelected() \n");
  vParameterPageSampleRateText->setText(QString("%1").arg(original_sampling_rate));
  eParameterPageSampleRateText->setText(QString("%1").arg(original_sampling_rate));
  //vParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);
  //jeg int i = 0;
  //jeg while( ++i <= 25 )
  //jeg   vParameterPagePAPIDescriptionText->insertItem( QString::fromLatin1( "Item " ) + QString::number( i ), i );
  //jeg eParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);
}

void HW_CounterSampWizardPanel::vPrepareForSummaryPage()
{
  nprintf(DEBUG_PANELS) ("HW_CounterSampWizardPanel::vPrepareForSummaryPage() \n");
#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vPrepareForSummaryPage() \n");
#endif

  char buffer[2048];

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  } 

  // Find selected items
  QString PAPI_str = "\"";
  bool first_time = true;
  for ( unsigned int i = 0; i < vParameterPagePAPIDescriptionText->count(); i++ ) {
      QListBoxItem *item = vParameterPagePAPIDescriptionText->item( i );
      // if the item is selected...
      if ( item->isSelected() ) {
          
#ifdef DEBUG_HWCSampWizard
          printf("THIS ITEM IS SELECTED=(%s)\n", item->text().left(0).ascii());
#endif
        if (first_time) {
          first_time = false;
        } else {
          PAPI_str.append(",");
        }
        int left_string_len = item->text().find(' ');
        PAPI_str.append(QString(item->text().left(left_string_len)).ascii());
      }
  }
  PAPI_str.append("\"");

#ifdef DEBUG_HWCSampWizard
  printf("PAPI_str=(%s)\n", PAPI_str.ascii() );
#endif

  // Set this for future push_back in the Finish page
  selected_papi_events_str = PAPI_str;


  if( !mw->pidStr.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    sprintf(buffer, "<p align=\"left\">You've selected a HW CounterSamp experiment for process \"%s\" running on host \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%s\".<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"HWC\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->pidStr.ascii(), mw->hostStr.ascii(), vParameterPageSampleRateText->text().ascii() );
  } else if( !mw->executableName.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    if( mw->parallelPrefixCommandStr.isEmpty() || mw->parallelPrefixCommandStr.isNull() ) {

      if (getThisWizardsInstrumentorIsOffline()) {

         sprintf(buffer, "<p align=\"left\">You've selected a HW CounterSamp experiment for executable \"%s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%s\"<br>using offline instrumentation.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"hwCounterSamp\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), selected_papi_events_str.ascii(), vParameterPageSampleRateText->text().ascii() );
      } else {
         sprintf(buffer, "<p align=\"left\">You've selected a HW CounterSamp experiment for executable \"%s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%s\"<br>using online/dynamic instrumentation.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"hwCounterSamp\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), selected_papi_events_str.ascii(), vParameterPageSampleRateText->text().ascii() );
      } 

    } else {
      if (getThisWizardsInstrumentorIsOffline()) {
        sprintf(buffer, "<p align=\"left\">You've selected a HW CounterSamp experiment for command/executable<br>\"%s %s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%s\"<br>using offline instrumentation.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"hwCounterSamp\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), selected_papi_events_str.ascii(), vParameterPageSampleRateText->text().ascii() );
      } else {
        sprintf(buffer, "<p align=\"left\">You've selected a HW CounterSamp experiment for command/executable<br>\"%s %s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%s\"<br>using online/dynamic instrumentation.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"hwCounterSamp\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), selected_papi_events_str.ascii(), vParameterPageSampleRateText->text().ascii() );
      }

    }
  }

  vSummaryPageFinishLabel->setText( tr( buffer ) );
  mainWidgetStack->raiseWidget(2);
  mainWidgetStack->raiseWidget(vSummaryPageWidget);

  QString name = "loadPanel";
#ifdef DEBUG_HWCSampWizard
  printf("try to find panel (%s)\n", name.ascii() );
#endif
//  Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  Panel *loadPanel = getThisWizardsLoadPanel();

#ifdef DEBUG_HWCSampWizard
  printf("HW_CounterSampWizardPanel::vPrepareForSummaryPage(), loadPanel=0x%x\n", loadPanel);
#endif

  if( loadPanel ) {

#ifdef DEBUG_HWCSampWizard
     printf("Found the loadPanel... Try to hide it.\n");
#endif

     loadPanel->getPanelContainer()->hidePanel(loadPanel);

  }

}

void HW_CounterSampWizardPanel::vSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageBackButtonSelected() \n");

// RAISE EXISTING PANEL if one is there and request the second page of the wizard
// be focused/raised.

  Panel *p = findAndRaiseLoadPanel();

  if (p) {

#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {
#ifdef DEBUG_HWCSampWizard
    printf("HW_CounterSampWizardPanel::vSummaryPageBackButtonSelected, did not find loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);
    if (p) {
#ifdef DEBUG_HWCSampWizard
     printf("HW_CounterSampWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif
     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }

}

void HW_CounterSampWizardPanel::finishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("finishButtonSelected() \n");

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( mw->executableName.isEmpty() && mw->pidStr.isEmpty() )
  {
    if( vwizardMode->isOn() )
    {
      vSummaryPageFinishButtonSelected();
    } else
    {
      vSummaryPageFinishButtonSelected();
    }
  } else
  {
    vSummaryPageFinishButtonSelected();
  }
}


void HW_CounterSampWizardPanel::vSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageFinishButtonSelected() \n");
#ifdef DEBUG_HWCSampWizard
  printf("vSummaryPageFinishButtonSelected() \n");
#endif

  Panel *p = hwCounterSampPanel;
  if( getPanelContainer()->getMainWindow() )
  { 
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw ) {
      LoadAttachObject *lao = NULL;
      ParamList *paramList = new ParamList();
      paramList->push_back(vParameterPageSampleRateText->text() );
      bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();

#ifdef DEBUG_HWCSampWizard
      printf("vSummaryPageFinishButtonSelected(), A: sampling rate, push_back (%s), localInstrumentorIsOffline=%d\n", vParameterPageSampleRateText->text().ascii(), localInstrumentorIsOffline );
#endif

#ifdef DEBUG_HWCSampWizard
      printf("selected_papi_events_str=(%s)\n", selected_papi_events_str.ascii() );
#endif

      paramList->push_back(selected_papi_events_str);
#ifdef DEBUG_HWCSampWizard
      printf("vSummaryPageFinishButtonSelected(), A: papi events, push_back (%s)\n", selected_papi_events_str.ascii() );
#endif

      if( !mw->executableName.isEmpty() ) {

        // The offline flag indicates to the custom experiment panel that the experiment is using offline instrumentation.
        lao = new LoadAttachObject(mw->executableName, (char *)NULL, mw->parallelPrefixCommandStr, paramList, TRUE, localInstrumentorIsOffline);

      } else if( !mw->pidStr.isEmpty() ) {

#ifdef DEBUG_HWCSampWizard
        printf("vSummaryPageFinishButtonSelected(), A: pid was specified.\n");
#endif
        lao = new LoadAttachObject((char *)NULL, mw->pidStr, (char *)NULL, paramList, TRUE, localInstrumentorIsOffline);

      } else {
// printf("Warning: No attach or load parameters available.\n");
      }
      if( lao != NULL ) {
        vSummaryPageFinishButton->setEnabled(FALSE);
        eSummaryPageFinishButton->setEnabled(FALSE);
        vSummaryPageBackButton->setEnabled(FALSE);
        eSummaryPageBackButton->setEnabled(FALSE);
        qApp->flushX();

        if( !p ) {
          ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
          ao->lao = lao;
          ao->isInstrumentorOffline = localInstrumentorIsOffline;
#ifdef DEBUG_HWCSampWizard
          printf("HW_CounterSampleWizardPanel::vSummaryPageBackButtonSelected, creating HWC Sampling experiment panel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif

          p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("HWCSamp Panel", getPanelContainer(), ao);

          delete ao;
        } else {
          p->listener((void *)lao);
        }

//        getPanelContainer()->hidePanel((Panel *)this);
// The receiving routine should delete this...
// delete paramList;
      }
    }
  }

}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
HW_CounterSampWizardPanel::languageChange()
{

// printf("HW_CounterSampWizardPanel::languageChange() entered\n");


  unsigned int sampling_rate = original_sampling_rate;
  std::string PAPIDescriptionStr;;
// printf("HW_CounterSampWizardPanel::languageChange(A) entered\n");

  setCaption( tr( "HW CounterSampling - Wizard Panel" ) );
  vDescriptionPageTitleLabel->setText( tr( "<h1>HW CounterSampling - Wizard</h1>" ) );
  vDescriptionPageText->setText( tr( vHW_CounterSampDescription ) );
  vDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( vDescriptionPageIntroButton, tr( "Takes you back to the second page of the Intro Wizard so you can make a different selection." ) );
  vDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  vDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vDescriptionPageFinishButton->setEnabled(FALSE);

 QString papi_preset_event_strings = "<pre>";
 for(std::vector<papi_preset_event>::const_iterator it = papi_available_presets.begin(); it != papi_available_presets.end(); ++it)
  {
    papi_preset_event_strings += QString("%1   %2<br>").arg(it->first.c_str()).arg(it->second.c_str());
  }
  papi_preset_event_strings += "</pre>";

  vParameterPageDescriptionText->setText( tr( QString("The following options (parameters) are available to adjust.   These are the options the collector has exported.<br><br>\n"
"The smaller the number used for the sampling rate, the more\n"
"HW CounterSamp detail will be shown.   However, the trade off will be slower\n"
"performance and a larger data file.<br><br>\n"
"It may take a little experimenting to find the right setting for your \n"
"particular executable.   We suggest starting with the default setting\n"
"of %1.\n\n"
"<br><br>You may choose up to six (6) hardware counter events to\n"
"read when the sample is taken. Not all hardware counters are available together.\n"
"Some conflict because they share internal hardware resources.\n\n"
"WARNING: OpenSpeedShop does not currently verify that the events you choose will work together.\nWe suggest using papi_event_chooser to check for compatible combinations before\n"
"selection below. In general combinations that are valid will have to pass a test:\n\n"
"<br>     papi_event_chooser PRESET event1 event2 .. eventn<br>\n\n"
"The output for an valid/acceptable combination will contain this:\n\n"
"<br>   event_chooser.c                          PASSED<br>\n\n"
"\nThe available PAPI events that can be set are:<br>%2<br> ").arg(sampling_rate).arg(papi_preset_event_strings) ) );
  vParameterPageSampleRateHeaderLabel->setText( tr( "You can set the following option(s):" ) );
  vParameterPageSampleRateLabel->setText( tr( "Sampling rate (in samples per second):" ) );
  vParameterPageSampleRateText->setText( tr( QString("%1").arg(sampling_rate) ) );
  QToolTip::add( vParameterPageSampleRateText, tr( QString("The number of times to sampling before logging location.   (Default %1.)").arg(sampling_rate) ) );

  vParamaterPagePAPIDescriptionLabel->setText( tr( "PAPI String:" ) );
  appendComboBoxItems();
  QToolTip::add( vParameterPagePAPIDescriptionText, tr( QString("The HW CounterSamp one wants to leverage - Default Total Cycles: PAPI_TOT_CYC") ));

  vParameterPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vParameterPageBackButton, tr( "Takes you back one page." ) );
  vParameterPageResetButton->setText( tr( "Reset" ) );
  QToolTip::add( vParameterPageResetButton, tr( "Reset the values to the default setings." ) );
  vParameterPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vParameterPageNextButton, tr( "Advance to the next wizard page." ) );
  vParameterPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vParameterPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vParameterPageFinishButton->setEnabled(FALSE);


  vSummaryPageFinishLabel->setText( tr( "<p align=\"left\">\n"
"You've selected a HW CounterSamp experiment for executable \"%s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%d\".<br><br>To complete the exeriment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"hwCounterSamp\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>" ) );
  vSummaryPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vSummaryPageBackButton, tr( "Takes you back one page." ) );
  vSummaryPageFinishButton->setText( tr( "Finish..." ) );
  QToolTip::add( vSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"hwCounterSamp\" panel" ) );
  eDescriptionPageTitleLabel->setText( tr( "<h1>HW CounterSamp Wizard</h1>" ) );
  eDescriptionPageText->setText( tr( eHW_CounterSampDescription ) );
  eDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( eDescriptionPageIntroButton, tr( "Takes you back to the second page of the Intro Wizard so you can make a different selection." ) );
  eDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( eDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  eDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( eDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  eDescriptionPageFinishButton->setEnabled(FALSE);

  eParameterPageDescriptionLabel->setText( tr( "The following options (parameters) are available to adjust.     <br><br>These are the options the collector has exported.<br>%1" ).arg(papi_preset_event_strings) );
  eParameterPageSampleRateHeaderLabel->setText( tr( "You can set the following option(s):" ) );
  eParameterPageSampleRateLabel->setText( tr( "Sampling rate (in samples per second):" ) );
  eParameterPageSampleRateText->setText( tr( QString("%1").arg(sampling_rate) ) );
  QToolTip::add( eParameterPageSampleRateText, tr( QString("The number of times an sampling occurs before taking sample.   (Default %1.)").arg(sampling_rate) ) );

  eParamaterPagePAPIDescriptionLabel->setText( tr( "PAPI String:" ) );
  QToolTip::add( eParameterPagePAPIDescriptionText, tr( QString("The HW CounterSamp one wants to leverage - Default Total Cycles: PAPI_TOT_CYC") ));

  eParameterPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( eParameterPageBackButton, tr( "Takes you back one page." ) );
  eParameterPageResetButton->setText( tr( "Reset" ) );
  QToolTip::add( eParameterPageResetButton, tr( "Reset the values to the default setings." ) );
  eParameterPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( eParameterPageNextButton, tr( "Advance to the next wizard page." ) );
  eParameterPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( eParameterPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  eParameterPageFinishButton->setEnabled(FALSE);

  eSummaryPageFinishLabel->setText( tr( "<p align=\"left\">\n"
"You've selected a HW CounterSamp experiment for executable \"%s\" to be run on host \"%s\" tracing \"%s\".<br>Furthermore, you've chosen a sampling rate of \"%d\".<br><br></p>" ) );
  eSummaryPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( eSummaryPageBackButton, tr( "Takes you back one page." ) );
  eSummaryPageFinishButton->setText( tr( "Finish..." ) );
  QToolTip::add( eSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"HW CounterSamp\" panel" ) );
  vwizardMode->setText( tr( "Verbose Wizard Mode" ) );
  ewizardMode->setText( tr( "Verbose Wizard Mode" ) );

  // Look up default metrics.   There's only one in this case.
  // Get list of all the collectors from the FrameWork.
  // To do this, we need to create a dummy experiment.
  try {
    char *temp_name = tmpnam(NULL);
//    static std::string tmpdb = std::string(temp_name);
    std::string tmpdb = std::string(temp_name);
    OpenSpeedShop::Framework::Experiment::create (tmpdb);
    OpenSpeedShop::Framework::Experiment dummy_experiment(tmpdb);

    // Is there a hw experiment type?
    bool found_one = FALSE;
    std::set<Metadata> collectortypes = Collector::getAvailable();
    for( std::set<Metadata>::const_iterator mi = collectortypes.begin();
         mi != collectortypes.end(); mi++ )
    {
      if( mi->getUniqueId() == "hwcsamp" )
      {
        found_one = TRUE;
      }
    }
    if( found_one == FALSE )
    {
      return;
    }

    Collector hwCounterSampCollector = dummy_experiment.createCollector("hwcsamp");

    Metadata cm = hwCounterSampCollector.getMetadata();
      std::set<Metadata> md =hwCounterSampCollector.getParameters();
      std::set<Metadata>::const_iterator mi;
      for (mi = md.begin(); mi != md.end(); mi++) {
        Metadata m = *mi;
#ifdef DEBUG_HWCSampWizard
        printf("%s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
        printf("%s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
        printf("%s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );
#endif
      }
      hwCounterSampCollector.getParameterValue("sampling_rate", sampling_rate);
#ifdef DEBUG_HWCSampWizard
      printf("sampling_rate=%d\n", sampling_rate);
      printf("Initialize the text fields... (%d)\n", sampling_rate);
#endif
      vParameterPageSampleRateText->setText(QString("%1").arg(sampling_rate));
      eParameterPageSampleRateText->setText(QString("%1").arg(sampling_rate));
      original_sampling_rate = sampling_rate;
      hwCounterSampCollector.getParameterValue("event", PAPIDescriptionStr);
#ifdef DEBUG_HWCSampWizard
      printf("event==(PAPIDescriptionStr.c_str())=%s\n", PAPIDescriptionStr.c_str() );
#endif

      std::string lastToken;
      std::vector<std::string> tokens;
      createTokens(PAPIDescriptionStr.c_str(), tokens, ",");
      std::vector<std::string>::iterator k;

      for (k=tokens.begin();k != tokens.end(); k++) {
          lastToken = *k;
#ifdef DEBUG_HWCSampWizard
          printf("in for, lastToken.c_str()=%s\n", lastToken.c_str());
#endif
      }

      original_papi_str = findPAPIStr(QString(lastToken.c_str()));
//      original_papi_str = findPAPIStr(QString(PAPIDescriptionStr.c_str()));
      //vParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);
      //jeg int i = 0;
      //jeg while( ++i <= 25 )
      //jeg   vParameterPagePAPIDescriptionText->insertItem( QString::fromLatin1( "Item " ) + QString::number( i ), i );
      //jeg eParameterPagePAPIDescriptionText->setCurrentText(original_papi_str);

    if( temp_name )
    {
      (void) remove( temp_name );
    }

  }
  catch(const std::exception& error)
  {
    return;
  }

}

void
HW_CounterSampWizardPanel::initPapiTypes()
{
  papi_available_presets.clear();
  std::pair<std::string, std::string> *pe;

  for(unsigned i = 0; OpenSS_Papi_ALLEvents[i]  != NULL; ++i) {
	std::string ev = OpenSS_Papi_ALLEvents[i];

	char delimitor = ':';
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end =  ev.find_first_of(delimitor,start)) != std::string::npos) {
	pe = new std::pair<std::string, std::string>
				(std::string(ev,start,end-start),
				 std::string(ev,end+1,std::string::npos));
	papi_available_presets.push_back(*pe);
	
	start = end+1;
	}
  }


/*
printf("initPapiTypes() entered\n");

  for(std::vector<papi_preset_event>::const_iterator it = papi_available_presets.begin(); it != papi_available_presets.end(); ++it)
  {
    printf("%s   %s\n", it->first.c_str(), it->second.c_str() );
  }
*/

}

void
HW_CounterSampWizardPanel::appendComboBoxItems()
{
  for(std::vector<papi_preset_event>::const_iterator it = papi_available_presets.begin(); it != papi_available_presets.end(); ++it)
  {
    QString t = QString("%1  %2").arg(it->first.c_str()).arg(it->second.c_str());
#ifdef DEBUG_HWCSampWizard
    printf("PAPI presets arg first and arg second:%s   %s\n", it->first.c_str(), it->second.c_str() );
#endif
    
    vParameterPagePAPIDescriptionText->insertItem(t);
    eParameterPagePAPIDescriptionText->insertItem(t);
  }
  QListBoxItem* v_tot_cyc_item = vParameterPagePAPIDescriptionText->findItem("PAPI_TOT_CYC");
  QListBoxItem* e_tot_cyc_item = eParameterPagePAPIDescriptionText->findItem("PAPI_TOT_CYC");
  int v_tot_cyc_index = vParameterPagePAPIDescriptionText->index(v_tot_cyc_item);
  int e_tot_cyc_index = eParameterPagePAPIDescriptionText->index(e_tot_cyc_item);
#ifdef DEBUG_HWCSampWizard
  printf("v_tot_cyc_index=%d\n", v_tot_cyc_index);
  printf("e_tot_cyc_index=%d\n", e_tot_cyc_index);
#endif
  QListBoxItem* v_tot_ins_item = vParameterPagePAPIDescriptionText->findItem("PAPI_TOT_INS");
  QListBoxItem* e_tot_ins_item = eParameterPagePAPIDescriptionText->findItem("PAPI_TOT_INS");
  int v_tot_ins_index = vParameterPagePAPIDescriptionText->index(v_tot_ins_item);
  int e_tot_ins_index = eParameterPagePAPIDescriptionText->index(e_tot_ins_item);
#ifdef DEBUG_HWCSampWizard
  printf("v_tot_ins_index=%d\n", v_tot_ins_index);
  printf("e_tot_ins_index=%d\n", e_tot_ins_index);
#endif
  vParameterPagePAPIDescriptionText->setSelected(v_tot_cyc_item, TRUE);
  vParameterPagePAPIDescriptionText->setSelected(v_tot_ins_item, TRUE);
  eParameterPagePAPIDescriptionText->setSelected(e_tot_cyc_item, TRUE);
  eParameterPagePAPIDescriptionText->setSelected(e_tot_ins_item, TRUE);

}

QString
HW_CounterSampWizardPanel::findPAPIStr(QString param)
{
#ifdef DEBUG_HWCSampWizard
  printf("findPAPIStr() param=(%s)\n", param.ascii() );
#endif

  for(std::vector<papi_preset_event>::const_iterator it = papi_available_presets.begin(); it != papi_available_presets.end(); ++it)
  {
#ifdef DEBUG_HWCSampWizard
    printf("it->first.c_str()=%s it->second.c_str()=%s\n", it->first.c_str(), it->second.c_str() );
#endif
    if( param == QString(it->first.c_str()) )
    {
      return( QString("%1  %2").arg(it->first.c_str()).arg(it->second.c_str()) );
    }
  }
#ifdef DEBUG_HWCSampWizard
  printf("findPAPIStr() there are no presets!!!\n");
#endif
  QMessageBox::information( this, "PAPI preset information:", "There are no PAPI preset strings available..", QMessageBox::Ok );

  return(QString::null);
}
