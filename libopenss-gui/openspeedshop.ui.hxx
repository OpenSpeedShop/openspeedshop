////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2016 Krell Institute  All Rights Reserved.
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
  

// Take the comment slashes off of define below for debug
// output for the GUI routines that are present this file.
//#define DEBUG_GUI 1
//

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
#include <qinputdialog.h>
#include <qsettings.h>
#include "PluginInfo.hxx"
#include "ArgumentObject.hxx"
#include "preferencesdialog.hxx"
#include <qapplication.h>

extern QApplication *qapplication;

#include "plugin_handler.hxx"
#include <ltdl.h>
#include <assert.h>

// #include "debug.hxx"  // This includes the definition of dprintf
#include "AttachProcessDialog.hxx"
#include "AttachMPProcessDialog.hxx"
#include "AttachProgramDialog.hxx"
#include "AttachMPProgramDialog.hxx"
#include "SelectExperimentDialog.hxx"
#include "SaveAsExperimentDialog.hxx"
#include "AttachArgDialog.hxx"

/*! Here are the needed globals for this application... */
#include "openspeedshop.hxx"
#include "DebugPanel.hxx"


#include "LoadAttachObject.hxx"

#include "CLIInterface.hxx"
static bool loadedFromSavedFile = FALSE;

void OpenSpeedshop::fileLoadNewProgram()
{
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::fileLoadNewProgram() entered\n");
#endif

  pidStr = QString::null;

  loadNewProgram();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

void OpenSpeedshop::fileAttachNewProcess()
{
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::fileAttachNewProcess() entered\n");
#endif

  executableName = QString::null;

  attachNewProcess();

  // Send out a message to all those that might care about this change request.
  LoadAttachObject *lao = new LoadAttachObject(executableName, pidStr);
  topPC->notifyNearest((char *)lao);
}

#ifdef SAVESESSION
void OpenSpeedshop::fileSaveSession()
{
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::fileSaveSession() entered\n");
#endif

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
#ifdef DEBUG_GUI
      printf("fileName.ascii() = (%s)\n", fileName.ascii() );
#endif
      fn = strdup(fileName.ascii());
    } else
    {
      return;
    }
  }

#ifdef DEBUG_GUI
  printf("go and save the setup...\n");
#endif
  if( !fileName.isEmpty() )
  {
    ((PanelContainer *)topPC)->savePanelContainerTree(fn);
    free(fn);
  }
}
#endif // SAVESESSION


void OpenSpeedshop::fileOpenExperiment(int selectedID)
{
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::fileOpenExperiment(%d) entered\n", selectedID );
#endif

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  SelectExperimentDialog *dialog = new SelectExperimentDialog(this, "Select Experiment To Open Dialog", TRUE);

  QString expStr;
  int count = 0;
  int id = 0;
  int expID = 0;
//  PanelListViewItem *item = dialog->updateAvailableExperimentList(&id, &count);
  PanelListViewItem *item = NULL;
  dialog->updateAvailableExperimentList(&id, &count);

  QApplication::restoreOverrideCursor();

#ifdef DEBUG_GUI
  printf("id=%d\n", id);
#endif

  if( selectedID > 0 ) {
    id = selectedID;
  }


  if( count == 0 ) {
    QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("No experiments defined.  Try the \"Intro Wizard\" to create a new experiment."), QMessageBox::Ok );
    return;
  }

  expID = id;

#ifdef DEBUG_GUI
  printf("OpenSpeedshop::fileOpenExperiment, count=%d selectedID=%d\n", count, selectedID );
