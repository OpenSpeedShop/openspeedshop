#ifndef FRAME_H
#define FRAME_H

class PanelContainer;

#include "debug.hxx"
#include <qwidget.h>
#include <qframe.h>

#include "debug.hxx"

//! Frame Class
/*! Frame Class is the container class for all user PanelContainers.   
    The events handled are:
    - contextMenuEvent
      QContextMenuEvent is passed both to the PanelContainer to create
      it's menu, but also to the Panel to add any additional Panel specifi
      menu options.
    - resizeEvent
      This is passed trough to the Frame's parent PanelContainer for fielding.
    - dropEnterEvent
      Currently called but nothing happens
    - dropEnter
      This is the worker event for the drop event.
 */

class Frame : public QFrame
{
    //! Q_OBJECT is needed as there are slots defined for the Frame Class
    Q_OBJECT
public:
    //! Frame() - The default constructor the the Frame Class (unused).
    Frame( );

    //! Frame(PanelContainer *pc, QWidget *parent, const char *name)
    Frame( PanelContainer *pc, QWidget *parent=0, const char *name=0 );

    //! ~Frame() - The default destructor.
    ~Frame( );


    //! Set to TRUE if this Frame is able to be dragged.
    bool dragEnabled;

    //! Set to TRUE if this Frame is able to accept drops.
    bool dropEnabled;

    //! Convienience member function to return the name of this Frame.
    char *getName() { return name; }

    //! The parent PanelContainer of this Frame.
    PanelContainer *panelContainer;

    //! A static flag indicating that a drag is taking place.
    static bool dragging;

  protected slots:
    //! The standard Qt slot to change language information.
    virtual void languageChange();

  private:
    //! The event callback for the menu to be create.
    void contextMenuEvent ( QContextMenuEvent * );

    //! The event callback that a resize needs to be handle.
    void resizeEvent ( QResizeEvent * );

#ifdef  OLDWAY  // Left here for debugging...
    void enterEvent ( QEvent * );
    void leaveEvent ( QEvent * );
#endif // OLDWAY

#ifdef OLD_DRAG_AND_DROP
    //! The event callback that triggers the drag event.
    void mousePressEvent(QMouseEvent *e=NULL);

    //! The event callback that triggers the drop event.
    void mouseReleaseEvent(QMouseEvent *e=NULL);
#endif // OLD_DRAG_AND_DROP
#ifndef OLD_DRAG_AND_DROP
    //! The event called when Qt Drag-and-Drop enter event is fired. 
    void dragEnterEvent(QDragEnterEvent* event);

    //! The event called when Qt Drag-and-Drop drop event is fired. 
    void dropEvent(QDropEvent* event);
#endif // OLD_DRAG_AND_DROP

    //! The name of this Frame.
    char *name;
};
#endif // FRAME_H
