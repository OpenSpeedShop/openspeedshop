#ifndef DRAGNDROPPANEL_H
#define DRAGNDROPPANEL_H

#include "debug.hxx"

class PanelContainer;
class Frame;

#include <qwidget.h>
#include <qframe.h>
#include <qdragobject.h>

#include <qcursor.h>


//! Defines drag and drop functions.
class DragNDropPanel : public QStoredDrag
{
public:
    //! DragNDropPanel(PanelContainer *pc, Frame *sourceFrame)
    DragNDropPanel( const char *mimeType, PanelContainer *sourcePC, Frame *sourceFrame, char *name="DragNDropPanel" );

    //! ~Panel() - The default destructor.
    ~DragNDropPanel( );

    //! This is an internal handle to the frame that initiated the drag.
    Frame *frame;

    //! This is an internal handle to the PanelContainer initiating the drag.
    PanelContainer *panelContainer;

    //! The drop panel callback routine when Qt drag-n-drop fails.
    void DropPanel(PanelContainer *sourcePC, bool doubleClickedFLAG=FALSE);

#ifndef OLD_DRAG_AND_DROP
    //! The drop panel callback routine called when Qt drag-n-drop succeeds
    void DropPanelWithQtDnD(PanelContainer *targetPC);
#endif // OLD_DRAG_AND_DROP

    //! A pointer to 'this' object. 
    /*! We can only ever have one of these.
     */
    static DragNDropPanel *sourceDragNDropObject;

    //! The cursor shape for dragging a Panel.
    QCursor *customDragNDropCursor;

public slots:

private:
};
#endif // DRAGNDROPPANEL_H
