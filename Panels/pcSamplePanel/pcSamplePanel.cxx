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


#include "pcSamplePanel.hxx"   // Change this to your new class header file name
#include "PanelContainer.hxx"   // Do not remove
#include "plugin_entry_point.hxx"   // Do not remove

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qiconset.h>
#include <qfileinfo.h>
#include <qbitmap.h>

#include <qmessagebox.h>

#include "SourcePanel.hxx"
#include "UpdateObject.hxx"
#include "SourceObject.hxx"
#include "TopPanel.hxx"

#include "LoadAttachObject.hxx"

#include "SS_Input_Manager.hxx"
#include "CLIInterface.hxx"

using namespace OpenSpeedShop::Framework;


/*!  pcSamplePanel Class

     Autor: Al Stipek (stipek@sgi.com)
 */


/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
pcSamplePanel::pcSamplePanel(PanelContainer *pc, const char *n, void *argument) : Panel(pc, n)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("pcSamplePanel::pcSamplePanel() constructor called\n");

  executableNameStr = QString::null;
  pidStr = QString::null;

  mw = getPanelContainer()->getMainWindow();

  expID = -1;
  if( argument )
  {
    // We have an existing experiment, load the executable or pid if we 
    // have one associated.  (TODO)
    QString *expIDString = (QString *)argument;
    if( (int)argument == -1 )
    {
      printf("we're coming in from the pcSampleWizardPanel.\n");
      // We're comming in cold,
      // Check to see if there's a suggested executable or pid ...
      if( !mw->executableName.isEmpty() )
      {
        executableNameStr = mw->executableName;
      } else if( !mw->pidStr.isEmpty() )
      {
        pidStr = mw->pidStr;
      }
    } else if( expIDString->toInt() > 0 )
    {
      expID = expIDString->toInt();
      printf("we're coming in with an expID=%d\n", expID);
    }
  } else
  {
    printf("We're coming in cold (i.e. from main menu)\n");
  }


  frameLayout = new QVBoxLayout( getBaseWidgetFrame(), 1, 2, getName() );

  pco = new ProcessControlObject(frameLayout, getBaseWidgetFrame(), (Panel *)this );
  runnableFLAG = FALSE;
  pco->runButton->setEnabled(FALSE);
  pco->runButton->enabledFLAG = FALSE;

  statusLayout = new QHBoxLayout( 0, 10, 0, "statusLayout" );
  
  statusLabel = new QLabel( getBaseWidgetFrame(), "statusLabel");
  statusLayout->addWidget( statusLabel );

  statusLabelText = new QLineEdit( getBaseWidgetFrame(), "statusLabelText");
  statusLabelText->setReadOnly(TRUE);
  statusLayout->addWidget( statusLabelText );

  frameLayout->addLayout( statusLayout );

  languageChange();

  PanelContainerList *lpcl = new PanelContainerList();
  lpcl->clear();

  QWidget *pcSampleControlPanelContainerWidget = new QWidget( getBaseWidgetFrame(),
                                        "pcSampleControlPanelContainerWidget" );
  topPC = createPanelContainer( pcSampleControlPanelContainerWidget,
                              "PCSamplingControlPanel_topPC", NULL,
                              pc->getMasterPCList() );
  frameLayout->addWidget( pcSampleControlPanelContainerWidget );

OpenSpeedshop *mw = getPanelContainer()->getMainWindow();
// printf("Create a new pcSample experiment.\n");


  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

