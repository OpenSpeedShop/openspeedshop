#include <qapplication.h>
#include "wizardform.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    WizardForm w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
