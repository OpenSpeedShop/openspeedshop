#ifndef SPLISTVIEW_H
#define SPLISTVIEW_H

class StatsPanel;

#include "qlistview.h"
#include <qpopupmenu.h>

#include <qwidget.h>

class SPListView : public QListView
{
public:
  SPListView( StatsPanel *rp, QWidget *w, const char *name=0, int flags=0 );

  ~SPListView( );


  StatsPanel *resultsPanel;

  void contentsContextMenuEvent( QContextMenuEvent *e );
  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

public slots:

private:
};
#endif // SPLISTVIEW_H
