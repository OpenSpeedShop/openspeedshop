/*! \class SourceObject
    The is the message object passed around that will cause the SourcePanel
    to load, position, and highlight source. 

    Currently this is only implemented between TopPanel and SourcePanel.
 */
#include "SourceObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HighlightObject.hxx"

#include "debug.hxx"

/*! Unused constructor. */
SourceObject::SourceObject() : MessageObject("SourceObject")
{
  dprintf("SourceObject::SourceObject(entered.\n");
}

/*! Constructor for the SourceObject.   Initializes the filename to load, 
    the line to center in the SourcePanel, flags if the SourcePanel should
    be raised, and passes a list of lines to highlight. */
SourceObject::SourceObject(char *_functionName, char *_fileName, int l, bool rF, HighlightList *hll) : MessageObject("SourceObject")
{
  dprintf("SourceObject::SourceObject(entered.\n");
  functionName = strdup(_functionName);
  fileName = strdup(_fileName);
  line_number = l;
  raiseFLAG = rF;
  highlightList = hll;
}

/*! Destructor.   Releases the functionName and fileName. */
SourceObject::~SourceObject()
{
  if( functionName )
  {
    free( functionName );
  }
  if( fileName )
  {
    free( fileName );
  }
}

/*! Prints the objects fields.    Debug only. */
void
SourceObject::print()
{
  printf("SourceObject:\n");
  printf("	functionName=(%s)\n", functionName);
  printf("	fileName=(%s)\n", fileName);
  printf("	line_number=(%d)\n", line_number);
  printf("	raiseFLAG=(%d)\n", raiseFLAG);
  printf("	highlightList=(0x%x)\n", highlightList);
}
