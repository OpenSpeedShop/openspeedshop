/****************************************************************************
** Form interface generated from reading ui file 'preferencesdialog.ui'
**
** Created: Thu Jan 13 10:22:22 2005
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

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
    QLabel* precisionTextLabel;
    QLineEdit* precisionLineEdit;
    QCheckBox* setShowSplashScreenCheckBox;
    QCheckBox* setShowColoredTabsCheckBox;
    QCheckBox* deleteEmptyPCCheckBox;
    QCheckBox* showGraphicsCheckBox;
    QPushButton* buttonHelp;
    QPushButton* buttonDefaults;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QWidget *matchPreferencesToStack(QString s);
    void createGeneralStackPage(QWidgetStack* stack, char *name );

    bool preferencesAvailable;
    QString globalFontFamily;
    int globalFontPointSize;
    int globalFontWeight;
    bool globalFontItalic;

    QSettings settings;
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
    QHBoxLayout* precisionLayout;
    QVBoxLayout* generalStackPageLayout_4;
    QHBoxLayout* preferenceDialogWidgetStackLayout;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // PREFERENCESDIALOG_H
