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
#include "ExpIDInUseObject.hxx"
#include "SourceObject.hxx"
#include "ArgumentObject.hxx"
#include "FocusObject.hxx"
#include "ManageProcessesPanel.hxx"

#include "LoadAttachObject.hxx"
#include "ArgumentObject.hxx"

#include "CLIInterface.hxx"

using namespace OpenSpeedShop::Framework;


class EPOutputClass : public ss_ostream
{
  public:
    CustomExperimentPanel *ep;
    void setEP(CustomExperimentPanel *_ep) { ep = _ep;line_buffer = QString::null; };
    QString line_buffer;
  private:
    virtual void output_string (std::string s)
    {
       line_buffer += s.c_str();
       if( QString(s).contains("\n") )
       {
         QString *data = new QString(line_buffer);
         ep->outputCLIData(data);
         line_buffer = QString::null;
       }
    }
    virtual void flush_ostream ()
    {
      qApp->flushX();
    }
};

/*!  CustomExperimentPanel Class
 */

CustomExperimentPanel::CustomExperimentPanel(PanelContainer *pc, const char *n, ArgumentObject *ao) : Panel(pc, n)
{
  init( pc, n, ao, (const char *)NULL);
}

CustomExperimentPanel::CustomExperimentPanel(PanelContainer *pc, const char *n, ArgumentObject *ao, const char *cn = NULL) : Panel(pc, n)
{
  init( pc, n, ao, cn);
}

void
CustomExperimentPanel::init( PanelContainer *pc, const char *n, ArgumentObject *ao, const char *cn = NULL)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("CustomExperimentPanel::CustomExperimentPanel() constructor called\n");

  collector_names = QString(" ");
  if( cn )
  {
    collector_names = QString(cn);
  }

  original_cview_command = QString::null;

// This flag only gets set to true when the data is read from a file
// or when the program is terminated.
staticDataFLAG = FALSE;
  if( collector_names.stripWhiteSpace().startsWith("pcsamp") )
  {
    wizardName = "pc Sample Wizard";
  } else if( collector_names.stripWhiteSpace().startsWith("usertime") )
  {
    wizardName = "User Time Wizard";
  } else if( collector_names.stripWhiteSpace().startsWith("hwc") )
  {
    wizardName = "HW Counter Wizard"; 
  } else if( collector_names.stripWhiteSpace().startsWith("mpi") )
  {
    wizardName = "MPI Wizard";
  } else if( collector_names.stripWhiteSpace().startsWith("io") )
  {
    wizardName = "IO Wizard";
  } else if( collector_names.stripWhiteSpace().startsWith("fpe") )
  {
    wizardName = "FPE Tracing Wizard";
  } else
  {
    wizardName = "Custom Experiment Wizard";
  }


#ifdef DEBUG_ATTACH_HOOK
if( attachFLAG )
{
  printf("Attach now!!! pid=%d\n", getpid() );

  sleep(10);
}
#endif // DEBUG_ATTACH_HOOK


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
  abortPanelFLAG = FALSE;

  mw = getPanelContainer()->getMainWindow();
  executableNameStr = mw->executableName;
  argsStr = mw->argsStr;
  pidStr = mw->pidStr;

  expStatsInfoStr = QString::null;

  expID = -1;
  leftSideExpID = -1;
  rightSideExpID = -1;

