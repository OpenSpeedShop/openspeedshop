/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <stdlib.h>
#include "PanelContainer.hxx"
#include "TopWidget.hxx"
#include <qvbox.h>
#include <qframe.h>
#include <qmessagebox.h>
#include <qassistantclient.h>
#include <qdir.h>
#include "PluginInfo.hxx"

#include <qapplication.h>
extern QApplication *qapplication;

#include "plugin_handler.hxx"

#include "debug.hxx"  // This includes the definition of dprintf
#include "AttachProcessDialog.hxx"

/*! Here are the needed globals for this application... */
#include "PanelContainer.hxx"
#include "openspeedshop.hxx"
// OpenSpeedshop *topPL = NULL;

void OpenSpeedshop::fileNew()
{
  printf("fileNew() entered\n");

  char *save_executable_name = NULL;
  if( executable_name != NULL )
  {
    free(executable_name);
    executable_name = NULL;
  }

  QString dirName = QString::null;
  if( lfd == NULL )
  {
    lfd = new QFileDialog(this, "file dialog", TRUE );
    lfd->setCaption( QFileDialog::tr("Enter executable or saved experiment:") );
//    lfd->setMode( QFileDialog::AnyFile );
    QString types(
                  "Any Files (*);;"
                  "Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "(*.c *.cpp *.cxx *.C *.c++ *.f* *.F*);;"
                  );
    lfd->setFilters( types );
//    lfd->setViewMode( QFileDialog::Detail );
    lfd->setDir(dirName);
  }
  lfd->setSelection(QString::null);

  QString fileName = QString::null;
  if( lfd->exec() == QDialog::Accepted )
  {
    fileName = lfd->selectedFile();
    if( !fileName.isEmpty() )
    {
      printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      executable_name = strdup( fileName.ascii() );
    }
  }
}

void OpenSpeedshop::fileAttach()
{
  printf("fileAttach() entered\n");

  QString pidString = QString::null;
  AttachProcessDialog *dialog = new AttachProcessDialog(this, "AttachProcessDialog", TRUE);
  if( dialog->exec() == QDialog::Accepted )
  {
// printf("QDialog::Accepted\n");
    pidString = dialog->selectedProcesses();
    if( !pidString.isEmpty() )
    {
      printf("pidString.ascii()=%s\n", pidString.ascii() );
      if( pid_str != NULL )
      {
        free( pid_str );
        pid_str = NULL;
      }
      pid_str = strdup( pidString.ascii() );
    } else
    {
printf("no pid accepted.   Clear the global.\n");
      if( pid_str != NULL )
      {
        free( pid_str );
      }
      pid_str = NULL;
    }
  }

printf("pid_str = %s\n", pid_str == NULL ? "" : pid_str);
  delete dialog;
}