#endif

  if( selectedID == 0 ) {
    if( dialog->exec() == QDialog::Accepted ) {

#ifdef DEBUG_GUI
       printf("QDialog::Accepted\n");
#endif

      item = dialog->selectedExperiment(&expID);

#ifdef DEBUG_GUI
      printf("OpenSpeedshop::fileOpenExperiment, item=0x%x\n", item);
#endif

      if( item == NULL || expID == 0 ) {
        return;
      }
    } else {
// printf("OpenSpeedshop::fileOpenExperiment, Cancel selected!\n");
      return;
    }
  }
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
// const char *name = p->getName();
// printf( "OpenSpeedshop::fileOpenExperiment, panel name = (%s)\n", name );
    p->getPanelContainer()->raisePanel(p);
  } else
  {
// printf("OpenSpeedshop::fileOpenExperiment, Create a new one!\n");
// printf("OpenSpeedshop::fileOpenExperiment, expID = (%d) \n", expID );
    QString expStr = QString("%1").arg(expID);

    QString command;
//    command = QString("listTypes -x %1").arg(expStr);
    command = QString("list -v exptypes -x %1").arg(expStr);
// printf("OpenSpeedshop::fileOpenExperiment, run command=(%s)\n", command.ascii() );
    std::list<std::string> list_of_collectors;
    
    InputLineObject *clip = NULL;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
  
    int knownCollectorType = FALSE;
    QString panel_type = "other";
    if( list_of_collectors.size() > 0 ) {

      for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ ) {

  //      std::string collector_name = *it;

        QString collector_name = (QString)*it;

#ifdef DEBUG_GUI
        printf("A: collector_name = (%s)\n", collector_name.ascii() );
#endif

        if( collector_name == "pcsamp" ) {
          knownCollectorType = TRUE;
          panel_type = "pc Sampling";
          break;
        } else if( collector_name == "usertime" ) {
          knownCollectorType = TRUE;
          panel_type = "User Time";
          break;
        } else if( collector_name == "fpe" ) {
          knownCollectorType = TRUE;
          panel_type = "FPE Tracing";
          break;
        } else if( collector_name == "hwcsamp" ) {
          panel_type = "HWCSamp Panel";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "hwctime" ) {
          panel_type = "HWCTime Panel";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "hwc" ) {
          panel_type = "HW Counter";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "iop" ) {
          knownCollectorType = TRUE;
          panel_type = "IOP";
          break;
        } else if( collector_name == "iot" ) {
          panel_type = "IOT";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "io" ) {
          panel_type = "IO";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "omptp" ) {
          panel_type = "OMPTP";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "pthreads" ) {
          panel_type = "PTHREADS";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mem" ) {
          panel_type = "MEM";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpip" ) {
          panel_type = "MPIP";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpit" ) {
          panel_type = "MPIT";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpiotf" ) {
           panel_type = "MPIOTF";
           knownCollectorType = TRUE;
           break;
        } else if( collector_name == "mpi" ) {
          panel_type = "MPI";
          knownCollectorType = TRUE;
          break;
        }

      }
    }

    if( knownCollectorType != TRUE )
    {
//      panel_type = "Construct New";
      panel_type = "Custom Experiment";
    }
  
    PanelContainer *bestFitPC = ((PanelContainer *)topPC)->findBestFitPanelContainer((PanelContainer *)topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expStr);
    ao->loadedFromSavedFile = loadedFromSavedFile;
    topPC->dl_create_and_add_panel((char *)panel_type.ascii(), bestFitPC, ao, (const char *)NULL );
    delete ao;

  }

  delete dialog;

  QApplication::restoreOverrideCursor();
}

void OpenSpeedshop::fileOpenSavedExperiment(QString filename, bool openPanel)
{

#ifdef DEBUG_GUI
 printf("OpenSpeedshop::fileOpenSavedExperiment() entered\n");
 if( !filename.isEmpty() ) {
    printf("OpenSpeedshop::fileOpenSavedExperiment() filename.ascii()=%s\n", filename.ascii());
 }
#endif

//printf("  Get a list of all the experiment files in the current directory\n");
//printf("  and in the environment variable >INSERTONEHERE<.   Then create\n");
//printf("  a dynamice menu with the list...    \n\n");
//printf("  When the list is selected, examine it for the type, then bring\n");
//printf("  up the associated experiment.    It would be nice, if the save\n");
//printf("  session information could be read to bring that experiment up\n");
//printf("  with the same layout as what it was left in during the prior\n");
//printf("  save.\n");

  QString fn  = QString::null;

  if( !filename.isEmpty() ) {
    fn = filename;
  } else {
    fn = QFileDialog::getOpenFileName(
                    "./",
                    "Open|SpeedShop files (*.openss);;Any Files(*.*)",
                    this,
                    "open file dialog",
                    "Choose a experiment file to open" );
  }

  if( !fn.isEmpty() ) {

    QString command;
    command = QString("expRestore -f %1").arg(fn);

//QMessageBox::information( (QWidget *)NULL, tr("Info: Unable to complete command"), tr("This feature currently under construction.\nCommand to be executed:\n%1").arg(command), QMessageBox::Ok );
//return;

    bool mark_value_for_delete = true;
    int64_t val = 0;

#ifdef DEBUG_GUI
    printf("OpenSpeedshop::fileOpenSavedExperiment() calling getIntValueFromCLI, command.ascii()=%s\n", 
           command.ascii());
#endif

    if( !cli->getIntValueFromCLI( (char *)command.ascii(), &val, mark_value_for_delete ) ) {

      QMessageBox::information( this, 
                                "No collector found for file.:", 
                                QString("Unable to issue command:\n  ")+command, 
                                QMessageBox::Ok );
    } else {

      loadedFromSavedFile = TRUE;

#ifdef DEBUG_GUI
    printf("OpenSpeedshop::fileOpenSavedExperiment() after calling getIntValueFromCLI, val=%d\n", val);
#endif

      if( openPanel ) {
        fileOpenExperiment(val);
      }

    }
  }
}

