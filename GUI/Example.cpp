/*!  Class PanelContainer

    This is a very (very) key class in the overall base functionality of the
    GUI.   It is responsible for the screen management within the tool.

    The base functionality it provides is the interactions with the user
    to create and manage PanelContainer.   A PanelContainer is reponsible for
    managing one or more Panels (User Panels).    There can be one or many 
    PanelContainers.   A PanelContainer may live only within the MainWindow
    (PerformanceLeader class) at a minimum, or there can be PanelContainers
    within PanelContainers, or PanelContainers within Panels.   

    A PanelContainer can then be split (horizontally creating a 
    left and right PanelContainer, or vertically creating a top and bottom
    PanelContainer)  Those PanelContainers can be recursively split as well.
    
    PanelContainers can also be removed effectively undoing the split.

    In addition to PanelContainer functionality, the PanelContainer functions
    also include managing Panels.   Panel resizing and Panel menu management
    are two key functions.   Also removing and adding Panels.
    
    It's also the reponsibility of the PanelContainer class to save its state
    to a file that can then be reread in (at a subsequent invocation) to 
    allow the tool to come up with the same layout.

    Author: Al Stipek   (stipek@sgi.com)
 */

#include "Panel.h"
#include "PanelContainer.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qevent.h>

#include <qcursor.h>

// #define DEBUG_OUTPUT_REQUESTED 1
#include "debug.h"  // This includes the definition of dprintf
#include <stdlib.h>
#include <string.h>

static char tmp_str[1024]; 
int depth = 1;
int max_depth = 1;
int pc_rename_count = 0;

#include <qapplication.h>  // For exit..


/*!
 * Default contructor for a PanelContainer.   It should never be called
 * and is only here for completeness.
 */
 
PanelContainer::PanelContainer( )
{
}

/*! 
 *  Constructs a PanelContainer as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
PanelContainer::fred( QWidget* _parent, const char* n, PanelContainer *_parentPanelContainer, PanelContainerList *pcl, WFlags fl) : QWidget( _parent, n, fl )
{

  if( strcmp(n, "topPC") == 0 )
  { // This is the very first panel container...   The very first time in here.
fprintf(stderr, "This is the first PC created.   set the count == 0 \n");
_doingMenuFLAG = FALSE;
    _topPC = this;
    _panel_container_count = 0;
  } else
  {
    if( _parentPanelContainer )
    {
      _topPC = _parentPanelContainer->_topPC;
    } else
    {
      PanelContainerList::Iterator it = pcl->begin();
      _topPC = (PanelContainer *)*it;
    }
  }

  


  pc_rename_count = 0;
  sprintf(pc_name_count, "pc:%d", _topPC->_panel_container_count);
  internal_name = strdup(pc_name_count);
  external_name = strdup(n);

  _panelContainerList = pcl;

  sourcePC = NULL;
  widget_to_reparent = NULL;
//  pos = 0;
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

//  widgetToReparent = NULL;
  dropSiteLayoutParent = NULL;
  dropSiteLayout = NULL;
  tabWidget = NULL;

  if( _topPC->_panel_container_count == 0 )
  {
    // Make sure the panelContainerList interator list is empty.
    dprintf("PanelContainer::PanelContainer() _topPC->_panel_container_count=0\n");
    _panelContainerList->clear();
  }
  // Add this panel container to the global list.
  dprintf("PanelContainer::PanelContainer() add PC getInternalName()=(%s)\n",
    getInternalName() );

  _panelContainerList->push_back(this);

  panelList.clear();  // Make sure the panelList interator list is empty.

  if( !internal_name )
  {
    setInternalName(pc_name_count);
  }

  panelContainerFrameLayout =
    new QVBoxLayout( parent, 0, 0, "panelContainerFrameLayout"); 

  // Now create the splitter so we can put a container (frame) on the left
  // and right or top and bottom.
  splitter = new QSplitter(parent, "splitter");

  // The orientation doesn't matter at this point... at least not yet.
  splitter->setOrientation( QSplitter::Vertical );

  // Create a frame for the left side and show it.
  sprintf(tmp_str, "originalLeftFrame for %s", getInternalName() );
  leftFrame = new Frame(this, splitter, "left_frame");
  leftFrame->dragEnabled = TRUE;
  leftFrame->dropEnabled = TRUE;

  // This is only used to hold panels... not PanelContainers.
  dropSiteLayoutParent = new QWidget( leftFrame, "dropSiteLayoutParent" );
  dropSiteLayout = new QVBoxLayout( dropSiteLayoutParent, 0, 0, "dropSiteLayout");

  tabWidget = new QTabWidget( dropSiteLayoutParent, "tabWidget" );

  dropSiteLayout->addWidget( tabWidget );
  dropSiteLayoutParent->hide();

  // Create a frame for the right side, but don't show it... yet.
  rightFrame = new Frame(this, splitter, "right_frame");
  rightFrame->dragEnabled = TRUE;
  rightFrame->hide();

  // Add the splitter to the the layout box.
  panelContainerFrameLayout->addWidget( splitter );
  splitter->show(); // Yes this is needed... Otherwise subsequent splits don't 
                    // show this splitter, but only the menu.

  languageChange();
  resize( QSize(542, 300).expandedTo(minimumSizeHint()) );

  // signals and slots connections

  _topPC->_panel_container_count++;
}

/*!
 *  Destroys the object and frees any allocated resources
 */
