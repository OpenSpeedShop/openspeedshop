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
// printf("gui.so gui_init(cliFLAG=%d) entered\n", cliFLAG);
  
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
if( cliFLAG )
{
  SPLASH = FALSE;
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
//    printf("create OpenSpeedshop()\n");

    w->show();
//    printf("show OpenSpeedshop()\n");

    qapplication->connect( qapplication, SIGNAL( lastWindowClosed() ), qapplication, SLOT( quit() ) );
//    printf("connect this up.\n");

#ifdef SPLASH_MESSAGES
    if( SPLASH )
    {
      sleep(1);
      splash->message( "Open/SpeedShop is brought to you by:");
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
#else // SPLASH_MESSAGES
    if( SPLASH )
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
#endif // SPLASH_MESSAGES

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
