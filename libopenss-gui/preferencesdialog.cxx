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
  remoteShellEdit->setText( tr(globalRemoteShell) );
  remoteShellEdit->setReadOnly(FALSE);

  setShowSplashScreenCheckBox->setChecked( TRUE );

  showGraphicsCheckBox->setChecked(FALSE);

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
