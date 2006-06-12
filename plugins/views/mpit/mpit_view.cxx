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
#include "MPITCollector.hxx"
#include "MPITDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage mpit collector data.
#define intime_temp VMulti_free_temp
#define incnt_temp VMulti_free_temp+1
#define extime_temp VMulti_free_temp+2
#define excnt_temp VMulti_free_temp+3
#define start_temp VMulti_free_temp+4
#define stop_temp VMulti_free_temp+5
#define min_temp VMulti_free_temp+6
#define max_temp VMulti_free_temp+7
#define ssq_temp VMulti_free_temp+8
#define tmean_temp  VMulti_free_temp+9
#define tmin_temp  VMulti_free_temp+10
#define tmax_temp  VMulti_free_temp+11

#define source_temp VMulti_free_temp+12
#define destination_temp VMulti_free_temp+13
#define size_temp VMulti_free_temp+14
#define tag_temp VMulti_free_temp+15
#define communicator_temp VMulti_free_temp+16
#define datatype_temp VMulti_free_temp+17
#define retval_temp VMulti_free_temp+18

// mpit view

#define def_MPIT_values                          \
            Time start = Time::TheEnd();         \
            Time end = Time::TheBeginning();     \
            double intime = 0.0;                 \
            int64_t incnt = 0;                   \
            double extime = 0.0;                 \
            int64_t excnt = 0;                   \
            double vmax = 0.0;                   \
            double vmin = LONG_MAX;              \
            double sum_squares = 0.0;            \
            int64_t detail_source = 0;           \
            int64_t detail_destination = 0;      \
            uint64_t detail_size = 0;            \
            int64_t detail_tag = 0;              \
            int64_t detail_communicator = 0;     \
            int64_t detail_datatype = 0;         \
            int64_t detail_retval = 0;

#define get_MPIT_invalues(primary,num_calls)                     \
              double v = primary.dm_time / num_calls;            \
              intime += v;                                       \
              incnt++;                                           \
              start = min(start,primary.dm_interval.getBegin()); \
              end = max(end,primary.dm_interval.getEnd());       \
              vmin = min(vmin,v);                                \
              vmax = max(vmax,v);                                \
              sum_squares += v * v;                              \
              detail_source = primary.dm_source;                 \
              detail_destination = primary.dm_destination;       \
              detail_size += primary.dm_size;                    \
              detail_tag = primary.dm_tag;                       \
              detail_communicator = primary.dm_communicator;     \
              detail_datatype = primary.dm_datatype;             \
              detail_retval = primary.dm_retval;

#define get_MPIT_exvalues(primary,num_calls)         \
              extime += primary.dm_time / num_calls; \
              excnt++;

#define get_inclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_MPIT_invalues(stdv[i],num_calls)      \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_MPIT_exvalues(stdv[i],num_calls)      \
            }                                           \
}