void OpenSpeedshop::fileSaveExperiment(int uid)
{
  PanelListViewItem *item = NULL;

#ifdef DEBUG_GUI
  printf("fileSaveExperiment(%d)\n", uid);
#endif


  int count = 0;
  int id = 0;
  SaveAsExperimentDialog *dialog = new SaveAsExperimentDialog(this, "Select Experiment To Save Dialog", TRUE);

  item = dialog->updateAvailableExperimentList(&id, &count);
  

  if( uid > 0 )
  {
    count = 1;
    id = uid;
  }

  if( count == 0 )
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::information( (QWidget *)NULL, tr("Info:"), tr("No experiments defined.  Try the \"Intro Wizard\" to create a new experiment."), QMessageBox::Ok );
    return;
  }

  int expID = 0;
  if( count == 1 )
  {
    expID = id;
  }

  if( uid == 0 && count > 1 )
  {
    if( dialog->exec() == QDialog::Accepted )
    {
      item = dialog->selectedExperiment(&expID);
// printf("item=0x%x\n", item);
      if( item == NULL || expID == 0 )
      {
        return;
      }
    }
  } else if( uid > 0 ) 
  {
    item = dialog->findExperiment(uid);
    if( item == NULL || expID == 0 )
    {
      return;
    }
  }

  const char *name = NULL;
  QString expStr = QString("%1").arg(expID);
  QString collectorName = QString("%1").arg(item->text(1));
  QString databaseName = QString("%1").arg(item->text(2));
// printf("expStr=(%s) collectorName=(%s) databaseName=(%s)\n", expStr.ascii(), collectorName.ascii(), databaseName.ascii() );
  QString dirName = "./";

  QFileDialog *sed = new QFileDialog(this, "file dialog", TRUE );
  sed->setCaption( QFileDialog::tr("Enter session name:") );
  sed->setMode( QFileDialog::AnyFile );
  QString types( "Open|SpeedShop files (*.openss);;Any Files(*.*)");
  sed->setFilters( types );
  sed->setDir(dirName);
//    const char *n = databaseName.ascii();
//    char *bn = basename( (char *)n );
  QString bn = QString("X%1.").arg(expID)+collectorName+".openss";
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

#include "qwidgetstack.h"
void OpenSpeedshop::filePreferences(QWidget *stackWidgetToRaise, QString panel_type)
{
// printf("filePreferences() entered.\n");
  if( stackWidgetToRaise )
  {
// printf("raise a particular panel's preferences\n");
    preferencesDialog->show();
    QWidgetStack *parent = (QWidgetStack *)stackWidgetToRaise->parent();
    parent->raiseWidget(stackWidgetToRaise);
    preferencesDialog->categoryListView->clearSelection();
    QListViewItem *item = preferencesDialog->categoryListView->findItem( panel_type, 0 );
    if( item )
    {
      preferencesDialog->categoryListView->setSelected(item, TRUE);
    }
  } else
  {
    preferencesDialog->show();
  }
}

#include "Commander.hxx"   // Contains the InputLineObject definition.
#include "GenericProgressDialog.hxx"
void OpenSpeedshop::fileExit()
{
// printf("fileExit() entered.\n");

  // REMOVE SESSION ONLY PREFERENCES
  QSettings *guiSettings = new QSettings(); 
  guiSettings->removeEntry((QString("/openspeedshop/ManageProcessesPanel/updateDisplayMultiProcessCommandLineEdit")));
  guiSettings->removeEntry((QString("/openspeedshop/ManageProcessesPanel/saveMPCommandCheckBox")));
  delete guiSettings;
  // END OF REMOVE SESSION ONLY PREFERENCES

  shuttingDown = TRUE;

//  QMessageBox::information( (QWidget *)this, tr("Info:"), tr("Closing down Open|SpeedShop... This may take a few seconds cleaning up...."),  QMessageBox::NoButton );

  pd = new GenericProgressDialog(this, "Exiting Open|SpeedShop", TRUE );
  pd->infoLabel->setText( tr("Waiting to exit: Cleaning up...") );

// printf("fileExit() close all external panel containers\n");
  topPC->closeAllExternalPanelContainers();

// printf("fileExit() now send the exit to the cli\n");
  CLIInterface *cli = topPC->getMainWindow()->cli;
  int wid = ((PanelContainer *)topPC)->getMainWindow()->widStr.toInt();
  InputLineObject *clip = cli->run_Append_Input_String( wid, "exit\n");

  qapplication->exit(0);
}


