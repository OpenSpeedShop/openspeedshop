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
  

#include "AttachProgramDialog.hxx"

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

AttachProgramDialog::AttachProgramDialog( QWidget* parent, const char* name, bool modal)
    : QFileDialog( parent, name, modal )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("AttachProgramDialog::AttachProgramDialog() constructor called.\n");
  
  if ( !name ) setName( "AttachProgramDialog" );

  label = new QLabel( tr("Command line options:"), this );
  lineedit = new QLineEdit( this );

  addWidgets( label, lineedit, (QPushButton *)NULL );
}

/*
 *  Destroys the object and frees any allocated resources
 */
AttachProgramDialog::~AttachProgramDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("AttachProgramDialog::AttachProgramDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AttachProgramDialog::languageChange()
{
  setCaption( tr( "AttachProgramDialog" ) );
}