PanelContainer::~PanelContainer()
{
  // no need to delete child widgets, Qt does it all for us
  dprintf("PanelContainer::~PanelContainer(%s) destructor called.\n",
    getInternalName() );
  dprintf("	Don't forget to delete the child panel containers... being\n");
  dprintf("	careful not to blow away any panels that might need to be kept.\n");

  menuEnabled = FALSE;

  _panelContainerList->remove(this);

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
  delete( panelContainerFrameLayout );

  if( internal_name )
  {
    dprintf("delete %s\n", getInternalName() );
    free(internal_name);
    internal_name = strdup("dead PC");
  }
  if( external_name )
  {
    dprintf("delete %s\n", getExternalName() );
    free(external_name);
    external_name = strdup("dead PC");
  }
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
PanelContainer::languageChange()
{
  setCaption( tr( "Panel Container" ) );
}

void
PanelContainer::split(Orientation orientation, bool showRight)
{
  // We've received a request to split the panelContainer.
  dprintf("PanelContainer::split(%s-%s)\n", getInternalName(), getExternalName() );
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
                              "leftPanelContainer", this, _panelContainerList);
//    leftPanelContainer->_topPC = _topPC;
  } 
  if( rightPanelContainer == NULL )
  {
    rightPanelContainer = createPanelContainer( rightFrame,
                              "rightPanelContainer", this, _panelContainerList);
//    rightPanelContainer->_topPC = _topPC;
  }

  splitter->setOrientation( orientation );

  // Resize on split ...
  // Get the size of the parent so we can split up the real estate.
  int width = this->width();
  int height = this->height();
  dprintf("::split: The parent's: width=%d height=%d\n", width, height);
  QValueList<int> sizeList;
  sizeList.clear();
  if( orientation == QSplitter::Vertical )
  {
    dprintf("	split try to resize left to w=%d h=%d\n", width, height/2);
    sizeList.push_back((int)(height/2));
    sizeList.push_back((int)(height/2));
  } else // Horizontal
  {
    dprintf("	split try to resize left to w=%d h=%d\n", width/2, height);
    sizeList.push_back((int)(width/2));
    sizeList.push_back((int)(width/2));
  }
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
    dprintf("showing all the right information (showRight == TRUE)\n");
    dprintf("Turn off the parent menu for the panel container\n");

    menuEnabled = FALSE;

    leftPanelContainer->show();
    leftPanelContainer->menuEnabled = TRUE;

    rightFrame->show();
    rightPanelContainer->show();
    rightPanelContainer->menuEnabled = TRUE;
  } else
  {  // Don't show anything right, but make sure you keep the parent pc menu
    dprintf("Don't show anything right, but make sure the parent menu is available\n");
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

void
PanelContainer::splitHorizontal()
{
  dprintf("PanelContainer::splitHorizontal()\n");

  // We've received a request to split the panelContainer horizontally.
  split( QSplitter::Horizontal );
}

void
PanelContainer::splitVertical()
{
  dprintf("PanelContainer::splitVertical()\n");

  // We've received a request to split the panelContainer vertically.
  split( QSplitter::Vertical );
}

Panel *
PanelContainer::getRaisedPanel()
{
  dprintf("PanelContainer::getRaisedPanel(%s) entered.\n", getInternalName() );
  Panel *p = NULL;

  if( !areTherePanels() )
  {
    return p;
  }

  if( tabWidget != NULL )
  {
    int currentPageIndex = tabWidget->currentPageIndex();
    dprintf("attempt to determine currentPageIndex=%d\n", currentPageIndex );
    int i=0;
    for( PanelList::Iterator it = panelList.begin();
             it != panelList.end();
             ++it )
    {
      if( i == currentPageIndex )
      {
        p = (Panel *)*it;
        dprintf("found the raised panel\n");
        return p;
      }
      i++;
    }
  }

  return p; 
}

void
PanelContainer::removeRaisedPanel()
{
  dprintf("PanelContainer::removeRaisedPanel(%s) entered.\n", getInternalName() );

  if( tabWidget != NULL )
  {
    QWidget *currentPage = tabWidget->currentPage();
    int currentPageIndex= tabWidget->currentPageIndex();
    Panel *p = getRaisedPanel();

    if( currentPageIndex == -1 || p == NULL)
    {
      fprintf(stderr, "Unable to remove raised panel.\n");
      return;
    }

    tabWidget->removePage(currentPage);
    if( p )
    {
      dprintf("we've got a raised panel to delete.  delete it (%s)\n",
        p->getName() );
      panelList.remove(p);   
      dprintf("PanelContainer::removeRaisedPanel() delete %s\n", p->getName() );
      delete p;
    }
  }

  if( panelList.empty() )
  {
    dprintf("That was the last panel.  PC is now empty of panels.\n");
    leftFrame->show();
    dropSiteLayoutParent->hide();
    tabWidget->hide();
    splitter->show();
  }
}

void
PanelContainer::barney()
{
  dprintf("PanelContainer::removePanelContainer(%s)\n", getInternalName() );

  PanelContainer *pcToReparent = NULL;
  PanelContainer *pcToRemove = NULL;
  PanelContainer *parentPC = parentPanelContainer;
  enum { None, Hide_Left_SIDE, Hide_Right_SIDE } sideType;


  PanelContainer *pcToRecover = NULL;
  bool doubleHIDE = FALSE;

  sideType = None;
  // First remove any panels in the panel container.
  if( areTherePanels() )
  {
    removePanels();
  }

  // If this is the master, be a bit more careful removing it.
  // If its the main PerformanceLeader window, consider this an exit().
  if( parentPC == NULL || topLevel == TRUE )
  {
    if( strcmp(getInternalName(), "pc:0") == 0 )
    {
      return;
    } else
    {
      parent->hide();
      delete(this);
    }
    return;
  }

  if( this == parentPC->leftPanelContainer )
  {
    // if the pc being delete is the left, pcToReparent=parentPC->rightPC;
    pcToReparent=parentPC->rightPanelContainer;
    pcToRemove = parentPC->leftPanelContainer;
    sideType = Hide_Left_SIDE;
    dprintf("removing left=(%s) reparenting parentPC->rightPanelContainer\n",
      getInternalName() );
  } else if( this == parentPC->rightPanelContainer )
  {
    // if the pc being delete is the right, pcToReparent=parentPC->leftPC;
    pcToReparent=parentPC->leftPanelContainer;
    pcToRemove = parentPC->rightPanelContainer;
    sideType = Hide_Right_SIDE;
    dprintf("removing right=(%s) reparenting parentPC->lpc=(%s)\n",
      getInternalName(), pcToReparent->getInternalName() );
  } else
  {
    fprintf(stderr, "Warning: Unexpected error removing panel\n");
    return;
  }
  Orientation orientation = pcToReparent->splitter->orientation();

  // Is there a panel to recover?
  if( parentPC->parentPanelContainer )
  {
    dprintf("We've got a parent.   Are either either marked for delete?\n");

    if( parentPC->parentPanelContainer->leftPanelContainer->markedForDelete )
    { 
      dprintf("The left pc (%s) is marked for deletion.\n",
        parentPC->parentPanelContainer->leftPanelContainer->getInternalName() );

      pcToRecover = parentPC->parentPanelContainer->leftPanelContainer;
    } else if( parentPC->parentPanelContainer->rightPanelContainer->markedForDelete )
    {
      dprintf("The right pc (%s) is marked for deletion.\n",
        parentPC->parentPanelContainer->rightPanelContainer->getInternalName() );

      pcToRecover = parentPC->parentPanelContainer->rightPanelContainer;
    }
  }

#ifdef DEBUG_OUTPUT_REQUESTED
  dprintf("Okay: we're here:\n");
  dprintf("pcToReparent:\n");
  pcToReparent->printPanelContainer(1);
  dprintf("parentPC:\n");
  parentPC->printPanelContainer(1);
  dprintf("Now do the actual reparent and paneContainer reduction.\n");
#endif // DEBUG_OUTPUT_REQUESTED

  // Begin I'm confused, why I need to do this...
  // disable the enter leave events
  // Now hide the parts that are going to be deleted...
  leftFrame->hide();
  rightFrame->hide();
  menuEnabled = FALSE;
  dropSiteLayoutParent->hide();
  pcToReparent->leftFrame->hide();
  pcToReparent->rightFrame->hide();
  pcToReparent->menuEnabled = FALSE;
  pcToReparent->dropSiteLayoutParent->hide();
  // End I'm confused why I need to do this...

  if( pcToReparent->leftPanelContainer &&
      pcToReparent->rightPanelContainer )
  {
    if( pcToReparent->leftPanelContainer->leftPanelContainer ||
        pcToReparent->rightPanelContainer->leftPanelContainer )
    {
      pcToRemove->hide();
      pcToRemove->menuEnabled = FALSE;

      PanelContainer *ppcppc = parentPC->parentPanelContainer;
      if( ppcppc &&
          (ppcppc->rightPanelContainer->markedForDelete == TRUE || ppcppc->leftPanelContainer->markedForDelete == TRUE) )
      {
        // We've got a double hidden!
        if( ppcppc->rightPanelContainer->markedForDelete == TRUE )
        {
          dprintf("mark %s for delete (Left)\n",
            ppcppc->leftPanelContainer->getInternalName() );

          ppcppc->leftPanelContainer->markedForDelete = TRUE;
          parentPC->leftPanelContainer->leftFrame->show();
          parentPC->leftPanelContainer->rightFrame->show();
          doubleHIDE = TRUE;
        } else
        {
          dprintf("mark %s for delete (Right)\n",
            ppcppc->rightPanelContainer->getInternalName() );

          ppcppc->rightPanelContainer->markedForDelete = TRUE;
          parentPC->rightPanelContainer->leftFrame->show();
          parentPC->rightPanelContainer->rightFrame->show();
          doubleHIDE = TRUE;
        }
      }

      if( sideType == Hide_Left_SIDE )
      { // If left side is to be hidden.
        dprintf("Hide the left side!\n");
        parentPC->leftPanelContainer->markedForDelete = TRUE;
        parentPC->leftFrame->hide();
        parentPC->rightFrame->show();

        parentPC->rightPanelContainer->show();
        parentPC->rightPanelContainer->leftFrame->show();
        parentPC->rightPanelContainer->rightFrame->show();
      } else
      { // If right side is to be hidden.
        dprintf("Hide the right side!\n");
        parentPC->rightPanelContainer->markedForDelete = TRUE;
        parentPC->rightFrame->hide();
        parentPC->leftFrame->show();

        parentPC->leftPanelContainer->show();
        parentPC->leftPanelContainer->leftFrame->show();
        parentPC->leftPanelContainer->rightFrame->show();
      }
    } else
    {
      // disable the enter leave events
      pcToReparent->leftPanelContainer->leftFrame->panelContainer = NULL;
      pcToReparent->leftPanelContainer->rightFrame->panelContainer = NULL;
      pcToReparent->leftPanelContainer->menuEnabled = FALSE;
      pcToReparent->rightPanelContainer->leftFrame->panelContainer = NULL;
      pcToReparent->rightPanelContainer->rightFrame->panelContainer = NULL;
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

      dprintf("WE NEED TO COLLAPSE!\n");

      if( pcToReparent->leftPanelContainer->areTherePanels() )
      {
        dprintf("COLLAPSE: and there are panels in leftPanelContainer\n");

        reparentPCPanels(parentPC->leftPanelContainer,
                       pcToReparent->leftPanelContainer);
      } else
      {
        dprintf("COLLAPSE: but no panels\n");
      }

      if( pcToReparent->rightPanelContainer->areTherePanels() )
      {
        dprintf("COLLAPSE: and thre are panels in rightPanelContainer\n");

        reparentPCPanels(parentPC->rightPanelContainer,
                       pcToReparent->rightPanelContainer);
      } else
      {
        dprintf("COLLAPSE: but no panels\n");
      }

      if( parentPC->leftPanelContainer->markedForDelete == TRUE )
      {
        dprintf("lpc marked for delete, but we can use it now.. Unmark it.\n");
        parentPC->leftPanelContainer->markedForDelete = FALSE;
        parentPC->leftFrame->show();
        parentPC->rightFrame->show();
      }
      if( parentPC->rightPanelContainer->markedForDelete == TRUE )
      {
        dprintf("rpc marked for delete, but we can use it now.. Unmark it.\n");
        parentPC->rightPanelContainer->markedForDelete = FALSE;
        parentPC->rightFrame->show();
        parentPC->leftFrame->show();
      }
  
      delete pcToReparent->leftPanelContainer;
      pcToReparent->leftPanelContainer = NULL;
      delete pcToReparent->rightPanelContainer;
      pcToReparent->rightPanelContainer = NULL;

      parentPC->menuEnabled = FALSE;
  
      parentPC->leftPanelContainer->leftFrame->show();
      parentPC->leftPanelContainer->menuEnabled = TRUE;
      
      parentPC->rightPanelContainer->leftFrame->show();
      parentPC->rightPanelContainer->menuEnabled = TRUE;
    }
  } else
  {
    // disable the enter leave events
    leftFrame->panelContainer = NULL;
    rightFrame->panelContainer = NULL;
    menuEnabled = FALSE;
    pcToReparent->leftFrame->panelContainer = NULL;
    pcToReparent->rightFrame->panelContainer = NULL;
    pcToReparent->menuEnabled = FALSE;

    dprintf("NO NEED TO COLLAPSE, just reparent.\n");

    if( pcToReparent->areTherePanels() )
    {
      dprintf("NO COLLAPSE: and there are panels to reparent.\n");
      reparentPCPanels(parentPC, pcToReparent);
      parentPC->dropSiteLayoutParent->show();
    } else
    {
      dprintf("NO COLLAPSE: but no panels\n");
      dropSiteLayoutParent->hide();
    }

    parentPC->rightPanelContainer->menuEnabled = FALSE;
    parentPC->leftPanelContainer->menuEnabled = FALSE;

    delete parentPC->rightPanelContainer;
    parentPC->rightPanelContainer = NULL;
    delete parentPC->leftPanelContainer;
    parentPC->leftPanelContainer = NULL;


    parentPC->show();
    parentPC->leftFrame->show();
    parentPC->rightFrame->hide();
// parentPC->menuEnabled = TRUE;   // Causes double menu event...
  }

  parentPC->splitter->setOrientation( orientation );

  parentPC->splitter->show();


  if( doubleHIDE == TRUE )
  {
    parentPC->leftFrame->hide();
    pcToRecover = NULL;
  }

  if( pcToRecover )
  {
    dprintf("There was an extra (hidden) pc (%s) to recover.   Delete it now.\n", pcToRecover->getInternalName() );
    pcToRecover->removePanelContainer();
  }
 
  return;
}


void
PanelContainer::reparentPCPanels(PanelContainer *tPC, PanelContainer *fPC)
{
  dprintf("PanelContainer::reparentPCPanels(%s, %s)\n",
    tPC->getInternalName(), fPC->getInternalName() );

  sourcePC = fPC;
  tPC->movePanelsToNewPanelContainer( sourcePC );
}

PanelContainer *
PanelContainer::findInternalNamedPanelContainer(char *panel_container_name)
{
printf("findInternalNamedPanelContainer(%s) entered\n", panel_container_name );
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _panelContainerList->begin();
               it != _panelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    dprintf("findInternalNamedPanelContainer: is (%s) (%s)\n",
      pc->getInternalName(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getInternalName()) == 0 )
    {
      dprintf("findInternalNamedPanelContainer(%s): found one\n", pc->getInternalName() );
      return pc;
    }
  }

  // Didn't find the named panel container.   Try to find an empty panel
  // container to put this..
  
  pc = findFirstEmptyPanelContainer(_topPC);
  dprintf("findInternalNamedPanelContainer(%s) says drop it in %s instead.\n",
    panel_container_name, pc->getInternalName() );

  return( pc );
} 

