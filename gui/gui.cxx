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

#include <pthread.h>
#define PTMAX 10
pthread_t phandle[PTMAX];
#include "ArgClass.hxx"


extern "C" 
{
  // This routine starts another QApplication gui.  It is called from 
  // gui_init to have the gui started in it's own thread.
  void
  guithreadinit(void *ptr)
  {
    ArgStruct *arg_struct = (ArgStruct *)ptr;
    int argc = arg_struct->argc;
    char **argv = arg_struct->argv;
    bool splashFLAG=TRUE;
  
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
//        usage();
        return; // Failure to complete...
      } else if( arg == "-cli" )
      {
        // Valid: though not currently documented.   Just ignore 
        // and fall through.
      } else if( !arg.contains("openspeedshop") )
      {
        printf("Unknown argument syntax: argument in question: (%s)\n", arg.ascii() );
//        usage();
        return; // Failure to complete...
      }

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
  char *fontname = getenv("OPENSPEEDSHOP_FONTNAME");
  if( fontname )
  {
    QFont m_font = QFont(fontname);
    m_font.setRawName(fontname);
    qapplication->setFont(m_font);
  }

    OpenSpeedshop *w;

    w = new OpenSpeedshop();
    w->executableName = executableStr;
    w->pidStr = pidStr;
    w->rankStr = rankStr;
    w->expStr = expStr;
    w->hostStr = hostStr;
    w->argsStr = argsStr;

    if( w->expStr != NULL )
    {
      if( w->expStr == "pcsamp" )
      {
        w->topPC->dl_create_and_add_panel("pc Sampling");
      } else if( w->expStr == "usertime" )
      {
        w->topPC->dl_create_and_add_panel("User Time");
      } else if( w->expStr == "fpe" )
      {
        w->topPC->dl_create_and_add_panel("FPE Tracing");
      } else if( w->expStr == "hwc" )
      {
        w->topPC->dl_create_and_add_panel("HW Counter");
      } else if( w->expStr == "io" )
      { 
        w->topPC->dl_create_and_add_panel("IO");
      } else if( w->expStr == "mpi" )
      { 
        w->topPC->dl_create_and_add_panel("MPI");
      } else
      {
        fprintf(stderr, "Unknown experiment type.   Try using the IntroWizard.\n");
        exit(0);
      }
    }

    w->show();

    qapplication->connect( qapplication, SIGNAL( lastWindowClosed() ), qapplication, SLOT( quit() ) );

    if( splashFLAG )
    {
      splash->raise();
      sleep(1);
      splash->raise();
      splash->message( "Plugins loaded..." );
      sleep(1);
      splash->raise();
      sleep(1);
      splash->raise();
      splash->finish(w);
      delete splash;
      delete splash_pixmap;
    }

    qapplication->exec();

    return;
  }

  // This is simple the routine that is called to start the gui it's own 
  // thread.   Called from Start.cxx, when the gui is requested at startup.
  // Otherwise, it can be called from the command line at any time to 
  // initialize the gui.
  int
  gui_init( void *arg_struct )
  {
      int stat = pthread_create(&phandle[0], 0, (void *(*)(void *))guithreadinit,arg_struct);

      return 1;
  }

}
