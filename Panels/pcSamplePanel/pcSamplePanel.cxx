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
#include "ManageCollectorsDialog.hxx"

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

  ExperimentObject *eo = NULL;
  experiment = NULL;
  executableNameStr = QString::null;
  pidStr = QString::null;

  mw = getPanelContainer()->getMainWindow();

  expID = -1;
  if( argument )
  {
    // We have an existing experiment, load the executable or pid if we 
    // have one associated.  (TODO)
    QString *expIDString = (QString *)argument;
    if( expIDString->toInt() == -1 )
    {
      nprintf( DEBUG_PANELS ) ("we're coming in from the pcSampleWizardPanel.\n");
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
      nprintf( DEBUG_PANELS ) ("we're coming in with an expID=%d\n", expID);
      // Look to see if any collectors have already been assigned.   If not, we
      // need to attach the pcsample collector.
      eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() )
      {
        experiment = eo->FW();
      }
      ThreadGroup tgrp = experiment->getThreads();
      if( tgrp.size() == 0 )
      {
        fprintf(stderr, "There are no known threads for this experiment.\n");
        return;
      }
      ThreadGroup::iterator ti = tgrp.begin();
      Thread t1 = *ti; 
      CollectorGroup cgrp = experiment->getCollectors();
      if( cgrp.size() == 0 )
      {
        nprintf( DEBUG_PANELS ) ("There are no known collectors for this experiment so add one.\n");
        QString command = QString("expAttach -x %1 pcsamp").arg(expID);
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
        if( !cli->runSynchronousCLI((char *)command.ascii() ) )
        {
          return;
        }
      }

    }
  } else
  {
    nprintf( DEBUG_PANELS ) ("We're coming in cold (i.e. from main menu)\n");
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

  QWidget *pcSampleControlPanelContainerWidget =
    new QWidget( getBaseWidgetFrame(), "pcSampleControlPanelContainerWidget" );
  topPC = createPanelContainer( pcSampleControlPanelContainerWidget,
                              "PCSamplingControlPanel_topPC", NULL,
                              pc->getMasterPCList() );
  frameLayout->addWidget( pcSampleControlPanelContainerWidget );

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();

  nprintf( DEBUG_PANELS ) ("Create a new pcSample experiment.\n");

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  if( expID == -1 )
  {
    // We're coming in cold, or we're coming in from the pcSampleWizardPanel.
    char command[1024];
    if( !executableNameStr.isEmpty() )
    {
      sprintf(command, "expCreate -f %s pcsamp\n", executableNameStr.ascii() );
    } else if( !pidStr.isEmpty() )
    { 
      sprintf(command, "expCreate -x %s pcsamp\n", pidStr.ascii() );
    } else
    {
      sprintf(command, "expCreate pcsamp\n" );
    }
    bool mark_value_for_delete = true;
    int64_t val = 0;

    steps = 0;
    pd = new QProgressDialog("Loading process in progress.",
                             NULL, 1000, this, NULL, TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    nprintf( DEBUG_PANELS ) ("Attempting to do an (%s)\n", command );

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    if( !cli->getIntValueFromCLI(command, &val, mark_value_for_delete ) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
//    return;
    }
    expID = val;
    eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
    }

//  fprintf(stdout, "A: MY VALUE! = (%d)\n", val);
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    loadTimer->stop();
    pd->cancel();
    pd->hide();

    if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
    {
      runnableFLAG = TRUE;
      pco->runButton->setEnabled(TRUE);
      pco->runButton->enabledFLAG = TRUE;
    }
  } else
  {
    nprintf( DEBUG_PANELS ) ("pcSample has been requested to open an existing experiment!!!\n");
    // Look up the framework experiment and squirrel it away.
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
    }
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;
  }

  char name_buffer[100];
  sprintf(name_buffer, "%s [%d]", getName(), expID);
  setName(name_buffer);
  groupID = expID;


  pcSampleControlPanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;

  if( expID > 0 && !executableNameStr.isEmpty() )
  {
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;
  } else if( expID > 0 )
  {
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if( tgrp.size() == 0 )
    {
      statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" with the local menu.") );
    } else
    {
      statusLabelText->setText( tr(QString("Process Loaded: Click on the \"Run\" button to begin the experiment.")) );
      updateInitialStatus();
    }
  } else if( executableNameStr.isEmpty() )
  {
    statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" with the local menu.") );
    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
  }

  // Set up the timer that will monitor the progress of the experiment.
  statusTimer = new QTimer( this, "statusTimer" );
  connect( statusTimer, SIGNAL(timeout()), this, SLOT(statusUpdateTimerSlot()) );
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
   nprintf( DEBUG_PANELS ) ("NOTE: This does not need to be a syncronous call.\n");
    int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
    InputLineObject *clip = Append_Input_String( wid, command);
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
  contextMenu->insertItem(tr("pc S&tats Panel..."), this, SLOT(loadStatsPanel()), CTRL+Key_T );

  return( TRUE );
}

