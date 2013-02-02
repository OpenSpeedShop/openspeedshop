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
  

#ifndef ATTACHPROCESSDIALOG_H
#define ATTACHPROCESSDIALOG_H

#include <stddef.h>
#include <qvariant.h>
#include <qdialog.h>
#include <qvaluelist.h>

#include "ProcessListObject.hxx"  // For getting pid list off a host...
#include "ProcessEntryClass.hxx"

typedef QValueList<QString> IncExcList;

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QPushButton;
class QLineEdit;
class QLabel;
class QComboBox;
class QListView;
class QListViewItem;
class QRadioButton;
class QCheckBox;
class QSplitter;
class QSettings;

class AttachProcessDialog : public QDialog
{
    Q_OBJECT

public:
    AttachProcessDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AttachProcessDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* updateOk;
    QPushButton* buttonFilter;
    QPushButton* buttonCancel;

    QLabel* attachHostLabel;
    QComboBox * attachHostComboBox;
    QListView* availableProcessListView;
    QListView* exclusionInclusionList;
    QCheckBox *mpiCB;
    QRadioButton *inclusionRB;
    QRadioButton *exclusionRB;

    QVBoxLayout *layout;
    QFrame *topFrame;
    QFrame *bottomFrame;
    QVBoxLayout *bottomLayout;

    QSplitter *attachProcessSplitter;

    QHBoxLayout *addLayout;
    QLabel *addButtonLabel;
    QLineEdit *addButtonText;
    QPushButton *addButton;

    bool filterFLAG;

//    QString selectedProcesses(bool *);
    QStringList * selectedProcesses(bool *);
    void updateAttachableProcessList();
    void updateFilterList();

    bool addSelectedFLAG; // Workaround for Qt bug that sends errant signal to buttonFilterSelected() 

    ProcessListObject *plo;

    void accept();

    int width;
    int height;

    IncExcList incExcList;

    QSettings *settings;

protected:
    QVBoxLayout* AttachProcessDialogLayout;
    QHBoxLayout* buttonLayout;
    QSpacerItem* buttonSpacing;

    bool includeOrExcludeThisItem(const char *);
    void readFilterList();

private slots:
   void ok_accept();

protected slots:
    virtual void languageChange();

public slots:
    void attachHostComboBoxActivated();
    void buttonFilterSelected();
    void inclusionRBSelected();
    void exclusionRBSelected();
    void addSelected();
    void removeSelected();
    void saveSelected();

};

#endif // ATTACHPROCESSDIALOG_H
