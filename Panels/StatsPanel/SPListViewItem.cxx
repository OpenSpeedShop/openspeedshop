/*! \class SPListViewItem
    This class overloads the chart widget to allow the contents menu event
    to be recognized.
 */
#include "SPListViewItem.hxx"

#include <qpopupmenu.h>
#include <qcursor.h>
#include "StatsPanel.hxx"

#include "debug.hxx"

#include <stdlib.h>  // for the free() call below.

/*! Work constructor.   Set's the name of the frame, the pointer to the
    parent panel container, and the frame shape and shadow characteristics.
*/
SPListViewItem::SPListViewItem( StatsPanel *sp, QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  statsPanel = sp;
}

SPListViewItem::SPListViewItem( StatsPanel *sp, SPListViewItem * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
     : QListViewItem( (SPListViewItem *)parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
  statsPanel = sp;
}

/*! The default destructor. */
SPListViewItem::~SPListViewItem( )
{
  // default destructor.
  dprintf ("  SPListViewItem::~SPListViewItem( ) destructor called\n");
}

QString
SPListViewItem::key(int c, bool b) const
{
  dprintf("GROWL! c=%d\n", c);
  QString s;
  if( statsPanel->metricHeaderTypeArray[c] == INT_T )
  {
    /* sorting by int */
    dprintf("sort by int\n");
    s.sprintf("%08d",text(c).toInt());
  } else if( statsPanel->metricHeaderTypeArray[c] == FLOAT_T )
  {
    dprintf("sort by float %07.2lf\n", text(c).toDouble() );
    /* sorting by float */
    s.sprintf("%07.2lf",text(c).toDouble());
  } else
  {
    /* sorting alphanumeric */
    dprintf("sort by alphanumeric\n");
    s.sprintf("%s",text(c).ascii());
  }
    
  return s;
}

#ifdef HOLD
int
SPListViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
  printf("GRAAAAOOOOWL!\n");

  return key( col, ascending ).compare( i->key( col, ascending) );
}
#endif // HOLD
