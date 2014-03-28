////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006-2014 The Krell Institue. All Rights Reserved.
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

/** @file
 *
 * Definition of the SS_Timings class which is used for internal performance timing.
 *
 */



#ifndef SS_TIMINGS_H
#define SS_TIMINGS_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>

#include "SS_Input_Manager.hxx"
#include "Queries.hxx"


  /**
   * Class for Performance data events and performance timing for the CLI
   *
   * Contains public and private members to support the gathering of CLI
   * self performance analysis/timing.
   */
class SS_Timings 
{
  public: 
    // The default constructor.  
    SS_Timings( );
    // The default constructor.  Calls initialize() with filename.
    SS_Timings( char *filename );

    // The closes the file.
    ~SS_Timings();

   // Analyze performance data and print results
   void CLIPerformanceStatistics();

   /** Function to decide if performance debugging for this class is enabled. */
   bool is_debug_perf_enabled() ;

   /** Function to decide if performance debugging is processing an expAttach command. */
   bool in_expAttach() ;

   /** Function to set whether or not were are processing an expAttach command. */
   void in_expAttach(bool flag) ;

   /** Function to decide if performance debugging is processing an expCreate command. */
   bool in_expCreate() ;

   /** Function to set whether or not were are processing an expCreate command. */
   void in_expCreate(bool flag) ;


#if 0
  enum CLI_PerformanceSubEvents {
      Start,
      Count, 
      Min,
      Max, 
      Total, 
      End  
   }

  enum CLI_PerformanceDataEvents {
      viewGenerationPEvent, /** < Time the CLI Started Generation of a report. */
      expAttachPEvent, /** < Time the last expAttach Command Started */ 
      expCreatePEvent, /** < Time the last expCreate Command Started */ 
      expComparePEvent, /** < Time the last expCompare Command Started */ 
      cvClustersPEvent, /** < Time the last cvClusters Command Started */ 
      cvInfoPEvent, /** < Time the last cvInfo Command Started */ 
      detailBaseReportPEvent, /** < Time the last detailBaseReport generation Started */ 
      detailTraceReportPEvent, /** < Time the last detailTraceReport generation Started */ 
      detailCallStackReportPEvent, /** < Time the last detailCallStackReport generation Started */ 
      detailButterFlyReportPEvent,  /** Time the last detailButterFlyReport generation Started */ 
      entireCLIPEvent              /**< The CLI Start time. */
  }

        /** Performance data for this class instantiation. */
   std::vector<CLI_PerformanceDataEvents, CLI_PerformanceSubEvents> perf_event_table;
#endif

