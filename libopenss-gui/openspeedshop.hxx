////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute  All Rights Reserved.
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
  


#ifndef OPENSPEEDSHOP_H
#define OPENSPEEDSHOP_H

#include <stddef.h>
#include <qvariant.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include "PanelContainer.hxx"
#include <qassistantclient.h>
#include "preferencesdialog.hxx"

#include <qfile.h>   // For the file dialog box.
#include <qfiledialog.h>  // For the file dialog box.

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class AttachProgramDialog;
class AttachMPProgramDialog;
class AttachProcessDialog;
class AttachMPProcessDialog;
class AttachArgDialog;
class GenericProgressDialog;

class CLIInterface;

typedef QValueList<QWidget *> PreferencesStackPagesList;

//! The main window skeleton containing statusBar and menubar.
class OpenSpeedshop : public QMainWindow
{
    //! Q_OBJECT is needed as there are slots defined for the OpenSpeedshop
    Q_OBJECT

  public:
    //! Constructor for the QMainWindow.
    OpenSpeedshop( int wid=0, int climode=0, QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );

    //! Destructor for the QMainWindow.
    ~OpenSpeedshop();

    /*! \todo The destructor is never called, so this has to be public so it can be called externally;
              this needs to be fixed by properly disposing of this object. */
    //! Stores the window geometry for later restoration
    void storeGeometry();

    //! The mainwindow menu bar.
    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *helpMenu;

    QAction* fileLoadNewAction;
    QAction* fileAttachNewProcessAction;

    QAction* fileOpenExperimentAction;
    QAction* fileOpenSavedExperimentAction;
    QAction* fileSaveExperimentAction;
#ifdef EXPORT
    QAction* fileExportExperimentDataAction;
#endif // EXPORT
#ifdef SAVESESSION
    QAction* fileSaveSessionAction;
#endif // SAVESESSION
    QAction* filePreferencesAction;
    QAction* fileCloseAction;
    QAction* fileExitAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;

    QAssistantClient *assistant;

    CLIInterface *cli;
    bool climode;

    //! Global flag indicating if we're shutting down.   Caused by File->exit or CmdPanel "exit" command.
    bool shuttingDown;

    //! A list to all plugin panel preferences 
    PreferencesStackPagesList preferencesStackPagesList;

    //! A pointer to the top PanelContainer that is parented to this mainwindow.
    PanelContainer *topPC;

    //! Raise the gui (from the cli after hide())
    void raiseGUI();

    //! The window id string associated with the cli
    QString widStr;

    //! The hinted process id string that is to be attached (eventually).
    QString pidStr;
    QStringList *pidStrList;

    //! The host to the peruse.
    QString hostStr;

    //! The hinted executable_name that is to be loaded (eventually).
    QString executableName;

    //! The args to the executable that is to be loaded (eventually).
    QString parallelPrefixCommandStr;

    //! The args to the executable that is to be loaded (eventually).
    QString argsStr;

    //! The experiment to attach to
    QString expStr;

    //! The thread/rank to attach to
    QString rankStr;

    //! The hinted experiment_name that is to be intialized.. 
    QString experimentName;

    //! The hint that this is an mpi attach
    bool mpiFLAG;

    //! The attach process dialog.
    AttachProcessDialog *afd;

    //! The attach multi-process dialog.
    AttachMPProcessDialog *MPafd;

    //! The load file dialog.
    AttachProgramDialog *lfd;

    //! The load file dialog.
    AttachMPProgramDialog *MPlfd;

    //! The load file dialog.
    AttachArgDialog *Arglfd;

    //! The hinted executable argument filename to be input to executableName program.
    QString ArgFileName;

    void closeEvent(QCloseEvent *e);

    //! The save experiment dialog.
//    QFileDialog *sed;

    //! The save file dialog.
    QFileDialog *sfd;

    PreferencesDialog *preferencesDialog;
    //! print out some internal debug information about this class.
    void print();

    //! Routine that is called when work was done from the command line.
    int lookForExperiment( bool hadOfflineArg);

    //! Routine that is called when no hints are given on the command line.
    void loadTheWizard();


    void fileLoadNewProgram();
    void fileAttachNewProcess();
    
#ifdef EXPORT // move back to 'public slots:' when needed.
    virtual void fileExportExperimentData();
#endif // EXPORT
#ifdef SAVESESSION // move back to 'public slots: when needed.
    virtual void fileSaveSession();
#endif // SAVESESSION

    QVBoxLayout* OpenSpeedshopLayout;
    PanelContainerList topLevelPanelContainersToHideList;
  public slots:
    virtual void fileOpenExperiment(int selectedID=0);
    virtual void fileOpenSavedExperiment(QString filename=QString::null, bool openPanel=TRUE);
    virtual void fileSaveExperiment(int id = 0);
    virtual void filePreferences(QWidget *stackWidgetToRaise = NULL, QString panel_type = QString::null );
    virtual void fileClose();
    virtual void fileExit();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();

    virtual void loadNewProgram();
    virtual Panel* loadNewProgramPanel( PanelContainer *pc, PanelContainer *topPC, int64_t expID, Panel *targetPanel, bool isOffline);
    virtual void loadNewMultiProcessProgram();
    virtual void loadArgumentFile();
    virtual void attachNewProcess();
    virtual void attachNewMultiProcess();

    virtual void myQuit();
    void progressUpdate();
    void raiseTheGUI();

protected slots:
    //! Change the language by calling tr().
    virtual void languageChange();

private:
    //! Routine that is call after initial setup.
    void init();

    //! Routine that is call just pior to this class' destructor.
    void destroy();

    int steps;
    GenericProgressDialog *pd;

    const char *doc_dir;
};

#endif // OPENSPEEDSHOP_H
