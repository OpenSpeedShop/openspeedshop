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

#ifndef CANVASRECTANGLE_H
#define CANVASRECTANGLE_H

#include <qcanvas.h>

class QFont;


//! Defines a simple canvas ellipse used to draw a pie chart.
class CanvasRectangle : public QCanvasRectangle
{
public:
    enum { CANVAS_RECTANGLE = 1201 };

    CanvasRectangle( int i, int s1, int s2, int a, int extent, QCanvas *m_canvas )
	: QCanvasRectangle( s1, s2, a, extent, m_canvas ), m_index( i ) {}

    int index() const { return m_index; }
    void setIndex( int index ) { m_index = index; }

    int rtti() const { return CANVAS_RECTANGLE; }

private:
    int m_index;
};

#endif // CANVASRECTANGLE_H
