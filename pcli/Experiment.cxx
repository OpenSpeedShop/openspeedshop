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

// Private Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
std::list<ExperimentObject *> ExperimentObject_list;
static std::string tmpdb = std::string("./ssdbtmpcmd.openss");

// Terminate all experiments and free associated files.
// Called from the drivers to clean up after an "Exit" command or fatal error.
void Experiment_Termination () {
  (void) remove (tmpdb.c_str());
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

parse_val_t *Get_Simple_File_Name (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = (p_result != NULL) ? p_result->GetTargetList() : NULL;
  if (p_tlist == NULL) {
    return NULL;
  }
  if (p_tlist->begin() == p_tlist->end()) {
    return NULL;
  }
  vector<ParseTarget>::iterator pi = p_tlist->begin();
  vector<ParseRange> *f_list = (*pi).getFileList();
  if (f_list == NULL) {
    return NULL;
  }
  if (f_list->begin() == f_list->end()) {
    return NULL;
  }
  vector<ParseRange>::iterator fi = f_list->begin();
  parse_range_t *f_range = (*fi).getRange();
  if (f_range == NULL) {
    return NULL;
  }
  return &f_range->start_range;
}

bool Look_For_KeyWord (CommandObject *cmd, std::string Key) {
  Assert (cmd->P_Result() != NULL);

 // Look at general modifier types for a specific KeyWord option.
  vector<string> *p_slist = cmd->P_Result()->getModifierList();
  vector<string>::iterator j;

  for (j=p_slist->begin();j != p_slist->end(); j++) {
    std::string S = *j;
    if (!strcmp(S.c_str(),Key.c_str())) {
      return true;
    }
  }

  return false;
}

bool Thread_Already_Exists (Thread **returnThread, ExperimentObject *exp, std::string myhost, pid_t mypid) {
  ThreadGroup current_tgrp = exp->FW()->getThreads();
  ThreadGroup::iterator ti;
  for (ti = current_tgrp.begin(); ti != current_tgrp.end(); ti++) {
    Thread T = *ti;
    std::string host = T.getHost();
    pid_t pid = T.getProcessId();
    if (!strcmp(host.c_str(), myhost.c_str()) &&
        (pid = mypid) &&
#ifdef HAVE_MPI
        !T.getPosixThreadId().hasValue() &&
        !T.getMpiRank().hasValue()) {
#else
        !T.getPosixThreadId().hasValue()) {
#endif
      *returnThread = &T;
      return true;
    }
  }
  return false;
}

Collector Get_Collector (ExperimentObject *exp, std::string myname) {
  CollectorGroup current_cgrp = exp->FW()->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = current_cgrp.begin(); ci != current_cgrp.end(); ci++) {
    Collector C = *ci;
    std::string name = C.getMetadata().getUniqueId();
    if (!strcmp(name.c_str(), myname.c_str())) {
      return C;
    }
  }
  return exp->FW()->createCollector(myname);
}

static void Attach_Command (CommandObject *cmd, ExperimentObject *exp, Thread t, Collector c) {
  try {
    c.attachThread(t);
  }
  catch(const std::exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }
}

static void Detach_Command (CommandObject *cmd, ExperimentObject *exp, Thread t, Collector c) {
  try {
    c.detachThread(t);
  }
  catch(const std::exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }
}

// Look through the parse results objects and figure out the rank specification.
static void Resolve_R_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp,
                              ParseTarget pt, std::string host_name, pid_t mypid) {

  vector<ParseRange> *r_list = pt.getRankList();

 // Okay. Process the rank specification.
  vector<ParseRange>::iterator r_iter;

  for (r_iter=r_list->begin();r_iter != r_list->end(); r_iter++) {
    parse_range_t *r_range = r_iter->getRange();
    parse_val_t *r_val1 = &r_range->start_range;
    parse_val_t *r_val2 = r_val1;
    if (r_range->is_range) {
      r_val2 = &r_range->end_range;
    }

    int64_t myrank;
    for ( myrank = (int64_t)r_val1->num; myrank <= (int64_t)r_val2->num; myrank++) {
      try {
        Thread t = exp->FW()->attachPosixThread(mypid, myrank, host_name);
        tgrp->push_back(t);
      }
      catch(const std::exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        return;
      }
    }
  }
}

