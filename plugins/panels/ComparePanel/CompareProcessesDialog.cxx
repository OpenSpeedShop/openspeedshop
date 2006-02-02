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
  

#include "CompareProcessesDialog.hxx"

#include "PanelListViewItem.hxx"

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
#include <qtooltip.h>
#include <qapplication.h>

#include "SS_Input_Manager.hxx"
CompareProcessesDialog::CompareProcessesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() constructor called.\n");

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;

  
  if ( !name ) setName( "CompareProcessesDialog" );

  setSizeGripEnabled( TRUE );
  CompareProcessesDialogLayout = new QVBoxLayout( this, 11, 6, "CompareProcessesDialogLayout"); 

  headerLabel = new QLabel(this, "headerLabel");
  CompareProcessesDialogLayout->addWidget( headerLabel );

  availableProcessesListView = new QListView( this, "availableProcessesListView" );
  availableProcessesListView->addColumn( tr( "Available Processes:" ) );
  availableProcessesListView->setSelectionMode( QListView::Single );
  availableProcessesListView->setShowSortIndicator( TRUE );
  availableProcessesListView->setSorting( 0, FALSE );
  availableProcessesListView->setAllColumnsShowFocus(TRUE);
  availableProcessesListView->setSortOrder( Qt::Ascending );
  availableProcessesListView->setRootIsDecorated(TRUE);
  availableProcessesListView->setResizeMode(QListView::LastColumn);

  CompareProcessesDialogLayout->addWidget( availableProcessesListView );

 QToolTip::add( availableProcessesListView, tr( "Drag-n-drop entries from this list onto the Column containing your process group." ) );

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
  CompareProcessesDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CompareProcessesDialog::~CompareProcessesDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("CompareProcessesDialog::CompareProcessesDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CompareProcessesDialog::languageChange()
{
  setCaption( tr( name() ) );
  headerLabel->setText( tr( "Add/Delete processes:" ) );
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
}

PanelListViewItem *
CompareProcessesDialog::selectedExperiment(int *expID)
{
  PanelListViewItem *selectedItem = (PanelListViewItem *)availableProcessesListView->selectedItem();


  if( selectedItem )
  {

  } else
  {
    nprintf( DEBUG_PANELS ) ("NO ITEMS SELECTED\n");
    return( NULL );
  }
}


void
CompareProcessesDialog::updateInfo()
{

  QApplication::restoreOverrideCursor();
}
