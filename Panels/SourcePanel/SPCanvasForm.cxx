#include "SPCanvasForm.hxx"

#include "debug.hxx"

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPCanvasForm::SPCanvasForm( QWidget *parent, const char *n, WFlags fl )
    : QWidget( parent, n, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPCanvasForm::SPCanvasForm( ) constructor called\n");

  canvasTextList.clear();

  /*! Put all this in a layout so the resize does the right thing...  */
  canvasFormLayout = new QVBoxLayout( this, 0, -1, "CanvasFormLayout");

  canvasFormHeaderLayout = new QHBoxLayout( canvasFormLayout, 2, "canvasFormHeaderLayout" );
  canvasFormHeaderLayout->setMargin(1);

  label = new QLabel( this, "canvas", 0);
  label->setCaption("canvas label");
  QFont font = label->font();
  font.setBold(TRUE);
  label->setFont(font);
  label->setFrameStyle( QFrame::NoFrame );
  label->setSizePolicy(QSizePolicy( (QSizePolicy::SizeType)5,
                                    (QSizePolicy::SizeType)0, 0, 0,
                                    label->sizePolicy().hasHeightForWidth() ) );
  QString label_text = "No label.";
  label->setText(label_text);
  canvasFormHeaderLayout->addWidget( label );

  label->show();

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
SPCanvasForm::setHighlights(QFont canvas_font, float lineHeight, int topLine, int visibleLines, int top_offset)
{
  nprintf(DEBUG_CONST_DESTRUCT) ("SPCanvasForm::setHighlights()\n");
  nprintf(DEBUG_CONST_DESTRUCT) ("lineHeight=%f topLine=%d visibleLines=%d\n", lineHeight, topLine, visibleLines );

  int i = 0;
  char buffer[100];
  for(i=0;i<visibleLines;i++)
  {
    sprintf(buffer, "%d", topLine+i);
    QCanvasText *text = new QCanvasText( buffer, canvas_font, canvasArea);
    text->setColor("black");
    text->setX(10);
//    text->setY(i*lineHeight);
    text->setY( (i*lineHeight)-top_offset );
// printf("put out label (%s) at %dx%f\n", buffer, 10, i*lineHeight);
    text->setZ(1);
    text->show();
    canvasTextList.push_back(text);
  }

  canvasArea->update();
}

void
SPCanvasForm::clearAllItems()
{
    nprintf(DEBUG_CONST_DESTRUCT) ("clearAllItems(%d) entered\n", canvasTextList.count() );

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