void OpenSpeedshop::fileClose()
{
  dprintf("fileClose() entered.\n");

  topLevelPanelContainersToHideList.clear();

  
  PanelContainer *pc = NULL;
  for( PanelContainerList::Iterator it = topPC->getMasterPCList()->begin();
               it != topPC->getMasterPCList()->end();
               it++ )
  {
    pc = (PanelContainer *)*it;
    if( pc->topLevel == TRUE && pc->outsidePC == TRUE )
    {
      // Don't close the masterPC here.  It can only be closed from
      // OpenSpeedShop::fileExit().
      if( strcmp(pc->getExternalName(),"masterPC") != 0 )
      { 
        topLevelPanelContainersToHideList.push_back(pc);
      }
    }
  }

  if( !topLevelPanelContainersToHideList.empty() )
  {
    for( PanelContainerList::Iterator it = topLevelPanelContainersToHideList.begin();
               it != topLevelPanelContainersToHideList.end();
               it++ )
    {
      pc = (PanelContainer *)*it;
      pc->topWidget->hide();
    }
  }
  hide();
}

void OpenSpeedshop::helpIndex()
{
  dprintf("helpIndex() entered.\n");
}

void OpenSpeedshop::helpContents()
{
 dprintf("helpContents() entered.\n");

  QString sub_path = QString::null;
  char *plugin_directory = NULL;

//  doc_dir = getenv("OPENSS_DOC_DIR");
  doc_dir = getenv("OPENSS_DOC_DIR");

  if( assistant == NULL )
  {
    assistant = new QAssistantClient(NULL);
    assistant->setArguments(QStringList("-hideSidebar"));
    QStringList slist;
    slist.append("-profile");


    if( !doc_dir )
    {
      const char *lt_dlpath = lt_dlgetsearchpath();
      QStringList fields = QStringList::split(":", lt_dlpath);
      for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it )
      {
        sub_path = ((QString)*it).stripWhiteSpace();
        //sub_path += "/../../share/doc/openspeedshop";
	sub_path += "/../../share/doc/packages/OpenSpeedShop";
// printf("sub_path=(%s)\n", sub_path.ascii() );
        QFileInfo *fileInfo = new QFileInfo(QDir(sub_path), "help.adp");
        if( fileInfo->exists() )
        {
//          doc_dir = strdup(sub_path.ascii());
          doc_dir = sub_path.ascii();
          break;
        }
  
      }
    }
    slist.append(QString(doc_dir)+"/help.adp");
   
    assistant->setArguments(slist);
  }
  
  assistant->openAssistant();
  QString s = QString("%1/users_guide/index.html").arg(doc_dir);

  assistant->showPage( s );
}

#include "AboutClass.hxx"
void OpenSpeedshop::helpAbout()
{
// printf("helpAbout() entered.\n");

 AboutClass *aboutClass = new AboutClass(topPC);
 aboutClass->show();
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
          bestFitPC->addPanel((Panel *)debugPanel, bestFitPC, (char *) "Debug Panel");

          return(TRUE);
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
// printf("OpenSpeedShop::init() called.\n");
  if( hostStr.isEmpty() )
  {
    char host_name_buffer[1024];
    if( gethostname(host_name_buffer, 1024) == 0 )
    {
      hostStr = host_name_buffer;
    }
  }

//  topPL = this;
  char pc_plugin_file[2048];
  // Insure the libltdl user-defined library search path has been set
  assert(lt_dlgetsearchpath() != NULL);
  // Load base and plugin libraries
  char *pc_dl_name=(char *)"libopenss-guibase";
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
  char *ph_dl_name = (char *)"libopenss-guiplugin";
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
#ifdef DEBUG_GUI
      printf("const char *)pi->plugin_name.ascii() = (%s)\n", (const char *)pi->plugin_name );
#endif
      lt_dlhandle dl_object = lt_dlopenext((const char *)pi->plugin_name);
//      lt_dlhandle dl_object = lt_dlopen((const char *)pi->plugin_name);

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
//  topPC->dl_create_and_add_panel("Intro Wizard", topPC->leftPanelContainer);
  topPC->dl_create_and_add_panel("Command Panel", topPC->rightPanelContainer);
#endif // SAVESESSION


   AppEventFilter *myEventFilter = new AppEventFilter(this, masterPC);
   qApp->installEventFilter( myEventFilter );

  qapplication->connect( qApp, SIGNAL( lastWindowClosed() ), this, SLOT( myQuit() ) );


  // REMOVE SESSION ONLY PREFERENCES
  // In case these were not removed when exiting because of a crash.  Make sure
  // they are not present when starting up the GUI because they are session only
  // preferences.
  QSettings *guiSettings = new QSettings(); 
  guiSettings->removeEntry((QString("/openspeedshop/ManageProcessesPanel/updateDisplayMultiProcessCommandLineEdit")));
  guiSettings->removeEntry((QString("/openspeedshop/ManageProcessesPanel/saveMPCommandCheckBox")));
  delete guiSettings;
  // END OF REMOVE SESSION ONLY PREFERENCES

}

