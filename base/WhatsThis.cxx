/*! \class WhatsThis
    Overloaded function so we can trap hook into the hide() function.
 */

#include <qapplication.h>
#include <qevent.h>
#include "WhatsThis.hxx"

#include "debug.hxx"  // This includes the definition of nprintf(DEBUG_PANELCONTAINERS) 

WhatsThis::WhatsThis( QWidget *widget )
    : QWhatsThis( widget )
{
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis constructor called.\n");
}

/*! The default destructor. */
WhatsThis::~WhatsThis( )
{
  // default destructor.
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis Destructor called.\n");
}

void
WhatsThis::hide( QObject *obj )
{
  nprintf(DEBUG_PANELCONTAINERS) ("WhatsThis::hide() entered\n");

  QMouseEvent me( QEvent::MouseButtonPress, QCursor::pos(), 0, 0 );
  QApplication::sendEvent( obj, &me );
}
