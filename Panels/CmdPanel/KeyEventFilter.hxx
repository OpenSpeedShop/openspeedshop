#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qevent.h>
#include <qtimer.h>
#include <qpoint.h>

#include "CmdPanel.hxx"
class CmdPanel;

class KeyEventFilter : public QObject
{
  public:
    //! The working constructor for KeyEventFilter(...)
    /*! This constructor is the work constructor for KeyEventFilter.
     */
    KeyEventFilter(QObject *t, CmdPanel *cp);
    //! Default destructor
    /*! Nothing extra is allocatated, nothing extra is destroyed.
     */
    ~KeyEventFilter() {};
  private:
    bool eventFilter( QObject *o, QEvent *e );
    CmdPanel *cmdPanel;
public slots:
};
#endif // KEYEVENTFILTER_H
