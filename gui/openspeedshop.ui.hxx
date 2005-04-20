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
#include <libgen.h> // basename()
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
#include <ltdl.h>
#include <assert.h>

// #include "debug.hxx"  // This includes the definition of dprintf
#include "AttachProcessDialog.hxx"
#include "AttachProgramDialog.hxx"
#include "SelectExperimentDialog.hxx"
#include "SaveAsExperimentDialog.hxx"

/*! Here are the needed globals for this application... */
#include "openspeedshop.hxx"
#include "DebugPanel.hxx"

#include "LoadAttachObject.hxx"

#include "CLIInterface.hxx"

void OpenSpeedshop::fileLoadNewProgram()
{
//printf("OpenSpeedshop::fileLoadNewProgram() entered\n");

  pidStr = QString::null;

  loadNewProgram();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

void OpenSpeedshop::fileAttachNewProcess()
{
//printf("OpenSpeedshop::fileAttachNewProcess() entered\n");
  executableName = QString::null;

  attachNewProcess();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

#ifdef SAVESESSION
void OpenSpeedshop::fileSaveSession()
{
//printf("OpenSpeedshop::fileSaveSession() entered\n");

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
//printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      fn = strdup(fileName.ascii());
    } else
    {
      return;
    }
  }

//printf("go and save the setup...\n");
  if( !fileName.isEmpty() )
  {
    ((PanelContainer *)topPC)->savePanelContainerTree(fn);
    free(fn);
  }
}
#endif // SAVESESSION


void OpenSpeedshop::fileOpenExperiment()
{
  QApplication::setOverrideCursor(QCursor::WaitCursor);
  SelectExperimentDialog *dialog = new SelectExperimentDialog(this, "Select Experiment To Open Dialog", TRUE);

  QString expStr;
  if( dialog->exec() == QDialog::Accepted )
  {
//printf("QDialog::Accepted\n");
    int expID = 0;
    PanelListViewItem *item = dialog->selectedExperiment(&expID);
    Panel *p = NULL;
    if( item )
    {
      if( item->parent() != NULL )
      {
        p = item->panel;
      }
    }
    if( p )
    {
      //const char *name = p->getName();
      //printf( "panel name = (%s)\n", name );
      p->getPanelContainer()->raisePanel(p);
    } else
    {
      //printf("Create a new one!\n");
      //printf("expID = (%d) \n", expID );
      QString expStr = QString("%1").arg(expID);

  QString command;
  command = QString("listTypes -x %1").arg(expStr);
  std::list<std::string> list_of_collectors;
  if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
         &list_of_collectors, FALSE ) )
  {
    printf("Unable to run %s command.\n", command.ascii() );
  }

  int knownCollectorType = FALSE;
  QString panel_type = "other";
  if( list_of_collectors.size() > 0 )
  {
    for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
    {
//      std::string collector_name = *it;
      QString collector_name = (QString)*it;
//      printf("(%s)\n", collector_name.ascii() );
      if( collector_name == "pcsamp" )
      {
        knownCollectorType = TRUE;
        panel_type = "pc Sampling";
        break;
      } else if( collector_name == "usertime" )
      {
        knownCollectorType = TRUE;
        panel_type = "User Time";
        break;
      } else if( collector_name == "fpe" )
      {
        knownCollectorType = TRUE;
        panel_type = "FPE Tracing";
        break;
      } else if( collector_name == "hw" )
      {
        panel_type = "HW Counter";
        knownCollectorType = TRUE;
        break;
      } else if( collector_name == "io" )
      {
        panel_type = "IO";
        knownCollectorType = TRUE;
        break;
      } else if( collector_name == "mpi" )
      {
        panel_type = "MPI";
        knownCollectorType = TRUE;
        break;
      }
      
    }
  }


//printf("pane_type.ascii() = %s\n", panel_type.ascii() );
  PanelContainer *bestFitPC = ((PanelContainer *)topPC)->findBestFitPanelContainer((PanelContainer *)topPC);
  topPC->dl_create_and_add_panel((char *)panel_type.ascii(), bestFitPC, (void *)&expStr);
    }
  }

  delete dialog;

  QApplication::restoreOverrideCursor();
}

void OpenSpeedshop::fileOpenSavedExperiment()
{
//printf("OpenSpeedshop::fileOpenSavedExperiment() entered\n");
//printf("  Get a list of all the experiment files in the current directory\n");
//printf("  and in the environment variable >INSERTONEHERE<.   Then create\n");
//printf("  a dynamice menu with the list...    \n\n");
//printf("  When the list is selected, examine it for the type, then bring\n");
//printf("  up the associated experiment.    It would be nice, if the save\n");
//printf("  session information could be read to bring that experiment up\n");
//printf("  with the same layout as what it was left in during the prior\n");
//printf("  save.\n");

  QString fn = QFileDialog::getOpenFileName(
                    "./",
                    "Open|SpeedShop files (*.openss);;",
                    this,
                    "open file dialog",
                    "Choose a experiment file to open" );

  if( !fn.isEmpty() )
  {
    QString command;
    command = QString("expRestore -f %1").arg(fn);
//QMessageBox::information( (QWidget *)NULL, tr("Info: Unable to complete command"), tr("This feature currently under construction.\nCommand to be executed:\n%1").arg(command), QMessageBox::Ok );
//return;
    if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
  }
}

