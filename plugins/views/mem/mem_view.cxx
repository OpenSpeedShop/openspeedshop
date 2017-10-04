/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2012-2014 Krell Institute. All Rights Reserved.
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
#include "MemCollector.hxx"
#include "MemDetail.hxx"
#include "MemTraceableFunctions.h"

uint64_t prev_max_bytesval=0;
uint64_t prev_min_bytesval=LONG_MAX;

/* Uncomment for debug traces
#define DEBUG_Mem 1
*/


// There are 2 reserved locations in the predefined-temporay table.
// Additional items may be defined for individual collectors.

// These are needed to manage mem collector data.
#define intime_temp VMulti_free_temp
#define incnt_temp VMulti_free_temp+1
#define extime_temp VMulti_free_temp+2
#define excnt_temp VMulti_free_temp+3
#define start_temp VMulti_free_temp+4
#define stop_temp VMulti_free_temp+5
#define min_temp VMulti_free_temp+6
#define max_temp VMulti_free_temp+7
#define ssq_temp VMulti_free_temp+8
#define memtype_temp  VMulti_free_temp+9
#define retval_temp  VMulti_free_temp+10
#define ptr_temp  VMulti_free_temp+11
#define size1_temp  VMulti_free_temp+12
#define size2_temp  VMulti_free_temp+13
#define id_temp  VMulti_free_temp+14
#define rank_temp  VMulti_free_temp+15
#define thread_temp  VMulti_free_temp+16
#define minbytes_temp  VMulti_free_temp+17
#define minbytescount_temp  VMulti_free_temp+18
#define maxbytes_temp  VMulti_free_temp+19
#define maxbytescount_temp  VMulti_free_temp+20
#define totbytes_temp  VMulti_free_temp+21
#define runningtot_temp  VMulti_free_temp+22
#define count_temp  VMulti_free_temp+23
#define max_alloc_temp  VMulti_free_temp+24
#define min_alloc_temp  VMulti_free_temp+25
#define total_alloc_temp  VMulti_free_temp+26
#define reason_temp  VMulti_free_temp+27

#define First_ByThread_Temp VMulti_free_temp+28
#define ByThread_use_intervals 1 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 // otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"

// this is based on the enum CBTF_mem_reason defined in the Mem_data.h
// (via Mem_data.x).If this enum is changed then these names must
// reflect those changes.
const char * reasons[] = {
	"Memory Leak",
	"Unique Call Path",
	"Duration of Allocation",
	"Max Allocation",
	"Min Allocation",
	"New Highwater",
	"Unique Allocation Address",
	"Unknown Reason"
};
// this is based on the enum CBTF_mem_type defined in the Mem_data.h
// (via Mem_data.x).If this enum is changed then these names must
// reflect those changes.
const char * memnames[] = {
	"unknown",
	"malloc",
	"calloc",
	"realloc",
	"free",
	"memalign",
	"posix_memalign"
};

// mem view

#define def_Mem_values \
            Time start = Time::TheEnd();         \
            Time end = Time::TheBeginning();     \
            double intime = 0.0;                 \
            uint64_t incnt = 0;                   \
            double extime = 0.0;                 \
            uint64_t excnt = 0;                   \
            double vmax = 0.0;                   \
            double vmin = LONG_MAX;              \
            double sum_squares = 0.0;            \
            uint64_t detail_memtype = 0;          \
            uint64_t detail_reason = 0;          \
            uint64_t detail_retval = 0;          \
            uint64_t min_bytesval = LONG_MAX;     \
            uint64_t min_bytesval_count = 0;      \
            uint64_t max_bytesval = 0;            \
            uint64_t max_bytesval_count = 0;      \
            uint64_t tot_bytesval = 0;            \
            uint64_t runningtot_val = 0;          \
            uint64_t detail_ptr = 0;             \
            uint64_t detail_size1 = 0;           \
            uint64_t detail_size2 = 0;           \
            uint64_t detail_count = 0;           \
            uint64_t detail_max_alloc = 0;           \
            uint64_t detail_min_alloc = 0;           \
            uint64_t detail_total_alloc = 0;           \
            std::string detail_id = "";          \
            int64_t detail_rank = 0;             \
            uint64_t detail_thread = 0;

