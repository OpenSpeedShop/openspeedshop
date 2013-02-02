////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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


#ifndef SPCANVAS_H
#define SPCANVAS_H

#include <stddef.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qcanvas.h>
#include <qfont.h>
#include <qvaluelist.h>
#include <qheader.h>

#include "SPCanvasForm.hxx"
#include "SPCanvasView.hxx"

#define DEFAULT_CANVAS_WIDTH 100
#define DEFAULT_CANVAS_MIN 20

#include "HighlightObject.hxx"

typedef QValueList<QCanvasText *> CanvasTextList;
class SPCanvasForm : public QWidget
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPCanvasForm( int label_height, QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~SPCanvasForm( );

    int numColumns;
    QVBoxLayout * canvasFormLayout;
    QCanvas *canvas;
    SPCanvasView *canvasView;
    QHBoxLayout * canvasFormHeaderLayout;
    QHeader *header;

    QCanvasItemList canvasItemList; // Not used anymore...
    CanvasTextList canvasTextList;
    void clearAllItems();
    void setHighlights(QFont font, int lineHeight,
                       int lastTop, int visibleLines,
                       int line_count = 0, int top_offset=-2,
                       HighlightList *highlightList = NULL);
protected: 

public slots:

private:
};
#endif // SPCANVAS_H
