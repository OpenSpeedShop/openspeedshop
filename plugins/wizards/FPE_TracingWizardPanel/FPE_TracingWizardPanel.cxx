////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007, 2008 Krell Institute All Rights Reserved.
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
//#define DEBUG_FPEWizard 1
//
// This comments out the restrictions for using offline in the GUI Wizards - leave this set
#define WHEN_OFFLINE_READY 1

#include <stdio.h>
#include "FPE_TracingWizardPanel.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "AttachProcessDialog.hxx"

#define MAXROWS 8
#define MAXCOLUMNS 8

#include <qapplication.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
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

#include <qrect.h>

#include "LoadAttachObject.hxx"

#include "FPE_TracingDescription.hxx"

#include "SS_Input_Manager.hxx"

// Temporary default value for instrumentor is offline preference
// We need to set up a class for the preference defaults to access them across the GUI panels and wizards.
bool defaultValue_instrumentorIsOffline = TRUE;

using namespace OpenSpeedShop::Framework;

FPE_TracingWizardPanel::FPE_TracingWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ("FPE_TracingWizardPanel::FPE_TracingWizardPanel() constructor called\n");
  if ( !getName() )
  {
	setName( "FPE_Tracing" );
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

  paramList.clear();

#if 1
  // Initialize the settings for offline before setting with actual values
  setGlobalToolInstrumentorIsOffline(defaultValue_instrumentorIsOffline);
  setThisWizardsInstrumentorIsOffline(defaultValue_instrumentorIsOffline);
  setThisWizardsPreviousInstrumentorIsOffline(defaultValue_instrumentorIsOffline);

  QSettings *settings = new QSettings();
  bool boolOK = false;
  bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline", defaultValue_instrumentorIsOffline, &boolOK);
  setGlobalToolInstrumentorIsOffline(temp_instrumentorIsOffline);
#ifdef DEBUG_FPEWizard
  printf("FPE_Wizard setup: /openspeedshop/general/instrumentorIsOffline=(%d), boolOK=%d\n", temp_instrumentorIsOffline, boolOK );
#endif
  delete settings;
#endif

  fpeFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  mainFrame = new QFrame( getBaseWidgetFrame(), "mainFrame" );
  mainFrame->setMinimumSize( QSize(10,10) );
  mainFrame->setFrameShape( QFrame::StyledPanel );
  mainFrame->setFrameShadow( QFrame::Raised );
  mainFrameLayout = new QVBoxLayout( mainFrame, 11, 6, "mainFrameLayout"); 

  mainWidgetStack = new QWidgetStack( mainFrame, "mainWidgetStack" );
  mainWidgetStack->setMinimumSize( QSize(10,10) );

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

#ifdef DEBUG_FPEWizard
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
  vParameterPageDescriptionText->setMinimumSize( QSize(10,10) );

  vParameterPageDescriptionText->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum, 0, 0, FALSE ) );
  vParameterPageDescriptionText->setMinimumSize( QSize(10,10) );
  vParameterPageDescriptionText->setWordWrap( QTextEdit::WidgetWidth );
  vParameterPageLayout->addWidget( vParameterPageDescriptionText );


  vParameterPageLine = new QFrame( vParameterPageWidget, "vParameterPageLine" );
  vParameterPageLine->setMinimumSize( QSize(10,10) );
  vParameterPageLine->setFrameShape( QFrame::HLine );
  vParameterPageLine->setFrameShadow( QFrame::Sunken );
  vParameterPageLine->setFrameShape( QFrame::HLine );
  vParameterPageLayout->addWidget( vParameterPageLine );

  vParameterPageParameterLayout = new QVBoxLayout( 0, 0, 6, "vParameterPageParameterLayout"); 

  vParameterTraceCheckBox = new QCheckBox( vParameterPageWidget, "vParameterTraceComboBox" );
  vParameterTraceCheckBox->setText(tr("Gather additional information for each FPE call. (fpet)") );
  vParameterPageParameterLayout->addWidget( vParameterTraceCheckBox );
