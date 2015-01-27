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


#include "SS_Input_Manager.hxx"

//#define DEBUG_REUSEVIEWS 1
//#define DEBUG_COMPARE 1
//#define DEBUG_COMPARE_SETS 1
//#define DEBUG_CVIEWINFO 1

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
    return CR1->LT( CR2);
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
#if DEBUG_COMPARE
    printf("SSCOMPARE: CustomView1 -- Experiment_Sequence_Number=%d, Exp_ID=%dn", Experiment_Sequence_Number, Exp_ID);
#endif
    p_result = p;
    CustomView_List.push_front(this);
    Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);
  }
  CustomView (OpenSpeedShop::cli::ParseResult *p,
              const Framework::ThreadGroup& tgrp) {
    Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
    Exp_ID = ++Experiment_Sequence_Number;
#if DEBUG_COMPARE
    printf("SSCOMPARE: CustomView2 -- Experiment_Sequence_Number=%d, Exp_ID=%d\n", Experiment_Sequence_Number, Exp_ID);
#endif
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
  std::string viewName;
  std::string hostId;
  pid_t pidId;
  pthread_t threadId;
  int64_t rankId;
  std::vector<ParseInterval> timeSegment;
  std::list<CommandResult *> partial_view;
  std::map<int64_t, CommandResult *> merge_map;

  selectionTarget () {
    headerPrefix = "";
    numColumns = 0;
    pResult = NULL;
    base_tgrp = NULL;
    Exp = NULL;
    viewName = "";
    hostId = "";
    pidId = 0;
    threadId = 0;
    rankId = -1;
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
    timeSegment = S.timeSegment;
  }

  void Print (std::ostream& to) {
    if (pResult != NULL) {
      pResult->ParseResult::dumpInfo();
    }
    if (Exp != NULL) to << " -x " << Exp->ExperimentObject_ID();
    if (viewName != "") to << " " << viewName;
    if (hostId != "") to << " -h " << hostId;
    if (pidId) to << " -p " << pidId;
    if (threadId) to << " -t " << threadId;
    if (rankId >= 0) to << " -r " << rankId;
    to << " (numColumns = " << numColumns << ")";
    to << " prefix = " << headerPrefix;
    to << std::endl;
  }
};


#if 0
static void
SS_printRange(std::vector<ParseRange> *p_list, const char *label, bool is_hex)
{
    std::vector<ParseRange>::iterator iter;
//#if DEBUG_CLI
    printf("SS_printRange, label=%s, is_hex=%d\n", label, is_hex);
//#endif

    if (is_hex){
        std::cout.setf(std::ios_base::hex,std::ios_base::basefield);
        std::cout.setf(std::ios_base::showbase);
    }
    else {
        std::cout.setf(std::ios_base::dec,std::ios_base::basefield);
        std::cout.unsetf(std::ios_base::showbase);
    }

    if (p_list->begin() != p_list->end()) {
            std::cout << "\t\t" << label << ": " ;
    } else {
        //std::cerr << "SS_printRange, p_list begin and end are the same, label=" << label << " is_hex=" << is_hex << std::endl;
    }

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
        parse_range_t *p_range = iter->getRange();
        if (p_range->is_range) {
            parse_val_t *p_val1 = &p_range->start_range;
            parse_val_t *p_val2 = &p_range->end_range;
            if (p_val1->tag == VAL_STRING) {
                std::cout << p_val1->name << ":";
                //std::cerr << "SS_printRange, there is a range, p_val1->name=" << p_val1->name << std::endl;
            }
            else {
                std::cout << p_val1->num << ":";
                //std::cerr << "SS_printRange, there is a range, p_val1->num=" << p_val1->num << std::endl;
            }
            if (p_val2->tag == VAL_STRING) {
                std::cout << p_val2->name << " ";
                //std::cerr << "SS_printRange, there is a range, p_val2->name=" << p_val2->name << std::endl;
            }
            else {
                std::cout << p_val2->num << " ";
                //std::cerr << "SS_printRange, there is a range, p_val2->num=" << p_val2->num << std::endl;
            }
        }
        else {
            parse_val_t *p_val1 = &p_range->start_range;
            if (p_val1->tag == VAL_STRING) {
                std::cout << p_val1->name << " ";
                //std::cerr << "SS_printRange, there is NOT a range, p_val1->name=" << p_val1->name << std::endl;
            }
            else {
                std::cout << p_val1->num << " ";
                //std::cerr << "SS_printRange, there is NOT a range, p_val1->num=" << p_val1->num << std::endl;
            }
        }
    }
    if (p_list->begin() != p_list->end())
            std::cout << std::endl ;

}
#endif


