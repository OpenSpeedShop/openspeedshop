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

    Author: Al Stipek   (stipek@sgi.com)
 */

#include "Panel.hxx"
#include "PanelContainer.hxx"

#include "DragNDropPanel.hxx"
#include <qdragobject.h>

#include <qcolor.h>
#include <qvariant.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qpixmap.h>
#include <TabBarWidget.hxx>
#include <qevent.h>

#include <qsizegrip.h>  // Debuggging only.

#include <qcursor.h>

#include "WhatsThis.hxx"

unsigned int __internal_debug_setting = 0x0000;  // Referenced in debug.hxx, define here.
#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 
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

/*! It should never be called and is only here for completeness.
 */
PanelContainer::PanelContainer( )
{
  nprintf(DEBUG_CONST_DESTRUCT) ("PanelContainer::PanelContainer() unused constructor called.\n");
}

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

  topLevel = FALSE;

  if( strcmp(n, "masterPC") == 0 )
  { // This is the very first panel container...   The very first time in here.
    nprintf(DEBUG_PANELCONTAINERS) ("This is the first PC created.   set the count == 0 \n");
    _doingMenuFLAG = FALSE;
    _masterPC = this;
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
      _masterPC = _parentPanelContainer->_masterPC;
    } else
    {
      PanelContainerList::Iterator it = pcl->begin();
      _masterPC = (PanelContainer *)*it;
    }
  }

  pc_rename_count = 0;
  sprintf(pc_name_count, "pc:%d", _masterPC->_panel_container_count);
  internal_name = strdup(pc_name_count);
  external_name = strdup(n);

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

  if( _masterPC->_panel_container_count == 0 )
  {
    // Make sure the panelContainerList interator list is empty.
    nprintf(DEBUG_PANELCONTAINERS) ("MAKE SURE _masterPanelContainerList empty!!!!\n");
    _masterPanelContainerList->clear();
  }
  // Add this panel container to the global list.
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::PanelContainer() add PC getInternalName()=(%s)\n",
    getInternalName() );

  _masterPanelContainerList->push_back(this);

  panelList.clear();  // Make sure the panelList interator list is empty.

  if( !internal_name ) // Set the internal name...
  {
    setInternalName(pc_name_count);
  }

  // Create an internal layout for this PanelContainer
  panelContainerFrameLayout =
    new QVBoxLayout( parent, 0, 0, "panelContainerFrameLayout"); 

  // Now create the splitter so we can put a container (frame) on the left
  // and right or top and bottom.
  splitter = new QSplitter(parent, "splitter");
  strcpy(cn,"splitter:");strcat(cn, internal_name);strcat(cn,"-");strcat(cn,external_name); splitter->setCaption(cn);
  if( debug(DEBUG_FRAMES) ) splitter->setBackgroundColor("red");

  // The orientation doesn't matter at this point... at least not yet.
  splitter->setOrientation( QSplitter::Vertical );

  // Create a frame for the left side and show it.
  sprintf(tmp_str, "originalLeftFrame for %s", getInternalName() );
  leftFrame = new Frame(this, splitter, "left_frame");
  if( debug(DEBUG_FRAMES) ) leftFrame->setBackgroundColor("white");
  leftFrame->setDragEnabled(TRUE);
  leftFrame->setDropEnabled(TRUE);

  // This is only used to hold panels... not PanelContainers.
  dropSiteLayoutParent = new QWidget( leftFrame, "dropSiteLayoutParent" );
  if( debug(DEBUG_FRAMES) ) dropSiteLayoutParent->setBackgroundColor("blue");
  strcpy(cn,"dropSiteLayoutParent:");strcat(cn, internal_name);strcat(cn,"-");strcat(cn,external_name); dropSiteLayoutParent->setCaption(cn);

  dropSiteLayout = new QVBoxLayout( dropSiteLayoutParent, 0, 0, "dropSiteLayout");

  tabWidget = new TabWidget( dropSiteLayoutParent, "tabWidget" );
  if( debug(DEBUG_FRAMES) ) tabWidget->setBackgroundColor("orange");
  strcpy(cn,"tabWidget:");strcat(cn, internal_name);strcat(cn,"-");strcat(cn,external_name); tabWidget->setCaption(cn);

  tabBarWidget = new TabBarWidget( this, dropSiteLayoutParent, "tabBarWidget");
  if( debug(DEBUG_FRAMES) ) tabBarWidget->setBackgroundColor("pink");
  strcpy(cn,"tabBarWidget:");strcat(cn, internal_name);strcat(cn,"-");strcat(cn,external_name); tabBarWidget->setCaption(cn);

  tabWidget->setTabBar(tabBarWidget);

  dropSiteLayout->addWidget( tabWidget );
  dropSiteLayoutParent->hide();

  // Create a frame for the right side, but don't show it... yet.
  rightFrame = new Frame(this, splitter, "right_frame");
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

  _masterPC->_panel_container_count++;
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
  nprintf(DEBUG_CONST_DESTRUCT) ("->>>>    PanelContainer::~PanelContainer(%s-%s) destructor called.\n",
    getInternalName(), getExternalName() );

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



  delete tabBarWidget;
  delete tabWidget;
  delete dropSiteLayout;
  delete dropSiteLayoutParent;
  delete rightFrame;
  delete leftFrame;
  delete splitter;

  delete( panelContainerFrameLayout );

  if( internal_name )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("delete %s", getInternalName() );
    delete internal_name;
    internal_name = NULL;
  }
  if( external_name )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("  -%s\n", getExternalName() );
    delete external_name;
    external_name = NULL;
  }
}