PanelContainer *
PanelContainer::arney(PanelContainer *start_pc)
{
  PanelContainer *pc = start_pc;
  if( pc == NULL )
  {
    pc = _topPC;
  }

  if( start_pc == NULL )
  {
    start_pc = _topPC;
  }

  dprintf("findBestFitPanelContainer() from %s %s\n", start_pc->getInternalName(), start_pc->getExternalName() );


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


PanelContainer *
PanelContainer::findPanelContainerWithNamedPanel(char *panel_container_name)
{
  dprintf("findPanelContainerWithNamedPanel(%s) entered\n", panel_container_name );
  PanelContainer *pc = NULL;
  PanelContainer *foundPC = NULL;
  for( PanelContainerList::Iterator it = _panelContainerList->begin();
               it != _panelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    dprintf("findPanelContainerWithNamedPanel: is (%s) (%s)\n",
      pc->getExternalName(), panel_container_name );
    if( pc->markedForDelete == FALSE && 
        strcmp(panel_container_name, pc->getExternalName()) == 0 )
    {
      dprintf("findPanelContainerWithNamedPanel(%s): found one\n", pc->getExternalName() );
      // We found one
      if( pc->getRaisedPanel() )
      { // The matching PanelContainer with a raised Panel always
        // get's priority.  NOTE: Does this need to be a toplevel panel?
        return pc;
      }
      pc = foundPC;
    }
  }

  return foundPC;
} 

PanelContainer *
PanelContainer::findPanelContainerByMouseLocation()
{
  QPoint mouse_pos = QCursor::pos ();
  int mouse_x = mouse_pos.x();
  int mouse_y = mouse_pos.y();

  dprintf("mouse_pos.x()=%d mouse_pos.y()=%d\n", mouse_pos.x(), mouse_pos.y() );

  PanelContainer *pc = NULL;
  PanelContainer *found_pc = NULL;

  for( PanelContainerList::Iterator it = _panelContainerList->begin();
               it != _panelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->markedForDelete == FALSE && 
        !pc->leftPanelContainer && !pc->rightPanelContainer )
    {
      QPoint point = pc->leftFrame->pos();
      QPoint abs_point = pc->leftFrame->mapToGlobal(point);
      int x=abs_point.x();
      int y=abs_point.y();
      int width=pc->parent->width();
      int height=pc->parent->height();

      dprintf("(%s) x=%d y=%d width=%d height=%d\n",
        pc->getInternalName(), x, y, width, height );

      if( mouse_x >= x && mouse_x <= x+width  &&
          mouse_y >= y && mouse_y <= y+height )
      {
        dprintf("findPanelContainerByMouseLocation(%s-%s): found one\n", pc->getInternalName(), pc->getExternalName() );
        // Before you just return this PC, see if there's a toplevel
        // panel in this location.   If there is continue to see if
        // there's a match there..
        Panel *p = pc->getRaisedPanel();
        if( p && p->topLevel )
        {
          found_pc = pc;
        } else
        {
          return pc;
        }
      }
    }
  }

  return( found_pc );
} 




