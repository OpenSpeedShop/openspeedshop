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
#include "MPICollector.hxx"
#include "MPIDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage mpi collector data.
#define start_temp 2
#define stop_temp 3
#define min_temp 4
#define max_temp 5
#define cnt_temp 6
#define ssq_temp 7


// mpi view

#define def_MPI_values \
            Time start = Time::TheEnd(); \
            Time end = Time::TheBeginning(); \
            int64_t cnt = 0; \
            double sum = 0.0; \
            double vmax = 0.0; \
            double vmin = LONG_MAX; \
            double sum_squares = 0.0;

#define get_MPI_values(vi) \
              double v = (*vi).dm_time; \
              start = min(start,(*vi).dm_interval.getBegin()); \
              end = max(end,(*vi).dm_interval.getEnd()); \
              cnt ++; \
              vmin = min(vmin,v); \
              vmax = max(vmax,v); \
              sum += v; \
              sum_squares += v * v;

#define set_MPI_values  \
              if (num_temps > VMulti_sort_temp) (*vcs)[VMulti_sort_temp] = NULL; \
              if (num_temps > start_temp) {  \
                double x = (start-base_time) / 1000000000.0; \
                (*vcs)[start_temp] = CRPTR (x); \
              } \
              if (num_temps > stop_temp) { \
                double x = (end-base_time) / 1000000000.0; \
                (*vcs)[stop_temp] = CRPTR (x); \
              } \
              if (num_temps > VMulti_time_temp) (*vcs)[VMulti_time_temp] = CRPTR (vmin); \
              if (num_temps > min_temp) (*vcs)[min_temp] = CRPTR (vmax); \
              if (num_temps > max_temp) (*vcs)[max_temp] = CRPTR (sum); \
              if (num_temps > cnt_temp) (*vcs)[cnt_temp] = CRPTR (cnt); \
              if (num_temps > ssq_temp) (*vcs)[ssq_temp] = CRPTR (sum_squares);

static void Determine_Objects (
               CommandObject *cmd,
               ExperimentObject *exp,
               ThreadGroup& tgrp,
               std::set<Function>& objects) {
 // Get the list of desired functions.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  if (p_tlist->begin() == p_tlist->end()) {
   // There is no <target> list for filtering.
   // Get all the mpi functions for all the threads.
    objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
  } else {
   // There is a list.  Is there a "-f" specifier?
    vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

    if ((f_list == NULL) || (f_list->empty()) ||
        Look_For_KeyWord(cmd, "ButterFly")) {
     // There is no Function filtering requested or a ButerFly views is requested.
     // Get all the functions in the already selected thread groups.
     // Function filtering will be done later for ButerFly views.
      for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

       // Check for asnychonous abort command
       // Check for asnychonous abort command
        if (cmd->Status() == CMD_ABORTED) {
          return;
        }

        Thread thread = *ti;
        std::set<Function> threadObjects;
        OpenSpeedShop::Queries::GetSourceObjects(thread, threadObjects);
        objects.insert(threadObjects.begin(), threadObjects.end());
      }
    } else {
     // There is some sort of file filter specified.
     // Determine the names of desired functions and get Function objects for them.
     // Thread filtering will be done in GetMetricInThreadGroup.
        vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
        for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
          OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
          OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
          Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
          std::string F_Name = pval1.name;
          std::set<Function> new_objects = exp->FW()->getFunctionsByNamePattern (F_Name);
          if (!new_objects.empty()) {
            objects.insert (new_objects.begin(), new_objects.end());
          }
        }
    }
  }
}

static bool MPI_Trace_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

 // Acquire base set of metric values.
  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent.getTimeInterval().getBegin();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
          std::map<Framework::StackTrace, std::vector<MPIDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            CommandResult *base_CSE = NULL;
            Framework::StackTrace st = (*si).first;
            std::vector<MPIDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
             // Use macro to alocate temporaries
              def_MPI_values
             // Use macro to assign to temporaries
              get_MPI_values(vi)

             // Use macro to assign temporaries to the result array
              SmartPtr<std::vector<CommandResult *> > vcs
                       = Framework::SmartPtr<std::vector<CommandResult *> >(
                                   new std::vector<CommandResult *>(num_temps)
                                   );
              set_MPI_values

              CommandResult *CSE;
              if (base_CSE == NULL) {
                SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, false, st);
                base_CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
                CSE = base_CSE;
              } else {
                CSE = Dup_CommandResult (base_CSE);
              }
              c_items.push_back(std::make_pair(CSE, vcs));
            }
          }
        }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
  return Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Trace, c_items, view_output);
}

