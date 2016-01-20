////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2016 Krell Institute  All Rights Reserved.
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
  
//#define DEBUG_PREF 1

#include "preferencesdialog.hxx"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qscrollview.h>
#include <qvbox.h>


#include "preferencesdialog.ui.hxx"

/*
 *  Constructs a PreferencesDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
   panelContainer = (PanelContainer *)parent;

   globalFontFamily = "Bitstream Charter";
   globalFontPointSize = 10;
   globalFontWeight = QFont::Normal;
   globalFontItalic = FALSE;

   // For the views 
   viewDateTimePrecision = 3;
   viewFieldSizeIsDynamic = TRUE;
   saveViewsForReuse = FALSE;
   autoCreateDerivedMetrics = TRUE;
   saveViewsTime = 10;

   viewFieldSize = 10; 
   viewPrecision = 6; 
   historyLimit = 100; 
   historyDefault = 24; 
   maxAsyncCommands = 20; 
   helpLevelDefault = 2; 
   viewFullPath = FALSE;  
   saveExperimentDatabase = TRUE; 
   viewMangledName = FALSE; 
   allowPythonCommands = TRUE; 
   askAboutChangingArgs = TRUE; 
   askAboutSavingTheDatabase = TRUE; 
   onRerunSaveCopyOfExperimentDatabase = TRUE; 
   instrumentorIsOffline = TRUE; 
   lessRestrictiveComparisons = TRUE;
   viewBlankInPlaceOfZero = FALSE;


   globalRemoteShell = "/usr/bin/rsh";

    if ( !name )
	setName( "PreferencesDialog" );

    setSizeGripEnabled( TRUE );
    PreferencesDialogLayout = new QVBoxLayout( this, 11, 6, "PreferencesDialogLayout"); 

    preferenceDialogListLayout = new QVBoxLayout( 0, 0, 6, "preferenceDialogListLayout"); 

/*  

    These are the values as of 2/3/2009 from QT 3 for the QSizePolicy::SizeType:

      QSizePolicy::Fixed=0
      QSizePolicy::Minimum=1
      QSizePolicy::Ignored=2
      QSizePolicy::MinimumExpanding=3
      QSizePolicy::Maximum=4
      QSizePolicy::Preferred=5
      QSizePolicy::Expanding=7

*/

#if 0
    printf("QSizePolicy::Fixed=%d\n", QSizePolicy::Fixed);
    printf("QSizePolicy::Minimum=%d\n", QSizePolicy::Minimum);
    printf("QSizePolicy::Maximum=%d\n", QSizePolicy::Maximum);
    printf("QSizePolicy::Preferred=%d\n", QSizePolicy::Preferred);
    printf("QSizePolicy::Expanding=%d\n", QSizePolicy::Expanding);
    printf("QSizePolicy::MinimumExpanding=%d\n", QSizePolicy::MinimumExpanding);
    printf("QSizePolicy::Ignored=%d\n", QSizePolicy::Ignored);
#endif

    mainSplitter = new QSplitter( this, "mainSplitter" );
    mainSplitter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, mainSplitter->sizePolicy().hasHeightForWidth() ) );
    mainSplitter->setOrientation( QSplitter::Horizontal );
    mainSplitter->setChildrenCollapsible( FALSE );

    preferenceDialogLeftFrame = new QFrame( mainSplitter, "preferenceDialogLeftFrame" );
    preferenceDialogLeftFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogLeftFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrame->setMinimumSize( QSize( 195, 10 ) );
    preferenceDialogLeftFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogLeftFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogLeftFrameLayout = new QVBoxLayout( preferenceDialogLeftFrame, 11, 6, "preferenceDialogLeftFrameLayout"); 

    categoryListView = new QListView( preferenceDialogLeftFrame, "categoryListView" );
    categoryListView->addColumn( tr( "Categories" ) );
    // Don't sort this list... Add the General first and the rest as they
    // come.   If you're going to sort, then sort ascending, alphabetic, but
    // leave the "General" category at the top of the list.
    categoryListView->setSortColumn( -1 );
    // There's only one column header, hide it unless its shows up as a problem
    // during usability studies.
    categoryListView->header()->hide();

    categoryListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)7, 0, 0, categoryListView->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrameLayout->addWidget( categoryListView );

    preferenceDialogRightFrame = new QFrame( mainSplitter, "preferenceDialogRightFrame" );
