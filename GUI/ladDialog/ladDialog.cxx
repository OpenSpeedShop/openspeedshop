/****************************************************************************
** Form implementation generated from reading ui file 'mydialog1.ui'
**
** Created: Fri Apr 23 12:37:14 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "ladDialog.hxx"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qcombobox.h>

#include <qbitmap.h>
#include "rightarrow.xpm"
#include "leftarrow.xpm"

/*
 *  Constructs a MyDialog1 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MyDialog1::MyDialog1( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
plo = NULL;
    if ( !name )
	setName( "MyDialog1" );
    setSizeGripEnabled( TRUE );
    MyDialog1Layout = new QVBoxLayout( this, 11, 6, "MyDialog1Layout"); 

    ladTabWidget = new QTabWidget( this, "ladTabWidget" );

    loadPageWidget = new QWidget( ladTabWidget, "loadPageWidget" );
    ladTabWidget->insertTab( loadPageWidget, QString("") );
{
    loadPageWidgetLayout = new QVBoxLayout( loadPageWidget, 11, 6, "loadPageWidgetLayout"); 
    QPushButton *tempButton = new QPushButton( loadPageWidget, "tempButton" );
    loadPageWidgetLayout->addWidget( tempButton );
    tempButton->setText( tr( "tempButton:" ) );
}

    attachPageWidget = new QWidget( ladTabWidget, "attachPageWidget" );
    attachPageWidgetLayout = new QVBoxLayout( attachPageWidget, 11, 6, "attachPageWidgetLayout"); 

    layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 

    attachIntroTextLabel = new QLabel( attachPageWidget, "attachIntroTextLabel" );
    attachIntroTextLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, attachIntroTextLabel->sizePolicy().hasHeightForWidth() ) );
    layout8->addWidget( attachIntroTextLabel );

    line1 = new QFrame( attachPageWidget, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );
    layout8->addWidget( line1 );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    attachHostLabel = new QLabel( attachPageWidget, "attachHostLabel" );
    layout7->addWidget( attachHostLabel );

  attachHostComboBox = new QComboBox( attachPageWidget, "attachHostComboBox");
  attachHostComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, attachHostComboBox->sizePolicy().hasHeightForWidth() ) );
attachHostComboBox->setEditable(TRUE);
    layout7->addWidget( attachHostComboBox );
    layout8->addLayout( layout7 );
    attachPageWidgetLayout->addLayout( layout8 );

    layout8_2 = new QHBoxLayout( 0, 0, 6, "layout8_2"); 

    availableProcessListView = new QListView( attachPageWidget, "availableProcessListView" );
    availableProcessListView->addColumn( tr( "Processes belonging to '%s':" ) );
    availableProcessListView->setSelectionMode( QListView::Extended );
    availableProcessListView->setAllColumnsShowFocus( FALSE );
    availableProcessListView->setShowSortIndicator( FALSE );
    layout8_2->addWidget( availableProcessListView );

    layout10 = new QVBoxLayout( 0, 0, 6, "layout10"); 
    spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout10->addItem( spacer7 );

    layout9 = new QVBoxLayout( 0, 0, 6, "layout9"); 

    addPushButton = new QPushButton( attachPageWidget, "addPushButton" );
    addPushButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, addPushButton->sizePolicy().hasHeightForWidth() ) );
    QPixmap *rightarrow_pixmap = new QPixmap( rightarrow_xpm );
    rightarrow_pixmap->setMask( rightarrow_pixmap->createHeuristicMask() );
    addPushButton->setPixmap( *rightarrow_pixmap );
    layout9->addWidget( addPushButton );
addPushButton->setFlat(TRUE);

    removePushButton = new QPushButton( attachPageWidget, "removePushButton" );
    removePushButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, removePushButton->sizePolicy().hasHeightForWidth() ) );
    QPixmap *leftarrow_pixmap = new QPixmap( leftarrow_xpm );
    leftarrow_pixmap->setMask( leftarrow_pixmap->createHeuristicMask() );
    removePushButton->setPixmap( *leftarrow_pixmap );
removePushButton->setFlat(TRUE);
    layout9->addWidget( removePushButton );
    layout10->addLayout( layout9 );
    spacer8 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout10->addItem( spacer8 );
    layout8_2->addLayout( layout10 );

    attachToListView = new QListView( attachPageWidget, "attachToListView" );
    attachToListView->addColumn( tr( "List of process to attach:" ) );
    attachToListView->setSelectionMode( QListView::Extended );
    attachToListView->setAllColumnsShowFocus( FALSE );
    layout8_2->addWidget( attachToListView );
    attachPageWidgetLayout->addLayout( layout8_2 );
    ladTabWidget->insertTab( attachPageWidget, QString("") );

    detachPageWidget = new QWidget( ladTabWidget, "detachPageWidget" );
    detachPageWidgetLayout = new QVBoxLayout( detachPageWidget, 11, 6, "detachPageWidgetLayout"); 

    attachedProcessesListView = new QListView( detachPageWidget, "attachedProcessesListView" );
    attachedProcessesListView->addColumn( tr( "List of attached processes:" ) );
    detachPageWidgetLayout->addWidget( attachedProcessesListView );
    ladTabWidget->insertTab( detachPageWidget, QString("") );
    MyDialog1Layout->addWidget( ladTabWidget );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    MyDialog1Layout->addLayout( Layout1 );
    languageChange();
//    resize( QSize(513, 450).expandedTo(minimumSizeHint()) );
    resize( QSize(800, 450).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
//    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelpSelected() ) );
    connect( addPushButton, SIGNAL( clicked() ), this, SLOT( addPushButtonSelected() ) );
    connect( removePushButton, SIGNAL( clicked() ), this, SLOT( removePushButtonSelected() ) );
    connect( attachHostComboBox, SIGNAL( textChanged(const QString &) ), this, SLOT( attachHostComboBoxTextChanged() ) );
    connect( attachHostComboBox, SIGNAL( activated(const QString &) ), this, SLOT( attachHostComboBoxActivated() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MyDialog1::~MyDialog1()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MyDialog1::languageChange()
{
    setCaption( tr( "MyDialog1" ) );
    ladTabWidget->changeTab( loadPageWidget, tr( "Load" ) );
    attachIntroTextLabel->setText( tr( "This page will help attach to a running process (or processes)." ) );
    attachHostLabel->setText( tr( "Host:" ) );
    availableProcessListView->header()->setLabel( 0, tr( "Processes belonging to '%s':" ) );
    availableProcessListView->clear();

    addPushButton->setText( QString::null );
    removePushButton->setText( QString::null );
    attachToListView->header()->setLabel( 0, tr( "List of process to attach:" ) );
    attachToListView->clear();
    QListViewItem *item = new QListViewItem( attachToListView, 0 );
    item->setText( 0, tr( "localhost     5432     b.out" ) );

    ladTabWidget->changeTab( attachPageWidget, tr( "Attach" ) );
    attachedProcessesListView->header()->setLabel( 0, tr( "List of attached processes:" ) );
    attachedProcessesListView->clear();
    QListViewItem * item_3 = new QListViewItem( attachedProcessesListView, 0 );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, 0 );
    item->setText( 0, tr( "5432     b.out" ) );
    item_3->setOpen( TRUE );
    item = new QListViewItem( item_3, item );
    item->setText( 0, tr( "1234     a.out" ) );
    item_3->setText( 0, tr( "localhost" ) );

    ladTabWidget->changeTab( detachPageWidget, tr( "Detach" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );

attachHostComboBox->insertItem( tr("localhost") );
attachHostComboBox->insertItem( tr("clink.americas.sgi.com") );
attachHostComboBox->insertItem( tr("hope.americas.sgi.com") );
attachHostComboBox->insertItem( tr("hope2.americas.sgi.com") );
attachHostComboBox->insertItem( tr("leaper.americas.sgi.com") );
}

void MyDialog1::buttonHelpSelected()
{
    qWarning( "MyDialog1::buttonHelpSelected(): Not implemented yet" );
}

void MyDialog1::addPushButtonSelected()
{
    qWarning( "MyDialog1::addPushButtonSelected(): Not implemented yet" );
}

void MyDialog1::removePushButtonSelected()
{
    qWarning( "MyDialog1::removePushButtonSelected(): Not implemented yet" );
}

void MyDialog1::attachHostComboBoxTextChanged()
{
printf("TextChanged\n");
//    updateAttachableProcessList();
}

void MyDialog1::attachHostComboBoxActivated()
{
printf("attachHostComboBoxActivated\n");
    updateAttachableProcessList();
}

// Begin: Non designer specific code:
void MyDialog1::updateAttachableProcessList()
{
  char *host = (char *)attachHostComboBox->currentText().ascii();
  ProcessEntry *pe = NULL;
  char entry_buffer[1024];

  if( plo )
  {
    delete(plo);
  }
printf("look up processes on host=(%s)\n", host);
  plo = new ProcessListObject(host);

  availableProcessListView->clear();
  QListViewItem *item_2 = new QListViewItem( availableProcessListView, 0 );
  item_2->setOpen( TRUE );
  item_2->setText( 0, tr( "hostname" ) );

  ProcessEntryList::Iterator it;
  for( it = plo->processEntryList.begin();
       it != plo->processEntryList.end();
       ++it )
  {
    pe = (ProcessEntry *)*it;
//    printf("%-20s %-10d %-20s\n", pe->host_name, pe->pid, pe->process_name);
    sprintf(entry_buffer, "%-20s %-10d %-20s\n", pe->host_name, pe->pid, pe->process_name);
    QListViewItem *item = new QListViewItem( item_2, 0 );
    item->setText( 0, tr(entry_buffer) );
  }
    
/*
    QListViewItem * item_2 = new QListViewItem( availableProcessListView, 0 );
    item_2->setOpen( TRUE );
    QListViewItem * item = new QListViewItem( item_2, 0 );
    item->setText( 0, tr( "5432      b.out" ) );
    item_2->setOpen( TRUE );
    item = new QListViewItem( item_2, item );
    item->setText( 0, tr( "1234      a.out" ) );
    item_2->setText( 0, tr( "localhost" ) );
*/
}
// End: Non designer specific code:
