////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////



#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#include "SS_Timings.hxx"

static const struct {

        /** Performance data event code. */
        SS_Timings::CLIPerformanceDataEvents count_cli_event;
        SS_Timings::CLIPerformanceDataEvents max_cli_event;
        SS_Timings::CLIPerformanceDataEvents min_cli_event;
        SS_Timings::CLIPerformanceDataEvents total_cli_event;


    } MaxMinTable[] = {
        { 
          SS_Timings::expAttachCount, 
          SS_Timings::expAttachMax, 
          SS_Timings::expAttachMin, 
          SS_Timings::expAttachTotal },
        { 
          SS_Timings::expAttach_resolveTargetList_Count,
          SS_Timings::expAttach_resolveTargetList_Max,
          SS_Timings::expAttach_resolveTargetList_Min,
          SS_Timings::expAttach_resolveTargetList_Total },
        { 
          SS_Timings::expAttach_getCollector_Count, 
          SS_Timings::expAttach_getCollector_Max,
          SS_Timings::expAttach_getCollector_Min,
          SS_Timings::expAttach_getCollector_Total },
        { 
          SS_Timings::expAttach_linkThreads_Count, 
          SS_Timings::expAttach_linkThreads_Max, 
          SS_Timings::expAttach_linkThreads_Min,
          SS_Timings::expAttach_linkThreads_Total},
        { 
          SS_Timings::expAttach_FW_createProcess_Count, 
          SS_Timings::expAttach_FW_createProcess_Max, 
          SS_Timings::expAttach_FW_createProcess_Min,
          SS_Timings::expAttach_FW_createProcess_Total},
        { 
          SS_Timings::expCreateCount, 
          SS_Timings::expCreateMax, 
          SS_Timings::expCreateMin,
          SS_Timings::expCreateTotal},
        { 
          SS_Timings::expCreate_wait_Count, 
          SS_Timings::expCreate_wait_Max, 
          SS_Timings::expCreate_wait_Min,
          SS_Timings::expCreate_wait_Total},
        { 
          SS_Timings::expCreate_allocExp_Count, 
          SS_Timings::expCreate_allocExp_Max, 
          SS_Timings::expCreate_allocExp_Min,
          SS_Timings::expCreate_allocExp_Total},
        { 
          SS_Timings::expCreate_resolveTargetList_Count, 
          SS_Timings::expCreate_resolveTargetList_Max, 
          SS_Timings::expCreate_resolveTargetList_Min,
          SS_Timings::expCreate_resolveTargetList_Total},
        { 
          SS_Timings::expCreate_getCollector_Count, 
          SS_Timings::expCreate_getCollector_Max, 
          SS_Timings::expCreate_getCollector_Min,
          SS_Timings::expCreate_getCollector_Total},
        { 
          SS_Timings::expCreate_linkThreads_Count, 
          SS_Timings::expCreate_linkThreads_Max, 
          SS_Timings::expCreate_linkThreads_Min,
          SS_Timings::expCreate_linkThreads_Total},
        { 
          SS_Timings::expCreate_FW_createProcess_Count, 
          SS_Timings::expCreate_FW_createProcess_Max, 
          SS_Timings::expCreate_FW_createProcess_Min,
          SS_Timings::expCreate_FW_createProcess_Total},
        { 
          SS_Timings::expCreate_focusExp_Count, 
          SS_Timings::expCreate_focusExp_Max, 
          SS_Timings::expCreate_focusExp_Min,
          SS_Timings::expCreate_focusExp_Total},
        { 
          SS_Timings::expCompareCount, 
          SS_Timings::expCompareMax, 
          SS_Timings::expCompareMin,
          SS_Timings::expCompareTotal},
        { 
          SS_Timings::expGoCount, 
          SS_Timings::expGoMax, 
          SS_Timings::expGoMin,
          SS_Timings::expGoTotal},
        { 
          SS_Timings::cvClustersCount, 
          SS_Timings::cvClustersMax, 
          SS_Timings::cvClustersMin,
          SS_Timings::cvClustersTotal},
        { 
          SS_Timings::cvInfoCount, 
          SS_Timings::cvInfoMax, 
          SS_Timings::cvInfoMin,
          SS_Timings::cvInfoTotal},
        { 
          SS_Timings::viewGenerationCount, 
          SS_Timings::viewGenerationMax, 
          SS_Timings::viewGenerationMin,
          SS_Timings::viewGenerationTotal},
        { 
          SS_Timings::detailBaseReportCount, 
          SS_Timings::detailBaseReportMax, 
          SS_Timings::detailBaseReportMin,
          SS_Timings::detailBaseReportTotal},
        { 
          SS_Timings::detailTraceReportCount, 
          SS_Timings::detailTraceReportMax, 
          SS_Timings::detailTraceReportMin,
          SS_Timings::detailTraceReportTotal},
        { 
          SS_Timings::detailCallStackReportCount, 
          SS_Timings::detailCallStackReportMax, 
          SS_Timings::detailCallStackReportMin,
          SS_Timings::detailCallStackReportTotal},
        { 
          SS_Timings::detailButterFlyReportCount, 
          SS_Timings::detailButterFlyReportMax, 
          SS_Timings::detailButterFlyReportMin,
          SS_Timings::detailButterFlyReportTotal},
        { 
          SS_Timings::cliAllCount, 
          SS_Timings::cliAllMax, 
          SS_Timings::cliAllMin,
          SS_Timings::cliAllTotal},
        { 
          SS_Timings::cliBasicInitCount, 
          SS_Timings::cliBasicInitMax, 
          SS_Timings::cliBasicInitMin,
          SS_Timings::cliBasicInitTotal},
        { 
          SS_Timings::cliCmdLinePythonCount, 
          SS_Timings::cliCmdLinePythonMax, 
          SS_Timings::cliCmdLinePythonMin,
          SS_Timings::cliCmdLinePythonTotal},
        { 
          SS_Timings::cliWindowInitCount, 
          SS_Timings::cliWindowInitMax, 
          SS_Timings::cliWindowInitMin,
          SS_Timings::cliWindowInitTotal},
        { 
          SS_Timings::cliGuiLoadCount, 
          SS_Timings::cliGuiLoadMax, 
          SS_Timings::cliGuiLoadMin,
          SS_Timings::cliGuiLoadTotal},
        { 
          SS_Timings::perfTableNullCount, 
          SS_Timings::perfTableNullMax, 
          SS_Timings::perfTableNullMin, 
          SS_Timings::perfTableNullTotal }  // End Of Table Entry
    };


