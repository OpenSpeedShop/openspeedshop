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
#include "UserTimeCollector.hxx"
#include "UserTimeDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage usertime collector data.
#define extime_temp 2
#define excnt_temp 3
#define intime_temp 4
#define incnt_temp 5


// usertime view

#define def_UserTime_values \
            double ex_time = 0.0; \
            uint64_t ex_cnt = 0; \
            double in_time = 0.0; \
            uint64_t in_cnt = 0;

#define get_UserTime_values1(primary) \
                in_time += primary.dm_time; \
                in_cnt +=  primary.dm_count;

#define get_UserTime_values2(secondary) \
                ex_time += secondary.dm_time; \
                ex_cnt +=  secondary.dm_count;

#define set_UserTime_values(sort_extime)  \
              if (num_temps > VMulti_sort_temp) (*vcs)[VMulti_sort_temp] = NULL; \
              if (num_temps > VMulti_time_temp) (*vcs)[VMulti_time_temp] = CRPTR (sort_extime ? ex_time : in_time); \
              if (num_temps > VMulti_time_temp) (*vcs)[extime_temp] = CRPTR (ex_time); \
              if (num_temps > excnt_temp) (*vcs)[excnt_temp] = CRPTR (ex_cnt); \
              if (num_temps > intime_temp) (*vcs)[intime_temp] = CRPTR (in_time); \
              if (num_temps > incnt_temp) (*vcs)[incnt_temp] = CRPTR (in_cnt);

static void Determine_Objects (
               CommandObject *cmd,
               ExperimentObject *exp,
               ThreadGroup& tgrp,
               std::set<Function>& objects) {
 // Get the list of desired functions.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
/*
  if ((p_tlist == NULL) || (p_tlist->empty()) ||
      Look_For_KeyWord(cmd, "ButterFly")) {
*/
  if ((p_tlist == NULL) || (p_tlist->empty())) {
   // There is no Function filtering requested or a ButerFly views is requested.
   // Get all the functions in the already selected thread groups.
   // Function filtering will be done later for ButerFly views.
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

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
   // There is a list.  Is there a "-f" specifier?
    vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

/*
    if ((f_list == NULL) || (f_list->empty()) ||
        Look_For_KeyWord(cmd, "ButterFly")) {
*/
    if ((f_list == NULL) || (f_list->empty())) {
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

static bool Determine_Metric_Ordering (std::vector<ViewInstruction *>& IV) {
 // Determine which metric is the primary.
  int64_t master_temp = 0;
  int64_t search_column = 0;

  while ((search_column == 0) &&
         (search_column < IV.size())) {
    ViewInstruction *primary_column = Find_Column_Def (IV, search_column++);
    if (primary_column == NULL) {
      break;
    }
    if (primary_column->OpCode() == VIEWINST_Display_Tmp) {
      master_temp = primary_column->TMP1();
      break;
    }
  }

  if ((master_temp != intime_temp) &&
      (master_temp != extime_temp)) {
    master_temp = intime_temp;
  }
  return (master_temp == intime_temp);
}
              

template <typename TI>
bool UserTime_Base_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output,
              View_Form_Category vfc,
              TI *item) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = Get_Collector (exp->FW(), "usertime");
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

  std::string primary_metric;
  std::string secondary_metric;
  bool primary_is_inclusive = Determine_Metric_Ordering (IV);

 // Get the list of desired functions.
  std::set<TI> objects;
/*
  Determine_Objects ( cmd, exp, tgrp, objects);
*/
  Get_Filtered_Objects (cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

   // Get raw data for exclusive_tiems metric.
    SmartPtr<std::map<TI,
                      std::map<Framework::StackTrace,
                               UserTimeDetail> > > raw_items1;
    GetMetricInThreadGroup (collector, "inclusive_detail", tgrp, objects, raw_items1);

   // Get raw data for inclusive_tiems metric.
    SmartPtr<std::map<TI,
                      std::map<Framework::StackTrace,
                               UserTimeDetail> > > raw_items2;
    GetMetricInThreadGroup (collector, "exclusive_detail", tgrp, objects, raw_items2);

   // Combine all the items for each function.
    typename std::map<TI, std::map<Framework::StackTrace, UserTimeDetail> >::iterator fi1;
    for (fi1 = raw_items1->begin(); fi1 != raw_items1->end(); fi1++) {
     // Use macro to allocate imtermediate temporaries
      def_UserTime_values

      TI F = (*fi1).first;

      std::map<Framework::StackTrace, UserTimeDetail>:: iterator si1;
      for (si1 = (*fi1).second.begin(); si1 != (*fi1).second.end(); si1++) {
       // Use macro to accumulate all the separate samples
        UserTimeDetail detail1 = (*si1).second;
        get_UserTime_values1(detail1)
      }

      typename std::map<TI, std::map<Framework::StackTrace, UserTimeDetail> >::iterator fi2;
      fi2 = raw_items2->find(F);
      if (fi2 != raw_items2->end()) {
        std::map<Framework::StackTrace, UserTimeDetail>:: iterator si2;
        for (si2 = (*fi2).second.begin(); si2 != (*fi2).second.end(); si2++) {
         // Use macro to accumulate all the separate samples
          UserTimeDetail detail2 = (*si2).second;
          get_UserTime_values2(detail2)
        }
      }

     // Use macro to construct result array
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_temps)
                           );
      set_UserTime_values(primary_is_inclusive)

     // Construct callstack for last entry in the stack trace.
      std::map<Framework::StackTrace,
               UserTimeDetail>::iterator first_si = (*fi1).second.begin();
      Framework::StackTrace st = (*first_si).first;

      SmartPtr<std::vector<CommandResult *> > call_stack =
               Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
      call_stack->push_back(CRPTR (F));
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
  return Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc, c_items, view_output);
}

