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
  

#ifndef COMPAREPROCESSESDIALOG_H
#define COMPAREPROCESSESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

#include "openspeedshop.hxx"
#include "CLIInterface.hxx"
#include "PanelListViewItem.hxx"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLabel;
class QComboBox;
class QListView;
class MPListView;
class QListViewItem;
class QLineEdit;

class CompareClass;
class CompareSet;
class ColumnSet;

class DescriptionClassObject;
typedef QValueList<DescriptionClassObject *> DescriptionClassObjectList;

class CompareProcessesDialog : public QDialog
{
    Q_OBJECT

public:
    CompareProcessesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CompareProcessesDialog();

    QLabel* headerLabel;
#ifdef PULL
    QLabel* addProcessesHostLabel;
#endif // PULL
    QLabel* addProcessesLabel;
    QLabel* removeProcessesLabel;
    QLineEdit* addProcessesRegExpLineEdit;
#ifdef PULL
    QLineEdit *addProcessesHostRegExpLineEdit;
    QLineEdit* removeProcessesRegExpLineEdit;
#endif // PULL
    QPushButton* buttonHelp;
#ifdef PULL
    QPushButton* buttonOk;
    QPushButton* applyOk;
#endif // PULL
    QPushButton* buttonCancel;

    MPListView* availableProcessesListView;

    PanelListViewItem *selectedExperiment(int *expID);
    void updateInfo();

    void updateFocus(int, CompareClass *, CompareSet *, ColumnSet *);
    CompareClass *compareClass;
    CompareSet *compareSet;
    ColumnSet *columnSet;
    int expID;

protected:
    QVBoxLayout* CompareProcessesDialogLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

    CLIInterface *cli;
    OpenSpeedshop *mw;

private:
    DescriptionClassObjectList * validateHostPid(QString host_pidstr);

#ifdef PULL
    void buttonOkSelected();
    void applyOkSelected();
#endif // PULL

private slots:
    void addProcesses();
    void removeProcesses();

protected slots:
    virtual void languageChange();

public slots:

};

#endif // COMPAREPROCESSESDIALOG_H
