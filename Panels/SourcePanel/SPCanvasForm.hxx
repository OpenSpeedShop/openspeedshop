#ifndef SPCANVAS_H
#define SPCANVAS_H

#include <qwidget.h>
#include <qlayout.h>
#include <qcanvas.h>
#include <qfont.h>
#include <qvaluelist.h>
#include <qheader.h>

#include "SPCanvasForm.hxx"
#include "SPCanvasView.hxx"

#define DEFAULT_CANVAS_WIDTH 30

typedef QValueList<QCanvasText *> CanvasTextList;
class SPCanvasForm : public QWidget
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPCanvasForm( int label_height, QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~SPCanvasForm( );

    int numColumns;
    QVBoxLayout * canvasFormLayout;
    QCanvas *canvas;
    SPCanvasView *canvasView;
    QHBoxLayout * canvasFormHeaderLayout;
    QHeader *header;

    QCanvasItemList canvasItemList; // Not used anymore...
    CanvasTextList canvasTextList;
    void clearAllItems();
    void setHighlights(QFont font, int lineHeight,
                       int lastTop, int visibleLines,
                       int line_count = 0, int top_offset=-2);
protected: 

public slots:

private:
};
#endif // SPCANVAS_H
