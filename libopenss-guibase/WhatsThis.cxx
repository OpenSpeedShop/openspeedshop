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


/*! \class WhatsThis
    Overloaded function so we can trap hook into the hide() function.
 */

#include <cstddef>
#include <qapplication.h>
#include <qevent.h>
#include "WhatsThis.hxx"

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

WhatsThis::WhatsThis( QWidget *widget )
    : QWhatsThis( widget )
{
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis constructor called.\n");
}

/*! The default destructor. */
WhatsThis::~WhatsThis( )
{
  // default destructor.
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis Destructor called.\n");
}

void
WhatsThis::hide( QObject *obj )
{
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis::hide() entered\n");

  QMouseEvent me( QEvent::MouseButtonPress, QCursor::pos(), 0, 0 );
  QApplication::sendEvent( obj, &me );
}