static bool MPI_Function_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent .getTimeInterval().getBegin();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Combine all the items for each function.
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
         // Use macro to allocate imtermediate temporaries
          def_MPI_values
          std::map<Framework::StackTrace, std::vector<MPIDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            std::vector<MPIDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
             // Use macro to accumulate all the separate samples
              get_MPI_values(vi)
            }
          }

         // Use macro to construct result array
          SmartPtr<std::vector<CommandResult *> > vcs
                   = Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>(num_temps)
                               );
          set_MPI_values

         // Construct callstack for last entry in the stack trace.
          Function F = (*fi).first;
          std::map<Framework::StackTrace,
                   std::vector<MPIDetail> >::iterator first_si = 
                                      (*fi).second.begin();
          Framework::StackTrace st = (*first_si).first;
          std::set<Statement> T = st.getStatementsAt(st.size()-1);

          SmartPtr<std::vector<CommandResult *> > call_stack =
                   Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>()
                               );
          call_stack->push_back(new CommandResult_Function (F, T));
          CommandResult *CSE = new CommandResult_CallStackEntry (call_stack);
          c_items.push_back(std::make_pair(CSE, vcs));
        }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
  return Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Function, c_items, view_output);
}

static bool MPI_CallStack_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent .getTimeInterval().getBegin();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Construct complete call stack
        std::map<Function,
                 std::map<Framework::StackTrace,
                          std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
         // Foreach MPI function ...
          std::map<Framework::StackTrace,
                   std::vector<MPIDetail> >::iterator sti;
          for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
           // Use macro to allocate temporary array
            def_MPI_values
            int64_t len = (*sti).second.size();
            for (int64_t i = 0; i < len; i++) {
             // Use macro to combine all the values.
              get_MPI_values(&(*sti).second[i])
            }

           // Use macro to set values into return structure.
            SmartPtr<std::vector<CommandResult *> > vcs
                     = Framework::SmartPtr<std::vector<CommandResult *> >(
                                 new std::vector<CommandResult *>(num_temps)
                                 );
            set_MPI_values

           // Foreach call stack ...
            Framework::StackTrace st = (*sti).first;

           // Construct result entry
            SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st);
            CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
            c_items.push_back(std::make_pair(CSE, vcs));
          }
        }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
  return Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack,c_items, view_output);
}

static std::string allowed_mpi_V_options[] = {
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Trace",
  "ButterFly",
  "CallTree",
  "CallTrees",
  "TraceBack",
  "TraceBacks",
  "FullStack",
  "FullStacks",
  "DontExpand",
  "Summary",
  ""
};

static void define_mpi_columns (
            CommandObject *cmd,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Total time is always placed in first column.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, stop_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, cnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, ssq_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_Summary = (Look_For_KeyWord(cmd, "Summary") & !Look_For_KeyWord(cmd, "ButterFly"));

  if (Generate_Summary) {
   // Total time is always displayed - also add display of the summary time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    bool time_metric_selected = false;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      bool column_is_DateTime = false;
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
        if (!time_metric_selected &&
            !strcasecmp(M_Name.c_str(), "time") ||
            !strcasecmp(M_Name.c_str(), "times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, VMulti_time_temp));
          HV.push_back("Exclusive Time");
          last_column++;
        } else if (!time_metric_selected &&
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, VMulti_time_temp));
          HV.push_back("Inclusive Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "min")) {
         // display min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, min_temp));
          HV.push_back("Min Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // display max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, max_temp));
          HV.push_back("Max Time");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, cnt_temp));
          HV.push_back("Number of Calls");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column, VMulti_time_temp, cnt_temp));
          HV.push_back("Average Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, VMulti_time_temp));
          HV.push_back("% of Total");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum, sum of squares and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column,
                                            VMulti_time_temp, ssq_temp, cnt_temp));
          HV.push_back("Standard Deviation");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "start_time")) {
          if (vfc == VFC_Trace) {
           // display start time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
            HV.push_back("Start Time");
            last_column++;
            column_is_DateTime = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "stop_time")) {
          if (vfc == VFC_Trace) {
           // display stop time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
            HV.push_back("Stop Time");
            last_column++;
            column_is_DateTime = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m stop_time' only supported for '-v Trace' option.");
          }
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
      if (last_column == 1) {
        IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, (int64_t)(column_is_DateTime) ? 1 : 0));
      }
    }
  } else {
   // If nothing is requested ...
    if (vfc == VFC_Trace) {
      // Insert start and end times into report.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
      HV.push_back("Start Time");
      IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, 1)); // final report in ascending time order
      last_column++;
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
      HV.push_back("Stop Time");
      last_column++;
    }
   // Always display elapsed time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, VMulti_time_temp));
    HV.push_back("Exclusive Time");
  }
}