void OpenSpeedshop::fileSaveExperiment()
{
  SaveAsExperimentDialog *dialog = new SaveAsExperimentDialog(this, "Select Experiment To Save Dialog", TRUE);

  QString expStr;
  if( dialog->exec() == QDialog::Accepted )
  {
    const char *name = NULL;
    int expID = 0;
    PanelListViewItem *item = dialog->selectedExperiment(&expID);
    QString expStr = QString("%1").arg(expID);
    QString collectorName = QString("%1").arg(item->text(1));
    QString databaseName = QString("%1").arg(item->text(2));
//printf("expStr=(%s) collectorName=(%s) databaseName=(%s)\n", expStr.ascii(), collectorName.ascii(), databaseName.ascii() );
    QString dirName = "./";

    QFileDialog *sed = new QFileDialog(this, "file dialog", TRUE );
    sed->setCaption( QFileDialog::tr("Enter session name:") );
    sed->setMode( QFileDialog::AnyFile );
    QString types(
                  "Open|SpeedShop files (*.openss);;"
                  );
    sed->setFilters( types );
    sed->setDir(dirName);
//    const char *n = databaseName.ascii();
//    char *bn = basename( (char *)n );
    QString bn = collectorName+".openss";
    sed->setSelection(bn);
  
    QString fileName = QString::null;
    if( sed->exec() == QDialog::Accepted )
    {
      fileName = sed->selectedFile();
      if( !fileName.isEmpty() )
      {
// printf("fileName.ascii() = (%s)\n", fileName.ascii() );
        QString command;
        command = QString("expSave -x %1 -f %2").arg(expID).arg(fileName);
// printf("command=(%s)\n", command.ascii() );
        if( !cli->runSynchronousCLI( (char *)command.ascii() ) )
        {
          printf("Unable to run %s command.\n", command.ascii() );
        }
      } else
      {
        return;
      }
    }
  }

  delete dialog;

}

#ifdef EXPORT
void OpenSpeedshop::fileExportExperimentData()
{
//printf("OpenSpeedshop::fileExportExperimentData() entered\n");
//printf("  Get a list of all the current experiments openned.  Present\n");
//printf("  list to the user so they can chose to have the experiment data\n");
//printf("  to export.  Also prompt them for the export format type.\n\n");

  QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("This feature currently under construction. - Unable to fulfill request."), QMessageBox::Ok );
}
#endif // EXPORT

void OpenSpeedshop::filePreferences()
{
//printf("filePreferences() entered.\n");

  preferencesDialog->show();
}

#include "Commander.hxx"   // Contains the InputLineObject definition.
void OpenSpeedshop::fileExit()
{
 dprintf("fileExit() entered.\n");

#ifdef OLDWAY
  int wid = ((PanelContainer *)topPC)->getMainWindow()->widStr.toInt();
//  InputLineObject *ilp = Append_Input_String( wid, "quit");
  InputLineObject *ilp = Append_Input_String( wid, "exit\n");
//  Append_Input_String( wid, "exit\n");

  if( ilp == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli for exit attempting exit regardless.\n");
  }

 /* close all the panel containers.   Well all except the masterPC's
    That one we need to do explicitly. (See the next line.) */
 ((PanelContainer *)topPC)->getMasterPC()->closeAllExternalPanelContainers();

 /* Now close the master pc's information. */
 ((PanelContainer *)topPC)->closeWindow((PanelContainer *)topPC);


 qApp->closeAllWindows();
 dprintf("fileExit() called closeAllWindows.\n");


 qApp->exit();
 pthread_exit(EXIT_SUCCESS);
 dprintf("fileExit() called pthread_exit.\n");
#else // OLDWAY

 /* close all the panel containers.   Well all except the masterPC's
    That one we need to do explicitly. (See the next line.) */
  ((PanelContainer *)topPC)->getMasterPC()->closeAllExternalPanelContainers();

 /* Now close the master pc's information. */
  ((PanelContainer *)topPC)->closeWindow((PanelContainer *)topPC);


  int wid = ((PanelContainer *)topPC)->getMainWindow()->widStr.toInt();
  InputLineObject *ilp = Append_Input_String( wid, "exit\n");
#endif  // OLDWAY
}