  /**
   * Performance data events enumeration.
   *
   * Enumeration defining all the different event times that are recorded
   * as part of the per-process performance data.
   */
  enum CLIPerformanceDataEvents {
      expAttachStart, /** < Time the last expAttach Command Started */ 
      expAttachCount, /** < Number of expAttach commands being timed */ 
      expAttachMax, /** <Maximum time spent in expAttach command for one call */
      expAttachMin, /** <Minimum time spent in expAttach command for one call */
      expAttachTotal, /** < Total expAttach command time possibly over multiple calls */ 
      expAttachEnd, /** < Time the last expAttach Command Ended */ 
      expAttach_resolveTargetList_Start, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_resolveTargetList_Count, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_resolveTargetList_Max, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_resolveTargetList_Min, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_resolveTargetList_Total, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_resolveTargetList_End, /** < expAttach sub-task measurement process the target list and attach if mpi*/
      expAttach_getCollector_Start, /** < Start Time for expAttach sub-task measurement for getting the collector info */
      expAttach_getCollector_Count, /** < Number of times expAttach sub-task measurement for getting the collector info */
      expAttach_getCollector_Max, /** < Maximum Time for expAttach sub-task measurement for getting the collector info  */
      expAttach_getCollector_Min, /** < Minimum Time for expAttach sub-task measurement for getting the collector info */
      expAttach_getCollector_Total, /** < Total Time for expAttach sub-task measurement for getting the collector info */
      expAttach_getCollector_End, /** < End Time for expAttach sub-task measurement for getting the collector info */
      expAttach_linkThreads_Start, /** < Start Time for expAttach sub-task measurement for linking threads to the collectors */
      expAttach_linkThreads_Count, /** < Number of times expAttach sub-task measurement for linking threads to the collectors */
      expAttach_linkThreads_Max, /** < Maximum Time for expAttach sub-task measurement for linking threads to the collectors  */
      expAttach_linkThreads_Min, /** < Minimum Time for expAttach sub-task measurement for linking threads to the collectors */
      expAttach_linkThreads_Total, /** < Total Time for expAttach sub-task measurement for linking threads to the collectors */
      expAttach_linkThreads_End, /** < End Time for expAttach sub-task measurement for linking threads to the collectors */
      expAttach_FW_createProcess_Start, /** < Start Time for expAttach sub-task measurement for FW calls to process create */
      expAttach_FW_createProcess_Count, /** < Number of times expAttach sub-task measurement for FW calls to process create */
      expAttach_FW_createProcess_Max, /** < Maximum Time for expAttach sub-task measurement for FW calls to process create */
      expAttach_FW_createProcess_Min, /** < Minimum Time for expAttach sub-task measurement for FW calls to process create */
      expAttach_FW_createProcess_Total, /** < Total Time for expAttach sub-task measurement for FW calls to process create */
      expAttach_FW_createProcess_End, /** < End Time for expAttach sub-task measurement for FW calls to process create */
      expCreateStart, /** < Time the last expCreate Command Started */ 
      expCreateCount, /** < Number of expCreate commands being timed */ 
      expCreateMax, /** <Maximum time spent in expCreate command for one call */
      expCreateMin, /** <Minimum time spent in expCreate command for one call */
      expCreateTotal, /** < Total expCreate command time possibly over multiple calls */ 
      expCreateEnd, /** < Time the last expCreate Comman Ended */ 
      expCreate_wait_Start, /** <expCreate sub-task measurement waittime Start */
      expCreate_wait_Count, /** <expCreate sub-task measurement waittime # times */
      expCreate_wait_Max,/** <expCreate sub-task measurement waittime Max */
      expCreate_wait_Min,/** <expCreate sub-task measurement waittime Min */
      expCreate_wait_Total,/** <expCreate sub-task measurement waittime Total */
      expCreate_wait_End,/** <expCreate sub-task measurement waittime End */
      expCreate_allocExp_Start,/** <expCreate sub-task measurement experiment obj creation Start time */
      expCreate_allocExp_Count,/** <expCreate sub-task measurement experiment obj creation # times */
      expCreate_allocExp_Max,/** <expCreate sub-task measurement experiment obj creation maximum time */
      expCreate_allocExp_Min,/** <expCreate sub-task measurement experiment obj creation minimum time */
      expCreate_allocExp_Total,/** <expCreate sub-task measurement experiment obj creation Total time */
      expCreate_allocExp_End,/** <expCreate sub-task measurement experiment obj creation End time */
      expCreate_resolveTargetList_Start, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_resolveTargetList_Count, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_resolveTargetList_Max, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_resolveTargetList_Min, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_resolveTargetList_Total, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_resolveTargetList_End, /** < expCreate sub-task measurement process the target list and attach if mpi*/
      expCreate_getCollector_Start, /** < Start Time for expCreate sub-task measurement for getting the collector info */
      expCreate_getCollector_Count, /** < Number of times expCreate sub-task measurement for getting the collector info */
      expCreate_getCollector_Max, /** < Maximum Time for expCreate sub-task measurement for getting the collector info  */
      expCreate_getCollector_Min, /** < Minimum Time for expCreate sub-task measurement for getting the collector info */
      expCreate_getCollector_Total, /** < Total Time for expCreate sub-task measurement for getting the collector info */
      expCreate_getCollector_End, /** < End Time for expCreate sub-task measurement for getting the collector info */
      expCreate_linkThreads_Start, /** < Start Time for expCreate sub-task measurement for linking threads to the collectors */
      expCreate_linkThreads_Count, /** < Number of times expCreate sub-task measurement for linking threads to the collectors */
      expCreate_linkThreads_Max, /** < Maximum Time for expCreate sub-task measurement for linking threads to the collectors  */
      expCreate_linkThreads_Min, /** < Minimum Time for expCreate sub-task measurement for linking threads to the collectors */
      expCreate_linkThreads_Total, /** < Total Time for expCreate sub-task measurement for linking threads to the collectors */
      expCreate_linkThreads_End, /** < End Time for expCreate sub-task measurement for linking threads to the collectors */
      expCreate_FW_createProcess_Start, /** < Start Time for expCreate sub-task measurement for FW calls to process create */
      expCreate_FW_createProcess_Count, /** < Number of times expCreate sub-task measurement for FW calls to process create */
      expCreate_FW_createProcess_Max, /** < Maximum Time for expCreate sub-task measurement for FW calls to process create */
      expCreate_FW_createProcess_Min, /** < Minimum Time for expCreate sub-task measurement for FW calls to process create */
      expCreate_FW_createProcess_Total, /** < Total Time for expCreate sub-task measurement for FW calls to process create */
      expCreate_FW_createProcess_End, /** < End Time for expCreate sub-task measurement for FW calls to process create */
      expCreate_focusExp_Start, /** < expCreate sub-task measurement focus experiment and bookkeeping Start time */
      expCreate_focusExp_Count, /** < expCreate sub-task measurement focus experiment and bookkeeping # times */
      expCreate_focusExp_Max, /** < expCreate sub-task measurement focus experiment and bookkeeping maximum time */
      expCreate_focusExp_Min, /** < expCreate sub-task measurement focus experiment and bookkeeping minimum time */
      expCreate_focusExp_Total, /** < expCreate sub-task measurement focus experiment and bookkeeping Total time */
      expCreate_focusExp_End, /** < expCreate sub-task measurement focus experiment and bookkeeping End time */
      expCompareStart, /** < Time the last expCompare Command Started */ 
      expCompareCount, /** < Number of expCompare commands being timed */ 
      expCompareMax, /** <Maximum time spent in expCompare command for one call */
      expCompareMin, /** <Minimum time spent in expCompare command for one call */
      expCompareTotal, /** < Total expCompare command time possibly over multiple calls */ 
      expCompareEnd, /** < Time the last expCompare command Ended */ 
      expDetachStart, /** < Time the last expDetach command Started */ 
      expDetachCount, /** < Number of expDetach commands being timed */ 
      expDetachMax, /** <Maximum time spent in expDetach command for one call */
      expDetachMin, /** <Minimum time spent in expDetach command for one call */
      expDetachTotal, /** < Total expDetach command time possibly over multiple calls */ 
      expDetachEnd, /** < Time the last expDetach command Ended */ 
      expDisableStart, /** < Time the last expDisable command Started */ 
      expDisableCount, /** < Number of expDisable commands being timed */ 
      expDisableMax, /** <Maximum time spent in expDisable command for one call */
      expDisableMin, /** <Minimum time spent in expDisable command for one call */
      expDisableTotal, /** < Total expDisable command time possibly over multiple calls */ 
      expDisableEnd, /** < Time the last expDisable command Ended */ 
      expEnableStart, /** < Time the last expEnable command Started */ 
      expEnableCount, /** < Number of expEnable commands being timed */ 
      expEnableMax, /** <Maximum time spent in expEnable command for one call */
      expEnableMin, /** <Minimum time spent in expEnable command for one call */
      expEnableTotal, /** < Total expEnable command time possibly over multiple calls */ 
      expEnableEnd, /** < Time the last expEnable command Ended */ 
      expFocusStart, /** < Time the last expFocus command Started */ 
      expFocusCount, /** < Number of expFocus commands being timed */ 
      expFocusMax, /** <Maximum time spent in expFocus command for one call */
      expFocusMin, /** <Minimum time spent in expFocus command for one call */
      expFocusTotal, /** < Total expFocus command time possibly over multiple calls */ 
      expFocusEnd, /** < Time the last expFocus command Ended */ 
      expGoStart, /** < Time the last expGo Command Started */ 
      expGoCount, /** < Number of expGo commands being timed */ 
      expGoMax, /** <Maximum time spent in expGo command for one call */
      expGoMin, /** <Minimum time spent in expGo command for one call */
      expGoTotal, /** < Total expGo command time possibly over multiple calls */ 
      expGoEnd, /** < Time the last expGo Comman Ended */ 
      expPauseStart, /** < Time the last expPause Command Started */ 
      expPauseCount, /** < Number of expPause commands being timed */ 
      expPauseMax, /** <Maximum time spent in expPause command for one call */
      expPauseMin, /** <Minimum time spent in expPause command for one call */
      expPauseTotal, /** < Total expPause command time possibly over multiple calls */ 
      expPauseEnd, /** < Time the last expPause Comman Ended */ 
      expRestoreStart, /** < Time the last expRestore Command Started */ 
      expRestoreCount, /** < Number of expRestore commands being timed */ 
      expRestoreMax, /** <Maximum time spent in expRestore command for one call */
      expRestoreMin, /** <Minimum time spent in expRestore command for one call */
      expRestoreTotal, /** < Total expRestore command time possibly over multiple calls */ 
      expRestoreEnd, /** < Time the last expRestore Comman Ended */ 
      expSaveStart, /** < Time the last expSave Command Started */ 
      expSaveCount, /** < Number of expSave commands being timed */ 
      expSaveMax, /** <Maximum time spent in expSave command for one call */
      expSaveMin, /** <Minimum time spent in expSave command for one call */
      expSaveTotal, /** < Total expSave command time possibly over multiple calls */ 
      expSaveEnd, /** < Time the last expSave Comman Ended */ 
      expSetParamStart, /** < Time the last expSetParam Command Started */ 
      expSetParamCount, /** < Number of expSetParam commands being timed */ 
      expSetParamMax, /** <Maximum time spent in expSetParam command for one call */
      expSetParamMin, /** <Minimum time spent in expSetParam command for one call */
      expSetParamTotal, /** < Total expSetParam command time possibly over multiple calls */ 
      expSetParamEnd, /** < Time the last expSetParam Comman Ended */ 
      expCmd_InfoStart, /** < Time the last custom view Command Started */ 
      expCmd_InfoCount, /** < Number of custom view commands being timed */ 
      expCmd_InfoMax, /** <Maximum time spent in the custom view command for one call */
      expCmd_InfoMin, /** <Minimum time spent in the custom view command for one call */
      expCmd_InfoTotal, /** < Total custom view command time possibly over multiple calls */ 
      expCmd_InfoEnd, /** < Time the last custom view Command Ended */ 
      expCmd_C_ViewStart, /** < Time the last custom view Command Started */ 
      expCmd_C_ViewCount, /** < Number of custom view commands being timed */ 
      expCmd_C_ViewMax, /** <Maximum time spent in the custom view command for one call */
      expCmd_C_ViewMin, /** <Minimum time spent in the custom view command for one call */
      expCmd_C_ViewTotal, /** < Total custom view command time possibly over multiple calls */ 
      expCmd_C_ViewEnd, /** < Time the last custom view Command Ended */ 
      expCmd_ViewStart, /** < Time the last custom view Command Started */ 
      expCmd_ViewCount, /** < Number of custom view commands being timed */ 
      expCmd_ViewMax, /** <Maximum time spent in the custom view command for one call */
      expCmd_ViewMin, /** <Minimum time spent in the custom view command for one call */
      expCmd_ViewTotal, /** < Total custom view command time possibly over multiple calls */ 
      expCmd_ViewEnd, /** < Time the last custom view Command Ended */ 
      cvClustersStart, /** < Time the last cvClusters Command Started */ 
      cvClustersCount, /** < Number of cvClusters commands being timed */ 
      cvClustersMax, /** <Maximum time spent in the cvClusters command for one call */
      cvClustersMin, /** <Minimum time spent in the cvClusters command for one call */
      cvClustersTotal, /** < Total cvClusters command time possibly over multiple calls */ 
      cvClustersEnd, /** < Time the last cvClusters Command Ended */ 
      cvCreateStart, /** < Time the last cvCreate Command Started */ 
      cvCreateCount, /** < Number of cvCreate commands being timed */ 
      cvCreateMax, /** <Maximum time spent in the cvCreate command for one call */
      cvCreateMin, /** <Minimum time spent in the cvCreate command for one call */
      cvCreateTotal, /** < Total cvCreate command time possibly over multiple calls */ 
      cvCreateEnd, /** < Time the last cvCreate Command Ended */ 
      cvInfoStart, /** < Time the last cvInfo Command Started */ 
      cvInfoCount, /** < Number of cvInfo commands being timed */ 
      cvInfoMax, /** <Maximum time spent in the cvInfo command for one call */
      cvInfoMin, /** <Minimum time spent in the cvInfo command for one call */
      cvInfoTotal, /** < Total cvInfo command time possibly over multiple calls */ 
      cvInfoEnd, /** < Time the last cvInfo Command Ended */ 
      viewGenerationStart, /** < Time the CLI Started Generation of a report. */
      viewGenerationCount, /** < Number of times view generation was/is being timed. */ 
      viewGenerationMax, /** <Maximum time spent in view generation for one call */
      viewGenerationMin, /** <Minimum time spent in view generation for one call */
      viewGenerationTotal, /** < Total time spent in view generation, possibly over multiple calls. */
      viewGenerationEnd, /** < Time the CLI Completed Generation of a report. */
      detailBaseReportStart, /** < Time the last detailBaseReport generation Started */ 
      detailBaseReportCount, /** < Number of detailBaseReport generations being timed */ 
      detailBaseReportMax, /** <Maximum time spent in detailed base report generation for one call */
      detailBaseReportMin, /** <Minimum time spent in detailed base report generation for one call */
      detailBaseReportTotal, /** < Total detailBaseReport generation time possibly over multiple calls */ 
      detailBaseReportEnd, /** < Time the last detailBaseReport Generation Ended */ 
      detailTraceReportStart, /** < Time the last detailTraceReport generation Started */ 
      detailTraceReportCount, /** < Number of detailTraceReport generations being timed */ 
      detailTraceReportMax, /** <Maximum time spent in detailed trace report generation for one call */
      detailTraceReportMin, /** <Minimum time spent in detailed trace report generation for one call */
      detailTraceReportTotal, /** < Total detailTraceReport generation time possibly over multiple calls */ 
      detailTraceReportEnd, /** < Time the last detailTraceReport Generation Ended */ 
      detailCallStackReportStart, /** < Time the last detailCallStackReport generation Started */ 
      detailCallStackReportCount, /** < Number of detailCallStackReport generations being timed */ 
      detailCallStackReportMax, /** <Maximum time spent in detailed callstack report generation for one call */
      detailCallStackReportMin, /** <Minimum time spent in detailed callstack report generation for one call */
      detailCallStackReportTotal, /** < Total detailCallStackReport generation time possibly over multiple calls */ 
      detailCallStackReportEnd, /** < Time the last detailCallStackReport Generation Ended */
      detailButterFlyReportStart,  /** Time the last detailButterFlyReport generation Started */ 
      detailButterFlyReportCount, /** < Number of detailButterFlyReport generations being timed */ 
      detailButterFlyReportMax, /** <Maximum time spent in detailed butterfly report generation for one call */
      detailButterFlyReportMin, /** <Minimum time spent in detailed butterfly report generation for one call */
      detailButterFlyReportTotal, /** < Total detailButterFlyReport generation time possibly over multiple calls */ 
      detailButterFlyReportEnd, /** < Time the last detailButterFlyReport Generation Ended */ 
      cliAllStart,              /**< The CLI Start time. */
      cliAllCount, /** < Number of times the CLI was/is being timed  */
      cliAllMax, /** Maximum time spent in the CLI for one call */
      cliAllMin, /** Minimum time spent in the CLI for one call */
      cliAllTotal, /** < Total time spent in CLI, possibly over multiple calls. */ 
      cliAllEnd,  /**< The CLI End/Exit time. */
      cliBasicInitStart, /**< Start Time for CLI basic init. */
      cliBasicInitCount, /** < Number of times the CLI basic init was/is being timed  */
      cliBasicInitMax, /** Maximum time spent in CLI basic init for one call */
      cliBasicInitMin, /** Minimum time spent in CLI basic init for one call */
      cliBasicInitTotal, /** < Total Time in cli basic init. */ 
      cliBasicInitEnd,  /**< End Time for cli basic init. */
      cliCmd_EXT_CreateStart, /**< Start Time for CLI routine that creates a command. */
      cliCmd_EXT_CreateCount, /** < Number of times the CLI routine that creates a command  was/is being timed  */
      cliCmd_EXT_CreateMax, /** Maximum time spent in CLI routine that creates a command for one call */
      cliCmd_EXT_CreateMin, /** Minimum time spent in CLI routine that creates a command for one call */
      cliCmd_EXT_CreateTotal, /** < Total Time in cli routine that creates a command . */ 
      cliCmd_EXT_CreateEnd, /**< End Time for cli routine that creates a command. */
      cliCmdLinePythonStart, /**< Start Time for CLI command line and python init. */
      cliCmdLinePythonCount, /** < Number of times the CLI command line and python init was/is being timed  */
      cliCmdLinePythonMax, /** Maximum time spent in CLI command line and python init for one call */
      cliCmdLinePythonMin, /** Minimum time spent in CLI command line and python init for one call */
      cliCmdLinePythonTotal, /** < Total Time in cli command line and python init. */ 
      cliCmdLinePythonEnd, /**< End Time for cli command line and python init. */
      cliExecuteCmdStart, /**< Start Time for CLI looping through commands. */
      cliExecuteCmdCount, /** < Number of times the CLI looping through commands was/is being timed  */
      cliExecuteCmdMax, /** Maximum time spent in CLI looping through commands for one call */
      cliExecuteCmdMin, /** Minimum time spent in CLI looping through commands for one call */
      cliExecuteCmdTotal, /** < Total Time in cli looping through commands. */ 
      cliExecuteCmdEnd, /**< End Time for cli looping through commands. */
      cliWatcherStart, /**< Start Time for CLI watcher looping. */
      cliWatcherCount, /** < Number of times the CLI watcher looping was/is being timed  */
      cliWatcherMax, /** Maximum time spent in CLI watcher looping for one call */
      cliWatcherMin, /** Minimum time spent in CLI watcher looping for one call */
      cliWatcherTotal, /** < Total Time in cli watcher thread looping. */ 
      cliWatcherEnd, /**< End Time for cli watcher thread looping. */
      cliWindowInitStart, /**< Start Time for CLI window init. */
      cliWindowInitCount, /** < Number of times the CLI window init was/is being timed  */
      cliWindowInitMax, /** Maximum time spent in CLI window init for one call */
      cliWindowInitMin, /** Minimum time spent in CLI window init for one call */
      cliWindowInitTotal, /** < Total Time in cli window init. */ 
      cliWindowInitEnd,  /**< End Time for cli window init. */
      cliGuiLoadStart, /**< Start Time for CLI gui window load. */
      cliGuiLoadCount, /** < Number of times the CLI gui window load was/is being timed  */
      cliGuiLoadMax, /** Maximum time spent in CLI gui window load for one call */
      cliGuiLoadMin, /** Minimum time spent in CLI gui window load for one call */
      cliGuiLoadTotal, /** < Total Time in cli gui window load  */ 
      cliGuiLoadEnd, /**< End Time for cli gui window load  */
      cliPurgeDispatchQueueStart, /** < Start Time for routine that purges the cli command queue */
      cliPurgeDispatchQueueCount, /** < Number of times the CLI routine that waits for other cli commands to terminate and exit was/is being timed */
      cliPurgeDispatchQueueMax, /** < Maximum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call */
      cliPurgeDispatchQueueMin, /** < Minimum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call */
      cliPurgeDispatchQueueTotal, /** < Total time spent in CLI routine that waits for other cli commands to terminate and exit, possibly over multiple calls */
      cliPurgeDispatchQueueEnd, /** < End Time for routine that waits for other cli commands to terminate */
      cliWaitStart, /**< Start Time for CLI Wait command. */
      cliWaitCount, /** < Number of times the CLI Wait command was/is being timed  */
      cliWaitMax, /** Maximum time spent in CLI Wait command for one call */
      cliWaitMin, /** Minimum time spent in CLI Wait command for one call */
      cliWaitTotal, /** < Total Time in cli Wait command  */ 
      cliWaitEnd, /**< End Time for cli Wait command  */
      cliWaitForOthersTermStart, /** < Start Time for routine that waits for other cli commands to terminate */
      cliWaitForOthersTermCount, /** < Number of times the CLI routine that waits for other cli commands to terminate and exit was/is being timed */
      cliWaitForOthersTermMax, /** < Maximum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call */
      cliWaitForOthersTermMin, /** < Minimum time spent in the CLI routine that waits for other cli commands to terminate and exit for one call */
      cliWaitForOthersTermTotal, /** < Total time spent in CLI routine that waits for other cli commands to terminate and exit, possibly over multiple calls */
      cliWaitForOthersTermEnd, /** < End Time when CLI routine that waits for other cli commands to terminate and exit ended */
      cliWaitForPrevCmdsStart, /** < Start Time for routine that waits for previous cli pthreads to finish */
      cliWaitForPrevCmdsCount, /** < Number of times the CLI routine that waits for previous cli pthreads to finis was/is being timed */
      cliWaitForPrevCmdsMax, /** < Maximum time spent in the CLI routine that waits for previous cli pthreads to finish for one call */
      cliWaitForPrevCmdsMin, /** < Minimum time spent in the CLI routine that waits for previous cli pthreads to finish for one call */
      cliWaitForPrevCmdsTotal, /** < Total time spent in CLI routine that waits for previous cli pthreads to finish, possibly over multiple calls */
      cliWaitForPrevCmdsEnd, /** < End Time when CLI routine that waits for previous cli pthreads to finish ended */

