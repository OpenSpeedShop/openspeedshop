#include <qpushbutton.h>
#include <qbitmap.h>
#include "AnimatedQPushButton.hxx"

/*! \class AnimatedQPushButton
    This animated push button simple takes as input a list of images.
    It then, upon a mouseEnterEvent, cycles though the pixmap list
    to do the animation.

    This is/was simple prototype code to demonstrate this possibility.

 */
AnimatedQPushButton::AnimatedQPushButton(QWidget *p, const char *n) : QPushButton( p, n)
{
  printf("AnimatedQPushButton::AnimatedQPushButton() constructor called\n");
  imageList.clear();
}

AnimatedQPushButton::~AnimatedQPushButton()
{
  printf("  AnimatedQPushButton::~AnimatedQPushButton() destructor called\n");
  imageList.clear();
}

void
AnimatedQPushButton::push_back(QPixmap *image)
{
  imageList.push_back(image);
}

void
AnimatedQPushButton::remove(QPixmap *image)
{
  imageList.remove(image);
}

void
AnimatedQPushButton::enterEvent ( QEvent *e )
{
//  printf("AnimatedQPushButton::enterEvent()\n");

  if( imageList.empty() )
  {
    return;
  }

  QPixmap *pm;

  for( ImageList::Iterator it = imageList.begin();
       it != imageList.end();  it++)
  {
    pm = (QPixmap *)*it;
    this->setPixmap( *pm );
    QPushButton::paintEvent( (QPaintEvent *)e);
  }
}

void
AnimatedQPushButton::leaveEvent ( QEvent *e )
{
//  printf("AnimatedQPushButton::leaveEvent()\n");

  if( imageList.empty() )
  {
    return;
  }


  ImageList::Iterator it = imageList.begin();
  QPixmap *pm = (QPixmap *)*it;
  
  this->setPixmap( *pm );
  QPushButton::paintEvent( (QPaintEvent *)e);
}
