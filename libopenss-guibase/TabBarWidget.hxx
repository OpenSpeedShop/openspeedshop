////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


#ifndef TABBARWIDGET_H
#define TABBARWIDGET_H

#include <cstddef>
#include <qwidget.h>
#include <qtabbar.h>

class PanelContainer;

//! Manages the tab bar events.
class TabBarWidget : public QTabBar
{
    Q_OBJECT
  public:
    //! default constructor.   Here for completeness only. (unused)
    TabBarWidget( );

    //! This is the work constructor.
    TabBarWidget( PanelContainer *pc, QWidget *parent=0, const char *name=0 );

    //! The default destructor.
    ~TabBarWidget( );

    //! Function to return the panelContainer pointer.
    PanelContainer *getPanelContainer() { return _panelContainer; };

    //! Function to set the panelContainer pointer.
    void setPanelContainer(PanelContainer *pc)  { _panelContainer = pc; }
  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

    virtual void dragIt();

  private:
    //! Fields the mousePressEvent for drag and drop.
    void mousePressEvent(QMouseEvent *e=NULL);

    //! Fields the mouseReleaseEvent for drag and drop.
    void mouseReleaseEvent(QMouseEvent *e=NULL);
    bool mouseDown;
    QPoint downPos;

    //! Fields the mousePressEvent for drag and drop.
    void mouseDoubleClickEvent(QMouseEvent *e=NULL);

    //! A pointer to the parent panelContainer.
    PanelContainer *_panelContainer;
};
#endif // TABBARWIDGET_H
