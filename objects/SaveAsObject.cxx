/*! \class SaveAsObject
    The is the message object passed around that will cause the SourcePanel
    to load, position, and highlight source. 

    Currently this is only implemented between TopPanel and SourcePanel.
 */
#include "SaveAsObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Constructor for the SaveAsObject.   Initializes the filename to load.
    or the pid to attach to. */
SaveAsObject::SaveAsObject(QString fn) : MessageObject("SaveAsObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("SaveAsObject::SaveAsObject(entered.\n");

  fileName = fn;

  ts = NULL;

  if( !fileName.isEmpty() )
  {
    if( f != NULL )
    {
      f = new QFile( fileName );
      if( !f->open( IO_WriteOnly ) )
      {
        f->close();
fprintf(stderr, "Unable to open (%s) for writing.\n", fileName.ascii() );
        f = NULL;
        return;
      }
    }
    ts = new QTextStream(f);
  }
  return;
}

/*! Destructor.   Releases the functionName and fileName. */
SaveAsObject::~SaveAsObject()
{
  f->close();
}

/*! Prints the objects fields.    Debug only. */
void
SaveAsObject::print()
{
  printf("SaveAsObject:\n");
  printf("	fileName=(%s)\n", fileName.ascii());
}
