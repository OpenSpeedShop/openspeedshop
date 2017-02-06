/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2012-14 Krell Institute. All Rights Reserved.
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
#include "PthreadsCollector.hxx"
#include "PthreadsDetail.hxx"
#include "PthreadsTraceableFunctions.h"

/* Uncomment for debug traces
#define DEBUG_Pthreads 1
*/


// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage pthreads collector data.
#define intime_temp VMulti_free_temp
#define incnt_temp VMulti_free_temp+1
#define extime_temp VMulti_free_temp+2
#define excnt_temp VMulti_free_temp+3
#define start_temp VMulti_free_temp+4
#define stop_temp VMulti_free_temp+5
#define min_temp VMulti_free_temp+6
#define max_temp VMulti_free_temp+7
#define ssq_temp VMulti_free_temp+8
#define pthreadtype_temp  VMulti_free_temp+9
#define retval_temp  VMulti_free_temp+10
#define ptr1_temp  VMulti_free_temp+11
#define ptr2_temp  VMulti_free_temp+12
#define ptr3_temp  VMulti_free_temp+13
#define id_temp  VMulti_free_temp+14
#define rank_temp  VMulti_free_temp+15
#define thread_temp  VMulti_free_temp+16


#define First_ByThread_Temp VMulti_free_temp+17
#define ByThread_use_intervals 1 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 // otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"


// pthreads view

#define def_Pthreads_values \
            Time start = Time::TheEnd();      \
            Time end = Time::TheBeginning();  \
            double intime = 0.0;              \
            int64_t incnt = 0;                \
            double extime = 0.0;              \
            int64_t excnt = 0;                \
            double vmax = 0.0;                \
            double vmin = LONG_MAX;           \
            double sum_squares = 0.0;         \
            int64_t detail_pthreadtype = 0;   \
            uint64_t detail_retval = 0;       \
            uint64_t detail_ptr1 = 0;         \
            uint64_t detail_ptr2 = 0;         \
            uint64_t detail_ptr3 = 0;         \  
            std::string detail_id = "";       \
            int64_t detail_rank = 0;          \
            int64_t detail_thread = 0;


#define get_Pthreads_invalues(primary,num_calls, function_name)  \
              double v = primary.dm_time / num_calls;            \
              intime += v;                                       \
              incnt++;                                           \
              start = std::min(start,primary.dm_interval.getBegin()); \
              end = std::max(end,primary.dm_interval.getEnd());       \
              vmin = std::min(vmin,v);                                \
              vmax = std::max(vmax,v);                            \
              sum_squares += v * v;                               \
              detail_pthreadtype = primary.dm_pthreadtype;        \
              detail_retval = primary.dm_retval;                  \
              detail_ptr1 = primary.dm_ptr1;		          \
              detail_ptr2 = primary.dm_ptr2;		          \
              detail_ptr3 = primary.dm_ptr3;                      \
               std::stringstream ss1;                             \
               std::stringstream ss2;                             \
               std::string delim = ":";                           \
               if ( primary.dm_id.first != -1 ) {                 \
                 ss1 << primary.dm_id.first;                      \
                 detail_rank = primary.dm_id.first;               \
               } else {                                           \
                 ss1 << "";                                       \
                 delim = "";                                      \
                 detail_rank = -1;                                \
               }                                                  \
               if ( primary.dm_id.second != 0 ) {                 \
                 ss2 << primary.dm_id.second;                     \
                 detail_thread = primary.dm_id.second;            \
               } else {                                           \
                 ss2 << "";                                       \
                 delim = "";                                      \
                 detail_thread = 0;                               \
               }                                                  \
               detail_id = ss1.str() + delim + ss2.str();


#define get_Pthreads_exvalues(secondary,num_calls)           \
              extime += secondary.dm_time / num_calls; \
              excnt++;

#define get_inclusive_values(stdv, num_calls, function_name)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_Pthreads_invalues(stdv[i],num_calls, function_name)        \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           int64_t len = stdv.size();                  \
            for (int64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_Pthreads_exvalues(stdv[i],num_calls)        \
            }                                           \
}

