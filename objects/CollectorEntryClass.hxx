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


#ifndef COLLECTORENTRYCLASS_H
#define COLLECTORENTRYCLASS_H
class CollectorEntry
{
  public:
    CollectorEntry();
    CollectorEntry(QString n, QString sn, QString d, QString p, QString pv)
    {
      name = n;
      short_name = sn;
      description = d;
      param = p;
      param_val = pv;
    }
    ~CollectorEntry()
    {
    }
    QString name;
    QString short_name;
    QString description;
    QString param;  // (Really needs to and will be a list...)
    QString param_val;  // (Really needs to and will be a list...)
};
#endif // COLLECTORENTRYCLASS_H
