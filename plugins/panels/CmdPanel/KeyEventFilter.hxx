////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 The Krell Institute All Rights Reserved.
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


#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include <stddef.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qevent.h>
#include <qtimer.h>
#include <qpoint.h>

#include "CmdPanel.hxx"
class CmdPanel;

class KeyEventFilter : public QObject
{
  public:
    //! The working constructor for KeyEventFilter(...)
    /*! This constructor is the work constructor for KeyEventFilter.
     */
    KeyEventFilter(QObject *t, CmdPanel *cp);
    //! Default destructor
    /*! Nothing extra is allocatated, nothing extra is destroyed.
     */
    ~KeyEventFilter() {};
  private:
    bool eventFilter( QObject *o, QEvent *e );
    CmdPanel *cmdPanel;
public slots:
};
#endif // KEYEVENTFILTER_H
