#ifndef WHATSTHIS_H
#define WHATSTHIS_H

#include <qwhatsthis.h>

class WhatsThis : public QWhatsThis
{
public:
    WhatsThis( QWidget *widget=0 );

    ~WhatsThis( );

     void hide( QObject *obj );
  public slots:

  protected slots:

private:
};
#endif // WHATSTHIS_H
