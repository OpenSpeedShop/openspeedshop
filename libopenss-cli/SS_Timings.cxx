////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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
          SS_Timings::expDetachCount, 
          SS_Timings::expDetachMax, 
          SS_Timings::expDetachMin,
          SS_Timings::expDetachTotal},
        { 
          SS_Timings::expDisableCount, 
          SS_Timings::expDisableMax, 
          SS_Timings::expDisableMin,
          SS_Timings::expDisableTotal},
        { 
          SS_Timings::expEnableCount, 
          SS_Timings::expEnableMax, 
          SS_Timings::expEnableMin,
          SS_Timings::expEnableTotal},
        { 
          SS_Timings::expFocusCount, 
          SS_Timings::expFocusMax, 
          SS_Timings::expFocusMin,
          SS_Timings::expFocusTotal},
        { 
          SS_Timings::expGoCount, 
          SS_Timings::expGoMax, 
          SS_Timings::expGoMin,
          SS_Timings::expGoTotal},
        { 
          SS_Timings::expPauseCount, 
          SS_Timings::expPauseMax, 
          SS_Timings::expPauseMin,
          SS_Timings::expPauseTotal},
        { 
          SS_Timings::expRestoreCount, 
          SS_Timings::expRestoreMax, 
          SS_Timings::expRestoreMin,
          SS_Timings::expRestoreTotal},
        { 
          SS_Timings::expSaveCount, 
          SS_Timings::expSaveMax, 
          SS_Timings::expSaveMin,
          SS_Timings::expSaveTotal},
        { 
          SS_Timings::expSetParamCount, 
          SS_Timings::expSetParamMax, 
          SS_Timings::expSetParamMin,
          SS_Timings::expSetParamTotal},
        { 
          SS_Timings::expCmd_InfoCount, 
          SS_Timings::expCmd_InfoMax, 
          SS_Timings::expCmd_InfoMin,
          SS_Timings::expCmd_InfoTotal},
        { 
          SS_Timings::expCmd_C_ViewCount, 
          SS_Timings::expCmd_C_ViewMax, 
          SS_Timings::expCmd_C_ViewMin,
          SS_Timings::expCmd_C_ViewTotal},
        { 
          SS_Timings::expCmd_ViewCount, 
          SS_Timings::expCmd_ViewMax, 
          SS_Timings::expCmd_ViewMin,
          SS_Timings::expCmd_ViewTotal},
        { 
          SS_Timings::cvClustersCount, 
          SS_Timings::cvClustersMax, 
          SS_Timings::cvClustersMin,
          SS_Timings::cvClustersTotal},
        { 
          SS_Timings::cvCreateCount, 
          SS_Timings::cvCreateMax, 
          SS_Timings::cvCreateMin,
          SS_Timings::cvCreateTotal},
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
          SS_Timings::cliCmd_EXT_CreateCount, 
          SS_Timings::cliCmd_EXT_CreateMax, 
          SS_Timings::cliCmd_EXT_CreateMin,
          SS_Timings::cliCmd_EXT_CreateTotal},
        { 
          SS_Timings::cliExecuteCmdCount, 
          SS_Timings::cliExecuteCmdMax, 
          SS_Timings::cliExecuteCmdMin,
          SS_Timings::cliExecuteCmdTotal},
        { 
          SS_Timings::cliCmdLinePythonCount, 
          SS_Timings::cliCmdLinePythonMax, 
          SS_Timings::cliCmdLinePythonMin,
          SS_Timings::cliCmdLinePythonTotal},
        { 
          SS_Timings::cliWatcherCount, 
          SS_Timings::cliWatcherMax, 
          SS_Timings::cliWatcherMin,
          SS_Timings::cliWatcherTotal},
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
          SS_Timings::cliPurgeDispatchQueueCount, 
          SS_Timings::cliPurgeDispatchQueueMax, 
          SS_Timings::cliPurgeDispatchQueueMin, 
          SS_Timings::cliPurgeDispatchQueueTotal}, 
        { 
          SS_Timings::cliWaitCount, 
          SS_Timings::cliWaitMax, 
          SS_Timings::cliWaitMin,
          SS_Timings::cliWaitTotal},
        { 
          SS_Timings::cliWaitForOthersTermCount,
          SS_Timings::cliWaitForOthersTermMax,
          SS_Timings::cliWaitForOthersTermMin,
          SS_Timings::cliWaitForOthersTermTotal},
        { 
          SS_Timings::cliWaitForPrevCmdsCount,
          SS_Timings::cliWaitForPrevCmdsMax,
          SS_Timings::cliWaitForPrevCmdsMin,
          SS_Timings::cliWaitForPrevCmdsTotal},
        { 
          SS_Timings::listGenericCount, 
          SS_Timings::listGenericMax, 
          SS_Timings::listGenericMin,
          SS_Timings::listGenericTotal},
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
    cli_perf_count[ cliExecuteCmdCount ] = 0 ;
    cli_perf_count[ cliCmd_EXT_CreateCount ] = 0 ;
    cli_perf_count[ cliWatcherCount ] = 0 ;
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
    cli_perf_count[ expDetachCount ] = 0;
    cli_perf_count[ expDisableCount ] = 0;
    cli_perf_count[ expEnableCount ] = 0;
    cli_perf_count[ expFocusCount ] = 0;
    cli_perf_count[ expPauseCount ] = 0;
    cli_perf_count[ expRestoreCount ] = 0;
    cli_perf_count[ expSaveCount ] = 0;
    cli_perf_count[ listGenericCount ] = 0;
    cli_perf_count[ cvCreateCount ] = 0;
    cli_perf_count[ cvClustersCount ] = 0;
    cli_perf_count[ expCmd_InfoCount ] = 0;
    cli_perf_count[ expCmd_ViewCount ] = 0;
    cli_perf_count[ expCmd_C_ViewCount ] = 0;
    cli_perf_count[ cvInfoCount ] = 0;
    cli_perf_count[ detailBaseReportCount ] = 0;
    cli_perf_count[ detailTraceReportCount ] = 0;
    cli_perf_count[ detailCallStackReportCount ] = 0;
    cli_perf_count[ detailButterFlyReportCount ] = 0;
    cli_perf_count[ expSetParamCount ] = 0;
    cli_perf_count[ cliWaitCount ] = 0;
    cli_perf_count[ cliPurgeDispatchQueueCount ] = 0;
    cli_perf_count[ cliWaitForPrevCmdsCount ] = 0;
    cli_perf_count[ cliWaitForOthersTermCount ] = 0;
    cli_perf_count[ perfTableNullCount ] = 0;

