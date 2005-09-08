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


#include "SPCanvasForm.hxx"
#include <qstyle.h>

#include <qlabel.h> // REMOVE!
#include "debug.hxx"

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPCanvasForm::SPCanvasForm( int label_height, QWidget *parent, const char *n, WFlags fl )
    : QWidget( parent, n, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPCanvasForm::SPCanvasForm( ) constructor called\n");
  numColumns = 1;  // We by default create one... yet unused, but created.

  // This is the key to the resize (on intialization working)...
  setMinimumSize( QSize(DEFAULT_CANVAS_MIN,DEFAULT_CANVAS_MIN) );
  canvasTextList.clear();

  /*! Put all this in a layout so the resize does the right thing...  */
  canvasFormLayout = new QVBoxLayout( this, 0, -1, "CanvasFormLayout");

  canvasFormHeaderLayout = new QHBoxLayout( canvasFormLayout, -1, "canvasFormHeaderLayout" );
  canvasFormHeaderLayout->setMargin(1);

  QSpacerItem *spacerItem = new QSpacerItem(1,label_height, QSizePolicy::Fixed, QSizePolicy::Minimum );
  canvasFormHeaderLayout->addItem( spacerItem );

  header = new QHeader( this, "canvas header" );
  header->setFixedHeight(label_height);
  header->setCaption("canvas header");
  header->setStretchEnabled(TRUE, 0);
  canvasFormHeaderLayout->addWidget( header );
  header->addLabel(n, DEFAULT_CANVAS_WIDTH);
  header->show();

  canvas = new QCanvas( this );
  canvas->setBackgroundColor(parent->backgroundColor());
  canvasView = new SPCanvasView(canvas, this, n);
  canvasFormLayout->addWidget(canvasView);
  canvasView->show();
}


/*! The default destructor. */
SPCanvasForm::~SPCanvasForm( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPCanvasForm::~SPCanvasForm( ) destructor called\n");

  clearAllItems();
}

void
SPCanvasForm::setHighlights(QFont canvas_font, int lineHeight, int topLine, int visibleLines, int line_count, int top_offset, HighlightList *highlightList)
{
  nprintf(DEBUG_PANELS) ("SPCanvasForm::setHighlights()\n");
  nprintf(DEBUG_PANELS) ("lineHeight=%d topLine=%d visibleLines=%d line_count=%d\n", lineHeight, topLine, visibleLines, line_count );
// printf ("SPCanvasForm::setHighlights()\n");
// printf ("lineHeight=%d topLine=%d visibleLines=%d line_count=%d\n", lineHeight, topLine, visibleLines, line_count );

  int i = 0;
  char buffer[100];
  for(i=0;i<visibleLines;i++)
  {
    // Don't allow us to print past the number of lines in the file...
    if( topLine+i > line_count )
    {
      nprintf(DEBUG_PANELS) ("line_count=%d topLine=%d i=%d == %d\n", line_count, topLine, i, topLine+i );
      break;
    }
// printf ("LOOKUP HIGHLIGHT FOR THIS LINE=%d\n", i);
    if( highlightList != NULL )
    {
      for( HighlightList::Iterator it = highlightList->begin();
           it != highlightList->end();
           ++it)
      {
        HighlightObject *hlo = (HighlightObject *)*it;
// printf ("Look for %d\n", topLine+i);
        if( hlo->line == topLine+i && !hlo->value.isEmpty() )
        {
          nprintf(DEBUG_PANELS) ("We have data at that line!!!\n");
//printf ("We have data at that line!!!\n");
//hlo->print();
          sprintf(buffer, "%s", hlo->value.ascii() );
          QCanvasText *text = new QCanvasText( buffer, canvas_font, canvas);
          text->setColor("red");
          text->setX(10);
          text->setY( (i*lineHeight)-top_offset );
          text->setZ(1);
          text->show(); 
          canvasTextList.push_back(text);
    
          break;
        }
      }
    } else
    {
// printf("no highlightlist.\n");
    }
  }

  canvas->update();
}

void
SPCanvasForm::clearAllItems()
{
    nprintf(DEBUG_PANELS) ("clearAllItems(%d) entered\n", canvasTextList.count() );

  if( canvasTextList.empty() )
  {
    return;
  }
  
  QCanvasText *text_item = NULL;
  for( CanvasTextList::iterator it = canvasTextList.begin(); it != canvasTextList.end(); ++it )
  {
    text_item = (QCanvasText *)*it;
    text_item->hide();  
    if( text_item )
    {
      delete text_item;
    }
  }

  canvasTextList.clear();
}
