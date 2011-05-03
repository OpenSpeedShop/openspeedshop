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


#ifndef MYCHARTFORM_H
#define MYCHARTFORM_H

class StatsPanel;

#include "chartform.hxx"

#include <qwidget.h>
#include <qtextedit.h>
#include <qsplitter.h>

//! Overrides the Chart implementation to allow context menu event.
class SPChartForm : public ChartForm
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
  SPChartForm( StatsPanel *sp, QSplitter *splitter, const char *name=0, int flags=0 );

  ~SPChartForm( );


  StatsPanel *statsPanel;

  int mouseClicked( int );
  void contentsContextMenuEvent( QContextMenuEvent *e );
  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };
  QPopupMenu *popupMenu;
  QPopupMenu *panelMenu;

public slots:
   void goToLine();

private:
};
#endif // MYCHARTFORM_H
