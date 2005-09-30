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


#include "CustomExperimentPanel.hxx"   // Change this to your new class header file name
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
#include "ArgumentObject.hxx"
#include "FocusObject.hxx"
#include "ManageProcessesPanel.hxx"

#include "LoadAttachObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"

using namespace OpenSpeedShop::Framework;


/*!  CustomExperimentPanel Class

     Autor: Al Stipek (stipek@sgi.com)
 */

// static bool attachFLAG = TRUE;
static bool attachFLAG = FALSE;

/*! Constructs a new UserPanel object */
/*! This is the most often used constructor call.
    \param pc    The panel container the panel will initially be attached.
    \param n     The initial name of the panel container
 */
CustomExperimentPanel::CustomExperimentPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("CustomExperimentPanel::CustomExperimentPanel() constructor called\n");


if( attachFLAG )
{
  printf("Attach now!!! pid=%d\n", getpid() );

  sleep(10);
}


  ExperimentObject *eo = NULL;
  experiment = NULL;
  executableNameStr = QString::null;
  argsStr = QString::null;
  pidStr = QString::null;
  exitingFLAG = FALSE;
  last_status = ExpStatus_NonExistent;
  aboutToRunFLAG = FALSE;
  postProcessFLAG = FALSE;
  readyToRunFLAG = TRUE;

  mw = getPanelContainer()->getMainWindow();
  executableNameStr = mw->executableName;
  argsStr = mw->argsStr;
  pidStr = mw->pidStr;

/*
if( ao )
{
  ao->print();
}
*/
  expID = -1;
  if( ao && ao->qstring_data )
  {
    // We have an existing experiment, load the executable or pid if we 
    // have one associated.  (TODO)
    QString *expIDString = ao->qstring_data;
    if( expIDString->toInt() == -1 )
    {
      nprintf( DEBUG_PANELS ) ("we're coming in from the constructNewExperimentWizardPanel.\n");
    } else if( expIDString->toInt() > 0 )
    {
      expID = expIDString->toInt();
      nprintf( DEBUG_PANELS ) ("we're coming in with an expID=%d\n", expID);
      // Look to see if any collectors have already been assigned.   If not, we
      // need to attach the  le collector.
      eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() )
      {
        experiment = eo->FW();
      }
//      ThreadGroup tgrp = experiment->getThreads();
//      if( tgrp.size() == 0 )
//      {
//        fprintf(stderr, "There are no known threads for this experiment.\n");
//        return;
//      }
//      ThreadGroup::iterator ti = tgrp.begin();
//      Thread t1 = *ti; 
      CollectorGroup cgrp = experiment->getCollectors();
      if( cgrp.size() == 0 )
      {
        nprintf( DEBUG_PANELS ) ("There are no known collectors for this experiment so add one.\n");
        QString command = QString("expAttach -x %1  ").arg(expID);
if( getPanelContainer()->getMainWindow()->mpiFLAG == TRUE )
{
  command += " -v mpi";
}
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("A: command=(%s)\n", command.ascii() );
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

  QWidget *constructNewExperimentControlPanelContainerWidget =
    new QWidget( getBaseWidgetFrame(), "constructNewExperimentControlPanelContainerWidget" );
  topPC = createPanelContainer( constructNewExperimentControlPanelContainerWidget,
                              "PCSamplingControlPanel_topPC", NULL,
                              pc->getMasterPCList() );
  frameLayout->addWidget( constructNewExperimentControlPanelContainerWidget );

  OpenSpeedshop *mw = getPanelContainer()->getMainWindow();

  nprintf( DEBUG_PANELS ) ("Create a new constructNewExperiment experiment.\n");

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;

  if( expID == -1 )
  {
    // We're coming in cold, or we're coming in from the constructNewExperimentWizardPanel.
    QString command = QString::null;
    command = QString("expCreate  \n");
    bool mark_value_for_delete = true;
    int64_t val = 0;

    steps = 0;
	pd = new GenericProgressDialog(this, "Loading experiment...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);

    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("command=(%s)\n", command.ascii() );
// printf("B: command=(%s)\n", command.ascii() );
    if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete ) )
    {
//      fprintf(stderr, "Error retreiving experiment id. \n");
      QMessageBox::information( this, "No collector found:", QString("Unable to issue command:\n  ")+command, QMessageBox::Ok );
      command = QString("expCreate"); 
// printf("C: command=(%s)\n", command.ascii() );
      if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete ) )
      { // fatal errror.
        QMessageBox::critical( this, QString("Critical error:"), QString("Command line not responding as expected:\n  ")+command, QMessageBox::Ok,  QMessageBox::NoButton );
      }
    }
    expID = val;
    eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
    }

    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    loadTimer->stop();
    pd->hide();

    if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
    {
      runnableFLAG = TRUE;
      pco->runButton->setEnabled(TRUE);
      pco->runButton->enabledFLAG = TRUE;
    }
  } else
  {
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


  constructNewExperimentControlPanelContainerWidget->show();
  topPC->show();
  topLevel = TRUE;
  topPC->topLevel = TRUE;

  // Set up the timer that will monitor the progress of the experiment.
  statusTimer = new QTimer( this, "statusTimer" );
  connect( statusTimer, SIGNAL(timeout()), this, SLOT(statusUpdateTimerSlot()) );

  if( expID > 0 && !executableNameStr.isEmpty() )
  {
// printf("Here A: \n");
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;
// printf("C: call updateInitialStatus() \n");
    updateInitialStatus();
  } else if( expID > 0 )
  {
// printf("Here B: \n");
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if( tgrp.size() == 0 )
    {
      statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\".") );
        PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
      ArgumentObject *ao = new ArgumentObject("ArgumentObject", (Panel *)this);
      topPC->dl_create_and_add_panel("Custom Experiemnt Wizard", bestFitPC, ao);
      delete ao;
    } else
    {

      if( ao && ao->loadedFromSavedFile == TRUE )
      {
        topPC->splitVertical(40);
        postProcessFLAG = TRUE;
// printf("postProcessFLAG == TRUE!\n");

        // If we default a report panel bring it up here...
        //printf("Split:  Now loadStatsPanel()\n");
        Panel *p = loadStatsPanel();
            
        // Now focus on the first entry...
          //printf("Now focus the statsPanel\n");
        FocusObject *msg = new FocusObject(expID, NULL, NULL, TRUE);
        p->listener(msg);

//        updateInitialStatus();
      } else
      {
        statusLabelText->setText( tr(QString("Process Loaded: Click on the \"Run\" button to begin the experiment.")) );
// printf("D: call updateInitialStatus() \n");
        updateInitialStatus();
      }
    }
  } else if( executableNameStr.isEmpty() )
  {
// printf("Here C: \n");
    statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\".") );
    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
