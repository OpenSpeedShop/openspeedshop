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
  nprintf( DEBUG_CONST_DESTRUCT ) ("SourceObject::SourceObject(entered.\n");
}

/*! Constructor for the SourceObject.   Initializes the filename to load, 
    the line to center in the SourcePanel, flags if the SourcePanel should
    be raised, and passes a list of lines to highlight. */
SourceObject::SourceObject(QString _functionName, QString _fileName, int l, bool rF, HighlightList *hll) : MessageObject("SourceObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("SourceObject::SourceObject(entered.\n");
  functionName = _functionName;
  fileName = _fileName;
  line_number = l;
  raiseFLAG = rF;
  highlightList = hll;
}

/*! Destructor.   Releases the functionName and fileName. */
SourceObject::~SourceObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
SourceObject::print()
{
  printf("SourceObject:\n");
  printf("	functionName=(%s)\n", functionName.ascii());
  printf("	fileName=(%s)\n", fileName.ascii());
  printf("	line_number=(%d)\n", line_number);
  printf("	raiseFLAG=(%d)\n", raiseFLAG);
  printf("	highlightList=(0x%x)\n", highlightList);
}
