/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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

/* Take the define below out of comments for debug
   output in the CLI routines that
   are present in this file.
#define DEBUG_STATUS 1
#define DEBUG_CLI 1
#define DEBUG_CLI_APPC 1
#define DEBUG_CLI_LOOPS 1
*/


#include "SS_Input_Manager.hxx"
#include "SS_Timings.hxx"

// Include the folllowing functions from SS_Configure.cxx
bool List_ConfigInfo(CommandObject *cmd);
bool set_ConfigValue(std::string name, std::string value);
bool set_ConfigValue(std::string name, int64_t value);
bool set_ConfigValue(std::string name, bool value);


#include "Python.h"

using namespace OpenSpeedShop::cli;

// Private Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
pthread_mutex_t Experiment_List_Lock = PTHREAD_MUTEX_INITIALIZER;
std::list<ExperimentObject *> ExperimentObject_list;
static std::string tmpdb = std::string("./ssdbtmpcmd.openss");

/**
 * Utility: Experiment_Termination ()
 * 
 * This routine is called when closing down Openss, to
 * gracefully terminate all definied experiments  by
 * freeing files and reclaiming memory space.
 *     
 * @param   none.
 *
 * @return  void
 *
 * @error   None reported, but thrown exceptions are caught and
 *          ignored so that we can do as good a job as possible.
 *
 */
// Global Termination Call -

// Terminate all experiments and frees the associated files.
// Called from the drivers to clean up after an "Exit" command or fatal error.
void Experiment_Termination () {
  (void) remove (tmpdb.c_str());
  ExperimentObject *exp = NULL;
  std::list<ExperimentObject *>::iterator expi;
  for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); ) {
    try {
      ExperimentObject *exp = (*expi);
      expi++;
      delete exp;
    }
    catch (const Exception& error) {
     // We are trying to terminate - ignore all the errors.
     // But stay in the loop and try to clean up other experiments.
    }
  }
}

// Experiment Utilities.

/**
 * Utility: int2str ()
 * 
 * Convert an int64_t value to a string.
 *     
 * @param   e - the int64_t value.
 *
 * @return  std::string the character representation of the value.
 *
 */
inline std::string int2str (int64_t e) {
  char s[40];
  sprintf ( s, "%lld", e);
  return std::string (s);
}

/**
 * Utility: str2int ()
 * 
 * Convert a string value into an int64_t.
 *     
 * @param   e - the int64_t value.
 *
 * @return  std::string the character representation of the value.
 *
 */
inline int64_t str2int (std::string s) {
  int64_t value = atoi(s.c_str()); 
  return value;
}

/**
 * Utility: Wait_For_Exp_State ()
 * 
 * Loop, looking for a specific execution state of an experiment
 * while periodically suspending execution and allowing other
 * processes to use the cpu resources so that the desired state
 * can be reached.
 *     
 * @param   cmd - the CommandObject that is being processed.
 * @param   to_state - the state that is being lloked for.
 * @param   exp - the experiment that is being checked.
 *
 * @return  int - the last value of the state when no longer waiting.
 *
 * @error   "NonExistent", "Terminated" and "error" states also stop
 *          the wait loop and are considered normal exit situations.
 *
 */
static int Wait_For_Exp_State (CommandObject *cmd, int to_state, ExperimentObject *exp) {
 // After changing the state of each thread, wait for the
 // status of the experiment to change.  This is necessary
 // because of the asynchronous nature of the FrameWork.
#ifdef DEBUG_CLI
  printf("Wait_For_Exp_State, enter to_state=%d\n", to_state);
#endif
  exp->Q_Lock (cmd, false);
  int latest = exp->Determine_Status();
  exp->Q_UnLock ();
  while ((latest != to_state) &&
         (latest != ExpStatus_NonExistent) &&
         (latest != ExpStatus_Terminated) &&
         (latest != ExpStatus_InError)) {
   // Check for asnychonous abort command 
    if ((cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED)) {
      break;
    }
#if 0
    usleep (100000);
#else
      struct timespec wait;
      wait.tv_sec = 0;
      wait.tv_nsec = 250 * 1000 * 1000;
      while(nanosleep(&wait, &wait));
#endif
    exp->Q_Lock (cmd, false);
    latest = exp->Determine_Status();
    exp->Q_UnLock ();
  }

#ifdef DEBUG_CLI
  printf("Wait_For_Exp_State, exit, latest=%d\n", latest);
#endif
  return latest;
}

/**
 * Utility: Wait_For_Thread_Connected ()
 * 
 * Loop, looking for other processes to complete the task of
 * connecting an experiment to a thread.
 *     
 * @param   cmd - the CommandObject being processed.
 * @param   t - the Thread we are waiting for.
 *
 * @return  void
 *
 * @error   send periodic messages to users to let them
 *          know that Openss is still alive, but waiting
 *          for an event to occure.
 *
 */
static void Wait_For_Thread_Connected (CommandObject *cmd, Thread t) {
 // Where do we send a message?
  InputLineObject *clip = cmd->Clip();
  CMDWID to_window = (clip != NULL) ? clip->Who() : 0;

  const uint64_t sleep_interval = 10000;
  uint64_t message_interval = sleep_interval * 500;
  uint64_t current_interval = 0;
  uint64_t message_count = 0;

  while (t.getState() == Thread::Connecting) {
   // Check for asnychonous abort command 
    if ((cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED)) {
      break;
    }
    if ((to_window > 0) && (message_interval <= current_interval)) {
      std::ostringstream S(std::ios::out);
      S << "Trying to connect to thread "
        << t.getProcessId();
      Send_Message_To_Window ( to_window, S.str());
      message_count++;
      current_interval = 0;
    }
    usleep (sleep_interval);
    current_interval += sleep_interval;
  }
}

// This routine is strongly based (copy of) 
// on the Tokenizer routine found at this URL:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html

void createTokens(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);

#ifdef DEBUG_CLI
        std::cerr << "createTokens, in while, str.c_str()=" << str.c_str()
	    << " , lastPos = " << lastPos << ", pos = " << pos << std::endl;
#endif

    }
}

static int getBEcountFromCommand(std::string command) {

  int retval = 1;
#ifdef DEBUG_CLI
  std::cerr << "Enter getBEcountFromCommand, command=" << command
	<< " retval=" << retval << std::endl;
#endif

  std::vector<std::string> tokens;
  createTokens(command, tokens );
  std::vector<std::string>::iterator k;
  std::string S = "";

  bool found_be_count = false;

  for (k=tokens.begin();k != tokens.end(); k++) {
#ifdef DEBUG_CLI
    std::cerr << "In getBEcountFromCommand, token=" << *k << std::endl;
#endif
    S = *k;
    if (found_be_count) {
      S = *k;
#ifdef DEBUG_CLI
      std::cerr << "In getBEcountFromCommand, found, -np, before calling "
	<< "st2int, S.c_str()=" << S << std::endl;
#endif
      retval = str2int( S );
#ifdef DEBUG_CLI
      std::cerr << "In getBEcountFromCommand, found, -np, retval="
	<< retval << std::endl;
#endif
      break;
    } else if (!strcmp( S.c_str(), std::string("-np").c_str())) {
      found_be_count = true;
#ifdef DEBUG_CLI
      std::cerr << "In getBEcountFromCommand, found, -np, S="
	<< S << std::endl;
#endif
    } else if (!strcmp(S.c_str(), std::string("-n").c_str())) {
      found_be_count = true;
#ifdef DEBUG_CLI
      std::cerr << "In getBEcountFromCommand, found, -n, S=" << S << std::endl;
#endif
    }
  }


  return retval;
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
static ExperimentObject *Find_Specified_Experiment (CommandObject *cmd, bool putOutWarnings=true) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

#ifdef DEBUG_CLI
   printf("Enter Find_Specified_Experiment, clip=%d\n", clip);
#endif
 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : 0;
  ExperimentObject *exp = NULL;

#ifdef DEBUG_CLI
   printf("In Find_Specified_Experiment, ExperimentID=%d\n", ExperimentID);
#endif

 // If not experiment has been specified, pick up the
 // "focused" experiment associated witht he input window.
  if (ExperimentID == 0) {

#ifdef DEBUG_CLI
   printf("In Find_Specified_Experiment, before calling Experiment_Focus, ExperimentID=%d\n", ExperimentID);
#endif

    ExperimentID = Experiment_Focus ( WindowID );

#ifdef DEBUG_CLI
   printf("In Find_Specified_Experiment, after calling Experiment_Focus, ExperimentID=%d\n", ExperimentID);
#endif

    if (ExperimentID == 0 && putOutWarnings) {
      Mark_Cmd_With_Soft_Error(cmd, "There is no focused experiment.");
      return NULL;
    }
  }
  exp = Find_Experiment_Object (ExperimentID);

#ifdef DEBUG_CLI
   printf("In Find_Specified_Experiment, after calling Find_Experiment_Object, ExperimentID=%d\n", ExperimentID);
#endif


  if (exp == NULL && putOutWarnings) {
    Mark_Cmd_With_Soft_Error(cmd, "The requested experiment ID does not exist.");
    return NULL;
  }

 // Is there an FrameWork Experiment to look at?
  if (exp && exp->FW() == NULL && putOutWarnings) {
    Mark_Cmd_With_Soft_Error(cmd, "The requested FrameWork experiment does not exist.");
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
  std::vector<ParseTarget> *p_tlist = (p_result != NULL) ? p_result->getTargetList() : NULL;
  if (p_tlist == NULL) {
    return NULL;
  }
  if (p_tlist->begin() == p_tlist->end()) {
    return NULL;
  }
  std::vector<ParseTarget>::iterator pi = p_tlist->begin();
  std::vector<ParseRange> *f_list = (*pi).getFileList();
  if (f_list == NULL) {
    return NULL;
  }
  if (f_list->begin() == f_list->end()) {
    return NULL;
  }
  std::vector<ParseRange>::iterator fi = f_list->begin();
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
  std::vector<std::string> *p_slist = cmd->P_Result()->getModifierList();
  std::vector<std::string>::iterator j;

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
 * Utility: Filter_Uses_F()
 * 
 * Determine if any of the <target_list> specifiers
 * make use of the optional -f' field.
 *     
 * @param CommandObject *cmd to access the parse objects.
 *
 * @return bool - true if there is a '-f' field, false otherwise.
 *
 */
bool Filter_Uses_F (CommandObject *cmd) {
#if DEBUG_CLI
  printf("Enter Filter_Uses_F\n");
#endif
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();

 // For each set of filter specifications ...
  std::vector<ParseTarget>::iterator pi;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *f_list = pt.getFileList();
    if ((f_list != NULL) && !f_list->empty()) {
#if DEBUG_CLI
      printf("Exit Filter_Uses_F return true\n");
#endif
      return true;
    }
  }

#if DEBUG_CLI
  printf("Exit Filter_Uses_F return false\n");
#endif
  return false;
}

/**
 * Utility: Filter_ThreadGroup()
 * 
 * Scan ths <target_list> specifier for a command and
 * determine which of the original set of threads are
 * begin requested.
 *
 * A thread is selected if one of the itees in each  of
 * the specified -h -p -r -t lists matchs the similar
 * information for the thread for one of the <target>
 * specifiers in the given <target_list>. The -f option
 * is ignored.
 *
 * Output is done by overwriting the input argument.
 *     
 * @param ParseResult *p_result points to the parse object.
 *        ThreadGroup& tgrp  - the original set of threads.
 *
 * @return  void, but one of the input arguments is altered.
 *
 */
void Filter_ThreadGroup (OpenSpeedShop::cli::ParseResult *p_result, ThreadGroup& tgrp) {
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->begin() == p_tlist->end()) {
   // There are no filters.
#if DEBUG_CLI
    printf("Exit Filter_ThreadGroup return false\n");
#endif
    return;
  }

  Thread firstThread(*tgrp.begin());
  try {
    firstThread.lockDatabase();
    ThreadGroup dgrp;
    ThreadGroup rgrp;

   // Go through every thread and decide if it is included.
    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {

      Thread t = *ti;
      std::string hid = t.getHost();
      pid_t pid = t.getProcessId();
      std::pair<bool, int> pthread = t.getOpenMPThreadId();
      bool threadHasThreadId = false;
      int64_t pthreadid = -1;
      if (pthread.first) {
        threadHasThreadId = true;
        pthreadid = pthread.second;
      } else {
        std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
        if (posixthread.first) {
          threadHasThreadId = true;
          pthreadid = posixthread.second;
        }
      }
      std::pair<bool, int> prank = t.getMPIRank();
      int64_t rank = prank.first ? prank.second : -1;

     // For each set of filter specifications ...
      std::vector<ParseTarget>::iterator pi;
      for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
        bool include_thread = true;

        ParseTarget pt = *pi;
        std::vector<ParseRange> *h_list = pt.getHostList();
        std::vector<ParseRange> *f_list = pt.getFileList();
        std::vector<ParseRange> *p_list = pt.getPidList();
        std::vector<ParseRange> *t_list = pt.getThreadList();
        std::vector<ParseRange> *r_list = pt.getRankList();

        bool has_h = !((h_list == NULL) || h_list->empty());
        bool has_f = !((f_list == NULL) || f_list->empty());
        bool has_p = !((p_list == NULL) || p_list->empty());
        bool has_t = !((t_list == NULL) || t_list->empty());
        bool has_r = !((r_list == NULL) || r_list->empty());

        if (include_thread && has_r && prank.first) {
         // Does it match a rank ID?
          bool within_list = false;

          std::vector<ParseRange>::iterator pr_iter;
          for (pr_iter=r_list->begin();pr_iter != r_list->end(); pr_iter++) {
            if (within_range(rank, *pr_iter->getRange())) {
              within_list = true;
              break;
            }
          }
          include_thread = within_list;
        }

        if (include_thread && has_t && threadHasThreadId) {
         // Does it match a pthread ID?
          bool within_list = false;

          std::vector<ParseRange>::iterator pr_iter;
          for (pr_iter=t_list->begin();pr_iter != t_list->end(); pr_iter++) {
            if (within_range(pthreadid, *pr_iter->getRange())) {
              within_list = true;
              break;
            }
          }
          include_thread = within_list;
        }

        if (include_thread && has_p) {
         // Does it match a pid?
          bool within_list = false;

          std::vector<ParseRange>::iterator pr_iter;
          for (pr_iter=p_list->begin();pr_iter != p_list->end(); pr_iter++) {
            if (within_range(pid, *pr_iter->getRange())) {
              within_list = true;
              break;
            }
          }
          include_thread = within_list;
        }

        if (include_thread && has_h) {
         // Does it match a host?
          bool within_list = false;
          std::vector<ParseRange>::iterator pr_iter;
          for (pr_iter=h_list->begin();pr_iter != h_list->end(); pr_iter++) {
            if (within_range(hid, *pr_iter->getRange())) {
              within_list = true;
              break;
            }
          }
          include_thread = within_list;
        }


       // Add matching threads to rgrp.
        if (include_thread) {
          rgrp.insert(t);
          break;
        }
      }

    }

   // Copy selected threads to output argument.
    tgrp.clear();
    tgrp = rgrp;
  }
  catch(const Exception& error) {
    tgrp.clear();
  }
  firstThread.unlockDatabase();
}

/**
 * Utility: Parse_Interval_Specification()
 *
 * Look for a "-I" specification and determine the actual
 * 'Time' values that specify the beginning and ending
 * values of each range.
 *
 * The framework will include all the samples between the
 * start and end of the given range.  It will also include
 * any samples collected at exactly the start time and will
 * exclude any samples taken at the end time.
 *
 * To provide a way to ge the last data sample included,
 * the end time is adjusted (here) if the user specifies
 * an end of 100% or gives the exact end time.
 *     
 * @param  CommandObject *
 * @param  ExperimentObject *
 * @param  std::vector<std::pair<Time,Time> >&
 *
 * @return  bool to indicate success(true) or failure(false),
 *          also the resulting ranges are placed in the last param.
 *
 * @errors  out-of-range errors cause the range be redefined
 *          as first-to-last, thus it is possible to use the
 *          result vector to generate a report although it may
 *          not reflect the user's intent.
 *
 */
