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
