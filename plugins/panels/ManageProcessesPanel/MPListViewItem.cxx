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


#include "MPListViewItem.hxx"

#include "debug.hxx"

MPListViewItem::MPListViewItem( QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  descriptionClassObject = NULL;
}

MPListViewItem::MPListViewItem( MPListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (MPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  descriptionClassObject = NULL;
}

MPListViewItem::MPListViewItem( QListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (MPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  descriptionClassObject = NULL;
}



/*! The default destructor. */
MPListViewItem::~MPListViewItem( )
{
  // default destructor.
// printf ("  MPListViewItem::~MPListViewItem( ) destructor called\n");
  if( descriptionClassObject )
  {
    delete descriptionClassObject;
  }
}
