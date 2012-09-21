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


#include <cstddef>
#include "canvasview.hxx"
#include "chartform.hxx"

#include <qcursor.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>

#include <qapplication.h>

#include "debug.hxx"

void CanvasView::contentsContextMenuEvent( QContextMenuEvent *e )
{
  nprintf(DEBUG_PANELS) ("CanvasView::contentsContextMenuEvent() entered.\n");


  ChartForm *form = (ChartForm*)parent();
  form->contentsContextMenuEvent(e);

}


void CanvasView::viewportResizeEvent( QResizeEvent *e )
{
  nprintf(DEBUG_PANELS) ("CanvasView::viewportResizeEvent() entered\n");
    canvas()->resize( e->size().width(), e->size().height() );

// FIX ?  I'm not sure you need this...
  qApp->flushX();
  qApp->syncX();


  ((ChartForm*)parent())->drawElements();
}


void CanvasView::contentsMousePressEvent( QMouseEvent *e )
{
    nprintf(DEBUG_PANELS) ("CanvasView::contentsMousePressEvent() entered.\n");

    if( e->button() != LeftButton )
    {
      return;
    }

    QCanvasItemList list = canvas()->collisions( e->pos() );

    QCanvasItemList::iterator it = NULL;
	ChartForm *form = (ChartForm*)parent();
    m_movingItem = NULL;
    int index = -1;
    for ( it = list.begin(); it != list.end(); ++it )
    {
      CanvasEllipse *item = NULL;
	  if ( (*it)->rtti() == CanvasEllipse::CANVAS_ELLIPSE )
      {
        nprintf(DEBUG_PANELS) ("Match CANVAS_ELLIPSE\n");
	    m_movingItem = *it;
	    m_pos = e->pos();
        nprintf(DEBUG_PANELS) ("m_pos.x()=%d m_pos.y()=%d\n", m_pos.x(), m_pos.y() );
	    int chartType = form->chartType();
        item = (CanvasEllipse *)m_movingItem;
        index = item->index();
      }
      if ( (*it)->rtti() == CanvasRectangle::CANVAS_RECTANGLE )
      {
        CanvasRectangle *item = NULL;
        nprintf(DEBUG_PANELS) ("Match CANVAS_RECTANGLE\n");
        m_movingItem = *it;
        m_pos = e->pos();
        nprintf(DEBUG_PANELS) ("m_pos.x()=%d m_pos.y()=%d\n", m_pos.x(), m_pos.y() );
        int chartType = form->chartType();
        item = (CanvasRectangle *)m_movingItem;
        index = item->index();
      }
      if( index > -1 )
      {

        nprintf(DEBUG_PANELS) ("call form->mouseClicked(%d)\n", index);
        form->mouseClicked(index);

        return;
      }
    }
}

#ifdef MOUSE_MOVE
void CanvasView::contentsMouseMoveEvent( QMouseEvent *e )
{
    nprintf(DEBUG_PANELS) ("CanvasView::contentsMouseMoveEvent() entered.\n");

    if ( m_movingItem ) {
	QPoint offset = e->pos() - m_pos;
	m_movingItem->moveBy( offset.x(), offset.y() );
	m_pos = e->pos();
	ChartForm *form = (ChartForm*)parent();
	form->setChanged( TRUE );
	int chartType = form->chartType();
	CanvasText *item = (CanvasText*)m_movingItem;
	int i = item->index();
    nprintf(DEBUG_PANELS) ("THE USER SELECTED ELEMENT =%d\n", i );

	(*m_elements)[i].setProX( chartType, item->x() / canvas()->width() );
	(*m_elements)[i].setProY( chartType, item->y() / canvas()->height() );

	canvas()->update();
    }

}
#endif // MOUSE_MOVE


