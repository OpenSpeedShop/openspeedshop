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


#ifndef SPTEXTEDIT_H
#define SPTEXTEDIT_H

class StatsPanel;

#include <stddef.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>

//! Simply overloads the QTextEdit so we can detect the context menu event.
class SPTextEdit : public QTextEdit
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPTextEdit( );

    //! SPTextEdit(StatsPanel *sp, QWidget *parent, const char *name)
    SPTextEdit( StatsPanel *sp, QWidget *parent=0, const char *name=0 );

    virtual ~SPTextEdit( );

    QPopupMenu* createPopupMenu( const QPoint & pos );
    QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

    void annotateScrollBarLine( int line, QColor);
    void clearScrollBarLine(int line);
    void clearScrollBar();

    QScrollBar *vbar;
    QScrollBar *hbar;
protected: 
public slots:

private:
    StatsPanel *statsPanel;

    QPixmap *vannotatePixmap;
};
#endif // SPTEXTEDIT_H