// Look through the parse results objects and figure out the thread specification.
static void Resolve_T_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp,
                              ParseTarget pt, std::string host_name, pid_t mypid) {

  vector<ParseRange> *t_list = pt.getThreadList();

 // Okay. Process the thread specification.
  vector<ParseRange>::iterator t_iter;

  for (t_iter=t_list->begin();t_iter != t_list->end(); t_iter++) {
    parse_range_t *t_range = t_iter->getRange();
    parse_val_t *t_val1 = &t_range->start_range;
    parse_val_t *t_val2 = t_val1;
    if (t_range->is_range) {
      t_val2 = &t_range->end_range;
    }

    int64_t mythread;
    for ( mythread = (int64_t)t_val1->num; mythread <= (int64_t)t_val2->num; mythread++) {
#ifdef HAVE_OPENMP
      try {
        Thread t = exp->FW()->attachOpenMPThread(mypid, mythread, host_name);
        tgrp->push_back(t);
      }
      catch(const std::exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        return;
      }
#endif
    }
  }
}

// Look through the parse results objects and figure out the pid specification.
// We do not need to worry about file specifiers.
// We also know that there is not both a thread and a rank specifier.
static void Resolve_P_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp,
                              ParseTarget pt, std::string host_name) {

  vector<ParseRange> *p_list = pt.getPidList();
  vector<ParseRange> *t_list = pt.getThreadList();
  vector<ParseRange> *r_list = pt.getRankList();

  bool has_t = !((t_list == NULL) || t_list->empty());
  bool has_r = !((r_list == NULL) || r_list->empty());

 // Okay. Process the pid specification.
  vector<ParseRange>::iterator p_iter;

  for (p_iter=p_list->begin();p_iter != p_list->end(); p_iter++) {
    parse_range_t *p_range = p_iter->getRange();
    parse_val_t *p_val1 = &p_range->start_range;
    parse_val_t *p_val2 = p_val1;
    if (p_range->is_range) {
      p_val2 = &p_range->end_range;
    }

    pid_t mypid;
    for ( mypid = (pid_t)p_val1->num; mypid <= (pid_t)p_val2->num; mypid++) {
      if (has_t) {
        Resolve_T_Target ( cmd, exp, tgrp, pt, host_name, mypid);
      } else if (has_r) {
        Resolve_R_Target ( cmd, exp, tgrp, pt, host_name, mypid);
      } else {
        Thread *pt;
        if (Thread_Already_Exists (&pt, exp, host_name, mypid)) {
          tgrp->push_back (*pt);
          continue;
        }
        try {
          ThreadGroup ngrp = exp->FW()->attachProcess(mypid, host_name);
          for(ThreadGroup::const_iterator tgi = ngrp.begin(); tgi != ngrp.end(); ++tgi) {
            Thread t = *tgi;
            tgrp->push_back(t);
          }
        }
        catch(const std::exception& error) {
          Mark_Cmd_With_Std_Error (cmd, error);
          return;
        }
      }
    }
  }
}

// Look through the parse results objects and figure out the file specification.
// This routine is called because there is a file specification.
// We also know that there is no thread or rank specification.
static void Resolve_F_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp,
                              ParseTarget pt, std::string host_name) {

  vector<ParseRange> *f_list = pt.getFileList();

 // Okay. Process the file specification.
  vector<ParseRange>::iterator f_iter;

  for (f_iter=f_list->begin();f_iter != f_list->end(); f_iter++) {
    parse_range_t *f_range = f_iter->getRange();
    parse_val_t *f_val1 = &f_range->start_range;
    if (f_range->is_range) {
      parse_val_t *f_val2 = &f_range->end_range;
// TODO:
    } else {
      try {
      Thread t = exp->FW()->createProcess(f_val1->name, host_name);
      tgrp->push_back(t);
      }
      catch(const std::exception& error) {
         Mark_Cmd_With_Std_Error (cmd, error);
         return;
      }
    }
  }
}