PanelContainer *
PanelContainer::findFirstAvailablePanelContainer(PanelContainer *pc)
{
  PanelContainer *foundPC = NULL;

  dprintf("findFirstAvailablePanelContainer: entered\n");

  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    dprintf("A: just return pc\n");
    return pc;
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    dprintf("try to find on down the left.\n");
    foundPC = findFirstAvailablePanelContainer(pc->leftPanelContainer);
    if( foundPC )
    {
      dprintf("found an empty PC!\n");
      return( foundPC );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    dprintf("try to find on down the right.\n");
    foundPC = findFirstAvailablePanelContainer(pc->rightPanelContainer);
    if( foundPC )
    {
      dprintf("found an available PC!\n");
      return( foundPC );
    }
  }

  return NULL;
}


PanelContainer *
PanelContainer::findFirstEmptyPanelContainer(PanelContainer *pc)
{
  PanelContainer *foundPC = NULL;

  dprintf("findFirstEmptyPanelContainer: entered\n");
  if( pc->markedForDelete == FALSE &&
      !pc->leftPanelContainer && !pc->rightPanelContainer )
  {
    if( !pc->areTherePanels() )
    {
      dprintf("A Found an empty space right away.  Return pc.\n");
      return pc;
    }
  }
  if( pc->leftPanelContainer &&
      pc->leftPanelContainer->markedForDelete == FALSE )
  {
    dprintf("try to find on down the left.\n");
    foundPC = findFirstEmptyPanelContainer(pc->leftPanelContainer);
    if( foundPC && !foundPC->areTherePanels() )
    {
      dprintf("A: found an empty PC!\n");
      return( foundPC );
    }
  } 
  if( pc->rightPanelContainer &&
      pc->rightPanelContainer->markedForDelete == FALSE )
  {
    dprintf("try to find on down the right.\n");
    foundPC = findFirstEmptyPanelContainer(pc->rightPanelContainer);
    if( foundPC && !foundPC->areTherePanels() )
    {
      dprintf("B: found an empty PC!\n");
      return( foundPC );
    }
  }

  return NULL;
}


