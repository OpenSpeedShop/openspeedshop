/*! \class SPListView
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "SPListView.hxx"

#include <qpopupmenu.h>
#include <qcursor.h>
#include "StatsPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPListView::SPListView( StatsPanel *rp, QWidget *w, const char *n, int flags )
     : QListView( w, n, flags )
{
  resultsPanel = rp;
  dprintf ( "SPListView::SPListView( ) constructor called\n");
}

/*! The default destructor. */
SPListView::~SPListView( )
{
  // default destructor.
  dprintf ("  SPListView::~SPListView( ) destructor called\n");
}

/*! Create the local popup menu for the list items. */
void
SPListView::contentsContextMenuEvent( QContextMenuEvent *e)
{
  dprintf ("SPListView::contentsContextMenuEvent() entered\n");

  createPopupMenu( QCursor::pos() );
}

QPopupMenu*
SPListView::createPopupMenu( const QPoint & pos )
{
  dprintf ("SPListView: Hello from down under the hood.\n");

  // First create the default Qt widget menu...
//  QPopupMenu *popupMenu = SPListView::createPopupMenu(pos);
  QPopupMenu *popupMenu = new QPopupMenu(this);

  if( resultsPanel->createPopupMenu(popupMenu, pos) )
  {
    popupMenu->exec( pos );
    return popupMenu;
  }

  delete popupMenu;

  return NULL;
}