void OpenSpeedshop::fileClose()
{
  dprintf("fileClose() entered.\n");
  dprintf("Try to hide the main window!\n");
  hide();
}

void OpenSpeedshop::helpIndex()
{
  dprintf("helpIndex() entered.\n");
}

void OpenSpeedshop::helpContents()
{
 dprintf("helpContents() entered.\n");

 char *plugin_directory = NULL;
  
 plugin_directory = getenv("OPENSS_DOC_DIR");
 if( plugin_directory )
 {
   assistant->showPage( QString("%1/index.html").arg(plugin_directory) );

   return;
 }
 plugin_directory = getenv("OPENSS_PLUGIN_PATH");

 QString base_dir(plugin_directory);
 QString relative_dir("/../../../OpenSpeedShop/current/doc");

 QString docsPath = QDir(base_dir+relative_dir).absPath();
 assistant->showPage( QString("%1/index.html").arg(docsPath) );
}

void OpenSpeedshop::helpAbout()
{
 dprintf("helpAbout() entered.\n");

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
//printf("QEvent::MouseMove: we have timers!\n");
        if( masterPC->sleepTimer->isActive() )
        { // If we're sleeping, just ignore this...
//printf ("we're sleeping, just return.\n");
          masterPC->sleepTimer->start(1000, TRUE);
        } else
        { // Otherwise, check to see if there's a timer set.   If it is set
          // just go to sleep for a whil and return.   Otherwise, set a new one.
          if( masterPC->popupTimer->isActive() )
          {
//printf ("popupTimer is already active... start sleeping...\n");
            masterPC->sleepTimer->start(1000, TRUE);
            masterPC->popupTimer->stop();
          } else
          {
//printf ("start the popup timer...\n");
            masterPC->sleepTimer->stop();
            masterPC->popupTimer->start(1000, TRUE);
          }
        }
      } else
      {
//printf("QEvent::MouseMove: NO timers!\n");
      }
      if( masterPC->whatsThisActive == TRUE && masterPC->whatsThis )
      {
//printf("QEvent::MouseMove: SEND MouseButtonPress event to app!\n");
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

  // Begin For catching the debug key sequence.
  if( e->type() == QEvent::KeyPress )
  {
    QKeyEvent *key_event = (QKeyEvent *)e;
    if( key_event->state() ==
        (Qt::ControlButton + Qt::ShiftButton + Qt::AltButton) )
    {
      if( key_event->key() == Qt::Key_D )
      {
        if( masterPC != NULL )
        {
          PanelContainer *bestFitPC = masterPC->findBestFitPanelContainer(masterPC);
          DebugPanel *debugPanel = new DebugPanel(bestFitPC, "Debug Panel", NULL);
          bestFitPC->addPanel((Panel *)debugPanel, bestFitPC, "Debug Panel");
        }
      }
    }
  }
  // End - For catching the debug key sequence.

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
  char pc_plugin_file[2048];
  assert(lt_dlinit() == 0);
  // Start with an empty user-defined search path
  assert(lt_dlsetsearchpath("") == 0);
  // Add the user-specified plugin path
  if(getenv("OPENSS_PLUGIN_PATH") != NULL)
  {
    char *user_specified_path = getenv("OPENSS_PLUGIN_PATH");
    const char *currrent_search_path = lt_dlgetsearchpath();
    assert(lt_dladdsearchdir(user_specified_path) == 0);
  }
  // Add the install plugin path
  char *openss_install_dir = getenv("OPENSS_INSTALL_DIR");
  if( openss_install_dir != NULL)
  {
    char *install_path = (char *)calloc(strlen(openss_install_dir)+
                                        strlen("/lib/openspeedshop")+1,
                                        sizeof(char *) );
    strcpy(install_path, openss_install_dir);
    strcat(install_path, "/lib/openspeedshop");
    const char *currrent_search_path = lt_dlgetsearchpath();
    assert(lt_dladdsearchdir(install_path) == 0);
  }
  // Add the compile-time plugin path
  assert(lt_dladdsearchdir(PLUGIN_PATH) == 0);
  // Now search for plugins in all these paths

  char *pc_dl_name="libopenss-Base";
  lt_dlhandle dl_pc_object = lt_dlopenext((const char *)pc_dl_name);
  if( dl_pc_object == NULL )
  {
   fprintf(stderr, "lt_dlerror()=%s\n", lt_dlerror() );
    return;
  }
  PanelContainer *(*dl_pc_init_routine)(QWidget *, QVBoxLayout *);
  dl_pc_init_routine = (PanelContainer * (*)( QWidget *, QVBoxLayout * ))lt_dlsym(dl_pc_object, "pc_init");
  if( dl_pc_init_routine == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s lt_dlerror()=%s\n", "pc_init", pc_plugin_file, lt_dlerror() );
  }
  PanelContainer *masterPC = (PanelContainer *)(*dl_pc_init_routine)( centralWidget(), OpenSpeedshopLayout);

  masterPC->setMainWindow(this);
  topPC = masterPC;

  char ph_file[2048];
  char *ph_dl_name = "libopenss-Plugin";
  lt_dlhandle dl_ph_object = lt_dlopenext((const char *)ph_dl_name);
  if( dl_ph_object == NULL )
  {
   fprintf(stderr, "lt_dlerror()=%s\n", lt_dlerror() );
    return;
  }
  lt_ptr (*dl_ph_init_routine)(QWidget *, PanelContainer *);
  dl_ph_init_routine = (lt_ptr (*)( QWidget *, PanelContainer * ))lt_dlsym(dl_ph_object, "ph_init");
  if( dl_ph_init_routine == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s lt_dlerror()=%s\n", "pc_init", ph_file, lt_dlerror() );
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
//      lt_dlhandle dl_object = lt_dlopenext((const char *)pi->plugin_name);
      lt_dlhandle dl_object = lt_dlopen((const char *)pi->plugin_name);

      if( dl_object != NULL )
      {
        QWidget * (*dl_plugin_info_init_preferences_routine)(QSettings *, QWidgetStack*, char *) =
        (QWidget * (*)(QSettings *, QWidgetStack*, char *))lt_dlsym(dl_object, "initialize_preferences_entry_point" );
        if( dl_plugin_info_init_preferences_routine != NULL )
        {
          QWidget *panelStackPage = (QWidget *)(*dl_plugin_info_init_preferences_routine)(preferencesDialog->settings, preferencesDialog->preferenceDialogWidgetStack, pi->preference_category);
          if( panelStackPage )
          {
            preferencesStackPagesList.push_back(panelStackPage);
            QListViewItem *item = new QListViewItem( preferencesDialog->categoryListView );
            item->setText( 0, tr( panelStackPage->name() ) );
          }
        }
        lt_dlclose(dl_object);
      }
    }
  }
    preferencesStackPagesList.push_back(preferencesDialog->generalStackPage);
    QListViewItem *item = new QListViewItem( preferencesDialog->categoryListView );
    item->setText( 0, tr( "General" ) );
    preferencesDialog->categoryListView->setSelected(item, TRUE);
    // End load preferences

