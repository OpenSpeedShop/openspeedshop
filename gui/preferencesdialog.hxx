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

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PreferencesDialog();

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
    QWidget* sourcePanelStackPage;
    QGroupBox* sourcePanelGroupBox;
    QCheckBox* showStatisticsCheckBox;
    QCheckBox* showLineNumbersCheckBox;
    QWidget* statsPanelStackPage;
    QGroupBox* statsPanelGroupBox;
    QCheckBox* sortDecendingCheckBox;
    QLabel* showTopNTextLabel;
    QLineEdit* showTopNLineEdit;
    QWidget* cmdPanelStackPage;
    QGroupBox* cmdPanelGroupBox;
    QPushButton* buttonHelp;
    QPushButton* buttonDefaults;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

bool preferencesAvailable;
QString globalFontFamily;
int globalFontPointSize;
int globalFontWeight;
bool globalFontItalic;
    void readPreferencesOnEntry();
    void savePreferencesOnExit();
    void applyPreferences();
public slots:
    virtual void resetPreferenceDefaults();
    virtual void setGlobalPrecision();
    virtual void setShowSplashScreen();
    virtual void setShowColoredPanelTabs();
    virtual void setRemoveEmptyPC();
    virtual void setShowAvailableGraphics();
    virtual void setShoeLineNumbers();
    virtual void setShowLineNumbers();
    virtual void setSortDescending();
    virtual void setShowTopN();
    virtual void setShowStats();
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
    QVBoxLayout* generalStackPageLayout_2;
    QVBoxLayout* layout6;
    QVBoxLayout* generalStackPageLayout_3;
    QVBoxLayout* layout8;
    QHBoxLayout* layout7;
    QVBoxLayout* generalStackPageLayout_4;
    QHBoxLayout* preferenceDialogWidgetStackLayout;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // PREFERENCESDIALOG_H