static bool UserTime_CallStack_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = Get_Collector (exp->FW(), "usertime");
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));

  std::string primary_metric;
  std::string secondary_metric;
  bool primary_is_inclusive = Determine_Metric_Ordering (IV);

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);
/*
  Get_Filtered_Objects (cmd, exp, tgrp, objects);
*/

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

   // Get raw data for exclusive_tiems metric.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               UserTimeDetail> > > raw_items1;
    GetMetricInThreadGroup (collector, "inclusive_detail", tgrp, objects, raw_items1);

   // Get raw data for inclusive_tiems metric.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               UserTimeDetail> > > raw_items2;
    GetMetricInThreadGroup (collector, "exclusive_detail", tgrp, objects, raw_items2);

   // Combine all the items for each function.
    std::map<Function, std::map<Framework::StackTrace, UserTimeDetail> >::iterator fi1;
    for (fi1 = raw_items1->begin(); fi1 != raw_items1->end(); fi1++) {
     // Use macro to allocate imtermediate temporaries
      def_UserTime_values

      Function F = (*fi1).first;

      std::map<Framework::StackTrace, UserTimeDetail>:: iterator si1;
      for (si1 = (*fi1).second.begin(); si1 != (*fi1).second.end(); si1++) {
       // Use macro to accumulate all the separate samples
        UserTimeDetail detail1 = (*si1).second;
        get_UserTime_values1(detail1)
      }

      std::map<Function, std::map<Framework::StackTrace, UserTimeDetail> >::iterator fi2;
      fi2 = raw_items2->find(F);
      if (fi2 != raw_items2->end()) {
        std::map<Framework::StackTrace, UserTimeDetail>:: iterator si2;
        for (si2 = (*fi2).second.begin(); si2 != (*fi2).second.end(); si2++) {
         // Use macro to accumulate all the separate samples
          UserTimeDetail detail2 = (*si2).second;
          get_UserTime_values2(detail2)
        }
      }

     // Use macro to construct result array
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_temps)
                           );
      set_UserTime_values(primary_is_inclusive)

     // Construct result entry
      std::map<Framework::StackTrace,
               UserTimeDetail>::iterator first_si = (*fi1).second.begin();
      Framework::StackTrace st = (*first_si).first;

      SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st);
      CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
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
  return Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack, c_items, view_output);
}

