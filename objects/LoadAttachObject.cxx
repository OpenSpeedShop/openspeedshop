/*! \class LoadAttachObject
    The is the message object passed around that will cause the SourcePanel
    to load, position, and highlight source. 

    Currently this is only implemented between TopPanel and SourcePanel.
 */
#include "LoadAttachObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Unused constructor. */
LoadAttachObject::LoadAttachObject() : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");
}

/*! Constructor for the LoadAttachObject.   Initializes the filename to load.
    or the pid to attach to. */
LoadAttachObject::LoadAttachObject(QString executable_name, QString pid_string, QString param_list) : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");
  executableName = executable_name;
  pidStr = pid_string;
  paramList = param_list;
}

/*! Destructor.   Releases the functionName and fileName. */
LoadAttachObject::~LoadAttachObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
LoadAttachObject::print()
{
  printf("LoadAttachObject:\n");
  printf("	executableName=(%s)\n", executableName.ascii());
  printf("	pidStr=(%s)\n", pidStr.ascii());
  printf("	paramList=(%s)\n", paramList.ascii() );
}
