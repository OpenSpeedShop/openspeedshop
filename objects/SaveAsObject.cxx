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