void
PanelContainer::savePanelContainerTree()
{
  char *fn = "ft.geometry";
  FILE *fd = fopen(fn, "w");
  if( fd == NULL ) 
  {
    fprintf(stderr, "Unable to open file (%s) for saving session.\n", fn);
    return;
  }


  pc_rename_count = 0;
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _panelContainerList->begin();
               it != _panelContainerList->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->topLevel == TRUE )
    { // If this is a topLevel tree walk the tree.
      depth = 1;
      dprintf("save pc=(%s)\n", pc->getInternalName() );
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

void
PanelContainer::renamePanelContainerTree(PanelContainer *pc)
{
fprintf(stderr, "renamePanelConantinerTree() from pc (%s)\n", pc->getInternalName() );
//  pc_rename_count = 0;
  for( int i = 0; i<max_depth+1; i++ )
  {
    // From the top again...  Rename sequencing off depth.
    _renamePanelContainerTree(pc, i); 
  }
}

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

void
PanelContainer::foo()
{
  char tmpstr[100];

  free(internal_name);
  free(external_name);

  sprintf(tmpstr, "pc:%d", pc_rename_count);
  pc_rename_count++;

  setInternalName(tmpstr);
}


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
      parentPanelContainer ? parent->width() : tl->width(),
      parentPanelContainer ? parent->height() : tl->height(),
      markedForDelete, tl->x(), tl->y() );
      dprintf("%s(%s) (%s) SPLIT: o=%d ls=%d w=%d h=%d mfd=%d x=%d y=%d depth=%d\n",
        indent_buffer, getInternalName(),
        parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
        splitter->orientation(), 
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
        parentPanelContainer ? parent->width() : tl->width(),
        parentPanelContainer ? parent->height() : tl->width(),
        markedForDelete, tl->x(), tl->y(), _depth );

    fwrite(buffer, BUFSIZE, 1, fd);
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    sprintf(buffer, "%s %s %d %d %d %d %d %d %d %d\n",
      getInternalName(),
      parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
      !SPLIT, -1,
      parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      parentPanelContainer ? parent->width() : tl->width(),
      parentPanelContainer ? parent->height() : tl->height(),
      markedForDelete, tl->x(), tl->y() );
      dprintf("%s(%s) (%s) NO SPLIT: ls=%d w=%d h=%d mfd=%d x=%d y=%d depth=%d\n",
      indent_buffer, getInternalName(),
      parentPanelContainer ? parentPanelContainer->getInternalName() : "toplevel",
parentPanelContainer ? parentPanelContainer->leftPanelContainer == this : 0,
      parentPanelContainer ? parent->width() : tl->width(),
      parentPanelContainer? parent->height() : tl->height(),
      markedForDelete, tl->x(), tl->y(), _depth );
