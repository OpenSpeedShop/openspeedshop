/****************************************************************************
** Form interface generated from reading ui file 'performanceleader.ui'
**
** Created: Fri Jul 2 06:06:22 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PERFORMANCELEADER_H
#define PERFORMANCELEADER_H

#include <qvariant.h>
#include <qmainwindow.h>
#include "PanelContainer.hxx"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;

class PerformanceLeader : public QMainWindow
{
    Q_OBJECT

public:
    PerformanceLeader( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~PerformanceLeader();

    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *helpMenu;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* fileExitAction;
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;

    int frabulate;

//    virtual bool eventFilter( QObject *, QEvent * e );

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void fileExit();
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editPaste();
    virtual void editFind();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();

protected:
    QVBoxLayout* PerformanceLeaderLayout;

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();

};

#endif // PERFORMANCELEADER_H
