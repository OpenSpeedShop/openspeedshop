/****************************************************************************
** Form implementation generated from reading ui file 'preferencesdialog.ui'
**
** Created: Thu Jan 13 10:22:22 2005
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

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

   globalFontFamily = "Helvetica";
   globalFontPointSize = 12;
   globalFontWeight = QFont::Normal;
   globalFontItalic = FALSE;

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
categoryListView->setSortColumn( -1 );

    categoryListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)7, 0, 0, categoryListView->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogLeftFrameLayout->addWidget( categoryListView );

    preferenceDialogRightFrame = new QFrame( mainSplitter, "preferenceDialogRightFrame" );
    preferenceDialogRightFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogRightFrame->sizePolicy().hasHeightForWidth() ) );
    preferenceDialogRightFrame->setFrameShape( QFrame::StyledPanel );
    preferenceDialogRightFrame->setFrameShadow( QFrame::Raised );
    preferenceDialogRightFrameLayout = new QVBoxLayout( preferenceDialogRightFrame, 11, 6, "preferenceDialogRightFrameLayout"); 

    preferenceDialogWidgetStack = new QWidgetStack( preferenceDialogRightFrame, "preferenceDialogWidgetStack" );
    preferenceDialogWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, preferenceDialogWidgetStack->sizePolicy().hasHeightForWidth() ) );

    generalStackPage = new QWidget( preferenceDialogWidgetStack, "generalStackPage" );
    generalStackPageLayout = new QVBoxLayout( generalStackPage, 11, 6, "generalStackPageLayout"); 

    GeneralGroupBox = new QGroupBox( generalStackPage, "GeneralGroupBox" );

    QWidget* generalPrivateLayout = new QWidget( GeneralGroupBox, "rightSideLayout" );
//    generalPrivateLayout->setGeometry( QRect( 10, 20, 237, 183 ) );
    generalPrivateLayout->setGeometry( QRect( 10, 20, 300, 200 ) );
    rightSideLayout = new QVBoxLayout( generalPrivateLayout, 11, 6, "rightSideLayout"); 

    fontLayout = new QHBoxLayout( 0, 0, 6, "fontLayout"); 

    setFontButton = new QPushButton( generalPrivateLayout, "setFontButton" );
    fontLayout->addWidget( setFontButton );

    fontLineEdit = new QLineEdit( generalPrivateLayout, "fontLineEdit" );
    fontLayout->addWidget( fontLineEdit );
    rightSideLayout->addLayout( fontLayout );

    precisionLayout = new QHBoxLayout( 0, 0, 6, "precisionLayout"); 

    precisionTextLabel = new QLabel( generalPrivateLayout, "precisionTextLabel" );
    precisionLayout->addWidget( precisionTextLabel );

    precisionLineEdit = new QLineEdit( generalPrivateLayout, "precisionLineEdit" );
    precisionLayout->addWidget( precisionLineEdit );
    rightSideLayout->addLayout( precisionLayout );

    setShowSplashScreenCheckBox = new QCheckBox( generalPrivateLayout, "setShowSplashScreenCheckBox" );
    setShowSplashScreenCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, setShowSplashScreenCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( setShowSplashScreenCheckBox );

    setShowColoredTabsCheckBox = new QCheckBox( generalPrivateLayout, "setShowColoredTabsCheckBox" );
    setShowColoredTabsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, setShowColoredTabsCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( setShowColoredTabsCheckBox );

    deleteEmptyPCCheckBox = new QCheckBox( generalPrivateLayout, "deleteEmptyPCCheckBox" );
    deleteEmptyPCCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, deleteEmptyPCCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( deleteEmptyPCCheckBox );

    showGraphicsCheckBox = new QCheckBox( generalPrivateLayout, "showGraphicsCheckBox" );
    showGraphicsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showGraphicsCheckBox->sizePolicy().hasHeightForWidth() ) );
    rightSideLayout->addWidget( showGraphicsCheckBox );
    generalStackPageLayout->addWidget( GeneralGroupBox );
    preferenceDialogWidgetStack->addWidget( generalStackPage, 0 );

    sourcePanelStackPage = new QWidget( preferenceDialogWidgetStack, "sourcePanelStackPage" );
    generalStackPageLayout_2 = new QVBoxLayout( sourcePanelStackPage, 11, 6, "generalStackPageLayout_2"); 

    sourcePanelGroupBox = new QGroupBox( sourcePanelStackPage, "sourcePanelGroupBox" );

    QWidget* sourcePanelPrivateLayout = new QWidget( sourcePanelGroupBox, "layout6" );
//    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 150, 52 ) );
    sourcePanelPrivateLayout->setGeometry( QRect( 10, 40, 200, 100 ) );
    layout6 = new QVBoxLayout( sourcePanelPrivateLayout, 11, 6, "layout6"); 

    showStatisticsCheckBox = new QCheckBox( sourcePanelPrivateLayout, "showStatisticsCheckBox" );
    showStatisticsCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showStatisticsCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showStatisticsCheckBox );

    showLineNumbersCheckBox = new QCheckBox( sourcePanelPrivateLayout, "showLineNumbersCheckBox" );
    showLineNumbersCheckBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, showLineNumbersCheckBox->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( showLineNumbersCheckBox );
    generalStackPageLayout_2->addWidget( sourcePanelGroupBox );
    preferenceDialogWidgetStack->addWidget( sourcePanelStackPage, 1 );

    statsPanelStackPage = new QWidget( preferenceDialogWidgetStack, "statsPanelStackPage" );
    generalStackPageLayout_3 = new QVBoxLayout( statsPanelStackPage, 11, 6, "generalStackPageLayout_3"); 

    statsPanelGroupBox = new QGroupBox( statsPanelStackPage, "statsPanelGroupBox" );

    QWidget* statsPanelPrivateLayout = new QWidget( statsPanelGroupBox, "layout8" );
//    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 271, 57 ) );
    statsPanelPrivateLayout->setGeometry( QRect( 20, 86, 300, 100 ) );
    layout8 = new QVBoxLayout( statsPanelPrivateLayout, 11, 6, "layout8"); 

    sortDecendingCheckBox = new QCheckBox( statsPanelPrivateLayout, "sortDecendingCheckBox" );
    layout8->addWidget( sortDecendingCheckBox );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    showTopNTextLabel = new QLabel( statsPanelPrivateLayout, "showTopNTextLabel" );
    layout7->addWidget( showTopNTextLabel );

    showTopNLineEdit = new QLineEdit( statsPanelPrivateLayout, "showTopNLineEdit" );
    layout7->addWidget( showTopNLineEdit );
    layout8->addLayout( layout7 );
    generalStackPageLayout_3->addWidget( statsPanelGroupBox );
    preferenceDialogWidgetStack->addWidget( statsPanelStackPage, 2 );

    cmdPanelStackPage = new QWidget( preferenceDialogWidgetStack, "cmdPanelStackPage" );
    generalStackPageLayout_4 = new QVBoxLayout( cmdPanelStackPage, 11, 6, "generalStackPageLayout_4"); 

    cmdPanelGroupBox = new QGroupBox( cmdPanelStackPage, "cmdPanelGroupBox" );
    generalStackPageLayout_4->addWidget( cmdPanelGroupBox );
    preferenceDialogWidgetStack->addWidget( cmdPanelStackPage, 3 );
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
    connect( precisionLineEdit, SIGNAL( returnPressed() ), this,
      SLOT( setGlobalPrecision() ) );
    connect( setShowSplashScreenCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setShowSplashScreen() ) );
    connect( setShowColoredTabsCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setShowColoredPanelTabs() ) );
    connect( deleteEmptyPCCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setRemoveEmptyPC() ) );
    connect( showGraphicsCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setShowAvailableGraphics() ) );
    connect( showStatisticsCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setShowStats() ) );
    connect( showLineNumbersCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setShowLineNumbers() ) );
    connect( sortDecendingCheckBox, SIGNAL( stateChanged(int) ), this,
      SLOT( setSortDescending() ) );
    connect( showTopNLineEdit, SIGNAL( selectionChanged() ), this,
      SLOT( setShowTopN() ) );
    connect( categoryListView, SIGNAL( clicked(QListViewItem*) ), this,
      SLOT( listItemSelected(QListViewItem*) ) );

    preferencesAvailable = FALSE;
    readPreferencesOnEntry();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PreferencesDialog::~PreferencesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PreferencesDialog::languageChange()
{

printf("PreferencesDialog::languageChange() entered\n");
  globalFontFamily = "Helvetica";
  globalFontPointSize = 12;
  globalFontWeight = QFont::Normal;
  globalFontItalic = FALSE;
  fontLineEdit->setText( globalFontFamily );
fontLineEdit->setReadOnly(TRUE);
  setShowSplashScreenCheckBox->setChecked( TRUE );
  setShowColoredTabsCheckBox->setChecked(FALSE);
  deleteEmptyPCCheckBox->setChecked(FALSE);
  showGraphicsCheckBox->setChecked(FALSE);
  showStatisticsCheckBox->setChecked(FALSE);
  showLineNumbersCheckBox->setChecked(FALSE);
  sortDecendingCheckBox->setChecked( TRUE );

    setCaption( tr( "Preferences Dialog" ) );
    categoryListView->header()->setLabel( 0, tr( "Categories" ) );
    categoryListView->clear();
    QListViewItem * item = new QListViewItem( categoryListView, 0 );
    item->setText( 0, tr( "General" ) );

    item = new QListViewItem( categoryListView, item );
    item->setText( 0, tr( "Source Panel" ) );

    item = new QListViewItem( categoryListView, item );
    item->setText( 0, tr( "Stats Panel" ) );

    item = new QListViewItem( categoryListView, item );
    item->setText( 0, tr( "Cmd Panel" ) );

    GeneralGroupBox->setTitle( tr( "General" ) );
    setFontButton->setText( tr( "Font:" ) );
    precisionTextLabel->setText( tr( "Precision:" ) );
    precisionLineEdit->setText( tr( "7" ) );
    setShowSplashScreenCheckBox->setText( tr( "Show splash screen on startup" ) );
    setShowColoredTabsCheckBox->setText( tr( "Show related panel tabs in color" ) );
    deleteEmptyPCCheckBox->setText( tr( "Remove empty panel containers" ) );
    showGraphicsCheckBox->setText( tr( "Show graphics when available" ) );
    sourcePanelGroupBox->setTitle( tr( "Source Panel" ) );
    showStatisticsCheckBox->setText( tr( "Show statistics" ) );
    showLineNumbersCheckBox->setText( tr( "Show line numbers" ) );
    statsPanelGroupBox->setTitle( tr( "Stats Panel" ) );
    sortDecendingCheckBox->setText( tr( "Sort Descending" ) );
    showTopNTextLabel->setText( tr( "Show top N items:" ) );
    showTopNLineEdit->setText( tr( "5" ) );
    cmdPanelGroupBox->setTitle( tr( "Cmd Panel" ) );
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
