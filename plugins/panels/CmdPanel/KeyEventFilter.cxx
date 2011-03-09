////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 The Krell Institute All Rights Reserved.
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
  cmdPanel->user_line_buffer = QString::null;
} 

/* QObject *o;   o is really the object the event was install. */
bool
KeyEventFilter::eventFilter( QObject *o, QEvent *e)
{
//printf("eventFilter entered type=(%d)\n", e->type() );
  if( e->type() == QEvent::KeyPress )
  {
    if( cmdPanel->editingHistory == FALSE )
    {
      cmdPanel->positionToEnd();
    }
    QKeyEvent *key_event = (QKeyEvent *)e;
//printf("The user pressed a (%s)\n", key_event->text().ascii() );
    if( key_event->key() == Qt::Key_Up ) 
    {
      cmdPanel->user_line_buffer = QString::null;
      cmdPanel->upKey();
      return TRUE;
    } else if( key_event->key() == Qt::Key_Down ) 
    {
      cmdPanel->user_line_buffer = QString::null;
      cmdPanel->downKey();
      return TRUE;
    } else if( key_event->key() == Qt::Key_Return ) 
    {
// #ifdef OLDWAY
      cmdPanel->positionToEnd();
      cmdPanel->returnPressed();
      cmdPanel->user_line_buffer = QString::null;
      return TRUE;
// #endif // OLDWAY
    } else if( key_event->key() == Qt::Key_Backspace )
    {
      QString tstr = QString::null;
      tstr = cmdPanel->user_line_buffer.left(cmdPanel->user_line_buffer.length()-1);
      cmdPanel->user_line_buffer = tstr;
    } else if( key_event->state() == QEvent::ControlButton && key_event->key() == Qt::Key_C
 ) {
     if( cmdPanel->user_line_buffer.isEmpty() )
     {
// printf("Send the cli an interrupt!\n");
     } else 
     {
       cmdPanel->user_line_buffer = QString::null;
     }
     cmdPanel->putOutPrompt();
   } else
   {
      cmdPanel->user_line_buffer += key_event->text();
   }

   if( key_event->state() == QEvent::ControlButton && key_event->key() == Qt::Key_C )
    {
//printf("User pressed a Cntrl->C\n");
      cmdPanel->CntrlC();
    }

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

  }

  return FALSE;
}
