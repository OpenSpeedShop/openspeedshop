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
  

#include "AboutDialog.hxx"

#include "debug.hxx"

#include <qlabel.h>
#include <qtextedit.h>
#include <qpushbutton.h>


AboutDialog::AboutDialog( QWidget* parent, const char* name, bool modal, WFlags fl, QString message )
    : QDialog( parent, name, modal, fl )
{
// printf("AboutDialog::AboutDialog() constructor called.\n");
  setCaption( name );
  
  setSizeGripEnabled( TRUE );
  AboutDialogLayout = new QVBoxLayout( this, 11, 6, "AboutDialogLayout"); 

  availableHostsLabel = new QLabel( this, "availableHostsLabel" );
  AboutDialogLayout->addWidget( availableHostsLabel );

  textEdit = new QTextEdit( this, "attachCollectorsListView" );

  textEdit->setText(message);

  textEdit->setReadOnly(TRUE);

  AboutDialogLayout->addWidget( textEdit );


  Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

#ifdef HELP
  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setAutoDefault( TRUE );
  Layout1->addWidget( buttonHelp );
#endif // HELP
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

#ifdef OK
  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  Layout1->addWidget( buttonOk );
#endif // OK

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( buttonCancel );
  AboutDialogLayout->addLayout( Layout1 );
  languageChange();
  resize( QSize(511, 282).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  // signals and slots connections
#ifdef OK
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
#endif // OK
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
AboutDialog::~AboutDialog()
{
  // no need to delete child widgets, Qt does it all for us
  nprintf(DEBUG_CONST_DESTRUCT) ("AboutDialog::AboutDialog() destructor called.\n");
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AboutDialog::languageChange()
{
#ifdef HELP
  buttonHelp->setText( tr( "&Help" ) );
  buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
#endif // HELP
#ifdef OK
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAccel( QKeySequence( QString::null ) );
#endif // OK
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAccel( QKeySequence( QString::null ) );
  availableHostsLabel->setText( tr("Information:") );
}
