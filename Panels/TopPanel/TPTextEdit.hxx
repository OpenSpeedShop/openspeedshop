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


#ifndef TPTEXTEDIT_H
#define TPTEXTEDIT_H

class TopPanel;

#include <qwidget.h>
#include <qtextedit.h>

class TPTextEdit : public QTextEdit
{
public:
  TPTextEdit( );

  //! TPTextEdit(TopPanel *sp, QWidget *parent, const char *name)
  TPTextEdit( TopPanel *sp, QWidget *parent=0, const char *name=0 );

  ~TPTextEdit( );


  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

protected: 

public slots:

private:
  TopPanel *topFivePanel;

};
#endif // TPTEXTEDIT_H
