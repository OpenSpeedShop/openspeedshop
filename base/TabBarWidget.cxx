/*! \class TabBarWidget
    This class manages mouse events of the QTabBarWidget.
 */

#include "TabWidget.hxx"
#include "TabBarWidget.hxx"

#include "PanelContainer.hxx"

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
 
  nprintf(DEBUG_PANELCONTAINERS) ("  pc=(%s)(%s)\n", getPanelContainer()->getInternalName(), getPanelContainer()->getExternalName() );

  QTab *selectedTab = NULL;
  selectedTab = QTabBar::selectTab( e->pos() );
  if( selectedTab )
  {
   nprintf(DEBUG_PANELCONTAINERS) ("pos() is the answer()\n");
  
    QTabBar::setCurrentTab(selectedTab);
  //  int id = selectedTab->currentTab();
  
  // nprintf(DEBUG_PANELCONTAINERS) ("current id=%d\n", id);
  
    if( e->button() == RightButton )
    {
      nprintf(DEBUG_PANELCONTAINERS) ("RightButton!\n");
      nprintf(DEBUG_PANELCONTAINERS) ("TabBarWidget() call the menu....\n");
      getPanelContainer()->panelContainerContextMenuEvent( getPanelContainer(), TRUE );
      return;
    }
    if( e->button() == LeftButton )
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

