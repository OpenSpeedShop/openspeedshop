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
