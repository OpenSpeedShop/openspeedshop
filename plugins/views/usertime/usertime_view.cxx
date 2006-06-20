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
#define extime_temp VMulti_free_temp
#define excnt_temp  VMulti_free_temp+1
#define intime_temp VMulti_free_temp+2
#define incnt_temp  VMulti_free_temp+3
#define tmean_temp  VMulti_free_temp+4
#define tmin_temp  VMulti_free_temp+5
#define tmax_temp  VMulti_free_temp+6


// usertime view

#define def_UserTime_values          \
            double ex_time = 0.0;    \
            uint64_t ex_cnt = 0;     \
            double in_time = 0.0;    \
            uint64_t in_cnt = 0;

#define get_inclusive_values(primary, num_calls)        \
                in_time += primary.dm_time / num_calls; \
                in_cnt +=  primary.dm_count;

#define get_exclusive_values(secondary, num_calls)        \
                ex_time += secondary.dm_time / num_calls; \
                ex_cnt +=  secondary.dm_count;

#define set_UserTime_values(value_array, sort_extime)                                       \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;       \
              if (num_temps > VMulti_time_temp) value_array[VMulti_time_temp]               \
                                                 = CRPTR (sort_extime ? ex_time : in_time); \
              if (num_temps > VMulti_time_temp) value_array[extime_temp] = CRPTR (ex_time); \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (ex_cnt);         \
              if (num_temps > intime_temp) value_array[intime_temp] = CRPTR (in_time);      \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (in_cnt);

#define set_ExtraMetric_values(value_array, ExtraValues, index)                                      \
              if (num_temps > tmean_temp) {                                                          \
                if (ExtraValues[ViewReduction_mean]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_mean]->end()) {   \
                  value_array[tmean_temp]                                                            \
                       = Dup_CommandResult(ExtraValues[ViewReduction_mean]->find(index)->second);    \
                } else {                                                                             \
                  value_array[tmean_temp] = CRPTR ((double)0.0);                                     \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmin_temp) {                                                           \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
                  value_array[tmin_temp]                                                             \
                       = Dup_CommandResult(ExtraValues[ViewReduction_min]->find(index)->second);     \
                } else {                                                                             \
                  value_array[tmin_temp] = CRPTR ((double)0.0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmax_temp) {                                                           \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
                  value_array[tmax_temp]                                                             \
                       = Dup_CommandResult(ExtraValues[ViewReduction_max]->find(index)->second);     \
                } else {                                                                             \
                  value_array[tmax_temp] = CRPTR ((double)0.0);                                      \
                }                                                                                    \
              }

#define def_Detail_values def_UserTime_values
#define set_Detail_values set_UserTime_values
#define Determine_Objects Get_Filtered_Objects
#include "SS_View_detail.txx"

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

static bool define_usertime_columns (
            CommandObject *cmd,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Number of columns of information displayed.
  int64_t totalIndex  = 0;  // Number of totals needed to perform % calculations.
  int64_t first_time_temp = 0;

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, intime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, intime_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool Generate_Summary = Look_For_KeyWord(cmd, "Summary");

  if (Generate_Summary) {
    if (Generate_ButterFly) {
      Generate_Summary = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summary' is not supported with '-v ButterFly'.");
    } else {
     // Total time is always displayed - also add display of the summary time.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
    }
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "usertime")) {
         // We only know what to do with the usertime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'usertime' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          return false;
        }
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
        if (Generate_ButterFly &&
            (!strcasecmp(M_Name.c_str(), "time") ||
             !strcasecmp(M_Name.c_str(), "times")) ) {
         // display total inclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
          HV.push_back( Find_Metadata ( CV[0], "inclusive_time" ).getDescription() );
        } else if (Generate_ButterFly &&
                   (!strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls"))) {
           // display total inclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Number of Counts");
        } else if (Generate_ButterFly &&
                   !strcasecmp(M_Name.c_str(), "percent")) {
          IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++,
                       (first_time_temp == 0) ? intime_temp : first_time_temp, totalIndex++));
          HV.push_back("% of Total Inclusive CPU Time");
        } else if (!strcasecmp(M_Name.c_str(), "time") ||
                   !strcasecmp(M_Name.c_str(), "times") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
          HV.push_back( Find_Metadata ( CV[0], "exclusive_time" ).getDescription() );
        } else if (!strcasecmp(M_Name.c_str(), "inclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
          HV.push_back( Find_Metadata ( CV[0], "inclusive_time" ).getDescription() );
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          HV.push_back("Number of Exclusive Counts");
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts")) {
         // display total inclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Number of Inclusive Counts");
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcmp(M_Name.c_str(), "%")           ||
                   !strcasecmp(M_Name.c_str(), "%time")   ||
                   !strcasecmp(M_Name.c_str(), "%times") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_details") ) {
         // percent is calculate from 2 temps: time for this row and total exclusive time.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total time.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp, totalIndex++));
          HV.push_back("% of Total Exclusive Time");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_details")) {
         // percent is calculate from 2 temps: time for this row and total inclusive time.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total time.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp, totalIndex++));
          HV.push_back("% of Total Exclusive Time");
        } else if (!strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%call") ||
                   !strcasecmp(M_Name.c_str(), "%calls") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m exclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
          HV.push_back("% of Total Exclusive Counts");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_counts") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m inclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, incnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp, totalIndex++));
          HV.push_back("% of Total Inclusive Counts");
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                   !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
            std::string H = Find_Metadata ( CV[1], MV[1] ).getDescription();
            HV.push_back(std::string("Average ") + H + " Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) { 
         // Find the By-Thread Min.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
            std::string H = Find_Metadata ( CV[1], MV[1] ).getDescription();
            HV.push_back(std::string("Min ") + H + " Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
            std::string H = Find_Metadata ( CV[1], MV[1] ).getDescription();
            HV.push_back(std::string("Max ") + H + " Across Threads");
          }
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }

    }
  } else if (Generate_ButterFly) {
   // Default ButterFly view.
   // Column[0] is inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
    HV.push_back( Find_Metadata( CV[0], "inclusive_time" ).getDescription() );

  // Column[1] in % of inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp, totalIndex++));
    HV.push_back("% of Total Exclusive CPU Time");

  } else {
   // If nothing is requested ...
   // Column[0] is exclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
    Metadata m = Find_Metadata ( CV[0], "exclusive_time");
    std::string H = m.getDescription();
    HV.push_back(m.getDescription());

   // Column[1] is inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
    HV.push_back( Find_Metadata( CV[0], "inclusive_time" ).getDescription() );

   // Column[2] is percent, calculated from 2 temps: time for this row and total inclusive time.
    if (Filter_Uses_F(cmd)) {
     // Use the metric needed for calculating total time.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
    } else {
     // Sum the extime_temp values.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
    }
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp, totalIndex++));
    HV.push_back("% of Total Exclusive CPU Time");
  }
  return (HV.size() > 0);
}

