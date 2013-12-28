////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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


/*!  \class PanelContainer

    This is a very (very) key class in the overall base functionality of the
    GUI.   It is responsible for the screen management within the tool.

    The base functionality it provides is the interactions with the user
    to create and manage Panels.   A PanelContainer is reponsible for
    managing one or more Panels (User Panels).    There can be one or many 
    PanelContainers.   There is one master PanelContainer in the MainWindow
    which is the first PanelContainer created.   This master PanelContainer
    is responsible management of all other PanelContainers.  

    A PanelContainer may live only within the master window.  There can be
    PanelContainers within PanelContainers.   There can be PanelContainers
    within Panels.   

    A PanelContainer can be split (horizontally creating a left and right
    PanelContainer, or vertically creating a top and bottom PanelContainer).
    PanelContainers can be recursively split as well.
    
    PanelContainers can also be removed by undoing the split.

    In addition PanelContainers also manage Panels.   Panel resizing, menu
    management, moving, deleting, and adding Panels are key functionality.
    
    It's also the reponsibility of the PanelContainer class to save its state
    to a file that can then be reread in (at a subsequent invocation) to 
    allow the tool to come up with the same layout. (Yet to be implemented.)

    A PanelContainer can be have:
    - no children (i.e. It's Panel ready.)
    - 1 or more Panels.
    - 2 PanelContainers as children.  (i.e it's split with a left and right
      or a top and bottom.)
    
    Due to the collapsing logic, some panel containers can be hidden.
    These hidden panel containers will be recovered at the first possible 
    opportunity.

 */

//
// Debug Flag
//#define DEBUG_PContainer 1
//

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

#include "Panel.hxx"
#include "PanelContainer.hxx"

#include "DragNDropPanel.hxx"
#include <qdragobject.h>

#include "ClosingDownObject.hxx"
#include "LocalToolbar.hxx"
#include <qpushbutton.h>

#include <qcolor.h>
#include <qvariant.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <TabBarWidget.hxx>
#include <qevent.h>
#include <qaction.h>
#include <qmessagebox.h>

#include <qsizegrip.h>  // Debuggging only.

#include <qcursor.h>

#include "WhatsThis.hxx"

unsigned int __internal_debug_setting = 0x0000;  // Referenced in debug.hxx, define here.
#include <stdlib.h>
#include <string.h>

static char tmp_str[1024]; 
static int depth = 1;
static int max_depth = 1;
static int pc_rename_count = 0;

#include <qapplication.h>  // For exit.. and local event loop
#include <qeventloop.h>

#include <qwidgetlist.h> // For debugging loop of allWidgets
extern QEventLoop *qeventloop;

#include "PluginInfo.hxx"

#include "down_triangle_boxed.xpm"
#include "menu.xpm"
#include "hsplit.xpm"
#include "vsplit.xpm"
#include "x.xpm"

static  void indentString(int indent_count, char *buffer);

/*! PanelContainer(QWidget *parent, const char *name,
                       PanelContainer *parentPanelContainer,
                       PanelContainerList *panelContainerList,
                       WFlags fl=0)
    This constructor is the work constructor.   It is called to 
    create a PanelConatiner and attach it to a parentPanelContainer. 
    \param parent is a pointer to parent object that this PanelContainer
               should be initially attached.
    \param name is the name give to the PanelContainer
    \param parentPanelContainer  the parent PanelContainer, if there is one.
    \param panelContainerList the parent PanelContainer list, if any.
    \param fl  The flags to create the child QWidget.
 */
PanelContainer::PanelContainer( QWidget* _parent, const char* n, PanelContainer *_parentPanelContainer, PanelContainerList *pcl, WFlags fl) : QWidget( _parent, n, fl )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("PanelContainer::PanelContainer() working constructor called.\n");

  char cn[1024]; // Caption Name space

  outsidePC = FALSE;
  topWidget = NULL;
  topLevel = FALSE;

  if( strcmp(n, "masterPC") == 0 )
  { // This is the very first panel container...   The very first time in here.
    nprintf(DEBUG_PANELCONTAINERS) ("This is the first PC created.   set the count == 0 \n");
    _doingMenuFLAG = FALSE;
    setMasterPC( this );
    setMainWindow( NULL );
    _pluginRegistryList = NULL;
    _panel_container_count = 0;
    _eventsEnabled = TRUE;
    _resizeEventsEnabled = TRUE;
    _lastPC = this;
    sleepTimer = NULL;
    popupTimer = NULL;
    whatsThis = NULL;
    whatsThisActive = FALSE;
  } else
  {
    if( _parentPanelContainer )
    {
      setMasterPC( _parentPanelContainer->getMasterPC() );
    } else
    {
      PanelContainerList::Iterator it = pcl->begin();
      setMasterPC( (PanelContainer *)*it );
    }
  }

  pc_rename_count = 0;
  sprintf(pc_name_count, "pc:%d", getMasterPC()->_panel_container_count);
  internal_name = QString(pc_name_count);
  external_name = QString(n);

  _masterPanelContainerList = pcl;

  sourcePC = NULL;
  widget_to_reparent = NULL;
  menuEnabled = TRUE;
  markedForDelete = FALSE;
  _depth = 0;

  parent = _parent;
  parentPanelContainer = _parentPanelContainer;
  panelContainerFrameLayout = NULL;
  leftFrame = NULL;
  leftPanelContainer = NULL;
  rightFrame = NULL;
  rightPanelContainer = NULL;
  splitter = NULL;

  dropSiteLayoutParent = NULL;
  dropSiteLayout = NULL;
  tabWidget = NULL;
  tabBarWidget = NULL;

  if( getMasterPC()->_panel_container_count == 0 )
  {
    // Make sure the panelContainerList interator list is empty.
    nprintf(DEBUG_PANELCONTAINERS) ("MAKE SURE _masterPanelContainerList empty!!!!\n");
    _masterPanelContainerList->clear();
  }
  // Add this panel container to the global list.
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::PanelContainer() add PC getInternalName()=(%s)\n",
    getInternalName().ascii() );

  _masterPanelContainerList->push_back(this);

  panelList.clear();  // Make sure the panelList interator list is empty.

  if( internal_name.isEmpty() ) // Set the internal name...
  {
    setInternalName(pc_name_count);
  }

  // Create an internal layout for this PanelContainer
  panelContainerFrameLayout =
    new QVBoxLayout( parent, 0, 0, "panelContainerFrameLayout"); 

  // Now create the splitter so we can put a container (frame) on the left
  // and right or top and bottom.
  splitter = new QSplitter(parent, "splitter");
  splitter->setMinimumSize( QSize(10,10) );
  strcpy(cn,"splitter:");strcat(cn, internal_name.ascii());strcat(cn,"-");strcat(cn,external_name.ascii()); splitter->setCaption(cn);
  if( debug(DEBUG_FRAMES) ) splitter->setBackgroundColor("red");

  // The orientation doesn't matter at this point... at least not yet.
  splitter->setOrientation( QSplitter::Vertical );

  // Create a frame for the left side and show it.
  sprintf(tmp_str, "originalLeftFrame for %s", getInternalName().ascii() );
  leftFrame = new Frame(this, splitter, "left_frame");
  leftFrame->setMinimumSize( QSize(10,10) );
  if( debug(DEBUG_FRAMES) ) leftFrame->setBackgroundColor(Qt::white);
  leftFrame->setDragEnabled(TRUE);
  leftFrame->setDropEnabled(TRUE);

{
  QPixmap *pm = NULL;
  ltb = new LocalToolbar(leftFrame, "pc pushbutton");

  pm = new QPixmap( hsplit_xpm );
  pm->setMask(pm->createHeuristicMask());
  AnimatedQPushButton *splitHorizontalButton = ltb->addButton(pm);
  connect( splitHorizontalButton, SIGNAL( clicked() ), this, SLOT( splitHorizontal() ) );
  QToolTip::add( splitHorizontalButton, tr( "Split this container horizontally." ) );

  pm = new QPixmap( vsplit_xpm );
  pm->setMask(pm->createHeuristicMask());
  AnimatedQPushButton *splitVerticalButton = ltb->addButton(pm);
  connect( splitVerticalButton, SIGNAL( clicked() ), this, SLOT( splitVertical() ) );
  QToolTip::add( splitVerticalButton, tr( "Split this container vertically." ) );

  pm = new QPixmap( x_xpm );
  pm->setMask(pm->createHeuristicMask());
  deleteButton = ltb->addButton(pm);
  connect( deleteButton, SIGNAL( clicked() ), this, SLOT( setThenRemoveLastPanelContainer() ) );
  QToolTip::add( deleteButton, tr( "Remove the current tab (Panel)." ) );

//  ltb->resize(18*3,16);
  ltb->show();
}

  // This is only used to hold panels... not PanelContainers.
  dropSiteLayoutParent = new QWidget( leftFrame, "dropSiteLayoutParent" );
dropSiteLayoutParent->setMinimumSize( QSize(10,10) );
  if( debug(DEBUG_FRAMES) ) dropSiteLayoutParent->setBackgroundColor("blue");
  strcpy(cn,"dropSiteLayoutParent:");strcat(cn, internal_name.ascii());strcat(cn,"-");strcat(cn,external_name.ascii()); dropSiteLayoutParent->setCaption(cn);

  dropSiteLayout = new QVBoxLayout( dropSiteLayoutParent, 0, 0, "dropSiteLayout");


  tabWidget = new TabWidget( this, dropSiteLayoutParent, "tabWidget" );
tabWidget->setMinimumSize( QSize(10,10) );
  if( debug(DEBUG_FRAMES) ) tabWidget->setBackgroundColor("orange");
  strcpy(cn,"tabWidget:");strcat(cn, internal_name.ascii());strcat(cn,"-");strcat(cn,external_name.ascii()); tabWidget->setCaption(cn);

  tabBarWidget = new TabBarWidget( this, dropSiteLayoutParent, "tabBarWidget");
  if( debug(DEBUG_FRAMES) ) tabBarWidget->setBackgroundColor("pink");
  strcpy(cn,"tabBarWidget:");strcat(cn, internal_name.ascii());strcat(cn,"-");strcat(cn,external_name.ascii()); tabBarWidget->setCaption(cn);

  tabWidget->setTabBar(tabBarWidget);

  dropSiteLayout->addWidget( tabWidget );
  dropSiteLayoutParent->hide();

  // Create a frame for the right side, but don't show it... yet.
  rightFrame = new Frame(this, splitter, "right_frame");
rightFrame->setMinimumSize( QSize(10,10) );
  if( debug(DEBUG_FRAMES) ) rightFrame->setBackgroundColor("green");


  rightFrame->setDragEnabled(TRUE);
  rightFrame->hide();

  // Add the splitter to the the layout box.
  panelContainerFrameLayout->addWidget( splitter );
  splitter->show(); // Yes this is needed... Otherwise subsequent splits don't 
                    // show this splitter, but only the menu.

  languageChange();
  resize( QSize(542, 300).expandedTo(minimumSizeHint()) );

  // signals and slots connections

  getMasterPC()->_panel_container_count++;

}

/*! Currently those allocated resources are:
    - The global PanelContainerList (panelContainerList)
    - The list of Panels in this PanelContainer (panelList)
    - The PanelContainer Layout widget (panelContainerFrameLayout)
    - The PanelContainer name (name)
    .
*/
PanelContainer::~PanelContainer()
{
  nprintf(DEBUG_CONST_DESTRUCT) ("->>>> 0x%x   PanelContainer::~PanelContainer(%s-%s) destructor called.\n", this, getInternalName().ascii(), getExternalName().ascii() );

  menuEnabled = FALSE;

  _masterPanelContainerList->remove(this);

  if( !panelList.empty() )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("We have a problem.  panelList is not empty.\n");
  }
  panelList.clear();

  if( rightPanelContainer )
  {
    rightPanelContainer->hide();
  }
  if( leftPanelContainer )
  {
    leftPanelContainer->hide();
  }

  splitter->hide();
  leftFrame->hide();
  rightFrame->hide();



#ifdef PULL
  delete tabBarWidget;
  delete tabWidget;
  delete dropSiteLayout;
  delete dropSiteLayoutParent;
  delete rightFrame;
  delete leftFrame;
  delete splitter;
#endif // PULL

  delete( panelContainerFrameLayout );

  if( !internal_name.isEmpty() )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("delete %s", getInternalName().ascii() );
    internal_name = QString::null;
  }
  if( !external_name.isEmpty() )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("  -%s\n", getExternalName().ascii() );
    external_name = QString::null;
  }
}

/*!  Sets the strings of the subwidgets using the current language.
 */
void
PanelContainer::languageChange()
{
  char name_buffer[1024];
  name_buffer[0] = '\0';
  strcpy(name_buffer, external_name.ascii());
  strcat(name_buffer, "-");
  strcat(name_buffer, internal_name.ascii());
  setCaption( tr( name_buffer ) );
}

/*! Split 'this' PanelContainer base on the orientation.    If the 
    showRight flag is set to TRUE show() both the left and right 
    PanelContainers.   Otherwise, only show the left.
 */
