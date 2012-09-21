////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef INFOEVENTFILTER_H
#define INFOEVENTFILTER_H

#include <cstddef>
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