if( expID == -1 )
{
// We're coming in cold, or we're coming in from the pcSampleWizardPanel.
  char command[1024];
// NOTE: "example" is used in place of "pcsamp" because it works for the
//       test case.   "example" will eventually be replaced again with "pcsamp."
  if( !executableNameStr.isEmpty() )
  {
    sprintf(command, "expCreate -f %s example\n", executableNameStr.ascii() );
  } else if( !pidStr.isEmpty() )
  { 
    sprintf(command, "expCreate -x %s example\n", pidStr.ascii() );
  } else
  {
    sprintf(command, "expCreate example\n" );
  }
  bool mark_value_for_delete = true;
  int64_t val = 0;


steps = 0;
pd = new QProgressDialog("Loading process in progress.", NULL, 1000, this, NULL, TRUE);
loadTimer = new QTimer( this, "progressTimer" );
connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
loadTimer->start( 0 );
pd->show();
statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

runnableFLAG = FALSE;
pco->runButton->setEnabled(FALSE);
pco->runButton->enabledFLAG = FALSE;
printf("Attempting to do an (%s)\n", command );

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  if( !cli->getIntValueFromCLI(command, &val, mark_value_for_delete, 60000 ) )
  {
    fprintf(stderr, "Error retreiving experiment id. \n");
//    return;
  }
  expID = val;

//  fprintf(stdout, "A: MY VALUE! = (%d)\n", val);
  statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the Run button to begin the experiment.")) );
loadTimer->stop();
pd->cancel();
pd->hide();

if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
{
  runnableFLAG = TRUE;
  pco->runButton->setEnabled(TRUE);
  pco->runButton->enabledFLAG = TRUE;
}
}

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);
  groupID = expID;


  pcSampleControlPanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;


  SourcePanel *sp = (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", topPC, (char *)expID);

// Begin demo position at dummy file... For the real stuff we'll need to 
// look up the main()... and position at it...
if( expID == -1 )
{
  if( mw && !executableNameStr.isEmpty() && executableNameStr.endsWith("fred_calls_ted") )
  {
    char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
    char buffer[200];
    strcpy(buffer, plugin_directory);
    strcat(buffer, "/../../../usability/phaseI/fred_calls_ted.c");
    nprintf( DEBUG_CONST_DESTRUCT ) ("load (%s)\n", buffer);
    SourceObject *spo = new SourceObject("main", buffer, 22, TRUE, NULL);
  
    if( !sp->listener((void *)spo) )
    {
      fprintf(stderr, "Unable to position at main in %s\n", buffer);
    } else
    {
      nprintf( DEBUG_CONST_DESTRUCT ) ("Positioned at main in %s ????? \n", buffer);
    }
  }
}


}


//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
pcSamplePanel::~pcSamplePanel()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("  pcSamplePanel::~pcSamplePanel() destructor called\n");
  delete frameLayout;

  char command[1024];
  sprintf(command, "expClose -x %d", expID );

  if( !QMessageBox::question( this,
            tr("Delete (expClose) the experiment?"),
            tr("Selecting Yes will delete the experiment.\n"
                "Selecting No will only close the window."),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
  {
    printf("NOTE: This does not need to be a syncronous call.\n");
    bool mark_value_for_delete = true;
    int64_t val = 0;
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    if( !cli->runSynchronousCLI(command) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
      return;
    }
  }
}

//! Add user panel specific menu items if they have any.
bool
pcSamplePanel::menu(QPopupMenu* contextMenu)
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::menu() requested.\n");

  contextMenu->insertSeparator();
  contextMenu->insertItem("&Save As ...", this, SLOT(saveAsSelected()), CTRL+Key_S ); 
  contextMenu->insertSeparator();
  contextMenu->insertItem(tr("Load &New Program..."), this, SLOT(loadNewProgramSelected()), CTRL+Key_N );
  contextMenu->insertItem(tr("Detach &From Program..."), this, SLOT(detachFromProgramSelected()), CTRL+Key_N );
  contextMenu->insertItem(tr("Attach To &Executable..."), this, SLOT(attachToExecutableSelected()), CTRL+Key_E );
  contextMenu->insertSeparator();
  contextMenu->insertItem(tr("&Manage Collectors..."), this, SLOT(manageCollectorsSelected()), CTRL+Key_M );
  contextMenu->insertItem(tr("Manage &Processes..."), this, SLOT(manageProcessesSelected()), CTRL+Key_P );
  contextMenu->insertItem(tr("&Manage &Data Sets..."), this, SLOT(manageDataSetsSelected()), CTRL+Key_D );
  contextMenu->insertSeparator();
  contextMenu->insertItem(tr("S&ource Panel..."), this, SLOT(loadSourcePanel()), CTRL+Key_O );
  contextMenu->insertItem(tr("S&tats Panel..."), this, SLOT(loadStatsPanel()), CTRL+Key_T );

  return( TRUE );
}

