#ifndef SPTEXTEDIT_H
#define SPTEXTEDIT_H

class CmdPanel;

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
    //! SPTextEdit(CmdPanel *sp, QWidget *parent, const char *name)
    SPTextEdit( CmdPanel *sp, QWidget *parent=0, const char *name=0 );

    ~SPTextEdit( );

protected: 
    void contentsMouseMoveEvent( QMouseEvent *e );

public slots:

private:
    CmdPanel *cmdPanel;
};
#endif // SPTEXTEDIT_H
