#ifndef SPLISTVIEWITEM_H
#define SPLISTVIEWITEM_H

#include "qlistview.h"
#include <qpopupmenu.h>

#include <qwidget.h>

class StatsPanel;

class SPListViewItem : public QListViewItem
{
public:
  SPListViewItem( StatsPanel *sp, QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
  SPListViewItem( StatsPanel *sp, SPListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

  ~SPListViewItem( );

  QString key(int c, bool b) const;
 
#ifdef HOLD
  int compare( QListViewItem *i, int col, bool ascending ) const;
#endif // HOLD

   StatsPanel *statsPanel;

public slots:

private:
};
#endif // SPLISTVIEWITEM_H
