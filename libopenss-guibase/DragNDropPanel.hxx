////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute. All Rights Reserved.
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


#ifndef DRAGNDROPPANEL_H
#define DRAGNDROPPANEL_H

#include "debug.hxx"

class PanelContainer;
class Frame;

#include <cstddef>
#include <qwidget.h>
#include <qframe.h>
#include <qdragobject.h>

#include <qcursor.h>


//! Defines drag and drop functions.
class DragNDropPanel : public QStoredDrag
{
public:
    //! DragNDropPanel(PanelContainer *pc, Frame *sourceFrame)
    DragNDropPanel( const char *mimeType, PanelContainer *sourcePC, Frame *sourceFrame, char *name= (char *) "DragNDropPanel" );

    //! ~Panel() - The default destructor.
    ~DragNDropPanel( );

    //! This is an internal handle to the frame that initiated the drag.
    Frame *frame;

    //! This is an internal handle to the PanelContainer initiating the drag.
    PanelContainer *panelContainer;

    //! The drop panel callback routine when Qt drag-n-drop fails.
    void DropPanel(PanelContainer *sourcePC, bool doubleClickedFLAG=FALSE);

#ifndef OLD_DRAG_AND_DROP
    //! The drop panel callback routine called when Qt drag-n-drop succeeds
    void DropPanelWithQtDnD(PanelContainer *targetPC);
#endif // OLD_DRAG_AND_DROP

    //! A pointer to 'this' object. 
    /*! We can only ever have one of these.
     */
    static DragNDropPanel *sourceDragNDropObject;

    //! The cursor shape for dragging a Panel.
    QCursor *customDragNDropCursor;

public slots:

private:
};
#endif // DRAGNDROPPANEL_H