void OpenSpeedshop::destroy()
{
  //fprintf(stderr, "OpenSpeedshop::destroy() entered.\n");
  qApp->restoreOverrideCursor();
}

#if 0
void OpenSpeedshop::loadArgumentFile()
{
  if( Arglfd == NULL ) {
    Arglfd = new AttachArgDialog(this, "file dialog", TRUE );
  }
}
#endif
 

void OpenSpeedshop::loadArgumentFile()
{
  QString dirName = QString::null;
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::loadArgumentFile() entered \n");
#endif

  if( Arglfd == NULL ) {
    Arglfd = new AttachArgDialog(this, "file dialog", TRUE );
#ifdef DEBUG_GUI
    printf("OpenSpeedshop::loadArgumentFile() Arglfd=%0x\n", Arglfd);
#endif

    Arglfd->setCaption( QFileDialog::tr("Enter application argument file:") );
    Arglfd->setMode( QFileDialog::AnyFile );
    QString types(
                  "Any Files (*);;"
                  "Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "(*.c *.cpp *.cxx *.C *.c++ *.f* *.F*);;"
                  );
    Arglfd->setFilters( types );
    Arglfd->setDir(dirName);
  }
  Arglfd->setSelection(QString::null);

  QString fileName = QString::null;
  if( Arglfd->exec() == QDialog::Accepted )
  {
    fileName = Arglfd->selectedFile();
    if( !fileName.isEmpty() )
    {
#ifdef DEBUG_GUI
      printf("OpenSpeedshop::loadArgumentFile(), fileName.ascii() = (%s)\n", fileName.ascii() );
#endif
      QFileInfo fi(fileName);
      ArgFileName = fileName;
    }
  }
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::loadArgumentFile() exited\n");
#endif
}