// printPanels(indent_buffer);
    fwrite(buffer, BUFSIZE, 1, fd);
  }
}

void
PanelContainer::_saveOrderedPanelContainerTree(PanelContainer *pc, FILE *fd)
{
fprintf(stderr, "_saveOrderedPanelContainerTree() from pc (%s)\n", pc->getInternalName() );
  for( int i = 0; i<max_depth+1; i++ )
  {
    // From the top again...  save by depth
    __saveOrderedPanelContainerTree(pc, fd, i); 
  }
}


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


Panel *
PanelContainer::addPanel(Panel *p, PanelContainer *panel_container, char *tab_name)
{
  PanelContainer *pc = panel_container;

  dprintf("PanelContinaer::addPanel(%s, %s) in (%s)\n", tab_name, p->getName(), pc->getInternalName() );
  if( pc->tabWidget == NULL )
  {
    fprintf(stderr, "ERROR: addPanel.  No tabWidget\n");
    return NULL;
  }

  dprintf("PanelContinaer::addPanel() add the tab\n");
  pc->tabWidget->addTab( p->getBaseWidgetFrame(), tab_name );

  // Add the panel to the iterator list.   This list (panelList) aids us
  // when cleaning up PanelContainers.  We'll just loop over this list
  // deleting all the panels.
  // Note: This needs to work with Panel->removePanel(),
  // i.e. Panel->removePanel(), will need to call:
  //  PanelContainer::removePanel(int panel_index) to have the panel and
  // it's location in the iterator list removed.
  dprintf("PanelContinaer::addPanel() push_back() it!\n");
  pc->panelList.push_back(p);

  dprintf("PanelContinaer::addPanel() setCurrentPage(%d)\n", pc->tabWidget->count()-1);


  // Put the newly added tab on top
  dprintf("Try to put %d on top\n", pc->tabWidget->count()-1 );

  pc->tabWidget->setCurrentPage(pc->tabWidget->count()-1);

  pc->handleSizeEvent((QResizeEvent *)NULL);

  // Make it visible to the world.  At somepoint we may want to create a tab,
  // but not make it visible.  If/when that day comes, you'll need to pass in 
  // an additional parameter (showing by default) that let's one toggle this 
  // call.
  pc->dropSiteLayoutParent->show();
  dprintf("PanelContinaer::addPanel() show() it!\n");
  pc->tabWidget->show();

  dprintf("PanelContinaer::addPanel() returning\n");

  return( p );
}