void OpenSpeedshop::fileOpen()
{
  printf("fileOpen() entered\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
}

void OpenSpeedshop::fileSave()
{
  printf("OpenSpeedshop::fileSave() entered\n");

#ifndef DEMO
  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
#else // DEMO
  QString dirName = QString::null;
  if( sfd == NULL )
  {
    sfd = new QFileDialog(this, "file dialog", TRUE );
    sfd->setCaption( QFileDialog::tr("Enter session name:") );
    sfd->setMode( QFileDialog::AnyFile );
//    sfd->setSelection(".openss.geometry");
//    sfd->setSelection(QString("Example.cpp"));
    sfd->setSelection(QString(".openss.geometry"));
    QString types(
                  "Any Files (*);;"
                  "Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "(*.c *.cpp *.cxx *.C *.c++ *.f* *.F*);;"
                  );
    sfd->setFilters( types );
//    sfd->setViewMode( QFileDialog::Detail );
    sfd->setDir(dirName);
  }

  char *fn = NULL;
  QString fileName = QString::null;
  if( sfd->exec() == QDialog::Accepted )
  {
    fileName = sfd->selectedFile();
    if( !fileName.isEmpty() )
    {
      printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      fn = strdup(fileName.ascii());
    } else
    {
      return;
    }
  }

  printf("go and save the setup...\n");
  ((PanelContainer *)topPC)->savePanelContainerTree(fn);
  if( !fileName.isEmpty() )
  {
    free(fn);
  }
#endif // DEMO
}

void OpenSpeedshop::fileExit()
{
 printf("fileExit() entered.\n");

 qApp->closeAllWindows();
 qApp->exit();

 exit(EXIT_SUCCESS);
}

#ifdef EVENTUALLY
void OpenSpeedshop::editUndo()
{
 printf("editUndo() entered.\n");

}

void OpenSpeedshop::editRedo()
{
 printf("editRedo() entered.\n");

}

void OpenSpeedshop::editCut()
{
 printf("editCut() entered.\n");

}

void OpenSpeedshop::editPaste()
{
 printf("editPaste() entered.\n");

}

void OpenSpeedshop::editFind()
{
 printf("editFind() entered.\n");


}
#endif // EVENTUALLY

void OpenSpeedshop::helpIndex()
{
 printf("helpIndex() entered.\n");
}

void OpenSpeedshop::helpContents()
{
 printf("helpContents() entered.\n");

 char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");

 QString base_dir(plugin_directory);
 QString relative_dir("/../../../doc");
 
 QString docsPath = QDir(base_dir+relative_dir).absPath();
 assistant->showPage( QString("%1/index.html").arg(docsPath) );
}

void OpenSpeedshop::helpAbout()
{
 printf("helpAbout() entered.\n");

 QMessageBox::about(this, "Open/SpeedShop", "Open/SpeedShop about example....");
}


#include <dlfcn.h>

/*! \class AppEventFilter
    AppEventFilter catches all the events.   All events are caught to 
    to prevent events from, soon to be deleted, PanelContainers from being
    called after the PanelContainer has been deleted.
*/
//! Catches all events and process them when flagged to do so.
class AppEventFilter : public QObject
{
  public:
    //! Default contructor for a AppEventFilter.   
    /*! It should never be called and is only here for completeness.
     */
    AppEventFilter() {};
    //! The working constructor for AppEventFilter(...)
    /*! This constructor is the work constructor for AppEventFilter.
       Neither paramater is used.
     */
    AppEventFilter(QObject *, PanelContainer *);
    //! Default destructor
    /*! Nothing extra is allocatated, nothing extra is destroyed.
     */
    ~AppEventFilter() {};
  private:
    //! Filter unwanted events based on flags in the master PanelContainer.
    /*! Ignore the mouse and enter leave events.    Not ignoring these caused
        the PanelContainer removal logic fits as events were being fielded
        while we were trying to remove the PanelContainer objects.   Aborts 
        were the result.  
        
        This simple filter simply ignores the events from the beginning of the
        PanelContainer::removePanelContainer() to the end of the cleanup.
     */
    bool eventFilter( QObject *o, QEvent *e );
    //! A pointer to the Master PanelContainer
    PanelContainer *masterPC;
};

AppEventFilter::AppEventFilter(QObject *obj, PanelContainer *pc) : QObject(obj)
{
  dprintf("AppEventFilter(...) constructor entered\n");
  masterPC = pc;
} 

bool 
AppEventFilter::eventFilter( QObject *obj, QEvent *e )
{
//  dprintf("AppEventFilter::eventFilter(%d) 0x%x entered.\n", e->type(), obj );
//  dprintf("AppEventFilter::eventFilter(%d) entered.\n", e->type() );
  if( masterPC->_resizeEventsEnabled == FALSE && e->type() == QEvent::Resize )
  {
//    dprintf("  ... ignore this resize event.\n");
    return TRUE;
  }

  // This is for WhatsThis actions.   If the whatsThis box needs to be
  // remove, remove it, mark it disabled, remove the timers and 
  // return.
  // The hide is automatic for button clicks, but not on the move.
  // When we get a MouseMove we call hide(), which constructs a click
  // event which gets sent to drop the WhatsThis box.
  switch( e->type() )
  {
    case QEvent::MouseMove:
//      masterPC->last_pos  = QCursor::pos();
//      masterPC->last_pos  = QCursor::pos();
      if( masterPC->sleepTimer && masterPC->popupTimer )
      {
//         printf("QEvent::MouseMove: we have timers!\n");
        if( masterPC->sleepTimer->isActive() )
        { // If we're sleeping, just ignore this...
//          printf ("we're sleeping, just return.\n");
          masterPC->sleepTimer->start(1000, TRUE);
        } else
        { // Otherwise, check to see if there's a timer set.   If it is set
          // just go to sleep for a whil and return.   Otherwise, set a new one.
          if( masterPC->popupTimer->isActive() )
          {
//            printf ("popupTimer is already active... start sleeping...\n");
            masterPC->sleepTimer->start(1000, TRUE);
            masterPC->popupTimer->stop();
          } else
          {
//            printf ("start the popup timer...\n");
            masterPC->sleepTimer->stop();
            masterPC->popupTimer->start(1000, TRUE);
          }
        }
      } else
      {
//        printf("QEvent::MouseMove: NO timers!\n");
      }
      if( masterPC->whatsThisActive == TRUE && masterPC->whatsThis )
      {
//        printf("QEvent::MouseMove: SEND MouseButtonPress event to app!\n");
        masterPC->whatsThis->hide( obj );
        masterPC->whatsThisActive = FALSE;
      }
      break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
      if( masterPC->sleepTimer )
      {
        masterPC->sleepTimer->stop();
      }
      if( masterPC->popupTimer )
      {
        masterPC->popupTimer->stop();
      }
      break;
  } 

  if( masterPC->_eventsEnabled == TRUE )
  {
//    dprintf("Handle it!\n");
    return FALSE;
  } else
  {
//    dprintf("Events currently disabled\n");
    switch( e->type() )
    {
      case QEvent::MouseButtonPress:
      case QEvent::MouseButtonRelease:
      case QEvent::MouseButtonDblClick:
      case QEvent::Enter:
      case QEvent::Leave:
//        dprintf("  ... And it was one we wanted to ignore.\n");
        return TRUE;
        break;
      default:
        return FALSE;
    } 
  }
};


void OpenSpeedshop::init()
{
//  topPL = this;
// dprintf(stderr, "OpenSpeedshop::init() entered\n");

  char pc_plugin_file[2048];
  char *pc_dl_name="/ossBase.so";
  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
  if( !plugin_directory )
  {
    fprintf(stderr, "Can't find the PanelContainer plugin. $OPENSPEEDSHOP_PLUGIN_PATH not set correctly.\n");
      return;
  }
  sprintf(pc_plugin_file, "%s%s", plugin_directory, pc_dl_name);
  void *dl_pc_object = dlopen((const char *)pc_plugin_file, (int)RTLD_NOW );
  if( !dl_pc_object )
  {
   fprintf(stderr, "dlerror()=%s\n", dlerror() );
    return;
  }
  PanelContainer * (*dl_pc_init_routine)(QWidget *, QVBoxLayout *);
  dl_pc_init_routine = (PanelContainer * (*)( QWidget *, QVBoxLayout * ))dlsym(dl_pc_object, "pc_init");
  if( dl_pc_init_routine == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", "pc_init", pc_plugin_file, dlerror() );
  }
  PanelContainer *masterPC = (*dl_pc_init_routine)( centralWidget(), OpenSpeedshopLayout);

  masterPC->setMainWindow(this);
  topPC = masterPC;
  

fprintf(stderr, "OpenSpeedshop::init(A)\n");
  char ph_file[2048];
  char *ph_dl_name = "/ossPlugin.so";
  sprintf(ph_file, "%s%s", plugin_directory, ph_dl_name);
  void *dl_ph_object = dlopen((const char *)ph_file, (int)RTLD_NOW );
  if( !dl_ph_object )
  {
   fprintf(stderr, "dlerror()=%s\n", dlerror() );
    return;
  }
  int (*dl_ph_init_routine)(QWidget *, PanelContainer *);
  dl_ph_init_routine = (int (*)( QWidget *, PanelContainer * ))dlsym(dl_ph_object, "ph_init");
  if( dl_ph_init_routine == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", "pc_init", ph_file, dlerror() );
  }

// Load the GUI plugins...
#ifdef VERBOSE
printf("# http://sahara.engr.sgi.com/Tools/WDH/ASCI/Framework-API-V3/P1.html\n");
printf("GUI Action: Load the GUI plugins.\n");
#endif // VERBOSE
  (*dl_ph_init_routine)( (QWidget *)this, masterPC);

#ifdef VERBOSE
printf("NOTE: Check the GUI plugins against known Collectors.\n");
printf("CLI Action: Get a list of all the Collectors.\n");
printf("#    Collector *collectorSet = Collector::getAllCollectors();\n");
printf("GUI Action: Match the Collectors up to the GUI panels.... Set the menu sensitivities.\n");
#endif // VERBOSE

// Begin: Set up a saved session geometry.
const int BUFSIZE=100;
char *fn = ".openss.geometry";
FILE *fd = fopen(fn, "r");

char line_buffer[1024];
char pc_name[100];
char pc_parent_name[100];
int orientation = 0;
int split=0;
int leftSide=0; // Is this a left side 
int width=0;
int height=0;
int x=0;
int y=0;
int not_shown=0;
PanelContainer *pc = NULL;
PanelContainer *lastTopPC = masterPC;

if( fd )
{
  int firstTime = TRUE;
  int done = fread(line_buffer, sizeof(char), BUFSIZE, fd);

  while( done == BUFSIZE )
  {
    sscanf(line_buffer, "%s%s%d%d%d%d%d%d%d%d", pc_name, pc_parent_name, &split, &orientation, &leftSide, &width, &height, &not_shown, &x, &y);
  
    dprintf("(%s) (%s) s=%d o=%d ls=%d w=%d h=%d hidden=%d x=%d y=%d\n", pc_name, pc_parent_name, split, orientation, leftSide, width, height, not_shown, x, y );
  
    if( strcmp(pc_parent_name, "toplevel") == 0 && !firstTime )
    {
      dprintf("We have a new top level.   Create one before continuing.\n");
      //    QWidget *topLevelWidget = new QWidget( 0, "toplevel", 0);
      // Ugh... FIX Why won't the #includes define this?
      // WDestructiveClose = 0x00010000;    (This is the same comment as in file
      // DragNDropPanel.cpp
      //    QWidget *topWidget = new QWidget( 0, "sub toplevel" );
//      QWidget *topWidget = new QWidget( 0, "sub toplevel", 0x00010000 );
      TopWidget *topWidget = new TopWidget( 0, "toplevel" );
      topWidget->setCaption("toplevel");
        
      lastTopPC = createPanelContainer( topWidget, pc_name, NULL, lastTopPC->getMasterPCList() );
  
      lastTopPC->topLevel = TRUE;
  
      topWidget->setGeometry(x,y, width, height);

      topWidget->show();
      lastTopPC->show();
    }

    if( split )
    {
      if( firstTime )
      {
        pc = (PanelContainer *)masterPC;
        dprintf("first time setGeometry(%d, %d, %d, %d)\n", x, y, width, height );
        setGeometry(x,y, width, height);
        pc->resize(width, height);
        pc->dropSiteLayoutParent->resize(width, height);
      } else
      {
        dprintf("find pc_name(%s)\n", pc_name);
        pc = lastTopPC->findInternalNamedPanelContainer(pc_name);
      }
      if( pc )
      {
        if( leftSide == TRUE )
        {
          dprintf("This is a split and a leftSide pc=(%s)\n", pc_name);
          QValueList<int> sizeList;
          sizeList.clear();
          if( pc->parentPanelContainer->splitter->orientation() == QSplitter::Vertical )
          {
            sizeList.push_back(height);
            sizeList.push_back(pc->parentPanelContainer->parent->height()-height);
            dprintf("It's heights should be %d %d\n", height, pc->parentPanelContainer->parent->height()-height);
          } else
          {
            sizeList.push_back(width);
            sizeList.push_back(pc->parentPanelContainer->parent->width()-width);
            dprintf("It's widths should be %d %d\n", width, pc->parentPanelContainer->parent->width()-width);
          }
          pc->splitter->setSizes(sizeList);
        }
        dprintf("split (%s) %s\n", pc->getInternalName(), orientation == 0 ? "HORIZONTAL" : "VERTICAL" );
//          pc->split((Qt::Orientation)orientation, split);
        if( orientation == QSplitter::Vertical )
        {
          pc->splitVertical();
        } else
        {
          pc->splitHorizontal();
        }
      } else
      {
        dprintf("Did not find a PC\n");
      }
    } else
    {
      pc = lastTopPC->findInternalNamedPanelContainer(pc_name);
      if( pc )
      {
        if( firstTime )
        {
          dprintf("no split, first time, setGeometry(%d, %d, %d, %d)\n",
            x, y, width, height );
width+=25;   // FIX
height+=50;   // FIX
          setGeometry(x,y, width, height);
          pc->resize(width, height);
          pc->dropSiteLayoutParent->resize(width, height);
        }
        if( leftSide == TRUE )
        {
          dprintf("This is pc(%s) (a left side).\n", pc->getInternalName() );
          QValueList<int> sizeList;
          sizeList.clear();
          if( pc->parentPanelContainer->splitter->orientation() == QSplitter::Vertical )
          {
            sizeList.push_back(height);
            sizeList.push_back(pc->parentPanelContainer->parent->height()-height);
            dprintf("It's heights should be %d %d\n", height, pc->parentPanelContainer->parent->height()-height);
          } else
          {
            sizeList.push_back(width);
            sizeList.push_back(pc->parentPanelContainer->parent->width()-width);
            dprintf("It's widths should be %d %d\n", width, pc->parentPanelContainer->parent->width()-width);
          }
          pc->splitter->setSizes(sizeList);
        }
      }
      if( not_shown )
      {
        if( pc )
        {
          dprintf("pc (%s) needs to be restored, but hidden.\n", pc->getInternalName() );
          if( pc == pc->parentPanelContainer->leftPanelContainer )
          {
            dprintf("Hide the left side!\n");
            pc->parentPanelContainer->leftPanelContainer->markedForDelete = TRUE;
            pc->parentPanelContainer->leftFrame->hide();
            pc->parentPanelContainer->rightFrame->show();
            pc->parentPanelContainer->rightPanelContainer->show();
            pc->parentPanelContainer->rightPanelContainer->leftFrame->show();
            pc->parentPanelContainer->rightPanelContainer->rightFrame->show();
          } else if( pc == pc->parentPanelContainer->rightPanelContainer )
          {
            dprintf("Hide the right side!\n");
            pc->parentPanelContainer->rightPanelContainer->markedForDelete = TRUE;
            pc->parentPanelContainer->rightFrame->hide();
            pc->parentPanelContainer->leftFrame->show();
            pc->parentPanelContainer->leftPanelContainer->show();
            pc->parentPanelContainer->leftPanelContainer->leftFrame->show();
            pc->parentPanelContainer->leftPanelContainer->rightFrame->show();
          }
        }
      }
      
    }

    firstTime = FALSE;
  
    done = fread(line_buffer, sizeof(char), BUFSIZE, fd);
  }
} else
{
  topPC->splitVertical(80);
  topPC->dl_create_and_add_panel("Intro Wizard", topPC->leftPanelContainer);
  topPC->dl_create_and_add_panel("Command Panel", topPC->rightPanelContainer);
  
}

// End: Set up a saved session geometry.


   AppEventFilter *myEventFilter = new AppEventFilter(this, masterPC);
   qApp->installEventFilter( myEventFilter );
}

void OpenSpeedshop::destroy()
{
fprintf(stderr, "OpenSpeedshop::destroy() entered.\n");
  qApp->restoreOverrideCursor();
}