void
PanelContainer::split(Orientation orientation, bool showRight, int leftSidePercent)
{
  // We've received a request to split the panelContainer.
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::split(%s-%s)\n", getInternalName().ascii(), getExternalName().ascii() );

  menuEnabled = FALSE;

  if( splitter == NULL )
  {
    fprintf(stderr, "Warning: Unable to split panel container.\n");
    return;
  }

  // This is the PC that, if it has any Panels, will need to have them
  // moved.
  sourcePC = this;

  // Before we even start, make sure this wasn't already set up...
  if( leftPanelContainer == NULL )
  {
    leftPanelContainer = createPanelContainer( leftFrame,
                              "leftPanelContainer", this, 
                              _masterPanelContainerList);
  } 
  if( rightPanelContainer == NULL )
  {
    rightPanelContainer = createPanelContainer( rightFrame,
                              "rightPanelContainer", this, 
                              _masterPanelContainerList);
  }

  splitter->setOrientation( orientation );

  // Resize on split ...
  // Get the size of the parent so we can split up the real estate.
  int width = this->width();
  int height = this->height();
  nprintf(DEBUG_PANELCONTAINERS) ("::split: The parent's: width=%d height=%d\n", width, height);
  QValueList<int> sizeList;
  sizeList.clear();
  int left_side_size = 0;
  int right_side_size = 0;
  if( orientation == QSplitter::Vertical )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("	split try to resize top to w=%d h=%d p=%d\n", width, height/2, leftSidePercent);
    if( leftSidePercent > -1 )
    {
      left_side_size = (int)(height*(leftSidePercent*.01));
      right_side_size = height-left_side_size;
    } else
    {
      right_side_size = left_side_size = (int)(height/2);
    }
  } else // Horizontal
  {
    nprintf(DEBUG_PANELCONTAINERS) ("	split try to resize left to w=%d h=%d p=%d\n", width/2, height, leftSidePercent);
    if( leftSidePercent > -1 )
    {
      left_side_size = (int)(width*(leftSidePercent*.01));
      right_side_size = width-left_side_size;
    } else
    {
      right_side_size = left_side_size = (int)(width/2);
    }
  }
  sizeList.push_back(left_side_size);
  sizeList.push_back(right_side_size);

  splitter->setSizes(sizeList);

  leftPanelContainer->show();
  leftPanelContainer->leftFrame->show();

  if( showRight == TRUE )
  {
    rightPanelContainer->leftFrame->show();
    rightPanelContainer->show();
  }

  // Now get the right...
  if( showRight == TRUE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("showing all the right information (showRight == TRUE)\n");

    // Turn off the parent menu for the panel container
    menuEnabled = FALSE;

    leftPanelContainer->show();
    leftPanelContainer->menuEnabled = TRUE;

    rightFrame->show();
    rightPanelContainer->show();
    rightPanelContainer->menuEnabled = TRUE;
  } else
  {
    // Don't show anything right, but make sure the parent menu is available
    rightFrame->hide();
    menuEnabled = FALSE;
    rightPanelContainer->hide();
  }

  if( showRight == FALSE )
  {
    // Make sure this is really kept hidden as we're only interested in the
    // left side at this point.  (That is, the split was caused by a new
    // panel being added to this panel container.
    menuEnabled = TRUE;
    leftPanelContainer->menuEnabled = FALSE;
    rightPanelContainer->menuEnabled = FALSE;
    rightFrame->hide();
  } else
  {
    // Disable the parent PanelContainer, show the left and right and make 
    // sure the left and right's menus are enabled.
    menuEnabled = FALSE;
    rightPanelContainer->menuEnabled = TRUE;
    leftPanelContainer->menuEnabled = TRUE;
    rightFrame->show();
    leftFrame->show();
  }


  // Now, if there was a widget to reparent, we've split everything, now
  // reparent the widget into the new left side panel container's drop site.

  // During the usability 
  // studies (phaseI) many people suggested that when splitting and 
  // when mulitiple panels are in the split panel container that we should
  // divide the panels up.    So, keep the focused panel in the left/top
  // and move the remaining panels to the right/bottom panel container.
  if( sourcePC->areTherePanels() )
  {
    if( sourcePC->panelList.count() > 1 && sourcePC->tabWidget->count() > 1 ) 
    {
      rightPanelContainer->movePanelsToNewPanelContainer( sourcePC );
      leftPanelContainer->moveCurrentPanelToNewPanelContainer( rightPanelContainer );
    } else
    {
      leftPanelContainer->movePanelsToNewPanelContainer( sourcePC );
    }
  }
}

/*! This routine is called when a PanelContainer's menu is requested.
    It passes the contextMenu through to the raised panel (if there is
    one).   The panel's have a virtual function (Panel::menu) that if
    implemented by the Panel developer, will append any menu items 
    to the PanelContainer menu.
 */
bool
PanelContainer::addPanelMenuItems(QPopupMenu* contextMenu)
{
  Panel *p = getRaisedPanel();
  if( p )
  {
    return( p->menu(contextMenu) );
  }
  return( FALSE );
}

/*! Set the orientation to QSplitter::Horizontal in the call to split. */
void
PanelContainer::splitHorizontal(int leftSidePercent)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::splitHorizontal()\n");

  // We've received a request to split the panelContainer horizontally.
  split( QSplitter::Horizontal, TRUE, leftSidePercent );
}

/*! Set the orientation to QSplitter::Vertical in the call to split. */
void
PanelContainer::splitVertical(int leftSidePercent )
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::splitVertical()\n");

  // We've received a request to split the panelContainer vertically.
  split( QSplitter::Vertical, TRUE, leftSidePercent );
}

/*! If there are panels in this PanelContainer, determine which panel
   is on top and return a pointer to it.
 */
Panel *
PanelContainer::getRaisedPanel()
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::getRaisedPanel(%s-%s) entered.\n", getInternalName().ascii(), getExternalName().ascii() );
  Panel *p = NULL;

  if( !areTherePanels() )
  {
    return p;
  }

  if( tabWidget != NULL )
  {
    int currentPageIndex = tabWidget->currentPageIndex();
    nprintf(DEBUG_PANELCONTAINERS) ("attempt to determine currentPageIndex=%d\n", currentPageIndex );
    int i=0;
    for( PanelList::Iterator it = panelList.begin();
             it != panelList.end();
             ++it )
    {
      p = (Panel *)*it;
      int indexOf = tabWidget->indexOf(p->getBaseWidgetFrame());
      if( indexOf == -1 )
      {
        continue;
      }
      if( indexOf == currentPageIndex )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("found the raised panel (%s)\n", p->getName() );
        return p;
      }
    }
  }

  return p; 
}

#ifdef OLD_DRAG_AND_DROP
// This routine is called when it's determined we're dragging a panel.
// If so, we create a static DragNDropPanel object that contains all the
// needed information to drag a panel.
// It also grabs the mouse.   This must be released.  See
// PanelContainer::mouseReleaseEvent() for the release.

void PanelContainer::mouseReleaseEvent(QMouseEvent *e)
{   
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::mouseRelease()\n");

  if( Frame::dragging == TRUE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("do the drop.\n");

    DragNDropPanel::sourceDragNDropObject->DropPanel(this);
  }
 
  releaseMouse();

  // Change the cursor... back
  nprintf(DEBUG_PANELCONTAINERS) (" change the cursor back\n");
  Frame::dragging = FALSE;
  QApplication::restoreOverrideCursor();
}
#endif // OLD_DRAG_AND_DROP


#ifndef OLD_DRAG_AND_DROP
#include <qbitmap.h>
#include "drag.xpm"
/*
static const char *drag_xpm[]={
"16 16 2 1",
"# c #000000",
". c #ffffff",
"..####..........",
".#....#.........",
"#......#########",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"#..............#",
"################"};
*/

/*! This routine is called upon completion of the drag event.   
    The exception to a simple drag is that we want to be able to drop
    the Panel on a desktop as well.   So if the Qt Drag-n-drop fails
    we roll over to an internal lookup to see if we're able to drop it
    somewhere outside the tool.    
*/
void
PanelContainer::dragRaisedPanel()
{
  nprintf(DEBUG_DND) ("PanelContainer::dragRaisedPanel(%s) entered.\n", getInternalName().ascii() );

  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel((char *) "OpenSpeedShop-Drag-N-Drop-Event", this, leftFrame);
  QDragObject *d = (QDragObject *)DragNDropPanel::sourceDragNDropObject;

  QPixmap drag_pm(drag_xpm);
  drag_pm.setMask(drag_pm.createHeuristicMask());
  d->setPixmap(drag_pm);
  d->dragMove();
  // do NOT delete d (?)

  nprintf(DEBUG_DND) ("drag completed... was it successful= (0x%x)\n", d->target() );
  if( d->target() == NULL )
  {
  nprintf(DEBUG_DND) ("DragNDropPanel::sourceDragNDropObject= (0x%x)\n", DragNDropPanel::sourceDragNDropObject );
    DragNDropPanel::sourceDragNDropObject->DropPanel(this);
  } else
  {
  nprintf(DEBUG_DND) ("DragNDropPanel::sourceDragNDropObject= d->target() was not null.\n");
  }

  if( DragNDropPanel::sourceDragNDropObject == NULL )
  {
    Frame::dragging = FALSE;
  }
  nprintf(DEBUG_DND) ("DragNDropPanel::dragRaisedPanel() returning\n");
}
#else // OLD_DRAG_AND_DROP
void
PanelContainer::dragRaisedPanel()
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::dragRaisedPanel(%s) entered.\n", getInternalName().ascii() );

  // First find the associated Frame.
  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel("OpenSpeedShop-Drag-N-Drop-Event", this, leftFrame);
  if( DragNDropPanel::sourceDragNDropObject == NULL )
  {
    Frame::dragging = FALSE;
  }

  grabMouse();
}

#endif // OLD_DRAG_AND_DROP




/*! A simple intermediate call to move panels from one PanelContainer
    to another. 
    \note  See PanelContainer::movePanelsToNewPanelContainer() for details.
  */
#include "MessageObject.hxx"
void
PanelContainer::reparentPCPanels(PanelContainer *tPC, PanelContainer *fPC)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::reparentPCPanels(%s-%s, %s-%s)\n",
    tPC->getInternalName().ascii(), tPC->getExternalName().ascii(), fPC->getInternalName().ascii(), fPC->getExternalName().ascii() );

  sourcePC = fPC;
  tPC->movePanelsToNewPanelContainer( sourcePC );
}

/*! Find nearest Panel by name.
*/
Panel *
PanelContainer::findNamedPanel(PanelContainer *start_pc, char *panel_name)
{
  Panel *foundPanel = NULL;
  int return_value = 0; // unused

#ifdef DEBUG_PContainer
  printf("PanelContainer::findNamedPanel, start_pc=%d, panel_name=%s\n", start_pc, panel_name );
#endif
  MessageObject *msg = new MessageObject(panel_name);
  foundPanel = findNearestInterestedPanel(start_pc, (char *)msg, &return_value);

  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    PanelContainer *pc = (PanelContainer *)*it;
    foundPanel = findNearestInterestedPanel(pc, (char *)msg, &return_value);
    if( foundPanel )
    {
      break;
    }
  }

  delete msg;

  return foundPanel;
}

/*! Return a list of all panels with the same id.
*/
PanelList *
PanelContainer::getPanelListByID(int id)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::getPanelListByID(%d) entered\n", id);

  PanelList *panelList = new PanelList();
  panelList->clear();

  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    for( PanelList::Iterator it = pc->panelList.begin();
             it != pc->panelList.end();
             ++it )
    {
      Panel *p = (Panel *)*it;
      if( p->groupID == id )
      {
        panelList->push_back(p);
      }
    }
  }

  if( panelList->count() == 0 )
  {
    delete panelList;
    panelList = NULL;
  }

  return panelList;
}


/*! Look for PanelContainer based on it's internal name.
    Find a named PanelContainer, somewhere in the master list of
    PanelContainers.  If no named panel is found, return a default
    PanelContainer.
 */
PanelContainer *
PanelContainer::findInternalNamedPanelContainer(char *panel_container_name)
{
  nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer(%s) entered\n", panel_container_name );

  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer: is (%s) (%s)\n",
      pc->getInternalName().ascii(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getInternalName().ascii()) == 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer(%s): found one\n", pc->getInternalName().ascii() );
      return pc;
    }
  }

  // Didn't find the named panel container.   Try to find an empty panel
  // container to put this..
  
  pc = findFirstEmptyPanelContainer(getMasterPC());
  nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer(%s) says drop it in %s instead.\n",
    panel_container_name, pc->getInternalName().ascii() );

  return( pc );
} 

/*! Given a start PanelContainer, try to find the nearest best PanelContainer
    that could be used to place a panel.
 */
PanelContainer *
PanelContainer::findBestFitPanelContainer(PanelContainer *start_pc)
{
  nprintf(DEBUG_PANELCONTAINERS) ("findBestFitPanelContainer() entered\n");
  PanelContainer *pc = start_pc;
  if( pc == NULL )
  {
    pc = getMasterPC();
  }

  if( start_pc == NULL )
  {
    start_pc = getMasterPC();
  }

  if( start_pc->leftPanelContainer && start_pc->rightPanelContainer )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("WARNING! You can't add this to this panelContainer!  It's split!\n");
  }

  nprintf(DEBUG_PANELCONTAINERS) ("findBestFitPanelContainer() from %s %s\n", start_pc->getInternalName().ascii(), start_pc->getExternalName().ascii() );


  PanelContainer *epc = NULL;
  // First if there are panels and the raised one has panel containers...
  // Walk that tree first.
  if( start_pc->areTherePanels() )
  {
    Panel *p = start_pc->getRaisedPanel();
    if( p && p->topLevel == TRUE )
    {
      epc = findFirstEmptyPanelContainer(p->topPC);
      if( !epc )
      {
        epc = findFirstAvailablePanelContainer(p->topPC);
      }
      return epc;
    }
  }

  // Otherwise, find the simple best fit.
  epc = findFirstEmptyPanelContainer(start_pc);
  if( !epc )
  {
    if( start_pc->areTherePanels() )
    {
      Panel *p = start_pc->getRaisedPanel();
      if( p && p->topLevel == TRUE )
      {
        epc = findFirstEmptyPanelContainer(p->topPC);
        if( !epc )
        {
          epc = findFirstAvailablePanelContainer(p->topPC);
        }
        if( epc )
        {
          return epc;
        }
      }
    } else
    {
      pc = findFirstAvailablePanelContainer(start_pc);
      return pc;
    }
  } else
  {
    pc = epc;
  }


  return( pc );
  
}


/*! This is the name the user (i.e. Panel developer named the Panel Container.)
 */
PanelContainer *
PanelContainer::findPanelContainerWithNamedPanel(char *panel_container_name)
{
  nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerWithNamedPanel(%s) entered\n", panel_container_name );
  PanelContainer *pc = NULL;
  PanelContainer *foundPC = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerWithNamedPanel: is (%s) (%s)\n",
      pc->getExternalName().ascii(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getExternalName().ascii()) == 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerWithNamedPanel(%s): found one\n", pc->getExternalName().ascii() );
      // We found one
      if( pc->getRaisedPanel() )
      { // The matching PanelContainer with a raised Panel always
        // get's priority.  NOTE: Does this need to be a toplevel panel?
        return pc;
      }
      foundPC = pc;
    }
  }

  return foundPC;
} 

/*! Give the current cursor position, return the last PanelContainer located at 
    that location.   Unless one of the found PanelContainers is itself, then
    simply return a pointer to itself.
    \par
    This is currently sub-optimal since all PanelContainers that are at that
    x,y location meet the requirements.    I was not able to simply determine
    which PanelContainer is 'on top' so currently return the last one.  
    This may or may not be the one of interest.   At some point I'd like to 
    improve this routine to return only the 'toppest' PanelContainer.
    \bug If the user peels off a Pane into it's separate PanelContainer, then 
    overlaps the new window with another PanelContainer, unexpected results
    may occur.   The return from this function will be the last PanelContainer
    created.    This should be fixed in the future.
 */
    