#ifdef SAVESESSION
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
      // Mark the new PanelContainer as s outsidePC.
      targetPC->outsidePC = TRUE;
      targetPC->topWidget = topLevelWidget;

  
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
#else // SAVESESSION
  topPC->splitVertical(80);
  topPC->dl_create_and_add_panel("Intro Wizard", topPC->leftPanelContainer);
  topPC->dl_create_and_add_panel("Command Panel", topPC->rightPanelContainer);
#endif // SAVESESSION


   AppEventFilter *myEventFilter = new AppEventFilter(this, masterPC);
   qApp->installEventFilter( myEventFilter );

  qapplication->connect( qApp, SIGNAL( lastWindowClosed() ), this, SLOT( myQuit() ) );
}

void OpenSpeedshop::destroy()
{
fprintf(stderr, "OpenSpeedshop::destroy() entered.\n");
  qApp->restoreOverrideCursor();
}

void OpenSpeedshop::loadNewProgram()
{
// all load programs requests come through here!
  QString dirName = QString::null;
  if( lfd == NULL )
  {
    lfd = new AttachProgramDialog(this, "file dialog", TRUE );
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
//printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      QFileInfo fi(fileName);
      if( !fi.isExecutable() )
      {
        QMessageBox::information( (QWidget *)this, tr("Info:"), tr("The selected file is not executable."), QMessageBox::Ok );
        return;
      }
      executableName = fileName;
      if( lfd->lineedit->text().isEmpty() )
      {
        argsStr = QString::null;
      } else
      {
        argsStr = lfd->lineedit->text();
// printf("argsStr=(%s)\n", argsStr.ascii() );
      }
    }
  }
}

void OpenSpeedshop::attachNewProcess()
{
  AttachProcessDialog *dialog = new AttachProcessDialog(this, "AttachProcessDialog", TRUE);
  if( dialog->exec() == QDialog::Accepted )
  {
    //printf("QDialog::Accepted\n");
    pidStr = dialog->selectedProcesses();
  }

  //printf("pidStr = %s\n", pidStr.ascii() );
  delete dialog;
}

void OpenSpeedshop::myQuit()
{
  dprintf("myQuit called!   You closed the last window!!!!\n");

  fileExit();
  qapplication->flushX();
}

void OpenSpeedshop::raiseTheGUI()
{
  dprintf("raiseTheGUI entered.\n");
  show();
  qapplication->flushX();
}


void OpenSpeedshop::raiseGUI()
{
  dprintf("raiseGUI called!\n");

  QTimer::singleShot( 1, this, SLOT(raiseTheGUI()) );
}
