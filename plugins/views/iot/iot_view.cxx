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
#include "IOTCollector.hxx"
#include "IOTDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage io collector data.
#define intime_temp 2
#define incnt_temp 3
#define extime_temp 4
#define excnt_temp 5
#define start_temp 6
#define stop_temp 7
#define min_temp 8
#define max_temp 8
#define ssq_temp 10

// io view

#define def_IOT_values \
            Time start = Time::TheEnd();         \
            Time end = Time::TheBeginning();     \
            double intime = 0.0;                 \
            int64_t incnt = 0;                   \
            double extime = 0.0;                 \
            int64_t excnt = 0;                   \
            double vmax = 0.0;                   \
            double vmin = LONG_MAX;              \
            double sum_squares = 0.0;

#define get_IOT_invalues(primary,num_calls)                      \
              double v = primary.dm_time / num_calls;            \
              intime += v;                                       \
              incnt++;                                           \
              start = min(start,primary.dm_interval.getBegin()); \
              end = max(end,primary.dm_interval.getEnd());       \
              vmin = min(vmin,v);                                \
              vmax = max(vmax,v);                                \
              sum_squares += v * v;

#define get_IOT_exvalues(secondary,num_calls)          \
              extime += secondary.dm_time / num_calls; \
              excnt++;

#define get_inclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_IOT_invalues(stdv[i],num_calls)       \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_IOT_exvalues(stdv[i],num_calls)       \
            }                                           \
}

#define set_IOT_values(value_array, sort_extime)                                          \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;     \
              if (num_temps > start_temp) {                                               \
                double x = (start-base_time) / 1000000.0;                                 \
                value_array[start_temp] = CRPTR (x);                                      \
              }                                                                           \
              if (num_temps > stop_temp) {                                                \
                double x = (end-base_time) / 1000000.0;                                   \
                value_array[stop_temp] = CRPTR (x);                                       \
              }                                                                           \
              if (num_temps > VMulti_time_temp) value_array[VMulti_time_temp]             \
                                                 = CRPTR (sort_extime ? extime : intime); \
              if (num_temps > intime_temp) value_array[intime_temp] = CRPTR (intime);     \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (incnt);        \
              if (num_temps > extime_temp) value_array[extime_temp] = CRPTR (extime);     \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (excnt);        \
              if (num_temps > min_temp) value_array[min_temp] = CRPTR (vmin);             \
              if (num_temps > max_temp) value_array[max_temp] = CRPTR (vmax);             \
              if (num_temps > ssq_temp) value_array[ssq_temp] = CRPTR (sum_squares);

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

#define def_Detail_values def_IOT_values
#define set_Detail_values set_IOT_values
#define Determine_Objects Get_Filtered_Objects
#include "SS_View_detail.txx"

static std::string allowed_iot_V_options[] = {
  "Function",
  "Functions",
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

static void define_iot_columns (
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
  IV.push_back(new ViewInstruction (VIEWINST_Add, intime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, ssq_temp));
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
    bool time_metric_selected = false;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      bool column_is_DateTime = false;
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "iot")) {
         // We only know what to do with the usertime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'iot' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          continue;
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
            !strcasecmp(M_Name.c_str(), "inclusive_time") ||
            !strcasecmp(M_Name.c_str(), "inclusive_times") ||
            !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
            !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display sum of times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, intime_temp));
          HV.push_back("Inclusive Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "exclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, extime_temp));
          HV.push_back("Exclusive Time");
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
                    !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, incnt_temp));
          HV.push_back("Number of Calls");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_counts")) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, excnt_temp));
          HV.push_back("Exclusive Calls");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column, VMulti_time_temp, intime_temp));
          HV.push_back("Average Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, intime_temp));
          HV.push_back("% of Total");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum, sum of squares and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column,
                                            VMulti_time_temp, ssq_temp, intime_temp));
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
  } else if (Generate_ButterFly) {
   // Default ButterFly view.
   // Column[0] is inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, intime_temp));
    HV.push_back("Inclusive Time");
    last_column++;

  // Column[1] in % of inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, intime_temp));
    HV.push_back("% of Total");
    last_column++;
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
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, intime_temp));
    HV.push_back("Inclusive Time");
  }
}

static bool iot_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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
    std::string M_Name("inclusive_details");
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_iot_V_options);
    define_iot_columns (cmd, IV, HV, vfc);

    return true;
}


static std::string VIEW_iot_brief = "I/O Trace Report";
static std::string VIEW_iot_short = "Report the time spent in each io function.";
static std::string VIEW_iot_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'iot' to indicate the maximum number of items in the report."
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
                                      "\n\t'-v Trace' will produce a report of each individual  call to an io"
                                      " function."
                                      " It will be sorted in ascending order of the starting time for the event."
                                      " The information available for display from an 'iot' experiment is very"
                                      " limited when compared to what is available from an 'iot' experiment."
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
                                      " Each option reports information about the set of io calls that is"
                                      " reported for the function on that particular line in the report."
                                      " \n\t'-m exclusive_times' reports the wall clock time used in the function."
                                      " \n\t'-m min' reports the minimum time spent in the function."
                                      " \n\t'-m max' reports the maximum time spent in the function."
                                      " \n\t'-m average' reports the average time spent in the function."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of io time the function represents."
                                      " \n\t'-m stddev' reports the standard deviation of the average io time"
                                      " that the function represents.";
static std::string VIEW_iot_example = "\texpView io\n"
                                      "\texpView -v CallTrees,FullStack io10 -m min,max,count\n";
static std::string VIEW_iot_metrics[] =
  { "time",
    "inclusive_times",
    "inclusive_details",
    "exclusive_details",
    "exclusive_times",
    ""
  };
static std::string VIEW_iot_collectors[] =
  { "iot",
    ""
  };
class iot_view : public ViewType {

 public: 
  iot_view() : ViewType ("iot",
                         VIEW_iot_brief,
                         VIEW_iot_short,
                         VIEW_iot_long,
                         VIEW_iot_example,
                        &VIEW_iot_metrics[0],
                        &VIEW_iot_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "iot"));  // Define the collector
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (iot_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<IOTDetail> dummyVector;
      IOTDetail *dummyDetail;
      switch (Determine_Form_Category(cmd)) {
       case VFC_Trace:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), &dummyVector, view_output);
        } else {
          return Detail_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), dummyDetail, view_output);
        }
       case VFC_CallStack:
        return Detail_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), &dummyVector, view_output);
       case VFC_Function:
        Framework::Function *dummyObject;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), dummyObject,
                                   VFC_Function, &dummyVector, view_output);
      }
    }
    Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name recognized.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void iot_view_LTX_ViewFactory () {
  Define_New_View (new iot_view());
}
