/*! \class SPTextEdit
  This is simple a reimplementation of the QTextEdit widget so we can
  popup a context sensitive menu.
*/
#include "SPTextEdit.hxx"

#include <qpopupmenu.h>
#include "SourcePanel.hxx"

#include <qscrollbar.h>

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
SPTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
  nprintf(DEBUG_PANELS) ("SPTextEdit::contentsMouseMoveEvent() entered\n");

  sourcePanel->armPanelsWhatsThis();
}

#include <qpainter.h>
void
SPTextEdit::paintEvent( QPaintEvent *e )
{
  printf("SPTextEdit::paintEvent() entered\n");
  vbar = verticalScrollBar();
  hbar = horizontalScrollBar();

  if( vannotatePixmap != NULL )
  { // Delete the old one.
    delete vannotatePixmap;
  }

  // Create a new pixmap for the background of the vertical scrollbar.
  vannotatePixmap = new QPixmap( vbar->width(), vbar->height() );
  printf("depth=%d\n", vannotatePixmap->depth() );
  vannotatePixmap->fill(vbar->backgroundColor());
}

void
SPTextEdit::annotateLine(int line, char *color)
{

printf("SPTextEdit::annotateLine() line=%d color=%s\n", line, color);

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