// Initialize time totals to zero at start of gathering

    cli_perf_count[ cliAllTotal ] = 0 ;
    cli_perf_count[ cliBasicInitTotal ] = 0 ;
    cli_perf_count[ cliCmdLinePythonTotal ] = 0 ;
    cli_perf_count[ cliExecuteCmdTotal ] = 0 ;
    cli_perf_count[ cliCmd_EXT_CreateTotal ] = 0 ;
    cli_perf_count[ cliWindowInitTotal ] = 0 ;
    cli_perf_count[ cliWatcherTotal ] = 0 ;
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
    cli_perf_count[ expCmd_InfoTotal ] = 0;
    cli_perf_count[ expCmd_ViewTotal ] = 0;
    cli_perf_count[ expCmd_C_ViewTotal ] = 0;
    cli_perf_count[ cvInfoTotal ] = 0;
    cli_perf_count[ detailBaseReportTotal ] = 0;
    cli_perf_count[ detailTraceReportTotal ] = 0;
    cli_perf_count[ detailCallStackReportTotal ] = 0;
    cli_perf_count[ detailButterFlyReportTotal ] = 0;
    cli_perf_count[ expDetachTotal ] = 0;
    cli_perf_count[ expDisableTotal ] = 0;
    cli_perf_count[ expEnableTotal ] = 0;
    cli_perf_count[ expFocusTotal ] = 0;
    cli_perf_count[ expPauseTotal ] = 0;
    cli_perf_count[ expRestoreTotal ] = 0;
    cli_perf_count[ expSaveTotal ] = 0;
    cli_perf_count[ listGenericTotal ] = 0;
    cli_perf_count[ cvCreateTotal ] = 0;
    cli_perf_count[ expSetParamTotal ] = 0;
    cli_perf_count[ cliWaitTotal ] = 0;
    cli_perf_count[ cliPurgeDispatchQueueTotal ] = 0;
    cli_perf_count[ cliWaitForPrevCmdsTotal ] = 0;
    cli_perf_count[ cliWaitForOthersTermTotal ] = 0;
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
      eventType == expCmd_InfoStart ||
      eventType == expCmd_ViewStart ||
      eventType == expCmd_C_ViewStart ||
      eventType == cvInfoStart ||
      eventType == detailBaseReportStart ||
      eventType == detailTraceReportStart ||
      eventType == detailCallStackReportStart ||
      eventType == detailButterFlyReportStart ||
      eventType == cliBasicInitStart ||
      eventType == cliCmdLinePythonStart ||
      eventType == cliCmd_EXT_CreateStart ||
      eventType == cliExecuteCmdStart ||
      eventType == cliCmd_EXT_CreateStart ||
      eventType == cliWindowInitStart ||
      eventType == cliWatcherStart ||
      eventType == cliGuiLoadStart ||
      eventType == expDetachStart ||
      eventType == expDisableStart  ||
      eventType == expEnableStart  ||
      eventType == expFocusStart  ||
      eventType == expPauseStart  ||
      eventType == expRestoreStart  ||
      eventType == expSaveStart  ||
      eventType == listGenericStart ||
      eventType == cvCreateStart ||
      eventType == expSetParamStart ||
      eventType == cliWaitStart ||
      eventType == cliPurgeDispatchQueueStart ||
      eventType == cliWaitForPrevCmdsStart ||
      eventType == cliWaitForOthersTermStart ||
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
      eventType == expCmd_InfoEnd ||
      eventType == expCmd_ViewEnd ||
      eventType == expCmd_C_ViewEnd ||
      eventType == cvInfoEnd ||
      eventType == detailBaseReportEnd ||
      eventType == detailTraceReportEnd ||
      eventType == detailCallStackReportEnd ||
      eventType == detailButterFlyReportEnd ||
      eventType == cliBasicInitEnd ||
      eventType == cliCmdLinePythonEnd ||
      eventType == cliExecuteCmdEnd ||
      eventType == cliCmd_EXT_CreateEnd ||
      eventType == cliWindowInitEnd ||
      eventType == cliWatcherEnd ||
      eventType == cliGuiLoadEnd ||
      eventType == expDetachEnd ||
      eventType == expDisableEnd  ||
      eventType == expEnableEnd  ||
      eventType == expFocusEnd  ||
      eventType == expPauseEnd  ||
      eventType == expRestoreEnd  ||
      eventType == expSaveEnd  ||
      eventType == listGenericEnd ||
      eventType == cvCreateEnd ||
      eventType == expSetParamEnd ||
      eventType == cliWaitEnd ||
      eventType == cliPurgeDispatchQueueEnd ||
      eventType == cliWaitForPrevCmdsEnd ||
      eventType == cliWaitForOthersTermEnd ||
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
      eventType == expCmd_InfoCount ||
      eventType == expCmd_ViewCount ||
      eventType == expCmd_C_ViewCount ||
      eventType == cvInfoCount ||
      eventType == detailBaseReportCount ||
      eventType == detailTraceReportCount ||
      eventType == detailCallStackReportCount ||
      eventType == detailButterFlyReportCount ||
      eventType == cliBasicInitCount ||
      eventType == cliCmdLinePythonCount ||
      eventType == cliExecuteCmdCount ||
      eventType == cliCmd_EXT_CreateCount ||
      eventType == cliWindowInitCount ||
      eventType == cliWatcherCount ||
      eventType == cliGuiLoadCount ||
      eventType == expDetachCount ||
      eventType == expDisableCount  ||
      eventType == expEnableCount  ||
      eventType == expFocusCount  ||
      eventType == expPauseCount  ||
      eventType == expRestoreCount  ||
      eventType == expSaveCount  ||
      eventType == listGenericCount ||
      eventType == cvCreateCount ||
      eventType == expSetParamCount ||
      eventType == cliWaitCount ||
      eventType == cliPurgeDispatchQueueCount ||
      eventType == cliWaitForPrevCmdsCount ||
      eventType == cliWaitForOthersTermCount ||
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
      eventType == expCmd_InfoTotal ||
      eventType == expCmd_ViewTotal ||
      eventType == expCmd_C_ViewTotal ||
      eventType == cvClustersTotal ||
      eventType == cvInfoTotal ||
      eventType == detailBaseReportTotal ||
      eventType == detailTraceReportTotal ||
      eventType == detailCallStackReportTotal ||
      eventType == detailButterFlyReportTotal ||
      eventType == cliBasicInitTotal ||
      eventType == cliCmdLinePythonTotal ||
      eventType == cliExecuteCmdTotal ||
      eventType == cliCmd_EXT_CreateTotal ||
      eventType == cliWindowInitTotal ||
      eventType == cliWatcherTotal ||
      eventType == cliGuiLoadTotal ||
      eventType == expDetachTotal ||
      eventType == expDisableTotal  ||
      eventType == expEnableTotal  ||
      eventType == expFocusTotal  ||
      eventType == expPauseTotal  ||
      eventType == expRestoreTotal  ||
      eventType == expSaveTotal  ||
      eventType == listGenericTotal ||
      eventType == cvCreateTotal ||
      eventType == expSetParamTotal ||
      eventType == cliWaitTotal ||
      eventType == cliPurgeDispatchQueueTotal ||
      eventType == cliWaitForPrevCmdsTotal ||
      eventType == cliWaitForOthersTermTotal ||
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
      eventType == expCmd_InfoMax ||
      eventType == expCmd_ViewMax ||
      eventType == expCmd_C_ViewMax ||
      eventType == cvClustersMax ||
      eventType == cvInfoMax ||
      eventType == detailBaseReportMax ||
      eventType == detailTraceReportMax ||
      eventType == detailCallStackReportMax ||
      eventType == detailButterFlyReportMax ||
      eventType == cliBasicInitMax ||
      eventType == cliCmdLinePythonMax ||
      eventType == cliExecuteCmdMax ||
      eventType == cliCmd_EXT_CreateMax ||
      eventType == cliWindowInitMax ||
      eventType == cliWatcherMax ||
      eventType == cliGuiLoadMax ||
      eventType == expDetachMax ||
      eventType == expDisableMax  ||
      eventType == expEnableMax  ||
      eventType == expFocusMax  ||
      eventType == expPauseMax  ||
      eventType == expRestoreMax  ||
      eventType == expSaveMax  ||
      eventType == listGenericMax ||
      eventType == cvCreateMax ||
      eventType == expSetParamMax ||
      eventType == cliWaitMax ||
      eventType == cliPurgeDispatchQueueMax ||
      eventType == cliWaitForPrevCmdsMax ||
      eventType == cliWaitForOthersTermMax ||
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
      eventType == expCmd_InfoMin ||
      eventType == expCmd_ViewMin ||
      eventType == expCmd_C_ViewMin ||
      eventType == cvClustersMin ||
      eventType == cvInfoMin ||
      eventType == detailBaseReportMin ||
      eventType == detailTraceReportMin ||
      eventType == detailCallStackReportMin ||
      eventType == detailButterFlyReportMin ||
      eventType == cliBasicInitMin ||
      eventType == cliCmdLinePythonMin ||
      eventType == cliExecuteCmdMin ||
      eventType == cliCmd_EXT_CreateMin ||
      eventType == cliWindowInitMin ||
      eventType == cliWatcherMin ||
      eventType == cliGuiLoadMin ||
      eventType == expDetachMin ||
      eventType == expDisableMin  ||
      eventType == expEnableMin  ||
      eventType == expFocusMin  ||
      eventType == expPauseMin  ||
      eventType == expRestoreMin  ||
      eventType == expSaveMin  ||
      eventType == listGenericMin ||
      eventType == cvCreateMin ||
      eventType == expSetParamMin ||
      eventType == cliWaitMin ||
      eventType == cliPurgeDispatchQueueMin ||
      eventType == cliWaitForPrevCmdsMin ||
      eventType == cliWaitForOthersTermMin ||
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
  std::cerr << "BAD exit findTotalEvent inputCountEvent = " << inputCountEvent << " \n ";
  return perfTableNullTotal;
}