bool Parse_Interval_Specification (
        CommandObject *cmd,
        ExperimentObject *exp,
        std::vector<std::pair<Time,Time> >& intervals) {
  Assert (exp != NULL);
  Assert (exp->FW() != NULL);
#if DEBUG_CLI
    std::cerr << "Enter Parse_Interval_Specification" << std::endl;
#endif
  OpenSpeedShop::cli::ParseResult *parse_result = cmd->P_Result();
  std::vector<ParseInterval> *I_list = parse_result->getParseIntervalList();

  if (I_list->empty()) {
    intervals.push_back(std::make_pair<Time,Time>(Time::TheBeginning(),Time::TheEnd()));
#if DEBUG_CLI
    std::cerr << "Exit TRUE Parse_Interval_Specification I_list is empty" << std::endl;
#endif
    return true;
  }

  enum { percent, s, ms, us, ns } units = ms;
  if (parse_result->isIntervalAttribute()) {
    const std::string *unit_string = parse_result->getIntervalAttribute();
    if (!strcasecmp(unit_string->c_str(), "%")) {
      units = percent;
    } else if (!strcasecmp(unit_string->c_str(), "s")) {
      units = s;
    } else if (!strcasecmp(unit_string->c_str(), "ms")) {
      units = ms;
    } else if (!strcasecmp(unit_string->c_str(), "us")) {
      units = us;
    } else if (!strcasecmp(unit_string->c_str(), "ns")) {
      units = ns;
    } else {
      std::ostringstream S("The attribute for the specified range '");
      S << unit_string << "'  is not valid.";
      Mark_Cmd_With_Soft_Error(cmd, S.str());
      intervals.push_back(std::make_pair<Time,Time>(Time::TheBeginning(),Time::TheEnd()));
#if DEBUG_CLI
    std::cerr << "Exit TRUE Parse_Interval_Specification invalid attribute" << std::endl;
#endif
      return true;
    }
  }

  Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
  Time first_time = databaseExtent.getTimeInterval().getBegin();
  Time last_time = databaseExtent.getTimeInterval().getEnd();

  for (std::vector<ParseInterval>::iterator
          pi = I_list->begin(); pi != I_list->end(); pi++) {
    ParseInterval P = *pi;
    Time Istart = first_time;
    Time Iend = last_time;

    if (units == percent) {
      double span = last_time - first_time;
      double fstart = 0.0;
      double fend = 1.0;
      if (P.isStartInt()) {
        fstart = P.getStartInt();
      } else {
        fstart = P.getStartdouble();
      }
      if (P.isEndInt()) {
        fend = P.getEndInt();
      } else {
        fend = P.getEndDouble();
      }
      Istart = first_time + static_cast<int64_t>((span * fstart) / 100.0);
      if (fend == 100.0) {
       // The end point is critical and must be exact. 
       // We can not tolerate the inaccuracies of floating point computation.
        Iend = last_time;
      } else {
        Iend = first_time + static_cast<int64_t>((span * fend) / 100.0);
      }
    } else {
      if (P.isStartInt()) {
        int64_t offset = P.getStartInt();
        switch (units) {
         case s: offset *= 1000;
         case ms: offset *= 1000;
         case us: offset *= 1000;
        }
        Istart += offset;
      } else {
        double fstart = P.getStartdouble();
        switch (units) {
         case s: fstart *= 1000.0;
         case ms: fstart *= 1000.0;
         case us: fstart *= 1000.0;
        }
        Istart += static_cast<int64_t>(fstart);
      }
      if (P.isEndInt()) {
        int64_t offset = P.getEndInt();
        switch (units) {
         case s: offset *= 1000;
         case ms: offset *= 1000;
         case us: offset *= 1000;
        }
        Iend = first_time + offset;
      } else {
        double fend = P.getEndDouble();
        switch (units) {
         case s: fend *= 1000.0;
         case ms: fend *= 1000.0;
         case us: fend *= 1000.0;
        }
        Iend  = first_time + static_cast<int64_t>(fend);
      }
    }

    if (Istart >= Iend) {
      std::ostringstream S;
      S << "The start time, " << Istart << ", is greater than the end time, " << Iend << ".";
      Mark_Cmd_With_Soft_Error(cmd, S.str());
      intervals.clear();
      intervals.push_back(std::make_pair<Time,Time>(Time::TheBeginning(),Time::TheEnd()));
#if DEBUG_CLI
    std::cerr << "Exit TRUE Parse_Interval_Specification start time is greater than end time" << std::endl;
#endif
      return true;
    }

#if DEBUG_CLI
    std::cerr << "In	Parse_Interval_Specification, Istart= " << Istart << std::endl;
    std::cerr << "In	Parse_Interval_Specification, Iend= " << Iend << "last_time=" << last_time << std::endl;
    std::cerr << "EXIT Parse_Interval_Specification, Istart= " << Istart << std::endl;
    std::cerr << "EXIT Parse_Interval_Specification, Iend= " << Iend << "last_time=" << last_time << std::endl;
#endif

    intervals.push_back(std::make_pair(Istart, ((Iend == last_time) ? Iend + 1 :  Iend)));
//    intervals.push_back(std::make_pair<Time,Time>(Istart,
//                                                  ((Iend == last_time) ? Iend + 1 :  Iend)));
  }

  return true;
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

  std::vector<ParseRange> *r_list = pt.getRankList();

 // Okay. Process the rank specification.
  std::vector<ParseRange>::iterator r_iter;

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
//        Thread t = exp->FW()->attachPosixThread(mypid, myrank, host_name);
//        tgrp->insert(t);
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

  std::vector<ParseRange> *t_list = pt.getThreadList();

 // Okay. Process the thread specification.
  std::vector<ParseRange>::iterator t_iter;

  for (t_iter=t_list->begin();t_iter != t_list->end(); t_iter++) {
    parse_range_t *t_range = t_iter->getRange();
    parse_val_t *t_val1 = &t_range->start_range;
    parse_val_t *t_val2 = t_val1;
    if (t_range->is_range) {
      t_val2 = &t_range->end_range;
    }

    uint64_t mythread;
    ThreadGroup original_threads = exp->FW()->getThreads();

    for ( mythread = t_val1->num; mythread <= t_val2->num; mythread++) {
      try {

        // Loop through thread group original_threads and check if any of the
	// threads specified in the thread list (t_val1 thru t_val2) exist.
        // If we find a match, add the thread to the thread group tgrp.

        for(ThreadGroup::const_iterator
            i = original_threads.begin(); i != original_threads.end(); ++i) {
	  std::pair<bool, pthread_t> posixthread = (*i).getPosixThreadId();
	  if (posixthread.first && mythread == posixthread.second) {
            tgrp->insert(*i);
	  }
        }

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
// Look through the parse results objects and figure out the pid specification.
// We do not need to worry about file specifiers.
// We also know that there is not both a thread and a rank specifier.
static void Resolve_P_Target (CommandObject *cmd, ExperimentObject *exp, ThreadGroup *tgrp,
                              ParseTarget pt, std::string host_name) {

  std::vector<ParseRange> *p_list = pt.getPidList();
  std::vector<ParseRange> *t_list = pt.getThreadList();
  std::vector<ParseRange> *r_list = pt.getRankList();

  bool has_t = !((t_list == NULL) || t_list->empty());
  bool has_r = !((r_list == NULL) || r_list->empty());

 // Okay. Process the pid specification.
  bool Mpi_KeyWord = Look_For_KeyWord (cmd, "mpi");
  std::vector<ParseRange>::iterator p_iter;

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

  std::vector<ParseRange> *f_list = pt.getFileList();

 // Okay. Process the file specification.
  std::vector<ParseRange>::iterator f_iter;

  for (f_iter=f_list->begin();f_iter != f_list->end(); f_iter++) {
    parse_range_t *f_range = f_iter->getRange();
    parse_val_t *f_val1 = &f_range->start_range;
    if (f_range->is_range) {
      parse_val_t *f_val2 = &f_range->end_range;
// TODO:
    } else {
      try {

#if BUILD_CLI_TIMING
        // Start to gather performance information on the sub-task portions of expCreate
        // Here the processing of creating the process in the Framework
        if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && 
            cli_timing_handle->in_expCreate()) {
          cli_timing_handle->cli_perf_data[SS_Timings::expCreate_FW_createProcess_Start] = Time::Now();
        }
#endif

#if BUILD_CLI_TIMING
        // Start to gather performance information on the sub-task portions of
        // expAttach. Here the processing of creating the process in the
        // Framework.
        if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled()  && 
            cli_timing_handle->in_expAttach()) {
          cli_timing_handle->cli_perf_data[SS_Timings::expAttach_FW_createProcess_Start] = Time::Now();
        }
#endif

        // offline changes: don't create a process for offline, we are executing
        // this outside of the cli/framework infrastructure see expGo
        // (actually Execute_Experiment for details of how we are executing
        // under offline)

        bool offlineInstrumentor = exp->getIsInstrumentorOffline();

#ifdef DEBUG_CLI
        std::cerr << "In Resolve_F_Target, local value of offlineInstrumentor=" << offlineInstrumentor << "\n";
        std::cerr << "In Resolve_F_Target, f_val1->name=" << f_val1->name << "\n" << std::endl;
#endif


	if (offlineInstrumentor) {
#ifdef DEBUG_CLI
          std::cerr << "In Resolve_F_Target, calling setOfflineAppCommand, offline case, f_val1->name=" << f_val1->name << "\n";
          std::cerr << "In Resolve_F_Target, calling setOfflineAppCommand, offline case, host_name=" << host_name << "\n";
#endif
          int numBE = getBEcountFromCommand(f_val1->name);
          exp->FW()->setBEprocCount( numBE );
          ThreadGroup tg = exp->FW()->createProcess( f_val1->name, host_name, numBE, 
                                                     OutputCallback(&ReDirect_User_Stdout,(void *)w), 
                                                     OutputCallback(&ReDirect_User_Stderr,(void *)w)   );
          tgrp->insert(tg.begin(), tg.end());
          exp->setOfflineAppCommand(f_val1->name.c_str());

        } else {

	    // FIXME???:  This is not the offline path!
#ifdef DEBUG_CLI
            std::cerr << "In Resolve_F_Target, calling setOfflineAppCommand, offline case, f_val1->name=" << f_val1->name << "\n";
            std::cerr << "In Resolve_F_Target, calling setOfflineAppCommand, offline case, host_name=" << host_name << "\n";
#endif

            exp->setOfflineAppCommand(f_val1->name.c_str());
        }

#if BUILD_CLI_TIMING
        if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
          cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_FW_createProcess_Start,
                                                    SS_Timings::expCreate_FW_createProcess_Count,
                                                    SS_Timings::expCreate_FW_createProcess_Max,
                                                    SS_Timings::expCreate_FW_createProcess_Min,
                                                    SS_Timings::expCreate_FW_createProcess_Total,
                                                    SS_Timings::expCreate_FW_createProcess_End);
        }
#endif

#if BUILD_CLI_TIMING
        if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled()  && cli_timing_handle->in_expAttach()) {
          cli_timing_handle->processTimingEventEnd( SS_Timings::expAttach_FW_createProcess_Start,
                                                    SS_Timings::expAttach_FW_createProcess_Count,
                                                    SS_Timings::expAttach_FW_createProcess_Max,
                                                    SS_Timings::expAttach_FW_createProcess_Min,
                                                    SS_Timings::expAttach_FW_createProcess_Total,
                                                    SS_Timings::expAttach_FW_createProcess_End);
        }
#endif


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

  std::vector<ParseRange> *h_list = pt.getHostList();
  std::vector<ParseRange> *f_list = pt.getFileList();
  std::vector<ParseRange> *p_list = pt.getPidList();
  std::vector<ParseRange> *t_list = pt.getThreadList();
  std::vector<ParseRange> *r_list = pt.getRankList();

  bool has_h = !((h_list == NULL) || h_list->empty());
  bool has_f = !((f_list == NULL) || f_list->empty());
  bool has_p = !((p_list == NULL) || p_list->empty());
  bool has_t = !((t_list == NULL) || t_list->empty());
  bool has_r = !((r_list == NULL) || r_list->empty());

  if (!has_h) {
    char HostName[MAXHOSTNAMELEN+1];
    if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
      Mark_Cmd_With_Soft_Error(cmd, "Can not retrieve host name.");
      return;
    }
    //pt.pushHostPoint (HostName[0]);
    //h_list = pt.getHostList();
    h_list->push_back (ParseRange(&HostName[0]));
    has_h = true;
  }

 // Semantic check for illegal combinations.
  if ( has_f && (has_p || has_t || has_r) ) {
    Mark_Cmd_With_Soft_Error(cmd, "The -f option can not be used with -p -t or -r options.");
    return;
  }
  if ( has_t && has_r ) {
    Mark_Cmd_With_Soft_Error(cmd, "The -t option can not be used with the -r option.");
    return;
  }

 // Okay. Process the host specification.
  std::vector<ParseRange>::iterator h_iter;

  for (h_iter=h_list->begin();h_iter != h_list->end(); h_iter++) {
    parse_range_t *h_range = h_iter->getRange();
    parse_val_t *h_val1 = &h_range->start_range;
    if (h_range->is_range) {
      parse_val_t *h_val2 = &h_range->end_range;
    } else {
      // TODO:
    }
    if (has_f) {
      Resolve_F_Target ( cmd, exp, tgrp, pt, h_val1->name);
    } else if (has_p) {
      Resolve_P_Target ( cmd, exp, tgrp, pt, h_val1->name);
    } else if (has_r) {
      Resolve_R_Target ( cmd, exp, tgrp, pt, h_val1->name, -1);
    } else if (has_t) {
      Resolve_T_Target ( cmd, exp, tgrp, pt, h_val1->name, -1);
    } else {
      // TODO:
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
static void Resolve_Target_List (CommandObject *cmd, ExperimentObject *exp, ThreadGroup &tgrp) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->begin() != p_tlist->end()) {
    std::vector<ParseTarget>::iterator ti;
    for (ti = p_tlist->begin(); ti != p_tlist->end(); ti++) {
      try {
        Resolve_H_Target (cmd, exp, &tgrp, *ti);
      }
      catch(const Exception& error) {
        Mark_Cmd_With_Std_Error (cmd, error);
        tgrp.clear(); // return an empty ThreadGroup
        return;
      }
    }
  }
  return;
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
  std::vector<std::string> *p_slist = p_result->getExpList();

#ifdef DEBUG_CLI
  std::cerr << "Enter Process_expTypes" << "\n";
#endif

  // offline collection variable is based on the global setting which can be overridden locally (per experiment)
  bool offlineInstrumentor = exp->getIsInstrumentorOffline();

#ifdef DEBUG_CLI
  std::cerr << "Enter Process_expTypes, offlineInstrumentor=" << offlineInstrumentor << "\n";
#endif

 // Determine the specified (or implied) set of Collectors.
  CollectorGroup cgrp;

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the processing of getting the collector info
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_getCollector_Start] = Time::Now();
  }
#endif


#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the processing of getting the collector info
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expAttach() ) {
    cli_timing_handle->cli_perf_data[SS_Timings::expAttach_getCollector_Start] = Time::Now();
  }
#endif

  if (p_slist->begin() != p_slist->end()) {

#ifdef DEBUG_CLI
  std::cerr << "In Process_expTypes, in p_slist->begin() != p_slist->end() section" << "\n";
#endif

   // The command contains a list of collectors to use.
   // Be sure they are all linked to the experiment.
    std::vector<std::string>::iterator si;
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      try {

#ifdef DEBUG_CLI
  std::cerr << "In Process_expTypes, in p_slist->begin() != p_slist->end() section, before Get_Collector, (*si)=" 
       << (*si) << " offlineInstrumentor=" << offlineInstrumentor << "\n";
#endif

       if (offlineInstrumentor) {

         exp->offlineCollectorList.push_back((*si));

         size_t found;

         std::string tmpStr = (*si);
         found=(*si).find("mpi-");
         if (found!=std::string::npos) {
           tmpStr = "mpi";
         }
         found=(*si).find("mpit-");
         if (found!=std::string::npos) {
           tmpStr = "mpi";
         }
         found=(*si).find("mpiotf-");
         if (found!=std::string::npos) {
           tmpStr = "mpiotf";
         } 

#ifdef DEBUG_CLI
         std::cerr << "In Process_expTypes, in p_slist->begin() != p_slist->end() section, before Get_Collector, tmpStr="  << tmpStr <<  "\n";
#endif
         Collector C = Get_Collector (exp->FW(), tmpStr);
         cgrp.insert (C);
        } else {
         Collector C = Get_Collector (exp->FW(), *si);
         cgrp.insert (C);
        }

      }

      catch(const Exception& error) {

#ifdef DEBUG_CLI
        std::cerr << "In Process_expTypes, catch exception, Get_Collector" << "\n";
#endif

        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

  } else {

#ifdef DEBUG_CLI
        std::cerr << "In Process_expTypes, else of p_slist->begin() != p_slist->end() section" << "\n";
#endif

   // Use all the collectors that are already part of the experiment.
    try {
      cgrp = exp->FW()->getCollectors();
      exp->FW()->setIsInstrumentorOffline(offlineInstrumentor);
    }
    catch(const Exception& error) {

#ifdef DEBUG_CLI
      std::cerr << "In Process_expTypes, catch error, else of p_slist->begin() != p_slist->end() section" << "\n";
#endif

      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  }

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expCreate
  // Here the processing of getting the collector info
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_getCollector_Start,
                                              SS_Timings::expCreate_getCollector_Count,
                                              SS_Timings::expCreate_getCollector_Max,
                                              SS_Timings::expCreate_getCollector_Min,
                                              SS_Timings::expCreate_getCollector_Total,
                                              SS_Timings::expCreate_getCollector_End);
  }
#endif


#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expAttach
  // Here the processing of getting the collector info
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled()  && cli_timing_handle->in_expAttach()) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expAttach_getCollector_Start,
                                              SS_Timings::expAttach_getCollector_Count,
                                              SS_Timings::expAttach_getCollector_Max,
                                              SS_Timings::expAttach_getCollector_Min,
                                              SS_Timings::expAttach_getCollector_Total,
                                              SS_Timings::expAttach_getCollector_End);
  }
#endif

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the processing of resolving target list and attaching if mpi
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_resolveTargetList_Start] = Time::Now();
  }
#endif

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expAttach
  // Here the processing of resolving target list and attaching if mpi
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled()  && cli_timing_handle->in_expAttach()) {
    cli_timing_handle->cli_perf_data[SS_Timings::expAttach_resolveTargetList_Start] = Time::Now();
  }
#endif

 // Determine the specified (or implied) set of Threads.
  ThreadGroup tgrp;


#ifdef DEBUG_CLI
  std::cerr << "In Process_expTypes, calling Resolve_Target_List" << "\n";
#endif

  Resolve_Target_List (cmd, exp, tgrp);

#ifdef DEBUG_CLI
  std::cerr << "In Process_expTypes, early exit check, offlineInstrumentor=" << offlineInstrumentor << "\n";
#endif
  
  if (offlineInstrumentor) {
#ifdef DEBUG_CLI
   std::string appCommand = exp->FW()->getApplicationCommand();
   std::cerr << "In Process_expTypes, early exit check, getApplicationCommand == appCommand.c_str()=" << appCommand.c_str() << "\n" << std::endl;
#endif
   // Save the application command for use in Execute_Experiment
   // exp->FW()->setApplicationCommand(appCommand.c_str(), false /* trust_me */);

#ifdef DEBUG_CLI
     std::cerr << "EARLY EXIT Process_expTypes" << "\n";
#endif

     return true;
  }

  if (tgrp.empty()) {
   // Use the threads that are already part of the experiment.
    try {
      tgrp = exp->FW()->getThreads();

      if (Look_For_KeyWord (cmd, "mpi")) {
       // Do an 'attachMPIJob' for all known threads.
        ThreadGroup Expanded_tgrp;
        ThreadGroup::iterator ti;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          ThreadGroup ngrp = exp->FW()->attachMPIJob ((*ti).getProcessId(), (*ti).getHost());
          Expanded_tgrp.insert(ngrp.begin(),ngrp.end());
        }
        tgrp = Expanded_tgrp;
      }
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  }

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expCreate
  // Here the processing of resolving target list and attaching if mpi
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_resolveTargetList_Start,
                                              SS_Timings::expCreate_resolveTargetList_Count,
                                              SS_Timings::expCreate_resolveTargetList_Max,
                                              SS_Timings::expCreate_resolveTargetList_Min,
                                              SS_Timings::expCreate_resolveTargetList_Total,
                                              SS_Timings::expCreate_resolveTargetList_End);
  }
#endif

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expAttach
  // Here the processing of resolving target list and attaching if mpi
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expAttach() ) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expAttach_resolveTargetList_Start,
                                              SS_Timings::expAttach_resolveTargetList_Count,
                                              SS_Timings::expAttach_resolveTargetList_Max,
                                              SS_Timings::expAttach_resolveTargetList_Min,
                                              SS_Timings::expAttach_resolveTargetList_Total,
                                              SS_Timings::expAttach_resolveTargetList_End);
  }
#endif

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the processing of linking threads to the collectors
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_linkThreads_Start] = Time::Now();
  }
#endif

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expAttach
  // Here the processing of linking threads to the collectors
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expAttach() ) {
    cli_timing_handle->cli_perf_data[SS_Timings::expAttach_linkThreads_Start] = Time::Now();
  }
#endif

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
      bool skip_mpirun = false;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
	std::pair<bool, std::string> mpi_impl =(*ti).getMPIImplementation();
	if (mpi_impl.first && !skip_mpirun) {
	    // The first thread in an mpi job started via expcreate
	    // is the mpirun program (orterun,mpirun,ssrun,etc).
	    // This is intended to defer adding a collector to the mpirun
	    // program itself. Need to verify this with mpich attach.
	    skip_mpirun = true;
	    continue;
	}
        (*cmdfunc) (cmd, exp, *ti, c);
      }
    }
  }

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expCreate
  // Here the processing of linking threads to the collectors
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_linkThreads_Start,
                                              SS_Timings::expCreate_linkThreads_Count,
                                              SS_Timings::expCreate_linkThreads_Max,
                                              SS_Timings::expCreate_linkThreads_Min,
                                              SS_Timings::expCreate_linkThreads_Total,
                                              SS_Timings::expCreate_linkThreads_End);
  }
#endif

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expAttach
  // Here the processing of linking threads to the collectors
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expAttach()) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expAttach_linkThreads_Start,
                                              SS_Timings::expAttach_linkThreads_Count,
                                              SS_Timings::expAttach_linkThreads_Max,
                                              SS_Timings::expAttach_linkThreads_Min,
                                              SS_Timings::expAttach_linkThreads_Total,
                                              SS_Timings::expAttach_linkThreads_End);
  }
#endif

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

#ifdef DEBUG_CLI
  std::cerr << "Enter Destroy_Experiment, calling Cancle_Exp_Wait" << "\n";
#endif
 // Clean up the notice board.
  Cancle_Async_Notice (exp);
  Cancle_Exp_Wait     (exp);

  exp->Q_Lock (cmd, false);
  int latest = exp->Determine_Status();
  exp->Q_UnLock ();

  if (Kill_KeyWord &&
      (exp->FW() != NULL) &&
       ((exp->Status() == ExpStatus_NonExistent) ||
        (exp->Status() == ExpStatus_Paused) ||
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

//#ifdef CLONE_COMMAND

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
bool SS_expClone (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool cmd_executed = false;
  Framework::Experiment *output_experiment = NULL;
  Framework::Experiment *input_experiment = NULL;

 // Wait for all executing commands to terminante.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when an entry is
 // deleted from it.
  Wait_For_Previous_Cmds ();

  // Find the experiment associated with the input expId using the cmd
  ExperimentObject *input_exp = Find_Specified_Experiment (cmd);
  if (input_exp == NULL) {
    return false;
  }

  // Find the experiment id of the experiment to be cloned
  EXPID input_exp_id = input_exp->ExperimentObject_ID();

  input_experiment = input_exp->FW();

#ifdef DEBUG_CLI
  printf("SS_expClone, exp_id of input experiment is %d\n", input_exp_id);
#endif

  std::string Data_File_Name = input_experiment->getName();

#ifdef DEBUG_CLI
  printf("SS_expClone, Data_File_Name of input experiment is %s\n", Data_File_Name.c_str());
#endif

  std::string new_data_base_name = input_exp->createName(input_exp_id, false);

#ifdef DEBUG_CLI
  printf("SS_expClone, new_data_base_name for copyTo is %s\n", new_data_base_name.c_str());
#endif

  try {
     // Wait for previous comands to complete so that
     // the copy has all the requested information.
      Wait_For_Previous_Cmds ();

      input_exp->CopyDB (new_data_base_name);
    }
  catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
  }


 // There is no specified experiment.  Allocate a new Experiment with the copied DB.
  ExperimentObject *output_exp = new ExperimentObject ( new_data_base_name);
  if (output_exp->FW() == NULL) {
    Mark_Cmd_With_Soft_Error(cmd, "Unable to create a new experiment in the FrameWork.");
    return false;
  }

  output_experiment = output_exp->FW();
  EXPID output_exp_id = output_exp->ExperimentObject_ID();

  std::string renamed_new_data_base_name = input_exp->createName(output_exp_id, false);

#ifdef DEBUG_CLI
  printf("SS_expClone, renamed_new_data_base_name for copyTo is %s\n", renamed_new_data_base_name.c_str());
#endif

  try {
     // Wait for previous comands to complete so that
     // the copy has all the requested information.
      Wait_For_Previous_Cmds ();

      output_exp->RenameDB (renamed_new_data_base_name);
    }
  catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
  }


#ifdef DEBUG_CLI
  printf("SS_expClone, exp_id of new/output experiment is %d\n", output_exp_id);
#endif
  // Received a clone request for an already created experiment, 
  // Do preparation to clone the existing experiment and prepare
  // the new experiments thread state etc for running.

  output_experiment->prepareToRerun(
      OutputCallback(&ReDirect_User_Stdout, (void*)WindowID),
      OutputCallback(&ReDirect_User_Stderr, (void*)WindowID)
      );

 // When we allocate a new experiment, set the focus to point to it.
  (void)Experiment_Focus (WindowID, output_exp_id);

 // Annotate the command
  cmd->Result_Annotation ("[openss]: The newly cloned experiment identifier is:  -x ");

 // Return the EXPID for this command.
  cmd->Result_Int (output_exp_id);

  cmd->set_Status(CMD_COMPLETE);
  return cmd_executed;
}

//#endif

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

#if BUILD_CLI_TIMING
  // Gather performance information on the sub-task portions of expCreate
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_wait_Start] = Time::Now();
  }
#endif

 // Wait for all executing commands to terminate.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when a new entry
 // is added to it.
  Wait_For_Previous_Cmds ();
//  cmd->Result_String ("Creating the new experiment. Reading symbols and inserting instrumentation.\n" );
    std::cerr << "[openss]: Creating a new experiment: Inserting instrumentation and preparing the experiment to run." << std::endl;

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_wait_Start,
                                              SS_Timings::expCreate_wait_Count,
                                              SS_Timings::expCreate_wait_Max,
                                              SS_Timings::expCreate_wait_Min,
                                              SS_Timings::expCreate_wait_Total,
                                              SS_Timings::expCreate_wait_End);
  }
#endif

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the allocation of the Experiment experiment
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_allocExp_Start] = Time::Now();
  }
#endif

#if DEBUG_CLI
  std::cerr << "SS_expCreate, actualCLIStartMode=" <<  actualCLIStartMode << std::endl;
