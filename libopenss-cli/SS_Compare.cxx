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

static int64_t Get_Trailing_Int (std::string viewname, int64_t start) {
  int64_t topn = 0;
  for (int i = start; i < viewname.length(); i++) {
    char c = viewname[i];
    if ((c >= *"0") && (c <= *"9")) {
      topn = (topn * 10) + (c - *"0");
    }
  }
  return topn;
}

struct ltCR {
  bool operator() (CommandResult *CR1, CommandResult *CR2) {
    return CommandResult_lt (CR1, CR2);
  }
};

// Data used to track Custom View definitions.
pthread_mutex_t CustomView_List_Lock;
class CustomView;
std::list<CustomView *> CustomView_List;

class CustomView
{
 private:
  EXPID Exp_ID;
  OpenSpeedShop::cli::ParseResult *p_result;
  Framework::ThreadGroup Custom_tgrp;

 public:
  CustomView (OpenSpeedShop::cli::ParseResult *p) {
    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
    Exp_ID = ++Experiment_Sequence_Number;
    p_result = p;
    CustomView_List.push_front(this);
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);
  }
  CustomView (OpenSpeedShop::cli::ParseResult *p,
              const Framework::ThreadGroup& tgrp) {
    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
    Exp_ID = ++Experiment_Sequence_Number;
    p_result = p;
    Custom_tgrp = tgrp;
    CustomView_List.push_front(this);
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);
  }
  ~CustomView () {
    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
    Exp_ID = 0;
    OpenSpeedShop::cli::ParseResult *p = p_result;
    CustomView_List.remove (this);
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);
    delete p;
  }

  EXPID cvId () { return Exp_ID; }
  OpenSpeedShop::cli::ParseResult *cvPr () { return p_result; }
  Framework::ThreadGroup *cvTgrp() { return &Custom_tgrp; }
};

CustomView *Find_CustomView (EXPID ID)
{
// Search for existing entry.
  if (ID > 0) {
    std::list<CustomView *>::iterator cvi;
    for (cvi = CustomView_List.begin(); cvi != CustomView_List.end(); cvi++) {
      if (ID == (*cvi)->cvId()) {
        return *cvi;
      }
    }
  }

  return NULL;
}

// Data used to generate a view from customized definitions.
struct selectionTarget {
  std::string headerPrefix;
  int64_t numColumns;
  OpenSpeedShop::cli::ParseResult *pResult;
  ExperimentObject *Exp;
  ThreadGroup *base_tgrp;
  std:: string viewName;
  std::string hostId;
  pid_t pidId;
  pthread_t threadId;
  int64_t rankId;
  std::list<CommandResult *> partial_view;
  std::map<int64_t, CommandResult *> merge_map;

  selectionTarget () {
    headerPrefix = "";
    numColumns = 0;
    pResult = NULL;
    Exp = NULL;
    viewName = "";
    hostId = "";
    pidId = 0;
    threadId = 0;
    rankId = 0;
  }

  selectionTarget (const selectionTarget& S) {
    headerPrefix = S.headerPrefix;
    numColumns = S.numColumns;
    pResult = S.pResult;
    base_tgrp = S.base_tgrp;
    Exp = S.Exp;
    viewName = S.viewName;
    hostId = S.hostId;
    pidId = S.pidId;
    threadId = S.threadId;
    rankId = S.rankId;
  }

  void Print (ostream& to) {
    if (pResult != NULL) {
      pResult->ParseResult::dumpInfo();
    } else {
      if (Exp != NULL) to << " -x " << Exp->ExperimentObject_ID();
      if (viewName != "") to << " " << viewName;
      if (hostId != "") to << " -h " << hostId;
      if (pidId) to << " -p " << pidId;
      if (threadId) to << " -t " << threadId;
      if (rankId) to << " -r " << rankId;
    }
    to << " (numColumns = " << numColumns << ")";
    to << " prefix = " << headerPrefix;
    to << std::endl;
  }
};

/**
 * Method: Selectr_ThreadGroup()
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
 * @param CommandObject *cmd - containing the parse object.
 *        ThreadGroup& tgrp  - the original set of threads.
 *
 * @return  void, but one of the input arguments is altered.
 *
 */
void Select_ThreadGroup (selectionTarget& S, ThreadGroup& base_grp, ThreadGroup& rgrp) {

   // Go through every thread and decide if it is included.
    ThreadGroup::iterator ti;
    for (ti = base_grp.begin(); ti != base_grp.end(); ti++) {
      Thread t = *ti;
      bool include_thread = true;

      if (S.hostId != "") {
       // Does it match the host?
        std::string hid = Experiment::getCanonicalName(t.getHost());
        include_thread = (hid == S.hostId);
      }

      if (include_thread && (S.pidId != 0)) {
       // Does it match the pid?
        pid_t pid = t.getProcessId();
        include_thread = (pid == S.pidId);
      }

      if (include_thread && (S.threadId != 0)) {
       // Does it match a pthread ID?
        std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
        if (pthread.first) {
          int64_t tid = pthread.second;
          include_thread = (tid == S.threadId);
        } else include_thread = false;
      }

      if (include_thread && (S.rankId != 0) && t.getOpenMPThreadId().first) {
       // Does it match a rank ID?
        int64_t rid = t.getOpenMPThreadId().second;
        include_thread = (rid == S.rankId);
      } else if (include_thread && (S.rankId != 0) && t.getMPIRank().first) {
       // Does it match a rank ID?
        int64_t rid = t.getMPIRank().second;
        include_thread = (rid == S.rankId);
      } else include_thread = false;

     // Add matching threads to rgrp.
      if (include_thread) {
        rgrp.insert(t);
      }

    }

}

static bool Generate_CustomView (CommandObject *cmd,
                                 std::vector<selectionTarget>& Quick_Compare_Set) {
  int64_t numQuickSets = Quick_Compare_Set.size();
  int64_t i;

  if (numQuickSets == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "There are no valid comparisons specified.");
    return false;
  }

/* TEST */
/*
  cerr << "Number of Quick_Compare_Sets is " << numQuickSets << std::endl;
  for (i = 0; i < numQuickSets; i++) {
    cerr << i;
    Quick_Compare_Set[i].Print(cerr);
  }
*/


 // Generate all the views in the list.
  for (i = 0; i < numQuickSets; i++) {
   // Try to Generate the Requested View for each comparison set!
    ExperimentObject *exp = Quick_Compare_Set[i].Exp;
    std::string viewname = Quick_Compare_Set[i].viewName;
    ViewType *vt = Find_View (viewname);
    if (vt == NULL) {
      std::ostringstream M;
      M << "The requested view, '" << viewname << "' is not available.";
      Mark_Cmd_With_Soft_Error(cmd, M.ostringstream::str());
      return false;
    }

    OpenSpeedShop::cli::ParseResult *sResult = NULL;
    ThreadGroup tgrp;
    if (Quick_Compare_Set[i].pResult != NULL) {
     // Use the parse object from the cView definition.
      sResult = cmd->swapParseResult (Quick_Compare_Set[i].pResult);
      if ((Quick_Compare_Set[i].base_tgrp != NULL) &&
          (!Quick_Compare_Set[i].base_tgrp->empty())) {
        tgrp = *Quick_Compare_Set[i].base_tgrp;
      } else {
       // Get all the threads that are in the experiment.
        tgrp = exp->FW()->getThreads();
       // Retain only the threads that may be of interest with this parse object.
        Filter_ThreadGroup (cmd->P_Result(), tgrp);
      }
    } else {
     // Get all the threads that are in the experiment.
        ThreadGroup base_tgrp = exp->FW()->getThreads();
     // Retain only the threads that may be of interest.
      Filter_ThreadGroup (cmd->P_Result(), base_tgrp);
     // Select specific ones.
      Select_ThreadGroup (Quick_Compare_Set[i], base_tgrp, tgrp);
    }
    if (tgrp.size() > 0) {
      bool success = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                                       tgrp, Quick_Compare_Set[i].partial_view);
      if (!success) {
        Reclaim_CR_Space (Quick_Compare_Set[i].partial_view);
        Quick_Compare_Set[i].partial_view.clear();
      }
    }
    if (sResult != NULL) {
     // Restore the original parse object.
      (void)(cmd->swapParseResult (sResult));
    }
  }

  if (numQuickSets == 1 ) {
   // There is only one valid result. Treat this case as a normal view.
    cmd->Result_Predefined (Quick_Compare_Set[0].partial_view);
    return true;
  }