void
PanelContainer::removePanels()
{
  dprintf("PanelContainer::removePanels(%s)\n", getInternalName() );
  if( tabWidget != NULL )
  {
    Panel *p = NULL;
    for( PanelList::Iterator it = panelList.begin();
             it != panelList.end();
             ++it )
    {
      p = (Panel *)*it;
      dprintf("PanelContainer::removePanels() about to delete %s\n",
        p->getName() );
      delete p;
    }
  }
  panelList.clear();

  if( panelList.empty() )
  {
    leftFrame->show();
    dropSiteLayoutParent->hide();
//    tabWidget->hide();
    splitter->show();
  }

  return;
}

void
PanelContainer::handleSizeEvent(QResizeEvent *)
{
//  dprintf("PanelContainer::handleSizeEvent() for %s\n", getInternalName() );

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

//  dprintf("PanelContainer::parent(width=%d height=%d)\n", width, height);

  // resize the Panels in this container...
resize( width, height );
  dropSiteLayoutParent->resize( width, height );
//  if( widgetToReparent )
//  {
//    widgetToReparent->resize( width, height );
//  }
  
//  dprintf("PanelContainer::handleSizeEvent() returning.\n");

  return;
}

// A simple convienience routine to see if a panel container has panels.
bool
PanelContainer::areTherePanels()
{
  return( !panelList.empty() );
}


void
PanelContainer::setInternalName( const char *n )
{
  internal_name = new char(strlen(n)+1);
  strcpy(internal_name, n);
}

void
PanelContainer::setExternalName( const char *n )
{
  external_name = new char(strlen(n)+1);
  strcpy(external_name, n);
}

PanelContainer *
createPanelContainer( QWidget* parent, const char* name, PanelContainer *parentPanelContainer, PanelContainerList *panelContainerList )
{
  dprintf("createPanelContainer(%s) entered.\n", name);
  PanelContainer *npc = new PanelContainer( parent, name, parentPanelContainer, panelContainerList );

  dprintf("created npc=(%s) (%s)\n", npc->getInternalName(), npc->getExternalName() );

  if( strcmp("TOPLEVEL", npc->getExternalName()) == 0 )
  {
    npc->topLevel = TRUE;
  }

   return( npc );
}

void
PanelContainer::movePanelsToNewPanelContainer( PanelContainer *sourcePC)
{
  PanelContainer *targetPC = this;

  dprintf("movePanelsToNewPanelContainer targetPC=(%s)\n", targetPC->getInternalName() );

  if( sourcePC == NULL || sourcePC == targetPC || 
      sourcePC->tabWidget == NULL )
  {
    return;
  }

  QWidget *currentPage = sourcePC->tabWidget->currentPage();
  Panel *p = sourcePC->getRaisedPanel();
  if( !p )
  {
    fprintf(stderr, "Error: Couldn't locate a panel to drag.\n");
    return;
  }

  QPoint point;
  QWidget *widget_to_reparent = currentPage;
  int original_index = sourcePC->tabWidget->currentPageIndex();

  dprintf("I think there are %d panels to move ", sourcePC->tabWidget->count() );
 
  dprintf("onto targetPC=(%s)\n", targetPC->getInternalName() );

  QWidget *w = targetPC->dropSiteLayoutParent;

  if( targetPC->dropSiteLayout == NULL )
  {
    targetPC->dropSiteLayout = new QVBoxLayout( w, 0, 0, "dropSiteLayout");
    targetPC->tabWidget = new QTabWidget( w, "tabWidget" );
  
    targetPC->dropSiteLayout->addWidget( targetPC->tabWidget );
  }

  // Just to make sure the targetPC has an empty panelList
  targetPC->panelList.clear();

  if( !sourcePC->panelList.empty() )
  {
    int i = 0;
    dprintf("we're going to try to move these panels:\n");
    for( PanelList::Iterator pit = sourcePC->panelList.begin();
             pit != sourcePC->panelList.end();
             ++pit )
    {
      Panel *p = (Panel *)*pit;
      dprintf("  try to move p (%s)\n", p->getName() );
      widget_to_reparent = currentPage = sourcePC->tabWidget->page(i);
      QWidget *panel_base = (QWidget *)p;
      p->panelContainer = targetPC;
      p->baseWidgetFrame->panelContainer = targetPC;
      p->baseWidgetFrame->reparent(targetPC->dropSiteLayoutParent, 0,
                                   point, TRUE);
      panel_base->reparent((QWidget *)targetPC, 0, point, TRUE);
      widget_to_reparent->reparent(targetPC->tabWidget, 0, point, TRUE);
      targetPC->tabWidget->addTab( currentPage, p->getName() );
//      if( targetPC->panelList == NULL )
//      {
//        fprintf(stderr, "Destination panelContainer is NULL.\n");
//        fprintf(stderr, "This should never happen.\n");
//        targetPC->panelList = new PanelList();
//        targetPC->panelList->clear();
//      }
      targetPC->panelList.push_back(p);
      p->getBaseWidgetFrame()->show();
      targetPC->dropSiteLayoutParent->show();
      sourcePC->tabWidget->removePage(currentPage);
    }
    sourcePC->dropSiteLayoutParent->hide();
    targetPC->handleSizeEvent(NULL);
  }

  int count = targetPC->tabWidget->count();
  dprintf("we've moved %d panels\n", count);
  if( count > 0 )
  {
    targetPC->tabWidget->setCurrentPage(original_index);
  }

  sourcePC->panelList.clear();

  dprintf("\n\n\n");
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

void
PanelContainer::traversePanelContainerTree()
{
printf("PanelContainer::traversePanelContainerTree() entered.\n");
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = _panelContainerList->begin();
               it != _panelContainerList->end();
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

void
PanelContainer::printPanelContainer(int depth)
{
  char indent_buffer[1024];
  indent_buffer[0] = '\0';

  if( leftPanelContainer && rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    printf("%s(%s-%s) o=%d w=%d h=%d mfd=%d menuEnabled=%d depth=%d\n",
       indent_buffer, getInternalName(), getExternalName(),
       splitter->orientation(), width(), height(),
       markedForDelete, menuEnabled, _depth );
  } else if( !leftPanelContainer && !rightPanelContainer )
  {
    indentString(depth,indent_buffer);
    printf("%s(%s-%s) o=%d w=%d h=%d mfd=%d menuEnabled=%d depth=%d\n",
      indent_buffer, getInternalName(), getExternalName(),
      splitter->orientation(), width(), height(),
      markedForDelete, menuEnabled, _depth );
    printf("%s  leftFrame->panelContainer=(%s)\n",  indent_buffer,
      leftFrame->panelContainer ? leftFrame->panelContainer->getInternalName() : "" );
    printf("%s  rightFrame->panelContainer=(%s)\n",  indent_buffer,
      rightFrame->panelContainer ? rightFrame->panelContainer->getInternalName() : "" );
    printPanels(indent_buffer);
  }
}

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
    printf("%s    Panel (%s) pc=(%s)\n", buffer, p->getName(), p->panelContainer->getInternalName() );
  }
}