#endif

 // There is no specified experiment.  Allocate a new Experiment.
  ExperimentObject *exp = new ExperimentObject ();
  if (exp->FW() == NULL) {
    Mark_Cmd_With_Soft_Error(cmd, "Unable to create a new experiment in the FrameWork.");
    return false;
  }
  EXPID exp_id = exp->ExperimentObject_ID();

 // Look for the -i option, this is the instrumentor type.
 std::vector<std::string> *p_slist = cmd->P_Result()->getInstrumentor();
 std::vector<std::string>::iterator j;

 extern bool isOfflineCmd;
 isOfflineCmd = false;
 bool overriding_command_line_option = false; 

 for (j=p_slist->begin();j != p_slist->end(); j++) {
   std::string S = *j;

    // If the -i option is equal to the offline instrumentor type, then set a flag in 
    // the experiment object indicating this.  Other routines in the cli will look at this
    // flag and do special actions if offline is the instrumentor.
   if (!strcasecmp(S.c_str(),"offline")) {

      exp->setIsInstrumentorOffline(true);
      isOfflineCmd = true;
      overriding_command_line_option = true; 

#if DEBUG_CLI
      std::cerr << "SS_expCreate, FOUND OFFLINE INSTRUMENTOR INDICATION (-i offline) " <<  std::endl;
#endif

   } else if (!strcasecmp(S.c_str(),"online")) {

      exp->setIsInstrumentorOffline(false);
      isOfflineCmd = false;
      overriding_command_line_option = true; 
#if DEBUG_CLI
      std::cerr << "SS_expCreate, FOUND ONLINE INSTRUMENTOR INDICATION (-i online) " <<  std::endl;
#endif

   }


 }

 // If we didn't see the -i offline or -i online option then take a look at the command line option

 if (!overriding_command_line_option) {

    if (actualCLIStartMode == SM_Offline) {
      exp->setIsInstrumentorOffline(true);
#if DEBUG_CLI
      std::cerr << "SS_expCreate, SM_Offline, setting setIsInstrumentorOffline(true)" << " actualCLIStartMode=" <<  actualCLIStartMode << std::endl;
#endif
      isOfflineCmd = true;
    } else if (actualCLIStartMode == SM_Online || actualCLIStartMode == SM_Batch) {
      exp->setIsInstrumentorOffline(false);
#if DEBUG_CLI
      std::cerr << "SS_expCreate, SM_Online, setting setIsInstrumentorOffline(false)" << " actualCLIStartMode=" <<  actualCLIStartMode << std::endl;
#endif
      isOfflineCmd = false;
    } else {

      // now default to the command line option if -online or -offline was specified
#if DEBUG_CLI
      std::cerr << "SS_expCreate, default mode, using actualCLIStartMode=" <<  actualCLIStartMode << " to set global flag" << std::endl;
#endif
      // default to offline
      // FIXME - we could try to look at the online/offline setting in the ~USER/.qt/openspeedshoprc file
      exp->setIsInstrumentorOffline(true);
#if DEBUG_CLI
      std::cerr << "SS_expCreate, else, setting setIsInstrumentorOffline(true)" << " actualCLIStartMode=" <<  actualCLIStartMode << std::endl;
#endif
      isOfflineCmd = true;
   } 

 } 

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expCreate
  // Here the allocation of the Experiment experiment
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate() ) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_allocExp_Start,
                                              SS_Timings::expCreate_allocExp_Count,
                                              SS_Timings::expCreate_allocExp_Max,
                                              SS_Timings::expCreate_allocExp_Min,
                                              SS_Timings::expCreate_allocExp_Total,
                                              SS_Timings::expCreate_allocExp_End);
  }
#endif

 // See Process_expTypes for the code to gather performance information on the sub-task portions of expCreate
 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, true);

#if DEBUG_CLI
      std::cerr << "SS_expCreate, calling Process_expTypes" <<  std::endl;
#endif

 // Determine target and collectors and link them together.
  if (!Process_expTypes (cmd, exp, &Attach_Command )) {
   // Something went wrong - delete the experiment.
    exp->Q_UnLock ();
    delete exp;
    return false;
  }

#if BUILD_CLI_TIMING
  // Start to gather performance information on the sub-task portions of expCreate
  // Here the processing of focusing and bookkeeping work
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
    cli_timing_handle->cli_perf_data[SS_Timings::expCreate_focusExp_Start] = Time::Now();
  }
#endif

 // When we allocate a new experiment, set the focus to point to it.
  (void)Experiment_Focus (WindowID, exp_id);

 // Annotate the command
  cmd->Result_Annotation ("[openss]: The new focused experiment identifier is:  -x ");

 // Return the EXPID for this command.
  cmd->Result_Int (exp_id);
  cmd->set_Status(CMD_COMPLETE);
  exp->Q_UnLock ();

#if BUILD_CLI_TIMING
  // End the gathering of performance information on the sub-task portions of expCreate
  // Here the processing of focusing and bookkeeping work
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() && cli_timing_handle->in_expCreate()) {
    cli_timing_handle->processTimingEventEnd( SS_Timings::expCreate_focusExp_Start,
                                              SS_Timings::expCreate_focusExp_Count,
                                              SS_Timings::expCreate_focusExp_Max,
                                              SS_Timings::expCreate_focusExp_Min,
                                              SS_Timings::expCreate_focusExp_Total,
                                              SS_Timings::expCreate_focusExp_End);
  }
#endif

 // Let other comamnds get access to the experiment and new focus.
  SafeToDoNextCmd ();
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
#ifdef DEBUG_CLI
  std::cerr << "Enter Disable_Experiment, calling Cancle_Exp_Wait" << "\n";
#endif
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

#ifdef DEBUG_CLI
  std::cerr << "Enter Enable_Experiment, calling Cancle_Exp_Wait" << "\n";
#endif
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
  cmd->Result_Annotation ("[openss]: The current focused experiment is:  -x ");
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

  Framework::Experiment *experiment = NULL;

 // Get the current status of this experiment.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status();
  // Get the experiment class object
  experiment = exp->FW();
  exp->Q_UnLock ();

#ifdef DEBUG_CLI
  std::cerr << "Enter Execute_Experiment, exp->ExpStatus_Name() " << exp->ExpStatus_Name() << "\n";
  exp->Q_Lock (cmd, false);
  if (exp->FW() != NULL) {
    std::cerr << "Enter Execute_Experiment,  exp->FW()->getRerunCount() " << exp->FW()->getRerunCount() << "\n";
  }
  exp->Q_UnLock ();
#endif


  bool offlineInstrumentor = exp->getIsInstrumentorOffline();
  if (offlineInstrumentor) {

#ifdef DEBUG_CLI
      std::cerr << "In Execute_Experiment, offlineInstrumentor=" << offlineInstrumentor << "\n";
#endif

    exp->Q_Lock (cmd, false);
    std::string appCommand = exp->getOfflineAppCommand();

#ifdef DEBUG_CLI
    std::cerr << "In Execute_Experiment, offline, appCommand=" << appCommand << "\n";
#endif

#if 0
  // Get the experiment class object
//  experiment = exp->FW();

    CollectorGroup cgrp = exp->FW()->getCollectors();
    CollectorGroup::iterator ci;
    bool atleastone = false;
    std::string S ;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Metadata m = c.getMetadata();
      if (atleastone) {
        S = S + ",";
      } else {
//        cmd->Result_String ("  Previously Used Data Collectors:");
        S = S + "    ";
        atleastone = true;
      }
     S = S + m.getUniqueId();

    } // end for collector group

    if (atleastone) {

#ifdef DEBUG_CLI
       std::cerr << "In Execute_Experiment, offline, starting with S (collector)=" << S << "\n";
#endif

       size_t found;
       found=S.find_first_of(" ");
       while (found!=string::npos) {
         S.erase(found, 1);
#ifdef DEBUG_CLI
         std::cerr << "In Execute_Experiment, offline, checking for blanks, S (collector)=" << S << "\n";
#endif
         found=S.find_first_of(" ",found);
       }

#ifdef DEBUG_CLI
       std::cerr << "In Execute_Experiment, offline, S (collector)=" << S << "\n";
#endif
    } // end atleastone
#endif


    // Now let's check to see if we have a mpi type experiment and then
    // modify the collector to match what is necessary to pass to
    // RunOfflineExp or the CBTF version.
    std::string S ;
    S.clear();
    std::list<std::string>::iterator offc;
    bool firstTime = true;
    for (offc = exp->offlineCollectorList.begin();
	 offc != exp->offlineCollectorList.end(); offc++) {

        if (firstTime) {
          firstTime = false;
        } else {
          S = S + ",";
        }
#ifdef DEBUG_CLI
        std::cerr << "In Execute_Experiment, offline, "
	    << "exp->offlineCollectorList, (collector)=" << (*offc)
	    << std::endl;
#endif
        S = (*offc);

    }

//jeg 11-2-08    exp->Q_UnLock ();

    // Run the offline experirment via RunOfflineExp instead of using the MRNet instrumentor
    // This call is for the interactive offline experiment not the oss convenience calls    
    // CBTF: hope we know its cbtf here. CBTFRunOfflineExp is used.
    std::string runOfflineCmd;
    if ( exp->getInstrumentorUsesCBTF() ) {
     runOfflineCmd = "RunCBTFOfflineExp(program=\"" + appCommand
		     + "\", collector=\"" + S + "\")";
    } else {
     runOfflineCmd = "RunOfflineExp(program=\"" + appCommand
		     + "\", collector=\"" + S + "\")";
    }
#ifdef DEBUG_CLI
    std::cerr << "In Execute_Experiment, runOfflineCmd="
	<< runOfflineCmd.c_str() << std::endl;
    fflush(stderr);
#endif
    exp->FW()->setApplicationCommand(appCommand.c_str(), false /* trust_me */);

#ifdef DEBUG_CLI
    std::cerr << "In Execute_Experiment, offline, runOfflineCmd="
	<< runOfflineCmd << "\n" << std::endl ;
#endif


    PyRun_SimpleString(runOfflineCmd.c_str());

// move the unlock here - jeg 11-2-08    
    exp->Q_UnLock ();
//jeg 11-2-08    

    exp->Q_Lock (cmd, false);
    exp->setStatus(ExpStatus_Terminated);
    exp->Q_UnLock ();
    return true;

  } // end if offlineInstrumentor
  

  exp->Q_Lock (cmd, false);

  if (exp->FW() == NULL) {
    Mark_Cmd_With_Soft_Error(cmd,
                             "The experiment can not be run because "
                             "it is not atached to an application.");
    return false;
  }
  exp->Q_UnLock ();

#ifdef DEBUG_CLI
  std::cerr << "In Execute_Experiment, exp->Status()=" << exp->Status()
	<< std::endl;
