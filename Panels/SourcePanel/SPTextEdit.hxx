#ifndef SPTEXTEDIT_H
#define SPTEXTEDIT_H

class SourcePanel;

#include <qwidget.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qpixmap.h>

//! Simply overloads the QTextEdit so we can detect the context menu event.
class SPTextEdit : public QTextEdit
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    SPTextEdit( );

    //! SPTextEdit(SourcePanel *sp, QWidget *parent, const char *name)
    SPTextEdit( SourcePanel *sp, QWidget *parent=0, const char *name=0 );

    ~SPTextEdit( );

    QPopupMenu* createPopupMenu( const QPoint & pos );
    QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

    void annotateLine( int line, char *color );
protected: 
    void contentsMouseMoveEvent( QMouseEvent *e );

    void paintEvent( QPaintEvent *e );

public slots:

private:
    SourcePanel *sourcePanel;

    QScrollBar *vbar;
    QScrollBar *hbar;
    QPixmap *vannotatePixmap;
};
#endif // SPTEXTEDIT_H