#include <qobjcoll.h>
#include <qobject.h>
void
PanelContainer::debugPanel()
{
  printf("here's the tree.\n");

//  ((PanelContainer *)topPC)->traversePanelContainerTree();
  _topPC->traversePanelContainerTree();
}
// End Debug routines


void
PanelContainer::panelContainerContextMenuEvent( PanelContainer *targetPC )
{
dprintf("PanelContainer::contextMenuEvent(%s-%s) targetPC=(%s-%s)\n", getInternalName(), getExternalName(), targetPC->getInternalName(), targetPC->getExternalName() );
  if( this != _topPC )
  {
    fprintf(stderr, "PanelContainer::contextMenuEvent() Not master pc\n");
    return;
  }


  if( !targetPC->menuEnabled )
  {
    return;
  }

  if( _doingMenuFLAG == TRUE )
  {
    return;
  }

  _doingMenuFLAG = TRUE;

#ifdef NO_CASCADING
  QPopupMenu *contextMenu = new QPopupMenu( this );
  Q_CHECK_PTR( contextMenu );
  contextMenu->insertItem( "Split &Horizontal",  targetPC, SLOT(splitHorizontal()), CTRL+Key_H );
  contextMenu->insertItem( "Split &Vertical", targetPC, SLOT(splitVertical()), CTRL+Key_V );
  contextMenu->insertItem( "&Remove Container", targetPC, SLOT(removePanelContainer()), CTRL+Key_R );
  contextMenu->insertItem( "Remove Raised &Panel", targetPC, SLOT(removeRaisedPanel()), CTRL+Key_P );
  contextMenu->insertItem( "&Debug Panel", targetPC, SLOT(debugPanel()), CTRL+Key_D );

  // Now call the Panel's menu() function to add it's menus (if any).
  targetPC->addPanelMenuItems(contextMenu);
#else // NO_CASCADING
  QPopupMenu *pcMenu = new QPopupMenu( this );
  Q_CHECK_PTR( pcMenu );
  pcMenu->insertItem( "Split &Horizontal",  targetPC, SLOT(splitHorizontal()), CTRL+Key_H );
  pcMenu->insertItem( "Split &Vertical", targetPC, SLOT(splitVertical()), CTRL+Key_V );
  pcMenu->insertItem( "&Remove Container", targetPC, SLOT(removePanelContainer()), CTRL+Key_R );
  pcMenu->insertItem( "Remove Raised &Panel", targetPC, SLOT(removeRaisedPanel()), CTRL+Key_P );
  pcMenu->insertItem( "&Debug Panel", targetPC, SLOT(debugPanel()), CTRL+Key_D );
  QPopupMenu *contextMenu = new QPopupMenu( this );
  contextMenu->insertItem("&Panel Container Menu", pcMenu, CTRL+Key_P );

  // Now call the Panel's menu() function to add it's menus (if any).
  if( !targetPC->addPanelMenuItems(contextMenu) )
  {  // There were no panel menus... Show the panel Container menus
     // without cascading...
    delete contextMenu;
    contextMenu = pcMenu;
  }
#endif // NO_CASCADING

  contextMenu->exec( QCursor::pos() );

  _doingMenuFLAG = FALSE;

  delete contextMenu;
}

void
noop_printf()
{
}