#endif

  if (exp->Status() == ExpStatus_InError) {

   // Can not run if ExpStatus_InError
    std::string s("The experiment can not be run because it is in the "
    	    	    + exp->ExpStatus_Name() + " state.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;

  } else if (exp->Status() != ExpStatus_Paused &&
	     exp->FW()->getRerunCount() != -1) {

   // Received a run request of a non-paused process, try to rerun
   // Do preparation to rerun before falling into the code below which
   // sets up the thread state and issues the run/rerun.
   // FIXME - how to determine whether the paused state is from 1st time
   // run (then don't do the prepareToRerun) or an actual pause via the user.
   // If we are starting from a user pause, then we want to restart
   // so call prepareToRerun.

  std::string Data_File_Name = experiment->getName();

#ifdef DEBUG_CLI
  std::cerr << "Execute_Experiment, Data_File_Name of input experiment is "
	<< Data_File_Name.c_str();
#endif

      //
      // If the preference is set to save the data from the previous run
      // when doing a rerun on the same experiment, then copy the database file
      // to a new named file before rerunning the experiment.
      //
      if (OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE) {

        EXPID active_exp_id = exp->ExperimentObject_ID();
        
        exp->FW()->incrementRerunCount();
        std::string new_data_base_name =
		exp->createRerunNameFromCurrentName(active_exp_id, 
                                                    exp->FW()->getRerunCount(), 
                                                    Data_File_Name.c_str());
#ifdef DEBUG_CLI
        std::cerr << "Execute_Experiment, new_data_base_name of input "
	    << "experiment is " << new_data_base_name.c_str();
#endif
        exp->Q_Lock (cmd, false);
        exp->CopyDB (new_data_base_name);
        exp->Q_UnLock ();
      }
       

#ifdef DEBUG_CLI
    exp->Q_Lock (cmd, false);
    std::string appCommand = experiment->getApplicationCommand();
    if (appCommand.empty()) {
      std::cerr << "Execute_Experiment, appCommand for the experiment is EMPTY"
	<< std::endl;
    } else {
      std::cerr << "Execute_Experiment, appCommand for the experiment is "
	<< appCommand.c_str() << std::endl;
    } 
    exp->Q_UnLock ();
#endif

      exp->Q_Lock (cmd, false);
      InputLineObject* clip = cmd->Clip();
      CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
      experiment->prepareToRerun(
          OutputCallback(&ReDirect_User_Stdout, (void*)WindowID),
	  OutputCallback(&ReDirect_User_Stderr, (void*)WindowID)
	  );

      exp->setStatus(ExpStatus_Paused);
      exp->Q_UnLock ();

#ifdef DEBUG_CLI
      std::cerr << "Execute_Experiment, after prepareToRerun, "
	<< "exp->ExpStatus_Name() " << exp->ExpStatus_Name()
	<< std::endl;
#endif

  } 

  // After the first run of the application/executable we are in rerun mode
  // when an expgo is issued.  Check if this is the first time if it is 
  // put us into rerun mode by clearing the -1 initial setting to zero.
  // The -1 is set in the framework Experiment.cxx Experiment constructor.
  if (exp->FW()->getRerunCount() == -1) {
     exp->FW()->setRerunCount(0);
  }

  if ((exp->Status() == ExpStatus_NonExistent) ||
      (exp->Status() == ExpStatus_Paused) ||
      (exp->Status() == ExpStatus_Running)) {

   // Verify that there are threads.
    ThreadGroup tgrp;
    try {
      tgrp = exp->FW()->getThreads();
      if (tgrp.empty()) {
    	Mark_Cmd_With_Soft_Error(cmd, "There are no applications specified for the experiment.");
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
#ifdef DEBUG_CLI
          std::cerr << "In Execute_Experiment, after prepareToRerun, "
	    << "terminated or nonexistent clause, t.getState()= "
	    << t.getState() << std::endl;
#endif
          continue;
        }
        Mark_Cmd_With_Std_Error (cmd, error);
        return false;
      }
    }

   // After changing the state of each thread, wait for the
   // something to actually start executing.
    (void) Wait_For_Exp_State (cmd, ExpStatus_Running, exp);

#ifdef DEBUG_CLI
     std::cerr << "In Execute_Experiment, Embedded_WindowID= "
	<< Embedded_WindowID << std::endl;
#endif
   // Notify the user when the experiment has terminated.
    if (Embedded_WindowID == 0) {
      Request_Async_Notice_Of_Termination (cmd, exp);
    }

    std::string appCommand = exp->FW()->getApplicationCommand();

#ifdef DEBUG_CLI
     std::cerr << "In Execute_Experiment, appCommand= "
	<< appCommand << std::endl;
#endif

    // Protect against empty application command and then use default
    // execution message which does not contain the executable name
    if (appCommand.empty() ) {

      // Annotate the command
      cmd->Result_Annotation ("[openss]: Start asynchronous execution of experiment:  -x "
                            + int2str(exp->ExperimentObject_ID()) + "\n" );
    } else {

      // Annotate the command
      cmd->Result_Annotation ("[openss]: Start asynchronous execution of experiment:  -x "
                            + int2str(exp->ExperimentObject_ID()) + "\n" + "[openss]: Running executable: " + appCommand
                            + " for experiment -x " + int2str(exp->ExperimentObject_ID()) +  "\n");
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
static bool Continue_Experiment (CommandObject *cmd, ExperimentObject *exp) {
 // Get the current status of this experiment.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status();
  exp->Q_UnLock ();

  if (exp->FW() == NULL) {
    Mark_Cmd_With_Soft_Error(cmd,
                             "The experiment can not be run because "
                             "it is not atached to an application.");
    return false;
  }

  if ((exp->Status() == ExpStatus_Terminated) ||
      (exp->Status() == ExpStatus_InError)) {
   // Can not run if ExpStatus_Terminated or ExpStatus_InError
    std::string s("The experiment can not be continued because it is in the "
                    + exp->ExpStatus_Name() + " state.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  if ((exp->Status() == ExpStatus_NonExistent) ||
      (exp->Status() == ExpStatus_Paused) ||
      (exp->Status() == ExpStatus_Running)) {

   // Verify that there are threads.
    ThreadGroup tgrp;
    try {
      tgrp = exp->FW()->getThreads();
      if (tgrp.empty()) {
        Mark_Cmd_With_Soft_Error(cmd, "There are no applications specified for the experiment.");
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
    cmd->Result_Annotation ("[openss]: Continue asynchronous execution of experiment:  -x "
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
bool SS_expCont (CommandObject *cmd) {
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Continue_Experiment (cmd, exp)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Continue_Experiment (cmd, exp)) {
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
bool SS_expGo (CommandObject *cmd) {

  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::iterator expi;
    for (expi = ExperimentObject_list.begin(); expi != ExperimentObject_list.end(); expi++) {
      ExperimentObject *exp = *expi;
      if (!Execute_Experiment (cmd, exp)) {
        return false;
      } else {
//        exp->setExpRunAtLeastOnceAlready(true);
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!Execute_Experiment (cmd, exp)) {
      return false;
    } else {
//      exp->setExpRunAtLeastOnceAlready(true);
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
#ifdef DEBUG_CLI
  std::cerr << "Enter Pause_Experiment, calling Cancle_Exp_Wait" << "\n";
#endif
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
    cmd->Result_Annotation ("[openss]: Suspend execution of experiment:  -x "
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

static std::string eraseAll(
  std::string result, 
  const std::string& eraseWhat )
{
  while(1)
  {
    const int pos = result.find(eraseWhat);
    if (pos==-1) break;
    result.erase(pos,eraseWhat.size());
  }
  return result;
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

  ExperimentObject *exp = NULL;
#ifdef DEBUG_CLI
  std::cerr << "Enter SS_expRestore " << "\n";
#endif

  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

#ifdef DEBUG_CLI_APPC
  std::cerr << "Enter SS_expRestore before printing clip=" << clip << "\n";
  if (clip != NULL) {
    clip->Print(std::cerr);
  }
  std::cerr << "\nEnter SS_expRestore after printing clip=" << clip << "\n";
#endif

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    Mark_Cmd_With_Soft_Error(cmd, "A file name for the Database is required.");
    return false;
  }

#ifdef DEBUG_CLI_APPC
  std::cerr << "SS_expRestore application command=" << clip->Command() << "\n";
#endif

  std::string data_base_name = file_name_value->name;
#ifdef DEBUG_CLI
  std::cerr << "SS_expRestore data_base_name=" << data_base_name << "\n";
#endif

 // Wait for all executing commands to terminate.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when a new entry
 // is added to it.
  Wait_For_Previous_Cmds ();

#if 0
 // Create a new experiment and connect it to the saved data base.
  exp = new ExperimentObject (data_base_name);
  if ((exp == NULL) ||
      (exp->ExperimentObject_ID() <= 0)) {
    Mark_Cmd_With_Soft_Error(cmd, "The specified file name is not a legal data base.");
    return false;
  }

#else

#ifdef DEBUG_CLI_APPC
     std::cerr << "In SS_expRestore, after calling new ExperimentObject " << "\n";
#endif

  exp = Find_Specified_Experiment (cmd, false /* do not put out the no focus warning */);

  EXPID preferredEXP = 0;

#ifdef DEBUG_CLI_APPC
     std::cerr << "In SS_expRestore, before checking exp= " << exp << "\n";
#endif

  bool offlineInstrumentor = true;

  if (exp != NULL) {

     offlineInstrumentor = exp->getIsInstrumentorOffline();

#ifdef DEBUG_CLI_APPC
     std::cerr << "In SS_expRestore, offlineInstrumentor= " << offlineInstrumentor << "\n";
#endif

     if (offlineInstrumentor) {
       preferredEXP = exp->ExperimentObject_ID();
#ifdef DEBUG_CLI_APPC
       std::cerr << "In SS_expRestore, preferredEXP= " << preferredEXP << "\n";
#endif
     }
  }

#ifdef DEBUG_CLI_APPC
  std::cerr << "In SS_expRestore, calling new ExperimentObject with data_base_name=" << data_base_name << " preferredEXP=" << preferredEXP << "\n";
#endif

  exp = new ExperimentObject (data_base_name, preferredEXP);
  offlineInstrumentor = exp->getIsInstrumentorOffline();
#ifdef DEBUG_CLI_APPC
   std::cerr << "In SS_expRestore, before runOfflineCmd code, offlineInstrumentor=" <<  offlineInstrumentor 
             << " exp=" << exp << " preferredEXP=" << preferredEXP << "\n";
#endif

  //
  // Set the application command into the database for the oss convenience type offline 
  // invocation.  The interactive saving of the application command is done in Execute_Experiment
  //
  if (offlineInstrumentor && exp &&exp->FW() != NULL && clip != NULL) {
      std::string runOfflineCmd = clip->Command();
      if (runOfflineCmd.c_str() ) {
         std::string::size_type found = std::string::npos;
         found = runOfflineCmd.find("program=");
         if (found!=std::string::npos) {
           found = found+9;
           runOfflineCmd.erase(0,found);
#ifdef DEBUG_CLI_APPC
           std::cerr << "In SS_expRestore, 1st erase, runOfflineCmd=" << runOfflineCmd.c_str() << "\n";
#endif
           found = runOfflineCmd.find(" ,collector=");
           if (found!=std::string::npos) {
              runOfflineCmd.erase(found,runOfflineCmd.length() );
#ifdef DEBUG_CLI_APPC
              std::cerr << "In SS_expRestore, 2nd erase, runOfflineCmd=" << runOfflineCmd.c_str() << "\n";
#endif
           }
//           std::string findCmd = runOfflineCmd;
//           std::string findCmd = eraseAll(runOfflineCmd, "\"");
           runOfflineCmd.erase( remove( runOfflineCmd.begin(), runOfflineCmd.end(), '\"' ), runOfflineCmd.end());

#ifdef DEBUG_CLI_APPC
           std::cerr << "In SS_expRestore, runOfflineCmd=" << runOfflineCmd.c_str() << "\n";
           fflush(stderr);
#endif
           exp->FW()->setApplicationCommand(runOfflineCmd.c_str(), true /* trust_me */);
         } 
      }
  }
#endif


#ifdef DEBUG_CLI_APPC
  std::cerr << "In SS_expRestore, after calling new ExperimentObject with data_base_name=" << data_base_name << " preferredEXP=" << preferredEXP << " exp=" << exp << "\n";
  if (exp) {
    std::cerr << "In SS_expRestore, after calling new ExperimentObject with data_base_name=" << data_base_name << " preferredEXP=" << preferredEXP << " exp->ExperimentObject_ID()=" << exp->ExperimentObject_ID() << "\n";
  }
#endif

  if ((exp == NULL) ||
      (exp->ExperimentObject_ID() <= 0)) {
    Mark_Cmd_With_Soft_Error(cmd, "The specified file name is not a legal data base.");
    return false;
  }

 // Pick up the EXPID for an allocated experiment.
  EXPID ExperimentID = exp->ExperimentObject_ID();

#ifdef DEBUG_CLI_APPC
  std::cerr << "In SS_expRestore, after calling exp->ExperimentObject_ID(), ExperimentID=" << ExperimentID << " preferredEXP=" << preferredEXP << " exp=" << exp << "\n";
#endif

 // Set the focus to point to the new EXPID.
  (void)Experiment_Focus (WindowID, ExperimentID);

#ifdef DEBUG_CLI_APPC
  std::cerr << "In SS_expRestore, after calling Experiment_Focus() with ExperimentID=" << ExperimentID << " preferredEXP=" << preferredEXP << " exp=" << exp << "\n";
#endif


 // Annotate the command
  cmd->Result_Annotation ("[openss]: The restored experiment identifier is:  -x ");

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
  // Wait for previous comands to complete so that
  // the copy has all the requested information.
  Wait_For_Previous_Cmds ();

 // Extract the savefile name.
  parse_val_t *file_name_value = Get_Simple_File_Name (cmd);
  if (file_name_value == NULL) {
    Mark_Cmd_With_Soft_Error(cmd, "Need a file name for the Database.");
    return false;
  }

  std::string data_base_name = file_name_value->name;

 // Look at general modifier types for "copy" option.
  bool Copy_KeyWord = Look_For_KeyWord (cmd, "copy");

  if (Copy_KeyWord) {
    try {
      // Prevent this experiment from changing until we are done.
      exp->Q_Lock (cmd, false);
      exp->CopyDB (data_base_name);
      exp->Q_UnLock ();
    }
    catch(const Exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }
  } else {
    try {
      // Prevent this experiment from changing until we are done.
      exp->Q_Lock (cmd, false);
      exp->RenameDB (data_base_name);
      exp->Q_UnLock ();
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
 * Utility: setparam()
 * 
 * .
 *     
 * @param   .
 *
 * @return  bool
 *
 * @todo    Error handling.
 *
 */
static bool setparam (Collector C, std::string pname, std::vector<ParamVal> *value_list) {

 // Stop the collector so we can change the parameter value.
  ThreadGroup active;
  active = C.getThreads();
  active.postponeCollecting(C);

#ifdef DEBUG_CLI
  printf("Enter setparam(), pname==%s\n", pname.c_str() );
#endif

  std::set<Metadata> md = C.getParameters();
  std::set<Metadata>::const_iterator mi;

  for (mi = md.begin(); mi != md.end(); mi++) {

    Metadata m = *mi;
    if (m.getUniqueId() != pname) {
     // Not the one we want - keep looking.
      continue;
    }

    if ( m.isType(typeid(std::map<std::string, bool>)) ) {

     // Set strings in the value_list to true.
      std::map<std::string, bool> Value;
      C.getParameterValue(pname, Value);

     // Set all the booleans to true, if the corresponding name is in the list,
     // and false otherwise.

      for (std::map<std::string, bool>::iterator im = Value.begin(); im != Value.end(); im++) {

        bool name_in_list = false;
        for (std::vector<ParamVal>::iterator iv = value_list->begin(); iv != value_list->end(); iv++) {

          Assert (iv->getValType() == PARAM_VAL_STRING);

#ifdef DEBUG_CLI
          printf("In setparam(), im->first.c_str()=%s, iv->getSVal()=%s\n", im->first.c_str(), iv->getSVal() );
#endif

          if (!strcasecmp( im->first.c_str(), iv->getSVal() )) {
#ifdef DEBUG_CLI
            printf("In setparam(), SETTING NAME_IN_LIST TO TRUE, im->first.c_str()=%s, iv->getSVal()=%s\n", im->first.c_str(), iv->getSVal() );
#endif
            name_in_list = true;
            break;
          } else {
#ifdef DEBUG_CLI
            printf("In setparam(), NAME NOT IN LIST-------------->SETTING NAME_IN_LIST TO FALSE, im->first.c_str()=%s, iv->getSVal()=%s\n", 
                   im->first.c_str(), iv->getSVal() );
#endif
          }
        } // end inner loop that contains actual arguments sent to SS_expSetParam

#ifdef DEBUG_CLI
        printf("In setparam(), SETTING im->second to name_in_list=%d, im->first.c_str()=%s\n", name_in_list, im->first.c_str() );
#endif
        // set boolean representing whether or not the parameter in a list obtained from the collector matches the list sent to expSetParam
        im->second = name_in_list;
      } // end outer loop, which loops through the list of parameter items from the list obtained from the collector


      // swap loops to search for names not appearing in the getParamValue list

      for (std::vector<ParamVal>::iterator iv = value_list->begin(); iv != value_list->end(); iv++) {

        bool no_match_at_all = TRUE;
        for (std::map<std::string, bool>::iterator im = Value.begin(); im != Value.end(); im++) {
          if (!strcasecmp( im->first.c_str(), iv->getSVal() )) {
             no_match_at_all = FALSE;
          }
        } // end outer loop, which loops through the list of parameter items from the list obtained from the collector

        std::map<std::string, bool>::iterator im_end = Value.end();
        if (no_match_at_all) {
         Value.insert(std::make_pair(iv->getSVal(), TRUE));

#ifdef DEBUG_CLI
         printf("In setparam(), NAME NOT IN LIST--INSERTING NEW ITEM INTO VALUE---iv->getSVal()=%s\n", iv->getSVal() );
#endif

        }

      } // end inner loop that contains actual arguments sent to SS_expSetParam
    

      C.setParameterValue(pname,Value);

    } else {

#ifdef DEBUG_CLI
      printf("In setparam(), ELSE not  m.isType(typeid(std::map<std::string, bool>)) \n" );
#endif

      ParamVal pvalue = (*value_list)[0];

      if( m.isType(typeid(int)) ) {
        int ival;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%d", &ival);
        } else {
          ival = (int)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(int)ival);
      } else if( m.isType(typeid(int64_t)) ) {
        int64_t i64val;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%lld", &i64val);
        } else {
          i64val = (int64_t)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(int64_t)i64val);
      } else if( m.isType(typeid(uint)) ) {
        uint uval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%d", &uval);
        } else {
          uval = (uint)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(uint)uval);
      } else if( m.isType(typeid(uint64_t)) ) {
        uint64_t u64val;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%lld", &u64val);
        } else {
          u64val = (uint64_t)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(uint64_t)u64val);
      } else if( m.isType(typeid(float)) ) {
        float fval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%f", &fval);
        } else {
          fval = (float)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(float)fval);
      } else if( m.isType(typeid(double)) ) {
        double dval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
          sscanf ( pvalue.getSVal(), "%llf", &dval);
        } else {
          dval = (double)(pvalue.getIVal());
        }
        C.setParameterValue(pname,(double)dval);
      } else if( m.isType(typeid(std::string)) ) {
        std::string sval;
        if (pvalue.getValType() == PARAM_VAL_STRING) {
	  // The paramter values for hwcsamp are comma seperated lists.
	  // We need to preserve this list as a csv since the grammar
	  // uses the comma itself to distinguish params.
	  // It also seems to be that most parameters seem to allow
	  // only one value. The string param list for hwcsamp is
	  // different from the tracing collectors in that it does
	  // not map a string to a boolean.
	  for (std::vector<ParamVal>::iterator iv = value_list->begin();
		iv != value_list->end(); iv++) {
	    sval.append((*iv).getSVal());
	    if (iv + 1  != value_list->end() ) {
	       sval.append(","); 
	    }
	  }
        } else {
          char cval[20];
          sprintf( cval, "%d", pvalue.getIVal());
          sval = std::string(&cval[0]);
        }
#ifdef DEBUG_CLI
       printf("In setparam(), ELSE string, sval==%s\n", sval.c_str() );
#endif

        C.setParameterValue(pname,(std::string)sval);
      }
    }

   // There should only be one name that matches!
   // Once we find and set it, we are done.
   // Restart the collector with a different parameter value.
    active.startCollecting(C);
    return true;

  } // end main for loop

 // Restart the collector with the old parameter value.
  active.startCollecting(C);

 // We didn't find the named parameter in this collector.
  return false;
}

// Forward reference.
static void Most_Common_Executable (CommandObject *cmd, ExperimentObject *exp, bool returnFullPath, std::list<std::string>& ExList);

/**
 * SemanticRoutine: SS_expSetArgs()
 * 
 * Implement the 'expSetArgs' command by reading values form the
 * parse object and sending them to the collectors attached to an
 * experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful completion of the command.
 *
 * @error   "false is returned if no experiment can be determined or
 *          if a thrown exception is caught while accessing the
 *          experiment's database.
 *
 */
bool SS_expSetArgs (CommandObject *cmd) {

  std::string S;
  std::string newAppExecutableCommand;
  std::string maxSizeCommand;
  std::string::size_type matchPos = std::string::npos;
  std::string::size_type maxPos = std::string::npos;
  int maxExeSize;
  int newExecutablePathSize;
  int lastTokenStrSize;
  std::string lastToken;



  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

#if DEBUG_CLI
  printf("Enter SS_expSetArgs(), exp=%d\n", exp);
#endif

  Assert(cmd->P_Result() != NULL);
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();

#if DEBUG_CLI
  if (p_result != NULL) {
      p_result->ParseResult::dumpInfo();
  }
#endif

    std::vector<std::string> *p_slist = p_result->getStringList();
    std::vector<std::string>::iterator j;

    for (j=p_slist->begin();j != p_slist->end(); j++) {

       S = *j;

#ifdef DEBUG_CLI
       printf("In SS_expSetArgs(), S.c_str()=%s\n", S.c_str());
       std::cerr << *j << " " ;
#endif

    } // end for j

#ifdef DEBUG_CLI
    if (p_slist->begin() != p_slist->end())
        std::cerr << std::endl ;
#endif

  exp->Q_Lock (cmd, true);

  std::string appCommand = exp->FW()->getApplicationCommand();

  // only do the code if there is an application command (-f executable case).
  // -p < process > or -r < rank > will not have an applicaiton command

  if (!appCommand.empty()) {

#ifdef DEBUG_CLI
  printf("In SS_expSetArgs(), appCommand.c_str()=%s\n", appCommand.c_str());
#endif


  // Form new executable and arguments command to set into database via the 
  // framework routing setApplicationCommand.

  std::list<std::string> ExList;

  // Need to return the full path for this feature to work
  // The third argument specifies this

  Most_Common_Executable (cmd, exp, true, ExList);

  if (ExList.empty()) {
//    FIXME
//    this is an error condition
  } else {
    for (std::list<std::string>::iterator xi = ExList.begin(); xi != ExList.end(); xi++) {
      newAppExecutableCommand = *xi;

#ifdef DEBUG_CLI
      printf("In SS_expSetArgs(), newAppExecutableCommand.c_str()=%s\n", newAppExecutableCommand.c_str());
      printf("In SS_expSetArgs(), appCommand.c_str()=%s\n", appCommand.c_str());
#endif

      // Find the executable in the orginal command that this executable represents
      // and replace the reference in the original command with this full path reference
      // remember the position in the original command

      matchPos = appCommand.find(newAppExecutableCommand);

#ifdef DEBUG_CLI
      printf("In SS_expSetArgs(), INITIAL MATCHPOS CHECK, matchPos=%d\n", matchPos);
#endif

      if (matchPos == std::string::npos) {
        // no match for full path, look at finding non-path executable

        newExecutablePathSize = newAppExecutableCommand.length();

#ifdef DEBUG_CLI
        printf("In SS_expSetArgs(), INSIDE TOKENIZE NEEDED, newExecutablePathSize=%d\n", newExecutablePathSize);
#endif

        std::vector<std::string> tokens;
        createTokens(newAppExecutableCommand, tokens, "/");
        std::vector<std::string>::iterator k;

        for (k=tokens.begin();k != tokens.end(); k++) {
          lastToken = *k;
        }

        // after for loop lastToken should be the filename for this executable

#ifdef DEBUG_CLI
        printf("after the while loop, lastToken.c_str()=%s\n", lastToken.c_str());
        printf("after the while loop, lastToken.length()=%d\n", lastToken.length());
#endif
       
        // Let's locate that in the rebuilt string and replace it with
        // the full path version, if needed.  Otherwise, record it's position
        // and string for the final argument replacement.  We are looking for the
        // new executable command string that is closest to the argument's position
        // in the appCommand string.

        matchPos = appCommand.find(lastToken);

#ifdef DEBUG_CLI
        printf("after the while loop, lastToken.c_str()=%s, newAppExecutableCommand.c_str()=%s, matchPos=%d\n", lastToken.c_str(), appCommand.c_str(), matchPos);
#endif

        if (matchPos != std::string::npos) {

          lastTokenStrSize = lastToken.length();
          newExecutablePathSize = newAppExecutableCommand.length();

#ifdef DEBUG_CLI
          printf("In SS_expSetArgs(), found the new executable filename in the original command, lastToken.c_str()=%s newExecutablePathSize=%d\n",
                   lastToken.c_str(), newExecutablePathSize);
          printf("In SS_expSetArgs(), found the new executable filename in the original command, lastTokenStrSize=%d\n", lastTokenStrSize);
          printf("In SS_expSetArgs(), BEFORE REPLACEMENT, newAppExecutableCommand.c_str()=%s\n", newAppExecutableCommand.c_str());

          printf("In SS_expSetArgs(), BEFORE REPLACEMENT, appCommand.c_str()=%s\n", appCommand.c_str());
#endif

          appCommand.replace(matchPos, lastTokenStrSize, newAppExecutableCommand);


#ifdef DEBUG_CLI
          printf("In SS_expSetArgs(), AFTER REPLACEMENT, newAppExecutableCommand.c_str()=%s\n", newAppExecutableCommand.c_str());
          printf("In SS_expSetArgs(), AFTER REPLACEMENT, appCommand.c_str()=%s\n", appCommand.c_str());
          printf("In SS_expSetArgs(), BEFORE CASE CHECKS matchPos, maxPos=%d, matchPos=%d\n", maxPos, matchPos);
#endif

          // save the position farthest toward the arguments so we can clip off the 
          // old arguments and replace with the new
          if (maxPos == std::string::npos) {
            maxPos = matchPos;
            maxExeSize = newAppExecutableCommand.length();
            maxSizeCommand = newAppExecutableCommand;

#ifdef DEBUG_CLI
            printf("In SS_expSetArgs(), INITIAL SET CASE from matchPos, maxPos=%d, maxExeSize=%d\n", maxPos, maxExeSize);
#endif
          } else if (matchPos > maxPos) {
            maxPos = matchPos;
            maxExeSize = newAppExecutableCommand.length();
            maxSizeCommand = newAppExecutableCommand;
#ifdef DEBUG_CLI
            printf("In SS_expSetArgs(), REPLACEMENT CASE RESET from matchPos, maxPos=%d, maxExeSize=%d\n", maxPos, maxExeSize);
#endif
          }
        } 
        
      } else {

#ifdef DEBUG_CLI
          printf("In SS_expSetArgs(), INSIDE FULLPATH MATCHED, maxPos=%d, matchPos=%d, maxExeSize=%d\n", maxPos, matchPos, maxExeSize);
#endif

        // full path version found
        if (maxPos == std::string::npos) {
          maxPos = matchPos;
          maxExeSize = newAppExecutableCommand.length();
          maxSizeCommand = newAppExecutableCommand;

#ifdef DEBUG_CLI
          printf("In SS_expSetArgs(), NULL first setting of maxPos=%d, maxExeSize=%d\n", maxPos, maxExeSize);
          printf("In SS_expSetArgs(), NULL first setting of maxPos=%d, maxExeSize=%d, maxSizeCommand.c_str()=%s\n", maxPos, maxExeSize, maxSizeCommand.c_str());
#endif

        } else {

          // if this executable string is further right, record the position, length, and save it.
          if (matchPos > maxPos) {
             maxPos = matchPos;
             maxExeSize = newAppExecutableCommand.length();
             maxSizeCommand = newAppExecutableCommand;
#ifdef DEBUG_CLI
             printf("In SS_expSetArgs(), RESET from matchPos, maxPos=%d, maxExeSize=%d, maxSizeCommand.c_str()=%s\n", maxPos, maxExeSize, maxSizeCommand.c_str());
#endif
          }
        }
      }

#ifdef DEBUG_CLI
      printf("In SS_expSetArgs(), at bottom of for appCommand.c_str()=%s\n", appCommand.c_str());
#endif
    } // end for xi
  }

#ifdef DEBUG_CLI
  printf("In SS_expSetArgs(), after the for loop  appCommand.c_str()=%s\n", appCommand.c_str());
  printf("In SS_expSetArgs(), after the for loop  appCommand.length()=%d\n", appCommand.length());
  printf("**** In SS_expSetArgs(), after the for loop  maxPos=%d, maxExeSize=%d\n", maxPos, maxExeSize);
#endif

  // Because of inserting the fullpath string for the non-fullpath version positioning
  // can be a problem to compute and keep correct.   Just find the position again after
  // all the insertion is completed.

  matchPos = appCommand.find(maxSizeCommand);

#ifdef DEBUG_CLI
  printf("**** In SS_expSetArgs(), after the for loop  matchPos=%d, maxSizeCommand.length()=%d\n", matchPos, maxSizeCommand.length());
  printf("**** In SS_expSetArgs(), after the for loop  (matchPos+maxExeSize+1)=%d, (appCommand.length()-(matchPos+maxExeSize))=%d\n", (matchPos+maxExeSize+1), (appCommand.length()-(matchPos+maxExeSize)));
#endif
  
//  basically the replace does this first:   appCommand = appCommand + " " - existing args;
//  basically the replace does this second:  appCommand = appCommand + " " + S;
  int replace_length = (appCommand.length()-(matchPos+maxExeSize));
  if ( replace_length  > 0) {
    appCommand.replace( (matchPos+maxExeSize+1), replace_length, S);
  } else if ( replace_length ==  0) {
    appCommand = appCommand + " " + S;
  } 

#ifdef DEBUG_CLI
  printf("In SS_expSetArgs(), after the new args added  appCommand.c_str()=%s\n", appCommand.c_str());
#endif

  } // end appCommand not empty

  exp->FW()->setApplicationCommand(appCommand.c_str(), false /* trust_me */);
  exp->Q_UnLock ();

  if ((cmd->Status() == CMD_ERROR) ||
      (cmd->Status() == CMD_ABORTED)) {
    return false;
  }

#ifdef DEBUG_CLI
  printf("Exit SS_expSetArgs()\n");
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_expSetParam()
 * 
 * Implement the 'expSetParam' command by reading values form the
 * parse object and sending them to the collectors attached to an
 * experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful completion of the command.
 *
 * @error   "false is returned if no experiment can be determined or
 *          if a thrown exception is caught while accessing the
 *          experiment's database.
 *
 */
bool SS_expSetParam (CommandObject *cmd) {

  ExperimentObject *exp = Find_Specified_Experiment (cmd);
  if (exp == NULL) {
    return false;
  }

#ifdef DEBUG_CLI
  printf("Enter SS_expSetParam()\n");
#endif

  Assert(cmd->P_Result() != NULL);
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();

  if (p_result->isParam()) {
    ParseParam *p_param = p_result->getParam();
    char *type_name = p_param->getExpType();
    char *param_name = p_param->getParamType();
    std::vector<ParamVal> *value_list = p_param->getValList();

#ifdef DEBUG_CLI
    printf("In SS_expSetParam(), type_name=%s, param_name=%s\n", type_name, param_name);
#endif

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    if (type_name != NULL) {
     // The user has specified a particular collector.
     // Set the paramater for just that one collector.
      std::string C_name = std::string(type_name);
      try {
        Collector C = Get_Collector (exp->FW(), C_name);
#ifdef DEBUG_CLI
        printf("In SS_expSetParam(), calling setparam, C_name==%s\n", C_name.c_str() );
#endif
        (void) setparam(C, param_name, value_list);
      }
      catch(const Exception& error) {
       // Return message, but continue to process other collectors.
        Mark_Cmd_With_Std_Error (cmd, error);
      }
    } else {
     // Get the list of collectors used in the specified experiment.
     // Set the paramater for every collector that is part of the experiment.
      bool param_was_set = false;
      CollectorGroup cgrp = exp->FW()->getCollectors();
      CollectorGroup::iterator ci;
      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        try {
          param_was_set |= setparam(*ci, param_name, value_list);
        }
        catch(const Exception& error) {
         // Return message, but continue to process other collectors.
          Mark_Cmd_With_Std_Error (cmd, error);
          continue;
        }
      }

      if (!param_was_set) {
       // Record the error but continue to try to set other parameters.
    	std::string s("The specified parameter, ");
        s = s + param_name + ", was not set for any collector.";
    	Mark_Cmd_With_Soft_Error(cmd,s);
      }
    }
    exp->Q_UnLock ();

    if ((cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED)) {
      return false;
    }

  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


// Information Commands

/**
 * Utility: Get_Collector_Metadata ()
 * 
 * Helper routine for ReportStatus() to get the value of parameter
 * and return a typeless CommandResult object.
 *     
 * @param   c - the Framework::Collector that contains the parameter.
 * @param   m - the Metadata describing the desired parameter.ameter.
 *
 * @return  CommandResult * for the created object.
 *
 * @error   the returned CommandResult is the string "Unknown type."
 *          if the data type of the parameter is not a known, simple type.
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
    uint64_t Value;
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
  } else if( m.isType(typeid(std::string)) ) {
    std::string Value;
    c.getParameterValue(id, Value);
    Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(std::map<std::string, bool>))) {
    std::map<std::string, bool> Value;
    c.getParameterValue(id, Value);

   // Get the names of all the functions that are marked as true.
    std::string s;
    bool need_comma = false;
    for (std::map<std::string, bool>::iterator
               im = Value.begin(); im != Value.end(); im++) {
#if DEBUG_CLI
//        printf("In Get_Collector_Metadata, im->second=%d, im->first.c_str()=%s\n", im->second, im->first.c_str());     
#endif
      if (im->second) {
        if (need_comma) s += ", ";
        s += im->first;
#if DEBUG_CLI
//        printf("In Get_Collector_Metadata, building parameter list, s.c_str()=%s\n", s.c_str());     
#endif
        need_comma = true;
      }
    }

    Param_Value = CRPTR (s);
  } else {
    Param_Value = CRPTR ("Unknown type.");
  }
  return Param_Value;
}

/**
 * SemanticRoutine: getListOfRanks ()
 * 
 * List all the MPI ranks associated with an experiment.
 *     
 * @param   cmd - the CommandObject baing processed.
 *
 * @return  "true" on successful complation.
 *
 * @error   "false" returned if no experiment can be determined
 *          or if an "-f" filter is attached to the command.
 *
 */
static bool getListOfRanks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the PIDs on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks, all' is not supported.");
    return false;
  } else {
   // Get the Rankss for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place every rank into a set so that it will only be listed once.
    std::set<int64_t> rset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        rset.clear();
        break;
      }

      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      if (prank.first) {
        rset.insert ( prank.second );
      }
    }


    for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//      printf(" list ranks, *rseti=%d\n", *rseti);
      cmd->Result_Int ( *rseti );
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

//
// getRankRange tries to take the list of ranks and create a string
// that has a range of ranks or groups of ranges.
//
static std::string getRankRange(std::list<int64_t> &list_of_ranks)
{


  std::string infoString = "";
  int64_t previous_rank = -1;
  int64_t max_range_rank = -1;
  int64_t min_range_rank = -1;
  char tRankStr[12] ;
  char tMaxRankStr[12];
  char tMinRankStr[12];
  std::string rankStr = "";
  std::string maxRankStr = "";
  std::string minRankStr = "";


#if DEBUG_CLI
  printf("getRankRange() , list_of_ranks.size()=%d\n", list_of_ranks.size());
#endif

  if( list_of_ranks.size() > 0 )
  {
    bool first_time = true;
    int rank_count = 0;
    for( std::list<int64_t>::const_iterator it = list_of_ranks.begin(); it != list_of_ranks.end(); it++ )
    {
      rank_count = rank_count + 1;
      int64_t rank = (int64_t)*it;
      sprintf(tRankStr, "%d", rank);
      rankStr =+ tRankStr;
     
#ifdef DEBUG_CLI
      printf("getRankRange(), rank=%ld, rank_count=%d, list_of_ranks.size()=%d\n", rank, rank_count, list_of_ranks.size() );
#endif

      // Handle first time or only one rank cases
      if( list_of_ranks.size() == 1 ) {
          infoString += rankStr;
          break;
      } else if (first_time) {
          max_range_rank = rank;
          min_range_rank = rank;
          previous_rank = rank;
          first_time = false;
#ifdef DEBUG_CLI
          printf("getRankRange(), FIRST TIME, min_range_rank=%ld, max_range_rank=%ld\n", 
                 min_range_rank, max_range_rank );
#endif
          continue;
      }

      if (rank > previous_rank ) {

#ifdef DEBUG_CLI
         printf("getRankRange(), rank>prev, previous_rank=%ld, rank=%ld\n", previous_rank, rank );
#endif

        if (rank == previous_rank + 1  && (rank_count != list_of_ranks.size()) ) {

#ifdef DEBUG_CLI
          printf("getRankRange(), rank==prev+1, before(max_range_rank=%ld), rank=%ld\n", max_range_rank, rank );
#endif
          max_range_rank = rank;

        } else {

#ifdef DEBUG_CLI
          printf("getRankRange(), NOT rank==prev+1, max_range_rank=%ld, min_range_rank=%ld\n", max_range_rank, min_range_rank );
#endif
          if (max_range_rank != min_range_rank && (rank_count != list_of_ranks.size()) ) {

            sprintf(tMaxRankStr, "%d", max_range_rank);
            maxRankStr = tMaxRankStr;
            infoString += maxRankStr;
#ifdef DEBUG_CLI
            printf("getRankRange(), NOT rank==prev+1, max_range_rank=%ld != min_range_rank=%ld\n", max_range_rank, min_range_rank );
            printf("getRankRange(), NOT rank==prev+1, rank_count=%ld != list_of_ranks.size()=%ld\n", rank_count, list_of_ranks.size() );
#endif

          } else {

            // if in a range creation and you get to the end of the for
            // need to update the last item and output the range
            if  (rank_count == list_of_ranks.size()) {
#ifdef DEBUG_CLI
               printf("getRankRange(), NOT rank==prev+1, rank_count=%d == list_of_ranks.size(), setting max_range_rank=%ld = rank=%ld\n", rank_count, max_range_rank, rank );
#endif
               max_range_rank = rank;
            }

            // You've arrived here because you encountered a value that is
            // not consequitive or we have gotten to the end of the ranks 
            // and need to output the range creation prior to leaving the loop.

            sprintf(tMaxRankStr, "%d", max_range_rank);
            maxRankStr = tMaxRankStr;
            sprintf(tMinRankStr, "%d", min_range_rank);
            minRankStr = tMinRankStr;
            if (min_range_rank != max_range_rank) {
              if (min_range_rank+1 == max_range_rank || previous_rank + 1 == max_range_rank) {
                infoString += minRankStr + "-" + maxRankStr;
#ifdef DEBUG_CLI
               printf("getRankRange(), max_range_rank=%ld, min_range_rank=%ld\n", max_range_rank, min_range_rank );
#endif
              } else {
                // if end of the list put out both values, if not just the min
                if (rank_count == list_of_ranks.size()) {
                  infoString += minRankStr;
                  infoString += maxRankStr;
                } else {
                  infoString += " ";
                  infoString += minRankStr;
                  infoString += " ";
                }
              }
            } else {
              infoString += " ";
              infoString += minRankStr;
              infoString += " ";
            }
#ifdef DEBUG_CLI
            printf("getRankRange(), arrived here because you encountered a value that is not consequitive\n" );
            printf("getRankRange(), arrived .... infoString=%s\n", infoString.c_str() );
            printf("getRankRange(), arrived .... rank=%ld, min_range_rank=%ld, max_range_rank=%ld\n", infoString.c_str(), min_range_rank, max_range_rank );
#endif
            min_range_rank = rank;
            max_range_rank = rank;
          } 

        } // end else -> not (rank == previous_rank + 1)



      } else {
#ifdef DEBUG_CLI
       printf("ERROR - ranks not ascending\n");
       break;
#endif
      } 


#ifdef DEBUG_CLI
     printf("getRankRange(), SET prev at end of for, previous_rank=%ld, rank=%ld\n", previous_rank, rank );
#endif
     previous_rank = rank;
    } // end for

  }
  return(infoString);
}

/**
 * Utility: ReportComponents ()
 * 
 * Helper routine for the 'list -v components' command.  Extract
 * information form a database and format a report about
 * what it contains..
 *     
 * @param   cmd - the CommandObject being processed.
 * @param   exp - the Experiment being interrogated.
 *
 * @return  "true" on successful generation of information.
 *
 * @error   "false" is returned if a thrown exception is caught
 *          while looking at the database.
 *
 */
static bool ReportComponents(CommandObject *cmd, ExperimentObject *exp, bool FullDisplay) {

 std::list<int64_t> list_of_ranks;
 std::list<std::string> list_of_hosts;
 int FullRankLimitValue = 128;
 char tNumHostsStr[10];

#if DEBUG_CLI
  printf("In ReportComponents, FullDisplay=%d\n", FullDisplay);
#endif

 // Prevent this experiment from changing until we are done.
  exp->Q_Lock (cmd, false);
  exp->Determine_Status ();

//  char id[20]; sprintf(&id[0],"%lld",(int64_t)exp->ExperimentObject_ID());
//  cmd->Result_String ("Experiment definition");
//  std::string TmpDB = exp->Data_Base_Is_Tmp() ? "Temporary" : "Saved";
//  cmd->Result_String ("{ # ExpId is " + std::string(&id[0])
//                         + ", Status is " + exp->ExpStatus_Name()
//                         + ", " + TmpDB + " database is " + exp->Data_Base_Name ());
  try {
      if (exp->FW() != NULL) {

//        bool offlineInstrumentor = exp->getIsInstrumentorOffline();
#if OFFLINE_IN_CLI
//        if (offlineInstrumentor) {
//          cmd->Result_String ("    Instrumentor: Offline");
//        } else {
#if HAVE_MRNET
//          cmd->Result_String ("    Instrumentor: Online (MRNet)");
#else
//          cmd->Result_String ("    Instrumentor: Online (DPCL)");
#endif
//        } 
#endif

        ThreadGroup tgrp = exp->FW()->getThreads();
        ThreadGroup::iterator ti;
        bool atleastone = false;
#if DEBUG_CLI
        printf("ReportComponents, tgrp.size()=%d\n", tgrp.size());
#endif

      if (!FullDisplay) {

        // Place every rank into a set so that it will only be listed once.
         std::set<int64_t> rset;
         std::set<std::string> hset;

         for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {

             Thread t = *ti;

            // Check for asynchronous abort command
             if (cmd->Status() == CMD_ABORTED) {
               rset.clear();
               hset.clear();
               break;
             }

             std::pair<bool, int> prank = t.getMPIRank();
             if (prank.first) {
               rset.insert ( prank.second );
             }

             std::string myHost = t.getHost();
             hset.insert ( myHost );

           }

            for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
              list_of_hosts.push_back(*hseti );
            }

           for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//             printf(" list ranks, *rseti=%d\n", *rseti);
             list_of_ranks.push_back(*rseti );
           }

#if DEBUG_CLI
           printf(" list ranks, list_of_ranks.size()=%d\n", list_of_ranks.size());
           printf(" list hosts, list_of_hosts.size()=%d\n", list_of_hosts.size());
#endif
        }
          
         sprintf(tNumHostsStr, "%d", list_of_hosts.size());
         std::string numHostsStr = tNumHostsStr;
         std::string listRankStr = getRankRange(list_of_ranks);

        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          if (!atleastone) {
            atleastone = true;
            int tgrp_size = tgrp.size();
            if (!FullDisplay && tgrp_size > FullRankLimitValue ) {
              
              char sTgrpSize[20]; sprintf(&sTgrpSize[0],"%lld",tgrp_size);
              std::string SCSC = "  Currently Specified Components: (Summary Information, use expStatus -v full to see all of the individual " + std::string(&sTgrpSize[0]) + " Components)";
              cmd->Result_String (SCSC);
            } else {
              cmd->Result_String ("  Currently Specified Components:");
            } 

          }

          std::string S = "";
          if ( FullDisplay || (!FullDisplay && tgrp.size() <= FullRankLimitValue) ) {

            std::string host = t.getHost();
            pid_t pid = t.getProcessId();
            int64_t p = pid;
            char spid[20]; sprintf(&spid[0],"%lld",p);
            S = "    -h " + host + " -p " + std::string(&spid[0]);
  
            std::pair<bool, int> pthread = t.getOpenMPThreadId();
            bool threadHasThreadId = false;
            int64_t pthreadid = 0;
            if (pthread.first) {
              threadHasThreadId = true;
              pthreadid = pthread.second;
            } else {
              std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
              if (posixthread.first) {
                threadHasThreadId = true;
                pthreadid = posixthread.second;
              }
            }
            if (threadHasThreadId) {
              char tid[20]; sprintf(&tid[0],"%lld",pthreadid);
              S = S + " -t " + std::string(&tid[0]);
            }

            std::pair<bool, int> rank = t.getMPIRank();
            if (rank.first) {
              int64_t r = rank.second;
              char rid[20]; sprintf(&rid[0],"%lld",r);
              S = S + " -r " + std::string(&rid[0]);
            }
  
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

     	    std::pair<bool, LinkedObject> tExe = t.getExecutable();
	    if (tExe.first) {
	    	    std::string epath = tExe.second.getPath().getBaseName();
		    S = S + " (" + epath + ")";
	    }
            cmd->Result_String ( S );

          } else {
            cmd->Result_String ("  Ranks: " + listRankStr);
            cmd->Result_String ("  Hosts: " + numHostsStr);
            std::string host = t.getHost();
            cmd->Result_String ("  First Host: " + host);
            pid_t pid = t.getProcessId();
            int64_t p = pid;
            char spid[20]; sprintf(&spid[0],"%lld",p);
            cmd->Result_String ("  First Process Id: " + std::string(&spid[0]));
            break;
          }
        }

      }
//    cmd->Result_String ( "}");
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


// Forward reference.
static bool SS_ListSavedViews (CommandObject *cmd, ExperimentObject *exp);
/**
 * Utility: ReportStatus ()
 * 
 * Helper routine for the 'expStatus' command.  Extract
 * information form a database and format a report about
 * what it contains..
 *     
 * @param   cmd - the CommandObject being processed.
 * @param   exp - the Experiment being interrogated.
 *
 * @return  "true" on successful generation of information.
 *
 * @error   "false" is returned if a thrown exception is caught
 *          while looking at the database.
 *
 */
static bool ReportStatus(CommandObject *cmd, ExperimentObject *exp, bool FullDisplay) {

 std::list<int64_t> list_of_ranks;
 std::list<std::string> list_of_hosts;
 int FullRankLimitValue = 128;
 char tNumHostsStr[10];

#if DEBUG_CLI
  printf("In ReportStatus, FullDisplay=%d\n", FullDisplay);
#endif

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

        bool offlineInstrumentor = exp->getIsInstrumentorOffline();
#if OFFLINE_IN_CLI
        if (offlineInstrumentor) {
          cmd->Result_String ("    Instrumentor: Offline");
        } else {
#if HAVE_MRNET
          cmd->Result_String ("    Instrumentor: Online (MRNet)");
#else
          cmd->Result_String ("    Instrumentor: Online (DPCL)");
#endif
        } 
#endif
        Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
        if ((databaseExtent.getTimeInterval().getBegin() == Time::TheBeginning()) ||
            (databaseExtent.getTimeInterval().getBegin() ==
                             databaseExtent.getTimeInterval().getEnd())) {
          cmd->Result_String ("    There is no performance data recorded in the database.");
        } else {
          std::ostringstream lt(std::ios::out);
          std::ostringstream st(std::ios::out);
          std::ostringstream et(std::ios::out);
          Time ST = databaseExtent.getTimeInterval().getBegin();
          Time ET = databaseExtent.getTimeInterval().getEnd();

          int64_t elapsed_time = ((ET - ST));
          int64_t scaled_time = (elapsed_time / 1000000000);
          CommandResult *elapsed_cr = new CommandResult_Duration (elapsed_time);
         // Format elapsed time.
          lt << elapsed_cr->Form();
         // Determine resulting units.
          std::string UNITS;
          if (scaled_time >= (60 * 60 * 24)) {
            UNITS = "dd:hh:mm:ss";
          } else if (scaled_time >= (60 * 60)) {
            UNITS = "hh:mm:ss";
          } else if (scaled_time >= (60)) {
            UNITS = "mm:ss";
          } else if (scaled_time >= 1) {
            UNITS = "seconds";
          } else if (elapsed_time >= 1000000) {
            UNITS = "ms";
          } else {
            UNITS = "ns";
          }
          delete elapsed_cr;

          st << databaseExtent.getTimeInterval().getBegin();
          et << databaseExtent.getTimeInterval().getEnd();
          cmd->Result_String ("    Performance data spans "
                              + lt.str()
                              + " " + UNITS + "  from "
                              + st.str()
                              + " to "
                              + et.str());
        }

        std::list<std::string> ExList;
        Most_Common_Executable (cmd, exp, false, ExList);
        cmd->Result_String ("  Executables Involved:");
        if (ExList.empty()) {
          if (offlineInstrumentor) {
            cmd->Result_String ("    (Not available for offline experiment at this time.)");
          } else {
            cmd->Result_String ("    (none)");
          }
        } else {
          for (std::list<std::string>::iterator xi = ExList.begin(); xi != ExList.end(); xi++) {
            cmd->Result_String ("    " + *xi);
          }
        }

        ThreadGroup tgrp = exp->FW()->getThreads();
        ThreadGroup::iterator ti;
        bool atleastone = false;
#if DEBUG_CLI
        printf("ReportStatus, tgrp.size()=%d\n", tgrp.size());
#endif

      if (!FullDisplay) {

        // Place every rank into a set so that it will only be listed once.
         std::set<int64_t> rset;
         std::set<std::string> hset;

         for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {

             Thread t = *ti;

            // Check for asynchronous abort command
             if (cmd->Status() == CMD_ABORTED) {
               rset.clear();
               hset.clear();
               break;
             }

             std::pair<bool, int> prank = t.getMPIRank();
             if (prank.first) {
               rset.insert ( prank.second );
             }

             std::string myHost = t.getHost();
             hset.insert ( myHost );

           }

            for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
              list_of_hosts.push_back(*hseti );
            }

           for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//             printf(" list ranks, *rseti=%d\n", *rseti);
             list_of_ranks.push_back(*rseti );
           }

#if DEBUG_CLI
           printf(" list ranks, list_of_ranks.size()=%d\n", list_of_ranks.size());
           printf(" list hosts, list_of_hosts.size()=%d\n", list_of_hosts.size());
#endif
        }
          
         sprintf(tNumHostsStr, "%d", list_of_hosts.size());
         std::string numHostsStr = tNumHostsStr;
         std::string listRankStr = getRankRange(list_of_ranks);

        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          if (!atleastone) {
            atleastone = true;
            int tgrp_size = tgrp.size();
            if (!FullDisplay && tgrp_size > FullRankLimitValue ) {
              
              char sTgrpSize[20]; sprintf(&sTgrpSize[0],"%lld",tgrp_size);
              std::string SCSC = "  Currently Specified Components: (Summary Information, use expStatus -v full to see all of the individual " + std::string(&sTgrpSize[0]) + " Components)";
              cmd->Result_String (SCSC);
            } else {
              cmd->Result_String ("  Currently Specified Components:");
            } 

          }

          std::string S = "";
          if ( FullDisplay || (!FullDisplay && tgrp.size() <= FullRankLimitValue) ) {

            std::string host = t.getHost();
            pid_t pid = t.getProcessId();
            int64_t p = pid;
            char spid[20]; sprintf(&spid[0],"%lld",p);
            S = "    -h " + host + " -p " + std::string(&spid[0]);
  
            std::pair<bool, int> pthread = t.getOpenMPThreadId();
            bool threadHasThreadId = false;
            int64_t pthreadid = 0;
            if (pthread.first) {
              threadHasThreadId = true;
              pthreadid = pthread.second;
            } else {
              std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
              if (posixthread.first) {
                threadHasThreadId = true;
                pthreadid = posixthread.second;
              }
            }
            if (threadHasThreadId) {
              char tid[20]; sprintf(&tid[0],"%lld",pthreadid);
              S = S + " -t " + std::string(&tid[0]);
            }

            std::pair<bool, int> rank = t.getMPIRank();
            if (rank.first) {
              int64_t r = rank.second;
              char rid[20]; sprintf(&rid[0],"%lld",r);
              S = S + " -r " + std::string(&rid[0]);
            }
  
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

     	    std::pair<bool, LinkedObject> tExe = t.getExecutable(Time::Now());
	    if (tExe.first) {
	    	    std::string epath = tExe.second.getPath().getBaseName();
		    S = S + " (" + epath + ")";
	    }
            cmd->Result_String ( S );

          } else {
            cmd->Result_String ("  Ranks: " + listRankStr);
            cmd->Result_String ("  Hosts: " + numHostsStr);
            std::string host = t.getHost();
            cmd->Result_String ("  First Host: " + host);
            pid_t pid = t.getProcessId();
            int64_t p = pid;
            char spid[20]; sprintf(&spid[0],"%lld",p);
            cmd->Result_String ("  First Process Id: " + std::string(&spid[0]));
            break;
          }
        }

        CollectorGroup cgrp = exp->FW()->getCollectors();
        CollectorGroup::iterator ci;
        atleastone = false;
        std::string SColl ;
        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector c = *ci;
          ThreadGroup tgrp = c.getThreads();
          if (tgrp.empty()) {
            Metadata m = c.getMetadata();
            if (atleastone) {
              SColl = SColl + ",";
            } else {
              cmd->Result_String ("  Previously Used Data Collectors:");
              SColl = SColl + "    ";
              atleastone = true;
            }
            SColl = SColl + m.getUniqueId();
          }
        }
        if (atleastone) {
          cmd->Result_String ( SColl );
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
              SColl = "    " + cm.getUniqueId() + "::" +  m.getUniqueId();
              cmd->Result_String (SColl);
            }
          }

          cmd->Result_String ("  Parameter Values:");
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata cm = c.getMetadata();
            std::set<Metadata> md = c.getParameters();
            std::set<Metadata>::const_iterator mi;
            for (mi = md.begin(); mi != md.end(); mi++) {
              CommandResult_Columns *C = new CommandResult_Columns ();
              Metadata m = *mi;
              SColl = "    " + cm.getUniqueId() + "::" + m.getUniqueId() + " =";
              C->CommandResult_Columns::Add_Column (new CommandResult_RawString (SColl));
#if DEBUG_CLI
              printf("In ReportStatus, Parameter Values=%s\n", SColl.c_str());
#endif
              C->CommandResult_Columns::Add_Column (Get_Collector_Metadata (c, m));
              cmd->Result_Predefined (C);
            }
          }

          cmd->Result_String ("  Available Views:");
          SS_Get_Views (cmd, exp->FW(), "    ");
        }

        if (FullDisplay) {
          cmd->Result_String ("  Saved View Files:\n");
          (void) SS_ListSavedViews ( cmd, exp );
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
 * SemanticRoutine SS_expStatus ()
 * 
 * Print information about an experiment.
 *     
 * @param   cmd- the CommandObject being processed.
 *
 * @return  "true" if the command was successful.
 *
 * @error   "false" returned if no experiment is specified
 *          or if an error was detected while looking at
 *          the associated database.
 *
 */
bool SS_expStatus(CommandObject *cmd) {

  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  bool Full_KeyWord = Look_For_KeyWord (cmd, "full");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      if (!ReportStatus (cmd, exp, Full_KeyWord)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!ReportStatus (cmd, exp, Full_KeyWord)) {
      return false;
    }
  }

#if DEBUG_CLI
  printf("In SS_expStatus, calling cmd->set_Status(CMD_COMPLETE); before exiting\n");
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


/**
 * SemanticRoutine: SS_expView()
 * 
 * Generate a report for an experiment.
 *     
 * @param   cmd - the CommandObject baing processed.
 *
 * @return  "true" if no errors were encountered.
 *
 * @error   return "false" if no valid experiment or
 *          view is specified or if the view plugin
 *          can not be located.
 *
 */
bool SS_expView (CommandObject *cmd) {
  Framework::Experiment *experiment = NULL;
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  bool view_result = true;

#ifdef DEBUG_CLI
  std::cerr << "Enter SS_expView before printing clip=" << clip << "\n";
  if (clip != NULL) {
    clip->Print(std::cerr);
  }
  std::cerr << "\nEnter SS_expView after printing clip=" << clip << "\n";
#endif

#if DEBUG_CLI
  printf("In SS_expView, Entered, before calling Wait_For_Previous_Cmds()\n");
#endif

 // Wait for all executing commands to terminate.
 // We do this so that another command thread won't get burned
 // looking through the ExperimentObject_list when a new entry
 // is added to it.
  Wait_For_Previous_Cmds ();

#if DEBUG_CLI
  printf("In SS_expView, Entered, after calling Wait_For_Previous_Cmds()\n");
#endif

 // Some views do not need depend on an ExperimentObject.
 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : Experiment_Focus ( WindowID );
  ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;

#if DEBUG_CLI
  experiment = exp->FW();
  std::string Data_File_Name = experiment->getName();
  printf("In SS_expView, ExperimentID=%d, DATA_FILE_NAME=%s\n", ExperimentID, Data_File_Name.c_str());
#endif

 // For batch processing, wait for completion before generating a report.
 // Unless, of course, the user explicitly tells us not to wait.
  if ((exp != NULL) && !Window_Is_Async(WindowID) && (Embedded_WindowID == 0) &&
      !Look_For_KeyWord(cmd, "NoWait")) {
#if DEBUG_CLI
     printf("In SS_expView, (batch processing) calling Wait_For_Exp_State, ExperimentID=%d\n", ExperimentID);
#endif
    (void) Wait_For_Exp_State (cmd, ExpStatus_Paused, exp);
   // Be sure that write buffers are actually written to the database.
   // This assures us of getting all the data when we generate the view.
    try {
#if DEBUG_CLI
     printf("In SS_expView, (batch processing) before calling exp->FW()->flushPerformanceData()\n");
#endif
      exp->FW()->flushPerformanceData();
#if DEBUG_CLI
     printf("In SS_expView, (batch processing) after calling exp->FW()->flushPerformanceData()\n");
#endif
    }
    catch (const Exception& error) {
     // Ignore any errors and let them be regenerated when we try to do something else.
    }
  }

#if DEBUG_CLI
  printf("In SS_expView, before calling exp->Q_Lock (cmd, true)\n");
#endif

 // Prevent this experiment from changing until we are done.
  if (exp != NULL) exp->Q_Lock (cmd, true);

#if DEBUG_CLI
  printf("In SS_expView, after calling exp->Q_Lock (cmd, true)\n");
#endif

 // Pick up the <viewType> from the comand.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getViewList();
  std::vector<std::string>::iterator si;
  if (p_slist->begin() == p_slist->end()) {
#if DEBUG_CLI
    printf("In SS_expView, The user has not selected a view\n");
#endif
   // The user has not selected a view.
    if ((exp == NULL) ||
        (exp->FW() == NULL)) {
     // No experiment was specified, so we can't find a useful view to gneerate.
      Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified.");
      view_result = false;
    } else {
     // Look for a view that would be meaningful.
      CollectorGroup cgrp = exp->FW()->getCollectors();
      if (cgrp.begin() == cgrp.end()) {
       // No collector was used.
    	Mark_Cmd_With_Soft_Error(cmd, "No performance measurements were made for the experiment.");
        view_result = false;
      } else {
        bool view_found = false;
        CollectorGroup::iterator cgi;
        for (cgi = cgrp.begin(); cgi != cgrp.end(); cgi++) {
         // See if there is a view by the same name.
          Collector c = *cgi;
          Metadata m = c.getMetadata();
          std::string collector_name = m.getUniqueId();
          ViewType *vt = Find_View (collector_name);
          if (vt != NULL) {
           // Generate the selected view
            view_found = true;

#if BUILD_CLI_TIMING
            // Gather performance information on the generic generation of views
            if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
               cli_timing_handle->cli_perf_data[SS_Timings::viewGenerationStart] = Time::Now();
            }
#endif

            view_result = SS_Generate_View (cmd, exp, collector_name);

#if BUILD_CLI_TIMING
            if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
               cli_timing_handle->processTimingEventEnd( SS_Timings::viewGenerationStart,
                                                         SS_Timings::viewGenerationCount,
                                                         SS_Timings::viewGenerationMax,
                                                         SS_Timings::viewGenerationMin,
                                                         SS_Timings::viewGenerationTotal,
                                                         SS_Timings::viewGenerationEnd);
            }
#endif
          }
        }

        if (!view_found) {
         // Use generic view as default
          std::string use_view = "stats";

         // On second though, just report this situation as an error.
          std::string S("A view can not be created for the experiment because ");
          if (cgrp.empty()) {
            S += "no data collectors were attached.";
          } else {
            S += "the view plugin could not be found.";
          }
          Mark_Cmd_With_Soft_Error(cmd, S);
          return false;
        }
      }
    }
  } else {
   // Generate all the views in the list.
    
    for (si = p_slist->begin(); si != p_slist->end(); si++) {
      std::string view = *si;

#if BUILD_CLI_TIMING
       // Gather performance information on the generic generation of views
      if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::viewGenerationStart] = Time::Now();
      }
#endif

      view_result = SS_Generate_View (cmd, exp, view);

#if BUILD_CLI_TIMING
      if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::viewGenerationStart,
                                                   SS_Timings::viewGenerationCount,
                                                   SS_Timings::viewGenerationMax,
                                                   SS_Timings::viewGenerationMin,
                                                   SS_Timings::viewGenerationTotal,
                                                   SS_Timings::viewGenerationEnd);
      }
