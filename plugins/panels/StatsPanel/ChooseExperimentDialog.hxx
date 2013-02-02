////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013  Krell Institute  All Rights Reserved.
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

#ifndef CHOOSEEXPERIMENTDIALOG_H
#define CHOOSEEXPERIMENTDIALOG_H

#include <stddef.h>
#include <qdialog.h>
#include <vector>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QPushButton;
class QCheckBox;
class QGroupBox;
class QSpacerItem;

class ChooseExperimentDialog : public QDialog
{
    Q_OBJECT

public:
    ChooseExperimentDialog( QWidget* parent = 0, const char* name = 0, std::vector<int> *argCompareExpIDs = NULL,  std::vector<QString> *argCompareExpDBNames = NULL, std::vector<QString> *argCompareSrcFilenames = NULL, int argFocusOnExpIDsCheckBox=-1, bool modal = FALSE, WFlags f = 0 );
    ~ChooseExperimentDialog();

    QHBoxLayout *buttonLayout;
    QSpacerItem *buttonSpacer;
    QVBoxLayout *mainLayout;
    QVBoxLayout *checkBoxLayout;
    std::vector<int> *compareExpIDs;
    std::vector<QString> *compareExpDBNames;
    std::vector<QString> *compareSrcFilenames;
    int focusOnExpIDsCheckBox;

    QButtonGroup *ExperimentChoiceBG;
    QGroupBox *GeneralGroupBox;
    QCheckBox *ExpChoiceCheckBox1;
    QCheckBox *ExpChoiceCheckBox2;
    QCheckBox *ExpChoiceCheckBox3;
    QCheckBox *ExpChoiceCheckBox4;
    QCheckBox *ExpChoiceCheckBox5;
    QCheckBox *ExpChoiceCheckBox6;
    QPushButton *buttonCancel;
    QPushButton *buttonOk;
    int focusExpID;

protected:

public slots:
    void buttonOkSelected();
    void buttonCancelSelected();
    void ExpChoiceCheckBox1Selected();
    void ExpChoiceCheckBox2Selected();
    void ExpChoiceCheckBox3Selected();
    void ExpChoiceCheckBox4Selected();
    void ExpChoiceCheckBox5Selected();

protected slots:
    virtual void languageChange();

};

#endif // CHOOSEEXPERIMENTDIALOG_H

