////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
  

#include "SelectTimeSegmentDialog.hxx"

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
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qtooltip.h>
#include <qapplication.h>

#include "SS_Input_Manager.hxx"
SelectTimeSegmentDialog::SelectTimeSegmentDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectTimeSegmentDialog::SelectTimeSegmentDialog() constructor called.\n");

  mw = (OpenSpeedshop *)parent;
  cli = mw->cli;

  
  if ( !name ) setName( "SelectTimeSegmentDialog" );

  setSizeGripEnabled( TRUE );
  SelectTimeSegmentDialogLayout = new QVBoxLayout( this, 11, 6, "SelectTimeSegmentDialogLayout"); 

  headerLabel = new QLabel(this, "headerLabel");
  SelectTimeSegmentDialogLayout->addWidget( headerLabel );

  // Insert your timesegment stuff here!
  startLabel = new QLabel(this, "startLabel");
  SelectTimeSegmentDialogLayout->addWidget( startLabel );
  startValue = new QLineEdit(this, "startValue");
  SelectTimeSegmentDialogLayout->addWidget( startValue );

  endLabel = new QLabel(this, "endLabel");
  SelectTimeSegmentDialogLayout->addWidget( endLabel );
  endValue = new QLineEdit(this, "endValue");
  SelectTimeSegmentDialogLayout->addWidget( endValue );


  Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

#if 0
  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  Layout1->addWidget( buttonHelp );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );
#endif // 0

  buttonDefaults = new QPushButton( this, "buttonDefaults" );
  Layout1->addWidget( buttonDefaults );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  Layout1->addWidget( buttonOk );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );
  SelectTimeSegmentDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( buttonDefaults, SIGNAL( clicked() ), this, SLOT( buttonDefaultsSelected() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelectTimeSegmentDialog::~SelectTimeSegmentDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("SelectTimeSegmentDialog::SelectTimeSegmentDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelectTimeSegmentDialog::languageChange()
{
  setCaption( tr( name() ) );
  headerLabel->setText( tr( "Select the time segment:" ) );
  startLabel->setText( tr( "Start Value:" ) );
  endLabel->setText( tr( "End Value:" ) );
  startValue->setText( tr( "0") );
  endValue->setText( tr( "100" ) );
#if 0
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
#endif // 0
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  buttonDefaults->setText( tr( "&Defaults" ) );
  buttonDefaults->setAccel( QKeySequence( QString::null ) );
}

void
SelectTimeSegmentDialog::buttonDefaultsSelected()
{
  languageChange();
}
