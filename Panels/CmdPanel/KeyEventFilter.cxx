
#include "KeyEventFilter.hxx"

#include <qevent.h>

KeyEventFilter::KeyEventFilter(QObject *t, CmdPanel *p) : QObject(t)
{
  cmdPanel = p;
} 

/* QObject *o;   o is really the object the event was install. */
bool
KeyEventFilter::eventFilter( QObject *o, QEvent *e)
{

  if( e->type() == QEvent::KeyPress )
  {
    QKeyEvent *key_event = (QKeyEvent *)e;
    if( key_event->key() == Qt::Key_Up ) 
    {
//      printf("Qt::Key_Up\n");
      cmdPanel->upKey();
      return TRUE;
    } else if( key_event->key() == Qt::Key_Down ) 
    {
//      printf("Qt::Key_Down\n");
      cmdPanel->downKey();
      return TRUE;
    } else if( key_event->key() == Qt::Key_Return ) 
    {
//      printf("Qt::Key_Return\n");
      cmdPanel->positionToEndForReturn();
      return FALSE;
    }
  }

  return FALSE;
}
