/*! \class MyQTextEdit
  This is simple a reimplementation of the QTextEdit widget so we can
  popup a context sensitive menu.
*/
#include "MyQTextEdit.hxx"

#include <qpopupmenu.h>
#include "SourcePanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! default constructor.   Here for completeness only. */
MyQTextEdit::MyQTextEdit( )
{
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
MyQTextEdit::MyQTextEdit( SourcePanel *sp, QWidget *parent, const char *n )
    : QTextEdit( parent, n )
{
  printf( "MyQTextEdit::MyQTextEdit( ) constructor called\n");
  sourcePanel = sp;
}

/*! The default destructor. */
MyQTextEdit::~MyQTextEdit( )
{
  // default destructor.
  printf("  MyQTextEdit::~MyQTextEdit( ) destructor called\n");
}

QPopupMenu*
MyQTextEdit::createPopupMenu( const QPoint & pos )
{
  dprintf("Hello from down under the hood.\n");

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
MyQTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
  printf("MyQTextEdit::contentsMouseMoveEvent() entered\n");

  sourcePanel->armPanelsWhatsThis();
}