vParameterTraceCheckBox->hide();
  QToolTip::add( vParameterTraceCheckBox, tr( "Records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );

  vParameterPageFunctionListHeaderLabel = new QLabel( vParameterPageWidget, "vParameterPageFunctionListHeaderLabel" );
  vParameterPageFunctionListHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  vParameterPageParameterLayout->addWidget( vParameterPageFunctionListHeaderLabel );


  vParameterPageFunctionListLayout = new QVBoxLayout( 0, 0, 6, "vParameterPageFunctionListLayout");

  sv = new QScrollView( vParameterPageWidget, "scrollView" );
  big_box_w = new QWidget(sv->viewport(), "big_box(viewport)" );
  big_box_w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  const QColor color = vParameterTraceCheckBox->paletteBackgroundColor();
  sv->viewport()->setBackgroundColor(color);
  // sv->viewport()->setPaletteBackgroundColor(color);
  sv->addChild(big_box_w);
  vParameterPageFunctionListLayout->addWidget( sv );

  // For debugging layout
  // big_box_w->setBackgroundColor("Red");


  QHBoxLayout *glayout = new QHBoxLayout( big_box_w, 0, 6, "glayout");

  vParameterPageFunctionListGridLayout = new QGridLayout( glayout, MAXROWS, MAXCOLUMNS, 3, "vParameterPageFunctionListGridLayout"); 

//  vParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Fixed );
  vParameterPageSpacer = new QSpacerItem( 400, 30, QSizePolicy::Preferred, QSizePolicy::Expanding );
  vParameterPageFunctionListLayout->addItem( vParameterPageSpacer );

  vParameterPageParameterLayout->addLayout( vParameterPageFunctionListLayout );
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

#ifdef DEBUG_FPEWizard
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

  eParameterTraceCheckBox = new QCheckBox( eParameterPageWidget, "eParameterTraceComboBox" );
  eParameterTraceCheckBox->setText(tr("Gather additional information for each FPE function call.") );
  eParameterPageParameterLayout->addWidget( eParameterTraceCheckBox );
eParameterTraceCheckBox->hide();
  QToolTip::add( eParameterTraceCheckBox, tr( "Records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );


  eParameterPageFunctionListHeaderLabel = new QLabel( eParameterPageWidget, "eParameterPageFunctionListHeaderLabel" );
  eParameterPageFunctionListHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eParameterPageParameterLayout->addWidget( eParameterPageFunctionListHeaderLabel );


  eParameterPageFunctionListLayout = new QHBoxLayout( 0, 0, 6, "eParameterPageFunctionListLayout"); 

  eParameterPageFunctionListGridLayout = new QGridLayout( eParameterPageFunctionListLayout, MAXROWS, MAXCOLUMNS, 3, "eParameterPageFunctionListGridLayout"); 

  eParameterPageSpacer = new QSpacerItem( 20, 1, QSizePolicy::Preferred, QSizePolicy::Fixed );
  eParameterPageFunctionListLayout->addItem( eParameterPageSpacer );

  eParameterPageParameterLayout->addLayout( eParameterPageFunctionListLayout );
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
  fpeFormLayout->addWidget( mainFrame );
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

  connect( eSummaryPageBackButton, SIGNAL( clicked() ), this,
           SLOT( eSummaryPageBackButtonSelected() ) );
  connect( eSummaryPageFinishButton, SIGNAL( clicked() ), this,
           SLOT( eSummaryPageFinishButtonSelected() ) );
  connect( vDescriptionPageNextButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageNextButtonSelected() ) );
  connect( vDescriptionPageIntroButton, SIGNAL( clicked() ), this,
           SLOT( vDescriptionPageIntroButtonSelected() ) );
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
// parent fpePanel's hook.    
// This should only be > 1 when we're calling this wizard from within
// a fpePanel session to help the user load an executable.
  fpePanel = NULL;
//  if( (int)argument > 1 )
  if( ao && ao->panel_data != NULL )
  {
    fpePanel = (Panel *)ao->panel_data;
  }

// end debug

}

//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
FPE_TracingWizardPanel::~FPE_TracingWizardPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  FPE_TracingWizardPanel::~FPE_TracingWizardPanel() destructor called\n");
}

//! Add user panel specific menu items if they have any.
bool
FPE_TracingWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
FPE_TracingWizardPanel::save()
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
FPE_TracingWizardPanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
FPE_TracingWizardPanel::listener(void *msg)
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::listener() requested.\n");
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::listener() requested.\n");
#endif

  MessageObject *messageObject = (MessageObject *)msg;
  nprintf(DEBUG_PANELS) ("  FPE_TracingWizardPanel::listener, messageObject->msgType = %s\n", messageObject->msgType.ascii() );
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::listener,  messageObject->msgType = %s\n", messageObject->msgType.ascii() );
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
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, try to find panel (%s)\n", name.ascii() );
#endif
//    Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    Panel *loadPanel = getThisWizardsLoadPanel();
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, loadPanel=0x%x\n", loadPanel);
#endif
    if( loadPanel ) {
#ifdef DEBUG_FPEWizard
      printf("FPE_TracingWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, Found the loadPanel... Try to hide it.\n");
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
    
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::listener, Wizard_Raise_Summary_Page if block \n");
#endif
    vPrepareForSummaryPage();
    return 1;
  }

#if 1
  if( messageObject->msgType == "PreferencesChangedObject" ) {

   bool temp_instrumentorIsOffline = getToolPreferenceInstrumentorIsOffline();
#ifdef DEBUG_FPEWizard
   printf("FPEWizard::listener, PREFERENCE-CHANGED-OBJECT temp_instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
    return 1;

 }
#endif

  return 0;  // 0 means, did not want this message and did not act on anything.
}

//! This function broadcasts messages.
int 
FPE_TracingWizardPanel::broadcast(char *msg)
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::broadcast() requested.\n");
  return 0;
}

