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


#ifndef MPLISTVIEWITEM_H
#define MPLISTVIEWITEM_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "qlistview.h"
#include <qwidget.h>

#define CPS "Compare Process Set"
#define DPS "Dynamic Process Set"
#define UDPS "User Defined Process Set"

class DescriptionClassObject
{
  public:
    DescriptionClassObject();
    DescriptionClassObject(bool r, QString p, QString h=QString::null, QString pn=QString::null, QString rn=QString::null, QString c=QString::null)
    {
      root = r;
      pset_name = p;
      host_name = h;
      pid_name = pn;
      rid_name = rn;
      collector_name = c;
      all = FALSE;
    }
    ~DescriptionClassObject()
    {
    };

    bool all;  // Is this for all processes.  This is a special case.
    bool root;
    QString pset_name;
    QString host_name;
    QString pid_name;
    QString rid_name;
    QString collector_name;

    void Print()
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
      if( !collector_name.isEmpty() )
      {
        cout << "collector_name=" << collector_name << endl;
      }
    };
};

class MPListViewItem : public QListViewItem
{
public:
  MPListViewItem( QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  MPListViewItem( MPListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  MPListViewItem( QListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  ~MPListViewItem( );

  DescriptionClassObject *descriptionClassObject;

public slots:

private:
};

#endif // MPLISTVIEWITEM_H
