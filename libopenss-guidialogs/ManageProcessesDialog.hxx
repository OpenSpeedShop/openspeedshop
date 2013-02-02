////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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
  

#ifndef ATTACHPROCESSESDIALOG_H
#define ATTACHPROCESSESDIALOG_H

#include <stddef.h>
#include <qvariant.h>
#include <qdialog.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include "openspeedshop.hxx"
#include "CLIInterface.hxx"

#include "CollectorListObject.hxx"  // For getting pid list off a host...
#include "CollectorEntryClass.hxx"
#include "GenericProgressDialog.hxx"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLabel;
class QComboBox;
class QListView;
class QListViewItem;

class ManageProcessesDialog : public QDialog
{
    Q_OBJECT

public:
    ManageProcessesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, int exp_id = -1 );
    ~ManageProcessesDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QLabel* availableHostsLabel;
    QComboBox * availableHostsComboBox;
    QListView* attachCollectorsListView;

    QString selectedCollectors();
    void updateAttachedCollectorsList();

    CollectorListObject *clo;

protected:
    QVBoxLayout* ManageProcessesDialogLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

    CLIInterface *cli;
    OpenSpeedshop *mw;

    int steps;
    QTimer *loadTimer;
    GenericProgressDialog *pd;

protected slots:
    virtual void languageChange();

public slots:
    virtual void availableHostsComboBoxActivated();
    void contextMenuRequested( QListViewItem *item, const QPoint &pos, int col );


private slots:
    void addProcessSelected();
    void removeEntrySelected();
    void addProgramSelected();
    void progressUpdate();

private:
    int expID;

};

#endif // ATTACHPROCESSESDIALOG_H