void
pcSamplePanel::loadNewProgramSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::loadNewProgramSelected()\n");
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

    nprintf( DEBUG_PANELS ) ("Attempt to load %s\n", mw->executableName.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

    char command[1024];
    sprintf(command, "expAttach -f %s\n", executableNameStr.ascii() );

    steps = 0;
    pd = new QProgressDialog("Loading process in progress.", NULL, 1000,
                             this, NULL, TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    nprintf( DEBUG_PANELS ) ("Attempting to do an (%s)\n", command );

    if( !cli->runSynchronousCLI(command) )
    {
      fprintf(stderr, "Error retreiving experiment id. \n");
  //    return;
    }

    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
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

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  char command[1024];
  sprintf(command, "expClose -x %d\n", expID );

  int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
  InputLineObject *clip = Append_Input_String( wid, command);

  updateInitialStatus();


  SourceObject *spo = new SourceObject(QString::null, QString::null, 0, TRUE, NULL);

  broadcast((char *)spo, NEAREST_T);

  runnableFLAG = FALSE;
  nprintf( DEBUG_PANELS ) ("WARNING: Disable this window!!!!! Until an experiment (pcsamp) is restarted.\n");
}

void
pcSamplePanel::attachToExecutableSelected()
{
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::attachToExecutableSelected()\n");
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
    nprintf( DEBUG_PANELS ) ("Attempt to load %s\n", mw->pidStr.ascii() );
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
    nprintf( DEBUG_PANELS ) ("Attempting to do an (%s)\n", command );

    int64_t val = 0;  // unused
    bool mark_value_for_delete = true;
    if( !cli->getIntValueFromCLI(command, &val, mark_value_for_delete) )
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
// BEGIN DEBUG
// Just a place holder to see if we set the sampling_rate correctly.
        unsigned int sampling_rate = 3;
        // Set the sample_rate of the collector.
        try
        {
          ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
          if( eo && eo->FW() )
          {
            experiment = eo->FW();
          }
          ThreadGroup tgrp = experiment->getThreads();
          if( tgrp.size() == 0 )
          {
            fprintf(stderr, "There are no known threads for this experiment.\n");
            return;
          }
          ThreadGroup::iterator ti = tgrp.begin();
          Thread t1 = *ti; 
          CollectorGroup cgrp = experiment->getCollectors();
          if( cgrp.size() > 0 )
          {
            CollectorGroup::iterator ci = cgrp.begin();
            Collector pcSampleCollector = *ci;
            
            Metadata cm = pcSampleCollector.getMetadata();
            std::set<Metadata> md =pcSampleCollector.getParameters();
            std::set<Metadata>::const_iterator mi;
            for (mi = md.begin(); mi != md.end(); mi++)
            {
              Metadata m = *mi;
              printf("%s::%s\n", cm.getUniqueId().c_str(), m.getUniqueId().c_str() );
              printf("%s::%s\n", cm.getShortName().c_str(), m.getShortName().c_str() );
              printf("%s::%s\n", cm.getDescription().c_str(), m.getDescription().c_str() );
            }

             pcSampleCollector.getParameterValue("sampling_rate", sampling_rate);
          printf("sampling_rate=%u\n", sampling_rate);
          }
        }
        catch(const std::exception& error)
        {
          return;
        }
// END DEBUG
  ManageCollectorsDialog *dialog = new ManageCollectorsDialog(this, "ManageCollectorsDialog", TRUE, expID);
  if( dialog->exec() == QDialog::Accepted )
  {
    printf("QDialog::Accepted\n");
//    pidStr = dialog->selectedProcesses();
  }

  //printf("pidStr = %s\n", pidStr.ascii() );
  delete dialog;
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
        sprintf(command, "expAttach  -x %d\n", expID);
/*
        if( !cli->runSynchronousCLI(command) )
        {
          fprintf(stderr, "Error (%s).\n", command);
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Attach to a process (%s)\n", command);
        ret_val = 1;
        break;
      case  DETACH_PROCESS_T:
        sprintf(command, "expDetach -x %d\n", expID);
/*
        if( !cli->runSynchronousCLI(command) )
        {
          fprintf(stderr, "Error (%s).\n", command);
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Detach from a process (%s)\n", command);
        ret_val = 1;
        break;
      case  ATTACH_COLLECTOR_T:
        sprintf(command, "expAttach -x %d\n", expID);
/*
        if( !cli->runSynchronousCLI(command) )
        {
          fprintf(stderr, "Error (%s).\n", command);
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Attach to a collector (%s)\n", command);
        ret_val = 1;
        break;
      case  REMOVE_COLLECTOR_T:
        sprintf(command, "expDetach -x %d\n", expID);
/*
        if( !cli->runSynchronousCLI(command) )
        {
          fprintf(stderr, "Error (%s).\n", command);
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Remove a collector (%s)\n", command);
        ret_val = 1;
        break;
      case  RUN_T:
        sprintf(command, "expGo -x %d\n", expID);
        {
        int status = -1;
        nprintf( DEBUG_MESSAGES ) ("Run\n");
        statusLabelText->setText( tr("Process running...") );

        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, command);
  
        ret_val = 1;
        }
        break;
      case  PAUSE_T:
        {
        nprintf( DEBUG_MESSAGES ) ("Pause\n");
        sprintf(command, "expPause -x %d\n", expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, command);
        statusLabelText->setText( tr("Process Paused...") );
        }
        ret_val = 1;
        break;
      case  CONT_T:
        {
        nprintf( DEBUG_MESSAGES ) ("Continue\n");
        sprintf(command, "expCont -x %d\n", expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, command);
        statusLabelText->setText( tr("Process continued...") );
        }
        ret_val = 1;
        break;
      case  UPDATE_T:
        nprintf( DEBUG_MESSAGES ) ("Update\n");
        {
        UpdateObject *update_object = new UpdateObject(NULL, expID, QString::null, FALSE);
        broadcast((char *)update_object, ALL_T);
        }
        ret_val = 1;
        break;
      case  INTERRUPT_T:
        nprintf( DEBUG_MESSAGES ) ("Interrupt\n");
/*
// cli->setInterrupt(true);
CLIInterface::interrupt = true;
*/
        ret_val = 1;
        break;
      case  TERMINATE_T:
        {
        statusLabelText->setText( tr("Process terminated...") );
        sprintf(command, "expStop -x %d\n", expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, command);
        ret_val = 1;
        nprintf( DEBUG_MESSAGES ) ("Terminate\n");
        }
        break;
      default:
        break;
    }
    // We just sent an asynchronous command down to the cli.  
    // Update status, will update the status bar as the status 
    // of the command progresses to completion.
    updateStatus();
 } else if( lao )
 {
   nprintf( DEBUG_MESSAGES ) ("we've got a LoadAttachObject message\n");

   if( lao->loadNowHint == TRUE || runnableFLAG == FALSE )
   {
     mw->executableName = lao->executableName;
     mw->pidStr = lao->pidStr;

      if( lao->paramList ) // Really not a list yet, just one param.
      {
        unsigned int sampling_rate = lao->paramList.toUInt();
        // Set the sample_rate of the collector.
        try
        {
          ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
          if( eo && eo->FW() )
          {
            experiment = eo->FW();
          }
          ThreadGroup tgrp = experiment->getThreads();
          if( tgrp.size() == 0 )
          {
            fprintf(stderr, "There are no known threads for this experiment.\n");
            return 0;
          }
          ThreadGroup::iterator ti = tgrp.begin();
          Thread t1 = *ti; 
          CollectorGroup cgrp = experiment->getCollectors();
          if( cgrp.size() > 0 )
          {
            CollectorGroup::iterator ci = cgrp.begin();
            nprintf( DEBUG_MESSAGES ) ("sampling_rate=%u\n", sampling_rate);
            Collector pcSampleCollector = *ci;
            pcSampleCollector.setParameterValue("sampling_rate", sampling_rate);
          }
        }
        catch(const std::exception& error)
        {
          return 0;
        }
      }

      updateInitialStatus();
 
      ret_val = 1;
    }
  }

  return ret_val;  // 0 means, did not want this message and did not act on anything.
}