#define get_Mem_invalues(primary,num_calls, function_name)       \
              double v = primary.dm_time / num_calls;            \
              intime += v;                                       \
              incnt++;                                           \
              start = std::min(start,primary.dm_interval.getBegin()); \
              end = std::max(end,primary.dm_interval.getEnd());       \
              vmin = std::min(vmin,v);                                \
              vmax = std::max(vmax,v);                                \
              if (primary.dm_memtype != CBTF_MEM_FREE) {              \
                if (primary.dm_size1 > 0 ) {                          \
		  uint64_t realsize = (uint64_t)primary.dm_size1; \
		  if (primary.dm_memtype == CBTF_MEM_CALLOC) {              \
		    realsize = (uint64_t)primary.dm_size1 * (uint64_t)primary.dm_size2; \
                  }                                                         \
                  prev_min_bytesval = min_bytesval ;                  \
                  min_bytesval = std::min(min_bytesval,realsize); \
                  if ( min_bytesval == prev_min_bytesval && realsize == min_bytesval ) {       \
                    min_bytesval_count = min_bytesval_count + 1;            \
                  } else if ( min_bytesval != prev_min_bytesval && realsize == min_bytesval ) {       \
                    min_bytesval_count = 1;                                 \
                  }                                                         \
                  prev_max_bytesval = max_bytesval ;                \
                  max_bytesval = std::max(max_bytesval,realsize); \
                  if ( max_bytesval == prev_max_bytesval && realsize == max_bytesval ) {                \
                    max_bytesval_count = max_bytesval_count + 1;            \
                  } else if ( max_bytesval != prev_max_bytesval && realsize == max_bytesval ) {       \
                    max_bytesval_count = 1;                                 \
                  }                                                         \
                  tot_bytesval += realsize;      \
                  runningtot_val += realsize;  \
                }                                                           \
              } else {                                                      \
                    min_bytesval = 0; \
                    min_bytesval_count = 0;                                 \
                    max_bytesval_count = 0;                                 \
              }                                                             \
              sum_squares += v * v;                     \
              detail_memtype = primary.dm_memtype;      \
              detail_reason = primary.dm_reason;      \
              detail_count += primary.dm_count;      \
              detail_total_alloc += primary.dm_total_allocation;      \
              detail_max_alloc = primary.dm_max;      \
              detail_min_alloc = primary.dm_min;      \
              detail_retval = primary.dm_retval;        \
              detail_ptr = primary.dm_ptr;		\
              detail_size1 = primary.dm_size1;		\
              detail_size2 = primary.dm_size2;          \
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
              if ( primary.dm_id.second >= 0 ) {                 \
                ss2 << primary.dm_id.second;                     \
                detail_thread = primary.dm_id.second;            \
              } else {                                           \
                ss2 << "";                                       \
                delim = "";                                      \
                detail_thread = 0;                               \
              }                                                  \
              detail_id = ss1.str() + delim + ss2.str();


#define get_Mem_exvalues(secondary,num_calls)           \
              extime += secondary.dm_time / num_calls; \
              excnt++;

#define get_inclusive_values(stdv, num_calls, function_name)           \
{           uint64_t len = stdv.size();                  \
            for (uint64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_Mem_invalues(stdv[i],num_calls, function_name)        \
            }                                           \
}

#define get_exclusive_values(stdv, num_calls)           \
{           uint64_t len = stdv.size();                  \
            for (uint64_t i = 0; i < len; i++) {         \
             /* Use macro to combine all the values. */ \
              get_Mem_exvalues(stdv[i],num_calls)        \
            }                                           \
}