/*!  Sets the strings of the subwidgets using the current language.
 */
void
PanelContainer::languageChange()
{
  char name_buffer[1024];
  name_buffer[0] = '\0';
  strcpy(name_buffer, external_name);
  strcat(name_buffer, "-");
  strcat(name_buffer, internal_name);
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
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::split(%s-%s)\n", getInternalName(), getExternalName() );

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
  leftPanelContainer->movePanelsToNewPanelContainer( sourcePC );
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
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::getRaisedPanel(%s-%s) entered.\n", getInternalName(), getExternalName() );
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
      if( i == currentPageIndex )
      {
        p = (Panel *)*it;
        nprintf(DEBUG_PANELCONTAINERS) ("found the raised panel (%s)\n", p->getName() );
        return p;
      }
      i++;
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

/*! This routine is called upon completion of the drag event.   
    The exception to a simple drag is that we want to be able to drop
    the Panel on a desktop as well.   So if the Qt Drag-n-drop fails
    we roll over to an internal lookup to see if we're able to drop it
    somewhere outside the tool.    
*/
void
PanelContainer::dragRaisedPanel()
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::dragRaisedPanel(%s) entered.\n", getInternalName() );

  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel(this, leftFrame);

  QString s = "Open/SpeedShop-QDragString";
  QWidget *w = new QWidget(this, "dragNDropWidget");
  QDragObject *d = new QTextDrag(s, w, "QDragObject");
  QPixmap drag_pm(drag_xpm);
  drag_pm.setMask(drag_pm.createHeuristicMask());
  d->setPixmap(drag_pm);
  d->dragMove();
  // do NOT delete d (?)

  nprintf(DEBUG_PANELCONTAINERS) ("drag completed... was it successful? (0x%x)\n", d->target() );
  if( d->target() == NULL )
  {
    DragNDropPanel::sourceDragNDropObject->DropPanel(this);
  }

  if( DragNDropPanel::sourceDragNDropObject == NULL )
  {
    Frame::dragging = FALSE;
  }
}
#else // OLD_DRAG_AND_DROP
void
PanelContainer::dragRaisedPanel()
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::dragRaisedPanel(%s) entered.\n", getInternalName() );

  // First find the associated Frame.
  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel(this, leftFrame);
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
void
PanelContainer::reparentPCPanels(PanelContainer *tPC, PanelContainer *fPC)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::reparentPCPanels(%s-%s, %s-%s)\n",
    tPC->getInternalName(), tPC->getExternalName(), fPC->getInternalName(), fPC->getExternalName() );

  sourcePC = fPC;
  tPC->movePanelsToNewPanelContainer( sourcePC );
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
      pc->getInternalName(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getInternalName()) == 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer(%s): found one\n", pc->getInternalName() );
      return pc;
    }
  }

  // Didn't find the named panel container.   Try to find an empty panel
  // container to put this..
  
  pc = findFirstEmptyPanelContainer(_masterPC);
  nprintf(DEBUG_PANELCONTAINERS) ("findInternalNamedPanelContainer(%s) says drop it in %s instead.\n",
    panel_container_name, pc->getInternalName() );

  return( pc );
} 

/*! Given a start PanelContainer, try to find the nearest best PanelContainer
    that could be used to place a panel.
 */
