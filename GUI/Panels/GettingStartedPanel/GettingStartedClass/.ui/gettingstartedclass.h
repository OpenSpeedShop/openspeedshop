/****************************************************************************
** Form interface generated from reading ui file 'gettingstartedclass.ui'
**
** Created: Fri Jun 25 14:22:52 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GETTINGSTARTEDCLASS_H
#define GETTINGSTARTEDCLASS_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QTabWidget;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QTextEdit;

class GettingStartedClass : public QWidget
{
    Q_OBJECT

public:
    GettingStartedClass( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~GettingStartedClass();

    QLabel* textLabel2;
    QTabWidget* tabWidget3_2;
    QWidget* tab;
    QLabel* textLabel6_2_2;
    QLineEdit* lineEdit2_2_2;
    QListBox* listBox1;
    QLabel* textLabel7_2_2;
    QLineEdit* lineEdit3_2_2;
    QPushButton* pushButton1_2_2;
    QWidget* tab_2;
    QLabel* textLabel5_3;
    QLineEdit* lineEdit1_3;
    QLabel* textLabel6_3;
    QLineEdit* lineEdit2_3;
    QTextEdit* textEdit6_3;
    QLabel* textLabel7_3;
    QLineEdit* lineEdit3_3;
    QPushButton* pushButton1_3;
    QWidget* tab_3;
    QLabel* textLabel5_2_2;
    QLineEdit* lineEdit1_2_2;
    QLabel* textLabel8_2;
    QTextEdit* textEdit7_2;
    QLabel* textLabel9_2;
    QLineEdit* lineEdit5_2;
    QPushButton* pushButton2_2;
    QTextEdit* textEdit5;

public slots:
    virtual void experimentListSlot( QListBoxItem * );

protected:
    QVBoxLayout* GettingStartedClassLayout;
    QVBoxLayout* tabLayout;
    QHBoxLayout* layout3_2_2;
    QHBoxLayout* layout5_2_2;
    QHBoxLayout* layout4_2_2;
    QVBoxLayout* tabLayout_2;
    QHBoxLayout* layout1_3;
    QHBoxLayout* layout3_3;
    QHBoxLayout* layout5_3;
    QHBoxLayout* layout4_3;
    QVBoxLayout* tabLayout_3;
    QHBoxLayout* layout1_2_2;
    QHBoxLayout* layout8_2;
    QHBoxLayout* layout7_2;

protected slots:
    virtual void languageChange();

};

#endif // GETTINGSTARTEDCLASS_H
