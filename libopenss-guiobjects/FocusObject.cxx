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


/*! \class FocusObject
 */
#include "FocusObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Constructor for the FocusObject.
 */
FocusObject::FocusObject(int id, QString hn, QString pn, bool rf) : MessageObject("FocusObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("FocusObject::FocusObject(entered.\n");
  expID = id;
  host_name = hn;
  pidString = pn;
  raiseFLAG = rf;
  host_pid_vector.clear();
}

/*! Destructor */
FocusObject::~FocusObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
FocusObject::print()
{
  printf("FocusObject:\n");
  printf("	expID=(%d)\n", expID );
  printf("	host_name=(%s)\n", host_name.ascii());
  printf("	pidString=(%s)\n", pidString.ascii());
  printf("	raiseFLAG=(%d)\n", raiseFLAG);
}
