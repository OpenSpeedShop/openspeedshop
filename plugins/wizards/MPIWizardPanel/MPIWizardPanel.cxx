////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006,2007,2008 Krell Institute All Rights Reserved.
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
// For debug output comment in the following define(s)
//#define DEBUG_MPIWizard 1
//#define DEBUG_CHECKBOX 1

// This comments out the restrictions for using offline in the GUI Wizards - leave this set
#define WHEN_OFFLINE_READY 1

#include <stdio.h>
#include "MPIWizardPanel.hxx"
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove
#include "AttachProcessDialog.hxx"

#define MAXROWS 9
#define MAXCOLUMNS 9

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

#include "MPIDescription.hxx"
#include "SS_Input_Manager.hxx"

#include "LoadAttachObject.hxx"

// Temporary default value for instrumentor is offline preference
// We need to set up a class for the preference defaults to access them across the GUI panels and wizards.
bool defaultValue_instrumentorIsOffline = TRUE;


// Category Name Information Class
// Specify the category name to be displayed and whether that
// category group is enabled or not.
class catNameMap
{
private:
  MPI_Category_Type currentCatType;
public:
  catNameMap() {}
  catNameMap( std::string funcName, bool funcEnableStatus) {
      mpiCategoryName = funcName;
      enabled = funcEnableStatus;
  }
  virtual ~catNameMap() {}
  std::string mpiCategoryName;
  bool enabled;
//  void setCurrentCat(MPI_Category_Type newKey) {
//          currentCatType = newKey;
//  }
//  MPI_Category_Type getCurrentCat() {
//          return currentCatType;
//  }
};

std::vector<catNameMap*> mpiCatNames;
std::vector<catNameMap*> sendToCliMpiCatNames;
std::vector<catNameMap*> e_lastStatusCategories;
std::vector<catNameMap*> v_lastStatusCategories;

void setupInitialLastStatus() {
  v_lastStatusCategories.push_back(new catNameMap("MPI Functions (all)", true));
  v_lastStatusCategories.push_back(new catNameMap("Collective Communicators", true));
  v_lastStatusCategories.push_back(new catNameMap("Persistent_Communicators", true));
  v_lastStatusCategories.push_back(new catNameMap("Synchronous Point to Point", true));
  v_lastStatusCategories.push_back(new catNameMap("Asynchronous Point to Point", true));
  v_lastStatusCategories.push_back(new catNameMap("Process Topologies", true));
  v_lastStatusCategories.push_back(new catNameMap("Groups Contexts Communicators", true));
  v_lastStatusCategories.push_back(new catNameMap("Environment", true));
  v_lastStatusCategories.push_back(new catNameMap("Datatypes", true));

  e_lastStatusCategories.push_back(new catNameMap("MPI Functions (all)", true));
  e_lastStatusCategories.push_back(new catNameMap("Collective Communicators", true));
  e_lastStatusCategories.push_back(new catNameMap("Persistent_Communicators", true));
  e_lastStatusCategories.push_back(new catNameMap("Synchronous Point to Point", true));
  e_lastStatusCategories.push_back(new catNameMap("Asynchronous Point to Point", true));
  e_lastStatusCategories.push_back(new catNameMap("Process Topologies", true));
  e_lastStatusCategories.push_back(new catNameMap("Groups Contexts Communicators", true));
  e_lastStatusCategories.push_back(new catNameMap("Environment", true));
  e_lastStatusCategories.push_back(new catNameMap("Datatypes", true));
}
//
// Function Name Information Class

void setupSendToCliCatNames() {
  sendToCliMpiCatNames.push_back(new catNameMap("all", true));
  sendToCliMpiCatNames.push_back(new catNameMap("collective_com", true));
  sendToCliMpiCatNames.push_back(new catNameMap("persistent_com", true));
  sendToCliMpiCatNames.push_back(new catNameMap("synchronous_p2p", true));
  sendToCliMpiCatNames.push_back(new catNameMap("asynchronous_p2p", true));
  sendToCliMpiCatNames.push_back(new catNameMap("process_topologies", true));
  sendToCliMpiCatNames.push_back(new catNameMap("graphs_contexts_comms", true));
  sendToCliMpiCatNames.push_back(new catNameMap("environment", true));
  sendToCliMpiCatNames.push_back(new catNameMap("datatypes", true));
}

void setupCatNames() {
  mpiCatNames.push_back(new catNameMap("MPI Functions (all)", true));
  mpiCatNames.push_back(new catNameMap("Collective Communicators", true));
  mpiCatNames.push_back(new catNameMap("Persistent_Communicators", true));
  mpiCatNames.push_back(new catNameMap("Synchronous Point to Point", true));
  mpiCatNames.push_back(new catNameMap("Asynchronous Point to Point", true));
  mpiCatNames.push_back(new catNameMap("Process Topologies", true));
  mpiCatNames.push_back(new catNameMap("Groups Contexts Communicators", true));
  mpiCatNames.push_back(new catNameMap("Environment", true));
  mpiCatNames.push_back(new catNameMap("Datatypes", true));
}
//
// Function Name Information Class
// Specify the category type, the MPI function name and whether that
// MPI function selection is enabled or not.
// 
class functionMap
{
public:
  functionMap() {}
  functionMap( MPI_Category_Type funcCategory, std::string funcName, bool funcEnableStatus) {
      mpiCatType = funcCategory;
      mpiFunctionName = funcName;
      enabled = funcEnableStatus;
  }
  virtual ~functionMap() {}
  MPI_Category_Type mpiCatType;
  std::string mpiFunctionName;
  bool enabled;
};

//
// Specify a vector for function Map entries, one for each MPI function being 
// profiled.
//
std::vector<functionMap*> functionVector;

using namespace OpenSpeedShop::Framework;

