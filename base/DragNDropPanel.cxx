/*! \class DragNDropPanel

    This class is responsible for managing the drag and drop work.   It
    doesn't handle the events that cause the d-n-d (see PanelContainer.cxx and
    Frame.cxx), but all other work is done here.

    In a nutshell, when the drag is initiated a static DragNDropPanel class is 
    created.   It holds handles to the PanelContainer that initiated
    the request and to the PanelContainer Frame class.   This initiation
    is triggered through a left mouse down (hold) action using Qt's drag-n-drop
    mechanism.    

    Once a drag is started and all the required validation checks are done
    the drop action will be called.  (See member function DropPanel() )   On
    the drop more validation takes place, the drop location is defined, and
    the Panel being drag-n-drop is reparented to a new PanelContainer.

    Control is then returned to the event loop.

    Author: Al Stipek   (stipek@sgi.com)
 */

#include "debug.hxx"  // This includes the definition of nprintf

#include "DragNDropPanel.hxx"

#include <qbitmap.h>
#include <qcursor.h>
#include <qmessagebox.h>


#include "PanelContainer.hxx"
#include "Frame.hxx"
#include "TabBarWidget.hxx"
#include "TopWidget.hxx"


#ifdef OLD_DRAG_AND_DROP
#define CHANGE_CURSOR 1
#endif // OLD_DRAG_AND_DROP

#ifdef CHANGE_CURSOR
#define panel_drag_bitmap_width 16
#define panel_drag_bitmap_height 16
static unsigned char panel_drag_bitmap_bits[] = {
   0x00, 0x00, 0x7c, 0x00, 0x82, 0x00, 0x02, 0x7f, 0x02, 0x40, 0x02, 0x40,
   0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
   0x02, 0x40, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00};

#define panel_dragmask_width 16
#define panel_dragmask_height 16
static unsigned char panel_dragmask_bits[] = {
   0x00, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0x86, 0x7f, 0x06, 0xff, 0x06, 0xc0,
   0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0,
   0x06, 0xc0, 0x06, 0xc0, 0xfe, 0xff, 0xfc, 0xff};
#endif // CHANGE_CURSOR

DragNDropPanel *DragNDropPanel::sourceDragNDropObject = NULL;

#include <qapplication.h>

/*! This constructor is not called.   It is here for completeness.
 */
DragNDropPanel::DragNDropPanel( )
{
  panelContainer = NULL;
}

/*! This constructor is the work constructor.   It is called to 
    create a dragable object that will be passed to the drop site.
    \param sourcePC is a pointer to PanelContainer that initiated the drag.
    \param sourceFrame is a pointer to the frame that initiated the drag.
 */
DragNDropPanel::DragNDropPanel( PanelContainer *pc, Frame *sourceFrame )
{
  panelContainer = pc;
  frame = sourceFrame;

  nprintf(DEBUG_DND) ("\n\n\nDragNDropPanel(load: %s) Frame=(%s)\n", panelContainer->getInternalName(), sourceFrame->getName() );

#ifdef CHANGE_CURSOR
  QBitmap custom_bits(
    panel_drag_bitmap_width, panel_drag_bitmap_height,
    panel_drag_bitmap_bits, TRUE );
  QBitmap custom_map(
    panel_dragmask_width, panel_dragmask_height,
    panel_dragmask_bits, TRUE );
  customDragNDropCursor = new QCursor( custom_bits, custom_map );
//  nprintf(DEBUG_DND) ("setOverrideCursor() \n");
//  qapplication->setOverrideCursor( *customDragNDropCursor );
  qApp->setOverrideCursor( *customDragNDropCursor );
#endif // CHANGE_CURSOR
}

/*! No extra objects are created.   None need to be deleted. */
DragNDropPanel::~DragNDropPanel( )
{
  // default destructor.
  nprintf(DEBUG_DND) ("DragNDropPanel (%s) destructor called\n", panelContainer ? panelContainer->getInternalName() : "" );

  panelContainer = NULL;

#ifdef CHANGE_CURSOR
  nprintf(DEBUG_DND) ("restoreOverrideCursor() \n");
  qApp->restoreOverrideCursor( );
#endif // CHANGE_CURSOR
}