PanelContainer *
PanelContainer::findPanelContainerByMouseLocation()
{
  QPoint mouse_pos = QCursor::pos ();
  int mouse_x = mouse_pos.x();
  int mouse_y = mouse_pos.y();

  nprintf(DEBUG_PANELCONTAINERS) ("mouse_pos.x()=%d mouse_pos.y()=%d\n", mouse_pos.x(), mouse_pos.y() );

  PanelContainer *pc = NULL;
  PanelContainer *found_pc = NULL;

int found_count = 0;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->markedForDelete == FALSE && 
        !pc->leftPanelContainer && !pc->rightPanelContainer &&
        pc->leftFrame->isVisible() )
    {
      QPoint point = pc->leftFrame->pos();
      QPoint abs_point = pc->leftFrame->mapToGlobal(point);
      int x=abs_point.x();
      int y=abs_point.y();
      int width=pc->parent->width();
      int height=pc->parent->height();

      nprintf(DEBUG_PANELCONTAINERS) ("(%s) x=%d y=%d width=%d height=%d\n",
        pc->getInternalName().ascii(), x, y, width, height );

      if( mouse_x >= x && mouse_x <= x+width  &&
          mouse_y >= y && mouse_y <= y+height )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerByMouseLocation(%s-%s): found one\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );

        // If one of the matches is itself, just return 'this'.
        if( pc == this )
        {
          nprintf(DEBUG_PANELCONTAINERS) ("FOUND YOURSELF!\n");
          return pc;
        }

        // Before you just return this PC, see if there's a toplevel
        // panel in this location.   If there is continue to see if
        // there's a match there..
        Panel *p = pc->getRaisedPanel();
        if( p && p->topLevel )
        {
          found_pc = pc;
        } else
        {
//          return pc;
          found_pc = pc;
        }
found_count++;
      }
    }
  }

nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerByMouseLocation() found (%d) possible candidates.\n", found_count );
nprintf(DEBUG_PANELCONTAINERS) ("  the found_pc=(%s-%s)\n", found_pc ? found_pc->getInternalName().ascii() : "", found_pc ? found_pc->getExternalName().ascii() : "" );

  return( found_pc );
} 




/*! Recursively, traverse the PanelContainer tree to find the first
    available PanelContainer that will accept a panel.
*/
PanelContainer *
PanelContainer::findFirstAvailablePanelContainer(PanelContainer *pc)
{
  PanelContainer *foundPC = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("findFirstAvailablePanelContainer: entered\n");

  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("A: just return pc\n");
    return pc;
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("try to find on down the left.\n");
    foundPC = findFirstAvailablePanelContainer(pc->leftPanelContainer);
    if( foundPC )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("found an empty PC!\n");
      return( foundPC );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("try to find on down the right.\n");
    foundPC = findFirstAvailablePanelContainer(pc->rightPanelContainer);
    if( foundPC )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("found an available PC!\n");
      return( foundPC );
    }
  }

  return NULL;
}

/*! Recursively, traverse the PanelContainer tree to find the first
    empty PanelContainer that will accept a panel.
*/
PanelContainer *
PanelContainer::findFirstEmptyPanelContainer(PanelContainer *pc)
{
  PanelContainer *foundPC = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("findFirstEmptyPanelContainer: entered\n");
  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    if( !pc->areTherePanels() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("A Found an empty space right away.  Return pc.\n");
      return pc;
    }
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("try to find on down the left.\n");
    foundPC = findFirstEmptyPanelContainer(pc->leftPanelContainer);
    if( foundPC && !foundPC->areTherePanels() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("A: found an empty PC!\n");
      return( foundPC );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("try to find on down the right.\n");
    foundPC = findFirstEmptyPanelContainer(pc->rightPanelContainer);
    if( foundPC && !foundPC->areTherePanels() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("B: found an empty PC!\n");
      return( foundPC );
    }
  }

  return NULL;
}

/*! This routine is called from the main window's File->Save menu entry.
    It will iterate over all known PanelContainers (in the master
    PanelContainer list) and save them to a file that can later be 
    read in and recreated.
 */
void
PanelContainer::savePanelContainerTree(char *fn)
{
  if( fn == NULL )
  {
    fn = (char *) ".openss.geometry";
  }
  FILE *fd = fopen(fn, "w");
  if( fd == NULL ) 
  {
    fprintf(stderr, "Unable to open file (%s) for saving session.\n", fn);
    return;
  }


  pc_rename_count = 0;
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->topLevel == TRUE )
    { // If this is a topLevel tree walk the tree.
      depth = 1;
      nprintf(DEBUG_PANELCONTAINERS) ("save pc=(%s)\n", pc->getInternalName().ascii() );
// First rename panel containers in the tree...
      max_depth = 1;
      setDepthPanelContainerTree(pc);
// Now rename the pc's in the tree...
      renamePanelContainerTree(pc);
      _saveOrderedPanelContainerTree(pc, fd);
    }
  }
 
  fclose(fd);
}


/*! In order to save the tree structure away for later reinstantiation, it's
    much easier if the tree is named and ordered correctly.  This routine
    set's the depth element of the tree.
 */
void
PanelContainer::setDepthPanelContainerTree(PanelContainer *pc)
{
  pc->_depth = depth++;
  if( pc->_depth > max_depth )
  {
    max_depth = pc->_depth;
  }

  if( pc->leftPanelContainer )
  {
    setDepthPanelContainerTree(pc->leftPanelContainer);
    depth--;
  }
  if( pc->rightPanelContainer )
  {
    setDepthPanelContainerTree(pc->rightPanelContainer);
    depth--;
  }
}

/*! Rename the PanelContainer tree base on the depth. */
void
PanelContainer::renamePanelContainerTree(PanelContainer *pc)
{
  nprintf(DEBUG_PANELCONTAINERS) ("renamePanelConantinerTree() from pc (%s)\n", pc->getInternalName().ascii() );
  for( int i = 0; i<max_depth+1; i++ )
  {
    // From the top again...  Rename sequencing off depth.
    _renamePanelContainerTree(pc, i); 
  }
}

/*! This routine is called from PanelContainer::renamePanelContainerTree().
    It's task is to recursively call itself to rename the PanelContainers.
 */
void
PanelContainer::_renamePanelContainerTree(PanelContainer *pc, int level)
{
  if( level == pc->_depth )
  {
    pc->renamePanelContainer();
  }

  if( pc->leftPanelContainer )
  {
    _renamePanelContainerTree(pc->leftPanelContainer, level);
  }
  if( pc->rightPanelContainer )
  {
    _renamePanelContainerTree(pc->rightPanelContainer, level);
  }
}

/*! This routine actually changes the internal name field in the PanelContainer.
 */
void
PanelContainer::renamePanelContainer()
{
  char tmpstr[100];

  sprintf(tmpstr, "pc:%d", pc_rename_count);
  pc_rename_count++;

  setInternalName(tmpstr);
}


/*! This routine does the actual write() call to save the PanelContainer
    tree information to a file.
 */
void
PanelContainer::savePanelContainer(int depth, FILE *fd)
{
  //extern void indentString(int indent_count, char *buffer);
  char indent_buffer[1024];
  indent_buffer[0] = '\0';


  const int BUFSIZE=100;
  char buffer[BUFSIZE];
  memset(buffer, '\0', BUFSIZE);
  
  bool SPLIT = TRUE;

  QWidget *tl = parent->topLevelWidget();


  if( leftPanelContainer && rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    sprintf(buffer, "%s %s  %d %d %d %d %d %d %d %d\n",
      getInternalName().ascii(),
      parentPanelContainer ? parentPanelContainer->getInternalName().ascii() : "toplevel",
      SPLIT, splitter->orientation(), 
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      width(), height(),
      markedForDelete, tl->x(), tl->y() );
      nprintf(DEBUG_PANELCONTAINERS) ("%s(%s) (%s) SPLIT: o=%d ls=%d w=%d h=%d mfd=%s x=%d y=%d\n",
        indent_buffer, getInternalName().ascii(),
        parentPanelContainer ? parentPanelContainer->getInternalName().ascii() : "toplevel",
        splitter->orientation(), 
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
        width(), height(),
        markedForDelete ? "mfd" : "---", tl->x(), tl->y() );

    fwrite(buffer, BUFSIZE, 1, fd);
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    sprintf(buffer, "%s %s %d %d %d %d %d %d %d %d\n",
      getInternalName().ascii(),
      parentPanelContainer ? parentPanelContainer->getInternalName().ascii() : "toplevel",
      !SPLIT, -1,
      parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      width(), height(),
      markedForDelete, tl->x(), tl->y() );
      nprintf(DEBUG_PANELCONTAINERS) ("%s(%s) (%s) NO SPLIT: ls=%d w=%d h=%d mfd=%s x=%d y=%d \n",
      indent_buffer, getInternalName().ascii(),
      parentPanelContainer ? parentPanelContainer->getInternalName().ascii() : "toplevel",
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      width(), height(),
      markedForDelete ? "mfd" : "---", tl->x(), tl->y() );
// printPanels(indent_buffer);
    fwrite(buffer, BUFSIZE, 1, fd);
  }
}

/*! This routine is called for each toplevel PanelContainer, calling yet
    another routine to traverse it's PanelContainer tree.
*/
void
PanelContainer::_saveOrderedPanelContainerTree(PanelContainer *pc, FILE *fd)
{
  nprintf(DEBUG_PANELCONTAINERS) ("_saveOrderedPanelContainerTree() from pc (%s)\n", pc->getInternalName().ascii() );
  for( int i = 0; i<max_depth+1; i++ )
  {
    // From the top again...  save by depth
    __saveOrderedPanelContainerTree(pc, fd, i); 
  }
}


/* This routine is recursively called to save the PanelContainer tree to 
   a file.
*/
void
PanelContainer::__saveOrderedPanelContainerTree(PanelContainer *pc, FILE *fd, int level)
{
  if( level == pc->_depth )
  {
    pc->savePanelContainer(level, fd);
  }

  if( pc->leftPanelContainer )
  {
    __saveOrderedPanelContainerTree(pc->leftPanelContainer, fd, level);
  }
  if( pc->rightPanelContainer )
  {
    __saveOrderedPanelContainerTree(pc->rightPanelContainer, fd, level);
  }
}


/*! The routine to add a Panel to a PanelContainer.
    It simply adds the passed Panel to the QTabWidget (tabWidget)
    raising it.   It also adds the Panel to the PanelContainers
    panelList.   ... And it gives the tab the passed in name.
*/
Panel *
PanelContainer::addPanel(Panel *p, PanelContainer *panel_container, char *tab_name)
{
  PanelContainer *start_pc = panel_container;

  if( panel_container->leftPanelContainer && panel_container->rightPanelContainer )
  {
    fprintf(stderr, tr("Internal Warning: PC:addPanel() You can't add this to this panelContainer!  It's split!\n"));
    // start_pc = panel_container->findBestFitPanelContainer(start_pc);
    return NULL;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel(%s, %s) in (%s)\n", tab_name, p->getName(), start_pc->getInternalName().ascii() );
  if( start_pc->tabWidget == NULL )
  {
    fprintf(stderr, tr("Internal Warning: addPanel.  No tabWidget\n"));
    return NULL;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() add the tab\n");
//  start_pc->tabWidget->addTab( p->getBaseWidgetFrame(), tab_name );
  
  start_pc->tabWidget->addTab( p->getBaseWidgetFrame(), p->getName() );

//  start_pc->augmentTab( p->getBaseWidgetFrame() );
  start_pc->augmentTab( p->getBaseWidgetFrame(), p );

  // Add the panel to the iterator list.   This list (panelList) aids us
  // when cleaning up PanelContainers.  We'll just loop over this list
  // deleting all the panels.
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() push_back() it!\n");
  start_pc->panelList.push_back(p);

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() setCurrentPage(%d)\n", start_pc->tabWidget->count()-1);


  // Put the newly added tab on top
  nprintf(DEBUG_PANELCONTAINERS) ("Try to put %d on top\n", start_pc->tabWidget->count()-1 );

  start_pc->tabWidget->setCurrentPage(start_pc->tabWidget->count()-1);

  start_pc->handleSizeEvent((QResizeEvent *)NULL);

  // Make it visible to the world.  At somepoint we may want to create a tab,
  // but not make it visible.  If/when that day comes, you'll need to pass in 
  // an additional parameter (showing by default) that let's one toggle this 
  // call.
  start_pc->dropSiteLayoutParent->show();
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() show() it!\n");
  start_pc->tabWidget->show();

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() returning\n");

  return( p );
}

// In this PanelContainer, find the panel with the passed in name and 
// raise it.
Panel *
PanelContainer::raiseNamedPanel(char *panel_name)
{
#ifdef DEBUG_PContainer
   printf("raiseNamedPanel (%s)\n", panel_name );
#endif
  // In this panel container... raise the panel with the matching name
  
  int count=0;
  Panel *p = NULL;
  for( PanelList::Iterator it = panelList.begin();
           it != panelList.end();
           ++it )
  {
    p = (Panel *)*it;
    if( strcmp(p->getName(), panel_name) == 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS)("Found the panel to raise. count=%d\n", count );
      int indexOf = tabWidget->indexOf(p->getBaseWidgetFrame());
      if( indexOf == -1 )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("We have an hidden index of!!!\n");
        tabWidget->insertTab(p->getBaseWidgetFrame(), p->getName(), count );
        tabWidget->showPage(p->getBaseWidgetFrame());
      }
      // We hide this if the tab count went to zero.  Make sure we re-show it.
      tabWidget->show();
      tabWidget->setCurrentPage(count);
      nprintf(DEBUG_PANELCONTAINERS) ("setCurrentPage(%d)\n", count);
      p->getPanelContainer()->handleSizeEvent(NULL);
      { // Make sure if this is a nested panel (inside a toplevel, such
        // as an experiment panel, that the entire heirarchy is raised.
      Panel *rp = raiseToTop(p);
      if( rp )
      {
        return(rp);
      }
      }
      return(p);
    }
    int isHidden = tabWidget->indexOf(p->getBaseWidgetFrame());
    if( isHidden != -1 )
    {
      count++;
    }
  }

  return(NULL);
}


// In this PanelContainer, find the passed in Panel and raise it.
Panel *
PanelContainer::raisePanel(Panel *panel)
{
  // In this panel container... raise the panel with the matching name
#ifdef DEBUG_PContainer
  printf("raisePanel (%s)\n", panel->getName() );
#endif
  
  int count=0;
  Panel *p = NULL;
  for( PanelList::Iterator it = panelList.begin();
           it != panelList.end();
           ++it )
  {
    p = (Panel *)*it;
    if( p == panel )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("raisePanel the panel to raise. count=%d\n", count );

      int indexOf = tabWidget->indexOf(p->getBaseWidgetFrame());
      if( indexOf == -1 )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("We have an hidden index of!!!\n");
        tabWidget->insertTab(p->getBaseWidgetFrame(), p->getName(), count );
        tabWidget->showPage(p->getBaseWidgetFrame());
      }
      nprintf(DEBUG_PANELCONTAINERS) ("setCurrentPage(%d)\n", count);
      tabWidget->setCurrentPage(count);
      p->getPanelContainer()->handleSizeEvent(NULL);

      { // Make sure if this is a nested panel (inside a toplevel, such
        // as an experiment panel, that the entire heirarchy is raised.
      Panel *rp = raiseToTop(p);
      if( rp )
      {
        return(rp);
      }
      }
      return(p);
    }
    int isHidden = tabWidget->indexOf(p->getBaseWidgetFrame());
    if( isHidden != -1 )
    {
      count++;
    }
  }

  return(NULL);
}

