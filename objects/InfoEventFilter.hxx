#ifndef INFOEVENTFILTER_H
#define INFOEVENTFILTER_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qevent.h>
#include <qtimer.h>
#include <qpoint.h>

class Panel;
class PanelContainer;



class InfoEventFilter : public QObject
{
  //! Q_OBJECT is needed as there are slots defined for the Panel Class
   Q_OBJECT
  public:
    //! Default contructor for a InfoEventFilter.   
    /*! It should never be called and is only here for completeness.
     */
    InfoEventFilter() {};
    //! The working constructor for InfoEventFilter(...)
    /*! This constructor is the work constructor for InfoEventFilter.
     */
    InfoEventFilter(QObject *t, Panel *panel);
    //! Default destructor
    /*! Nothing extra is allocatated, nothing extra is destroyed.
     */
    ~InfoEventFilter() {};
  private:
    void armWhatsThis();
    //! Filter unwanted events based on flags in the master PanelContainer.
    /*! Ignore the mouse and enter leave events.    Not ignoring these caused
        the PanelContainer removal logic fits as events were being fielded
        while we were trying to remove the PanelContainer objects.   Aborts 
        were the result.  
        
        This simple filter simply ignores the events from the beginning of the
        PanelContainer::removePanelContainer() to the end of the cleanup.
     */
    bool eventFilter( QObject *o, QEvent *e );
    //! A pointer to the User Panel
    Panel *panel;

    //! A pointer to the Master PanelContainer
    PanelContainer *_masterPC;

    //! A pointer to the target Object  (The one sending the event.)
    QObject *target;

public slots:
  void wakeupFromSleep();
  void popupInfo();
};
#endif // INFOEVENTFILTER_H