MPIWizardPanel::MPIWizardPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf(DEBUG_CONST_DESTRUCT) ("MPIWizardPanel::MPIWizardPanel() constructor called\n");
  if ( !getName() )
  {
	setName( "MPI" );
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


  // Setup the default version of the category checkbox status (all set).
  setupCatNames();
  setupSendToCliCatNames();

  // Setup the initial version of the category checkbox previous status (all set).
  // This is used to compare which check boxes have changed
  setupInitialLastStatus();

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::MPIWizardPanel() CLEAR paramList.\n");
#endif
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
#ifdef DEBUG_MPIWizard
  printf("MPIWizard setup: /openspeedshop/general/instrumentorIsOffline=(%d), boolOK=%d\n", temp_instrumentorIsOffline, boolOK );
#endif
  delete settings;
#endif

  mpiFormLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

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

#ifdef DEBUG_MPIWizard
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
  vParameterTraceCheckBox->setText(tr("TRACING: Gather additional information for each MPI function call. (mpit)\n") );
  vParameterPageParameterLayout->addWidget( vParameterTraceCheckBox );
  QToolTip::add( vParameterTraceCheckBox, tr( "Records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );


  vParameterOTFTraceCheckBox = new QCheckBox( vParameterPageWidget, "vParameterOTFTraceComboBox" );
  vParameterOTFTraceCheckBox->setText(tr("TRACING: Write OTF files and gather additional information for each MPI function call. (mpiotf)\n") );
  vParameterPageParameterLayout->addWidget( vParameterOTFTraceCheckBox );
  QToolTip::add( vParameterOTFTraceCheckBox, tr( "Writes MPI Trace Data to OTF files and records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );



  vParameterPageLine2 = new QFrame( vParameterPageWidget, "vParameterPageLine2" );
  vParameterPageLine2->setMinimumSize( QSize(10,10) );
  vParameterPageLine2->setFrameShape( QFrame::HLine );
  vParameterPageLine2->setFrameShadow( QFrame::Sunken );
  vParameterPageLine2->setFrameShape( QFrame::HLine );
  vParameterPageParameterLayout->addWidget( vParameterPageLine2 );


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

#ifdef DEBUG_MPIWizard
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
  eParameterTraceCheckBox->setText(tr("Gather additional information for each MPI function call. (mpit)") );
  eParameterPageParameterLayout->addWidget( eParameterTraceCheckBox );
  QToolTip::add( eParameterTraceCheckBox, tr( "Records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );


  eParameterOTFTraceCheckBox = new QCheckBox( eParameterPageWidget, "eParameterOTFTraceComboBox" );
  eParameterOTFTraceCheckBox->setText(tr("Gather additional information for each MPI function call. (mpiotf)") );
  eParameterPageParameterLayout->addWidget( eParameterOTFTraceCheckBox );
  QToolTip::add( eParameterOTFTraceCheckBox, tr( "Writes MPI Trace Data to OTF files and records extra information, with more overhead, including\nsource rank, destination rank, size of message, tag of event,\ncomminicator used, data type of event, and the return value\nof the event.") );


  eParameterPageFunctionListHeaderLabel = new QLabel( eParameterPageWidget, "eParameterPageFunctionListHeaderLabel" );
  eParameterPageFunctionListHeaderLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed, 0, 0, FALSE ) );
  eParameterPageParameterLayout->addWidget( eParameterPageFunctionListHeaderLabel );


  eParameterPageFunctionListLayout = new QHBoxLayout( 0, 0, 6, "eParameterPageFunctionListLayout"); 

#if 1
  e_sv = new QScrollView( eParameterPageWidget, "scrollView" );
  e_big_box_w = new QWidget(e_sv->viewport(), "e_big_box(viewport)" );
  e_big_box_w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  const QColor e_color = vParameterTraceCheckBox->paletteBackgroundColor();
  e_sv->viewport()->setBackgroundColor(e_color);
  // sv->viewport()->setPaletteBackgroundColor(color);
  e_sv->addChild(e_big_box_w);
  eParameterPageFunctionListLayout->addWidget( e_sv );

  // For debugging layout
  // big_box_w->setBackgroundColor("Red");


  QHBoxLayout *e_glayout = new QHBoxLayout( e_big_box_w, 0, 6, "e_glayout");

  eParameterPageFunctionListGridLayout = new QGridLayout( e_glayout, MAXROWS, MAXCOLUMNS, 3, "eParameterPageFunctionListGridLayout"); 
#else
  eParameterPageFunctionListGridLayout = new QGridLayout( eParameterPageFunctionListLayout, MAXROWS, MAXCOLUMNS, 3, "eParameterPageFunctionListGridLayout"); 
#endif

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
  mpiFormLayout->addWidget( mainFrame );
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
// parent mpiPanel's hook.    
// This should only be > 1 when we're calling this wizard from within
// a mpiPanel session to help the user load an executable.
  mpiPanel = NULL;
//  if( (int)argument > 1 )
  if( ao && ao->panel_data != NULL )
  {
    mpiPanel = (Panel *)ao->panel_data;
  }

// end debug

}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
MPIWizardPanel::~MPIWizardPanel()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("  MPIWizardPanel::~MPIWizardPanel() destructor called\n");
}

//! Add user panel specific menu items if they have any.
bool
MPIWizardPanel::menu(QPopupMenu* contextMenu)
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::menu() requested.\n");

  return( FALSE );
}

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
MPIWizardPanel::save()
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
MPIWizardPanel::saveAs()
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
MPIWizardPanel::listener(void *msg)
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::listener() requested.\n");
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::listener() requested.\n");
#endif

  MessageObject *messageObject = (MessageObject *)msg;
  nprintf(DEBUG_PANELS) ("  MPIWizardPanel::listener, messageObject->msgType = %s\n", messageObject->msgType.ascii() );
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::listener,  messageObject->msgType = %s\n", messageObject->msgType.ascii() );
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
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, try to find panel (%s)\n", name.ascii() );
#endif
//    Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    Panel *loadPanel = getThisWizardsLoadPanel();
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, loadPanel=0x%x\n", loadPanel);
#endif
    if( loadPanel ) {
#ifdef DEBUG_MPIWizard
      printf("MPIWizardPanel::listener, Wizard_Raise_LoadPanel_Back_Page, Found the loadPanel... Try to hide it.\n");
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
    
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::listener, Wizard_Raise_Summary_Page if block \n");
#endif
    vPrepareForSummaryPage();
    return 1;
  }
#if 1
  if( messageObject->msgType == "PreferencesChangedObject" ) {

   bool temp_instrumentorIsOffline = getToolPreferenceInstrumentorIsOffline();
#ifdef DEBUG_MPIWizard
   printf("MPIWizard::listener, PREFERENCE-CHANGED-OBJECT temp_instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
    return 1;

 }
#endif

  return 0;  // 0 means, did not want this message and did not act on anything.
}

//! This function broadcasts messages.
int 
MPIWizardPanel::broadcast(char *msg)
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::broadcast() requested.\n");
  return 0;
}

bool MPIWizardPanel::getToolPreferenceInstrumentorIsOffline()
{
  QSettings *settings = new QSettings();
  bool temp_instrumentorIsOffline = settings->readBoolEntry( "/openspeedshop/general/instrumentorIsOffline");
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::getToolPreferenceInstrumentorIsOffline, /openspeedshop/general/instrumentorIsOffline == instrumentorIsOffline=(%d)\n", temp_instrumentorIsOffline );
#endif
  delete settings;
}

void MPIWizardPanel::vOfflineRBSelected()
{
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = vOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
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

void MPIWizardPanel::vOnlineRBSelected()
{
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = vOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}


void MPIWizardPanel::eOfflineRBSelected()
{
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eOfflineRBSelected() entered.\n");
#endif
  bool offlineCheckBoxValue = eOfflineRB->isOn();
  if ( offlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eOfflineRBSelected() offlineCheckBoxValue=(%d)\n", offlineCheckBoxValue);
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

void MPIWizardPanel::eOnlineRBSelected()
{
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eOnlineRBSelected() entered.\n");
#endif
  bool onlineCheckBoxValue = eOnlineRB->isOn();
  if ( onlineCheckBoxValue ) {
   // toggle the button settings or is this done for us?
  }
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eOnlineRBSelected() onlineCheckBoxValue=(%d)\n", onlineCheckBoxValue);
#endif
  setThisWizardsInstrumentorIsOffline(!onlineCheckBoxValue);
}



void MPIWizardPanel::vwizardModeSelected()
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

void MPIWizardPanel::ewizardModeSelected()
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


void MPIWizardPanel::wizardModeSelected()
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
  vwizardMode->setChecked( TRUE );
}

#if 1
Panel* MPIWizardPanel::findAndRaiseLoadPanel()
{
  // Try to raise the load panel if there is one hidden
#if 1

  Panel *p = getThisWizardsLoadPanel();
  if (getThisWizardsInstrumentorIsOffline() == getThisWizardsPreviousInstrumentorIsOffline() ) {
#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::findAndRaiseLoadPanel, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
  } else {
#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::findAndRaiseLoadPanel, SET P NULL, p=%x, getThisWizardsInstrumentorIsOffline()=%d, getThisWizardsPreviousInstrumentorIsOffline()=%d\n",
            p, getThisWizardsInstrumentorIsOffline(), getThisWizardsPreviousInstrumentorIsOffline());
#endif
     // create a new loadPanel
     p = NULL;
  }

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::findAndRaiseLoadPanel, found thisWizardsLoadPanel - now raising, p=%x\n", p);
  if (p) {
    printf("MPIWizardPanel::findAndRaiseLoadPanel, p->getName()=%s\n", p->getName() );
  }
#endif

  if (p) {
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }

#else

  QString name = QString("loadPanel");
  Panel *p = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if (p) {
#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::findAndRaiseLoadPanel, found loadPanel - now raising, p=%x\n", p);
     if (p) {
       printf("MPIWizardPanel::findAndRaiseLoadPanel, found loadPanel, p->getName()=%s\n", p->getName() );
     }
#endif
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }
#endif

  return p;
}

#else
Panel* MPIWizardPanel::findAndRaiseLoadPanel()
{
  // Try to raise the load panel if there is one hidden

#if 1
  Panel *p = getThisWizardsLoadPanel();
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::findAndRaiseLoadPanel, found thisWizardsLoadPanel - now raising, p=0x%x\n", p);
#endif
  if (p) {
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }

#else

  QString name = QString("loadPanel");
  Panel *p = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if (p) {
#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::findAndRaiseLoadPanel, found loadPanel - now raising, p=%x\n", p);
#endif
     p->getPanelContainer()->raisePanel(p);
  } else {
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::findAndRaiseLoadPanel, did not find loadPanel\n");
#endif
  }
#endif

  return p;
}
#endif


void MPIWizardPanel::eDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eDescriptionPageNextButtonSelected() \n");
  mainWidgetStack->raiseWidget(eParameterPageWidget);
}

void MPIWizardPanel::eDescriptionPageIntroButtonSelected()
{
  getPanelContainer()->hidePanel((Panel *)this);

  nprintf(DEBUG_PANELS) ("eDescriptionPageIntroButtonSelected() \n");

  Panel *p = getPanelContainer()->raiseNamedPanel((char *) "Intro Wizard");
  if( !p )
  {
    getPanelContainer()->getMasterPC()->dl_create_and_add_panel("Intro Wizard", getPanelContainer(), NULL );
  }
}

void MPIWizardPanel::eParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageBackButtonSelected() \n");
  mainWidgetStack->raiseWidget(eDescriptionPageWidget);
}

void MPIWizardPanel::eParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageNextButtonSelected() \n");

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;

  } else {

#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel, calling loadNewProgramPanel()\n");
#endif
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw ) {
     mw->executableName = QString((const char *)0);
     mw->argsStr = QString((const char *)0);
     mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel calling mw->loadNewProgramPanel, this=0x%x, getThisWizardsInstrumentorIsOffline()=%d \n", this, getThisWizardsInstrumentorIsOffline()  );
#endif

     Panel* p = mw->loadNewProgramPanel(getPanelContainer(), getPanelContainer()->getMasterPC(), /* expID */-1, (Panel *) this, getThisWizardsInstrumentorIsOffline() );
     setThisWizardsLoadPanel(p);
#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::eParameterPageNextButtonSelected, after calling mw->loadNewProgramPanel, p=0x%x\n", p );
     if (p) {
       printf("MPIWizardPanel::eParameterPageNextButtonSelected, p->getName()=%s\n", p->getName() );
     }
#endif

     QString executableNameStr = mw->executableName;
     if( !mw->executableName.isEmpty() ) {
#ifdef DEBUG_MPIWizard
      printf("MPIWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif
     } else {
#ifdef DEBUG_MPIWizard
      printf("MPIWizardPanel, executableName is empty\n" );
#endif
     } // end if clause for empty executable name
    } // end if clause for mw
  } // end else clause for create new load panel
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);


}

void MPIWizardPanel::eParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eParameterPageResetButtonSelected() \n");
}

void MPIWizardPanel::eSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageBackButtonSelected() \n");

// JUST RAISE EXISTING PANEL if one is there.

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eSummaryPageBackButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::eSummaryPageBackButtonSelected, creating loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);

    if (p) {

#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::eSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }

}