Panel* OpenSpeedshop::loadNewProgramPanel( PanelContainer *pc, PanelContainer *topPC, int64_t expID, Panel *targetPanel, bool isOffline)
{
    QString name = "loadPanel";
#ifdef DEBUG_GUI
    printf("OpenSpeedshop::loadNewProgramPanel() trying to raise name.ascii()=%s, isOffline=%d\n", (char *)name.ascii(), isOffline );
    printf("OpenSpeedshop::loadNewProgramPanel() targetPanel=%d\n", targetPanel);
    if (targetPanel) {
      printf("OpenSpeedshop::loadNewProgramPanel() targetPanel->getName()=%s\n",  (char *)targetPanel->getName() );
    }
#endif

//    Panel *myLoadPanel = pc->findNamedPanel(pc->getMasterPC(), (char *)name.ascii() );

//    QString name = QString("Load Panel [%1]").arg(expID);
//    Panel *myLoadPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );


    PanelContainer *bestFitPC = pc->getMasterPC()->findBestFitPanelContainer(topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", targetPanel);
    ao->isInstrumentorOffline = isOffline;
//printf("OpenSpeedshop::loadNewProgramPanel() create a new load Panel!!\n");
    Panel *myLoadPanel = topPC->dl_create_and_add_panel("loadPanel", bestFitPC, ao, (const char *)NULL);
    delete ao;



#ifdef DEBUG_GUI
    printf("OpenSpeedshop::loadNewProgramPanel() trying to raise name.ascii()=%s, myLoadPanel=%d\n", (char *)name.ascii(), myLoadPanel);
#endif

    if( myLoadPanel ) {
      // raise the panel using the passed in panel container
#ifdef DEBUG_GUI
      printf("OpenSpeedshop::loadNewProgramPanel() raising name.ascii()=%s\n", (char *)name.ascii() );
#endif
      pc->raisePanel(myLoadPanel);
    } else {
#ifdef DEBUG_GUI
      printf("OpenSpeedshop::loadNewProgramPanel() unable to find Load Panel\n");
#endif
    }

  return myLoadPanel;
}

void OpenSpeedshop::loadNewProgram()
{

// all load programs requests come through here!
  QString dirName = QString::null;

#ifdef DEBUG_GUI
  printf("OpenSpeedshop::loadNewProgram()\n");
#endif
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
    lfd->setViewMode( QFileDialog::Detail );
    lfd->setDir(dirName);
  }
  lfd->setSelection(QString::null);

  QString fileName = QString::null;
  if( lfd->exec() == QDialog::Accepted )
  {
    fileName = lfd->selectedFile();
    if( !fileName.isEmpty() )
    {
//    printf("fileName.ascii() = (%s)\n", fileName.ascii() );
      QFileInfo fi(fileName);
      if( !fi.isExecutable() )
      {
        QMessageBox::information( (QWidget *)this, tr("Info:"), tr("The selected file is not executable."), QMessageBox::Ok );
        return;
      }
      executableName = fileName;
      if( lfd->lineedit->text().isEmpty() ) {

        // If you want a small dialog box to popup after hitting ok in the main dialog box then
        // enable this Enter Arguments Dialog section of code directly following this comment line...
        // BEGIN add Enter Arguments Dialog back in jeg 03/12/2007 #if 0
#if 0
        argsStr = QString::null;
        bool ok;
        argsStr = QInputDialog::getText("Enter Arguments Dialog:", QString("Enter command line arguments:"), QLineEdit::Normal, QString::null, &ok, this);
#endif
        // END add Enter Arguments Dialog back in jeg 03/12/2007 #endif
      } else {
        argsStr = lfd->lineedit->text();
#ifdef DEBUG_GUI
        printf("line argsStr=(%s)\n", argsStr.ascii() );
#endif
      }
#if MPP
      if( lfd->parallelPrefixLineedit->text().isEmpty() )
      {
        // If you want a small dialog box to popup after hitting ok in the main dialog box then
        //   enable this Enter Arguments Dialog section of code directly following this comment line...
#if 0
        parallelPrefixCommandStr = QString::null;
        bool ok;
        parallelPrefixCommandStr = QInputDialog::getText("Enter Arguments Dialog:", QString("Enter parallel command prefix:"), QLineEdit::Normal, QString::null, &ok, this);
#endif
      } else
      {
        parallelPrefixCommandStr = lfd->parallelPrefixLineedit->text();
//      printf("parallelPrefixLinedit--> parallelPrefixCommandStr=(%s)\n", parallelPrefixCommandStr.ascii() );
      }
#endif
    }
  }

}

void OpenSpeedshop::loadNewMultiProcessProgram()
{

  // all multi-process load programs requests come through here!

  QString dirName = QString::null;

#ifdef DEBUG_GUI
  printf("OpenSpeedshop::loadNewMultiProcessProgram(), MPlfd=%0llx\n", MPlfd);
#endif

  if( MPlfd == NULL ) {

    MPlfd = new AttachMPProgramDialog(this, "file dialog", TRUE );

#ifdef DEBUG_GUI
    printf("OpenSpeedshop::loadNewMultiProcessProgram(), after new Att, MPlfd=%0llx\n", MPlfd);
#endif

    MPlfd->setCaption( QFileDialog::tr("Enter multiprocess executable or saved experiment:") );
//    MPlfd->setMode( QFileDialog::AnyFile );
    QString types(
                  "Any Files (*);;"
                  "Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "(*.c *.cpp *.cxx *.C *.c++ *.f* *.F*);;"
                  );
    MPlfd->setFilters( types );
//    MPlfd->setViewMode( QFileDialog::Detail );
    MPlfd->setDir(dirName);

  }

  MPlfd->setSelection(QString::null);

  QString fileName = QString::null;
  if( MPlfd->exec() == QDialog::Accepted )
  {
    fileName = MPlfd->selectedFile();
    if( !fileName.isEmpty() )
    {
#ifdef DEBUG_GUI
      printf("loadNewMultiProcessProgram(), fileName.ascii() = (%s)\n", fileName.ascii() );
#endif
      QFileInfo fi(fileName);
      if( !fi.isExecutable() )
      {
        QMessageBox::information( (QWidget *)this, tr("Info:"), tr("The selected file is not executable."), QMessageBox::Ok );
        return;
      }
      executableName = fileName;
      if( MPlfd->lineedit->text().isEmpty() ) {

        // If you want a small dialog box to popup after hitting ok in the main dialog box then
        // enable this Enter Arguments Dialog section of code directly following this comment line...
        // BEGIN add Enter Arguments Dialog back in jeg 03/12/2007 #if 0
        argsStr = QString::null;
        bool ok;
        argsStr = QInputDialog::getText("Enter Arguments Dialog:", QString("Enter command line arguments:"), QLineEdit::Normal, QString::null, &ok, this);
        // END add Enter Arguments Dialog back in jeg 03/12/2007 #endif
      } else
      {
        argsStr = MPlfd->lineedit->text();
#ifdef DEBUG_GUI
        printf("loadNewMultiProcessProgram(), line argsStr=(%s)\n", argsStr.ascii() );
#endif
      }
      if( MPlfd->parallelPrefixLineedit->text().isEmpty() )
      {
        // If you want a small dialog box to popup after hitting ok in the main dialog box then
        //   enable this Enter Arguments Dialog section of code directly following this comment line...
#if 0
        parallelPrefixCommandStr = QString::null;
        bool ok;
        parallelPrefixCommandStr = QInputDialog::getText("Enter Arguments Dialog:", QString("Enter parallel command prefix:"), QLineEdit::Normal, QString::null, &ok, this);
#endif
      } else {
        parallelPrefixCommandStr = MPlfd->parallelPrefixLineedit->text();
#ifdef DEBUG_GUI
        printf("loadNewMultiProcessProgram(), parallelPrefixLinedit--> parallelPrefixCommandStr=(%s)\n", parallelPrefixCommandStr.ascii() );
#endif
      }
    }
  }
}

