/*! \class TabBarWidget
    This class manages mouse events of the QTabBarWidget.
 */

#include "TabWidget.hxx"
#include "TabBarWidget.hxx"

#include "PanelContainer.hxx"

#include "DragNDropPanel.hxx"

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

/*! Unused. */
TabBarWidget::TabBarWidget( )
{
  fprintf(stderr, "TabBarWidget::TabBarWidget() should not be called.\n");
  fprintf(stderr, "see: TabBarWidget::TabBarWidget( PanelContainer *pc, QWidget *parent, const char *n );\n");
  setPanelContainer(NULL);
}

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
TabBarWidget::TabBarWidget( PanelContainer *pc, QWidget *parent, const char *n )
    : QTabBar( parent, n )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabBarWidget constructor called.\n");

  setPanelContainer(pc);

  languageChange();
}

/*! Nothing was allocated.   Nothing needs to be deleted. */
TabBarWidget::~TabBarWidget( )
{
  // default destructor.
  nprintf(DEBUG_PANELCONTAINERS) ("TabBarWidget Destructor called.\n");
}

/*! This is how a tab is placed into it's own panel container.   It's another
    shortcut for dragging and dropping onto the desktop. */
void
TabBarWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
  nprintf(DEBUG_PANELCONTAINERS) ("you double clicked.\n");

  // First find the associated Frame.
  Frame::dragging = TRUE;

  DragNDropPanel::sourceDragNDropObject = new DragNDropPanel("OpenSpeedShop-Drag-N-Drop-Event", getPanelContainer(), getPanelContainer()->leftFrame);
  if( DragNDropPanel::sourceDragNDropObject == NULL )
  {
    Frame::dragging = FALSE;
  }

  DragNDropPanel::sourceDragNDropObject->DropPanel(getPanelContainer(), TRUE);

  Frame::dragging = FALSE;
}

/*! This is how drag is enabled for a panel.  (if the dragEnable flag is set.)
    The sourceDragNDropObject is created and a global flag is set notifying 
    everyone that a drag is undeway.
 */
void TabBarWidget::mousePressEvent(QMouseEvent *e)
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabBarWidget::mousePressEvent()\n");

  nprintf(DEBUG_PANELCONTAINERS) ("e->pos().x()=%d e->pos().y()=%d\n", e->pos().x(), e->pos().y() );

  if( getPanelContainer() == NULL )
  {
    return;
  }

  if( getPanelContainer()->leftPanelContainer && getPanelContainer()->rightPanelContainer)
  {  // There should be no menu action for this split panel container.  Only
     // for it's children.
    nprintf(DEBUG_PANELCONTAINERS) ("  There are children.  - return\n");
     return;
  }

  if( getPanelContainer()->getMasterPC()->_doingMenuFLAG == TRUE )
  {
    nprintf(DEBUG_PANELCONTAINERS) ("  already doing menu - return\n");
    return;
  }

  nprintf(DEBUG_PANELCONTAINERS) ("  pc=(%s)(%s)\n", getPanelContainer()->getInternalName(), getPanelContainer()->getExternalName() );

  QTab *selectedTab = NULL;
  selectedTab = QTabBar::selectTab( e->pos() );
  if( selectedTab )
  {
   nprintf(DEBUG_PANELCONTAINERS) ("pos() is the answer()\n");
  
    QTabBar::setCurrentTab(selectedTab);
  
    nprintf(DEBUG_PANELCONTAINERS) ("e->pos().x()=%d e->pos().y()=%d\n", e->pos().x(), e->pos().y() );
    nprintf(DEBUG_PANELCONTAINERS) ("height()=%d\n", height() );
    
    QRect rect = selectedTab->rect();
    nprintf(DEBUG_PANELCONTAINERS) ("rect.left()=%d\n", rect.left() );

    if( e->button() == RightButton || e->pos().x()-rect.left() < height() )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("RightButton!\n");
      nprintf(DEBUG_PANELCONTAINERS) ("TabBarWidget() call the menu....\n");
      // The frame has all the locking for handling PanelContainer events.
      // Use that path...
      getPanelContainer()->leftFrame->contextMenuEvent( (QContextMenuEvent *)e, TRUE );
    } else if( e->button() == LeftButton )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("LeftButton!\n");
      getPanelContainer()->dragRaisedPanel();
    }
  }

  return;
}

/*!  Sets the strings of the subwidgets using the current language.
 */
void
TabBarWidget::languageChange()
{ 
  setCaption( tr( "TabBarWidget" ) );
} 

