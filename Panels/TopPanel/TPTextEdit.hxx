#ifndef TPTEXTEDIT_H
#define TPTEXTEDIT_H

class TopPanel;

#include <qwidget.h>
#include <qtextedit.h>

class TPTextEdit : public QTextEdit
{
public:
  TPTextEdit( );

  //! TPTextEdit(TopPanel *sp, QWidget *parent, const char *name)
  TPTextEdit( TopPanel *sp, QWidget *parent=0, const char *name=0 );

  ~TPTextEdit( );


  QPopupMenu* createPopupMenu( const QPoint & pos );
  QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

protected: 
    void contentsMouseMoveEvent( QMouseEvent *e );

public slots:

private:
  TopPanel *topFivePanel;

};
#endif // TPTEXTEDIT_H