PanelContainer *
PanelContainer::findBestFitPanelContainer(PanelContainer *start_pc)
{
  PanelContainer *pc = start_pc;
  if( pc == NULL )
  {
    pc = _masterPC;
  }

  if( start_pc == NULL )
  {
    start_pc = _masterPC;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("findBestFitPanelContainer() from %s %s\n", start_pc->getInternalName(), start_pc->getExternalName() );


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
      pc->getExternalName(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getExternalName()) == 0 )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerWithNamedPanel(%s): found one\n", pc->getExternalName() );
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
        pc->getInternalName(), x, y, width, height );

      if( mouse_x >= x && mouse_x <= x+width  &&
          mouse_y >= y && mouse_y <= y+height )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("findPanelContainerByMouseLocation(%s-%s): found one\n", pc->getInternalName(), pc->getExternalName() );

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
nprintf(DEBUG_PANELCONTAINERS) ("  the found_pc=(%s-%s)\n", found_pc->getInternalName(), found_pc->getExternalName() );

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
PanelContainer::savePanelContainerTree()
{
  char *fn = ".openss.geometry";
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
      nprintf(DEBUG_PANELCONTAINERS) ("save pc=(%s)\n", pc->getInternalName() );
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
  nprintf(DEBUG_PANELCONTAINERS) ("renamePanelConantinerTree() from pc (%s)\n", pc->getInternalName() );
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

  free(internal_name);
  free(external_name);

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
  extern void indentString(int indent_count, char *buffer);
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
      getInternalName(),
      parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
      SPLIT, splitter->orientation(), 
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      width(), height(),
      markedForDelete, tl->x(), tl->y() );
      nprintf(DEBUG_PANELCONTAINERS) ("%s(%s) (%s) SPLIT: o=%d ls=%d w=%d h=%d mfd=%s x=%d y=%d\n",
        indent_buffer, getInternalName(),
        parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
        splitter->orientation(), 
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
        width(), height(),
        markedForDelete ? "mfd" : "---", tl->x(), tl->y() );

    fwrite(buffer, BUFSIZE, 1, fd);
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    sprintf(buffer, "%s %s %d %d %d %d %d %d %d %d\n",
      getInternalName(),
      parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
      !SPLIT, -1,
      parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      width(), height(),
      markedForDelete, tl->x(), tl->y() );
      nprintf(DEBUG_PANELCONTAINERS) ("%s(%s) (%s) NO SPLIT: ls=%d w=%d h=%d mfd=%s x=%d y=%d \n",
      indent_buffer, getInternalName(),
      parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
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
  nprintf(DEBUG_PANELCONTAINERS) ("_saveOrderedPanelContainerTree() from pc (%s)\n", pc->getInternalName() );
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
  PanelContainer *pc = panel_container;

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel(%s, %s) in (%s)\n", tab_name, p->getName(), pc->getInternalName() );
  if( pc->tabWidget == NULL )
  {
    fprintf(stderr, "ERROR: addPanel.  No tabWidget\n");
    return NULL;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() add the tab\n");
  pc->tabWidget->addTab( p->getBaseWidgetFrame(), tab_name );

  // Add the panel to the iterator list.   This list (panelList) aids us
  // when cleaning up PanelContainers.  We'll just loop over this list
  // deleting all the panels.
  // Note: This needs to work with Panel->removePanel(),
  // i.e. Panel->removePanel(), will need to call:
  //  PanelContainer::removePanel(int panel_index) to have the panel and
  // it's location in the iterator list removed.
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() push_back() it!\n");
  pc->panelList.push_back(p);

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() setCurrentPage(%d)\n", pc->tabWidget->count()-1);


  // Put the newly added tab on top
  nprintf(DEBUG_PANELCONTAINERS) ("Try to put %d on top\n", pc->tabWidget->count()-1 );

  pc->tabWidget->setCurrentPage(pc->tabWidget->count()-1);

  pc->handleSizeEvent((QResizeEvent *)NULL);

  // Make it visible to the world.  At somepoint we may want to create a tab,
  // but not make it visible.  If/when that day comes, you'll need to pass in 
  // an additional parameter (showing by default) that let's one toggle this 
  // call.
  pc->dropSiteLayoutParent->show();
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() show() it!\n");
  pc->tabWidget->show();

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContinaer::addPanel() returning\n");

  return( p );
}

// In this PanelContainer, find the panel with the passed in name and 
// raise it.
Panel *
PanelContainer::raiseNamedPanel(char *panel_name)
{
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
      tabWidget->setCurrentPage(count);
      return(p);
    }
    count++;
  }

  return(NULL);
}


// In this PanelContainer, find the passed in Panel and raise it.
Panel *
PanelContainer::raisePanel(Panel *panel)
{
  // In this panel container... raise the panel with the matching name
  
  int count=0;
  Panel *p = NULL;
  for( PanelList::Iterator it = panelList.begin();
           it != panelList.end();
           ++it )
  {
    p = (Panel *)*it;
    if( p == panel )
    {
      tabWidget->setCurrentPage(count);
      return(p);
    }
    count++;
  }

  return(NULL);
}

/*! This recursively called routine will clean up a PanelContainer from 
     the top down.  It will delete all panels and all child PanelContainers.
 */