void MPIWizardPanel::eSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("eSummaryPageFinishButtonSelected() \n");

  vSummaryPageFinishButtonSelected();

}

void MPIWizardPanel::vDescriptionPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vDescriptionPageNextButtonSelected() \n");


  mainWidgetStack->raiseWidget(vParameterPageWidget);
}

void MPIWizardPanel::vDescriptionPageIntroButtonSelected()
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

void MPIWizardPanel::vParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("vParameterPageSampleRateTextReturnPressed() \n");
}

void MPIWizardPanel::eParameterPageSampleRateTextReturnPressed()
{
  nprintf(DEBUG_PANELS) ("eParameterPageSampleRateTextReturnPressed() \n");
}

void MPIWizardPanel::vParameterPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageBackButtonSelected() \n");

  mainWidgetStack->raiseWidget(vDescriptionPageWidget);
}

void MPIWizardPanel::vParameterPageNextButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageNextButtonSelected() \n");

 // See if loadPanel already exists - if user used the back button
 // we may have hidden the panel and now just need to raise it instead
 // of creating a new one.

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vParameterPageNextButtonSelected, calling findAndRaiseLoadPanel()\n");
#endif
  Panel *p = findAndRaiseLoadPanel();

  if (p) {
    // raise the first page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_First_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_MPIWizard
   printf("MPIWizardPanel, calling loadNewProgramPanel()\n");
#endif
   OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
   if( mw ) {
    mw->executableName = QString((const char *)0);
    mw->argsStr = QString((const char *)0);
    mw->parallelPrefixCommandStr = QString((const char *)0);

#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel -2- calling mw->loadNewProgramPanel, this=0x%x, getThisWizardsInstrumentorIsOffline()=%d \n", this, getThisWizardsInstrumentorIsOffline()  );
#endif

    Panel* p = mw->loadNewProgramPanel(getPanelContainer(), getPanelContainer()->getMasterPC(), /* expID */-1, (Panel *) this, getThisWizardsInstrumentorIsOffline());
    setThisWizardsLoadPanel(p);
#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::vParameterPageNextButtonSelected, after calling mw->loadNewProgramPanel, p=0x%x\n", p );
    if (p) {
      printf("MPIWizardPanel::vParameterPageNextButtonSelected, p->getName()=%s\n", p->getName() );
    }
#endif

    QString executableNameStr = mw->executableName;
    if( !mw->executableName.isEmpty() ) {
#ifdef DEBUG_MPIWizard
      printf("MPIWizardPanel, executableName=%s\n", mw->executableName.ascii() );
#endif
    } else {
#ifdef DEBUG_MPIWizard
      printf("MPIWizardPanel, executableName is empty\n" );
#endif
    }
   }
  }
//jeg  mainWidgetStack->raiseWidget(vSummaryPageWidget);

}

void MPIWizardPanel::vParameterPageResetButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vParameterPageResetButtonSelected() \n");
}

void MPIWizardPanel::vPrepareForSummaryPage()
{
  nprintf(DEBUG_PANELS) ("MPIWizardPanel::vPrepareForSummaryPage() \n");
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vPrepareForSummaryPage() \n");
#endif

  char buffer[2048];

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( !mw ) {
    return;
  } 

  if( !mw->pidStr.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    sprintf(buffer, "<p align=\"left\">You've selected a MPI experiment for process \"%s\" running on host \"%s\".<br>Furthermore, you've chosen to monitor \"%s\" mpi functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"mpi\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->pidStr.ascii(), mw->hostStr.ascii(), paramString.ascii() );

  } else if( !mw->executableName.isEmpty() ) {

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);

    if( mw->parallelPrefixCommandStr.isEmpty() || mw->parallelPrefixCommandStr.isNull() ) {
        if (getThisWizardsInstrumentorIsOffline()) {
          sprintf(buffer, "<p align=\"left\">You've selected a MPI experiment for executable \"%s\" to be run on host \"%s\"<br>using offline instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" mpi functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"mpi\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        } else {
          sprintf(buffer, "<p align=\"left\">You've selected a MPI experiment for executable \"%s\" to be run on host \"%s\"<br>using online/dynamic instrumentation.<br>Furthermore, you've chosen to monitor \"%s\" mpi functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"mpi\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        }



    } else {

        if (getThisWizardsInstrumentorIsOffline()) {
          sprintf(buffer, "<p align=\"left\">You've selected a MPI experiment for command/executable<br>\"%s %s\" to be run on host \"%s\"<br>using offline instrumentation.  Furthermore, you've chosen to monitor \"%s\" mpi functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"mpi\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        } else {
          sprintf(buffer, "<p align=\"left\">You've selected a MPI experiment for command/executable<br>\"%s %s\" to be run on host \"%s\"<br>using online/dynamic instrumentation.  Furthermore, you've chosen to monitor \"%s\" mpi functions.<br><br>To complete the experiment setup select the \"Finish\" button.<br><br>After selecting the \"Finish\" button an experiment \"mpi\" panel will be raised to allow you to futher control the experiment.<br><br>Press the \"Back\" button to go back to the previous page.</p>", mw->parallelPrefixCommandStr.ascii(), mw->executableName.ascii(), mw->hostStr.ascii(), paramString.ascii() );
        }

    }
  }

  vSummaryPageFinishLabel->setText( tr( buffer ) );
  mainWidgetStack->raiseWidget(2);
  mainWidgetStack->raiseWidget(vSummaryPageWidget);

  QString name = "loadPanel";
#ifdef DEBUG_MPIWizard
  printf("try to find panel (%s)\n", name.ascii() );
#endif
//  Panel *loadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  // Find our specific load panel not any other wizards/experiments 
  // that might be hidden or raised for that matter
  Panel *loadPanel = getThisWizardsLoadPanel();

#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vPrepareForSummaryPage(), loadPanel=0x%x\n", loadPanel);
#endif
  if( loadPanel ) {
#ifdef DEBUG_MPIWizard
     printf("Found the loadPanel... Try to hide it.\n");
#endif
     loadPanel->getPanelContainer()->hidePanel(loadPanel);
  }

}

void MPIWizardPanel::vSummaryPageBackButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageBackButtonSelected() \n");

// RAISE EXISTING PANEL if one is there and request the second page of the wizard
// be focused/raised.

  Panel *p = findAndRaiseLoadPanel();

  if (p) {

#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

    // raise the second page of the load panel
    MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
    p->listener((void *)msg);
    delete msg;
  } else {

#ifdef DEBUG_MPIWizard
    printf("MPIWizardPanel::vSummaryPageBackButtonSelected, did not find loadPanel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif

    ArgumentObject *ao = new ArgumentObject("ArgumentObject", -1 );
    bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
    ao->isInstrumentorOffline = localInstrumentorIsOffline;
    p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("loadPanel", getPanelContainer(), ao);
    if (p) {

#ifdef DEBUG_MPIWizard
     printf("MPIWizardPanel::vSummaryPageBackButtonSelected, found loadPanel, p=%x\n", p);
#endif

     MessageObject *msg = new MessageObject("Wizard_Raise_Second_Page");
     p->listener((void *)msg);
     delete msg;
    }
  }

}

void MPIWizardPanel::finishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("finishButtonSelected() \n");

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
  if( mw->executableName.isEmpty() && mw->pidStr.isEmpty() )
  {
    if( vwizardMode->isOn() ) {
      vSummaryPageFinishButtonSelected();
    } else {
      vSummaryPageFinishButtonSelected();
    }
  } else
  {
    vSummaryPageFinishButtonSelected();
  }
}


void MPIWizardPanel::vSummaryPageFinishButtonSelected()
{
  nprintf(DEBUG_PANELS) ("vSummaryPageFinishButtonSelected() \n");
// printf("vSummaryPageFinishButtonSelected() \n");

  Panel *p = mpiPanel;
  if( getPanelContainer()->getMainWindow() ) { 
    OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
    if( mw ) {

      LoadAttachObject *lao = NULL;
      bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();

//      ParamList *paramList = new ParamList();
// printf("A: push_back (%s)\n", vParameterPageSampleRateText->text().ascii() );

      if( !mw->executableName.isEmpty() ) {

#ifdef DEBUG_MPIWizard
        printf("vSummaryPageFinishButtonSelected(), A: executable name was specified.\n");
#endif

        // The offline flag indicates to the custom experiment panel that the experiment is using offline instrumentation.

        lao = new LoadAttachObject((QString) mw->executableName, 
                                   (QString) QString::null,
                                   (QString) mw->parallelPrefixCommandStr, 
                                   &paramList, 
                                   true, 
                                   (bool) localInstrumentorIsOffline);

      } else if( !mw->pidStr.isEmpty() ) {

#ifdef DEBUG_MPIWizard
        printf("vSummaryPageFinishButtonSelected(), A: pid was specified.\n");
#endif

        // The offline flag doesn't mean anything for attaching to a pid, but is passed for consistency

        QString nullQString = QString::null;
        lao = new LoadAttachObject( nullQString,
                                    mw->pidStr, 
                                    nullQString,
                                    &paramList, 
                                    true, 
                                    localInstrumentorIsOffline);

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
          bool localInstrumentorIsOffline = getThisWizardsInstrumentorIsOffline();
          ao->isInstrumentorOffline = localInstrumentorIsOffline;

#ifdef DEBUG_MPIWizard
          printf("MPIWizardPanel::vSummaryPageBackButtonSelected, creating pc Sampling experiment panel, getThisWizardsInstrumentorIsOffline()=%d\n", getThisWizardsInstrumentorIsOffline());
#endif


          if( (vwizardMode->isChecked() && vParameterTraceCheckBox->isChecked()) ||
              (ewizardMode->isChecked() && eParameterTraceCheckBox->isChecked()) ) {

              p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPIT", getPanelContainer(), ao);

          } else if( (vwizardMode->isChecked() && vParameterOTFTraceCheckBox->isChecked()) ||
              (ewizardMode->isChecked() && eParameterOTFTraceCheckBox->isChecked()) ) {

              p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPIOTF", getPanelContainer(), ao);

          } else {

              p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("MPI", getPanelContainer(), ao);

          }

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
MPIWizardPanel::languageChange()
{
  unsigned int traced_functions = 100;

  setCaption( tr( "MPI/MPIT - Wizard Panel" ) );
  vDescriptionPageTitleLabel->setText( tr( "<h1>MPI/MPIT - Wizard</h1>" ) );
//  vDescriptionPageText->setText( tr( vMPIDescription ) );
  vDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( vDescriptionPageIntroButton, tr( "Takes you back to the second page of the Intro Wizard so you can make a different selection." ) );
  vDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( vDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  vDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( vDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  vDescriptionPageFinishButton->setEnabled(FALSE);

  vParameterPageDescriptionText->setText( tr( QString("The check boxes below represent the list of functions that the MPI experiment is able to trace/monitor.  By selecting or deselecting the check boxes, you can chose which MPI functions to monitor/analyze.<br>NOTE: The TRACING option (below) when selected, records each MPI call chronologically to form a trace of the MPI calls made in your MPI application.  The option also causes more MPI related information to be recorded.<br>\n") ) );

  vParameterPageFunctionListHeaderLabel->setText( tr( "You can monitor the following mpi functions(s):" ) );
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
  QToolTip::add( vSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"mpi\" panel" ) );
  eDescriptionPageTitleLabel->setText( tr( "<h1>MPI Wizard</h1>" ) );
  eDescriptionPageText->setText( tr( eMPIDescription ) );
  eDescriptionPageIntroButton->setText( tr( "< Back" ) );
  QToolTip::add( eDescriptionPageIntroButton, tr( "Takes you back to the second page of the Intro Wizard so you can make a different selection." ) );
  eDescriptionPageNextButton->setText( tr( "> Next" ) );
  QToolTip::add( eDescriptionPageNextButton, tr( "Advance to the next wizard page." ) );
  eDescriptionPageFinishButton->setText( tr( ">> Finish" ) );
  QToolTip::add( eDescriptionPageFinishButton, tr( "Not active, present in this page for consistent use of buttons." ) );
  eDescriptionPageFinishButton->setEnabled(FALSE);

  eParameterPageDescriptionLabel->setText( tr( "The following options (parameters) are available to adjust.     <br>These are the options the collector has exported." ) );
  eParameterPageFunctionListHeaderLabel->setText( tr( "You can monitor the following mpi function(s):" ) );
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
  QToolTip::add( eSummaryPageFinishButton, tr( "Finishes loading the wizard information and brings up a \"MPI\" panel" ) );
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

    // Is there a mpi experiment type?
    bool found_one = FALSE;
    std::set<Metadata> collectortypes = Collector::getAvailable();
    for( std::set<Metadata>::const_iterator mi = collectortypes.begin();
         mi != collectortypes.end(); mi++ )
    {
      if( mi->getUniqueId() == "mpi" )
      {
        found_one = TRUE;
      }
    }
    if( found_one == FALSE )
    {
      return;
    }

    Collector mpiCollector = dummy_experiment.createCollector("mpi");

    Metadata cm = mpiCollector.getMetadata();
    std::set<Metadata> md =mpiCollector.getParameters();
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
      mpiCollector.getParameterValue("traced_functions", tracedFunctions);
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
MPIWizardPanel::appendFunctionsToMonitor()
{

  functionVector.clear();

// Collective Communication:
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Allgather", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Allgatherv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Allreduce", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Alltoall", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Alltoallv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Barrier", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Bcast", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Gather", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Gatherv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Reduce", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Reduce_scatter", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Scan", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Scatter", true));
  functionVector.push_back(new functionMap(MPI_Cat_Collective_Comm, "MPI_Scatterv", true));

// Persistent Communication:
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Bsend_init", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Recv_init", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Rsend_init", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Send_init", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Ssend_init", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Start", true));
  functionVector.push_back(new functionMap(MPI_Cat_Persistent_Comm, "MPI_Startall", true));

// Synchronous Point to Point:
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Bsend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Get_count", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Probe", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Recv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Rsend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Send", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Sendrecv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Sendrecv_replace", true));
  functionVector.push_back(new functionMap(MPI_Cat_Synchronous_P2P, "MPI_Ssend", true));

// Asynchronous Point to Point:
//
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Cancel", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Ibsend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Iprobe", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Irecv", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Irsend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Isend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Issend", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Request_free", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Test", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Testall", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Testany", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Testsome", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Wait", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Waitall", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Waitany", true));
  functionVector.push_back(new functionMap(MPI_Cat_Asynchronous_P2P, "MPI_Waitsome", true));

