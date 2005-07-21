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

// Global Termination Call -

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

static int Wait_For_Exp_State (CommandObject *cmd, int to_state, ExperimentObject *exp) {
 // After changing the state of each thread, wait for the
 // status of the experiment to change.  This is necessary
 // because of the asynchronous nature of the FrameWork.
  int latest = exp->Determine_Status();
  while ((latest != to_state) &&
         (latest != ExpStatus_NonExistent) &&
         (latest != ExpStatus_Terminated) &&
         (latest != ExpStatus_InError)) {
   // Check for asnychonous abort command 
    if ((cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED)) {
      break;
    }
    usleep (10000);
    latest = exp->Determine_Status();
  }

  return latest;
}

static void Wait_For_Thread_Connected (CommandObject *cmd, Thread t) {
  while (t.getState() == Thread::Connecting) {
   // Check for asnychonous abort command 
    if ((cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED)) {
      break;
    }
    usleep (10000);
  }
}

ExperimentObject *Find_Experiment_Object (EXPID ExperimentID)
{
// Search for existing entry.
  if (ExperimentID > 0) {
    std::list<ExperimentObject *>::iterator exp;
    for (exp = ExperimentObject_list.begin(); exp != ExperimentObject_list.end(); exp++) {
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

 // Is there an FrameWork Experiment to look at?
  if (exp->FW() == NULL) {
    cmd->Result_String ("The requested FrameWork experiment does not exist");
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
        !T.getPosixThreadId().first &&
        !T.getMPIRank().first) {
#else
        !T.getPosixThreadId().first) {
#endif
      *returnThread = &T;
      return true;
    }
  }
  return false;
}

bool Collector_Used_In_Experiment (OpenSpeedShop::Framework::Experiment *fexp, std::string myname) {
  CollectorGroup current_cgrp = fexp->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = current_cgrp.begin(); ci != current_cgrp.end(); ci++) {
    Collector C = *ci;
    std::string name = C.getMetadata().getUniqueId();
    if (!strcmp(name.c_str(), myname.c_str())) {
      return true;
    }
  }
  return false;
}

Collector Get_Collector (OpenSpeedShop::Framework::Experiment *fexp, std::string myname) {
  CollectorGroup current_cgrp = fexp->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = current_cgrp.begin(); ci != current_cgrp.end(); ci++) {
    Collector C = *ci;
    std::string name = C.getMetadata().getUniqueId();
    if (!strcmp(name.c_str(), myname.c_str())) {
      return C;
    }
  }
  return fexp->createCollector(myname);
}

// Utilities to restrict a set of OpenSpeedShop::Framework::Experiment::Thread
// using a <target_list> specification.

static bool within_range (int64_t Value, parse_range_t R) {
  parse_val_t pval1 = R.start_range;
  Assert (pval1.tag == VAL_NUMBER);
  int64_t Rvalue1 = pval1.num;
  if (R.is_range) {
    parse_val_t pval2 = R.end_range;
    Assert (pval2.tag == VAL_NUMBER);
    int64_t Rvalue2 = pval2.num;
    if ((Value >= Rvalue1) &&
        (Value <= Rvalue2)) {
      return true;
    }
  } else if (Value == Rvalue1) {
    return true;
  }
  return false;
}

static bool within_range (std::string S, parse_range_t R) {
  parse_val_t pval1 = R.start_range;
  Assert (pval1.tag == VAL_STRING);
  std::string Rname1 = pval1.name;
  if (R.is_range) {
    parse_val_t pval2 = R.end_range;
    Assert (pval2.tag == VAL_STRING);
    std::string Rname2 = pval2.name;
    if ((S >= Rname1) &&
        (S <= Rname2)) {
      return true;
    }
  } else if (S == Rname1) {
    return true;
  }
  return false;
}