//    updateInitialStatus();
  }

  if( ao && ao->lao )
  {
    processLAO(ao->lao);
// printf("A: Attempt to remove the wizard panel from the  le panel.\n");
    QString name = QString("Custom Experiemnt Wizard");
// printf("try to find (%s)\n", name.ascii() );
    Panel *wizardPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    if( wizardPanel )
    {
//printf("Found the wizard... Try to hide it.\n");
      wizardPanel->getPanelContainer()->hidePanel(wizardPanel);
    }
  }


}

//! Destroys the object and frees any allocated resources
/*! The only thing that needs to be cleaned up is the baseWidgetFrame.
 */
CustomExperimentPanel::~CustomExperimentPanel()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("  CustomExperimentPanel::~CustomExperimentPanel() destructor called\n");
  statusTimer->stop();
// delete statusTimer;

//  delete frameLayout;
}


//! Add user panel specific menu items if they have any.
bool
CustomExperimentPanel::menu(QPopupMenu* contextMenu)
{
  nprintf( DEBUG_PANELS ) ("CustomExperimentPanel::menu() requested.\n");

  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "EditName");
  qaction->addTo( contextMenu );
  qaction->setText( "Edit Panel Name..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( editPanelName() ) );
  qaction->setStatusTip( tr("Change the name of this panel...") );

  qaction = new QAction( this,  "expStatus");
  qaction->addTo( contextMenu );
  qaction->setText( "Experiment Status..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( experimentStatus() ) );
  qaction->setStatusTip( tr("Get general information about this experiment...") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "StatsPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Stats Panel..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadStatsPanel() ) );
  qaction->setStatusTip( tr("Bring up the data statistics for the experiment.") );

  qaction = new QAction( this,  "sourcePanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Source Panel..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadSourcePanel() ) );
  qaction->setStatusTip( tr("Bring up the source panel.") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "manageProcessesPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Manage Processes ..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadManageProcessesPanel() ) );
  qaction->setStatusTip( tr("Bring up the process and collector manager.") );


  qaction = new QAction( this,  "manageDataSets");
  qaction->addTo( contextMenu );
  qaction->setText( "Manage Data Sets..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( manageDataSetsSelected() ) );
  qaction->setStatusTip( tr("Combine data sets from multiple runs. (Currently unimplemented)") );

  contextMenu->insertSeparator();

  qaction = new QAction( this,  "CustomExperimentPanelSaveAs");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Data..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( saveAsSelected() ) );
  qaction->setStatusTip( tr("Export data from all the CustomExperimentPanel's windows to an ascii file.") );

  return( TRUE );
}


void
CustomExperimentPanel::manageDataSetsSelected()
{
  nprintf( DEBUG_PANELS ) ("CustomExperimentPanel::manageDataSetsSelected()\n");
  QString str(tr("This feature is currently under construction.\n") );
  QMessageBox::information( this, "Informational", str, "Manage Data Sets not yet implemented." );
}   

//! Save ascii version of this panel.
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.
 */
void 
CustomExperimentPanel::save()
{
  nprintf( DEBUG_PANELS ) ("CustomExperimentPanel::save() requested.\n");
}

//! Save ascii version of this panel (to a file).
/*! If the user panel provides save to ascii functionality, their function
     should provide the saving.  This callback will invoke a popup prompting
     for a file name.
 */
void 
CustomExperimentPanel::saveAs()
{
  nprintf( DEBUG_SAVEAS ) ("CustomExperimentPanel::saveAs() requested.\n");
}

//! This function listens for messages.
int 
CustomExperimentPanel::listener(void *msg)
{
  nprintf( DEBUG_MESSAGES ) ("CustomExperimentPanel::listener() requested.\n");
  int ret_val = 0; // zero means we didn't handle the message.

  ControlObject *co = NULL;
  LoadAttachObject *lao = NULL;
  UpdateObject *uo = NULL;

  MessageObject *mo = (MessageObject *)msg;

  nprintf( DEBUG_MESSAGES ) ("CustomExperimentPanel::listener(%s) requested.\n", mo->msgType.ascii() );

  if( mo->msgType == getName() )
  {
    nprintf(DEBUG_MESSAGES) ("CustomExperimentPanel::listener() interested!\n");
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
  } else if( mo->msgType == "ClosingDownObject" )
  {
    nprintf( DEBUG_MESSAGES ) ("CustomExperimentPanel::listener() ClosingDownObject!\n");
    if( exitingFLAG == FALSE )
    {
      QString command = QString::null;
      command = QString("expClose -x %1").arg(expID);

      if( !QMessageBox::question( NULL,
              tr("Delete (expClose) the experiment?"),
              tr("Selecting Yes will delete the experiment.\n"
                  "Selecting No will only close the window."),
              tr("&Yes"), tr("&No"),
              QString::null, 0, 1 ) )
      {
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        Append_Input_String( wid, (char *)command.ascii());
      }
      exitingFLAG = TRUE;
    }
  } else if( mo->msgType == "UpdateExperimentDataObject" )
  {
    uo = (UpdateObject *)msg;
    nprintf( DEBUG_MESSAGES ) ("we've got an UpdateObject\n");
    updateStatus();
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

    QString command = QString::null;
    bool mark_value_for_delete = true;
    int64_t val = 0;
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;


    switch( (int)co->cot )
    {
      case  ATTACH_PROCESS_T:
        command = QString("expAttach  -x %1\n").arg(expID);
if( getPanelContainer()->getMainWindow()->mpiFLAG == TRUE )
{
  command += " -v mpi";
}
/*
        if( !cli->runSynchronousCLI(command.ascii()) )
        {
          fprintf(stderr, "Error (%s).\n", command.ascii());
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Attach to a process (%s)\n", command.ascii());
        ret_val = 1;
        break;
      case  DETACH_PROCESS_T:
        command = QString("expDetach -x %1\n").arg(expID);
/*
        if( !cli->runSynchronousCLI(command.ascii()) )
        {
          fprintf(stderr, "Error (%s).\n", command.ascii());
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Detach from a process (%s)\n", command.ascii());
        ret_val = 1;
        break;
      case  ATTACH_COLLECTOR_T:
        command = QString("expAttach -x %1\n").arg(expID);
if( getPanelContainer()->getMainWindow()->mpiFLAG == TRUE )
{
  command += " -v mpi";
}
/*
        if( !cli->runSynchronousCLI(command.ascii()) )
        {
          fprintf(stderr, "Error (%s).\n", command.ascii() );
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Attach to a collector (%s)\n", command.ascii() );
        ret_val = 1;
        break;
      case  REMOVE_COLLECTOR_T:
        command = QString("expDetach -x %1\n").arg(expID);
/*
        if( !cli->runSynchronousCLI(command.ascii() ) )
        {
          fprintf(stderr, "Error (%s).\n", command.ascii() );
        }
*/
        nprintf( DEBUG_MESSAGES ) ("Remove a collector (%s)\n", command.ascii() );
        ret_val = 1;
        break;
      case  RUN_T:
// printf("pco->argtext =(%s)\n", pco->argtext.ascii() );
        // Put out some notification that we're about to run this.
        // Otherwise there's a deafning silence between when the user
        // clicks the run button and when the run actually begins.
        // This gives the user immediate feedback of what's taking place.
        aboutToRunFLAG = TRUE;
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
        pco->updateButton->setEnabled(FALSE);
        pco->updateButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;

        command = QString("expGo -x %1\n").arg(expID);
        {
        int status = -1;
        nprintf( DEBUG_MESSAGES ) ("Run\n");
        statusLabelText->setText( tr("Process running...") );

        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii());
  
        ret_val = 1;
        }
        break;
      case  PAUSE_T:
        {
        nprintf( DEBUG_MESSAGES ) ("Pause\n");
        command = QString("expPause -x %1\n").arg(expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii());
        statusLabelText->setText( tr("Process Paused...") );
        }
        ret_val = 1;
        break;
#ifdef CONTINUE_BUTTON
      case  CONT_T:
        {
        nprintf( DEBUG_MESSAGES ) ("Continue\n");
        command = QString("expCont -x %1\n").arg(expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii() );
        statusLabelText->setText( tr("Process continued...") );
        }
        ret_val = 1;
        break;
#endif // CONTINUE_BUTTON
      case  UPDATE_T:
        nprintf( DEBUG_MESSAGES ) ("Update\n");
        {
        UpdateObject *update_object = new UpdateObject(NULL, expID, QString::null, FALSE);
//        broadcast((char *)update_object, ALL_T);
        broadcast((char *)update_object, GROUP_T);
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
// NOTE expClose is too drastic.   I don't want to remove the experiment. 
//      I just want to terminate it.
//        command = QString("expClose -x %1 -kill\n").arg(expID);
        command = QString("expPause -x %1\n").arg(expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii() );
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

    ret_val = processLAO(lao);
// printf("B: Attempt to remove the wizard panel from the  le panel.\n");
    QString name = QString("Custom Experiemnt Wizard");
// printf("try to find (%s)\n", name.ascii() );
    Panel *wizardPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    if( wizardPanel )
    {
// printf("Found the wizard... Try to hide it.\n");
      wizardPanel->getPanelContainer()->hidePanel(wizardPanel);
    }

  }

  return ret_val;  // 0 means, did not want this message and did not act on anything.
}

void
CustomExperimentPanel::updateInitialStatus()
{
// printf("A: loadMain()\n");
  loadMain();
}

/*
*  Sets the strings of the subwidgets using the current
 *  language.
 */
void
CustomExperimentPanel::languageChange()
{
  statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" with the local menu.") );

  QToolTip::add(statusLabelText, tr("Shows the current status of the processes/threads in the experiment.") );
}

#include "SaveAsObject.hxx"
void
CustomExperimentPanel::saveAsSelected()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("From this pc on down, send out a saveAs message and put it to a file.\n");

  QFileDialog *sfd = NULL;
  QString dirName = QString::null;
  if( sfd == NULL )
  {
    sfd = new QFileDialog(this, "file dialog", TRUE );
    sfd->setCaption( QFileDialog::tr("Enter filename:") );
    sfd->setMode( QFileDialog::AnyFile );
    sfd->setSelection(QString("CustomExperimentPanel.txt"));
    QString types(
                  "Any Files (*);;"
                  "Text files (*.txt);;"
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

      sao->f->flush();

  
      broadcast((char *)sao, ALL_DECENDANTS_T, topPC);

      delete( sao );
    }
  }
}

void
CustomExperimentPanel::loadSourcePanel()
{
nprintf( DEBUG_PANELS ) ("CustomExperimentPanel::loadSourcePanel()\n");
//printf("CustomExperimentPanel::loadSourcePanel()!!!!!\n");

  QString name = QString("Source Panel [%1]").arg(expID);
//printf("try to find (%s)\n", name.ascii() );
  Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
  if( !sourcePanel )
  {
    PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
//printf("CustomExperimentPanel::loadSourcePanel() create a new Source Panel!!\n");
    (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", bestFitPC, ao);
    delete ao;
  } else
  {
// printf("Raise the source panel!\n");
  }
}

#include <qinputdialog.h>
void
CustomExperimentPanel::editPanelName()
{
  nprintf( DEBUG_PANELS ) ("editPanelName.\n");

  bool ok;
  QString text = QInputDialog::getText(
            "Open|SpeedShop", "Enter your name:", QLineEdit::Normal,
            QString::null, &ok, this );
  if( ok && !text.isEmpty() )
  {
    // user entered something and pressed OK
    setName(text);
  } else
  {
    // user entered nothing or pressed Cancel
  }
}


void
CustomExperimentPanel::experimentStatus()
{
  nprintf( DEBUG_PANELS ) ("experimentStatus.\n");

  QString command = QString("expStatus -x %1").arg(expID);
  QString info_str = QString::null;
  int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
  InputLineObject *clip = Append_Input_String( wid, (char *)command.ascii());

  std::list<CommandObject *>::iterator coi;
  coi = clip->CmdObj_List().begin();

  CommandObject *cmd = (CommandObject *)(*coi);
  ExperimentObject *exp = Find_Experiment_Object((EXPID)expID);

  char id[20]; sprintf(&id[0],"%lld",(int64_t)exp->ExperimentObject_ID());
//  cmd->Result_String ("Experiment definition");
// cout << "Experiment definition" << "\n";
  info_str +=  "Experiment definition\n";
  std::string TmpDB = exp->Data_Base_Is_Tmp() ? "Temporary" : "Saved";
//  cmd->Result_String ("{ # ExpId is " + std::string(&id[0])
//                         + ", Status is " + exp->ExpStatus_Name()
//                         + ", " + TmpDB + " database is " + exp->Data_Base_Name ());
// cout<< "{ # ExpId is " + std::string(&id[0]) + ", Status is " + exp->ExpStatus_Name() + ", " + TmpDB + " database is " + exp->Data_Base_Name() + "\n" ;
  info_str += "# ExpId is " + std::string(&id[0]) + "\n";
  info_str += "Status is " + exp->ExpStatus_Name() + "\n";
  info_str += TmpDB + " database is " + exp->Data_Base_Name() + "\n" ;
  try {
      if (exp->FW() != NULL) {
        ThreadGroup tgrp = exp->FW()->getThreads();
        ThreadGroup::iterator ti;
        bool atleastone = false;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          std::string host = t.getHost();
          pid_t pid = t.getProcessId();
          if (!atleastone) {
            atleastone = true;
//            cmd->Result_String ("  Currently Specified Components:");
// cout << "  Currently Specified Components:" << "\n";
info_str += "  Currently Specified Components:\n";
          }
          int64_t p = pid;
          char spid[20]; sprintf(&spid[0],"%lld",p);
          std::string S = "    -h " + host + " -p " + std::string(&spid[0]);
          std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
          if (pthread.first) {
            int64_t t = pthread.second;
            char tid[20]; sprintf(&tid[0],"%lld",t);
            S = S + std::string(&tid[0]);
          }
#ifdef HAVE_MPI
          std::pair<bool, int> rank = t.getMPIRank();
          if (rank.first) {
            int64_t r = rank.second;
            char rid[20]; sprintf(&rid[0],"%lld",r);
            S = S + std::string(&rid[0]);
          }
#endif
          CollectorGroup cgrp = t.getCollectors();
          CollectorGroup::iterator ci;
          int collector_count = 0;
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata m = c.getMetadata();
            if (collector_count) {
              S = S + ",";
            } else {
              S = S + " ";
              collector_count = 1;
            }
            S = S + m.getUniqueId();
          }
//          cmd->Result_String ( S );
// cout << S << "\n";
info_str += S + "\n";
        }

        CollectorGroup cgrp = exp->FW()->getCollectors();
        CollectorGroup::iterator ci;
        atleastone = false;
        std::string S ;
        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector c = *ci;
          ThreadGroup tgrp = c.getThreads();
          if (tgrp.empty()) {
            Metadata m = c.getMetadata();
            if (atleastone) {
              S = S + ",";
            } else {
//              cmd->Result_String ("  Previously Used Data Collectors:");
//cout << "  Previously Used Data Collectors:" << "\n";
info_str  += "  Previously Used Data Collectors:\n";
              S = S + "    ";
              atleastone = true;
            }
            S = S + m.getUniqueId();
          }
        }
        if (atleastone) {
//          cmd->Result_String ( S );
// cout << S << "\n";
info_str += S + "\n";
        }

        if (cgrp.begin() != cgrp.end()) {

//          cmd->Result_String ("  Metrics:");
//cout << "  Metrics:" << "\n";
info_str += "  Metrics:\n";
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata cm = c.getMetadata();
            std::set<Metadata> md = c.getMetrics();
            std::set<Metadata>::const_iterator mi;
            for (mi = md.begin(); mi != md.end(); mi++) {
              Metadata m = *mi;
              S = "    " + cm.getUniqueId() + "::" +  m.getUniqueId();
//              cmd->Result_String (S);
// cout << S << "\n";
info_str += S + "\n";
            }
          }

//          cmd->Result_String ("  Parameter Values:");
// cout << "  Parameter Values:" << "\n";
info_str += "  Parameter Values:\n";
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata cm = c.getMetadata();
            std::set<Metadata> md = c.getParameters();
            std::set<Metadata>::const_iterator mi;
            for (mi = md.begin(); mi != md.end(); mi++) {
              CommandResult_Columns *C = new CommandResult_Columns (2);
              Metadata m = *mi;
              S = "    " + cm.getUniqueId() + "::" + m.getUniqueId() + " =";
              C->CommandResult_Columns::Add_Column (new CommandResult_RawString (S));
//              C->CommandResult_Columns::Add_Column (Get_Collector_Metadata (c, m));
//              cmd->Result_Predefined (C);
// cout << S ;
info_str += S;
QString param = QString(m.getUniqueId().c_str());
QString param_val = QString::null; 
double double_param;
std::string string_param;
unsigned int uint_param = 0;
int int_param = 0;
if( m.isType(typeid(int)) )
{
// printf("int\n");
  c.getParameterValue(param.ascii(), int_param);
  param_val = QString("%1").arg(int_param);
} else if( m.isType(typeid(unsigned int)) )
{
// printf("unsigned int\n");
  c.getParameterValue(param.ascii(), uint_param);
  param_val = QString("%1").arg(uint_param);
} else if( m.isType(typeid(double)) )
{
// printf("double\n");
  c.getParameterValue(param.ascii(), double_param);
  param_val = QString("%1").arg(double_param);
} else if( m.isType(typeid(std::string)) )
{
// printf("std::string\n");
  c.getParameterValue(param.ascii(), string_param);
  param_val = QString("%1").arg(string_param.c_str());
} else
{
  param_val = QString("Unknown type.");
}
// cout << param_val.ascii() << "\n";
info_str += param_val.ascii();
info_str += "\n";


            }
          }
        }

      }
