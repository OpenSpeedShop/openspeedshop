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


/*! \class TabWidget
   Overloads the QTabWidget so we can trap and manage mouse events.
   Specifically menu events.
 */  
#include "TabWidget.hxx"

#include <qtooltip.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include "x.xpm"

#include "AnimatedQPushButton.hxx"
#include "PanelContainer.hxx"


#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

/*! default constructor.   Here for completeness only. (unused) */
TabWidget::TabWidget( )
{
  fprintf(stderr, "TabWidget::TabWidget() should not be called.\n");
  fprintf(stderr, "see: TabWidget::TabWidget(QWidget *parent, const char *n );\n");
}

/*! Work constructor.   Overloads the QTabWidget so we can accept mouse 
    events for tab specific menu callbacks.
*/
TabWidget::TabWidget( PanelContainer *pc, QWidget *parent, const char *n )
    : QTabWidget( parent, n )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabWidget constructor called.\n");

  _panelContainer = pc;

  cw = new AnimatedQPushButton(this, "corner widget");
  QPixmap *pm = new QPixmap( x_xpm );
  pm->setMask(pm->createHeuristicMask());
  cw->setPixmap( *pm );
  cw->push_back( pm );

  cw->setFlat(TRUE);
  cw->setEnabled(TRUE);
  QToolTip::add( cw, tr( "Remove the current tab (Panel)." ) );

cw->resize(16,16);

  setCornerWidget( cw );

  cw->show();

  connect( cw, SIGNAL( clicked() ), this, SLOT( deletePanelButtonSelected() ) );

  languageChange();
}

/*! The default destructor. */
TabWidget::~TabWidget( )
{
  // default destructor.
  nprintf(DEBUG_PANELCONTAINERS) ("TabWidget Destructor called.\n");
}


/*! Sets the default tab bar to our overloaded tab bar. */
void
TabWidget::setTabBar( QTabBar *tb)
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabWidget::setTabBar() entered\n");
  QTabWidget::setTabBar(tb);
}

/*! Gets the tab bar. */
QTabBar *
TabWidget::tabBar( )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabWidget::tabBar() entered\n");
  return( QTabWidget::tabBar() );
}


/*!  Sets the strings of the subwidgets using the current language.
 */
void
TabWidget::languageChange()
{ 
  setCaption( tr( "TabWidget" ) );
} 

void
TabWidget::deletePanelButtonSelected()
{
  _panelContainer->getMasterPC()->removeRaisedPanel( _panelContainer );
}
