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


/*! \class SPListViewItem
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "SPListViewItem.hxx"

#include <qpopupmenu.h>
#include <qcursor.h>
#include "StatsPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPListViewItem::SPListViewItem( StatsPanel *sp, QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  statsPanel = sp;
}

SPListViewItem::SPListViewItem( StatsPanel *sp, SPListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (SPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  statsPanel = sp;
}

/*! The default destructor. */
SPListViewItem::~SPListViewItem( )
{
  // default destructor.
  dprintf ("  SPListViewItem::~SPListViewItem( ) destructor called\n");
}

// #ifdef GROWL
QString
SPListViewItem::key(int c, bool b) const
{
  dprintf("GROWL! c=%d\n", c);
  // Currently we only sort on column 0;
  if( c != 0 )
  {
    dprintf("You shouldn't be allowed to do this!\n");
    return QString::null;
  }

  QString s;
  if( statsPanel->metricHeaderTypeArray[c] == INT_T )
  {
    /* sorting by int */
    dprintf("sort by int\n");
    s.sprintf("%08d",text(c).toInt());
  } else if( statsPanel->metricHeaderTypeArray[c] == FLOAT_T )
  {
    dprintf("sort by float %07.2lf\n", text(c).toDouble() );
    /* sorting by float */
    s.sprintf("%07.2lf",text(c).toDouble());
  } else
  {
    /* sorting alphanumeric */
    dprintf("sort by alphanumeric\n");
    s.sprintf("%s",text(c).ascii());
  }
    
  return s;
}
// #endif // GROWL

#ifdef HOLD
int
SPListViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
  printf("GRAAAAOOOOWL! %d\n", col);

//  return key( col, ascending ).compare( i->key( col, ascending) );
}
#endif // HOLD
