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
SPListViewItem::SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, QListView * parent, SPListViewItem *after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, after, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  funcName = _funcName;
  fileName = _fileName;
  lineNumber = _lineNumber;
  statsPanel = sp;
}

SPListViewItem::SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, SPListViewItem * parent, SPListViewItem *after, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (SPListViewItem *)parent, (SPListViewItem *)after, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  funcName = _funcName;
  fileName = _fileName;
  lineNumber = _lineNumber;
  statsPanel = sp;
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
        if (column >= 0 && column < (int)fonts.size()) {
            QFontMetrics fm2(fonts[column]);
            width = QListViewItem::width(fm2, lv, column);
        }
        else
            width = QListViewItem::width(fm, lv, column);
        return width;
}

int
SPListViewItem::compare(QListViewItem *other, int c, bool b) const
{

  bool ok1 = 0;
  bool ok2 = 0;

// printf("statsPanel->splv->sortColumn=(%d)\n", statsPanel->splv->sortColumn() );

  if( c != statsPanel->splv->sortColumn() )
  {
    return 0;
  }



// printf("other->text(%d)=%s text(%d)=%s\n", c, other->text(c).ascii(), c, text(c).ascii() );
  
  QString s = text(c);
  QString other_s = other->text(c);
  double d = 0;
  double other_d = 0;

  bool absFLAG = statsPanel->absDiffFLAG;
 
  d = text(c).toDouble(&ok1);
  if( absFLAG )
  {
    d = fabs(d);
  }
  other_d = other->text(c).toDouble(&ok2);
  if( absFLAG )
  {
    other_d = fabs(other_d);
  }
  int i = 0;
  int other_i = 0;
  
  if( ok1 && ok2 )
  {  // It's numeric!
    if( s.contains(".") || other_s.contains(".") )
    { // Treat the compare as a doubles.
// printf("sort by double %s vs %s\n", s.ascii(), other_s.ascii() );
      /* sorting by double */
      d = s.toDouble();
      if( absFLAG )
      {
        d = fabs(d);
      }
      other_d = other_s.toDouble();
      if( absFLAG )
      {
        other_d = fabs(other_d);
      }
// printf("sort by double %f vs %f\n", d, other_d );
  
      if( d < other_d )
      {
// printf("     <   \n");
        return -1;
      } else if( d > other_d )
      {
// printf("     >   \n");
        return 1;
      } else 
      {
// printf("     ==   \n");
        return 0;
      }
    } else 
    { // Threat the compare as integers.
      /* sorting by int */
// printf("sort by int %s vs %s\n", s.ascii(), other_s.ascii());
      i = s.toInt();
      if( absFLAG )
      {
        i = abs(i);
      }
      other_i = other_s.toInt();
      if( absFLAG )
      {
        other_i = abs(other_i);
      }
  
      if( i < other_i )
      {
        return -1;
      } else if( i > other_i )
      {
        return 1;
      } else 
      {
        return 0;
      }
    }
  } else
  {
// printf("sort by other %s vs %s\n", s.ascii(), other_s.ascii());
    if( s < other_s )
    {
      return -1;
    } else if( s > other_s )
    {
      return 1;
    } else 
    {
      return 0;
    }
  }
    
}