#endif

      if (!view_result) {
        break;
      }
    }
  }

#if DEBUG_CLI
  printf("In SS_expView, about to cmd->set_Status(CMD_COMPLETE), if view_result=%d\n", view_result);
#endif

  if (view_result) {
    cmd->set_Status(CMD_COMPLETE);
  }

 // Release the experiment lock.
  if (exp != NULL) exp->Q_UnLock ();

  return view_result;
}

/**
 * SemanticRoutine: SS_View()
 *
 * Decide if this is a simple expView or more complicated cView.
 *
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" if successfully completed.
 *
 */
bool SS_View (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *primary_result = cmd->P_Result();
  std::vector<ParseRange> *cv_list = primary_result->getViewSet ();
  if ((cv_list == NULL) || (cv_list->empty())) {
    return SS_expView (cmd);
  } else {
    return SS_cView (cmd);
  }
}

/**
 * SemanticRoutine: SS_Info ()
 *
 * Not Yet Implemented.
 *
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true"
 *
 * @todo    Error handling.
 *
 */
bool SS_Info (CommandObject *cmd) {
  return true;
}

// Primitive Information Commands

/**
 * SemanticRoutine: SS_ListAppCommand ()
 * 
 * List the application command that was run to create the
 * performance data.
 *     
 * @param   cmd - the CommandObject being processed..
 *
 * @return  "true"
 *
 * @error   If experiment not found put out message to
 *          user to load or activate an experiment..
 *
 */
