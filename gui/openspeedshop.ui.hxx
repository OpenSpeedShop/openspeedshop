////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
  


#include <stdlib.h>
#include "PanelContainer.hxx"
#include "TopWidget.hxx"
#include <qvbox.h>
#include <qframe.h>
#include <qmessagebox.h>
#include <qassistantclient.h>
#include <qdir.h>
#include <qlistview.h>
#include "PluginInfo.hxx"

#include "preferencesdialog.hxx"

#include <qapplication.h>
extern QApplication *qapplication;

#include "plugin_handler.hxx"
#include <dlfcn.h>

// #include "debug.hxx"  // This includes the definition of dprintf
#include "AttachProcessDialog.hxx"
#include "SelectExperimentDialog.hxx"

/*! Here are the needed globals for this application... */
#include "PanelContainer.hxx"
#include "openspeedshop.hxx"

#include "LoadAttachObject.hxx"

#include "CLIInterface.hxx"

void OpenSpeedshop::fileLoadNewProgram()
{
//  printf("OpenSpeedshop::fileLoadNewProgram() entered\n");

  pidStr = QString::null;

  loadNewProgram();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

void OpenSpeedshop::fileAttachNewProcess()
{
//  printf("OpenSpeedshop::fileAttachNewProcess() entered\n");
  executableName = QString::null;

  attachNewProcess();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

void OpenSpeedshop::fileSaveSession()
{
// printf("OpenSpeedshop::fileSaveSession() entered\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction."), QMessageBox::Ok );

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
//      printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      fn = strdup(fileName.ascii());
    } else
    {
      return;
    }
  }

//  printf("go and save the setup...\n");
  if( !fileName.isEmpty() )
  {
    ((PanelContainer *)topPC)->savePanelContainerTree(fn);
    free(fn);
  }
}


void OpenSpeedshop::fileOpenExperiment()
{
  SelectExperimentDialog *dialog = new SelectExperimentDialog(this, "Select Experiment To Open Dialog", TRUE);

   QString expStr;
  if( dialog->exec() == QDialog::Accepted )
  {
// printf("QDialog::Accepted\n");
    expStr = dialog->selectedExperiment();

printf("expStr = %s\n", expStr.ascii() );

// Given an expStr (the -x experiment id) look up all the data... 
QString experimentName = QString("pc Sampling ["+expStr+"]");
const char *name = experimentName.ascii();
printf("Try to locate panel with name/id pair (%s)\n", experimentName.ascii() );
// Determine if an experiment already exits for the experiment and if so 
// raise the panel.
    Panel *p = topPC->findNamedPanel(topPC, (char *)name);
    if( p )
    {
      printf("FOUND ONE!\n");
      p->getPanelContainer()->raisePanel(p);
    } else
    {
// Otherwise, create a new pcSamplePanel (maybe passing the -x id down
// as an argument for it to load an existing experiment rather than 
// creating a new one.
      printf("Create a new one!\n");
      topPC->dl_create_and_add_panel("pc Sampling", topPC->leftPanelContainer, (void *)&expStr);
    }
  }

  delete dialog;

}