// printf("CustomExperimentPanel::init()\n");

  if( ao && !ao->qstring_data.isEmpty() )
  {
    // We have an existing experiment, load the executable or pid if we 
    // have one associated.  (TODO)
    QString expIDString = ao->qstring_data;
// printf("CustomExperimentPanel::init(%s)\n", expIDString.ascii());
    if( expIDString.toInt() == -1 )
    {
      nprintf( DEBUG_PANELS ) ("we're coming in from the constructNewExperimentWizardPanel.\n");
    } else if( expIDString.toInt() > 0 )
    {
      expID = expIDString.toInt();
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
        QString command = QString("expAttach -x %1 %2").arg(expID).arg(collector_names);
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
    // We're coming in cold, or we're coming in from the wizard panel.
    QString command = QString::null;
    command = QString("expCreate %1\n").arg(collector_names);
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
// fprintf(stderr, "Error retreiving experiment id. \n");
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
      if( eo->Determine_Status() == ExpStatus_NonExistent || eo->Determine_Status() == ExpStatus_InError || eo->Determine_Status() == ExpStatus_Terminated )
      {
        statusLabelText->setText( tr(QString("Loaded saved data from file.") ) );
        loadStatsPanel();

        runnableFLAG = FALSE;
        pco->runButton->setEnabled(FALSE);
        pco->runButton->enabledFLAG = FALSE;
staticDataFLAG == TRUE;
      } else
      {
        statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
        runnableFLAG = TRUE;
        pco->runButton->setEnabled(TRUE);
        pco->runButton->enabledFLAG = TRUE;
      }
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
    statusLabelText->setText( tr(QString("Loaded:  "))+mw->executableName+tr(QString("  Click on the \"Run\" button to begin the experiment.")) );
    runnableFLAG = TRUE;
    pco->runButton->setEnabled(TRUE);
    pco->runButton->enabledFLAG = TRUE;
// printf("C: call updateInitialStatus() \n");
    updateInitialStatus();
  } else if( expID > 0 )
  {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( ao && ao->loadedFromSavedFile == TRUE )
      {
staticDataFLAG = TRUE;
#ifdef SPLIT
        topPC->splitVertical(40);
#endif // SPLIT
        postProcessFLAG = TRUE;
staticDataFLAG = TRUE;
// printf("postProcessFLAG == TRUE!\n");

        // If we default a report panel bring it up here...
        //printf("Split:  Now loadStatsPanel()\n");
        Panel *p = loadStatsPanel();
            
#ifdef DOUBLEREFRESH
        // Now focus on the first entry...
          //printf("Now focus the statsPanel\n");
        FocusObject *msg = new FocusObject(expID, NULL, NULL, TRUE);
        p->listener(msg);
#endif // DOUBLEREFRESH
      } else
      {
        statusLabelText->setText( tr(QString("Process Loaded: Click on the \"Run\" button to begin the experiment.")) );
// printf("D: call updateInitialStatus() \n");
        updateInitialStatus();
      }
  } else if( executableNameStr.isEmpty() )
  {
// printf("Here C: \n");
    statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" or \"Use the Wizard to begin your experiment...\"") );
    runnableFLAG = FALSE;
    pco->runButton->setEnabled(FALSE);
    pco->runButton->enabledFLAG = FALSE;
