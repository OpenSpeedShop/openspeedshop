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


/*! \class UpdateObject
 */
#include "UpdateObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Constructor for the UpdateObject.
 */
UpdateObject::UpdateObject(void *expr, int id, QString ename, bool rF) : MessageObject("UpdateExperimentDataObject")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("UpdateObject::UpdateObject(entered.\n");
  fw_expr = expr;
  expID = id;
  experiment_name = ename;
  raiseFLAG = rF;
}

/*! Destructor */
UpdateObject::~UpdateObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
UpdateObject::print()
{
  printf("UpdateObject:\n");
  printf("	fw+expr=(0x%x)\n", fw_expr );
  printf("	expID=(%d)\n", expID );
  printf("	experiment_name=(%s)\n", experiment_name.ascii());
  printf("	raiseFLAG=(%d)\n", raiseFLAG);
}
