////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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
#include <cstddef>
#include "LoadAttachObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.hxx"

//#define DEBUG_LAO 1

/*! Unused constructor. */
LoadAttachObject::LoadAttachObject() : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");
}

/*! Constructor for the LoadAttachObject.   Initializes the filename to load.
    or the pid to attach to. */
LoadAttachObject::LoadAttachObject(QString executable_name, 
                                   QString pid_string, 
                                   QString parallelPrefix, 
                                   ParamList *param_list, 
                                   bool lnh, 
                                   bool isOff,
                                   compareByType localCompareByType) : MessageObject("LoadAttachObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LoadAttachObject::LoadAttachObject(entered.\n");

#ifdef DEBUG_LAO

  if (parallelPrefix) {
    printf( "LoadAttachObject::LoadAttachObject(entered). parallelPrefix.ascii()=%s\n", parallelPrefix.ascii());
  } else {
    printf( "LoadAttachObject::LoadAttachObject(entered). parallelPrefix is NULL\n");
  }

  if (executable_name) {
    printf( "LoadAttachObject::LoadAttachObject(entered). executable_name.ascii()=%s\n", executable_name.ascii());
  } else {
    printf( "LoadAttachObject::LoadAttachObject(entered). executable_name is NULL\n");
  }

  if (pid_string) {
    printf( "LoadAttachObject::LoadAttachObject(entered). pid_string.ascii()=%s\n", pid_string.ascii());
  } else {
    printf( "LoadAttachObject::LoadAttachObject(entered). pid_string is NULL\n");
  }

    printf( "LoadAttachObject::LoadAttachObject(entered). isOff=%d\n", isOff);

    printf( "LoadAttachObject::LoadAttachObject(entered). localCompareByType=%d\n", localCompareByType);
#endif

  executableName = executable_name;
  pidStr = pid_string;
  parallelprefixstring = parallelPrefix;
  paramList = param_list;
  loadNowHint = lnh;
  doesThisExperimentUseOfflineInstrumentation = isOff;
  compareByThisType = localCompareByType;

#ifdef DEBUG_LAO
  // debug for now
  print();
#endif

}

/*! Destructor.   Releases the functionName and fileName. */
LoadAttachObject::~LoadAttachObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
LoadAttachObject::print()
{

#ifdef DEBUG_LAO

  printf("LoadAttachObject::print() entered\n");
  if (executableName)
    printf("	executableName=(%s)\n", executableName.ascii());
  if (parallelprefixstring)
    printf("	parallelprefixstring=(%s)\n", parallelprefixstring.ascii());
  if (pidStr)
    printf("	pidStr=(%s)\n", pidStr.ascii());
  if (paramList) {
    printf("	paramList contains: \n");
    for( ParamList::Iterator pit = paramList->begin();
              pit != paramList->end(); ++pit )
    {
      QString paramStr = (QString)*pit;
      if (paramStr) {
        printf("  paramStr=(%s)\n", paramStr.ascii() );
      }
    }
  } // end paramList if
  printf("	loadNowHint=(%d)\n", loadNowHint);
  printf("	doesThisExperimentUseOfflineInstrumentation=(%d)\n", doesThisExperimentUseOfflineInstrumentation);
  printf("      compareByThisType=(%d)\n", compareByThisType); 

  printf("LoadAttachObject::print() exitted\n");

#endif
}