Panel * PanelContainer::raiseToTop(Panel *p)
{
  PanelContainer *pc = p->getPanelContainer();
//  debugPanelContainerTree();
  bool found = FALSE;
  PanelContainer *pcmatch = NULL;
  Panel *pmatch = NULL;
  while( pc )
  {
#ifdef DEBUG_PContainer
    printf("hmm: (%s:%s)\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
#endif
    if( pc->topLevel == TRUE )
    {
      //    printf("pc is toplevel.\n");
      // Look through all the panels, matching "pc" to p->topPC... if we find
      // a match recursively call this (raisePanel) to continue walking up the
      // tree.
      for( PanelContainerList::Iterator itmatch = _masterPanelContainerList->begin();
               itmatch != _masterPanelContainerList->end();
               itmatch++ )
      {
        pcmatch = (PanelContainer *)*itmatch;
        for( PanelList::Iterator pitmatch = pcmatch->panelList.begin();
               pitmatch != pcmatch->panelList.end(); ++pitmatch )
        {
           pmatch = (Panel *)*pitmatch;
           if( pmatch->topPC == pc )
           {
//       printf("Can you continue from here???!!\n");
             found = TRUE;
             break;
           }
        }
        if( found )
        {
          break;
        }
      }
      if( found )
      {
        pcmatch->raisePanel(pmatch);
        return(p);
      } else
      {
        return(p);
      }
    } else
    {
      pc = pc->parentPanelContainer;
    }
  }
  return(NULL);
}

/*! This recursively called routine will clean up a PanelContainer from 
     the top down.  It will delete all panels and all child PanelContainers.
 */
void
PanelContainer::removeTopLevelPanelContainer(PanelContainer *toppc, bool recursive)
{
#ifdef DEBUG_PContainer
  printf("removeTopLevelPanelContainer(%s-%s)\n", toppc->getInternalName().ascii(), toppc->getExternalName().ascii() );
#endif

  nprintf(DEBUG_PANELCONTAINERS) ("Here is the panel container to delete (%s-%s):\n", toppc->getInternalName().ascii(), toppc->getExternalName().ascii() );


  bool savedResizeEnableState = getMasterPC()->_resizeEventsEnabled;
  getMasterPC()->_resizeEventsEnabled = FALSE;

  // For each panel in the panel container, check for top levels inside of them.
  // If there are any toplevel panels, recursively call this routine, removing
  // all instances.

  if( toppc->areTherePanels() )
  {
    PanelList topLevelPanelListToDelete;
    PanelList simplePanelListToDelete;
    int i = toppc->panelList.count();
    nprintf(DEBUG_PANELCONTAINERS) ("You think there are %d panels to iterator over ...\n", i);
// printf("You think there are %d panels to iterator over ...\n", i);
    for( PanelList::Iterator pit = toppc->panelList.begin();
               pit != toppc->panelList.end(); ++pit )
    {
      Panel *p = (Panel *)*pit;
      if( p->topLevel == TRUE && p->topPC != NULL )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("Nested top pc push (%s) onto it's separate list... \n",
          p->getName() );
        topLevelPanelListToDelete.push_back(p);
      } else
      {
        nprintf(DEBUG_PANELCONTAINERS) ("We've got a simple panel (%s)\n", p->getName() );
        simplePanelListToDelete.push_back(p);
      }
    }

    if( !simplePanelListToDelete.empty() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("now iterate over the list of simple panels to delete.\n");
      for( PanelList::Iterator pit = simplePanelListToDelete.begin();
                 pit != simplePanelListToDelete.end(); ++pit )
      {
        Panel *p = (Panel *)*pit;
        nprintf(DEBUG_PANELCONTAINERS) ("remove the simple panel for (%s) in (%s-%s)\n",
          p->getName(),
          p->getPanelContainer()->getInternalName().ascii(),
          p->getPanelContainer()->getExternalName().ascii() );
        if( recursive == TRUE )
        {
          nprintf(DEBUG_PANELCONTAINERS) ("wah:C: delete (%s)\n", p->getName() );
          delete p;
        }
      }
    }
    // These are the top level panel containers we need to delete.
    if( !topLevelPanelListToDelete.empty() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("now iterate over the list of toplevel panels to delete.\n");
      for( PanelList::Iterator pit = topLevelPanelListToDelete.begin();
                 pit != topLevelPanelListToDelete.end(); ++pit )
      {
        Panel *p = (Panel *)*pit;
        nprintf(DEBUG_PANELCONTAINERS) ("do the removal of all panels for (%s) in (%s-%s)\n",
          p->getName(),
          p->getPanelContainer()->getInternalName().ascii(),
          p->getPanelContainer()->getExternalName().ascii() );
        getMasterPC()->removePanels(p->getPanelContainer());
      }
    }
  } else
  {
    if( toppc->rightPanelContainer )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("I think you want to delete right %s-%s\n", toppc->rightPanelContainer->getInternalName().ascii(), toppc->rightPanelContainer->getExternalName().ascii() );
// printf("I think you want to delete right %s-%s\n", toppc->rightPanelContainer->getInternalName().ascii(), toppc->rightPanelContainer->getExternalName().ascii() );
  
      getMasterPC()->removeTopLevelPanelContainer(toppc->rightPanelContainer, TRUE);
    } 
  
    if( toppc->leftPanelContainer )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("I think you want to delete left %s-%s\n", toppc->leftPanelContainer->getInternalName().ascii(), toppc->leftPanelContainer->getExternalName().ascii() );
// printf("I think you want to delete left %s-%s\n", toppc->leftPanelContainer->getInternalName().ascii(), toppc->leftPanelContainer->getExternalName().ascii() );
  
      getMasterPC()->removeTopLevelPanelContainer(toppc->leftPanelContainer, TRUE);
    }
  }

// This should get remove with the delet...
//  getMasterPC()->_masterPanelContainerList->remove(toppc);

  toppc->panelList.clear(); // You should have already deleted these panels.
  toppc->leftPanelContainer = NULL;
  toppc->rightPanelContainer = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("wah:DeletePC.   Delete (%s-%s)\n", toppc->getInternalName().ascii(), toppc->getExternalName().ascii() );
 
  delete toppc;

  getMasterPC()->_resizeEventsEnabled = savedResizeEnableState;

  return;
}

void
PanelContainer::deleteHiddenPanels(PanelContainer *sourcePC)
{
  PanelList panelListToDelete;
  // If the panelList is not empty, but all the panels are hidden.   Then
  // remove the hidden panels and hide the internals.
  for( PanelList::Iterator pit = sourcePC->panelList.begin();
             pit != sourcePC->panelList.end();
             ++pit )
  {
    Panel *p = (Panel *)*pit;
    int indexOf = sourcePC->tabWidget->indexOf(p->getBaseWidgetFrame());
    nprintf(DEBUG_PANELCONTAINERS) ("indexOf=%d (%s)\n", indexOf, p->getName() );
    if( indexOf == -1 )
    {
      panelListToDelete.push_back(p);
    }
  }

  nprintf(DEBUG_PANELCONTAINERS) ( "there are %d panels to delete.\n", sourcePC->panelList.count() );
  for( PanelList::Iterator pit = panelListToDelete.begin();
           pit != panelListToDelete.end();
           ++pit )
  {
    Panel *p = (Panel *)*pit;
    nprintf(DEBUG_PANELCONTAINERS) ("delete %s\n", p->getName() );
    deletePanel(p, sourcePC);
  }
  panelListToDelete.clear();
}

/*! This routine does the actual delete of the p from the panel 
    container.  User's should never call it!
 */
void
PanelContainer::deletePanel(Panel *p, PanelContainer *targetPC)
{
  if( p )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("we've got a hidden panel to delete.  delete it (%s)\n",
    p->getName() );
    targetPC->panelList.remove(p);   
    nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::deletePanel() delete %s\n", p->getName() );
    // If the panel is a toplevel, delete any panel containers it may have.
    if( p->topLevel == TRUE && p->topPC != NULL )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("whoaaa! before you get too carried away, this is a toplevel panel.\n");
      removePanelContainer(p->topPC);
      removeTopLevelPanelContainer(p->topPC, FALSE);
      nprintf(DEBUG_PANELCONTAINERS) ("wah:AA: delete (%s)\n", p->getName() );
      delete p;
    }  else
    {
      nprintf(DEBUG_PANELCONTAINERS) ("wah:BB: delete (%s)\n", p->getName() );
      delete p;
    }
  }
}

/*! This routine looks, in 'this' panel container, for the panel
    passed in and hides it.
 */
void
PanelContainer::hidePanel(Panel *targetPanel)
{
  int index = 0;
  QWidget *currentPage = NULL;
  for( PanelList::Iterator pit = panelList.begin();
             pit != panelList.end();
             ++pit )
  {
    Panel *p = (Panel *)*pit;
    if( p == targetPanel )
    {
      nprintf(DEBUG_PANELCONTAINERS)("matched a panel to remove (%s)\n", p->getName() );
      currentPage = tabWidget->page(index);
      currentPage->hide();
      tabWidget->removePage(currentPage);
// printf("tabWidget->count() = (%d)\n", tabWidget->count() );
      // Hide the tabwidget when we're finished with it.  This keeps
      // noise off the screen.
      if( tabWidget->count() == 0 )
      {
        tabWidget->hide();
        ltb->show();
      }
      break;
    }
    // Don't bump the index for hidden panels.
    int indexOf = tabWidget->indexOf(p->getBaseWidgetFrame());
    if( indexOf != -1 )
    {
      index++;
    }
  }
}

/*! If there is a Panel raised, remove it. */
#include "openspeedshop.hxx"

void
PanelContainer::removeRaisedPanel(PanelContainer *targetPC)
{
   nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removeRaisedPanel from (%s-%s) entered.\n", getInternalName().ascii(), getExternalName().ascii() );
  if( targetPC == NULL )
  {
    targetPC = getMasterPC()->_lastPC;
  } 
  nprintf(DEBUG_PANELCONTAINERS) ("targetPC = (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );
  nprintf(DEBUG_PANELCONTAINERS)("There are %d panels in this PC.\n", targetPC->panelList.count() );

  if( targetPC->tabWidget != NULL )
  {
    QWidget *currentPage = targetPC->tabWidget->currentPage();
    int currentPageIndex= targetPC->tabWidget->currentPageIndex();
    Panel *p = targetPC->getRaisedPanel();
    nprintf(DEBUG_PANELCONTAINERS)("Current page (raised tab) = %d (%s) p=(%s)\n", currentPageIndex, targetPC->tabWidget->label(currentPageIndex).ascii(), p->getName() );

   if( strcmp(p->getName(), "&Command Panel") == 0  )
   {
// printf("Attempting to remove Command Panel\n");
     OpenSpeedshop *mw = getMainWindow();
     if( mw->shuttingDown == TRUE )
     {
        PanelContainer *start_pc = findBestFitPanelContainer(getMasterPC());
        if( start_pc != targetPC )
        {
          p->getPanelContainer()->movePanel(p, currentPage, start_pc);
        }
     } else
{
  PanelContainer *start_pc = findFirstAvailablePanelContainer(getMasterPC());

// printf("start_pc() (%s-%s)\n", start_pc->getInternalName().ascii(), start_pc->getExternalName().ascii() );

  if( start_pc != p->getPanelContainer() )
  {
    p->getPanelContainer()->movePanel(p, currentPage, start_pc);
  }
}
   } else
   {
     if( currentPageIndex == -1 || p == NULL)
     {
       fprintf(stderr, "Unable to remove raised panel.\n");
       return;
     }

// printf("%d: Current page (raised tab) = %d (%s) p=(%s)\n", getMasterPC()->_eventsEnabled, currentPageIndex, targetPC->tabWidget->label(currentPageIndex).ascii(), p->getName() );
      ClosingDownObject *cdo = new ClosingDownObject();
      p->listener((char *)cdo);
   
      targetPC->tabWidget->removePage(currentPage);
      deletePanel(p, targetPC);
    }
  }

  targetPC->getMasterPC()->deleteHiddenPanels(targetPC);

  if( targetPC->panelList.empty() )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("That was the last panel.  PC is now empty of panels.\n");
    targetPC->leftFrame->show();
    targetPC->dropSiteLayoutParent->hide();
    targetPC->tabWidget->hide();
    targetPC->splitter->show();
  }
}

/*! This routine removes all the Panels from a PanelContainer.
 */
void
PanelContainer::removePanels(PanelContainer *targetPC)
{
//   nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanels (%s-%s)\n", getInternalName().ascii(), getExternalName().ascii() );
  if( targetPC == NULL )
  {
    targetPC = getMasterPC()->_lastPC;
  }
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanels from (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );

  bool savedEnableState = getMasterPC()->_eventsEnabled;
  getMasterPC()->_eventsEnabled = FALSE;

  if( targetPC->tabWidget != NULL )
  {

    int count = targetPC->tabWidget->count();
    for( int i = count; i > 0; i-- )
    {

      targetPC->tabWidget->setCurrentPage(0);
      QWidget *w = targetPC->tabWidget->currentPage();

      QString tab_label =  targetPC->tabWidget->tabLabel(w);
      nprintf(DEBUG_PANELCONTAINERS) ("removePanels() deleting tab[%d] (%s)\n", 0, tab_label.ascii() );
      
      getMasterPC()->removeRaisedPanel(targetPC);
    }
  }
  getMasterPC()->_eventsEnabled = savedEnableState;
}

void
PanelContainer::setThenRemoveLastPanelContainer() 
{
//printf("setThenRemoveLastPanelContainer() - %s:%s\n", getInternalName().ascii(), getExternalName().ascii() );
  getMasterPC()->_lastPC = this;

  // I'm not sure this is going to work...!!!!   FIX
  getMasterPC()->removePanelContainer(this);
}

void
PanelContainer::removeLastPanelContainer() 
{
// printf("removeLastPanelContainer() - %s:%s\n", getInternalName().ascii(), getExternalName().ascii() );
  removePanelContainer(getMasterPC()->_lastPC);
}

/*! Remove a PanelContainer.  It will delete any Panels 
   in the targetted PanelContainer, then reparent any Panels in the opposite
   PanelContainer.   Finally it will delete the opposite and the targetted
   PanelContainer since any panels will have been parented to the parent
   PanelContainer.
  
   This is only called from the masterPC targeting another PanelContainer.
    \note When panel containers get deep, it can be easier
    to temporarily hide them (the PanelContainer) than to delete them.
    Later, when collapsing the panel container
    tree, the panels marked for deletion are recovered.
 */