      listGenericStart, /**< Start Time for processing the last list generic command execution. */
      listGenericCount, /** < Number of times the list generic command was/is being timed  */
      listGenericMax, /** Maximum time spent in processing list generic commands for one call */
      listGenericMin, /** Minimum time spent in processing list generic commands for one call */
      listGenericTotal, /** < Total Time in processing list generic commands  */ 
      listGenericEnd, /**< End Time for last list generic command execution */
      perfTableNullStart, /** < dummy entry */
      perfTableNullCount, /** < dummy entry */
      perfTableNullMax, /** < dummy entry */
      perfTableNullMin, /** < dummy entry */
      perfTableNullTotal, /** < dummy entry */
      perfTableNullEnd /** < dummy entry */

  };
        /** Performance data for this class instantiation. */
   std::map<CLIPerformanceDataEvents, Time> cli_perf_data;

        /** Performance count information for this class instantiation. */
   std::map<CLIPerformanceDataEvents, uint64_t > cli_perf_count;

        /** Performance subtask information for this class instantiation. */
   std::map<CLIPerformanceDataEvents, bool > cli_perf_subtask;

   /** Routine to determine if the event is a total time event? */
   bool isTotalEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a start time event? */
   bool isStartEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a end time event? */
   bool isEndEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a count number of events event? */
   bool isCountEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a maximum time event? */
   bool isMaxEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a minimum time event? */
   bool isMinEvent(CLIPerformanceDataEvents eventType);

   /** Routine to process the performance event occurance */
   void processTimingEventEnd(
                 CLIPerformanceDataEvents eventTypeStart,
                 CLIPerformanceDataEvents eventTypeCount,
                 CLIPerformanceDataEvents eventTypeMax,
                 CLIPerformanceDataEvents eventTypeMin,
                 CLIPerformanceDataEvents eventTypeTotal,
                 CLIPerformanceDataEvents eventTypeEnd);

   /** Routine to find the corresponding Total event for the input Count event */
   CLIPerformanceDataEvents findTotalEvent( CLIPerformanceDataEvents inputCountEvent);

   /** Routine to find the corresponding Max event for the input Count event */
   CLIPerformanceDataEvents findMaxEvent( CLIPerformanceDataEvents inputCountEvent);

   /** Routine to find the corresponding Min event for the input Count event */
   CLIPerformanceDataEvents findMinEvent( CLIPerformanceDataEvents inputCountEvent);


  private:

    // Lookups up and opens the preference file.
    void initialize(char *filename);
    bool debug_perf_enabled;
    bool processing_expAttach;
    bool processing_expCreate;

};

/** Performance data Timings class instantiation handle  */
extern SS_Timings *cli_timing_handle ;

#endif // SS_TIMINGS_H


