/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
#include "HWTimeCollector.hxx"
#include "HWTimeDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage hwctime collector data.
#define exevents_temp VMulti_free_temp
#define excnt_temp VMulti_free_temp+1
#define inevents_temp VMulti_free_temp+2
#define incnt_temp VMulti_free_temp+3
#define texeventmean_temp VMulti_free_temp+4
#define texeventmin_temp VMulti_free_temp+5
#define texeventmax_temp VMulti_free_temp+6


// hwctime view

#define def_HwcTime_values          \
            uint64_t ex_events = 0; \
            uint64_t ex_cnt = 0;    \
            uint64_t in_events = 0; \
            uint64_t in_cnt = 0;

#define get_inclusive_values(primary, num_calls)            \
                in_events += primary.dm_events / num_calls; \
                in_cnt +=  primary.dm_count;

#define get_exclusive_values(secondary, num_calls)            \
                ex_events += secondary.dm_events / num_calls; \
                ex_cnt +=  secondary.dm_count;

#define set_HwcTime_values(value_array, sort_exevents)                                            \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;             \
              if (num_temps > VMulti_time_temp) value_array[VMulti_time_temp]                     \
                                                 = CRPTR (sort_exevents ? ex_events : in_events); \
              if (num_temps > exevents_temp) value_array[exevents_temp] = CRPTR (ex_events);      \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (ex_cnt);               \
              if (num_temps > inevents_temp) value_array[inevents_temp] = CRPTR (in_events);      \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (in_cnt);

#define set_ExtraMetric_values(value_array, ExtraValues, index)                                      \
              if (num_temps > texeventmean_temp) {                                                   \
                if (ExtraValues[ViewReduction_mean]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_mean]->end()) {   \
                  value_array[texeventmean_temp]                                                     \
                       = ExtraValues[ViewReduction_mean]->find(index)->second->Copy();               \
                } else {                                                                             \
                  value_array[texeventmean_temp] = CRPTR ((uint64_t)0);                              \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > texeventmin_temp) {                                                    \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
                  value_array[texeventmin_temp]                                                      \
                       = ExtraValues[ViewReduction_min]->find(index)->second->Copy();                \
                } else {                                                                             \
                  value_array[texeventmin_temp] = CRPTR ((uint64_t)0);                               \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > texeventmax_temp) {                                                    \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
                  value_array[texeventmax_temp]                                                      \
                       = ExtraValues[ViewReduction_max]->find(index)->second->Copy();                \
                } else {                                                                             \
                  value_array[texeventmax_temp] = CRPTR ((uint64_t)0);                               \
                }                                                                                    \
              }

#define def_Detail_values def_HwcTime_values
#define set_Detail_values set_HwcTime_values
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

  if ((master_temp != inevents_temp) &&
      (master_temp != exevents_temp)) {
    master_temp = inevents_temp;
  }
  return (master_temp == inevents_temp);
}

