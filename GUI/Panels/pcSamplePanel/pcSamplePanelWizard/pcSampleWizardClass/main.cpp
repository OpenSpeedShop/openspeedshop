#include <qapplication.h>
#include "pcsample.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    pcsample w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
