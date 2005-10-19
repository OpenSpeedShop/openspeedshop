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
  

#ifndef ATTACHCOLLECTORDIALOG_H
#define ATTACHCOLLECTORDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpopupmenu.h>

#include "openspeedshop.hxx"
#include "CLIInterface.hxx"

#include "CollectorListObject.hxx"  // For getting pid list off a host...
#include "CollectorEntryClass.hxx"

#include "GenericProgressDialog.hxx"

enum DialogSortType  { COLLECTOR_T, PID_T, MPIRANK_T, HOST_T };

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

class PanelContainer;
class ManageProcessesPanel;

class ManageCollectorsClass : public QWidget
{
    Q_OBJECT

public:
    ManageCollectorsClass( Panel *p, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, int exp_id = -1 );
    ~ManageCollectorsClass();

    QListView* attachCollectorsListView;

    QString selectedCollectors();
    void updateAttachedList();

    CollectorListObject *clo;

    bool menu(QPopupMenu* contextMenu);

    int expID;
    Panel *p;

protected:
    QVBoxLayout* ManageCollectorsClassLayout;
    QHBoxLayout* AddCollectorLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

    CLIInterface *cli;
    OpenSpeedshop *mw;

    int steps;
    QTimer *loadTimer;
    GenericProgressDialog *pd;

    QPopupMenu *popupMenu;
    QPopupMenu *paramMenu;
    QPopupMenu *collectorMenu;
    QPopupMenu *collectorPopupMenu;
    DialogSortType dialogSortType;
    std::list<std::string> list_of_collectors;
    
protected slots:
    virtual void languageChange();

public slots:
    void contextMenuRequested( QListViewItem *item, const QPoint &pos, int col );


private slots:
    void attachCollectorSelected(int);
    void detachSelected();
    void disableSelected();
    void enableSelected();
    void attachProcessSelected();
    void focusOnProcessSelected();
    void focusOnProcessSelected(QListViewItem*);
    void loadProgramSelected();
    void fileCollectorAboutToShowSelected();
    void paramSelected(int);
    void sortByProcess();
    void sortByCollector();
    void sortByHost();
    void sortByMPIRank();
    void updatePanel();

    void progressUpdate();

private:

};

#endif // ATTACHCOLLECTORDIALOG_H