#define set_MPIT_values(value_array, sort_extime) \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;     \
              if (num_temps > start_temp) {                                               \
                int64_t x= (start-base_time);                                             \
                value_array[start_temp] = new CommandResult_Duration (x);                 \
              }                                                                           \
              if (num_temps > stop_temp) {                                                \
                int64_t x= (end-base_time);                                               \
                value_array[stop_temp] = new CommandResult_Duration (x);                  \
              }                                                                           \
              if (num_temps > VMulti_time_temp) value_array[VMulti_time_temp]             \
                            = new CommandResult_Interval (sort_extime ? extime : intime); \
              if (num_temps > intime_temp) value_array[intime_temp]                       \
                            = new CommandResult_Interval (intime);                        \
              if (num_temps > incnt_temp) value_array[incnt_temp] = CRPTR (incnt);        \
              if (num_temps > extime_temp) value_array[extime_temp]                       \
                            = new CommandResult_Interval (extime);                        \
              if (num_temps > excnt_temp) value_array[excnt_temp] = CRPTR (excnt);        \
              if (num_temps > min_temp) value_array[min_temp]                             \
                            = new CommandResult_Interval (vmin);                          \
              if (num_temps > max_temp) value_array[max_temp]                             \
                            = new CommandResult_Interval (vmax);                          \
              if (num_temps > ssq_temp) value_array[ssq_temp]                             \
                            = new CommandResult_Interval (sum_squares);                   \
              if (num_temps > source_temp) value_array[source_temp] = CRPTR (detail_source); \
              if (num_temps > destination_temp) value_array[destination_temp] = CRPTR (detail_destination); \
              if (num_temps > size_temp) value_array[size_temp] = CRPTR (detail_size); \
              if (num_temps > tag_temp) value_array[tag_temp] = CRPTR (detail_tag); \
              if (num_temps > communicator_temp) value_array[communicator_temp] = CRPTR (detail_communicator); \
              if (num_temps > datatype_temp) value_array[datatype_temp] = CRPTR (detail_datatype); \
              if (num_temps > retval_temp) value_array[retval_temp] = CRPTR (detail_retval);

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

    if ((f_list == NULL) || (f_list->empty())) { 
     // There is no <file> list for filtering.
     // Get all the mpi functions for all, previously selected, threads.
      objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
    } else {
     // use the general utility to select the specified threads.
      Get_Filtered_Objects (cmd, exp, tgrp, objects);
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

#define def_Detail_values def_MPIT_values
#define get_inclusive_trace get_MPIT_invalues
#define get_exclusive_trace get_MPIT_exvalues
#define set_Detail_values set_MPIT_values
#include "SS_View_detail.txx"

static std::string allowed_mpit_V_options[] = {
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

static bool define_mpit_columns (
            CommandObject *cmd,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {

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

 // Most detail fields are not combinable in a meaningful way.
  IV.push_back(new ViewInstruction (VIEWINST_Add, size_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  std::string Default_Header = Find_Metadata ( CV[0], MV[1] ).getShortName();

  if (Generate_Summary) {
    if (Generate_ButterFly) {
      Generate_Summary = false;
      Mark_Cmd_With_Soft_Error(cmd,"Warning: 'summary' is not supported with '-v ButterFly'.");
    } else {
     // Total time is always displayed - also add display of the summary time.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
    }
  }

  int64_t last_column = 0;
  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      bool column_is_DateTime = false;
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "mpit")) {
         // We only know what to do with the usertime collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'mpit' view.");
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
            !strcasecmp(M_Name.c_str(), "exclusive_time") ||
            !strcasecmp(M_Name.c_str(), "exclusive_times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_detail") ||
            !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
          HV.push_back(std::string("Exclusive ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "inclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
          HV.push_back(std::string("Inclusive ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "min")) {
         // display min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, min_temp));
          HV.push_back(std::string("Minimum ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // display max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, max_temp));
          HV.push_back(std::string("Maximum ") + Default_Header + "(ms)");
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "exclusive_counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          HV.push_back("Number of Calls");
        } else if ( !strcasecmp(M_Name.c_str(), "inclusive_count") ||
                    !strcasecmp(M_Name.c_str(), "inclusive_counts")) {
         // display total exclusive counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Inclusive Calls");
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column++, VMulti_time_temp, extime_temp));
          HV.push_back("Average Time(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcasecmp(M_Name.c_str(), "%") ||
                   !strcasecmp(M_Name.c_str(), "%time") ||
                   !strcasecmp(M_Name.c_str(), "%times")) {
         // percent is calculate from 2 temps: time for this row and total time.
          if (Generate_ButterFly) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp));
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, VMulti_time_temp));
          }
          HV.push_back("% of Total");
        } else if (!strcasecmp(M_Name.c_str(), "%exclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_times")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp));
          HV.push_back("% of Total");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_times")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp));
          HV.push_back("% of Total");
        } else if (!strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts")) {
         // percent is calculate from 2 temps: counts for this row and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp));
          HV.push_back("% of Total Counts");
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum, sum of squares and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column++,
                                            VMulti_time_temp, ssq_temp, incnt_temp));
          HV.push_back("Standard Deviation");
        } else if (!strcasecmp(M_Name.c_str(), "start_time")) {
          if (vfc == VFC_Trace) {
           // display start time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
            HV.push_back("Start Time(d:h:m:s)");
            column_is_DateTime = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "stop_time")) {
          if (vfc == VFC_Trace) {
           // display stop time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, stop_temp));
            HV.push_back("Stop Time(d:h:m:s)");
            column_is_DateTime = true;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m stop_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                   !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
            HV.push_back(std::string("Average ") + Default_Header + " Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
         // Find the By-Thread Min.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
            HV.push_back(std::string("Min ") + Default_Header + " Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
            HV.push_back(std::string("Max ") + Default_Header + " Across Threads");
          }
        } else if (!strcasecmp(M_Name.c_str(), "source")) {
          if (vfc == VFC_Trace) {
           // display source rank
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, source_temp));
            HV.push_back("Source Rank");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m source' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "dest")) {
          if (vfc == VFC_Trace) {
           // display destination rank
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, destination_temp));
            HV.push_back("Destination Rank");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m dest' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "size")) {
         // display size of message
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, size_temp));
          HV.push_back("Message Size");
        } else if (!strcasecmp(M_Name.c_str(), "tag")) {
          if (vfc == VFC_Trace) {
           // display tag of the message
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tag_temp));
            HV.push_back("Message Tag");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m tag' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "comm")) {
          if (vfc == VFC_Trace) {
           // display communicator used
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, communicator_temp));
            HV.push_back("Communicator Used");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m comm' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "datatype")) {
          if (vfc == VFC_Trace) {
           // display data type of the message
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, datatype_temp));
            HV.push_back("Message Type");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m datatype' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "retval")) {
          if (vfc == VFC_Trace) {
           // display enumerated return value
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, retval_temp));
            HV.push_back("Return Value");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m retval' only supported for '-v Trace' option.");
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
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
    HV.push_back("Inclusive Time(ms)");

  // Column[1] in % of inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp));
    HV.push_back("% of Total");
  } else {
   // If nothing is requested ...
    if (vfc == VFC_Trace) {
      // Insert start times into report.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
      HV.push_back("Start Time(d:h:m:s)");
      IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, 1)); // final report in ascending time order
    }

   // Always display elapsed time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
    HV.push_back(std::string("Exclusive ") + Default_Header + "(ms)");

  // and include % of exclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp));
    HV.push_back("% of Total");
  }
  return (HV.size() > 0);
}

