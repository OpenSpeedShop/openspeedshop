/****************************************************************************
** Form interface generated from reading ui file 'introwizardform.ui'
**
** Created: Mon Apr 19 13:12:25 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef INTROWIZARDFORM_H
#define INTROWIZARDFORM_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QWidgetStack;
class QLabel;
class QCheckBox;
class QRadioButton;
class QPushButton;

class IntroWizardForm : public QWidget
{
    Q_OBJECT

public:
    IntroWizardForm( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~IntroWizardForm();

    QFrame* frame5;
    QWidgetStack* widgetStack5;
    QWidget* WStackPage;
    QLabel* epage1Header_2;
    QLabel* vpage1Label;
    QCheckBox* vpage1LoadExperimentCheckBox;
    QFrame* line3;
    QRadioButton* vpage1pcSampleRB;
    QRadioButton* vpage1UserTimeRB;
    QRadioButton* vpage1HardwareCounterRB;
    QRadioButton* vpage1FloatingPointRB;
    QRadioButton* vpage1InputOutputRB;
    QRadioButton* vpage1MpiRB;
    QPushButton* vpage1NextButton;
    QWidget* WStackPage_2;
    QLabel* epage1Header;
    QLabel* epage1Label;
    QCheckBox* epage1LoadExperimentCheckBox;
    QFrame* line2;
    QRadioButton* epage1pcSampleRB;
    QRadioButton* epage1UserTimeRB;
    QRadioButton* epage1HardwareCounterRB;
    QRadioButton* epage1FloatingPointRB;
    QRadioButton* epage1InputOutputRB;
    QRadioButton* epage1MpiRB;
    QPushButton* epage1NextButton;
    QCheckBox* wizardMode;
    QLabel* broughtToYouByLabel;

public slots:
    virtual void wizardModeSelected();
    virtual void epage1NextButtonSelected();
    virtual void vpage1NextButtonSelected();

protected:
    QVBoxLayout* IntroWizardFormLayout;
    QVBoxLayout* frame5Layout;
    QVBoxLayout* WStackPageLayout;
    QVBoxLayout* layout21;
    QHBoxLayout* layout20;
    QSpacerItem* spacer5_3;
    QHBoxLayout* layout9;
    QSpacerItem* spacer5;
    QHBoxLayout* layout10;
    QSpacerItem* spacer6;
    QHBoxLayout* layout21_2;
    QSpacerItem* spacer7;
    QHBoxLayout* layout20_2;
    QSpacerItem* spacer7_3;
    QHBoxLayout* layout19;
    QSpacerItem* spacer7_4;
    QHBoxLayout* layout18;
    QSpacerItem* spacer7_5;
    QHBoxLayout* layout52;
    QSpacerItem* spacer44;
    QVBoxLayout* WStackPageLayout_2;
    QVBoxLayout* layout20_3;
    QHBoxLayout* layout19_2;
    QSpacerItem* spacer5_2_2;
    QHBoxLayout* layout9_2;
    QSpacerItem* spacer5_2;
    QHBoxLayout* layout10_2;
    QSpacerItem* spacer6_2;
    QHBoxLayout* layout21_2_2;
    QSpacerItem* spacer7_2;
    QHBoxLayout* layout20_2_2;
    QSpacerItem* spacer7_3_2;
    QHBoxLayout* layout19_2_2;
    QSpacerItem* spacer7_4_2;
    QHBoxLayout* layout18_2;
    QSpacerItem* spacer7_5_2;
    QHBoxLayout* layout44;
    QSpacerItem* spacer37;
    QHBoxLayout* layout5;
    QSpacerItem* spacer1;

protected slots:
    virtual void languageChange();

};

#endif // INTROWIZARDFORM_H