void
pcSamplePanel::loadNewProgramSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::loadNewProgramSelected()\n");
printf("pcSamplePanel::loadNewProgramSelected()\n");
  if( runnableFLAG == TRUE )
  {
    nprintf( DEBUG_PANELS ) ("Disconnect First?\n"); 
    if( detachFromProgramSelected() == FALSE )
    {
      return;
    }
  }
  if( mw )
  {
    mw->executableName = QString::null;
    mw->pidStr = QString::null;
    mw->loadNewProgram();
    if( !mw->executableName.isEmpty() )
    {
      executableNameStr = mw->executableName;

 printf("Attempt to load %s\n", mw->executableName.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

    char command[1024];
    sprintf(command, "expAttach -f %s\n", executableNameStr.ascii() );

    steps = 0;
    pd = new QProgressDialog("Loading process in progress.", NULL, 1000, this, NULL, TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    printf("Attempting to do an (%s)\n", command );

    if( !cli->runSynchronousCLI(command) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
  //    return;
    }

    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the Run button to begin the experiment.")) );
    loadTimer->stop();
    pd->cancel();
    pd->hide();

    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;

    } else if( !mw->pidStr.isEmpty() )
    {
      pidStr = mw->pidStr;
    }
    if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
    {
      updateInitialStatus();
    }
  }
}   

bool
pcSamplePanel::detachFromProgramSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::detachFromProgramSelected()\n");

  
  if( QMessageBox::question(
            this,
            tr("Detach?"),
            tr("Process or executable already attached: Do you want to detach from the exising process(es)?"),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
  {
    return FALSE;
  }

  if( mw )
  {
    mw->executableName = QString::null;
    mw->pidStr = QString::null;
  }

  updateInitialStatus();

  SourceObject *spo = new SourceObject(QString::null, QString::null, 0, TRUE, NULL);

  broadcast((char *)spo, NEAREST_T);

runnableFLAG = FALSE;
}

void
pcSamplePanel::attachToExecutableSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::attachToExecutableSelected()\n");
printf("pcSamplePanel::attachToExecutableSelected()\n");
  if( runnableFLAG == TRUE )
  {
    if( detachFromProgramSelected() == FALSE )
    {
      return;
    }
  }
  if( mw )
  {
    mw->executableName = QString::null;
    mw->pidStr = QString::null;
    mw->attachNewProcess();
  }

  if( !mw->pidStr.isEmpty() )
  {
 printf("Attempt to load %s\n", mw->pidStr.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

    char command[1024];
    sprintf(command, "expAttach -p %d\n", pidStr.toInt() );

    steps = 0;
    pd = new QProgressDialog("Loading process in progress.", NULL, 1000, this, NULL, TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->pidStr);

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    printf("Attempting to do an (%s)\n", command );

    int64_t val = 0;  // unused
    bool mark_value_for_delete = true;
    if( !cli->getIntValueFromCLI(command, &val, mark_value_for_delete, 60000 ) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
  //    return;
    }

    statusLabelText->setText( tr(QString("Attached:  "))+mw->pidStr+tr(QString("  Click on the Continu button to continue with the experiment.")) );
    loadTimer->stop();
    pd->cancel();
    pd->hide();

    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;
pco->continueButton->setEnabled(TRUE);
pco->continueButton->enabledFLAG = TRUE;
  }
}   

void
pcSamplePanel::manageCollectorsSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::manageCollectorsSelected()\n");
}   

void
pcSamplePanel::manageProcessesSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::managerProcessesSelected()\n");
}   