#define set_Pthreads_values(value_array, sort_extime)                                           \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;     \
              if (num_temps > start_temp) {                                               \
                int64_t x= (start.getValue() /*-base_time*/);                             \
                value_array[start_temp] = new CommandResult_Time (x);                     \
              }                                                                          \
              if (num_temps > stop_temp) {                                               \
                int64_t x= (end.getValue() /*-base_time*/);                               \
                value_array[stop_temp] = new CommandResult_Time (x);                      \
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
                            = new CommandResult_Interval (sum_squares);			  \
              if (num_temps > pthreadtype_temp) {						  \
		CommandResult * p = CRPTR (detail_pthreadtype);				  \
		p->SetValueIsID();							  \
		value_array[pthreadtype_temp] = p;					  \
              }										  \
              if (num_temps > retval_temp) {						  \
		CommandResult * p = CRPTR (detail_retval);				  \
		p->SetValueIsID();							  \
		value_array[retval_temp] = p;						  \
              }										  \
              if (num_temps > ptr1_temp) {						  \
		CommandResult * p = CRPTR (detail_ptr1);				  \
		p->SetValueIsID();							  \
		value_array[ptr1_temp] = p;						  \
              }										  \
              if (num_temps > ptr2_temp) {						  \
		CommandResult * p = CRPTR (detail_ptr2);				  \
		p->SetValueIsID();							  \
		value_array[ptr2_temp] = p;						  \
              }										  \
              if (num_temps > ptr3_temp) {						  \
		CommandResult * p = CRPTR (detail_ptr3);				  \
		p->SetValueIsID();							  \
		value_array[ptr3_temp] = p;						  \
              }                                                                           \
              if (num_temps > id_temp) {                                                  \
                CommandResult * p = CRPTR (detail_id);                                    \
                p->SetValueIsID();                                                        \
                value_array[id_temp] = p;                                                 \
              }                                                                           \
              if (num_temps > rank_temp) {                                                \
                CommandResult * p = CRPTR (detail_rank);                                  \
                p->SetValueIsID();                                                        \
                value_array[rank_temp] = p;                                               \
              }                                                                           \
              if (num_temps > thread_temp) {                                              \
                CommandResult * p = CRPTR (detail_thread);                                \
                p->SetValueIsID();                                                        \
                value_array[thread_temp] = p;                                             \
              }



// The code here restricts any view for Functions (e.g. -v Functions)
// to the functions listed in PthreadsTTraceablefunctions.h.  In this case,
// the pthread functions are the only events with data. All other functions
// normally returned are just members of the callstacks and are displayed
// by the various views that use the StackTrace details.
static void Determine_Objects (
               CommandObject *cmd,
               ExperimentObject *exp,
               ThreadGroup& tgrp,
               std::set<Function>& objects) {

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  if (p_tlist->begin() == p_tlist->end()) {

    std::set<Function> pthreads_objects;

    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
	std::string match = "*";
	std::string f = match + TraceableFunctions[i] + match;
	objects = exp->FW()->getFunctionsByNamePattern (f);
	if (objects.size() > 0 ) {
	    for (std::set<Function>::const_iterator j = objects.begin();
		 j != objects.end(); ++j) {
		LinkedObject lo = (*j).getLinkedObject();
		std::string lopath = lo.getPath();
		std::set<AddressRange> lor = lo.getAddressRange();
		std::set<Thread> lot = lo.getThreads();
		if (1) {

#if 0
		    std::cerr << "Determine_Object PthreadsT INSERT "
			<< " objname " << (*j).getName()
			<< " FROM LO " << lo.getPath() << std::endl;

		    for (std::set<AddressRange>::const_iterator k = lor.begin();
			 k != lor.end(); ++k) {
			std::cerr << " @ " << (*k) << std::endl;
		    }

		    for (std::set<Thread>::const_iterator l = lot.begin();
			 l != lot.end(); ++l) {
			std::cerr << " PID " << (*l).getProcessId() << std::endl;
			ExtentGroup eg = lo.getExtentIn(*l);
			Extent bounds = eg.getBounds();
			std::cerr << "  Bound AR: " << bounds.getAddressRange() << std::endl;
			std::cerr << "  Bound TI: " << bounds.getTimeInterval() << std::endl;

			for(std::vector<Extent>::const_iterator m = eg.begin();
			    m != eg.end(); ++m) {
			    std::cerr << "  E AR: " << (*m).getAddressRange() << std::endl;
                            std::cerr << "  E TI: " << (*m).getTimeInterval() << std::endl;
			}
		    }
#endif
		    pthreads_objects.insert(*j);
		} else {
		    pthreads_objects.insert(*j);
		}
	    }
	}
    }

    objects = pthreads_objects;

  } else {
    std::vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

    if ((f_list == NULL) || (f_list->empty())) {

      std::set<Function> pthreads_objects;

      for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {

	std::string match = "*";
	std::string f = match + TraceableFunctions[i] + match;
	objects = exp->FW()->getFunctionsByNamePattern (f);

	if (objects.size() > 0 ) {

	    for (std::set<Function>::const_iterator j = objects.begin();
		 j != objects.end(); ++j) {

		LinkedObject lo = (*j).getLinkedObject();
		std::string lopath = lo.getPath();
		std::set<AddressRange> lor = lo.getAddressRange();
		std::set<Thread> lot = lo.getThreads();

		if (1) {
#if 0
		    std::cerr << "Determine_Object PthreadsT INSERT " << f
			<< " FROM LO " << lo.getPath() << std::endl;

		    for (std::set<AddressRange>::const_iterator k = lor.begin();
			 k != lor.end(); ++k) {
			std::cerr << " @ " << (*k) << std::endl;
		    }

		    for (std::set<Thread>::const_iterator l = lot.begin();
			 l != lot.end(); ++l) {
			std::cerr << " PID " << (*l).getProcessId() << std::endl;
			ExtentGroup eg = lo.getExtentIn(*l);
			Extent bounds = eg.getBounds();
			std::cerr << "  Bound AR: " << bounds.getAddressRange() << std::endl;
			std::cerr << "  Bound TI: " << bounds.getTimeInterval() << std::endl;

			for(std::vector<Extent>::const_iterator m = eg.begin();
			    m != eg.end(); ++m) {
			    std::cerr << "  E AR: " << (*m).getAddressRange() << std::endl;
                        std::cerr << "  E TI: " << (*m).getTimeInterval() << std::endl;
			}
		    }
#endif
		    pthreads_objects.insert(*j);
		}
	    }
	}
      }

    objects = pthreads_objects;

    } else {
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

#define def_Detail_values def_Pthreads_values
#define get_inclusive_trace get_Pthreads_invalues
#define get_exclusive_trace get_Pthreads_exvalues
#define set_Detail_values set_Pthreads_values
#include "SS_View_detail.txx"

static std::string allowed_pthreads_V_options[] = {
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

static bool define_pthreads_columns (
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
  IV.push_back(new ViewInstruction (VIEWINST_Add, intime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, incnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, extime_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, stop_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, ssq_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, id_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, rank_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, thread_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, intime_temp));

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
  std::string Default_Header = Find_Metadata ( CV[0], MV[1] ).getShortName();
  std::string ByThread_Header = Find_Metadata ( CV[1], MV[1] ).getDescription();


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
  MetricMap["time"] = extime_temp;
  MetricMap["times"] = extime_temp;
  MetricMap["count"] = excnt_temp;
  MetricMap["counts"] = excnt_temp;
  MetricMap["call"] = excnt_temp;
  MetricMap["calls"] = excnt_temp;
  MetricMap["exclusive_time"] = extime_temp;
  MetricMap["exclusive_times"] = extime_temp;
  MetricMap["exclusive_detail"] = extime_temp;
  MetricMap["exclusive_details"] = extime_temp;
  MetricMap["exclusive_count"] = excnt_temp;
  MetricMap["exclusive_counts"] = excnt_temp;
  MetricMap["inclusive_time"] = intime_temp;
  MetricMap["inclusive_times"] = intime_temp;
  MetricMap["inclusive_detail"] = intime_temp;
  MetricMap["inclusive_details"] = intime_temp;
  MetricMap["inclusive_count"] = incnt_temp;
  MetricMap["inclusive_counts"] = incnt_temp;
  MetricMap["minimum"] = min_temp;
  MetricMap["min"] = min_temp;
  MetricMap["maximum"] = max_temp;
  MetricMap["max"] = max_temp;
  MetricMap["pthreadtype"] = pthreadtype_temp;
  MetricMap["ptr1"] = ptr1_temp;
  MetricMap["ptr2"] = ptr2_temp;
  MetricMap["ptr3"] = ptr3_temp;
  MetricMap["retval"] = retval_temp;
  if (vfc == VFC_Trace) {
    MetricMap["start_time"] = start_temp;
    MetricMap["stop_time"] = stop_temp;
    MetricMap["id"] = id_temp;
    MetricMap["rank"] = rank_temp;
    MetricMap["thread"] = thread_temp;
    //MetricMap["retval"] = retval_temp;
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    bool time_metric_selected = false;
    std::vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {

// Look for a metric expression and invoke processing.
#include "SS_View_metric_expressions.hxx"

      bool column_is_DateTime = false;
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "pthreads")) {
         // We only know what to do with the pthreads collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'pthreads' view.");
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
          generate_nested_accounting = true;
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
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, incnt_temp));
          HV.push_back("Inclusive Calls");
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column++, VMulti_time_temp, incnt_temp));
          HV.push_back("Average Time(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                   !strcasecmp(M_Name.c_str(), "%") ||
                   !strcasecmp(M_Name.c_str(), "%time") ||
                   !strcasecmp(M_Name.c_str(), "%times")) {
         // percent is calculate from 2 temps: time for this row and total time.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total time.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, VMulti_time_temp, totalIndex++));
          HV.push_back("% of Total");
        } else if (!strcasecmp(M_Name.c_str(), "%exclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_times")) {
         // percent is calculate from 2 temps: time for this row and total time.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total time.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp, totalIndex++));
          HV.push_back("% of Total");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_time") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_times")) {
         // percent is calculate from 2 temps: number of counts for this row and total inclusive counts.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // Use the metric needed for calculating total time.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
          } else {
           // Sum the extime_temp values.
            generate_nested_accounting = true;
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp, totalIndex++));
          HV.push_back("% of Total Inclusive Counts");
        } else if (!strcasecmp(M_Name.c_str(), "%count") ||
                   !strcasecmp(M_Name.c_str(), "%counts") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%exclusive_counts")) {
         // percent is calculate from 2 temps: counts for this row and total counts.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m exclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the extime_temp values.
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
          HV.push_back("% of Total Counts");
        } else if (!strcasecmp(M_Name.c_str(), "%inclusive_count") ||
                   !strcasecmp(M_Name.c_str(), "%inclusive_counts")) {
         // percent is calculate from 2 temps: number of counts for this row and total inclusive counts.
          if (!Generate_ButterFly && Filter_Uses_F(cmd)) {
           // There is no metric available for calculating total counts.
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m inclusive_counts' is not supported with '-f' option.");
            continue;
          } else {
           // Sum the extime_temp values.
            generate_nested_accounting = true;
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
          }
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, incnt_temp, totalIndex++));
          HV.push_back("% of Total Inclusive Counts");
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
        } else if (!strcasecmp(M_Name.c_str(), "pthreadtype")) {

            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, pthreadtype_temp));
            HV.push_back("PthreadType Enum");

        } else if (!strcasecmp(M_Name.c_str(), "retval")) {
          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, retval_temp));
            HV.push_back("Function Dependent Return Value");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m retval' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ptr1")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, ptr1_temp));
            HV.push_back("Ptr 1 Arg");
 
        } else if (!strcasecmp(M_Name.c_str(), "ptr2")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, ptr2_temp));
            HV.push_back("Ptr 2 Arg");
 
        } else if (!strcasecmp(M_Name.c_str(), "ptr3")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, ptr3_temp));
            HV.push_back("Ptr 3 Arg");
 
        } else if ( (!strcasecmp(M_Name.c_str(), "threadid")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, thread_temp));
            HV.push_back("Event Identifier(s)");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m thread' only supported for '-v Trace' option.");
          }
        } else if ( (!strcasecmp(M_Name.c_str(), "rankid")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, rank_temp));
            HV.push_back("Event Identifier(s)");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m rank' only supported for '-v Trace' option.");
          }
        } else if ( (!strcasecmp(M_Name.c_str(), "id")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, id_temp));
            HV.push_back("Event Identifier(s)");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m id' only supported for '-v Trace' option.");
          }

        } else if (!strcasecmp(M_Name.c_str(), "absdiff")) {
        }