static std::string allowed_hwctime_V_options[] = {
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

static std::string Event_Name_Header (Collector& collector, std::string metric) {
 // Get the name of the event that we were collecting.
 // Use this for the next column header in the report rather then the name of the metric.
  std::string name;
  collector.getParameterValue ("event", name);
  std::string prename;
  if (Metadata_hasName( collector, metric )) {
    Metadata m = Find_Metadata ( collector, metric );
    prename = m.getShortName();
  } else {
    prename = metric;
  }
  int64_t blank_at = prename.find(" ");
  if (blank_at > 0) {
    prename = std::string(prename, 0, blank_at);
  }
  return (prename + " " + name);
}

static bool define_hwctime_columns (
            CommandObject *cmd,
            std::vector<Collector>& CV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Number of columns of information displayed.
  int64_t totalIndex  = 0;  // Number of totals needed to perform % calculations.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, exevents_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, inevents_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, inevents_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  bool generate_nested_accounting = false;

  if (Generate_Summary) {
    if (Generate_ButterFly) {
      Generate_Summary = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summary' is not supported with '-v ButterFly'.");
    } else {
     // Add display of the summary line.
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
        if (!strcasecmp(M_Name.c_str(), "hwctime")) {
         // We only know what to do with the hwctime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'hwctime' view.");
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
            (!strcasecmp(M_Name.c_str(), "event") ||
             !strcasecmp(M_Name.c_str(), "events") ||
             !strcasecmp(M_Name.c_str(), "count") ||
             !strcasecmp(M_Name.c_str(), "counts")) ) {
         // With the '-v Butterfly' option, the default meaning of
         // 'counts' and 'events' is 'inclusive_counts' and 'inclusive_events'.
         // display inclusive events
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, inevents_temp));
          std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
          HV.push_back(H + " Counts");
        } else if (!strcasecmp(M_Name.c_str(), "event") ||
                   !strcasecmp(M_Name.c_str(), "events") ||
                   !strcasecmp(M_Name.c_str(), "count") ||
                   !strcasecmp(M_Name.c_str(), "counts") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_details") ) {
         // display the sum of exclusive events
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, exevents_temp));
          std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
          HV.push_back(H + " Counts");
        } else if (!strcasecmp(M_Name.c_str(), "inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display the sum of inclusive events
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, inevents_temp));
          std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
          HV.push_back(H + " Counts");
        } else if (!strcasecmp(M_Name.c_str(), "overflow") ||
                   !strcasecmp(M_Name.c_str(), "overflows") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_overflow") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_overflows") ) {
         // display total exclusive count of the number of times the hardware counter overflowed.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
          HV.push_back(H + " Overflows");
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_overflow") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_overflows")) {
         // display total inclusive count of the number of times the hardware counter overflowed.
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
          HV.push_back(H + " Overflows");
        } else if (!strcasecmp(M_Name.c_str(), "%overflow")   ||
                   !strcasecmp(M_Name.c_str(), "%overflows") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_overflow") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_overflows") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total overflows.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the exevent_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, exevents_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, exevents_temp, totalIndex++));
          std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
          HV.push_back(std::string("% of Total ") + H + " Overflows");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_overflow") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_overflows")) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total overflows.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the exevent_temp values.
            generate_nested_accounting = true;
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, exevents_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, inevents_temp, totalIndex++));
          std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
          HV.push_back(std::string("% of Total ") + H + " Overflows");
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcmp(M_Name.c_str(), "%")           ||
                   !strcasecmp(M_Name.c_str(), "%event") ||
                   !strcasecmp(M_Name.c_str(), "%events") ||
                   !strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m exclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the excnt_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
          std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
          HV.push_back(std::string("% of Total ") + H + " Counts");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_details") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m inclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the excnt_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp, totalIndex++));
          std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
          HV.push_back(std::string("% of Total ") + H + " Counts");
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                   !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average of the overflows..
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, texeventmean_temp));
            std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
            HV.push_back(std::string("Average ") + H + " Counts Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
         // Find the By-Thread Min.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, texeventmin_temp));
            std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
            HV.push_back(std::string("Min ") + H + " Counts Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, texeventmax_temp));
            std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
            HV.push_back(std::string("Max ") + H + " Counts Across Threads");
          }
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
    }
  } else if (Generate_ButterFly) {
   // Default ButterFly view.
   // Column[0] is inclusive events
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, inevents_temp));
    std::string H = Event_Name_Header (CV[0], "inclusive_overflows");
    HV.push_back(H + " Counts");

  // Column[1] in % of inclusive events
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, exevents_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, inevents_temp, totalIndex++));
    HV.push_back(std::string("% of Total ") + H + " Counts");

  } else {
   // If nothing is requested ...
   // Column[0] is exclusive events
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, exevents_temp));
    std::string H = Event_Name_Header (CV[0], "exclusive_overflows");
    HV.push_back(H + " Counts");

   // Column[1] is inclusive events
    generate_nested_accounting = true;
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, inevents_temp));
    HV.push_back(Event_Name_Header (CV[0], "inclusive_overflows") + " Counts");

   // and include % of exclusive time
    if (Filter_Uses_F(cmd)) {
     // Use the metric needed for calculating total time.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
    } else {
     // Sum the exevent_temp values.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, exevents_temp));
    }
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, exevents_temp, totalIndex++));
    HV.push_back(std::string("% of Total ") + H + " Counts");
  }
  if (generate_nested_accounting) {
    IV.push_back(new ViewInstruction (VIEWINST_StackExpand, inevents_temp));
    IV.push_back(new ViewInstruction (VIEWINST_StackExpand, incnt_temp));
  }
  return (HV.size() > 0);
}

