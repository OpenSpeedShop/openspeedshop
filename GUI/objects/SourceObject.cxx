#include "SourceObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HighlightObject.hxx"

#include "debug.hxx"

SourceObject::SourceObject() : MessageObject("SourceObject")
{
  dprintf("SourceObject::SourceObject(entered.\n");
}

SourceObject::SourceObject(char *_functionName, char *_fileName, int l, bool rF, HighlightList *hll) : MessageObject("SourceObject")
{
  dprintf("SourceObject::SourceObject(entered.\n");
  functionName = strdup(_functionName);
  fileName = strdup(_fileName);
  line_number = l;
  raiseFLAG = rF;
  highlightList = hll;
}

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
