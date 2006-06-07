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
#include "FPECollector.hxx"
#include "FPEDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage fpe collector data.
#define excnt_temp   VMulti_free_temp
#define incnt_temp   VMulti_free_temp+1
#define start_temp   VMulti_free_temp+2
#define fpeType_temp VMulti_free_temp+3
#define tmean_temp   VMulti_free_temp+4
#define tmin_temp    VMulti_free_temp+5
#define tmax_temp    VMulti_free_temp+6
#define extra_division_by_zero_temp VMulti_free_temp+7
#define extra_inexact_result_temp VMulti_free_temp+8
#define extra_invalid_temp VMulti_free_temp+9
#define extra_overflow_temp VMulti_free_temp+10
#define extra_underflow_temp VMulti_free_temp+11
#define extra_unknown_temp VMulti_free_temp+12
#define extra_unnormal_temp VMulti_free_temp+13

#define division_by_zero_index ViewReduction_Count
#define inexact_index ViewReduction_Count+1
#define invalid_index ViewReduction_Count+2
#define overflow_index ViewReduction_Count+3
#define underflow_index ViewReduction_Count+4
#define unknown_index ViewReduction_Count+5
#define unnormal_index ViewReduction_Count+6

// fpe view

#define def_FPE_values                                 \
            Time start = Time::TheEnd();               \
            int64_t incnt = 0;                         \
            int64_t excnt = 0;                         \
            int64_t fpeType = 0;

#define get_FPE_invalues(primary, num_calls)           \
              start = min(start,primary.dm_time);      \
              fpeType = primary.dm_type;               \
              incnt++;

#define get_FPE_exvalues(secondary,num_calls)           \
              excnt++;

#define get_inclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_FPE_invalues(stdv[i],num_calls)       \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_FPE_exvalues(stdv[i],num_calls)       \
            }                                           \
}

#define set_FPE_values(value_array, sort_excnt)                                              \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;        \
              if (num_temps > start_temp) {                                                  \
                int64_t x= (start-base_time);                                                \
                value_array[start_temp] = new CommandResult_Duration (x);                    \
              }                                                                              \
              if (num_temps > VMulti_time_temp)  {                                           \
               /* By default, sort on counts, not time. */                                   \
                value_array[VMulti_time_temp]  = CRPTR (sort_excnt ? excnt : incnt);         \
              }                                                                              \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (incnt);           \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (excnt);           \
              if (num_temps > fpeType_temp) value_array[fpeType_temp] = CRPTR (fpeType);

