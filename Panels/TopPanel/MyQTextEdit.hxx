#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

class TopPanel;

#include <qwidget.h>
#include <qtextedit.h>

class MyQTextEdit : public QTextEdit
{
public:
  MyQTextEdit( );

  //! MyQTextEdit(TopPanel *sp, QWidget *parent, const char *name)
  MyQTextEdit( TopPanel *sp, QWidget *parent=0, const char *name=0 );

  ~MyQTextEdit( );


  TopPanel *topFivePanel;

  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

protected: 
    void contentsMouseMoveEvent( QMouseEvent *e );

public slots:

private:
};
#endif // MYQTEXTEDIT_H
