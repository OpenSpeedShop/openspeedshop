////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 Krell Institute  All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////
  

#include <stddef.h> 
#include <stdio.h> 
#include <unistd.h>

#include <qapplication.h>
#include "openspeedshop.hxx"

#include <qvaluelist.h>

#include "PanelContainer.hxx"

QApplication *qapplication;

#include <qeventloop.h>
#include <qlineedit.h>
QEventLoop *qeventloop;

#include "splash.xpm"
#include <qsplashscreen.h>
#include <qcheckbox.h>

#include <pthread.h>
// #define PTMAX 10
// pthread_t phandle[PTMAX];
#include "ArgClass.hxx"


//#define DEBUG_GUI 1

extern "C" 
{
  static bool isGUIBeenCreate = FALSE;
  OpenSpeedshop *w = NULL;

  // This routine starts another QApplication gui.  It is called from 
  // gui_init to have the gui started in it's own thread.

  void guithreadinit(void *ptr)
  {

#if DEBUG_GUI
    printf("guithreadinit() entered\n");
#endif

    ArgStruct *arg_struct = NULL;
    int argc = 0;
    char **argv = NULL;
    bool climode = true;
    bool offlineMode = false;
    bool onlineMode = false;
    bool onlineOptionsFound = false;

    if( ptr != NULL ) {
      arg_struct = (ArgStruct *)ptr;
      argc = arg_struct->argc;
      argv = arg_struct->argv;
    }

    bool splashFLAG=TRUE;
  

    QString fontname = QString::null;
    for(int i=0;i<argc;i++)
    {

#if DEBUG_GUI
      printf("guithreadinit(), argv[%d]=(%s)\n", i, argv[i] );
      printf("guithreadinit(), argv[%d]=(0x%lx)\n", i, argv[i] );
#endif

      QString arg = argv[i];
      if( arg == "-fn" || arg == "--fn" )
      {
        fontname = argv[i+1];
        break;
      }
    }

    for(int i=0;i<argc;i++)
    {

#if DEBUG_GUI
      printf("guithreadinit(),  argv[%d]=(%s)\n", i, argv[i] );
#endif

      QString arg = argv[i];
      if( arg == "-gui" || arg == "--gui" )
      {
        climode = false;
        break;
      }
    }

    for(int i=0;i<argc;i++)
    {

#if DEBUG_GUI
      printf("guithreadinit(),  search for -online argv[%d]=(%s)\n", i, argv[i] );
#endif

      QString arg = argv[i];
      if( arg == "-r" || 
          arg == "-p" || 
          arg == "-t" || 
          arg == "-c" || 
          arg == "-h" ) {

        onlineOptionsFound = true;
#if DEBUG_GUI
        printf("guithreadinit(),  online options FOUND\n", i, argv[i] );
#endif
        break;
      }
    }

    for(int i=0;i<argc;i++)
    {

#if DEBUG_GUI
      printf("guithreadinit(),  search for -online argv[%d]=(%s)\n", i, argv[i] );
#endif

      QString arg = argv[i];
      if( arg == "-online" || arg == "--online" )
      {
        onlineMode = true;
#if DEBUG_GUI
        printf("guithreadinit(),  -online FOUND\n", i, argv[i] );
#endif
        break;
      }
    }

    // jeg - 6/30/09 as of 1.9.2 to get online you must specify the -online option
    // so, if we don't see the -online, default to offline mode

#if DEBUG_GUI
    printf("guithreadinit(), did we find onlineMode?, onlineMode=%d\n", onlineMode);
#endif

    if (onlineMode == false && !onlineOptionsFound) {
       offlineMode = true;
#if DEBUG_GUI
       printf("guithreadinit(),  Defaulting to offlineMode because online was not found and no dynamic/online options were found.\n");
#endif
    }

    for(int i=0;i<argc;i++)
    {

#if DEBUG_GUI
      printf("guithreadinit(),  search for -offline argv[%d]=(%s)\n", i, argv[i] );
#endif

      QString arg = argv[i];
      if( arg == "-offline" || arg == "--offline" )
      {
        offlineMode = true;
#if DEBUG_GUI
        printf("guithreadinit(),  -offline FOUND\n", i, argv[i] );
#endif
        break;
      }
    }

    // If we found dynamic options that can only be used with the online version of openss
    // and we are in offline mode, switch to online and warn the user that we are in online mode.
    if (onlineMode == false && onlineOptionsFound) {
       onlineMode = true;
       // Issue Warning to user
        fprintf(stderr, "WARNING: One or more dynamic options (-c, -h, -p, -r, -t) were found, but openss is currently in offline mode, switching to online.\n");
       // end Issue Warning to user
#if DEBUG_GUI
       printf("guithreadinit(),  Defaulting to onlineMode because dynamic/online options were found.\n");
#endif
    }

#if DEBUG_GUI
    printf("guithreadinit(),climode=(%d)\n", climode );
#endif

    if( climode == TRUE ) {  
       // Before you intialize the gui, make sure you won't abort just trying
       // to get started.   Do the obvious here... Check to see if we have a
       // DIPSLAY set and that we can open it.
      char *DISPLAY = getenv("DISPLAY");
    
      if( DISPLAY == NULL || 
          strcmp(DISPLAY,"") == 0 ) {
        fprintf(stderr, "No DISPLAY variable set...  Unable to start gui.\n");
        return;
      }
    }

    qapplication = new QApplication( argc, argv );

    if( argv == NULL ) {
      argc = 0;
    }

    QString hostStr = QString::null;
    QString executableStr = QString::null;
    QString widStr = QString::null;
    QString argsStr = QString::null;
    QString parallelPrefixCommandStr = QString::null;
    QString pidStr = QString::null;
    QStringList pidStrList = NULL;
    QString rankStr = QString::null;
    QString expStr = QString::null;
#if DEBUG_GUI
    printf("guithreadinit(),argc=%d\n", argc);
#endif
    for(int i=0;i<argc;i++)
    {
      QString arg = argv[i];

#if DEBUG_GUI
      printf("guithreadinit(),argv[%d]=(%s)\n", i, argv[i]);
      printf("guithreadinit(),argv[%d]=(%s)\n", i+1, argv[i+1]);
#endif

      if( arg == "-wid" )
      { // You have a window id from the cli
        for( ;i<argc-1;)
        {
#if DEBUG_GUI
          printf("guithreadinit(),inside for i< argc, argv[%d]=(%s)\n", i, argv[i]);
#endif
          widStr += QString(argv[i+1]);
          widStr += QString(" ");
#if DEBUG_GUI
          printf("guithreadinit(),inside for i< argc, widStr.ascii()=(%s)\n", widStr.ascii());
          printf("guithreadinit(),inside for i< argc, widStr.toInt()=(%d)\n", widStr.toInt());
#endif
          i = i + 1;
        }
      }

    }


#if DEBUG_GUI
    printf("libopenss-gui/gui.cxx: guithreadinit(),calling new OpenSpeedshop widStr.toInt()=%d, widStr.ascii()=%s\n", widStr.toInt(), widStr.ascii());
#endif
    w = new OpenSpeedshop(widStr.toInt(), climode);

    QPixmap *splash_pixmap = NULL;
    QSplashScreen *splash = NULL;

    if( !w->preferencesDialog->setShowSplashScreenCheckBox->isChecked() ) {
      splashFLAG = FALSE;
    } 

    if( splashFLAG ) {
      splash_pixmap = new QPixmap( splash_xpm );
      splash = new QSplashScreen( *splash_pixmap );
      splash->setCaption("splash");
      splash->show();
      splash->message( "Loading plugins" );
      splash->raise();
    }

    // Set the font from the preferences...
    QFont *m_font = NULL;

    if( w->preferencesDialog->preferencesAvailable == TRUE ) {
      m_font = new QFont( w->preferencesDialog->globalFontFamily,
                      w->preferencesDialog->globalFontPointSize,
                      w->preferencesDialog->globalFontWeight,
                      w->preferencesDialog->globalFontItalic );
    }

    if( m_font != NULL ) {
      qApp->setFont(*m_font, TRUE);

      delete m_font;
    }


    w->executableName = executableStr;
    w->widStr = widStr;
#if DEBUG_GUI
    printf("guithreadinit(),w->widStr.ascii()=%s\n", w->widStr.ascii());
    printf("guithreadinit(),w->widStr.toInt()=%d\n", w->widStr.toInt());
#endif
    w->pidStr = pidStr;
    w->pidStrList = NULL;
    w->rankStr = rankStr;
    w->expStr = expStr;
    if( !hostStr.isEmpty() ) {
      w->hostStr = hostStr;
    }
    w->argsStr = argsStr;
    w->parallelPrefixCommandStr = parallelPrefixCommandStr;

#if DEBUG_GUI
    printf("guithreadinit(),w->argsStr.ascii()=%s\n", w->argsStr.ascii());
    printf("guithreadinit(),w->parallelPrefixCommandStr.ascii()=%s\n", w->parallelPrefixCommandStr.ascii());
#endif

    w->show();

    if( splashFLAG ) {
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

    int number_of_found_experiments = 0;
    // The user specified some arguments on the command line.
    // See if they've defined any experiments.
//    if( argc > 4 )
    {
      number_of_found_experiments = w->lookForExperiment(offlineMode);
    }
    // If the use has not defined any experiments, then 
    // load the wizard to help them...

#if DEBUG_GUI
    printf("guithreadinit(),number_of_found_experiments=%d\n", number_of_found_experiments );
#endif

    if( number_of_found_experiments == 0 ) {
      w->loadTheWizard();
    }

    isGUIBeenCreate = TRUE;

    qapplication->exec();

    return;
  }

  // This is simple the routine that is called to start the gui it's own 
  // thread.   Called from Start.cxx, when the gui is requested at startup.
  // Otherwise, it can be called from the command line at any time to 
  // initialize the gui.
  int
  gui_init( void *arg_struct, pthread_t *phandle )
  {
#if DEBUG_GUI
    printf("gui_init entered\n");
#endif
    if( isGUIBeenCreate == TRUE ) {
      if( !qapplication ) {
        fprintf(stderr, "Unable to reattach to the gui.  No qapplication.  Returning.\n");
      } else {
        w->raiseGUI();
      }
    } else {
//    gui_thread_id = pthread_create(&phandle[0], 0, (void *(*)(void *))guithreadinit,arg_struct);
      pthread_create(phandle, 0, (void *(*)(void *))guithreadinit,arg_struct);
    }

    return 1;
  }

  int
  gui_exit()
  {
#if DEBUG_GUI
      printf("Exit the gui!\n");
#endif

      // Store the geometry of the main window
      w->storeGeometry();

      w->topPC->closeAllExternalPanelContainers();
      delete w->topPC;
  }

}