#define set_ExtraMetric_values(value_array, ExtraValues, index)                                      \
              if (num_temps > tmean_temp) {                                                          \
                if (ExtraValues[ViewReduction_mean]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_mean]->end()) {   \
                  value_array[tmean_temp]                                                            \
                       = Dup_CommandResult(ExtraValues[ViewReduction_mean]->find(index)->second);    \
                } else {                                                                             \
                  value_array[tmean_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmin_temp) {                                                           \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
                  value_array[tmin_temp]                                                             \
                       = Dup_CommandResult(ExtraValues[ViewReduction_min]->find(index)->second);     \
                } else {                                                                             \
                  value_array[tmin_temp] = CRPTR ((int64_t)0);                                       \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmax_temp) {                                                           \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
                  value_array[tmax_temp]                                                             \
                       = Dup_CommandResult(ExtraValues[ViewReduction_max]->find(index)->second);     \
                } else {                                                                             \
                  value_array[tmax_temp] = CRPTR ((int64_t)0);                                       \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_division_by_zero_temp) {                                         \
                if (ExtraValues[division_by_zero_index]->find(index)                                 \
                                                      != ExtraValues[division_by_zero_index]->end()) { \
                  value_array[extra_division_by_zero_temp]                                             \
                       = Dup_CommandResult(ExtraValues[division_by_zero_index]->find(index)->second);  \
                } else {                                                                             \
                  value_array[extra_division_by_zero_temp] = CRPTR ((uint64_t)0);                    \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_inexact_result_temp) {                                           \
                if (ExtraValues[inexact_index]->find(index)                                          \
                                                      != ExtraValues[inexact_index]->end()) {        \
                  value_array[extra_inexact_result_temp]                                             \
                       = Dup_CommandResult(ExtraValues[inexact_index]->find(index)->second);         \
                } else {                                                                             \
                  value_array[extra_inexact_result_temp] = CRPTR ((uint64_t)0);                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_invalid_temp) {                                                  \
                if (ExtraValues[invalid_index]->find(index)                                          \
                                                      != ExtraValues[invalid_index]->end()) {        \
                  value_array[extra_invalid_temp]                                                    \
                       = Dup_CommandResult(ExtraValues[invalid_index]->find(index)->second);         \
                } else {                                                                             \
                  value_array[extra_invalid_temp] = CRPTR ((uint64_t)0);                             \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_overflow_temp) {                                                 \
                if (ExtraValues[overflow_index]->find(index)                                         \
                                                      != ExtraValues[overflow_index]->end()) {       \
                  value_array[extra_overflow_temp]                                                   \
                       = Dup_CommandResult(ExtraValues[overflow_index]->find(index)->second);        \
                } else {                                                                             \
                  value_array[extra_overflow_temp] = CRPTR ((uint64_t)0);                            \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_underflow_temp) {                                                \
                if (ExtraValues[underflow_index]->find(index)                                        \
                                                      != ExtraValues[underflow_index]->end()) {      \
                  value_array[extra_underflow_temp]                                                  \
                       = Dup_CommandResult(ExtraValues[underflow_index]->find(index)->second);       \
                } else {                                                                             \
                  value_array[extra_underflow_temp] = CRPTR ((uint64_t)0);                           \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_unknown_temp) {                                                  \
                if (ExtraValues[unknown_index]->find(index)                                          \
                                                      != ExtraValues[unknown_index]->end()) {        \
                  value_array[extra_unknown_temp]                                                    \
                       = Dup_CommandResult(ExtraValues[unknown_index]->find(index)->second);         \
                } else {                                                                             \
                  value_array[extra_unknown_temp] = CRPTR ((uint64_t)0);                             \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > extra_unnormal_temp) {                                                 \
                if (ExtraValues[unnormal_index]->find(index)                                         \
                                                      != ExtraValues[unnormal_index]->end()) {       \
                  value_array[extra_unnormal_temp]                                                   \
                       = Dup_CommandResult(ExtraValues[unnormal_index]->find(index)->second);        \
                } else {                                                                             \
                  value_array[extra_unnormal_temp] = CRPTR ((uint64_t)0);                            \
                }                                                                                    \
              }

#define def_Detail_values def_FPE_values
#define set_Detail_values set_FPE_values
#define Determine_Objects Get_Filtered_Objects
#include "SS_View_detail.txx"


static std::string allowed_fpe_V_options[] = {
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

static bool define_fpe_columns (
            CommandObject *cmd,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Total time is always placed in first column.
  bool user_defined = false;

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_division_by_zero_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_inexact_result_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_invalid_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_overflow_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_underflow_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_unknown_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extra_unnormal_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, incnt_temp));

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
        if (!strcasecmp(M_Name.c_str(), "fpe")) {
         // We only know what to do with the usertime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'fpe' view.");
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
         if ( !strcasecmp(M_Name.c_str(), "count") ||
              !strcasecmp(M_Name.c_str(), "counts") ||
              !strcasecmp(M_Name.c_str(), "event") ||
              !strcasecmp(M_Name.c_str(), "events") ||
              !strcasecmp(M_Name.c_str(), "total_count") ||
              !strcasecmp(M_Name.c_str(), "total_counts") ||
              !strcasecmp(M_Name.c_str(), "exclusive_count") ||
              !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
              !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
              !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          HV.push_back("Exclusive Fpe Events");
          user_defined = true;
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display total inclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Inclusive Fpe Events");
          user_defined = true;
        } else if ( !strcasecmp(M_Name.c_str(), "Type") ||
                    !strcasecmp(M_Name.c_str(), "Types")) {
         // display the event type
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, fpeType_temp));
          HV.push_back("Fpe Event Type");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcmp(M_Name.c_str(), "%")           ||
                   !strcasecmp(M_Name.c_str(), "%event") ||
                   !strcasecmp(M_Name.c_str(), "%events") ||
                   !strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_details") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp));
          HV.push_back("% of Total Exclusive Fpe Events");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_counts") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_details") ) {
         // percent is calculate from 2 temps: number of events for this row and total exclusive events.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp));
          HV.push_back("% of Total Inclusive Fpe Events");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "time") ||
                   !strcasecmp(M_Name.c_str(), "times") ||
                   !strcasecmp(M_Name.c_str(), "start_time") ||
                   !strcasecmp(M_Name.c_str(), "start_times")) {
          if (vfc == VFC_Trace) {
           // display event time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
            HV.push_back("Fpe Event Time(d:h:m:s)");
            column_is_DateTime = true;
            user_defined = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if ( !strcasecmp(M_Name.c_str(), "division_by_zero_count") ||
                    !strcasecmp(M_Name.c_str(), "inexact_result_count") ||
                    !strcasecmp(M_Name.c_str(), "invalid_count") ||
                    !strcasecmp(M_Name.c_str(), "overflow_count") ||
                    !strcasecmp(M_Name.c_str(), "underflow_count") ||
                    !strcasecmp(M_Name.c_str(), "unknown_count") ||
                    !strcasecmp(M_Name.c_str(), "unnormal_count")) {
         // display specific exception counts
         // Accomplish this by changing the requested metric.
          int64_t extra_index = 0;
          int64_t extra_temp = 0;
          if (!strcasecmp(M_Name.c_str(), "division_by_zero_count")) {
            extra_index = division_by_zero_index;
            extra_temp = extra_division_by_zero_temp;
          } else if (!strcasecmp(M_Name.c_str(), "inexact_result_count")) {
            extra_index = inexact_index;
            extra_temp = extra_inexact_result_temp;
          } else if (!strcasecmp(M_Name.c_str(), "invalid_count")) {
            extra_index = invalid_index;
            extra_temp = extra_invalid_temp;
          } else if (!strcasecmp(M_Name.c_str(), "overflow_count")) {
            extra_index = overflow_index;
            extra_temp = extra_overflow_temp;
          } else if (!strcasecmp(M_Name.c_str(), "underflow_count")) {
            extra_index = underflow_index;
            extra_temp = extra_underflow_temp;
          } else if (!strcasecmp(M_Name.c_str(), "unknown_count")) {
            extra_index = unknown_index;
            extra_temp = extra_unknown_temp;
          } else if (!strcasecmp(M_Name.c_str(), "unnormal_count")) {
            extra_index = unnormal_index;
            extra_temp = extra_unnormal_temp;
          }
          int64_t cmIx = CV.size();
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, extra_index, cmIx, ViewReduction_sum));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extra_temp));
          CV.push_back(CV[0]);
          MV.push_back(M_Name);
          HV.push_back( Find_Metadata(CV[cmIx],MV[cmIx]).getDescription() );
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                   !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average of the overflows..
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
          HV.push_back("Average Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
         // Find the By-Thread Min.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
          HV.push_back("Min Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
          HV.push_back("Max Exclusive Fpe Event Counts Across Threads");
          user_defined = true;
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
      if (last_column == 1) {
        IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, (int64_t)(column_is_DateTime) ? 1 : 0));
      }
    }
  } 

  if (!user_defined) {
    if (Generate_ButterFly) {
     // Default ButterFly view.
     // Column[0] is inclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
      HV.push_back("Inclusive Fpe Event Counts");

    // Column[1] in % of exclusive events
      IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp));
      HV.push_back("% of Total Counts");
    } else {
     // If nothing is requested ...
      if (vfc == VFC_Trace) {
        // Insert event time into report.
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
        HV.push_back("Fpe Event Time(d:h:m:s)");
        IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, 1)); // final report in ascending time order
      }
     // Always display counts.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
      HV.push_back("Exclusive Fpe Event Counts");

      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
      HV.push_back("Inclusive Fpe Event Counts");

    // And display % of exclusive events
      IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp));
      HV.push_back("% of Total Counts");
    }
  }
  return (HV.size() > 0);
}