// Process Topologies:

  functionVector.push_back(new functionMap(MPI_Cat_Process_Topologies, "MPI_Cart_create", true));
  functionVector.push_back(new functionMap(MPI_Cat_Process_Topologies, "MPI_Cart_sub", true));
  functionVector.push_back(new functionMap(MPI_Cat_Process_Topologies, "MPI_Graph_create", true));

// Groups, Contexts, and Communicators:

  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Comm_create", true));
  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Comm_dup", true));
  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Comm_free", true));
  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Comm_split", true));
  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Intercomm_create", true));
  functionVector.push_back(new functionMap(MPI_Cat_Groups_Contexts_Comms, "MPI_Intercomm_merge", true));

//Environment:

  functionVector.push_back(new functionMap(MPI_Cat_Environment, "MPI_Finalize", true));
  functionVector.push_back(new functionMap(MPI_Cat_Environment, "MPI_Init", true));

//Datatypes:

  functionVector.push_back(new functionMap(MPI_Cat_Datatypes, "MPI_Pack", true));
  functionVector.push_back(new functionMap(MPI_Cat_Datatypes, "MPI_Unpack", true));

  QCheckBox *vCategoryParameterPageCheckBox;
  QCheckBox *vParameterPageCheckBox;
  QCheckBox *eCategoryParameterPageCheckBox;
  QCheckBox *eParameterPageCheckBox;

  int i = 0;
  int r = 0;
  int c = 0;
  CheckBoxInfoClass *e_cbic = NULL;
  CheckBoxInfoClass *v_cbic = NULL;

  vCheckBoxInfoClassList.clear();
  eCheckBoxInfoClassList.clear();

  eCategoryCheckBoxInfoClassList.clear();
  vCategoryCheckBoxInfoClassList.clear();

  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {

    // Handle the Category header
    std::vector<catNameMap*>::iterator cit = mpiCatNames.begin() + catKey;

    vCategoryParameterPageCheckBox = new QCheckBox( big_box_w, "CategoryParameterPageCheckBox1" );
    vCategoryParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );

//    eCategoryParameterPageCheckBox = new QCheckBox( eParameterPageWidget, "CategoryParameterPageCheckBox1" );
    eCategoryParameterPageCheckBox = new QCheckBox( e_big_box_w, "CategoryParameterPageCheckBox1" );
    eCategoryParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );

#if DEBUG_CHECKBOX
    printf("catKey=%d\n", catKey);
    printf("(*cit)->mpiCategoryName.c_str()=%s\n", (*cit)->mpiCategoryName.c_str());
#endif

    // Set the category checkbox text to bold
    QFont catFont = vCategoryParameterPageCheckBox->font();
    catFont.setBold(TRUE);
    vCategoryParameterPageCheckBox->setFont( catFont ) ;
    
#if 0
    QString fontString = vCategoryParameterPageCheckBox->font().family();
    printf("fontString.ascii()=%s\n", fontString.ascii());
    int psize = vCategoryParameterPageCheckBox->font().pointSize();
    printf("psize=%d\n", psize);
    int pxsize = vCategoryParameterPageCheckBox->font().pixelSize();
    printf("pxsize=%d\n", pxsize);
//    vCategoryParameterPageCheckBox->setFont(QFont("Times", 12, QFont::Bold) );
//    vCategoryParameterPageCheckBox->font().setBold(TRUE);
#endif

    vCategoryParameterPageCheckBox->setText( (*cit)->mpiCategoryName );
    vParameterPageFunctionListGridLayout->addWidget( vCategoryParameterPageCheckBox, r, c );
    vCategoryParameterPageCheckBox->setChecked((*cit)->enabled);

    // Set the category checkbox text to bold
    catFont = eCategoryParameterPageCheckBox->font();
    catFont.setBold(TRUE);
    eCategoryParameterPageCheckBox->setFont( catFont ) ;

    eCategoryParameterPageCheckBox->setText( (*cit)->mpiCategoryName );
//    eCategoryParameterPageCheckBox->setFont(QFont("Times", 12, QFont::Bold) );
    eParameterPageFunctionListGridLayout->addWidget( eCategoryParameterPageCheckBox, r, c );
    eCategoryParameterPageCheckBox->setChecked((*cit)->enabled);

    e_cbic = new CheckBoxInfoClass();
    e_cbic->checkbox = eCategoryParameterPageCheckBox;
    eCategoryCheckBoxInfoClassList.push_back(e_cbic);

    v_cbic = new CheckBoxInfoClass();
    v_cbic->checkbox = vCategoryParameterPageCheckBox;
    vCategoryCheckBoxInfoClassList.push_back(v_cbic);