void
PanelContainer::removeTopLevelPanelContainer(PanelContainer *toppc, bool recursive)
{

  nprintf(DEBUG_PANELCONTAINERS) ("Here is the panel container to delete (%s-%s):\n", toppc->getInternalName(), toppc->getExternalName() );


  bool savedResizeEnableState = _masterPC->_resizeEventsEnabled;
  _masterPC->_resizeEventsEnabled = FALSE;

  // For each panel in the panel container, check for top levels inside of them.
  // If there are any toplevel panels, recursively call this routine, removing
  // all instances.

  if( toppc->areTherePanels() )
  {
    PanelList topLevelPanelListToDelete;
    PanelList simplePanelListToDelete;
    int i = toppc->panelList.count();
    nprintf(DEBUG_PANELCONTAINERS) ("You think there are %d panels to iterator over ...\n", i);
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
          p->getPanelContainer()->getInternalName(),
          p->getPanelContainer()->getExternalName() );
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
          p->getPanelContainer()->getInternalName(),
          p->getPanelContainer()->getExternalName() );
        _masterPC->removePanels(p->getPanelContainer());
      }
    }
  } else
  {
    if( toppc->rightPanelContainer )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("I think you want to delete right %s-%s\n", 
        toppc->rightPanelContainer->getInternalName(),
        toppc->rightPanelContainer->getExternalName() );
  
      _masterPC->removeTopLevelPanelContainer(toppc->rightPanelContainer, TRUE);
    } 
  
    if( toppc->leftPanelContainer )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("I think you want to delete left %s-%s\n", 
        toppc->leftPanelContainer->getInternalName(),
        toppc->leftPanelContainer->getExternalName() );
  
      _masterPC->removeTopLevelPanelContainer(toppc->leftPanelContainer, TRUE);
    }
  }

  _masterPC->_masterPanelContainerList->remove(toppc);
  toppc->panelList.clear(); // You should have already deleted these panels.
  toppc->leftPanelContainer = NULL;
  toppc->rightPanelContainer = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("wah:DeletePC.   Delete (%s-%s)\n", toppc->getInternalName(), toppc->getExternalName() );
 
  delete toppc;

  _masterPC->_resizeEventsEnabled = savedResizeEnableState;

  return;
}