// Look through the parse results objects and figure out the host specification.
static void Resolve_H_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp, ParseTarget pt) {

  vector<ParseRange> *c_list = pt.getClusterList();
  vector<ParseRange> *h_list = pt.getHostList();
  vector<ParseRange> *f_list = pt.getFileList();
  vector<ParseRange> *p_list = pt.getPidList();
  vector<ParseRange> *t_list = pt.getThreadList();
  vector<ParseRange> *r_list = pt.getRankList();

  bool has_h = !((h_list == NULL) || h_list->empty());
  bool has_f = !((f_list == NULL) || f_list->empty());
  bool has_p = !((p_list == NULL) || p_list->empty());
  bool has_t = !((t_list == NULL) || t_list->empty());
  bool has_r = !((r_list == NULL) || r_list->empty());

  if (!has_h) {
    char HostName[MAXHOSTNAMELEN+1];
    if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
      cmd->Result_String ("can not retreive host name");
      cmd->set_Status(CMD_ERROR);
      return;
    }
    //pt.pushHostPoint (HostName[0]);
    //h_list = pt.getHostList();
    h_list->push_back (ParseRange(&HostName[0]));
    has_h = true;
  }

 // Semantic check for illegal combinations.
  if ( has_f && (has_p || has_t || has_r) ) {
    cmd->Result_String ("The -f option can not be used with -p -t or -r options.");
    cmd->set_Status(CMD_ERROR);
    return;
  }
  if ( has_t && has_r ) {
    cmd->Result_String ("The -t option can not be used with the -r option.");
    cmd->set_Status(CMD_ERROR);
    return;
  }

 // Okay. Process the host specification.
  vector<ParseRange>::iterator h_iter;

  for (h_iter=h_list->begin();h_iter != h_list->end(); h_iter++) {
    parse_range_t *h_range = h_iter->getRange();
    parse_val_t *h_val1 = &h_range->start_range;
    if (h_range->is_range) {
      parse_val_t *h_val2 = &h_range->end_range;
// TODO:
    } else {
      if (has_f) {
        Resolve_F_Target ( cmd, exp, tgrp, pt, h_val1->name);
      } else if (has_p) {
        Resolve_P_Target ( cmd, exp, tgrp, pt, h_val1->name);
      } else {
//TODO:
      }
    }
  }
}

static ThreadGroup Resolve_Target_List (CommandObject *cmd, ExperimentObject *exp) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = p_result->GetTargetList();
  ThreadGroup tgrp;
  if (p_tlist->begin() != p_tlist->end()) {
    vector<ParseTarget>::iterator ti;
    for (ti = p_tlist->begin(); ti != p_tlist->end(); ti++) {
      try {
        Resolve_H_Target (cmd, exp, &tgrp, *ti);
      }
      catch(const std::exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        return ThreadGroup();  // return an empty ThreadGroup
      }
    }
  }
  return tgrp;
}

static bool Process_expTypes (CommandObject *cmd, ExperimentObject *exp,
                              void (*cmdfunc) (CommandObject *cmd, ExperimentObject *exp,
                                           Thread t, Collector c) ) {
  if (exp->FW() == NULL) {
    cmd->Result_String ("The experiment has been disabled and can not be changed");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getExpList();

 // Determine the specified (or implied) set of Collectors.
  CollectorGroup cgrp;

  if (p_slist->begin() != p_slist->end()) {
   // The command contains a list of collectors to use.
   // Be sure they are all linked to the experiment.
    vector<string>::iterator si;
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      try {
        Collector c = Get_Collector (exp, *si);
        cgrp.push_back (c);
      }
      catch(const std::exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }
  } else {
   // Use all the collectors that are already part of the experiment.
    cgrp = exp->FW()->getCollectors();
  }


 // Determine the specified (or implied) set of Threads.
  ThreadGroup tgrp;

  tgrp = Resolve_Target_List (cmd, exp);
  if (tgrp.empty()) {
   // Use the threads that are already part of the experiment.
    tgrp = exp->FW()->getThreads();
  }

 // Don't do anything if errors have been detected.
  if ((cmd->Status() == CMD_ERROR) ||
      (cmd->Status() == CMD_ABORTED)) {
    return false;
  }

 // For each thread and each collector, perform the desired function.
  if ((tgrp.begin() != tgrp.end()) &&
      (cgrp.begin() != cgrp.end())) {
   // Link a set of threads to a set of collectors.
    CollectorGroup::iterator ci;
    for (ci=cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      ThreadGroup::iterator ti;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
        (*cmdfunc) (cmd, exp, *ti, c);
      }
    }
  }

  return true;
}

// Experiment Building Block Commands