// Recognize and generate pseudo instructions to calculate and display By Thread metrics for
// ThreadMax, ThreadMaxIndex, ThreadMin, ThreadMinIndex, ThreadAverage and loadbalance.
#include "SS_View_bythread_recognize.hxx"
        else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
          return false;
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
    HV.push_back("Inclusive Time");

  // Column[1] in % of inclusive time
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, intime_temp, totalIndex++));
    HV.push_back("% of Total");
  } else {
   // If nothing is requested ...
    if (vfc == VFC_Trace) {
      // Insert start and end times into report.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, start_temp));
      HV.push_back("Start Time(d:h:m:s)");
      IV.push_back(new ViewInstruction (VIEWINST_Sort_Ascending, 1)); // final report in ascending time order
    }
   // Always display elapsed time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
    HV.push_back(std::string("Exclusive ") + Default_Header + "(ms)");

  // and include % of exclusive time
    if (Filter_Uses_F(cmd)) {
     // Use the metric needed for calculating total time.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
    } else {
     // Sum the extime_temp values.
      IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
    }
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp, totalIndex++));
    HV.push_back("% of Total");
  // display a count of the calls to each function
    if (vfc != VFC_Trace) {
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
      HV.push_back("Number of Calls");
    } else {
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, id_temp));
      HV.push_back("Event Identifier(s)");
    }
  }
  if (generate_nested_accounting) {
    IV.push_back(new ViewInstruction (VIEWINST_StackExpand, intime_temp));
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

static bool pthreads_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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

    // Warn about misspelled of meaningless options and exit command processing without generating a view.
    bool all_valid = Validate_V_Options (cmd, allowed_pthreads_V_options);
    if ( all_valid == false ) {
      return false;
    }

    return define_pthreads_columns (cmd, exp, CV, MV, IV, HV, vfc);
}