static bool hwctime_definition (
                             CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {

    Validate_V_Options (cmd, allowed_hwctime_V_options);

    CV.push_back (Get_Collector (exp->FW(), "hwctime"));  // Define the collector
    MV.push_back ("inclusive_detail"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "hwctime"));  // Define the collector
    MV.push_back ("exclusive_overflows"); // define the metric needed for calculating total time.
    return define_hwctime_columns (cmd, CV, IV, HV, vfc);
}

static std::string VIEW_hwctime_brief = "Hardware Counter Report";
static std::string VIEW_hwctime_short = "Report the amount of time spent in a code unit.";
static std::string VIEW_hwctime_long  = 
                  "\nA positive integer can be added to the end of the keyword"
                  " 'hwctime' to indicate the maximum number of items in the report."
                  "\n\nThe report may include exclusive events (recorded when program"
                  " execution is within a function) or inclusive events (recorded when"
                  " the function is part of the call stack)."
                  " The form of the information displayed can be controlled through"
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
                  " '-m exclusive_counts, inclusive_counts, percent'."
                  " Each value pertains to the function, statement or linked object that is"
                  " on that row of the report.  The 'Thread...' selections pertain to the"
                  " process unit that the program was partitioned into: Pid's,"
                  " Posix threads, Mpi threads or Ranks."
                  " \n\t'-m exclusive_counts' reports the number of times the measured event"
                  " occurred in the code unit."
                  " \n\t'-m exclusive_overflows' reports the number of times the hardware's counters"
                  " overflowed and samples were recorded for the code unit."
                  " \n\t'-m inclusive_counts' reports the number of times the measured event"
                  " occurred in the code unit and all the units it calls."
                  " \n\t'-m inclusive_overflows' reports the number of times the hardware's counters"
                  " overflowed and samples were recorded for the code unit and all the units it calls."
                  " \n\t'-m percent' reports the percent of total counts the code unit represents."
                  " \n\t'-m ThreadAverage' reports the average counts for a process."
                  " \n\t'-m ThreadMin' reports the minimum counts for a process."
                  " \n\t'-m ThreadMax' reports the maximum counts for a process."
                  "\n";
static std::string VIEW_hwctime_example = "\texpView hwctime\n"
                                           "\texpView -v LinkedObjects hwctime\n"
                                           "\texpView -v Statements hwctime20\n"
                                           "\texpView -v Functions hwctime10 -m hwctime::exclusive_overflows\n"
                                           "\texpView hwctime20 -m exclusive_overflows, inclusive_overflows\n"
                                           "\texpView -v CallTrees,FullStack hwctime10 -m exclusive_counts\n";
static std::string VIEW_hwctime_metrics[] =
  { "exclusive_overflows",
    "exclusive_detail",
    "inclusive_overflow",
    "inclusive_detail",
    ""
  };
static std::string VIEW_hwctime_collectors[] =
  { "hwctime",
    ""
  };
class hwctime_view : public ViewType {

 public: 
  hwctime_view() : ViewType ("hwctime",
                         VIEW_hwctime_brief,
                         VIEW_hwctime_short,
                         VIEW_hwctime_long,
                         VIEW_hwctime_example,
                        &VIEW_hwctime_metrics[0],
                        &VIEW_hwctime_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (hwctime_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {
      HWTimeDetail *dummyDetail;
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
extern "C" void hwctime_view_LTX_ViewFactory () {
  Define_New_View (new hwctime_view());
}
