#include "MyQTextEdit.hxx"

#include <qpopupmenu.h>
#include "TopPanel.hxx"

#include "debug.hxx"

#include <qcursor.h> // For debugging only.  Remove.

#include <stdlib.h>  // for the free() call below.

/*! default constructor.   Here for completeness only. */
MyQTextEdit::MyQTextEdit( )
{
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
MyQTextEdit::MyQTextEdit( TopPanel *t5p, QWidget *parent, const char *n )
    : QTextEdit( parent, n )
{
  topFivePanel = t5p;
  nprintf(DEBUG_CONST_DESTRUCT) ( "MyQTextEdit::MyQTextEdit( ) constructor called\n");
}

/*! The default destructor. */
MyQTextEdit::~MyQTextEdit( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  MyQTextEdit::~MyQTextEdit( ) destructor called\n");
}

QPopupMenu*
MyQTextEdit::createPopupMenu( const QPoint & pos )
{
  nprintf(DEBUG_PANELS) ("MyQTextEdit::createPopupMenu() Hello from down under the hood.\n");

  nprintf(DEBUG_PANELS) ("pos->x()=%d pos->y=%d\n", pos.x(), pos.y() );
  nprintf(DEBUG_PANELS) ("QCursor x=%d y=%d\n", QCursor::pos().x(), QCursor::pos().y() );

  // First create the default Qt widget menu...
  QPopupMenu *popupMenu = QTextEdit::createPopupMenu(pos);

  // Now create the panel specific menu... and add it to the popup as
  // a cascading menu.
  QPopupMenu *panelMenu = new QPopupMenu(this);
  topFivePanel->menu(panelMenu);
  popupMenu->insertSeparator();
  popupMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_P );

  // Now look to see if there are any dynamic menus to add.
  popupMenu->insertSeparator();
  topFivePanel->createTextEditPopupMenu(popupMenu, pos);
  return popupMenu;
}

void
MyQTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
  nprintf(DEBUG_PANELS) ("MyQTextEdit::contentsMouseMoveEvent() entered\n");

  topFivePanel->armPanelsWhatsThis();
}
