////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 Krell Institute All Rights Reserved.
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


/*! \class SPListView
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "SPListView.hxx"

#include <qpopupmenu.h>
#include <qtooltip.h>
#include <qcursor.h>
#include "StatsPanel.hxx"

//#define DEBUG_SPL 1

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPListView::SPListView( StatsPanel *sp, QSplitter *splitter, const char *n, int flags )
     : QListView( splitter, n, flags )
{
#if DEBUG_SPL
  fprintf (stderr, "SPListView::SPListView( ) constructor called\n");
#endif

  statsPanel = sp;

  setAllColumnsShowFocus(TRUE);

  // If there are subitems, then indicate with root decorations.
  setRootIsDecorated(TRUE);

  // If there should be sort indicators in the header, show them here.
  setShowSortIndicator(TRUE);
#if DEBUG_SPL
  fprintf (stderr, "SPListView::contentsContextMenuEvent() entered\n");
#endif

//  QToolTip::add( this, tr( "The list of statistics collected." ) );


  clear();

}

/*! The default destructor. */
SPListView::~SPListView( )
{
  // default destructor.
  dprintf ("  SPListView::~SPListView( ) destructor called\n");
}

/*! Create the local popup menu for the list items. */
void
SPListView::contentsContextMenuEvent( QContextMenuEvent *e)
{
  dprintf ("SPListView::contentsContextMenuEvent() entered\n");

  createPopupMenu( QCursor::pos() );
}

QPopupMenu*
SPListView::createPopupMenu( const QPoint & pos )
{
  dprintf ("SPListView: Hello from down under the hood.\n");

  QPopupMenu *popupMenu = new QPopupMenu(this);

  if( statsPanel->createPopupMenu(popupMenu, pos) )
  {
    popupMenu->exec( pos );
    return popupMenu;
  }

  delete popupMenu;

  return NULL;
}

