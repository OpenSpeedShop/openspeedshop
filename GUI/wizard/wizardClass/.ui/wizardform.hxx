/****************************************************************************
** Form interface generated from reading ui file 'wizardform.ui'
**
** Created: Thu Mar 18 16:39:16 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef WIZARDFORM_H
#define WIZARDFORM_H

#include <qvariant.h>
#include <qwizard.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QWidget;
class QFrame;
class QLabel;
class QButtonGroup;
class QRadioButton;
class QCheckBox;

class WizardForm : public QWizard
{
    Q_OBJECT

public:
    WizardForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~WizardForm();

    QWidget* page1;
    QFrame* frame3;
    QLabel* textLabel4;
    QLabel* textLabel5;
    QButtonGroup* buttonGroup2;
    QRadioButton* instrumentCodeRadioButton;
    QRadioButton* radioButton2;
    QRadioButton* radioButton3;
    QRadioButton* radioButton4;
    QWidget* page2;
    QFrame* frame4;
    QLabel* textLabel6;
    QButtonGroup* buttonGroup3;
    QCheckBox* checkBox1;
    QCheckBox* checkBox2;
    QCheckBox* checkBox3;
    QCheckBox* checkBox5;
    QCheckBox* checkBox6;
    QCheckBox* checkBox7;
    QWidget* page3;
    QLabel* textLabel7;
    QButtonGroup* buttonGroup4;
    QRadioButton* radioButton6;
    QRadioButton* radioButton7;
    QRadioButton* radioButton8;
    QRadioButton* radioButton9_2;
    QWidget* page4;
    QLabel* textLabel8;
    QButtonGroup* buttonGroup6;
    QCheckBox* checkBox8;
    QCheckBox* checkBox9;
    QRadioButton* radioButton9;
    QLabel* textLabel9;

    virtual void selected( const QString & );

public slots:
    virtual void wizardPageSlot();

protected:
    QVBoxLayout* page1Layout;
    QVBoxLayout* frame3Layout;
    QVBoxLayout* buttonGroup2Layout;
    QVBoxLayout* page2Layout;
    QVBoxLayout* frame4Layout;
    QVBoxLayout* buttonGroup3Layout;
    QVBoxLayout* page3Layout;
    QVBoxLayout* buttonGroup4Layout;
    QVBoxLayout* page4Layout;
    QVBoxLayout* buttonGroup6Layout;

protected slots:
    virtual void languageChange();

};

#endif // WIZARDFORM_H
