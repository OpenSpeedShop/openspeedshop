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

struct selectionTarget {
  std::string headerPrefix;
  int64_t numColumns;
  std::string hostId;
  pid_t pidId;
  pthread_t threadId;
  int64_t rankId;
  std::list<CommandResult *> partial_view;
  std::map<int64_t, CommandResult *> merge_map;

  selectionTarget () {
    headerPrefix = "";
    numColumns = 0;
    hostId = "";
    pidId = 0;
    threadId = 0;
    rankId = 0;
  }

  selectionTarget (const selectionTarget& S) {
    headerPrefix = S.headerPrefix;
    numColumns = S.numColumns;
    hostId = S.hostId;
    pidId = S.pidId;
    threadId = S.threadId;
    rankId = S.rankId;
  }

  void Print (ostream& to) {
    if (hostId != "") to << " -h " << hostId;
    if (pidId) to << " -p " << pidId;
    if (threadId) to << " -t " << threadId;
    if (rankId) to << " -r " << rankId;
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

#if HAS_OPENMP
      if (include_thread && (S.rankId != 0)) {
       // Does it match a rank ID?
        int64_t rid = t.getOmpThreadId();
        include_thread = (rid == S.rankId);
      }
#endif

     // Add matching threads to rgrp.
      if (include_thread) {
        rgrp.insert(t);
      }

    }

}

bool SS_expCompare (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Examine the parsed command for a "-x" specifier.
  Assert(cmd->P_Result() != NULL);
  EXPID ExperimentID = (cmd->P_Result()->isExpId()) ? cmd->P_Result()->getExpId() : Experiment_Focus ( WindowID );
  ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;
  bool view_result = false;
  int64_t i;

 // Prevent this experiment from changing until we are done.
 // if (exp != NULL) exp->Q_Lock (cmd, true);

 // Pick up the <viewType> from the command.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<string> *p_slist = p_result->getViewList();
  if (p_slist->size() > 1) {
    Mark_Cmd_With_Soft_Error(cmd, "More than 1 view name is not supproted.");
    return false;
  }
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
           // Generate the selected view
            p_slist->push_back (collector_name);
          }
        }

        if (!view_found) {
         // Use generic view as default
          p_slist->push_back (std::string("stats"));
        }
      }
    }
  }
  

 // The <target_spec> will define the comparisons.
  vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->begin() == p_tlist->end()) {
   // There are no filters.
    Mark_Cmd_With_Soft_Error(cmd, "There is no target specification.");
    return false;
  }
  if (p_tlist->size() > 1) {
   // There are no filters.
    Mark_Cmd_With_Soft_Error(cmd, "There is more than one target specification.");
    return false;
  }

 // Scan the <target_spec> and determine the comparison sets.
  ParseTarget pt = (*p_tlist)[0];
  vector<ParseRange> *h_list = pt.getHostList();
  vector<ParseRange> *p_list = pt.getPidList();
  vector<ParseRange> *t_list = pt.getThreadList();
  vector<ParseRange> *r_list = pt.getRankList();

  std::vector<selectionTarget> Quick_Compare_Set;

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
    }
  }

  if (!((p_list == NULL) || p_list->empty())) {
   // Start by building a vector of all the host names.
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
        char s[40];
        sprintf ( s, "-p %lld: ", pids[j]);
        selectionTarget S;
        S.headerPrefix = s;
        S.pidId = pids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
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
          char s[40];
          sprintf ( s, "-p %lld: ", pids[j]);
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
        char s[40];
        sprintf ( s, "-t %lld: ", threadids[j]);
        selectionTarget S;
        S.headerPrefix = s;
        S.threadId = threadids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
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
          char s[40];
          sprintf ( s, "-t %lld: ", threadids[j]);
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += s;
          Quick_Compare_Set[segmentStart+k].threadId = threadids[j];
        }
        segmentStart += initialSetCnt;
      }
    }
  }

#if HAS_OPENMP
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
        char s[40];
        sprintf ( s, "-r %lld: ", rankids[j]);
        selectionTarget S;
        S.headerPrefix = s;
        S.rankId = rankids[j];
        Quick_Compare_Set.push_back (S);
      }
    } else {
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
          char s[40];
          sprintf ( s, "-r %lld: ", rankids[j]);
          selectionTarget S;
          Quick_Compare_Set[segmentStart+k].headerPrefix += s;
          Quick_Compare_Set[segmentStart+k].rankId = rankids[j];
        }
        segmentStart += initialSetCnt;
      }
    }
  }
#endif

  int64_t numQuickSets = Quick_Compare_Set.size();

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


 // Determine the data set of interest.
  ThreadGroup base_tgrp;
  if ((exp != NULL) &&
      (exp->FW() != NULL)) {
   // Get all the threads that are in the experiment.
    base_tgrp = exp->FW()->getThreads();
   // Retain only the ones of interest.
    Filter_ThreadGroup (cmd, base_tgrp);
  }
  if (base_tgrp.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "The selection criteria excludes all data.");
    return false;
  }

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

    for (i = 0; i < numQuickSets; i++) {
     // Try to Generate the Requested View for each comparison set!
      ThreadGroup tgrp;
      Select_ThreadGroup (Quick_Compare_Set[i], base_tgrp, tgrp);
      if (tgrp.size() > 0) {
        bool success = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                                         tgrp, Quick_Compare_Set[i].partial_view);
        if (!success) {
          Reclaim_CR_Space (Quick_Compare_Set[i].partial_view);
          Quick_Compare_Set[i].partial_view.clear();
        }
      }
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
    Mark_Cmd_With_Soft_Error(cmd, "(There was not data available to compare.)");
    return false;
  }

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

 // Release the experiment lock.
 // if (exp != NULL) exp->Q_UnLock ();

 // Free remaining space for CommandResults.
  for (i = 0; i < numQuickSets; i++) {
    Reclaim_CR_Space (Quick_Compare_Set[i].partial_view);
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}
