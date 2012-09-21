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


#ifndef FRAME_H
#define FRAME_H

#include <cstddef>
#include "debug.hxx"

class PanelContainer;

#include "debug.hxx"
#include <qwidget.h>
#include <qframe.h>


//! This is the container for a PanelContainer.
class Frame : public QFrame
{
    //! Q_OBJECT is needed as there are slots defined for the Frame Class
    Q_OBJECT
public:
    //! Frame() - The default constructor the the Frame Class (unused).
    Frame( );

    //! Frame(PanelContainer *pc, QWidget *parent, const char *name)
    Frame( PanelContainer *pc, QWidget *parent=0, const char *name=0 );

    //! ~Frame() - The default destructor.
    ~Frame( );

    //! Convienience member function to return the name of this Frame.
    PanelContainer *getPanelContainer() { return _panelContainer; }
    PanelContainer *setPanelContainer(PanelContainer *pc) { _panelContainer = pc; }
    //! Convienience member function to return the this flag setting.
    bool isDragEnabled() { return _dragEnabled; }
    void setDragEnabled(bool setting) { _dragEnabled = setting; }

    //! Convienience member function to return the this flag setting.
    bool isDropEnabled() { return _dropEnabled; }
    void setDropEnabled(bool setting) { _dropEnabled = setting; }

    //! Convienience member function to return the this flag setting.

    const char *getName() { return name; }

    //! A static flag indicating that a drag is taking place.
    static bool dragging;

    //! The event callback for the menu to be create.
    void contextMenuEvent ( QContextMenuEvent *, bool localMenu=FALSE );
  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

  private:
    //! The event callback for the menu to be create.
    void contextMenuEvent ( QContextMenuEvent * );

    //! The event callback that a resize needs to be handle.
    void resizeEvent ( QResizeEvent * );

    //! Set to TRUE if this Frame is able to be dragged.
    bool _dragEnabled;

    //! Set to TRUE if this Frame is able to accept drops.
    bool _dropEnabled;

#ifdef  OLDWAY  // Left here for debugging...
    void enterEvent ( QEvent * );
    void leaveEvent ( QEvent * );
#endif // OLDWAY

#ifdef OLD_DRAG_AND_DROP
    //! The event callback that triggers the drag event.
    void mousePressEvent(QMouseEvent *e=NULL);

    //! The event callback that triggers the drop event.
    void mouseReleaseEvent(QMouseEvent *e=NULL);
#endif // OLD_DRAG_AND_DROP
#ifndef OLD_DRAG_AND_DROP
    //! The event called when Qt Drag-and-Drop enter event is fired. 
    void dragEnterEvent(QDragEnterEvent* event);

    //! The event called when Qt Drag-and-Drop drop event is fired. 
    void dropEvent(QDropEvent* event);
#endif // OLD_DRAG_AND_DROP

    //! The parent PanelContainer of this Frame.
    PanelContainer *_panelContainer;


    //! The name of this Frame.
    char *name;
};
#endif // FRAME_H
