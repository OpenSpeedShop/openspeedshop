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

// Debug Flag
//#define DEBUG_chart 1
// Debug Flag

/*! \class SPChartForm
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "SPChartForm.hxx"

#include <qpopupmenu.h>
#include <qcursor.h>
#include "StatsPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPChartForm::SPChartForm( StatsPanel *sp, QSplitter *splitter, const char *n, int flags )
    : ChartForm( splitter, n, flags )
{
  statsPanel = sp;
  popupMenu = NULL;
  panelMenu = NULL;
  nprintf(DEBUG_CONST_DESTRUCT) ( "SPChartForm::SPChartForm( ) constructor called\n");
#ifdef DEBUG_chart
  printf("SPChartForm::SPChartForm( ) constructor called\n");
#endif
}

/*! The default destructor. */
SPChartForm::~SPChartForm( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  SPChartForm::~SPChartForm( ) destructor called\n");
#ifdef DEBUG_chart
  printf("  SPChartForm::~SPChartForm( ) destructor called\n");
#endif
}

int
SPChartForm::mouseClicked( int item )
{
  nprintf(DEBUG_PANELS) ("SPChartForm::mouseClicked(item=%d) called.\n", item);
#ifdef DEBUG_chart
  printf("SPChartForm::mouseClicked() entered, setting statsPanel->itemSelected(item)\n");
#endif
  statsPanel->itemSelected(item);
}

void
SPChartForm::contentsContextMenuEvent( QContextMenuEvent *e)
{
  nprintf(DEBUG_PANELS) ("SPChartForm::contentsContextMenuEvent() entered\n");
#ifdef DEBUG_chart
  printf("SPChartForm::contentsContextMenuEvent() entered\n");
#endif

  createPopupMenu( QCursor::pos() );
}

QPopupMenu*
SPChartForm::createPopupMenu( const QPoint & pos )
{
  nprintf(DEBUG_PANELS) ("SPChartForm: Hello from down under the hood.\n");
#ifdef DEBUG_chart
  printf("SPChartForm::createPopupMenu, Hello from down under the hood.\n");
#endif

  // First create the default Qt widget menu...
  if( panelMenu )
  {
    delete panelMenu;
  }

  popupMenu = ChartForm::createPopupMenu(pos);

  // Now create the panel specific menu... and add it to the popup as
  // a cascading menu.
  
  panelMenu = new QPopupMenu(this);
  statsPanel->menu(panelMenu);
//  popupMenu->insertSeparator();
  popupMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_P );
//  popupMenu->insertSeparator();
//  popupMenu->insertItem("&Goto Line...", this, SLOT(goToLine()) );

  popupMenu->exec( pos );

  return popupMenu;
}

void
SPChartForm::goToLine() 
{
#ifdef DEBUG_chart
  printf("SPChartForm::goToLine() entered, calling statsPanel->gotoSource(TRUE)\n");
#endif
  statsPanel->gotoSource(TRUE);
}
