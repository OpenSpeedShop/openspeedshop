#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"
#include "MessageObject.hxx"

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
