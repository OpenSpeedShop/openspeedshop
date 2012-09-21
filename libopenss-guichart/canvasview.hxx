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


#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <cstddef>
#include "element.hxx"
#include "canvastext.hxx"
#include "canvasellipse.hxx"
#include "canvasshadowellipse.hxx"
#include "canvasrectangle.hxx"

#include <qcanvas.h>


class QPoint;


//! The qcanvasview that holds the chart.
class CanvasView : public QCanvasView
{
    Q_OBJECT
public:
    CanvasView( QCanvas *canvas, ElementVector *elements,
		QWidget* parent = 0, const char* name = "canvas view",
		WFlags f = 0 )
	: QCanvasView( canvas, parent, name, f ),
	  m_elements( elements ) { }

    int getItemFromPos( QPoint pos );
protected:
    void viewportResizeEvent( QResizeEvent *e );
    virtual void contentsMousePressEvent( QMouseEvent *e );
#ifdef MOUSE_MOVE
    void contentsMouseMoveEvent( QMouseEvent *e );
#endif // MOUSE_MOVE
    void contentsContextMenuEvent( QContextMenuEvent *e );

private:
    QCanvasItem *m_movingItem;
    QPoint m_pos;
    ElementVector *m_elements;
    bool handlingEvent;
};


#endif