//    (*cit)->setCurrentCat((MPI_Category_Type) catKey);


    connect( e_cbic->checkbox, SIGNAL( clicked() ), this, SLOT( eDoAllOfCategorySelected() ) );

    connect( v_cbic->checkbox, SIGNAL( clicked() ), this, SLOT( vDoAllOfCategorySelected() ) );

    // start: need to update pointers in case we are at a boundary in the window
    i++;
    if( i%MAXROWS == 0 ) {
      r = -1;  // It's going to be incremented by one...
      c++;
      if( c > MAXCOLUMNS ) {
         fprintf(stderr, "There were over %d function entries.   Not all functions may be displayed.\n", MAXROWS*MAXCOLUMNS);
      }
    }
    r++;
    // end: need to update pointers in case we are at a boundary in the window


    // Loop through the MPI function class entries and print the entries that correspond to the MPI category
    // we are interested in.
    std::vector<functionMap*>::iterator it;
    for(  it = functionVector.begin(); it < functionVector.end(); it++) {

     if ( (*it)->mpiCatType == catKey) {

       vParameterPageCheckBox = new QCheckBox( big_box_w, "vParameterPageCheckBox3" );
       vParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
//       std::string v_blanks = "   ";
//       std::string v_indented_string = v_blanks +  (*it)->mpiFunctionName ;
//       printf("v_indented_string=%s\n", v_indented_string.c_str());
       vParameterPageCheckBox->setText( (*it)->mpiFunctionName.c_str() );
//       vParameterPageCheckBox->setText( v_indented_string.c_str() );
       vParameterPageFunctionListGridLayout->addWidget( vParameterPageCheckBox, r, c );
       vParameterPageCheckBox->setChecked((*it)->enabled);
       vParameterPageCheckBox->setEnabled(TRUE);
       v_cbic = new CheckBoxInfoClass();
       v_cbic->checkbox = vParameterPageCheckBox;
       vCheckBoxInfoClassList.push_back(v_cbic);

       connect( v_cbic->checkbox, SIGNAL( clicked() ), this, SLOT( vParameterPageCheckBoxSelected() ) );

    
       eParameterPageCheckBox = new QCheckBox( e_big_box_w, "eParameterPageCheckBox3" );
       eParameterPageCheckBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0, FALSE ) );
//       std::string e_blanks = "   ";
//       std::string e_indented_string = e_blanks +  (*it)->mpiFunctionName ;
//       printf("e_indented_string=%s\n", e_indented_string.c_str());
       eParameterPageCheckBox->setText( (*it)->mpiFunctionName.c_str() );
//       eParameterPageCheckBox->setText( e_indented_string.c_str() );
       eParameterPageFunctionListGridLayout->addWidget( eParameterPageCheckBox, r, c );
       eParameterPageCheckBox->setChecked((*it)->enabled);
       eParameterPageCheckBox->setEnabled(TRUE);
       e_cbic = new CheckBoxInfoClass();
       e_cbic->checkbox = eParameterPageCheckBox;
       eCheckBoxInfoClassList.push_back(e_cbic);
       connect( e_cbic->checkbox, SIGNAL( clicked() ), this, SLOT( eParameterPageCheckBoxSelected() ) );

       i++;
       if( i%MAXROWS == 0 ) {
         r = -1;  // It's going to be incremented by one...
         c++;
         if( c > MAXCOLUMNS ) {
            fprintf(stderr, "There were over %d function entries.   Not all functions may be displayed.\n", MAXROWS*MAXCOLUMNS);
         }
       }
       r++;

   } // end if catKey ==
  } // end for
 } // end catKey loop

}



void
MPIWizardPanel::handleSizeEvent(QResizeEvent *e)
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


void MPIWizardPanel::vParameterPageCheckBoxSelected()
{

  paramList.clear();
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::vParameterPageCheckBoxSelected(), CLEAR paramList.\n");
#endif
  paramString = QString::null;

#if DEBUG_CHECKBOX
   printf("vParameterPageCheckBoxSelected() entered\n");
#endif

   // Variables to hold counts of enabled functions within the MPI category
   int enabledFunctionCatCount[MPI_Cat_NULL+1];
   int disabledFunctionCatCount[MPI_Cat_NULL+1];

  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      enabledFunctionCatCount[catKey] = 0;
      disabledFunctionCatCount[catKey] = 0;
  }

  // Do initial loop to see what is set and what is not set set counts for enabled/disabled.
  // If values are either all enabled or disabled we can send down the mpi category not all the individual "enabled" functions.
  // We will need this information below
  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
     for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
         CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
         std::vector<functionMap*>::iterator it;
         for(  it = functionVector.begin(); it < functionVector.end(); it++) {
            if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
               if (func_cbic->checkbox->isChecked())  {
                    enabledFunctionCatCount[catKey] = enabledFunctionCatCount[catKey] + 1;
               } else { 
                    disabledFunctionCatCount[catKey] = disabledFunctionCatCount[catKey] + 1;
               }
            } // end if text matches cat name
          } // end for functionVector
     } // end for vCheckBoxInfo
  } // end for category loop

#if DEBUG_CHECKBOX
  cerr << "vParameterPageCheckBoxSelected(), After loop through categories the status before the button click was this:"  << endl;
  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
     cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
             " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
             " disabled function checkboxes" << endl;
  }
#endif
   // If all categories are set and all function boxes with in are enabled, send down "all" mpi functions to be traced
   bool allSet = false;
   for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      if ( enabledFunctionCatCount[catKey] > 0 && disabledFunctionCatCount[catKey] == 0) {
           allSet = true;
      } else {
           // MPI_Cat_All doesn't have any functions so don't consider it.
           if (catKey != MPI_Cat_All) {
             allSet = false;
             break;
           }
      }
   } // end for category loop

  // -----------------------------------------------------------------------------------
  // Try to read all the category checkboxes and function checkboxes to set the proper paramString
  // and paramList using the mpi category names if possible
  // -----------------------------------------------------------------------------------

   if (!allSet) {
    for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
      if (catKey != MPI_Cat_All) {

        // if all function checkboxes are set for this category then send down the category text and include the text in the paramString
        // The logic is:
        //          If all enabled then send down category name
        //          else all other cases go through the individual checkbox loop
        //                because if all not set nothing will be sent down, if some are some aren't, then the set ones will be sent down individually

        if ( enabledFunctionCatCount[catKey] > 0 && disabledFunctionCatCount[catKey] == 0) {
#if DEBUG_CHECKBOX
             cerr << "vParameterPageCheckBoxSelected(), category checkbox was true and all function checkboxes were also true in not wantedKey paramList setup loop, adding category="
                  << (*cli_cit)->mpiCategoryName.c_str() << endl;
#endif
             paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
             if( paramString.isEmpty() ) {
               paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
             } else {
               paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
             }
        } else {
        // not all function checkboxes are enabled/disabled  , we need to send down the ones that are set

          for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
              CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
              std::vector<functionMap*>::iterator it;
              for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                 if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                    if (func_cbic->checkbox->isChecked())  {

#if DEBUG_CHECKBOX
                      cerr << "vParameterPageCheckBoxSelected(), checkbox was true in not wantedKey paramList setup loop, adding (PUSH_BACK) function=" << func_cbic->checkbox->text() << endl;
#endif
                      paramList.push_back(func_cbic->checkbox->text() );
                      if( paramString.isEmpty() ) {
                        paramString += QString("%1").arg(func_cbic->checkbox->text());
                      } else {
                        paramString += QString(",%1").arg(func_cbic->checkbox->text());
                      }


                    } else {
#if DEBUG_CHECKBOX
                         cerr << "vParameterPageCheckBoxSelected(), checkbox was false in not wantedKey paramList setup loop, skipping function=" << func_cbic->checkbox->text() << endl;
#endif
                    }
                 } // end if text matches cat name
               } // end for functionVector
          } // end for vCheckBoxInfo

        } // end else enabled/disabled


       } // end not equal wantedKey

     } // end for category loop
    } // !allSet
    else {
       std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() /* + 0 */ ;
       paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
       if( paramString.isEmpty() ) {
         paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
       } else {
         paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
       }
    }



#if DEBUG_CHECKBOX
  printf("vParameterPageCheckBoxSelected() EXIT, paramString=(%s)\n", paramString.ascii() );
#endif
}



void MPIWizardPanel::eParameterPageCheckBoxSelected()
{

  paramList.clear();
#ifdef DEBUG_MPIWizard
  printf("MPIWizardPanel::eParameterPageCheckBoxSelected(), CLEAR paramList.\n");
#endif
  paramString = QString::null;

#if DEBUG_CHECKBOX
   printf("eParameterPageCheckBoxSelected() entered\n");
#endif

   // Variables to hold counts of enabled functions within the MPI category
   int enabledFunctionCatCount[MPI_Cat_NULL+1];
   int disabledFunctionCatCount[MPI_Cat_NULL+1];

  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      enabledFunctionCatCount[catKey] = 0;
      disabledFunctionCatCount[catKey] = 0;
  }

  // Do initial loop to see what is set and what is not set set counts for enabled/disabled.
  // If values are either all enabled or disabled we can send down the mpi category not all the individual "enabled" functions.
  // We will need this information below
  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
     for( CheckBoxInfoClassList::Iterator cbit = eCheckBoxInfoClassList.begin(); cbit != eCheckBoxInfoClassList.end(); ++cbit) {
         CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
         std::vector<functionMap*>::iterator it;
         for(  it = functionVector.begin(); it < functionVector.end(); it++) {
            if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
               if (func_cbic->checkbox->isChecked())  {
                    enabledFunctionCatCount[catKey] = enabledFunctionCatCount[catKey] + 1;
               } else { 
                    disabledFunctionCatCount[catKey] = disabledFunctionCatCount[catKey] + 1;
               }
            } // end if text matches cat name
          } // end for functionVector
     } // end for eCheckBoxInfo
  } // end for category loop

