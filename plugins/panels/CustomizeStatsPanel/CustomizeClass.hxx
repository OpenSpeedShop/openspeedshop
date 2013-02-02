////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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
  

#ifndef COMPARECLASS_H
#define COMPARECLASS_H

#include <stddef.h>
#include <qvariant.h>
#include <qdialog.h>
#include <qpopupmenu.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>


#include "openspeedshop.hxx"
#include "CLIInterface.hxx"

// #include "CollectorListObject.hxx"  // For getting pid list off a host...
// #include "CollectorEntryClass.hxx"

#include "GenericProgressDialog.hxx"
#include "MPListView.hxx"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLabel;
class QPopupMenu;
class QListView;
class QListViewItem;
class QSplitter;
class QToolBox;
class QToolBar;
class QTabWidget;
class QComboBox;
class QRadioButton;

class PanelContainer;
class CustomizeStatsPanel;
class CompareProcessesDialog;

#include "ColumnSet.hxx"
#include "CompareSet.hxx"

#include "customize_update_icon.xpm"
#include "add_processes_icon.xpm"
#include "remove_processes_icon.xpm"
#include "add_column_icon.xpm"
#include "remove_column_icon.xpm"
#include "focus_stats_icon.xpm"
#include "load_experiment.xpm"


typedef QValueList <QListView *> CompareList;
typedef QValueList <CompareSet *> CompareSetList;

class CustomizeClass : public QWidget
{
    Q_OBJECT

public:
    CustomizeClass( Panel *p, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, int exp_id = -1,  QString expIDStr = QString::null );
    ~CustomizeClass();

    bool menu(QPopupMenu* contextMenu);

    int expID;
    int focusedExpID;
    Panel *p;

    CompareSetList *csl;
    void updateInfo();

    int ccnt;

    CompareProcessesDialog *dialog;
    QString currentCompareTypeStr;
    int currentCompareByType;
    QString getCollectorName();


    QRadioButton *vCompareTypeFunctionRB;
    QRadioButton *vCompareTypeStatementRB;
    QRadioButton *vCompareTypeLinkedObjectRB;
    QButtonGroup *vCompareTypeBG;
    QToolBar *fileTools;
    QSplitter *splitterA;
    QLabel *toolbar_status_label;



protected:
    QVBoxLayout* compareClassLayout;
    QSplitter *splitter;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

      CompareList compareList;
      QToolBox *csetTB;

protected slots:
    virtual void languageChange();

public slots:
    void compareByFunction();
    void compareByStatement();
    void compareByLinkedObject();
    void updatePanel();
    void addNewCSet();
    void addNewColumn();
    void addNewColumn(CompareSet *);
    void addProcessesSelected();
    void removeUserPSet();
    void removeCSet();
    void removeRaisedTab();
    void focusOnCSetSelected();
    void loadAdditionalExperimentSelected();

private slots:

private:

    CompareSet *findCurrentCompareSet();

};

#endif // COMPARECLASS_H
