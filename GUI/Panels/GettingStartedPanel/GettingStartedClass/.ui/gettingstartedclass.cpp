/****************************************************************************
** Form implementation generated from reading ui file 'gettingstartedclass.ui'
**
** Created: Fri Jun 25 14:22:52 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "gettingstartedclass.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../gettingstartedclass.ui.h"
/*
 *  Constructs a GettingStartedClass as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
GettingStartedClass::GettingStartedClass( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "GettingStartedClass" );
    GettingStartedClassLayout = new QVBoxLayout( this, 11, 6, "GettingStartedClassLayout"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );
    GettingStartedClassLayout->addWidget( textLabel2 );

    tabWidget3_2 = new QTabWidget( this, "tabWidget3_2" );

    tab = new QWidget( tabWidget3_2, "tab" );
    tabLayout = new QVBoxLayout( tab, 11, 6, "tabLayout"); 

    layout3_2_2 = new QHBoxLayout( 0, 0, 6, "layout3_2_2"); 

    textLabel6_2_2 = new QLabel( tab, "textLabel6_2_2" );
    textLabel6_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel6_2_2->sizePolicy().hasHeightForWidth() ) );
    layout3_2_2->addWidget( textLabel6_2_2 );

    lineEdit2_2_2 = new QLineEdit( tab, "lineEdit2_2_2" );
    layout3_2_2->addWidget( lineEdit2_2_2 );
    tabLayout->addLayout( layout3_2_2 );

    listBox1 = new QListBox( tab, "listBox1" );
    tabLayout->addWidget( listBox1 );

    layout5_2_2 = new QHBoxLayout( 0, 0, 6, "layout5_2_2"); 

    layout4_2_2 = new QHBoxLayout( 0, 0, 6, "layout4_2_2"); 

    textLabel7_2_2 = new QLabel( tab, "textLabel7_2_2" );
    textLabel7_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel7_2_2->sizePolicy().hasHeightForWidth() ) );
    layout4_2_2->addWidget( textLabel7_2_2 );

    lineEdit3_2_2 = new QLineEdit( tab, "lineEdit3_2_2" );
    layout4_2_2->addWidget( lineEdit3_2_2 );
    layout5_2_2->addLayout( layout4_2_2 );

    pushButton1_2_2 = new QPushButton( tab, "pushButton1_2_2" );
    layout5_2_2->addWidget( pushButton1_2_2 );
    tabLayout->addLayout( layout5_2_2 );
    tabWidget3_2->insertTab( tab, QString("") );

    tab_2 = new QWidget( tabWidget3_2, "tab_2" );
    tabLayout_2 = new QVBoxLayout( tab_2, 11, 6, "tabLayout_2"); 

    layout1_3 = new QHBoxLayout( 0, 0, 6, "layout1_3"); 

    textLabel5_3 = new QLabel( tab_2, "textLabel5_3" );
    textLabel5_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel5_3->sizePolicy().hasHeightForWidth() ) );
    layout1_3->addWidget( textLabel5_3 );

    lineEdit1_3 = new QLineEdit( tab_2, "lineEdit1_3" );
    layout1_3->addWidget( lineEdit1_3 );
    tabLayout_2->addLayout( layout1_3 );

    layout3_3 = new QHBoxLayout( 0, 0, 6, "layout3_3"); 

    textLabel6_3 = new QLabel( tab_2, "textLabel6_3" );
    textLabel6_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel6_3->sizePolicy().hasHeightForWidth() ) );
    layout3_3->addWidget( textLabel6_3 );

    lineEdit2_3 = new QLineEdit( tab_2, "lineEdit2_3" );
    layout3_3->addWidget( lineEdit2_3 );
    tabLayout_2->addLayout( layout3_3 );

    textEdit6_3 = new QTextEdit( tab_2, "textEdit6_3" );
    textEdit6_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, textEdit6_3->sizePolicy().hasHeightForWidth() ) );
    textEdit6_3->setMaximumSize( QSize( 32767, 32767 ) );
    tabLayout_2->addWidget( textEdit6_3 );

    layout5_3 = new QHBoxLayout( 0, 0, 6, "layout5_3"); 

    layout4_3 = new QHBoxLayout( 0, 0, 6, "layout4_3"); 

    textLabel7_3 = new QLabel( tab_2, "textLabel7_3" );
    textLabel7_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel7_3->sizePolicy().hasHeightForWidth() ) );
    layout4_3->addWidget( textLabel7_3 );

    lineEdit3_3 = new QLineEdit( tab_2, "lineEdit3_3" );
    layout4_3->addWidget( lineEdit3_3 );
    layout5_3->addLayout( layout4_3 );

    pushButton1_3 = new QPushButton( tab_2, "pushButton1_3" );
    layout5_3->addWidget( pushButton1_3 );
    tabLayout_2->addLayout( layout5_3 );
    tabWidget3_2->insertTab( tab_2, QString("") );

    tab_3 = new QWidget( tabWidget3_2, "tab_3" );
    tabLayout_3 = new QVBoxLayout( tab_3, 11, 6, "tabLayout_3"); 

    layout1_2_2 = new QHBoxLayout( 0, 0, 6, "layout1_2_2"); 

    textLabel5_2_2 = new QLabel( tab_3, "textLabel5_2_2" );
    textLabel5_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel5_2_2->sizePolicy().hasHeightForWidth() ) );
    layout1_2_2->addWidget( textLabel5_2_2 );

    lineEdit1_2_2 = new QLineEdit( tab_3, "lineEdit1_2_2" );
    layout1_2_2->addWidget( lineEdit1_2_2 );
    tabLayout_3->addLayout( layout1_2_2 );

    textLabel8_2 = new QLabel( tab_3, "textLabel8_2" );
    textLabel8_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, textLabel8_2->sizePolicy().hasHeightForWidth() ) );
    tabLayout_3->addWidget( textLabel8_2 );

    textEdit7_2 = new QTextEdit( tab_3, "textEdit7_2" );
    tabLayout_3->addWidget( textEdit7_2 );

    layout8_2 = new QHBoxLayout( 0, 0, 6, "layout8_2"); 

    layout7_2 = new QHBoxLayout( 0, 0, 6, "layout7_2"); 

    textLabel9_2 = new QLabel( tab_3, "textLabel9_2" );
    layout7_2->addWidget( textLabel9_2 );

    lineEdit5_2 = new QLineEdit( tab_3, "lineEdit5_2" );
    layout7_2->addWidget( lineEdit5_2 );
    layout8_2->addLayout( layout7_2 );

    pushButton2_2 = new QPushButton( tab_3, "pushButton2_2" );
    layout8_2->addWidget( pushButton2_2 );
    tabLayout_3->addLayout( layout8_2 );
    tabWidget3_2->insertTab( tab_3, QString("") );
    GettingStartedClassLayout->addWidget( tabWidget3_2 );

    textEdit5 = new QTextEdit( this, "textEdit5" );
    textEdit5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, textEdit5->sizePolicy().hasHeightForWidth() ) );
    textEdit5->setMaximumSize( QSize( 32767, 25 ) );
    GettingStartedClassLayout->addWidget( textEdit5 );
    languageChange();
    resize( QSize(422, 303).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( listBox1, SIGNAL( clicked(QListBoxItem*) ), this, SLOT( experimentListSlot(QListBoxItem*) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
GettingStartedClass::~GettingStartedClass()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GettingStartedClass::languageChange()
{
    setCaption( tr( "GettingStartedClass" ) );
    textLabel2->setText( tr( "Select a executable, process/host, or experiment:" ) );
    textLabel6_2_2->setText( tr( "Look in directory:" ) );
    lineEdit2_2_2->setText( tr( "/usr/home/slc" ) );
    listBox1->clear();
    listBox1->insertItem( tr( "Francine.ut12345.expr" ) );
    listBox1->insertItem( tr( "Francine.ut54321.expr" ) );
    textLabel7_2_2->setText( tr( "Selected Executables:" ) );
    pushButton1_2_2->setText( tr( "OK" ) );
    tabWidget3_2->changeTab( tab, tr( "Experiment List" ) );
    textLabel5_3->setText( tr( "Host (list):" ) );
    lineEdit1_3->setText( tr( "hope1" ) );
    textLabel6_3->setText( tr( "Look in directory:" ) );
    lineEdit2_3->setText( tr( "/usr/home/slc" ) );
    textLabel7_3->setText( tr( "Selected Executables:" ) );
    pushButton1_3->setText( tr( "OK" ) );
    tabWidget3_2->changeTab( tab_2, tr( "Application List" ) );
    textLabel5_2_2->setText( tr( "Host (list):" ) );
    lineEdit1_2_2->setText( tr( "hope" ) );
    textLabel8_2->setText( tr( "Process List" ) );
    textEdit7_2->setText( tr( "2932   Francine" ) );
    textLabel9_2->setText( tr( "Selected host/process list:" ) );
    lineEdit5_2->setText( tr( "hope:2932" ) );
    pushButton2_2->setText( tr( "OK" ) );
    tabWidget3_2->changeTab( tab_3, tr( "Running Process List" ) );
    textEdit5->setText( tr( "ft>" ) );
    QToolTip::add( textEdit5, tr( "Input ascii command line" ) );
    QWhatsThis::add( textEdit5, tr( "Put words in here about command line help." ) );
}