/*! If there is a Panel raised, remove it. */
void
PanelContainer::removeRaisedPanel(PanelContainer *targetPC)
{
//   nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removeRaisedPanel from (%s-%s) entered.\n", getInternalName(), getExternalName() );
  if( targetPC == NULL )
  {
    targetPC = _masterPC->_lastPC;
  } 
  nprintf(DEBUG_PANELCONTAINERS) ("targetPC = (%s-%s)\n", targetPC->getInternalName(), targetPC->getExternalName() );

  if( targetPC->tabWidget != NULL )
  {
    QWidget *currentPage = targetPC->tabWidget->currentPage();
    int currentPageIndex= targetPC->tabWidget->currentPageIndex();
    Panel *p = targetPC->getRaisedPanel();

    if( currentPageIndex == -1 || p == NULL)
    {
      fprintf(stderr, "Unable to remove raised panel.\n");
      return;
    }

    targetPC->tabWidget->removePage(currentPage);
    if( p )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("we've got a raised panel to delete.  delete it (%s)\n",
        p->getName() );
      targetPC->panelList.remove(p);   
      nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removeRaisedPanel() delete %s\n", p->getName() );
      // If the panel is a toplevel, delete any panel containers it may have.
      if( p->topLevel == TRUE && p->topPC != NULL )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("whoaaa! before you get too carried away, this is a toplevel panel.\n");
        removePanelContainer(p->topPC);
        removeTopLevelPanelContainer(p->topPC, FALSE);
        nprintf(DEBUG_PANELCONTAINERS) ("wah:A: delete (%s)\n", p->getName() );
        delete p;
      }  else
      {
        nprintf(DEBUG_PANELCONTAINERS) ("wah:B: delete (%s)\n", p->getName() );
        delete p;
      }
    }
  }

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
//   nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanels (%s-%s)\n", getInternalName(), getExternalName() );
  if( targetPC == NULL )
  {
    targetPC = _masterPC->_lastPC;
  }
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanels from (%s-%s)\n", targetPC->getInternalName(), targetPC->getExternalName() );
 
  bool savedEnableState = _masterPC->_eventsEnabled;
  _masterPC->_eventsEnabled = FALSE;

  if( targetPC->tabWidget != NULL )
  {

    int count = targetPC->tabWidget->count();
    for( int i = count; i > 0; i-- )
    {

      targetPC->tabWidget->setCurrentPage(0);
      QWidget *w = targetPC->tabWidget->currentPage();

      QString tab_label =  targetPC->tabWidget->tabLabel(w);
      nprintf(DEBUG_PANELCONTAINERS) ("removePanels() deleting tab[%d] (%s)\n", 0, tab_label.ascii() );
      
      _masterPC->removeRaisedPanel(targetPC);
    }
  }
  _masterPC->_eventsEnabled = savedEnableState;
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
    targetPC = _masterPC->_lastPC;
  }

  bool savedEnableState = _masterPC->_eventsEnabled;
  _masterPC->_eventsEnabled = FALSE;
  bool savedResizeEnableState = _masterPC->_resizeEventsEnabled;
  _masterPC->_resizeEventsEnabled = FALSE;

  if( this != this->_masterPC )
  {
    fprintf(stderr, "Warning: You should only be here when this is the master panel container.\n");
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::removePanelContainer(%s-%s) from (%s-%s)\n", targetPC->getInternalName(), targetPC->getExternalName(), getInternalName(), getExternalName() );
  nprintf(DEBUG_PANELCONTAINERS) ("targetPC=0x%x targetPC->parentWidget=0x%x\n", targetPC, targetPC->parentWidget() );

  PanelContainer *pcToReparent = NULL;
  PanelContainer *pcToRemove = NULL;
  PanelContainer *parentPC = targetPC->parentPanelContainer;
  enum { None, Hide_Left_SIDE, Hide_Right_SIDE } sideType;


  sideType = None;
  // First remove any panels in the panel container.
  if( targetPC->areTherePanels() )
  {
    _masterPC->removePanels(targetPC);
  }

  // If this is the master, be a bit more careful removing it.
  // If its the main PerformanceLeader window, consider this an exit().
  if( parentPC == NULL || targetPC->topLevel == TRUE )
  {
    if( strcmp(targetPC->getInternalName(), "pc:0") == 0 )
    {
      _masterPC->_eventsEnabled = savedEnableState;
      _masterPC->_resizeEventsEnabled = savedResizeEnableState;
      return;
    } else
    {
//      targetPC->parent->hide();
#ifdef CAUSE_ABORT_WITH
// Performance
// Delete PC with Performance
//      nprintf(DEBUG_PANELCONTAINERS) ("We have another type of top level!  We need an explicit delete for this one.\n");
//      _masterPC->removeTopLevelPanelContainer(targetPC, TRUE);
#endif // CAUSE_ABORT_WITH
    }
    _masterPC->_eventsEnabled = savedEnableState;
    _masterPC->_resizeEventsEnabled = savedResizeEnableState;
    return;
  }

  if( targetPC == parentPC->leftPanelContainer )
  {
    // if the pc being delete is the left, pcToReparent=parentPC->rightPC;
    pcToReparent=parentPC->rightPanelContainer;
    pcToRemove = parentPC->leftPanelContainer;
    sideType = Hide_Left_SIDE;
    nprintf(DEBUG_PANELCONTAINERS) ("removing left=(%s-%s) reparenting parentPC->rightPanelContainer\n",
      targetPC->getInternalName(), targetPC->getExternalName() );
  } else if( targetPC == parentPC->rightPanelContainer )
  {
    // if the pc being delete is the right, pcToReparent=parentPC->leftPC;
    pcToReparent=parentPC->leftPanelContainer;
    pcToRemove = parentPC->rightPanelContainer;
    sideType = Hide_Right_SIDE;
    nprintf(DEBUG_PANELCONTAINERS) ("removing right=(%s-%s) reparenting parentPC->lpc=(%s-%s)\n",
      targetPC->getInternalName(), targetPC->getExternalName(),
      pcToReparent->getInternalName(), pcToReparent->getExternalName() );
  } else
  {
    fprintf(stderr, "Warning: Unexpected error removing panel\n");
    _masterPC->_eventsEnabled = savedEnableState;
    return;
  }
  Orientation orientation = pcToReparent->splitter->orientation();

#ifdef DEBUG_OUTPUT_REQUESTED
  nprintf(DEBUG_PANELCONTAINERS) ("Okay: we're here:\n");
  nprintf(DEBUG_PANELCONTAINERS) ("pcToReparent:\n");
  pcToReparent->printPanelContainer(1);
  nprintf(DEBUG_PANELCONTAINERS) ("parentPC:\n");
  parentPC->printPanelContainer(1);
  nprintf(DEBUG_PANELCONTAINERS) ("Now do the actual reparent and panelContainer reduction.\n");
#endif // DEBUG_OUTPUT_REQUESTED

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

  _masterPC->_resizeEventsEnabled = savedResizeEnableState;

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
          parentPC->getInternalName(), parentPC->getExternalName() );
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
          parentPC->getInternalName(), parentPC->getExternalName() );
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
        parentPC->parentPanelContainer->getInternalName(),
        parentPC->parentPanelContainer->getExternalName() );
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
  _masterPC->_eventsEnabled = savedEnableState;
  return;
}

