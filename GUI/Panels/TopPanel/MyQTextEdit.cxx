#include "MyQTextEdit.hxx"

#include <qpopupmenu.h>
#include "TopPanel.hxx"

#include "debug.hxx"

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
  printf( "MyQTextEdit::MyQTextEdit( ) constructor called\n");
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
  printf("MyQTextEdit::createPopupMenu() Hello from down under the hood.\n");

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