bool FPE_TracingWizardPanel::getToolPreferenceInstrumentorIsOffline()
{
  QSettings *settings = new QSettings();
  bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline");
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::getToolPreferenceInstrumentorIsOffline, /openspeedshop/general/instrumentorIsOffline == instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
  delete settings;
}

void FPE_TracingWizardPanel::vOfflineRBSelected()
{
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = vOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
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

void FPE_TracingWizardPanel::vOnlineRBSelected()
{
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = vOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}

void FPE_TracingWizardPanel::eOfflineRBSelected()
{
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = eOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
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

void FPE_TracingWizardPanel::eOnlineRBSelected()
{
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = eOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}

#if 0
void FPE_TracingWizardPanel::instrumentorIsOfflineModeSelected()
{
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::instrumentorIsOfflineModeSelected() entered.\n");
#endif
  bool checkBoxValue = instrumentorIsOfflineMode->isOn();
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::instrumentorIsOfflineModeSelected() checkBoxValue=(%d)\n", checkBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(checkBoxValue);
}
#endif


void FPE_TracingWizardPanel::vwizardModeSelected()
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

void FPE_TracingWizardPanel::ewizardModeSelected()
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


void FPE_TracingWizardPanel::wizardModeSelected()
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

#if 1
Panel* FPE_TracingWizardPanel::findAndRaiseLoadPanel()
{
  // Try to raise the load panel if there is one hidden
#if 1

  Panel *p = getThisWizardsLoadPanel();
  if (getThisWizardsInstrumentorIsOffline() == getThisWizardsPreviousInstrumentorIsOffline() ) {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
  } else {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, SET P NULL, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
     // create a new loadPanel
     p = NULL;
  }

#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, found thisWizardsLoadPanel - now raising, p=%x\n", p);
  if (p) {
    printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, p->getName()=%s\n", p->getName() );
  }
#endif

  if (p) {
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }

#else

  QString name = QString("loadPanel");
  Panel *p = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if (p) {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, found loadPanel - now raising, p=%x\n", p);
     if (p) {
       printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, found loadPanel, p->getName()=%s\n", p->getName() );
     }
#endif
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }
#endif

  return p;
}

#else
Panel* FPE_TracingWizardPanel::findAndRaiseLoadPanel()
{
  // Try to raise the load panel if there is one hidden
#if 1
  Panel *p = getThisWizardsLoadPanel();
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, found thisWizardsLoadPanel - now raising, p=0x%x\n", p);
#endif
  if (p) {
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }

#else

  QString name = QString("loadPanel");
  Panel *p = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if (p) {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, found loadPanel - now raising, p=%x\n", p);
#endif
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }
#endif

  return p;
}

#endif

void FPE_TracingWizardPanel::eDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eDescriptionPageNextButtonSelected() \n");
  mainWidgetStack->raiseWidget(eParameterPageWidget);
}

void FPE_TracingWizardPanel::eDescriptionPageIntroButtonSelected()
{
  getPanelContainer()->hidePanel((Panel *)this);

  nprintf(DEBUG_PANELS) ("eDescriptionPageIntroButtonSelected() \n");

  Panel *p = getPanelContainer()->raiseNamedPanel((char *) "Intro Wizard");
  if( !p )
  {
    getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Intro Wizard", getPanelContainer(), NULL );
  }
}

void FPE_TracingWizardPanel::eParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageBackButtonSelected() \n");
  mainWidgetStack->raiseWidget(eDescriptionPageWidget);
}