//    preferenceDialogRightFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, preferenceDialogRightFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogRightFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogRightFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogRightFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogRightFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogRightFrameLayout = new QVBoxLayout( preferenceDialogRightFrame, 11, 6, "preferenceDialogRightFrameLayout"); 

    preferenceDialogWidgetStack = new QWidgetStack( preferenceDialogRightFrame, "preferenceDialogWidgetStack" );
    preferenceDialogWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogWidgetStack->sizePolicy().hasHeightForWidth() ) );
//    preferenceDialogWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, preferenceDialogWidgetStack->sizePolicy().hasHeightForWidth() ) );


    createGeneralStackPage(preferenceDialogWidgetStack, (char *) "General" );

    preferenceDialogRightFrameLayout->addWidget( preferenceDialogWidgetStack );
    preferenceDialogListLayout->addWidget( mainSplitter );

    preferenceDialogWidgetStackLayout = new QHBoxLayout( 0, 0, 6, "preferenceDialogWidgetStackLayout"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    preferenceDialogWidgetStackLayout->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    preferenceDialogWidgetStackLayout->addItem( Horizontal_Spacing2 );

    buttonDefaults = new QPushButton( this, "buttonDefaults" );
    preferenceDialogWidgetStackLayout->addWidget( buttonDefaults );

    buttonApply = new QPushButton( this, "buttonApply" );
    buttonApply->setAutoDefault( TRUE );
    buttonApply->setDefault( TRUE );
    preferenceDialogWidgetStackLayout->addWidget( buttonApply );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    preferenceDialogWidgetStackLayout->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    preferenceDialogWidgetStackLayout->addWidget( buttonCancel );
    preferenceDialogListLayout->addLayout( preferenceDialogWidgetStackLayout );
    PreferencesDialogLayout->addLayout( preferenceDialogListLayout );
    languageChange();
    resize( QSize(620, 390).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonDefaults, SIGNAL( clicked() ), this,
      SLOT( resetPreferenceDefaults() ) );
    connect( buttonApply, SIGNAL( clicked() ), this,
      SLOT( buttonApplySelected() ) );
    connect( buttonOk, SIGNAL( clicked() ), this,
      SLOT( buttonOkSelected() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this,
      SLOT( reject() ) );
    connect( setFontButton, SIGNAL( clicked() ), this,
      SLOT( selectGlobalFont() ) );
    connect( categoryListView, SIGNAL( clicked(QListViewItem*) ), this,
      SLOT( listItemSelected(QListViewItem*) ) );

    preferencesAvailable = FALSE;

    settings = new QSettings();
// printf("settings initailize to 0x%x\n", settings );
//    settings->insertSearchPath( QSettings::Unix, "openspeedshop" );

    readPreferencesOnEntry();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PreferencesDialog::~PreferencesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void
PreferencesDialog::createGeneralStackPage(QWidgetStack* stack, char *name )
{
    generalStackPage = new QWidget( stack, name );
    generalStackPageLayout = new QVBoxLayout( generalStackPage, 11, 6, "generalStackPageLayout"); 

    vpage0sv = new QScrollView( generalStackPage, "vpage0sv" );
    vpage0sv->setSizePolicy(QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 0, 0, FALSE ) );

#if DEBUG_PREF
    // match this up with the real call above to see what qt thinks the settings are 
    QSizePolicy *qscrollview_policy = NULL;
    qscrollview_policy = new QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 0, 0, FALSE );

    printf("qscrollview_policy->mayGrowHorizontally()=%d\n", qscrollview_policy->mayGrowHorizontally() );
    printf("qscrollview_policy->mayGrowVertically()=%d\n", qscrollview_policy->mayGrowVertically() );
    printf("qscrollview_policy->horStretch()=%d\n", qscrollview_policy->horStretch() );
    printf("qscrollview_policy->verStretch()=%d\n", qscrollview_policy->verStretch() );
    printf("qscrollview_policy->verData()=%d\n", qscrollview_policy->verData() );
    printf("qscrollview_policy->horData()=%d\n", qscrollview_policy->horData() );
#endif

    vpage0big_box = new QGroupBox( vpage0sv->viewport(), "vpage0big_box" );
    vpage0big_box->setColumnLayout(0, Qt::Vertical );
    vpage0big_box->layout()->setSpacing( 6 );
    vpage0big_box->layout()->setMargin( 11 );
//    vpage0big_box->layout()->setResizeMode(QLayout::Minimum);

    rightSideLayout = new QVBoxLayout( vpage0big_box->layout(), 11, "rightSideLayout"); 
    vpage0sv->addChild(vpage0big_box);

#if 0
    GeneralGroupBox = new QGroupBox( generalStackPage, "GeneralGroupBox" );
    GeneralGroupBox->setColumnLayout(0, Qt::Vertical );
    GeneralGroupBox->layout()->setSpacing( 6 );
    GeneralGroupBox->layout()->setMargin( 11 );
    rightSideLayout = new QVBoxLayout( GeneralGroupBox->layout(), 11, "rightSideLayout"); 
#endif

    fontLayout = new QHBoxLayout( 0, 0, 6, "fontLayout"); 

    setFontButton = new QPushButton( vpage0big_box, "setFontButton" );
    fontLayout->addWidget( setFontButton );

    fontLineEdit = new QLineEdit( vpage0big_box, "fontLineEdit" );
    fontLayout->addWidget( fontLineEdit );
    rightSideLayout->addLayout( fontLayout );

    remoteShellLayout = new QHBoxLayout( 0, 0, 6, "remoteShellLayout"); 
    remoteShellLabel = new QLabel( vpage0big_box, "remote shell label" );
    remoteShellLayout->addWidget( remoteShellLabel );

    const QColor vpage0color = remoteShellLabel->paletteBackgroundColor();
    vpage0sv->viewport()->setBackgroundColor(vpage0color);
    vpage0sv->viewport()->setPaletteBackgroundColor(vpage0color);



    remoteShellEdit = new QLineEdit( vpage0big_box, "remoteShellEdit" );
    remoteShellLayout->addWidget( remoteShellEdit );
    rightSideLayout->addLayout( remoteShellLayout );

    setShowSplashScreenCheckBox = new QCheckBox( vpage0big_box, "setShowSplashScreenCheckBox" );
    setShowSplashScreenCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, setShowSplashScreenCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( setShowSplashScreenCheckBox );

    showGraphicsCheckBox = new QCheckBox( vpage0big_box, "showGraphicsCheckBox" );
    showGraphicsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showGraphicsCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( showGraphicsCheckBox );

    { // VIEW_FIELD_SIZE
    viewFieldSizeLayout = new QHBoxLayout( 0, 0, 6, "viewFieldSizeLayout");
    viewFieldSizeLabel = new QLabel( vpage0big_box, "viewFieldSizeLabel" );
    viewFieldSizeLabel->setText("View field size:");
    viewFieldSizeLayout->addWidget( viewFieldSizeLabel );
    viewFieldSizeLineEdit = new QLineEdit( vpage0big_box, "viewFieldSizeLineEdit" );
    viewFieldSizeLineEdit->setText("123");
    viewFieldSizeLineEdit->setValidator( new QIntValidator( 1, 99999, viewFieldSizeLineEdit ) );
    viewFieldSizeLayout->addWidget( viewFieldSizeLineEdit );
    rightSideLayout->addLayout( viewFieldSizeLayout );
    }
    { // VIEW_PRECISION
    viewPrecisionLayout = new QHBoxLayout( 0, 0, 6, "viewPrecisionLayout");
    viewPrecisionLabel = new QLabel( vpage0big_box, "viewPrecisionLabel" );
    viewPrecisionLabel->setText("View Precision:");
    viewPrecisionLayout->addWidget( viewPrecisionLabel );
    viewPrecisionLineEdit = new QLineEdit( vpage0big_box, "viewPrecisionLineEdit" );
    viewPrecisionLineEdit->setText("123");
    viewPrecisionLineEdit->setValidator( new QIntValidator( 0, 99999, viewPrecisionLineEdit ) );
    viewPrecisionLayout->addWidget( viewPrecisionLineEdit );
    rightSideLayout->addLayout( viewPrecisionLayout );
    }
    { // HISTORY_LIMIT
    historyLimitLayout = new QHBoxLayout( 0, 0, 6, "historyLimitLayout");
    historyLimitLabel = new QLabel( vpage0big_box, "historyLimitLabel" );
    historyLimitLabel->setText("History Limit:");
    historyLimitLayout->addWidget( historyLimitLabel );
    historyLimitLineEdit = new QLineEdit( vpage0big_box, "historyLimitLineEdit" );
    historyLimitLineEdit->setText("123");
    historyLimitLineEdit->setValidator( new QIntValidator( 0, 99999, historyLimitLineEdit ) );
    historyLimitLayout->addWidget( historyLimitLineEdit );
    rightSideLayout->addLayout( historyLimitLayout );
    }
    { // HISTORY_DEFAULT
    historyDefaultLayout = new QHBoxLayout( 0, 0, 6, "historyDefaultLayout");
    historyDefaultLabel = new QLabel( vpage0big_box, "historyDefaultLabel" );
    historyDefaultLabel->setText("History Default:");
    historyDefaultLayout->addWidget( historyDefaultLabel );
    historyDefaultLineEdit = new QLineEdit( vpage0big_box, "historyDefaultLineEdit" );
    historyDefaultLineEdit->setText("123");
    historyDefaultLineEdit->setValidator( new QIntValidator( 0, 99999, historyDefaultLineEdit ) );
    historyDefaultLayout->addWidget( historyDefaultLineEdit );
    rightSideLayout->addLayout( historyDefaultLayout );
    }
    { // MAX_ASYNC_COMMANDS
    maxAsyncCommandsLayout = new QHBoxLayout( 0, 0, 6, "maxAsyncCommandsLayout");
    maxAsyncCommandsLabel = new QLabel( vpage0big_box, "maxAsyncCommandsLabel" );
    maxAsyncCommandsLabel->setText("Max Async Commands:");
    maxAsyncCommandsLayout->addWidget( maxAsyncCommandsLabel );
    maxAsyncCommandsLineEdit = new QLineEdit( vpage0big_box, "maxAsyncCommandsLineEdit" );
    maxAsyncCommandsLineEdit->setText("123");
    maxAsyncCommandsLineEdit->setValidator( new QIntValidator( 1, 99999, maxAsyncCommandsLineEdit ) );
    maxAsyncCommandsLayout->addWidget( maxAsyncCommandsLineEdit );
    rightSideLayout->addLayout( maxAsyncCommandsLayout );
    }
    { // HELP_LEVEL_DEFAULT
    helpLevelDefaultLayout = new QHBoxLayout( 0, 0, 6, "helpLevelDefaultLayout");
    helpLevelDefaultLabel = new QLabel( vpage0big_box, "helpLevelDefaultLabel" );
    helpLevelDefaultLabel->setText("Help Level Default:");
    helpLevelDefaultLayout->addWidget( helpLevelDefaultLabel );
    helpLevelDefaultLineEdit = new QLineEdit( vpage0big_box, "helpLevelDefaultLineEdit" );
    helpLevelDefaultLineEdit->setValidator( new QIntValidator( 0, 99999, helpLevelDefaultLineEdit ) );
    helpLevelDefaultLineEdit->setText("123");
    helpLevelDefaultLayout->addWidget( helpLevelDefaultLineEdit );
    rightSideLayout->addLayout( helpLevelDefaultLayout );
    }

    { // VIEW_FULLPATH
    viewFullPathCheckBox = new QCheckBox( vpage0big_box, "viewFullPathCheckBox" );
    viewFullPathCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, viewFullPathCheckBox->sizePolicy().hasHeightForWidth() ) );
    viewFullPathCheckBox->setChecked( TRUE );
    viewFullPathCheckBox->setText( tr( "View Full Path" ) );
    rightSideLayout->addWidget( viewFullPathCheckBox );
    }

    { // SAVE_EXPERIMENT_DATABASE
    saveExperimentDatabaseCheckBox = new QCheckBox( vpage0big_box, "saveExperimentDatabaseCheckBox" );
    saveExperimentDatabaseCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, saveExperimentDatabaseCheckBox->sizePolicy().hasHeightForWidth() ) );
    saveExperimentDatabaseCheckBox->setChecked( TRUE );
    saveExperimentDatabaseCheckBox->setText( tr( "Save Experiment Database" ) );
    rightSideLayout->addWidget( saveExperimentDatabaseCheckBox );
    }

    { // ASK_ABOUT_CHANGING_ARGS
    askAboutChangingArgsCheckBox = new QCheckBox( vpage0big_box, "askAboutChangingArgsCheckBox" );
    askAboutChangingArgsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, askAboutChangingArgsCheckBox->sizePolicy().hasHeightForWidth() ) );
    askAboutChangingArgsCheckBox->setChecked( askAboutChangingArgs );
    askAboutChangingArgsCheckBox->setText( tr( "On Rerun Allow Changing Application Arguments" ) );
    rightSideLayout->addWidget( askAboutChangingArgsCheckBox );
    }

    { // ASK_ABOUT_SAVING_THE_DATABASE
    askAboutSavingTheDatabaseCheckBox = new QCheckBox( vpage0big_box, "askAboutSavingTheDatabaseCheckBox" );
    askAboutSavingTheDatabaseCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, askAboutSavingTheDatabaseCheckBox->sizePolicy().hasHeightForWidth() ) );
    askAboutSavingTheDatabaseCheckBox->setChecked( askAboutSavingTheDatabase);
    askAboutSavingTheDatabaseCheckBox->setText( tr( "On Rerun Ask About Saving The Experiment Database" ) );
    rightSideLayout->addWidget( askAboutSavingTheDatabaseCheckBox );
    }


    { // ON_RERUN_SAVE_EXPERIMENT_DATABASE
    onRerunSaveCopyOfExperimentDatabaseCheckBox = new QCheckBox( vpage0big_box, "onRerunSaveCopyOfExperimentDatabaseCheckBox" );
    onRerunSaveCopyOfExperimentDatabaseCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, onRerunSaveCopyOfExperimentDatabaseCheckBox->sizePolicy().hasHeightForWidth() ) );
    onRerunSaveCopyOfExperimentDatabaseCheckBox->setChecked( onRerunSaveCopyOfExperimentDatabase);
    onRerunSaveCopyOfExperimentDatabaseCheckBox->setText( tr( "On Rerun Save Copy of Experiment Database from previous run" ) );
    rightSideLayout->addWidget( onRerunSaveCopyOfExperimentDatabaseCheckBox );
    }

    { // VIEW MANGLED NAMES
    viewMangledNameCheckBox = new QCheckBox( vpage0big_box, "viewMangledNameCheckBox" );
    viewMangledNameCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, viewMangledNameCheckBox->sizePolicy().hasHeightForWidth() ) );
    viewMangledNameCheckBox->setChecked( TRUE );
    viewMangledNameCheckBox->setText( tr( "View Mangled Names" ) );
    rightSideLayout->addWidget( viewMangledNameCheckBox );
    }

    { // ALLOW_PYTHON_COMMANDS
    allowPythonCommandsCheckBox = new QCheckBox( vpage0big_box, "allowPythonCommandsCheckBox" );
    allowPythonCommandsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, allowPythonCommandsCheckBox->sizePolicy().hasHeightForWidth() ) );
    allowPythonCommandsCheckBox->setChecked( TRUE );
    allowPythonCommandsCheckBox->setText( tr( "Allow Python Commands" ) );
    rightSideLayout->addWidget( allowPythonCommandsCheckBox );
    }

    { // INSTRUMENTOR IS OFFLINE
    instrumentorIsOfflineCheckBox = new QCheckBox( vpage0big_box, "instrumentorIsOfflineCheckBox" );
    instrumentorIsOfflineCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, instrumentorIsOfflineCheckBox->sizePolicy().hasHeightForWidth() ) );
    instrumentorIsOfflineCheckBox->setChecked( TRUE );
    instrumentorIsOfflineCheckBox->setText( tr( "Instrumentor Is Offline" ) );
    rightSideLayout->addWidget( instrumentorIsOfflineCheckBox );
    }

    { // COMPARISONS IGNORING OBJECT AND DIRECTORY PATH
    lessRestrictiveComparisonsCheckBox = new QCheckBox( vpage0big_box, "lessRestrictiveComparisonsCheckBox" );
    lessRestrictiveComparisonsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, lessRestrictiveComparisonsCheckBox->sizePolicy().hasHeightForWidth() ) );
    lessRestrictiveComparisonsCheckBox->setChecked( TRUE );
    lessRestrictiveComparisonsCheckBox->setText( tr( "Less Restrictive Comparisons" ) );
    rightSideLayout->addWidget( lessRestrictiveComparisonsCheckBox );
    }

    { // COMPARISON OUTPUT NULL ENTRIES OPTIONAL BLANKS VS ZERO ENTRY OPTION
    viewBlankInPlaceOfZeroCheckBox = new QCheckBox( vpage0big_box, "viewBlankInPlaceOfZeroCheckBox" );
    viewBlankInPlaceOfZeroCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, viewBlankInPlaceOfZeroCheckBox->sizePolicy().hasHeightForWidth() ) );
    viewBlankInPlaceOfZeroCheckBox->setChecked( FALSE );
    viewBlankInPlaceOfZeroCheckBox->setText( tr( "Use Blanks In Comparison Output" ) );
    rightSideLayout->addWidget( viewBlankInPlaceOfZeroCheckBox );
    }

    { // DYNAMICALLY ADJUST VIEW FIELD SIZE
    viewFieldSizeIsDynamicCheckBox = new QCheckBox( vpage0big_box, "viewFieldSizeIsDynamicCheckBox" );
    viewFieldSizeIsDynamicCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, viewFieldSizeIsDynamicCheckBox->sizePolicy().hasHeightForWidth() ) );
    viewFieldSizeIsDynamicCheckBox->setChecked( TRUE );
    viewFieldSizeIsDynamicCheckBox->setText( tr( "Dynamically Adjust View Field Size To Data Size" ) );
    rightSideLayout->addWidget( viewFieldSizeIsDynamicCheckBox );
    }


    { // OPENSS_SAVE_VIEWS_FOR_REUSE
    saveViewsForReuseCheckBox = new QCheckBox( vpage0big_box, "saveViewsForReuseCheckBox" );
    saveViewsForReuseCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, saveViewsForReuseCheckBox->sizePolicy().hasHeightForWidth() ) );
    saveViewsForReuseCheckBox->setChecked( FALSE );
    saveViewsForReuseCheckBox->setText( tr( "Save Views for Reuse in CLI and GUI" ) );
    rightSideLayout->addWidget( saveViewsForReuseCheckBox );
    }

    { // OPENSS_SAVE_VIEWS_TIME
    saveViewsTimeLayout = new QHBoxLayout( 0, 0, 6, "saveViewsTimeLayout");
    saveViewsTimeLabel = new QLabel( vpage0big_box, "saveViewsTimeLabel" );
    saveViewsTimeLabel->setText("Save Views if Time Exceeds this value:");
    saveViewsTimeLayout->addWidget( saveViewsTimeLabel );
    saveViewsTimeLineEdit = new QLineEdit( vpage0big_box, "saveViewsTimeLineEdit" );
    saveViewsTimeLineEdit->setText("10");
    saveViewsTimeLineEdit->setValidator( new QIntValidator( 0, 99999, saveViewsTimeLineEdit ) );
    saveViewsTimeLayout->addWidget( saveViewsTimeLineEdit );
    rightSideLayout->addLayout( saveViewsTimeLayout );
    }


    { // VIEW_DATE TIME PRECISION - for I/O MPI -v trace type displays
    viewDateTimePrecisionLayout = new QHBoxLayout( 0, 0, 6, "viewDateTimePrecisionLayout");
    viewDateTimePrecisionLabel = new QLabel( vpage0big_box, "viewDateTimePrecisionLabel" );
    viewDateTimePrecisionLabel->setText("Start/End Time view field size extension:");
    viewDateTimePrecisionLayout->addWidget( viewDateTimePrecisionLabel );
    viewDateTimePrecisionLineEdit = new QLineEdit( vpage0big_box, "viewDateTimePrecisionLineEdit" );
    viewDateTimePrecisionLineEdit->setText("123");
    viewDateTimePrecisionLineEdit->setValidator( new QIntValidator( 1, 99999, viewDateTimePrecisionLineEdit ) );
    viewDateTimePrecisionLayout->addWidget( viewDateTimePrecisionLineEdit );
    rightSideLayout->addLayout( viewDateTimePrecisionLayout );
    }

    { // OPENSS_AUTO_CREATE_DERIVED_METRICS
    autoCreateDerivedMetricsCheckBox = new QCheckBox( vpage0big_box, "autoCreateDerivedMetricsCheckBox" );
    autoCreateDerivedMetricsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, autoCreateDerivedMetricsCheckBox->sizePolicy().hasHeightForWidth() ) );
    autoCreateDerivedMetricsCheckBox->setChecked( TRUE );
    autoCreateDerivedMetricsCheckBox->setText( tr( "Automatically Create Derived Metrics" ) );
    rightSideLayout->addWidget( autoCreateDerivedMetricsCheckBox );
    }


    generalStackPageLayout->addWidget( vpage0sv );
    stack->addWidget( generalStackPage, 0 );
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PreferencesDialog::languageChange()
{

// printf("PreferencesDialog::languageChange() entered\n");
  globalFontFamily = "Bitstream Charter";
  globalFontPointSize = 10;
  globalFontWeight = QFont::Normal;
  globalFontItalic = FALSE;
  fontLineEdit->setText( globalFontFamily );
  fontLineEdit->setReadOnly(TRUE);

  remoteShellLabel->setText( tr("Remote Shell Command:") );
  globalRemoteShell = "/usr/bin/rsh";
  remoteShellEdit->setText( globalRemoteShell );
  remoteShellEdit->setReadOnly(FALSE);

  setShowSplashScreenCheckBox->setChecked( TRUE );

  showGraphicsCheckBox->setChecked(FALSE);

  viewDateTimePrecisionLineEdit->setText( QString("%1").arg(viewDateTimePrecision) );
  QToolTip::add(viewDateTimePrecisionLineEdit,
                tr("Define the width of the field used for date and time displays when\nan 'expView' command is printed.  The default is 3 (display millisecond). A setting of 6 would display microseconds") );

  viewFieldSizeLineEdit->setText( QString("%1").arg(viewFieldSize) );
  QToolTip::add(viewFieldSizeLineEdit,
                tr("Define the width of the field used for each column when the result of\nan 'expView' command is printed.  The default is 20.") );

  viewPrecisionLineEdit->setText( QString("%1").arg(viewPrecision) );
  QToolTip::add(viewPrecisionLineEdit,
                tr("Define the precision used to format a floating point number when\nthe result of an 'expView' command is printed.  The default is 4.") );

  historyLimitLineEdit->setText( QString("%1").arg(historyLimit) );
  QToolTip::add(historyLimitLineEdit,
                tr("Define the maximum number of commands that are remembered for the\n'history' command.  If the command is issued with a larger number,\nthis limit is automatically increased.  The default is 100.") );

  historyDefaultLineEdit->setText( QString("%1").arg(historyDefault) );
  QToolTip::add(historyDefaultLineEdit,
                tr("Define the number of previous commands that will be printed when\nthe 'history' command is issued without a requesting length. The\ndefault is 24;") );

  maxAsyncCommandsLineEdit->setText( QString("%1").arg(maxAsyncCommands) );
  QToolTip::add(maxAsyncCommandsLineEdit,
                tr("Define the maximum number of commands that can be processed at\nthe same time. This is a limit on the parallel execution of\ncommands in Open|SpeedShop and controls the degree to which commands\ncan be overlapped.  The default is 20.  This value must be greater than zero.\n") );

  helpLevelDefaultLineEdit->setText( QString("%1").arg(helpLevelDefault) );
  QToolTip::add(helpLevelDefaultLineEdit,
                tr("Define the level of help information that is displayed when\nthe 'help' command is issued without a <verbosity_list_spec>.\nThe default is 2.") );

  viewFullPathCheckBox->setChecked(viewFullPathCheckBox);
  QToolTip::add(viewFullPathCheckBox,
                tr("Declare whether or not a full path is displayed in place of\na file name when the function, linkedobject, or statement\nlocation is displayed as part of an 'expView' command.  The\ndefault is false, allowing only the containing file name to\nbe displayed.") );

  saveExperimentDatabaseCheckBox->setChecked(saveExperimentDatabase);
  QToolTip::add(saveExperimentDatabaseCheckBox,
                tr("Declare that the database created when an 'expCreate'\ncommand is issued will be saved when the Open|SpeedShop session is\nterminated.  The saved database will be in the user's\ncurrent directory and will be of the form:\n \"X<exp_id>_iiiiii.openss\"\nwhere the 'iiiiii' field is an integer, starting with 0,\nthat generates a unique file name.  The default is 'false'\nand experiment databases will be deleted when the Open|SpeedShop\nsession terminates unless the user has issued an 'expSave'\ncommand.") );

  askAboutChangingArgsCheckBox->setChecked(askAboutChangingArgs);
  QToolTip::add(askAboutChangingArgsCheckBox,
                tr("When rerunning an experiment, pop up a dialog box that allows the application arguments to be changed") );

  viewFieldSizeIsDynamicCheckBox->setChecked(viewFieldSizeIsDynamic);
  QToolTip::add(viewFieldSizeIsDynamicCheckBox,
                tr("When displaying data, dynamically compute the size of the display field required to place the data into.\nThis may cost a bit more time for display but keeps the display more compact.") );

  autoCreateDerivedMetricsCheckBox->setChecked(autoCreateDerivedMetrics);
  QToolTip::add(autoCreateDerivedMetricsCheckBox,
                tr("Automatically create derived metrics from hardware counters, if possible, based on availability.") );

  saveViewsForReuseCheckBox->setChecked(saveViewsForReuse);
  QToolTip::add(saveViewsForReuseCheckBox,
                tr("When generating CLI and GUI views, save the view so it does not have to be recreated again.") );

  saveViewsTimeLineEdit->setText( QString("%1").arg(saveViewsTime) );
  QToolTip::add(saveViewsTimeLineEdit,
                tr("Only save the view for reuse if the time to generate exceeds this time preference value.") );

  askAboutSavingTheDatabaseCheckBox->setChecked(askAboutSavingTheDatabase);
  QToolTip::add(askAboutChangingArgsCheckBox,
                tr("When rerunning an experiment, pop up a dialog box that allows the existing database file to be named by the user and saved") );

  onRerunSaveCopyOfExperimentDatabaseCheckBox->setChecked(onRerunSaveCopyOfExperimentDatabase);
  onRerunSaveCopyOfExperimentDatabaseCheckBox->setChecked(onRerunSaveCopyOfExperimentDatabase);
  QToolTip::add(onRerunSaveCopyOfExperimentDatabaseCheckBox,
                tr("When rerunning an experiment, save a copy of the database that was created when the 'expCreate'\ncommand was issued or from the previous rerun.  To make sure these copies will be saved when the Open|SpeedShop session is\nterminated choose the 'save Experiment Database' checkbox.  The copies of the saved database will be in the user's\ncurrent directory and will be of the form:\n \"X<exp_id>_iiiiii-nn.openss\"\"\nwhere the 'iiiiii' field is an integer, starting with 0,\nwhich generates a unique file name.\nand where the 'nn' field is an integer, starting with 1,\nwhich represents the rerun count.") );


  viewMangledNameCheckBox->setChecked(viewMangledName);
  QToolTip::add(viewMangledNameCheckBox,
                tr("Declare whether or not a mangled name is displayed when a function is displayed\nas part of an 'expView' command. The default is false, allowing the demangled\nnames, that are used in the source code, to be displayed.") );

  allowPythonCommandsCheckBox->setChecked(allowPythonCommands);
  QToolTip::add(allowPythonCommandsCheckBox,
                tr("Declare that Python commands may be intermixed with Open|SpeedShop\ncommands.  The default is true.") );

  instrumentorIsOfflineCheckBox->setChecked(instrumentorIsOffline);
  QToolTip::add(instrumentorIsOfflineCheckBox,
                tr("Declare whether or not the underlying instrumention mechanism for Open|SpeedShop\nis offline if the checkbox is enabled, or if not set, dynamic.  What this means is\nthat, under the hood, the mechanism used to gather the performance\ndata is based on LD_PRELOAD, not dynamic instrumentation.  Attaching\nto a running process or any process control of the running\nperformance experiment is not able to be done because of the static nature of running based on\nusing LD_PRELOAD.  Use the dynamic\ninstrumentation mechanism for those\nfeatures") );


  lessRestrictiveComparisonsCheckBox->setChecked(lessRestrictiveComparisons);
  QToolTip::add(lessRestrictiveComparisonsCheckBox,
                tr("Declare whether or not comparisons should consider the directory path and linked object\nwhen comparing performance data for a particular function.  If this preference is set,\nthe directory path of the source file and the linked object will not be considered.  Use this if you are comparing\nthe same program but have different source versions of the program in separate directories") );

  viewBlankInPlaceOfZeroCheckBox->setChecked(viewBlankInPlaceOfZero);
  QToolTip::add(viewBlankInPlaceOfZeroCheckBox,
                tr("Declare whether or not comparisons should print blank spaces in the output\nwhen comparing and outputing performance data comparison results.  If this preference is set,\nthen blanks are printed in the output.  If this is set to false then zeros are output.\nThe zeros are necessary when creating a comma separated list file from the comparison output") );

    setCaption( tr( "Preferences Dialog" ) );
    categoryListView->header()->setLabel( 0, tr( "Categories" ) );
    categoryListView->clear();

    vpage0big_box->setTitle( tr( "General" ) );
    setFontButton->setText( tr( "Font:" ) );

    setShowSplashScreenCheckBox->setText( tr( "Show splash screen on startup" ) );
    showGraphicsCheckBox->setText( tr( "Show graphics when available" ) );

    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonDefaults->setText( tr( "&Defaults" ) );
    buttonDefaults->setAccel( QKeySequence( tr( "Alt+D" ) ) );
    buttonApply->setText( tr( "&Apply" ) );
    buttonApply->setAccel( QKeySequence( QString::null ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}
