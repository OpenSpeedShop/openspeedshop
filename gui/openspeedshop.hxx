/****************************************************************************
** Form interface generated from reading ui file 'openspeedshop.ui'
**
** Created: Fri Jul 2 06:06:22 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef OPENSPEEDSHOP_H
#define OPENSPEEDSHOP_H

#include <qvariant.h>
#include <qmainwindow.h>
#include "PanelContainer.hxx"
#include <qassistantclient.h>

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

//! The main window skeleton containing statusBar and menubar.
class OpenSpeedshop : public QMainWindow
{
    //! Q_OBJECT is needed as there are slots defined for the OpenSpeedshop
    Q_OBJECT

public:
    //! Constructor for the QMainWindow.
    OpenSpeedshop( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );

    //! Destructor for the QMainWindow.
    ~OpenSpeedshop();

    //! The mainwindow menu bar.
    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *helpMenu;
    QAction* fileNewAction;
QAction* fileAttachAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
#ifdef EVENTUALLY
    QAction* fileSaveAsAction;
#endif // EVENTUALLY
    QAction* fileExitAction;
#ifdef EVENTUALLY
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
#endif // EVENTUALLY
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;

    QAssistantClient *assistant;

    //! A pointer to the top PanelContainer that is parented to this mainwindow.
    PanelContainer *topPC;

    //! The hinted process id string that is to be attached (eventually).
    char *pid_str;

    //! The hinted executable_name that is to be loaded (eventually).
    char *executable_name;

    //! The hinted experiment_name that is to be intialized.. 
    char *experiment_name;

    //! The load file dialog.
    QFileDialog *lfd;

    //! The save file dialog.
    QFileDialog *sfd;


#ifdef EVENTUALLY // Move back to 'public slots:' if needed.
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editPaste();
    virtual void editFind();
    virtual void fileSaveAs();
#endif // EVENTUALLY
public slots:
    virtual void fileNew();
    virtual void fileAttach();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileExit();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();

protected:
    //! A vertical box to place the child widgets.
    QVBoxLayout* OpenSpeedshopLayout;

protected slots:
    //! Change the language by calling tr().
    virtual void languageChange();

private:
    //! Routine that is call after initial setup.
    void init();

    //! Routine that is call just pior to this class' destructor.
    void destroy();
};

#endif // OPENSPEEDSHOP_H