void FPE_TracingWizardPanel::eParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageNextButtonSelected() \n");

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;

  } else {

#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel, calling loadNewProgramPanel()\n");
#endif
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw ) {
     mw->executableName = QString((const char *)0);
     mw->argsStr = QString((const char *)0);
     mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel calling mw->loadNewProgramPanel, this=0x%x\n", this );
#endif

     bool tmp_is_offline = false;
     Panel* p = mw->loadNewProgramPanel(getPanelContainer(), getPanelContainer()->getMasterPC(), /* expID */-1, (Panel *) this, tmp_is_offline);
     setThisWizardsLoadPanel(p);

     QString executableNameStr = mw->executableName;
     if( !mw->executableName.isEmpty() ) {
#ifdef DEBUG_FPEWizard
      printf("FPE_TracingWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif
     } else {
#ifdef DEBUG_FPEWizard
      printf("FPE_TracingWizardPanel, executableName is empty\n" );
#endif
     } // end if clause for empty executable name
    } // end if clause for mw
  } // end else clause for create new load panel
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);
}

void FPE_TracingWizardPanel::eParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageResetButtonSelected() \n");
}

void FPE_TracingWizardPanel::eSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageBackButtonSelected() \n");

// JUST RAISE EXISTING PANEL if one is there.

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::eSummaryPageBackButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPan::eSummaryPageBackButtonSelected, creating loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n",
            getThisWizardsInstrumentorIsOffline());
#endif
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);

    if (p) {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::eSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif
     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }
}

void FPE_TracingWizardPanel::eSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageFinishButtonSelected() \n");

  vSummaryPageFinishButtonSelected();

}

void FPE_TracingWizardPanel::vDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageNextButtonSelected() \n");


  mainWidgetStack->raiseWidget(vParameterPageWidget);
}

void FPE_TracingWizardPanel::vDescriptionPageIntroButtonSelected()
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

void FPE_TracingWizardPanel::vParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("vParameterPageSampleRateTextReturnPressed() \n");
}

void FPE_TracingWizardPanel::eParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("eParameterPageSampleRateTextReturnPressed() \n");
}

void FPE_TracingWizardPanel::vParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageBackButtonSelected() \n");

  mainWidgetStack->raiseWidget(vDescriptionPageWidget);
}

void FPE_TracingWizardPanel::vParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageNextButtonSelected() \n");

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_FPEWizard
   printf("FPE_TracingWizardPanel, calling loadNewProgramPanel()\n");
#endif
   OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
   if( mw ) {
    mw->executableName = QString((const char *)0);
    mw->argsStr = QString((const char *)0);
    mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel calling mw->loadNewProgramPanel, this=0x%x\n", this );
#endif

#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel -2- calling mw->loadNewProgramPanel, this=0x%x, getThisWizardsInstrumentorIsOffline()=%d \n", this, getThisWizardsInstrumentorIsOffline()  );
#endif

    Panel* p = mw->loadNewProgramPanel(getPanelContainer(), getPanelContainer()->getMasterPC(), /* expID */-1, (Panel *) this, getThisWizardsInstrumentorIsOffline());
    setThisWizardsLoadPanel(p);
    QString executableNameStr = mw->executableName;
    if( !mw->executableName.isEmpty() ) {
#ifdef DEBUG_FPEWizard
      printf("FPE_TracingWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif
    } else {
#ifdef DEBUG_FPEWizard
      printf("FPE_TracingWizardPanel, executableName is empty\n" );
#endif
    }
   }
  }
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);

}