/*! Delete the empty PanelContainer and close the window. */
void
PanelContainer::closeWindow(PanelContainer *targetPC)
{
  if( targetPC == NULL )
  {
    targetPC = _masterPC->_lastPC;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::closeWindow (%s-%s)\n", targetPC->getInternalName(), targetPC->getExternalName() );

  targetPC->_masterPC->removePanelContainer(targetPC);

  targetPC->parent->hide();
  delete targetPC;
}

/*! If there are PanelContainers to recover,  (That is they were only 
  temporarily hidden.) recover them with this routine. */
void
PanelContainer::recover(PanelContainer *pc)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::recover (%s-%s)\n", pc->getInternalName(), pc->getExternalName() );

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
PanelContainer::handleSizeEvent(QResizeEvent *)
{
//  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::handleSizeEvent() for %s-%s\n", getInternalName(), getExternalName() );

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
  internal_name = new char(strlen(n)+1);
  strcpy(internal_name, n);
}

/*! Simply 'new' an external name for this PanelContainer */
/*! \note This name can (and will) change.   Do not rely on it
    as a Panel developer to stay the same.
 */
void
PanelContainer::setExternalName( const char *n )
{
  external_name = new char(strlen(n)+1);
  strcpy(external_name, n);
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
Panel *
PanelContainer::dl_create_and_add_panel(char *panel_type, PanelContainer *targetPC)
{
  if( _masterPC && _masterPC->_pluginRegistryList )
  {
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = _masterPC->_pluginRegistryList->begin();
         it != _masterPC->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
      if( strcmp(pi->panel_type, panel_type) == 0 )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("HE SHOOTS!   HE SCORES!\n");
        Panel *p = (*(pi->dl_create_and_add_panel))((void *)pi, targetPC);
p->show();
        return p;
      }
    }
  }
  return NULL;   // No panel was found to launch.
}

/*!  This static routines is the routine that actually creates a new
     PanelContainer.
 */
