////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"
#include "ArgumentObject.hxx"

/*! \class ArgumentObject
  This class passes around arguments for panels creation.
*/
ArgumentObject::ArgumentObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("ArgumentObject::ArgumentObject(entered)\n");
  msgType = "ArgumentObject";
  init();
}

void
ArgumentObject::init()
{
  int_data = 0;
  panel_data = NULL;
  qstring_data = QString::null;
  loadedFromSavedFile = FALSE;
  lao = NULL;
  isInstrumentorOffline=FALSE;
}

ArgumentObject::ArgumentObject(QString msg_type, int d)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("ArgumentObject::ArgumentObject(%s)\n", msg_type.ascii() );

  msgType = msg_type;
  init();
  int_data = d;
  qstring_data = QString::null;
}

ArgumentObject::ArgumentObject(QString msg_type, QString d)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("ArgumentObject::ArgumentObject(%s)\n", msg_type.ascii() );

  msgType = msg_type;
  init();
  qstring_data = d;
}

ArgumentObject::ArgumentObject(QString msg_type, Panel *d)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("ArgumentObject::ArgumentObject(%s)\n", msg_type.ascii() );

  msgType = msg_type;
  init();
  panel_data = d;
  qstring_data = QString::null;
}

ArgumentObject::~ArgumentObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("ArgumentObject::~ArgumentObject(%s)\n", msgType.ascii() );
}

void
ArgumentObject::print()
{
  printf("ArgumentObject:\n");
  printf("	msgType=(%s)\n", msgType.ascii() );
  printf("  panel_data=0x%x\n", panel_data);
  printf("  int_data=%d\n", int_data);
  printf("  qstring_data=%s\n", qstring_data.ascii());
  printf("  loadedFromSavedFile=%d\n", loadedFromSavedFile);
  printf("  lao:\n");
  if( lao )
  {
    lao->print();
  }
  printf("  isInstrumentorOffline=%d:\n", isInstrumentorOffline);
}
