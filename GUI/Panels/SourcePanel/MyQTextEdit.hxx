#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

class SourcePanel;

#include <qwidget.h>
#include <qtextedit.h>

class MyQTextEdit : public QTextEdit
{
public:
    MyQTextEdit( );

    //! MyQTextEdit(SourcePanel *sp, QWidget *parent, const char *name)
    MyQTextEdit( SourcePanel *sp, QWidget *parent=0, const char *name=0 );

    ~MyQTextEdit( );


    SourcePanel *sourcePanel;

    QPopupMenu* createPopupMenu( const QPoint & pos );
    QPopupMenu* createPopupMenu( ) { /* obsoleted function. */ return NULL; };

public slots:

private:
};
#endif // MYQTEXTEDIT_H
