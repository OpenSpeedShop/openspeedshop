////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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


#ifndef SPLISTVIEWITEM_H
#define SPLISTVIEWITEM_H

#include <stddef.h>
#include "qlistview.h"
#include <qpopupmenu.h>
#include <qvaluevector.h>

#include <qwidget.h>

class StatsPanel;

class SPListViewItem : public QListViewItem
{
public:
  SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, QListView * parent, SPListViewItem *after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
  SPListViewItem( StatsPanel *sp, QString _funcName, QString _fileName, int _lineNumber, SPListViewItem * parent, SPListViewItem *after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  ~SPListViewItem( );

  int compare(QListViewItem *other, int c, bool b) const;


   StatsPanel *statsPanel;

    void paintCell(QPainter *painter, const QColorGroup &cg,
		       int column, int width, int align);
    int width(const QFontMetrics &fm, const QListView *lv, int column) const;
    
    QFont font(uint column) const;
    void setFont(uint column, const QFont &font);
    QColor background(uint column) const;
    void setBackground(uint column, const QColor &color);

    QString funcName;
    QString fileName;
    int lineNumber;
    
  private:
    QValueVector<QFont> fonts;
    QValueVector<QColor> backgrounds;

public slots:
};
#endif // SPLISTVIEWITEM_H
