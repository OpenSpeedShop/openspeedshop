////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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
  

/*! \class OpenSpeedshop
    This is the implementation of the MainWinwdow container for Open Speed
    Shop.  It contains the container for the statusBar, and the menu bar.
 */

//#define DEBUG_GUI 1

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
OpenSpeedshop::OpenSpeedshop( int _wid, int _climode, QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
  afd = NULL;
  MPafd = NULL;
  lfd = NULL;
  MPlfd = NULL;
  Arglfd = NULL;
  sfd = NULL;
  widStr = QString::null;
  pidStr = QString::null;
  pidStrList = NULL;
  executableName = QString::null;
  experimentName = QString::null;
  ArgFileName = QString::null;
  argsStr = QString::null;
  parallelPrefixCommandStr = QString::null;
  rankStr = QString::null;
  hostStr = QString::null;
  expStr = QString::null;
  preferencesDialog = NULL;
  mpiFLAG = FALSE;
  shuttingDown = FALSE;

  widStr = QString("%1").arg(_wid);

#if DEBUG_GUI
  printf("OpenSpeedshop::OpenSpeedshop, _wid=%d, widStr=%s\n",  _wid, widStr.ascii());
#endif

  climode = _climode;

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
  fileOpenSavedExperimentAction = new QAction( this, "fileOpenSavedExperimentAction" );
//  fileOpenSavedExperimentAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
  fileSaveExperimentAction = new QAction( this, "fileSaveExperimentAction" );
//  fileSaveExperimentAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );

#ifdef EXPORT
  fileExportExperimentDataAction = new QAction( this, "fileExportExperimentDataAction" );
//  fileExportExperimentDataAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
#endif // EXPORT

#ifdef SAVESESSION
  fileSaveSessionAction = new QAction( this, "fileSaveSessionAction" );
//  fileSaveSessionAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
#endif // SAVESESSION
  filePreferencesAction = new QAction( this, "filePreferencesAction" );
  if( climode )
  {
    fileCloseAction = new QAction( this, "fileCloseAction" );
  }
  fileExitAction = new QAction( this, "fileExitAction" );
  helpContentsAction = new QAction( this, "helpContentsAction" );
  helpIndexAction = new QAction( this, "helpIndexAction" );
  helpAboutAction = new QAction( this, "helpAboutAction" );


  // menubar
  menubar = new QMenuBar( this, "menubar" );

  fileMenu = new QPopupMenu( this );

  fileOpenExperimentAction->addTo( fileMenu );
  fileOpenSavedExperimentAction->addTo( fileMenu );
  fileSaveExperimentAction->addTo( fileMenu );
#ifdef EXPORT
  fileExportExperimentDataAction->addTo( fileMenu );
#endif // EXPORT

#ifdef SAVESESSION
  fileMenu->insertSeparator();
  fileSaveSessionAction->addTo( fileMenu );
#endif // SAVESESSION

  fileMenu->insertSeparator();
  QPopupMenu *experimentsMenu = new QPopupMenu(this);
  QPopupMenu *wizardsMenu = new QPopupMenu(this);
  fileMenu->insertItem( "&Experiments", experimentsMenu, 1 );
  fileMenu->insertItem( "&Wizards", wizardsMenu, 3 );

  fileMenu->insertSeparator();
  filePreferencesAction->addTo( fileMenu );
  fileMenu->insertSeparator();
  if( climode )
  {
    fileCloseAction->addTo( fileMenu );
    fileMenu->insertSeparator();
  }
  fileExitAction->addTo( fileMenu );
  menubar->insertItem( QString("&File"), fileMenu, 1 );

  editMenu = new QPopupMenu( this );

    // signals and slots connections
  connect( fileOpenExperimentAction, SIGNAL( activated() ), this, SLOT( fileOpenExperiment() ) );
  connect( fileOpenSavedExperimentAction, SIGNAL( activated() ), this, SLOT( fileOpenSavedExperiment() ) );
  connect( fileSaveExperimentAction, SIGNAL( activated() ), this, SLOT( fileSaveExperiment() ) );
#ifdef EXPORT
  connect( fileExportExperimentDataAction, SIGNAL( activated() ), this, SLOT( fileExportExperimentData() ) );
#endif // EXPORT

#ifdef SAVESESSION
  connect( fileSaveSessionAction, SIGNAL( activated() ), this, SLOT( fileSaveSession() ) );
#endif // SAVESESSION
  connect( filePreferencesAction, SIGNAL( activated() ), this, SLOT( filePreferences() ) );
  connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
  if( climode ) 
  {
    connect( fileCloseAction, SIGNAL( activated() ), this, SLOT( fileClose() ) );
  }
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

  assistant = NULL;

  languageChange();

  resize( QSize(850, 620).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  QSettings settings;
  int x = settings.readNumEntry("/openspeedshop/geometry/x", this->x());
  int y = settings.readNumEntry("/openspeedshop/geometry/y", this->y());
  int width = settings.readNumEntry("/openspeedshop/geometry/width", this->width());
  int height = settings.readNumEntry("/openspeedshop/geometry/height", this->height());
  this->setGeometry(x,y,width,height);

  cli = new CLIInterface(_wid);
}

/*!
 *  Destroys the object and frees any allocated resources
 */
OpenSpeedshop::~OpenSpeedshop()
{
  storeGeometry();

    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*!
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void OpenSpeedshop::languageChange()
{
  setCaption( tr( "Open|SpeedShop" ) );

  fileOpenExperimentAction->setText( tr( "Open Existing Experiment..." ) );
  fileOpenExperimentAction->setMenuText( tr( "&Open Existing Experiment..." ) );
  fileOpenExperimentAction->setAccel( tr( "Ctrl+O" ) );
fileOpenExperimentAction->setStatusTip( tr("Open an experiment that has been created in the cli, but is not currently in a window.") );

  fileOpenSavedExperimentAction->setText( tr( "Open Saved Experiment (From Saved File) ..." ) );
  fileOpenSavedExperimentAction->setMenuText( tr( "Open S&aved Experiment..." ) );
fileOpenSavedExperimentAction->setStatusTip( tr("Open an experiment that was saved to a file for later viewing.") );
  fileOpenSavedExperimentAction->setAccel( tr( "Ctrl+A" ) );

  fileSaveExperimentAction->setText( tr( "Save Experiment Data..." ) );
  fileSaveExperimentAction->setMenuText( tr( "&Save Experiment Data..." ) );
  fileSaveExperimentAction->setAccel( tr( "Ctrl+S" ) );
fileSaveExperimentAction->setStatusTip( tr("Save the experiment data to a file for later viewing.") );

#ifdef EXPORT
  fileExportExperimentDataAction->setText( tr( "Export Experiment Data" ) );
  fileExportExperimentDataAction->setMenuText( tr( "&Export Experiment Data" ) );
  fileExportExperimentDataAction->setAccel( tr( "Ctrl+E" ) );
#endif // EXPORT

#ifdef SAVESESSION
  fileSaveSessionAction->setText( tr( "Save Window Setup" ) );
  fileSaveSessionAction->setMenuText( tr( "Save &Window Setup" ) );
  fileSaveSessionAction->setAccel( tr( "Ctrl+W" ) );
#endif // SAVESESSION

  filePreferencesAction->setText( tr( "Preferences" ) );
  filePreferencesAction->setMenuText( tr( "&Preferences..." ) );
  filePreferencesAction->setAccel( QString::null );
filePreferencesAction->setStatusTip( tr("Open the Preferences Panel to set persistent preferences.") );

  if( climode )
  {
    fileCloseAction->setText( tr( "Close" ) );
    fileCloseAction->setMenuText( tr( "C&lose" ) );
    fileCloseAction->setStatusTip( tr("Close the windows, but don't exit the tool.  \"opengui\" form the openss>> prompt reopens the windows.") );
  
    fileCloseAction->setEnabled(climode);
    fileCloseAction->setAccel( QString::null );
  }

  fileExitAction->setText( tr( "Exit" ) );
  fileExitAction->setMenuText( tr( "E&xit" ) );
fileExitAction->setStatusTip( tr("Exit the entire session closing down all experiments.") );

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
//    menubar->findItem(1)->setText( tr( "&File" ) );
    menubar->findItem(1)->setText( "&File" );
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
  printf("parallelPrefixCommandStr = %s\n",  parallelPrefixCommandStr.ascii() );
  printf("argsStr = %s\n",  argsStr.ascii() );
  printf("rankStr = %s\n",  rankStr.ascii() );
  printf("hostStr = %s\n",  hostStr.ascii() );
  printf("expStr = %s\n",  expStr.ascii() );
}

void
OpenSpeedshop::closeEvent(QCloseEvent *e)
{
#if DEBUG_GUI
  printf("OpenSpeedshop::closeEvent() entered.\n");
#endif

  int ret_val =  QMessageBox::Cancel;

  if( climode ) {
    ret_val =  QMessageBox::question(
            this,
            tr("Finished?"),
            tr("Do you really want to exit Open|SpeedShop?\n  - Yes will exit.\n  - No will close the gui windows.\n  - Cancel will do nothing."),
            QMessageBox::Yes,  QMessageBox::No,  QMessageBox::Cancel );
  } else {
    ret_val =  QMessageBox::question(
            this,
            tr("Finished?"),
            tr("Do you really want to exit Open|SpeedShop?\n  - Yes will exit.\n  - Cancel will do nothing."),
            QMessageBox::Yes,  QMessageBox::Cancel );
  }
  if( ret_val ==  QMessageBox::Yes ) {
    fileExit();
  } else if( ret_val ==  QMessageBox::No ) {
    fileClose();
  } else  //  QMessageBox::Cancel
  {
  }
}

/*! 
 * Stores the window geometry for later restoration.
 */
void 
OpenSpeedshop::storeGeometry()
{
  QSettings settings;
  settings.writeEntry("/openspeedshop/geometry/x", this->x());
  settings.writeEntry("/openspeedshop/geometry/y", this->y());
  settings.writeEntry("/openspeedshop/geometry/width", this->width());
  settings.writeEntry("/openspeedshop/geometry/height", this->height());
}

