#ifndef SPCANVASVIEW_H
#define SPCANVASVIEW_H

#include <qwidget.h>
#include <qcanvas.h>

class SPCanvasView : public QCanvasView
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPCanvasView( QCanvas *canvas=0, QWidget *parent = 0, const char *name=0 );
    ~SPCanvasView( );
protected: 
    void viewportResizeEvent( QResizeEvent *e );

public slots:

private:

    QCanvas *canvas;
};
#endif // SPCANVASVIEW_H
