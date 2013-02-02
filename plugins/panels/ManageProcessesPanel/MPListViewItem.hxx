////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "qlistview.h"
#include <qwidget.h>

#define CPS "Compare Process Set"
#define DPS "Dynamic Process Set"
#define UDPS "User Defined Process Set"

#include "DescriptionClassObject.hxx"

class MPListViewItem : public QListViewItem
{
public:
  MPListViewItem( QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  MPListViewItem( MPListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  MPListViewItem( QListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  ~MPListViewItem( );

  virtual QString key(int col, bool ascending_order) const;
  virtual int compare(QListViewItem *i, int col, bool ascending_order) const;

  DescriptionClassObject *descriptionClassObject;
  QListViewItem * QLVI_parent;
  MPListViewItem * MPLVI_parent;
  QListView * QLV_parent;

public slots:

private:
};

#endif // MPLISTVIEWITEM_H
