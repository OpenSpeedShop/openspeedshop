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

   globalFontFamily = "Helvetica";
   globalFontPointSize = 12;
   globalFontWeight = QFont::Normal;
   globalFontItalic = FALSE;

   viewFieldSize = 20; 
   viewPrecision = 4; 
   historyLimit = 100; 
   historyDefault = 24; 
   maxAsyncCommands = 20; 
   helpLevelDefault = 2; 
   viewFullPath = FALSE;  
   saveExperimentDatabase = FALSE; 
   allowPythonCommands = TRUE; 
   logByDefault = FALSE; 
   limitSignalCatching = FALSE; 


   globalRemoteShell = "/usr/bin/rsh";

    if ( !name )
	setName( "PreferencesDialog" );
    setSizeGripEnabled( TRUE );
    PreferencesDialogLayout = new QVBoxLayout( this, 11, 6, "PreferencesDialogLayout"); 

    preferenceDialogListLayout = new QVBoxLayout( 0, 0, 6, "preferenceDialogListLayout"); 

    mainSplitter = new QSplitter( this, "mainSplitter" );
    mainSplitter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, mainSplitter->sizePolicy().hasHeightForWidth() ) );
    mainSplitter->setOrientation( QSplitter::Horizontal );
    mainSplitter->setChildrenCollapsible( FALSE );

    preferenceDialogLeftFrame = new QFrame( mainSplitter, "preferenceDialogLeftFrame" );
    preferenceDialogLeftFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogLeftFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrame->setMinimumSize( QSize( 15, 0 ) );
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
    preferenceDialogRightFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogRightFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogRightFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogRightFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogRightFrameLayout = new QVBoxLayout( preferenceDialogRightFrame, 11, 6, "preferenceDialogRightFrameLayout"); 

    preferenceDialogWidgetStack = new QWidgetStack( preferenceDialogRightFrame, "preferenceDialogWidgetStack" );
    preferenceDialogWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogWidgetStack->sizePolicy().hasHeightForWidth() ) );


    createGeneralStackPage(preferenceDialogWidgetStack, "General" );

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

    GeneralGroupBox = new QGroupBox( generalStackPage, "GeneralGroupBox" );
    GeneralGroupBox->setColumnLayout(0, Qt::Vertical );
    GeneralGroupBox->layout()->setSpacing( 6 );
    GeneralGroupBox->layout()->setMargin( 11 );

    rightSideLayout = new QVBoxLayout( GeneralGroupBox->layout(), 11, "rightSideLayout"); 

    fontLayout = new QHBoxLayout( 0, 0, 6, "fontLayout"); 

    setFontButton = new QPushButton( GeneralGroupBox, "setFontButton" );
    fontLayout->addWidget( setFontButton );

    fontLineEdit = new QLineEdit( GeneralGroupBox, "fontLineEdit" );
    fontLayout->addWidget( fontLineEdit );
    rightSideLayout->addLayout( fontLayout );

    remoteShellLayout = new QHBoxLayout( 0, 0, 6, "remoteShellLayout"); 
    remoteShellLabel = new QLabel( GeneralGroupBox, "remote shell label" );
    remoteShellLayout->addWidget( remoteShellLabel );

    remoteShellEdit = new QLineEdit( GeneralGroupBox, "remoteShellEdit" );
    remoteShellLayout->addWidget( remoteShellEdit );
    rightSideLayout->addLayout( remoteShellLayout );

    setShowSplashScreenCheckBox = new QCheckBox( GeneralGroupBox, "setShowSplashScreenCheckBox" );
    setShowSplashScreenCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, setShowSplashScreenCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( setShowSplashScreenCheckBox );

    showGraphicsCheckBox = new QCheckBox( GeneralGroupBox, "showGraphicsCheckBox" );
    showGraphicsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showGraphicsCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( showGraphicsCheckBox );

    { // VIEW_FIELD_SIZE
    viewFieldSizeLayout = new QHBoxLayout( 0, 0, 6, "viewFieldSizeLayout");
    viewFieldSizeLabel = new QLabel( GeneralGroupBox, "viewFieldSizeLabel" );
    viewFieldSizeLabel->setText("View field size:");
    viewFieldSizeLayout->addWidget( viewFieldSizeLabel );
    viewFieldSizeLineEdit = new QLineEdit( GeneralGroupBox, "viewFieldSizeLineEdit" );
    viewFieldSizeLineEdit->setText("123");
    viewFieldSizeLayout->addWidget( viewFieldSizeLineEdit );
    rightSideLayout->addLayout( viewFieldSizeLayout );
    }
    { // VIEW_PRECISION
    viewPrecisionLayout = new QHBoxLayout( 0, 0, 6, "viewPrecisionLayout");
    viewPrecisionLabel = new QLabel( GeneralGroupBox, "viewPrecisionLabel" );
    viewPrecisionLabel->setText("View Precision:");
    viewPrecisionLayout->addWidget( viewPrecisionLabel );
    viewPrecisionLineEdit = new QLineEdit( GeneralGroupBox, "viewPrecisionLineEdit" );
    viewPrecisionLineEdit->setText("123");
    viewPrecisionLayout->addWidget( viewPrecisionLineEdit );
    rightSideLayout->addLayout( viewPrecisionLayout );
    }
    { // HISTORY_LIMIT
    historyLimitLayout = new QHBoxLayout( 0, 0, 6, "historyLimitLayout");
    historyLimitLabel = new QLabel( GeneralGroupBox, "historyLimitLabel" );
    historyLimitLabel->setText("History Limit:");
    historyLimitLayout->addWidget( historyLimitLabel );
    historyLimitLineEdit = new QLineEdit( GeneralGroupBox, "historyLimitLineEdit" );
    historyLimitLineEdit->setText("123");
    historyLimitLayout->addWidget( historyLimitLineEdit );
    rightSideLayout->addLayout( historyLimitLayout );
    }
    { // HISTORY_DEFAULT
    historyDefaultLayout = new QHBoxLayout( 0, 0, 6, "historyDefaultLayout");
    historyDefaultLabel = new QLabel( GeneralGroupBox, "historyDefaultLabel" );
    historyDefaultLabel->setText("History Default:");
    historyDefaultLayout->addWidget( historyDefaultLabel );
    historyDefaultLineEdit = new QLineEdit( GeneralGroupBox, "historyDefaultLineEdit" );
    historyDefaultLineEdit->setText("123");
    historyDefaultLayout->addWidget( historyDefaultLineEdit );
    rightSideLayout->addLayout( historyDefaultLayout );
    }
    { // MAX_ASYNC_COMMANDS
    maxAsyncCommandsLayout = new QHBoxLayout( 0, 0, 6, "maxAsyncCommandsLayout");
    maxAsyncCommandsLabel = new QLabel( GeneralGroupBox, "maxAsyncCommandsLabel" );
    maxAsyncCommandsLabel->setText("Max Async Commands:");
    maxAsyncCommandsLayout->addWidget( maxAsyncCommandsLabel );
    maxAsyncCommandsLineEdit = new QLineEdit( GeneralGroupBox, "maxAsyncCommandsLineEdit" );
    maxAsyncCommandsLineEdit->setText("123");
    maxAsyncCommandsLayout->addWidget( maxAsyncCommandsLineEdit );
    rightSideLayout->addLayout( maxAsyncCommandsLayout );
    }
    { // HELP_LEVEL_DEFAULT
    helpLevelDefaultLayout = new QHBoxLayout( 0, 0, 6, "helpLevelDefaultLayout");
    helpLevelDefaultLabel = new QLabel( GeneralGroupBox, "helpLevelDefaultLabel" );
    helpLevelDefaultLabel->setText("Help Level Default:");
    helpLevelDefaultLayout->addWidget( helpLevelDefaultLabel );
    helpLevelDefaultLineEdit = new QLineEdit( GeneralGroupBox, "helpLevelDefaultLineEdit" );
    helpLevelDefaultLineEdit->setText("123");
    helpLevelDefaultLayout->addWidget( helpLevelDefaultLineEdit );
    rightSideLayout->addLayout( helpLevelDefaultLayout );
    }
    { // VIEW_FULLPATH
    viewFullPathCheckBox = new QCheckBox( GeneralGroupBox, "viewFullPathCheckBox" );
    viewFullPathCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, viewFullPathCheckBox->sizePolicy().hasHeightForWidth() ) );
    viewFullPathCheckBox->setChecked( TRUE );
    viewFullPathCheckBox->setText( tr( "View Full Path" ) );
    rightSideLayout->addWidget( viewFullPathCheckBox );
    }
    { // SAVE_EXPERIMENT_DATABASE
    saveExperimentDatabaseCheckBox = new QCheckBox( GeneralGroupBox, "saveExperimentDatabaseCheckBox" );
    saveExperimentDatabaseCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, saveExperimentDatabaseCheckBox->sizePolicy().hasHeightForWidth() ) );
    saveExperimentDatabaseCheckBox->setChecked( TRUE );
    saveExperimentDatabaseCheckBox->setText( tr( "Save Experiment Database" ) );
    rightSideLayout->addWidget( saveExperimentDatabaseCheckBox );
    }
    { // ALLOW_PYTHON_COMMANDS
    allowPythonCommandsCheckBox = new QCheckBox( GeneralGroupBox, "allowPythonCommandsCheckBox" );
    allowPythonCommandsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, allowPythonCommandsCheckBox->sizePolicy().hasHeightForWidth() ) );
    allowPythonCommandsCheckBox->setChecked( TRUE );
    allowPythonCommandsCheckBox->setText( tr( "Allow Python Commands" ) );
    rightSideLayout->addWidget( allowPythonCommandsCheckBox );
    }
    { // LOG_BY_DEFAULT
    logByDefaultCheckBox = new QCheckBox( GeneralGroupBox, "logByDefaultCheckBox" );
    logByDefaultCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, logByDefaultCheckBox->sizePolicy().hasHeightForWidth() ) );
    logByDefaultCheckBox->setChecked( TRUE );
    logByDefaultCheckBox->setText( tr( "Log By Default" ) );
    rightSideLayout->addWidget( logByDefaultCheckBox );
    }
    { // LIMIT_SIGNAL_CATCHING
    limitSignalCatchingCheckBox = new QCheckBox( GeneralGroupBox, "limitSignalCatchingCheckBox" );
    limitSignalCatchingCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, limitSignalCatchingCheckBox->sizePolicy().hasHeightForWidth() ) );
    limitSignalCatchingCheckBox->setChecked( TRUE );
    limitSignalCatchingCheckBox->setText( tr( "Limit Signal Catching" ) );
    rightSideLayout->addWidget( limitSignalCatchingCheckBox );
    }

    generalStackPageLayout->addWidget( GeneralGroupBox );
    stack->addWidget( generalStackPage, 0 );
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PreferencesDialog::languageChange()
{

// printf("PreferencesDialog::languageChange() entered\n");
  globalFontFamily = "Helvetica";
  globalFontPointSize = 12;
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
  tr("Define the maximum number of commands that can be processed at\nthe same time. This is a limit on the parallel execution of\ncommands in OpenSS and controls the degree to which commands\ncan be overlapped.  The default is 20.\n") );

  helpLevelDefaultLineEdit->setText( QString("%1").arg(helpLevelDefault) );