static bool SS_ListAppCommand (CommandObject *cmd) {
  Framework::Experiment *experiment = NULL;
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  if (exp == NULL) {
      cmd->Result_String ("'list -v appcommand' must have an experiment active.");
      return false;
  }

  experiment = exp->FW();
  if (experiment == NULL) {
      cmd->Result_String ("'list -v appcommand' must have an experiment active.");
      return false;
  }

  std::string appCommand = experiment->getApplicationCommand();
  cmd->Result_String ( appCommand );

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListBreaks ()
 * 
 * Not yet implemented.
 *     
 * @param   cmd - the CommandObject being processed..
 *
 * @return  "true"
 *
 * @error   Always issue a "Not Yet Implemented" message.
 *
 */
static bool SS_ListBreaks (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("'List -v Breaks' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListDatabase()
 *
 * Process 'list -v database' commands and return the
 * name of the database that is part of an experiment.
 *
 * @param   cmd - the CommandObject being processed.
 *
 * @return  bool - "true" if processing was successful.
 *
 * @error   failure to find any experiment returns "false".
 *
 */
static bool SS_ListDatabase (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // Get the database names for all defined experiments.
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
      cmd->Result_String ((*expi)->Data_Base_Name ());
    }
  } else {
   // Get the database name for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    cmd->Result_String (exp->Data_Base_Name ());
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * Utility:  Most_Common_Executable()
 *
 * Determine the executables associated with an experiment
 * Link the list (most frequently used, first) to the return
 * argument.  If OPENSS_VIEW_FULLPATH is set, return the full
 * path to this executable.
 *
 * @param   cmd - the current command beind processed.
 * @param   exp - the experiment to look at.
 * @param   returnFullPath - return the fullpath, if set
 * @param   ExList - the return list of names.
 *
 * @return  void - results are through the third argument.
 *
 * @error   failure to find any name returns "(none)".
 * @error   any error caught in a "catch" claues returns "(error)".
 *
 */
static void Most_Common_Executable (CommandObject *cmd,
                                    ExperimentObject *exp,
                                    bool returnFullPath,
                                    std::list<std::string>& ExList)
{
    try {

	std::map<std::string, int64_t> NameMap;

	// Iterate over all executable names in this experiment
	std::set<LinkedObject> executables = exp->FW()->getExecutables();
	for(std::set<LinkedObject>::const_iterator
		i = executables.begin(); i != executables.end(); ++i) {
	    
	    // Check for asynchronous abort command
	    if(cmd->Status() == CMD_ABORTED) {
		NameMap.clear();
		break;
	    }
	    
	    // Get the name of the executable
	    std::string executable = (OPENSS_VIEW_FULLPATH || returnFullPath) ?
		i->getPath() : i->getPath().getBaseName();
	    
	    // Add it to the name map
	    if(NameMap.find(executable) == NameMap.end())
		NameMap[executable] = 1;
	    else
		NameMap[executable]++;
	}

	// Build a new map that is sorted by frequency
	std::multimap<int64_t, std::string> CountMap;
	for(std::map<std::string, int64_t>::const_iterator
		i = NameMap.begin(); i != NameMap.end(); ++i)
	    CountMap.insert(std::make_pair(i->second, i->first));

	// Provide the executable names to the caller, most common first
	if(CountMap.empty())
	    cmd->Result_String ("(none)");
	else {
	    for(std::multimap<int64_t, std::string>::reverse_iterator 
		    i = CountMap.rbegin(); i != CountMap.rend(); ++i)
		ExList.push_back(i->second);
	}
	
    }
    catch (...) {
	cmd->Result_String ("(error)");
    }
}

/**
 * SemanticRoutine: SS_ListExecutable()
 *
 * Process 'list -v executable' commands and return the
 * name of the executable that is part of an experiment.
 *
 * @param   cmd - the CommandObject being processed.
 *
 * @return  bool - "true" if processing was successful.
 *
 * @error   failure to find any experiment returns "false".
 *
 */
static bool SS_ListExecutable (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  std::list<std::string> ExList;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // Get the Executable names for all defined experiments.
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
      Most_Common_Executable (cmd, *expi, false, ExList);
    }
  } else {
   // Get the Executable name for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

   // Scan the experiment for the executable name.
    Most_Common_Executable (cmd, exp, false, ExList);
  }

 // Put the names onto the command.
  for (std::list<std::string>::iterator xi = ExList.begin(); xi != ExList.end(); xi++) {
    cmd->Result_String (*xi);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListExp ()
 * 
 * List the experiment ID's for all the defined experiments.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" to always indicate successful completion.
 *
 * @error   none.
 *
 * Go through the definition list in reverse order becasue
 * new experiments (higher nubmers) are added to the top
 * of the list.  Doing this provides output that is in
 * ascending order.
 *
 */
static bool SS_ListExp (CommandObject *cmd) {
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
 * SemanticRoutine: SS_ListHosts ()
 * 
 * List all the hosts that are used in an experiment.
 *     
 * @param   cmd - the CommandObject begin processed.
 *
 * @return  "true" on successful completion.
 *
 * @error   "false" returned if no experiment is specified
 *          or if "-v all" is attached tot he command.
 *
 */
static bool SS_ListHosts (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Hosts on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v hosts, all' is not supported.");
    return false;
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

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        hset.clear();
        break;
      }

      Thread t = *ti;
      hset.insert (t.getHost());
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
 * SemanticRoutine SS_ListComponents ()
 * 
 * Print information about an experiment.
 *     
 * @param   cmd- the CommandObject being processed.
 *
 * @return  "true" if the command was successful.
 *
 * @error   "false" returned if no experiment is specified
 *          or if an error was detected while looking at
 *          the associated database.
 *
 */
bool SS_ListComponents(CommandObject *cmd) {

  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  bool Full_KeyWord = Look_For_KeyWord (cmd, "full");

  if (All_KeyWord) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      if (!ReportStatus (cmd, exp, Full_KeyWord)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!ReportComponents (cmd, exp, Full_KeyWord)) {
      return false;
    }
  }

#if DEBUG_CLI
  printf("In SS_ListComponents, calling cmd->set_Status(CMD_COMPLETE); before exiting\n");
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine SS_ListPreferences ()
 * 
 * Print information about an experiment.
 *     
 * @param   cmd- the CommandObject being processed.
 *
 * @return  "true" if the command was successful.
 *
 * @error   "false" returned if no experiment is specified
 *          or if an error was detected while looking at
 *          the associated database.
 *
 */
bool SS_ListPreferences(CommandObject *cmd) {

  if (!List_ConfigInfo(cmd)) {
    return false;
  }

/*
  if (All_KeyWord) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      if (!ReportStatus (cmd, exp, Full_KeyWord)) {
        return false;
      }
    }
  } else {
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }
    if (!ReportComponents (cmd, exp, Full_KeyWord)) {
      return false;
    }
  }
*/

#if DEBUG_CLI
  printf("In SS_ListPreferences, calling cmd->set_Status(CMD_COMPLETE); before exiting\n");
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}



/**
 * SemanticRoutine: SS_ListMetrics ()
 * 
 * List all the metrics for the collectors that are used
 * in an experiment..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful completion.
 *
 * @error   "false" returned if not experiment can be determined or
 *          if an exception, thrown from the Framework, is caught.
 *
 */
static bool SS_ListMetrics (CommandObject *cmd) {
  bool cmd_error = false;
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

#if DEBUG_CLI
  printf("In SS_ListMetrics\n");
#endif
 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getExpList();

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

      std::vector<std::string>::iterator si;
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
#if DEBUG_CLI
  printf("EXIT SS_ListMetrics\n");
#endif
  return cmd_error;
}

/**
 * SemanticRoutine: SS_ListMPIFunctions ()
 * 
 * Not yet implemented.
 *     
 * @param   cmd - the CommandObject being processed..
 *
 * @return  "true"
 *
 * @error   Always issue a "Not Yet Implemented" message.
 *
 */
static bool SS_ListMPIFunctions (CommandObject *cmd) {
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("'list -v mpifunctions' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListMPICategories ()
 * 
 * Not yet implemented.
 *     
 * @param   cmd - the CommandObject being processed..
 *
 * @return  "true"
 *
 * @error   Always issue a "Not Yet Implemented" message.
 *
 */
static bool SS_ListMPICategories (CommandObject *cmd) {

#if 1
  bool cmd_error = false;
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getExpList();

  OpenSpeedShop::Framework::Experiment *fw_exp = NULL;
  CollectorGroup cgrp;

  if (cmd->P_Result()->isExpId()) {
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

      std::vector<std::string>::iterator si;
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
  // use the collector group here to get the category information
  cmd->Result_String ("'list -v mpicategories' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

#else
  ExperimentObject *exp = Find_Specified_Experiment (cmd);

  cmd->Result_String ("'list -v mpicategories' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
#endif
  return true;
}

/**
 * SemanticRoutine: SS_ListObj ()
 * 
 * List the names of all LindekObjects that are aprt of an experiment..
 *     
 * @param   cmd - the CommandObject baing processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" returned if no experiment can be determined.
 *
 */
static bool SS_ListObj (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a LinkedObject list.
  std::set<LinkedObject> ulset;

#if DEBUG_CLI
  printf("In SS_ListObj, processing -f option by calling Get_Filtered_Objects\n");
#endif

  Get_Filtered_Objects (cmd, exp, tgrp, ulset);

 // Now go through the list of unique LinkedObjects and list their names.
  for (std::set<LinkedObject>::iterator lseti = ulset.begin(); lseti != ulset.end(); lseti++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      break;
    }

    LinkedObject lobj = *lseti;
    std::string L = lobj.getPath();
#if DEBUG_CLI
    printf("In SS_ListObj, LinkedObject lobj path name=%s\n", L.c_str());
#endif
    cmd->Result_String ( L );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);

#if DEBUG_CLI
    printf("Exit SS_ListObj\n");
#endif

  return true;
}


/**
 * SemanticRoutine: SS_ListParams ()
 * 
 * List the parameters that can be set for collectors that are
 * used in experiments.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful completion of the command.
 *
 * @error   "false" returned if no experiment can be determined
 *          or if an exception, thrown by the Framework, is caught.
 *
 * The algorithm first gets a list of all the collectors of interest
 * and then scans the list to find all the parameters. Note that the
 * current values of the parameters is not determined.
 *
 */
static bool SS_ListParams (CommandObject *cmd, bool showValues, bool onlyValues) {
  bool cmd_error = false;
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

#ifdef DEBUG_CLI
  std::cerr << "Enter SS_ListParams before printing clip=" << clip << "\n";
  if (clip != NULL) {
    clip->Print(std::cerr);
  }
#endif

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getExpList();

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

      std::vector<std::string>::iterator si;
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
    std::string S;
    for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
      Collector c = *ci;
      Metadata cm = c.getMetadata();
      std::set<Metadata> md = c.getParameters();
      std::set<Metadata>::const_iterator mi;
      for (mi = md.begin(); mi != md.end(); mi++) {
        CommandResult_Columns *C = new CommandResult_Columns ();
        Metadata m = *mi;

        if (!showValues) {
          cmd->Result_String ( cm.getUniqueId() + "::" +  m.getUniqueId() );
        } else if (onlyValues) {
           cmd->Result_Predefined (Get_Collector_Metadata (c, m));
        } else {
           S = "    " + cm.getUniqueId() + "::" + m.getUniqueId() + " =";
           C->CommandResult_Columns::Add_Column (new CommandResult_RawString (S));
#if DEBUG_CLI
           printf("In SS_ListParams, Parameter Values=%s\n", S.c_str());
#endif
           C->CommandResult_Columns::Add_Column (Get_Collector_Metadata (c, m));
           cmd->Result_Predefined (C);
        } 
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
 * SemanticRoutine: SS_ListPids ()
 * 
 * List all the Process ID's associated with an experiment..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" returned if no experiment cna be determined.
 *
 */
static bool SS_ListPids (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the PIDs on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v pids, all' is not supported.");
    return false;
  } else {
   // Get the Pids for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v pids' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);
    std::set<pid_t> pset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        pset.clear();
        break;
      }

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
 * SemanticRoutine: SS_ListRanks ()
 * 
 * List all the MPI ranks associated with an experiment.
 *     
 * @param   cmd - the CommandObject baing processed.
 *
 * @return  "true" on successful complation.
 *
 * @error   "false" returned if no experiment can be determined
 *          or if an "-f" filter is attached to the command.
 *
 */
static bool SS_ListRanks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the PIDs on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks, all' is not supported.");
    return false;
  } else {
   // Get the Rankss for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place every rank into a set so that it will only be listed once.
    std::set<int64_t> rset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        rset.clear();
        break;
      }

      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      if (prank.first) {
        rset.insert ( prank.second );
      }
    }

    for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//      printf(" list ranks, *rseti=%d\n", *rseti);
      cmd->Result_Int ( *rseti );
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

int SS_getNumRanks(CommandObject *cmd, ExperimentObject *exp) {

#if 1
   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);
#endif

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place every rank into a set so that it will only be listed once.
    std::set<int64_t> rset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        rset.clear();
        break;
      }

      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      if (prank.first) {
        rset.insert ( prank.second );
      }
    }

    int64_t rank_count = 0;
    for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//      printf(" list ranks, *rseti=%d\n", *rseti);
      rank_count = rank_count + 1;
    }

#if 1
    exp->Q_UnLock ();
#endif

    return rank_count;

}




/**
 * SemanticRoutine: SS_ListNumberOfRanks ()
 * 
 * List all the MPI ranks associated with an experiment.
 *     
 * @param   cmd - the CommandObject baing processed.
 *
 * @return  "true" on successful complation.
 *
 * @error   "false" returned if no experiment can be determined
 *          or if an "-f" filter is attached to the command.
 *
 */
static bool SS_ListNumberOfRanks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the PIDs on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks, all' is not supported.");
    return false;
  } else {
   // Get the Rankss for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v ranks' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place every rank into a set so that it will only be listed once.
    std::set<int64_t> rset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        rset.clear();
        break;
      }

      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      if (prank.first) {
        rset.insert ( prank.second );
      }
    }

    int64_t rank_count = 0;
    for (std::set<int64_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
//      printf(" list ranks, *rseti=%d\n", *rseti);
      rank_count = rank_count + 1;
    }

    cmd->Result_Int ( rank_count );

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListSavedViews ()
 * 
 * List the generated views that have been saved for the experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *          exp - the associated experiment.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" returned if no experiment can be determined.
 *
 */
static bool SS_ListSavedViews (CommandObject *cmd, ExperimentObject *exp) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // List the generated views that have been saved for the experiment.
  if (exp == NULL) {
    return false;
  }

#if 1
  if (exp->FW() != NULL) {
     std::string db_name = exp->FW()->getName();
     std::vector<std::string> savedDbCommands = exp->FW()->Experiment::getSavedCommandList(db_name);
     std::vector<std::string>::iterator k;
     int i = 0;
     std::string infoheader("");
     char header1[80];
     sprintf(header1, "Views from database: %s", db_name.c_str());
     infoheader += std::string(header1);
     infoheader += "\n";
     cmd->Result_String ( infoheader );
     for (k=savedDbCommands.begin();k != savedDbCommands.end(); k++) {
        std::string info("  ");
        info += *k;
#if 0
        // This info should be saved in the database
        savedViewInfo *svi = exp->Get_savedViewInfo (i);
        //std::cerr << "DEBUG: savedViewInfo, svi=" << svi << " command=" << *k << std::endl;
        if (svi != NULL )  {
          if (svi->NewFile()) {
            info += "  (new)";
          }
          if (svi->GenTime() != 0) {
            char buffer[80];
            sprintf(buffer, "  Generation Time: %lld Seconds.", (svi->GenTime() / 1000000000));
            info += std::string(buffer);
          }
          info += "  `";
          info += svi->GenCmd();
          info += "`";
        }
#endif
        //std::cerr << "Saved View command=" << *k << std::endl;
        info += "\n";
        i = i + 1;
        cmd->Result_String ( info );
     }
  }

#else
  int64_t num_files = exp->Get_SavedViewFileCnt();
  for (int64_t i = 0; i < num_files; i++) {
    savedViewInfo *svi = exp->Get_savedViewInfo (i);
    if (svi == NULL) continue;
    std::string info("\t");
    info += svi->FileName();
    if (svi->NewFile()) {
      info += "  (new)";
    }
    if (svi->GenTime() != 0) {
      char buffer[80];
      sprintf(buffer, "  Generation Time: %lld Seconds.", (svi->GenTime() / 1000000000));
      info += std::string(buffer);
    }
    info += "  `";
    info += svi->GenCmd();
    info += "`";

    cmd->Result_String ( info );
  }
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListSrc ()
 * 
 * List the basename or full path (based on the value of OPENSS_VIEW_FULLPATH)
 * for every function that is part of a specified experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   " false" returned if no experiment can be determined.
 *
 */
static bool SS_ListSrc (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;
    std::set<Statement> sobj = fobj.getDefinitions();
    if( sobj.size() > 0 ) {
      std::set<Statement>::const_iterator sobji = sobj.begin();
      std::string F = (OPENSS_VIEW_FULLPATH) ? sobji->getPath() : sobji->getPath().getBaseName();
      mset.insert ( F );
    }
  }

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


/**
 * SemanticRoutine: SS_ListSrcFullPath ()
 * 
 * List the basename or full path (based on the value of OPENSS_VIEW_FULLPATH)
 * for every function that is part of a specified experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   " false" returned if no experiment can be determined.
 *
 */
static bool SS_ListSrcFullPath (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;
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

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


static bool SS_ListStatements (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;
    std::set<Statement> sobj = fobj.getStatements();
    if( sobj.size() > 0 ) {
      for (std::set<Statement>:: iterator si = sobj.begin(); si != sobj.end(); si++) {
        Statement st = *si;
	std::string SL = st.getPath().getBaseName();
	SL += ":";
	std::stringstream ssl;
	ssl << st.getLine();
	SL += ssl.str();
	//mset.insert ( SL );
	std::set<Thread> st_threads = st.getThreads();
	for (std::set<Thread>:: iterator ti = st_threads.begin();
		ti != st_threads.end(); ti++) {
		Thread t = *ti;
		AddressRange ar = st.getExtentIn(t).getBounds().getAddressRange();  
		std::stringstream ssa;
		ssa << ar.getBegin();
		SL += ":";
		SL += ssa.str();
		break;
	}
	mset.insert ( SL );
      }
    }
  }

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


static bool SS_ListLoopStatements (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;

    std::set<Loop> loopobj = fobj.getLoops();

#ifdef DEBUG_CLI_LOOPS
    std::cerr << "DEBUG: from function.getLoops, loopobj.size()=" << loopobj.size() << std::endl;
#endif

    if( loopobj.size() > 0 ) {
      for (std::set<Loop>:: iterator li = loopobj.begin(); li != loopobj.end(); li++) {
        Loop lpobj = *li;
        std::set<Statement> sobj = lpobj.getStatements();

#ifdef DEBUG_CLI_LOOPS
        std::cerr << "DEBUG: from getStatements, sobj.size()=" << sobj.size() << std::endl;
#endif

        if( sobj.size() > 0 ) {
         for (std::set<Statement>:: iterator si = sobj.begin(); si != sobj.end(); si++) {
           Statement st = *si;
	   std::string SL = st.getPath().getBaseName();
	   SL += ":";
	   std::stringstream ssl;
	   ssl << st.getLine();
	   SL += ssl.str();
	   //mset.insert ( SL );
	   std::set<Thread> st_threads = st.getThreads();
	   for (std::set<Thread>:: iterator ti = st_threads.begin();
		ti != st_threads.end(); ti++) {
		Thread t = *ti;
		AddressRange ar = st.getExtentIn(t).getBounds().getAddressRange();  
		std::stringstream ssa;
		ssa << ar.getBegin();
		SL += ":";
		SL += ssa.str();
		break;
	   }
   	   mset.insert ( SL );
         } // loop through statements
        } // end are there statements
      } // end for loop
    } // end are there loop in this function or module
  } // end function loop

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool SS_ListLoops (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;

    std::set<Loop> loopobj = fobj.getLoops();

#ifdef DEBUG_CLI_LOOPS
    std::cerr << "DEBUG: from function.getLoops, loopobj.size()=" << loopobj.size() << std::endl;
#endif

    if( loopobj.size() > 0 ) {
      for (std::set<Loop>:: iterator li = loopobj.begin(); li != loopobj.end(); li++) {
        Loop lpobj = *li;
        std::set<Statement> loop_definitions = lpobj.getDefinitions();

#ifdef DEBUG_CLI_LOOPS
        std::cerr << "DEBUG: from getDefintions, loop_definitions.size()=" << loop_definitions.size() << std::endl;
#endif
        if( loop_definitions.size() > 0 ) {
         for (std::set<Statement>:: iterator si = loop_definitions.begin(); si != loop_definitions.end(); si++) {
           Statement st = *si;
	   std::string SL = st.getPath().getBaseName();
	   SL += ":";
	   std::stringstream ssl;
	   ssl << st.getLine();
	   SL += ssl.str();
	   //mset.insert ( SL );
	   std::set<Thread> st_threads = st.getThreads();
	   for (std::set<Thread>:: iterator ti = st_threads.begin();
		ti != st_threads.end(); ti++) {
		Thread t = *ti;
		AddressRange ar = st.getExtentIn(t).getBounds().getAddressRange();  
		std::stringstream ssa;
		ssa << ar.getBegin();
		SL += ":";
		SL += ssa.str();
		break;
	   }
   	   mset.insert ( SL );
         } // loop through statements
        } // end are there statements
      } // end for loop
    } // end are there loop in this function or module
  } // end function loop

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool SS_ListFunctions (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;
    mset.insert ( fobj.getName() );
    mset.insert ( fobj.getMangledName() );
  }

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static bool SS_ListMangled (CommandObject *cmd) {
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
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Filter that thread list with any "-f" specification and get a Function list.
  std::set<Function> flset;
  Get_Filtered_Objects (cmd, exp, tgrp, flset);

 // Put the names of all the functions in a set to eliminate duplicates.
  std::set<std::string> mset;
  for (std::set<Function>:: iterator fi = flset.begin(); fi != flset.end(); fi++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      mset.clear();
      break;
    }

    Function fobj = *fi;
    std::string MN = fobj.getMangledName();
    std::string DN = fobj.getDemangledName();
    if (MN != DN) {
	std::string SN = DN;
	SN += ";";
	SN += MN;
	mset.insert ( SN );
    }
    //mset.insert (  );
  }

 // Now we're ready to list the file names.
  for (std::set<std::string>::iterator mseti = mset.begin(); mseti != mset.end(); mseti++) {
    cmd->Result_String ( *mseti );
  }

  exp->Q_UnLock ();

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


/**
 * SemanticRoutine: SS_ListStatus ()
 * 
 * List the current status of the experiment.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complationof the command.
 *
 * @error   "false" return if no experiment can be determined.
 *
 */
static bool SS_ListStatus (CommandObject *cmd) {

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
 * SemanticRoutine SS_ListWallTime ()
 * 
 * Print information about an experiment.
 *     
 * @param   cmd- the CommandObject being processed.
 *
 * @return  "true" if the command was successful.
 *
 * @error   "false" returned if no experiment is specified
 *          or if an error was detected while looking at
 *          the associated database.
 *
 */
bool SS_ListWallTime(CommandObject *cmd) {


    bool All_Keyword = Look_For_KeyWord (cmd, "all");
    if (All_Keyword) {
      //printf("SS_ListWallTime, found all keyword\n");
    }

    bool msTime_Keyword = Look_For_KeyWord (cmd, "mstimeonly");
    if (msTime_Keyword) {
      //printf("SS_ListWallTime, found mstimeonly keyword\n");
    }

    bool Time_Keyword = Look_For_KeyWord (cmd, "timeonly");
    if (Time_Keyword) {
      //printf("SS_ListWallTime, found timeonly keyword\n");
    }

    bool Range_Keyword = Look_For_KeyWord (cmd, "rangeonly");
    if (Range_Keyword) {
      //printf("SS_ListWallTime, found rangeonly keyword\n");
    }

    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, false);
    exp->Determine_Status ();

   try {

    if (exp->FW() != NULL) {

    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    if ((databaseExtent.getTimeInterval().getBegin() == Time::TheBeginning()) ||
        (databaseExtent.getTimeInterval().getBegin() ==
                        databaseExtent.getTimeInterval().getEnd())) {
       cmd->Result_String ("    There is no performance data recorded in the database.");
    } else {
       std::ostringstream lt(std::ios::out);
       std::ostringstream st(std::ios::out);
       std::ostringstream et(std::ios::out);
       Time ST = databaseExtent.getTimeInterval().getBegin();
       Time ET = databaseExtent.getTimeInterval().getEnd();

       int64_t elapsed_time = ((ET - ST));
       int64_t scaled_time = (elapsed_time / 1000000000);
       CommandResult *elapsed_cr ;

       //std::cerr << "BEFORE SETTING UNITS, elapsed_time=" << elapsed_time << " scaled_time=" << scaled_time << std::endl;

       // Determine resulting units.
       std::string UNITS;
       if (scaled_time >= (60 * 60 * 24)) {
         UNITS = "dd:hh:mm:ss";
       } else if (scaled_time >= (60 * 60)) {
         UNITS = "hh:mm:ss";
       } else if (scaled_time >= (60)) {
         UNITS = "mm:ss";
       } else if (scaled_time >= 1) {
         UNITS = "seconds";
       } else if (elapsed_time >= 1000000) {
         UNITS = "ms";
       } else {
         UNITS = "ns";
       }

       //std::cerr << "msTime_Keyword=" << msTime_Keyword << " UNITS=" << UNITS 
       //          << " elapsed_time=" << elapsed_time << " scaled_time=" << scaled_time << std::endl;

       if (msTime_Keyword) {
         double temp_double_elapsed_time = double(elapsed_time);
         if (!strcasecmp(UNITS.c_str(), "ns")) {
            std::cerr << "in ns block, elapsed_time=" << elapsed_time << std::endl;
            // convert ns to ms
            if (elapsed_time >= 1000000) {
              temp_double_elapsed_time = temp_double_elapsed_time / 1000.0  ;
            }
         }

         UNITS = "ms";
         elapsed_cr = new CommandResult_Float (temp_double_elapsed_time);
         //std::cerr << "in ms block, elapsed_time=" << elapsed_time << std::endl;

       } else {

         // not ms adjusted, we are using the base cli tools to give the command time duration
         elapsed_cr = new CommandResult_Duration (elapsed_time);
         //std::cerr << "in not ms adjusted, elapsed_time=" << elapsed_time << std::endl;

       }
         
       // Format elapsed time.
       lt << elapsed_cr->Form();

       delete elapsed_cr;

       st << databaseExtent.getTimeInterval().getBegin();
       et << databaseExtent.getTimeInterval().getEnd();

       if (Time_Keyword || msTime_Keyword) {
          cmd->Result_String (lt.str());
       } else if (Range_Keyword) {
          cmd->Result_String (st.str()
                              + " to "
                              + et.str());
       } else {
          cmd->Result_String ("Performance data spans "
                              + lt.str()
                              + " " + UNITS + "  from "
                              + st.str()
                              + " to "
                              + et.str());
       }
    }

   }
  } // try


  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    cmd->Result_String ( "}");
    exp->Q_UnLock ();
    return false;
  }

  exp->Q_UnLock ();

#if DEBUG_CLI
  printf("In SS_ListWallTime, calling cmd->set_Status(CMD_COMPLETE); before exiting\n");
#endif
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListThreads ()
 * 
 * List the OpenMP or Posix thread Id's for the application
 * attached to an experiment.  If both thread types are present,
 * return the OpenMP thread Id..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" is returned if no experiment can be determined
 *          or the "-f" filter option is specified.
 *
 */
static bool SS_ListThreads (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Threads on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v threads, all' is not supported.");
    return false;
  } else {
   // Get the Threads for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v threads' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<int64_t> tset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        tset.clear();
        break;
      }

      Thread t = *ti;

      std::pair<bool, int> pthread = t.getOpenMPThreadId();
      int64_t pthreadid = 0;
      if (pthread.first) {
        pthreadid = pthread.second;
        tset.insert (pthreadid);
      } else {
        std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
        if (posixthread.first) {
          pthreadid = posixthread.second;
          tset.insert (pthreadid);
        }
      }
    }
    for (std::set<int64_t>::iterator tseti = tset.begin(); tseti != tset.end(); tseti++) {
      cmd->Result_Int ( *tseti );
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

int SS_getNumThreads (CommandObject *cmd, ExperimentObject *exp) {

    int num_threads;

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);


   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<int64_t> tset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        tset.clear();
        break;
      }

      Thread t = *ti;
      int rankid ;

      std::pair<bool, int> prank = t.getMPIRank();
      // MPI, just check for threads with MPI involved
      if (prank.first) {
           rankid = prank.second;

           std::pair<bool, int> pthread = t.getOpenMPThreadId();
           int64_t pthreadid = 0;
           if (pthread.first) {
             // Add one thread to the count
             num_threads = num_threads + 1;
           } else {
             std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
             if (posixthread.first) {
               // Add one thread to the count
               num_threads = num_threads + 1;
             }
           }
      } else {
       // no mpi, just check for threads w/o MPI involved
        std::pair<bool, int> pthread = t.getOpenMPThreadId();
        int64_t pthreadid = 0;
        if (pthread.first) {
          // Add one thread to the count
          num_threads = num_threads + 1;
        } else {
          std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
          if (posixthread.first) {
            // Add one thread to the count
            num_threads = num_threads + 1;
          }
        }
      }
    }

    exp->Q_UnLock ();

    return num_threads;
}

