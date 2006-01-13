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
  


#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QSplitter;
class QFrame;
class QListView;
class QListViewItem;
class QWidgetStack;
class QWidget;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QCheckBox;
#include "qsettings.h"

class PanelContainer;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PreferencesDialog();

    PanelContainer *panelContainer;

    QSplitter* mainSplitter;
    QFrame* preferenceDialogLeftFrame;
    QListView* categoryListView;
    QFrame* preferenceDialogRightFrame;
    QWidgetStack* preferenceDialogWidgetStack;
    QWidget* generalStackPage;
    QGroupBox* GeneralGroupBox;
    QPushButton* setFontButton;
    QLineEdit* fontLineEdit;
    QLabel* remoteShellLabel;
    QLineEdit* remoteShellEdit;
    QCheckBox* setShowSplashScreenCheckBox;
    QCheckBox* showGraphicsCheckBox;
    QPushButton* buttonHelp;
    QPushButton* buttonDefaults;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QWidget *matchPreferencesToStack(QString s);
    void createGeneralStackPage(QWidgetStack* stack, char *name );

    bool preferencesAvailable;
QString globalRemoteShell;
    QString globalFontFamily;
    int globalFontPointSize;
    int globalFontWeight;
    bool globalFontItalic;

    int viewFieldSize;
    int viewPrecision; 
    int historyLimit; 
    int historyDefault; 
    int maxAsyncCommands; 
    int helpLevelDefault; 
    bool viewFullPath; 
    bool saveExperimentDatabase; 
    bool allowPythonCommands; 

    QSettings *settings;
    void readPreferencesOnEntry();
    void savePreferences();
    void applyPreferences();
public slots:
    virtual void resetPreferenceDefaults();
    virtual void listItemSelected(QListViewItem *);
    virtual void selectGlobalFont();
    virtual void buttonApplySelected();
    virtual void buttonOkSelected();

protected:
    QVBoxLayout* PreferencesDialogLayout;
    QVBoxLayout* preferenceDialogListLayout;
    QVBoxLayout* preferenceDialogLeftFrameLayout;
    QVBoxLayout* preferenceDialogRightFrameLayout;
    QVBoxLayout* generalStackPageLayout;
    QVBoxLayout* rightSideLayout;
    QHBoxLayout* fontLayout;
    QHBoxLayout* remoteShellLayout;
    QVBoxLayout* generalStackPageLayout_4;
    QHBoxLayout* preferenceDialogWidgetStackLayout;
    QSpacerItem* Horizontal_Spacing2;

    QHBoxLayout *viewFieldSizeLayout;
      QLabel *viewFieldSizeLabel;
      QLineEdit *viewFieldSizeLineEdit;
    QHBoxLayout *viewPrecisionLayout;
      QLabel *viewPrecisionLabel;
      QLineEdit *viewPrecisionLineEdit;
    QHBoxLayout *historyLimitLayout;
      QLabel *historyLimitLabel;
      QLineEdit *historyLimitLineEdit;
    QHBoxLayout *historyDefaultLayout;
      QLabel *historyDefaultLabel;
      QLineEdit *historyDefaultLineEdit;
    QHBoxLayout *maxAsyncCommandsLayout;
      QLabel *maxAsyncCommandsLabel;
      QLineEdit *maxAsyncCommandsLineEdit;
    QHBoxLayout *helpLevelDefaultLayout;
      QLabel *helpLevelDefaultLabel;
      QLineEdit *helpLevelDefaultLineEdit;
    QCheckBox *viewFullPathCheckBox;
    QCheckBox *saveExperimentDatabaseCheckBox;
    QCheckBox *allowPythonCommandsCheckBox;
    QCheckBox *logByDefaultCheckBox;
    QCheckBox *limitSignalCatchingCheckBox;

protected slots:
    virtual void languageChange();

};

#endif // PREFERENCESDIALOG_H
