////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#ifndef CANVASTEXT_H
#define CANVASTEXT_H

#include <qcanvas.h>

class QFont;


//! Text to be added to the charts.
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