#define set_Mem_values(value_array, sort_extime)                                          \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;     \
              if (num_temps > start_temp) {                                               \
                uint64_t x= (start.getValue() /*-base_time*/);                             \
                value_array[start_temp] = new CommandResult_Time (x);                     \
              }                                                                           \
              if (num_temps > stop_temp) {                                                \
                uint64_t x= (end.getValue() /*-base_time*/);                               \
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
              if (num_temps > memtype_temp) {						  \
		CommandResult * p = CRPTR (memnames[detail_memtype]);				  \
		p->SetValueIsID();							  \
		value_array[memtype_temp] = p;					          \
              }										  \
              if (num_temps > reason_temp) {						  \
		CommandResult * p = CRPTR (reasons[detail_reason]);				  \
		p->SetValueIsID();							  \
		value_array[reason_temp] = p;					          \
              }										  \
              if (num_temps > retval_temp) {						  \
	 	value_array[retval_temp] = new CommandResult_Address (detail_retval);     \
              }										  \
              if (num_temps > ptr_temp) {						  \
	 	value_array[ptr_temp] = new CommandResult_Address (detail_ptr);           \
              }										  \
              if (num_temps > size1_temp) {						  \
		CommandResult * p = CRPTR (detail_size1);				  \
		p->SetValueIsID();							  \
		value_array[size1_temp] = p;						  \
              }										  \
              if (num_temps > size2_temp) {						  \
		CommandResult * p = CRPTR (detail_size2);				  \
		p->SetValueIsID();							  \
		value_array[size2_temp] = p;						  \
              }										  \
              if (num_temps > count_temp) {						  \
		CommandResult * p = CRPTR (detail_count);				  \
		value_array[count_temp] = p;						  \
              }										  \
              if (num_temps > total_alloc_temp) {						  \
		CommandResult * p = CRPTR (detail_total_alloc);				  \
		value_array[total_alloc_temp] = p;						  \
              }										  \
              if (num_temps > max_alloc_temp) {						  \
		CommandResult * p = CRPTR (detail_max_alloc);				  \
		value_array[max_alloc_temp] = p;						  \
              }										  \
              if (num_temps > min_alloc_temp) {						  \
		CommandResult * p = CRPTR (detail_min_alloc);				  \
		value_array[min_alloc_temp] = p;						  \
              }										  \
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
              }                                                                           \
              if (num_temps > minbytes_temp) {                                            \
                CommandResult * p = CRPTR (min_bytesval);                                 \
                value_array[minbytes_temp] = p;                                           \
              }                                                                           \
              if (num_temps > maxbytescount_temp) {                                       \
                CommandResult * p = CRPTR (max_bytesval_count);                           \
                value_array[maxbytescount_temp] = p;                                      \
              }                                                                           \
              if (num_temps > maxbytes_temp) {                                            \
                CommandResult * p = CRPTR (max_bytesval);                                 \
                value_array[maxbytes_temp] = p;                                           \
              }                                                                           \
              if (num_temps > minbytescount_temp) {                                       \
                CommandResult * p = CRPTR (min_bytesval_count);                           \
                value_array[minbytescount_temp] = p;                                      \
              }                                                                           \
              if (num_temps > totbytes_temp) {                                            \
                CommandResult * p = CRPTR (tot_bytesval);                                 \
                value_array[totbytes_temp] = p;                                           \
              }                                                                           \
              if (num_temps > runningtot_temp) {                                          \
                CommandResult * p = CRPTR (runningtot_val);                               \
                value_array[runningtot_temp] = p;                                         \
              }

// The code here restricts any view for Functions (e.g. -v Functions)
// to the functions listed in MemTTraceablefunctions.h.  In this case,
// the memory functions are the only events with data. All other functions
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

    std::set<Function> mem_objects;

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
		if (lopath.find("libpthread") != std::string::npos ||
                    lopath.find("libc") != std::string::npos) {

#if 0
		    std::cerr << "Determine_Object MemT INSERT "
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
		    mem_objects.insert(*j);
		} else {
		    mem_objects.insert(*j);
		}
	    }
	}
    }

    objects = mem_objects;

  } else {
    std::vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

    if ((f_list == NULL) || (f_list->empty())) {

      std::set<Function> mem_objects;

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
		    std::cerr << "Determine_Object MemT INSERT " << f
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
		    mem_objects.insert(*j);
		}
	    }
	}
      }

    objects = mem_objects;

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

#define def_Detail_values def_Mem_values
#define get_inclusive_trace get_Mem_invalues
#define get_exclusive_trace get_Mem_exvalues
#define set_Detail_values set_Mem_values
#include "SS_View_detail.txx"

static std::string allowed_mem_V_options[] = {
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
  "Unique",
  "Leaked",
  "HighWater",
  ""
};