static bool mpi_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
    Assert (CV.begin() != CV.end());
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
   // Clean the other vectors
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());
    HV.erase(HV.begin(), HV.end());

    CollectorGroup cgrp = exp->FW()->getCollectors();
    Collector C = *CV.begin();
    if (cgrp.find(C) == std::set<Collector>::iterator(cgrp.end())) {
      std::string C_Name = C.getMetadata().getUniqueId();
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
/* TEST
    std::string M_Name("exclusive_times");
TEST */
    std::string M_Name("exclusive_details");
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_mpi_V_options);
    define_mpi_columns (cmd, IV, HV, vfc);

    return true;
}


static std::string VIEW_mpi_brief = "Mpi Report";
static std::string VIEW_mpi_short = "Report the time spent in each mpi function.";
static std::string VIEW_mpi_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'mpi' to indicate the maximum number of items in the report."
                                      " When the '-v Trace' option is selected, the selected items are"
                                      " the ones that use the most time.  In all other cases"
                                      " the selection will be based on the values displayed in"
                                      " left most column of the report."
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option.  Except for the '-v Trace' option, the report will"
                                      " be sorted in descending order of the value in the left most column"
                                      " displayed on a line. [See '-m' option for controlling this field.]"
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option."
                                      "\n\t'-v Functions' will produce a summary report that"
                                      " will be sorted in descending order of the value in the left most"
                                      " column (see the '-m' option).  This is the default display."
                                      "\n\t'-v Trace' will produce a report of each individual  call to an mpi"
                                      " function."
                                      " It will be sorted in ascending order of the starting time for the event."
                                      " The information available for display from an 'mpi' experiment is very"
                                      " limited when compared to what is available from an 'mpit' experiment."
                                      "\n\t'-v CallTrees' will produce a calling stack report that is presented"
                                      " in calling tree order - from the start of the program to the measured"
                                      " program."
                                      "\n\t'-v TraceBacks' will produce a calling stack report that is presented"
                                      " in traceback order - from the measured function to the start of the"
                                      " program."
                                      "\n\tThe addition of 'FullStack' with either 'CallTrees' of 'TraceBacks'"
                                      " will cause the report to include the full call stack for each measured"
                                      " function.  Redundant portions of a call stack are suppressed by default."
                                      "\n\tThe addition of 'Summary' to the '-v' option list along with 'Functions',"
                                      " 'CallTrees' or 'TraceBacks' will result in an additional line of output at"
                                      " the end of the report that summarizes the information in each column."
                                      "\n\t'-v ButterFly' along with a '-f <function_list>' will produce a report"
                                      " that summarizes the calls to a function and the calls from the function."
                                      " The calling functions will be listed before the named function and the"
                                      " called functions afterwards, by default, although the addition of"
                                      " 'TraceBacks' to the '-v' specifier will reverse this ordering."
                                      "\n\nThe information included in the report can be controlled with the"
                                      " '-m' option.  More than one item can be selected but only the items"
                                      " listed after the option will be printed and they will be printed in"
                                      " the order that they are listed."
                                      " If no '-m' option is specified, the default is equivalent to"
                                      " '-m exclusive times'."
                                      " The full set of available options is: 'exclusive_times',"
                                      " 'min', 'max', 'average', 'count', 'percent', and 'stddev'."
                                      " Each option reports information about the set of mpi calls that is"
                                      " reported for the function on that particular line in the report."
                                      " \n\t'-m exclusive_times' reports the wall clock time used in the function."
                                      " \n\t'-m min' reports the minimum time spent in the function."
                                      " \n\t'-m max' reports the maximum time spent in the function."
                                      " \n\t'-m average' reports the average time spent in the function."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of mpi time the function represents."
                                      " \n\t'-m stddev' reports the standard deviation of the average mpi time"
                                      " that the function represents.";
static std::string VIEW_mpi_example = "\texpView mpi\n"
                                      "\texpView -v CallTrees,FullStack mpi10 -m min,max,count\n";
static std::string VIEW_mpi_metrics[] =
  { "exclusive_details",
    "exclusive_times",
    "inclusive_details",
    "inclusive_times",
    ""
  };
static std::string VIEW_mpi_collectors[] =
  { "mpi",
    ""
  };
class mpi_view : public ViewType {

 public: 
  mpi_view() : ViewType ("mpi",
                         VIEW_mpi_brief,
                         VIEW_mpi_short,
                         VIEW_mpi_long,
                         VIEW_mpi_example,
                        &VIEW_mpi_metrics[0],
                        &VIEW_mpi_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "mpi"));  // Define the collector
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (mpi_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      switch (vfc) {
       case VFC_Trace:
        return MPI_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_CallStack:
        return MPI_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_Function:
        return MPI_Function_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
      }
    }
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mpi_view_LTX_ViewFactory () {
  Define_New_View (new mpi_view());
}
