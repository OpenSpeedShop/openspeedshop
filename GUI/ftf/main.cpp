#include "performanceleader.h"

// #include "splash/.ui/splashclass.h"
#include "unistd.h"

#include <qvaluelist.h>

#include <qapplication.h>
#include "PanelContainer.hxx"

QApplication *qapplication = NULL;

int main( int argc, char ** argv )
{
    qapplication = new QApplication( argc, argv );

    PerformanceLeader w;
    w.show();
    qapplication->connect( qapplication, SIGNAL( lastWindowClosed() ), qapplication, SLOT( quit() ) );

    return( qapplication->exec() );
}