QToolTip::add(helpLevelDefaultLineEdit,
  tr("Define the level of help information that is displayed when\nthe 'help' command is issued without a <verbosity_list_spec>.\nThe default is 2.") );

  viewFullPathCheckBox->setChecked(viewFullPathCheckBox);
QToolTip::add(viewFullPathCheckBox,
  tr("Declare whether or not a full path is displayed in place of\na file name when the function, linkedobject, or statement\nlocation is displayed as part of an 'expView' command.  The\ndefault is false, allowing only the containing file name to\nbe displayed.") );

  saveExperimentDatabaseCheckBox->setChecked(saveExperimentDatabase);
QToolTip::add(saveExperimentDatabaseCheckBox,
  tr("Declare that the database created when an 'expCreate'\ncommand is issued will be saved when the OpenSS session is\nterminated.  The saved database will be in the user's\ncurrent directory and will be of the form:\n \"X<exp_id>_iiiiii.openss\"\nwhere the 'iiiiii' field is an integer, starting with 0,\nthat generates a unique file name.  The default is 'false'\nand experiment databases will be deleted when the OpenSS\nsession terminates unless the user has issued an 'expSave'\ncommand.") );

  allowPythonCommandsCheckBox->setChecked(allowPythonCommands);
QToolTip::add(allowPythonCommandsCheckBox,
  tr("Declare that Python commands may be intermixed with OpenSS\ncommands.  The default is true.") );

  logByDefaultCheckBox->setChecked(logByDefault);
QToolTip::add(logByDefaultCheckBox,
  tr("Declare that a log file will be opened and each command\nwill be tracked through the various internal processing\nstep of OpenSS.  This is intended to be an internal debug\naid and is not generally useful.  The default is false.") );

  limitSignalCatchingCheckBox->setChecked(limitSignalCatching);
QToolTip::add(limitSignalCatchingCheckBox,
  tr("Declare that OpenSS should limit the types of signals it\ntraps. When set to true, OpenSS will ignore the following\nfaults:\n SIGILL - illegal instructions\n SIGFPE - floating point exceptions\n SIGBUS - bus errors\n SIGSEGV - illegal memory addresses\n SIGSYS - system errors\nIgnoring the errors will allow a core file to be generated,\nso this is intended to be an internal debug aid.  The default\nvalue is false and OpenSS will attempt to clean up if an\nerror is encountered.  Setting the value to true may result\nin a number of files being left around if OpenSS encounters\na fault.") );

    setCaption( tr( "Preferences Dialog" ) );
    categoryListView->header()->setLabel( 0, tr( "Categories" ) );
    categoryListView->clear();

    GeneralGroupBox->setTitle( tr( "General" ) );
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
