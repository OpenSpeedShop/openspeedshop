#include "TabWidget.hxx"

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

/*! This overloads QTabWidget so the tabbar can accept mouse down events. */

/*! default constructor.   Here for completeness only. (unused) */
TabWidget::TabWidget( )
{
  fprintf(stderr, "TabWidget::TabWidget() should not be called.\n");
  fprintf(stderr, "see: TabWidget::TabWidget(QWidget *parent, const char *n );\n");
}

/*! Work constructor.   Overloads the QTabWidget so we can accept mouse 
    events for tab specific menu callbacks.
*/
TabWidget::TabWidget( QWidget *parent, const char *n )
    : QTabWidget( parent, n )
{
  nprintf(DEBUG_PANELCONTAINERS) ("TabWidget constructor called.\n");

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

