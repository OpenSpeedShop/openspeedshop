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


#ifndef DESCRIPTIONCLASSOBJECT_H
#define DESCRIPTIONCLASSOBJECT_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "qvaluelist.h"

class DescriptionClassObject
{
  public:
    DescriptionClassObject();
    DescriptionClassObject(bool r, QString p, QString h=QString::null, QString pn=QString::null, QString rn=QString::null, QString tn = QString::null, QString c=QString::null);
    ~DescriptionClassObject();

    bool all;  // Is this for all processes.  This is a special case.
    bool root;
    QString pset_name;
    QString host_name;
    QString pid_name;
    QString rid_name;
    QString tid_name;
    QString collector_name;

    void Print();
};
#endif // DESCRIPTIONCLASSOBJECT_H
