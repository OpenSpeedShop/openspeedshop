#include <stdio.h> 

#include <qapplication.h>
#include "performanceleader.hxx"
#include <qvaluelist.h>

#include "PanelContainer.hxx"

QApplication *qapplication;

#include <qeventloop.h>
QEventLoop *qeventloop;

#include "splash.xpm"
#include <qsplashscreen.h>

extern "C" 
{
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
    bool SPLASH=TRUE;
//    printf("gui.so gui_init(cliFLAG=%d) entered\n", cliFLAG);
  
    qeventloop = new QEventLoop();
    qapplication = new QApplication( argc, argv );

    for(int i=0;i<argc;i++)
    {
      if(strcmp(argv[i],"--no_splash") == 0 || 
         strcmp(argv[i],"-ns") == 0 )
      {
        SPLASH = FALSE;
      }
    }

    QPixmap *splash_pixmap = NULL;
    QSplashScreen *splash = NULL;
    if( SPLASH )
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
//    printf("create QApplication(A)\n");

    PerformanceLeader *w;
// fprintf(stderr, "gui_init() entered.  call new PerformanceLeader()\n");
    w = new PerformanceLeader();
//    printf("create Performanceleader()\n");

    w->show();
//    printf("show Performanceleader()\n");

    qapplication->connect( qapplication, SIGNAL( lastWindowClosed() ), qapplication, SLOT( quit() ) );
//    printf("connect this up.\n");

    if( SPLASH )
    {
      sleep(1);
      splash->message( "OpenSpeedShop is brought to you by:");
      splash->raise();
      event_routine();
      sleep(1);
      splash->message( "Jim ... ");
      splash->raise();
      event_routine();
      splash->message( "Jim ...  Steve ...");
      splash->raise();
      event_routine();
      splash->message( "Jim ...  Steve ...  Jack ...  Amaury ...");
      splash->raise();
      event_routine();
      sleep(1);
      splash->message( "Al ...");
      splash->raise();
      event_routine();
      splash->message( "Al ...  David ...");
      splash->raise();
      event_routine();
      splash->message( "Al ...  David ...  Helen ...");
      splash->raise();
      event_routine();
      sleep(1);
      splash->message( "and Bill.");
      splash->raise();
      event_routine();
      sleep(1);
      splash->message( "... with financial help from the US Government....");
      splash->raise();
      event_routine();
      sleep(1);
      splash->finish(w);
      delete splash;
      delete splash_pixmap;
    }

printf("argc=%d\n", argc);
if( SPLASH )
{
  if( argc == 1 )
  { // Pretend absolutely no clues are there for us...
printf("Pretend absolutely no clues are there for us...\n");
    w->topPC->dl_create_and_add_panel("Intro Wizard", w->topPC);
  } else if( argc == 2 )
  {
  // Pretend there is an a.out and/or experiment file in the directory.
printf("Pretend there is an a.out and/or experiment file in the directory.\n");
    w->topPC->dl_create_and_add_panel("Intro Wizard", w->topPC);
    w->topPC->dl_create_and_add_panel("Getting Started", w->topPC);
  } else if( argc == 3 )
  { // Pretend there was an executable file given...
printf("Pretend there was an executable file given...\n");
    w->topPC->dl_create_and_add_panel("Getting Started", w->topPC);
  } else if( argc == 4 )
  {  // Pretend there was an experiment file given...
printf("Pretend there was an experiment file given...\n");
    w->topPC->dl_create_and_add_panel("pcSample Panel", w->topPC);
  }
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