void OpenSpeedshop::attachNewProcess()
{
  mpiFLAG = FALSE;
  if( afd == NULL )
  {
    afd = new AttachProcessDialog(this, "AttachProcessDialog", TRUE);
  }
  if( afd->exec() == QDialog::Accepted )
  {
    pidStrList = afd->selectedProcesses(&mpiFLAG);

    pidStr = QString::null;
    for( QStringList::Iterator it = pidStrList->begin();
               it != pidStrList->end();
               it++ )
    {
      QString qs = (QString)*it;

      QString host_name = qs.section(' ', 0, 0, QString::SectionSkipEmpty);
      QString pid_name = qs.section(' ', 1, 1, QString::SectionSkipEmpty);
      QString prog_name = qs.section(' ', 2, 2, QString::SectionSkipEmpty);

      hostStr = host_name;

      if( pidStr == QString::null )
      {
        pidStr += pid_name;
      } else
      {
        pidStr += ",";
        pidStr += pid_name;
      }
      
    }

  }
}

void OpenSpeedshop::attachNewMultiProcess()
{
  mpiFLAG = FALSE;

  if( MPafd == NULL ) {
    MPafd = new AttachMPProcessDialog(this, "AttachMPProcessDialog", TRUE);
  }

  if( MPafd->exec() == QDialog::Accepted ) {
    pidStrList = MPafd->selectedProcesses(&mpiFLAG);

    pidStr = QString::null;
    for( QStringList::Iterator it = pidStrList->begin();
               it != pidStrList->end();
               it++ )
    {
      QString qs = (QString)*it;

      QString host_name = qs.section(' ', 0, 0, QString::SectionSkipEmpty);
      QString pid_name = qs.section(' ', 1, 1, QString::SectionSkipEmpty);
      QString prog_name = qs.section(' ', 2, 2, QString::SectionSkipEmpty);

      hostStr = host_name;

      if( pidStr == QString::null ) {
        pidStr += pid_name;
      } else {
        pidStr += ",";
        pidStr += pid_name;
      }
      
    }

  }
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
  PanelContainer *pc = NULL;
  if( !topLevelPanelContainersToHideList.empty() )
  {
    for( PanelContainerList::Iterator it = topLevelPanelContainersToHideList.begin();
               it != topLevelPanelContainersToHideList.end();
               it++ )
    {
      pc = (PanelContainer *)*it;
      pc->topWidget->show();
    }
  }
  qapplication->flushX();
}


void OpenSpeedshop::raiseGUI()
{
  dprintf("raiseGUI called!\n");

  QTimer::singleShot( 1, this, SLOT(raiseTheGUI()) );
}

static bool step_forward = TRUE;
void
OpenSpeedshop::progressUpdate()
{
  pd->qs->setValue( steps );
  if( step_forward )
  {
    steps++;
  } else
  {
    steps--;
  }
  if( steps == 10 )
  {
    step_forward = FALSE;
  } else if( steps == 0 )
  {
    step_forward = TRUE;
  }
}


