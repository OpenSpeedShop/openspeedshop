#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "element.hxx"
#include "canvastext.hxx"
#include "canvasellipse.hxx"
#include "canvasshadowellipse.hxx"

#include <qcanvas.h>


class QPoint;


class CanvasView : public QCanvasView
{
    Q_OBJECT
public:
    CanvasView( QCanvas *canvas, ElementVector *elements,
		QWidget* parent = 0, const char* name = "canvas view",
		WFlags f = 0 )
	: QCanvasView( canvas, parent, name, f ),
	  m_elements( elements ) { }
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