bool SS_expAttach (CommandObject *cmd) {

 // Determine the specified experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Determine target and collectors and link them together.
  if (!Process_expTypes (cmd, exp, &Attach_Command )) {
   // Don't return anything more if errors have been detected.
    return false;
  }

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool Destroy_Experiment (CommandObject *cmd, ExperimentObject *exp, bool Kill_KeyWord) {
  if (Kill_KeyWord &&
   // Terminate all threads so the application can not continue
   // executing when we release it from control of OpenSpeedShop.
      (exp->FW() != NULL)) {
    ThreadGroup tgrp = exp->FW()->getThreads();
    try {
      tgrp.changeState (Thread::Terminated );
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  }

 // Remove all trace of the experiment from the Command Windows.
  Experiment_Purge_Focus  (exp->ExperimentObject_ID()); // remove any Focus on this experiment
  delete exp;
  return true;
}

bool SS_expClose (CommandObject *cmd) {
 // Terminate the experiment and purge the data structure
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  bool Kill_KeyWord = Look_For_KeyWord (cmd, "kill");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); ) {
      ExperimentObject *exp = *expi;
      expi++;
      if (!Destroy_Experiment (cmd, exp, Kill_KeyWord)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Destroy_Experiment (cmd, exp, Kill_KeyWord)) {
      return false;
    }
  }


 // No result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expCreate (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // There is no specified experiment.  Allocate a new Experiment.
  ExperimentObject *exp = new ExperimentObject ();
  if (exp->FW() == NULL) {
    cmd->Result_String ("Unable to create a new experiment in the FrameWork.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 // Set the parsed command structure to fake a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  cmd->P_Result()->SetExpId(exp->ExperimentObject_ID());

 // Let SS_expAttach do the rest of the work for this command.
  if (!SS_expAttach(cmd)) {
   // Something went wrong - delete the experiment.
    delete exp;
    return false;
  }

 // When we allocate a new experiment, set the focus to point to it.
  (void)Experiment_Focus (WindowID, exp->ExperimentObject_ID());

 // Return the EXPID for this command.
 // Note: SS_expAttach has already done a cmd->set_Status(CMD_COMPLETE);
  cmd->Result_Int (exp->ExperimentObject_ID());
  return true;
}

bool SS_expDetach (CommandObject *cmd) {

 // Determine the specified experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Determine target and collectors and break the link between them.
  if (!Process_expTypes (cmd, exp, &Detach_Command )) {
   // Don't return anything more if errors have been detected.
    return false;
  }

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool Disable_Experiment (CommandObject *cmd, ExperimentObject *exp) {
  exp->Determine_Status();
  if ((exp->Status() != ExpStatus_Paused) &&
      (exp->Status() != ExpStatus_Running)) {
   // These are the only states that can be changed.
    cmd->Result_String ("The experiment can not be Disabled because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 // Disconnect the FrameWork from the experiment
  exp->Suspend();
  return true;
}

bool SS_expDisable (CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Disable_Experiment (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Disable_Experiment (cmd, exp)) {
      return false;
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool Enable_Experiment (CommandObject *cmd, ExperimentObject *exp) {
  if (exp->Status() != ExpStatus_Suspended) {
   // This is the only state that can be enabled.
    cmd->Result_String ("The experiment can not be Enabled because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 // reconnect the FrameWork to the experiment
  exp->ReStart();

  if (exp->FW() == NULL) {
    cmd->Result_String ("The experiment could not be successfully restarted.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }
}

bool SS_expEnable (CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Enable_Experiment (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Enable_Experiment (cmd, exp)) {
      return false;
    }
  }

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

static bool Execute_Experiment (CommandObject *cmd, ExperimentObject *exp) {
  exp->Determine_Status();
  if ((exp->FW() != NULL) &&
      ((exp->Status() == ExpStatus_Paused) ||
       (exp->Status() == ExpStatus_Running))) {

   // Verify that there are threads.
    ThreadGroup tgrp = exp->FW()->getThreads();

    if (tgrp.empty()) {
      cmd->Result_String ("There are no applications specified for the experiment");
      cmd->set_Status(CMD_ERROR);
      return false;
    }

   // Activate the collectors
    CollectorGroup cgrp = exp->FW()->getCollectors();
    CollectorGroup::iterator ci = cgrp.begin();
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      ThreadGroup tgrp = c.getThreads();
      if (!tgrp.empty()) {
        c.startCollecting();
      }
    }

   // Go through the ThreadGroup to handle "don't care" errors.
    for(ThreadGroup::const_iterator tgi = tgrp.begin(); tgi != tgrp.end(); ++tgi) {
      Thread t = *tgi;
      try {
        t.changeState (Thread::Running);
      }
      catch(const std::exception& error) {
        if (t.getState() == Thread::Terminated) {
         // This state causes an error, but we can ignore it.
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // After changing the state of each thread, update that of the ExperimentObject
    exp->Determine_Status();
  } else {
   // Can not run if ExpStatus_Terminated or ExpStatus_Suspended.
    cmd->Result_String ("The experiment can not be run because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }
}

bool SS_expGo (CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Execute_Experiment (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Execute_Experiment (cmd, exp)) {
      return false;
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool Pause_Experiment (CommandObject *cmd, ExperimentObject *exp) {
  exp->Determine_Status();
  if ((exp->Status() != ExpStatus_Paused) &&
      (exp->Status() != ExpStatus_Running)) {
   // These are the only states that can be changed.
    cmd->Result_String ("The experiment can not Pause because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  if ((exp->FW() != NULL) &&
      (exp->Status() == ExpStatus_Running)) {
    ThreadGroup tgrp = exp->FW()->getThreads();

   // Go through the ThreadGroup to handle "don't care" errors.
    for(ThreadGroup::const_iterator tgi = tgrp.begin(); tgi != tgrp.end(); ++tgi) {
      Thread t = *tgi;
      try {
        t.changeState (Thread::Suspended);
      }
      catch(const std::exception& error) {
        if (t.getState() == Thread::Terminated) {
         // This state causes an error, but we can ignore it.
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // After changing the state of each thread, update that of the ExperimentObject
    exp->Determine_Status();
  }
}

bool SS_expPause (CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Pause_Experiment (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Pause_Experiment (cmd, exp)) {
      return false;
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expRestore (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    cmd->Result_String ("need a file name for the Data Base.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Create a new experiment and connect it to the saved data base.
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

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    cmd->Result_String ("need a file name for the Data Base.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Look at general modifier types for "copy" option.
  bool Copy_KeyWord = Look_For_KeyWord (cmd, "copy");

  bool cmd_success = false;
  if (Copy_KeyWord) {
    cmd_success = exp->CopyDB (data_base_name);
  } else {
    cmd_success = exp->RenameDB (data_base_name);
  }

  cmd->set_Status(cmd_success ? CMD_COMPLETE : CMD_ERROR);
  return cmd_success;
}

bool SS_expSetParam (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expView (CommandObject *cmd) {

  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }


  Experiment *fw_experiment = exp->FW();
  if (exp->FW() == NULL) {
    cmd->Result_String ("The experiment has been disabled");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 // Pick up the <viewType> from the comand.
  Assert(cmd->P_Result() != NULL);

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  vector<string>::iterator si;
  for (si = p_slist->begin(); si != p_slist->end(); si++) {
    std::string view = *si;
    if (!SS_Determine_View (cmd, exp, view)) {
      return false;
    }
  }

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
 // List all the allocated experiments
  std::list<ExperimentObject *>::reverse_iterator expi;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
  {
   // Return the EXPID for every known experiment
    cmd->Result_Int ((*expi)->ExperimentObject_ID());
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

bool SS_ListMetrics (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getExpList();

  OpenSpeedShop::Framework::Experiment *fw_exp = NULL;
  CollectorGroup cgrp;

  if (All_KeyWord) {
   // Get list of all the collectors from the FrameWork.
   // To do this, we need to create a dummy experiment.
    try {
      OpenSpeedShop::Framework::Experiment::create (tmpdb);
      fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);
      std::set<Metadata> collectortypes = Collector::getAvailable();
      for (std::set<Metadata>::const_iterator mi = collectortypes.begin();
                mi != collectortypes.end(); mi++) {
        fw_exp->createCollector ( mi->getUniqueId() );
      }
      cgrp = fw_exp->getCollectors();
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else if (cmd->P_Result()->IsExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  } else if (p_slist->begin() != p_slist->end()) {
   // Get the list of collectors from the command.
    try {
      OpenSpeedShop::Framework::Experiment::create (tmpdb);
      fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);

      vector<string>::iterator si;
      for (si = p_slist->begin(); si != p_slist->end(); si++) {
       //  Get a collector object from the framework.
        fw_exp->createCollector ( *si );
      }
      cgrp = fw_exp->getCollectors();
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else {
   // Get the list of colectors from the focused experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  }

  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
    Collector c = *ci;
    Metadata cm = c.getMetadata();
    std::set<Metadata> md = c.getMetrics();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      cmd->Result_String ( cm.getUniqueId() + "::" +  m.getUniqueId() );
    }
  }

  if (fw_exp != NULL) {
    (void) remove (tmpdb.c_str());
    delete fw_exp;
  }

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

bool SS_ListParams (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getExpList();

  OpenSpeedShop::Framework::Experiment *fw_exp = NULL;
  CollectorGroup cgrp;

  if (All_KeyWord) {
   // Get list of all the collectors from the FrameWork.
   // To do this, we need to create a dummy experiment.
    try {
      OpenSpeedShop::Framework::Experiment::create (tmpdb);
      fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);
      std::set<Metadata> collectortypes = Collector::getAvailable();
      for (std::set<Metadata>::const_iterator mi = collectortypes.begin();
                mi != collectortypes.end(); mi++) {
        fw_exp->createCollector ( mi->getUniqueId() );
      }
      cgrp = fw_exp->getCollectors();
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else if (cmd->P_Result()->IsExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  } else if (p_slist->begin() != p_slist->end()) {
   // Get the list of collectors from the command.
    try {
      OpenSpeedShop::Framework::Experiment::create (tmpdb);
      fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);

      vector<string>::iterator si;
      for (si = p_slist->begin(); si != p_slist->end(); si++) {
       //  Get a collector object from the framework.
        fw_exp->createCollector ( *si );
      }
      cgrp = fw_exp->getCollectors();
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else {
   // Get the list of colectors from the focused experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  }

  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
    Collector c = *ci;
    Metadata cm = c.getMetadata();
    std::set<Metadata> md = c.getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      cmd->Result_String ( cm.getUniqueId() + "::" +  m.getUniqueId() );
    }
  }

  if (fw_exp != NULL) {
    (void) remove (tmpdb.c_str());
    delete fw_exp;
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListPids (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
// TODO:  List all the PIDs on the system.
    cmd->Result_String ("not yet implemented");
  } else {
   // Get the Pids for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Is there an FrameWork Experiment to look at?
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
   // Get the list of threads used in the specified experiment.
    ThreadGroup cgrp = exp->FW()->getThreads();
    ThreadGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Thread c = *ci;
      pid_t Pid = c.getProcessId();
      cmd->Result_Int ( (int64_t)c.getProcessId() );
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListRanks (CommandObject *cmd) {
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

bool SS_ListStatus (CommandObject *cmd) {

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List the status all the allocated experiments
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
     // Return the EXPID for every known experiment
      cmd->Result_String ((*expi)->ExpStatus_Name());
    }
  } else {
   // Get the status of a specific Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    cmd->Result_String (exp->ExpStatus_Name());
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListThreads (CommandObject *cmd) {
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

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the avaialble experiment types.
    std::set<Metadata> collectortypes = Collector::getAvailable();
    for (std::set<Metadata>::const_iterator mi = collectortypes.begin(); mi != collectortypes.end(); mi++) {
      cmd->Result_String ( mi->getUniqueId() );
    }
  } else {
   // Get the types for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Is there an FrameWork Experiment to look at?
    if (exp->FW() == NULL) {
      cmd->Result_String ("The experiment has been disabled");
      cmd->set_Status(CMD_ERROR);
      return false;
    }
   // Get the list of collectors used in the specified experiment.
    CollectorGroup cgrp = exp->FW()->getCollectors();
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Metadata m = c.getMetadata();
      cmd->Result_String ( m.getUniqueId() );
    }
  }
  
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListViews (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
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
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

 // Default action with no arguments: Dump the history file.
  R = Command_Trace (cmd, CMDW_TRACE_ORIGINAL_COMMANDS, WindowID,
                     (f_val != NULL) ? f_val->name : std::string(""));

  cmd->set_Status( R ? CMD_COMPLETE : CMD_ERROR);
  return R;
}

bool SS_Log (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool R = false;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
    R = Command_Log_OFF (WindowID);
  } else {
    R = Command_Log_ON(WindowID, f_val->name );
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
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
    cmd->Result_String ("can not determine file name");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  Push_Input_File (WindowID, f_val->name,
                   &Default_TLI_Line_Output, &Default_TLI_Command_Output);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
   (void)Command_Trace_OFF (WindowID);
  } else {
    (void)Command_Trace_ON(WindowID, f_val->name);
  }

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


