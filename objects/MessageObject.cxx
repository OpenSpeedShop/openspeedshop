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
  nprintf(DEBUG_MESSAGES) ("MessageObject::MessageObject(entered)\n");
  msgType = "MessageObject";
}

MessageObject::MessageObject(QString msg_type)
{
  nprintf(DEBUG_MESSAGES) ("MessageObject::MessageObject(%s)\n", msg_type.ascii() );

  msgType = msg_type;
}

MessageObject::~MessageObject()
{
  nprintf(DEBUG_MESSAGES) ("MessageObject::~MessageObject(%s)\n", msgType.ascii() );
}

void
MessageObject::print()
{
  printf("MessageObject:\n");
  printf("	msgType=(%s)\n", msgType);
}
