#ifndef CANVASTEXT_H
#define CANVASTEXT_H

#include <qcanvas.h>

class QFont;


class CanvasText : public QCanvasText
{
public:
    enum { CANVAS_TEXT = 1100 };

    CanvasText( int index, QCanvas *canvas )
	: QCanvasText( canvas ), m_index( index ) {}
    CanvasText( int index, const QString& text, QCanvas *canvas )
	: QCanvasText( text, canvas ), m_index( index ) {}
    CanvasText( int index, const QString& text, QFont font, QCanvas *canvas )
	: QCanvasText( text, font, canvas ), m_index( index ) {}

    int index() const { return m_index; }
    void setIndex( int index ) { m_index = index; }

    int rtti() const { return CANVAS_TEXT; }

private:
    int m_index;
};

#endif

