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
//
//#define MPList_DEBUG 1
//
#include "MPListViewItem.hxx"
#include "ManageCollectorsClass.hxx"
#include "debug.hxx"

  QListViewItem * QLVI_parent;
  MPListViewItem * MPLVI_parent;
  QListView * QLV_parent;



MPListViewItem::MPListViewItem( QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
#if MPList_DEBUG
  printf("MPListViewItem::MPListViewItem, 1\n");
#endif
  descriptionClassObject = NULL;
  QLV_parent = parent;
  MPLVI_parent = NULL;
  QLVI_parent = NULL;
}

MPListViewItem::MPListViewItem( MPListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (MPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
#if MPList_DEBUG
  printf("MPListViewItem::MPListViewItem, 2\n");
#endif
  descriptionClassObject = NULL;
  QLV_parent = NULL;
  MPLVI_parent = parent;
  QLVI_parent = NULL;
}

MPListViewItem::MPListViewItem( QListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (MPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
#if MPList_DEBUG
  printf("MPListViewItem::MPListViewItem, 3\n");
#endif
  descriptionClassObject = NULL;
  QLV_parent = NULL;
  MPLVI_parent = NULL;
  QLVI_parent = parent;
}

/*! The default destructor. */
MPListViewItem::~MPListViewItem( )
{
#if MPList_DEBUG
  printf("MPListViewItem::~MPListViewItem, 1\n");
#endif
}

// Don't really need this override

QString MPListViewItem::key( int col, bool ascending ) const
{
#if MPList_DEBUG
   printf("MPListViewItem::key, col=%d, ascending=%d\n", col, ascending);
#endif
   QListViewItem::key(col, ascending);
}

// Override the default QListViewItem::compare routine so we can handle 
// numeric sorts.
// 
// We have to look for the headers of the parent of the MPListViewItem 
// to determine whether to do a numeric or alphanumeric sort.
// This takes a bit of chicanery.  We save the parent of each MPListViewItem
// when they are built and use this value/ptr to get at the column header text
// for each column.  We use that to determine how to sort.   
//
// If we are going to sort alphanumerically we use the default routine provided
// by QT.
// jeg 5-8-2007
//

int MPListViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
    bool isAlphaNumeric = FALSE;
    bool isNumeric = FALSE;


    isAlphaNumeric = FALSE;
    isNumeric = FALSE;

    if (QLV_parent != NULL) {
      if (! QLV_parent->columnText(col).isEmpty()) {
         if (QLV_parent->columnText(col).contains("Hosts:") ) {
           isAlphaNumeric = TRUE;
         } else if ((QLV_parent->columnText(col).contains("Rank")) ||
                    (QLV_parent->columnText(col).contains("Processes:") ) ) {
           isNumeric = TRUE;
         } else {
           // default to alpha numeric values as the expected data
           isAlphaNumeric = TRUE;
         }
      }
    } else if (i->parent() ) {

      if ( i->parent()->listView() && (!i->parent()->listView()->columnText(col).isEmpty() ) ) {
         if (i->parent()->listView()->columnText(col).contains("Process Sets") ) {
           isAlphaNumeric = TRUE;
         } else if ((i->parent()->listView()->columnText(col).contains("Rank")) ||
                    (i->parent()->listView()->columnText(col).contains("Thread") ) ||
                    (i->parent()->listView()->columnText(col).contains("PID") ) ) {
           isNumeric = TRUE;
         } else {
           // default to alpha numeric values as the expected data
           isAlphaNumeric = TRUE;
         }
      } else if (i->parent()->parent() ) { 
        if (i->parent()->parent()->text(col) && !i->parent()->parent()->text(col).isEmpty()) { 
         if (i->parent()->parent()->text(col).contains("Hosts:") ) {
           isAlphaNumeric = TRUE;
         } else if (i->parent()->parent()->text(col).contains("Rank") ||
                    i->parent()->parent()->text(col).contains("Processes:" )) {
           isNumeric = TRUE;
         } else {
           // default to alpha numeric values as the expected data
           isAlphaNumeric = TRUE;
         }
        }
      } else if (!i->parent()->text(col).isEmpty()) { 
         if (i->parent()->text(col).contains("Hosts:") ) {
           isAlphaNumeric = TRUE;
         } else if (i->parent()->text(col).contains("Rank") ||
                    i->parent()->text(col).contains("Processes:") ) {
           isNumeric = TRUE;
         } else {
           // default to alpha numeric values as the expected data
           isAlphaNumeric = TRUE;
         }
      } else {
         // no matches
      } 

    }
    
    if (isNumeric ) {
        int val;
        val = text(col).toInt() - i->text(col).toInt();

        if (val > 0) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return QString::localeAwareCompare( text( col ), i->text( col ) );
    }
}
