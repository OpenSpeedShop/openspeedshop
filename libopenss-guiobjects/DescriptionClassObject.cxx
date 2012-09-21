////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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


#include <cstddef>
#include "DescriptionClassObject.hxx"

DescriptionClassObject::DescriptionClassObject()
{
};

DescriptionClassObject::DescriptionClassObject(bool r, QString p, QString h, QString pn, QString rn, QString tn, QString c)
{
  root = r;
  pset_name = p;
  host_name = h;
  pid_name = pn;
  rid_name = rn;
  tid_name = tn;
  collector_name = c;
  all = FALSE;
}

DescriptionClassObject::~DescriptionClassObject()
{
};

void 
DescriptionClassObject::Print()
{
  printf("all=(%d)\n", all);
  printf("root=(%d)\n", root);
  if( !pset_name.isEmpty() )
  {
    printf("pset_name=(%s)\n", pset_name.ascii() );
  }
  if( !host_name.isEmpty() )
  {
    printf("host_name=(%s)\n", host_name.ascii() );
  }
  if( !pid_name.isEmpty() )
  {
    printf("pid_name=(%s)\n", pid_name.ascii() );
  }
  if( !rid_name.isEmpty() )
  {
    printf("rid_name=(%s)\n", rid_name.ascii() );
  }
  if( !tid_name.isEmpty() )
  {
    printf("tid_name=(%s)\n", tid_name.ascii() );
  }
  if( !collector_name.isEmpty() )
  {
    printf("collector_name=(%s)\n", collector_name.ascii() );
  }

  printf("End of dco\n");
}
