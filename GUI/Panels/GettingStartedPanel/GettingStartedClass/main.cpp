#include <qapplication.h>
#include "gettingstartedclass.h"

#include "GettingStartedPanel.hxx"
GettingStartedPanel *gettingStartedPanel = NULL;

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    GettingStartedClass w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
