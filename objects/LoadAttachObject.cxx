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
LoadAttachObject::LoadAttachObject(char *executable_name, char *pid_string) : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");
  if( executable_name )
  {
    executableName = strdup(executable_name);
  } else
  {
    executableName = NULL;
  }
  if( pid_string )
  {
    pidStr = strdup(pid_string);
  } else
  {
    pidStr = NULL;
  }
}

/*! Destructor.   Releases the functionName and fileName. */
LoadAttachObject::~LoadAttachObject()
{
  if( executableName )
  {
    free( executableName );
  }
  if( pidStr )
  {
    free( pidStr );
  }
}

/*! Prints the objects fields.    Debug only. */
void
LoadAttachObject::print()
{
  printf("LoadAttachObject:\n");
  printf("	executableName=(%s)\n", executableName);
  printf("	pidStr=(%s)\n", pidStr);
}