#if DEBUG_CHECKBOX
  cerr << "eParameterPageCheckBoxSelected(), After loop through categories the status before the button click was this:"  << endl;
  for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
     cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
             " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
             " disabled function checkboxes" << endl;
  }
#endif

  // -----------------------------------------------------------------------------------
  // Try to read all the category checkboxes and function checkboxes to set the proper paramString
  // and paramList using the mpi category names if possible
  // -----------------------------------------------------------------------------------

   for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
      if (catKey != MPI_Cat_All) {

        // if all function checkboxes are set for this category then send down the category text and include the text in the paramString
        // The logic is:
        //          If all enabled then send down category name
        //          else all other cases go through the individual checkbox loop
        //                because if all not set nothing will be sent down, if some are some aren't, then the set ones will be sent down individually

        if ( enabledFunctionCatCount[catKey] > 0 && disabledFunctionCatCount[catKey] == 0) {
#if DEBUG_CHECKBOX
             cerr << "eParameterPageCheckBoxSelected(), category checkbox was true and all function checkboxes were also true in not wantedKey paramList setup loop, PUSH_BACK category="
                  << (*cli_cit)->mpiCategoryName.c_str() << endl;
#endif
             paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
             if( paramString.isEmpty() ) {
               paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
             } else {
               paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
             }
        } else {
        // not all function checkboxes are enabled/disabled  , we need to send down the ones that are set

          for( CheckBoxInfoClassList::Iterator cbit = eCheckBoxInfoClassList.begin(); cbit != eCheckBoxInfoClassList.end(); ++cbit) {
              CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
              std::vector<functionMap*>::iterator it;
              for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                 if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                    if (func_cbic->checkbox->isChecked())  {

#if DEBUG_CHECKBOX
                      cerr << "eParameterPageCheckBoxSelected(), checkbox was true in not wantedKey paramList setup loop, PUSH_BACK function=" << func_cbic->checkbox->text() << endl;
#endif
                      paramList.push_back(func_cbic->checkbox->text() );
                      if( paramString.isEmpty() ) {
                        paramString += QString("%1").arg(func_cbic->checkbox->text());
                      } else {
                        paramString += QString(",%1").arg(func_cbic->checkbox->text());
                      }


                    } else {
#if DEBUG_CHECKBOX
                         cerr << "eParameterPageCheckBoxSelected(), checkbox was false in not wantedKey paramList setup loop, skipping function=" << func_cbic->checkbox->text() << endl;
#endif
                    }
                 } // end if text matches cat name
               } // end for functionVector
          } // end for eCheckBoxInfo

        } // end else enabled/disabled


       } // end not equal wantedKey

     } // end for category loop

#if DEBUG_CHECKBOX
  printf("eParameterPageCheckBoxSelected() EXIT, paramString=(%s)\n", paramString.ascii() );
#endif
}


void MPIWizardPanel::vDoAllOfCategorySelected()
{ 

   // Variables to hold counts of enabled functions within the MPI category
   int enabledFunctionCatCount[MPI_Cat_NULL+1];
   int disabledFunctionCatCount[MPI_Cat_NULL+1];


#if DEBUG_CHECKBOX
   printf("vDoAllOfCategorySelected() entered, ewizardMode->isOn() =%d \n", ewizardMode->isOn() );
#endif

if( ewizardMode->isOn() ) {
   return;
}

// Default to saying the check boxes were set
  bool thisCategoriesBoxCheckedValue = TRUE;

// paramList is the string that is passed to the collector.  This must have all the MPI function names
// that are to be profiled
  paramList.clear();
  paramString = QString::null;

#if DEBUG_CHECKBOX
  printf("vDoAllOfCategorySelected, paramList CLEAR\n");
#endif


  MPI_Category_Type wantedKey = MPI_Cat_NULL;

  // Loop through the category checkboxes.  There are about 8 of these.   
  // See if they are set or unset.  Then adjust the corresponding individual entries to this
  for( CheckBoxInfoClassList::Iterator it = vCategoryCheckBoxInfoClassList.begin(); it != vCategoryCheckBoxInfoClassList.end(); ++it) {
    CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;

#if DEBUG_CHECKBOX
    printf("vDoAllOfCategorySelected, cbic: (%s) == (%d)\n\n", cbic->checkbox->text().ascii(), cbic->checkbox->isChecked());
#endif


    // See which category the text corresponds to.  Then go through the list of functions and adjust their
    // checkbox setting to match that of the category checkbox setting.
    // For example if the Datatypes checkbox is disabled then find MPI_Pack and MPI_Unpack and disable their
    // checkboxes.
    //
    for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      // Handle the Category header
      std::vector<catNameMap*>::iterator last_cit = v_lastStatusCategories.begin() + catKey;
      std::vector<catNameMap*>::iterator cit = mpiCatNames.begin() + catKey;

#if DEBUG_CHECKBOX
      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
      printf("vDoAllOfCategorySelected, (*cit)->enabled =%d, (*cit)->mpiCategoryName.c_str()=%s\n", (*cit)->enabled,  (*cit)->mpiCategoryName.c_str() );
      printf("vDoAllOfCategorySelected, (*last_cit)->enabled=%d, (*last_cit)->mpiCategoryName.c_str()=%s\n", (*last_cit)->enabled, (*last_cit)->mpiCategoryName.c_str() );
      printf("vDoAllOfCategorySelected, cbic->checkbox->isChecked()=%d, catKey=%d, wantedKey=%d, cbic->checkbox->text().ascii()=%s\n\n", cbic->checkbox->isChecked(), catKey, wantedKey, cbic->checkbox->text().ascii() );
      printf("vDoAllOfCategorySelected, (*cli_cit)->enabled =%d, (*cli_cit)->mpiCategoryName.c_str()=%s\n", (*cli_cit)->enabled,  (*cli_cit)->mpiCategoryName.c_str() );
#endif


      if ( cbic->checkbox->text() == (*cit)->mpiCategoryName && (*last_cit)->enabled != cbic->checkbox->isChecked() ) {

          wantedKey = (MPI_Category_Type) catKey;
          // Set a global boolean with the value of this categories check box 
          if(cbic->checkbox->isChecked() ) {
            thisCategoriesBoxCheckedValue = TRUE;
          } else {
            thisCategoriesBoxCheckedValue = FALSE;
          }

#if DEBUG_CHECKBOX
          printf("vDoAllOfCategorySelected() FOUND wantedKey=%d, cbic->checkbox->text().ascii()=%s, thisCategoriesBoxCheckedValue=%d\n", 
                  wantedKey, cbic->checkbox->text().ascii(), thisCategoriesBoxCheckedValue );
#endif
          break;
        }
     } // end for catKey

   
    //
    // -------------------------------------------------------------------------------
    // If we have a valid wantedKey we loop through the MPI function class entries and 
    // adjust the entries that correspond to the MPI category we are interested in.
    // We handle the ALL case separately because it is the simplest and can be streamlined
    // 
    // We handle the other categories in a separate if block
    //   In that block we do some preliminary work to see if all the function checkboxes
    //   are enabled or disabled.  That allows us to make some quick decisions about 
    //   sending down to the cli the mpi category names or sending down individual
    //   function names.  mpi category names are preferred.
    //
    // Another thing to look for is the v_lastStatusCategories variable that is the 
    // key to telling which category button was actually selected in the wizard
    // We don't get the exact button, just that one was clicked, so we need to figure
    // that out by keeping the previous state of the button settings.
    // -------------------------------------------------------------------------------
    //

    if (wantedKey != MPI_Cat_NULL) {

     if (wantedKey == MPI_Cat_All) {

        int keyIndx = 0;
        // loop through all function and category checkboxes setting false or true and exit
        for( CheckBoxInfoClassList::Iterator it = vCategoryCheckBoxInfoClassList.begin(); it != vCategoryCheckBoxInfoClassList.end(); ++it) {
            CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;
            cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;
#if DEBUG_CHECKBOX
            printf("vDoAllOfCategorySelected() setting all category boxes enabled/disabled, thisCategoriesBoxCheckedValue=%d, category=%s\n", thisCategoriesBoxCheckedValue, cbic->checkbox->text().ascii() );
#endif
             std::vector<catNameMap*>::iterator last_cat_it = v_lastStatusCategories.begin() + keyIndx;
             (*last_cat_it)->enabled = thisCategoriesBoxCheckedValue;
             keyIndx = keyIndx + 1;
        } // for

        for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
            CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
            func_cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;  
#if DEBUG_CHECKBOX
           printf("vDoAllOfCategorySelected() setting all function boxes enabled/disabled, thisCategoriesBoxCheckedValue=%d, function=%s\n", thisCategoriesBoxCheckedValue, func_cbic->checkbox->text().ascii() );
#endif
        } // for

        // Set the MPI Category in for all
        std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + wantedKey;
        if (thisCategoriesBoxCheckedValue) {
          paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
#if DEBUG_CHECKBOX
         printf("vDoAllOfCategorySelected() PUSH_BACK all category=%s\n", (*cli_cit)->mpiCategoryName.c_str() );
#endif
          if( paramString.isEmpty() ) {
            paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
          } else {
            paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
          }
        }

     } else {

        // All other categories other than MPI_Cat_All come through here

        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
             enabledFunctionCatCount[catKey] = 0;
             disabledFunctionCatCount[catKey] = 0;
        }

        // Do initial loop to see what is set and what is not set set counts for enabled/disabled.
        // If values are either all enabled or disabled we can send down the mpi category not all the individual "enabled" functions.
        // We will need this information below
        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
           for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
               CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
               std::vector<functionMap*>::iterator it;
               for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                  if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                     if (func_cbic->checkbox->isChecked())  {
                          enabledFunctionCatCount[catKey] = enabledFunctionCatCount[catKey] + 1;
                     } else { 
                          disabledFunctionCatCount[catKey] = disabledFunctionCatCount[catKey] + 1;
                     }
                  } // end if text matches cat name
                } // end for functionVector
           } // end for vCheckBoxInfo
        } // end for category loop

