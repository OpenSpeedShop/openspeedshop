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


/*! \class TopWidget
    Overloaded function so we can trap the closeEvent.
 */

#include "ClosingDownObject.hxx"
#include "TopWidget.hxx"
#include "PanelContainer.hxx"

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

/*! default constructor.   Here for completeness only. */
TopWidget::TopWidget( )
{
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
TopWidget::TopWidget( QWidget *parent, const char *n, WFlags f )
    : QWidget( parent, n, f )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TopWidget constructor called.\n");

  languageChange();
  panelContainer = NULL;
}

/*! The default destructor. */
TopWidget::~TopWidget( )
{
  // default destructor.
  nprintf(DEBUG_PANELCONTAINERS) ("TopWidget Destructor called.\n");
}

void
TopWidget::closeEvent( QCloseEvent *e )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TopWidget::closeEvent() entered.\n");
// printf("TopWidget::closeEvent() entered.\n");

  if( panelContainer && panelContainer->getMasterPC() )
  {
// printf("panelContainer(%s:%s)\n", panelContainer->getInternalName(), panelContainer->getExternalName() );
    // before you remove the panels notify everyone that they'r going away.
    ClosingDownObject *cdo = new ClosingDownObject();
    panelContainer->notifyAllDecendants((char *)cdo, panelContainer);
// printf("TopWidget::closeEvent() you actually removed %s:%s\n", panelContainer->getInternalName(), panelContainer->getExternalName() );


    Panel *p = panelContainer->findNamedPanel(panelContainer,(char *) "&Command Panel");
    if( p )
    {
// printf("TopWidget::closeEvent() found a CommandPanel to reparent.\n");

      // Only do this if we're removing the top widget that contains the
      // CommandPanel.
      if( panelContainer == p->getPanelContainer() )
      {
        panelContainer->raisePanel(p);
      
        panelContainer->removeRaisedPanel(p->getPanelContainer());
      }
    }

    panelContainer->getMasterPC()->removePanelContainer(panelContainer);
  }

#ifdef ABORTS
  // Peel off
  // SH
  // 'x' the peeled off window.
  delete this;
#else // ABORTS
  hide();
#endif // ABORTS
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
TopWidget::languageChange()
{ 
  setCaption( tr( "TopWidget" ) );
} 

