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


#ifndef SPLISTVIEWITEM_H
#define SPLISTVIEWITEM_H

#include "qlistview.h"
#include <qpopupmenu.h>

#include <qwidget.h>

class StatsPanelBase;

class SPListViewItem : public QListViewItem
{
public:
  SPListViewItem( StatsPanelBase *sp, QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
  SPListViewItem( StatsPanelBase *sp, SPListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  ~SPListViewItem( );

  QString key(int c, bool b) const;
 
#ifdef HOLD
  int compare( QListViewItem *i, int col, bool ascending ) const;
#endif // HOLD

   StatsPanelBase *statsPanel;

public slots:

private:
};
#endif // SPLISTVIEWITEM_H