// The constructor for creating an initial Timing entry
SS_Timings::SS_Timings()
{
    debug_perf_enabled = (getenv("OPENSS_DEBUG_PERF_CLI") != NULL);

    cli_perf_data[cliAllStart] = Time::Now();

// Initialize counters to zero at start of gathering

    cli_perf_count[ cliAllCount ] = 0 ;
    cli_perf_count[ cliBasicInitCount ] = 0 ;
    cli_perf_count[ cliCmdLinePythonCount ] = 0 ;
    cli_perf_count[ cliWindowInitCount ] = 0 ;
    cli_perf_count[ cliGuiLoadCount ] = 0 ;
    cli_perf_count[ viewGenerationCount ] = 0;
    cli_perf_count[ expAttachCount ] = 0;
    cli_perf_count[ expAttach_resolveTargetList_Count ] = 0;
    cli_perf_count[ expAttach_getCollector_Count ] = 0;
    cli_perf_count[ expAttach_linkThreads_Count ] = 0;
    cli_perf_count[ expAttach_FW_createProcess_Count ] = 0;
    cli_perf_count[ expCreateCount ] = 0;
    cli_perf_count[ expCreate_wait_Count ] = 0;
    cli_perf_count[ expCreate_allocExp_Count ] = 0;
    cli_perf_count[ expCreate_resolveTargetList_Count ] = 0;
    cli_perf_count[ expCreate_focusExp_Count ] = 0;
    cli_perf_count[ expCreate_getCollector_Count ] = 0;
    cli_perf_count[ expCreate_linkThreads_Count ] = 0;
    cli_perf_count[ expCreate_FW_createProcess_Count ] = 0;
    cli_perf_count[ expCompareCount ] = 0;
    cli_perf_count[ cvClustersCount ] = 0;
    cli_perf_count[ cvInfoCount ] = 0;
    cli_perf_count[ detailBaseReportCount ] = 0;
    cli_perf_count[ detailTraceReportCount ] = 0;
    cli_perf_count[ detailCallStackReportCount ] = 0;
    cli_perf_count[ detailButterFlyReportCount ] = 0;
    cli_perf_count[ perfTableNullCount ] = 0;

// Initialize time totals to zero at start of gathering

    cli_perf_count[ cliAllTotal ] = 0 ;
    cli_perf_count[ cliBasicInitTotal ] = 0 ;
    cli_perf_count[ cliCmdLinePythonTotal ] = 0 ;
    cli_perf_count[ cliWindowInitTotal ] = 0 ;
    cli_perf_count[ cliGuiLoadTotal ] = 0 ;
    cli_perf_count[ viewGenerationTotal ] = 0;
    cli_perf_count[ expAttachTotal ] = 0;
    cli_perf_count[ expAttach_resolveTargetList_Total ] = 0;
    cli_perf_count[ expAttach_getCollector_Total ] = 0;
    cli_perf_count[ expAttach_linkThreads_Total ] = 0;
    cli_perf_count[ expAttach_FW_createProcess_Total ] = 0;
    cli_perf_count[ expCreateTotal ] = 0;
    cli_perf_count[ expCreate_wait_Total ] = 0;
    cli_perf_count[ expCreate_allocExp_Total ] = 0;
    cli_perf_count[ expCreate_resolveTargetList_Total ] = 0;
    cli_perf_count[ expCreate_focusExp_Total ] = 0;
    cli_perf_count[ expCreate_getCollector_Total ] = 0;
    cli_perf_count[ expCreate_linkThreads_Total ] = 0;
    cli_perf_count[ expCreate_FW_createProcess_Total ] = 0;
    cli_perf_count[ expCompareTotal ] = 0;
    cli_perf_count[ cvClustersTotal ] = 0;
    cli_perf_count[ cvInfoTotal ] = 0;
    cli_perf_count[ detailBaseReportTotal ] = 0;
    cli_perf_count[ detailTraceReportTotal ] = 0;
    cli_perf_count[ detailCallStackReportTotal ] = 0;
    cli_perf_count[ detailButterFlyReportTotal ] = 0;
    cli_perf_count[ perfTableNullTotal ] = 0;
}

/**
 * SS_Timings destructor
 */
SS_Timings::~SS_Timings()
{
}

bool SS_Timings::is_debug_perf_enabled()
{
  if (debug_perf_enabled) {
    return true;
  }
  return false;
}