static bool define_mem_columns (
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
  IV.push_back(new ViewInstruction (VIEWINST_Min, minbytes_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, minbytescount_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, maxbytes_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, maxbytescount_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, totbytes_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, runningtot_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_alloc_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_alloc_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, total_alloc_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, count_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Summary_Max, intime_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  bool ViewLeaks = Look_For_KeyWord(cmd, "leaked");
  bool ViewHighwater = Look_For_KeyWord(cmd, "highwater");
  bool Generate_Summary = false;
  bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");
  if (!Generate_Summary_Only) {
     Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  }

  bool generate_nested_accounting = false;
  int64_t View_ByThread_Identifier = Determine_ByThread_Id (exp, cmd);
  std::string Default_Header = Find_Metadata ( CV[0], MV[1] ).getShortName();
  //std::string ByThread_Header = Find_Metadata ( CV[1], MV[1] ).getDescription();
  std::string ByThread_Header = Default_Header;

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
  MetricMap["memtype"] = memtype_temp;
  MetricMap["ptr"] = ptr_temp;
  MetricMap["size1"] = size1_temp;
  MetricMap["size2"] = size2_temp;
  MetricMap["retval"] = retval_temp;
  MetricMap["start_time"] = start_temp;
  MetricMap["min_bytescount"] = minbytescount_temp;
  MetricMap["min_bytes"] = minbytes_temp;
  MetricMap["max_bytescount"] = maxbytescount_temp;
  MetricMap["max_bytes"] = maxbytes_temp;
  MetricMap["tot_bytes"] = totbytes_temp;
  MetricMap["runningtot"] = runningtot_temp;
  MetricMap["allocation"] = total_alloc_temp;
  MetricMap["max_allocation"] = max_alloc_temp;
  MetricMap["min_allocation"] = min_alloc_temp;
  MetricMap["reason"] = reason_temp;
  MetricMap["path_counts"] = count_temp;

  // these are only valid for a trace view.
  if (vfc == VFC_Trace) {
    MetricMap["stop_time"] = stop_temp;
    MetricMap["id"] = id_temp;
    MetricMap["rank"] = rank_temp;
    MetricMap["thread"] = thread_temp;
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
        if (!strcasecmp(M_Name.c_str(), "mem")) {
         // We only know what to do with the mem collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'mem' view.");
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
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ) {
         // display times
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, intime_temp));
          HV.push_back(std::string("Inclusive ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "unique_inclusive_details") ||
		   !strcasecmp(M_Name.c_str(), "inclusive_details") ||
		   !strcasecmp(M_Name.c_str(), "inclusive_detail") ) {
         // display times
          //generate_nested_accounting = true;
	    // display a count of the calls to each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, count_temp));
	    HV.push_back("Number of Calls");
	    // display total return value
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
	    HV.push_back(std::string("Total Bytes Allocated") );
        } else if (!strcasecmp(M_Name.c_str(), "leaked_inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "leaked_inclusive_details")) {
         // display LEAKS DEFAULT
          generate_nested_accounting = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
          HV.push_back("Number of Leaks");
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
          HV.push_back(std::string("Total Bytes Leaked") );
        } else if (!strcasecmp(M_Name.c_str(), "highwater_inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "highwater_inclusive_details")) {
         // display HIGHWATER DEFAULT
          generate_nested_accounting = true;
	  // display a count of the calls to each function
	  IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
	  HV.push_back("Number of Calls");
	  // display total return value
	  IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
	  HV.push_back(std::string("Total Bytes Allocated") );
        } else if (!strcasecmp(M_Name.c_str(), "min")) {
         // display min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, min_temp));
          HV.push_back(std::string("Minimum ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // display max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, max_temp));
          HV.push_back(std::string("Maximum ") + Default_Header + "(ms)");
        } else if (!strcasecmp(M_Name.c_str(), "min_bytescount")) {
         // display the number of time the min allocation requested number of bytes was encountered
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, minbytescount_temp));
          HV.push_back(std::string("Min Request Count") );
        } else if (!strcasecmp(M_Name.c_str(), "min_bytes")) {
         // display the min allocation requested number of bytes
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, minbytes_temp));
          HV.push_back(std::string("Min Requested Bytes") );
        } else if (!strcasecmp(M_Name.c_str(), "max_bytescount")) {
         // display the number of time the max allocation requested number of bytes was encountered
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, maxbytescount_temp));
          HV.push_back(std::string("Max Request Count") );
        } else if (!strcasecmp(M_Name.c_str(), "max_bytes")) {
         // display the max allocation requested number of bytes
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, maxbytes_temp));
          HV.push_back(std::string("Max Requested Bytes") );
        } else if (!strcasecmp(M_Name.c_str(), "tot_bytes")) {
         // display total return value
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
          HV.push_back(std::string("Total Bytes Requested") );
        } else if (!strcasecmp(M_Name.c_str(), "runningtot")) {
         // display high water mark?
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, runningtot_temp));
          HV.push_back(std::string("High Water Mark") );
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
            //IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
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
            //IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
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
            //IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
            IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
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
           // WHOA! Why is this not allowed but %times above are???
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
        } else if (!strcasecmp(M_Name.c_str(), "memtype")) {

            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, memtype_temp));
            HV.push_back("Mem Call");

        } else if (!strcasecmp(M_Name.c_str(), "reason")) {

            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, reason_temp));
            HV.push_back("Reason");

        } else if (!strcasecmp(M_Name.c_str(), "retval")) {
          if (1 || vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, retval_temp));
            HV.push_back("Function Dependent Return Value");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m retval' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ptr")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, ptr_temp));
            HV.push_back("Ptr Arg");
 
        } else if (!strcasecmp(M_Name.c_str(), "size1")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, size1_temp));
            HV.push_back("Size Arg");
 
        } else if (!strcasecmp(M_Name.c_str(), "size2")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, size2_temp));
            HV.push_back("Size Arg");
 
        } else if (!strcasecmp(M_Name.c_str(), "max_allocation")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, max_alloc_temp));
            HV.push_back("Max Allocation");
 
        } else if (!strcasecmp(M_Name.c_str(), "min_allocation")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, min_alloc_temp));
            HV.push_back("Min Allocation");
 
        } else if (!strcasecmp(M_Name.c_str(), "allocation")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, total_alloc_temp));
            HV.push_back("Total Allocation");
 
        } else if (!strcasecmp(M_Name.c_str(), "path_counts")) {
 
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, count_temp));
            HV.push_back("Counts This Path");
 
        } else if ( (!strcasecmp(M_Name.c_str(), "threadid")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, thread_temp));
            HV.push_back("Thread");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m thread' only supported for '-v Trace' option.");
          }
        } else if ( (!strcasecmp(M_Name.c_str(), "rankid")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, rank_temp));
            HV.push_back("Rank");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m rank' only supported for '-v Trace' option.");
          }
        } else if ( (!strcasecmp(M_Name.c_str(), "id")) ) {

          if (vfc == VFC_Trace) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, id_temp));
            HV.push_back("Id");
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m id' only supported for '-v Trace' option.");
          }

        } else if (!strcasecmp(M_Name.c_str(), "absdiff")) {
        // Ignore this because cview -c 3 -c 5 -mtime,absdiff actually works outside of this view code
        // Mark_Cmd_With_Soft_Error(cmd,"AbsDiff option, '-m " + M_Name + "'");
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
	// display id of event for each function call
	IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, id_temp));
	HV.push_back("Event Ids");
	if (ViewHighwater || ViewLeaks) {
	    // display function size 1 value for each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, size1_temp));
	    HV.push_back("Size Arg1");
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, size2_temp));
	    HV.push_back("Size Arg2");
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, ptr_temp));
	    HV.push_back("Ptr Arg");
	    // display function return values for each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, retval_temp));
	    HV.push_back("Return Value");
	}
	if (!ViewHighwater && !ViewLeaks) {
	    // display a count of the calls to each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, count_temp));
	    HV.push_back("Number of Calls");
	    // display total return value
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, total_alloc_temp));
	    HV.push_back(std::string("Bytes Allocated") );
	}
	if (ViewHighwater) {
	    // display total return value
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, total_alloc_temp));
	    HV.push_back(std::string("New Highwater") );
	}
    }

    if (vfc != VFC_Trace) {
	if (ViewLeaks) {
	    // display a count of the calls to each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
	    HV.push_back("Number of Leaks");
	} else if (ViewHighwater) {
	    // display a count of the calls to each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
	    HV.push_back("Number of Calls");
	} else {
	    // Always display elapsed time.
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, extime_temp));
	    // removing Default_Header from display since all metrics are compured via inclusive_details.
	    HV.push_back(std::string("Exclusive ") + "(ms)");

	    // and include % of exclusive time
	    // filter while view type is VFC_Function should not show percent anything.
	    // FIXME: This makes no sense if we are in vfc == VFC_Trace!
	    if (Filter_Uses_F(cmd) && vfc == VFC_Function) {
	    // Use the metric needed for calculating total time.
	    //IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Metric, totalIndex, 1));
	    //IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
	    } else {
	    // Sum the extime_temp values.
	    IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, extime_temp));
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, extime_temp, totalIndex++));
	    HV.push_back("% of Total Time");
	    }

	    // display a count of the calls to each function
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, count_temp));
	    HV.push_back("Number of Calls");
	}
    }

    if (vfc == VFC_Function) {
	if (ViewLeaks) {
	    // display total leaked value
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
	    HV.push_back(std::string("Total Bytes Leaked") );
	} else if (ViewHighwater) {
	    // display total return value
	    //IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, total_alloc_temp));
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
	    HV.push_back(std::string("Total Bytes Allocated") );
	} else {
	    // display the number of time the min allocation requested number of bytes was encountered
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, minbytescount_temp));
	    HV.push_back(std::string("Min Request Count") );
	    // display the min allocation requested number of bytes
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, minbytes_temp));
	    HV.push_back(std::string("Min Requested Bytes") );
	    // display the number of time the max allocation requested number of bytes was encountered
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, maxbytescount_temp));
	    HV.push_back(std::string("Max Request Count") );
	    // display the max allocation requested number of bytes
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, maxbytes_temp));
	    HV.push_back(std::string("Max Requested Bytes") );
	    // display total return value
	    //IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, totbytes_temp));
	    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, total_alloc_temp));
	    HV.push_back(std::string("Total Bytes Requested") );
	}
    }

  } // end if nothing requested

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