/* TEST */
/*
  cerr << "Number of Quick_Compare_Sets is " << numQuickSets << std::endl;
  for (i = 0; i < numQuickSets; i++) {
    cerr << i;
    Quick_Compare_Set[i].Print(cerr);
  }
*/

/* TEST */
/*
  cerr << "Display generated views\n";
  for (i = 0; i < numQuickSets; i++) {
    cerr << "Set " << i << "\n";
    if (Quick_Compare_Set[i].partial_view.empty()) {
      cerr << "   (empty)\n";
    } else {
      std::list<CommandResult *>::iterator coi;
      for (coi = Quick_Compare_Set[i].partial_view.begin();
           coi != Quick_Compare_Set[i].partial_view.end();
           coi++) {
        CommandResult *c = *coi;
        c->Print(cerr);
        cerr << "\n";
      }
    }
  }
*/

// Merge the results from each separate view into a single view.

 // Start by determining the new column headers.
  int64_t num_columns = 0;
  int64_t rows_in_Set0 = 0;
  CommandResult_Columns *C = new CommandResult_Columns ();
  CommandResult *last_header = NULL;
  std::list<CommandResult *>::iterator coi;
  for (i = 0; i < numQuickSets; i++) {
    for (coi = Quick_Compare_Set[i].partial_view.begin(); coi != Quick_Compare_Set[i].partial_view.end(); coi++) {
      CommandResult *c = *coi;
      if (c->Type() == CMD_RESULT_COLUMN_HEADER) {
        int64_t cnt = 0;
        ((CommandResult_Headers *)c)->Value(cnt);
        Quick_Compare_Set[i].numColumns = cnt - 1;
        if (cnt > 1) {
          num_columns += (cnt - 1);
          std::list<CommandResult *> H;
          ((CommandResult_Headers *)c)->Value(H);
          std::list<CommandResult *>::iterator hi;
          for (hi = H.begin(), cnt--; (hi != H.end()) && (cnt > 0); hi++, cnt--) {
            std::string s;
            ((CommandResult_String *)(*hi))->Value(s);
            C->CommandResult_Columns::Add_Column (
                      new CommandResult_String (Quick_Compare_Set[i].headerPrefix + s));
          }
          if (last_header == NULL) {
           // Save the first right-most column header for the final report.
            last_header = *hi;
          }
        }
      } else if (c->Type() == CMD_RESULT_COLUMN_VALUES) {
        if (i == 0) rows_in_Set0++;
      }
    }
  }

  if (num_columns == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There was no data available to compare.)");
    return false;
  }

