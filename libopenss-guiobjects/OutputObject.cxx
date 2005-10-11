////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


/*! \class OutputObject
  This routine simply contains the buffer returned from
  a command that was passed to the cli factory.
  
  The output should simple be displayed to the CmdPanel's
  output window.
*/
#include "OutputObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

OutputObject::OutputObject()
{
//  dprintf("OutputObject::OutputObject(entered.\n");
}

OutputObject::OutputObject(char *buffer)
{
//  dprintf("OutputObject::OutputObject(entered.\n");
  outputBuffer = strdup(buffer);
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