static bool mem_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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

    // Warn about misspelled of meaningless options and exit command processing without generating a view.
    bool all_valid = Validate_V_Options (cmd, allowed_mem_V_options);
    if ( all_valid == false ) {
      return false;
    }

    return define_mem_columns (cmd, exp, CV, MV, IV, HV, vfc);
}


static std::string VIEW_mem_brief = "Memory Tracing (mem) Report";
static std::string VIEW_mem_short = "Report general statictics on all unique callpaths to the "
				    "traced memory calls.  Report on potential leaked memory "
				    "allocations.  Report timeline of all allocations that set "
				    "a new highwater memory mark."
					;
static std::string VIEW_mem_long  =
                  "\nA positive integer can be added to the end of the keyword"
                  " 'mem' to indicate the maximum number of items in the report."
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
                  "\n\t'-v Trace' will produce a report of each individual  call to an memory"
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
                  " \n\t'-m percent' reports the percent of exclusive time the function represents."
                  " \n\t'-m stddev' reports the standard deviation of the average exclusive time"
                  " that the function represents."
                  " The available '-v trace -m' options are:"
                  " \n\t'-m memtype' reports the memory call enum associated  with the function."
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
                  "\n"
                  " \nThe mem views also include three key metrics specific to the memory experiment."
                  " \nThe list of specific memory views are as follows:"
                  "\n"
                  "The view names and combination views that can be used and the description of the information displayed in the view follow:"
                  " \n\t'-v unique' Show times, call counts per path, min,max bytes allocation, total allocation "
                  " \n\t to all unique paths to memory calls that the mem collector saw."
                   "\n                                    "
                  " \n\t'-v leaked' Show function view of allocations that were not released while the mem collector was active."
                   "\n                                    "
                  " \n\t'-v leaked,trace' Will show a timeline of any allocation calls that were not released."
                   "\n                                    "
                  " \n\t'-v leaked,fullstack' Display a full callpath to each unique leaked allocation."
                   "\n                                    "
                  " \n\t'-m highwater,trace' Is a timeline of mem calls that set a new high-water"
                  " \n\t The last entry is the allocation call that the set the high-water for the complete run"
                  " \n\t Investigate the last calls in the timeline and look at allocations that have the largest"
                  " \n\t allocation size (size1,size2,etc) if your application is consuming lots of system ram."
                  "\n" ;