//    updateInitialStatus();
  }

  if( ao && ao->lao )
  {
    processLAO(ao->lao);
// printf("A: Attempt to remove the wizard panel from the  le panel.\n");
    hideWizard();
  }


  if( expID > 0 )
  {
// printf("Put out local wizard?\n");
// Now get the threads.
//    QString command = QString("listPids -x %1").arg(expID);
    QString command = QString("list -v pids -x %1").arg(expID);
// printf("attempt to run (%s)\n", command.ascii() );
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    std::list<int64_t> list_of_pids;
    list_of_pids.clear();
    InputLineObject *clip = NULL;
    if( !cli->getIntListValueFromCLI( (char *)command.ascii(),
           &list_of_pids, clip, TRUE ) )
    {
      printf("Unable to run %s command.\n", command.ascii() );
    }
// printf("ran %s\n", command.ascii() );

    if( clip )
    {
      clip->Set_Results_Used();
    }

    

  if( abortPanelFLAG == FALSE )
  {
// printf("size=(%d)\n",  list_of_pids.size()  );
      if( list_of_pids.size() == 0 )
      {
        statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("\"Load a New Program...\" or \"Attach to Executable...\" or \"Use the Wizard to begin your experiment...\".") );
          PanelContainer *bestFitPC = getPanelContainer()->getMasterPC()->findBestFitPanelContainer(topPC);
        ArgumentObject *ao = new ArgumentObject("ArgumentObject", (Panel *)this);
        topPC->dl_create_and_add_panel(wizardName.ascii(), bestFitPC, ao, (const char *)NULL);
        delete ao;
      } else
      {
        if( ao && ao->loadedFromSavedFile != TRUE )
        {
          loadManageProcessesPanel();
        }
      }
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

  // If we've disabled this panel, don't allow any menus to be called.
  if( abortPanelFLAG == TRUE )
  {
    return TRUE;
  }

  contextMenu->insertSeparator();

  QAction *qaction = new QAction( this,  "EditName");
  qaction->addTo( contextMenu );
  qaction->setText( "Edit Panel Name..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( editPanelName() ) );
  qaction->setStatusTip( tr("Change the name of this panel...") );

  qaction = new QAction( this,  "saveExp");
  qaction->addTo( contextMenu );
  qaction->setText( "Save Experiment Data..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( saveExperiment() ) );
  qaction->setStatusTip( tr("Save the experiment file...") );

  qaction = new QAction( this,  "CustomExperimentPanelSaveAs");
  qaction->addTo( contextMenu );
  qaction->setText( "Export Data..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( saveAsSelected() ) );
  qaction->setStatusTip( tr("Export data from all the related windows to an ascii file.") );

  qaction = new QAction( this,  "expStatus");
  qaction->addTo( contextMenu );
  qaction->setText( "Experiment Status..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( experimentStatus() ) );
  qaction->setStatusTip( tr("Get general information about this experiment...") );

  if( experiment != NULL )
  {
    ThreadGroup tgrp = experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    if( tgrp.size() == 0 )
    {
      qaction = new QAction( this,  "loadProgram");
      qaction->addTo( contextMenu );
      qaction->setText( tr("Load Program...") );
      connect( qaction, SIGNAL( activated() ), this, SLOT( loadProgramSelected() ) );
      qaction->setStatusTip( tr("Opens dialog box to load application from disk.") );
  
      qaction = new QAction( this,  "attachProcess");
      qaction->addTo( contextMenu );
      qaction->setText( tr("Attach Process...") );
      connect( qaction, SIGNAL( activated() ), this, SLOT( attachProcessSelected() ) );
      qaction->setStatusTip( tr("Opens dialog box to attach to running process.") );
    }
  }

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

//  contextMenu->insertSeparator();

  qaction = new QAction( this,  "manageProcessesPanel");
  qaction->addTo( contextMenu );
  qaction->setText( "Manage Processes ..." );
  connect( qaction, SIGNAL( activated() ), this, SLOT( loadManageProcessesPanel() ) );
  qaction->setStatusTip( tr("Bring up the process and collector manager.") );


  return( TRUE );
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

  int64_t leftSideCval = 0;
  int64_t rightSideCval = 0;

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
// printf("we've got a LoadAttachObject\n");
    lao = (LoadAttachObject *)msg;
    if( lao && !lao->leftSideExperiment.isEmpty() )
    {
// printf("%s: %s\n", lao->leftSideExperiment.ascii(), lao->rightSideExperiment.ascii() );
      CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
      int64_t val = 0;
      // Left side
      bool mark_value_for_delete = true;
      QString command = QString("expRestore -f %1").arg(lao->leftSideExperiment); 
      if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete ) )
      {
        printf("Unable to restore for %s\n", lao->leftSideExperiment.ascii() );
        return 0;
      }
      QString collector_name = "stats";
      {
        command = QString("list -v exptypes -x %1").arg(val);
        std::list<std::string> list_of_collectors;
      
        InputLineObject *clip = NULL;
        if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
                   &list_of_collectors, clip, TRUE ) )
        {
          printf("Unable to run %s command.\n", command.ascii() );
        }
        if( list_of_collectors.size() > 0 )
        {
          if( list_of_collectors.size() == 1 ) 
          {
            for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
            {
              collector_name = (QString)*it;
            }
          }
        }
      }
      QString metric = getMostImportantMetric(collector_name);

      command = QString("cViewCreate -x %1 %2 %3").arg(val).arg(collector_name).arg(metric); 
      leftSideExpID = val;
// printf("            CustomExperimentPanel.cxx command=(%s)\n", command.ascii() );
      if( !cli->getIntValueFromCLI(command.ascii(), &leftSideCval, mark_value_for_delete ) )
      {
        printf("Unable to create cview for %d\n", leftSideCval);
        return 0;
      }
      // Right side
      command = QString("expRestore -f %1").arg(lao->rightSideExperiment); 
      if( !cli->getIntValueFromCLI(command.ascii(), &val, mark_value_for_delete ) )
      {
        printf("Unable to restore for %s\n", lao->rightSideExperiment.ascii() );
        return 0;
      }
      collector_name = "stats";
      {
        command = QString("list -v exptypes -x %1").arg(val);
        std::list<std::string> list_of_collectors;
      
        InputLineObject *clip = NULL;
        if( !cli->getStringListValueFromCLI( (char *)command.ascii(),
                   &list_of_collectors, clip, TRUE ) )
        {
          printf("Unable to run %s command.\n", command.ascii() );
        }
        if( list_of_collectors.size() > 0 )
        {
          if( list_of_collectors.size() == 1 ) 
          {
            for( std::list<std::string>::const_iterator it = list_of_collectors.begin();         it != list_of_collectors.end(); it++ )
            {
              collector_name = (QString)*it;
            }
          }
        }
      }
      metric = getMostImportantMetric(collector_name);

      command = QString("cViewCreate -x %1 %2 %3").arg(val).arg(collector_name).arg(metric); 
      rightSideExpID = val;
// printf("            CustomExperimentPanel.cxx: command=(%s)\n", command.ascii() );
      if( !cli->getIntValueFromCLI(command.ascii(), &rightSideCval, mark_value_for_delete ) )
      {
        printf("Unable to create cview for %d\n", rightSideCval);
        return 0;
      }


      // Now load the stats panel with the data...
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
      }

      command = QString("cview -c %1, %2").arg(leftSideCval).arg(rightSideCval);
      original_cview_command = command;
      UpdateObject *msg =
         new UpdateObject((void *)NULL, -1, command.ascii(), 1);
      statsPanel->listener( (void *)msg );

      statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("Experiment %1 is being compared with experiment %2").arg(leftSideExpID).arg(rightSideExpID) );

    }
  } else if( mo->msgType == "ClosingDownObject" )
  {
    nprintf( DEBUG_MESSAGES ) ("CustomExperimentPanel::listener() ClosingDownObject!\n");
    if( exitingFLAG == FALSE && mw->shuttingDown == FALSE )
    {
      QString command = QString::null;
      command = QString("expClose -x %1").arg(expID);

      // First check to see if anyone (compare experiment namely) still
      // has a need for this expID.
      ExpIDInUseObject *expIDInUseObject = new ExpIDInUseObject(expID);
      bool ret_val = broadcast((char *)expIDInUseObject, ALL_T);
      if( ret_val == TRUE )
      {
        return TRUE;
      }

      if( !QMessageBox::question( NULL,
              tr("Delete (expClose) the experiment?"),
              tr("Selecting Yes will delete the experiment.\n"
                  "Selecting No will only close the window."),
              tr("&Yes"), tr("&No"),
              QString::null, 0, 1 ) )
      {
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
        cli->runSynchronousCLI(command.ascii());
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
// printf("NOOP: Attach to a process (%s)\n", command.ascii());
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
  
        cli->runSynchronousCLI(command.ascii());
        ret_val = 1;
        }
        break;
      case  PAUSE_T:
        {
        nprintf( DEBUG_MESSAGES ) ("Pause\n");
        command = QString("expPause -x %1\n").arg(expID);
        int wid = getPanelContainer()->getMainWindow()->widStr.toInt();
        cli->runSynchronousCLI(command.ascii());
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
        cli->runSynchronousCLI(command.ascii());
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
        cli->runSynchronousCLI(command.ascii());
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
// printf("ret_val from processLAO()=%d\n", ret_val );
// printf("B: Attempt to remove the wizard panel from the  le panel.\n");
    loadManageProcessesPanel();
    QString name = wizardName;
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
    (SourcePanel *)topPC->dl_create_and_add_panel("Source Panel", bestFitPC, ao, (const char *)NULL);
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
CustomExperimentPanel::saveExperiment()
{
  mw->fileSaveExperiment(expID);
}


#include "AboutDialog.hxx"
void
CustomExperimentPanel::experimentStatus()
{
  nprintf( DEBUG_PANELS ) ("experimentStatus.\n");


  expStatsInfoStr = QString::null;

  QString command = QString("expStatus -x %1").arg(expID);
  QString info_str = QString::null;
  int wid = getPanelContainer()->getMainWindow()->widStr.toInt();


  EPOutputClass *epoclass = new EPOutputClass();
  epoclass->setEP(this);

  CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
  Redirect_Window_Output(cli->wid, epoclass, epoclass);

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  InputLineObject *clip = cli->run_Append_Input_String( wid, (char *)command.ascii());

  Input_Line_Status status = ILO_UNKNOWN;

  while( !clip->Semantics_Complete() )
  {
    status = cli->checkStatus(clip);
    if( !status || status == ILO_ERROR )
    { // An error occurred.... A message should have been posted.. return;
      QApplication::restoreOverrideCursor();
      if( clip ) 
      {
        clip->Set_Results_Used();
      }
      return;
    }

    qApp->processEvents(1000);

    if( !cli->shouldWeContinue() )
    {
// printf("RETURN FALSE!   COMMAND FAILED!\n");
      QApplication::restoreOverrideCursor();
      if( clip ) 
      {
        clip->Set_Results_Used();
      }
      return;
    }

    sleep(1);
  }
  QApplication::restoreOverrideCursor();

  //Test putting the output to statspanel stream.
  Default_TLI_Line_Output(clip);

  if( clip )
  {
    clip->Set_Results_Used();
  }

  
  AboutDialog *aboutDialog = new AboutDialog(this, "Experiment status", FALSE, 0, expStatsInfoStr);
  aboutDialog->show();

  resetRedirect();


  delete epoclass;
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
// printf("loadStatsPanel: staticDataFLAG=%d\n", staticDataFLAG );
    if( staticDataFLAG == TRUE )
    {
      ao->loadedFromSavedFile = TRUE;
    }
    statsPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel((const char *)"Stats Panel", pc, ao, (const char *)NULL);
    delete ao;

    nprintf( DEBUG_PANELS )("call (%s)'s listener routine.\n", statsPanel->getName());
// printf("CustomExperimentPanel:: call (%s)'s listener routine.\n", statsPanel->getName());
    if( original_cview_command.isEmpty() )
    {
      ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
      if( eo && eo->FW() )
      {
        experiment = eo->FW();
        UpdateObject *msg =
          new UpdateObject((void *)experiment, expID, collector_names, 1);
        statsPanel->listener( (void *)msg );
      }
    } else
    {
      UpdateObject *msg =
         new UpdateObject((void *)NULL, -1, original_cview_command.ascii(), 1);
      statsPanel->listener( (void *)msg );

      statusLabel->setText( tr("Status:") ); statusLabelText->setText( tr("Experiment %1 is being compared with experiment %2").arg(leftSideExpID).arg(rightSideExpID) );
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
    manageProcessPanel = getPanelContainer()->getMasterPC()->dl_create_and_add_panel("ManageProcessesPanel", pc, ao, (const char *)NULL);
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
        new UpdateObject((void *)experiment, expID, collector_names, 1);
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
    const std::string main_string = std::string("main");
    std::pair<bool, Function> function = thread.getFunctionByName(main_string);
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
          sourcePanel = getPanelContainer()->dl_create_and_add_panel(panel_type, bestFitPC, ao, (const char *)NULL);
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
staticDataFLAG = TRUE;
    return;
  }
  ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
  if( eo && eo->FW() )
  {
    int status = eo->Determine_Status();
// printf(" : eo->Determine_Status()=%d\n", eo->Determine_Status() );
    if( eo->Determine_Status() == ExpStatus_NonExistent || eo->Determine_Status() == ExpStatus_InError || eo->Determine_Status() == ExpStatus_Terminated )
    {
      statusLabelText->setText( tr(QString("Loaded saved data from file.") ) );
      runnableFLAG = FALSE;
      pco->runButton->setEnabled(FALSE);
      pco->runButton->enabledFLAG = FALSE;
staticDataFLAG = TRUE;
    }
    nprintf( DEBUG_PANELS ) ("status=%d\n", status);
    switch( status )
    {
      case ExpStatus_NonExistent:
// printf("ExpStatus_NonExistent:\n");
//        statusLabelText->setText( "0: ExpStatus_NonExistent" );
//        statusLabelText->setText( tr("No available experiment status available") );
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
staticDataFLAG = TRUE;
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
//NOTE: ALL the exeriments use the same basic template for the 
//      experiments.   Occassionally you'll see code related 
//      to a specific collector type in the code.   This is 
//      done simply to keep all experiment's source bases consistent.
//      At least until I get time to rearchitect this to 
//      have a base class of experiment and have all the 
//      experiments inherit from that...   Not hard, just timeconsuming.
// printf("ProcessLOA entered (%s) mpiFLAG=%d\n", getName(), getPanelContainer()->getMainWindow()->mpiFLAG );

  if( QString(getName()).startsWith("MPI") || QString(getName()).startsWith("MPT") )
  {
    // Currently we don't set any mpi or fpe parameters.
  } else if( QString(getName()).startsWith("FPE") || QString(getName()).startsWith("IO") )
  {
// printf("WHY AREN'T YOU HERE!\n");
    QString paramStr = QString::null;
    bool checkAll = FALSE;
    for( ParamList::Iterator it = lao->paramList->begin(); it != lao->paramList->end(); ++it)
    {
      QString val = (QString)*it;
      if( paramStr.isEmpty() )
      {
        paramStr = QString("%1").arg(val);
      } else
      {
        paramStr += QString(",%1").arg(val);
      }                   
    }
// printf("paramStr: (%s)\n", paramStr.ascii() );

    QString command = QString::null;
    if( !paramStr.isEmpty() )
    {
      if( QString(getName()).startsWith("FPE") )
      {
        command = QString("expSetParam -x %1 fpe::traced_fpes=%2").arg(expID).arg(paramStr);
// printf("paramStr: fpe =(%s)\n", paramStr.ascii() );
      } else if( QString(getName()).startsWith("IO") )
      {
        command = QString("expSetParam -x %1 io::traced_functions=%2").arg(expID).arg(paramStr);
// printf("paramStr: IO =(%s)\n", paramStr.ascii() );
      }
      CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("%s command=(%s)\n", getName(), command.ascii() );
      if( !cli->runSynchronousCLI((char *)command.ascii() ) )
      {
        return 0;
      }
    }
  } else if( lao->paramList )
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
        // All others have a sampling_rate parameter... hwc, hwt,
        //  , and usertime
        nprintf( DEBUG_MESSAGES ) ("sampling_rate=%u\n", sampling_rate);
        QString command = QString("expSetParam -x %1 sampling_rate = %2").arg(expID).arg(sampling_rate);
        CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
// printf("E: %s command=(%s)\n", getName(), command.ascii() );
        if( !QString(getName()).startsWith("IO") )  // IO* doesn't have a sampling_rate
        {
          if( !cli->runSynchronousCLI((char *)command.ascii() ) )
          {
            return 0;
          }
        }
        if( QString(getName()).contains("HW Counter") )
        {
//        printf("We're the HW Counter Panel!!!\n");
              
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
//    command = QString("expCreate  %1\n").arg(collector_names);
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
      // We're needing to abort this panel.  Since I can't delete myself
      // simply disable all actions.
      abortPanelFLAG = TRUE;
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

void
CustomExperimentPanel::outputCLIData(QString *data)
{
// printf("data=%s\n", data->ascii() );

 expStatsInfoStr += *data;  
}



void
CustomExperimentPanel::attachProcessSelected()
{
  mw->executableName = QString::null;
  mw->pidStr = QString::null;
  mw->attachNewProcess();


  if( !mw->pidStr.isEmpty() )
  {
    QString command;

    // Hack to get host and pid strings for the attach... This will be 
    // replace with something better shortly.

    QString host_name = mw->pidStr.section(' ', 0, 0, QString::SectionSkipEmpty);
    QString pid_name = mw->pidStr.section(' ', 1, 1, QString::SectionSkipEmpty);
    QString prog_name = mw->pidStr.section(' ', 2, 2, QString::SectionSkipEmpty);
    command = QString("expAttach -x %1 -p %2 -h %3\n").arg(expID).arg(mw->pidStr).arg(mw->hostStr); 
// printf("A: command=(%s)\n", command.ascii() );

    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();

    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    if( !cli->runSynchronousCLI(command.ascii()) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  //    return;
    }

    // Send out a message to all those that might care about this change request
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    

    loadTimer->stop();
    pd->hide();

    if( eo->FW() != NULL )
    {
      UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
      broadcast((char *)msg, GROUP_T);
    }

    updateInitialStatus();
    updateStatus();

    hideWizard();
  }
}

void
CustomExperimentPanel::loadProgramSelected()
{
// printf("CustomExperimentPanel::loadProgramSelected()\n");
  mw->executableName = QString::null;
  mw->argsStr = QString::null;
  mw->loadNewProgram();
  QString executableNameStr = mw->executableName;
  if( !mw->executableName.isEmpty() )
  {
// printf("CustomExperimentPanel::loadProgramSelected() executableName=%s\n", mw->executableName.ascii() );
    executableNameStr = mw->executableName;
    QString command =
      QString("expAttach -x %1 -f \"%2 %3\"").arg(expID).arg(executableNameStr).arg(mw->argsStr);

// printf("command=(%s)\n", command.ascii() );
    steps = 0;
    pd = new GenericProgressDialog(this, "Loading process...", TRUE);
    loadTimer = new QTimer( this, "progressTimer" );
    connect( loadTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()) );
    loadTimer->start( 0 );
    pd->show();
        
    CLIInterface *cli = getPanelContainer()->getMainWindow()->cli;
    if( !cli->runSynchronousCLI(command.ascii() ) )
    {
      QMessageBox::information( this, tr("Error issuing command to cli:"), tr("Unable to run %1 command.").arg(command), QMessageBox::Ok );
  //    return;
  
    }
    loadTimer->stop();
    pd->hide();

//    delete(pd);

    // Send out a message to all those that might care about this change request
    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
    
// printf("Send out update?\n");
    if( eo->FW() != NULL )
    {
// printf("Yes!  Send out update?\n");
      UpdateObject *msg = new UpdateObject(eo->FW(), expID,  NULL, 0);
      broadcast((char *)msg, GROUP_T);
    }
  
    updateInitialStatus();
    updateStatus();

    hideWizard();
  }
}

void
CustomExperimentPanel::hideWizard()
{
    QString name = wizardName;
// printf("try to find (%s)\n", name.ascii() );
    Panel *wizardPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), (char *)name.ascii() );
    if( wizardPanel )
    {
//printf("Found the wizard... Try to hide it.\n");
      wizardPanel->getPanelContainer()->hidePanel(wizardPanel);
    }
}

void
CustomExperimentPanel::resetRedirect()
{
// Just make sure any pending output goes "somewhere".
  Panel *cmdPanel = getPanelContainer()->findNamedPanel(getPanelContainer()->getMasterPC(), "&Command Panel");
  if( cmdPanel )
  {
    MessageObject *msg = new MessageObject("Redirect_Window_Output()");
    cmdPanel->listener((void *)msg);
    delete msg;
  } else
  {
    fprintf(stderr, "Unable to redirect output to the cmdpanel.\n");
  }
}

// These defaults are pulled directly from the -cli documentation.
QString
CustomExperimentPanel::getMostImportantMetric(QString collector_name)
{
  QString metric = QString::null;


  if( collector_name == "pcsamp" )
  {
    metric = "-m pcsamp::time";
  } else if( collector_name == "usertime" )
  {
    metric = "-m usertime::exclusive_time";
  } else if( collector_name == "hwc" )
  {
    metric = "-m hwc::overflows";
  } else if( collector_name == "hwctime" )
  {
    metric = "-m hwc::exclusive_overflows";
  } else if( collector_name == "mpi" )
  {
    metric = "-m mpi::exclusive_times";
  } else if( collector_name == "mpit" )
  {
    metric = "-m mpit::start_time, mpit::end_time, mpit::exclusive";
  } else if( collector_name == "io" )
  {
    metric = "-m io::exclusive_times";
  } else if( collector_name == "iot" )
  {
    metric = "-m io::exclusive_times";
  }
  
  return(metric);
}