int
OpenSpeedshop::lookForExperiment(bool hadOfflineArg)
{
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::lookForExperiment(),The user may have loaded an experiment... as there was something on the command line.\n");
  printf("OpenSpeedshop::lookForExperiment(), hadOfflineArg=%d\n", hadOfflineArg);
#endif

  QString command;
  std::list<int64_t> int_list;

  steps = 0;
  pd = new GenericProgressDialog(this, "Progress Dialog", TRUE );
  pd->infoLabel->setText( tr("Processing command line initialization...") );
  QTimer *loadTimer = new QTimer( this, "progressTimer" );
  connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
  loadTimer->start( 1000 );

  pd->show();

  command = QString("list -v exp");
#ifdef DEBUG_GUI
  printf("command.ascii()=%s\n", command.ascii() );
#endif

  int_list.clear();

  InputLineObject *clip = NULL;

  if( !cli->getIntListValueFromCLI( (char *)command.ascii(), &int_list, clip, TRUE ) )
  {

#ifdef DEBUG_GUI
    printf("Unable to run %s command.\n", command.ascii() );
#endif

    QMessageBox::information(this, QString(tr("Initialization warning:")), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );
  }

  std::list<int64_t>::iterator it;
#ifdef DEBUG_GUI
  printf("OpenSpeedshop::lookForExperiment(), int_list.size() =%d\n", int_list.size() );
#endif
  if (int_list.size() > 0) {
  for(it = int_list.begin(); it != int_list.end(); it++ )
  {
    int64_t expID = (int64_t)(*it);

    QString expStr = QString("%1").arg(expID);

//    command = QString("listTypes -x %1").arg(expStr);
    command = QString("list -v expTypes -x %1").arg(expStr);
    std::list<std::string> list_of_collectors;
    if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
           &list_of_collectors, clip, TRUE ) )
    {
#ifdef DEBUG_GUI
      printf("Unable to run %s command.\n", command.ascii() );
#endif
      
      loadTimer->stop();
      delete loadTimer;
      pd->hide();

      QMessageBox::information(this, QString(tr("Initialization warning:")), QString("Unable to run \"%1\" command.").arg(command.ascii()), QMessageBox::Ok );

      return 0;
    }
  
    int knownCollectorType = FALSE;
    QString panel_type = "other";
    if( list_of_collectors.size() > 0 ) {

      for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
      {

//      std::string collector_name = *it;

        QString collector_name = (QString)*it;

#ifdef DEBUG_GUI
        printf("B: collector_name=(%s)\n", collector_name.ascii() );
#endif

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
        } else if( collector_name == "hwc" )
        {
          panel_type = "HW Counter";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "hwcsamp" )
        {
          panel_type = "HWCSamp Panel";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "hwctime" )
        {
          panel_type = "HWCTime Panel";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "iop" )
        {
          panel_type = "IOP";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "io" )
        {
          panel_type = "IO";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "iot" )
        {
          panel_type = "IOT";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "omptp" )
        {
          panel_type = "OMPTP";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "pthreads" )
        {
          panel_type = "PTHREADS";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mem" )
        {
          panel_type = "MEM";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpi" )
        {
          panel_type = "MPI";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpip" )
        {
          panel_type = "MPIP";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpit" )
        {
          panel_type = "MPIT";
          knownCollectorType = TRUE;
          break;
        } else if( collector_name == "mpiotf" )
        {
           panel_type = "MPIOTF";
           knownCollectorType = TRUE;
           break;
        }
      }
    }
  
    if( knownCollectorType != TRUE )
    {
//      panel_type = "Construct New";
      panel_type = "Custom Experiment";
    }

#ifdef DEBUG_GUI
   printf("pane_type.ascii() = %s\n", panel_type.ascii() );
#endif
    PanelContainer *bestFitPC = ((PanelContainer *)topPC)->findBestFitPanelContainer((PanelContainer *)topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expStr);
    // Set the instrumentor mode flag to what we saw on the command line
    ao->isInstrumentorOffline = hadOfflineArg;
#ifdef DEBUG_GUI
    printf("calling topPC->dl_create_and_add_panel, with panel_type.ascii()=\n", panel_type.ascii() );
#endif
    topPC->dl_create_and_add_panel((char *)panel_type.ascii(), bestFitPC, ao);
    delete ao;
  }
 }

  loadTimer->stop();
  delete loadTimer;
  pd->hide();

#ifdef DEBUG_GUI
  printf("return %d\n", int_list.size() );
#endif

  return( int_list.size() );
}

void
OpenSpeedshop::loadTheWizard()
{
  topPC->dl_create_and_add_panel("Intro Wizard", topPC->leftPanelContainer);
}