static std::string VIEW_mem_example = "\texpView mem\n"
                                      "\texpView -vtrace,unique\n"
                                      "\texpView -vleaked\n"
                                      "\texpView -vtrace,leaked\n"
                                      "\texpView -vtrace,fullstack,leaked > leakedTraceWFullstacks.txt\n"
                                      "\texpView -vhighwater\n"
                                      "\texpView -vtrace,highwater\n"
					;
static std::string VIEW_mem_metrics[] =
  { "time",
    "inclusive_times",
    "inclusive_details",
    "unique_inclusive_details",
    "leaked_inclusive_details",
    "highwater_inclusive_details",
    "exclusive_times",
    "exclusive_details",
    ""
  };
static std::string VIEW_mem_collectors[] =
  { "mem",
    ""
  };
class mem_view : public ViewType {

 public: 
  mem_view() : ViewType ("mem",
                         VIEW_mem_brief,
                         VIEW_mem_short,
                         VIEW_mem_long,
                         VIEW_mem_example,
                        &VIEW_mem_metrics[0],
                        &VIEW_mem_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

   bool use_highwater = false;
   bool use_leaked = false;
   bool use_unique = false;
   OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
   std::vector<ParseRange> *p_slist = p_result->getexpMetricList();
   if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    std::vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (!strcasecmp(M_Name.c_str(), "mem")) {
         // We only know what to do with the mem collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'mem' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          continue;
        }
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

      if (M_Name.length() > 0) {
        if (!strcasecmp(M_Name.c_str(), "leaked_inclusive_detail") ||
	    !strcasecmp(M_Name.c_str(), "leaked_inclusive_details")) {
	    use_leaked = true;
        } else if (!strcasecmp(M_Name.c_str(), "highwater_inclusive_detail") ||
                   !strcasecmp(M_Name.c_str(), "highwater_inclusive_details")) {
	    use_highwater = true;
	} else {
	    use_unique = true;
	}
      }
    }
   }

   CV.push_back (Get_Collector (exp->FW(), "mem"));  // Define the collector
   if (Look_For_KeyWord(cmd, "Unique") || use_unique) {
	MV.push_back ("unique_inclusive_details"); // define the basic mem metric needed
   } else if (Look_For_KeyWord(cmd, "HighWater") || use_highwater) {
	MV.push_back ("highwater_inclusive_details"); // define the basic mem metric needed
   } else if (Look_For_KeyWord(cmd, "Leaked") || use_leaked) {
	MV.push_back ("leaked_inclusive_details"); // define the basic mem metric needed
   } else {
	// this default to unique callpath events.
	MV.push_back ("inclusive_details"); // define the basic mem metric needed
   }

    CV.push_back (Get_Collector (exp->FW(), "mem"));  // Define the collector
    MV.push_back ("time"); // define the metric needed for calculating total time.

    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (mem_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      std::vector<MemDetail> dummyVector;
      MemDetail *dummyDetail;
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
                                   Determine_Metric_Ordering(IV), dummyObject,
                                   VFC_Function, &dummyVector, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(There is no supported view name recognized.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report for 'mem' view.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mem_view_LTX_ViewFactory () {
  Define_New_View (new mem_view());
}

/*
 * NOTES on viewing reduced data
 *
 * Default view is for all unique callpaths to memory calls.
 * timeline of all events with all arguments.
 * expview -vtrace -mstart_time -m time -m id -msize1,size2,ptr,retval
 * timeline of all event calltrees with all arguments.
 * expview -vtrace,calltrees -mstart_time -m time -m id -msize1,size2,ptr,retval
 *
 * timeline of all leak events with all arguments.
 * expview -vtrace,leaked -mstart_time -m time -m id -msize1,size2,ptr,retval
 * timeline of all leak event calltrees with all arguments.
 * expview -vtrace,calltrees,leaked -mstart_time -m time -m id -msize1,size2,ptr,retval
 *
 * timeline of all highwater events with all arguments.
 * expview -vtrace,highwater -mstart_time,allocation,time,id,size1,size2,ptr,retval
 * timeline of all highwater event calltrees with all arguments.
 * expview -vtrace,calltrees,highwater -mstart_time,allocation,time,id,size1,size2,ptr,retval
 */