static bool mpit_definition (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_mpit_V_options);
    return define_mpit_columns (cmd, CV, MV, IV, HV, vfc);
}

static std::string VIEW_mpit_brief = "Mpit Report";
static std::string VIEW_mpit_short = "Report information about the calls to mpi functions.";
static std::string VIEW_mpit_long  =
                  "\n\nA positive integer can be added to the end of the keyword"
                  " 'mpit' to indicate the maximum number of items in the report."
                  " When the '-v Trace' option is selected, the selected items are"
                  " the ones that use the most time.  In all other cases"
                  " the selection will be based on the values displayed in"
                  " left most column of the report."
                  "\n\nThe form of the information displayed can be controlled through"
                  " the  '-v' option.  Except for the '-v Trace' option, the report will"
                  " be sorted in descending order of the value in the left most column"
                  " displayed on a line. [See '-m' option for controlling this field.]"
                  "\n\t'-v Functions' will produce a summary report for each function."
                  " This is the default report, if nothing else is requested."
                  "\n\t'-v Trace' will produce a report of each call to an mpi function."
                  " It will be sorted in ascending order of the starting time for the event."
                  "\n\t'-v CallTrees' will produce report that expands the call stack for"
                  " a trace, providing the sequence of functions called from the start of"
                  " the program to the measured function."
                  "\n\t'-v TraceBacks' will produce a report that expands the call stack for"
                  " a trace, providing the sequence of functions called from the function"
                  " back to the start of the program."
                  "\n\tThe addition of 'FullStack' with either 'CallTrees' of 'TraceBacks'"
                  " will cause the report to include the full call stack for each measured"
                  " function.  Redundant portions of a call stack are suppressed if this"
                  " option is not specified."
                  "\n\tThe addition of 'Summary' to the '-v' option list will result in an"
                  " additional line of output at"
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
                  " Each value pertains to the MPI function that is"
                  " on that row of the report.  The 'Thread...' selections pertain to the"
                  " process unit that the program was partitioned into: Pid's,"
                  " Posix threads, Mpi threads or Ranks."
                  " If no '-m' option is specified, the default is equivalent to"
                  " '-m exclusive times, percent'."
                  " Clearly, not every value will be meaningful with every '-v' option."
                  " The available '-m' options are:"
                  " \n\t'-m exclusive_times' reports the wall clock time used in the event."
                  " \n\t'-m min' reports the minimum time spent in the event."
                  " \n\t'-m max' reports the maximum time spent in the event."
                  " \n\t'-m average' reports the average time spent in the event."
                  " \n\t'-m count' reports the number of times the event occured."
                  " \n\t'-m percent' reports the percent of mpi time the event represents."
                  " \n\t'-m stddev' reports the standard deviation of the average mpi time"
                  " that the event represents."
                  " \n\t'-m ThreadAverage' reports the average cpu time for a process."
                  " \n\t'-m ThreadMin' reports the minimum cpu time for a process."
                  " \n\t'-m ThreadMin' reports the maximum cpu time for a process."
                  " \n\t'-m start_time' reports the starting time of the event."
                  " \n\t'-m stop_time' reports the ending time of the event."
                  " \n\t'-m source' reports the source rank of the event."
                  " \n\t'-m dest' reports the destination rank of the event."
                  " \n\t'-m size' reports the number of bytes in the message."
                  " \n\t'-m tag' reports the tag of the event."
                  " \n\t'-m comm' reports the communicator used for the event."
                  " \n\t'-m datatype' reports the data type of the message."
                  " \n\t'-m retval' reports the return value of the event."
                  "\n";
