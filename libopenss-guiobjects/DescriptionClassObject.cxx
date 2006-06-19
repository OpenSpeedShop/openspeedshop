////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
  cout << "all=" << all << endl;
  cout << "root=" << root << endl;
  if( !pset_name.isEmpty() )
  {
    cout << "pset_name=" << pset_name << endl;
  }
  if( !host_name.isEmpty() )
  {
    cout << "host_name=" << host_name << endl;
  }
  if( !pid_name.isEmpty() )
  {
    cout << "pid_name=" << pid_name << endl;
  }
  if( !rid_name.isEmpty() )
  {
    cout << "rid_name=" << rid_name << endl;
  }
  if( !tid_name.isEmpty() )
  {
    cout << "tid_name=" << rid_name << endl;
  }
  if( !collector_name.isEmpty() )
  {
    cout << "collector_name=" << collector_name << endl;
  }
}