bool SS_Timings::in_expAttach()
{
  if (SS_Timings::processing_expAttach == true) {
    return true;
  } else {
    return false;
  }
}

void SS_Timings::in_expAttach(bool flag)
{
  if (flag) {
    SS_Timings::processing_expAttach = true;
  } else {
    SS_Timings::processing_expAttach = false;
  }
}

bool SS_Timings::in_expCreate()
{
  if (SS_Timings::processing_expCreate == true) {
    return true;
  } else {
    return false;
  }
}

void SS_Timings::in_expCreate(bool flag)
{
  if (flag) {
    SS_Timings::processing_expCreate = true;
  } else {
    SS_Timings::processing_expCreate = false;
  }
}

/**
 * 
 * Processes the End performance event and records the min, max, and total
 * times spent in the timing event.
 * 
 */
void SS_Timings::processTimingEventEnd(
              CLIPerformanceDataEvents eventTypeStart,
              CLIPerformanceDataEvents eventTypeCount,
              CLIPerformanceDataEvents eventTypeMax,
              CLIPerformanceDataEvents eventTypeMin,
              CLIPerformanceDataEvents eventTypeTotal,
              CLIPerformanceDataEvents eventTypeEnd)
{
      cli_timing_handle->cli_perf_data[eventTypeEnd] = Time::Now();
      cli_timing_handle->cli_perf_count[eventTypeCount] += 1;
      uint64_t this_event_time =
               cli_timing_handle->cli_perf_data[eventTypeEnd]
             - cli_timing_handle->cli_perf_data[eventTypeStart] ;
      cli_timing_handle->cli_perf_count[eventTypeTotal] += this_event_time;
      if (this_event_time > cli_timing_handle->cli_perf_count[eventTypeMax]) {
            cli_timing_handle->cli_perf_count[eventTypeMax] = this_event_time;
      }
      if (cli_timing_handle->cli_perf_count[eventTypeCount] == 1) {
            cli_timing_handle->cli_perf_count[eventTypeMin] = this_event_time;
      } else {
         if (this_event_time < cli_timing_handle->cli_perf_count[eventTypeMin]) {
               cli_timing_handle->cli_perf_count[eventTypeMin] = this_event_time;
         }
      }

}

