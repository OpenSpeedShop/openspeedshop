////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#ifndef SS_TIMINGS_H
#define SS_TIMINGS_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

#include "SS_Input_Manager.hxx"
#include "Queries.hxx"


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

   /** Flag indicating if performance debugging for this class is enabled. */
   bool is_debug_perf_enabled() ;


  /**
   * Performance data events enumeration.
   *
   * Enumeration defining all the different event times that are recorded
   * as part of the per-process performance data.
   */
  enum CLIPerformanceDataEvents {
      ViewGenerationStart, /** < Time the CLI Started Generation of a report. */
      ViewGenerationCount, /** < Number of times view generation was/is being timed. */ 
      ViewGenerationTotal, /** < Total time spent in view generation, possibly over multiple calls. */
      ViewGenerationEnd, /** < Time the CLI Completed Generation of a report. */
      expAttachStart, /** < Time the last expAttach Command Started */ 
      expAttachCount, /** < Number of expAttach commands being timed */ 
      expAttachTotal, /** < Total expAttach command time possibly over multiple calls */ 
      expAttachEnd, /** < Time the last expAttach Command Ended */ 
      expCreateStart, /** < Time the last expCreate Command Started */ 
      expCreateCount, /** < Number of expCreate commands being timed */ 
      expCreateTotal, /** < Total expCreate command time possibly over multiple calls */ 
      expCreateEnd, /** < Time the last expCreate Comman Ended */ 
      expCompareStart, /** < Time the last expCompare Command Started */ 
      expCompareCount, /** < Number of expCompare commands being timed */ 
      expCompareTotal, /** < Total expCompare command time possibly over multiple calls */ 
      expCompareEnd, /** < Time the last expCompare Comman Ended */ 
      cvClustersStart, /** < Time the last cvClusters Command Started */ 
      cvClustersCount, /** < Number of cvClusters commands being timed */ 
      cvClustersTotal, /** < Total cvClusters command time possibly over multiple calls */ 
      cvClustersEnd, /** < Time the last cvClusters Command Ended */ 
      cvInfoStart, /** < Time the last cvInfo Command Started */ 
      cvInfoCount, /** < Number of cvInfo commands being timed */ 
      cvInfoTotal, /** < Total cvInfo command time possibly over multiple calls */ 
      cvInfoEnd, /** < Time the last cvInfo Command Ended */ 
      DetailBaseReportStart, /** < Time the last DetailBaseReport generation Started */ 
      DetailBaseReportCount, /** < Number of DetailBaseReport generations being timed */ 
      DetailBaseReportTotal, /** < Total DetailBaseReport generation time possibly over multiple calls */ 
      DetailBaseReportEnd, /** < Time the last DetailBaseReport Generation Ended */ 
      DetailTraceReportStart, /** < Time the last DetailTraceReport generation Started */ 
      DetailTraceReportCount, /** < Number of DetailTraceReport generations being timed */ 
      DetailTraceReportTotal, /** < Total DetailTraceReport generation time possibly over multiple calls */ 
      DetailTraceReportEnd, /** < Time the last DetailTraceReport Generation Ended */ 
      DetailCallStackReportStart, /** < Time the last DetailCallStackReport generation Started */ 
      DetailCallStackReportCount, /** < Number of DetailCallStackReport generations being timed */ 
      DetailCallStackReportTotal, /** < Total DetailCallStackReport generation time possibly over multiple calls */ 
      DetailCallStackReportEnd, /** < Time the last DetailCallStackReport Generation Ended */
      DetailButterFlyReportStart,  /** Time the last DetailButterFlyReport generation Started */ 
      DetailButterFlyReportCount, /** < Number of DetailButterFlyReport generations being timed */ 
      DetailButterFlyReportTotal, /** < Total DetailButterFlyReport generation time possibly over multiple calls */ 
      DetailButterFlyReportEnd, /** < Time the last DetailButterFlyReport Generation Ended */ 
      CLIStart,              /**< The CLI Start time. */
      CLICount, /** < Number of times the CLI was/is being timed  */
      CLITotal, /** < Total time spent in CLI, possibly over multiple calls. */ 
      CLIEnd                 /**< The CLI End/Exit time. */
  };
        /** Performance data for this class instantiation. */
   std::map<CLIPerformanceDataEvents, Time> cli_perf_data;

        /** Performance data for this class instantiation. */
   std::map<CLIPerformanceDataEvents, uint64_t > cli_perf_count;

   /** Routine to determine if the event is a total time event? */
   bool isTotalEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a start time event? */
   bool isStartEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a end time event? */
   bool isEndEvent(CLIPerformanceDataEvents eventType);

   /** Routine to determine if the event is a count number of events event? */
   bool isCountEvent(CLIPerformanceDataEvents eventType);

  private:

    // Lookups up and opens the preference file.
    void initialize(char *filename);
    bool debug_perf_enabled;

};

/** Performance data Timings class instantiation handle  */
extern SS_Timings *cli_timing_handle ;

#endif // SS_TIMINGS_H


