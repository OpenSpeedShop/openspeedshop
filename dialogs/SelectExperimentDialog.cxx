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
  

#include "SelectExperimentDialog.hxx"

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

#include "openspeedshop.hxx" // For cli

SelectExperimentDialog::SelectExperimentDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectExperimentDialog::SelectExperimentDialog() constructor called.\n");

  OpenSpeedshop *mw = (OpenSpeedshop *)parent;
  cli = mw->cli;
  
  if ( !name ) setName( "SelectExperimentDialog" );

  setSizeGripEnabled( TRUE );
  SelectExperimentDialogLayout = new QVBoxLayout( this, 11, 6, "SelectExperimentDialogLayout"); 

  hostLabel = new QLabel( this, "hostLabel" );
  SelectExperimentDialogLayout->addWidget( hostLabel );
  hostComboBox = new QComboBox( this, "hostComboBox");
  hostComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)0, 0, 0, hostComboBox->sizePolicy().hasHeightForWidth() ) );
  hostComboBox->setEditable(TRUE);
  SelectExperimentDialogLayout->addWidget( hostComboBox );

  availableExperimentsListView = new QListView( this, "availableExperimentsListView" );
  availableExperimentsListView->addColumn( tr( "id:" ) );
  availableExperimentsListView->addColumn( tr( "Name:" ) );
  availableExperimentsListView->addColumn( tr( "Description:" ) );
  availableExperimentsListView->setSelectionMode( QListView::Single );
  availableExperimentsListView->setAllColumnsShowFocus( FALSE );
  availableExperimentsListView->setShowSortIndicator( TRUE );
  availableExperimentsListView->setSorting( 0, FALSE );
  availableExperimentsListView->setSortOrder( Qt::Ascending );
  SelectExperimentDialogLayout->addWidget( availableExperimentsListView );


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
  SelectExperimentDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
#ifdef LATER
  connect( hostComboBox, SIGNAL( activated(const QString &) ), this, SLOT( hostComboBox() ) );
#endif // LATER

  updateAvailableExperimentList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelectExperimentDialog::~SelectExperimentDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectExperimentDialog::SelectExperimentDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelectExperimentDialog::languageChange()
{
  setCaption( tr( name() ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  hostLabel->setText( tr("Host:") );
  hostComboBox->insertItem( "localhost" );
#ifdef LATER
  hostComboBox->insertItem( "clink.americas.sgi.com" );
  hostComboBox->insertItem( "hope.americas.sgi.com" );
  hostComboBox->insertItem( "hope1.americas.sgi.com" );
  hostComboBox->insertItem( "hope2.americas.sgi.com" );
#endif // LATER
}

QString
SelectExperimentDialog::selectedExperiment()
{
  QListViewItem *selectedItem = availableExperimentsListView->selectedItem();
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
SelectExperimentDialog::updateAvailableExperimentList()
{
  char *host = (char *)hostComboBox->currentText().ascii();
  availableExperimentsListView->clear();
  char entry_buffer[1024];

  QString command("listExp");
  std::list<int64_t> int_list;

  int_list.clear();
  if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &int_list ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

  std::list<int64_t>::iterator it;
// printf("int_list.size() =%d\n", int_list.size() );
  for(it = int_list.begin(); it != int_list.end(); it++ )
  {
    int64_t cr_int = (int64_t)(*it);

    printf("Here are the experiment ids that can be saved (%d)\n", cr_int);
    sprintf(entry_buffer, "%4d %-20s %-20s\n", cr_int, "Experiment Name", "other info..." );
    QListViewItem *item = new QListViewItem( availableExperimentsListView, QString("%1").arg(cr_int), "Name", "Description" );
//    item->setText( 0, entry_buffer );
  }
}

void SelectExperimentDialog::attachHostComboBoxActivated()
{
//  printf("hostComboBox\n");
    updateAvailableExperimentList();
}