static std::string VIEW_mpit_example = "\texpView mpit\n"
                                       "\texpView -v Trace mpit10\n" 
                                       "\texpView -v Trace mpit100 -m start_time, inclusive_time, size\n";
static std::string VIEW_mpit_metrics[] =
  { "time",
    "inclusive_times",
    "inclusive_details",
    "exclusive_times",
    "exclusive_details",
    ""
  };
static std::string VIEW_mpit_collectors[] =
  { "mpit",
    ""
  };
class mpit_view : public ViewType {

 public: 
  mpit_view() : ViewType ("mpit",
                          VIEW_mpit_brief,
                          VIEW_mpit_short,
                          VIEW_mpit_long,
                          VIEW_mpit_example,
                         &VIEW_mpit_metrics[0],
                         &VIEW_mpit_collectors[0],
                          true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "mpit"));  // Define the collector
    MV.push_back ("inclusive_details"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "mpit"));  // Define the collector
    MV.push_back ("time"); // define the metric needed for calculating total time.
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (mpit_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<MPITDetail> dummyVector;
      MPITDetail *dummyDetail;
      switch (Determine_Form_Category(cmd)) {
       case VFC_Trace:
        return Detail_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                    Determine_Metric_Ordering(IV), dummyDetail, view_output);
       case VFC_CallStack:
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          return Detail_ButterFly_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), &dummyVector, view_output);
        } else {
          return Detail_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                          Determine_Metric_Ordering(IV), &dummyVector, view_output);
        }
       case VFC_Function:
        Framework::Function *dummyObject;
        return Detail_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   Determine_Metric_Ordering(IV), dummyObject, VFC_Function, &dummyVector, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name recognized.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mpit_view_LTX_ViewFactory () {
  Define_New_View (new mpit_view());
}
