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
  

#ifndef SELECTEXPERIMENTDIALOG_H
#define SELECTEXPERIMENTDIALOG_H

#include <stddef.h>
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
class QListViewItem;

class SelectExperimentDialog : public QDialog
{
    Q_OBJECT

public:
    SelectExperimentDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SelectExperimentDialog();

    QLabel* headerLabel;
#if 0
    QPushButton* buttonHelp;
#endif // 0
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QListView* availableExperimentsListView;

    PanelListViewItem *selectedExperiment(int *expID);
    PanelListViewItem *updateAvailableExperimentList(int *returned_expID, int *count);

protected:
    QVBoxLayout* SelectExperimentDialogLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

    CLIInterface *cli;
    OpenSpeedshop *mw;

protected slots:
    virtual void languageChange();

public slots:

};

#endif // SELECTEXPERIMENTDIALOG_H
