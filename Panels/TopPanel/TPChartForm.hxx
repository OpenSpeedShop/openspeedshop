#ifndef MYCHARTFORM_H
#define MYCHARTFORM_H

class TopPanel;

#include "chartform.hxx"

#include <qwidget.h>
#include <qtextedit.h>
#include <qsplitter.h>

//! Overrides the Chart implementation to allow context menu event.
class TPChartForm : public ChartForm
{
public:
  TPChartForm( TopPanel *sp, QSplitter *splitter, const char *name=0, int flags=0 );

  ~TPChartForm( );


  TopPanel *topFivePanel;

int mouseClicked( int );
void contentsContextMenuEvent( QContextMenuEvent *e );
  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

public slots:

private:
};
#endif // MYCHARTFORM_H
