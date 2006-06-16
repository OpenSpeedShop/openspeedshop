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
  

#include "GenericProgressDialog.hxx"

#include "debug.hxx"

#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qapplication.h>

GenericProgressDialog::GenericProgressDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("GenericProgressDialog::GenericProgressDialog() constructor called.\n");

  if ( !name ) setName( "GenericProgressDialog" );

  setSizeGripEnabled( TRUE );
  GenericProgressDialogLayout = new QVBoxLayout( this, 11, 6, "GenericProgressDialogLayout"); 

  infoLabel = new QLabel( this, "infoLabel" );
  qs = new QSlider(this, "qslider");
  qs->setOrientation(Qt::Horizontal);
  qs->setRange(0,10);
  GenericProgressDialogLayout->addWidget( infoLabel );
  GenericProgressDialogLayout->addWidget( qs );

  languageChange();
  resize( QSize(500, 60).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
GenericProgressDialog::~GenericProgressDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("GenericProgressDialog::GenericProgressDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GenericProgressDialog::languageChange()
{
  setCaption( tr( name() ) );
  infoLabel->setText( tr("Loading executable...") );
}
