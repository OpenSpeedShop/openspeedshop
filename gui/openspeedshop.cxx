/*! \class OpenSpeedshop
    This is the implementation of the MainWinwdow container for Open Speed
    Shop.  It contains the container for the statusBar, and the menu bar.
 */

#include "openspeedshop.hxx"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "openspeedshop.ui.hxx"
#include "CLIInterface.hxx"
/*!
 *  Constructs a OpenSpeedshop as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
OpenSpeedshop::OpenSpeedshop( int _wid, QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
  lfd = NULL;
  sfd = NULL;
  widStr = QString::null;
  pidStr = QString::null;
  executableName = QString::null;
  experimentName = QString::null;
  argsStr = QString::null;
  rankStr = QString::null;
  hostStr = QString::null;
  expStr = QString::null;
  preferencesDialog = NULL;

  (void)statusBar();

  if ( !name )
  {
	setName( "OpenSpeedshop" );
  }
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
  setCentralWidget( new QWidget( this, "qt_central_widget" ) );
  OpenSpeedshopLayout = new QVBoxLayout( centralWidget(), 11, 6, "OpenSpeedshopLayout"); 

  // actions
  fileOpenExperimentAction = new QAction( this, "fileOpenExperimentAction" );
//  fileOpenExperimentAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
  fileSaveExperimentAction = new QAction( this, "fileSaveExperimentAction" );
//  fileSaveExperimentAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );

  fileExportExperimentDataAction = new QAction( this, "fileExportExperimentDataAction" );
//  fileExportExperimentDataAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );

  fileSaveSessionAction = new QAction( this, "fileSaveSessionAction" );
//  fileSaveSessionAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
  filePreferencesAction = new QAction( this, "filePreferencesAction" );
  fileCloseAction = new QAction( this, "fileCloseAction" );
  fileExitAction = new QAction( this, "fileExitAction" );
  helpContentsAction = new QAction( this, "helpContentsAction" );
  helpIndexAction = new QAction( this, "helpIndexAction" );
  helpAboutAction = new QAction( this, "helpAboutAction" );


  // menubar
  menubar = new QMenuBar( this, "menubar" );

  fileMenu = new QPopupMenu( this );

  fileOpenExperimentAction->addTo( fileMenu );
  fileSaveExperimentAction->addTo( fileMenu );
  fileExportExperimentDataAction->addTo( fileMenu );

  fileMenu->insertSeparator();

  fileSaveSessionAction->addTo( fileMenu );
  fileMenu->insertSeparator();
  filePreferencesAction->addTo( fileMenu );
  fileMenu->insertSeparator();
  fileMenu->insertSeparator();
  fileCloseAction->addTo( fileMenu );
  fileMenu->insertSeparator();
  fileExitAction->addTo( fileMenu );
  menubar->insertItem( QString(""), fileMenu, 1 );

  editMenu = new QPopupMenu( this );

    // signals and slots connections
  connect( fileOpenExperimentAction, SIGNAL( activated() ), this, SLOT( fileOpenExperiment() ) );
  connect( fileSaveExperimentAction, SIGNAL( activated() ), this, SLOT( fileSaveExperiment() ) );
  connect( fileExportExperimentDataAction, SIGNAL( activated() ), this, SLOT( fileExportExperimentData() ) );

  connect( fileSaveSessionAction, SIGNAL( activated() ), this, SLOT( fileSaveSession() ) );
  connect( filePreferencesAction, SIGNAL( activated() ), this, SLOT( filePreferences() ) );
  connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
  connect( fileCloseAction, SIGNAL( activated() ), this, SLOT( fileClose() ) );
  connect( helpIndexAction, SIGNAL( activated() ), this, SLOT( helpIndex() ) );
  connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
  connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
  init();


  menubar->insertSeparator();

  helpMenu = new QPopupMenu( this );
  helpContentsAction->addTo( helpMenu );
  helpIndexAction->addTo( helpMenu );
  helpMenu->insertSeparator();
  helpAboutAction->addTo( helpMenu );
  menubar->insertItem( tr("&Help"), helpMenu );

  assistant = new QAssistantClient(NULL);
//    assistant->setArguments(QStringList("-hideSidebar"));
  QStringList slist;
  slist.append("-profile");
  slist.append("doc/help.adp");
//    slist.append("-hideSidebar");
  assistant->setArguments(slist);

  languageChange();

  resize( QSize(850, 620).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  cli = new CLIInterface(_wid);
}

/*!
 *  Destroys the object and frees any allocated resources
 */
OpenSpeedshop::~OpenSpeedshop()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*!
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void OpenSpeedshop::languageChange()
{
  setCaption( tr( "Open/SpeedShop" ) );

  fileOpenExperimentAction->setText( tr( "Open Saved Experiment..." ) );
  fileOpenExperimentAction->setMenuText( tr( "&Open Saved Experiment..." ) );
  fileOpenExperimentAction->setAccel( tr( "Ctrl+O" ) );

  fileSaveExperimentAction->setText( tr( "Save Experiment Data" ) );
  fileSaveExperimentAction->setMenuText( tr( "&Save Experiment Data" ) );
  fileSaveExperimentAction->setAccel( tr( "Ctrl+S" ) );

  fileExportExperimentDataAction->setText( tr( "Export Experiment Data" ) );
  fileExportExperimentDataAction->setMenuText( tr( "&Export Experiment Data" ) );
  fileExportExperimentDataAction->setAccel( tr( "Ctrl+E" ) );

  fileSaveSessionAction->setText( tr( "Save Window Setup" ) );
  fileSaveSessionAction->setMenuText( tr( "Save &Window Setup" ) );
  fileSaveSessionAction->setAccel( tr( "Ctrl+W" ) );

  filePreferencesAction->setText( tr( "Preferences" ) );
  filePreferencesAction->setMenuText( tr( "&Preferences..." ) );
  filePreferencesAction->setAccel( QString::null );
  fileCloseAction->setText( tr( "Close" ) );
  fileCloseAction->setMenuText( tr( "C&lose" ) );
  fileCloseAction->setAccel( QString::null );
  fileExitAction->setText( tr( "Exit" ) );
  fileExitAction->setMenuText( tr( "E&xit" ) );
  fileExitAction->setAccel( QString::null );
  helpContentsAction->setText( tr( "Contents" ) );
  helpContentsAction->setMenuText( tr( "&Contents..." ) );
  helpContentsAction->setAccel( QString::null );
  helpIndexAction->setText( tr( "Index" ) );
  helpIndexAction->setMenuText( tr( "&Index..." ) );
  helpIndexAction->setAccel( QString::null );
  helpAboutAction->setText( tr( "About" ) );
  helpAboutAction->setMenuText( tr( "&About" ) );
  helpAboutAction->setAccel( QString::null );
  if (menubar->findItem(1))
  {
    menubar->findItem(1)->setText( tr( "&File" ) );
  }
#ifdef HOLD
  if (menubar->findItem(3))
  {
    menubar->findItem(3)->setText( tr( "&Help" ) );
  }
#endif // HOLD
}

void
OpenSpeedshop::print()
{
  printf("widStr = %s\n",  widStr.ascii() );
  printf("pidStr = %s\n",  pidStr.ascii() );
  printf("executableName = %s\n",  executableName.ascii() );
  printf("argsStr = %s\n",  argsStr.ascii() );
  printf("rankStr = %s\n",  rankStr.ascii() );
  printf("hostStr = %s\n",  hostStr.ascii() );
  printf("expStr = %s\n",  expStr.ascii() );
}
