/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/


#include "SS_Input_Manager.hxx"

#include "Python.h"

using namespace std;

#include "SS_Parse_Result.hxx"
#include "SS_Parse_Target.hxx"

using namespace OpenSpeedShop::cli;

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
std::list<ExperimentObject *> ExperimentObject_list;

// Terminate all experiments and free associated files.
// Called from the drivers to clean up after an "Exit" command or fatal error.
void Experiment_Termination () {
  ExperimentObject *exp = NULL;
  std::list<ExperimentObject *>::iterator expi;
  for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); ) {
    ExperimentObject *exp = (*expi);
    expi++;
    delete exp;
  }
}

// Experiment Utilities.

ExperimentObject *Find_Experiment_Object (EXPID ExperimentID)
{
// Search for existing entry.
  if (ExperimentID > 0) {
    std::list<ExperimentObject *>::iterator exp;
    for (exp = ExperimentObject_list.begin(); exp != ExperimentObject_list.end(); exp++) {
      // if (ExperimentID == ((*exp)->Exp_ID)) {
      if (ExperimentID == (*exp)->ExperimentObject_ID()) {
        return *exp;
      }
    }
  }

  return NULL;
}

static ExperimentObject *Find_Specified_Experiment (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->IsExpId()) ? cmd->P_Result()->GetExpId() : 0;
  ExperimentObject *exp = NULL;

 // If not experiment has been specified, pick up the
 // "focused" experiment associated witht he input window.
  if (ExperimentID == 0) {
    ExperimentID = Experiment_Focus ( WindowID );
    if (ExperimentID == 0) {
      cmd->Result_String ("There is no focused experiment");
      cmd->set_Status(CMD_ERROR);
      return NULL;
    }
  }
  exp = Find_Experiment_Object (ExperimentID);
  if (exp == NULL) {
    cmd->Result_String ("The requested experiment ID does not exist");
    cmd->set_Status(CMD_ERROR);
    return NULL;
  }

  return exp;
}

static void Attach_Command (CommandObject *cmd, ExperimentObject *exp, Thread t, Collector c) {
  c.attachThread(t);
}

/* TEST - process old "command" structure to pick out information that will soon be in the new "Target" structure. */
bool Command_Has_Target_Info () {
  if (command.host_table.cur_node ||
      command.file_table.cur_node ||
      command.pid_table.cur_node ||
      command.thread_table.cur_node ||
      command.rank_table.cur_node) {
    return true;
  }
  return false;
}

// Temporary code to look through the old parse results objects
static ThreadGroup Resolve_Command_Targets (CommandObject *cmd, ExperimentObject *exp) {
  ThreadGroup tgrp;

  if (command.host_table.cur_node == 0) {
      char HostName[MAXHOSTNAMELEN+1];
      if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
        cmd->Result_String ("can not retreive host name");
        cmd->set_Status(CMD_ERROR);
        return ThreadGroup();  // return an empty ThreadGroup
      }
    push_host_name (&HostName[0]);
  }
  host_id_t *host_tab = (host_id_t *)command.host_table.table;
  char **f_tab = NULL;
  char **p_tab = NULL;
  char **t_tab = NULL;
  char **r_tab = NULL;
  int h_len = command.host_table.cur_node;
  int f_len = 0;
  int p_len = 0;
  int t_len = 0;
  int r_len = 0;
  if (command.file_table.cur_node) {
      f_tab = (char **)command.file_table.table;
      f_len = command.file_table.cur_node;
  }
  if (command.pid_table.cur_node) {
      p_tab = (char **)command.pid_table.table;
      p_len = command.pid_table.cur_node;
  }
  if (command.thread_table.cur_node) {
      t_tab = (char **)command.thread_table.table;
      t_len = command.thread_table.cur_node;
  }
  if (command.rank_table.cur_node) {
      r_tab = (char **)command.rank_table.table;
      r_len = command.rank_table.cur_node;
  }

 // Semantic check for illegal combinations.
  if ( h_len && ((f_len + p_len) == 0) ) {
    cmd->Result_String ("THe -h option requires a companion -f or -p option.");
    cmd->set_Status(CMD_ERROR);
    return ThreadGroup();  // return an empty ThreadGroup
  }
  if ( f_len && ((p_len + t_len + r_len) != 0) ) {
    cmd->Result_String ("The -f option can not be used with -p -t or -r options.");
    cmd->set_Status(CMD_ERROR);
    return ThreadGroup();  // return an empty ThreadGroup
  }
  if ( t_len && r_len ) {
    cmd->Result_String ("The -t option can not be used with the -r option.");
    cmd->set_Status(CMD_ERROR);
    return ThreadGroup();  // return an empty ThreadGroup
  }