/*! This member function is called on the drop callback.   It is called
    when the Qt drag and drop attempt fails.   It first attempts to 
    determine if there is a valid PanelContainer (Qt's drag and drop
    should have caught this one) to drop to.  It's must make
    sure there is a valid sourcePC and a valid dropSite.    
    The dropSite is looked up base on the x,y locations of the mouse
    event.    The PanelContainer::findPanelContainerByMouseLocation();
    function is called to determine the target PanelContainer.
    It then is responsible for reparenting the Panel's widgets that
    are being dragged, creating a new toplevel widget if the Panel is
    being dropped onto the desktop.
    \param pc is the source PanelContainer.

    This call results from a drag off the tool onto the desktop.
 */
void
DragNDropPanel::DropPanel( PanelContainer *sourcePC )
{
  nprintf(DEBUG_DND) ("DropPanel  sourcePC=(%s)\n", sourcePC->getInternalName() );

  // Now, get the location of the mouse and return a PanelContainer given 
  // current mouse location.
  PanelContainer *targetPC = sourcePC->findPanelContainerByMouseLocation();

#ifndef OLD_DRAG_AND_DROP
  // We only care about this value if we've tried to drop on ourself.
  // Otherwise, set the value to null and drop the Panel on the desktop.
  if( targetPC != sourcePC )
  {
    targetPC = NULL;
  }
#endif // OLD_DRAG_AND_DROP

  // Make sure there is a valid place to drop it.
  if( sourcePC == targetPC || sourcePC->tabWidget == NULL )
  {
    nprintf(DEBUG_DND) ("sourcePC and targePC are the same or there is no tabWidget to drop to.  Abort the DragNDrop\n");
    delete( DragNDropPanel::sourceDragNDropObject );
    return;
  }

  // From the sourcePC, snag the currentPage and get it's Panel.
  QWidget *currentPage = sourcePC->tabWidget->currentPage();
  Panel *p = sourcePC->getRaisedPanel();
  if( !p )
  {
    fprintf(stderr, "Error: Couldn't locate a panel to drag.\n");
    delete( DragNDropPanel::sourceDragNDropObject );
    return;
  }
nprintf(DEBUG_DND) ("Drag panel=(%s)\n", p->getName() );

  QPoint point;
  // If there's not a targetPC, then the request is to create one.
  if( targetPC == NULL )
  {
    nprintf(DEBUG_DND) ("No target PC\n");
    QPoint mouse_pos = QCursor::pos ();
    nprintf(DEBUG_DND) ("x=%d y=%d\n", mouse_pos.x(), mouse_pos.y() );

    TopWidget *topLevelWidget = new TopWidget( 0, "toplevel" );
    
    // Put a PanelContainer in the new toplevel 
    targetPC = createPanelContainer(topLevelWidget, "outside PC", NULL, sourcePC->getMasterPCList() );
    targetPC->setMasterPC(sourcePC->getMasterPC());
    // Mark the new PanelContainer as s topLevel.
    targetPC->topLevel = TRUE;


    // We must set this for proper destructor cleanup.   Otherwise no
    // destructors will be called after the window is closed with the
    // window manager's 'x' or 'close' functions.
    topLevelWidget->setPanelContainer(targetPC);


    // Now resize the new window to the current size of the Panel that is
    // being dragged.
    int width = sourcePC->parent->width();
    int height = sourcePC->parent->height();
    nprintf(DEBUG_DND) ("Try to resize the new window to w=%d h=%d\n", width, height);
    topLevelWidget->resize(width, height);
    topLevelWidget->setCaption( "Open/SpeedShop" );

    // Move the new window to the drop mouse location.
    topLevelWidget->move(mouse_pos.x(), mouse_pos.y() );

    // Make sure the user can see the new window by show()ing it's widgets
    topLevelWidget->show();
    targetPC->show();
  }

 
  nprintf(DEBUG_DND) ("onto targetPC=(%s)\n", targetPC->getInternalName() );
  QWidget *w = targetPC->dropSiteLayoutParent;

  // Make sure there is a drop site.   If there's not one create one.
  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
//    targetPC->tabWidget = new QTabWidget( w, "tabWidget" );
    targetPC->tabWidget = new TabWidget( targetPC, w, "tabWidget" );
fprintf(stderr, "WHOAOOOOO Expect to error shortly!\n");
targetPC->tabBarWidget = new TabBarWidget( targetPC, w, "tabBarWidget");
targetPC->tabWidget->setTabBar(targetPC->tabBarWidget);

  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

// nprintf(DEBUG_DND) ("OVERRIDE THE EVENTS AND MAKE THEM ACTIVE!!!\n");
// targetPC->getMasterPC->_eventsEnabled = TRUE;

  if( p->topLevel )
  {
    nprintf(DEBUG_DND) ("Check for parenting issues on the drop site targetPC=(%s:%s)\n", targetPC->getInternalName(), targetPC->getExternalName() );
    nprintf(DEBUG_DND) ("p->topPC=(%s:%s)\n", p->topPC->getInternalName(), p->topPC->getExternalName() );
     // Make sure this topLevel isn't being parented into one of it's own
     // child panel containers.
    PanelContainer *upPC = targetPC;
    while( upPC != NULL )
    {
      nprintf(DEBUG_DND) ("Check %s-%s.\n", upPC->getInternalName(), upPC->getExternalName() );
      if( upPC == p->topPC )
      {
        QString msg;
        delete( DragNDropPanel::sourceDragNDropObject );
        msg =
          QString("Unable to drag a Parent Panel to one of it's child panel containers.");
        QMessageBox::information( (QWidget *)p, "Drop target", msg, QMessageBox::Ok );

        return;
      }
      upPC = upPC->parentPanelContainer;
    }
  }
  // We're about to move the Panel.

  // First remove the Panel from the old PanelContainer list.
  p->getPanelContainer()->panelList.remove(p);

  // Set the Panels parent PanelContainer field to the new PanelContainer.
  p->setPanelContainer(targetPC);

  // Also set the Panel's base frame's pointer to the new PanelContainer.
  p->getBaseWidgetFrame()->setPanelContainer(targetPC);

  // Set the TabBarWidget to the new panel container.
  p->getBaseWidgetFrame()->getPanelContainer()->tabBarWidget->setPanelContainer(targetPC);
  
  // Reparent the tabWidget.
  currentPage->reparent(p->getPanelContainer()->tabWidget, 0, point, TRUE);
  // Reparent the baseWidget
  p->getBaseWidgetFrame()->reparent(w, 0, point, TRUE);

  // Reparent the Panel's base widget. (Create with the call to the Panel
  // constructor.
  QWidget *panel_base = (QWidget *)p;
  panel_base->reparent((QWidget *)targetPC, 0, point, TRUE);

  // Now add the actual tab to the tabWidget in the new PanelContainer.
  p->getPanelContainer()->tabWidget->addTab( currentPage, p->getName() );

  p->getPanelContainer()->augmentTab( currentPage );

  // Add the move Panel to the new PanelContainers panelList.
  p->getPanelContainer()->panelList.push_back(p);

  // Make sure we can see it, so call show....
  p->getBaseWidgetFrame()->show();
  p->getPanelContainer()->dropSiteLayoutParent->show();

  p->getPanelContainer()->handleSizeEvent(NULL);

  // Now set the newest one current...
  int count = p->getPanelContainer()->tabWidget->count();
  if( count > 0 )
  {
    p->getPanelContainer()->tabWidget->setCurrentPage(count-1);
  }

  // Remove the tab from the old PanelContainer.
  sourcePC->tabWidget->removePage(currentPage);


  // If we just pulled off the last Panel from the old PanelContainer
  // hide the dropSiteLayoutParent (since it's only needed for the tabWidget
  // to show Panels.   This cleans up the look for the PanelContainer to 
  // keep it fresh and clean like a split().
  if( !sourcePC->areTherePanels() )
  {
    sourcePC->dropSiteLayoutParent->hide();
  }
 
  // Once you know this happened correctly!
  delete( DragNDropPanel::sourceDragNDropObject );

  // Make sure the new PanelContainer has everything showing.
  targetPC->leftFrame->show();
  targetPC->dropSiteLayoutParent->show();
  targetPC->tabWidget->show();
  targetPC->splitter->show();

  nprintf(DEBUG_DND) ("\n\n\n");
}