/**
 * SemanticRoutine: SS_ListNumberOfThreads ()
 * 
 * List the OpenMP or Posix thread Id's for the application
 * attached to an experiment.  If both thread types are present,
 * return the OpenMP thread Id..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" is returned if no experiment can be determined
 *          or the "-f" filter option is specified.
 *
 */
static bool SS_ListNumberOfThreads (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  int64_t num_threads=0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Threads on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v threads, all' is not supported.");
    return false;
  } else {
   // Get the Threads for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v threads' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);


   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<int64_t> tset;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        tset.clear();
        break;
      }

      Thread t = *ti;
      int rankid ;

      std::pair<bool, int> prank = t.getMPIRank();
      // MPI, just check for threads with MPI involved
      if (prank.first) {
           rankid = prank.second;

           std::pair<bool, int> pthread = t.getOpenMPThreadId();
           int64_t pthreadid = 0;
           if (pthread.first) {
             // Add one thread to the count
             num_threads = num_threads + 1;
           } else {
             std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
             if (posixthread.first) {
               // Add one thread to the count
               num_threads = num_threads + 1;
             }
           }
      } else {
       // no mpi, just check for threads w/o MPI involved
        std::pair<bool, int> pthread = t.getOpenMPThreadId();
        int64_t pthreadid = 0;
        if (pthread.first) {
          // Add one thread to the count
          num_threads = num_threads + 1;
        } else {
          std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
          if (posixthread.first) {
            // Add one thread to the count
            num_threads = num_threads + 1;
          }
        }
      }
    }

    cmd->Result_Int ( num_threads );
   

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}


/**
 * SemanticRoutine: SS_ListPidsAndThreads ()
 * 
 * List the Process ID and OpenMP or Posix thread Id's for the application
 * attached to an experiment.  If both thread types are present,
 * return the OpenMP thread Id..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" is returned if no experiment can be determined
 *          or the "-f" filter option is specified.
 *
 */
static bool SS_ListPidsAndThreads (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Threads on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v pidsandthreads, all' is not supported.");
    return false;
  } else {
   // Get the Threads for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v pidsandthreads' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<int64_t> tset;
    std::set<std::string> pair_pt_set;
    std::string tmp_pair;
    std::string tmp_pair_p;
    std::string tmp_pair_t;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        pair_pt_set.clear();
        break;
      }

      Thread t = *ti;

      std::pair<bool, int> pthread = t.getOpenMPThreadId();
      int64_t pthreadid = 0;
      if (pthread.first) {
        pthreadid = pthread.second;
        
        std::stringstream pid_out;
        pid_out << t.getProcessId();
        tmp_pair_p = pid_out.str();

        std::stringstream pthread_out;
        pthread_out << pthreadid;
        tmp_pair_t = pthread_out.str();

        tmp_pair = tmp_pair_p + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);

        
      } else {
        std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
        if (posixthread.first) {
          pthreadid = posixthread.second;
        }

        std::stringstream pid_out;
        pid_out << t.getProcessId();
        tmp_pair_p = pid_out.str();

        std::stringstream pthread_out;
        pthread_out << pthreadid;
        tmp_pair_t = pthread_out.str();

        tmp_pair = tmp_pair_p + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);
      }
    }
    for (std::set<std::string>::iterator pair_pt_seti = pair_pt_set.begin(); pair_pt_seti != pair_pt_set.end(); pair_pt_seti++) {
      cmd->Result_String ( *pair_pt_seti );
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}



/**
 * SemanticRoutine: SS_ListRanksAndPids ()
 * 
 * List the Process ID and OpenMP or Posix thread Id's for the application
 * attached to an experiment.  If both thread types are present,
 * return the OpenMP thread Id..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" is returned if no experiment can be determined
 *          or the "-f" filter option is specified.
 *
 */
