/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
#include "SS_View_Expr.hxx"
#include "HWTimeCollector.hxx"
#include "HWTimeDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage hwctime collector data.
#define exevents_temp VMulti_free_temp
#define excnt_temp VMulti_free_temp+1
#define inevents_temp VMulti_free_temp+2
#define incnt_temp VMulti_free_temp+3

#define First_ByThread_Temp VMulti_free_temp+4
#define ByThread_use_intervals 0 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 //  otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"


// hwctime view

#define def_HwcTime_values          \
            uint64_t ex_events = 0; \
            uint64_t ex_cnt = 0;    \
            uint64_t in_events = 0; \
            uint64_t in_cnt = 0;

#define get_inclusive_values(primary, num_calls, function_name)            \
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
  "Loop",
  "Loops",
  "ButterFly",
  "CallTree",
  "CallTrees",
  "TraceBack",
  "TraceBacks",
  "FullStack",
  "FullStacks",
  "DontExpand",
  "Summary",
  "SummaryOnly",
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
            ExperimentObject *exp,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Number of columns of information displayed.
  int64_t totalIndex  = 0;  // Number of totals needed to perform % calculations.
  int64_t last_used_temp = Last_ByThread_Temp; // Track maximum temps - needed for expressions.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, exevents_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, inevents_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, inevents_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool Generate_Summary = false;
  bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");
  if (!Generate_Summary_Only) {
     Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  }

  bool generate_nested_accounting = false;
  int64_t View_ByThread_Identifier = Determine_ByThread_Id (exp, cmd);
  std::string ByThread_Header = "exclusive_overflows Counts";

 if (Generate_Summary_Only) {
    if (Generate_ButterFly) {
      Generate_Summary_Only = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summaryonly' is not supported with '-v ButterFly'.");
    }
  } else if (Generate_Summary) {
    if (Generate_ButterFly) {
      Generate_Summary = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summary' is not supported with '-v ButterFly'.");
    }
  }

 // Define map for metrics to metric temp.
  std::map<std::string, int64_t> MetricMap;
  MetricMap["exclusive_detail"] = exevents_temp;
  MetricMap["exclusive_details"] = exevents_temp;
  MetricMap["exclusive_count"] = exevents_temp;
  MetricMap["exclusive_counts"] = exevents_temp;
  MetricMap["overflow"] = excnt_temp;
  MetricMap["overflows"] = excnt_temp;
  MetricMap["exclusive_overflow"] = excnt_temp;
  MetricMap["exclusive_overflows"] = excnt_temp;
  MetricMap["inclusive_detail"] = inevents_temp;
  MetricMap["inclusive_details"] = inevents_temp;
  MetricMap["inclusive_count"] = inevents_temp;
  MetricMap["inclusive_counts"] = inevents_temp;
  MetricMap["inclusive_overflow"] = incnt_temp;
  MetricMap["inclusive_overflows"] = incnt_temp;
  if (Generate_ButterFly) {
    MetricMap["event"] = inevents_temp;
    MetricMap["events"] = inevents_temp;
    MetricMap["count"] = inevents_temp;
    MetricMap["counts"] = inevents_temp;
  } else {
    MetricMap["event"] = exevents_temp;
    MetricMap["events"] = exevents_temp;
    MetricMap["count"] = exevents_temp;
    MetricMap["counts"] = exevents_temp;
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    std::vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {

// Look for a metric expression and invoke processing.
#include "SS_View_metric_expressions.hxx"

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
       } else if (!strcasecmp(M_Name.c_str(), "absdiff")) {
        // Ignore this because cview -c 3 -c 5 -mtime,absdiff actually works outside of this view code
        //         // Mark_Cmd_With_Soft_Error(cmd,"AbsDiff option, '-m " + M_Name + "'");
       }
// Recognize and generate pseudo instructions to calculate and display By Thread metrics for
// ThreadMax, ThreadMaxIndex, ThreadMin, ThreadMinIndex, ThreadAverage and loadbalance.
#include "SS_View_bythread_recognize.hxx"
        else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
          return false;
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

  // Add display of the summary time.
  if (Generate_Summary_Only) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Only));
   } else if (Generate_Summary) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
   }

  return (HV.size() > 0);
}

static bool hwctime_definition (
                             CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {

    // Warn about misspelled of meaningless options and exit command processing without generating a view.
    bool all_valid = Validate_V_Options (cmd, allowed_hwctime_V_options);
    if ( all_valid == false ) {
      return false;
    }


    CV.push_back (Get_Collector (exp->FW(), "hwctime"));  // Define the collector
    MV.push_back ("inclusive_detail"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "hwctime"));  // Define the collector
    MV.push_back ("exclusive_overflows"); // define the metric needed for calculating total time.
    return define_hwctime_columns (cmd, exp, CV, MV, IV, HV, vfc);
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
                  "\n\t'-v Loops' will report times by loop."
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
                  "\n\tThe addition of 'SummaryOnly' to the '-v' option list along with 'Functions',"
                  " 'Statements', 'LinkedObjects' or 'Loops' or without those options will cause only the"
                  " one line of output at the end of the report that summarizes the information in each column."
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
                  " \n\t'-m ThreadMin' reports the minimum counts for a process."
                  " \n\t'-m ThreadMinIndex' reports the Rank for the thread of the 'ThreadMin'."
                  " \n\t'-m ThreadMax' reports the maximum counts for a process."
                  " \n\t'-m ThreadMaxIndex' reports the Rank for the thread of the 'ThreadMax'."
                  " \n\t'-m ThreadAverage' reports the average counts for a process."
                  " \n\t'-m loadbalance' is the same as '-m ThreadMax, ThreadMaxIndex, ThreadMin,"
                  " ThreadMinIndex, ThreadAverage'."
                  "\n";
static std::string VIEW_hwctime_example = "\texpView hwctime\n"
                                           "\texpView -v LinkedObjects hwctime\n"
                                           "\texpView -v Statements hwctime20\n"
                                           "\texpView -v Loops hwctime5\n"
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
       case VFC_Loop:
        Loop *loopp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), loopp, vfc, dummyDetail, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(We could not determine which format to use for the report.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report for 'hwctime' view.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void hwctime_view_LTX_ViewFactory () {
  Define_New_View (new hwctime_view());
}
