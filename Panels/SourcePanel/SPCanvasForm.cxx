#include "SPCanvasForm.hxx"

#include "debug.hxx"

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPCanvasForm::SPCanvasForm( QWidget *parent, const char *n, WFlags fl )
    : QWidget( parent, n, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPCanvasForm::SPCanvasForm( ) constructor called\n");

  /*! Put all this in a layout so the resize does the right thing...  */
//  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );

  canvasTextList.clear();

  canvasFormLayout = new QHBoxLayout( this, 0, -1, "CanvasFormLayout");

  canvasArea = new QCanvas( this );
  canvasArea->setBackgroundColor(parent->backgroundColor());
  canvasView = new SPCanvasView(canvasArea, this, "SPCanvasView");
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
SPCanvasForm::updateHighlights(QFont canvas_font, int lineHeight, int topLine)
{
printf("SPCanvasForm::updateHighlights()\n");
//   canvasItemList = canvasArea->allItems();
QCanvasItem *item = NULL;
QCanvasText *text_item = NULL;
  int i = 0;
char buffer[100];
int textEdit_header_offset = 20;
  for( CanvasTextList::iterator it = canvasTextList.begin(); it != canvasTextList.end(); ++it )
  {
printf("redraw item %d\n", i);
sprintf(buffer, "%d", topLine+i);
// item = *it;
// text_item = (QCanvasText *)item;
text_item = *it;
text_item->setText(buffer);
text_item->setColor("black");
text_item->show();
i++;
  }
}

void
SPCanvasForm::setHighlights(QFont canvas_font, int lineHeight, int topLine, int visibleLines)
{
printf("SPCanvasForm::setHighlights()\n");
printf("lineHeight=%d topLine=%d visibleLines=%d\n", lineHeight, topLine, visibleLines );

  int i = 0;
  char buffer[100];
  int textEdit_header_offset = 20;
  for(i=0;i<visibleLines;i++)
  {
    sprintf(buffer, "%d", topLine+i);
    QCanvasText *text = new QCanvasText( buffer, canvas_font, canvasArea);
    text->setColor("black");
    text->setX(10);
    text->setY((i*lineHeight)+textEdit_header_offset);
printf("put out label (%s) at %dx%d\n", buffer, 10, i*lineHeight);
    text->setZ(1);
    text->show();
    canvasTextList.push_back(text);
  }

}

void
SPCanvasForm::clearAllItems()
{
  printf("clearAllItems() entered\n");

  if( canvasTextList.empty() )
  {
    return;
  }
  
  QCanvasText *text_item = NULL;
  for( CanvasTextList::iterator it = canvasTextList.begin(); it != canvasTextList.end(); ++it )
  {
    text_item = *it;
text_item->hide();  
  }

  canvasTextList.clear();
}
