#ifndef CANVASSHADOWELLIPSE_H
#define CANVASSHADOWELLIPSE_H

#include <qcanvas.h>

class QFont;


//! Defines a simple shadow for the canvas ellipse.
class CanvasShadowEllipse : public QCanvasEllipse
{
public:
    enum { CANVAS_SHADOW_ELLIPSE = 1300 };

    CanvasShadowEllipse( int i, int s1, int s2, int a, int extent, QCanvas *m_canvas )
	: QCanvasEllipse( s1, s2, a, extent, m_canvas ), m_index( i ) {}

    int index() const { return m_index; }
    void setIndex( int index ) { m_index = index; }

    int rtti() const { return CANVAS_SHADOW_ELLIPSE; }

private:
    int m_index;
};

#endif // CANVASSHADOWELLIPSE_H
