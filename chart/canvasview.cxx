#include "canvasview.hxx"
#include "chartform.hxx"
// enum ChartType { PIEWITHSHADOW, PIEWITHNOSHADOW, PIEWITH3D, VERTICAL_BAR, HORIZONTAL_BAR, BARWITH3D };

#include <qcursor.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>

#include <qapplication.h>

#include "debug.hxx"

void CanvasView::contentsContextMenuEvent( QContextMenuEvent *e )
{
  nprintf(DEBUG_PANELS) ("CanvasView::contentsContextMenuEvent() entered.\n");


#ifdef OLDWAY
  ((ChartForm*)parent())->createPopupMenu( QCursor::pos() );
#endif  // OLDWAY

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


#define SELECTMODE 0
void CanvasView::contentsMousePressEvent( QMouseEvent *e )
{
    nprintf(DEBUG_PANELS) ("CanvasView::contentsMousePressEvent() entered.\n");

    if( e->button() != LeftButton )
    {
      return;
    }

    QCanvasItemList list = canvas()->collisions( e->pos() );

    CanvasEllipse *item = NULL;
    QCanvasItemList::iterator it = NULL;
	ChartForm *form = (ChartForm*)parent();
    m_movingItem = NULL;
    for ( it = list.begin(); it != list.end(); ++it )
    {
	  if ( (*it)->rtti() == CanvasEllipse::CANVAS_ELLIPSE )
      {
        nprintf(DEBUG_PANELS) ("Match CANVAS_ELLIPSE\n");
	    m_movingItem = *it;
	    m_pos = e->pos();
        nprintf(DEBUG_PANELS) ("m_pos.x()=%d m_pos.y()=%d\n", m_pos.x(), m_pos.y() );
	    int chartType = form->chartType();
        item = (CanvasEllipse *)m_movingItem;
      }
      if( item != NULL )
      {
        int selected = item->index();
        nprintf(DEBUG_PANELS) ("A: THE USER SELECTED ELEMENT =%d\n", selected );

        int chartType = form->chartType();
        CanvasShadowEllipse *cse = NULL;
        if( chartType == PIEWITHSHADOW || chartType == PIEWITH3D || chartType == PIEWITHNOSHADOW )
        {
          QCanvasItemList all_list = canvas()->allItems();
          nprintf(DEBUG_PANELS) ("drawElements() entered\n");
          for ( QCanvasItemList::iterator shadow_it = all_list.begin(); shadow_it != all_list.end(); ++shadow_it )
          {
            if ( (*shadow_it)->rtti() == CanvasShadowEllipse::CANVAS_SHADOW_ELLIPSE )
            {
              cse = (CanvasShadowEllipse *)*shadow_it;
              nprintf(DEBUG_PANELS) ("... looking ... index=%d!\n", cse->index() );
              if( cse->index() == selected )
              {
                nprintf(DEBUG_PANELS) ("FOUND IT!\n");
                break;
              }
            }
          }

          int shadow_angle = 3;
          static double initial_z = -1;
          m_movingItem->moveBy( -shadow_angle,  -shadow_angle );

          if( cse )
          {
            cse->moveBy( -shadow_angle, -shadow_angle );
          }

          double z = m_movingItem->z();
          if( initial_z == -1 )
          {
            initial_z = m_movingItem->z();
          }
          m_movingItem->setZ( z+100 );
          canvas()->update();
          // move back.
          m_movingItem->moveBy( shadow_angle, shadow_angle );
          m_movingItem->setZ( initial_z );
          if( cse )
          {
            cse->moveBy( shadow_angle, shadow_angle );
          }
        }

        nprintf(DEBUG_PANELS) ("call form->mouseClicked(%d)\n", selected);
        form->mouseClicked(selected);

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


