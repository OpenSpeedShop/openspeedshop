#ifndef CANVASELLIPSE_H
#define CANVASELLIPSE_H

#include <qcanvas.h>

class QFont;


class CanvasEllipse : public QCanvasEllipse
{
public:
    enum { CANVAS_ELLIPSE = 1200 };

    CanvasEllipse( int i, int s1, int s2, int a, int extent, QCanvas *m_canvas )
	: QCanvasEllipse( s1, s2, a, extent, m_canvas ), m_index( i ) {}

    int index() const { return m_index; }
    void setIndex( int index ) { m_index = index; }

    int rtti() const { return CANVAS_ELLIPSE; }

private:
    int m_index;
};

#endif // CANVASELLIPSE_H