static std::string allowed_usertime_V_options[] = {
  "LinkedObject",
  "LinkedObjects",
  "Dso",
  "Dsos",
  "Function",
  "Functions",
  "Statement",
  "Statements",
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

static void define_usertime_columns (
            CommandObject *cmd,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Total time is always placed in first column.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, intime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));

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
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (strcasecmp(M_Name.c_str(), "usertime")) {
         // We only know what to do with the usertime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'usertime' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          return;
        }
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
        if (!strcasecmp(M_Name.c_str(), "time") ||
            !strcasecmp(M_Name.c_str(), "times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, extime_temp));
          HV.push_back("Exclusive Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, intime_temp));
          HV.push_back("Inclusive Time");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, excnt_temp));
          HV.push_back("Number of Calls");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts")) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, incnt_temp));
          HV.push_back("Inclusive Calls");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
          IV.push_back(new ViewInstruction (VIEWINST_Define_Total, 0));  // total the first metric in CV, MV pairs
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, last_column, 0));
          HV.push_back("% of Total");
          last_column++;
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
    }
  } else {
   // If nothing is requested ...
   // Column[0] is inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, intime_temp));
    HV.push_back("Inclusive Time");
    last_column++;

   // Column[1] is exclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, extime_temp));
    HV.push_back("Exclusive Time");
    last_column++;

    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, 0));  // total the first metric in CV, MV pairs
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, last_column, 0));
    HV.push_back("% of Total");
    last_column++;
  }
}

static bool usertime_definition (
                             CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {

    Validate_V_Options (cmd, allowed_usertime_V_options);

    CV.push_back (Get_Collector (exp->FW(), "usertime"));  // Define the collector
    MV.push_back ("exclusive_time"); // define the metric needed for calculating total time.
    define_usertime_columns (cmd, IV, HV, vfc);

    return true;
}

static std::string VIEW_usertime_brief = "UserTime Report";
static std::string VIEW_usertime_short = "Report the amount of time spent in a code unit.";
static std::string VIEW_usertime_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'usertime' to indicate the maximum number of items in the report."
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option.  The report will"
                                      " be sorted in descending order of the value in the left most column"
                                      " displayed on a line. [See '-m' option for controlling this field.]"
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option."
                                      "\n\t'-v LinkedObjects' will report times by linked object."
                                      "\n\t'-v Statements' will report times by statement."
                                      "\n\t'-v Functions' will report times by function. This is the default."
                                      " will be sorted in descending order of the value in the left most"
                                      " column (see the '-m' option).  This is the default display."
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
                                      " '-m exclusive_times'."
                                      " The full set of available options is: 'exclusive_times',"
                                      " 'count' and 'percent'."
                                      " Each option reports information"
                                      " reported for the code unitn on that particular line in the report."
                                      " \n\t'-m exclusive_times' reports the wall clock time used in the code unit."
                                      " \n\t'-m inclusive_times' reports the wall clock time used in the aggregate"
                                      " by the unit and all the units it calls."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of usertime time the function represents.";
static std::string VIEW_usertime_example = "\texpView usertime\n"
                                           "\texpView -v LinkedObjects usertime\n"
                                           "\texpView -v Statements usertime20\n"
                                           "\texpView -v Functions usertime10 -m usertime::inclusive_time\n"
                                           "\texpView usertime20 -m inclusive_time, exclusive_time\n"
                                           "\texpView -v CallTrees,FullStack usertime10 -m count\n";
static std::string VIEW_usertime_metrics[] =
  { "exclusive_details",
    "exclusive_times",
    "inclusive_details",
    "inclusive_times",
    ""
  };
static std::string VIEW_usertime_collectors[] =
  { "usertime",
    ""
  };
class usertime_view : public ViewType {

 public: 
  usertime_view() : ViewType ("usertime",
                         VIEW_usertime_brief,
                         VIEW_usertime_short,
                         VIEW_usertime_long,
                         VIEW_usertime_example,
                        &VIEW_usertime_metrics[0],
                        &VIEW_usertime_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (usertime_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

/*
      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }
*/

      switch (vfc) {
       case VFC_CallStack:
        return UserTime_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_Function:
        Function *fp;
        return UserTime_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output, vfc, fp);
       case VFC_LinkedObject:
        LinkedObject *lp;
        return UserTime_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output, vfc, lp);
       case VFC_Statement:
        Statement *sp;
        return UserTime_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output, vfc, sp);
      }
    }
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void usertime_view_LTX_ViewFactory () {
  Define_New_View (new usertime_view());
}
