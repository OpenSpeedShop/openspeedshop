////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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
  
// Debug Flag 
//#define DEBUG_AAD 1
//

#include "AttachArgDialog.hxx"
//jeg#include "../plugins/panels/ManageProcessesPanel/preference_plugin_info.hxx"

#include "debug.hxx"


#include <qcheckbox.h>
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
#include <qsettings.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qdialog.h>

AttachArgDialog::AttachArgDialog( QWidget* parent, const char* name, bool modal)
    : QFileDialog( parent, name, modal )
{

    QObjectList *l = topLevelWidget()->queryList( "QPushButton" );
    QObjectListIt it( *l ); // iterate over the buttons
    QObject *obj;

    while ( (obj = it.current()) != 0 ) {
        // for each found object...
        ++it;
#ifdef DEBUG_AAD
       printf("AttachArgDialog::AttachArgDialog, button->text=%s\n",((QPushButton*)obj)->text().ascii());
#endif
        char* result = (char *)strstr( "OK", ((QPushButton*)obj)->text().ascii() );
        if( result != NULL ) {
          connect(((QPushButton*)obj), SIGNAL( clicked() ), this, SLOT( accepted() ) );
        }
    }
    delete l; // delete the list, not the objects

  nprintf(DEBUG_CONST_DESTRUCT) ("AttachArgDialog::AttachArgDialog() constructor called.\n");
#ifdef DEBUG_AAD
  printf("AttachArgDialog::AttachArgDialog() constructor called.\n");
#endif
  
  // Set the name of this dialog box
  if ( !name ) setName( "AttachArgDialog" );

#ifdef DEBUG_AAD
  printf("AttachArgDialog::AttachArgDialog() constructor exits.\n");
#endif
}


/*
 *  Destroys the object and frees any allocated resources
 */
AttachArgDialog::~AttachArgDialog()
{
#ifdef DEBUG_AAD
  printf("AttachArgDialog::AttachArgDialog() destructor called.\n");
#endif

// no need to delete child widgets, Qt does it all for us
  
#ifdef DEBUG_AAD
  printf("AttachArgDialog::AttachArgDialog() destructor exits.\n");
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AttachArgDialog::languageChange()
{
  setCaption( tr( "AttachArgDialog" ) );
#ifdef DEBUG_AAD
  printf("AttachArgDialog::languageChange() called.\n");
#endif
}

void AttachArgDialog::accept()
{

#ifdef DEBUG_AAD
  printf("AttachArgDialog::accept() called.\n");
#endif

  // We need to call the main dialog accept or else we hang....
  QDialog::accept();

#ifdef DEBUG_AAD
  printf("AttachArgDialog::accept() exits.\n");
#endif
}