void
pcSamplePanel::updateInitialStatus()
{
  loadMain();
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
  nprintf( DEBUG_PANELS ) ("pcSamplePanel::loadSourcePanel()\n");

  QString name = QString("Source Panel [%1]").arg(expID);
  Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if( !sourcePanel )
  {
    PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
    SourcePanel *sp = (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", bestFitPC, (char *)expID);
  } else
  {
printf("Raise the source panel!\n");
  }
}

void
pcSamplePanel::loadStatsPanel()
{
  nprintf( DEBUG_PANELS ) ("load the stats panel.\n");

  QString name = QString("pc Stats Panel [%1]").arg(expID);


  Panel *statsPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( statsPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadStatsPanel() found pc Stats Panel found.. raise it.\n");
    getPanelContainer()->raisePanel(statsPanel);
  } else
  {
    nprintf( DEBUG_PANELS ) ("loadStatsPanel() no pc Stats Panel found.. create one.\n");
    PanelContainer *pc = topPC->findBestFitPanelContainer(topPC);
    statsPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("pc Stats Panel", pc, (void *)expID);
  }

  if( statsPanel )
  {
    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", statsPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, "pcsamp", 1);
      statsPanel->listener( (void *)msg );
    }
  }
}

void
pcSamplePanel::wakeUpAndCheckExperimentStatus()
{
  printf("pcSamplePanel::wakeUpAndCheckExperimentStatus() entered\n");
}

