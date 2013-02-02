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


#ifndef MPLISTVIEW_H
#define MPLISTVIEW_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "qlistview.h"
#include <qwidget.h>

#include "MPListViewItem.hxx"

struct StatusStruct
{
  QString status;
  QString host;
  QString pid;
  QString tid;
  QString rid;
};


class MPListView : public QListView
{
  Q_OBJECT
public:
  MPListView( QWidget *parent, const char *name, WFlags f );

  ~MPListView( );

  void contentsDragEnterEvent( QDragEnterEvent *e );
  void contentsDragMoveEvent( QDragMoveEvent *e );
  void contentsDragLeaveEvent( QDragLeaveEvent *e );
  void contentsDropEvent( QDropEvent *e );
  void contentsMouseMoveEvent( QMouseEvent *e );
  void contentsMousePressEvent( QMouseEvent *e );
  void contentsMouseReleaseEvent( QMouseEvent *e );

  void dragEnterEvent( QDragEnterEvent* event );

  QPoint presspos;
  bool mousePressed;

  QListView::SelectionMode savedSelectionMode;

  static MPListViewItem *oldCurrent;
  static bool draggingFLAG;
  QListViewItem *dropItem;
  bool isThisADuplicate(MPListViewItem *item);
  virtual void sort();


public slots:

private:

};

#endif // MPLISTVIEW_H