void
PanelContainer::removePanelContainer(PanelContainer *targetPC)
{
  if( targetPC == NULL )
  {
    targetPC = getMasterPC()->_lastPC;
  }

  // Prewarn all panels that they're going away...
  ClosingDownObject *cdo = new ClosingDownObject();
//  getMasterPC()->notifyAllDecendants((char *)cdo, getMasterPC()->_lastPC);   

// printf("removePanelContainer() notify all decendants that we're closing down.\n");
  getMasterPC()->notifyAllDecendants((char *)cdo, targetPC);
 

// printf("PanelContainer::removePanelContainer(%s:%s) entered\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );


  bool savedEnableState = getMasterPC()->_eventsEnabled;
  getMasterPC()->_eventsEnabled = FALSE;
  bool savedResizeEnableState = getMasterPC()->_resizeEventsEnabled;
  getMasterPC()->_resizeEventsEnabled = FALSE;

  if( this != this->getMasterPC() )
  {
    fprintf(stderr, "Warning: You should only be here when this is the master panel container.\n");
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanelContainer(%s-%s) from (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii(), getInternalName().ascii(), getExternalName().ascii() );
// printf("PanelContainer::removePanelContainer(%s-%s) from (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii(), getInternalName().ascii(), getExternalName().ascii() );
  nprintf(DEBUG_PANELCONTAINERS) ("targetPC=0x%x targetPC->parentWidget=0x%x\n", targetPC, targetPC->parentWidget() );

  PanelContainer *pcToReparent = NULL;
  PanelContainer *pcToRemove = NULL;
  PanelContainer *parentPC = targetPC->parentPanelContainer;
  enum { None, Hide_Left_SIDE, Hide_Right_SIDE } sideType;


  sideType = None;
  // First remove any panels in the panel container.
  if( targetPC->areTherePanels() )
  {
// printf("first remove the panels from this panel container.\n");
    getMasterPC()->removePanels(targetPC);
  }

  // If this is the master, be a bit more careful removing it.
  // If its the main PerformanceLeader window, consider this an exit().
  if( parentPC == NULL || targetPC->topLevel == TRUE )
  {
    if( strcmp(targetPC->getInternalName().ascii(), "pc:0") == 0 )
    {
      getMasterPC()->_eventsEnabled = savedEnableState;
      getMasterPC()->_resizeEventsEnabled = savedResizeEnableState;
      return;
    } else
    {
      if( targetPC->outsidePC == TRUE )
      {
// We only do this when this is a toplevel, "outside PC"
// printf("YOU'RE AN OUTSIDE WINDOW... remove don't forget to null the panelContainer reference!!!!\n");
  targetPC->topWidget->hide();
  targetPC->topWidget->panelContainer = NULL;

        getMasterPC()->removeTopLevelPanelContainer(targetPC, TRUE);
      }
    }
    getMasterPC()->_eventsEnabled = savedEnableState;
    getMasterPC()->_resizeEventsEnabled = savedResizeEnableState;
    return;
  }

  if( targetPC == parentPC->leftPanelContainer )
  {
    // if the pc being delete is the left, pcToReparent=parentPC->rightPC;
    pcToReparent=parentPC->rightPanelContainer;
    pcToRemove = parentPC->leftPanelContainer;
    sideType = Hide_Left_SIDE;
    nprintf(DEBUG_PANELCONTAINERS) ("removing left=(%s-%s) reparenting parentPC->rightPanelContainer\n",
      targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );
  } else if( targetPC == parentPC->rightPanelContainer )
  {
    // if the pc being delete is the right, pcToReparent=parentPC->leftPC;
    pcToReparent=parentPC->leftPanelContainer;
    pcToRemove = parentPC->rightPanelContainer;
    sideType = Hide_Right_SIDE;
    nprintf(DEBUG_PANELCONTAINERS) ("removing right=(%s-%s) reparenting parentPC->lpc=(%s-%s)\n",
      targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii(),
      pcToReparent->getInternalName().ascii(), pcToReparent->getExternalName().ascii() );
  } else
  {
    fprintf(stderr, "Warning: Unexpected error removing panel\n");
    getMasterPC()->_eventsEnabled = savedEnableState;
    return;
  }
  Orientation orientation = pcToReparent->splitter->orientation();

#ifdef DEBUG_OUTPUT_REQUESTED0
  nprintf(DEBUG_PANELCONTAINERS) ("Okay: we're here:\n");
  nprintf(DEBUG_PANELCONTAINERS) ("pcToReparent:\n");
  pcToReparent->printPanelContainer(1);
  nprintf(DEBUG_PANELCONTAINERS) ("parentPC:\n");
  parentPC->printPanelContainer(1);
  nprintf(DEBUG_PANELCONTAINERS) ("Now do the actual reparent and panelContainer reduction.\n");
#endif // DEBUG_OUTPUT_REQUESTED0

  // Disable the enter leave events
  // Now hide the parts that are going to be deleted...
  targetPC->leftFrame->hide();
  targetPC->rightFrame->hide();
  targetPC->menuEnabled = FALSE;
  targetPC->dropSiteLayoutParent->hide();
  pcToReparent->leftFrame->hide();
  pcToReparent->rightFrame->hide();
  pcToReparent->menuEnabled = FALSE;
  pcToReparent->dropSiteLayoutParent->hide();
  // End I'm confused why I need to do this...

  getMasterPC()->_resizeEventsEnabled = savedResizeEnableState;

  if( pcToReparent->leftPanelContainer &&
      pcToReparent->rightPanelContainer )
  {
    if( pcToReparent->leftPanelContainer->leftPanelContainer ||
        pcToReparent->rightPanelContainer->leftPanelContainer )
    {
      pcToRemove->hide();
      pcToRemove->menuEnabled = FALSE;

      if( sideType == Hide_Left_SIDE )
      { // If left side is to be hidden.
        nprintf(DEBUG_PANELCONTAINERS) ("Hide the left side of (%s-%s)!\n",
          parentPC->getInternalName().ascii(), parentPC->getExternalName().ascii() );
        parentPC->leftPanelContainer->markedForDelete = TRUE;
        parentPC->leftFrame->hide();
        parentPC->rightFrame->show();

        parentPC->rightPanelContainer->show();
        parentPC->rightPanelContainer->leftFrame->show();
        if( parentPC->rightPanelContainer &&
            (parentPC->rightPanelContainer->leftPanelContainer->markedForDelete == TRUE ||
             parentPC->rightPanelContainer->rightPanelContainer->markedForDelete == TRUE ) )
        {
          parentPC->rightPanelContainer->rightFrame->hide();
        } else
        {
          parentPC->rightPanelContainer->rightFrame->show();
        }

      } else
      { // If right side is to be hidden.
        nprintf(DEBUG_PANELCONTAINERS) ("Hide the right side! of (%s-%s)\n",
          parentPC->getInternalName().ascii(), parentPC->getExternalName().ascii() );
        parentPC->rightPanelContainer->markedForDelete = TRUE;
        parentPC->rightFrame->hide();
        parentPC->leftFrame->show();

        parentPC->leftPanelContainer->show();
        parentPC->leftPanelContainer->rightFrame->show();
        if( parentPC->leftPanelContainer &&
            (parentPC->leftPanelContainer->leftPanelContainer->markedForDelete == TRUE ||
             parentPC->leftPanelContainer->rightPanelContainer->markedForDelete ) )
        {
          parentPC->leftPanelContainer->leftFrame->hide();
        } else
        {
          parentPC->leftPanelContainer->leftFrame->show();
        }
      }
      // Explicit showing/hiding for parentPC,
      // parentPC->[left][right]PanelContainer, AND
      // parentPC->[left][right]PanelContainer->[left][right]PanelContainer.
      // This should really be a recursive walking of the tree, either the
      // entire tree (overkill) or from the parentPC on down.   
      //
      // This current approach should pretty much // catch all the 'reasonable'
      // panel splittings and removals.
      if( parentPC->leftPanelContainer && parentPC->rightPanelContainer )
      {
        if( parentPC->leftPanelContainer->leftPanelContainer )
        {
          if(parentPC->leftPanelContainer->leftPanelContainer->markedForDelete)
          {
           parentPC->leftPanelContainer->leftFrame->hide();
           parentPC->leftPanelContainer->rightFrame->show();
          } else if( parentPC->leftPanelContainer->rightPanelContainer->markedForDelete )
          {
            parentPC->leftPanelContainer->rightFrame->hide();
            parentPC->leftPanelContainer->leftFrame->show();
          } else
          {
            parentPC->leftPanelContainer->rightFrame->show();
            parentPC->leftPanelContainer->leftFrame->show();
          }
        }
        if( parentPC->rightPanelContainer->rightPanelContainer )
        {
          if( parentPC->rightPanelContainer->rightPanelContainer->markedForDelete )
          {
            parentPC->rightPanelContainer->rightFrame->hide();
            parentPC->rightPanelContainer->leftFrame->show();
          } else if( parentPC->rightPanelContainer->leftPanelContainer->markedForDelete )
          {
            parentPC->rightPanelContainer->leftFrame->hide();
            parentPC->rightPanelContainer->rightFrame->show();
          } else
          {
            parentPC->rightPanelContainer->leftFrame->show();
            parentPC->rightPanelContainer->rightFrame->show();
          }
        }
        if( parentPC->leftPanelContainer->markedForDelete )
        {
          parentPC->leftFrame->hide();
          parentPC->rightFrame->show();
        } else if( parentPC->rightPanelContainer->markedForDelete )
        {
          parentPC->leftFrame->show();
          parentPC->rightFrame->hide();
        } else
        {
          parentPC->leftFrame->show();
          parentPC->rightFrame->show();
        }
      }
    } else
    {
      // disable the enter leave events
      pcToReparent->leftPanelContainer->leftFrame->setPanelContainer(NULL);
      pcToReparent->leftPanelContainer->rightFrame->setPanelContainer(NULL);
      pcToReparent->leftPanelContainer->menuEnabled = FALSE;
      pcToReparent->rightPanelContainer->leftFrame->setPanelContainer(NULL);
      pcToReparent->rightPanelContainer->rightFrame->setPanelContainer(NULL);
      pcToReparent->rightPanelContainer->menuEnabled = FALSE;

      // Begin I'm confused, why I need to do this...
      // Now hide the parts that are going to be deleted...
      pcToReparent->leftPanelContainer->leftFrame->hide();
      pcToReparent->leftPanelContainer->rightFrame->hide();
      pcToReparent->leftPanelContainer->menuEnabled = FALSE;
      pcToReparent->leftPanelContainer->dropSiteLayoutParent->hide();
      pcToReparent->rightPanelContainer->leftFrame->hide();
      pcToReparent->rightPanelContainer->rightFrame->hide();
      pcToReparent->rightPanelContainer->menuEnabled = FALSE;
      pcToReparent->rightPanelContainer->dropSiteLayoutParent->hide();
      // End I'm confused why I need to do this...

      nprintf(DEBUG_PANELCONTAINERS) ("WE NEED TO COLLAPSE!\n");

      if( pcToReparent->leftPanelContainer->areTherePanels() )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("COLLAPSE: and there are panels in leftPanelContainer\n");

        reparentPCPanels(parentPC->leftPanelContainer,
                       pcToReparent->leftPanelContainer);
      } else
      {
        nprintf(DEBUG_PANELCONTAINERS) ("COLLAPSE: but no panels on leftPanelContainer\n");
      }

      if( pcToReparent->rightPanelContainer->areTherePanels() )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("COLLAPSE: and there are panels in rightPanelContainer\n");

        reparentPCPanels(parentPC->rightPanelContainer,
                       pcToReparent->rightPanelContainer);
      } else
      {
        nprintf(DEBUG_PANELCONTAINERS) ("COLLAPSE: but no panels on rightPanelContainer\n");
      }

      if( parentPC->leftPanelContainer->markedForDelete == TRUE )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("lpc marked for delete, but we can use it now.. Unmark it.\n");
        parentPC->leftPanelContainer->markedForDelete = FALSE;
        parentPC->leftFrame->show();
        parentPC->rightFrame->show();
      }
      if( parentPC->rightPanelContainer->markedForDelete == TRUE )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("rpc marked for delete, but we can use it now.. Unmark it.\n");
        parentPC->rightPanelContainer->markedForDelete = FALSE;
        parentPC->rightFrame->show();
        parentPC->leftFrame->show();
      }
  
      nprintf(DEBUG_PANELCONTAINERS) ("A: about to delete the left and right panel containers...\n");
      delete pcToReparent->leftPanelContainer;
      pcToReparent->leftPanelContainer = NULL;
      delete pcToReparent->rightPanelContainer;
      pcToReparent->rightPanelContainer = NULL;

      parentPC->menuEnabled = FALSE;
  
      parentPC->leftPanelContainer->leftFrame->show();
      parentPC->leftPanelContainer->menuEnabled = TRUE;
      
      parentPC->rightPanelContainer->leftFrame->show();
      parentPC->rightPanelContainer->menuEnabled = TRUE;
      // If there are panels here, show them...
      if( parentPC->rightPanelContainer->areTherePanels() )
      {
        parentPC->rightPanelContainer->tabWidget->show();
      }
      // If there are panels here, show them...
      if( parentPC->leftPanelContainer->areTherePanels() )
      {
        parentPC->leftPanelContainer->tabWidget->show();
      }
    }
  } else
  {
    // disable the enter leave events
    targetPC->leftFrame->setPanelContainer(NULL);
    targetPC->rightFrame->setPanelContainer(NULL);
    targetPC->menuEnabled = FALSE;
    pcToReparent->leftFrame->setPanelContainer(NULL);
    pcToReparent->rightFrame->setPanelContainer(NULL);
    pcToReparent->menuEnabled = FALSE;

    nprintf(DEBUG_PANELCONTAINERS) ("NO NEED TO COLLAPSE, just reparent.\n");

    if( pcToReparent->areTherePanels() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("NO COLLAPSE: and there are panels to reparent.\n");
      reparentPCPanels(parentPC, pcToReparent);
      parentPC->dropSiteLayoutParent->show();
    } else
    {
      nprintf(DEBUG_PANELCONTAINERS) ("NO COLLAPSE: but no panels\n");
      targetPC->dropSiteLayoutParent->hide();
    }

    parentPC->rightPanelContainer->menuEnabled = FALSE;
    parentPC->leftPanelContainer->menuEnabled = FALSE;

    nprintf(DEBUG_PANELCONTAINERS) ("B: about to delete the left and right panel containers...\n");
    delete parentPC->rightPanelContainer;
    parentPC->rightPanelContainer = NULL;
    delete parentPC->leftPanelContainer;
    parentPC->leftPanelContainer = NULL;

    parentPC->show();
    parentPC->leftFrame->show();
    parentPC->rightFrame->hide();

    // Explitly make sure the panels are exposed.
    if( parentPC->parentPanelContainer )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("A: parentPC->parentPanelContainer(%s-%s)\n",
        parentPC->parentPanelContainer->getInternalName().ascii(),
        parentPC->parentPanelContainer->getExternalName().ascii() );
      if( !parentPC->parentPanelContainer->leftPanelContainer->markedForDelete )
      {
        parentPC->parentPanelContainer->leftPanelContainer->show();
        parentPC->parentPanelContainer->leftFrame->show();
        // If there are panels here, show them...
        if( parentPC->parentPanelContainer->leftPanelContainer->areTherePanels() )
        {
          parentPC->parentPanelContainer->leftPanelContainer->tabWidget->show();
        }
      }
      if( !parentPC->parentPanelContainer->rightPanelContainer->markedForDelete )
      {
        parentPC->parentPanelContainer->rightPanelContainer->show();
        parentPC->parentPanelContainer->rightFrame->show();
        // If there are panels here, show them...
        if( parentPC->parentPanelContainer->rightPanelContainer->areTherePanels() )
        {
          parentPC->parentPanelContainer->rightPanelContainer->tabWidget->show();
        }
      }
    } else
    {
      // If there are panels here, show them...
      if( !parentPC->rightPanelContainer && !parentPC->leftPanelContainer )
      {
        if( parentPC->areTherePanels() )
        {
          parentPC->tabWidget->show();
        }
      }
    }
  }

  parentPC->splitter->setOrientation( orientation );


  parentPC->splitter->show();


  // After removing the panel container, make sure we collapse any
  // panels that are marked for delete.
  PanelContainer *recoverPC = NULL;
  if( parentPC )
  {
    if( parentPC->parentPanelContainer )
    {
      recoverPC = parentPC->parentPanelContainer;
      if( recoverPC )
      {
        recover(recoverPC);
      }
    }
  }

  nprintf(DEBUG_PANELCONTAINERS) ("returning from removePanelContainer...\n");
  getMasterPC()->_eventsEnabled = savedEnableState;
  return;
}