PanelContainer *
createPanelContainer( QWidget* parent, const char* name, PanelContainer *parentPanelContainer, PanelContainerList *panelContainerList )
{


  nprintf(DEBUG_PANELCONTAINERS) ("createPanelContainer(%s) entered.\n", name);
  PanelContainer *npc = new PanelContainer( parent, name, parentPanelContainer, panelContainerList );

  nprintf(DEBUG_PANELCONTAINERS) ("created npc=(%s) (%s)\n", npc->getInternalName(), npc->getExternalName() );

  if( strcmp("TOPLEVEL", npc->getExternalName()) == 0 )
  {
    npc->topLevel = TRUE;
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

  nprintf(DEBUG_PANELCONTAINERS) ("movePanelsToNewPanelContainer targetPC=(%s-%s) from (%s-%s)\n", targetPC->getInternalName(), targetPC->getExternalName(), sourcePC->getInternalName(), sourcePC->getExternalName() );

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

  nprintf(DEBUG_PANELCONTAINERS) ("I think there are %d panels to move ", sourcePC->tabWidget->count() );
 
  nprintf(DEBUG_PANELCONTAINERS) ("onto targetPC=(%s)\n", targetPC->getInternalName() );

  QWidget *w = targetPC->dropSiteLayoutParent;

  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
    targetPC->tabWidget = new TabWidget( w, "tabWidget" );
    {
    char n[1024]; strcpy(n,"tabWidget:A:");strcat(n, targetPC->internal_name);strcat(n,"-");strcat(n,targetPC->external_name);
    targetPC->setCaption(n);
    }
    tabBarWidget = new TabBarWidget( targetPC, w, "tabBarWidget");
    {
    char n[1024]; strcpy(n,"tabBarWidget:");strcat(n, internal_name);strcat(n,"-");strcat(n,external_name);
    tabBarWidget->setCaption(n);
    }
    tabWidget->setTabBar(tabBarWidget);
  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

  // Just to make sure the targetPC has an empty panelList
  targetPC->panelList.clear();

  if( !sourcePC->panelList.empty() )
  {
    int i = 0;
    nprintf(DEBUG_PANELCONTAINERS) ("we're going to try to move these panels:\n");
    for( PanelList::Iterator pit = sourcePC->panelList.begin();
             pit != sourcePC->panelList.end();
             ++pit )
    {
      Panel *p = (Panel *)*pit;
      nprintf(DEBUG_PANELCONTAINERS) ("  try to move p (%s)\n", p->getName() );
      widget_to_reparent = currentPage = sourcePC->tabWidget->page(i);
      QWidget *panel_base = (QWidget *)p;
      p->setPanelContainer(targetPC);
      p->getBaseWidgetFrame()->setPanelContainer(targetPC);
      p->getBaseWidgetFrame()->reparent(targetPC->dropSiteLayoutParent, 0,
                                   point, TRUE);
      panel_base->reparent((QWidget *)targetPC, 0, point, TRUE);
      widget_to_reparent->reparent(targetPC->tabWidget, 0, point, TRUE);
      targetPC->tabWidget->addTab( currentPage, p->getName() );
      {
      TabBarWidget *tbw = (TabBarWidget *)targetPC->tabWidget->tabBar();
      tbw->panelContainer = targetPC;
      }
      targetPC->panelList.push_back(p);
      p->getBaseWidgetFrame()->show();
      targetPC->dropSiteLayoutParent->show();
      sourcePC->tabWidget->removePage(currentPage);
    }
    sourcePC->dropSiteLayoutParent->hide();
    targetPC->handleSizeEvent(NULL);
  }

  int count = targetPC->tabWidget->count();
  nprintf(DEBUG_PANELCONTAINERS) ("we've moved %d panels\n", count);
  if( count > 0 )
  {
    targetPC->tabWidget->setCurrentPage(original_index);
  }

  sourcePC->panelList.clear();

  nprintf(DEBUG_PANELCONTAINERS) ("\n\n\n");
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

  if( leftPanelContainer && rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    printf("%s(%s-%s) o=%d w=%d h=%d mfd=%s menuEnabled=%d depth=%d\n",
       indent_buffer, getInternalName(), getExternalName(),
       splitter->orientation(), width(), height(),
       markedForDelete ? "mfd" : "---", menuEnabled, _depth );
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    printf("%s(%s-%s) o=%d w=%d h=%d mfd=%s menuEnabled=%d depth=%d\n",
      indent_buffer, getInternalName(), getExternalName(),
      splitter->orientation(), width(), height(),
      markedForDelete ? "mfd" : "---", menuEnabled, _depth );
#ifdef MORE_INFO_NEEDED
    printf("%s-%s  leftFrame->panelContainer=(%s)\n",  indent_buffer,
      leftFrame->panelContainer ? leftFrame->panelContainer->getInternalName() : "aaack!", leftFrame->panelContainer ? leftFrame->panelContainer->getExternalName() : "aaack!" );
    printf("%s-%s  rightFrame->panelContainer=(%s)\n",  indent_buffer,
      rightFrame->panelContainer ? rightFrame->panelContainer->getInternalName() : "aaack!", rightFrame->panelContainer ? rightFrame->panelContainer->getExternalName() : "aaack!" );
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
    printf("%s    Panel (%s) pc=(%s)\n", buffer, p->getName(), p->getPanelContainer()->getInternalName() );
  }
}

#include <qobjcoll.h>
#include <qobject.h>
/*! Traverse the PanelContainer tree dumping out the information about 
    all known PanelContainers and Panels. */
void
PanelContainer::debugPanelContainerTree()
{
  printf("here's the tree.\n");
  _masterPC->traversePanelContainerTree();
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
  nprintf(DEBUG_PANELCONTAINERS) ("A: PanelContainer::panelContainerContextMenuEvent(%s-%s) targetPC=(%s-%s)\n", getInternalName(), getExternalName(), targetPC->getInternalName(), targetPC->getExternalName() );

   _masterPC->_lastPC = targetPC;

  // One more check...  NOTE: This should be cleaned up as well.
  if( targetPC->leftPanelContainer && targetPC->rightPanelContainer )
  {  // There should be no menu action for this split panel container.  Only
     // for it's children.
     return;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("\n\n\nPanelContainer::contextMenuEvent() acting on it!\n");

  _masterPC->pcMenu = new QPopupMenu( this );
  {
  char n[1024]; strcpy(n,"pcMenu:");strcat(n, internal_name);strcat(n,"-");strcat(n,external_name);
  _masterPC->pcMenu->setCaption(n);
  }

  Q_CHECK_PTR( _masterPC->pcMenu );
  _masterPC->pcMenu->insertItem( "Remove Raised &Panel", targetPC->_masterPC, SLOT(removeRaisedPanel()), CTRL+Key_P );
  _masterPC->pcMenu->insertItem( "Split &Horizontal",  targetPC, SLOT(splitHorizontal()), CTRL+Key_H );
  _masterPC->pcMenu->insertItem( "Split &Vertical", targetPC, SLOT(splitVertical()), CTRL+Key_V );
  _masterPC->pcMenu->insertItem( "&Remove Container", targetPC->_masterPC, SLOT(removePanelContainer()), CTRL+Key_R );

  _masterPC->contextMenu = NULL;
  if( localMenu == TRUE )
  {
    _masterPC->contextMenu = new QPopupMenu( this );
    {
    char n[1024]; strcpy(n,"contextMenu:A:");strcat(n, internal_name);strcat(n,"-");strcat(n,external_name);
//    _masterPC->pcMenu->setCaption(n);
    _masterPC->contextMenu->setCaption(n);
    }
    _masterPC->contextMenu->insertItem("&Panel Container Menu", _masterPC->pcMenu, CTRL+Key_P );
  
    // Now call the Panel's menu() function to add it's menus (if any).
    if( !targetPC->addPanelMenuItems(_masterPC->contextMenu) )
    {  // There were no panel menus... Show the panel Container menus
       // without cascading...
      delete _masterPC->contextMenu;
      _masterPC->contextMenu = _masterPC->pcMenu;
    }
  } else
  {
    _masterPC->contextMenu = _masterPC->pcMenu;
  }
  
  if( targetPC->topLevel == TRUE && targetPC != _masterPC /* && 
      !targetPC->areTherePanels() */ )
  {
if( targetPC->parent->isTopLevel() )
{
    _masterPC->pcMenu->insertItem( "&Close", targetPC->_masterPC, SLOT(closeWindow()), CTRL+Key_R );
}
  }

  nprintf(DEBUG_PANELCONTAINERS) ("About to popup pc contextMenu\n");
  _masterPC->contextMenu->exec( QCursor::pos() );

  nprintf(DEBUG_PANELCONTAINERS) ("finished with the context menu...\n");

  delete _masterPC->contextMenu;
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
int
PanelContainer::notifyNearest(char *msg)
{
  PanelContainer *pc = this;
  int return_value = 0;
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyNearest()\n");

  wasThereAnInterestedPanel(pc, msg, &return_value );
 
  if( return_value == 0 )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("Didn't find anyone local.   Walk up and around the tree.\n");
    if( pc->parentPanelContainer )
    {
      pc = pc->parentPanelContainer;
      if( findNearestInterestedPanel(pc, msg, &return_value) )
      {
        nprintf(DEBUG_PANELCONTAINERS) ("Found the nearest interesting panel and delivered message.\n");
        return(1);
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
  
          if( findNearestInterestedPanel(pc, msg, &return_value) )
          {
            nprintf(DEBUG_PANELCONTAINERS) ("WEll that was a lot of work, but we found one..\n");
            return(1);
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
          return(1);
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
          return(1);
        }
      }
    }
  }
  return(return_value);
}

/*! First check the raised Panel, then loop through all the Panels in the
    PanelContainer to see if there is a Panel interested in this message. 
    \p
    If there was a Panel interested, return the Panel's listener routines
    return value up the call chain.
 */
int
PanelContainer::wasThereAnInterestedPanel(PanelContainer *pc, char *msg, int *return_value )
{
  Panel *p = NULL;
  int ret_val = 0;
  // First see if there's a raised panel that's interested. (In this
  // PanelContaienr.)
  p = pc->getRaisedPanel();
  if( p )
  {
    ret_val = p->listener(msg);
    if( ret_val > 0 )
    {
      // At least on panel wanted to see this message.
      *return_value = ret_val;
      return(*return_value);
    }
  }

  // Then look in this PanelContainer
  for( PanelList::Iterator pit = pc->panelList.begin();
           pit != pc->panelList.end();
           ++pit )
  {
    p = (Panel *)*pit;
    ret_val = p->listener(msg);
    if( ret_val > 0 )
    {
      // At least on panel wanted to see this message.
      *return_value = ret_val;
    }
  }

  return(*return_value);
}

/*! Recursively walks the PanelContainer tree looking for a Panel to field
    this message.
   \note FIX: This routine isn't even close... It's just started for the sake
   of a demo and needs to be rethought out and reengineered. 
 */
PanelContainer *
PanelContainer::findNearestInterestedPanel(PanelContainer *pc, char *msg, int *ret_val)
{
  PanelContainer *foundPC = NULL;

  nprintf(DEBUG_PANELCONTAINERS) ("findNearestInterestedPanel: entered\n");

  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("A: just return pc\n");
    if( wasThereAnInterestedPanel(pc, msg, ret_val ) )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("There was!   There was a nearest intresting panel!\n");
      return pc;
    }
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    foundPC = findNearestInterestedPanel(pc->leftPanelContainer, msg, ret_val);
    if( foundPC )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("found an interested Panel!\n");
      return( foundPC );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    foundPC = findNearestInterestedPanel(pc->rightPanelContainer, msg, ret_val);
    if( foundPC )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("found an interested Panel!\n");
      return( foundPC );
    }
  }

  return NULL;
}

/*! This routine is currently unimplemented.   
    The vision is to be able to send a message to a specific group of 
    Panels that belong to a named group.   The named group is already
    supported in the Panel creation, its simply not being used at this 
    time.
    \note: Currently unimplemented.
 */
int
PanelContainer::notifyGroup(char *msg)
{
  nprintf(DEBUG_PANELCONTAINERS) ("PanelContainer::notifyGroup()\n");
  int return_value = 0;
  return(return_value);
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