static std::string VIEW_pthreads_brief = "Posix Thread (pthreads) Report";
static std::string VIEW_pthreads_short = "Report the time spent in each Posix thread function.";
static std::string VIEW_pthreads_long  =
                  "\nA positive integer can be added to the end of the keyword"
                  " 'pthreads' to indicate the maximum number of items in the report."
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
                  "\n\t'-v Trace' will produce a report of each individual  call to an pthread"
                  " function."
                  " It will be sorted in ascending order of the starting time for the event."
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
                  " Each value pertains to the function, statement or linked object that is"
                  " on that row of the report.  The 'Thread...' selections pertain to the"
                  " process unit that the program was partitioned into: Pid's,"
                  " Posix threads, Mpi threads or Ranks."
                  " If no '-m' option is specified, the default is equivalent to"
                  " '-m exclusive times, percent, count'."
                  " The available '-m' options are:"
                  " \n\t'-m exclusive_times' reports the wall clock time used in the function."
                  " \n\t'-m min' reports the minimum time spent in the function."
                  " \n\t'-m max' reports the maximum time spent in the function."
                  " \n\t'-m average' reports the average time spent in the function."
                  " \n\t'-m count' reports the number of times the function was called."
                  " \n\t'-m percent' reports the percent of exclusive time the pthread call represents."
                  " \n\t'-m stddev' reports the standard deviation of the average exclusive time"
                  " that the pthread call represents."
                  " The available '-v trace -m' options are:"
                  " \n\t'-m pthreadtype' reports the pthread call enum associated  with the function."
                  " \n\t'-m retval' reports the value returned from the call."
                  " \n\t'-m ptr' reports any ptr argument to the call."
                  " \n\t'-m size1' reports  a size argument to the function."
                  " \n\t'-m size2' reports  a second size argument to the function."
                  " \n\t'-m start_time' reports the time the event started."
                  " \n\t'-m stop_time' reports the time of the event ended."
                  " \n\t'-m id' reports the rank/pid thread pair of the event."
                  " \n\t'-m rankid' reports the rank number, or if rank not available then the process id of the event."
                  " \n\t'-m threadid' reports the thread number of the event."