#if DEBUG_CHECKBOX
       cerr << "After loop through categories the status before the button click was this:"  << endl;
       for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
          cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
                  " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
                  " disabled function checkboxes" << endl;
       }
#endif

      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + wantedKey;
      std::vector<functionMap*>::iterator it;
      bool category_pushed = false;


      // Now the wantedKey category was the one selected.  Update the function checkboxes and update the paramString, paramList
      //
      if ( thisCategoriesBoxCheckedValue == TRUE ) {
         //  update the enabled and disabled function counts
         enabledFunctionCatCount[wantedKey] = 99;
         disabledFunctionCatCount[wantedKey] = 0;
         paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
#if DEBUG_CHECKBOX
         printf("vDoAllOfCategorySelected() PUSH_BACK wantedKey category=%s\n", (*cli_cit)->mpiCategoryName.c_str() );
#endif
         if( paramString.isEmpty() ) {
           paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
         } else {
           paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
         }
            
      } else {
         enabledFunctionCatCount[wantedKey] = 0;
         disabledFunctionCatCount[wantedKey] = 99;
      }

      std::vector<catNameMap*>::iterator last_cit = v_lastStatusCategories.begin() + wantedKey;
      (*last_cit)->enabled = thisCategoriesBoxCheckedValue;

#if DEBUG_CHECKBOX
       cerr << "the status after the button click update of the selected category was this:"  << endl;
       for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
          cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
                  " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
                  " disabled function checkboxes" << endl;
       }
#endif

      // Now set the checkboxes correctly for wantedKey MPI category
      for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
           CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
           std::vector<functionMap*>::iterator it;
           for(  it = functionVector.begin(); it < functionVector.end(); it++) {
              if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == wantedKey ) {
#if DEBUG_CHECKBOX
                 cerr << "Setting the checkbox for the wantedKey function loop, setting value=" << thisCategoriesBoxCheckedValue << " for function=" << func_cbic->checkbox->text() << endl;
#endif
                 func_cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;
              } // end if text matches cat name
            } // end for functionVector
       } // end for vCheckBoxInfo
        
       // -----------------------------------------------------------------------------------
       // Now handle the other functions and category checkboxes that weren't the ones selected
       // -----------------------------------------------------------------------------------

        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
           std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
           if (catKey != wantedKey) {

             // if all function checkboxes are set for this category then send down the category text and include the text in the paramString
             // The logic is: 
             //          If all enabled then send down category name
             //          else all other cases go through the individual checkbox loop
             //                because if all not set nothing will be sent down, if some are some aren't, then the set ones will be sent down individually

             if ( enabledFunctionCatCount[catKey] > 0 && disabledFunctionCatCount[catKey] == 0) {
#if DEBUG_CHECKBOX
                  cerr << " category checkbox was true and all function checkboxes were also true in not wantedKey paramList setup loop, PUSH_BACK category=" 
                       << (*cli_cit)->mpiCategoryName.c_str() << endl;
#endif
                  paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
                  if( paramString.isEmpty() ) {
                    paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
                  } else {
                    paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
                  }
             } else {
             // not all function checkboxes are enabled/disabled  , we need to send down the ones that are set           

               for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
                   CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
                   std::vector<functionMap*>::iterator it;
                   for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                      if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                         if (func_cbic->checkbox->isChecked())  {

#if DEBUG_CHECKBOX
                           cerr << " checkbox was true in not wantedKey paramList setup loop, PUSH_BACK function=" << func_cbic->checkbox->text() << endl;
#endif
                           paramList.push_back(func_cbic->checkbox->text() );
                           if( paramString.isEmpty() ) {
                             paramString += QString("%1").arg(func_cbic->checkbox->text());
                           } else {
                             paramString += QString(",%1").arg(func_cbic->checkbox->text());
                           }


                         } else { 
#if DEBUG_CHECKBOX
                              cerr << " checkbox was false in not wantedKey paramList setup loop, skipping function=" << func_cbic->checkbox->text() << endl;
#endif
                         }
                      } // end if text matches cat name
                    } // end for functionVector
               } // end for vCheckBoxInfo

             } // end else enabled/disabled


            } // end not equal wantedKey

          } // end for category loop


      } // else MPI_Cat_All
     } // not NULL

    if (wantedKey != MPI_Cat_NULL) {
        // only go through this loop once
        break;
    }
  } // end for

#if DEBUG_CHECKBOX
  printf("vDoAllOfCategorySelected() after loop, wantedKey=%d, paramString=(%s)\n", wantedKey, paramString.ascii() );
#endif

  if (wantedKey != MPI_Cat_NULL) {

//    std::vector<catNameMap*>::iterator last_cit = v_lastStatusCategories.begin() + wantedKey;

    // This was the entry that changed last time.  Update it to the opposite value in the last status entry.
 //   (*last_cit)->enabled = !(*last_cit)->enabled;

  } else {
#if DEBUG_CHECKBOX
   printf("vDoAllOfCategorySelected() after loop, PROBLEMS, wantedKey=%d \n", wantedKey);
#endif
  }

}



