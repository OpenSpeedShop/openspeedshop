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
  

#include "ManageCollectorsDialog.hxx"

#include "debug.hxx"

#include <qvariant.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistview.h>

ManageCollectorsDialog::ManageCollectorsDialog( QWidget* parent, const char* name, bool modal, WFlags fl, int exp_id )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsDialog::ManageCollectorsDialog() constructor called.\n");
  
  clo = NULL;
  expID = exp_id;
  if ( !name ) setName( "ManageCollectorsDialog" );

  setSizeGripEnabled( TRUE );
  ManageCollectorsDialogLayout = new QVBoxLayout( this, 11, 6, "ManageCollectorsDialogLayout"); 

  attachHostLabel = new QLabel( this, "attachHostLabel" );
  ManageCollectorsDialogLayout->addWidget( attachHostLabel );
  attachHostComboBox = new QComboBox( this, "attachHostComboBox");
  attachHostComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, attachHostComboBox->sizePolicy().hasHeightForWidth() ) );
  attachHostComboBox->setEditable(TRUE);
  ManageCollectorsDialogLayout->addWidget( attachHostComboBox );

  attachCollectorsListView = new QListView( this, "attachCollectorsListView" );
  attachCollectorsListView->addColumn( tr( "Collectors belonging to '%s':" ) );
  attachCollectorsListView->setSelectionMode( QListView::Single );
  attachCollectorsListView->setAllColumnsShowFocus( FALSE );
  attachCollectorsListView->setShowSortIndicator( FALSE );
  ManageCollectorsDialogLayout->addWidget( attachCollectorsListView );


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
  ManageCollectorsDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( attachHostComboBox, SIGNAL( activated(const QString &) ), this, SLOT( attachHostComboBoxActivated() ) );

  updateAttachedCollectorsList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ManageCollectorsDialog::~ManageCollectorsDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("ManageCollectorsDialog::ManageCollectorsDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ManageCollectorsDialog::languageChange()
{
  setCaption( tr( "ManageCollectorsDialog" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  attachHostLabel->setText( tr("Host:") );
  attachHostComboBox->insertItem( "localhost" );
}

QString
ManageCollectorsDialog::selectedCollectors()
{
  QListViewItem *selectedItem = attachCollectorsListView->selectedItem();
  if( selectedItem )
  {
//    printf("Got an ITEM!\n");
    QString ret_value = selectedItem->text(0);
    return( ret_value );
  } else
  {
//    printf("NO ITEMS SELECTED\n");
    return( NULL );
  }
}



void
ManageCollectorsDialog::updateAttachedCollectorsList()
{
#ifdef OLDWAY
  char *host = (char *)attachHostComboBox->currentText().ascii();
  ProcessEntry *pe = NULL;
  char entry_buffer[1024];

  if( plo )
  {
    delete(plo);
  }
//  printf("look up processes on host=(%s)\n", host);
  plo = new ProcessListObject(host);

  attachCollectorsListView->clear();
  QListViewItem *item_2 = new QListViewItem( attachCollectorsListView, 0 );
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
#else // OLDWAY
  CollectorEntry *ce = NULL;
  char entry_buffer[1024];
  if( clo )
  {
    delete(clo);
  }

  int expID = 1;
  clo = new CollectorListObject(expID);

  CollectorEntryList::Iterator it;
  for( it = clo->collectorEntryList.begin();
       it != clo->collectorEntryList.end();
       ++it )
  {
    ce = (CollectorEntry *)*it;
    sprintf(entry_buffer, "%-20s\n", ce->name );
    QListViewItem *item = new QListViewItem( attachCollectorsListView, 0 );
    item->setText( 0, tr(entry_buffer) );
  }
#endif // OLDWAY
}

void ManageCollectorsDialog::attachHostComboBoxActivated()
{
//  printf("attachHostComboBoxActivated\n");
    updateAttachedCollectorsList();
}