void
pcSamplePanel::manageDataSetsSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::managerDataSetsSelected()\n");
}   

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
pcSamplePanel::save()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
pcSamplePanel::saveAs()
{
  nprintf( DEBUG_SAVEAS ) ("pcSamplePanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
pcSamplePanel::listener(void *msg)
{
  nprintf( DEBUG_MESSAGES ) ("pcSamplePanel::listener() requested.\n");
  int ret_val = 0; // zero means we didn't handle the message.

  ControlObject *co = NULL;
  LoadAttachObject *lao = NULL;

  MessageObject *mo = (MessageObject *)msg;

  if( mo->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("pcSamplePanel::listener() interested!\n");
    getPanelContainer()->raisePanel(this);
    return 1;
  }


  if( mo->msgType  == "ControlObject" )
  {
    co = (ControlObject *)msg;
    nprintf( DEBUG_MESSAGES ) ("we've got a ControlObject\n");
  } else if( mo->msgType  == "LoadAttachObject" )
  {
    lao = (LoadAttachObject *)msg;
    nprintf( DEBUG_MESSAGES ) ("we've got a LoadAttachObject\n");
  } else
  {
//    fprintf(stderr, "Unknown object type recieved.\n");
//    fprintf(stderr, "msgType = %s\n", mo->msgType.ascii() );
    return 0;  // 0 means, did not act on message
  }

  if( co )
  {
//    if( DEBUG_MESSAGES )
//    {
//      co->print();
//    }

char command[1024];
bool mark_value_for_delete = true;
int64_t val = 0;
CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;


    switch( (int)co->cot )
    {
      case  ATTACH_PROCESS_T:
sprintf(command, "attach a process collector %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        nprintf( DEBUG_MESSAGES ) ("Attach to a process\n");
        break;
      case  DETACH_PROCESS_T:
sprintf(command, "detach a process %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        nprintf( DEBUG_MESSAGES ) ("Detach from a process\n");
        ret_val = 1;
        break;
      case  ATTACH_COLLECTOR_T:
sprintf(command, "attach a collector %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        nprintf( DEBUG_MESSAGES ) ("Attach to a collector\n");
        ret_val = 1;
        break;
      case  REMOVE_COLLECTOR_T:
sprintf(command, "remove a collector %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        nprintf( DEBUG_MESSAGES ) ("Remove a collector\n");
        ret_val = 1;
        break;
      case  RUN_T:
#ifdef OLDWAY
{
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    Experiment *experiment = eo->FW();
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    Thread thread = *ti;
    CollectorGroup cgrp = experiment->getCollectors();
    CollectorGroup::iterator ci = cgrp.begin();
    Collector collector =  *ci;
    collector.startCollecting();
  }
}
#endif // OLDWAY
sprintf(command, "expGo -x %d\n", expID);
{
int status = -1;
nprintf( DEBUG_MESSAGES ) ("Run\n");
statusLabelText->setText( tr("Process running...") );

int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
InputLineObject *clip = Append_Input_String( wid, command);
ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
if( eo && eo->FW() )
{
  status = eo->Determine_Status();
  while( status != ExpStatus_Terminated )
  {
    printf("sleep(1)\n");
    sleep(1);
    qApp->processEvents(1000);
    if( status == ExpStatus_InError )
    {
      printf("Process errored.\n");
      statusLabelText->setText( tr("Process errored...") );
    }
    status = eo->Determine_Status();
  }
}
if( status == ExpStatus_Terminated )
{
  statusLabelText->setText( tr("Process finished...") );
}

if( status == ExpStatus_Terminated || status == ExpStatus_InError )
{
  pco->runButton->setEnabled(TRUE);
  pco->runButton->enabledFLAG = TRUE;
  runnableFLAG = TRUE;
  pco->pauseButton->setEnabled(FALSE);
  pco->pauseButton->enabledFLAG = FALSE;
  pco->continueButton->setEnabled(FALSE);
  pco->continueButton->setEnabled(FALSE);
  pco->continueButton->enabledFLAG = FALSE;
  pco->updateButton->setEnabled(TRUE);
  pco->updateButton->setEnabled(TRUE);
  pco->updateButton->enabledFLAG = TRUE;
  pco->terminateButton->setEnabled(FALSE);
  pco->terminateButton->setFlat(TRUE);
  pco->terminateButton->setEnabled(FALSE);
} else
{
  pco->runButton->setEnabled(FALSE);
  pco->runButton->enabledFLAG = FALSE;
  runnableFLAG = FALSE;
  pco->pauseButton->setEnabled(TRUE);
  pco->pauseButton->enabledFLAG = TRUE;
  pco->continueButton->setEnabled(FALSE);
  pco->continueButton->setEnabled(FALSE);
  pco->continueButton->enabledFLAG = FALSE;
  pco->updateButton->setEnabled(TRUE);
  pco->updateButton->setEnabled(TRUE);
  pco->updateButton->enabledFLAG = TRUE;
  pco->terminateButton->setEnabled(TRUE);
  pco->terminateButton->setFlat(TRUE);
  pco->terminateButton->setEnabled(TRUE);
}

PanelContainer *pc = topPC->findBestFitPanelContainer(topPC);

  loadStatsPanel();
  
        ret_val = 1;
}
        break;
      case  PAUSE_T:
        nprintf( DEBUG_MESSAGES ) ("Pause\n");
sprintf(command, "expPause %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        statusLabelText->setText( tr("Process suspended...") );
        ret_val = 1;
        break;
      case  CONT_T:
        nprintf( DEBUG_MESSAGES ) ("Continue\n");
sprintf(command, "expCont %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
          statusLabelText->setText( tr("Process continued...") );
          sleep(1);
          statusLabelText->setText( tr("Process running...") );
        ret_val = 1;
        break;
      case  UPDATE_T:
        nprintf( DEBUG_MESSAGES ) ("Update\n");
sprintf(command, "expView %d\n", expID); // Get the new data..
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        ret_val = 1;
        break;
      case  INTERRUPT_T:
        nprintf( DEBUG_MESSAGES ) ("Interrupt\n");
// cli->setInterrupt(true);
CLIInterface::interrupt = true;
        ret_val = 1;
        break;
      case  TERMINATE_T:
        statusLabelText->setText( tr("Process terminated...") );
sprintf(command, "expStop %d\n", expID);
if( !cli->runSynchronousCLI(command) )
{
  fprintf(stderr, "Error (%s).\n", command);
}
        ret_val = 1;
 //       nprintf( DEBUG_MESSAGES ) ("Terminate\n");
        break;
      default:
        break;
    }
 } else if( lao )
 {
   nprintf( DEBUG_MESSAGES ) ("we've got a LoadAttachObject message\n");

   if( lao->loadNowHint == TRUE || runnableFLAG == FALSE )
   {
     mw->executableName = lao->executableName;
     mw->pidStr = lao->pidStr;
     updateInitialStatus();
 
     ret_val = 1;
    }
 }

  return ret_val;  // 0 means, did not want this message and did not act on anything.
}

void
pcSamplePanel::updateInitialStatus()
{
  if( !mw->executableName.isEmpty() )
  {

    // Begin demo position at dummy file... For the real stuff we'll need to 
    // look up the main()... and position at it...
    if( mw && !mw->executableName.isEmpty() && mw->executableName.endsWith("fred_calls_ted") )
    {
      loadMain();
    } else
    {
      QString msg = QString(tr("File entered is not an executable file.  No main() entry found.\n") );
      QMessageBox::information( (QWidget *)this, "Process or executable needed...",
                                   msg, QMessageBox::Ok );
      runnableFLAG = FALSE;
      pco->runButton->setEnabled(FALSE);
      pco->runButton->enabledFLAG = FALSE;

// DEMO PULL FIX  HARDCODE TO TRUE FOR NOW!
runnableFLAG = TRUE;
pco->runButton->setEnabled(TRUE);
pco->runButton->enabledFLAG = TRUE;
pco->continueButton->setEnabled(TRUE);
pco->continueButton->enabledFLAG = TRUE;
pco->updateButton->setEnabled(TRUE);
pco->updateButton->enabledFLAG = TRUE;
// PULL - FOR TESTING ONLY
// DEMO PULL FIX  HARDCODE TO TRUE FOR NOW!

      return;
    }
// End demo.

  } else if( !mw->pidStr.isEmpty() )
  {
    loadMain();
    if( runnableFLAG == TRUE )
    {
      if( detachFromProgramSelected() == FALSE )
      {
        return;
      }
    }
    statusLabelText->setText( tr(QString("Attached to:  "))+mw->pidStr+tr(QString("  Click on the Run button to begin collecting data.")) );
  } else
  {
    statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" with the local menu.") );

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    return;
  }
}

/*
*  Sets the strings of the subwidgets using the current
 *  language.
 */
void
pcSamplePanel::languageChange()
{
  statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" with the local menu.") );
}

#include "SaveAsObject.hxx"
void
pcSamplePanel::saveAsSelected()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("From this pc on down, send out a saveAs message and put it to a file.\n");

  QFileDialog *sfd = NULL;
  QString dirName = QString::null;
  if( sfd == NULL )
  {
    sfd = new QFileDialog(this, "file dialog", TRUE );
    sfd->setCaption( QFileDialog::tr("Enter filename:") );
    sfd->setMode( QFileDialog::AnyFile );
    sfd->setSelection(QString("pcSamplePanel.html"));
    QString types(
                  "Any Files (*);;"
                  "Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "(*.c *.cpp *.cxx *.C *.c++ *.f* *.F*);;"
                  );
    sfd->setFilters( types );
    sfd->setDir(dirName);
  }

  QString fileName = QString::null;
  if( sfd->exec() == QDialog::Accepted )
  {
    fileName = sfd->selectedFile();

    if( !fileName.isEmpty() )
    { 
      SaveAsObject *sao = new SaveAsObject(fileName);

      *sao->ts << "<html>";
      *sao->ts << "<head>";
      *sao->ts << "<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"> ";
      *sao->ts << "<title>pcSampleReport</title>";
      *sao->ts << "<h2>pcSampleReport</h2>";
      *sao->ts << "</head>";

sao->f->flush();

  
      broadcast((char *)sao, ALL_DECENDANTS_T, topPC);

      delete( sao );
    }
  }
}

void
pcSamplePanel::loadSourcePanel()
{
  printf("From this pc on down, send out a saveAs message and put it to a file.\n");
  SourcePanel *sp = (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", topPC, (char *)expID);
}

void
pcSamplePanel::loadStatsPanel()
{
  nprintf( DEBUG_PANELS ) ("load the stats panel.\n");
printf("load the stats panel.\n");

  PanelContainer *pc = topPC->findBestFitPanelContainer(topPC);

  Panel *p = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Stats Panel", pc, (void *)expID);

  if( p )
  {
    nprintf( DEBUG_PANELS ) ("call p(%s)'s listener routine.\n", p->getName() );
ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
if( eo && eo->FW() )
{
  Experiment *fw_experiment = eo->FW();
  UpdateObject *msg = new UpdateObject((void *)fw_experiment, expID, "example", 1);
    p->listener( (void *)msg );
}
  }
}

void
pcSamplePanel::__demoWakeUpToLoadExperiment()
{
  statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the Run button to begin the experiment.")) );
loadTimer->stop();
pd->cancel();
pd->hide();


  runnableFLAG = TRUE;
  pco->runButton->setEnabled(TRUE);
  pco->runButton->enabledFLAG = TRUE;
}

void
pcSamplePanel::loadMain()
{
printf("loadMain() entered\n");

// Begin Demo (direct connect to framework) only...
  if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
  {
    timer = new QTimer(this, "__demoOnly_OneTimeLoadExecutableTimer");
    connect( timer, SIGNAL(timeout()), this, SLOT(__demoWakeUpToLoadExperiment() ));
    timer->start(1000, TRUE);
  }
  statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

  runnableFLAG = FALSE;
  pco->runButton->setEnabled(FALSE);
  pco->runButton->enabledFLAG = FALSE;

// End Demo (direct connect to framework) only...

  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
  char buffer[200];
  strcpy(buffer, plugin_directory);
  strcat(buffer, "/../../../usability/phaseI/fred_calls_ted.c");
  SourceObject *spo = new SourceObject("main", buffer, 22, TRUE, NULL);
   if( broadcast((char *)spo, NEAREST_T) == 0 )
  { // No source view up...
    char *panel_type = "Source Panel";
    Panel *p = getPanelContainer()->dl_create_and_add_panel(panel_type, topPC, (char *)expID);
    if( p != NULL )
    {
      if( !p->listener((void *)spo) )
      {
        fprintf(stderr, "Unable to position at main in %s\n", buffer);
      } else
      {
        nprintf( DEBUG_CONST_DESTRUCT ) ("Positioned at main in %s ????? \n", buffer);
      }
    }
  }
}

void
pcSamplePanel::progressUpdate()
{
// printf("progressUpdate() entered..\n");
  pd->setProgress( steps );
  steps++;
  if( steps > 100 )
  {
// printf("progressUpdate() finished..\n");
    loadTimer->stop();
  }
//printf("progressUpdate() sleep..\n");
//  sleep(1);
}
