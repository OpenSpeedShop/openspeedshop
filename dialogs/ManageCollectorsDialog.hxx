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

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLabel;
class QPopupMenu;
class QComboBox;
class QListView;
class QListViewItem;

class ManageCollectorsDialog : public QDialog
{
    Q_OBJECT

public:
    ManageCollectorsDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, int exp_id = -1 );
    ~ManageCollectorsDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QPushButton* addOk;

    QLabel* availableCollectorsLabel;
    QComboBox * availableCollectorsComboBox;
    QListView* attachCollectorsListView;

    QString selectedCollectors();
    void updateAttachedCollectorsList();

    CollectorListObject *clo;

protected:
    QVBoxLayout* ManageCollectorsDialogLayout;
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
protected slots:
    virtual void languageChange();

public slots:
    virtual void availableCollectorsComboBoxActivated();
    void contextMenuRequested( QListViewItem *item, const QPoint &pos, int col );


private slots:
    void detachSelected();
    void disableSelected();
    void attachProcessSelected();
    void attachProgramSelected();

    void progressUpdate();

    void paramSelected(int);

    void addCollectorSelected();

private:
    int expID;

};

#endif // ATTACHCOLLECTORDIALOG_H
