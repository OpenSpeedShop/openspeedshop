#include <qapplication.h>
#include "introwizardform.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    IntroWizardForm w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
