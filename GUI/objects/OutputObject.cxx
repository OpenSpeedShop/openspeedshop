#include "OutputObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

OutputObject::OutputObject()
{
//  dprintf("OutputObject::OutputObject(entered.\n");
}

OutputObject::OutputObject(char *output_buffer)
{
//  dprintf("OutputObject::OutputObject(entered.\n");
  outputBuffer = strdup(output_buffer);
}

OutputObject::~OutputObject()
{
  if( outputBuffer )
  {
    free(outputBuffer);
  }
}

void
OutputObject::print()
{
  printf("OutputObject:\n");
  printf("	outputBuffer=(%s)\n", outputBuffer);
}
