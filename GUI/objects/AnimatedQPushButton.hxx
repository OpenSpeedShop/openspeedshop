#ifndef ANIMATEDQPUSHBUTTON_H
#define ANIMATEDQPUSHBUTTON_H

#include <qpushbutton.h>
#include <qvaluelist.h>

typedef QValueList<QPixmap *>ImageList;

class AnimatedQPushButton : public QPushButton
{
  public:
    AnimatedQPushButton(QWidget *, const char *, bool f=TRUE);

    ~AnimatedQPushButton();

    void push_back(QPixmap *);
    void remove(QPixmap *);

    ImageList imageList;

    void enterEvent ( QEvent * ); 
    void leaveEvent ( QEvent * );

    bool enabledFLAG;
  public slots:

  private:
};
#endif // ANIMATEDQPUSHBUTTON_H