// Get the description of the BY-Thread metrics.
#include "SS_View_bythread_help.hxx"
                  "\n"; 
static std::string VIEW_pthreads_example = "\texpView pthreads\n"
                                      "\texpView -v CallTrees,FullStack pthreads10 -m min,max,count\n";
static std::string VIEW_pthreads_metrics[] =
  { "time",
    "inclusive_times",
    "inclusive_details",
    "exclusive_times",
    "exclusive_details",
    ""
  };
static std::string VIEW_pthreads_collectors[] =
  { "pthreads",
    ""
  };
class pthreads_view : public ViewType {

 public: 
  pthreads_view() : ViewType ("pthreads",
                         VIEW_pthreads_brief,
                         VIEW_pthreads_short,
                         VIEW_pthreads_long,
                         VIEW_pthreads_example,
                        &VIEW_pthreads_metrics[0],
                        &VIEW_pthreads_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "pthreads"));  // Define the collector
    MV.push_back ("inclusive_details"); // define the metric needed for getting main time values
    CV.push_back (Get_Collector (exp->FW(), "pthreads"));  // Define the collector
    MV.push_back ("time"); // define the metric needed for calculating total time.

    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (pthreads_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<PthreadsDetail> dummyVector;
      switch (Determine_Form_Category(cmd)) {
       case VFC_Trace:
        PthreadsDetail *dummyDetail;
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
                                   Determine_Metric_Ordering(IV), dummyObject,
                                   VFC_Function, &dummyVector, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name recognized.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report for 'pthreads' view.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void pthreads_view_LTX_ViewFactory () {
  Define_New_View (new pthreads_view());
}