// cout << "}" << "\n";
info_str += "\n";
  }
  catch(const Exception& error) {
//    Mark_Cmd_With_Std_Error (cmd, error);
//    cmd->Result_String ( "}");
//    exp->Q_UnLock ();
//    return false;
  }

//  exp->Q_UnLock ();


  QMessageBox::information( this, "Experiment Information", info_str, QMessageBox::Ok );
}

Panel *
CustomExperimentPanel::loadStatsPanel()
{
  nprintf( DEBUG_PANELS ) ("load the stats panel.\n");

  QString name = QString("Stats Panel [%1]").arg(expID);


  Panel *statsPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( statsPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadStatsPanel() found Stats Panel found.. raise it.\n");
    getPanelContainer()->raisePanel(statsPanel);
  } else
  {
    nprintf( DEBUG_PANELS ) ("loadStatsPanel() no Stats Panel found.. create one.\n");
    PanelContainer *pc = topPC->findBestFitPanelContainer(topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    statsPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel((const char *)"Stats Panel", pc, ao);
    delete ao;

    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", statsPanel->getName());
// printf("CustomExperimentPanel:: call (%s)'s listener routine.\n", statsPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, " ", 1);
      statsPanel->listener( (void *)msg );
    }
  }

  return(statsPanel);
}