/**
 * Is this a performance event of type Start?
 *
 * Compares the input eventType to the known Start events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isStartEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationStart ||
      eventType == expAttachStart ||
      eventType == expAttach_resolveTargetList_Start ||
      eventType == expAttach_getCollector_Start ||
      eventType == expAttach_linkThreads_Start ||
      eventType == expAttach_FW_createProcess_Start ||
      eventType == expCreateStart ||
      eventType == expCreate_wait_Start ||
      eventType == expCreate_allocExp_Start ||
      eventType == expCreate_resolveTargetList_Start ||
      eventType == expCreate_focusExp_Start ||
      eventType == expCreate_getCollector_Start ||
      eventType == expCreate_linkThreads_Start ||
      eventType == expCreate_FW_createProcess_Start ||
      eventType == expCompareStart ||
      eventType == expGoStart ||
      eventType == cvClustersStart ||
      eventType == cvInfoStart ||
      eventType == detailBaseReportStart ||
      eventType == detailTraceReportStart ||
      eventType == detailCallStackReportStart ||
      eventType == detailButterFlyReportStart ||
      eventType == cliBasicInitStart ||
      eventType == cliCmdLinePythonStart ||
      eventType == cliWindowInitStart ||
      eventType == cliGuiLoadStart ||
      eventType == cliAllStart ) {
    return true;
  }
  return false;
}


/**
 * Is this a performance event of type End?
 *
 * Compares the input eventType to the known End events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isEndEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationEnd ||
      eventType == expAttachEnd ||
      eventType == expAttach_resolveTargetList_End ||
      eventType == expAttach_getCollector_End ||
      eventType == expAttach_linkThreads_End ||
      eventType == expAttach_FW_createProcess_End ||
      eventType == expCreateEnd ||
      eventType == expCreate_wait_End ||
      eventType == expCreate_allocExp_End ||
      eventType == expCreate_resolveTargetList_End ||
      eventType == expCreate_focusExp_End ||
      eventType == expCreate_getCollector_End ||
      eventType == expCreate_linkThreads_End ||
      eventType == expCreate_FW_createProcess_End ||
      eventType == expCompareEnd ||
      eventType == expGoEnd ||
      eventType == cvClustersEnd ||
      eventType == cvInfoEnd ||
      eventType == detailBaseReportEnd ||
      eventType == detailTraceReportEnd ||
      eventType == detailCallStackReportEnd ||
      eventType == detailButterFlyReportEnd ||
      eventType == cliBasicInitEnd ||
      eventType == cliCmdLinePythonEnd ||
      eventType == cliWindowInitEnd ||
      eventType == cliGuiLoadEnd ||
      eventType == cliAllEnd ) {
    return true;
  }
  return false;
}

/**
 * Is this a performance event of type Count?
 *
 * Compares the input eventType to the known Count events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isCountEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationCount ||
      eventType == expAttachCount ||
      eventType == expAttach_resolveTargetList_Count ||
      eventType == expAttach_getCollector_Count ||
      eventType == expAttach_linkThreads_Count ||
      eventType == expAttach_FW_createProcess_Count ||
      eventType == expCreateCount ||
      eventType == expCreate_wait_Count ||
      eventType == expCreate_allocExp_Count ||
      eventType == expCreate_resolveTargetList_Count ||
      eventType == expCreate_focusExp_Count ||
      eventType == expCreate_getCollector_Count ||
      eventType == expCreate_linkThreads_Count ||
      eventType == expCreate_FW_createProcess_Count ||
      eventType == expCompareCount ||
      eventType == expGoCount ||
      eventType == cvClustersCount ||
      eventType == cvInfoCount ||
      eventType == detailBaseReportCount ||
      eventType == detailTraceReportCount ||
      eventType == detailCallStackReportCount ||
      eventType == detailButterFlyReportCount ||
      eventType == cliBasicInitCount ||
      eventType == cliCmdLinePythonCount ||
      eventType == cliWindowInitCount ||
      eventType == cliGuiLoadCount ||
      eventType == cliAllCount ) {
    return true;
  }
  return false;
}

/**
 * Is this a performance event of type Total?
 *
 * Compares the input eventType to the known Total events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isTotalEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationTotal ||
      eventType == expAttachTotal ||
      eventType == expAttach_resolveTargetList_Total ||
      eventType == expAttach_getCollector_Total ||
      eventType == expAttach_linkThreads_Total ||
      eventType == expAttach_FW_createProcess_Total ||
      eventType == expCreateTotal ||
      eventType == expCreate_wait_Total ||
      eventType == expCreate_allocExp_Total ||
      eventType == expCreate_resolveTargetList_Total ||
      eventType == expCreate_focusExp_Total ||
      eventType == expCreate_getCollector_Total ||
      eventType == expCreate_linkThreads_Total ||
      eventType == expCreate_FW_createProcess_Total ||
      eventType == expCompareTotal ||
      eventType == expGoTotal ||
      eventType == cvClustersTotal ||
      eventType == cvInfoTotal ||
      eventType == detailBaseReportTotal ||
      eventType == detailTraceReportTotal ||
      eventType == detailCallStackReportTotal ||
      eventType == detailButterFlyReportTotal ||
      eventType == cliBasicInitTotal ||
      eventType == cliCmdLinePythonTotal ||
      eventType == cliWindowInitTotal ||
      eventType == cliGuiLoadTotal ||
      eventType == cliAllTotal ) {
    return true;
  }
  return false;
}

/**
 * Is this a performance event of type Max?
 *
 * Compares the input eventType to the known Max events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isMaxEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationMax ||
      eventType == expAttachMax ||
      eventType == expAttach_resolveTargetList_Max ||
      eventType == expAttach_getCollector_Max ||
      eventType == expAttach_linkThreads_Max ||
      eventType == expAttach_FW_createProcess_Max ||
      eventType == expCreateMax ||
      eventType == expCreate_wait_Max ||
      eventType == expCreate_allocExp_Max ||
      eventType == expCreate_resolveTargetList_Max ||
      eventType == expCreate_focusExp_Max ||
      eventType == expCreate_getCollector_Max ||
      eventType == expCreate_linkThreads_Max ||
      eventType == expCreate_FW_createProcess_Max ||
      eventType == expCompareMax ||
      eventType == expGoMax ||
      eventType == cvClustersMax ||
      eventType == cvInfoMax ||
      eventType == detailBaseReportMax ||
      eventType == detailTraceReportMax ||
      eventType == detailCallStackReportMax ||
      eventType == detailButterFlyReportMax ||
      eventType == cliBasicInitMax ||
      eventType == cliCmdLinePythonMax ||
      eventType == cliWindowInitMax ||
      eventType == cliGuiLoadMax ||
      eventType == cliAllMax ) {
    return true;
  }
  return false;
}

/**
 * Is this a performance event of type Min?
 *
 * Compares the input eventType to the known Min events and returns true
 * if there is a match and false otherwise.
 */
bool SS_Timings::isMinEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == viewGenerationMin ||
      eventType == expAttachMin ||
      eventType == expAttach_resolveTargetList_Min ||
      eventType == expAttach_getCollector_Min ||
      eventType == expAttach_linkThreads_Min ||
      eventType == expAttach_FW_createProcess_Min ||
      eventType == expCreateMin ||
      eventType == expCreate_wait_Min ||
      eventType == expCreate_allocExp_Min ||
      eventType == expCreate_focusExp_Min ||
      eventType == expCreate_resolveTargetList_Min ||
      eventType == expCreate_getCollector_Min ||
      eventType == expCreate_linkThreads_Min ||
      eventType == expCreate_FW_createProcess_Min ||
      eventType == expCompareMin ||
      eventType == expGoMin ||
      eventType == cvClustersMin ||
      eventType == cvInfoMin ||
      eventType == detailBaseReportMin ||
      eventType == detailTraceReportMin ||
      eventType == detailCallStackReportMin ||
      eventType == detailButterFlyReportMin ||
      eventType == cliBasicInitMin ||
      eventType == cliCmdLinePythonMin ||
      eventType == cliWindowInitMin ||
      eventType == cliGuiLoadMin ||
      eventType == cliAllMin ) {
    return true;
  }
  return false;
}

SS_Timings::CLIPerformanceDataEvents SS_Timings::findTotalEvent( CLIPerformanceDataEvents inputCountEvent)
{
    for(unsigned i = 0; MaxMinTable[i].count_cli_event != perfTableNullCount; ++i) {
      if (MaxMinTable[i].count_cli_event == inputCountEvent) {
         return MaxMinTable[i].total_cli_event;
      }
    }
  cerr << "BAD exit findTotalEvent inputCountEvent = " << inputCountEvent << " \n ";
  return perfTableNullTotal;
}

