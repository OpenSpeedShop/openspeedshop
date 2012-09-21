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


/*! \class Frame
    The Frame class is the container class for all user PanelContainers.   
    The events handled are:
    - contextMenuEvent
      QContextMenuEvent is passed both to the PanelContainer to create
      it's menu, but also to the Panel to add any additional Panel specifi
      menu options.
    - resizeEvent
      This is passed trough to the Frame's parent PanelContainer for fielding.
    - dropEnterEvent
      Currently called but nothing happens
    - dropEnter
      This is the worker event for the drop event.
 */

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_FRAMES) 

#include <cstddef>
#include "Frame.hxx"

#include "PanelContainer.hxx"
#include "DragNDropPanel.hxx"

#include <qdragobject.h>

#include <qpopupmenu.h>

#include <qcursor.h>

#define DEBUG_OUTPUT_REQUESTED 1

#include <stdlib.h>  // for the free() call below.

bool Frame::dragging = FALSE;

/*! This constructor should not be called. */
Frame::Frame( )
{
  fprintf(stderr, "This constructor should not be called.\n");
  fprintf(stderr, "see: Frame::Frame(PanelContainer *, QWidget *, const char *);\n");
  setDragEnabled(FALSE);
  setDropEnabled(FALSE);
  setPanelContainer(NULL);
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.

    \param pc is a pointer to PanelContainer it is attached to.
    \param parent is the parent widget to attach this Frame.
    \param name is the name give to the Frame.
 */
Frame::Frame( PanelContainer *pc, QWidget *parent, const char *n )
    : QFrame( parent, n )
{
  char tmp_name[1024];

  setAcceptDrops(TRUE);

  sprintf(tmp_name, "%s: %s", pc->getInternalName().ascii(), n);
  name = strdup(tmp_name);
  setDragEnabled(FALSE);
  setDropEnabled(FALSE);
  setFrameShape( QFrame::StyledPanel );
  setFrameShadow( QFrame::Raised );
  setPanelContainer(pc);

  languageChange();
}

/*! Destroys the object and frees any allocated resources, namely name.
 */
Frame::~Frame( )
{
  if( name )
  {
    free(name);
  }
}

#ifdef OLD_DRAG_AND_DROP
/*! Fields the mousePressEvent.
    This is how drag is enabled for a panel.  (if the dragEnable flag is set.)
    The sourceDragNDropObject is created and a global flag is set notifying 
    everyone that a drag is undeway.
 */
void Frame::mousePressEvent(QMouseEvent *e)
{
#ifdef OLDWAY
  if( getPanelContainer() == NULL )
  {
    return;
  }

  nprintf(DEBUG_FRAMES) ("Frame::mousePressEvent(%s) pc=(%s)\n", getName(), getPanelContainer()->getInternalName().ascii() );
printf ("HERE!!!!  Frame::mousePressEvent(%s) pc=(%s)\n", getName(), getPanelContainer()->getInternalName().ascii() );

  if( e->button() != LeftButton )
  {
    return;
  }

  if( !isDragEnabled() )
  {
      nprintf(DEBUG_FRAMES) (" dragging not enable for this frame\n");
    return;
  }
  nprintf(DEBUG_FRAMES) ("attempt to drag the panel \n");

  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel("OpenSpeedShop-Drag-N-Drop-Event", getPanelContainer(), this);

  if( DragNDropPanel::sourceDragNDropObject == NULL )
  {
    Frame::dragging = FALSE;
  }
#endif // OLDWAY
}

/*! Fields the mouseReleaseEvent.
    This is how the Panel drop is activated.   
    If a drag is occuring, then the DragNDropPanel::DropPanel() routine 
    is called to handle dropping of the Panel.
 */
void Frame::mouseReleaseEvent(QMouseEvent *)
{
  if( getPanelContainer() == NULL )
  {
    return;
  }

  nprintf(DEBUG_FRAMES) ("Frame::mouseReleaseEvent(%s-%s) isDropEnabled()=%d Frame::dragging=%d entered\n", getPanelContainer()->getInternalName().ascii(), getPanelContainer()->getExternalName().ascii(), isDropEnabled(), Frame::dragging );

  if( !isDropEnabled() || !Frame::dragging )
  {
    return;
  }
  nprintf(DEBUG_FRAMES) ("attempt to drop the panel \n");

  // Change the cursor... back
  nprintf(DEBUG_FRAMES) (" change the cursor back\n");
  Frame::dragging = FALSE;

  DragNDropPanel::sourceDragNDropObject->DropPanel(getPanelContainer());
}
#endif // OLD_DRAG_AND_DROP

#ifndef OLD_DRAG_AND_DROP
/*! This callback is currently unused.   */
void Frame::dragEnterEvent(QDragEnterEvent* event)
{
  nprintf(DEBUG_FRAMES) ("Frame::dragEnterEvent() entered\n");

#ifdef OLDWAY
  QString text;
  if( QTextDrag::canDecode(event) )
  {
    if( QTextDrag::decode(event, text) )
    {
      nprintf(DEBUG_FRAMES) ("text=(%s)\n", text.ascii() );
      if( strcmp(text.ascii(), "Open|SpeedShop-QDragString") == 0 )
      {
        event->accept( QTextDrag::canDecode(event) );
      }
    }
  }
#else // OLDWAY
  if( event->provides("OpenSpeedShop-Drag-N-Drop-Event") )
  {
    nprintf(DEBUG_DND)("dragEnterEvent() We think we accept the event!\n");

	event->accept();
  } else
  {
    nprintf(DEBUG_DND)("dragEnterEvent() NO I DON'T WANT YOU!\n");
  }
#endif // OLDWAY
}

/*! This callback is generated from the Qt Drag and Drop event.   Once fielded
    it will call a routine to determine where the drop event occurred and
    then act on it.
 */
void Frame::dropEvent(QDropEvent* event)
{
  nprintf(DEBUG_FRAMES) ("Frame::dropEnterEvent() entered\n");
  nprintf(DEBUG_FRAMES) ("Frame::dropEvent(%s-%s) isDropEnabled()=%d Frame::dragging=%d entered\n", getPanelContainer()->getInternalName().ascii(), getPanelContainer()->getExternalName().ascii(), isDropEnabled(), Frame::dragging );

  nprintf(DEBUG_FRAMES) ("attempt to drop the panel \n");

  if( event->provides("OpenSpeedShop-Drag-N-Drop-Event") )
  {
    nprintf(DEBUG_FRAMES) ("THINK WE'VE GOT A PROTECTED DROP!\n");
  } else
  {
    nprintf(DEBUG_FRAMES) ("part doesn't accept drops.\n");
  }

  // Change the cursor... back
  nprintf(DEBUG_FRAMES) (" change the cursor back\n");
  Frame::dragging = FALSE;

  DragNDropPanel::sourceDragNDropObject->DropPanelWithQtDnD(getPanelContainer());
}
#endif // OLD_DRAG_AND_DROP

/*! This event is passed on to the PanelContainer::resizeEvent(...) routine.
 */
void Frame::resizeEvent(QResizeEvent *e)
{ 
  if( !getPanelContainer() )
  {
    return;
  }

  getPanelContainer()->handleSizeEvent(e);
//  nprintf(DEBUG_FRAMES) ("Frame::resizeEvent()s\n");
} 

void
Frame::contextMenuEvent( QContextMenuEvent *)
{
  contextMenuEvent( (QContextMenuEvent *)NULL, TRUE );
}

/*! Fields the mouseReleaseEvent.
    This is how the Panel drop is activated.   
    If a drag is occuring, then the DragNDropPanel::DropPanel() routine 
    is called to handle dropping of the Panel.
 */
/*! Fields the menu event to create the PanelContainer menu, then 
    request the current (raised) Panel add it's menu entries. */
void
Frame::contextMenuEvent( QContextMenuEvent *, bool localMenu )
{
  bool *flag = NULL;
  if( getPanelContainer() == NULL )
  {
    nprintf(DEBUG_FRAMES) ("  no getPanelContainer() (ERROR).  - return\n");
    return;
  }
  if( getPanelContainer()->leftPanelContainer && getPanelContainer()->rightPanelContainer)
  {  // There should be no menu action for this split panel container.  Only
     // for it's children.
    nprintf(DEBUG_FRAMES) ("  There are children.  - return\n");
     return;
  }
  if( getPanelContainer()->getMasterPC()->_doingMenuFLAG == TRUE )
  {
    nprintf(DEBUG_FRAMES) ("  already doing menu - return\n");
    return;
  }
  getPanelContainer()->getMasterPC()->_doingMenuFLAG = TRUE;
  flag = &getPanelContainer()->getMasterPC()->_doingMenuFLAG;


  nprintf(DEBUG_FRAMES) ("Frame::contextMenuEvent(%s-%s)\n",
    getPanelContainer()->getInternalName().ascii(), getPanelContainer()->getExternalName().ascii() );

  getPanelContainer()->getMasterPC()->panelContainerContextMenuEvent( getPanelContainer(), localMenu );

  if( flag ) *flag = FALSE;
}


#ifdef OLDWAY
void Frame::enterEvent ( QEvent * )
{
  nprintf(DEBUG_FRAMES) ("Frame::enterEvent(%s) pc=(%s-%s)\n", getName(), getPanelContainer()->getInternalName().ascii(), getPanelContainer()->getExternalName().ascii() );
}


// Not active at this time, but it's the hook to handle leave events.
void Frame::leaveEvent ( QEvent * )
{
//  nprintf(DEBUG_FRAMES) ("Frame::leaveEvent(%s) pc=(%s)\n", getName(), getPanelContainer()->getInternalName().ascii() );
}
#endif // OLDWAY

/*!  Sets the strings of the subwidgets using the current language.
 */
void
Frame::languageChange()
{ 
  setCaption( tr( name ) );
} 



