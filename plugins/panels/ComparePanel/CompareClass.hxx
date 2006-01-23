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
  

#ifndef COMPARECLASS_H
#define COMPARECLASS_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpopupmenu.h>

#include "openspeedshop.hxx"
#include "CLIInterface.hxx"

#include "CollectorListObject.hxx"  // For getting pid list off a host...
#include "CollectorEntryClass.hxx"

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
class QTabWidget;
class QComboBox;

class PanelContainer;
class ComparePanel;
class CollectorListObject;

typedef std::pair<int64_t, std::string> pair_def;

typedef QValueList <QListView *> CompareList;

class CompareClass : public QWidget
{
    Q_OBJECT

public:
    CompareClass( Panel *p, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, int exp_id = -1 );
    ~CompareClass();

    bool menu(QPopupMenu* contextMenu);

    int expID;
    Panel *p;

protected:
    QHBoxLayout* CompareClassLayout;
    QHBoxLayout* AddCollectorLayout;
    QSplitter *splitter;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

      CompareList compareList;
      QToolBox *csetTB;
      int ccnt;

      QComboBox *experimentComboBox;
      QComboBox *collectorComboBox;
      QComboBox *metricComboBox;
int tcnt;

    CLIInterface *cli;
    OpenSpeedshop *mw;

protected slots:
    virtual void languageChange();

public slots:
    void addNewCSet();

private slots:
    void addNewColumn();
    void addNewColumn(QTabWidget *);
    void removeCSet();
    void removeRaisedTab();
    void removeUserPSet();
    void focusOnCSet();
    void focusOnCSetSelected();
    void addProcessesSelected();

    void changeExperiment( const QString &path );
    void changeCollector( const QString &path );

private:
    void updateInfo();
    void gatherInfo(QString collector_name = QString::null);
    CollectorListObject *clo;
    CollectorEntry *ce;

    std::vector<pair_def> experiment_list;

};

#endif // COMPARECLASS_H
