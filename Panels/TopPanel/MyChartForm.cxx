/*! \class MyChartForm
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "MyChartForm.hxx"

#include <qpopupmenu.h>
#include "TopPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
MyChartForm::MyChartForm( TopPanel *t5p, QSplitter *splitter, const char *n, int flags )
    : ChartForm( splitter, n, flags )
{
  topFivePanel = t5p;
  nprintf(DEBUG_CONST_DESTRUCT) ( "MyChartForm::MyChartForm( ) constructor called\n");
}

/*! The default destructor. */
MyChartForm::~MyChartForm( )
{
  // default destructor.
  nprintf(DEBUG_CONST_DESTRUCT) ("  MyChartForm::~MyChartForm( ) destructor called\n");
}

int
MyChartForm::mouseClicked( int item )
{
  nprintf(DEBUG_PANELS) ("MyChartForm::mouseClicked(item=%d) called.\n", item);
  topFivePanel->itemSelected(item);
}

void
MyChartForm::contentsContextMenuEvent( QContextMenuEvent *e)
{
  nprintf(DEBUG_PANELS) ("MyChartForm::contentsContextMenuEvent() entered\n");

  createPopupMenu( e->pos() );
}

QPopupMenu*
MyChartForm::createPopupMenu( const QPoint & pos )
{
  nprintf(DEBUG_PANELS) ("MyChartForm: Hello from down under the hood.\n");

  // First create the default Qt widget menu...
  QPopupMenu *popupMenu = ChartForm::createPopupMenu(pos);

  // Now create the panel specific menu... and add it to the popup as
  // a cascading menu.
  QPopupMenu *panelMenu = new QPopupMenu(this);
  topFivePanel->menu(panelMenu);
  popupMenu->insertSeparator();
  popupMenu->insertItem("&Panel Menu", panelMenu, CTRL+Key_P );

#ifdef ANOTHER_WAY
  // Now look to see if there are any dynamic menus to add.
  popupMenu->insertSeparator();
  topFivePanel->createChartPopupMenu(popupMenu, pos);
#endif // ANOTHER_WAY

  popupMenu->exec( pos );

  delete (panelMenu);

  return popupMenu;
}