/**
 * Method: Select_ThreadGroup()
 * 
 * Scan ths <target_list> specifier for a command and
 * determine which of the original set of threads are
 * begin requested.
 *
 * A thread is selected if one of the items in each  of
 * the specified -h -p -r -t lists matches the similar
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
static void Select_ThreadGroup (selectionTarget& S, ThreadGroup& base_grp, ThreadGroup& rgrp) {

#if DEBUG_COMPARE
  printf("SSCOMPARE: Select_ThreadGroup Select_ThreadGroup, base_grp.size()==%d, rgrp.size()=%d\n", base_grp.size(), rgrp.size());
  printf("SSCOMPARE: Select_ThreadGroup Select_ThreadGroup, S.threadId=%lld, S.rankId=%lld, S.hostId.c_str()=%s, S.pidId=%d\n", 
         S.threadId, S.rankId, S.hostId.c_str(), S.pidId);
#endif

   // Go through every thread and decide if it is included.
   // Note: Selection criteria is based on a single field.
    ThreadGroup::iterator ti;
    for (ti = base_grp.begin(); ti != base_grp.end(); ti++) {
      Thread t = *ti;
      bool include_thread = true;

      if (S.rankId >= 0) {
       // Does it match a rank ID?
        std::pair<bool, int> prank = t.getMPIRank();

#if DEBUG_COMPARE
        printf("SSCOMPARE: Select_ThreadGroup inside check for rankId, prank.first==%d\n", prank.first);
#endif
        include_thread = (prank.first && (prank.second == S.rankId));

      } else if (S.threadId >= 0) {
       // Does it match a pthread ID?
        std::pair<bool, int> pthread = t.getOpenMPThreadId();
        bool threadHasThreadId = false;
        int64_t pthreadid = 0;

#if DEBUG_COMPARE
        printf("SSCOMPARE: Select_ThreadGroup inside check for threadId, pthread.first=%d\n", pthread.first);
#endif

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
        include_thread = (threadHasThreadId && (pthreadid == S.threadId));

      } else if (S.pidId >= 0) {
       // Does it match the pid?

#if DEBUG_COMPARE
        printf("SSCOMPARE: Select_ThreadGroup inside check for pidId, S.pidId=%lld\n", S.pidId);
#endif

        pid_t pid = t.getProcessId();
        include_thread = (pid == S.pidId);

      } else if (S.hostId != "") {
       // Does it match the host?

#if DEBUG_COMPARE
        printf("SSCOMPARE: Select_ThreadGroup inside check for hostId, S.hostId.c_str()=%s\n", S.hostId.c_str());
#endif

        std::string hid = t.getHost();
        include_thread = (hid == S.hostId);

      } else {
       // There is no criteria for a selection. We shouldn't even be here!
        return;
      }

#if DEBUG_COMPARE
      printf("SSCOMPARE: Select_ThreadGroup before check for rgrp.insert include_thread=%d\n", include_thread );
#endif

     // Add matching threads to rgrp.
      if (include_thread) {
        rgrp.insert(t);
#if DEBUG_COMPARE
        printf("SSCOMPARE: Select_ThreadGroup rgrp.insert (t.getProcessId())=%lld\n", t.getProcessId());
#endif
      }

    }

}

static bool Generate_CustomView (CommandObject *cmd,
                                 std::vector<selectionTarget>& Quick_Compare_Set) {
  int64_t numQuickSets = Quick_Compare_Set.size();
  int64_t i;
  bool Compute_Delta = FALSE;

#if DEBUG_COMPARE
  printf("SSCOMPARE: ------------- Enter Generate_CustomView, numQuickSets=%d\n", numQuickSets);
#endif

 // Look in the metric list for a request to calculate differences.
  std::vector<ParseRange> *m_list = cmd->P_Result()->getexpMetricList();
  std::vector<ParseRange>::iterator j;

  for (j=m_list->begin();j != m_list->end(); j++) {
    parse_range_t *m_range = (*j).getRange();
    std::string S = m_range->start_range.name;
    bool pack_vector = FALSE;
    if (!strcasecmp(S.c_str(), "diff") ) {
      Compute_Delta = TRUE;
      pack_vector = TRUE;
    } else if (!strcasecmp(S.c_str(), "absdiff") ) {
      Compute_Delta = TRUE;
      pack_vector = TRUE;
    }
    if (pack_vector) {
     // Strip the request from the metric list.
     // The difference is calculated in this routine.
      m_list->erase(j);
      if (j == m_list->end()) {
        break;
      }
    }
  }

  if (numQuickSets == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "There are no valid comparisons requested.");
    return false;
  }


#if DEBUG_COMPARE_SETS
  printf("SSCOMPARE: In Generate_CustomView, START 1st print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
  for (i = 0; i < numQuickSets; i++) {
    std::cerr << i;
    Quick_Compare_Set[i].Print(std::cerr);
  }
  printf("SSCOMPARE: In Generate_CustomView, END 1st print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
#endif

 // Generate all the views in the list.
  for (i = 0; i < numQuickSets; i++) {

   // Check for asynchronous abort command
    if (cmd->Status() == CMD_ABORTED) {
      break;
    }

   // Try to Generate the Requested View for each comparison set!
    ExperimentObject *exp = Quick_Compare_Set[i].Exp;
    if ((exp == NULL) ||
        (exp->FW() == NULL)) {
      Mark_Cmd_With_Soft_Error(cmd, "(There was no experiment available to compare.)");
      return false;
    }
    std::string viewname = Quick_Compare_Set[i].viewName;
    ViewType *vt = Find_View (viewname);
    if (vt == NULL) {
      std::ostringstream M;
      M << "The requested view, '" << viewname << "' is not available.";
      Mark_Cmd_With_Soft_Error(cmd, M.str());
      return false;
    }

    OpenSpeedShop::cli::ParseResult *sResult = NULL;
    ThreadGroup tgrp;
    if (Quick_Compare_Set[i].pResult != NULL) {
     // This path implies that we have a custom view definition.
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

#if DEBUG_COMPARE
      printf("SSCOMPARE: In Generate_CustomView, processing expCompare command, numQuickSets=%d\n", numQuickSets);
#endif

     // This path implies we are processing an expCompare command.
     // Get all the threads that are in the experiment.
      ThreadGroup base_tgrp = exp->FW()->getThreads();

#if DEBUG_COMPARE
    printf("SSCOMPARE: In Generate_CustomView, START -------------- BASE_TGRP processing expCompare command, base_tgrp.size()=%d\n", base_tgrp.size());
    ThreadGroup::iterator ti;
    for (ti = base_tgrp.begin(); ti != base_tgrp.end(); ti++) {
      std::cerr << "SSCOMPARE: In Generate_CustomView, BASE_TGRP ProcessId in base_tgrp=" << (*ti).getProcessId() << " HostID in base_tgrp=" << (*ti).getHost() << std::endl;
      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      std::cerr << "SSCOMPARE: In Generate_CustomView, BASE_TGRP Rank in base_tgrp=" << prank.second << "\n" <<  std::endl;
    }
    printf("SSCOMPARE: In Generate_CustomView, END ----------------- BASE_TGRP processing expCompare command, base_tgrp.size()=%d\n", base_tgrp.size());
#endif

#if DEBUG_COMPARE
      printf("SSCOMPARE: In Generate_CustomView, before calling Filter_ThreadGroup\n");
#endif
     // Retain only the threads that may be of interest.
      Filter_ThreadGroup (cmd->P_Result(), base_tgrp);

#if DEBUG_COMPARE
      printf("SSCOMPARE: In Generate_CustomView, after calling Filter_ThreadGroup\n");
      printf("SSCOMPARE: In Generate_CustomView, before calling Select_ThreadGroup, base_tgrp.size()=%d\n", base_tgrp.size());
      printf("SSCOMPARE: In Generate_CustomView, before calling Select_ThreadGroup, tgrp.size()=%d\n", tgrp.size());
#endif

     // Select specific ones.
      Select_ThreadGroup (Quick_Compare_Set[i], base_tgrp, tgrp);

#if DEBUG_COMPARE
      printf("SSCOMPARE: In Generate_CustomView, after calling Select_ThreadGroup, base_tgrp.size()=%d\n", base_tgrp.size());
      printf("SSCOMPARE: In Generate_CustomView, after calling Select_ThreadGroup, tgrp.size()=%d\n", tgrp.size());
#endif

     // Are we processing a restricted time range?
      if (!Quick_Compare_Set[i].timeSegment.empty()) {
        std::vector<ParseInterval> *interval_list = cmd->P_Result()->getParseIntervalList();
        interval_list->clear();
        interval_list->push_back(Quick_Compare_Set[i].timeSegment[0]);
      }
    }

#if DEBUG_COMPARE
    printf("SSCOMPARE: Generate_CustomView, START ---------- processing expCompare command, tgrp.size()=%d\n", tgrp.size());
    ThreadGroup::iterator ti;
    for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      std::cerr << " SSCOMPARE: Generate_CustomView, ProcessId in tgrp=" << (*ti).getProcessId() << " HostID in tgrp=" << (*ti).getHost() << std::endl;
      Thread t = *ti;
      std::pair<bool, int> prank = t.getMPIRank();
      std::cerr << " SSCOMPARE: Generate_CustomView, Rank in tgrp=" << prank.second << "\n" <<  std::endl;
    }
    printf("SSCOMPARE: Generate_CustomView, END -------- processing expCompare command, tgrp.size()=%d\n", tgrp.size());
#endif

    if (tgrp.size() > 0) {

#if DEBUG_COMPARE
      printf("SSCOMPARE: Generate_CustomView, before calling vt->GenerateView, tgrp.size()=%d\n", tgrp.size());
#endif

      bool success = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                                       tgrp, Quick_Compare_Set[i].partial_view);
#if DEBUG_COMPARE
      printf("SSCOMPARE: Generate_CustomView, after calling vt->GenerateView, tgrp.size()=%d\n", tgrp.size());
#endif

      if (Quick_Compare_Set[i].partial_view.empty()) {
        std::string annotation_string = 
                "There were no data samples available to compare for the '"
                +Quick_Compare_Set[i].headerPrefix+"' selection.\n";
        cmd->Result_Annotation (annotation_string.c_str());
      }

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
   // There is only one valid result.
   // We could just set the result and return as follows:
   //
   // cmd->Result_Predefined (Quick_Compare_Set[0].partial_view);
   // return true;
   //
   // However, we want to prepend any header information
   // to the output so fall through.
  }

  if (Compute_Delta) {
   // Are we capable of generating a difference column?
    if (numQuickSets != 2) {
      Mark_Cmd_With_Soft_Error(cmd, "Differences between metrics are only meaningful when exactly 2 views are compared.");
      Compute_Delta = FALSE;
    } else if (Quick_Compare_Set[0].partial_view.empty()) {
      Mark_Cmd_With_Soft_Error(cmd, "The first view is empty.  No differences can be computed.");
      Compute_Delta = FALSE;
      cmd->Result_Predefined (Quick_Compare_Set[0].partial_view);
      return true;
    } else if (Quick_Compare_Set[1].partial_view.empty()) {
      Mark_Cmd_With_Soft_Error(cmd, "The second view is empty.  No differences can be computed.");
      Compute_Delta = FALSE;
      cmd->Result_Predefined (Quick_Compare_Set[0].partial_view);
      return true;
    }
  }

#if DEBUG_COMPARE_SETS
  printf("SSCOMPARE: IN Generate_CustomView, START 2nd print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
  for (i = 0; i < numQuickSets; i++) {
    std::cerr << i;
    Quick_Compare_Set[i].Print(std::cerr);
  }
  printf("SSCOMPARE: IN Generate_CustomView, END 2nd print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
#endif



#if DEBUG_COMPARE_SETS
  printf("SSCOMPARE: IN Generate_CustomView, START ------- print of generated views, numQuickSets=%d\n", numQuickSets);
  std::cerr << "Display generated views\n";
  for (i = 0; i < numQuickSets; i++) {
    std::cerr << "Set " << i << "\n";
    if (Quick_Compare_Set[i].partial_view.empty()) {
      std::cerr << "   (empty)\n";
    } else {
      std::list<CommandResult *>::iterator coi;
      for (coi = Quick_Compare_Set[i].partial_view.begin();
           coi != Quick_Compare_Set[i].partial_view.end();
           coi++) {
        CommandResult *c = *coi;
        c->Print(std::cerr);
        std::cerr << "\n";
      }
    }
  }
  printf("SSCOMPARE: IN Generate_CustomView, END ------- 1st print of generated views, numQuickSets=%d\n", numQuickSets);
#endif

// Merge the results from each separate view into a single view.

 // Start by determining the new column headers.
  int64_t num_columns = 0;
  int64_t rows_in_Set0 = 0;
  int64_t enders_in_allSets = 0;
  bool diff_column_needed = Compute_Delta;
  CommandResult_Headers *C = new CommandResult_Headers ();
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
            if (diff_column_needed) {
              std::string delta = std::string("Difference Between ") + s;
              C->CommandResult_Headers::Add_Header (CRPTR (delta));
              num_columns++;
              diff_column_needed = false;
            }
            std::string p = Quick_Compare_Set[i].headerPrefix;
            int64_t h_len = p.size();
            if (h_len > 0) {
              if (p[h_len-1] == *" ") {
               // Use "," between origial header and prefix.
                p.replace((h_len-1), 1, ", ");
              }
              p += s;
              C->CommandResult_Headers::Add_Header (CRPTR (p));
            } else {
              C->CommandResult_Headers::Add_Header (CRPTR (s));
            }
          }
          if (last_header == NULL) {
           // Save the first right-most column header for the final report.
            last_header = *hi;
          }
        }
      } else if (c->Type() == CMD_RESULT_COLUMN_VALUES) {
        if (i == 0) rows_in_Set0++;
      } else if (c->Type() == CMD_RESULT_COLUMN_ENDER) {
        enders_in_allSets++;
      }
    }
  }

  if (num_columns == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There was no data available to compare.)");
    return false;
  }

#if DEBUG_COMPARE_SETS
  printf("SSCOMPARE: IN Generate_CustomView, START 3rd print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
  for (i = 0; i < numQuickSets; i++) {
    std::cerr << i;
    Quick_Compare_Set[i].Print(std::cerr);
  }
  printf("SSCOMPARE: IN Generate_CustomView, END 3rd print of Quick_Compare_Sets, numQuickSets=%d\n", numQuickSets);
#endif

 // Add the header for the last column and attach all of them to the output report.
  C->CommandResult_Headers::Add_Header ( last_header->Copy() );
  cmd->Result_Predefined (C); // attach column headers to output

 // Build the master maps.
 // The master_map associates a CommandResult * with an index.
 // The index is into master_vector, which points to the Function, Statement, LinkedObject, ...

  int64_t num_rows = 0;
  std::vector<CommandResult *> master_vector(rows_in_Set0);
  std::map<CommandResult *, int64_t, ltCR> master_map;
  int64_t num_enders = 0;
//  std::vector<std::vector<CommandResult *> > master_ender_vector(enders_in_allSets,numQuickSets);

  std::vector<std::vector<CommandResult*> > master_ender_vector(
		enders_in_allSets, std::vector<CommandResult*>(numQuickSets));

  std::map<CommandResult *, int64_t, ltCR> master_ender_map;

 // Initial the master maps with information from Quick_Compare_Set[0].
  for (coi = Quick_Compare_Set[0].partial_view.begin(); coi != Quick_Compare_Set[0].partial_view.end(); coi++) {
    CommandResult *c = *coi;

#if DEBUG_COMPARE_SETS
    printf("SSCOMPARE: IN Generate_CustomView, inside loop going through Quick_Compare_Set[0].partial_view, c->Type()=%d, CMD_RESULT_COLUMN_VALUES=%d, CMD_RESULT_COLUMN_ENDER=%d\n", 
           c->Type(), CMD_RESULT_COLUMN_VALUES, CMD_RESULT_COLUMN_ENDER);
#endif

    if (c->Type() == CMD_RESULT_COLUMN_VALUES) {

      std::list<CommandResult *> L;
      ((CommandResult_Headers *)c)->Value(L);
      CommandResult *last_column = NULL;
      std::list<CommandResult *>::iterator li;
      for (li = L.begin(); li != L.end(); li++) { last_column = *li; }
      Assert (last_column != NULL);
      master_map[last_column] = num_rows;
      master_vector[num_rows++] = last_column;

    } else if (c->Type() == CMD_RESULT_COLUMN_ENDER) {

      std::list<CommandResult *> L;
      ((CommandResult_Enders *)c)->Value(L);
      CommandResult *last_column = NULL;
      std::list<CommandResult *>::iterator li;
      for (li = L.begin(); li != L.end(); li++) { last_column = *li; }
      Assert (last_column != NULL);
      master_ender_map[last_column] = num_enders;
      master_ender_vector[num_enders++][0] = c;

    }
  }
  int64_t Set0_rows = num_rows;

#if DEBUG_COMPARE_SETS
    printf("SSCOMPARE: IN Generate_CustomView, after loop going through Quick_Compare_Set[0].partial_view, Set0_rows=%d, numQuickSets=%d, num_enders=%d\n", 
           Set0_rows, numQuickSets, num_enders);
#endif

 // Build maps for all but the first compare sets.
 // These maps associate the right most column value with the index into the master map.
  for (i = 1; i < numQuickSets; i++) {

    std::list<CommandResult *>::iterator coi;
    for (coi = Quick_Compare_Set[i].partial_view.begin(); coi != Quick_Compare_Set[i].partial_view.end(); coi++) {

      CommandResult *c = *coi;

      if (c->Type() == CMD_RESULT_COLUMN_VALUES) {

#if DEBUG_COMPARE_SETS
        printf("SSCOMPARE: IN Generate_CustomView, in c->Type() == CMD_RESULT_COLUMN_VALUES section\n");
#endif

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
#if DEBUG_COMPARE_SETS
        printf("SSCOMPARE: IN Generate_CustomView, master_index section, master_index=%d\n", master_index);
#endif
       
       // Map the master_vector index to the start of data for this entry.
       // Warn about names that appear more than once in a view if the less restrictive comparison is not on
        if (Quick_Compare_Set[i].merge_map[master_index] != NULL && !OPENSS_LESS_RESTRICTIVE_COMPARISONS) {
         // This is an error and should be reported.
          std::ostringstream M;
          M << "The name portion of '" << last_column->Form()
            << "' appears more than once in the view."
            << " Only the first occurrence is included in the generated comparison.";
          Mark_Cmd_With_Soft_Error(cmd, M.str());
        } else {
          Quick_Compare_Set[i].merge_map[master_index] = c;
        }

      } else if (c->Type() == CMD_RESULT_COLUMN_ENDER) {

#if DEBUG_COMPARE_SETS
        printf("SSCOMPARE: IN Generate_CustomView, in c->Type() == CMD_RESULT_COLUMN_ENDER section\n");
#endif

        std::list<CommandResult *> L;
        ((CommandResult_Enders *)c)->Value(L);
        CommandResult *last_column = NULL;
        std::list<CommandResult *>::iterator li;
        for (li = L.begin(); li != L.end(); li++) { last_column = *li; }
        Assert (last_column != NULL);
        std::map<CommandResult *, int64_t, ltCR>::iterator result = master_ender_map.find(last_column);
        int64_t master_ender_index = -1;

        if (master_ender_map.end() == result) {

         // Need to add a new entry into the master.
          master_ender_index = num_enders++;
          master_ender_map[last_column] = master_ender_index;

        } else {

         // Use the index to an existing entry.
          master_ender_index = (*result).second;

        }

       // Map the master_ender_vector index to the start of data for this entry.
        Assert (master_ender_vector[master_ender_index][i] == NULL);
        master_ender_vector[master_ender_index][i] = c;

      }
    }
  }

 // Find a non-null value for each column so that we can know the
 // cmd_result_type_enum associated with the column.
 //
 // First: Determine the total number of columns of data.
  int64_t total_columns = Compute_Delta ? 1 : 0;
  for (i = 0; i < numQuickSets; i++) {
    int64_t numColumns = Quick_Compare_Set[i].numColumns;
    total_columns += numColumns;
  }
 // Second: Look for a valid data entry in each column and
 //         remember it so we can use it to create a NULL entry.
  std::vector<CommandResult *> Base_Column_Type(total_columns);
  int64_t next_column = 0;
  for (i = 0; i < numQuickSets; i++) {
    std::list<CommandResult *>::iterator Seti = Quick_Compare_Set[i].partial_view.begin();
    int64_t numColumns = Quick_Compare_Set[i].numColumns;
    for (int64_t rc = 0; rc < Quick_Compare_Set[i].partial_view.size(); rc++) {
     // Find the first set of column data in the Quick_Compare_Set.
      while ((*Seti)->Type() != CMD_RESULT_COLUMN_VALUES) { Seti++; }
     // Record the first row of data in the Base_Column_Type array.
      std::list<CommandResult *> DL;
      ((CommandResult_Columns *)(*Seti))->Value(DL);
      std::list<CommandResult *>::iterator Di = DL.begin();
      CommandResult *D = (*Di);
      if (Compute_Delta && (i == 0)) {
        Base_Column_Type[next_column++] = D;
      }
      for (int64_t j = 0; j < numColumns; j ++, Di++) {
        Base_Column_Type[next_column++] = D;
      }
      Assert (next_column <= total_columns);
      break;
    }
  }
 // Third: Check that a base type entry is available to generate NULLs for every column.
  next_column = 0;

  if (Compute_Delta) {
    Assert (Base_Column_Type[next_column++] != NULL);
  }

  for (int64_t i = 0; i < Quick_Compare_Set[0].numColumns; i ++) {
    Assert (Base_Column_Type[next_column++] != NULL);
  }

  for (int64_t i = 1; i < numQuickSets; i++) {
    for (int64_t j = 0; j < Quick_Compare_Set[i].numColumns; j++) {
      if (Base_Column_Type[next_column] == NULL) {
       // Steal NULL type entry from base metrics and hope it will be okay.
        Base_Column_Type[next_column] = Base_Column_Type[(Compute_Delta?1:0) + j];
      }
      next_column++;
    }
  }

 // Merge the values from the various compare sets.
  int64_t Set0_Columns = Quick_Compare_Set[0].numColumns;
  std::list<CommandResult *> Set0 = Quick_Compare_Set[0].partial_view;
  std::list<CommandResult *>::iterator Set0i = Set0.begin();

#if DEBUG_COMPARE
   printf("SSCOMPARE: In Generate_CustomView, Merge the values from the various compare sets, numQuickSets=%d,  master_vector.size()=%d, Set0_rows=%d\n",  
           numQuickSets,  master_vector.size(), Set0_rows);
#endif

  for (int64_t rc = 0; rc < master_vector.size(); rc++) {
    CommandResult_Columns *NC = new CommandResult_Columns ();
    CommandResult *delta = NULL;
    int64_t next_column = 0;

   // Get data for set[0].
    if (rc < Set0_rows) {
     // Find the next set of column data.
      while ((*Set0i)->Type() != CMD_RESULT_COLUMN_VALUES) { Set0i++; }
     // Copy original view data into the compare report.
      std::list<CommandResult *> DL;
      ((CommandResult_Columns *)(*Set0i))->Value(DL);
      Assert (Set0_Columns == (DL.size() - 1));
      std::list<CommandResult *>::iterator Di = DL.begin();
      if (Compute_Delta) {
        delta = (*Di)->Copy();
        NC->CommandResult_Columns::Add_Column ( delta );
        next_column++;
      }
      for (int64_t j = 0; j < Set0_Columns; j ++, Di++) {
        NC->CommandResult_Columns::Add_Column ( (*Di)->Copy() );
        next_column++;
      }
      Set0i++;
    } else {
     // Fill the columns with place holders.
      if (Compute_Delta) {
        Assert(next_column < total_columns);
        NC->CommandResult_Columns::Add_Column ( CR_Init_of_CR_type( Base_Column_Type[next_column++] ) );
      }
      for (int64_t j = 0; j < Set0_Columns; j++) {
        Assert(next_column <= total_columns);
        NC->CommandResult_Columns::Add_Column ( CR_Init_of_CR_type( Base_Column_Type[next_column++] ) );
      }
    }

#if DEBUG_COMPARE
   printf("SSCOMPARE: In Generate_CustomView, get data for other sets numQuickSets=%d\n",  numQuickSets);
#endif

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
        if (Compute_Delta) {
          NC->CommandResult_Columns::Column_AbsDiff( (*Di), 0 );
        }
        for (int64_t j = 0; j < numColumns; j ++, Di++) {
          Assert(next_column <= total_columns);
          NC->CommandResult_Columns::Add_Column ( (*Di)->Copy() );
          next_column++;
        }
      } else {
       // Fill the columns with place holders.
        int64_t next_column = 0;
        for (int64_t j = 0; j < numColumns; j++) {
          NC->CommandResult_Columns::Add_Column ( CR_Init_of_CR_type( Base_Column_Type[next_column++] ) );
        }
      }
    }

    NC->CommandResult_Columns::Add_Column ( master_vector[rc]->Copy() );
    cmd->Result_Predefined (NC);
  }

 // Append the Enders.

#if DEBUG_COMPARE
   printf("SSCOMPARE: In Generate_CustomView, get data for each ender (num_enders=%d) of each numQuickSets=%d\n", num_enders, numQuickSets);
#endif

  for (int64_t ec = 0; ec < num_enders; ec++) {
    CommandResult_Enders *NC = new CommandResult_Enders ();
    CommandResult *last_column = NULL;
    CommandResult *delta = NULL;
    int64_t next_column = 0;

   // Get data for each ender of each Quick_Compare_Set.
    for (i = 0; i < numQuickSets; i++) {
      int64_t numColumns = Quick_Compare_Set[i].numColumns;
      CommandResult *Qset_Ender = master_ender_vector[ec][i];
      if (Qset_Ender !=NULL) {
       // Copy the column data from the original ender.
        std::list<CommandResult *> DL;
        ((CommandResult_Enders *)Qset_Ender)->Value(DL);

        std::list<CommandResult *>::iterator Di;
        for (Di = DL.begin(); Di != DL.end(); Di++) { last_column = *Di; }

        Di = DL.begin();
        if (Compute_Delta) {
          if (i == 0) {
            delta = (*Di)->Copy();
            NC->CommandResult_Enders::Add_Ender ( delta );
          } else {
            NC->CommandResult_Enders::Enders_AbsDiff( (*Di), 0 );
          }
          next_column++;
        }
        for (int64_t j = 0; j < numColumns; j ++, Di++) {
          NC->CommandResult_Enders::Add_Ender ( (*Di)->Copy() );
          next_column++;
        }
      } else {
       // Fill the columns with place holders.
        if (Compute_Delta && (i == 0)) {
          NC->CommandResult_Enders::Add_Ender ( CR_Init_of_CR_type( Base_Column_Type[next_column++] ) );
        }
        for (int64_t j = 0; j < numColumns; j++) {
          NC->CommandResult_Enders::Add_Ender ( CR_Init_of_CR_type( Base_Column_Type[next_column++] ) );
        }
      }
    }

    Assert (last_column != NULL);
    NC->CommandResult_Enders::Add_Ender ( last_column->Copy() );
    cmd->Result_Predefined (NC);
  }

 // Free remaining space for CommandResults.
  for (i = 0; i < numQuickSets; i++) {
    Reclaim_CR_Space (Quick_Compare_Set[i].partial_view);
  }

#if DEBUG_COMPARE
  printf("SSCOMPARE: EXIT Generate_CustomView, numQuickSets=%d\n", numQuickSets);
#endif

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_expCompare (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  Assert(cmd->P_Result() != NULL);
  Assert (cmd->P_Result()->getViewSet()->empty());

#if DEBUG_REUSEVIEWS
    std::cerr << "ENTER SS_expCompare in SS_Compare.cxx" << std::endl;
#endif

  int64_t i;
  std::vector<selectionTarget> Quick_Compare_Set;

 // Pick up the <expID> from the command.
 // Note: we must always generate at least one compare set for this list.
  std::vector<ParseRange> *p_elist = cmd->P_Result()->getExpIdList();
  if (p_elist->begin() == p_elist->end()) {
   // The user has not selected an experiment look for the focused experiment.
    EXPID ExperimentID = Experiment_Focus ( WindowID );
    ExperimentObject *exp = (ExperimentID != 0) ?
                            Find_Experiment_Object (ExperimentID) :
                            NULL;
    if ((exp == NULL) ||
        (exp->FW() == NULL)) {
     // No experiment was specified, so we can't find a useful view to generate.
      Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified for comparison.");
      return false;
    } else {
     // Initialize a single compare set for the focused experiment.
      selectionTarget S;
      S.Exp = exp;
      Quick_Compare_Set.push_back (S);
    }
  } else {
   // Generate compare sets for every experiment.
    std::vector<ParseRange>::iterator ei;
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
          Mark_Cmd_With_Soft_Error(cmd, M.str());
          return false;
        }

       // Define new compare sets for each experiment.
        std::ostringstream M;
        M << "-x " << ExperimentID << " ";
        selectionTarget S;
        S.Exp = exp;
        S.headerPrefix = M.str();
        Quick_Compare_Set.push_back (S);
      }

    }

  }


 // Pick up the <viewType> from the command or the experiment
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getViewList();
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
        Mark_Cmd_With_Soft_Error(cmd, M.str());
        return false;
      }

      if ((Quick_Compare_Set.size() > 1) && (views.size() > 1)) {
       // Too much stuff - we don't know what to compare.
        std::ostringstream M;
        M << "Multiple views possible for '-x " << exp->ExperimentObject_ID() << "'.";
        Mark_Cmd_With_Soft_Error(cmd, M.str());
        return false;
      }

     // For every viewname, create a compare set.
      if (views.size() == 1) {
       // One view for this experiment.
       // Add the view to the compare set for this experiment.
        Quick_Compare_Set[k].viewName = views[0];

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
          Quick_Compare_Set[j].headerPrefix += views[j] + " ";
        }

       // Don't continue the "k" loop.
       // There was originaly one experiment, but we just created
       // multiple compare sets, so our original loop is invalid.
        break;
      }

    }
  } else {
   // Generate all the views in the list.

    std::vector<std::string> views;
    std::vector<std::string>::iterator vi;
    for (vi=p_slist->begin();vi != p_slist->end(); vi++) {
      std::string viewname = *vi;
      ViewType *vt = Find_View (viewname);
      if (vt == NULL) {
        std::ostringstream M;
        M << "The requested view, '" << viewname << "' is not available.";
        Mark_Cmd_With_Soft_Error(cmd, M.str());
        return false;
      }
      views.push_back (viewname);
    }

    if (Quick_Compare_Set.size() == 0) {
     // Define new compare sets for each view.
      for (int64_t j = 0; j < views.size(); j++) {
        selectionTarget S;
        S.viewName = views[j];
        S.headerPrefix += views[j] + " ";
        Quick_Compare_Set.push_back (S);
      }
    } else if (views.size() == 1) {

     // Append the single view to the existing sets.
      for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
        Quick_Compare_Set[j].viewName = views[0];
        Quick_Compare_Set[j].headerPrefix += views[0] + " ";
      }
    } else if (views.size() > 1) {
      Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
      return false;
    } else {

     // Make N-1 copies of the original set.
      for (int64_t j = 1; j < views.size(); j++) {
        Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
      }

     // Append each view to the corresponding set.
      for (int64_t j = 0; j < views.size(); j++) {
        Quick_Compare_Set[j].viewName = views[j];
        Quick_Compare_Set[j].headerPrefix += views[j] + " ";
      }
    }

  }
  

 // The <target_spec> will define the comparisons.
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();
  if (p_tlist->size() > 1) {
   // There are no filters.
    Mark_Cmd_With_Soft_Error(cmd, "There is more than one target specification.");
    return false;
  }

 // Pick up the field definitions from the first ParseTarget.
  if (!p_tlist->empty()) {
    ParseTarget *pt = &((*p_tlist)[0]);
    std::vector<ParseRange> *h_list = pt->getHostList();
    std::vector<ParseRange> *p_list = pt->getPidList();
    std::vector<ParseRange> *t_list = pt->getThreadList();
    std::vector<ParseRange> *r_list = pt->getRankList();

    if ((h_list != NULL) && !h_list->empty()) {
     // Start by building a vector of all the host names.
      std::vector<std::string> hosts;
      std::vector<ParseRange>::iterator hi;
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

      if (Quick_Compare_Set.size() == 0) {
       // Define new compare sets for each host.
        for (int64_t j = 0; j < hosts.size(); j++) {
          selectionTarget S;
          S.headerPrefix = "-h " + hosts[j] + " ";
          S.hostId = Experiment::getCanonicalName(hosts[j]);
          Quick_Compare_Set.push_back (S);
        }
      } else if (hosts.size() == 1) {

       // Append the single host to the existing sets.
        for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
          Quick_Compare_Set[j].headerPrefix += "-h " + hosts[0] + " ";
        }
      } else if (Quick_Compare_Set.size() > 1) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      } else {

       // Make N-1 copies of the original set.
        for (int64_t j = 1; j < hosts.size(); j++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
        }

       // Append each host to the corresponding set.
        for (int64_t j = 0; j < hosts.size(); j++) {
          selectionTarget S;
          Quick_Compare_Set[j].headerPrefix += "-h " + hosts[j] + " ";
          Quick_Compare_Set[j].hostId = Experiment::getCanonicalName(hosts[j]);
        }

      }
    }

    if ((p_list != NULL) && !p_list->empty()) {
     // Start by building a vector of all the pids.
      std::vector<pid_t> pids;;
      std::vector<ParseRange>::iterator pi;
      for (pi = p_list->begin();pi != p_list->end(); pi++) {
        parse_range_t *p_range = pi->getRange();
        parse_val_t pval1 = p_range->start_range;
        parse_val_t pval2 = p_range->is_range ? p_range->end_range : pval1;
        Assert   (pval1.tag == VAL_NUMBER);
        Assert   (pval2.tag == VAL_NUMBER);

        pid_t mypid;
        for ( mypid = pval1.num; mypid <= pval2.num; mypid++) {
          pids.push_back (mypid);
        }

      }

      if (Quick_Compare_Set.size() == 0) {
       // Define new compare sets for each pid.
        for (int64_t j = 0; j < pids.size(); j++) {
          int64_t P = pids[j]; char s[40]; sprintf ( s, "-p %lld ", P);
          selectionTarget S;
          S.headerPrefix = s;
          S.pidId = pids[j];
          Quick_Compare_Set.push_back (S);
        }
      } else if (pids.size() == 1) {

       // Append the single pid to the existing sets.
        char s[40]; sprintf ( s, "-p %lld ", pids[0]);
        for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
          Quick_Compare_Set[j].headerPrefix += s;
        }
      } else if (Quick_Compare_Set.size() > 1) {
         Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
         return false;
      } else {

       // Make N-1 copies of the original set.
        for (int64_t j = 1; j < pids.size(); j++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
        }

       // Append each pid to the corresponding set.
        for (int64_t j = 0; j < pids.size(); j++) {
          int64_t P = pids[j]; char s[40]; sprintf ( s, "-p %lld ", P);
          Quick_Compare_Set[j].headerPrefix += s;
          Quick_Compare_Set[j].pidId = pids[j];
        }
      }
    }

    if ((t_list != NULL) && !t_list->empty()) {
     // Start by building a vector of all the threads names.
      std::vector<int64_t> threadids;;
      std::vector<ParseRange>::iterator pi;
      for (pi = t_list->begin();pi != t_list->end(); pi++) {
        parse_range_t *p_range = pi->getRange();
        parse_val_t *pval1 = &p_range->start_range;
        parse_val_t *pval2 = p_range->is_range ? &p_range->end_range : pval1;

        int64_t mythreadid;
        for ( mythreadid = pval1->num; mythreadid <= pval2->num; mythreadid++) {
          threadids.push_back (mythreadid);
        }

      }

      if (Quick_Compare_Set.size() == 0) {
       // Define new compare sets for each host.
        for (int64_t j = 0; j < threadids.size(); j++) {
          char s[40]; sprintf ( s, "-t %lld ", threadids[j]);
          selectionTarget S;
          S.headerPrefix = s;
          S.threadId = threadids[j];
          Quick_Compare_Set.push_back (S);
        }
      } else if (threadids.size() == 1) {

       // Append the single thread to the existing sets.
        char s[40]; sprintf ( s, "-t %lld ", threadids[0]);
        for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
          Quick_Compare_Set[j].headerPrefix += s;
        }
      } else if (Quick_Compare_Set.size() > 1) {
        Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
        return false;
      } else {

       // Make N-1 copies of the original set.
        for (int64_t j = 1; j < threadids.size(); j++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
        } 

       // Append each thread to the corresponding set.
        for (int64_t j = 0; j < threadids.size(); j++) {
          char s[40]; sprintf ( s, "-t %lld ", threadids[j]);
          selectionTarget S;
          Quick_Compare_Set[j].headerPrefix += s;
          Quick_Compare_Set[j].threadId = threadids[j];
        }
      }
    }

    if ((r_list != NULL) && !r_list->empty()) {
     // Start by building a vector of all the ranks.
      std::vector<int64_t> rankids;;
      std::vector<ParseRange>::iterator pi;
      for (pi = r_list->begin();pi != r_list->end(); pi++) {
        parse_range_t *p_range = pi->getRange();
        parse_val_t *pval1 = &p_range->start_range;
        parse_val_t *pval2 = p_range->is_range ? &p_range->end_range : pval1;

        for (int64_t  myrankid = pval1->num; myrankid <= pval2->num; myrankid++) {
          rankids.push_back (myrankid);
        }

      }

      if (Quick_Compare_Set.size() == 0) {
       // Define new compare sets for each host
        for (int64_t j = 0; j < rankids.size(); j++) {
          char s[40]; sprintf ( s, "-r %lld ", rankids[j]);
          selectionTarget S;
          S.headerPrefix = s;
          S.rankId = rankids[j];
          Quick_Compare_Set.push_back (S);
        }
      } else if (rankids.size() == 1) {

       // Append the single rank to the existing sets.
        char s[40]; sprintf ( s, "-r %lld ", rankids[0]);
        for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
          Quick_Compare_Set[j].headerPrefix += s;
        }
      } else if (Quick_Compare_Set.size() > 1) {
          Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
          return false;
      } else {

       // Make N-1 copies of the original set.
        for (int64_t j = 1; j < rankids.size(); j++) {
          Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
        } 

       // Append each rank to the corresponding set.
        for (int64_t j = 0; j < rankids.size(); j++) {
          char s[40]; sprintf ( s, "-r %lld ", rankids[j]);
          Quick_Compare_Set[j].headerPrefix += s;
          Quick_Compare_Set[j].rankId = rankids[j];
        }
      }
    }
  }

 // Pick up the time interval definitions from the ParseResult.
  std::vector<ParseInterval> *interval_list = p_result->getParseIntervalList();
  if (interval_list->size() > 1) {
   // More than 1 implies that we want to compare across time segments.

    if (Quick_Compare_Set.size() == 0) {
     // Define new compare sets for each time interval.
      for (std::vector<ParseInterval>::iterator
              iv = interval_list->begin(); iv != interval_list->end(); iv++) {
        std::ostringstream H;
        ParseInterval P = *iv;
        H << "-I ";
        if (p_result->isIntervalAttribute()) {
          H << (*p_result->getIntervalAttribute()) << " ";
        }
        if (P.isStartInt()) {
          H << P.getStartInt();
        } else {
          H << P.getStartdouble();
        }
        if (P.isEndInt()) {
          H << ":" << P.getEndInt();
        } else {
          H << ":" << P.getEndDouble();
        }

        selectionTarget S;
        S.headerPrefix = H.str() + " ";
        S.timeSegment.push_back(*iv);
        Quick_Compare_Set.push_back (S);
      }
    } else if (interval_list->size() == 1) {

       // Append the single time interval to the existing sets.
        ParseInterval P = (*interval_list)[0];
        std::ostringstream H;
        H << "-I ";
        if (p_result->isIntervalAttribute()) {
          H << (*p_result->getIntervalAttribute()) << " ";
        }
        if (P.isStartInt()) {
          H << P.getStartInt();
        } else {
          H << P.getStartdouble();
        }
        if (P.isEndInt()) {
          H << ":" << P.getEndInt();
        } else {
          H << ":" << P.getEndDouble();
        }
        H << " ";

        for (int64_t j = 0; j < Quick_Compare_Set.size(); j++) {
        Quick_Compare_Set[j].headerPrefix += H.str();
        Quick_Compare_Set[j].timeSegment.push_back(P);
        }
    } else if (Quick_Compare_Set.size() > 1) {
      Mark_Cmd_With_Soft_Error(cmd, "Multiple compare lists are not supported.");
      return false;
    } else {

     // Make N-1 copies of the original set.
      for (int64_t j = 1; j < interval_list->size(); j++) {
        Quick_Compare_Set.push_back (Quick_Compare_Set[0]);
      }

     // Append each time interval to the corresponding set.
      for (int64_t j = 0; j < interval_list->size(); j++) {
        ParseInterval P = (*interval_list)[j];
        std::ostringstream H;
        H << "-I ";
        if (p_result->isIntervalAttribute()) {
          H << (*p_result->getIntervalAttribute()) << " ";
        }
        if (P.isStartInt()) {
          H << P.getStartInt();
        } else {
          H << P.getStartdouble();
        }
        if (P.isEndInt()) {
          H << ":" << P.getEndInt();
        } else {
          H << ":" << P.getEndDouble();
        }
        H << " ";

        Quick_Compare_Set[j].headerPrefix += H.str();
        Quick_Compare_Set[j].timeSegment.push_back(P);
      }

    }

  }
#if DEBUG_REUSEVIEWS
    std::cerr << "IN SS_expCompare in SS_Compare.cxx, before saved view checks" << std::endl;
#endif

 // Look for a saved view.
  savedViewInfo *svi = NULL;

  if ( OPENSS_SAVE_VIEWS_FOR_REUSE && Find_SavedView( cmd ) ) {

#if DEBUG_REUSEVIEWS
    std::cerr << "In SS_expCompare in SS_Compare.cxx, reuse view from file:" << cmd->SaveResultFile().c_str() << std::endl;
#endif

    if (!cmd->SaveResult()) {
     // Previously generated output file found with requested view.

#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_expCompare (SS_Compare.cxx), Previously generated output file found with requested view:!cmd->SaveResult==true" << std::endl;
#endif
      cmd->set_Status(CMD_COMPLETE);
      return true;
    }

   // An existing saved view is not available but provision has been made to create a new one.
    svi = cmd->SaveResultViewInfo();

#if DEBUG_REUSEVIEWS
    std::cerr << "In SS_expCompare in SS_Compare.cxx, An existing saved view is not available but provision has been made to create a new one, svi=" << svi << std::endl;
#endif

    if (svi != NULL) {
     // Set StartTime to measure how long it takes to generate the view.
      svi->setStartTime();
    }
  }

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_expCompare in SS_Compare.cxx, before calling Generate_CustomView" << std::endl;
#endif

  bool success = Generate_CustomView (cmd, Quick_Compare_Set);

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_expCompare in SS_Compare.cxx, after calling Generate_CustomView" << std::endl;
#endif

 // Set EndTIme if saving info after generation.
  svi = cmd->SaveResultViewInfo();
  if (svi != NULL) {
    if (success) {
      svi->setEndTime();
      std::ostream *tof = svi->writeHeader ();
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_expCompare in SS_Compare.cxx, writing header and setting end-time, tof=" << tof << std::endl;
#endif
      cmd->setSaveResultOstream( tof );
    } else {
      svi->setDoNotSave();
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_expCompare in SS_Compare.cxx, setting DONOTSAVE, svi=" << svi << std::endl;
#endif
    }
  }

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
    std::vector<ParseRange> *cv_list = p_result->getViewSet ();
    std::vector<ParseRange>::iterator cvli;

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

static std::string CustomViewInfo (CommandObject *cmd, CustomView *cvp) {

  OpenSpeedShop::cli::ParseResult *p_result = cvp->cvPr();

#if DEBUG_CVIEWINFO
  std::cerr << "CustomViewInfo, beginning of dumping p_result containing information used to build cviewinfo outputs" << std::endl;
  if (p_result != NULL) {
      p_result->ParseResult::dumpInfo();
  }
  std::cerr << "CustomViewInfo, end of dumping p_result containing information used to build cviewinfo outputs" << std::endl;
#endif

  std::ostringstream S(std::ios::out);

 // List viewTypes.
  std::vector<std::string> *vtlist = p_result->getViewList();
  if (!vtlist->empty()) {
    bool first_item_found = false;
    std::vector<std::string>::iterator p_vtlist;
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
  std::vector<std::string> *v_list = p_result->getModifierList();
  if (!v_list->empty()) {
    bool first_item_found = false;
    std::vector<std::string>::iterator p_vlist;
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
  int global_overall_num_ranks = -1;
  int global_overall_num_threads = -1;
  EXPID ExperimentID = (p_result->isExpId()) ? p_result->getExpId() : 0;
  if (ExperimentID != 0) {
    ExperimentObject *exp = Find_Experiment_Object (ExperimentID) ;
    S << " -x " << ExperimentID;
    if (exp > 0) {
      global_overall_num_ranks = SS_getNumRanks(cmd, exp);
      global_overall_num_threads = SS_getNumThreads(cmd, exp);
    }
  }

  
 // For each set of filter specifications ...
  int64_t header_length = S.str().size();
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();
  std::vector<ParseTarget>::iterator pi;

#if 0
 // Add the "-h" list
  std::set<std::string> hset;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *h_list = pt.getHostList();

    if (!h_list->empty()) {
      std::vector<ParseRange>::iterator p_hlist;
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
    }
  }
  bool first_h_item_found = false;
  for (std::set<std::string>::iterator hseti = hset.begin(); hseti != hset.end(); hseti++) {
    if (first_h_item_found) {
      S << ", ";
    } else {
      S << " -h ";
    }
    first_h_item_found = true;
    S << *hseti;
  }
#endif

#if 1
 // Add the "-f" list.
  std::set<std::string> fset;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    //std::vector<ParseRange> *h_list = pt.getHostList();
    std::vector<ParseRange> *f_list = pt.getFileList();

    if (!f_list->empty()) {
      std::vector<ParseRange>::iterator p_flist;
      for (p_flist=f_list->begin();p_flist != f_list->end(); p_flist++) {
        parse_range_t p_range = *p_flist->getRange();
        // std::string N = Experiment::getCanonicalName(p_range.start_range.name);
        std::string N = p_range.start_range.name;
        if (p_range.is_range &&
            (p_range.start_range.name != p_range.end_range.name)) {
          N = N + ":" + p_range.end_range.name;
        }
        fset.insert (N);
      }
    }
  }
  bool first_f_item_found = false;
  for (std::set<std::string>::iterator fseti = fset.begin(); fseti != fset.end(); fseti++) {
    if (first_f_item_found) {
      S << ", ";
    } else {
      S << " -f ";
    }
    first_f_item_found = true;
    S << *fseti;
  }
#endif

#if 0
 // Add the "-p" list.
  std::map<pid_t, parse_range_t> pset;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *p_list = pt.getPidList();

    if (!p_list->empty()) {
      std::vector<ParseRange>::iterator p_plist;
      for (p_plist=p_list->begin();p_plist != p_list->end(); p_plist++) {
        parse_range_t p_range = *p_plist->getRange();
        pid_t start_range = p_range.start_range.num;
        std::map<pid_t, parse_range_t>::iterator pseti = pset.find(start_range);
        if (pseti == pset.end()) {
//        if ((*(pset.find(start_range))).first == 0) {
          pset[start_range] = p_range;
        }
      }
    }
  }
  bool first_p_item_found = false;
  for (std::map<pid_t, parse_range_t>::iterator pseti = pset.begin(); pseti != pset.end(); pseti++) {
    if (first_p_item_found) {
      S << ", ";
    } else {
      S << " -p ";
    }
    first_p_item_found = true;
    parse_range_t p_range = (*pseti).second;
    S << p_range.start_range.num;
    if (p_range.is_range &&
        (p_range.start_range.num != p_range.end_range.num)) {
      S << ":" << p_range.end_range.num;
    }
  }

 // Add the "-t" list.
  std::map<int64_t, parse_range_t> tset;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *t_list = pt.getThreadList();

    if (!t_list->empty()) {
      std::vector<ParseRange>::iterator p_tlist;
      for (p_tlist=t_list->begin();p_tlist != t_list->end(); p_tlist++) {
        parse_range_t p_range = *p_tlist->getRange();
        int64_t start_range = p_range.start_range.num;
        std::map<int64_t, parse_range_t>::iterator tseti = tset.find(start_range);
        if (tseti == tset.end()) {
          tset[start_range] = p_range;
        }
      }
    }
  }
  bool first_t_item_found = false;
  for (std::map<int64_t, parse_range_t>::iterator tseti = tset.begin(); tseti != tset.end(); tseti++) {
    if (first_t_item_found) {
      S << ", ";
    } else {
      S << " -t ";
    }
    first_t_item_found = true;
    parse_range_t p_range = (*tseti).second;
    S << p_range.start_range.num;
    if (p_range.is_range &&
        (p_range.start_range.num != p_range.end_range.num)) {
      S << ":" << p_range.end_range.num;
    }
  }

 // Add the "-r" list.
  std::map<int64_t, parse_range_t> rset;
  for (pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *t_list = pt.getRankList();

    if (!t_list->empty()) {
      std::vector<ParseRange>::iterator p_rlist;
      for (p_rlist=t_list->begin();p_rlist != t_list->end(); p_rlist++) {
        parse_range_t p_range = *p_rlist->getRange();
        int64_t start_range = p_range.start_range.num;
        std::map<int64_t, parse_range_t>::iterator rseti = rset.find(start_range);
        if (rseti == rset.end()) {
          rset[start_range] = p_range;
        }
      }
    }
  }
  bool first_r_item_found = false;
  for (std::map<int64_t, parse_range_t>::iterator rseti = rset.begin(); rseti != rset.end(); rseti++) {
    if (first_r_item_found) {
      S << ", ";
    } else {
      S << " -r ";
    }
    first_r_item_found = true;
    parse_range_t p_range = (*rseti).second;
    S << p_range.start_range.num;
    if (p_range.is_range &&
        (p_range.start_range.num != p_range.end_range.num)) {
      S << ":" << p_range.end_range.num;
    }
  }
#else
    std::vector<ParseTarget>::iterator t_iter;
    std::vector<ParseTarget> *compare_p_tlist = p_result->getTargetList();
    bool use_rank_thread_pairs = false;
    bool use_rank_only = false;
    bool qualifies_to_use_rank_thread_pairs = false;
    bool use_pid_thread_pairs = false;
    bool qualifies_to_use_pid_thread_pairs = false;
    std::vector<ParseRange>::iterator iter;
    parse_val_t *p_val1_rank;
    parse_val_t *p_val2_rank;
    parse_val_t *p_val1_pid;
    parse_val_t *p_val2_pid;
    parse_val_t *p_val1_thread;
    parse_val_t *p_val2_thread;
    parse_val_t *p_val1_host;
    parse_val_t *p_val2_host;

    int64_t count = 1;
    std::string prev_host = "";
    for (t_iter=compare_p_tlist->begin() ;t_iter != compare_p_tlist->end(); t_iter++) {

        bool have_a_host = false;
        std::vector<ParseRange> *p_list_host = t_iter->getHostList();
        for (iter=p_list_host->begin();iter != p_list_host->end(); iter++) {
#if DEBUG_CVIEWINFO
           std::cerr << "INSIDE HOST CVIEWINFO loop" << std::endl;
#endif
           parse_range_t *p_range = iter->getRange();
           if (p_range->is_range) {
#if DEBUG_CVIEWINFO
               std::cerr << "HOSTS have a range" << std::endl;
#endif
               p_val1_host = &p_range->start_range;
               p_val2_host = &p_range->end_range;
               have_a_host = true;
           } else {
#if DEBUG_CVIEWINFO
               std::cerr << "HOSTS DO NOT have a range" << std::endl;
#endif
               p_val1_host = &p_range->start_range;
               have_a_host = true;
           }
        }
        int host_size = sizeof(p_val1_host->name);
           
        if (have_a_host && p_val1_host && !p_val1_host->name.empty() && \
            strncasecmp(prev_host.c_str(), p_val1_host->name.c_str(),host_size) != 0) {
          S << " -h " << p_val1_host->name << " ";
          prev_host = p_val1_host->name;
        }

        std::vector<ParseRange> *p_list_rank = t_iter->getRankList(); 
        std::vector<ParseRange> *p_list_thread = t_iter->getThreadList(); 
#if DEBUG_CVIEWINFO
        std::cerr << "RANK CVIEWINFO before loop, p_list_rank->size()=" << p_list_rank->size() 
                  << " RANK CVIEWINFO before loop, p_list_threadrank->size()=" << p_list_thread->size() << std::endl;
#endif
        if ((p_list_rank->size() == 1) && ((p_list_thread->size() > 1) || (global_overall_num_threads > global_overall_num_ranks))) {
#if DEBUG_CVIEWINFO
           std::cerr << "RANKS section, setting use_rank_thread_pairs to TRUE" << std::endl;
#endif
           qualifies_to_use_rank_thread_pairs = true;
        }

#if 0
        if (qualifies_to_use_rank_thread_pairs) {
           S << " Rank Thread pairs: " ;
        } else {
           S << " Ranks: " ;
        } 
#endif
        for (iter=p_list_rank->begin();iter != p_list_rank->end(); iter++) {

#if DEBUG_CVIEWINFO
           std::cerr << "INSIDE RANK CVIEWINFO loop" << std::endl;
#endif
           parse_range_t *p_range = iter->getRange();
           if (p_range->is_range) {
#if DEBUG_CVIEWINFO
               std::cerr << "RANKS have a range" << std::endl;
#endif
               p_val1_rank = &p_range->start_range;
               p_val2_rank = &p_range->end_range;
           } else {
#if DEBUG_CVIEWINFO
               std::cerr << "RANKS do not have a range" << std::endl;
#endif
               p_val1_rank = &p_range->start_range;
               if ( p_val1_rank->num != -1 ) {
                  if (qualifies_to_use_rank_thread_pairs ) { 
                     use_rank_thread_pairs = true;
                  } else {
                     use_rank_only = true;
                  }
               }
           }
        }

        if (use_rank_only) {
          // Add the rank: to the output
          S << " " << p_val1_rank->num ;
        } else if (use_rank_thread_pairs) {
          // Add the rank: to the output
          S << " " << p_val1_rank->num << ":";
        } else {
          // ranks were not found look for a pid
          std::vector<ParseRange> *p_list_pid = t_iter->getPidList(); 
          std::vector<ParseRange> *p_list_thread = t_iter->getThreadList(); 
#if DEBUG_CVIEWINFO
          std::cerr << "PID CVIEWINFO before loop, p_list_pid->size()=" << p_list_rank->size() 
                    << "PID CVIEWINFO before loop, p_list_thread->size()=" << p_list_thread->size() << std::endl;
#endif
        if (p_list_pid->size() == 1 && p_list_thread->size() > 1) {
#if DEBUG_CVIEWINFO
           std::cerr << "PIDS section, setting use_pid_thread_pairs to TRUE" << std::endl;
#endif
           use_pid_thread_pairs = true;
        }
          for (iter=p_list_pid->begin();iter != p_list_pid->end(); iter++) {
#if DEBUG_CVIEWINFO
             std::cerr << "INSIDE PID CVIEWINFO loop" << std::endl;
#endif
             parse_range_t *p_range = iter->getRange();
             if (p_range->is_range) {
#if DEBUG_CVIEWINFO
                 std::cerr << "PIDS have a range" << std::endl;
#endif
                 p_val1_pid = &p_range->start_range;
                 p_val2_pid = &p_range->end_range;
             } else {
#if DEBUG_CVIEWINFO
                 std::cerr << "PIDS do not have a range" << std::endl;
#endif
                 p_val1_pid = &p_range->start_range;
             }
          }
          S << " " << p_val1_pid->num << ":";
        }
        // now add the threads to the rank: or pid:
        bool already_put_thread_out = false;
        //std::vector<ParseRange> *p_list_thread = t_iter->getThreadList(); 
        if (!use_rank_only) {
          for (iter=p_list_thread->begin();iter != p_list_thread->end(); iter++) {
#if DEBUG_CVIEWINFO
             std::cerr << "INSIDE THREAD CVIEWINFO loop" << std::endl;
#endif
             parse_range_t *p_range = iter->getRange();
             if (p_range->is_range) {
#if DEBUG_CVIEWINFO
                 std::cerr << "Threads have a range" << std::endl;
#endif
                 p_val1_thread = &p_range->start_range;
                 p_val2_thread = &p_range->end_range;
             } else {
#if DEBUG_CVIEWINFO
                 std::cerr << "Threads do not have a range" << std::endl;
#endif
                 p_val1_thread = &p_range->start_range;
                 if (already_put_thread_out) {
                   S << ":" << p_val1_thread->num ;
                 } else {
                   S << p_val1_thread->num ;
                   already_put_thread_out = true;
                 }
             }
           }
         }  // end not use_rank_only
    }

#endif

 // Output the '-m' list'
  std::vector<ParseRange> *p_mlist = p_result->getexpMetricList();
  if (!p_mlist->empty()) {
   // Add modifiers to output list.
    S << " -m ";
    bool first_item_found = false;
    std::vector<ParseRange>::iterator mi;
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

 // Return the accumulated result
  return S.str();
}

static void Report_CustomViewInfo (CommandObject *cmd,
                                   CustomView *cvp) {
  OpenSpeedShop::cli::ParseResult *p_result = cvp->cvPr();

 // Accumulate output in a string.
  std::ostringstream S(std::ios::out);
  S << "-c " << cvp->cvId() << ":";
  cmd->Result_String (S.str() + CustomViewInfo (cmd, cvp));
}

bool SS_cvInfo (CommandObject *cmd) {
  if (Look_For_KeyWord (cmd, "all")) {
   // Print all the defined custom views
    SafeToDoNextCmd ();
    std::list<CustomView *>::reverse_iterator cvi;
    for (cvi = CustomView_List.rbegin(); cvi != CustomView_List.rend(); cvi++)
    {
      Report_CustomViewInfo (cmd, (*cvi));
    }
  } else {
    OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
    std::vector<ParseRange> *cv_list = p_result->getViewSet ();
    if (cv_list->empty()) {
      if (!CustomView_List.empty()) {
       // Print the most recent one.
        Report_CustomViewInfo (cmd, *CustomView_List.begin());
      }
    } else {
     // Print the ones in the user's list.
      std::vector<ParseRange>::iterator cvli;

      for (cvli=cv_list->begin();cvli != cv_list->end(); cvli++) {
        parse_range_t *c_range = cvli->getRange();
        parse_val_t *c_val1 = &c_range->start_range;
        parse_val_t *c_val2 = (c_range->is_range) ? &c_range->end_range : c_val1;
        int64_t Rvalue1 = c_val1->num;
        int64_t Rvalue2 = c_val2->num;

        for (int64_t i = Rvalue1; i <= Rvalue2; i++) {
          CustomView *cvp = Find_CustomView (i);
          if (cvp != NULL) {
            Report_CustomViewInfo (cmd, cvp);
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
  EXPID primary_ExperimentID = 0;
  std::set<EXPID> referenced_experiments;

  std::vector<selectionTarget> Quick_Compare_Set;

  OpenSpeedShop::cli::ParseResult *primary_result = cmd->P_Result();
  std::vector<ParseRange> *cv_list = primary_result->getViewSet ();
  for (std::vector<ParseRange>::iterator
             cvli=cv_list->begin();cvli != cv_list->end(); cvli++) {
    parse_range_t *c_range = cvli->getRange();
    parse_val_t *c_val1 = &c_range->start_range;
    parse_val_t *c_val2 = (c_range->is_range) ? &c_range->end_range : c_val1;
    int64_t Rvalue1 = c_val1->num;
    int64_t Rvalue2 = c_val2->num;

    for (int64_t i = Rvalue1; i <= Rvalue2; i++) {
      CustomView *cvp = Find_CustomView (i);
      if (cvp == NULL) {
        std::ostringstream S(std::ios::out);
        S << "The requested Custom View ID, '-c " << i << "', is invalid on the 'cView' command.";
        Mark_Cmd_With_Soft_Error(cmd, S.str());
        continue;
      }

     // Make a copy of the current parse object to pick up any defined fields.
      OpenSpeedShop::cli::ParseResult *new_result = new ParseResult((*primary_result));
      std::vector<std::string> *new_view_list = new_result->getViewList();
      std::vector<std::string> *new_mod_list = new_result->getModifierList();
      std::vector<ParseRange> *new_met_list = new_result->getexpMetricList();
      std::vector<ParseTarget> *new_target_list = new_result->getTargetList();
      std::vector<ParseRange> *new_cv_list = new_result->getViewSet ();
      new_cv_list->clear();
      std::vector<ParseInterval> *new_interval_list = new_result->getParseIntervalList();

     // Get access to the parse object defined with the custom view.
      OpenSpeedShop::cli::ParseResult *old_result = cvp->cvPr();
      std::vector<std::string> *old_view_list = old_result->getViewList();
      std::vector<std::string> *old_mod_list = old_result->getModifierList();
      std::vector<ParseRange> *old_met_list = old_result->getexpMetricList();
      std::vector<ParseTarget> *old_target_list = old_result->getTargetList();
      std::vector<ParseInterval> *old_interval_list = old_result->getParseIntervalList();

      std::ostringstream N(std::ios::out);
      N << i;

     // Try to use the ID in the new parse object.
      EXPID ExperimentID = (new_result)->isExpId() ? (new_result)->getExpId() : 0;
      ExperimentObject *exp = (ExperimentID != 0) ? Find_Experiment_Object (ExperimentID) : NULL;
      if (exp == NULL) {
       // Try to get an Experiment ID from the old parse object or use the focused experiment.
        ExperimentID = old_result->isExpId() ? (old_result)->getExpId() : exp_focus;
        exp = Find_Experiment_Object (ExperimentID);
        if ((exp == NULL) ||
            (exp->FW() == NULL)) {
         // No experiment was specified, so we can't find a useful view to generate.
          Mark_Cmd_With_Soft_Error(cmd,
                                   "No valid experiment was specified for -c "
                                      + N.str() + ".");
          continue;
        }
      }

     // Collect information needed for 'savedView' support.
      if (primary_ExperimentID == 0) primary_ExperimentID = ExperimentID;
      referenced_experiments.insert(ExperimentID);

     // If there is no modifier list, get one from the custom view definition.
      if (new_mod_list->empty()) {
        (*new_mod_list) = (*old_mod_list);
      }

     // If there is no metric list, get one from the custom view definition.
      if (new_met_list->empty()) {
        (*new_met_list) = (*old_met_list);
      }

     // If there is no target list, get one from the custom view definition.
      if (new_target_list->empty()) {
        (*new_target_list) = (*old_target_list);
      }

     // If there is no view already defined, pick up the <viewType> from the custom definition.
      if (new_view_list->empty()) {
        (*new_view_list) = (*old_view_list);
      }

     // If there is no time interval specified, get one from the custom definition.
      if (new_interval_list->empty()) {
        (*new_interval_list) = (*old_interval_list);
        if (old_result->isIntervalAttribute()) {
          new_result->setIntervalAttribute ((char *)(old_result->getIntervalAttribute()->c_str()));
        }
      }

     // If there still is no view list, get one from the view definition.
      if (new_view_list->empty()) {
       // The user has not selected a view.
       // Look for a view that would be meaningful.
        Assert ((exp != NULL) && (exp->FW() != NULL));
        CollectorGroup cgrp = exp->FW()->getCollectors();
        if (cgrp.begin() == cgrp.end()) {
         // No collector was used.
          Mark_Cmd_With_Soft_Error(cmd,
                                   "No performance measurements were made for the experiment of -c "
                                      + N.str() + ".");
          continue;
        } else {
          bool view_found = false;
          CollectorGroup::iterator cgi;
          for (cgi = cgrp.begin(); cgi != cgrp.end(); cgi++) {
           // Generate a view for every collector.
            Collector c = *cgi;
            Metadata m = c.getMetadata();
            std::string collector_name = m.getUniqueId();
            new_view_list->push_back(collector_name);
          }
        }
      }

     // If there still are no views, use generic view as default.
      if (new_view_list->empty()) {
        new_view_list->push_back("stats");
      }

     // Generate all the views in the list.
      std::vector<std::string>::iterator si;
      for (si = new_view_list->begin(); si != new_view_list->end(); si++) {
       // Determine the availability of the view.
        std::string viewname = *si;
        ViewType *vt = Find_View (viewname);
        if (vt == NULL) {
          Mark_Cmd_With_Soft_Error(cmd, "The requested view '" + viewname + "' on -c "
                                          + N.str() + " is unavailable.");
          return false;
        }

       // Every compare set must have a unique ParseResult object.
        if (si != new_view_list->begin()) {
          new_result = new ParseResult((*new_result));
        }

       // Define new compare sets for each view.
        selectionTarget S;
        S.pResult = new_result;
        S.base_tgrp = cvp->cvTgrp();
        S.headerPrefix = std::string("-c ") + N.str() + " ";
        // S.headerPrefix = CustomViewInfo  (cvp) + " ";
        S.Exp = exp;
        S.viewName = viewname;
        Quick_Compare_Set.push_back (S);
      }

    }

  }

 // Look for a saved view.
  savedViewInfo *svi = NULL;

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_cView, before if OPENSS_SAVE_VIEWS_FOR_REUSE=" << OPENSS_SAVE_VIEWS_FOR_REUSE << std::endl;
#endif

  if ( OPENSS_SAVE_VIEWS_FOR_REUSE ) {
   // Find the experiment to attached the saved file to.
    std::vector<ParseRange> *expidlist = primary_result->getExpIdList();
    if ( expidlist->empty() ) {
     // Add the first experiment to the command so the
     // saved view file is associated with that experiment.
      primary_result->pushExpIdPoint(primary_ExperimentID);
    }

   // If there are multiple EXPIDs involved, create an extra ID tag for any saved view file.
    std::string new_x_str;

#if DEBUG_REUSEVIEWS
    std::cerr << "In SS_cView, before if referenced_experiments.size()=" << referenced_experiments.size() << std::endl;
#endif

    if (referenced_experiments.size() > 1) {
      for (std::set<EXPID>::iterator rx=referenced_experiments.begin();
           rx != referenced_experiments.end(); rx++) {     
        char S[40];
        sprintf( &S[0], "%lld", *rx);
        if (new_x_str.empty()) {
          new_x_str = "-x " + std::string(S);
        } else {
          new_x_str = new_x_str + ", " + std::string(S);
        }
      }
    }

#if DEBUG_REUSEVIEWS
    std::cerr << "In SS_cView, before calling Find_SavedView cmd=" << cmd << " new_x_str=" << new_x_str << std::endl;
#endif

    if ( Find_SavedView( cmd, new_x_str ) ) {

#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_cView, reuse view from file:" << cmd->SaveResultFile().c_str() << std::endl;
#endif

      if (!cmd->SaveResult()) {
       // Previously generated output file found with requested view.

       // Need to delete the copies made of the overriding Parse components.
        int64_t numQuickSets = Quick_Compare_Set.size();
        for (int64_t i = 0; i < numQuickSets; i++) {
          delete Quick_Compare_Set[i].pResult;
        }

        cmd->set_Status(CMD_COMPLETE);
        return true;
      }

     // An existing saved view is not available but provision has been made to create a new one.
      svi = cmd->SaveResultViewInfo();
      if (svi != NULL) {
       // Set StartTime to measure how long it takes to generate the view.
        svi->setStartTime();
      }
    }
  }
#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_cView, before calling Generate_CustomView" << std::endl;
#endif

  bool success = Generate_CustomView (cmd, Quick_Compare_Set);

#if DEBUG_REUSEVIEWS
  std::cerr << "In SS_cView, after calling Generate_CustomView" << std::endl;
#endif

 // Set EndTIme if saving info after generation.
  svi = cmd->SaveResultViewInfo();
  if (svi != NULL) {
    if (success) {
      svi->setEndTime();
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_cView, calling writeHeader and saving the Ostream" << std::endl;
#endif
      std::ostream *tof = svi->writeHeader ();
      cmd->setSaveResultOstream( tof );
    } else {
#if DEBUG_REUSEVIEWS
      std::cerr << "In SS_cView-1, setting DONOTSAVE" << std::endl;
#endif
      svi->setDoNotSave();
    }
  }

 // Need to delete the copies made of the overriding Parse components.
  int64_t numQuickSets = Quick_Compare_Set.size();
  for (int64_t i = 0; i < numQuickSets; i++) {
    delete Quick_Compare_Set[i].pResult;
  }

  cmd->set_Status(CMD_COMPLETE);
  return success;
}

template <typename TOBJECT, typename TI>
void Do_Analysis (
          TI *dummyType,
          Collector& collector,
          std::string& metric,
          std::vector<std::pair<Time,Time> >& intervals,
          ThreadGroup& tgrp,
          std::set<TOBJECT>& objects,
          std::set<Framework::ThreadGroup>& clusters) {

  SmartPtr<std::map<TOBJECT, std::map<Thread, TI> > > individual;
  for (std::vector<std::pair<Time,Time> >::iterator
               iv = intervals.begin(); iv != intervals.end(); iv++) {
    Queries::GetMetricValues(collector, metric,
                             TimeInterval(iv->first, iv->second),
                             tgrp, objects, individual);
  }
  clusters = Queries::ClusterAnalysis::ApplySimple(individual);
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

  std::vector<std::pair<Time,Time> > intervals;
  Parse_Interval_Specification (cmd, exp, intervals);

  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

  if( m.isType(typeid(unsigned int)) ) {
    uint *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else if( m.isType(typeid(uint64_t)) ) {
    uint64_t *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else if( m.isType(typeid(int)) ) {
    int *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else if( m.isType(typeid(float)) ) {
    float *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else if( m.isType(typeid(double)) ) {
    double *V;
    Do_Analysis (V, collector, metric, intervals, tgrp, objects, clusters);
  } else {
    std::string S("(Cluster Analysis can not be performed on metric '");
    S = S +  metric + "' of type '" + m.getType() + "'.)";
    Mark_Cmd_With_Soft_Error(cmd, S);
    return false;
  }
  if (clusters.size() == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for cluster analysis.)");
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
    Mark_Cmd_With_Soft_Error(cmd, "No valid experiment was specified for cluster analysis.");
    return false;
  }

 // Pick up components of the parse object.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<std::string> *p_slist = p_result->getViewList();
  std::vector<ParseRange> *met_list = p_result->getexpMetricList();
  std::vector<ParseTarget> *p_tlist = p_result->getTargetList();

 // Get a list of the unique threads used in the specified experiment.
  Framework::Experiment *experiment = exp->FW();
  ThreadGroup tgrp = experiment->getThreads();

 // Do we need to select a subset of these threads?
  bool threadgroupSubset = (p_tlist->begin() != p_tlist->end());
  if (threadgroupSubset) {
    Filter_ThreadGroup (cmd->P_Result(), tgrp);
  }

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

 // Be sure that there is exactly one viewType specified.
  if (p_slist->empty()) {
   // No view was specified, so we can't do any resonable work.
    Mark_Cmd_With_Soft_Error(cmd, "No valid view name was specified.");
    return false;
  } else if (p_slist->size() != 1) {
   // Too many views are specified, we don't handle this yet.
    Mark_Cmd_With_Soft_Error(cmd, "Too many view names were specified.");
    return false;
  }

 // Make sure the view name is valid.
  std::string viewname = *(p_slist->begin());
  ViewType *vt = Find_View (viewname);
  if (vt == NULL) {
    std::string S("The requested view, '");
    S = S + viewname + "', is unavailable.";
    Mark_Cmd_With_Soft_Error(cmd,S);
    return false;
  }
  viewname = vt->Unique_Name();

 // Determine the collector and metric.
  std::string C_Name = viewname;
  std::string M_Name;
  if (met_list->empty()) {
   // Use the first metric in the view definition.
    C_Name = viewname;
    M_Name = vt->Metrics()[0];
  } else if (!met_list->empty()) {
   // Use the first item on the '-m' list as a metric.
    std::vector<ParseRange>::iterator mi = met_list->begin();
    parse_range_t *m_range = (*mi).getRange();
    if (m_range->is_range) {
      C_Name = m_range->start_range.name;
      M_Name = m_range->end_range.name;
      if (C_Name != viewname) {
        std::string S("The specified collector, '" + C_Name +
                      "', can not be displayed as part of a '" + viewname + "' view.");
        Mark_Cmd_With_Soft_Error(cmd,S);
        return false;
      }
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

 // Set up for Cluster Analysis.
  View_Form_Category vfc = Determine_Form_Category(cmd);
  Collector C = Get_Collector (experiment, C_Name);

 // Look for a saved view.
  std::string extended_cmd = C_Name + " -m " + M_Name;
  savedViewInfo *svi = NULL;

#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters in SS_Compare.cxx, checking the experiment list, OPENSS_SAVE_VIEWS_FOR_REUSE=" 
              << OPENSS_SAVE_VIEWS_FOR_REUSE << " extended_cmd=" << extended_cmd << std::endl;
#endif

  if ( OPENSS_SAVE_VIEWS_FOR_REUSE ) {

   // Find the experiment to attached the saved file to.
    std::vector<ParseRange> *expidlist = cmd->P_Result()->getExpIdList();
    if ( expidlist->empty() ) {
     // Add the experiment to the command so the
     // saved view file is associated with that experiment.
      cmd->P_Result()->pushExpIdPoint(ExperimentID);
    }

#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters in SS_Compare.cxx, calling Find_SavedView, extended_cmd=" << extended_cmd << std::endl;
#endif

    if ( Find_SavedView( cmd, extended_cmd ) ) {

#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters in SS_Compare.cxx, reuse view from file: " << cmd->SaveResultFile().c_str() 
              << " extended_cmd=" << extended_cmd << std::endl;
#endif

      if (!cmd->SaveResult()) {
       // Previously generated output file found with requested view.
        cmd->set_Status(CMD_COMPLETE);

#if DEBUG_REUSEVIEWS
        std::cerr << "Early exit cvClusters in SS_Compare.cxx, reuse view from file: " << cmd->SaveResultFile().c_str() 
                  << " extended_cmd=" << extended_cmd << std::endl;
#endif
        return true;
      }

     // An existing saved view is not available but provision has been made to create a new one.

#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters in SS_Compare.cxx, create new view from file: " << cmd->SaveResultFile().c_str() << std::endl;
#endif

      svi = cmd->SaveResultViewInfo();
      if (svi != NULL) {
       // Set StartTIme to measure how long it takes to generate the view.
        svi->setStartTime();
      }
    }
  }

 // Perform Cluster Analysis.
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
  std::set<std::string> file_list;

 // Find all the functions that the user listed.
  for (std::vector<ParseTarget>::iterator
        pi = p_tlist->begin(); pi != p_tlist->end(); pi++) {
    ParseTarget pt = *pi;
    std::vector<ParseRange> *f_list = pt.getFileList();
    if ((f_list != NULL) && !f_list->empty()) {
      for (std::vector<ParseRange>::iterator
             f_iter = f_list->begin(); f_iter != f_list->end(); f_iter++) {
        parse_range_t *f_range = f_iter->getRange();
        parse_val_t f_val1 = f_range->start_range;
        Assert (!f_range->is_range);
        Assert (f_val1.tag == VAL_STRING);
        file_list.insert (f_val1.name);
      }
    }
  }

 // Step 1: collect the identifying information for each thread.
  std::vector<std::vector<ThreadInfo> > TIG(clusters.size());
  int64_t ix = 0;
  for(std::set<ThreadGroup>::const_iterator
         i = clusters.begin(); i != clusters.end(); ++i, ix++) {
   // Build the informational set needed to compress the thread descriptions.
    ThreadGroup tgrp = *i;
    Build_ThreadInfo (tgrp, TIG[ix]);
  }

 // Step 2: build cViewCreate descriptions for each cluster.
  ix = 0;
  for(std::set<ThreadGroup>::const_iterator
         i = clusters.begin(); i != clusters.end(); i++, ix++) {
   // Create new ParseResult and define ExpId and viewType.
    OpenSpeedShop::cli::ParseResult *new_result = new ParseResult();
    new_result->setCommandType (CMD_C_VIEW_CREATE);
    new_result->pushExpIdPoint (ExperimentID);

   // Copy viewType list from original ParseResult to new ParseResult.
    (*new_result->getViewList()) = (*p_slist);

   // Copy ModifierList from original ParseResult to new ParseResult.
    (*new_result->getModifierList()) = (*p_result->getModifierList());

   // Copy MetricList from original ParseResult to new ParseResult.
    (*new_result->getexpMetricList()) = (*met_list);

   // Copy time interval list from original ParseResult to new ParseResult.
    std::vector<ParseInterval> *interval_list = p_result->getParseIntervalList();
    if (!interval_list->empty()) {
      (*new_result->getParseIntervalList()) = (*interval_list);
      if (p_result->isIntervalAttribute()) {
        new_result->setIntervalAttribute ((char *)(p_result->getIntervalAttribute()->c_str()));
      }
    }

   // Each thread in the cluster is a distinct ParseTarget specification.
    for(ThreadGroup::const_iterator
             j = i->begin(); j != i->end(); ++j) {
     // Pick up the prevously defined ParseTarget information and copy it.
      ParseTarget *new_pt = new_result->currentTarget();

     // Copy the sum of all functions from the original set of target specs.
      for (std::set<std::string>::iterator
              fi = file_list.begin(); fi != file_list.end(); fi++) {
        new_pt->pushFilePoint ((char *)((*fi).c_str()));
      }

    }

   // Try to merge consecutive fields into range specifications.
    std::vector<ThreadRangeInfo> Out;
    Compress_ThreadInfo (TIG[ix], Out);

   // Create the target specification for the items in the cluster.

    for (std::vector<ThreadRangeInfo>::iterator tri = Out.begin(); tri != Out.end(); tri++) {
     // Pick up the prevously defined ParseTarget information and copy it.
      ParseTarget *new_pt = new_result->currentTarget();

     // Get host, pid, thread and rank info from the compressed spec.
      std::string hid = tri->hostName;
      new_pt->pushHostPoint ((char *)(tri->hostName.c_str()));

      for (pid_t pi = 0; pi < tri->processId.size(); pi++) {
        if (tri->processId[pi].first == tri->processId[pi].second) {
          new_pt->pushPidPoint (tri->processId[pi].first);
        } else {
          new_pt->pushPidRange (tri->processId[pi].first, tri->processId[pi].second);
        }
      }

      for (int64_t ti = 0; ti < tri->threadId.size(); ti++) {
        if (tri->threadId[ti].first == tri->threadId[ti].second) {
          new_pt->pushThreadPoint (tri->threadId[ti].first);
        } else {
          new_pt->pushThreadRange (tri->threadId[ti].first, tri->threadId[ti].second);
        }
      }

      for (int64_t ri = 0; ri < tri->rankId.size(); ri++) {
        if (tri->rankId[ri].first == tri->rankId[ri].second) {
          new_pt->pushRankPoint (tri->rankId[ri].first);
        } else {
          new_pt->pushRankRange (tri->rankId[ri].first, tri->rankId[ri].second);
        }
      }

     // New ParseTarget is complete, commit it and create a new currentTarget..
      new_result->pushParseTarget();

    }

#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters in SS_Compare.cxx, constructing a new CustomView" << std::endl;
#endif

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

 // Set EndTIme if saving info after generation.
  svi = cmd->SaveResultViewInfo();
  if (svi != NULL) {
    if (Analysis_Okay) {
      svi->setEndTime();
      std::ostream *tof = svi->writeHeader ();
#if DEBUG_REUSEVIEWS
    std::cerr << "In cvClusters-2 in SS_Compare.cxx, calling writeHeader and saving the Ostream, tof=" << tof << std::endl;
#endif
      cmd->setSaveResultOstream( tof );
    } else {
#if DEBUG_REUSEVIEWS
      std::cerr << "In cvClusters-2-1, setting DONOTSAVE" << std::endl;
#endif
      svi->setDoNotSave();
    }
  }

  cmd->set_Status(CMD_COMPLETE);
  return Analysis_Okay;
}