void FPE_TracingWizardPanel::vPrepareForSummaryPage()
{
  nprintf(DEBUG_PANELS) ("FPE_TracingWizardPanel::vPrepareForSummaryPage() \n");
#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vPrepareForSummaryPage() \n");
#endif

  char buffer[2048];

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw )
  {
    return;
  } 

  if( !mw->pidStr.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);
    sprintf(buffer, "<p align=\"left\">You've selected a FPE experiment for process \"%s\" running on host \"%s\".<br>Furthermore, you've chosen to monitor \"%s\" fpe functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"fpe\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->pidStr.ascii(), mw->hostStr.ascii(), paramString.ascii() );

  } else if( !mw->executableName.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    if (mw->parallelPrefixCommandStr.isEmpty() || mw->parallelPrefixCommandStr.isEmpty() ) {
        if (getThisWizardsInstrumentorIsOffline()) {
          sprintf(buffer, "<p align=\"left\">You've selected a FPE experiment for executable \"%s\" to be run on host \"%s\"<br>using offline instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" fpe functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"fpe\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        } else {
          sprintf(buffer, "<p align=\"left\">You've selected a FPE experiment for executable \"%s\" to be run on host \"%s\"<br>using online/dynamic instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" fpe functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"fpe\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        }


    } else {

        if (getThisWizardsInstrumentorIsOffline()) {
           sprintf(buffer, "<p align=\"left\">You've selected a FPE experiment for command/executable <br>\"%s %s\" to be run on host \"%s\"<br>using offline instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" fpe functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"fpe\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        } else {
           sprintf(buffer, "<p align=\"left\">You've selected a FPE experiment for command/executable <br>\"%s %s\" to be run on host \"%s\"<br>using online/dynamic instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" fpe functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"fpe\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        }


    }
  }

  vSummaryPageFinishLabel->setText( tr( buffer ) );
  mainWidgetStack->raiseWidget(2);
  mainWidgetStack->raiseWidget(vSummaryPageWidget);

  QString name = "loadPanel";
#ifdef DEBUG_FPEWizard
  printf("try to find panel (%s)\n", name.ascii() );
#endif
//  Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  Panel *loadPanel = getThisWizardsLoadPanel();

#ifdef DEBUG_FPEWizard
  printf("FPE_TracingWizardPanel::vPrepareForSummaryPage(), loadPanel=0x%x\n", loadPanel);
#endif
  if( loadPanel ) {
#ifdef DEBUG_FPEWizard
     printf("Found the loadPanel=0x%x... Try to hide it.\n", loadPanel);
#endif
     loadPanel->getPanelContainer()->hidePanel(loadPanel);
  }

}

void FPE_TracingWizardPanel::vParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageResetButtonSelected() \n");
}

void FPE_TracingWizardPanel::vSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageBackButtonSelected() \n");

// RAISE EXISTING PANEL if one is there and request the second page of the wizard
// be focused/raised.

  Panel *p = findAndRaiseLoadPanel();

  if (p) {

#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {
#ifdef DEBUG_FPEWizard
    printf("FPE_TracingWizardPanel::vSummaryPageBackButtonSelected, did not find loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);
    if (p) {
#ifdef DEBUG_FPEWizard
     printf("FPE_TracingWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif
     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }
}

void FPE_TracingWizardPanel::finishButtonSelected()
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


void FPE_TracingWizardPanel::vSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageFinishButtonSelected() \n");
// printf("vSummaryPageFinishButtonSelected() \n");

  Panel *p = fpePanel;
  if( getPanelContainer()->getMainWindow() ) { 

    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();

    if( mw ) {

      LoadAttachObject *lao = NULL;
      bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();

#ifdef DEBUG_FPEWizard
      printf("vSummaryPageFinishButtonSelected(), A: localInstrumentorIsOffline=%d\n", localInstrumentorIsOffline );
#endif

//      ParamList *paramList = new ParamList();
// printf("A: push_back the parameters... well, the checked ones.\n");
// printf("valueList=(%s)\n", valueList.ascii() );

      if( !mw->executableName.isEmpty() ) {

#ifdef DEBUG_FPEWizard
        printf("vSummaryPageFinishButtonSelected(), A: executable name was specified.\n");
#endif

        // The offline flag indicates to the custom experiment panel that the experiment is using offline instrumentation.
        lao = new LoadAttachObject(mw->executableName, (char *)NULL, mw->parallelPrefixCommandStr, &paramList, TRUE, localInstrumentorIsOffline);

      } else if( !mw->pidStr.isEmpty() ) {

#ifdef DEBUG_FPEWizard
        printf("vSummaryPageFinishButtonSelected(), A: pid was specified.\n");
#endif
        // The offline flag doesn't mean anything for attaching to a pid, but is passed for consistency
        lao = new LoadAttachObject((char *)NULL, mw->pidStr, (char *)NULL, &paramList, TRUE, localInstrumentorIsOffline);

      } else {
          printf("Warning: No attach or load parameter available.\n");
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
          bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
          ao->isInstrumentorOffline = localInstrumentorIsOffline;


          if( (vwizardMode->isChecked() && vParameterTraceCheckBox->isChecked()) ||
              (ewizardMode->isChecked() && eParameterTraceCheckBox->isChecked()) ) {
              p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing", getPanelContainer(), ao);
          } else {
              p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("FPE Tracing", getPanelContainer(), ao);
          }

          delete ao;
        } else {

// printf("Send the param list to the new panel\n");
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
FPE_TracingWizardPanel::languageChange()
{
  unsigned int traced_functions = 100;

  setCaption( tr( "FPE Tracing - Wizard Panel" ) );
  vDescriptionPageTitleLabel->setText( tr( "<h1>FPE Tracing - Wizard</h1>" ) );
  vDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( vDescriptionPageIntroButton, tr( "Takes you back to the Intro Wizard so you can make a different selection." ) );
  vDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  vDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vDescriptionPageFinishButton->setEnabled(FALSE);

  vParameterPageDescriptionText->setText( tr( QString("The following options (parameters) are available to adjust.   These are the list of functions that the FPE collector is able to monitor.<br><br>\n") ) );

  vParameterPageFunctionListHeaderLabel->setText( tr( "You can monitor the following fpe functions(s):" ) );
  vParameterPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vParameterPageBackButton, tr( "Takes you back one page." ) );
  vParameterPageResetButton->setText( tr( "Reset" ) );
  QToolTip::add( vParameterPageResetButton, tr( "Reset the values to the default setings." ) );
  vParameterPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vParameterPageNextButton, tr( "Advance to the next wizard page." ) );
  vParameterPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vParameterPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vParameterPageFinishButton->setEnabled(FALSE);


  appendFunctionsToMonitor();

  vSummaryPageFinishLabel->setText( tr( "No summary available.\n" ) );

  vSummaryPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( vSummaryPageBackButton, tr( "Takes you back one page." ) );
  vSummaryPageFinishButton->setText( tr( "Finish..." ) );
  QToolTip::add( vSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"fpe\" panel" ) );
  eDescriptionPageTitleLabel->setText( tr( "<h1>FPE Tracing Wizard</h1>" ) );
  eDescriptionPageText->setText( tr( vFPE_TracingDescription ) );
  eDescriptionPageIntroButton->setText( tr( "< Back" ) );
  eDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( eDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  eDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( eDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  eDescriptionPageFinishButton->setEnabled(FALSE);

  eParameterPageDescriptionLabel->setText( tr( "The following options (parameters) are available to adjust.     <br>These are the options the collector has exported." ) );
  eParameterPageFunctionListHeaderLabel->setText( tr( "You can monitor the following fpe function(s):" ) );
  eParameterPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( eParameterPageBackButton, tr( "Takes you back one page." ) );
  eParameterPageResetButton->setText( tr( "Reset" ) );
  QToolTip::add( eParameterPageResetButton, tr( "Reset the values to the default setings." ) );
  eParameterPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( eParameterPageNextButton, tr( "Advance to the next wizard page." ) );
  eParameterPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( eParameterPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  eParameterPageFinishButton->setEnabled(FALSE);

  eSummaryPageFinishLabel->setText( tr( "No summary yet available.") );
  eSummaryPageBackButton->setText( tr( "< Back" ) );
  QToolTip::add( eSummaryPageBackButton, tr( "Takes you back one page." ) );
  eSummaryPageFinishButton->setText( tr( "Finish..." ) );
  QToolTip::add( eSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"FPE Tracing\" panel" ) );
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

    // Is there a fpe experiment type?
    bool found_one = FALSE;
    std::set<Metadata> collectortypes = Collector::getAvailable();
    for( std::set<Metadata>::const_iterator mi = collectortypes.begin();
         mi != collectortypes.end(); mi++ )
    {
      if( mi->getUniqueId() == "fpe" )
      {
        found_one = TRUE;
      }
    }
    if( found_one == FALSE )
    {
      return;
    }

    Collector fpeCollector = dummy_experiment.createCollector("fpe");

    Metadata cm = fpeCollector.getMetadata();
    std::set<Metadata> md =fpeCollector.getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++)
    {
        Metadata m = *mi;
// printf("A: %s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
// printf("B: %s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
// printf("C: %s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );

      vDescriptionPageText->setText( tr( cm.getDescription().c_str() ) );
    }
      std::map<std::string,bool> tracedFunctions;
      fpeCollector.getParameterValue("event", tracedFunctions);
// printf("Initialize the text fields... (%s)\n", tracedFunctions.first);
//    vParameterPageSampleRateText->setText(QString("%1").arg(tracedFunctions.first));
//    eParameterPageSampleRateText->setText(QString("%1").arg(tracedFunctions.first));

    if( temp_name )
    {
      (void) remove( temp_name );
    }

  }
  catch(const std::exception& error)
  {
    return;
  }

  vParameterPageCheckBoxSelected();
}

void
FPE_TracingWizardPanel::appendFunctionsToMonitor()
{
  std::map<std::string, bool> function_map;

  //std::string str = "MPI_Allgather";

  // function_map.insert(std::make_pair(str, true));
  function_map.insert(std::make_pair("inexact_result", true));
  function_map.insert(std::make_pair("division_by_zero", true));
  function_map.insert(std::make_pair("underflow", true));
  function_map.insert(std::make_pair("overflow", true));
  function_map.insert(std::make_pair("invalid_operation", true));
//  function_map.insert(std::make_pair("all_supported_exceptions", true));
  function_map.insert(std::make_pair("all", true));

  QCheckBox *vParameterPageCheckBox;
  QCheckBox *eParameterPageCheckBox;
  
  int i = 0;
  int r = 0;
  int c = 0;
CheckBoxInfoClass *cbic = NULL;
vCheckBoxInfoClassList.clear();
eCheckBoxInfoClassList.clear();
  for( std::map<std::string, bool>::const_iterator it = function_map.begin();
       it != function_map.end(); it++)
  {

    vParameterPageCheckBox = new QCheckBox( big_box_w, "vParameterPageCheckBox3" );
    vParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
    vParameterPageCheckBox->setText( it->first.c_str() );
    vParameterPageFunctionListGridLayout->addWidget( vParameterPageCheckBox, r, c );
    vParameterPageCheckBox->setChecked(it->second);
    vParameterPageCheckBox->setEnabled(TRUE);

cbic = new CheckBoxInfoClass();
cbic->checkbox = vParameterPageCheckBox;
vCheckBoxInfoClassList.push_back(cbic);
connect( cbic->checkbox, SIGNAL( clicked() ), this,
           SLOT( vParameterPageCheckBoxSelected() ) );
    
    eParameterPageCheckBox = new QCheckBox( eParameterPageWidget, "eParameterPageCheckBox3" );
    eParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
    eParameterPageCheckBox->setText( it->first.c_str() );
    eParameterPageFunctionListGridLayout->addWidget( eParameterPageCheckBox, r, c );
    eParameterPageCheckBox->setChecked(it->second);
    eParameterPageCheckBox->setEnabled(TRUE);

cbic = new CheckBoxInfoClass();
cbic->checkbox = eParameterPageCheckBox;
eCheckBoxInfoClassList.push_back(cbic);
connect( cbic->checkbox, SIGNAL( clicked() ), this,
           SLOT( eParameterPageCheckBoxSelected() ) );

    i++;
    if( i%MAXROWS == 0 )
    {
      r = -1;  // It's going to be incremented by one...
      c++;
      if( c > MAXCOLUMNS )
      {
         fprintf(stderr, "There were over %d function entries.   Not all functions may be displayed.\n", MAXROWS*MAXCOLUMNS);
      }
    }
    r++;
  }

}



void
FPE_TracingWizardPanel::handleSizeEvent(QResizeEvent *e)
{
  int numRows = vParameterPageFunctionListGridLayout->numRows();
  int numCols = vParameterPageFunctionListGridLayout->numCols();

// printf("numRows()=(%d) numCols=(%d)\n", vParameterPageFunctionListGridLayout->numRows(), vParameterPageFunctionListGridLayout->numCols() );

  int calculated_height = 0;
  int calculated_width = 0;

// I know we only have 6 rows...
  QRect rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 0);
  calculated_width += rect.width();
  rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 1);
  calculated_width += rect.width();
  rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 2);
  calculated_width += rect.width();
  rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 3);
  calculated_width += rect.width();
  rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 4);
  calculated_width += rect.width();
  rect = vParameterPageFunctionListGridLayout->cellGeometry( 0, 5);
  calculated_width += rect.width();
// printf("rect.width=(%d) rect.height=(%d)\n", rect.width(), rect.height() );
  // Add in some margin material.
  calculated_width += 12;
// printf("height=(%d) width=%d\n", calculated_height, calculated_width );

  // override the calculated_height
  calculated_height = (vParameterTraceCheckBox->height()+ 6) * numRows;
// printf("calculated_height=(%d)\n", (vParameterTraceCheckBox->height()+ 6) * numRows );


  big_box_w->resize(calculated_width,calculated_height);
}

void FPE_TracingWizardPanel::vParameterPageCheckBoxSelected()
{
// printf("vParameterPageCheckBoxSelected() entered\n");
  bool checkAll = FALSE;
  paramList.clear();
paramString = QString::null;
  for( CheckBoxInfoClassList::Iterator it = vCheckBoxInfoClassList.begin(); it != vCheckBoxInfoClassList.end(); ++it)
  {
    CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;
// printf("v: cbic: (%s) == (%d)\n", cbic->checkbox->text().ascii(), cbic->checkbox->isChecked() );
    if( cbic->checkbox->isChecked() && cbic->checkbox->text() == "all" )
    {
      checkAll = TRUE;
    }
    if( checkAll == TRUE )
    {
     cbic->checkbox->setChecked(TRUE);
    }
    if( cbic->checkbox->isChecked() )
    {
      paramList.push_back(cbic->checkbox->text());
      if( paramString.isEmpty() )
      {
        paramString = QString("%1").arg(cbic->checkbox->text());
      } else
      {
        paramString += QString(",%1").arg(cbic->checkbox->text());
      }
    }
  }
  if( checkAll == TRUE )
  { // simplify the parsing and clear this ... the default is all.
    paramList.clear();
    paramString = "All";
  }
}

void FPE_TracingWizardPanel::eParameterPageCheckBoxSelected()
{
// printf("vParameterPageCheckBoxSelected() entered\n");
  bool checkAll = FALSE;
  paramList.clear();
  paramString = QString::null;
  for( CheckBoxInfoClassList::Iterator it = eCheckBoxInfoClassList.begin(); it != eCheckBoxInfoClassList.end(); ++it)
  {
    CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;
// printf("e: cbic: (%s) == (%d)\n", cbic->checkbox->text().ascii(), cbic->checkbox->isChecked() );
    if( cbic->checkbox->isChecked() && cbic->checkbox->text() == "all" )
    {
      checkAll = TRUE;
    }
    if( checkAll == TRUE )
    {
     cbic->checkbox->setChecked(TRUE);
    }
    if( cbic->checkbox->isChecked() )
    {
      paramList.push_back(cbic->checkbox->text());
      if( paramString.isEmpty() )
      {
        paramString = QString("%1").arg(cbic->checkbox->text());
      } else
      {
        paramString += QString(",%1").arg(cbic->checkbox->text());
      }
    }
  }
  
  if( checkAll == TRUE )
  { // simplify the parsing and clear this ... the default is all.
    paramList.clear();
    paramString = "All";
  }
}
