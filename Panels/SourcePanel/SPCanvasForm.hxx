#ifndef SPCANVAS_H
#define SPCANVAS_H

#include <qwidget.h>
#include <qlayout.h>
#include <qcanvas.h>
#include <qfont.h>
#include <qvaluelist.h>

#include "SPCanvasForm.hxx"
#include "SPCanvasView.hxx"

typedef QValueList<QCanvasText *> CanvasTextList;
class SPCanvasForm : public QWidget
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPCanvasForm( QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~SPCanvasForm( );

    QHBoxLayout * canvasFormLayout;
    QCanvas *canvasArea;
    SPCanvasView *canvasView;

    QCanvasItemList canvasItemList; // Not used anymore...
    CanvasTextList canvasTextList;
    void clearAllItems();
    void updateHighlights(QFont font, int lineHeight, int lastTop);
    void setHighlights(QFont font, int lineHeight, int lastTop, int visibleLines);
protected: 

public slots:

private:
};
#endif // SPCANVAS_H
