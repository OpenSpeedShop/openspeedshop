/*! \class TopWidget
    Overloaded function so we can trap the closeEvent.
 */

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

  panelContainer->_masterPC->removePanelContainer(panelContainer);

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