SS_Timings::CLIPerformanceDataEvents SS_Timings::findMinEvent( CLIPerformanceDataEvents inputCountEvent)
{
    for(unsigned i = 0; MaxMinTable[i].count_cli_event != perfTableNullCount; ++i) {
#if 0
      std::cerr << "in findMinEvent, i = " << i << " \n ";
      std::cerr << "MaxMinTable[i].count_cli_event = " << MaxMinTable[i].count_cli_event << " \n ";
      std::cerr << "inputCountEvent = " << inputCountEvent << " \n ";
#endif
      if (MaxMinTable[i].count_cli_event == inputCountEvent) {
         return MaxMinTable[i].min_cli_event;
      }
    }
  std::cerr << "BAD exit findMinEvent inputCountEvent = " << inputCountEvent << " \n ";
  return perfTableNullMin;
}

SS_Timings::CLIPerformanceDataEvents SS_Timings::findMaxEvent( CLIPerformanceDataEvents inputCountEvent)
{
    for(unsigned i = 0; MaxMinTable[i].count_cli_event != perfTableNullCount; ++i) {
      if (MaxMinTable[i].count_cli_event == inputCountEvent) {
         return MaxMinTable[i].max_cli_event;
      }
    }
  std::cerr << "BAD exit findMaxEvent inputCountEvent = " << inputCountEvent << "  \n";
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

        { SS_Timings::expAttachStart, "Start Time the last expAttach command started", false },
        { SS_Timings::expAttachCount, "Number of expAttach commands being timed", false },
        { SS_Timings::expAttachMax, "Maximum Time spent in expAttach for one call", false },
        { SS_Timings::expAttachMin, "Minimum Time spent in expAttach for one call", false },
        { SS_Timings::expAttachTotal, "Total Time expAttach command time possibly over multiple calls", false },
        { SS_Timings::expAttachEnd, "End Time the last expAttach command ended", false },
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
        { SS_Timings::expAttach_FW_createProcess_Count,"Number of expAttach sub-task measurement for calls to the FW for creating a process (included in resolve the target list)", true},
        { SS_Timings::expAttach_FW_createProcess_Max,"Maximum Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Min,"Minimum Time expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expAttach_FW_createProcess_Total,"Total Time expAttach sub-task measurement for calls to the FW for creating a process (included in resolve the target list)", true},
        { SS_Timings::expAttach_FW_createProcess_End,"End Time for expAttach sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreateStart, "Start Time the last expCreate command started", false },
        { SS_Timings::expCreateCount, "Number of expCreate commands being timed", false },
        { SS_Timings::expCreateMax, "Maximum Time spent in expCreate for one call", false },
        { SS_Timings::expCreateMin, "Minimum Time spent in expCreate for one call", false },
        { SS_Timings::expCreateTotal, "Total Time expCreate command time possibly over multiple calls", false },
        { SS_Timings::expCreateEnd, "End Time the last expCreate command ended", false },
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
        { SS_Timings::expCreate_FW_createProcess_Count,"Number of expCreate sub-task measurement for calls to the FW for creating a process (included in resolve the target list)", true},
        { SS_Timings::expCreate_FW_createProcess_Max,"Maximum Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Min,"Minimum Time expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_FW_createProcess_Total,"Total Time expCreate sub-task measurement for calls to the FW for creating a process (included in resolve the target list)", true},
        { SS_Timings::expCreate_FW_createProcess_End,"End Time for expCreate sub-task measurement for calls to the FW for creating a process", true},
        { SS_Timings::expCreate_focusExp_Start,"Start Time expCreate sub-task measurement focus experiment and bookkeeping Start time", true},
        { SS_Timings::expCreate_focusExp_Count,"Number of expCreate sub-task measurement focus experiment and bookkeeping # times", true},
        { SS_Timings::expCreate_focusExp_Max,"Maximum Time expCreate sub-task measurement focus experiment and bookkeeping maximum time", true},
        { SS_Timings::expCreate_focusExp_Min,"Minimum Time expCreate sub-task measurement focus experiment and bookkeeping minimum time", true},
        { SS_Timings::expCreate_focusExp_Total,"Total Time expCreate sub-task measurement focus experiment and bookkeeping Total time", true},
        { SS_Timings::expCreate_focusExp_End,"End Time for expCreate sub-task measurement focus experiment and bookkeeping End time", true},
        { SS_Timings::expCompareStart, "Start Time the last expCompare command started" , false},
        { SS_Timings::expCompareCount, "Number of expCompare commands being timed" , false},
        { SS_Timings::expCompareMax, "Maximum time spent in expCompare for one call" , false},
        { SS_Timings::expCompareMin, "Minimum time spent in expCompare for one call" , false},
        { SS_Timings::expCompareTotal, "Total expCompare command time possibly over multiple calls" , false},
        { SS_Timings::expCompareEnd, "End Time the last expCompare command ended" , false},
        { SS_Timings::expDetachStart, "Start Time the last expDetach command started" , false},
        { SS_Timings::expDetachCount, "Number of expDetach commands being timed" , false},
        { SS_Timings::expDetachMax, "Maximum time spent in expDetach for one call" , false},
        { SS_Timings::expDetachMin, "Minimum time spent in expDetach for one call" , false},
        { SS_Timings::expDetachTotal, "Total expDetach command time possibly over multiple calls" , false},
        { SS_Timings::expDetachEnd, "End Time the last expDetach command ended" , false},
        { SS_Timings::expDisableStart, "Start Time the last expDisable command started" , false},
        { SS_Timings::expDisableCount, "Number of expDisable commands being timed" , false},
        { SS_Timings::expDisableMax, "Maximum time spent in expDisable for one call" , false},
        { SS_Timings::expDisableMin, "Minimum time spent in expDisable for one call" , false},
        { SS_Timings::expDisableTotal, "Total expDisable command time possibly over multiple calls" , false},
        { SS_Timings::expDisableEnd, "End Time the last expDisable command ended" , false},
        { SS_Timings::expEnableStart, "Start Time the last expEnable command started" , false},
        { SS_Timings::expEnableCount, "Number of expEnable commands being timed" , false},
        { SS_Timings::expEnableMax, "Maximum time spent in expEnable for one call" , false},
        { SS_Timings::expEnableMin, "Minimum time spent in expEnable for one call" , false},
        { SS_Timings::expEnableTotal, "Total expEnable command time possibly over multiple calls" , false},
        { SS_Timings::expEnableEnd, "End Time the last expEnable command ended" , false},
        { SS_Timings::expFocusStart, "Start Time the last expFocus command started" , false},
        { SS_Timings::expFocusCount, "Number of expFocus commands being timed" , false},
        { SS_Timings::expFocusMax, "Maximum time spent in expFocus for one call" , false},
        { SS_Timings::expFocusMin, "Minimum time spent in expFocus for one call" , false},
        { SS_Timings::expFocusTotal, "Total expFocus command time possibly over multiple calls" , false},
        { SS_Timings::expFocusEnd, "End Time the last expFocus command ended" , false},
        { SS_Timings::expGoStart, "Start Time the last expGo command started" , false},
        { SS_Timings::expGoCount, "Number of expGo commands being timed" , false},
        { SS_Timings::expGoMax, "Maximum time spent in expGo for one call" , false},
        { SS_Timings::expGoMin, "Minimum time spent in expGo for one call" , false},
        { SS_Timings::expGoTotal, "Total expGo command time possibly over multiple calls" , false},
        { SS_Timings::expGoEnd, "End Time the last expGo command ended" , false},
        { SS_Timings::expPauseStart, "Start Time the last expPause command started" , false},
        { SS_Timings::expPauseCount, "Number of expPause commands being timed" , false},
        { SS_Timings::expPauseMax, "Maximum time spent in expPause for one call" , false},
        { SS_Timings::expPauseMin, "Minimum time spent in expPause for one call" , false},
        { SS_Timings::expPauseTotal, "Total expPause command time possibly over multiple calls" , false},
        { SS_Timings::expPauseEnd, "End Time the last expPause command ended" , false},
        { SS_Timings::expRestoreStart, "Start Time the last expRestore command started" , false},
        { SS_Timings::expRestoreCount, "Number of expRestore commands being timed" , false},
        { SS_Timings::expRestoreMax, "Maximum time spent in expRestore for one call" , false},
        { SS_Timings::expRestoreMin, "Minimum time spent in expRestore for one call" , false},
        { SS_Timings::expRestoreTotal, "Total expRestore command time possibly over multiple calls" , false},
        { SS_Timings::expRestoreEnd, "End Time the last expRestore command ended" , false},
        { SS_Timings::expSaveStart, "Start Time the last expSave command started" , false},
        { SS_Timings::expSaveCount, "Number of expSave commands being timed" , false},
        { SS_Timings::expSaveMax, "Maximum time spent in expSave for one call" , false},
        { SS_Timings::expSaveMin, "Minimum time spent in expSave for one call" , false},
        { SS_Timings::expSaveTotal, "Total expSave command time possibly over multiple calls" , false},
        { SS_Timings::expSaveEnd, "End Time the last expSave command ended" , false},
        { SS_Timings::expSetParamStart, "Start Time the last expSetParam command started" , false},
        { SS_Timings::expSetParamCount, "Number of expSetParam commands being timed" , false},
        { SS_Timings::expSetParamMax, "Maximum time spent in expSetParam for one call" , false},
        { SS_Timings::expSetParamMin, "Minimum time spent in expSetParam for one call" , false},
        { SS_Timings::expSetParamTotal, "Total expSetParam command time possibly over multiple calls" , false},
        { SS_Timings::expSetParamEnd, "End Time the last expSetParam command ended" , false},
        { SS_Timings::expCmd_InfoStart, "Start Time the last info command started" , false},
        { SS_Timings::expCmd_InfoCount, "Number of info commands being timed" , false},
        { SS_Timings::expCmd_InfoMax, "Maximum time spent in the info command for one call" , false},
        { SS_Timings::expCmd_InfoMin, "Minimum time spent in the info command for one call" , false},
        { SS_Timings::expCmd_InfoTotal, "Total info command time possibly over multiple calls" , false},
        { SS_Timings::expCmd_InfoEnd, "Time the last info command ended" , false},
        { SS_Timings::expCmd_C_ViewStart, "Start Time the last custom view command started" , false},
        { SS_Timings::expCmd_C_ViewCount, "Number of custom view commands being timed" , false},
        { SS_Timings::expCmd_C_ViewMax, "Maximum time spent in the custom view command for one call" , false},
        { SS_Timings::expCmd_C_ViewMin, "Minimum time spent in the custom view command for one call" , false},
        { SS_Timings::expCmd_C_ViewTotal, "Total custom view command time possibly over multiple calls" , false},
        { SS_Timings::expCmd_C_ViewEnd, "Time the last custom view command ended" , false},
        { SS_Timings::expCmd_ViewStart, "Start Time the last view command started" , false},
        { SS_Timings::expCmd_ViewCount, "Number of view commands being timed" , false},
        { SS_Timings::expCmd_ViewMax, "Maximum time spent in the view command for one call" , false},
        { SS_Timings::expCmd_ViewMin, "Minimum time spent in the view command for one call" , false},
        { SS_Timings::expCmd_ViewTotal, "Total view command time possibly over multiple calls" , false},
        { SS_Timings::expCmd_ViewEnd, "Time the last view command ended" , false},
        { SS_Timings::cvClustersStart, "Start Time the last cvClusters command started" , false},
        { SS_Timings::cvClustersCount, "Number of cvClusters commands being timed" , false},
        { SS_Timings::cvClustersMax, "Maximum time spent in the cvClusters command for one call" , false},
        { SS_Timings::cvClustersMin, "Minimum time spent in the cvClusters command for one call" , false},
        { SS_Timings::cvClustersTotal, "Total cvClusters command time possibly over multiple calls" , false},
        { SS_Timings::cvClustersEnd, "Time the last cvClusters command ended" , false},
        { SS_Timings::cvCreateStart, "Start Time the last cvCreate command started" , false},
        { SS_Timings::cvCreateCount, "Number of cvCreate commands being timed" , false},
        { SS_Timings::cvCreateMax, "Maximum time spent in the cvCreate command for one call" , false},
        { SS_Timings::cvCreateMin, "Minimum time spent in the cvCreate command for one call" , false},
        { SS_Timings::cvCreateTotal, "Total cvCreate command time possibly over multiple calls" , false},
        { SS_Timings::cvCreateEnd, "Time the last cvCreate command ended" , false},
        { SS_Timings::cvInfoStart, "Time the last cvInfo command started" , false},
        { SS_Timings::cvInfoCount, "Number of cvInfo commands being timed" , false},
        { SS_Timings::cvInfoMax, "Maximum time spent in the cvInfo command for one call" , false},
        { SS_Timings::cvInfoMin, "Minimum time spent in the cvInfo command for one call" , false},
        { SS_Timings::cvInfoTotal, "Total cvInfo command time possibly over multiple calls" , false},
        { SS_Timings::cvInfoEnd, "End Time the last cvInfo command ended" , false},
        { SS_Timings::viewGenerationStart, "Start Time the CLI started the generation of a report", false },
        { SS_Timings::viewGenerationCount, "Number of times view generation was/is being timed", false },
        { SS_Timings::viewGenerationMax, "Maximum Time spent in view generation for one call", false },
        { SS_Timings::viewGenerationMin, "Minimum Time spent in view generation for one call", false },
        { SS_Timings::viewGenerationTotal, "Total Time spent in view generation, possibly over multiple calls", false },
        { SS_Timings::viewGenerationEnd, "End Time the CLI Completed generation of a report", false },
        { SS_Timings::detailBaseReportStart, "Start Time the last detailBaseReport generation started" , false},
        { SS_Timings::detailBaseReportCount, "Number of detailBaseReport generations being timed" , false},
        { SS_Timings::detailBaseReportMax, "Maximum time spent in detailed base report generation for one call" , false},
        { SS_Timings::detailBaseReportMin, "Minimum time spent in detailed base report generation for one call" , false},
        { SS_Timings::detailBaseReportTotal, "Total detailBaseReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailBaseReportEnd, "End Time when the last detailBaseReport generation ended" , false},
        { SS_Timings::detailTraceReportStart, "Start Time the last detailTraceReport generation started" , false},
        { SS_Timings::detailTraceReportCount, "Number of detailTraceReport generations being timed" , false},
        { SS_Timings::detailTraceReportMax, "Maximum time spent in detailed trace report generation for one call" , false},
        { SS_Timings::detailTraceReportMin, "Minimum time spent in detailed trace report generation for one call" , false},
        { SS_Timings::detailTraceReportTotal, "Total detailTraceReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailTraceReportEnd, "End Time when the last detailTraceReport generation ended" , false},
        { SS_Timings::detailCallStackReportStart, "Start Time the last detailCallStackReport generation started" , false},
        { SS_Timings::detailCallStackReportCount, "Number of detailCallStackReport generations being timed" , false},
        { SS_Timings::detailCallStackReportMax, "Maximum time spent in detailed callstack report generation for one call" , false},
        { SS_Timings::detailCallStackReportMin, "Minimum time spent in detailed callstack report generation for one call" , false},
        { SS_Timings::detailCallStackReportTotal, "Total detailCallStackReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailCallStackReportEnd, "End Time when the last detailCallStackReport generation ended" , false},
        { SS_Timings::detailButterFlyReportStart, "Start Time the last detailButterFlyReport generation started" , false},
        { SS_Timings::detailButterFlyReportCount, "Number of detailButterFlyReport generations being timed" , false},
        { SS_Timings::detailButterFlyReportMax, "Maximum time spent in detailed butterfly report generation for one call" , false},
        { SS_Timings::detailButterFlyReportMin, "Minimum time spent in detailed butterfly report generation for one call" , false},
        { SS_Timings::detailButterFlyReportTotal, "Total detailButterFlyReport generation time possibly over multiple calls" , false},
        { SS_Timings::detailButterFlyReportEnd, "End Time when the last detailButterFlyReport generation ended" , false},
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
        { SS_Timings::cliCmd_EXT_CreateStart, "Start Time for when the CLI executes a command execution started" , true},
        { SS_Timings::cliCmd_EXT_CreateCount, "Number of times the CLI executes a command was/is being timed" , true},
        { SS_Timings::cliCmd_EXT_CreateMax, "Maximum time spent in the CLI executes a commandfor one call" , true},
        { SS_Timings::cliCmd_EXT_CreateMin, "Minimum time spent in the CLI executes a command for one call" , true},
        { SS_Timings::cliCmd_EXT_CreateTotal, "Total time spent in CLI executes a command, possibly over multiple calls" , true},
        { SS_Timings::cliCmd_EXT_CreateEnd, "End Time when CLI executes a command ended" , true},
        { SS_Timings::cliCmdLinePythonStart, "Start Time for when the CLI command line and python init execution started" , true},
        { SS_Timings::cliCmdLinePythonCount, "Number of times the CLI command line and python init was/is being timed" , true},
        { SS_Timings::cliCmdLinePythonMax, "Maximum time spent in the CLI command line and python init for one call" , true},
        { SS_Timings::cliCmdLinePythonMin, "Minimum time spent in the CLI command line and python init for one call" , true},
        { SS_Timings::cliCmdLinePythonTotal, "Total time spent in CLI command line and python init, possibly over multiple calls" , true},
        { SS_Timings::cliCmdLinePythonEnd, "End Time when CLI command line and python init ended" , true},
        { SS_Timings::cliExecuteCmdStart, "Start Time for when the CLI loops through executing commands execution started" , true},
        { SS_Timings::cliExecuteCmdCount, "Number of times the CLI loops through executing commands was/is being timed" , true},
        { SS_Timings::cliExecuteCmdMax, "Maximum time spent in CLI loops through executing commands for one call" , true},
        { SS_Timings::cliExecuteCmdMin, "Minimum time spent in CLI loops through executing commands for one call" , true},
        { SS_Timings::cliExecuteCmdTotal, "Total time spent in CLI loops through executing commands possibly over multiple calls" , true},
        { SS_Timings::cliExecuteCmdEnd, "End Time when CLI loops through executing commands ended" , true},
        { SS_Timings::cliPurgeDispatchQueueStart, "Start Time for routine that purges the cli command queue", true},
        { SS_Timings::cliPurgeDispatchQueueCount, "Number of times the CLI routine that waits for other cli commands to terminate and exit was/is being timed" , true},
        { SS_Timings::cliPurgeDispatchQueueMax, "Maximum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call" , true},
        { SS_Timings::cliPurgeDispatchQueueMin, "Minimum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call" , true},
        { SS_Timings::cliPurgeDispatchQueueTotal, "Total time spent in CLI routine that waits for other cli commands to terminate and exit, possibly over multiple calls" , true},
        { SS_Timings::cliPurgeDispatchQueueEnd, "End Time for routine that waits for other cli commands to terminate", true},
        { SS_Timings::cliWaitForOthersTermStart, "Start Time for routine that waits for other cli commands to terminate", true},
        { SS_Timings::cliWaitForOthersTermCount, "Number of times the CLI routine that waits for other cli commands to terminate and exit was/is being timed" , true},
        { SS_Timings::cliWaitForOthersTermMax, "Maximum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call" , true},
        { SS_Timings::cliWaitForOthersTermMin, "Minimum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call" , true},
        { SS_Timings::cliWaitForOthersTermTotal, "Total time spent in CLI routine that waits for other cli commands to terminate and exit, possibly over multiple calls" , true},
        { SS_Timings::cliWaitForOthersTermEnd, "End Time when CLI routine that waits for other cli commands to terminate and exit ended" , true},
        { SS_Timings::cliWatcherStart, "Start Time for when the CLI watcher loop execution started" , true},
        { SS_Timings::cliWaitForPrevCmdsStart, "Start Time for routine that waits for previous cli pthreads to finish", true},
        { SS_Timings::cliWaitForPrevCmdsCount, "Number of times the CLI routine that waits for previous cli pthreads to finis was/is being timed" , true},
        { SS_Timings::cliWaitForPrevCmdsMax, "Maximum time spent in the CLI routine that waits for previous cli pthreads to finish for one call" , true},
        { SS_Timings::cliWaitForPrevCmdsMin, "Minimum time spent in the CLI routine that waits for previous cli pthreads to finish for one call" , true},
        { SS_Timings::cliWaitForPrevCmdsTotal, "Total time spent in CLI routine that waits for previous cli pthreads to finish, possibly over multiple calls" , true},
        { SS_Timings::cliWaitForPrevCmdsEnd, "End Time when CLI routine that waits for previous cli pthreads to finish ended" , true},
        { SS_Timings::cliWatcherStart, "Start Time for when the CLI watcher loop execution started" , true},
        { SS_Timings::cliWatcherCount, "Number of times the CLI watcher loop was/is being timed" , true},
        { SS_Timings::cliWatcherMax, "Maximum time spent in the CLI watcher loop for one call" , true},
        { SS_Timings::cliWatcherMin, "Minimum time spent in the CLI watcher loop for one call" , true},
        { SS_Timings::cliWatcherTotal, "Total time spent in CLI watcher loop, possibly over multiple calls" , true},
        { SS_Timings::cliWatcherEnd, "End Time when CLI watcher loop ended" , true},
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
        { SS_Timings::cliWaitStart, "Start Time for when the CLI Wait command execution started" , false},
        { SS_Timings::cliWaitCount, "Number of times the CLI Wait command was/is being timed" , false},
        { SS_Timings::cliWaitMax, "Maximum time spent in the CLI Wait command for one call" , false},
        { SS_Timings::cliWaitMin, "Minimum time spent in the CLI Wait command for one call" , false},
        { SS_Timings::cliWaitTotal, "Total time spent in CLI Wait command, possibly over multiple calls" , false},
        { SS_Timings::cliWaitEnd, "End Time when CLI Wait command ended" , false},
        { SS_Timings::listGenericStart, "Start Time the last listGeneric command started" , false},
        { SS_Timings::listGenericCount, "Number of listGeneric commands being timed" , false},
        { SS_Timings::listGenericMax, "Maximum time spent in listGeneric for one call" , false},
        { SS_Timings::listGenericMin, "Minimum time spent in listGeneric for one call" , false},
        { SS_Timings::listGenericTotal, "Total listGeneric command time possibly over multiple calls" , false},
        { SS_Timings::listGenericEnd, "End Time the last listGeneric command ended" , false},
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

