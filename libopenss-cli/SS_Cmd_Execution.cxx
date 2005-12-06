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
using namespace OpenSpeedShop::cli;

// Private Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
pthread_mutex_t Experiment_List_Lock = PTHREAD_MUTEX_INITIALIZER;
std::list<ExperimentObject *> ExperimentObject_list;
static std::string tmpdb = std::string("./ssdbtmpcmd.openss");

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
inline std::string int2str (int64_t e) {
  char s[40];
  sprintf ( s, "%lld", e);
  return std::string (s);
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
    usleep (100000);
    latest = exp->Determine_Status();
  }

  return latest;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static ExperimentObject *Find_Specified_Experiment (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : 0;
  ExperimentObject *exp = NULL;

 // If not experiment has been specified, pick up the
 // "focused" experiment associated witht he input window.
  if (ExperimentID == 0) {
    ExperimentID = Experiment_Focus ( WindowID );
    if (ExperimentID == 0) {
      std::string s("There is no focused experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return NULL;
    }
  }
  exp = Find_Experiment_Object (ExperimentID);
  if (exp == NULL) {
    std::string s("The requested experiment ID does not exist.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return NULL;
  }

 // Is there an FrameWork Experiment to look at?
  if (exp->FW() == NULL) {
    std::string s("The requested FrameWork experiment does not exist.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return NULL;
  }

  return exp;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
parse_val_t *Get_Simple_File_Name (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = (p_result != NULL) ? p_result->getTargetList() : NULL;
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool Look_For_KeyWord (CommandObject *cmd, std::string Key) {
  Assert (cmd->P_Result() != NULL);

 // Look at general modifier types for a specific KeyWord option.
  vector<string> *p_slist = cmd->P_Result()->getModifierList();
  vector<string>::iterator j;

  for (j=p_slist->begin();j != p_slist->end(); j++) {
    std::string S = *j;
    if (!strcasecmp(S.c_str(),Key.c_str())) {
      return true;
    }
  }

  return false;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool within_range (pid_t Value, parse_range_t R) {
  parse_val_t pval1 = R.start_range;
  Assert (pval1.tag == VAL_NUMBER);
  pid_t Rvalue1 = pval1.num;
  if (R.is_range) {
    parse_val_t pval2 = R.end_range;
    Assert (pval2.tag == VAL_NUMBER);
    pid_t Rvalue2 = pval2.num;
    if ((Value >= Rvalue1) &&
        (Value <= Rvalue2)) {
      return true;
    }
  } else if (Value == Rvalue1) {
    return true;
  }
  return false;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool within_range (std::string S, parse_range_t R) {
  parse_val_t pval1 = R.start_range;
  Assert (pval1.tag == VAL_STRING);
  std::string Rname1 = Experiment::getCanonicalName(pval1.name);
  if (R.is_range) {
    parse_val_t pval2 = R.end_range;
    Assert (pval2.tag == VAL_STRING);
    std::string Rname2 = Experiment::getCanonicalName(pval2.name);
    if ((S >= Rname1) &&
        (S <= Rname2)) {
      return true;
    }
  } else if (S == Rname1) {
    return true;
  }
  return false;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool Filter_Uses_F (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->begin() == p_tlist->end()) {
   // There are no filters.
    return false;
  }

  ParseTarget pt = *p_tlist->begin(); // There can only be one!
  vector<ParseRange> *f_list = pt.getFileList();

  return !((f_list == NULL) || f_list->empty());
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void Filter_ThreadGroup (CommandObject *cmd, ThreadGroup& tgrp) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->begin() == p_tlist->end()) {
   // There are no filters.
    return;
  }

  ThreadGroup dgrp;

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

 // Remove non-matching hosts.
  if (has_h) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      std::string hid = Experiment::getCanonicalName(t.getHost());
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
        dgrp.insert(t);
      }
    }

  }

 // Remove non-matching pids.
  if (has_p) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      pid_t pid = t.getProcessId();
      bool within_list = false;

      vector<ParseRange>::iterator pr_iter;
      for (pr_iter=p_list->begin();pr_iter != p_list->end(); pr_iter++) {
        if (within_range(pid, *pr_iter->getRange())) {
          within_list = true;
          break;
        }
      }

     // Remove non-matching hosts from the ThreadGroup.
      if (!within_list) {
        dgrp.insert(t);
      }
    }

  }

 // Remove non-matching threads.
  if (has_t) {

    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
      if (pthread.first) {
        int64_t tid = pthread.second;
        bool within_list = false;

        vector<ParseRange>::iterator pr_iter;
        for (pr_iter=t_list->begin();pr_iter != t_list->end(); pr_iter++) {
          if (within_range(tid, *pr_iter->getRange())) {
            within_list = true;
            break;
          }
        }

       // Remove non-matching hosts from the ThreadGroup.
        if (!within_list) {
          dgrp.insert(t);
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
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      int64_t rid = t.getOmpThreadId();
      bool within_list = false;

      vector<ParseRange>::iterator pr_iter;
      for (pr_iter=r_list->begin();pr_iter != r_list->end(); pr_iter++) {
        if (within_range(rid, *pr_iter->getRange())) {
          within_list = true;
          break;
        }
      }

     // Remove non-matching hosts from the ThreadGroup.
      if (!within_list) {
        dgrp.insert(t);
      }
    }

  }
#endif

 // Remove the unneeded threads from the original group.
    ThreadGroup::iterator ti;
    for (ti = dgrp.begin(); ti != dgrp.end(); ti++) {
      Thread t = *ti;
      tgrp.erase(t);
    }

}

// Utilities to decode <target_list> and attach or detach

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
    for ( myrank = r_val1->num; myrank <= r_val2->num; myrank++) {
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
    for ( mythread = t_val1->num; mythread <= t_val2->num; mythread++) {
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
  bool Mpi_KeyWord = Look_For_KeyWord (cmd, "mpi");
  vector<ParseRange>::iterator p_iter;

  for (p_iter=p_list->begin();p_iter != p_list->end(); p_iter++) {
    parse_range_t *p_range = p_iter->getRange();
    parse_val_t *p_val1 = &p_range->start_range;
    parse_val_t *p_val2 = p_val1;
    if (p_range->is_range) {
      p_val2 = &p_range->end_range;
    }

    pid_t mypid;
    for ( mypid = p_val1->num; mypid <= p_val2->num; mypid++) {
      if (has_t) {
        Resolve_T_Target ( cmd, exp, tgrp, pt, host_name, mypid);
      } else if (has_r) {
        Resolve_R_Target ( cmd, exp, tgrp, pt, host_name, mypid);
      } else {
        try {
          ThreadGroup ngrp;
          if (Mpi_KeyWord) {
            ngrp = exp->FW()->attachMPIJob (mypid, host_name);
          } else {
            ngrp = exp->FW()->attachProcess(mypid, host_name);
          }
          ThreadGroup::iterator ngi;
          for( ngi = ngrp.begin(); ngi != ngrp.end(); ngi++) {
            Thread t = *ngi;
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
      std::string s("Can not retrieve host name.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return;
    }
    //pt.pushHostPoint (HostName[0]);
    //h_list = pt.getHostList();
    h_list->push_back (ParseRange(&HostName[0]));
    has_h = true;
  }

 // Semantic check for illegal combinations.
  if ( has_f && (has_p || has_t || has_r) ) {
    std::string s("The -f option can not be used with -p -t or -r options.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return;
  }
  if ( has_t && has_r ) {
    std::string s("The -t option can not be used with the -r option.");
    Mark_Cmd_With_Soft_Error(cmd,s);
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static ThreadGroup Resolve_Target_List (CommandObject *cmd, ExperimentObject *exp) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expAttach (CommandObject *cmd) {

 // Determine the specified experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

 // Determine target and collectors and link them together.
  bool process_OK = Process_expTypes (cmd, exp, &Attach_Command );
  exp->Q_UnLock ();

 // There is no result returned from this command.
  if (process_OK) {
    cmd->set_Status(CMD_COMPLETE);
  }
  return process_OK;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool Destroy_Experiment (CommandObject *cmd, ExperimentObject *exp, bool Kill_KeyWord) {
 // Clean up the notice board.
  Cancle_Async_Notice (exp);
  Cancle_Exp_Wait     (exp);

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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expClose (CommandObject *cmd) {
 // Terminate the experiment and purge the data structure
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  bool Kill_KeyWord = Look_For_KeyWord (cmd, "kill");
  bool cmd_executed = true;

 // Wait for all executing commands to terminante.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when an entry is
 // deleted from it.
  Wait_For_Previous_Cmds ();

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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expCreate (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Wait for all executing commands to terminate.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when a new entry
 // is added to it.
  Wait_For_Previous_Cmds ();

 // There is no specified experiment.  Allocate a new Experiment.
  ExperimentObject *exp = new ExperimentObject ();
  if (exp->FW() == NULL) {
    std::string s("Unable to create a new experiment in the FrameWork.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }
  EXPID exp_id = exp->ExperimentObject_ID();

 // Determine target and collectors and link them together.
  if (!Process_expTypes (cmd, exp, &Attach_Command )) {
   // Something went wrong - delete the experiment.
    delete exp;
    return false;
  }

 // When we allocate a new experiment, set the focus to point to it.
  (void)Experiment_Focus (WindowID, exp_id);

 // Let other comamnds get access to the experiment and new focus.
  SafeToDoNextCmd ();

 // Annotate the command
  cmd->Result_Annotation ("The new focused experiment identifier is:  -x ");

 // Return the EXPID for this command.
  cmd->Result_Int (exp_id);
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expDetach (CommandObject *cmd) {

 // Determine the specified experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

 // Determine target and collectors and break the link between them.
  if (!Process_expTypes (cmd, exp, &Detach_Command )) {
   // Don't return anything more if errors have been detected.
    return false;
  }

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  exp->Q_UnLock ();
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool Disable_Experiment (CommandObject *cmd, ExperimentObject *exp) {
 // Clean up the notice board.
  Cancle_Async_Notice (exp);
  Cancle_Exp_Wait     (exp);
  
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool Enable_Experiment (CommandObject *cmd, ExperimentObject *exp) {
 // Clean up the notice board.
  Cancle_Async_Notice (exp);
  Cancle_Exp_Wait     (exp);

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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expFocus  (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // The experiment specifier is optional and does not deafult to the focused experiment
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = 0;

  if (cmd->P_Result()->isExpId()) {
    ExperimentID = cmd->P_Result()->getExpId();
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
  cmd->Result_Annotation ("The current focused experiment is:  -x ");
  cmd->set_Status(CMD_COMPLETE);
  return true;
} 

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool Execute_Experiment (CommandObject *cmd, ExperimentObject *exp) {
 // Get the current status of this experiment.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status();
  exp->Q_UnLock ();

  if ((exp->FW() == NULL) ||
      (exp->Status() == ExpStatus_NonExistent)) {
    Mark_Cmd_With_Soft_Error(cmd,
                             "The experiment can not be run because "
                             "it is not atached to an application.");
    return false;
  }

  if ((exp->Status() == ExpStatus_Terminated) ||
      (exp->Status() == ExpStatus_InError)) {
   // Can not run if ExpStatus_Terminated or ExpStatus_InError
    std::string s("The experiment can not be run because it is in the "
    	    	    + exp->ExpStatus_Name() + " state.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  if ((exp->Status() == ExpStatus_Paused) ||
      (exp->Status() == ExpStatus_Running)) {

   // Verify that there are threads.
    ThreadGroup tgrp;
    try {
      tgrp = exp->FW()->getThreads();
      if (tgrp.empty()) {
    	std::string s("There are no applications specified for the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
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

   // Notify the user when the experiment has terminated.
    if (Embedded_WindowID == 0) {
      Request_Async_Notice_Of_Termination (cmd, exp);
    }

   // Annotate the command
    cmd->Result_Annotation ("Start asynchronous execution of experiment:  -x "
                             + int2str(exp->ExperimentObject_ID()) + "\n");
  }
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool Pause_Experiment (CommandObject *cmd, ExperimentObject *exp) {
 // Clean up the notice board.
  Cancle_Async_Notice (exp);
  Cancle_Exp_Wait     (exp);

 // Get the current status of this experiment.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status();
  exp->Q_UnLock ();

  if ((exp->Status() != ExpStatus_NonExistent) &&
      (exp->Status() != ExpStatus_Paused) &&
      (exp->Status() != ExpStatus_Running)) {
   // These are the only states that can be changed.
    std::string s("The experiment can not Pause because it is in the "
                         + exp->ExpStatus_Name() + " state.");
    Mark_Cmd_With_Soft_Error(cmd,s);
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

   // Annotate the command
    cmd->Result_Annotation ("Suspend execution of experiment:  -x "
                             + int2str(exp->ExperimentObject_ID()) + "\n");
  }
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expRestore (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    std::string s("A file name for the Database is required.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Wait for all executing commands to terminate.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when a new entry
 // is added to it.
  Wait_For_Previous_Cmds ();

 // Create a new experiment and connect it to the saved data base.
  ExperimentObject *exp = new ExperimentObject (data_base_name);
  if ((exp == NULL) ||
      (exp->ExperimentObject_ID() <= 0)) {
    std::string s("The specified file name is not a legal data base.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

 // Pick up the EXPID for an allocated experiment.
  EXPID ExperimentID = exp->ExperimentObject_ID();

 // Set the focus to point to the new EXPID.
  (void)Experiment_Focus (WindowID, ExperimentID);

 // Annotate the command
  cmd->Result_Annotation ("The restored experiment identifier is:  -x ");

 // Return the EXPID for this command.
  cmd->Result_Int (ExperimentID);
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expSave (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    std::string s("Need a file name for the Database.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Look at general modifier types for "copy" option.
  bool Copy_KeyWord = Look_For_KeyWord (cmd, "copy");

  if (Copy_KeyWord) {
    try {
     // Wait for previous comands to complete so that
     // the copy has all the requested information.
      Wait_For_Previous_Cmds ();

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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool setparam (Collector C, std::string pname, ParseParam pvalue) {
     // Stop the collector so we can change the parameter value.
  ThreadGroup active;

 // Stop the collector so we can change the parameter value.
  active = C.getThreads();
  active.postponeCollecting(C);

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

   // Restart the collector with a different parameter value.
    active.startCollecting(C);
    return true;
  }

 // Restart the collector with the old parameter value.
  active.startCollecting(C);

 // We didn't find the named parameter in this collector.
  return false;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expSetParam (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

  Assert(cmd->P_Result() != NULL);
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseParam> *p_list = p_result->getParmList();
  vector<ParseParam>::iterator iter;

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

  for (iter=p_list->begin();iter != p_list->end(); iter++) {
    std::string param_name = iter->getParmParamType();
    if (iter->getParmExpType()) {
     // The user has specified a particular collector.
     // Set the paramater for just that one collector.
      std::string C_name = std::string(iter->getParmExpType());
      try {
        Collector C = Get_Collector (exp->FW(), C_name);
        (void) setparam(C, param_name, *iter);
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
        try {
          param_was_set |= setparam(*ci, param_name, *iter);
        }
        catch(const Exception& error) {
         // Return message, but continue to process other collectors.
          Mark_Cmd_With_Std_Error (cmd, error);
          continue;
        }
      }

      if (!param_was_set) {
       // Record the error but continue to try to set other parameters.
    	std::string s("The specified parameter, " + param_name + 
	    	    	", was not set for any collector.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
      }
    }
  }
  exp->Q_UnLock ();

  if ((cmd->Status() == CMD_ERROR) ||
      (cmd->Status() == CMD_ABORTED)) {
    return false;
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


// Information Commands

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static CommandResult *Get_Collector_Metadata (Collector c, Metadata m) {
  CommandResult *Param_Value = NULL;
  std::string id = m.getUniqueId();
  if( m.isType(typeid(int)) ) {
    int Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(unsigned int)) ) {
    uint Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    int64_t Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(string)) ) {
    std::string Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else {
    Param_Value = CRPTR ("Unknown type.");
  }
  return Param_Value;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
static bool ReportStatus(CommandObject *cmd, ExperimentObject *exp) {

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status ();

  char id[20]; sprintf(&id[0],"%lld",(int64_t)exp->ExperimentObject_ID());
  cmd->Result_String ("Experiment definition");
  std::string TmpDB = exp->Data_Base_Is_Tmp() ? "Temporary" : "Saved";
  cmd->Result_String ("{ # ExpId is " + std::string(&id[0])
                         + ", Status is " + exp->ExpStatus_Name()
                         + ", " + TmpDB + " database is " + exp->Data_Base_Name ());
  try {
      if (exp->FW() != NULL) {
        ThreadGroup tgrp = exp->FW()->getThreads();
        ThreadGroup::iterator ti;
        bool atleastone = false;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          std::string host = Experiment::getCanonicalName(t.getHost());
          pid_t pid = t.getProcessId();
          if (!atleastone) {
            atleastone = true;
            cmd->Result_String ("  Currently Specified Components:");
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
          cmd->Result_String ( S );
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
              cmd->Result_String ("  Previously Used Data Collectors:");
              S = S + "    ";
              atleastone = true;
            }
            S = S + m.getUniqueId();
          }
        }
        if (atleastone) {
          cmd->Result_String ( S );
        }

        if (cgrp.begin() != cgrp.end()) {

          cmd->Result_String ("  Metrics:");
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata cm = c.getMetadata();
            std::set<Metadata> md = c.getMetrics();
            std::set<Metadata>::const_iterator mi;
            for (mi = md.begin(); mi != md.end(); mi++) {
              Metadata m = *mi;
              S = "    " + cm.getUniqueId() + "::" +  m.getUniqueId();
              cmd->Result_String (S);
            }
          }

          cmd->Result_String ("  Parameter Values:");
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
              C->CommandResult_Columns::Add_Column (Get_Collector_Metadata (c, m));
              cmd->Result_Predefined (C);
            }
          }

          cmd->Result_String ("  Available Views:");
          SS_Get_Views (cmd, exp->FW(), "    ");
        }

      }
    cmd->Result_String ( "}");
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    cmd->Result_String ( "}");
    exp->Q_UnLock ();
    return false;
  }

  exp->Q_UnLock ();
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expStatus(CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

  if (All_KeyWord) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      if (!ReportStatus (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!ReportStatus (cmd, exp)) {
      return false;
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_expView (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool view_result = true;

 // Some views do not need depend on an ExperimentObject.
 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : Experiment_Focus ( WindowID );
  ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;

 // For batch processing, wait for completion before generating a report.
  if ((exp != NULL) && !Window_Is_Async(WindowID) && (Embedded_WindowID == 0)) {
    (void) Wait_For_Exp_State (cmd, ExpStatus_Paused, exp);
  }

 // Prevent this experiment from changing until we are done.
  if (exp != NULL) exp->Q_Lock (cmd, true);

 // Pick up the <viewType> from the comand.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  vector<string>::iterator si;
  if (p_slist->begin() == p_slist->end()) {
   // The user has not selected a view.
    if ((exp == NULL) ||
        (exp->FW() == NULL)) {
     // No experiment was specified, so we can't find a useful view to gneerate.
      std::string s("No valid experiment was specified.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      view_result = false;
    } else {
     // Look for a view that would be meaningful.
      std::string use_view = "stats";  // Use generic view as default

      CollectorGroup cgrp = exp->FW()->getCollectors();
      if (cgrp.begin() == cgrp.end()) {
       // No collector was used.
    	std::string s("No performance measurements were made for the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
        view_result = false;
      } else {
        if (cgrp.size() == 1) {
         // The experiment used only one collector.
         // See if there is a view by the same name.
          Collector c = *(cgrp.begin());
          Metadata m = c.getMetadata();
          std::string collector_name = m.getUniqueId();
          ViewType *vt = Find_View (collector_name);
          if (vt != NULL) {
            use_view = collector_name;
          }
        }
       // Generate the selected view
        view_result = SS_Generate_View (cmd, exp, use_view);
      }
    }
  } else {
   // Generate all the views in the list.
    
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      std::string view = *si;

      view_result = SS_Generate_View (cmd, exp, view);

      if (!view_result) {
        break;
      }
    }
  }

 // Release the experiment lock.
  if (exp != NULL) exp->Q_UnLock ();

  if (view_result) {
    cmd->set_Status(CMD_COMPLETE);
  }
  return view_result;
}

// Primitive Information Commands

/**
 * Method: SS_ListGeneric()
 * 
 * Look at general modifier types for list 
 * type modifiers and then call respective
 * list command function.
 *     
 * @param   cmd Standard command object.
 *
 * @return  bool
 *
 * @todo    Error handling for multiple type modifiers.
 * @todo    Error handling for no type modifiers.
 *
 */
static enum {
    ENUM_BREAK,
    ENUM_EXP,
    ENUM_TYPES_NEW,
    ENUM_HOSTS,
    ENUM_METRICS,
    ENUM_OBJ,
    ENUM_PARAMS,
    ENUM_PIDS,
    ENUM_RANKS,
    ENUM_SRC,
    ENUM_STATUS,
    ENUM_THREADS,
    ENUM_TYPES_OLD,
    ENUM_TYPES,
    ENUM_VIEWS,
} list_enums;
static char *list_types[] = {
    "breaks",
    "exp",
    "exptypes",
    "hosts",
    "metrics",
    "obj",
    "params",
    "pids",
    "ranks",
    "src",
    "status",
    "threads",
    "types",
    "views",
    NULL
};
bool SS_ListGeneric (CommandObject *cmd) {
  Assert(cmd->P_Result() != NULL);

  int i = 0;
  // Keep looking until NULL
  while(list_types[i]) {
    bool KeyWord = Look_For_KeyWord (cmd, list_types[i]);

    if (KeyWord) {
    	// If I was clever I would have incorporated
	// the function name into the list_types array
	// and have made this a single call.
    	switch(i) {
	    case ENUM_BREAK:
	    	return SS_ListBreaks(cmd);
	    case ENUM_EXP:
	    	return SS_ListExp(cmd);
	    case ENUM_HOSTS:
	    	return SS_ListHosts(cmd);
	    case ENUM_METRICS:
	    	return SS_ListMetrics(cmd);
	    case ENUM_OBJ:
	    	return SS_ListObj(cmd);
	    case ENUM_PARAMS:
	    	return SS_ListParams(cmd);
	    case ENUM_PIDS:
	    	return SS_ListPids(cmd);
	    case ENUM_RANKS:
	    	return SS_ListRanks(cmd);
	    case ENUM_SRC:
	    	return SS_ListSrc(cmd);
	    case ENUM_STATUS:
	    	return SS_ListStatus(cmd);
	    case ENUM_THREADS:
	    	return SS_ListThreads(cmd);
	    case ENUM_TYPES_OLD:
	    case ENUM_TYPES_NEW:
	    	return SS_ListTypes(cmd);
	    case ENUM_VIEWS:
	    	return SS_ListViews(cmd);
	    default :
	    	break;
	}
    }
    ++i;
  }

  return false;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_ListBreaks (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_ListExp (CommandObject *cmd) {
 // List all the allocated experiments
  SafeToDoNextCmd ();
  std::list<ExperimentObject *>::reverse_iterator expi;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
  {
   // Return the EXPID for every known experiment
    cmd->Result_Int ((*expi)->ExperimentObject_ID());
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    std::set<std::string> hset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      Thread t = *ti;
      hset.insert (Experiment::getCanonicalName(t.getHost()));
    }
    for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
      cmd->Result_String ( *hseti );
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
    SafeToDoNextCmd ();
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
  } else if (cmd->P_Result()->isExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    cgrp = exp->FW()->getCollectors();

    exp->Q_UnLock ();
  } else if (p_slist->begin() != p_slist->end()) {
   // Get the list of collectors from the command.
    SafeToDoNextCmd ();
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

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    cgrp = exp->FW()->getCollectors();

    exp->Q_UnLock ();
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_ListObj (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // List the object files for a specified Experiment or the focused Experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

 // Get a list of the unique threads used in the specified experiment.
  ThreadGroup tgrp = exp->FW()->getThreads();
  Filter_ThreadGroup (cmd, tgrp);
  std::set<LinkedObject> ulset;
  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread t = *ti;
    std::set<LinkedObject> lset = t.getLinkedObjects();
    for (std::set<LinkedObject>::iterator lseti = lset.begin(); lseti != lset.end(); lseti++) {
     // Build a set of unique LinkedObjects.
      ulset.insert ( *lseti );
    }
  }
 // Now go through the list of unique LinkedObjects and list their names.
  for (std::set<LinkedObject>::iterator lseti = ulset.begin(); lseti != ulset.end(); lseti++) {
    LinkedObject lobj = *lseti;
    std::string L = lobj.getPath();
    cmd->Result_String ( L );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
  } else if (cmd->P_Result()->isExpId()) {
   // Get the list of collectors from the specified experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    cgrp = exp->FW()->getCollectors();

    exp->Q_UnLock ();
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

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    cgrp = exp->FW()->getCollectors();

    exp->Q_UnLock ();
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

    if (Filter_Uses_F(cmd)) {
      std::string s("Selection based on file name is not supported.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

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

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

    if (Filter_Uses_F(cmd)) {
      std::string s("Selection based on file name is not supported.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

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

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;

#else
  std::string s("The system does not support MPI Ranks.");
  Mark_Cmd_With_Soft_Error(cmd,s);
  return false;
#endif
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_ListSrc (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // List the functions for a specified Experiment or the focused Experiment.
  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

 // Get a list of the unique threads used in the specified experiment.
  ThreadGroup tgrp = exp->FW()->getThreads();
  Filter_ThreadGroup (cmd, tgrp);
  std::set<LinkedObject> ulset;
  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread t = *ti;
    std::set<LinkedObject> lset = t.getLinkedObjects();
    for (std::set<LinkedObject>::iterator lseti = lset.begin(); lseti != lset.end(); lseti++) {
     // Build a set of unique LinkedObjects.
      ulset.insert ( *lseti );
    }
  }
 // Now go through the list of unique LinkedObjects and find the functions.
  for (std::set<LinkedObject>::iterator lseti = ulset.begin(); lseti != ulset.end(); lseti++) {
    LinkedObject lobj = *lseti;
    std::string L = lobj.getPath();
    std::set<Function> fset = lobj.getFunctions();
    std::set<std::string> mset;
   // For each function in the LinkedObject, get the file name it is in
   // and build a set of these file names.  Take advantage of the property
   // of std::set operation that eliminates duplicates.
    for (std::set<Function>::iterator fseti = fset.begin(); fseti != fset.end(); fseti++) {
      Function fobj = *fseti;
      std::set<Statement> sobj = fobj.getDefinitions();
      if( sobj.size() > 0 ) {
        std::set<Statement>::const_iterator sobji = sobj.begin();
        std::string F = sobji->getPath();
        mset.insert ( F );
      }
    }
   // Now we're ready to list the file names.
    for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
      cmd->Result_String ( *mseti );
    }
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
      ExperimentObject *exp = *expi;
      exp->Q_Lock (cmd, false);
      exp->Determine_Status ();
      cmd->Result_String (exp->ExpStatus_Name());
      exp->Q_UnLock ();
    }
  } else {
   // Get the status of a specific Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);
    exp->Determine_Status ();
    cmd->Result_String (exp->ExpStatus_Name());
    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

    if (Filter_Uses_F(cmd)) {
      std::string s("Selection based on file name is not supported.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

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

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;

#else
  std::string s("The system does not support OpenMp Threads.");
  Mark_Cmd_With_Soft_Error(cmd,s);
  return false;
#endif
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of collectors used in the specified experiment.
    CollectorGroup cgrp = exp->FW()->getCollectors();
    CollectorGroup::iterator ci;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Metadata m = c.getMetadata();
      cmd->Result_String ( m.getUniqueId() );
    }

    exp->Q_UnLock ();
  }
  
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
  } else if (cmd->P_Result()->isExpId()) {
   // What views can be genrated from the information collected in this experiment?
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    SS_Get_Views (cmd, exp->FW());

    exp->Q_UnLock ();
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

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    SS_Get_Views (cmd, exp->FW());

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

// Session Commands

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_ClearBreaks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Exit (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  Assert (WindowID != 0);

 // Purge all waiting input and all commands awaiting dispatch.
  Purge_Input_Sources ();
  Purge_Dispatch_Queue ();

 // Wait for all executing commands to terminante.
  Wait_For_Previous_Cmds ();

 // Since Python is in control, we need to tell it to quit.
	if (Embedded_WindowID == 0) 
	    PyRun_SimpleString( "myparse.Do_quit ()\n");

 // Force another "Exit" command through the input controler.
 // This is done, after setting the Python signal with the
 // previous "myparse.Do_quit" call, to trigger the actual
 // return fromPython. (Note: any command will do the job.)
  Shut_Down = true;

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Help (CommandObject *cmd) {
  cmd->P_Result()->dumpHelp(cmd);
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_History (CommandObject *cmd) {

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();
  int64_t num = OPENSS_HISTORY_DEFAULT;  // How many to print out

 // Copy commands from the history list.
  std::list<std::string>::iterator hi = History.begin();  // Start at  beginning

 // The user may ask us to limit the output
  vector<ParseRange> *c_limit = cmd->P_Result()->getHistoryList();
  if ((c_limit != NULL) &&
      (c_limit->begin() != c_limit->end())) {
    parse_range_t *c_range = c_limit->begin()->getRange();
    parse_val_t *c_val1 = &c_range->start_range;
    int64_t val1 = c_val1->num;
    if (val1 >= 0) {
     // Go with the user's limit.
      num = val1;
    }
   // Reset the limit for the user.
    if (val1 > OPENSS_HISTORY_LIMIT) {
      OPENSS_HISTORY_LIMIT = val1;
    }
  }

 // Skip until there are just enough left in the list.
  if (num < History_Count) {
    for (hi = History.end(); num >= 0; hi--, num--) {}
  }

 // Go through the rest of the list and echo them.
 // Skip the last one because it is this "history" command.
  for ( ; hi != History.end(); ) {
    std::string S = *hi;
    if (++hi != History.end()) {
     // Attach result to CommandObject.
      cmd->Result_String (S);
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Log (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool R = false;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();

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
/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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
    	std::string s("Can not retreive host name.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Playback (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
    std::string s("Can not determine file name.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  if ( !Push_Input_File (WindowID, f_val->name,
                         &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
    std::string s("Unable to open alternate command file " + f_val->name + ".");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();

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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
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

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
bool SS_Wait (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();

  if (Look_For_KeyWord (cmd, "terminate")) {
   // Look for a specified experiment
    Assert(cmd->P_Result() != NULL);
    EXPID ExperimentID = 0;

    if (cmd->P_Result()->isExpId()) {
      ExperimentID = cmd->P_Result()->getExpId();
    } else {
     // Get the Focused experiment - if it doesn't exist, return the default "0".
      ExperimentID = Experiment_Focus ( WindowID );
    }
    ExperimentObject *exp = NULL;
    if (ExperimentID != 0) {
     // Be sure the requested experiment exists.
      exp = Find_Specified_Experiment (cmd);
      if (exp == NULL) {
        return false;
      }
    } 

    if (exp != NULL) {
     // Wait for the execution of the experiment to terminate.
      Wait_For_Exp (cmd, exp);
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

//
// Dummy routines for commands not finished yet.
//

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
// No value returned to python.
bool SS_no_value (CommandObject *cmd) {

 // There is no result returned for this command.
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
// String value returned to python.
bool SS_string_value (CommandObject *cmd) {

  cmd->Result_String ("fake string");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

/**
 * Method: ()
 * 
 * .
 *     
 * @param   .
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
// Integer (64 bit) value returned to python..
bool SS_int_value (CommandObject *cmd) {

  cmd->Result_Int (-1);
  cmd->set_Status(CMD_COMPLETE);

  return true;
}