SS_Timings::CLIPerformanceDataEvents SS_Timings::findMinEvent( CLIPerformanceDataEvents inputCountEvent)
{
    for(unsigned i = 0; MaxMinTable[i].count_cli_event != perfTableNullCount; ++i) {
#if 0
      cerr << "in findMinEvent, i = " << i << " \n ";
      cerr << "MaxMinTable[i].count_cli_event = " << MaxMinTable[i].count_cli_event << " \n ";
      cerr << "inputCountEvent = " << inputCountEvent << " \n ";
#endif
      if (MaxMinTable[i].count_cli_event == inputCountEvent) {
         return MaxMinTable[i].min_cli_event;
      }
    }
  cerr << "BAD exit findMinEvent inputCountEvent = " << inputCountEvent << " \n ";
  return perfTableNullMin;
}

SS_Timings::CLIPerformanceDataEvents SS_Timings::findMaxEvent( CLIPerformanceDataEvents inputCountEvent)
{
    for(unsigned i = 0; MaxMinTable[i].count_cli_event != perfTableNullCount; ++i) {
      if (MaxMinTable[i].count_cli_event == inputCountEvent) {
         return MaxMinTable[i].max_cli_event;
      }
    }
  cerr << "BAD exit findMaxEvent inputCountEvent = " << inputCountEvent << "  \n";
  return perfTableNullMax;
}



/**
 * Display performance statistics.
 *
 * Displays performance statistics for the processes in this process table
 * to the standard error stream. Reported information includes the minimum,
 * average, and maximum time spent during various phases of attaching to a
 * process.
 */