void
CustomExperimentPanel::loadManageProcessesPanel()
{
//  nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel\n");

  QString name = QString("ManageProcessesPanel [%1]").arg(expID);


  Panel *manageProcessPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );

  if( manageProcessPanel )
  { 
    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() found ManageProcessesPanel found.. raise it.\n");
    getPanelContainer()->raisePanel(manageProcessPanel);
  } else
  {
//    nprintf( DEBUG_PANELS ) ("loadManageProcessesPanel() no ManageProcessesPanel found.. create one.\n");

    PanelContainer *pc = topPC->findBestFitPanelContainer(topPC);
    ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
    manageProcessPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", pc, ao);
    delete ao;
  }

  if( manageProcessPanel )
  {
//    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", manageProcessPanel->getName());
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    if( eo && eo->FW() )
    {
      experiment = eo->FW();
      UpdateObject *msg =
        new UpdateObject((void *)experiment, expID, " ", 1);
      manageProcessPanel->listener( (void *)msg );
    }
  }
}

void
CustomExperimentPanel::wakeUpAndCheckExperimentStatus()
{
  printf("CustomExperimentPanel::wakeUpAndCheckExperimentStatus() entered\n");
}

void
CustomExperimentPanel::loadMain()
{
  nprintf( DEBUG_PANELS ) ("loadMain() entered\n");

  if( experiment != NULL )
  {
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if( tgrp.size() == 0 )
    {
      pco->runButton->setEnabled(FALSE);
      pco->runButton->enabledFLAG = FALSE;
      runnableFLAG = FALSE;
      return;
    }
    Thread thread = *ti;
    Time time = Time::Now();
    const std::string main_string = std::string("main");
    std::pair<bool, Function> function = thread.getFunctionByName(main_string, time);
    if( function.first )
    {
      std::set<Statement> statement_definition = function.second.getDefinitions();

      if(statement_definition.size() > 0 )
      {
        std::set<Statement>::const_iterator i = statement_definition.begin();
        SourceObject *spo = new SourceObject(main_string.c_str(), i->getPath(), i->getLine()-1, expID, TRUE, NULL);
  
        QString name = QString("Source Panel [%1]").arg(expID);
//printf("TRY TO FIND A SOURCE PANEL!!!  (loadMain() \n");
        Panel *sourcePanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
        if( !sourcePanel )
        {
          char *panel_type = "Source Panel";
//printf("CustomExperimentPanel:A: create a source panel.\n");
          //Find the nearest toplevel and start placement from there...
          PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
          ArgumentObject *ao = new ArgumentObject("ArgumentObject", expID);
          sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao);
          delete ao;
        }
        if( sourcePanel != NULL )
        {
          sourcePanel->listener((void *)spo);
        }
      }
    } else
    {
//      QMessageBox::information( this, "Experiment Information", "Unable to locate the main routine of the program to position the Source Panel.\nYour Source Panel may appear blank.\nHit the \"Run\" button to continue/start execution.", QMessageBox::Ok );
    }
    statusLabelText->setText( tr("Experiment is loaded:  Hit the \"Run\" button to continue execution.") );

    updateStatus();
  } else
  {
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    runnableFLAG = FALSE;
  }
}

