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
  sleepTimer = NULL;
  popupTimer = NULL;
}

/*! The default destructor. */
MyQTextEdit::~MyQTextEdit( )
{
  // default destructor.
  printf("  MyQTextEdit::~MyQTextEdit( ) destructor called\n");
#ifdef FINGERLINGER
  if( sleepTimer )
  {
    sleepTimer->stop();
    delete sleepTimer;
  }
  if( popupTimer )
  {
    popupTimer->stop();
    delete popupTimer;
  }
#endif // FINGERLINGER
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

#ifdef FINGERLINGER
void
MyQTextEdit::wakeupFromSleep()
{
// printf("wakeupFromSleep() called\n");
  popupTimer->start(250, TRUE);
}

void
MyQTextEdit::popupInfoAtLine()
{
  printf("Popup up some info, if any....\n");
  if( sourcePanel->whatIsAtPos(last_pos) )
  {
    int line = sourcePanel->textEdit->paragraphAt(last_pos);
    printf("What is at line %d that's so interesting?\n", line);
  }
}

void
MyQTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
//  printf("MyQTextEdit::contentsMouseMoveEvent() entered\n");

  last_pos = e->pos();

  if( sleepTimer && sleepTimer->isActive() )
  { // If we're sleeping, just ignore this...
//    printf("we're sleeping, just return.\n");
    sleepTimer->start(2000, TRUE);
    return;
  } else
  { // Otherwise, check to see if there's a timer set.   If it is set
    // just go to sleep for a whil and return.   Otherwise, set a new one.
    if( popupTimer && popupTimer->isActive() )
    {
//      printf("popupTimer is already active... start sleeping...\n");
      if( sleepTimer == NULL )
      {
        sleepTimer = new QTimer(this, "sleepTimer");
        connect( sleepTimer, SIGNAL(timeout()), this, SLOT(wakeupFromSleep()) );
      }
      sleepTimer->start(2000, TRUE);
      popupTimer->stop();
    } else
    {
//      printf("start the popup timer...\n");
      if( popupTimer == NULL )
      {
        popupTimer = new QTimer(this, "popupTimer");
        connect( popupTimer, SIGNAL(timeout()), this, SLOT(popupInfoAtLine()) );
      }
      if( sleepTimer )
      {
        sleepTimer->stop();
      }
      popupTimer->start(2000, TRUE);
    }
  }

}
#endif // FINGERLINGER