static bool SS_ListRanksAndPids (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Pids on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v ranksandpids, all' is not supported.");
    return false;
  } else {
   // Get the Pids for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v ranksandpids' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<std::string> pair_pt_set;
    std::string tmp_pair;
    std::string tmp_pair_r;
    std::string tmp_pair_t;
    int rankid = 0;
    bool early_exit_no_ranks = FALSE;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        pair_pt_set.clear();
        break;
      }

      Thread t = *ti;
      pid_t pid = t.getProcessId(); 
      //std::pair<bool, pid_t> pid = t.getProcessId();
      int64_t pid_id = 0;
      if (pid > 0) {
        pid_id = pid;
        
        // There may not be any rank information
        rankid = 0;
        std::pair<bool, int> prank = t.getMPIRank();
        //std::cerr << "looking for ranks, prank.first=" << prank.first << std::endl;
        if (prank.first) {
          rankid = prank.second;
          //std::cerr << "looking for ranks, prank.second=" << prank.second << std::endl;
        } else {
          early_exit_no_ranks = TRUE;
          break;
        }
        std::stringstream rank_out;
        rank_out << rankid;
        tmp_pair_r = rank_out.str();

        std::stringstream pthread_out;
        pthread_out << pid_id;
        tmp_pair_t = pthread_out.str();

        tmp_pair = tmp_pair_r + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);

        
      } else {
        //std::pair<bool, pid_t> pid = t.getProcessId();
        pid_t pid = t.getProcessId();
        if (pid > 0) {
          pid_id = pid;
        }

        // There may not be any rank information
        rankid = 0;
        std::pair<bool, int> prank = t.getMPIRank();
        //std::cerr << "looking for ranks, prank.first=" << prank.first << std::endl;
        if (prank.first) {
          rankid = prank.second;
          //std::cerr << "looking for ranks, prank.second=" << prank.second << std::endl;
        } else {
          early_exit_no_ranks = TRUE;
          break;
        }
        std::stringstream rank_out;
        rank_out << rankid;
        tmp_pair_r = rank_out.str();

        std::stringstream pid_out;
        pid_out << pid_id;
        tmp_pair_t = pid_out.str();

        tmp_pair = tmp_pair_r + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);
      }
    }
    //std::cerr << "early_exit_no_ranks=" << early_exit_no_ranks << std::endl;
    if (early_exit_no_ranks) {
#if 0
         std::string empty1;
         empty1.clear();
         cmd->Result_String ( empty1 );
         cmd->Result_String ( *pair_pt_set );
#endif
    } else {
       for (std::set<std::string>::iterator pair_pt_seti = pair_pt_set.begin(); pair_pt_seti != pair_pt_set.end(); pair_pt_seti++) {
         cmd->Result_String ( *pair_pt_seti );
       }
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListRanksAndThreads ()
 * 
 * List the Process ID and OpenMP or Posix thread Id's for the application
 * attached to an experiment.  If both thread types are present,
 * return the OpenMP thread Id..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" on successful complation of the command.
 *
 * @error   "false" is returned if no experiment can be determined
 *          or the "-f" filter option is specified.
 *
 */
static bool SS_ListRanksAndThreads (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");

  if (All_KeyWord) {
   // List all the Threads on the system.
   // We have decided not to support this option.
    Mark_Cmd_With_Soft_Error(cmd, "'list -v ranksandthreads, all' is not supported.");
    return false;
  } else {
   // Get the Threads for a specified Experiment or the focused Experiment.
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    if (Filter_Uses_F(cmd)) {
      Mark_Cmd_With_Soft_Error(cmd, "'list -v ranksandthreads' does not support the '-f' option.");
      return false;
    }

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

   // Get the list of threads used in the specified experiment.
    ThreadGroup tgrp = exp->FW()->getThreads();
    Filter_ThreadGroup (cmd->P_Result(), tgrp);

   // Place all the thread ID's into a set so each will be listed only once.
    std::set<std::string> pair_pt_set;
    std::string tmp_pair;
    std::string tmp_pair_r;
    std::string tmp_pair_t;
    int rankid = 0;
    bool early_exit_no_ranks = FALSE;
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        pair_pt_set.clear();
        break;
      }

      Thread t = *ti;

      std::pair<bool, int> pthread = t.getOpenMPThreadId();
      int64_t pthreadid = 0;
      if (pthread.first) {
        pthreadid = pthread.second;
        
        // There may not be any rank information
        rankid = 0;
        std::pair<bool, int> prank = t.getMPIRank();
        //std::cerr << "looking for ranks, prank.first=" << prank.first << std::endl;
        if (prank.first) {
          rankid = prank.second;
          //std::cerr << "looking for ranks, prank.second=" << prank.second << std::endl;
        } else {
          early_exit_no_ranks = TRUE;
          break;
        }
        std::stringstream rank_out;
        rank_out << rankid;
        tmp_pair_r = rank_out.str();

        std::stringstream pthread_out;
        pthread_out << pthreadid;
        tmp_pair_t = pthread_out.str();

        tmp_pair = tmp_pair_r + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);

        
      } else {
        std::pair<bool, pthread_t> posixthread = t.getPosixThreadId();
        if (posixthread.first) {
          pthreadid = posixthread.second;
        }

        // There may not be any rank information
        rankid = 0;
        std::pair<bool, int> prank = t.getMPIRank();
        //std::cerr << "looking for ranks, prank.first=" << prank.first << std::endl;
        if (prank.first) {
          rankid = prank.second;
          //std::cerr << "looking for ranks, prank.second=" << prank.second << std::endl;
        } else {
          early_exit_no_ranks = TRUE;
          break;
        }
        std::stringstream rank_out;
        rank_out << rankid;
        tmp_pair_r = rank_out.str();

        std::stringstream pthread_out;
        pthread_out << pthreadid;
        tmp_pair_t = pthread_out.str();

        tmp_pair = tmp_pair_r + ":" + tmp_pair_t ;
        pair_pt_set.insert (tmp_pair);
      }
    }
    //std::cerr << "early_exit_no_ranks=" << early_exit_no_ranks << std::endl;
    if (early_exit_no_ranks) {
#if 0
         std::string empty1;
         empty1.clear();
         cmd->Result_String ( empty1 );
         cmd->Result_String ( *pair_pt_set );
#endif
    } else {
       for (std::set<std::string>::iterator pair_pt_seti = pair_pt_set.begin(); pair_pt_seti != pair_pt_set.end(); pair_pt_seti++) {
         cmd->Result_String ( *pair_pt_seti );
       }
    }

    exp->Q_UnLock ();
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_ListTypes ()
 * 
 * List the names of all available collectors or the collectors
 * used in specifed experiments.
 *     
 * @param   cmd - the CommadnObject being processed.
 *
 * @return  "true" on successful completion of the command.
 *
 * @error   " false" is returned if no experiment can be determined.
 *
 */
static bool SS_ListTypes (CommandObject *cmd) {
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
 * Utility: SS_ViewTypeHint()
 * 
 * Dumps a hint on how to find out more info on
 * a specific viewtype.
 *     
 * @param   CommandObject *cmd.
 *
 * @return  void
 *
 */
static void
SS_ViewTypeHint(CommandObject *cmd)
{

    // More help for the user
    cmd->Result_String("For more information on the viewtypes use the");
    cmd->Result_String("\"help\" command for the specicific view, like:");
    cmd->Result_String("    help pcsamp");

}

/**
 * SemanticRoutine: SS_ListViews()
 * 
 * List all the available views or just the views that can be
 * used with the data collected for an experiment..
 *     
 * @param   CommandObject *cmd.
 *
 * @return  "true" on sucessful completion of the command.
 *
 * @error   "false" is returned if no experiment can be determined.
 *
 */
static bool SS_ListViews (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Look at general modifier types for "all" option.
  Assert(cmd->P_Result() != NULL);
  bool All_KeyWord = Look_For_KeyWord (cmd, "all");
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getExpList();
  CollectorGroup cgrp;

  if (All_KeyWord) {

    SS_ViewTypeHint(cmd);

   // What are all the known views that can be generated?
    SS_Get_Views (cmd);
  } else if (cmd->P_Result()->isExpId()) {
   // What views can be genrated from the information collected in this experiment?
    ExperimentObject *exp = Find_Specified_Experiment (cmd);
    if (exp == NULL) {
      return false;
    }

    SS_ViewTypeHint(cmd);

   // Prevent this experiment from changing until we are done.
    exp->Q_Lock (cmd, true);

    SS_Get_Views (cmd, exp->FW());

    exp->Q_UnLock ();
  } else if (p_slist->begin() != p_slist->end()) {
   // What views depend on a specific collector?
    std::vector<std::string>::iterator si;
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

/**
 * SemanticRoutine: SS_ListGeneric()
 * 
 * Look at general modifier types for list 
 * type modifiers and then call respective
 * list command function.
 *     
 * @param   cmd Standard command object.
 *
 * @return  bool
 *
 * @error   Multiple type modifiers.
 * @error   No type modifiers.
 *
 */
bool SS_ListGeneric (CommandObject *cmd) {
  Assert(cmd->P_Result() != NULL);

 // Look for a recognized option.
  bool first_listtype_found = false;
  bool result_of_first_list = false;
  std::vector<std::string> *p_slist = cmd->P_Result()->getModifierList();
  std::vector<std::string>::iterator j;

  for (j=p_slist->begin();j != p_slist->end(); j++) {
    bool valid_list_type_found = true;
    std::string S = *j;

    if (!strcasecmp(S.c_str(),"all")) {
      continue;
    }

    if (!strcasecmp(S.c_str(),"mstimeonly")) {
      continue;
    }

    if (!strcasecmp(S.c_str(),"timeonly")) {
      continue;
    }

    if (!strcasecmp(S.c_str(),"rangeonly")) {
      continue;
    }

    if (first_listtype_found) {
      std::string s1("More than one valid 'list -v' command argument is not supported.\n");
      s1 = s1 + "The '" + S + "' argument was not processed.\n";
      std::string s2("Try: help list");
      Mark_Cmd_With_Soft_Error(cmd,s1+s2);
      cmd->set_Status(CMD_ERROR);
      break;
    }

    first_listtype_found = true;
    if (!strcasecmp(S.c_str(),"appcommand")) {
      result_of_first_list = SS_ListAppCommand(cmd);
    } else if (!strcasecmp(S.c_str(),"breaks")) {
      result_of_first_list = SS_ListBreaks(cmd);
    } else if (!strcasecmp(S.c_str(),"components")) {
      result_of_first_list = SS_ListComponents(cmd);
    } else if (!strcasecmp(S.c_str(),"config") ||
               !strcasecmp(S.c_str(),"configure") ||
               !strcasecmp(S.c_str(),"configuration") ||
               !strcasecmp(S.c_str(),"preference") ||
               !strcasecmp(S.c_str(),"preferences")) {
      result_of_first_list = SS_ListPreferences(cmd);
    } else if (!strcasecmp(S.c_str(),"database") ||
               !strcasecmp(S.c_str(),"restoredfile")) {
      result_of_first_list = SS_ListDatabase(cmd);
    } else if (!strcasecmp(S.c_str(),"executable") ||
               !strcasecmp(S.c_str(),"executables")) {
      result_of_first_list = SS_ListExecutable(cmd);
    } else if (!strcasecmp(S.c_str(),"exp") ||
               !strcasecmp(S.c_str(),"expid")) {
      result_of_first_list = SS_ListExp(cmd);
    } else if (!strcasecmp(S.c_str(),"exptypes") ||
               !strcasecmp(S.c_str(),"types")) {
      result_of_first_list = SS_ListTypes(cmd);
    } else if (!strcasecmp(S.c_str(),"hosts")) {
      result_of_first_list = SS_ListHosts(cmd);
    } else if (!strcasecmp(S.c_str(),"metrics")) {
      result_of_first_list = SS_ListMetrics(cmd);
    } else if (!strcasecmp(S.c_str(),"mpifunctions")) {
      result_of_first_list = SS_ListMPIFunctions(cmd);
    } else if (!strcasecmp(S.c_str(),"mpicategories")) {
      result_of_first_list = SS_ListMPICategories(cmd);
    } else if (!strcasecmp(S.c_str(),"obj") ||
               !strcasecmp(S.c_str(),"objects") ||
               !strcasecmp(S.c_str(),"linkedobjs")) {
      result_of_first_list = SS_ListObj(cmd);
    } else if (!strcasecmp(S.c_str(),"params") ||
               !strcasecmp(S.c_str(),"param")) {
      result_of_first_list = SS_ListParams(cmd, false, false);
    } else if (!strcasecmp(S.c_str(),"paramvalues") ||
               !strcasecmp(S.c_str(),"paramval") ||
               !strcasecmp(S.c_str(),"paramvals") ||
               !strcasecmp(S.c_str(),"paramsval") ||
               !strcasecmp(S.c_str(),"paramsvals") ||
               !strcasecmp(S.c_str(),"paramsvalue") ||
               !strcasecmp(S.c_str(),"paramsvalues")) {
      result_of_first_list = SS_ListParams(cmd, true, false);
    } else if (!strcasecmp(S.c_str(),"justparamvalues")) {
      result_of_first_list = SS_ListParams(cmd, true, true);
    } else if (!strcasecmp(S.c_str(),"pids") ||
               !strcasecmp(S.c_str(),"processes")) {
      result_of_first_list = SS_ListPids(cmd);
    } else if (!strcasecmp(S.c_str(),"pidsandthreads")) {
      result_of_first_list = SS_ListPidsAndThreads(cmd);
    } else if (!strcasecmp(S.c_str(),"ranks")) {
      result_of_first_list = SS_ListRanks(cmd);
    } else if (!strcasecmp(S.c_str(),"numranks")) {
      result_of_first_list = SS_ListNumberOfRanks(cmd);
    } else if (!strcasecmp(S.c_str(),"ranksandthreads")) {
      result_of_first_list = SS_ListRanksAndThreads(cmd);
    } else if (!strcasecmp(S.c_str(),"ranksandpids")) {
      result_of_first_list = SS_ListRanksAndPids(cmd);
    } else if (!strcasecmp(S.c_str(),"savedviews")) {
      result_of_first_list = SS_ListSavedViews( cmd, Find_Specified_Experiment (cmd) );
    } else if (!strcasecmp(S.c_str(),"src")) {
      result_of_first_list = SS_ListSrc(cmd);
    } else if (!strcasecmp(S.c_str(),"srcfullpath")) {
      result_of_first_list = SS_ListSrcFullPath(cmd);
    } else if (!strcasecmp(S.c_str(),"functions")) {
      result_of_first_list = SS_ListFunctions(cmd);
    } else if (!strcasecmp(S.c_str(),"loops")) {
      result_of_first_list = SS_ListLoops(cmd);
    } else if (!strcasecmp(S.c_str(),"loopstatements")) {
      result_of_first_list = SS_ListLoopStatements(cmd);
    } else if (!strcasecmp(S.c_str(),"mangled")) {
      result_of_first_list = SS_ListMangled(cmd);
    } else if (!strcasecmp(S.c_str(),"statements")) {
      result_of_first_list = SS_ListStatements(cmd);
    } else if (!strcasecmp(S.c_str(),"status")) {
      result_of_first_list = SS_ListStatus(cmd);
    } else if (!strcasecmp(S.c_str(),"threads")) {
      result_of_first_list = SS_ListThreads(cmd);
    } else if (!strcasecmp(S.c_str(),"numthreads")) {
      result_of_first_list = SS_ListNumberOfThreads(cmd);
    } else if (!strcasecmp(S.c_str(),"views")) {
      result_of_first_list = SS_ListViews(cmd);
    } else if (!strcasecmp(S.c_str(),"walltime")) {
      result_of_first_list = SS_ListWallTime(cmd);
    } else if (!strcasecmp(S.c_str(),"cviews")) {
      result_of_first_list = SS_ListCviews(cmd);
    } else {
      valid_list_type_found = false;
      first_listtype_found = false;
    }

    if (!valid_list_type_found) {
      std::string s1("Non valid list command argument '");
      s1 = s1 + S + "'.\n";
      std::string s2("Try: help list");
      Mark_Cmd_With_Soft_Error(cmd,s1+s2);
      cmd->set_Status(CMD_ERROR);
    }
  }

  if (!first_listtype_found) {
//  cmd->P_Result()->setError("Non valid list command argument.");
    std::string s1("Missing list command argument.\n");
    std::string s2("Try: help list");
    Mark_Cmd_With_Soft_Error(cmd,s1+s2);
    cmd->set_Status(CMD_ERROR);
  }

  return result_of_first_list;
}

// Session Commands

/**
 * SemanticRoutine: SS_ClearBreaks ()
 * 
 * Not yet implemented.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true"
 *
 */
bool SS_ClearBreaks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("'clearBreak' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_Echo()
 *
 * Take the input string and send it back as a result..
 *
 * @param   cmd - the commadn being processed.
 *
 * @return  true to indicate successful completionof the command.
 *
 */
bool SS_Echo (CommandObject *cmd) {
  std::vector<std::string> *S = cmd->P_Result()->getStringList();
  std::vector<std::string>::iterator si;
  for (si = S->begin(); si != S->end(); si++) {
    cmd->Result_String(*si);
  }
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_Exit ()
 * 
 * Implement the 'exit' command by starting the shut down process.
 * All preceeding commands must be allowed to complete and all
 * following commands must not be allowed to start execution.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" to indicate successful completion of the command.
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
 // When python attempts to execute a command, it first
 // checks the global "Shut_Down" flag and will exit if
 // the flag is set.
 //
 // Setting the flag will also cause the input controler to
 // issues a second "Exit" command, which gives phtyon the
 // oppertunity to check the flag, terminate and return.
 // (Note: It doesn't need to be an "Exit", any command will do the job.)
  Shut_Down = true;

 // There is no result returned from this command.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_Help ()
 * 
 * Implement the 'help' command by calling 'dumpHelp'..
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" to indicate successful completion of the command.
 *
 */
bool SS_Help (CommandObject *cmd) {
  cmd->P_Result()->dumpHelp(cmd);
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_History ()
 * 
 * Implement the 'history' command by printing the list of
 * commands that was sent to Python..
 *     
 * @param   cmd - the CommandObbject being processed.
 *
 * @return  "true" to indicate successful completion of the command.
 *
 * @error   if the number of records requested is negative, ignore it
 *          and list all remembered commands.
 * @error   if the number of records requested is larger than the current
 *          limit on the number of remembered records, list all the remembered
 *          ones and increase the limit.
 *
 */
bool SS_History (CommandObject *cmd) {

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();
  int64_t num = OPENSS_HISTORY_DEFAULT;  // How many to print out

 // Copy commands from the history list.
  std::list<std::string>::iterator hi = History.begin();  // Start at  beginning

 // The user may ask us to limit the output
  std::vector<ParseRange> *c_limit = cmd->P_Result()->getHistoryList();
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
 * SemanticRoutine: SS_Log ()
 * 
 * Implement the 'log' command by setting the mechanisms that
 * will cause Openss to echo state changes for each command
 * that originated from the same input source that this command
 * was from.
 *
 * The command can "turn on" logging if a file is specifed and
 * will "turn off" logging if no file is specified with the command.
 *     
 * @param   cmd - the CommandObject for this command.
 *
 * @return  "true" to indicate successful completion of the command.
 *
 * @error   "false" is returned if the input source could not be determined
 *          or is no longer active.
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
 * SemanticRoutine: SS_OpenGui ()
 * 
 * Implement the 'opengui' command.  Be sure an source input window
 * has been defined for the gui to use and then call the same routine
 * to load and initiate the GUI that would have been called if it had
 * been opened on start up.
 *     
 * @param   cmd - the CommandObject beng processed.
 *
 * @return  "true" to indicate (apparently) successful completion.
 *
 * @error   "false" is returned if too many source input windows
 *          have already been defined and the GUI can not be loaded.
 *
 */
bool SS_OpenGui (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Load the GUI
 // How do we check to see if is already loaded?
  int argc = 0;
  const char **argv = NULL;
  ArgStruct *argStruct = new ArgStruct(argc, argv);
  if (gui_window == 0) {
   // The GUI was not opened before so we need to define an input control window for it.
    char HostName[MAXHOSTNAMELEN+1];
    if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
  	Mark_Cmd_With_Soft_Error(cmd, "Can not retreive host name.");
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

 // Call the startup routine for the GUI.
  loadTheGUI((ArgStruct *)argStruct);

 // The GUi will be spun off into it's own process.
 // As far as we can tell, this command is always successful.
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_Playback ()
 * 
 * Implement the 'playback' command by redirecting the input
 * windows to read from a new file.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" if input has been successfully redirected.
 *
 * @error   "false" is returned if the user failured to provide
 *           a single, valid filename.
 *
 */
bool SS_Playback (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  parse_val_t *f_val = Get_Simple_File_Name (cmd);

  if (f_val == NULL) {
    Mark_Cmd_With_Soft_Error(cmd, "Can not determine file name.");
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
 * SemanticRoutine: SS_Record ()
 * 
 * Implement the 'record' command by setting flags so that each
 * executed command is echoed to a file at the same point it is
 * passed into Python.  Recording is turned off when a 'playback'
 * command that has no "-f" specification is executed, but only
 * after waiting for all previously issued commands to complete.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true" to indicate successful completion.
 *
 * @error   "false" is returned if specified file could not be opened
 *          for writing.
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
 * SemanticRoutine: SS_SetBreak ()
 * 
 * Not yet implemented.
 *     
 * @param   cmd - the CommandObject being processed.
 *
 * @return  "true"
 *
 */
bool SS_SetBreak (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("'setBreak' is not yet implemented");
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

/**
 * SemanticRoutine: SS_Wait ()
 * 
 * Implement the 'wait' command by waiting for preceeding commands to
 * complete and for the execution of specified experiments to stop.
 * No other commands may be executed until the above conditions are satisfied.
 *     
 * @param   .
 *
 * @return  "true" on successful completion or if a thrown exception
 *          was caught when we attempt to flush performance data to
 *          the datbase.
 *
 * @error   "false" is returned if a needed experiment can not be determined.
 *
 */
bool SS_Wait (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Wait for all executing commands to terminate.
  Wait_For_Previous_Cmds ();

  if (cmd->P_Result()->getModifierList()->empty() ||
      Look_For_KeyWord (cmd, "terminate")) {
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
     // Be sure that write buffers are actually written to the database.
      try {
        exp->FW()->flushPerformanceData();
      }
      catch (const Exception& error) {
       // Ignore any errors and let them be regenerated when we try to do something else.
      }
    }
  } else if ( Look_For_KeyWord (cmd, "all")) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
     // Wait for the execution of the experiment to terminate.
      Wait_For_Exp (cmd, *expi);
     // Be sure that write buffers are actually written to the database.
      try {
        (*expi)->FW()->flushPerformanceData();
      }
      catch (const Exception& error) {
       // Ignore any errors and let them be regenerated when we try to do something else.
      }
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


