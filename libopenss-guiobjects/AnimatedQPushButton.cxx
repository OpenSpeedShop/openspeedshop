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
#include <qpushbutton.h>
#include <qbitmap.h>
#include "AnimatedQPushButton.hxx"
#include "debug.hxx"

AnimatedQPushButton::AnimatedQPushButton(QIconSet iconset, QString str, QWidget *p, bool f) : QPushButton( iconset, str, p)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("AnimatedQPushButton::AnimatedQPushButton() constructor called\n");
  enabledFLAG = f;
  imageList.clear();
}

AnimatedQPushButton::AnimatedQPushButton(QWidget *p, const char *n, bool f) : QPushButton( p, n)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("AnimatedQPushButton::AnimatedQPushButton() constructor called\n");
  enabledFLAG = f;
  imageList.clear();
}

AnimatedQPushButton::~AnimatedQPushButton()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("  AnimatedQPushButton::~AnimatedQPushButton() destructor called\n");
  imageList.clear();
}

void
AnimatedQPushButton::push_back(QPixmap *image)
{
  imageList.push_back(image);
}

void
AnimatedQPushButton::remove(QPixmap *image)
{
  imageList.remove(image);
}

void
AnimatedQPushButton::enterEvent ( QEvent *e )
{
//  printf("AnimatedQPushButton::enterEvent()\n");

  if( imageList.empty() )
  {
    return;
  }

  if( enabledFLAG == TRUE )
  {
    setFlat(FALSE);
  }

  QPixmap *pm;

  for( ImageList::Iterator it = imageList.begin();
       it != imageList.end();  it++)
  {
    pm = (QPixmap *)*it;
    this->setPixmap( *pm );
    QPushButton::paintEvent( (QPaintEvent *)e);
  }
}

void
AnimatedQPushButton::leaveEvent ( QEvent *e )
{
//  printf("AnimatedQPushButton::leaveEvent()\n");

  if( imageList.empty() )
  {
    return;
  }

  if( enabledFLAG == TRUE )
  {
    setFlat(TRUE);
  }


  ImageList::Iterator it = imageList.begin();
  QPixmap *pm = (QPixmap *)*it;
  
  this->setPixmap( *pm );
  QPushButton::paintEvent( (QPaintEvent *)e);
}
