#include <stdio.h> 

#include <qapplication.h>
#include "openspeedshop.hxx"
#include <qvaluelist.h>

#include "PanelContainer.hxx"

QApplication *qapplication;

#include <qeventloop.h>
QEventLoop *qeventloop;

#include "splash.xpm"
#include <qsplashscreen.h>

extern "C" 
{
  void usage()
  {
    printf("openss, version - prepreprepreRelease 0.01\n");
    printf("usage: openss [-f executable] [-h host] [-x experiment_name]\n");
    printf("              [-p process_ID [-r thread_rank] ]\n");
    printf("              [-a \"command line args\"]\n\n");
    printf("  -f : executable being measured\n");
    printf("  -h : host to locate target executable or process\n");
    printf("  -p : pid of target process\n");
    printf("  -r : rank of \n");
    printf("  -a : quoted command line arguments to be passed to executable.\n");
    printf("\n");
    printf("Example:  openss\n");
    printf("   >insert example here.<\n");
  }
  int
  event_routine()
  {
//    printf("gui.so event_routine() entered\n");

    // We need to only process for a while, then let the cli handle it's 
    // events.
    qeventloop->processEvents(QEventLoop::AllEvents, 2000);
    return 1;
  }

  int
  gui_init(int argc, char **argv, int cliFLAG)
  {
    bool splashFLAG=TRUE;
// printf("gui.so gui_init(cliFLAG=%d) entered\n", cliFLAG);
  
    qeventloop = new QEventLoop();
    qapplication = new QApplication( argc, argv );

    QString hostStr = QString::null;
    QString executableStr = QString::null;
    QString argsStr = QString::null;
    QString pidStr = QString::null;
    QString rankStr = QString::null;
    QString expStr = QString::null;
    for(int i=0;i<argc;i++)
    {
      QString arg = argv[i];
      if( arg == "--no_splash" ||
          arg == "-ns" ) 
      {
        splashFLAG = FALSE;
      } else if( arg == "-f" )
      {  // Get the target executableName.
        executableStr = QString(argv[++i]);
      } else if( arg == "-h" )
      { // Get the target host (or host list)
        hostStr = QString(argv[++i]);
      } else if( arg == "-r" )
      { // attach to the rank (list) specified
        rankStr = QString(argv[++i]);
      } else if( arg == "-p" )
      { // attach to the proces (list) specified )
        pidStr = QString(argv[++i]);
      } else if( arg == "-x" )
      { // load the collector (experiment) 
        expStr = QString(argv[++i]);
      } else if( arg == "-a" )
      { // load the command line arguments (to the exectuable)
        for( ;i<argc;)
        {
          argsStr += QString(argv[++i]);
          argsStr += QString(" ");
        }
      } else if( arg == "-help" || arg == "--help" )
      {
        usage();
        return 0; // Failure to complete...
      } else if( !arg.contains("openspeedshop") )
      {
        printf("Unknown argument syntax: argument in question: (%s)\n", arg.ascii() );
        usage();
        return 0; // Failure to complete...
      }

    }
    if( cliFLAG )
    {
      splashFLAG = FALSE;
    }

    QPixmap *splash_pixmap = NULL;
    QSplashScreen *splash = NULL;
    if( splashFLAG )
    {
      splash_pixmap = new QPixmap( splash_xpm );
      splash = new QSplashScreen( *splash_pixmap );
      splash->setCaption("splash");
      splash->show();
      splash->message( "Loading plugins" );
      event_routine();
      splash->raise();
    }

#ifdef FONT_CHANGE
    QFont m_font = QFont("Helvetica", 9, QFont::Normal );
    m_font.setFixedPitch(TRUE);
    qapplication->setFont(m_font);
#endif // FONT_CHANGE
#ifdef FONT_CHANGE
    QFont m_font = QFont("Courier", 10, QFont::Normal );
    m_font.setFixedPitch(TRUE);
    qapplication->setFont(m_font);
#endif // FONT_CHANGE
if( cliFLAG )
{
  char *fontname = getenv("OPENSPEEDSHOP_FONTNAME");
  if( fontname )
  {
// printf("we have a fontname=(%s)\n", fontname );
    QFont m_font = QFont(fontname);
    m_font.setRawName(fontname);
    qapplication->setFont(m_font);
  }
}
//    printf("create QApplication(A)\n");

    OpenSpeedshop *w;
// fprintf(stderr, "gui_init() entered.  call new OpenSpeedshop()\n");
    w = new OpenSpeedshop();
    w->executableName = executableStr;
    w->pidStr = pidStr;
    w->rankStr = rankStr;
    w->expStr = expStr;
    w->hostStr = hostStr;
    w->argsStr = argsStr;
//    printf("create OpenSpeedshop()\n");

    w->show();
//    printf("show OpenSpeedshop()\n");

    qapplication->connect( qapplication, SIGNAL( lastWindowClosed() ), qapplication, SLOT( quit() ) );
//    printf("connect this up.\n");

    if( splashFLAG )
    {
      splash->raise();
      event_routine();
      sleep(1);
      splash->raise();
      splash->message( "Plugins loaded..." );
      event_routine();
      sleep(1);
      splash->raise();
      event_routine();
      sleep(1);
      splash->raise();
      event_routine();
      splash->finish(w);
      delete splash;
      delete splash_pixmap;
  }

    if( cliFLAG == 1 )
    { // If this was launch from cli mode...
      // It calls the qt event loop from within the cli event loop
    } else
    {
      qeventloop->exec();
    }

    return 1;
  }
}