/*! Exit routine to recursively delete all panel containers and their
    associated panels. 
 */
void
PanelContainer::closeAllExternalPanelContainers()
{
  nprintf(DEBUG_PANELCONTAINERS) ("closeAllExternalPanelContainers() entered.\n");

  PanelContainerList topLevelPanelContainersToDeleteList;
  topLevelPanelContainersToDeleteList.clear();
 

  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->topLevel == TRUE )
    {
      // Don't close the masterPC here.  It can only be closed from
      // OpenSpeedShop::fileExit().
      if( strcmp(pc->getExternalName().ascii(),"masterPC") != 0 )
      { 
// printf("pushback pc(%s:%s)\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
        topLevelPanelContainersToDeleteList.push_back(pc);
      }
    }
  }

  if( !topLevelPanelContainersToDeleteList.empty() )
  {
    for( PanelContainerList::Iterator it = topLevelPanelContainersToDeleteList.begin();
               it != topLevelPanelContainersToDeleteList.end();
               it++ )
    {
      pc = (PanelContainer *)*it;
      nprintf(DEBUG_PANELCONTAINERS) ("attempt to close window %s-%s\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
      getMasterPC()->closeWindow(pc);
    }
  }
  nprintf(DEBUG_PANELCONTAINERS) ("Finished gracefully cleaning up and closing all toplevel pc.\n");
}

/*! Delete the empty PanelContainer and close the window. */
void
PanelContainer::closeWindow(PanelContainer *targetPC)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::closeWindow() entered\n");
  if( targetPC == NULL )
  {
    fprintf(stderr, "WARNING: PanelContainer::closeWindow() no targetPC!\n");
    targetPC = getMasterPC()->_lastPC;
  }

  nprintf(DEBUG_PANELCONTAINERS)  ("PanelContainer::closeWindow 0x%x (%s-%s)\n", targetPC, targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );

  targetPC->getMasterPC()->removePanelContainer(targetPC);
}

/*! If there are PanelContainers to recover,  (That is they were only 
  temporarily hidden.) recover them with this routine. */
void
PanelContainer::recover(PanelContainer *pc)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::recover (%s-%s)\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );

  if( pc->leftPanelContainer->markedForDelete )
  {
    // We've got a panel container marked for delete.   If the other 
    // leaf has not panel containers, we can collapse and recover.
    if( !pc->rightPanelContainer->leftPanelContainer )
    {
      removePanelContainer(pc->leftPanelContainer);
    }
  } else if( pc->rightPanelContainer->markedForDelete )
  {
    // We've got a panel container marked for delete.   If the other 
    // leaf has not panel containers, we can collapse and recover.
    if( !pc->leftPanelContainer->leftPanelContainer )
    {
      removePanelContainer(pc->rightPanelContainer);
    }
  } else 
  {
    nprintf(DEBUG_PANELCONTAINERS) ("Well, well.   I don't know why you even bothered to try to recover the panel containers.... \n");
  }
}

/*! Due to reparenting of widgets (Panels) with the PanelContainers the 
    layout widgets aren't able to handle the resize correctly.   By overriding
    the event we're able to resize the internal widgets with this routine.
 */
void
PanelContainer::handleSizeEvent(QResizeEvent *e)
{
//  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::handleSizeEvent() for %s-%s\n", getInternalName().ascii(), getExternalName().ascii() );

  if( !dropSiteLayout )
  {
    return;
  }

  if( leftPanelContainer && rightPanelContainer )
  { // This keeps the parent panel container from resizing it's inner 
    // panel containers, that are now resized correctly.
    return;
  }

  // Get new size
  int width = parent->width();
  int height = parent->height();

//  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::parent(width=%d height=%d)\n", width, height);

  // resize the Panels in this container...
  resize( width, height );
  dropSiteLayoutParent->resize( width, height );
  
//  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::handleSizeEvent() returning.\n");

  // If there's a raised panel, make sure they get notification that 
  // the panel has been resized...
  Panel *p = getRaisedPanel();
  if( p != NULL )
  {
// nprintf(DEBUG_PANELCONTAINERS) ("p->getName(%s) resize!\n", p->getName() );
    p->handleSizeEvent(e);
  }


  // Now relocate the toolbar to the right side...
//  ltb->move(width-(ltb->width()+5),3);

  // If there's no panel container that can be delete, then remove
  // the option to delete it...
  if( topLevel == TRUE )
  {
    deleteButton->hide();
  }

  return;
}

/*! A simple convienience routine to see if a panel container has panels. */
bool
PanelContainer::areTherePanels()
{
  return( !panelList.empty() );
}

/*! Simply 'new' an internal name for this PanelContainer */
void
PanelContainer::setInternalName( const char *n )
{
  internal_name = QString(n);
}

/*! Simply 'new' an external name for this PanelContainer */
/*! \note This name can (and will) change.   Do not rely on it
    as a Panel developer to stay the same.
 */
void
PanelContainer::setExternalName( const char *n )
{
  external_name = QString(n);
}

/*! This is the launching routine for Panels that create other panels.
    During the initial processing of all plugins, a list of panel types 
    was collected and registered.    When a panel requests to launch another
    panel, this routine looks through all the registered panel types.
    If/when it finds the panel type, it calls the actual entry point
    into the plugin information (PluginInfo) structure stored away at
    initialization.


    (NOTE: See SlotInfo::dynamicMenuCallback() for the hook from the
           main window's toolbar.)
*/
#include "MessageObject.hxx"
Panel *
PanelContainer::dl_create_and_add_panel(const char *panel_type, PanelContainer *targetPC, ArgumentObject *ao, const char *collector_names)
{
#ifdef DEBUG_PContainer
  printf("PanelContainer::dl_create_and_add_panel,entry\n");
  printf("PanelContainer::dl_create_and_add_panel,targetPC=%d\n", targetPC);
#endif

  // if targetPC is null, try starting from the nearest top level...
  if( !targetPC )
  {
    PanelContainer *pc = this;
    while( pc && pc->topLevel == FALSE && pc->parentPanelContainer )
    {
      pc->parentPanelContainer;
    }
#ifdef DEBUG_PContainer
    printf("PanelContainer::dl_create_and_add_panel, pc=%d\n", pc);
#endif
    if( pc )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("Nearest toplevel=(%s:%s)\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
#ifdef DEBUG_PContainer
      printf("PanelContainer::dl_create_and_add_panel(), Nearest toplevel=(%s:%s)\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
#endif
      targetPC = pc;
    }
  }

  if( getMasterPC() && getMasterPC()->_pluginRegistryList )
  {
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = getMasterPC()->_pluginRegistryList->begin();
         it != getMasterPC()->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
#ifdef DEBUG_PContainer
      printf("PanelContainer::dl_create_and_add_panel(), pi=%d, pi->panel_type=%s, panel_type=%s\n", pi, pi->panel_type, panel_type);
#endif
      if( strcmp(pi->panel_type, panel_type) == 0 )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("HE SHOOTS!   HE SCORES!\n");
#ifdef DEBUG_PContainer
        printf ("PanelContainer::dl_create_and_add_panel(), FOUND MATCHING PANEL, HE SHOOTS!   HE SCORES!\n");
#endif
        Panel *p = (*(pi->dl_create_and_add_panel))((void *)pi, targetPC, ao, collector_names);
        p->show();
#ifdef DEBUG_PContainer
        printf ("PanelContainer::dl_create_and_add_panel(), HE SHOOTS!   HE SCORES!, RETURN p=%d\n", p);
#endif
        return p;

      } else {

#ifdef DEBUG_PContainer
        printf ("PanelContainer::dl_create_and_add_panel(), CAN NOT FIND MATCHING PANEL, HE DOESNT SHOOT OR SCORE THIS TIME\n");
#endif

      }
    }
  }
  return NULL;   // No panel was found to launch.
}

/*!  This static routines is the routine that actually creates a new
     PanelContainer.
 */
#include "TopWidget.hxx"
PanelContainer *
createPanelContainer( QWidget* parent, const char* name, PanelContainer *parentPanelContainer, PanelContainerList *panelContainerList )
{
  nprintf(DEBUG_PANELCONTAINERS) ("createPanelContainer(%s) entered.\n", name);
#ifdef DEBUG_PContainer
  printf("(PanelContainer) createPanelContainer(%s) entered.\n", name);
#endif

  TopWidget *topWidget = NULL;
  if( parent == NULL )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("no parent create a toplevel\n");
#ifdef DEBUG_PContainer
    printf("(PanelContainer) no parent create a toplevel\n");
#endif
    topWidget = new TopWidget( 0, "toplevel" );
    topWidget->setCaption("topwidget");
    parent = (QWidget *)topWidget;


    topWidget->show();
  }

  PanelContainer *npc = new PanelContainer( parent, name, parentPanelContainer, panelContainerList );

  nprintf(DEBUG_PANELCONTAINERS) ("created npc=(%s) (%s)\n", npc->getInternalName().ascii(), npc->getExternalName().ascii() );

  if( strcmp("TOPLEVEL", npc->getExternalName().ascii()) == 0 )
  {
    npc->topLevel = TRUE;
  }

if( topWidget != NULL )
{
  npc->outsidePC = TRUE;
  npc->topWidget = topWidget;
}

   return( npc );
}

/*! This routine actually does the work to reparent widgets (Panels) from 
    one PanelContainer to another.   
    \par
    Given a source PanelContainer, look for Panels there and move them 
    to 'this' (targetPC) PanelContainer.
*/
void
PanelContainer::movePanelsToNewPanelContainer( PanelContainer *sourcePC)
{
  PanelContainer *targetPC = this;

  nprintf(DEBUG_PANELCONTAINERS) ("movePanelsToNewPanelContainer targetPC=(%s-%s) from (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii(), sourcePC->getInternalName().ascii(), sourcePC->getExternalName().ascii() );

  if( sourcePC == NULL || sourcePC == targetPC || 
      sourcePC->tabWidget == NULL )
  {
    return;
  }

  QWidget *currentPage = sourcePC->tabWidget->currentPage();
  Panel *p = sourcePC->getRaisedPanel();
  if( !p )
  {
//    fprintf(stderr, "Error: Couldn't locate a panel to drag.\n");
    return;
  }

  QPoint point;
  QWidget *widget_to_reparent = currentPage;
  int original_index = sourcePC->tabWidget->currentPageIndex();

  nprintf(DEBUG_PANELCONTAINERS) ("I think there are %d panels to move original_index=%d", sourcePC->tabWidget->count(), original_index );
 
  nprintf(DEBUG_PANELCONTAINERS) ("onto targetPC=(%s)\n", targetPC->getInternalName().ascii() );

  QWidget *w = targetPC->dropSiteLayoutParent;

  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
    targetPC->tabWidget = new TabWidget( targetPC, w, "tabWidget" );
    {
    char n[1024]; strcpy(n,"tabWidget:A:");strcat(n, targetPC->internal_name.ascii());strcat(n,"-");strcat(n,targetPC->external_name.ascii());
    targetPC->setCaption(n);
    }
    tabBarWidget = new TabBarWidget( targetPC, w, "tabBarWidget");
    {
    char n[1024]; strcpy(n,"tabBarWidget:");strcat(n, internal_name.ascii());strcat(n,"-");strcat(n,external_name.ascii());
    tabBarWidget->setCaption(n);
    }
    tabWidget->setTabBar(tabBarWidget);
  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

  // Just to make sure the targetPC has an empty panelList
  targetPC->panelList.clear();

  if( !sourcePC->panelList.empty() )
  {
    // Currently we delete all hidden panels when we do a split.  That's 
    // likely sub-marvelous, but that's the current approach.    It's likely
    // better to move these as well, and keep them hidden.  i.e. Write a 
    // moveHiddenPanels() routine...
    targetPC->getMasterPC()->deleteHiddenPanels(sourcePC);

    int i = 0;
    nprintf(DEBUG_PANELCONTAINERS) ("we're going to try to move these panels:\n");
    for( PanelList::Iterator pit = sourcePC->panelList.begin();
             pit != sourcePC->panelList.end();
             ++pit )
    {
      Panel *p = (Panel *)*pit;
      nprintf(DEBUG_PANELCONTAINERS) ("  try to move p (%s)\n", p->getName() );
      widget_to_reparent = currentPage = sourcePC->tabWidget->page(i);
      nprintf(DEBUG_PANELCONTAINERS) ("widget_to_reparent=0x%x\n", widget_to_reparent );
      QWidget *panel_base = (QWidget *)p;
      p->setPanelContainer(targetPC);
      p->getBaseWidgetFrame()->setPanelContainer(targetPC);
      p->getBaseWidgetFrame()->reparent(targetPC->dropSiteLayoutParent, 0,
                                   point, TRUE);
      panel_base->reparent((QWidget *)targetPC, 0, point, TRUE);
      widget_to_reparent->reparent(targetPC->tabWidget, 0, point, TRUE);
      targetPC->tabWidget->addTab( currentPage, p->getName() );
//      targetPC->augmentTab(currentPage);
     targetPC->augmentTab(currentPage, p);

      TabBarWidget *tbw = (TabBarWidget *)targetPC->tabWidget->tabBar();
      tbw->setPanelContainer(targetPC);
      targetPC->panelList.push_back(p);
      p->getBaseWidgetFrame()->show();
      targetPC->dropSiteLayoutParent->show();
      sourcePC->tabWidget->removePage(currentPage);
    }
    sourcePC->dropSiteLayoutParent->hide();
    targetPC->handleSizeEvent(NULL);
  }

  int count = targetPC->tabWidget->count();
  nprintf(DEBUG_PANELCONTAINERS) ("we've moved %d tabs and %d panels\n", count, sourcePC->panelList.count() );
  if( count > 0 )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("try to raise (and showPage?) original_index=%d\n", original_index);
    targetPC->tabWidget->setCurrentPage(original_index);
  }

  sourcePC->panelList.clear();

  nprintf(DEBUG_PANELCONTAINERS) ("\n\n\n");
}


