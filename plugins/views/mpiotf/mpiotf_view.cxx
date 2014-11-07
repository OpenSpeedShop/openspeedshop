/*******************************************************************************
** Copyright (c) 2007 Krell Institute All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute. All Rights Reserved.
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
#include "MPIOTFCollector.hxx"
#include "MPIOTFDetail.hxx"

// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage mpiotf collector data.
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

// mpiotf view

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

#define get_MPIT_invalues(primary,num_calls, function_name)                     \
              double v = primary.dm_time / num_calls;            \
              intime += v;                                       \
              incnt++;                                           \
              start = std::min(start,primary.dm_interval.getBegin()); \
              end = std::max(end,primary.dm_interval.getEnd());       \
              vmin = std::min(vmin,v);                                \
              vmax = std::max(vmax,v);                                \
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

#define get_inclusive_values(stdv, num_calls, function_name)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_MPIT_invalues(stdv[i],num_calls, function_name)      \
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
                       = ExtraValues[ViewReduction_mean]->find(index)->second->Copy();               \
                } else {                                                                             \
                  value_array[tmean_temp] = CRPTR ((double)0.0);                                     \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmin_temp) {                                                           \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
                  value_array[tmin_temp]                                                             \
                       = ExtraValues[ViewReduction_min]->find(index)->second->Copy();                \
                } else {                                                                             \
                  value_array[tmin_temp] = CRPTR ((double)0.0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmax_temp) {                                                           \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
                  value_array[tmax_temp]                                                             \
                       = ExtraValues[ViewReduction_max]->find(index)->second->Copy();                \
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
  std::vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  std::set<Function> mpi_objects;
  std::set<Function> pmpi_objects;
  if (p_tlist->begin() == p_tlist->end()) {
   // There is no <target> list for filtering.
   // Get all the mpi functions for all the threads.
    pmpi_objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
    mpi_objects = exp->FW()->getFunctionsByNamePattern ("MPI*");
    objects.insert(pmpi_objects.begin(), pmpi_objects.end());
    objects.insert(mpi_objects.begin(), mpi_objects.end());
  } else {
   // There is a list.  Is there a "-f" specifier?
    std::vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

    if ((f_list == NULL) || (f_list->empty())) {
     // There is no <file> list for filtering.
     // Get all the mpi functions for all, previously selected, threads.
      pmpi_objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
      mpi_objects = exp->FW()->getFunctionsByNamePattern ("MPI*");
      objects.insert(pmpi_objects.begin(), pmpi_objects.end());
      objects.insert(mpi_objects.begin(), mpi_objects.end());
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

static std::string allowed_mpiotf_V_options[] = {
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
  "SummaryOnly",
  ""
};

static bool define_mpiotf_columns (
            CommandObject *cmd,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {

  Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported View ");
  return (false);
}

static bool mpiotf_definition (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
  return false;
}

static std::string VIEW_mpiotf_brief = "Mpiotf Report";
static std::string VIEW_mpiotf_short = "Mpiotf Report is not supported in Open|SpeedShop, use Vampir-NG";
static std::string VIEW_mpiotf_long = "Mpiotf Report is not supported in Open|SpeedShop, use Vampir-NG";
static std::string VIEW_mpiotf_example = "\tPlease use Vampir-NG\n";
static std::string VIEW_mpiotf_metrics[] =
  { ""
  };
static std::string VIEW_mpiotf_collectors[] =
  { "mpiotf",
    ""
  };
class mpiotf_view : public ViewType {

 public: 
  mpiotf_view() : ViewType ("mpiotf",
                          VIEW_mpiotf_brief,
                          VIEW_mpiotf_short,
                          VIEW_mpiotf_long,
                          VIEW_mpiotf_example,
                         &VIEW_mpiotf_metrics[0],
                         &VIEW_mpiotf_collectors[0],
                          true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    
    Mark_Cmd_With_Message(cmd, "There is no view in Open|SpeedShop for this experiment, please use Vampir-NG");
    return true;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mpiotf_view_LTX_ViewFactory () {
  Define_New_View (new mpiotf_view());
}
