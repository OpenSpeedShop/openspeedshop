/*! \class ControlObject
    The is the message object passed to the owner Panel. 
    It is used to send the user's selected process control actions.

 */
#include "ControlObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Unused constructor. */
ControlObject::ControlObject() : MessageObject("ControlObject")
{
  dprintf("ControlObject::ControlObject(entered.\n");
  cot = NONE_T;
}

/*! Initialize a control object message with a message type. */
ControlObject::ControlObject(ControlObjectType _cot) : MessageObject("ControlObject")
{
  dprintf("ControlObject::ControlObject(entered.\n");
  cot = _cot;
}

/*! Destructor.   */
ControlObject::~ControlObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
ControlObject::print()
{
  printf("ControlObject:\n");

  printf("	control object type: %d\n", cot);
}