#ifndef OLD_DRAG_AND_DROP
/*! This is the resulting call when a successful Qt drag-n-drop within 
    the tool occurs.   (Not onto the desktop.)   This is duplicate code 
    from the previous (DragNDropPanel::DropPanel()) function and should
    be cleaned up.     The difference between this function and the previous
    function is that the input parameter here is the targetPC rather than 
    the sourcePC.
 */
void
DragNDropPanel::DropPanelWithQtDnD( PanelContainer *targetPC)
{
  nprintf(DEBUG_DND) ("DropPanel in targetPC=(%s)\n", targetPC->getInternalName() );

  // Make sure there is a valid place to drop it.
  if( panelContainer == targetPC || panelContainer->tabWidget == NULL )
  {
    nprintf(DEBUG_DND) ("sourcePC and targePC are the same or there is no tabWidget to drop to.  Abort the DragNDrop\n");
    delete( DragNDropPanel::sourceDragNDropObject );
    return;

  }

  // Make sure the target wasn't a grab bar that was located on top
  // of an already split PanelContainer.
  if( targetPC->leftPanelContainer && targetPC->rightPanelContainer )
  {
    nprintf(DEBUG_DND) ("targetPC has a left and right PanelContainer.\n");
    delete( DragNDropPanel::sourceDragNDropObject );
    return;
  }

  // From the sourcePC, snag the currentPage and get it's Panel.
  QWidget *currentPage = panelContainer->tabWidget->currentPage();
  Panel *p = panelContainer->getRaisedPanel();
  if( !p )
  {
    fprintf(stderr, "Error: Couldn't locate a panel to drag.\n");
    delete( DragNDropPanel::sourceDragNDropObject );
    return;
  }
nprintf(DEBUG_DND) ("Drag panel=(%s)\n", p->getName() );

  QPoint point;
  // If there's not a targetPC, then the request is to create one.
  if( targetPC == NULL )
  {
    nprintf(DEBUG_DND) ("No target PC\n");
    QPoint mouse_pos = QCursor::pos ();
    nprintf(DEBUG_DND) ("x=%d y=%d\n", mouse_pos.x(), mouse_pos.y() );

    TopWidget *topLevelWidget = new TopWidget( 0, "toplevel" );
    
    // Put a PanelContainer in the new toplevel 
    targetPC = createPanelContainer(topLevelWidget, "outside PC", NULL, panelContainer->getMasterPCList() );
    targetPC->setMasterPC(panelContainer->getMasterPC());
    // Mark the new PanelContainer as s topLevel.
    targetPC->topLevel = TRUE;


    // We must set this for proper destructor cleanup.   Otherwise no
    // destructors will be called after the window is closed with the
    // window manager's 'x' or 'close' functions.
    topLevelWidget->setPanelContainer(targetPC);


    // Now resize the new window to the current size of the Panel that is
    // being dragged.
    int width = panelContainer->parent->width();
    int height = panelContainer->parent->height();
    nprintf(DEBUG_DND) ("Try to resize the new window to w=%d h=%d\n", width, height);
    topLevelWidget->resize(width, height);
    topLevelWidget->setCaption( "Open/SpeedShop" );

    // Move the new window to the drop mouse location.
    topLevelWidget->move(mouse_pos.x(), mouse_pos.y() );

    // Make sure the user can see the new window by show()ing it's widgets
    topLevelWidget->show();
    targetPC->show();
  }

 
  nprintf(DEBUG_DND) ("onto targetPC=(%s)\n", targetPC->getInternalName() );
  QWidget *w = targetPC->dropSiteLayoutParent;

  // Make sure there is a drop site.   If there's not one create one.
  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
//    targetPC->tabWidget = new QTabWidget( w, "tabWidget" );
    targetPC->tabWidget = new TabWidget( targetPC, w, "tabWidget" );
fprintf(stderr, "WHOAOOOOO Expect to error shortly!\n");
targetPC->tabBarWidget = new TabBarWidget( targetPC, w, "tabBarWidget");
targetPC->tabWidget->setTabBar(targetPC->tabBarWidget);

  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

// nprintf(DEBUG_DND) ("OVERRIDE THE EVENTS AND MAKE THEM ACTIVE!!!\n");
// targetPC->_masterPC->_eventsEnabled = TRUE;

  if( p->topLevel )
  {
    nprintf(DEBUG_DND) ("Check for parenting issues on the drop site targetPC=(%s:%s)\n", targetPC->getInternalName(), targetPC->getExternalName() );
    nprintf(DEBUG_DND) ("p->topPC=(%s:%s)\n", p->topPC->getInternalName(), p->topPC->getExternalName() );
     // Make sure this topLevel isn't being parented into one of it's own
     // child panel containers.
    PanelContainer *upPC = targetPC;
    while( upPC != NULL )
    {
      nprintf(DEBUG_DND) ("Check %s-%s.\n", upPC->getInternalName(), upPC->getExternalName() );
      if( upPC == p->topPC )
      {
        QString msg;
        delete( DragNDropPanel::sourceDragNDropObject );
        msg =
          QString("Unable to drag a Parent Panel to one of it's child panel containers.");
        QMessageBox::information( (QWidget *)p, "Drop target", msg, QMessageBox::Ok );

        return;
      }
      upPC = upPC->parentPanelContainer;
    }
  }
  // We're about to move the Panel.

  // First remove the Panel from the old PanelContainer list.
  p->getPanelContainer()->panelList.remove(p);

  // Set the Panels parent PanelContainer field to the new PanelContainer.
  p->setPanelContainer(targetPC);

  // Also set the Panel's base frame's pointer to the new PanelContainer.
  p->getBaseWidgetFrame()->setPanelContainer(targetPC);
  
  // Reparent the tabWidget.
  currentPage->reparent(p->getPanelContainer()->tabWidget, 0, point, TRUE);
  // Reparent the baseWidget
  p->getBaseWidgetFrame()->reparent(w, 0, point, TRUE);

  // Reparent the Panel's base widget. (Create with the call to the Panel
  // constructor.
  QWidget *panel_base = (QWidget *)p;
  panel_base->reparent((QWidget *)targetPC, 0, point, TRUE);

  // Now add the actual tab to the tabWidget in the new PanelContainer.
  p->getPanelContainer()->tabWidget->addTab( currentPage, p->getName() );

  p->getPanelContainer()->augmentTab( currentPage );

  // Add the move Panel to the new PanelContainers panelList.
  p->getPanelContainer()->panelList.push_back(p);

  // Make sure we can see it, so call show....
  p->getBaseWidgetFrame()->show();
  p->getPanelContainer()->dropSiteLayoutParent->show();

  p->getPanelContainer()->handleSizeEvent(NULL);

  // Now set the newest one current...
  int count = p->getPanelContainer()->tabWidget->count();
  if( count > 0 )
  {
    p->getPanelContainer()->tabWidget->setCurrentPage(count-1);
  }

  // Remove the tab from the old PanelContainer.
  panelContainer->tabWidget->removePage(currentPage);


  // If we just pulled off the last Panel from the old PanelContainer
  // hide the dropSiteLayoutParent (since it's only needed for the tabWidget
  // to show Panels.   This cleans up the look for the PanelContainer to 
  // keep it fresh and clean like a split().
  if( !panelContainer->areTherePanels() )
  {
    panelContainer->dropSiteLayoutParent->hide();
  }
 
  // Once you know this happened correctly!
  delete( DragNDropPanel::sourceDragNDropObject );

  // Make sure the new PanelContainer has everything showing.
  targetPC->leftFrame->show();
  targetPC->dropSiteLayoutParent->show();
  targetPC->tabWidget->show();
  targetPC->splitter->show();

  nprintf(DEBUG_DND) ("\n\n\n");
}
#endif // OLD_DRAG_AND_DROP
