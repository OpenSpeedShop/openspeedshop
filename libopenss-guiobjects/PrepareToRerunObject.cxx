////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 The Krell Institute. All Rights Reserved.
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


/*! \class PrepareToRerunObject
 */
#include "PrepareToRerunObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

/*! Constructor for the PrepareToRerunObject.
 */
PrepareToRerunObject::PrepareToRerunObject(void *expr, int id, QString ename, bool rF, bool forceUpd) : MessageObject("PrepareToRerun")
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("PrepareToRerunObject::PrepareToRerunObject(entered.\n");
#if 0
  printf("PrepareToRerunObject::PrepareToRerunObject(entered), expr=0x%x\n", expr);
  printf("PrepareToRerunObject::PrepareToRerunObject(entered), id=%d\n", id);
  printf("PrepareToRerunObject::PrepareToRerunObject(entered), ename.ascii=%s\n", ename.ascii());
  printf("PrepareToRerunObject::PrepareToRerunObject(entered), rF=%d\n", rF);
  printf("PrepareToRerunObject::PrepareToRerunObject(entered), forceUpd=%d\n", forceUpd);
#endif
  fw_expr = expr;
  expID = id;
  experiment_name = ename;
  raiseFLAG = rF;
  forceFLAG = forceUpd;
}

/*! Destructor */
PrepareToRerunObject::~PrepareToRerunObject()
{
}

/*! Prints the objects fields.    Debug only. */
void
PrepareToRerunObject::print()
{
  printf("PrepareToRerunObject:\n");
  printf("	fw_expr=(0x%x)\n", fw_expr );
  printf("	expID=(%d)\n", expID );
  printf("	experiment_name=(%s)\n", experiment_name.ascii());
  printf("	raiseFLAG=(%d)\n", raiseFLAG);
  printf("	forceFLAG=(%d)\n", forceFLAG);
}