void SS_Timings::CLIPerformanceStatistics()
{
    static const unsigned InitialIndent = 2;
    static const unsigned TimeWidth = 8;

    static const struct {

        /** Performance data event code. */
        SS_Timings::CLIPerformanceDataEvents cli_event;

        /** Description of that event code. */
        std::string cli_description;

        /** Description of that event code. */
        bool subtask_event;

    } Table[] = {

        { SS_Timings::expAttachStart, "Start Time the last expAttach Command Started", false },
        { SS_Timings::expAttachCount, "Number of expAttach commands being timed", false },
        { SS_Timings::expAttachMax, "Maximum Time spent in expAttach for one call", false },
        { SS_Timings::expAttachMin, "Minimum Time spent in expAttach for one call", false },
        { SS_Timings::expAttachTotal, "Total Time expAttach command time possibly over multiple calls", false },
        { SS_Timings::expAttachEnd, "End Time the last expAttach Command Ended", false },
        { SS_Timings::expAttach_resolveTargetList_Start,"Start Time expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_resolveTargetList_Count,"Number of expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_resolveTargetList_Max,"Maximum Time expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_resolveTargetList_Min,"Minimum Time expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_resolveTargetList_Total,"Total Time expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_resolveTargetList_End,"End Time when expAttach sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expAttach_getCollector_Start,"Start Time expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_getCollector_Count,"Number of expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_getCollector_Max,"Maximum Time expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_getCollector_Min,"Minimum Time expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_getCollector_Total,"Total Time expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_getCollector_End,"End Time for expAttach sub-task measurement for getting the collectors", true},
        { SS_Timings::expAttach_linkThreads_Start,"Start Time expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_linkThreads_Count,"Number of expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_linkThreads_Max,"Maximum Time expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_linkThreads_Min,"Minimum Time expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_linkThreads_Total,"Total Time expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_linkThreads_End,"End Time for expAttach sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expAttach_FW_createProcess_Start,"Start Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Count,"Number of expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Max,"Maximum Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Min,"Minimum Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Total,"Total Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_End,"End Time for expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreateStart, "Start Time the last expCreate Command Started", false },
        { SS_Timings::expCreateCount, "Number of expCreate commands being timed", false },
        { SS_Timings::expCreateMax, "Maximum Time spent in expCreate for one call", false },
        { SS_Timings::expCreateMin, "Minimum Time spent in expCreate for one call", false },
        { SS_Timings::expCreateTotal, "Total Time expCreate command time possibly over multiple calls", false },
        { SS_Timings::expCreateEnd, "End Time the last expCreate Comman Ended", false },
        { SS_Timings::expCreate_wait_Start, "Start Time expCreate sub-task measurement waittime Start ", true },     
        { SS_Timings::expCreate_wait_Count, "Number of expCreate sub-task measurement waittime # times", true},
        { SS_Timings::expCreate_wait_Max,"Maximum Time expCreate sub-task measurement waittime Max", true},
        { SS_Timings::expCreate_wait_Min,"Minimum Time expCreate sub-task measurement waittime Min", true},
        { SS_Timings::expCreate_wait_Total,"Total Time expCreate sub-task measurement waittime Total", true},
        { SS_Timings::expCreate_wait_End,"End Time expCreate sub-task measurement waittime End", true},
        { SS_Timings::expCreate_allocExp_Start,"Start Time expCreate sub-task measurement experiment obj creation Start time", true},
        { SS_Timings::expCreate_allocExp_Count,"Number of expCreate sub-task measurement experiment obj creation # times", true},
        { SS_Timings::expCreate_allocExp_Max,"Maximum Time expCreate sub-task measurement experiment obj creation maximum time", true},
        { SS_Timings::expCreate_allocExp_Min,"Minimum Time expCreate sub-task measurement experiment obj creation minimum time", true},
        { SS_Timings::expCreate_allocExp_Total,"Total Time expCreate sub-task measurement experiment obj creation Total time", true},
        { SS_Timings::expCreate_allocExp_End,"End Time expCreate sub-task measurement experiment obj creation End time", true},
        { SS_Timings::expCreate_resolveTargetList_Start,"Start Time expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_resolveTargetList_Count,"Number of expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_resolveTargetList_Max,"Maximum Time expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_resolveTargetList_Min,"Minimum Time expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_resolveTargetList_Total,"Total Time expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_resolveTargetList_End,"End Time when expCreate sub-task measurement resolve the target list and attach or create processes", true},
        { SS_Timings::expCreate_getCollector_Start,"Start Time expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_getCollector_Count,"Number of expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_getCollector_Max,"Maximum Time expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_getCollector_Min,"Minimum Time expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_getCollector_Total,"Total Time expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_getCollector_End,"End Time for expCreate sub-task measurement for getting the collectors", true},
        { SS_Timings::expCreate_linkThreads_Start,"Start Time expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_linkThreads_Count,"Number of expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_linkThreads_Max,"Maximum Time expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_linkThreads_Min,"Minimum Time expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_linkThreads_Total,"Total Time expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_linkThreads_End,"End Time for expCreate sub-task measurement for linking threads to the collectors", true},
        { SS_Timings::expCreate_FW_createProcess_Start,"Start Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Count,"Number of expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Max,"Maximum Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Min,"Minimum Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Total,"Total Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_End,"End Time for expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_focusExp_Start,"Start Time expCreate sub-task measurement focus experiment and bookkeeping Start time", true},
        { SS_Timings::expCreate_focusExp_Count,"Number of expCreate sub-task measurement focus experiment and bookkeeping # times", true},
        { SS_Timings::expCreate_focusExp_Max,"Maximum Time expCreate sub-task measurement focus experiment and bookkeeping maximum time", true},
        { SS_Timings::expCreate_focusExp_Min,"Minimum Time expCreate sub-task measurement focus experiment and bookkeeping minimum time", true},
        { SS_Timings::expCreate_focusExp_Total,"Total Time expCreate sub-task measurement focus experiment and bookkeeping Total time", true},
        { SS_Timings::expCreate_focusExp_End,"End Time for expCreate sub-task measurement focus experiment and bookkeeping End time", true},
        { SS_Timings::expCompareStart, "Start Time the last expCompare Command Started" , false},
        { SS_Timings::expCompareCount, "Number of expCompare commands being timed" , false},
        { SS_Timings::expCompareMax, "Maximum time spent in expCompare for one call" , false},
        { SS_Timings::expCompareMin, "Minimum time spent in expCompare for one call" , false},
        { SS_Timings::expCompareTotal, "Total expCompare command time possibly over multiple calls" , false},
        { SS_Timings::expCompareEnd, "End Time the last expCompare Comman Ended" , false},
        { SS_Timings::expGoStart, "Start Time the last expGo Command Started" , false},
        { SS_Timings::expGoCount, "Number of expGo commands being timed" , false},
        { SS_Timings::expGoMax, "Maximum time spent in expGo for one call" , false},
        { SS_Timings::expGoMin, "Minimum time spent in expGo for one call" , false},
        { SS_Timings::expGoTotal, "Total expGo command time possibly over multiple calls" , false},
        { SS_Timings::expGoEnd, "End Time the last expGo Comman Ended" , false},
        { SS_Timings::cvClustersStart, "Start Time the last cvClusters Command Started" , false},
        { SS_Timings::cvClustersCount, "Number of cvClusters commands being timed" , false},
        { SS_Timings::cvClustersMax, "Maximum time spent in the cvClusters command for one call" , false},
        { SS_Timings::cvClustersMin, "Minimum time spent in the cvClusters command for one call" , false},
        { SS_Timings::cvClustersTotal, "Total cvClusters command time possibly over multiple calls" , false},
        { SS_Timings::cvClustersEnd, "Time the last cvClusters Command Ended" , false},
        { SS_Timings::cvInfoStart, "Time the last cvInfo Command Started" , false},
        { SS_Timings::cvInfoCount, "Number of cvInfo commands being timed" , false},
        { SS_Timings::cvInfoMax, "Maximum time spent in the cvInfo command for one call" , false},
        { SS_Timings::cvInfoMin, "Minimum time spent in the cvInfo command for one call" , false},
        { SS_Timings::cvInfoTotal, "Total cvInfo command time possibly over multiple calls" , false},
        { SS_Timings::cvInfoEnd, "End Time the last cvInfo Command Ended" , false},
        { SS_Timings::viewGenerationStart, "Start Time the CLI Started Generation of a report", false },
        { SS_Timings::viewGenerationCount, "Number of times view generation was/is being timed", false },
        { SS_Timings::viewGenerationMax, "Maximum Time spent in view generation for one call", false },
        { SS_Timings::viewGenerationMin, "Minimum Time spent in view generation for one call", false },
        { SS_Timings::viewGenerationTotal, "Total Time spent in view generation, possibly over multiple calls", false },
        { SS_Timings::viewGenerationEnd, "End Time the CLI Completed Generation of a report", false },
        { SS_Timings::detailBaseReportStart, "Start Time the last detailBaseReport generation Started" , false},
        { SS_Timings::detailBaseReportCount, "Number of detailBaseReport generations being timed" , false},
        { SS_Timings::detailBaseReportMax, "Maximum time spent in detailed base report generation for one call" , false},
        { SS_Timings::detailBaseReportMin, "Minimum time spent in detailed base report generation for one call" , false},
        { SS_Timings::detailBaseReportTotal, "Total detailBaseReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailBaseReportEnd, "End Time when the last detailBaseReport Generation Ended" , false},
        { SS_Timings::detailTraceReportStart, "Start Time the last detailTraceReport generation Started" , false},
        { SS_Timings::detailTraceReportCount, "Number of detailTraceReport generations being timed" , false},
        { SS_Timings::detailTraceReportMax, "Maximum time spent in detailed trace report generation for one call" , false},
        { SS_Timings::detailTraceReportMin, "Minimum time spent in detailed trace report generation for one call" , false},
        { SS_Timings::detailTraceReportTotal, "Total detailTraceReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailTraceReportEnd, "End Time when the last detailTraceReport Generation Ended" , false},
        { SS_Timings::detailCallStackReportStart, "Start Time the last detailCallStackReport generation Started" , false},
        { SS_Timings::detailCallStackReportCount, "Number of detailCallStackReport generations being timed" , false},
        { SS_Timings::detailCallStackReportMax, "Maximum time spent in detailed callstack report generation for one call" , false},
        { SS_Timings::detailCallStackReportMin, "Minimum time spent in detailed callstack report generation for one call" , false},
        { SS_Timings::detailCallStackReportTotal, "Total detailCallStackReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailCallStackReportEnd, "End Time when the last detailCallStackReport Generation Ended" , false},
        { SS_Timings::detailButterFlyReportStart, "Start Time the last detailButterFlyReport generation Started" , false},
        { SS_Timings::detailButterFlyReportCount, "Number of detailButterFlyReport generations being timed" , false},
        { SS_Timings::detailButterFlyReportMax, "Maximum time spent in detailed butterfly report generation for one call" , false},
        { SS_Timings::detailButterFlyReportMin, "Minimum time spent in detailed butterfly report generation for one call" , false},
        { SS_Timings::detailButterFlyReportTotal, "Total detailButterFlyReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailButterFlyReportEnd, "End Time when the last detailButterFlyReport Generation Ended" , false},
        { SS_Timings::cliAllStart, "Start Time for when the CLI execution started" , false},
        { SS_Timings::cliAllCount, "Number of times the CLI was/is being timed" , false},
        { SS_Timings::cliAllMax, "Maximum time spent in the CLI for one call" , false},
        { SS_Timings::cliAllMin, "Minimum time spent in the CLI for one call" , false},
        { SS_Timings::cliAllTotal, "Total time spent in CLI, possibly over multiple calls" , false},
        { SS_Timings::cliAllEnd, "End Time when CLI execution ended/exited" , false},
        { SS_Timings::cliBasicInitStart, "Start Time for when the CLI basic init execution started" , true},
        { SS_Timings::cliBasicInitCount, "Number of times the CLI basic init was/is being timed" , true},
        { SS_Timings::cliBasicInitMax, "Maximum time spent in the CLI basic init for one call" , true},
        { SS_Timings::cliBasicInitMin, "Minimum time spent in the CLI basic init for one call" , true},
        { SS_Timings::cliBasicInitTotal, "Total time spent in CLI basic init, possibly over multiple calls" , true},
        { SS_Timings::cliBasicInitEnd, "End Time when CLI basic init ended" , true},
        { SS_Timings::cliCmdLinePythonStart, "Start Time for when the CLI command line and python init execution started" , true},
        { SS_Timings::cliCmdLinePythonCount, "Number of times the CLI command line and python init was/is being timed" , true},
        { SS_Timings::cliCmdLinePythonMax, "Maximum time spent in the CLI command line and python init for one call" , true},
        { SS_Timings::cliCmdLinePythonMin, "Minimum time spent in the CLI command line and python init for one call" , true},
        { SS_Timings::cliCmdLinePythonTotal, "Total time spent in CLI command line and python init, possibly over multiple calls" , true},
        { SS_Timings::cliCmdLinePythonEnd, "End Time when CLI command line and python init ended" , true},
        { SS_Timings::cliWindowInitStart, "Start Time for when the CLI window init execution started" , true},
        { SS_Timings::cliWindowInitCount, "Number of times the CLI window init was/is being timed" , true},
        { SS_Timings::cliWindowInitMax, "Maximum time spent in the CLI window init for one call" , true},
        { SS_Timings::cliWindowInitMin, "Minimum time spent in the CLI window init for one call" , true},
        { SS_Timings::cliWindowInitTotal, "Total time spent in CLI window init, possibly over multiple calls" , true},
        { SS_Timings::cliWindowInitEnd, "End Time when CLI window init ended" , true},
        { SS_Timings::cliGuiLoadStart, "Start Time for when the CLI gui window load execution started" , true},
        { SS_Timings::cliGuiLoadCount, "Number of times the CLI gui window load was/is being timed" , true},
        { SS_Timings::cliGuiLoadMax, "Maximum time spent in the CLI gui window load for one call" , true},
        { SS_Timings::cliGuiLoadMin, "Minimum time spent in the CLI gui window load for one call" , true},
        { SS_Timings::cliGuiLoadTotal, "Total time spent in CLI gui window load, possibly over multiple calls" , true},
        { SS_Timings::cliGuiLoadEnd, "End Time when CLI gui window load ended" , true},
        { SS_Timings::perfTableNullStart, "dummy entry", false},
        { SS_Timings::perfTableNullCount, "dummy entry", false},
        { SS_Timings::perfTableNullMax, "dummy entry", false},
        { SS_Timings::perfTableNullMin,  "dummy entry", false},
        { SS_Timings::perfTableNullTotal, "dummy entry", false},
        { SS_Timings::perfTableNullEnd, "", false} // End of Table Entry

    };

// Iterate over the performance count data now
    // Display the header
    std::cerr << std::endl << std::endl << std::endl
              << std::setw(InitialIndent) << " "
              << "INDIVIDUAL TOTAL TIME VALUES FOR CLI TASK and subtask PERFORMANCE " << ""
              << std::endl 
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "" << std::endl
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "Time(mS)" << "  "
              << "Event" << std::endl << std::endl;


    // Iterate over each performance count event to be shown
    for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {


            // Skip this event entry if it didn't record this event
            if(cli_perf_count.find(Table[i].cli_event) == cli_perf_count.end())
                continue;

            // Calculate the "created-relative" event time for this event
            if (isTotalEvent(Table[i].cli_event)) {

               // Is it a sub-task - indent results
               if (Table[i].subtask_event) {
                  std::cerr << std::setw(InitialIndent) << "      ";
               }

               Time::difference_type totalTime = cli_perf_count[Table[i].cli_event]  / 1000000;
               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << totalTime << "  "
                         << Table[i].cli_description << std::endl;

            }

    } // end for i

    // Display the header
    std::cerr << std::endl << std::endl << std::endl
              << std::setw(InitialIndent) << " "
              << "ALL EVENT INDIVIDUAL VALUES FOR CLI TASK and subtask PERFORMANCE " << ""
              << std::endl 
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "" << std::endl
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "Time(mS)" << "  "
              << "Event" << std::endl << std::endl;


    // Iterate over each performance count event to be shown
    for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {


            // Skip this event entry if it didn't record this event
            if(cli_perf_count.find(Table[i].cli_event) == cli_perf_count.end())
                continue;

            // Is it a sub-task - indent results
            if (Table[i].subtask_event) {
               std::cerr << std::setw(InitialIndent) << "      ";
            }

            // Calculate the "created-relative" event time for this event
            if (isTotalEvent(Table[i].cli_event)) {

               Time::difference_type totalTime = cli_perf_count[Table[i].cli_event]  / 1000000;
               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << totalTime << "  "
                         << Table[i].cli_description << std::endl;

            }
            else if (isCountEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) <<  cli_perf_count[ Table[i].cli_event ] << "  "
                         << Table[i].cli_description << std::endl;

            }
            else if (isStartEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ]  << "  "
                         << Table[i].cli_description << std::endl;
            }
            else if (isEndEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ]  << "  "
                         << Table[i].cli_description << std::endl;
            }
            else if (isMinEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_count[ Table[i].cli_event ] / 1000000  << "  "
                         << Table[i].cli_description << std::endl;
            }
            else if (isMaxEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_count[ Table[i].cli_event ] / 1000000  << "  "
                         << Table[i].cli_description << std::endl;
            }
#if 0
// Re-enable if it is desirable to see the start time and end time for the
// event performance statistics gathering.  The start and end time will be
// related to the last timing event.  Intermediate start and end times are
// not saved at this time.   12/8/2006 jeg
            else if (isStartEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ] / 1000000 << "  "
                         << Table[i].cli_description << std::endl;
            }
            else if (isEndEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ] / 1000000 << "  "
                         << Table[i].cli_description << std::endl;
            }
#endif

    } // end for i



    int multi_event_counter = 0;
    for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {
       if (isCountEvent(Table[i].cli_event) && cli_perf_count[Table[i].cli_event] > 1) {
              multi_event_counter += 1;
       }
    }

// Setup code for multi event analysis - min, max, and average

    if (multi_event_counter > 0) {

    // Display the header
    std::cerr << std::endl << std::endl << std::endl
              << std::setw(InitialIndent) << " "
              << "SUMMARY OF CLI TASK and TASK subtask PERFORMANCE IF MULTIPLE EVENTS " << ""
              << std::endl 
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "" << std::endl
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "NumEvents" << "  "
              << std::setw(TimeWidth) << "Min Time(mS)" << "  "
              << std::setw(TimeWidth) << "Ave Time(mS)" << "  "
              << std::setw(TimeWidth) << "Max Time(mS)" << "  "
              << std::setw(TimeWidth) << "Event" << "  "
              << "" << std::endl << std::endl;

      for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {

         if (isCountEvent(Table[i].cli_event) && cli_perf_count[Table[i].cli_event] > 1) {

                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  cli_perf_count[ Table[i].cli_event ] << "  "
                           << "" << "  ";

                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  cli_perf_count[ findMinEvent(Table[i].cli_event) ]  / 1000000 << "  "
                           << "" << "  ";
                
                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  (cli_perf_count[findTotalEvent(Table[i].cli_event) ] / 1000000)/ cli_perf_count[ Table[i].cli_event ] << "  "
                           << "" << "  ";
                
                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  cli_perf_count[findMaxEvent(Table[i].cli_event) ] / 1000000 << "  "
                           << "" << "  ";

                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  Table[i].cli_description << "  "
                           << "" << std::endl;

           }
         }
      }


    // Display the tailer
    std::cerr << std::endl << std::endl << std::endl;
}

