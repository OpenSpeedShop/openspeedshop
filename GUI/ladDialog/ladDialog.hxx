/****************************************************************************
** Form interface generated from reading ui file 'mydialog1.ui'
**
** Created: Fri Apr 23 12:37:13 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MYDIALOG1_H
#define MYDIALOG1_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QWidget;
class QLabel;
class QFrame;
class QLineEdit;
class QPushButton;
class QListView;
class QListViewItem;
class QComboBox;

#include "ProcessListObject.hxx"  // For getting pid list off a host...
#include "ProcessEntryClass.hxx"
#include <qvaluelist.h>

class MyDialog1 : public QDialog
{
    Q_OBJECT

public:

// Begin: Non "designer" added functions.
    void updateAttachableProcessList();
    ProcessListObject *plo;
// End: Non "designer" added functions.

    MyDialog1( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MyDialog1();

    QTabWidget* ladTabWidget;
    QWidget* loadPageWidget;
    QWidget* attachPageWidget;
    QLabel* attachIntroTextLabel;
    QFrame* line1;
    QLabel* attachHostLabel;
    QComboBox * attachHostComboBox;
    QListView* availableProcessListView;
    QPushButton* addPushButton;
    QPushButton* removePushButton;
    QListView* attachToListView;
    QWidget* detachPageWidget;
    QListView* attachedProcessesListView;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

public slots:
    virtual void buttonHelpSelected();
    virtual void addPushButtonSelected();
    virtual void removePushButtonSelected();
    virtual void attachHostComboBoxTextChanged();
    virtual void attachHostComboBoxActivated();

protected:
    QVBoxLayout* MyDialog1Layout;
QVBoxLayout* loadPageWidgetLayout;
    QVBoxLayout* attachPageWidgetLayout;
    QVBoxLayout* layout8;
    QHBoxLayout* layout7;
    QHBoxLayout* layout8_2;
    QVBoxLayout* layout10;
    QSpacerItem* spacer7;
    QSpacerItem* spacer8;
    QVBoxLayout* layout9;
    QVBoxLayout* detachPageWidgetLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // MYDIALOG1_H
