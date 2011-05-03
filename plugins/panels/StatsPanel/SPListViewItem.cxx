////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
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
//#define DEBUG_StatsPanel 1
//

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
SPListViewItem::SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, QListView * parent, SPListViewItem *after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, after, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  funcName = _funcName;
  fileName = _fileName;
  lineNumber = _lineNumber;
  statsPanel = sp;
#if DEBUG_StatsPanel 
  printf("SPListViewItem::SPListViewItem1, fileName=(%s), funcName=(%s), lineNumber=%d\n", 
          fileName.ascii(), funcName.ascii(), lineNumber );
#endif
}

SPListViewItem::SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, SPListViewItem * parent, SPListViewItem *after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (SPListViewItem *)parent, (SPListViewItem *)after, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  funcName = _funcName;
  fileName = _fileName;
  lineNumber = _lineNumber;
  statsPanel = sp;
#if DEBUG_StatsPanel 
  printf("SPListViewItem::SPListViewItem2, fileName=(%s), funcName=(%s), lineNumber=%d\n", 
          fileName.ascii(), funcName.ascii(), lineNumber );
#endif
}

/*! The default destructor. */
SPListViewItem::~SPListViewItem( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPListViewItem::~SPListViewItem( ) destructor called\n");
}

#include <qpainter.h>
// This code shameless taken from:
// http://doc.trolltech.com/qq/qq08-fancy-list-view.html#fancylistviewitems
QFont
SPListViewItem::font(uint column) const
{
        if (column < fonts.size())
            return fonts[column];
        return listView()->font();
}

void
SPListViewItem::setFont(uint column, const QFont &font)
{
        if (column >= fonts.size())
            fonts.resize(column + 1, listView()->font());
        fonts[column] = font;
}

QColor
SPListViewItem::background(uint column) const
{
        if (column < backgrounds.size())
            return backgrounds[column];
        return listView()->colorGroup().base();
}

void
SPListViewItem::setBackground(uint column, const QColor &color)
{
        if (column >= backgrounds.size())
            backgrounds.resize(column + 1, listView()->colorGroup().base());
        backgrounds[column] = color;
}

void
SPListViewItem::paintCell(
        QPainter *painter, const QColorGroup &cg,
        int column, int width, int align)
{
        painter->save();
        if (column >= 0 && column < (int)fonts.size())
            painter->setFont(fonts[column]);
        QColorGroup grp(cg);
        if (column >= 0 && column < (int)backgrounds.size())
            grp.setColor(QColorGroup::Base, backgrounds[column]);
        QListViewItem::paintCell(painter, grp, column, width, align);
        painter->restore();
}

int
SPListViewItem::width(const QFontMetrics &fm, const QListView *lv, int column) const
{
        int width;
#if DEBUG_StatsPanel 
        printf("SPListViewItem::width, column=(%d), (int)fonts.size()=%d\n", column, (int)fonts.size() );
#endif
        if (column >= 0 && column < (int)fonts.size()) {
            QFontMetrics fm2(fonts[column]);
            width = QListViewItem::width(fm2, lv, column);
        }
        else {
            width = QListViewItem::width(fm, lv, column);
        }

#if DEBUG_StatsPanel 
  printf("SPListViewItem::width, return width=(%d)\n", width );
#endif
        return width;
}

int
SPListViewItem::compare(QListViewItem *other, int c, bool b) const
{

  bool ok1 = 0;
  bool ok2 = 0;

#if DEBUG_StatsPanel 
  printf("statsPanel->splv->sortColumn=(%d)\n", statsPanel->splv->sortColumn() );
#endif

  if( c != statsPanel->splv->sortColumn() ) {
    return 0;
  }

#if DEBUG_StatsPanel 
  printf("other->text(%lld)=%s text(%lld)=%s\n", c, other->text(c).ascii(), c, text(c).ascii() );
#endif
  
  QString s = text(c);
  QString other_s = other->text(c);
  double d = 0;
  double other_d = 0;

  d = text(c).toDouble(&ok1);
  other_d = other->text(c).toDouble(&ok2);
  uint64_t i = 0;
  uint64_t other_i = 0;
  
#if DEBUG_StatsPanel 
  printf("ok1=%d, ok2=%d\n", ok1, ok2 );
#endif

  if( ok1 && ok2 )

  {  // It's numeric!
#if DEBUG_StatsPanel 
  printf("it is numeric, ok1=%d, ok2=%d\n", ok1, ok2 );
#endif
    if( s.contains(".") || other_s.contains(".") ) {
    // Treat the compare as a doubles.
    
#if DEBUG_StatsPanel 
      printf("sort by double s: %s vs other: %s\n", s.ascii(), other_s.ascii() );
#endif

      /* sorting by double */
      d = s.toDouble();
      other_d = other_s.toDouble();

#if DEBUG_StatsPanel 
      printf("sort by double d: %f vs other: %f\n", d, other_d );
#endif
  
      if( d < other_d ) {
#if DEBUG_StatsPanel 
        printf("RETURN -1,     <   \n");
#endif
        return -1;

      } else if( d > other_d ) {
#if DEBUG_StatsPanel 
        printf("RETURN 1,     >   \n");
#endif
        return 1;

      } else {
#if DEBUG_StatsPanel 
        printf("RETURN 0,     ==   \n");
#endif
        return 0;
      }
    } else { // Threat the compare as integers.

      /* sorting by int */
#if DEBUG_StatsPanel 
      printf("sort by INT s: %lls vs other: %lls\n", s.ascii(), other_s.ascii());
#endif

      i = s.toLongLong();
      other_i = other_s.toLongLong();

#if DEBUG_StatsPanel 
      printf("sort by INT i: %lld vs other: %lld\n", i, other_i );
#endif
  
      if( i < other_i ) {

#if DEBUG_StatsPanel 
        printf("RETURN -1, sort by INT i: %lld less than other: %lld\n", i, other_i );
#endif
        return -1;

      } else if ( i > other_i ) {

#if DEBUG_StatsPanel 
        printf("RETURN 1, sort by INT i %lld greater than other: %lld\n", i, other_i );
#endif
        return 1;

      } else {

#if DEBUG_StatsPanel 
        printf("RETURN 0, sort by INT i: %lld equals other: %lld\n", i, other_i );
#endif

        return 0;
      }
    }
  } else {

#if DEBUG_StatsPanel 
    printf("sort by STRING, s: %s vs other: %s\n", s.ascii(), other_s.ascii());
#endif

    if( s < other_s ) {

#if DEBUG_StatsPanel 
    printf("RETURN -1, sort by STRING, s: %s less than other: %s\n", s.ascii(), other_s.ascii());
#endif

      return -1;
    } else if( s > other_s ) {
#if DEBUG_StatsPanel 
    printf("RETURN 1, sort by STRING, s: %s greater than other: %s\n", s.ascii(), other_s.ascii());
#endif
      return 1;
    } else {
#if DEBUG_StatsPanel 
    printf("RETURN 0, sort by STRING, s: %s equals other:%s\n", s.ascii(), other_s.ascii());
#endif
      return 0;
    }
  }
    
}