void MPIWizardPanel::eDoAllOfCategorySelected()
{ 

   // Variables to hold counts of enabled functions within the MPI category
   int enabledFunctionCatCount[MPI_Cat_NULL+1];
   int disabledFunctionCatCount[MPI_Cat_NULL+1];


#if DEBUG_CHECKBOX
   printf("eDoAllOfCategorySelected() entered, vwizardMode->isOn() =%d \n", vwizardMode->isOn() );
#endif

if( vwizardMode->isOn() ) {
   return;
}

// Default to saying the check boxes were set
  bool thisCategoriesBoxCheckedValue = TRUE;

// paramList is the string that is passed to the collector.  This must have all the MPI function names
// that are to be profiled
  paramList.clear();
#if DEBUG_CHECKBOX
  printf("eDoAllOfCategorySelected, paramList CLEAR\n");
#endif
  paramString = QString::null;

  MPI_Category_Type wantedKey = MPI_Cat_NULL;

  // Loop through the category checkboxes.  There are about 8 of these.   
  // See if they are set or unset.  Then adjust the corresponding individual entries to this
  for( CheckBoxInfoClassList::Iterator it = eCategoryCheckBoxInfoClassList.begin(); it != eCategoryCheckBoxInfoClassList.end(); ++it) {
    CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;

#if DEBUG_CHECKBOX
    printf("eDoAllOfCategorySelected, cbic: (%s) == (%d)\n\n", cbic->checkbox->text().ascii(), cbic->checkbox->isChecked());
#endif


    // See which category the text corresponds to.  Then go through the list of functions and adjust their
    // checkbox setting to match that of the category checkbox setting.
    // For example if the Datatypes checkbox is disabled then find MPI_Pack and MPI_Unpack and disable their
    // checkboxes.
    //
    for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
      // Handle the Category header
      std::vector<catNameMap*>::iterator last_cit = e_lastStatusCategories.begin() + catKey;
      std::vector<catNameMap*>::iterator cit = mpiCatNames.begin() + catKey;

#if DEBUG_CHECKBOX
      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
      printf("eDoAllOfCategorySelected, (*cit)->enabled =%d, (*cit)->mpiCategoryName.c_str()=%s\n", (*cit)->enabled,  (*cit)->mpiCategoryName.c_str() );
      printf("eDoAllOfCategorySelected, (*last_cit)->enabled=%d, (*last_cit)->mpiCategoryName.c_str()=%s\n", (*last_cit)->enabled, (*last_cit)->mpiCategoryName.c_str() );
      printf("eDoAllOfCategorySelected, cbic->checkbox->isChecked()=%d, catKey=%d, wantedKey=%d, cbic->checkbox->text().ascii()=%s\n\n", cbic->checkbox->isChecked(), catKey, wantedKey, cbic->checkbox->text().ascii() );
      printf("eDoAllOfCategorySelected, (*cli_cit)->enabled =%d, (*cli_cit)->mpiCategoryName.c_str()=%s\n", (*cli_cit)->enabled,  (*cli_cit)->mpiCategoryName.c_str() );
#endif


      if ( cbic->checkbox->text() == (*cit)->mpiCategoryName && (*last_cit)->enabled != cbic->checkbox->isChecked() ) {

          wantedKey = (MPI_Category_Type) catKey;
          // Set a global boolean with the value of this categories check box 
          if(cbic->checkbox->isChecked() ) {
            thisCategoriesBoxCheckedValue = TRUE;
          } else {
            thisCategoriesBoxCheckedValue = FALSE;
          }

#if DEBUG_CHECKBOX
          printf("eDoAllOfCategorySelected() FOUND wantedKey=%d, cbic->checkbox->text().ascii()=%s, thisCategoriesBoxCheckedValue=%d\n", 
                  wantedKey, cbic->checkbox->text().ascii(), thisCategoriesBoxCheckedValue );
#endif
          break;
        }
     } // end for catKey

   
    //
    // -------------------------------------------------------------------------------
    // If we have a valid wantedKey we loop through the MPI function class entries and 
    // adjust the entries that correspond to the MPI category we are interested in.
    // We handle the ALL case separately because it is the simplest and can be streamlined
    // 
    // We handle the other categories in a separate if block
    //   In that block we do some preliminary work to see if all the function checkboxes
    //   are enabled or disabled.  That allows us to make some quick decisions about 
    //   sending down to the cli the mpi category names or sending down individual
    //   function names.  mpi category names are preferred.
    //
    // Another thing to look for is the e_lastStatusCategories variable that is the 
    // key to telling which category button was actually selected in the wizard
    // We don't get the exact button, just that one was clicked, so we need to figure
    // that out by keeping the previous state of the button settings.
    // -------------------------------------------------------------------------------
    //

    if (wantedKey != MPI_Cat_NULL) {

     if (wantedKey == MPI_Cat_All) {

        int keyIndx = 0;
        // loop through all function and category checkboxes setting false or true and exit
        for( CheckBoxInfoClassList::Iterator it = eCategoryCheckBoxInfoClassList.begin(); it != eCategoryCheckBoxInfoClassList.end(); ++it) {
            CheckBoxInfoClass *cbic = (CheckBoxInfoClass *)*it;
            cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;
#if DEBUG_CHECKBOX
            printf("eDoAllOfCategorySelected() setting all category boxes enabled/disabled, thisCategoriesBoxCheckedValue=%d, category=%s\n", thisCategoriesBoxCheckedValue, cbic->checkbox->text().ascii() );
#endif
             std::vector<catNameMap*>::iterator last_cat_it = e_lastStatusCategories.begin() + keyIndx;
             (*last_cat_it)->enabled = thisCategoriesBoxCheckedValue;
             keyIndx = keyIndx + 1;
        } // for

        for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
            CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
            func_cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;  
#if DEBUG_CHECKBOX
           printf("eDoAllOfCategorySelected() setting all function boxes enabled/disabled, thisCategoriesBoxCheckedValue=%d, function=%s\n", thisCategoriesBoxCheckedValue, func_cbic->checkbox->text().ascii() );
#endif
        } // for

        // Set the MPI Category in for all
        std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + wantedKey;
        if (thisCategoriesBoxCheckedValue) {
#if DEBUG_CHECKBOX
          cerr << " eDoAllOfCategorySelected(), PUSH_BACK all category=" << (*cli_cit)->mpiCategoryName.c_str() << endl;
#endif
          paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
          if( paramString.isEmpty() ) {
            paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
          } else {
            paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
          }
        }

     } else {

        // All other categories other than MPI_Cat_All come through here

        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
             enabledFunctionCatCount[catKey] = 0;
             disabledFunctionCatCount[catKey] = 0;
        }

        // Do initial loop to see what is set and what is not set set counts for enabled/disabled.
        // If values are either all enabled or disabled we can send down the mpi category not all the individual "enabled" functions.
        // We will need this information below
        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
           for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
               CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
               std::vector<functionMap*>::iterator it;
               for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                  if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                     if (func_cbic->checkbox->isChecked())  {
                          enabledFunctionCatCount[catKey] = enabledFunctionCatCount[catKey] + 1;
                     } else { 
                          disabledFunctionCatCount[catKey] = disabledFunctionCatCount[catKey] + 1;
                     }
                  } // end if text matches cat name
                } // end for functionVector
           } // end for vCheckBoxInfo
        } // end for category loop

#if DEBUG_CHECKBOX
       cerr << "After loop through categories the status before the button click was this:"  << endl;
       for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
          cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
                  " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
                  " disabled function checkboxes" << endl;
       }
#endif

      std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + wantedKey;
      std::vector<functionMap*>::iterator it;
      bool category_pushed = false;


      // Now the wantedKey category was the one selected.  Update the function checkboxes and update the paramString, paramList
      //
      if ( thisCategoriesBoxCheckedValue == TRUE ) {
         //  update the enabled and disabled function counts
         enabledFunctionCatCount[wantedKey] = 99;
         disabledFunctionCatCount[wantedKey] = 0;
         paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
         if( paramString.isEmpty() ) {
           paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
         } else {
           paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
         }
            
      } else {
         enabledFunctionCatCount[wantedKey] = 0;
         disabledFunctionCatCount[wantedKey] = 99;
      }

      std::vector<catNameMap*>::iterator last_cit = e_lastStatusCategories.begin() + wantedKey;
      (*last_cit)->enabled = thisCategoriesBoxCheckedValue;

#if DEBUG_CHECKBOX
       cerr << "the status after the button click update of the selected category was this:"  << endl;
       for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
          cerr << "for catKey=" << catKey << " there are " <<  enabledFunctionCatCount[catKey] << 
                  " enabled function checkboxes and " << disabledFunctionCatCount[catKey] << 
                  " disabled function checkboxes" << endl;
       }
#endif

      // Now set the checkboxes correctly for wantedKey MPI category
      for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
           CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
           std::vector<functionMap*>::iterator it;
           for(  it = functionVector.begin(); it < functionVector.end(); it++) {
              if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == wantedKey ) {
#if DEBUG_CHECKBOX
                 cerr << "Setting the checkbox for the wantedKey function loop, setting value=" << thisCategoriesBoxCheckedValue << " for function=" << func_cbic->checkbox->text() << endl;
#endif
                 func_cbic->checkbox->setChecked(thisCategoriesBoxCheckedValue) ;
              } // end if text matches cat name
            } // end for functionVector
       } // end for vCheckBoxInfo
        
       // -----------------------------------------------------------------------------------
       // Now handle the other functions and category checkboxes that weren't the ones selected
       // -----------------------------------------------------------------------------------

        for (int catKey = MPI_Cat_All; catKey < MPI_Cat_NULL;  catKey++) {
           std::vector<catNameMap*>::iterator cli_cit = sendToCliMpiCatNames.begin() + catKey;
           if (catKey != wantedKey) {

             // if all function checkboxes are set for this category then send down the category text and include the text in the paramString
             // The logic is: 
             //          If all enabled then send down category name
             //          else all other cases go through the individual checkbox loop
             //                because if all not set nothing will be sent down, if some are some aren't, then the set ones will be sent down individually

             if ( enabledFunctionCatCount[catKey] > 0 && disabledFunctionCatCount[catKey] == 0) {
#if DEBUG_CHECKBOX
                  cerr << " category checkbox was true and all function checkboxes were also true in not wantedKey paramList setup loop, adding category=" 
                       << (*cli_cit)->mpiCategoryName.c_str() << endl;
#endif
                  paramList.push_back((*cli_cit)->mpiCategoryName.c_str());
                  if( paramString.isEmpty() ) {
                    paramString += QString("%1").arg((*cli_cit)->mpiCategoryName);
                  } else {
                    paramString += QString(",%1").arg((*cli_cit)->mpiCategoryName);
                  }
             } else {
             // not all function checkboxes are enabled/disabled  , we need to send down the ones that are set           

               for( CheckBoxInfoClassList::Iterator cbit = vCheckBoxInfoClassList.begin(); cbit != vCheckBoxInfoClassList.end(); ++cbit) {
                   CheckBoxInfoClass *func_cbic = (CheckBoxInfoClass *)*cbit;
                   std::vector<functionMap*>::iterator it;
                   for(  it = functionVector.begin(); it < functionVector.end(); it++) {
                      if (func_cbic->checkbox->text() == (*it)->mpiFunctionName && (*it)->mpiCatType == catKey ) {
                         if (func_cbic->checkbox->isChecked())  {

#if DEBUG_CHECKBOX
                           cerr << " checkbox was true in not wantedKey paramList setup loop, adding function=" << func_cbic->checkbox->text() << endl;
#endif
                           paramList.push_back(func_cbic->checkbox->text() );
                           if( paramString.isEmpty() ) {
                             paramString += QString("%1").arg(func_cbic->checkbox->text());
                           } else {
                             paramString += QString(",%1").arg(func_cbic->checkbox->text());
                           }


                         } else { 
#if DEBUG_CHECKBOX
                              cerr << " checkbox was false in not wantedKey paramList setup loop, skipping function=" << func_cbic->checkbox->text() << endl;
#endif
                         }
                      } // end if text matches cat name
                    } // end for functionVector
               } // end for vCheckBoxInfo

             } // end else enabled/disabled


            } // end not equal wantedKey

          } // end for category loop


      } // else MPI_Cat_All
     } // not NULL

    if (wantedKey != MPI_Cat_NULL) {
        // only go through this loop once
        break;
    }
  } // end for

#if DEBUG_CHECKBOX
  printf("eDoAllOfCategorySelected() after loop, wantedKey=%d, paramString=(%s)\n", wantedKey, paramString.ascii() );
#endif

  if (wantedKey != MPI_Cat_NULL) {

//    std::vector<catNameMap*>::iterator last_cit = e_lastStatusCategories.begin() + wantedKey;

    // This was the entry that changed last time.  Update it to the opposite value in the last status entry.
 //   (*last_cit)->enabled = !(*last_cit)->enabled;

  } else {
#if DEBUG_CHECKBOX
   printf("eDoAllOfCategorySelected() after loop, PROBLEMS, wantedKey=%d \n", wantedKey);
#endif
  }

}