/* TEST */
/*
  cerr << "Number of Quick_Compare_Sets is " << numQuickSets << std::endl;
  for (i = 0; i < numQuickSets; i++) {
    cerr << i;
    Quick_Compare_Set[i].Print(cerr);
  }
*/

 // Add the header for the last column and attach all of them to the output report.
  C->CommandResult_Columns::Add_Column ( Dup_CommandResult (last_header) );
  cmd->Result_Predefined (C); // attach column headers to output

 // Build the master maps.
 // The master_map associates a CommandResult * with an index.
 // The index is into master_vector, which points to the Function, Statement, LinkedObject, ...
  int64_t num_rows = 0;
  std::vector<CommandResult *> master_vector(rows_in_Set0);
  std::map<CommandResult *, int64_t, ltCR> master_map;

 // Initial the master maps with information from Quick_Compare_Set[0].
  for (coi = Quick_Compare_Set[0].partial_view.begin(); coi != Quick_Compare_Set[0].partial_view.end(); coi++) {
    CommandResult *c = *coi;
    if (c->Type() == CMD_RESULT_COLUMN_VALUES) {
      std::list<CommandResult *> L;
      ((CommandResult_Headers *)c)->Value(L);
      CommandResult *last_column = NULL;
      std::list<CommandResult *>::iterator li;
      for (li = L.begin(); li != L.end(); li++) { last_column = *li; }
      Assert (last_column != NULL);
      master_map[last_column] = num_rows;
      master_vector[num_rows++] = last_column;
    }
  }
  int64_t Set0_rows = num_rows;

 // Build maps for all but the first compare sets.
 // These maps associate the right most column value with the index into the master map.
  for (i = 1; i < numQuickSets; i++) {
    std::list<CommandResult *>::iterator coi;
    for (coi = Quick_Compare_Set[i].partial_view.begin(); coi != Quick_Compare_Set[i].partial_view.end(); coi++) {
      CommandResult *c = *coi;
      if (c->Type() == CMD_RESULT_COLUMN_VALUES) {
        std::list<CommandResult *> L;
        ((CommandResult_Columns *)c)->Value(L);
        CommandResult *last_column = NULL;
        std::list<CommandResult *>::iterator li;
        for (li = L.begin(); li != L.end(); li++) { last_column = *li; }
        Assert (last_column != NULL);
        std::map<CommandResult *, int64_t, ltCR>::iterator result = master_map.find(last_column);
        int64_t master_index = -1;
        if (master_map.end() == result) {
         // Need to add a new entry into the master.
          master_index = num_rows;
          master_map[last_column] = num_rows;
          master_vector.push_back (last_column);
          num_rows++;
        } else {
         // Use the index to an existing entry.
          master_index = (*result).second;
        }
       // Map the master_vector index to the start of data for this entry.
        Assert (Quick_Compare_Set[i].merge_map[master_index] == NULL);
        Quick_Compare_Set[i].merge_map[master_index] = c;
      }
    }
  }

 // Merge the values from the various compare sets.
  int64_t Set0_Columns = Quick_Compare_Set[0].numColumns;
  std::list<CommandResult *> Set0 = Quick_Compare_Set[0].partial_view;
  std::list<CommandResult *>::iterator Set0i = Set0.begin();
  for (int64_t rc = 0; rc < master_vector.size(); rc++) {
    CommandResult_Columns *NC = new CommandResult_Columns ();

   // Get data for set[0].
    if (rc < Set0_rows) {
     // Find the next set of column data.
      while ((*Set0i)->Type() != CMD_RESULT_COLUMN_VALUES) { Set0i++; }
     // Copy original view data into the compare report.
      std::list<CommandResult *> DL;
      ((CommandResult_Columns *)(*Set0i))->Value(DL);
      Assert (Set0_Columns == (DL.size() - 1));
      std::list<CommandResult *>::iterator Di = DL.begin();
      for (int64_t j = 0; j < Set0_Columns; j ++, Di++) {
        NC->CommandResult_Columns::Add_Column ( Dup_CommandResult (*Di) );
      }
      Set0i++;
    } else {
     // Fill the columns with place holders.
      for (int64_t j = 0; j < Set0_Columns; j++) {
        NC->CommandResult_Columns::Add_Column ( CRPTR("") );
      }
    }

   // Get data for other sets.
    for (i = 1; i < numQuickSets; i++) {
      int64_t numColumns = Quick_Compare_Set[i].numColumns;
      std::map<int64_t, CommandResult *>::iterator result = Quick_Compare_Set[i].merge_map.find(rc);
      if (result != Quick_Compare_Set[i].merge_map.end()) {
       // Copy the column data from the original view.
        CommandResult *C = (*result).second;
        std::list<CommandResult *> DL;
        ((CommandResult_Columns *)C)->Value(DL);
        std::list<CommandResult *>::iterator Di = DL.begin();
        for (int64_t j = 0; j < numColumns; j ++, Di++) {
          NC->CommandResult_Columns::Add_Column ( Dup_CommandResult (*Di) );
        }
      } else {
       // Fill the columns with place holders.
        for (int64_t j = 0; j < numColumns; j++) {
          NC->CommandResult_Columns::Add_Column ( CRPTR("") );
        }
      }
    }

    NC->CommandResult_Columns::Add_Column ( Dup_CommandResult (master_vector[rc]) );
    cmd->Result_Predefined (NC);
  }

 // Free remaining space for CommandResults.
  for (i = 0; i < numQuickSets; i++) {
    Reclaim_CR_Space (Quick_Compare_Set[i].partial_view);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expCompare (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  Assert(cmd->P_Result() != NULL);

  int64_t i;
  std::vector<selectionTarget> Quick_Compare_Set;

 // Pick up the <expID> from the command.
 // Note: we must always generate at least one compare set for this list.
  vector<ParseRange> *p_elist = cmd->P_Result()->getExpIdList();
  if (p_elist->begin() == p_elist->end()) {
   // The user has not selected a view look for the focused experiment.
    EXPID ExperimentID = Experiment_Focus ( WindowID );
    ExperimentObject *exp = (ExperimentID != 0) ?
                            Find_Experiment_Object (ExperimentID) :
                            NULL;
    if ((exp == NULL) ||
        (exp->FW() == NULL)) {
     // No experiment was specified, so we can't find a useful view to generate.
      Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified.");
      return false;
    } else {
     // Initialize a single compare set for the focused experiment.
      selectionTarget S;
      S.Exp = exp;
      Quick_Compare_Set.push_back (S);
    }
  } else {
   // Generate compare sets for every experiment.
    vector<ParseRange>::iterator ei;
    for (ei = p_elist->begin(); ei != p_elist->end(); ei++) {
     // Determine the experiment needed for the next set.
      parse_range_t *e_range = (*ei).getRange();
      Assert (e_range != NULL);
      parse_val_t eval1 = e_range->start_range;
      parse_val_t eval2 = (e_range->is_range) ? e_range->end_range : eval1;
      Assert (eval1.tag == VAL_NUMBER);

      for (int64_t i = eval1.num; i <= eval2.num; i++) {
        EXPID ExperimentID = i;
        ExperimentObject *exp = (ExperimentID != 0) ?
                              Find_Experiment_Object (ExperimentID) :
                              NULL;
        if ((exp == NULL) ||
            (exp->FW() == NULL)) {
         // This is an error and should be reported.
          std::ostringstream M;
          M << "Experiment ID '-x " << ExperimentID << "' is not valid.";
          Mark_Cmd_With_Soft_Error(cmd, M.ostringstream::str());
          return false;
        }

       // Define new compare sets for each experiment.
        std::ostringstream M;
        M << "-x " << ExperimentID << ": ";
        selectionTarget S;
        S.Exp = exp;
        S.headerPrefix = M.ostringstream::str();
        Quick_Compare_Set.push_back (S);
      }

    }

  }


 // Pick up the <viewType> from the command or the experiment
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  if (p_slist->begin() == p_slist->end()) {
   // The user has not selected a view.
   // Look for a view that would be meaningful.
    for (int64_t k = 0; k < Quick_Compare_Set.size(); k++) {
      ExperimentObject *exp = Quick_Compare_Set[k].Exp;

      CollectorGroup cgrp = exp->FW()->getCollectors();
      if (cgrp.begin() == cgrp.end()) {
       // No collector was used.
        Mark_Cmd_With_Soft_Error(cmd, "No performance measurements were made for the experiment.");
        return false;
      }

      bool view_found = false;
      std::vector<std::string> views;
      CollectorGroup::iterator cgi;
      for (cgi = cgrp.begin(); cgi != cgrp.end(); cgi++) {
       // See if there is a view by the same name.
        Collector c = *cgi;
        Metadata m = c.getMetadata();
        std::string collector_name = m.getUniqueId();
        ViewType *vt = Find_View (collector_name);
        if (vt != NULL) {
          view_found = true;
         // Generate a view for every collector.
          views.push_back (collector_name);
        }
      }

      if (!view_found) {
       // Nothing defined - we can't generate a report.
        std::ostringstream M;
        M << "No usable view available for '-x " << exp->ExperimentObject_ID() << "'.";
        Mark_Cmd_With_Soft_Error(cmd, M.ostringstream::str());
        return false;
      }

      if ((Quick_Compare_Set.size() > 1) && (views.size() > 1)) {
       // Too much stuff - we don't know what to compare.
        std::ostringstream M;
        M << "Multiple views possible for '-x " << exp->ExperimentObject_ID() << "'.";
        Mark_Cmd_With_Soft_Error(cmd, M.ostringstream::str());
        return false;
      }

     // For every viewname, create a compare set.
      if (views.size() == 1) {
       // One view for this experiment.
       // Add the view to the compare set for this experiment.
        Quick_Compare_Set[k].viewName = views[0];
        Quick_Compare_Set[k].headerPrefix += views[0] + ": ";


       // Continue the "k" loop to find one view for each experiment.
        continue;
      } else {
       // Multiple views, but only one experiment.
        int64_t j;

       // Create enough compare sets for all the views.
        for (j = 1; j < views.size(); j++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
        }

       // Fill in the view for each compare set.
        for (j = 0; j < views.size(); j++) {
          Quick_Compare_Set[j].viewName = views[j];
          Quick_Compare_Set[j].headerPrefix += views[j] + ": ";
        }

       // Don't continue the "k" loop.
       // There was originaly one experiment, but we just created
       // multiple compare sets, so out original loop is invalid.
        break;
      }

    }
  } else {
   // Generate all the views in the list.

    std::vector<std::string> views;
    vector<string>::iterator vi;
    for (vi=p_slist->begin();vi != p_slist->end(); vi++) {
      std::string viewname = *vi;
      ViewType *vt = Find_View (viewname);
      if (vt == NULL) {
        std::ostringstream M;
        M << "The requested view, '" << viewname << "' is not available.";
        Mark_Cmd_With_Soft_Error(cmd, M.ostringstream::str());
        return false;
      }
      views.push_back (viewname);
    }

   // For every viewname, create a compare set.
    if ((Quick_Compare_Set.size() > 1) && (views.size() > 1)) {
      Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
      return false;
    }
    int64_t initialSetCnt = Quick_Compare_Set.size();
    int64_t segmentStart = 0;
    for (int64_t j = 0; j < views.size(); j++) {
      if (j < (views.size() -1 )) {
       // There is at least one more iteration, so
       // preserve a copy of the original sets.
        for (int64_t k = 0; k < initialSetCnt; k++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[j]);
        }
      }

      for (int64_t k = 0; k < initialSetCnt; k++) {
        Quick_Compare_Set[segmentStart+k].viewName = views[j];
        Quick_Compare_Set[segmentStart+k].headerPrefix += views[j] + ": ";
      }
      segmentStart += initialSetCnt;
    }

  }
  

 // The <target_spec> will define the comparisons.
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->size() > 1) {
   // There are no filters.
    Mark_Cmd_With_Soft_Error(cmd, "There is more than one target specification.");
    return false;
  }

 // Scan the <target_spec> and determine the comparison sets.
  vector<ParseRange> *h_list = NULL;
  vector<ParseRange> *p_list = NULL;
  vector<ParseRange> *t_list = NULL;
  vector<ParseRange> *r_list = NULL;
  if ((p_tlist != NULL) && (!(*p_tlist).empty())){
    ParseTarget pt = (*p_tlist)[0];
    h_list = pt.getHostList();
    p_list = pt.getPidList();
    t_list = pt.getThreadList();
    r_list = pt.getRankList();
  }

  if (!((h_list == NULL) || h_list->empty())) {
   // Start by building a vector of all the host names.
    std::vector<std::string> hosts;
    vector<ParseRange>::iterator hi;
    for (hi=h_list->begin();hi != h_list->end(); hi++) {
      parse_range_t *pr = hi->getRange();
      parse_val_t pval1 = pr->start_range;
      Assert (pval1.tag == VAL_STRING);
      std::string hid = Experiment::getCanonicalName(pval1.name);
      if (pr->is_range) {
        Assert (!pr->is_range);
      } else {
        hosts.push_back (pval1.name);
      }
    }

    int64_t initialSetCnt = Quick_Compare_Set.size();
    if (initialSetCnt == 0) {
     // Define new compare sets for each host.
      for (int64_t j = 0; j < hosts.size(); j++) {
        selectionTarget S;
        S.headerPrefix = "-h " + hosts[j] + ": ";
        S.hostId = Experiment::getCanonicalName(hosts[j]);
        Quick_Compare_Set.push_back (S);
      }
    } else {
      if ((initialSetCnt > 1) && (hosts.size() > 1)) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      }
      int64_t segmentStart = 0;
      for (int64_t j = 0; j < hosts.size(); j++) {
        if (j < (hosts.size() -1 )) {
         // There is at least one more iteration, so
         // preserve a copy of the original sets.
          for (int64_t k = 0; k < initialSetCnt; k++) {
            Quick_Compare_Set.push_back (Quick_Compare_Set[segmentStart+k]);
          }
        }

       // Append this host to N copies of the original sets.
        for (int64_t k = 0; k < initialSetCnt; k++) {
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += "-h " + hosts[j] + ": ";
          Quick_Compare_Set[segmentStart+k].hostId = Experiment::getCanonicalName(hosts[j]);
        }
        segmentStart += initialSetCnt;
      }

    }
  }

  if (!((p_list == NULL) || p_list->empty())) {
   // Start by building a vector of all the pids.
    std::vector<pid_t> pids;;
    vector<ParseRange>::iterator pi;
    for (pi = p_list->begin();pi != p_list->end(); pi++) {
      parse_range_t *p_range = pi->getRange();
      parse_val_t *pval1 = &p_range->start_range;
      parse_val_t *pval2 = p_range->is_range ? &p_range->end_range : pval1;

      pid_t mypid;
      for ( mypid = pval1->num; mypid <= pval2->num; mypid++) {
        pids.push_back (mypid);
      }

    }

    int64_t initialSetCnt = Quick_Compare_Set.size();
    if (initialSetCnt == 0) {
     // Define new compare sets for each host.
      for (int64_t j = 0; j < pids.size(); j++) {
        int64_t P = pids[j]; char s[40]; sprintf ( s, "-p %lld: ", P);
        selectionTarget S;
        S.headerPrefix = s;
        S.pidId = pids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
      if ((initialSetCnt > 1) && (pids.size() > 1)) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      }
      int64_t segmentStart = 0;
      for (int64_t j = 0; j < pids.size(); j++) {
        if (j < (pids.size() -1 )) {
         // There is at least one more iteration, so
         // preserve a copy of the original sets.
          for (int64_t k = 0; k < initialSetCnt; k++) {
            Quick_Compare_Set.push_back (Quick_Compare_Set[segmentStart+k]);
          } 
        }

       // Append this pid to N copies of the original sets.
        for (int64_t k = 0; k < initialSetCnt; k++) {
          int64_t P = pids[j]; char s[40]; sprintf ( s, "-p %lld: ", P);
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += s;
          Quick_Compare_Set[segmentStart+k].pidId = pids[j];
        }
        segmentStart += initialSetCnt;
      }
    }
  }

  if (!((t_list == NULL) || t_list->empty())) {
   // Start by building a vector of all the host names.
    std::vector<int64_t> threadids;;
    vector<ParseRange>::iterator pi;
    for (pi = t_list->begin();pi != t_list->end(); pi++) {
      parse_range_t *p_range = pi->getRange();
      parse_val_t *pval1 = &p_range->start_range;
      parse_val_t *pval2 = p_range->is_range ? &p_range->end_range : pval1;

      int64_t mythreadid;
      for ( mythreadid = pval1->num; mythreadid <= pval2->num; mythreadid++) {
        threadids.push_back (mythreadid);
      }

    }

    int64_t initialSetCnt = Quick_Compare_Set.size();
    if (initialSetCnt == 0) {
     // Define new compare sets for each host.
      for (int64_t j = 0; j < threadids.size(); j++) {
        char s[40]; sprintf ( s, "-t %lld: ", threadids[j]);
        selectionTarget S;
        S.headerPrefix = s;
        S.threadId = threadids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
      if ((initialSetCnt > 1) && (threadids.size() > 1)) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      }
      int64_t segmentStart = 0;
      for (int64_t j = 0; j < threadids.size(); j++) {
        if (j < (threadids.size() -1 )) {
         // There is at least one more iteration, so
         // preserve a copy of the original sets.
          for (int64_t k = 0; k < initialSetCnt; k++) {
            Quick_Compare_Set.push_back (Quick_Compare_Set[segmentStart+k]);
          } 
        }

       // Append this threadid to N copies of the original sets.
        for (int64_t k = 0; k < initialSetCnt; k++) {
          char s[40]; sprintf ( s, "-t %lld: ", threadids[j]);
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += s;
          Quick_Compare_Set[segmentStart+k].threadId = threadids[j];
        }
        segmentStart += initialSetCnt;
      }
    }
  }

  if (!((r_list == NULL) || r_list->empty())) {
   // Start by building a vector of all the host names.
    std::vector<int64_t> rankids;;
    vector<ParseRange>::iterator pi;
    for (pi = r_list->begin();pi != r_list->end(); pi++) {
      parse_range_t *p_range = pi->getRange();
      parse_val_t *pval1 = &p_range->start_range;
      parse_val_t *pval2 = p_range->is_range ? &p_range->end_range : pval1;

      int64_t mythreadid;
      for ( mythreadid = pval1->num; mythreadid <= pval2->num; mythreadid++) {
        rankids.push_back (mythreadid);
      }

    }

    int64_t initialSetCnt = Quick_Compare_Set.size();
    if (initialSetCnt == 0) {
     // Define new compare sets for each host.
      for (int64_t j = 0; j < rankids.size(); j++) {
        char s[40]; sprintf ( s, "-r %lld: ", rankids[j]);
        selectionTarget S;
        S.headerPrefix = s;
        S.rankId = rankids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
      if ((initialSetCnt > 1) && (rankids.size() > 1)) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      }
      int64_t segmentStart = 0;
      for (int64_t j = 0; j < rankids.size(); j++) {
        if (j < (rankids.size() -1 )) {
         // There is at least one more iteration, so
         // preserve a copy of the original sets.
          for (int64_t k = 0; k < initialSetCnt; k++) {
            Quick_Compare_Set.push_back (Quick_Compare_Set[segmentStart+k]);
          } 
        }

       // Append this threadid to N copies of the original sets.
        for (int64_t k = 0; k < initialSetCnt; k++) {
          char s[40]; sprintf ( s, "-r %lld: ", rankids[j]);
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += s;
          Quick_Compare_Set[segmentStart+k].rankId = rankids[j];
        }
        segmentStart += initialSetCnt;
      }
    }
  }

  bool success = Generate_CustomView (cmd, Quick_Compare_Set);

  cmd->set_Status(CMD_COMPLETE);
  return success;
}