/*!  This routine checks the frameworks status for the experiment.
     Additionally it starts a time when the status is still in a
     state that can be changed.   When the timer is thrown, this
     routine is called again....
 */
void
CustomExperimentPanel::updateStatus()
{
// printf("updateStatus() entered\n");
  if( expID <= 0 )
  {
    statusLabelText->setText( "No expid" );
    statusTimer->stop();
    return;
  } 

  // If we're only post processing, disable the process control
  // buttons and turn off the timer.
  if( postProcessFLAG == TRUE )
  {
// printf("updateStatus() PostProcessFLAG == TRUE!\n");
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
    runnableFLAG = FALSE;
    pco->pauseButton->setEnabled(FALSE);
    pco->pauseButton->enabledFLAG = FALSE;
#ifdef CONTINUE_BUTTON
    pco->continueButton->setEnabled(FALSE);
    pco->continueButton->setEnabled(FALSE);
    pco->continueButton->enabledFLAG = FALSE;
#endif // CONTINUE_BUTTON
    pco->updateButton->setEnabled(TRUE);
    pco->updateButton->enabledFLAG = TRUE;
    pco->terminateButton->setEnabled(TRUE);
    pco->terminateButton->setFlat(FALSE);
    pco->terminateButton->setEnabled(FALSE);
    statusTimer->stop();
    statusLabelText->setText( tr("Save data restored.") );
    return;
  }
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    int status = eo->Determine_Status();
    nprintf( DEBUG_PANELS ) ("status=%d\n", status);
    switch( status )
    {
      case ExpStatus_NonExistent:
// printf("ExpStatus_NonExistent:\n");
//        statusLabelText->setText( "0: ExpStatus_NonExistent" );
        statusLabelText->setText( tr("No available experiment status available") );
        statusTimer->stop();
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
#ifdef CONTINUE_BUTTON
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
#endif // CONTINUE_BUTTON
        pco->updateButton->setEnabled(FALSE);
        pco->updateButton->setEnabled(FALSE);
        pco->updateButton->enabledFLAG = FALSE;
        pco->terminateButton->setEnabled(FALSE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(FALSE);
        statusTimer->stop();
        break;
      case ExpStatus_Paused:
// printf("ExpStatus_Paused:\n");
        if( (last_status == ExpStatus_NonExistent || 
            readyToRunFLAG == TRUE ) && aboutToRunFLAG == FALSE )
        {
          statusLabelText->setText( tr("Experiment is ready to run:  Hit the \"Run\" button to start execution.") );
        } else
        {
          if( aboutToRunFLAG == TRUE )
          {
            statusLabelText->setText( tr("Experiment is being initialized to run.") );
                
            statusTimer->start(2000);
            break;
          } else
          {
            statusLabelText->setText( tr("Experiment is Paused:  Hit the \"Run\" button to continue execution.") );
          }
        }
        pco->runButton->setEnabled(TRUE);
        pco->runButton->enabledFLAG = TRUE;
        runnableFLAG = TRUE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
#ifdef CONTINUE_BUTTON
        pco->continueButton->setEnabled(TRUE);
        pco->continueButton->setEnabled(TRUE);
        pco->continueButton->enabledFLAG = TRUE;
#endif // CONTINUE_BUTTON
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(TRUE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(TRUE);
        statusTimer->start(2000);
        break;
      case ExpStatus_Running:
// printf("ExpStatus_Running:\n");
        aboutToRunFLAG = FALSE;
        readyToRunFLAG = FALSE;
        if( status == ExpStatus_Running )
        {
//          statusLabelText->setText( "3: ExpStatus_Running" );
          statusLabelText->setText( tr("Experiment is Running.") );
        }
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(TRUE);
        pco->pauseButton->enabledFLAG = TRUE;
#ifdef CONTINUE_BUTTON
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
#endif // CONTINUE_BUTTON
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(TRUE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(TRUE);
        statusTimer->start(2000);
        break;
      case ExpStatus_Terminated:
      case ExpStatus_InError:
        if( status == ExpStatus_Terminated )
        {
// printf("ExpStatus_Terminated:\n");
//          statusLabelText->setText( "ExpStatus_Terminated" );
          statusLabelText->setText( tr("Experiment has Terminated") );
        } else if( status == ExpStatus_InError )
        {
// printf("ExpStatus_InError:\n");
//          statusLabelText->setText( "ExpStatus_InError" );
          statusLabelText->setText( tr("Experiment has encountered an Error.") );
        }
        statusTimer->stop();
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
        runnableFLAG = FALSE;
        pco->pauseButton->setEnabled(FALSE);
        pco->pauseButton->enabledFLAG = FALSE;
#ifdef CONTINUE_BUTTON
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->setEnabled(FALSE);
        pco->continueButton->enabledFLAG = FALSE;
#endif // CONTINUE_BUTTON
        pco->updateButton->setEnabled(TRUE);
        pco->updateButton->enabledFLAG = TRUE;
        pco->terminateButton->setEnabled(FALSE);
        pco->terminateButton->setFlat(TRUE);
        pco->terminateButton->setEnabled(FALSE);
        statusTimer->stop();
       // If we default a report panel bring it up here...
       loadStatsPanel();
        break;
      default:
// printf("unknown:\n");
        statusLabelText->setText( QString("%1: Unknown status").arg(status) );
          statusTimer->stop();
        break;
    }
last_status = status;
    
  } else
  {
    statusLabelText->setText( "Cannot find experiment for expID" );
    return;
  }

}

void
CustomExperimentPanel::statusUpdateTimerSlot()
{
//  statusTimer->stop();
  updateStatus();
}

static bool step_forward = TRUE;
void
CustomExperimentPanel::progressUpdate()
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
CustomExperimentPanel::processLAO(LoadAttachObject *lao)
{
// printf("ProcessLOA entered mpiFLAG=%d\n", getPanelContainer()->getMainWindow()->mpiFLAG );
  if( lao->paramList ) // Really not a list yet, just one param.
  {
    QString sample_rate_str = (QString)*lao->paramList->begin();
// printf("sample_rate_str=(%s)\n", sample_rate_str.ascii() );
    unsigned int sampling_rate = sample_rate_str.toUInt();
    // Set the sample_rate of the collector.
    try
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() )
      {
        experiment = eo->FW();
      }
      ThreadGroup tgrp = experiment->getThreads();
      CollectorGroup cgrp = experiment->getCollectors();
      if( cgrp.size() > 0 )
      {
        CollectorGroup::iterator ci = cgrp.begin();
        nprintf( DEBUG_MESSAGES ) ("sampling_rate=%u\n", sampling_rate);
        QString command = QString("expSetParam -x %1 sampling_rate = %2").arg(expID).arg(sampling_rate);
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("E: command=(%s)\n", command.ascii() );
        if( !cli->runSynchronousCLI((char *)command.ascii() ) )
        {
          return 0;
        }
        if( QString(getName()).contains("HW Counter") )
        {
//          printf("W'ere the HW Counter Panel!!!\n");
            
          ParamList::Iterator it = lao->paramList->begin();
          it++;
          if( it != lao->paramList->end() )
          {
            QString event_value = (QString)*it;
            
            QString command = QString("expSetParam -x %1 event = %2").arg(expID).arg(event_value);
            CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("F: command=(%s)\n", command.ascii() );
            if( !cli->runSynchronousCLI((char *)command.ascii() ) )
            {
              return 0;
            }
          }
        }
      }

    }
    catch(const std::exception& error)
    {
      return 0;
    }
    delete lao->paramList;
  }
  nprintf( DEBUG_MESSAGES ) ("we've got a LoadAttachObject message\n");

  if( lao->loadNowHint == TRUE || runnableFLAG == FALSE )
  {
    mw->executableName = lao->executableName;
    executableNameStr = lao->executableName;
    mw->pidStr = lao->pidStr;
    pidStr = lao->pidStr;
 
    executableNameStr = lao->executableName;
    pidStr = lao->pidStr;
 
    QString command = QString::null;
    if( !executableNameStr.isEmpty() )
    {
      command = QString("expAttach -x %1 -f \"%2 %3\"\n").arg(expID).arg(executableNameStr).arg(argsStr);
if( getPanelContainer()->getMainWindow()->mpiFLAG == TRUE )
{
  command += " -v mpi";
}
// printf("executableNameStr is not empty.\n");
    } else if( !pidStr.isEmpty() )
    { 
      QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
      QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
      QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);
// printf("host_name=(%s)\n", host_name.ascii() );
// printf("pid_name=(%s)\n", pid_name.ascii() );
// printf("prog_name=(%s)\n", prog_name.ascii() );

// printf("pidStr =%s\n", pidStr.ascii() );
// printf("mw->hostStr =%s\n", mw->hostStr.ascii() );

QString optionsStr = QString::null;
if( getPanelContainer()->getMainWindow()->mpiFLAG == TRUE )
{
  optionsStr += QString(" -v mpi");
}
optionsStr += QString(" -h %1").arg(mw->hostStr);
//      command = QString("expAttach -x %1 %2 -p  %3 -h %4 ").arg(expID).arg(mpiStr).arg(pidStr).arg(mw->hostStr);
      command = QString("expAttach -x %1 %2 -p  %3 ").arg(expID).arg(optionsStr).arg(pidStr);
// printf("command=(%s)\n", command.ascii() );
    } else
    {
      return 0;
//    command = QString("expCreate  \n");
    }
    bool mark_value_for_delete = true;
    int64_t val = 0;
 
    steps = 0;
 	pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
    statusLabelText->setText( tr(QString("Loading ...  "))+mw->executableName);
 
    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
  
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("D: command=(%s)\n", command.ascii() );
    if( !cli->runSynchronousCLI((char *)command.ascii() ) )
    {
      QMessageBox::information( this, "No collector found:", QString("Unable to issue command:\n  ")+command, QMessageBox::Ok );
      statusLabelText->setText( tr(QString("Unable to load executable name:  "))+mw->executableName);
      loadTimer->stop();
      pd->hide();
      return 1;
    }
 
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    loadTimer->stop();
    pd->hide();
 
    if( !executableNameStr.isEmpty() || !pidStr.isEmpty() )
    {
      runnableFLAG = TRUE;
      pco->runButton->setEnabled(TRUE);
      pco->runButton->enabledFLAG = TRUE;
    }
  
// printf("B: call updateInitialStatus() \n");
    updateInitialStatus();
  }

// We handled it...
  return 1;
}