void OpenSpeedshop::fileOpenSavedExperiment()
{
  printf("OpenSpeedshop::fileOpenSavedExperiment() entered\n");
  printf("  Get a list of all the experiment files in the current directory\n");
  printf("  and in the environment variable >INSERTONEHERE<.   Then create\n");
  printf("  a dynamice menu with the list...    \n\n");
  printf("  When the list is selected, examine it for the type, then bring\n");
  printf("  up the associated experiment.    It would be nice, if the save\n");
  printf("  session information could be read to bring that experiment up\n");
  printf("  with the same layout as what it was left in during the prior\n");
  printf("  save.\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
}

void OpenSpeedshop::fileSaveExperiment()
{
/*
  printf("OpenSpeedshop::fileSaveExperiment() entered\n");
  printf("  Get a list of all the current experiments openned.  Present\n");
  printf("  list to the user so they can chose to have the experiment data\n");
  printf("  saved away to a file.\n\n");
  printf("  Additionally, at this point, it might be nice to prompt the\n");
  printf("  user to save the session (window layout) information away as\n");
  printf("  well.\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
*/

  SelectExperimentDialog *dialog = new SelectExperimentDialog(this, "Select Experiment To Save Dialog", TRUE);
QString expStr;
  if( dialog->exec() == QDialog::Accepted )
  {
// printf("QDialog::Accepted\n");
    expStr = dialog->selectedExperiment();
  }

printf("expStr = %s\n", expStr.ascii() );
  delete dialog;

}

void OpenSpeedshop::fileExportExperimentData()
{
  printf("OpenSpeedshop::fileExportExperimentData() entered\n");
  printf("  Get a list of all the current experiments openned.  Present\n");
  printf("  list to the user so they can chose to have the experiment data\n");
  printf("  to export.  Also prompt them for the export format type.\n\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
}

void OpenSpeedshop::filePreferences()
{
//  printf("filePreferences() entered.\n");

  preferencesDialog->show();
}

#include "Commander.hxx"   // Contains the InputLineObject definition.
void OpenSpeedshop::fileExit()
{
  dprintf("fileExit() entered.\n");

 /* close all the panel containers.   Well all except the masterPC's
    That one we need to do explicitly. (See the next line.) */
 ((PanelContainer *)topPC)->getMasterPC()->closeAllExternalPanelContainers();

 /* Now close the master pc's information. */
 ((PanelContainer *)topPC)->closeWindow((PanelContainer *)topPC);

 qApp->closeAllWindows();
 dprintf("fileExit() called closeAllWindows.\n");

 // qApp->exit();
 //printf("fileExit() called qApp->exit.\n");

  int wid = ((PanelContainer *)topPC)->getMainWindow()->widStr.toInt();
  InputLineObject *ilp = Append_Input_String( wid, "quit");
  if( ilp == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli for exit attempting exit regardless.\n");
  }


 pthread_exit(EXIT_SUCCESS);
 dprintf("fileExit() called pthread_exit.\n");
}


void OpenSpeedshop::fileClose()
{
  dprintf("fileClose() entered.\n");

 /* close all the panel containers.   Well all except the masterPC's
    That one we need to do explicitly. (See the next line.) */
 ((PanelContainer *)topPC)->getMasterPC()->closeAllExternalPanelContainers();

 /* Now close the master pc's information. */
 ((PanelContainer *)topPC)->closeWindow((PanelContainer *)topPC);

 qApp->closeAllWindows();
 dprintf("fileClose() called closeAllWindows.\n");

 // qApp->exit();
 //printf("fileClose() called qApp->exit.\n");

 pthread_exit(EXIT_SUCCESS);
 dprintf("fileClose() called pthread_exit.\n");
}

void OpenSpeedshop::helpIndex()
{
// printf("helpIndex() entered.\n");
}

void OpenSpeedshop::helpContents()
{
// printf("helpContents() entered.\n");
 char *plugin_directory = NULL;
  
 plugin_directory = getenv("OPENSPEEDSHOP_DOC_DIR");
 if( plugin_directory )
 {
   assistant->showPage( QString("%1/index.html").arg(plugin_directory) );

   return;
 }
 plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");

 QString base_dir(plugin_directory);
 QString relative_dir("/../../../OpenSpeedShop/current/doc");

 QString docsPath = QDir(base_dir+relative_dir).absPath();
 assistant->showPage( QString("%1/index.html").arg(docsPath) );
}

void OpenSpeedshop::helpAbout()
{
// printf("helpAbout() entered.\n");

 QMessageBox::about(this, "Open/SpeedShop", "Open/SpeedShop about example....");
}


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
  (*dl_ph_init_routine)( (QWidget *)this, masterPC);

  // Create the master preferences dialog so we can set the defaults and 
  // so everyone else can reference them....
  preferencesDialog = new PreferencesDialog(masterPC);

  // Begin to load preferences
  //This is the base plugin directory.   In this directory there should
  // be a list of dso (.so) which are the plugins.
  char plugin_file[1024];
  if( masterPC && masterPC->_pluginRegistryList )
  {
    PluginInfo *pi = NULL;
    for( PluginRegistryList::Iterator it = masterPC->_pluginRegistryList->begin();
         it != masterPC->_pluginRegistryList->end();
         it++ )
    {
      pi = (PluginInfo *)*it;
      sprintf(plugin_file, "%s/%s", plugin_directory, pi->plugin_name );
// printf("about to open(%s).\n", plugin_file);
      void *dl_object = dlopen((const char *)plugin_file, (int)RTLD_LAZY );

      if( dl_object )
      {
// printf("about to lookup(%s).\n", "initialize_preferences_entry_point");
        QWidget * (*dl_plugin_info_init_preferences_routine)(QSettings *, QWidgetStack*, char *) =
          (QWidget * (*)(QSettings *, QWidgetStack*, char *))dlsym(dl_object, "initialize_preferences_entry_point" );
          if( dl_plugin_info_init_preferences_routine )
          {
// printf("about to call the routine.\n");
            QWidget *panelStackPage = (*dl_plugin_info_init_preferences_routine)(preferencesDialog->settings, preferencesDialog->preferenceDialogWidgetStack, pi->preference_category);
            if( panelStackPage )
            {
              preferencesStackPagesList.push_back(panelStackPage);
              QListViewItem *item = new QListViewItem( preferencesDialog->categoryListView );
              item->setText( 0, tr( panelStackPage->name() ) );
            }
          }
          dlclose(dl_object);
        }
      }
    }
    preferencesStackPagesList.push_back(preferencesDialog->generalStackPage);
    QListViewItem *item = new QListViewItem( preferencesDialog->categoryListView );
    item->setText( 0, tr( "General" ) );
    preferencesDialog->categoryListView->setSelected(item, TRUE);
    // End load preferences

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

void OpenSpeedshop::loadNewProgram()
{
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
//      printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      QFileInfo fi(fileName);
      if( !fi.isExecutable() )
      {
        QMessageBox::information( (QWidget *)this, tr("Info:"), tr("The selected file is not executable."), QMessageBox::Ok );
        return;
      }
      executableName = fileName;
    }
  }
}

void OpenSpeedshop::attachNewProcess()
{
  AttachProcessDialog *dialog = new AttachProcessDialog(this, "AttachProcessDialog", TRUE);
  if( dialog->exec() == QDialog::Accepted )
  {
// printf("QDialog::Accepted\n");
    pidStr = dialog->selectedProcesses();
  }

printf("pidStr = %s\n", pidStr.ascii() );
  delete dialog;
}