bool SS_cvSet (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  cmd->clearParseResult();
  CustomView *cvp = new CustomView (p_result);

 // Annotate the command
  cmd->Result_Annotation ("The new custom view identifier is:  -c ");
  
 // Return the EXPID for this command.
  cmd->Result_Int (cvp->cvId());
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

void SS_cvClear_All () {
  std::list<CustomView *>::iterator cvi;

  for (cvi = CustomView_List.begin(); cvi != CustomView_List.end(); ) {
    CustomView *cv = (*cvi);
    cvi++;
    delete cv;
  }

}

bool SS_cvClear (CommandObject *cmd) {
  if (Look_For_KeyWord (cmd, "all")) {
    SS_cvClear_All ();
  } else {
    OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
    vector<ParseRange> *cv_list = p_result->getViewSet ();
    vector<ParseRange>::iterator cvli;

    for (cvli=cv_list->begin();cvli != cv_list->end(); cvli++) {
      parse_range_t *c_range = cvli->getRange();
      parse_val_t *c_val1 = &c_range->start_range;
      parse_val_t *c_val2 = (c_range->is_range) ? &c_range->end_range : c_val1;
      int64_t Rvalue1 = c_val1->num;
      int64_t Rvalue2 = c_val2->num;

      for (int64_t i = Rvalue1; i <= Rvalue2; i++) {
        CustomView *cvp = Find_CustomView (i);
        if (cvp != NULL) {
          delete cvp;
        }
      }

    }

  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

static void CustomViewInfo (CommandObject *cmd,
                            CustomView *cvp) {
  OpenSpeedShop::cli::ParseResult *p_result = cvp->cvPr();

 // Accumulate output in a string.
  std::ostringstream S(ios::out);
  S << "-c " << cvp->cvId() << ":";

 // List viewTypes.
  vector<string> *vtlist = p_result->getViewList();
  if (!vtlist->empty()) {
    bool first_item_found = false;
    vector<string>::iterator p_vtlist;
    S << " ";
    for (p_vtlist=vtlist->begin(); p_vtlist != vtlist->end(); p_vtlist++) {
      if (first_item_found) {
        S << ", ";
      }
      first_item_found = true;
      S << *p_vtlist;
    }
  }

 // Look at general modifier types for a specific KeyWord option.
  vector<string> *v_list = p_result->getModifierList();
  if (!v_list->empty()) {
    bool first_item_found = false;
    vector<string>::iterator p_vlist;
    S << " -v ";
    for (p_vlist=v_list->begin();p_vlist != v_list->end(); p_vlist++) {
      if (first_item_found) {
        S << ", ";
      }
      first_item_found = true;
      S << *p_vlist;
    }

  }

 // Add the '-x' specifier.
  EXPID ExperimentID = (p_result->isExpId()) ? p_result->getExpId() : 0;
  if (ExperimentID != 0) {
    S << " -x " << ExperimentID;
  }

  
 // For each set of filter specifications ...
  int64_t header_length = S.ostringstream::str().size();
  bool first_TargetSpec_found = false;
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  vector<ParseTarget>::iterator pi;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    vector<ParseRange> *h_list = pt.getHostList();
    vector<ParseRange> *f_list = pt.getFileList();
    vector<ParseRange> *p_list = pt.getPidList();
    vector<ParseRange> *t_list = pt.getThreadList();
    vector<ParseRange> *r_list = pt.getRankList();

    if (first_TargetSpec_found) {
      S << " ;";
      S << "\n";
      S << std::string(header_length, *" ");
    }
    first_TargetSpec_found = true;

   // Add the "-h" list.
    if (!h_list->empty()) {
      S << " -h ";
      bool first_item_found = false;
      vector<ParseRange>::iterator p_hlist;
      std::set<std::string> hset;
      for (p_hlist=h_list->begin();p_hlist != h_list->end(); p_hlist++) {
        parse_range_t p_range = *p_hlist->getRange();
        // std::string N = Experiment::getCanonicalName(p_range.start_range.name);
        std::string N = p_range.start_range.name;
        if (p_range.is_range &&
            (p_range.start_range.name != p_range.end_range.name)) {
          N = N + ":" + p_range.end_range.name;
        }
        hset.insert (N);
      }
      for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        S << *hseti;
      }
    }

   // Add the "-f" list.
    if (!f_list->empty()) {
      S << " -f ";
      bool first_item_found = false;
      vector<ParseRange>::iterator p_flist;
      std::set<std::string> fset;
      for (p_flist=h_list->begin();p_flist != h_list->end(); p_flist++) {
        parse_range_t p_range = *p_flist->getRange();
        // std::string N = Experiment::getCanonicalName(p_range.start_range.name);
        std::string N = p_range.start_range.name;
        if (p_range.is_range &&
            (p_range.start_range.name != p_range.end_range.name)) {
          N = N + ":" + p_range.end_range.name;
        }
        fset.insert (N);
      }
      for (std::set<std::string>::iterator fseti = fset.begin(); fseti != fset.end(); fseti++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        S << *fseti;
      }
/*
      for (p_flist=f_list->begin();p_flist != f_list->end(); p_flist++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        parse_range_t p_range = *p_flist->getRange();
        S << Experiment::getCanonicalName(p_range.start_range.name);
        if (p_range.is_range &&
            (p_range.start_range.name != p_range.end_range.name)) {
          S << ":" << Experiment::getCanonicalName(p_range.end_range.name);
        }
      }
*/
    }

   // Add the "-p" list.
    if (!p_list->empty()) {
      S << " -p ";
      bool first_item_found = false;
      vector<ParseRange>::iterator p_plist;
      for (p_plist=p_list->begin();p_plist != p_list->end(); p_plist++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        parse_range_t p_range = *p_plist->getRange();
        S << p_range.start_range.num;
        if (p_range.is_range &&
            (p_range.start_range.num != p_range.end_range.num)) {
          S << ":" << p_range.end_range.num;
        }
      }
    }

   // Add the "-t" list.
    if (!t_list->empty()) {
      S << " -t ";
      bool first_item_found = false;
      vector<ParseRange>::iterator p_tlist;
      for (p_tlist=t_list->begin();p_tlist != t_list->end(); p_tlist++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        parse_range_t p_range = *p_tlist->getRange();
        S << p_range.start_range.num;
        if (p_range.is_range &&
            (p_range.start_range.num != p_range.end_range.num)) {
          S << ":" << p_range.end_range.num;
        }
      }
    }

   // Add the "-r" list.
    if (!r_list->empty()) {
      S << " -r ";
      bool first_item_found = false;
      vector<ParseRange>::iterator p_rlist;
      for (p_rlist=r_list->begin();p_rlist != r_list->end(); p_rlist++) {
        if (first_item_found) {
          S << ", ";
        }
        first_item_found = true;
        parse_range_t p_range = *p_rlist->getRange();
        S << p_range.start_range.num;
        if (p_range.is_range &&
            (p_range.start_range.num != p_range.end_range.num)) {
          S << ":" << p_range.end_range.num;
        }
      }
    }

  }

 // Output the '-m' list'
  vector<ParseRange> *p_mlist = p_result->getexpMetricList();
  if (!p_mlist->empty()) {
   // Add modifiers to output list.
    S << " -m ";
    bool first_item_found = false;
    vector<ParseRange>::iterator mi;
    for (mi = p_mlist->begin(); mi != p_mlist->end(); mi++) {
      if (first_item_found) {
        S << ", ";
      }
      first_item_found = true;
      parse_range_t *m_range = (*mi).getRange();
      std::string Name1 = m_range->start_range.name;
      S << Name1;
      if (m_range->is_range) {
        std::string Name2 = m_range->end_range.name;
        S << "::" << Name2;
      }
    }
  }

 // Attach result to command.
  cmd->Result_String (S.ostringstream::str());
}

bool SS_cvInfo (CommandObject *cmd) {
  if (Look_For_KeyWord (cmd, "all")) {
   // Print all the defined custom views
    SafeToDoNextCmd ();
    std::list<CustomView *>::reverse_iterator cvi;
    for (cvi = CustomView_List.rbegin(); cvi != CustomView_List.rend(); cvi++)
    {
      CustomViewInfo (cmd, (*cvi));
    }
  } else {
    OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
    vector<ParseRange> *cv_list = p_result->getViewSet ();
    if (cv_list->empty()) {
      if (!CustomView_List.empty()) {
       // Print the most recent one.
        CustomViewInfo (cmd, *CustomView_List.begin());
      }
    } else {
     // Print the ones in the user's list.
      vector<ParseRange>::iterator cvli;

      for (cvli=cv_list->begin();cvli != cv_list->end(); cvli++) {
        parse_range_t *c_range = cvli->getRange();
        parse_val_t *c_val1 = &c_range->start_range;
        parse_val_t *c_val2 = (c_range->is_range) ? &c_range->end_range : c_val1;
        int64_t Rvalue1 = c_val1->num;
        int64_t Rvalue2 = c_val2->num;

        for (int64_t i = Rvalue1; i <= Rvalue2; i++) {
          CustomView *cvp = Find_CustomView (i);
          if (cvp != NULL) {
            CustomViewInfo (cmd, cvp);
          }
        }

      }

    }

  }
  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListCviews (CommandObject *cmd) {
 // List all the defined custom views
  SafeToDoNextCmd ();
  std::list<CustomView *>::reverse_iterator cvi;
  for (cvi = CustomView_List.rbegin(); cvi != CustomView_List.rend(); cvi++)
  {
   // Return the EXPID for every known custom view.
    cmd->Result_Int ((*cvi)->cvId());
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_cView (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  EXPID exp_focus = Experiment_Focus ( WindowID );

  OpenSpeedShop::cli::ParseResult *old_presult = cmd->P_Result();
  vector<string> *view_list = old_presult->getViewList();
  vector<string> *mod_list = old_presult->getModifierList();
  vector<ParseRange> *met_list = old_presult->getexpMetricList();
  vector<ParseRange> *cv_list = old_presult->getViewSet ();
  vector<ParseRange>::iterator cvli;

  std::vector<selectionTarget> Quick_Compare_Set;

  for (cvli=cv_list->begin();cvli != cv_list->end(); cvli++) {
    parse_range_t *c_range = cvli->getRange();
    parse_val_t *c_val1 = &c_range->start_range;
    parse_val_t *c_val2 = (c_range->is_range) ? &c_range->end_range : c_val1;
    int64_t Rvalue1 = c_val1->num;
    int64_t Rvalue2 = c_val2->num;

    for (int64_t i = Rvalue1; i <= Rvalue2; i++) {
      CustomView *cvp = Find_CustomView (i);
      if (cvp == NULL) {
        std::ostringstream S(ios::out);
        S << "The requested Custom View ID, '-c " << i << "', is invalid on the 'cView' command.";
        Mark_Cmd_With_Soft_Error(cmd, S.ostringstream::str());
        continue;
      }
      OpenSpeedShop::cli::ParseResult *p_result = cvp->cvPr();
      vector<string> *p_slist = p_result->getViewList();
      EXPID ExperimentID = (p_result)->isExpId() ? cvp->cvPr()->getExpId() : exp_focus;
      std::ostringstream N(ios::out);
      N << i;
      ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;

     // Pick up the <viewType> from the command.
      if (p_slist->begin() == p_slist->end()) {
       // The user has not selected a view.
        if ((exp == NULL) ||
            (exp->FW() == NULL)) {
         // No experiment was specified, so we can't find a useful view to generate.
          Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified.");
        } else {
         // Look for a view that would be meaningful.
          CollectorGroup cgrp = exp->FW()->getCollectors();
          if (cgrp.begin() == cgrp.end()) {
           // No collector was used.
            Mark_Cmd_With_Soft_Error(cmd, "No performance measurements were made for the experiment.");
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
                view_found = true;
               // Generate a view for every collector.
                selectionTarget S;
                S.pResult = p_result;
                S.headerPrefix = std::string("-c ") + N.ostringstream::str() + ": ";
                S.Exp = exp;
                S.viewName = collector_name;
                Quick_Compare_Set.push_back (S);
              }
            }

            if (!view_found) {
             // Use generic view as default
              selectionTarget S;
              S.pResult = p_result;
              S.headerPrefix = std::string("-c ") + N.ostringstream::str() + ": ";
              S.Exp = exp;
              S.viewName = "stats";
            }
          }
        }
      } if (!view_list->empty()) {
       // Override any view in the Custom View Definition with view named on cView command.
        vector<string>::iterator si;
        for (si = view_list->begin(); si != view_list->end(); si++) {
       // Determine the availability of the view.
          std::string viewname = *si;
          ViewType *vt = Find_View (viewname);
          if (vt == NULL) {
            Mark_Cmd_With_Soft_Error(cmd, "The requested view is unavailable.");
            return false;
          }

         // Define new compare sets for each view.
          selectionTarget S;
          S.pResult = p_result;
          S.base_tgrp = cvp->cvTgrp();
          S.headerPrefix = std::string("-c ") + N.ostringstream::str() + ": ";
          S.Exp = exp;
          S.viewName = viewname;
          Quick_Compare_Set.push_back (S);
        }

      } else {
       // Generate all the views in the list.
        vector<string>::iterator si;
        for (si = p_slist->begin(); si != p_slist->end(); si++) {
       // Determine the availability of the view.
          std::string viewname = *si;
          ViewType *vt = Find_View (viewname);
          if (vt == NULL) {
            Mark_Cmd_With_Soft_Error(cmd, "The requested view is unavailable.");
            return false;
          }

         // Define new compare sets for each view.
          selectionTarget S;
          S.pResult = p_result;
          S.base_tgrp = cvp->cvTgrp();
          S.headerPrefix = std::string("-c ") + N.ostringstream::str() + ": ";
          S.Exp = exp;
          S.viewName = viewname;
          Quick_Compare_Set.push_back (S);
        }

      }
    }

  }

  if (!mod_list->empty() ||
      !met_list->empty()) {
   // Override Custome View Definitions of modifiers and/or metrics.
   // This involves duplicating the ParseObject and replacing certain lists.
    int64_t numQuickSets = Quick_Compare_Set.size();
    for (int64_t i = 0; i < numQuickSets; i++) {
      OpenSpeedShop::cli::ParseResult *old_result = Quick_Compare_Set[i].pResult;
      OpenSpeedShop::cli::ParseResult *new_result = new ParseResult();
      *new_result = *old_result;

   // Copy ModifierList from original ParseResult to new ParseResult.
      if (!mod_list->empty()) {
// TODO: clear old vector        new_result->getModifierList()->clear();
        for (vector<string>::const_iterator
                 modl = mod_list->begin(); modl != mod_list->end(); modl++) {
          new_result->pushModifiers ((char *)((*modl).c_str()));
        }
      }

   // Copy MetricList from original ParseResult to new ParseResult.
      if (!met_list->empty()) {
// TODO: clear old vector        new_result->getexpMetricList()->clear();
        for (vector<ParseRange>::iterator
                 metl = met_list->begin(); metl != met_list->end(); metl++) {
          parse_range_t *m_range = (*metl).getRange();
          parse_val_t pval1 = m_range->start_range;
          Assert (pval1.tag == VAL_STRING);
          if (m_range->is_range) {
            parse_val_t pval2 = m_range->end_range;
            Assert (pval2.tag == VAL_STRING);
            new_result->pushExpMetric ((char *)(pval1.name.c_str()), (char *)(pval2.name.c_str()));
          } else {
            new_result->pushExpMetric ((char *)(pval1.name.c_str()));
          }
        }
      }

      Quick_Compare_Set[i].pResult = new_result;
    }
  }

  bool success = Generate_CustomView (cmd, Quick_Compare_Set);

  if (!mod_list->empty() ||
      !met_list->empty()) {
   // Need to delete the copies made of the overriding Parse components.
    int64_t numQuickSets = Quick_Compare_Set.size();
    for (int64_t i = 0; i < numQuickSets; i++) {
      OpenSpeedShop::cli::ParseResult *tmp_result = Quick_Compare_Set[i].pResult;
      if (!mod_list->empty()) {
        vector<string> *mod_list = tmp_result->getModifierList();
        delete tmp_result->getModifierList();
      }
      if (!met_list->empty()) {
        vector<ParseRange> *met_list = tmp_result->getexpMetricList();
        delete tmp_result->getexpMetricList();
      }
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return success;
}

template <typename TOBJECT>
bool Cluster_Analysis (
          CommandObject *cmd, ExperimentObject *exp, ThreadGroup& tgrp,
          Collector &collector, std::string &metric,
          TOBJECT *dummyObject,
          std::set<Framework::ThreadGroup> &clusters) {

  Framework::Experiment *experiment = exp->FW();

 // Get the list of desired objects.
  std::set<TOBJECT> objects;
  Get_Filtered_Objects (cmd, exp, tgrp, objects);
  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no objects specified for cluster analysis.)");
    return false;
  }

/*
  SmartPtr<std::map<TOBJECT, std::map<Thread, double> > > individual;
  Queries::GetMetricValues(collector, metric,
                           TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                           tgrp, objects, individual);
  if (individual->begin() == individual->end()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested cluster analysis.)");
    return false;
  }

  clusters = Queries::ClusterAnalysis::ApplySimple(individual);
*/

  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

  if( m.isType(typeid(unsigned int)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, uint> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else if( m.isType(typeid(uint64_t)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, uint64_t> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else if( m.isType(typeid(int)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, int> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else if( m.isType(typeid(int64_t)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, int64_t> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else if( m.isType(typeid(float)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, float> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else if( m.isType(typeid(double)) ) {
    SmartPtr<std::map<TOBJECT, std::map<Thread, double> > > individual;
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                             tgrp, objects, individual);
    clusters = Queries::ClusterAnalysis::ApplySimple(individual);
  } else {
    std::string S("(Cluster Analysis can not be performed on metric '");
    S = S +  metric + "' of type '" + m.getType() + "'.)";
    Mark_Cmd_With_Soft_Error(cmd, S);
    return false;
  }

  return true;
}

bool SS_cvClusters (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : 0;
  if (ExperimentID == 0) {
    ExperimentID = Experiment_Focus ( WindowID );
    if (ExperimentID == 0) {
      Mark_Cmd_With_Soft_Error(cmd, "There is no focused experiment.");
      return false;
    }
  }
  ExperimentObject *exp = Find_Experiment_Object (ExperimentID);

  if ((exp == NULL) ||
      (exp->FW() == NULL)) {
   // No experiment was specified, so we can't do any resonable work.
    Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified.");
    return false;
  }

 // Get a list of the unique threads used in the specified experiment.
  Framework::Experiment *experiment = exp->FW();
  ThreadGroup tgrp = experiment->getThreads();
  Filter_ThreadGroup (cmd->P_Result(), tgrp);

 // Pick up components of the parse object.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  vector<string> *mod_list = p_result->getModifierList();
  vector<ParseRange> *met_list = p_result->getexpMetricList();

 // Is there a potential viewname?
  if (p_slist->begin() == p_slist->end()) {
     // Look for a view that would be meaningful.
      CollectorGroup cgrp = exp->FW()->getCollectors();
      if (cgrp.begin() != cgrp.end()) {
        bool view_found = false;
        CollectorGroup::iterator cgi;
        for (cgi = cgrp.begin(); cgi != cgrp.end(); cgi++) {
         // Push the name onto the parse list.
          Collector c = *cgi;
          Metadata m = c.getMetadata();
          std::string collector_name = m.getUniqueId();
          p_slist->push_back(collector_name);
        }
      }
  }

  std::string viewname;
  if (!p_slist->empty()) {
    viewname = *(p_slist->begin());
  }

 // Determine the collector and metric.
  std::string C_Name = viewname;
  std::string M_Name;
  if (met_list->empty()) {
   // Use the first metric in the view definition.
   // But be sure there is exactly one choice.
    if (p_slist->empty()) {
     // No view was specified, so we can't do any resonable work.
      Mark_Cmd_With_Soft_Error(cmd, "No valid view name was specified.");
      return false;
    } else if (p_slist->size() != 1) {
     // Too many views are specified, we don't handle this yet.
      Mark_Cmd_With_Soft_Error(cmd, "Too many view names were specified.");
      return false;
    }

    ViewType *vt = Find_View (viewname);
    if (vt == NULL) {
      std::string S("The requested view, '");
      S = S + viewname + "', is unavailable.";
      Mark_Cmd_With_Soft_Error(cmd,S);
      return false;
    }
    viewname = vt->Unique_Name();
    C_Name = vt->Unique_Name();
    M_Name = vt->Metrics()[0];
  } else if (!met_list->empty()) {
   // Use the first item on the '-m' list as a metric.
    vector<ParseRange>::iterator mi = met_list->begin();
    parse_range_t *m_range = (*mi).getRange();
    if (m_range->is_range) {
      C_Name = m_range->start_range.name;
      M_Name = m_range->end_range.name;
    } else {
      C_Name = viewname;
      M_Name = m_range->start_range.name;
    }
  }

  if (M_Name == "") {
   // Unable to determine a metric.
    Mark_Cmd_With_Soft_Error(cmd, "There is no metric available.");
    return false;
  }
  if (C_Name == "") {
   // Unable to determine a collector.
    Mark_Cmd_With_Soft_Error(cmd, "There is no collector specified.");
    return false;
  }

 // Perform Cluster Analysis.
  View_Form_Category vfc = Determine_Form_Category(cmd);
  Collector C = Get_Collector (experiment, C_Name);
  std::set<Framework::ThreadGroup> clusters;
  bool Analysis_Okay = false;
  switch (Determine_Form_Category(cmd)) {
   case VFC_Statement:
   {
    Framework::Statement *dummyObject;
    Analysis_Okay = Cluster_Analysis (cmd, exp, tgrp, C, M_Name,
                                      dummyObject, clusters);
    break;
   }
   case VFC_LinkedObject:
   {
    Framework::LinkedObject *dummyObject;
    Analysis_Okay = Cluster_Analysis (cmd, exp, tgrp, C, M_Name,
                                      dummyObject, clusters);
    break;
   }
   case VFC_Function:
   {
    Framework::Function *dummyObject;
    Analysis_Okay = Cluster_Analysis (cmd, exp, tgrp, C, M_Name,
                                      dummyObject, clusters);
    break;
   }
   default:
    Mark_Cmd_With_Soft_Error(cmd, "(There is no supported grouping of objects.)");
    return false;
  }

 // Build parse objects for each cluster and perform cViewCreate.
  bool first_cluster = true;
  std::set<string> file_list;

 // Find all the functions that the user listed.
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  for (vector<ParseTarget>::iterator
        pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    vector<ParseRange> *f_list = pt.getFileList();
    if ((f_list != NULL) || !f_list->empty()) {
      for (vector<ParseRange>::iterator
             f_iter = f_list->begin(); f_iter != f_list->end(); f_iter++) {
        parse_range_t *f_range = f_iter->getRange();
        parse_val_t f_val1 = f_range->start_range;
        Assert (!f_range->is_range);
        Assert (f_val1.tag == VAL_STRING);
        file_list.insert (f_val1.name);
      }
    }
  }

  for(std::set<ThreadGroup>::const_iterator
         i = clusters.begin(); i != clusters.end(); ++i) {
   // Create new ParseResult and define ExpId and viewType.
    OpenSpeedShop::cli::ParseResult *new_result = new ParseResult();
    new_result->setCommandType (CMD_C_VIEW_CREATE);
    new_result->pushExpIdPoint (ExperimentID);

   // Copy viewType list from original ParseResult to new ParseResult.
    for (vector<string>::const_iterator
             vl = p_slist->begin(); vl != p_slist->end(); vl++) {
      new_result->pushViewType ((char *)((*vl).c_str()));
    }

   // Copy ModifierList from original ParseResult to new ParseResult.
    for (vector<string>::const_iterator
             modl = mod_list->begin(); modl != mod_list->end(); modl++) {
      new_result->pushModifiers ((char *)((*modl).c_str()));

    }

   // Copy MetricList from original ParseResult to new ParseResult.
    for (vector<ParseRange>::iterator
             metl = met_list->begin(); metl != met_list->end(); metl++) {
      parse_range_t *m_range = (*metl).getRange();
      parse_val_t pval1 = m_range->start_range;
      Assert (pval1.tag == VAL_STRING);
      if (m_range->is_range) {
        parse_val_t pval2 = m_range->end_range;
        Assert (pval2.tag == VAL_STRING);
        new_result->pushExpMetric ((char *)(pval1.name.c_str()), (char *)(pval2.name.c_str()));
      } else {
        new_result->pushExpMetric ((char *)(pval1.name.c_str()));
      }
    }

   // Each thread in the cluster is a distinct ParseTarget specification.
    for(ThreadGroup::const_iterator
             j = i->begin(); j != i->end(); ++j) {
     // Pick up the prevously defined ParseTarget information and copy it.
      ParseTarget *new_pt = new_result->currentTarget();

     // Copy the sum of all functions from the original set of target specs.
      for (std::set<string>::iterator
              fi = file_list.begin(); fi != file_list.end(); fi++) {
        // char *c = (char *)((*fi).c_str());
        // new_pt->dm_file_list.push_back (range(c));
        new_pt->pushFilePoint ((char *)((*fi).c_str()));
      }

     // Get host, pid, thread and rank info from original target spec.
      std::string hid = j->getHost();
      new_pt->pushHostPoint ((char *)(hid.c_str()));

      pid_t pid = j->getProcessId();
      new_pt->pushPidPoint (pid);

      if (j->getPosixThreadId().first) {
        new_pt->pushThreadPoint (j->getPosixThreadId().second);
      }

      if (j->getMPIRank().first) {
        new_pt->pushRankPoint (j->getMPIRank().second);
      }

     // New ParseTarget is complete, commit it and create a new currentTarget..
      new_result->pushParseTarget();
    }

   // Construct a new CustomView.
    CustomView *cvp = new CustomView (new_result, *i);

    if (first_cluster) {
     // Annotate the command
      cmd->Result_Annotation ("The new custom view identifiers for the clusters are:\n");
      first_cluster = false;
    }

   // Return the EXPID for this command.
    cmd->Result_Int (cvp->cvId());
  }

/*
  for(std::set<ThreadGroup>::const_iterator
         i = clusters.begin(); i != clusters.end(); ++i) {
     printf("[");
     for(ThreadGroup::const_iterator
             j = i->begin(); j != i->end(); ++j) {
       pid_t pid = j->getProcessId();
       int64_t p = pid;
       printf(" %lld",p);
       std::pair<bool, pthread_t> pthread = j->getPosixThreadId();
       if (pthread.first) {
         int64_t t = pthread.second;
         printf(", %lld1)", t);
       } else {
        printf(", %d(2)", j->getMPIRank().second);
       }
     }
     printf(" ]\n\n");
  }
*/

  cmd->set_Status(CMD_COMPLETE);
  return Analysis_Okay;
}