// Disable calls to the framework until a couple of problems are resolved
  return ThreadGroup();  // return an empty ThreadGroup

 // Okay. Process them.
  for (int h = 0; h < h_len; h++) {
    try {
      if (f_len) {
        for (int i = 0; i < f_len; ++i) {
// fprintf(stdout,"Load Process(%d,%d): %s %s\n",h,i,host_tab[h].u.name,p_tab[i]);
          if (f_len) {
              Thread t = exp->FW()->createProcess(f_tab[i], host_tab[h].u.name);
              tgrp.push_back(t);
          }
        }
      } else if (p_len) {
        for (int i = 0; i < p_len; ++i) {
// fprintf(stdout,"Link Process(%d,%d): %s %s\n",h,i,host_tab[h].u.name,p_tab[i]);
          if (t_len) {
            for (int j = 0; j < p_len; j++) {
              Thread t = exp->FW()->attachPosixThread((pid_t)p_tab[i], (pthread_t)t_tab[j],host_tab[h].u.name);
              tgrp.push_back(t);
            }
          } else if (r_len) {
#ifdef HAVE_OPENMP
            for (int j = 0; j < r_len; j++) {
              Thread t = exp->FW()->attachOpenMPThread((pid_t)p_tab[i], (int)r_tab[r],host_tab[h].u.name);
              tgrp.push_back(t);
            }
#endif
          } else {
            ThreadGroup ngrp = exp->FW()->attachProcess((pid_t)p_tab[i], host_tab[h].u.name);
            for(ThreadGroup::const_iterator tgi = ngrp.begin(); tgi != ngrp.end(); ++tgi) {
              Thread t = *tgi;
              tgrp.push_back(t);
            }
          }
        }
      }
      continue;
    }
    catch(const std::exception& error) {
       cmd->Result_String ( ((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                             "Unknown runtime error." : error.what() );
       cmd->set_Status(CMD_ERROR);
       return ThreadGroup();  // return an empty ThreadGroup
    }
  }

  return tgrp;
}

/* Not needed until new ParseResult objects are produced
static Thread Resolve_Specified_Target (CommandObject *cmd, ExperimentObject *exp, Target *t) { 
  fprintf(stdout,"Enter Resolve_Specified_Target: ");dump_command();
  Thread T  = exp->FW()->createProcess("");
  return T;
}

static void Process_RPTs (CommandObject *cmd, ExperimentObject *exp, Collector c,
                          void (*cmdfunc) (CommandObject *cmd, ExperimentObject *exp,
                                           Thread t, Collector c) ) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<Target *> *p_tlist = p_result->getTargList();

  fprintf(stdout,"Enter Process_RPTs: ");dump_command();
  if (p_tlist->begin() != p_tlist->end()) {
   // Use the threads described by the user.
    vector<Target *>::iterator ti;
    for (ti = p_tlist->begin(); ti != p_tlist->end(); ti++) {
      // Thread t = exp->FW()->createProcess("");
      Thread t = Resolve_Specified_Target (cmd, exp, *ti);
      (*cmdfunc) (cmd, exp, t, c);
    }
  } else {
   // Use the threads that are already part of the experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      (*cmdfunc) (cmd, exp, *ti, c);
    }
  }

}
*/

static void Process_expTypes (CommandObject *cmd, ExperimentObject *exp,
                              void (*cmdfunc) (CommandObject *cmd, ExperimentObject *exp,
                                           Thread t, Collector c) ) {
  if (exp->FW() == NULL) {
    cmd->Result_String ("The experiment has been disabled and can not be changed");
    cmd->set_Status(CMD_ERROR);
    return;
  }

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getExpList();
  CollectorGroup cgrp;

  if (p_slist->begin() != p_slist->end()) {
   // The command contains a list of collectors to use.
   // Be sure they are all linked to the experiment.
    vector<string>::iterator si;
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      try {
        Collector c = exp->FW()->createCollector(*si);
        cgrp.push_back (c);
      }
      catch(const std::exception& error) {
         cmd->Result_String ( ((error.what() == NULL) || (strlen(error.what()) == 0)) ?
                               "Unknown runtime error." : error.what() );
         cmd->set_Status(CMD_ERROR);
         return;
      }
    }
  } else {
   // Use all the collectors that are already part of the experiment.
    cgrp = exp->FW()->getCollectors();
  }


  ThreadGroup tgrp;
  if (Command_Has_Target_Info ()) {
   // The command contains a list of threads to use.
   // Be sure they are all linked to the experiment.
    tgrp = Resolve_Command_Targets (cmd, exp);
/*
  vector<Target *> *p_tlist = p_result->getTargList();
  if (p_tlist->begin() != p_tlist->end()) {
    vector<Target *>::iterator ti;
    for (ti = p_tlist->begin(); ti != p_tlist->end(); ti++) {
      // Thread t = exp->FW()->createProcess("");
      Thread t = Resolve_Specified_Target (cmd, exp, *ti);
      tgrp.push_back(t);
    }
*/
  } else {
   // Use the threads that are already part of the experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
  }

  if ((tgrp.begin() != tgrp.end()) &&
      (cgrp.begin() != cgrp.end())) {
   // Linke a set of threads to a set of collectors.
    CollectorGroup::iterator ci;
    for (ci=cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      ThreadGroup::iterator ti;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
        (*cmdfunc) (cmd, exp, *ti, c);
      }
    }
  }

}