void Filter_ThreadGroup (CommandObject *cmd, ThreadGroup& tgrp) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = p_result->GetTargetList();
  if (p_tlist->begin() == p_tlist->end()) {
   // There are no filters.
    return;
  }

  ParseTarget pt = *p_tlist->begin(); // There can only be one!
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

  if (has_f) {
    cmd->Result_String ("Selection based on file name is not supported." );
    cmd->set_Status(CMD_ERROR);
    return;
  }

 // Remove non-matching hosts.
  if (has_h) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ) {
      Thread t = *ti;
      std::string hid = t.getHost();
      bool within_list = false;

      vector<ParseRange>::iterator pr_iter;
      for (pr_iter=h_list->begin();pr_iter != h_list->end(); pr_iter++) {
        if (within_range(hid, *pr_iter->getRange())) {
          within_list = true;
          break;
        }
      }

     // Remove non-matching hosts from the ThreadGroup.
      if (!within_list) {
        tgrp.erase(ti);
      } else {
        ti++;
      }
    }

  }

 // Remove non-matching pids.
  if (has_p) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ) {
      Thread t = *ti;
      pid_t pid = t.getProcessId();
      bool within_list = false;

      vector<ParseRange>::iterator pr_iter;
      for (pr_iter=h_list->begin();pr_iter != h_list->end(); pr_iter++) {
        if (within_range(pid, *pr_iter->getRange())) {
          within_list = true;
          break;
        }
      }

     // Remove non-matching hosts from the ThreadGroup.
      if (!within_list) {
        tgrp.erase(ti);
      } else {
        ti++;
      }
    }

  }

 // Remove non-matching threads.
  if (has_t) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ) {
      Thread t = *ti;
      std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
      if (pthread.first) {
        int64_t tid = pthread.second;
        bool within_list = false;

        vector<ParseRange>::iterator pr_iter;
        for (pr_iter=h_list->begin();pr_iter != h_list->end(); pr_iter++) {
          if (within_range(tid, *pr_iter->getRange())) {
            within_list = true;
            break;
          }
        }

       // Remove non-matching hosts from the ThreadGroup.
        if (!within_list) {
          tgrp.erase(ti);
          continue;
        }
      }
      ti++;
    }

  }

#if HAS_OPENMP
 // Remove non-matching ranks.
  if (has_r) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ) {
      Thread t = *ti;
      int64_t rid = t.getOmpThreadId();
      bool within_list = false;

      vector<ParseRange>::iterator pr_iter;
      for (pr_iter=h_list->begin();pr_iter != h_list->end(); pr_iter++) {
        if (within_range(rid, *pr_iter->getRange())) {
          within_list = true;
          break;
        }
      }

     // Remove non-matching hosts from the ThreadGroup.
      if (!within_list) {
        tgrp.erase(ti);
      } else {
        ti++;
      }
    }

  }
#endif
}

// Utilities to decode <target_list> and attach or detach

static void Attach_Command (CommandObject *cmd, ExperimentObject *exp, Thread t, Collector c) {
  try {
    if (t.getState() == Thread::Disconnected) {
      t.changeState (Thread::Connecting);
    }
    Wait_For_Thread_Connected (cmd, t);

    c.startCollecting(t);  // There is no point in attaching unless we intend to use it!
  }
  catch(const Exception& error) {
    if ((t.getState() == Thread::Terminated) ||
        (t.getState() == Thread::Nonexistent)) {
     // These states cause errors, but we can ignore them.
      return;
    }
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }
}