void
pcSamplePanel::loadMain()
{
  nprintf( DEBUG_PANELS ) ("loadMain() entered\n");

  if( experiment != NULL );
  {
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if( tgrp.size() == 0 )
    {
      return;
    }
    Thread thread = *ti;
    Time time = Time::Now();
    const std::string main_string = std::string("main");
    OpenSpeedShop::Framework::Function function = thread.getFunctionByName(main_string, time);

    Optional<Statement> statement_definition = function.getDefinition();
    if(statement_definition.hasValue())
    {
      SourceObject *spo = new SourceObject("main", statement_definition.getValue().getPath(), statement_definition.getValue().getLine()-1, TRUE, NULL);
  
      QString name = QString("Source Panel [%1]").arg(expID);
      Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
      if( !sourcePanel )
      {
        char *panel_type = "Source Panel";
        //Find the nearest toplevel and start placement from there...
        PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
        sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, (void *)groupID);
      }
      if( sourcePanel != NULL )
      {
        sourcePanel->listener((void *)spo);
      }
    }
  }
}

/*!  This routine checks the frameworks status for the experiment.
     Additionally it starts a time when the status is still in a
     state that can be changed.   When the timer is thrown, this
     routine is called again....
 */
void
pcSamplePanel::updateStatus()
{
  if( expID <= 0 )
  {
    statusLabelText->setText( "No expid" );
    statusTimer->stop();
    return;
  } 
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    int status = eo->Determine_Status();
    nprintf( DEBUG_PANELS ) ("status=%d\n", status);
    switch( status )
    {
      case 0:
//        statusLabelText->setText( "0: ExpStatus_NonExistent" );
        statusLabelText->setText( tr("No available experiment status available") );
        if( statusTimer ) statusTimer->stop();
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
        pco->updateButton->setEnabled(FALSE);
        pco->updateButton->setEnabled(FALSE);
        pco->updateButton->enabledFLAG = FALSE;
        pco->terminateButton->setEnabled(FALSE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(FALSE);
        statusTimer->stop();
        break;
      case 1:
//        statusLabelText->setText( "1: ExpStatus_Paused" );
        statusLabelText->setText( tr("Experiment is Paused:  Hit the \"Cont\" button to continue execution.") );
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
        pco->continueButton->setEnabled(TRUE);
        pco->continueButton->setEnabled(TRUE);
        pco->continueButton->enabledFLAG = TRUE;
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(TRUE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(TRUE);
        statusTimer->start(2000);
        break;
      case 3:
        if( status == 3 )
        {
//          statusLabelText->setText( "3: ExpStatus_Running" );
          statusLabelText->setText( tr("Experiment is Running.") );
        }
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(TRUE);
        pco->pauseButton->enabledFLAG = TRUE;
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(TRUE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(TRUE);
        statusTimer->start(2000);
        break;
      case 2:
      case 4:
      case 5:
        if( status == 2 )
        {
//          statusLabelText->setText( "2: ExpStatus_Suspended" );
          statusLabelText->setText( tr("Experiment is Susupended.") );
        } else if( status == 4 )
        {
//          statusLabelText->setText( "4: ExpStatus_Terminated" );
          statusLabelText->setText( tr("Experiment has Terminated") );
        } else if( status == 5 )
        {
//          statusLabelText->setText( "5: ExpStatus_InError" );
          statusLabelText->setText( tr("Experiment has encountered an Error.") );
        }
        if( statusTimer ) statusTimer->stop();
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(FALSE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(FALSE);
        statusTimer->stop();
        break;
      default:
        statusLabelText->setText( QString("%1: Unknown status").arg(status) );
          statusTimer->stop();
        break;
    }
    
  } else
  {
    statusLabelText->setText( "Cannot find experiment for expID" );
    return;
  }
}

void
pcSamplePanel::statusUpdateTimerSlot()
{
//  statusTimer->stop();
  updateStatus();
}
void
pcSamplePanel::progressUpdate()
{
  pd->setProgress( steps );
  steps++;
  if( steps > 100 )
  {
    loadTimer->stop();
  }
}
