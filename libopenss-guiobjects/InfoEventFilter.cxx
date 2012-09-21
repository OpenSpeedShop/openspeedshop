////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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


#include <cstddef>
#include "InfoEventFilter.hxx"

#include "Panel.hxx"
#include "PanelContainer.hxx"

InfoEventFilter::InfoEventFilter(QObject *t, Panel *_p) : QObject(t)
{
  target = t;
  panel = _p;
  _masterPC = panel->getPanelContainer()->getMasterPC();
} 

void
InfoEventFilter::armWhatsThis()
{
//   printf ("InfoEventFilter::armWhatsThis() entered.\n");

  if( _masterPC->sleepTimer == NULL )
  {
//    printf ("Create the sleepTimer\n");
    _masterPC->sleepTimer = new QTimer(this, "sleepTimer");
    connect( _masterPC->sleepTimer, SIGNAL(timeout()), this, SLOT(wakeupFromSleep()) );
  }
  if( _masterPC->popupTimer == NULL )
  {
//    printf ("Create the popupTimer\n");
    _masterPC->popupTimer = new QTimer(this, "popupTimer");
    connect( _masterPC->popupTimer, SIGNAL(timeout()), this, SLOT(popupInfo()) );
  }
  _masterPC->sleepTimer->stop();
  _masterPC->popupTimer->start(1000, TRUE);
}

/* QObject *o;   o is really the object the event was install. */
bool
InfoEventFilter::eventFilter( QObject *o, QEvent *e)
{
//  printf("InfoEventFilter: We just got an event type=%d\n", e->type() );
  switch( e->type() )
  {
    case QEvent::Enter:
//       printf("Enter!\n");
       armWhatsThis();
     break;
    case QEvent::Leave:
//       printf("Leave!\n");

       _masterPC->whatsThis->hide( o );
       if( _masterPC->popupTimer )
       {
         _masterPC->popupTimer->stop();
         delete _masterPC->popupTimer;
         _masterPC->popupTimer = NULL;
       }
       if( _masterPC->sleepTimer )
       {
         _masterPC->sleepTimer->stop();
         delete _masterPC->sleepTimer;
         _masterPC->sleepTimer = NULL;
       }
      break;
    default:
      break;
  }

  return FALSE;
}

void
InfoEventFilter::wakeupFromSleep()
{
//  printf("InfoEventFilter::wakeupFromSleep() entered.\n");
  
  _masterPC->popupTimer->start(250, TRUE);
}


void
InfoEventFilter::popupInfo()
{
//  printf("InfoEventFilter::popupInfo() entered.\n");
  
  panel->info(_masterPC->last_pos, target);
}