static bool usertime_definition (
                             CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {

    Validate_V_Options (cmd, allowed_usertime_V_options);

    CV.push_back (Get_Collector (exp->FW(), "usertime"));  // Define the collector
    MV.push_back ("inclusive_detail"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "usertime"));  // Define the collector
    MV.push_back ("exclusive_time"); // define the metric needed for calculating total time.

    return define_usertime_columns (cmd, CV, MV, IV, HV, vfc);
}

static std::string VIEW_usertime_brief = "UserTime Report";
static std::string VIEW_usertime_short = "Report the amount of sample time spent in a code unit.";
static std::string VIEW_usertime_long  =
                  "\nA positive integer can be added to the end of the keyword"
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
                  " If no '-m' options are specified, the default report is equivalent to"
                  " '-m inclusive_time, exclusive_time, percent'."
                  "\n\nThe information included in the report can be controlled with the"
                  " '-m' option.  More than one item can be selected but only the items"
                  " listed after the option will be printed and they will be printed in"
                  " the order that they are listed."
                  " If no '-m' option is specified, the default is equivalent to"
                  " '-m exclusive_time, inclusive_time, percent'."
                  " Each value pertains to the function, statement or linked object that is"
                  " on that row of the report.  The 'Thread...' selections pertain to the"
                  " process unit that the program was partitioned into: Pid's,"
                  " Posix threads, Mpi threads or Ranks."
                  " \n\t'-m exclusive_time' reports the wall clock time used in the code unit."
                  " \n\t'-m inclusive_time' reports the wall clock time used in the aggregate"
                  " by the unit and all the units it calls."
                  " \n\t'-m percent' reports the percent of total cpu the code unit represents."
                  " \n\t'-m count' reports the number calls into the code unit."
                  " \n\t'-m ThreadAverage' reports the average cpu time for a process."
                  " \n\t'-m ThreadMin' reports the minimum cpu time for a process."
                  " \n\t'-m ThreadMax' reports the maximum cpu time for a process."
                  "\n";
static std::string VIEW_usertime_example = "\texpView usertime\n"
                                           "\texpView -v LinkedObjects usertime\n"
                                           "\texpView -v Statements usertime20\n"
                                           "\texpView -v Functions usertime10 -m usertime::inclusive_time\n"
                                           "\texpView usertime20 -m inclusive_time, exclusive_time\n"
                                           "\texpView -v CallTrees,FullStack usertime10 -m count\n";
static std::string VIEW_usertime_metrics[] =
  { "exclusive_time",
    "exclusive_details",
    "inclusive_time",
    "inclusive_details",
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

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      UserTimeDetail *dummyDetail;
      switch (vfc) {
       case VFC_CallStack:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), dummyDetail, view_output);
        } else {
          return Detail_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), dummyDetail, view_output);
        }
       case VFC_Function:
        Function *fp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), fp, vfc, dummyDetail, view_output);
       case VFC_LinkedObject:
        LinkedObject *lp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), lp, vfc, dummyDetail, view_output);
       case VFC_Statement:
        Statement *sp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), sp, vfc, dummyDetail, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(We could not determine which format to use for the report.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void usertime_view_LTX_ViewFactory () {
  Define_New_View (new usertime_view());
}
