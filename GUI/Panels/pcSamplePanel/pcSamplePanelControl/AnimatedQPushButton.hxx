#ifndef ANIMATEDQPUSHBUTTON_H
#define ANIMATEDQPUSHBUTTON_H

#include <qpushbutton.h>
#include <qvaluelist.h>

typedef QValueList<QPixmap *>ImageList;

//! A simple animated push button.
class AnimatedQPushButton : public QPushButton
{
  public:
    AnimatedQPushButton(QWidget *, const char *);

    ~AnimatedQPushButton();

    void push_back(QPixmap *);
    void remove(QPixmap *);

    ImageList imageList;

    void enterEvent ( QEvent * ); 
    void leaveEvent ( QEvent * );
  public slots:

  private:
};
#endif // ANIMATEDQPUSHBUTTON_H
