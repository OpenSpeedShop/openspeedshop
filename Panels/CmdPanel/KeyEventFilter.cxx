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
#ifdef OLDWAY
printf("key_event->state()=%d\n", key_event->state() );
if( key_event->state() == QEvent::ControlButton && key_event->key() == Qt::Key_J )
{
  cmdPanel->downKey();
  return FALSE;
}
if( key_event->state() == QEvent::ControlButton && key_event->key() == Qt::Key_K )
{
  cmdPanel->upKey();
  return FALSE;
}
#endif // OLDWAY

  }

  return FALSE;
}