static bool fpe_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
    Assert (CV.begin() != CV.end());
    CollectorGroup cgrp = exp->FW()->getCollectors();
    Collector C = *CV.begin();
    if (cgrp.find(C) == std::set<Collector>::iterator(cgrp.end())) {
      std::string C_Name = C.getMetadata().getUniqueId();
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
    std::string M_Name = MV[0];
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_fpe_V_options);
    return define_fpe_columns (cmd, CV, MV, IV, HV, vfc);
}


static std::string VIEW_fpe_brief = "Fpe Report";
static std::string VIEW_fpe_short = "Report each floating point exception.";
static std::string VIEW_fpe_long  =
                  "\nA positive integer can be added to the end of the keyword"
                  " 'fpe' to indicate the maximum number of items in the report."
                  " The selection will be based on the number of times events occurred."
                  "\n\nThe form of the information displayed can be controlled through"
                  " the  '-v' option.  The report will"
                  " be sorted in descending order of the number of times events occurred."
                  "\n\t'-v Functions' will produce a summary report of the function the"
                  " in descending order of the number of times each fpe event"
                  " event ocurred in.  This is the default display."
                  "\n\t'-v Statements' will produce a summary report of the statement the"
                  " event occurred in."
                  "\n\t'-v LinkeObjects' will produce a summary report of the linked object"
                  " the event occurred in."
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
                  " 'Statements', 'LinkeObjects'"
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
                  " '-m counts, percent'."
                  " Each option reports information about the set of fpe events that is"
                  " reported for the items on that particular line in the report."
                  " \n\t'-m time' reports the first time at which the event occurred."
                  " \n\t'-m counts' reports the number of times the event occurred in the code unit."
                  " \n\t'-m percent' reports the percent of total counts the code unit."
                  " \n\t'-m ThreadAverage' reports the average counts for a process."
                  " \n\t'-m ThreadMin' reports the minimum counts for a process."
                  " \n\t'-m ThreadMax' reports the maximum counts for a process. "
                  " \n\t'-m type' reports the type of the floating point exception:"
                  " \n\t\t 0 implies inexact result"
                  " \n\t\t 1 implies underflow"
                  " \n\t\t 2 implies overflow"
                  " \n\t\t 3 implies division by zero"
                  " \n\t\t 4 implies unnormalized numbers"
                  " \n\t\t 5 implies invalid numbers"
                  " \n\t\t 6 implies an unknown event"
                  "\n\nIt is also possible to select a subset of event types to display."
                  " This is done with the '-m' option and a single key word:"
                  " \n\t'-m inexact_result_count'"
                  " \n\t'-m underflow_count'"
                  " \n\t'-m overflow_count'"
                  " \n\t'-m division_by_zero_count'"
                  " \n\t'-m unnormal_count'"
                  " \n\t'-m invalid_count'"
                  " \n\t'-m unknown_count'";