/*! This routine actually reparents widgets (One Panel) from 
    one PanelContainer to another.   
    \par
    Given a source PanelContainer, look for the current Panel there and
    it to 'this' (targetPC) PanelContainer.
*/
void
PanelContainer::moveCurrentPanelToNewPanelContainer( PanelContainer *sourcePC )
{
  PanelContainer *targetPC = this;

  nprintf(DEBUG_PANELCONTAINERS) ("moveCurrentPanelToNewPanelContainer targetPC=(%s-%s) from (%s-%s)\n", targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii(), sourcePC->getInternalName().ascii(), sourcePC->getExternalName().ascii() );

  if( sourcePC == NULL || sourcePC == targetPC || 
      sourcePC->tabWidget == NULL )
  {
    return;
  }

  QWidget *currentPage = sourcePC->tabWidget->currentPage();
  Panel *panelToMove = sourcePC->getRaisedPanel();
  if( !panelToMove )
  {
    fprintf(stderr, "Error: Couldn't locate a panel to move.\n");
    return;
  }

  QPoint point;
  QWidget *widget_to_reparent = currentPage;
  int original_index = sourcePC->tabWidget->currentPageIndex();

  nprintf(DEBUG_PANELCONTAINERS) ("I think there are %d panels to move ", sourcePC->tabWidget->count() );
 
  nprintf(DEBUG_PANELCONTAINERS) ("onto targetPC=(%s)\n", targetPC->getInternalName().ascii() );

  QWidget *w = targetPC->dropSiteLayoutParent;

  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
    targetPC->tabWidget = new TabWidget( targetPC, w, "tabWidget" );
    {
    char n[1024]; strcpy(n,"tabWidget:A:");strcat(n, targetPC->internal_name.ascii());strcat(n,"-");strcat(n,targetPC->external_name.ascii());
    targetPC->setCaption(n);
    }
    tabBarWidget = new TabBarWidget( targetPC, w, "tabBarWidget");
    {
    char n[1024]; strcpy(n,"tabBarWidget:");strcat(n, internal_name.ascii());strcat(n,"-");strcat(n,external_name.ascii());
    tabBarWidget->setCaption(n);
    }
    tabWidget->setTabBar(tabBarWidget);
  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

  if( !sourcePC->panelList.empty() )
  {
    int i = 0;
    nprintf(DEBUG_PANELCONTAINERS) ("we're going to try to move these panels:\n");
    for( PanelList::Iterator pit = sourcePC->panelList.begin();
             pit != sourcePC->panelList.end();
             ++pit )
    {
      Panel *p = (Panel *)*pit;
      nprintf(DEBUG_PANELCONTAINERS) ("p=(%s)=0x%x panelToMove=(%s)=0x%x\n", p->getName(), p, panelToMove->getName(), panelToMove );
      if( p == panelToMove )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("  try to move p (%s)\n", p->getName() );
        // We're about to move the Panel.
        p->getPanelContainer()->movePanel(p, currentPage, targetPC);

        break; // Only move one with this routine.
      }
    
    }
  }

  int count = targetPC->tabWidget->count();
  nprintf(DEBUG_PANELCONTAINERS) ("we've moved %d panels\n", count);
  if( count > 0 )
  {
    targetPC->tabWidget->setCurrentPage(original_index);
  }

  sourcePC->panelList.remove(panelToMove);

  nprintf(DEBUG_PANELCONTAINERS) ("\n\n\n");
}

/*! This routine actually reparents widgets (One Panel) from 
    one PanelContainer to another.   
    \par
    Given a source PanelContainer, look for the current Panel there and
    it to 'this' (targetPC) PanelContainer.
*/
void
PanelContainer::movePanel( Panel *p, QWidget *currentPage, PanelContainer *targetPC )
{
  PanelContainer *sourcePC = p->getPanelContainer();
  QPoint point;
  QWidget *w = targetPC->dropSiteLayoutParent;

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::movePanel() %s From(%s-%s) to(%s-%s)\n", p->getName(), sourcePC->getInternalName().ascii(), sourcePC->getExternalName().ascii(), targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );

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

//  p->getPanelContainer()->augmentTab( currentPage );
  p->getPanelContainer()->augmentTab( currentPage, p );

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

  targetPC->getMasterPC()->deleteHiddenPanels(sourcePC);

  // If we just pulled off the last Panel from the old PanelContainer
  // hide the dropSiteLayoutParent (since it's only needed for the tabWidget
  // to show Panels.   This cleans up the look for the PanelContainer to 
  // keep it fresh and clean like a split().
  if( !sourcePC->areTherePanels() )
  {
    sourcePC->dropSiteLayoutParent->hide();
  }

  // Make sure the new PanelContainer has everything showing.
  targetPC->leftFrame->show();
  targetPC->dropSiteLayoutParent->show();
  targetPC->tabWidget->show();
  targetPC->splitter->show();
}

// Begin Debug routines
static void
indentString(int indent_count, char *buffer)
{
  while(indent_count>0)
  {
    strcat(buffer, "  ");
    indent_count--;
  }
}

/*! This routine loops through the master PanelContainer list.  For each
    toplevel PanelContainer it finds it calls another routine to 
    recursively traverse that tree, print out its information.
 */  
void
PanelContainer::traversePanelContainerTree()
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::traversePanelContainerTree() entered.\n");
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _masterPanelContainerList->begin();
               it != _masterPanelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->topLevel == TRUE )
    { // If this is a topLevel tree walk the tree.
      depth = 1;
      _traversePanelContainerTree(pc);
    }
  }
}

/*! This routine recursively calls itself to print out it's PanelContainer
    tree information.
 */
void
PanelContainer::_traversePanelContainerTree(PanelContainer *pc)
{
  pc->printPanelContainer(depth);

  depth++;
  if( pc->leftPanelContainer )
  {
    _traversePanelContainerTree(pc->leftPanelContainer);
    depth--;
  } 
  if( pc->rightPanelContainer )
  {
    _traversePanelContainerTree(pc->rightPanelContainer);
    depth--;
  }

}

/*! PrintPanelContainer() prints the PanelContainer and any Panel information.
    \note  This is for debugging only.
 */
void
PanelContainer::printPanelContainer(int depth)
{
  char indent_buffer[1024];
  indent_buffer[0] = '\0';

bool brief = TRUE;
  if( leftPanelContainer && rightPanelContainer )
  {
    indentString(depth,indent_buffer);
if( brief )
{
    printf("%s(%s-%s) o=%d mfd=%s \n",
       indent_buffer, getInternalName().ascii(), getExternalName().ascii(),
       splitter->orientation(), 
       markedForDelete ? "mfd" : "---" );
} else 
{
    printf("0x%x %s(%s-%s) o=%d w=%d h=%d mfd=%s menuEnabled=%d depth=%d\n",
       this,
       indent_buffer, getInternalName().ascii(), getExternalName().ascii(),
       splitter->orientation(), width(), height(),
       markedForDelete ? "mfd" : "---", menuEnabled, _depth );
}
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
if( brief )
{
    printf("%s(%s-%s) o=%d mfd=%s menuEnabled=%d\n",
      indent_buffer, getInternalName().ascii(), getExternalName().ascii(),
      splitter->orientation(), 
      markedForDelete ? "mfd" : "---", menuEnabled );
} else
{
    printf("0x%x %s(%s-%s) o=%d w=%d h=%d mfd=%s menuEnabled=%d depth=%d\n",
      this,
      indent_buffer, getInternalName().ascii(), getExternalName().ascii(),
      splitter->orientation(), width(), height(),
      markedForDelete ? "mfd" : "---", menuEnabled, _depth );
}
#ifdef MORE_INFO_NEEDED
    printf("%s-%s  leftFrame->panelContainer=(%s)\n",  indent_buffer,
      leftFrame->panelContainer ? leftFrame->panelContainer->getInternalName().ascii() : "aaack!", leftFrame->panelContainer ? leftFrame->panelContainer->getExternalName().ascii() : "aaack!" );
    printf("%s-%s  rightFrame->panelContainer=(%s)\n",  indent_buffer,
      rightFrame->panelContainer ? rightFrame->panelContainer->getInternalName().ascii() : "aaack!", rightFrame->panelContainer ? rightFrame->panelContainer->getExternalName().ascii() : "aaack!" );
#endif // MORE_INFO_NEEDED
    printPanels(indent_buffer);
  }
}

/*! This routine prints information about a Panel.
    \note  This is for debugging only.
 */
void
PanelContainer::printPanels(char *buffer)
{
  if( panelList.empty() )
  {
    return;
  }

  for( PanelList::Iterator it = panelList.begin();
               it != panelList.end();
               ++it )
  {
    Panel *p = (Panel *)*it;
    printf("%s    Panel (%s) pc=(%s)\n", buffer, p->getName(), p->getPanelContainer()->getInternalName().ascii() );
  }
}

#include <qobject.h>
/*! Traverse the PanelContainer tree dumping out the information about 
    all known PanelContainers and Panels. */
void
PanelContainer::debugPanelContainerTree()
{
  printf("here's the tree.\n");
  getMasterPC()->traversePanelContainerTree();
}

/*! Dump out all the widget information for the entire application. */
void
PanelContainer::debugAllWidgets()
{
  printf("here's the widgets.\n");

  QWidgetList *list = QApplication::allWidgets();

  QWidgetListIt it( *list );   // iterate over the widgets
  QWidget *w;
  while( (w = it.current() ) != 0 )
  { // for each widget...
    ++it;
    printf("w->caption(%s) (0x%x)\n", w->caption().ascii(), w );
    w->update();
  } 
  delete list;   // delete the list, not the widgets
}
// End Debug routines


/*! This routine is called when the PanelContainer menu is requested.
    If localMenu is true the PanelContainer menu will be cascaded.   Otherwise
    it will simply be inlined.
 */
void
PanelContainer::panelContainerContextMenuEvent( PanelContainer *targetPC, bool localMenu )
{
  nprintf(DEBUG_PANELCONTAINERS) ("A: PanelContainer::panelContainerContextMenuEvent(%s-%s) targetPC=(%s-%s)\n", getInternalName().ascii(), getExternalName().ascii(), targetPC->getInternalName().ascii(), targetPC->getExternalName().ascii() );

   getMasterPC()->_lastPC = targetPC;

  // One more check...  NOTE: This should be cleaned up as well.
  if( targetPC->leftPanelContainer && targetPC->rightPanelContainer )
  {  // There should be no menu action for this split panel container.  Only
     // for it's children.
     return;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("\n\n\nPanelContainer::contextMenuEvent() acting on it!\n");

  getMasterPC()->pcMenu = new QPopupMenu( this );
  {
  char n[1024]; strcpy(n,"pcMenu:");strcat(n, internal_name.ascii());strcat(n,"-");strcat(n,external_name.ascii());
  getMasterPC()->pcMenu->setCaption(n);
  }

  Q_CHECK_PTR( getMasterPC()->pcMenu );
//  getMasterPC()->pcMenu->insertItem( "Remove Raised &Panel", targetPC->getMasterPC(), SLOT(removeRaisedPanel()), CTRL+Key_P );

  QPixmap *apm = new QPixmap( hsplit_xpm );
  apm->setMask( apm->createHeuristicMask());
  QAction *qaction = new QAction( this,  "Split &Horizontal");
  qaction->addTo( pcMenu );
  qaction->setIconSet( QIconSet( *apm ) );
  qaction->setText( tr( "Split Horizontal" ) );
  connect( qaction, SIGNAL( activated() ), targetPC, SLOT( splitHorizontal() ) );
  qaction->setStatusTip( tr("Split this window (panel container) horizontally.") );

  delete apm;
  apm = new QPixmap( vsplit_xpm );
  apm->setMask( apm->createHeuristicMask());
  qaction = new QAction( this,  "Split Vertical");
  qaction->addTo( pcMenu );
  qaction->setIconSet( QIconSet( *apm ) );
  qaction->setText( tr( "Split Vertical" ) );
  connect( qaction, SIGNAL( activated() ), targetPC, SLOT( splitVertical() ) );
  qaction->setStatusTip( tr("Split this window (panel container) vertically.") );

if( targetPC->areTherePanels() )
{
  apm = new QPixmap( drag_xpm );
  apm->setMask( apm->createHeuristicMask());
  qaction = new QAction( this,  "Drag Raised Panel");
  qaction->addTo( pcMenu );
  qaction->setIconSet( QIconSet( *apm ) );
  qaction->setText( tr( "Drag Raised Panel" ) );
  connect( qaction, SIGNAL( activated() ), targetPC, SLOT( dragRaisedPanel() ) );
  qaction->setStatusTip( tr("Drag the raise panel to a new location.") );
}

  getMasterPC()->pcMenu->insertSeparator();

  delete apm;
  apm = new QPixmap( x_xpm );
  apm->setMask( apm->createHeuristicMask());
  qaction = new QAction( this,  "Remove Container");
  qaction->addTo( pcMenu );
  qaction->setIconSet( QIconSet( *apm ) );
  qaction->setText( tr( "Remove Container" ) );
  connect( qaction, SIGNAL( activated() ), targetPC->getMasterPC(), SLOT( removeLastPanelContainer() ) );
  qaction->setStatusTip( tr("Remove the panel container and all it's panels.") );

  getMasterPC()->contextMenu = NULL;
  if( localMenu == TRUE )
  {
    getMasterPC()->contextMenu = new QPopupMenu( this );
    {
    char n[1024]; strcpy(n,"contextMenu:A:");strcat(n, internal_name.ascii());strcat(n,"-");strcat(n,external_name.ascii());
//    getMasterPC()->pcMenu->setCaption(n);
    getMasterPC()->contextMenu->setCaption(n);
    }
    getMasterPC()->contextMenu->insertItem("&Panel Container Menu", getMasterPC()->pcMenu, CTRL+Key_P );
  
    // Now call the Panel's menu() function to add it's menus (if any).
    if( !targetPC->addPanelMenuItems(getMasterPC()->contextMenu) )
    {  // There were no panel menus... Show the panel Container menus
       // without cascading...
      delete getMasterPC()->contextMenu;
      getMasterPC()->contextMenu = getMasterPC()->pcMenu;
    }
  } else
  {
    getMasterPC()->contextMenu = getMasterPC()->pcMenu;
  }
  
  if( targetPC->topLevel == TRUE && targetPC != getMasterPC() /* && 
      !targetPC->areTherePanels() */ )
  {
if( targetPC->parent->isTopLevel() )
{
    getMasterPC()->pcMenu->insertItem( "&Close", targetPC->getMasterPC(), SLOT(closeWindow()), CTRL+Key_R );
}
  }

  nprintf(DEBUG_PANELCONTAINERS) ("About to popup pc contextMenu\n");
  getMasterPC()->contextMenu->exec( QCursor::pos() );

  nprintf(DEBUG_PANELCONTAINERS) ("finished with the context menu...\n");

  delete getMasterPC()->contextMenu;
}

/*! Notify all the panels in a (this) PanelContainer of a message.
 */
int
PanelContainer::notifyPC(char *msg)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyPC()\n");
  PanelContainer *pc = this;
  int return_value = 0;
  int ret_val = 0;
  Panel *p = NULL;
  for( PanelList::Iterator pit = pc->panelList.begin();
           pit != pc->panelList.end();
           ++pit )
  {
    p = (Panel *)*pit;
    ret_val = p->listener(msg);
    if( ret_val > 0 )
    {
      // At least on panel wanted to see this message.
      return_value = ret_val;
    }
  }
  return(return_value);
}