static void Detach_Command (CommandObject *cmd, ExperimentObject *exp, Thread t, Collector c) {
  try {
    Wait_For_Thread_Connected (cmd, t);
    c.stopCollecting(t);  // We don't want to collect any more data for this thread!
  }
  catch(const Exception& error) {
    if ((t.getState() == Thread::Terminated) ||
        (t.getState() == Thread::Connecting) ||
        (t.getState() == Thread::Disconnected) ||
        (t.getState() == Thread::Nonexistent)) {
     // These states cause errors, but we can ignore them.
      return;
    }
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
        tgrp->insert(t);
      }
      catch(const Exception& error) {
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
        tgrp->insert(t);
      }
      catch(const Exception& error) {
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
          tgrp->insert (*pt);
          continue;
        }
        try {
          ThreadGroup ngrp = exp->FW()->attachProcess(mypid, host_name);
          for(ThreadGroup::const_iterator tgi = ngrp.begin(); tgi != ngrp.end(); ++tgi) {
            Thread t = *tgi;
            tgrp->insert(t);
          }
        }
        catch(const Exception& error) {
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

  InputLineObject *clip = cmd->Clip ();
  CMDWID w = (clip) ? clip->Who() : 0;

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
        Thread t = exp->FW()->createProcess(f_val1->name, host_name,
                                            OutputCallback(&ReDirect_User_Stdout,(void *)w),
                                            OutputCallback(&ReDirect_User_Stderr,(void *)w));
        tgrp->insert(t);
      }
      catch(const Exception& error) {
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
      catch(const Exception& error) {
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
        Collector C = Get_Collector (exp->FW(), *si);
        cgrp.insert (C);
      }
      catch(const Exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }
  } else {
   // Use all the collectors that are already part of the experiment.
    try {
      cgrp = exp->FW()->getCollectors();
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  }


 // Determine the specified (or implied) set of Threads.
  ThreadGroup tgrp;

  tgrp = Resolve_Target_List (cmd, exp);
  if (tgrp.empty()) {
   // Use the threads that are already part of the experiment.
    try {
      tgrp = exp->FW()->getThreads();
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
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

 // Was this really an expCreate comamnd?
 // If so, return to SS_expCreate and let it set an argument.
  if (cmd->Type() == CMD_EXP_ATTACH) {
   // There is no result returned from this command.
    cmd->set_Status(CMD_COMPLETE);
  }
  return true;
}

static bool Destroy_Experiment (CommandObject *cmd, ExperimentObject *exp, bool Kill_KeyWord) {
  if (Kill_KeyWord &&
      (exp->FW() != NULL) &&
       ((exp->Determine_Status() == ExpStatus_Paused) ||
        (exp->Status() == ExpStatus_Running))) {
   // These are the only states that can be changed.

   // Terminate all threads so the application can not continue
   // executing when we release it from control of OpenSpeedShop.

    ThreadGroup tgrp = exp->FW()->getThreads();
    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      try {
        Wait_For_Thread_Connected (cmd, t);
        t.changeState (Thread::Terminated );
      }
      catch(const Exception& error) {
        if ((t.getState() == Thread::Terminated) ||
            (t.getState() == Thread::Disconnected) ||
            (t.getState() == Thread::Nonexistent)) {
         // This state causes an error, but we can ignore it.
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // Be sure the application is in a terminal state.
    (void) Wait_For_Exp_State (cmd, ExpStatus_NonExistent , exp);
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
  bool cmd_executed = true;

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); ) {
      ExperimentObject *exp = *expi;
      expi++;
      if (!Destroy_Experiment (cmd, exp, Kill_KeyWord)) {
        cmd_executed = false;
        break;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if ((exp == NULL) ||
        !Destroy_Experiment (cmd, exp, Kill_KeyWord)) {
      cmd_executed = false;
    }
  }

 // No result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return cmd_executed;
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
  cmd->Result_Int (exp->ExperimentObject_ID());
  cmd->set_Status(CMD_COMPLETE);
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
  
 // Determine target and collectors and turn off data collection.
  try {
    CollectorGroup cgrp = exp->FW()->getCollectors();
    ThreadGroup tgrp = exp->FW()->getThreads();
    cgrp.postponeCollecting(tgrp);
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

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

 // Determine target and collectors and turn on data collection.
  ThreadGroup tgrp = exp->FW()->getThreads();

 // Be sure the Threads are connected.
  ThreadGroup::iterator ti;
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread t = *ti;
    try {
      if (t.getState() == Thread::Disconnected) {
        t.changeState (Thread::Connecting);
      }
    }
    catch(const Exception& error) {
      if ((t.getState() == Thread::Terminated) ||
          (t.getState() == Thread::Connecting) ||
          (t.getState() == Thread::Nonexistent)) {
       // These states cause errors, but we can ignore them.
        continue;
      }
      Mark_Cmd_With_Std_Error (cmd, error);
      continue;   // Keep processing!
    }
  }

 // Restart the collectors.
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread t = *ti;
    CollectorGroup cgrp = t.getPostponedCollectors();
    CollectorGroup::iterator ci;
    for (ci=cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Attach_Command (cmd, exp, t, c);
    }
  }

  return true;
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
  EXPID ExperimentID = 0;

  if (cmd->P_Result()->IsExpId()) {
    ExperimentID = cmd->P_Result()->GetExpId();
  } else {
   // Get the Focused experiment - if it doesn't exist, return the default "0".
    ExperimentID = Experiment_Focus ( WindowID );
  }
  if (ExperimentID != 0) {
   // Be sure the requested experiment exists.
    if (Find_Specified_Experiment (cmd) == NULL) {
      return false;
    }
  }
 // Set the Focus to the given experiment ID.
  ExperimentID = Experiment_Focus ( WindowID, ExperimentID);

 // Return the EXPID for this command.
  cmd->Result_Int (ExperimentID);
  cmd->set_Status(CMD_COMPLETE);
  return true;
} 

static bool Execute_Experiment (CommandObject *cmd, ExperimentObject *exp) {
  if ((exp == NULL) ||
      (exp->FW() == NULL) ||
      (exp->Status() == ExpStatus_NonExistent)) {
    cmd->Result_String ("The experiment can not be run because it does not exist.");
    return false;
  }

  exp->Determine_Status();

  if ((exp->Status() == ExpStatus_Terminated) ||
      (exp->Status() == ExpStatus_InError)) {
   // Can not run if ExpStatus_Terminated or ExpStatus_InError
    cmd->Result_String ("The experiment can not be run because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  if ((exp->Status() == ExpStatus_Paused) ||
      (exp->Status() == ExpStatus_Running)) {

   // Verify that there are threads.
    ThreadGroup tgrp;
    try {
      tgrp = exp->FW()->getThreads();
      if (tgrp.empty()) {
        cmd->Result_String ("There are no applications specified for the experiment");
        cmd->set_Status(CMD_ERROR);
        return false;
      }
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }

   // Go through the ThreadGroup to handle "don't care" errors.
    for(ThreadGroup::const_iterator tgi = tgrp.begin(); tgi != tgrp.end(); ++tgi) {
      Thread t = *tgi;
      try {
       // Be sure transitional states are complete before running.
        if (t.getState() == Thread::Disconnected) {
          t.changeState (Thread::Connecting);
        }
        Wait_For_Thread_Connected (cmd, t);

        t.changeState (Thread::Running);
      }
      catch(const Exception& error) {
        if ((t.getState() == Thread::Terminated) ||
            (t.getState() == Thread::Nonexistent)) {
         // These states cause errors, but we can ignore them.
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // After changing the state of each thread, wait for the
   // something to actually start executing.
    (void) Wait_For_Exp_State (cmd, ExpStatus_Running, exp);
  }
  return true;
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

 // No result returned from this command.
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
       // Be sure transitional states are complete before suspending.
        t.changeState (Thread::Suspended);
      }
      catch(const Exception& error) {
        if ((t.getState() == Thread::Terminated) ||
            (t.getState() == Thread::Connecting) ||
            (t.getState() == Thread::Disconnected) ||
            (t.getState() == Thread::Nonexistent)) {
         // These states cause errors, but we can ignore them.
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // After changing the state of each thread, wait for the
   // the experiment to actually stop.
    (void) Wait_For_Exp_State (cmd, ExpStatus_Paused, exp);
  }
  return true;
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
    cmd->Result_String ("A file name for the Data Base is required.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Create a new experiment and connect it to the saved data base.
  ExperimentObject *exp = new ExperimentObject (data_base_name);
  if ((exp == NULL) ||
      (exp->ExperimentObject_ID() <= 0)) {
    cmd->Result_String ("The specified file name is not a legal data base.");
    cmd->set_Status(CMD_ERROR);
    return false;
  }

 // Pick up the EXPID for an allocated experiment.
  EXPID ExperimentID = exp->ExperimentObject_ID();

 // Set the focus to point to the new EXPID.
  (void)Experiment_Focus (WindowID, ExperimentID);

 // Return the EXPID for this command.
  cmd->Result_Int (ExperimentID);
  cmd->set_Status(CMD_COMPLETE);
  return true;
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

  if (Copy_KeyWord) {
    try {
      exp->CopyDB (data_base_name);
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else {
    try {
      exp->RenameDB (data_base_name);
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool setparam (Collector C, std::string pname, ParseParam pvalue) {
  try {
    std::set<Metadata> md = C.getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      if (m.getUniqueId() != pname) {
       // Not the one we want - keep looking.
        continue;
      }

      if( m.isType(typeid(int)) ) {
        int ival;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%d", &ival);
        } else {
          ival = (int)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(int)ival);
      } else if( m.isType(typeid(int64_t)) ) {
        int64_t i64val;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%lld", &i64val);
        } else {
          i64val = (int64_t)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(int64_t)i64val);
      } else if( m.isType(typeid(uint)) ) {
        uint uval;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%d", &uval);
        } else {
          uval = (uint)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(uint)uval);
      } else if( m.isType(typeid(uint64_t)) ) {
        uint64_t u64val;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%lld", &u64val);
        } else {
          u64val = (uint64_t)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(uint64_t)u64val);
      } else if( m.isType(typeid(float)) ) {
        float fval;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%f", &fval);
        } else {
          fval = (float)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(float)fval);
      } else if( m.isType(typeid(double)) ) {
        double dval;
        if (pvalue.isValString()) {
          sscanf ( pvalue.getStingVal(), "%llf", &dval);
        } else {
          dval = (double)(pvalue.getnumVal());
        }
        C.setParameterValue(pname,(double)dval);
      } else if( m.isType(typeid(string)) ) {
        std::string sval;
        if (pvalue.isValString()) {
          sval = std::string(pvalue.getStingVal());
        } else {
          char cval[20];
          sprintf( cval, "%d", pvalue.getnumVal());
          sval = std::string(&cval[0]);
        }
        C.setParameterValue(pname,(std::string)sval);
      }
      return true;
    }
  }
  catch(const Exception& error) {
   // Ignore problems - the calling routine can figure something out.
  }

 // We didn't find the named parameter in this collector.
  return false;
}

bool SS_expSetParam (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

  Assert(cmd->P_Result() != NULL);
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseParam> *p_list = p_result->getParmList();
  vector<ParseParam>::iterator iter;

  for (iter=p_list->begin();iter != p_list->end(); iter++) {
    std::string param_name = iter->getParmParamType();
    if (iter->getParmExpType()) {
     // The user has specified a particular collector.
     // Set the paramater for just that one collector.
      std::string C_name = std::string(iter->getParmExpType());
      try {
        Collector C = Get_Collector (exp->FW(), C_name);
        setparam(C, param_name, *iter);
      }
      catch(const Exception& error) {
       // Return message, but continue to process other collectors.
        Mark_Cmd_With_Std_Error (cmd, error);
        continue;
      }
    } else {
     // Get the list of collectors used in the specified experiment.
     // Set the paramater for every collector that is part of the experiment.
      bool param_was_set = false;
      CollectorGroup cgrp = exp->FW()->getCollectors();
      CollectorGroup::iterator ci;
      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        param_was_set |= setparam(*ci, param_name, *iter);
      }

      if (!param_was_set) {
       // Record the error but continue to try to set other parameters.
        cmd->Result_String ("The specified parameter, " + param_name + ", was not set for any collector.");
        cmd->set_Status(CMD_ERROR);
      }
    }
  }

  if ((cmd->Status() == CMD_ERROR) ||
      (cmd->Status() == CMD_ABORTED)) {
    return false;
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expView (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Some views do not need depend on an ExperimentObject.
 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->IsExpId()) ? cmd->P_Result()->GetExpId() : Experiment_Focus ( WindowID );
  ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;

 // For batch processing, wait for completion before generating a report.
  if ((exp != NULL) && !Window_Is_Async(WindowID)) {
    (void) Wait_For_Exp_State (cmd, ExpStatus_Paused, exp);
  }

 // Pick up the <viewType> from the comand.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  vector<string>::iterator si;
  for (si = p_slist->begin(); si != p_slist->end(); si++) {
    std::string view = *si;
    if (!SS_Generate_View (cmd, exp, view)) {
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

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
// TODO:  List all the Hosts on the system.
    cmd->Result_String ("not yet implemented");
  } else {
   // Get the Hosts for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    std::set<std::string> hset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      hset.insert (t.getHost());
    }
    for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
      cmd->Result_String ( *hseti );
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListMetrics (CommandObject *cmd) {
  bool cmd_error = false;
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
        Collector C = Get_Collector (fw_exp, mi->getUniqueId());
        cgrp.insert (C);
      }
    }
    catch(const Exception& error) {
     // Return message, but fall through to clean up tmpdb.
      Mark_Cmd_With_Std_Error (cmd, error);
      cmd_error = true;
    }
  } else if (cmd->P_Result()->IsExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
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
        Collector C = Get_Collector (fw_exp, *si);
        cgrp.insert (C);
      }
    }
    catch(const Exception& error) {
     // Return message, but fall through to clean up tmpdb.
      Mark_Cmd_With_Std_Error (cmd, error);
      cmd_error = true;
    }
  } else {
   // Get the list of collectors from the focused experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  }

  if (!cmd_error) {
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
  }

  if (fw_exp != NULL) {
    (void) remove (tmpdb.c_str());
    delete fw_exp;
  }

  if (!cmd_error) {
    cmd->set_Status(CMD_COMPLETE);
  }
  return cmd_error;
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
  bool cmd_error = false;
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
        Collector C = Get_Collector (fw_exp, mi->getUniqueId() );
        cgrp.insert (C);
      }
    }
    catch(const Exception& error) {
     // Return message, but fall through to clean up tmpdb.
      Mark_Cmd_With_Std_Error (cmd, error);
      cmd_error = true;
    }
  } else if (cmd->P_Result()->IsExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
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
        Collector C = Get_Collector (fw_exp, *si);
        cgrp.insert (C);
      }
    }
    catch(const Exception& error) {
     // Return message, but fall through to clean up tmpdb.
      Mark_Cmd_With_Std_Error (cmd, error);
      cmd_error = true;
    }
  } else {
   // Get the list of collectors from the focused experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    cgrp = exp->FW()->getCollectors();
  }

  if (!cmd_error) {
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
  }

  if (fw_exp != NULL) {
    (void) remove (tmpdb.c_str());
    delete fw_exp;
  }

  if (!cmd_error) {
    cmd->set_Status(CMD_COMPLETE);
  }
  return cmd_error;
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

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd, tgrp);
    std::set<pid_t> pset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      pset.insert (t.getProcessId());
    }
    for (std::set<pid_t>::iterator pseti = pset.begin(); pseti != pset.end(); pseti++) {
      cmd->Result_Int ( *pseti );
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListRanks (CommandObject *cmd) {
#ifdef HAVE_MPI
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
// TODO:  List all the PIDs on the system.
    cmd->Result_String ("not yet implemented");
  } else {
   // Get the Rankss for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd, tgrp);
    std::set<int64_t> rset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      int64_t rank = 0;
#ifdef HAVE_MPI
      if (t.getPosixThreadId().first &&
          t.getMPIRank().first) {
        rank = t.getMPIRank().second;
      }
#else
      if (t.getPosixThreadId().first) {
        rank = t.getPosixThreadId().second;
      }
#endif
      rset.insert ( rank );
    }
    for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
      cmd->Result_Int ( *rseti );
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;

#else
  cmd->Result_String ("The system does not support MPI Ranks.");
  cmd->set_Status(CMD_ERROR);
  return false;
#endif
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
#ifdef HAVE_OPENMP
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
// TODO:  List all the Threads on the system.
    cmd->Result_String ("not yet implemented");
  } else {
   // Get the Threads for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd, tgrp);
    std::set<int64_t> tset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      tset.insert ( (int64_t)t.getOmpThreadId() );
    }
    for (std::set<int64_t>::iterator tseti = tset.begin(); tseti != tset.end(); tseti++) {
      cmd->Result_Int ( *tseti );
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;

#else
  cmd->Result_String ("The system does not support OpenMp Threads.");
  cmd->set_Status(CMD_ERROR);
  return false;
#endif
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

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getExpList();
  CollectorGroup cgrp;

  if (All_KeyWord) {
   // What are all the known views that can be generated?
    SS_Get_Views (cmd);
  } else if (cmd->P_Result()->IsExpId()) {
   // What views can be genrated from the information collected in this experiment?
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    SS_Get_Views (cmd, exp->FW());
  } else if (p_slist->begin() != p_slist->end()) {
   // What views depend on a specific collector?
    vector<string>::iterator si;
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      SS_Get_Views (cmd, *si );
    }
  } else {
   // What views can be generated for the information collected in the focused experiment?
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    SS_Get_Views (cmd, exp->FW());
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

  cmd->P_Result()->dumpHelp(cmd);
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_History (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool R = true;;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);
  std::string tofname = (f_val != NULL) ? f_val->name : std::string("");
  ss_ostream *to_ostream = Predefined_ostream (tofname);
  FILE *tof = NULL;

 // Determine where to sent output.
  if (to_ostream == NULL) {
    if (tofname.length() != 0) {
     // The user specified a name - use it!
      tof = fopen (tofname.c_str(), "a");
      if (tof == NULL) {
        cmd->Result_String ("Could not open output file " + tofname);
        cmd->set_Status(CMD_ERROR);
        return false;
      }
    } else {
     // No file specified - direct output to window that issued the command.
     // This is accomplished by attaching the strings to cmd.
    }
  }

 // Copy commands from the history list.
 // Skip the last one because it is this "history" command.
  std::list<std::string>::iterator hi;
  for (hi = History.begin(); hi != History.end(); ) {
    std::string S = *hi;
    if (++hi != History.end()) {
      if (to_ostream != NULL) {
        to_ostream->mystream() << S;
      } else if (tof != NULL) {
        fprintf(tof,"%s",S.c_str());
      } else {
       // Attach result to CommandObject.
        cmd->Result_String (S);
      }
    }
  }

  if (tof != NULL) {
    fflush(tof);
    fclose (tof);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
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
    if (!R) {
      cmd->Result_String (f_val->name + " could not be opened as a log file.");
      return false;
    }
  }

 // This command does not return a result.
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

  if ( !Push_Input_File (WindowID, f_val->name,
                         &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
    cmd->Result_String ("Unable to open alternate command file " + f_val->name);
    cmd->set_Status(CMD_ERROR);
    return false;
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
   (void)Command_Record_OFF (WindowID);
  } else {
    if (!Command_Record_ON(WindowID, f_val->name)) {
      cmd->Result_String (f_val->name + " could not be opened for recording.");
      return false;
    }
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