static std::string VIEW_fpe_example = "\texpView fpe\n"
                                      "\texpView -v CallTrees,FullStack fpe10 -m underflow_count\n";
static std::string VIEW_fpe_metrics[] =
  { "total_count",
    "exclusive_details",
    "inclusive_details",
    "division_by_zero_count",
    "inexact_result_count",
    "invalid_count",
    "overflow_count",
    "underflow_count",
    "unknown_count",
    "unnormal_count",
    ""
  };
static std::string VIEW_fpe_collectors[] =
  { "fpe",
    ""
  };
class fpe_view : public ViewType {

 public: 
  fpe_view() : ViewType ("fpe",
                         VIEW_fpe_brief,
                         VIEW_fpe_short,
                         VIEW_fpe_long,
                         VIEW_fpe_example,
                        &VIEW_fpe_metrics[0],
                        &VIEW_fpe_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "fpe"));  // Define the collector
    MV.push_back ("inclusive_details"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "fpe"));  // Define the collector
    MV.push_back ("total_count"); // define the metric needed for calculating total time.
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (fpe_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<FPEDetail> dummyVector;
      FPEDetail *dummyDetail;
      switch (vfc) {
       case VFC_Trace:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          true, &dummyVector, view_output);
        } else {
          return Detail_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                      true, dummyDetail, view_output);
        }
       case VFC_CallStack:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          true, &dummyVector, view_output);
        } else {
          return Detail_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          true, &dummyVector, view_output);
        }
       case VFC_Function:
        Framework::Function *fp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, fp, vfc, &dummyVector, view_output);
       case VFC_LinkedObject:
        LinkedObject *lp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, lp, vfc, &dummyVector, view_output);
       case VFC_Statement:
        Statement *sp;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, sp, vfc, &dummyVector, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name supplied.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(There is no requested information to report.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void fpe_view_LTX_ViewFactory () {
  Define_New_View (new fpe_view());
}
