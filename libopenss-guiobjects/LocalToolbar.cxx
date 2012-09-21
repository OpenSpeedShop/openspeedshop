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
#include <qbitmap.h>
#include <qwidget.h>
#include <qhbox.h>
#include <qlayout.h>
#include "debug.hxx"

#include "LocalToolbar.hxx"

LocalToolbar::LocalToolbar(QWidget *p, const char *n, bool f) : QWidget( p, n)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("LocalToolbar::LocalToolbar() constructor called\n");

  layout = new QHBoxLayout( this, 3, 0, n);

}

LocalToolbar::~LocalToolbar()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("  LocalToolbar::~LocalToolbar() destructor called\n");
}

AnimatedQPushButton *
LocalToolbar::addButton(QPixmap *image)
{
  AnimatedQPushButton *pb = new AnimatedQPushButton(this, "pushbutton");
  pb->setPixmap(*image);
//pb->setIconSet( QIconSet( *image ) );
  pb->setFlat(TRUE);
  pb->setEnabled(TRUE);

pb->setMinimumSize( QSize(18,18) );
pb->setSizePolicy(QSizePolicy( (QSizePolicy::SizeType)QSizePolicy::Fixed, (QSizePolicy::SizeType)QSizePolicy::Fixed, 0, 0, TRUE ) );

  pb->resize(18,18);
  layout->addWidget(pb);
  pb->show();


  return pb;
}
