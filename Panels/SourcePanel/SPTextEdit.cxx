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


/*! \class SPTextEdit
  This is simple a reimplementation of the QTextEdit widget so we can
  popup a context sensitive menu.
*/
#include "SPTextEdit.hxx"

#include <qpopupmenu.h>
#include "SourcePanel.hxx"

#include <qscrollbar.h>
#include <qpainter.h>

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! default constructor.   Here for completeness only. */
SPTextEdit::SPTextEdit( )
{
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPTextEdit::SPTextEdit( SourcePanel *sp, QWidget *parent, const char *n )
    : QTextEdit( parent, n )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPTextEdit::SPTextEdit( ) constructor called\n");
  sourcePanel = sp;
  vbar = NULL;
  hbar = NULL;
  vannotatePixmap = NULL;
}

/*! The default destructor. */
SPTextEdit::~SPTextEdit( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPTextEdit::~SPTextEdit( ) destructor called\n");
}

QPopupMenu*
SPTextEdit::createPopupMenu( const QPoint & pos )
{
  nprintf(DEBUG_PANELS) ("Hello from down under the hood.\n");

  // First create the default Qt widget menu...
  QPopupMenu *popupMenu = QTextEdit::createPopupMenu(pos);

  // Now create the panel specific menu... and add it to the popup as
  // a cascading menu.
  QPopupMenu *panelMenu = new QPopupMenu(this);
  panelMenu->setCaption("SourcePanel menu");
  sourcePanel->menu(panelMenu);
  popupMenu->insertSeparator();
  popupMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_P );

  // Now look to see if there are any dynamic menus to add.
  popupMenu->insertSeparator();
  sourcePanel->createPopupMenu(popupMenu, pos);
  return popupMenu;
}

void
SPTextEdit::annotateScrollBarLine(int line, QColor(color))
{

  nprintf(DEBUG_PANELS) ("SPTextEdit::annotateScrollBarLine() line=%d\n", line );

  if( vbar == NULL )
  {
    return;
  }
  QPainter p( vannotatePixmap );

  int lineCount = this->paragraphs();
  int sbHeight = vbar->height();
  int sbWidth = vbar->width();
  int workRectangleHeight=sbHeight-(sbWidth*2);
  float ratio = (float)((float)line/(float)lineCount);
  int offset = (int)((workRectangleHeight*ratio)+sbWidth);

  p.setPen(color);
  p.drawLine(0,offset, sbWidth, offset );
  vbar->setPaletteBackgroundPixmap( *vannotatePixmap );
}

void
SPTextEdit::clearScrollBarLine(int line)
{
 nprintf(DEBUG_PANELS) ("SPTextEdit::clearScrollBarLine(%d) entered.", line);
  if( vbar == NULL )
  {
    return;
  }

  annotateScrollBarLine(line, vbar->backgroundColor());
}


void
SPTextEdit::clearScrollBar()
{
  nprintf(DEBUG_PANELS) ("Currently SPTextEdit::clearScrollBar() entered.\n");
  if( vbar == NULL )
  {
    vbar = verticalScrollBar();
    hbar = horizontalScrollBar();
//    sourcePanel->addWhatsThis( vbar, sourcePanel );
  }

  if( vannotatePixmap != NULL )
  { // Delete the old one.
    delete vannotatePixmap;
  }

  // Create a new pixmap for the background of the vertical scrollbar.
  vannotatePixmap = new QPixmap( vbar->width(), vbar->height() );
  nprintf(DEBUG_PANELS) ("depth=%d\n", vannotatePixmap->depth() );
  vannotatePixmap->fill(vbar->backgroundColor());
  vbar->setPaletteBackgroundPixmap( *vannotatePixmap );
}
