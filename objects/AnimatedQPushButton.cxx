#include <qpushbutton.h>
#include <qbitmap.h>
#include "AnimatedQPushButton.hxx"
#include "debug.hxx"

AnimatedQPushButton::AnimatedQPushButton(QWidget *p, const char *n, bool f) : QPushButton( p, n)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("AnimatedQPushButton::AnimatedQPushButton() constructor called\n");
  enabledFLAG = f;
  imageList.clear();
}

AnimatedQPushButton::~AnimatedQPushButton()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("  AnimatedQPushButton::~AnimatedQPushButton() destructor called\n");
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

  if( enabledFLAG == TRUE )
  {
    setFlat(FALSE);
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

  if( enabledFLAG == TRUE )
  {
    setFlat(TRUE);
  }


  ImageList::Iterator it = imageList.begin();
  QPixmap *pm = (QPixmap *)*it;
  
  this->setPixmap( *pm );
  QPushButton::paintEvent( (QPaintEvent *)e);
}
