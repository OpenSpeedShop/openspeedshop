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


#include "SPCanvasView.hxx"

#include "debug.hxx"

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPCanvasView::SPCanvasView( QCanvas *c, QWidget *parent, const char *n )
    : QCanvasView( c, parent, n )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPCanvasView::SPCanvasView( ) constructor called\n");
  canvas = c;
}

/*! The default destructor. */
SPCanvasView::~SPCanvasView( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPCanvasView::~SPCanvasView( ) destructor called\n");
}

void
SPCanvasView::viewportResizeEvent( QResizeEvent *e )
{
  nprintf(DEBUG_PANELS) ("SPCanvasView::viewportResizeEvent() entered.\n");

  nprintf(DEBUG_PANELS) ("resize to %dx%d\n", e->size().width(), e->size().height() );

  canvas->resize( e->size().width(), e->size().height() );
}
