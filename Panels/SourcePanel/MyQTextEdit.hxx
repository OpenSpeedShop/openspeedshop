#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

class SourcePanel;

#define FINGERLINGER  1

#include <qwidget.h>
#include <qtextedit.h>

//! Simply overloads the QTextEdit so we can detect the context menu event.
class MyQTextEdit : public QTextEdit
{
  //! Q_OBJECT is needed as there are slots defined for the class
  Q_OBJECT
public:
    MyQTextEdit( );

    //! MyQTextEdit(SourcePanel *sp, QWidget *parent, const char *name)
    MyQTextEdit( SourcePanel *sp, QWidget *parent=0, const char *name=0 );

    ~MyQTextEdit( );


    SourcePanel *sourcePanel;

    QPopupMenu* createPopupMenu( const QPoint & pos );
    QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

    QPoint lastPos;
protected: 
#ifdef FINGERLINGER
    void contentsMouseMoveEvent( QMouseEvent *e );
#endif // FINGERLINGER

public slots:

private:
};
#endif // MYQTEXTEDIT_H