// Experiment Building Block Commands

bool SS_expAttach (CommandObject *cmd) {
  ApplicationGroupObject *App = NULL;
  Collector *Inst = NULL;

  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
    return false;
  }
  Process_expTypes (cmd, exp, &Attach_Command );
  // if (App != NULL) {
  //   exp->ExperimentObject_Add_Application (App);
  // }
  // if (Inst != NULL) {
  //   exp->ExperimentObject_Add_Instrumention (Inst);
  // }

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expClose (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

 // Terminate the experiment and purge the data structure
  if (exp == NULL) {
    return false;
  }

 // Remove all trace of the experiment from the Command Windows.
  Experiment_Purge_Focus  (exp->ExperimentObject_ID()); // remove any Focus on this experiment
  delete exp;

 // No result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expCreate (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // There is no specified experiment.  Allocate a new Experiment.
  ExperimentObject *exp = new ExperimentObject ();
 // When we allocate a new experiment, set the focus to point to it.
  (void)Experiment_Focus (WindowID, exp->ExperimentObject_ID());

 // Set the parsed command structure to fake a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  cmd->P_Result()->SetExpId(exp->ExperimentObject_ID());

 // Let SS_expAttach do the rest of the work for this command.
  if (!SS_expAttach(cmd)) {
    return false;
  }

//  Process_RPTs (cmd, exp, &Attach_Command );
//  if (App != NULL) {
//    exp->ExperimentObject_Add_Application (App);
//  }
//  if (Inst != NULL) {
//    exp->ExperimentObject_Add_Instrumention (Inst);
//  }

 // Return the EXPID for this command.
  cmd->Result_Int (exp->ExperimentObject_ID());
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expDetach (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expDisable (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
    return false;
  }

 // Disconnect the FrameWOrk from the experiment
  exp->Suspend();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expEnable (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
    return false;
  }

 // reconnect the FrameWork to the experiment
  exp->ReStart();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expFocus  (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // The experiment specifier is optional and does not deafult to the focused experiment
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->IsExpId()) ? cmd->P_Result()->GetExpId() : 0;

  if (ExperimentID == 0) {
   // Get the Focused experiment - if it doesn't exist, return the default "0".
    ExperimentID = Experiment_Focus ( WindowID );
  } else {
   // Be sure the requested experiment exists.
    if (Find_Specified_Experiment (cmd) == NULL) {
      return false;
    }
   // Set the Focus to the given experiment ID.
    ExperimentID = Experiment_Focus ( WindowID, ExperimentID);
  }

 // Return the EXPID for this command.
  cmd->Result_Int (ExperimentID);
  cmd->set_Status(CMD_COMPLETE);
  return true;
} 

bool SS_expGo (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
    return false;
  }

  ThreadGroup trg = exp->FW()->getThreads();

  if (trg.empty()) {
    cmd->Result_String ("There are no applications specified for the experiment");
    cmd->set_Status(CMD_ERROR);
    return false;
  }
  trg.changeState (Thread::Running);

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expPause (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
    return false;
  }

  ThreadGroup trg = exp->FW()->getThreads();

  if (!trg.empty()) {
    trg.changeState (Thread::Suspended);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expRestore (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  std::string data_base_name = std::string("ssdb.openss");;
  ExperimentObject *exp = new ExperimentObject (data_base_name);
  EXPID ExperimentID = 0;

  if (exp != NULL) {
   // When we allocate a new experiment, set the focus to point to it.
    ExperimentID = exp->ExperimentObject_ID();
    (void)Experiment_Focus (WindowID, ExperimentID);
  }

 // Return the EXPID for this command.
  if (ExperimentID > 0) {
    cmd->Result_Int (exp->ExperimentObject_ID());
    cmd->set_Status(CMD_COMPLETE);
    return true;
  } else {
    cmd->Result_String ("file name is not legal");
    cmd->set_Status(CMD_ERROR);
    return false;
  }
}

bool SS_expSave (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expSetParam (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expView (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

// Information Commands

bool SS_ListBreaks (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListExp (CommandObject *cmd) {

 // The experiment specifier is optional and doe snot default to the focused experiment.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->IsExpId()) ? cmd->P_Result()->GetExpId() : 0;

  if (ExperimentID <= 0) {
   // List all the allocated experiments
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
     // Return the EXPID for every known experiment
      cmd->Result_Int ((*expi)->ExperimentObject_ID());
    }
  } else {
   // Provide detailed information about a single experiment
    cmd->Result_Int (ExperimentID);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListHosts (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListObj (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListPids (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListMetrics (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListParams (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListReports (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListSrc (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListTypes (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  // ExperimentObject *exp = Find_Specified_Experiment (cmd);

 // The experiment specifier is optional and does not deafult to the focused experiment
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->IsExpId()) ? cmd->P_Result()->GetExpId() : 0;

  if (ExperimentID > 0) {
   // Get the list of collectors used in the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
    CollectorGroup cgrp = exp->FW()->getCollectors();
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Metadata m = c.getMetadata();
      cmd->Result_String ( m.getUniqueId() );
    }
  } else {
   // List all the avaialble experiment types.
    std::set<Metadata> collectortypes = Collector::getAvailable();
    for (std::set<Metadata>::const_iterator mi = collectortypes.begin(); mi != collectortypes.end(); mi++) {
      cmd->Result_String ( mi->getUniqueId() );
    }

  }
  
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

// Session Commands

bool SS_ClearBreaks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Exit (CommandObject *cmd) {
 // Since Python is in control, we need to tell it to quit.
  PyRun_SimpleString( "myparse.Do_quit ()\n");

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Help (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_History (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool R = true;;

 // Default action with no arguments: Dump the history file.
  R = Command_Trace (cmd, CMDW_TRACE_ORIGINAL_COMMANDS, WindowID, std::string(""));

  cmd->set_Status( R ? CMD_COMPLETE : CMD_ERROR);
  return R;
}

bool SS_Log (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool R = false;

  char *tofile = NULL;

  if (tofile == NULL) {
    R = Command_Log_OFF (WindowID);
  } else {
    R = Command_Log_ON(WindowID, tofile);
  }

 // This command does not reutrn a result.
  cmd->set_Status(R ? CMD_COMPLETE : CMD_ERROR);
  return R;
}

extern "C" void loadTheGUI(ArgStruct *);
bool SS_OpenGui (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Load the GUI
 // How do we check to see if is already loaded?
    int argc = 0;
    char **argv = NULL;
    ArgStruct *argStruct = new ArgStruct(argc, argv);
    if (gui_window == 0) {
     // The GUI was not opened before so we need to define an input control window for it.
      char HostName[MAXHOSTNAMELEN+1];
      if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
        cmd->Result_String ("can not retreive host name");
        cmd->set_Status(CMD_ERROR);
        return false;
      }
      pid_t my_pid = getpid();
      gui_window = GUI_Window ("GUI",&HostName[0],my_pid,0,true);
    }
   // Add the input window to the argument list
    argStruct->addArg("-wid");
    char buffer[10];
    sprintf(buffer, "%d", gui_window);
    argStruct->addArg(buffer);
    loadTheGUI((ArgStruct *)argStruct);

 // The GUi will be spun off into it's own process.
 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Playback (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  char *tofile = NULL;

  if (tofile == NULL) {
   (void)Command_Trace_OFF (WindowID);
  } else {
    (void)Command_Trace_ON(WindowID, tofile);
  }

 // There is no result returned for this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_SetBreak (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

//
// Dummy routines for commands not finished yet.
//

// No value returned to python.
bool SS_no_value (CommandObject *cmd) {

 // There is no result returned for this command.
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// String value returned to python.
bool SS_string_value (CommandObject *cmd) {

  cmd->Result_String ("fake string");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// Integer (64 bit) value returned to python..
bool SS_int_value (CommandObject *cmd) {

  cmd->Result_Int (-1);
  cmd->set_Status(CMD_COMPLETE);

  return true;
}


