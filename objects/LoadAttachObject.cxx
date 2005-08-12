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
LoadAttachObject::LoadAttachObject(QString executable_name, QString pid_string, ParamList *param_list, bool lnh) : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");
  executableName = executable_name;
  pidStr = pid_string;
  paramList = param_list;
  loadNowHint = lnh;
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
  for( ParamList::Iterator pit = paramList->begin();
            pit != paramList->end(); ++pit )
  {
    QString paramStr = (QString)*pit;
    printf("  paramStr=(%s)\n", paramStr.ascii() );
  }
}