/*! The logic first looks in the current (this)
    PanelContainer, then starts look out from there.   If no near
    PanelContainers are found shortly, then message is passed to the 
    all Panels in the master PanelContainer list.
 */
Panel *
PanelContainer::notifyNearest(char *msg)
{
  PanelContainer *pc = this;
  Panel *foundPanel = NULL;
  int return_value = 0;
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyNearest()\n");

  foundPanel = wasThereAnInterestedPanel(pc, msg, &return_value );
 
  if( return_value == 0 )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("Didn't find anyone local.   Walk up and around the tree.\n");
    if( pc->parentPanelContainer )
    {
      pc = pc->parentPanelContainer;
      foundPanel = findNearestInterestedPanel(pc, msg, &return_value);
      if( foundPanel )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("Found the nearest interesting panel and delivered message.\n");
        return(foundPanel);
      } else
      {
        while( pc )
        {
          
          if( pc->areTherePanels() )
          {
            Panel *p = pc->getRaisedPanel();
            if( p && p->topLevel == TRUE )
            {
              pc = p->getPanelContainer()->parentPanelContainer;
            }
          }
  
          foundPanel = findNearestInterestedPanel(pc, msg, &return_value);
          if( foundPanel )
          {
            nprintf(DEBUG_PANELCONTAINERS) ("WEll that was a lot of work, but we found one..\n");
            return(foundPanel);
          }
          pc = pc->parentPanelContainer;
        }
      }
   }

// Well geeze boss.    I didn't find any interested panels with that
// half-hearteed effort.   Simple rifle though them all and pick the first
// one.
    Panel *p = NULL;
    for( PanelContainerList::Iterator pcit = _masterPanelContainerList->begin();
                 pcit != _masterPanelContainerList->end();
                 pcit++ )
    {
      pc = (PanelContainer *)*pcit;
      // First, for each PanelContainer, see if there's a raised panel
      // that's interested.
      p = pc->getRaisedPanel();
      if( p )
      {
        return_value = p->listener(msg);
        if( return_value > 0 )
        {
          // At least one panel wanted to see this message.
          return(p);
        }
      }
      for( PanelList::Iterator pit = pc->panelList.begin();
               pit != pc->panelList.end();
               ++pit )
      {
        p = (Panel *)*pit;
        return_value = p->listener(msg);
        if( return_value > 0 )
        {
          // At least on panel wanted to see this message.
          return(p);
        }
      }
    }
  }
  return( foundPanel );
}

/*! First check the raised Panel, then loop through all the Panels in the
    PanelContainer to see if there is a Panel interested in this message. 
    \p
    If there was a Panel interested, return the Panel's listener routines
    return value up the call chain.
 */
Panel *
PanelContainer::wasThereAnInterestedPanel(PanelContainer *pc, char *msg, int *return_value )
{
  nprintf(DEBUG_PANELCONTAINERS) ("wasThereAnInterestedPanel() entered.\n");

  Panel *p = NULL;
  // First see if there's a raised panel that's interested. (In this
  // PanelContaienr.)
  p = pc->getRaisedPanel();
  if( p )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("wasThereAnInterestedPanel() there was one raised (%s) call it's listener.\n", p->getName() );
    if( p->listener(msg) > 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("wasThereAnInterestedPanel() The listener was interested!!!! Return (%s)\n", p->getName() );
      return(p);
    }
  }

  // Then look in this PanelContainer
  nprintf(DEBUG_PANELCONTAINERS) ("wasThereAnInterestedPanel() look at the rest of the panel container (%s,%s) panels.\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
  for( PanelList::Iterator pit = pc->panelList.begin();
           pit != pc->panelList.end();
           ++pit )
  {
    p = (Panel *)*pit;
    if( p->listener(msg) > 0 )
    {
      // At least on panel wanted to see this message.
      return(p);
    }
  }

  return (Panel *)0;
}

/*! Recursively walks the PanelContainer tree looking for a Panel to field
    this message.
   \note FIX: This routine isn't even close... It's just started for the sake
   of a demo and needs to be rethought out and reengineered. 
 */
Panel *
PanelContainer::findNearestInterestedPanel(PanelContainer *pc, char *msg, int *ret_val)
{
  Panel *foundPanel = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("findNearestInterestedPanel: (%s,%s) entered\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
#ifdef DEBUG_PContainer
  printf("findNearestInterestedPanel: (%s,%s) entered\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
#endif

  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("Call wasThereAnInsteredPanel()\n");
#ifdef DEBUG_PContainer
    printf("Call wasThereAnInsteredPanel()\n");
#endif
    foundPanel = wasThereAnInterestedPanel(pc, msg, ret_val );
    if( foundPanel )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("There was!   There was a nearest intresting panel!\n");
#ifdef DEBUG_PContainer
      printf("There was!   There was a nearest intresting panel!\n");
#endif
      return foundPanel;
    }
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("call left side\n");
#ifdef DEBUG_PContainer
    printf("call left side\n");
#endif
    foundPanel = findNearestInterestedPanel(pc->leftPanelContainer, msg, ret_val);
    if( foundPanel )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("L: found an interested Panel!\n");
#ifdef DEBUG_PContainer
      printf("L: found an interested Panel!\n");
#endif
      return( foundPanel );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("call right side\n");
#ifdef DEBUG_PContainer
    printf("call right side\n");
#endif
    foundPanel = findNearestInterestedPanel(pc->rightPanelContainer, msg, ret_val);
    if( foundPanel )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("R: found an interested Panel!\n");
#ifdef DEBUG_PContainer
      printf("R: found an interested Panel!\n");
#endif
      return( foundPanel );
    }
  }

  return (Panel *)foundPanel;
}

/*! This routine is currently unimplemented.   
    The vision is to be able to send a message to a specific group of 
    Panels that belong to a named group.   The named group is already
    supported in the Panel creation, its simply not being used at this 
    time.
    \note: Currently unimplemented.
 */
int
PanelContainer::notifyGroup(char *msg, int groupID)
{
  nprintf(DEBUG_MESSAGES) ("PanelContainer::notifyGroup()\n");

  Panel *p = NULL;
  PanelContainer *pc = NULL;
  int return_value = 0;
  int ret_val = 0;
  for( PanelContainerList::Iterator pcit = _masterPanelContainerList->begin();
               pcit != _masterPanelContainerList->end();
               pcit++ )
  {
    pc = (PanelContainer *)*pcit;
    for( PanelList::Iterator pit = pc->panelList.begin();
             pit != pc->panelList.end();
             ++pit )
    {
      p = (Panel *)*pit;
      nprintf(DEBUG_MESSAGES) ("p->groupID = %d groupID = %d\n", p->groupID, groupID);
      if( p->groupID == groupID )
      {
        nprintf(DEBUG_MESSAGES) ("Send to (%s)\n", p->getName() );
        ret_val = p->listener(msg);
        if( ret_val > 0 )
        {
          // At least on panel wanted to see this message.
          return_value = ret_val;
        }
      }
    }
  }

  return return_value;
}

/*! Notify all decendents of this panel container of the message.
 */
int
PanelContainer::notifyAllDecendants(char *msg, PanelContainer *startPC)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyAllDecendents(%s-%s)\n", startPC->getInternalName().ascii(), startPC->getExternalName().ascii() );
  int return_value = 0;
   
  _notifyAllDecendants(msg, startPC);

  return(return_value);
}


/*! Recursively, traverse the PanelContainer tree passing all decendands
    the message.
*/
void
PanelContainer::_notifyAllDecendants(char *msg, PanelContainer *pc)
{
  nprintf(DEBUG_PANELCONTAINERS) ("_notifyAllDecendants: entered\n");

  if( !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("notify all panels in %s-%s\n", pc->getInternalName().ascii(), pc->getExternalName().ascii() );
    for( PanelList::Iterator pit = pc->panelList.begin();
             pit != pc->panelList.end();
             ++pit )
    {
      Panel *p = (Panel *)*pit;
      nprintf(DEBUG_PANELCONTAINERS) ("p->getName() = %s\n", p->getName() );
// printf ("p->getName() = %s\n", p->getName() );
      if( p->topLevel == TRUE )
      {
// printf ("p->getName() = %s was topLevel\n", p->getName() );
        _notifyAllDecendants(msg, p->topPC);
      }
      p->listener(msg);
    }
  }

  depth++;
  if( pc->leftPanelContainer )
  {
    _notifyAllDecendants(msg, pc->leftPanelContainer);
    depth--;
  } 
  if( pc->rightPanelContainer )
  {
    _notifyAllDecendants(msg, pc->rightPanelContainer);
    depth--;
  }
}

/*! This routine simple iterates over all the Panel in the master PanelContainer
    list passing the message to each.   It doesn't stop when any Panel
    handles the message, but rather makes sure \e every panel gets the message.
 */
int
PanelContainer::notifyAll(char *msg)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyAll()\n");
  Panel *p = NULL;
  PanelContainer *pc = NULL;
  int return_value = 0;
  int ret_val = 0;
  for( PanelContainerList::Iterator pcit = _masterPanelContainerList->begin();
               pcit != _masterPanelContainerList->end();
               pcit++ )
  {
    pc = (PanelContainer *)*pcit;
    for( PanelList::Iterator pit = pc->panelList.begin();
             pit != pc->panelList.end();
             ++pit )
    {
      p = (Panel *)*pit;
      ret_val = p->listener(msg);
      if( ret_val > 0 )
      {
        // At least on panel wanted to see this message.
        return_value = ret_val;
      }
    }
  }

  return return_value;
}


void
PanelContainer::augmentTab( QWidget *targetWidget, Panel *p, QIconSet *iconset )
{
#ifdef DEBUG_PContainer
  printf("PanelContainer::augmentTab(), p=0x%x, iconset=0x%x\n", p, iconset);
#endif
  int index_of = tabWidget->indexOf( targetWidget );
  tabWidget->setCurrentPage(index_of);
  QWidget *cp = tabWidget->currentPage();
  if( p && p->pluginInfo && p->pluginInfo->plugin_short_description ) {
    tabWidget->setTabToolTip( cp, tr(p->pluginInfo->plugin_short_description));
  } else {
    tabWidget->setTabToolTip( cp, tr("Use right mouse to get a  menu,\nleft mouse to drag."));
  }

  if( iconset != NULL ) {
    tabWidget->setTabIconSet( cp, *iconset );
  } else {

    QPixmap *apm = new QPixmap( down_triangle_boxed );

#ifdef DEBUG_PContainer
    printf("PanelContainer::augmentTab, p->groupID=(%d)\n", p->groupID );
#endif

    apm->fill( getTabColor(p->groupID) );
//    apm->fill( "Black" );
//    tabWidget->setPaletteBackgroundColor("yellow");

//    QPalette palette = tabWidget->palette();
//    palette.setColor(QColorGroup::Background, getTabColor(p->groupID) );
//    tabWidget->setPalette(palette);

//    apm->setMask( QBitmap( down_triangle_boxed_width, 
//                           down_triangle_boxed_height, 
//                           down_triangle_boxed_bits ) );

    nprintf(DEBUG_PANELCONTAINERS) ("D: create the pulldown widget.\n");

    tabWidget->setTabIconSet( cp, QIconSet( *apm ));
  }
}

QColor
PanelContainer::getTabColor(int id)
{
 // Temporary hack to return cooperative colors...
 if( id > 16 )
 {
#ifdef DEBUG_PContainer
   printf("PanelContainer::getTabColor, reached the limit of colors (16).... FIX\n");
#endif
   while(id -= 16 )
   {
     if( id < 16 )
     {
       break;
     }
   }
 }

#ifdef DEBUG_PContainer
 printf("PanelContainer::getTabColor, this=0x%x, color id=%d\n", this, id);
#endif

 switch( id )
 {
   case 0:
    return( QColor(Qt::white) );
    break;
   case 1:
    return( QColor(Qt::red) );
    break;
   case 2:
    return( QColor(Qt::blue) );
    break;
   case 3:
    return( QColor(Qt::green) );
    break;
   case 4:
    return( QColor(Qt::black) );
    break;
   case 5:
    return( QColor(Qt::magenta) );
    break;
   case 6:
    return( QColor(Qt::yellow) );
    break;
   case 7:
    return( QColor(Qt::gray) );
    break;
   case 8:
    return( QColor(Qt::cyan) );
    break;
   case 9:
    return( QColor(Qt::darkRed) );
    break;
   case 10:
    return( QColor(Qt::darkGreen) );
    break;
   case 11:
    return( QColor(Qt::darkBlue) );
    break;
   case 12:
    return( QColor(Qt::darkCyan) );
    break;
   case 13:
    return( QColor(Qt::darkMagenta) );
    break;
   case 14:
    return( QColor(Qt::darkYellow) );
    break;
   case 15:
    return( QColor(Qt::darkGray) );
    break;
   case 16:
   default:
    return( QColor(Qt::lightGray) );
    break;
 }

  return( QColor(Qt::lightGray) );
}
