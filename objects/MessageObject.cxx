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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"
#include "MessageObject.hxx"

/*! \class MessageObject
  This is the base class for all messages being passed around to panels.
  It contains a simple QString type that is used by the ::listener() routine
  to determine if it's a message type it cares to field.
*/
MessageObject::MessageObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("MessageObject::MessageObject(entered)\n");
  msgType = "MessageObject";
}

MessageObject::MessageObject(QString msg_type)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("MessageObject::MessageObject(%s)\n", msg_type.ascii() );

  msgType = msg_type;
}

MessageObject::~MessageObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("MessageObject::~MessageObject(%s)\n", msgType.ascii() );
}

void
MessageObject::print()
{
  printf("MessageObject:\n");
  printf("	msgType=(%s)\n", msgType.ascii() );
}
