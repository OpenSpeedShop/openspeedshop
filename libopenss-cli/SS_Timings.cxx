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



#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#include "SS_Timings.hxx"

// The constructor for creating an initial Timing entry
SS_Timings::SS_Timings()
{
    debug_perf_enabled = (getenv("OPENSS_DEBUG_PERF_CLI") != NULL);

    cli_perf_data[CLIStart] = Time::Now();

// Initialize counters to zero at start of gathering

    cli_perf_count[CLICount] = 0 ;
    cli_perf_count[ViewGenerationCount] = 0;
    cli_perf_count[ expAttachCount ] = 0;
    cli_perf_count[ expCreateCount ] = 0;
    cli_perf_count[ expCompareCount ] = 0;
    cli_perf_count[ cvClustersCount ] = 0;
    cli_perf_count[ cvInfoCount ] = 0;
    cli_perf_count[ DetailBaseReportCount ] = 0;
    cli_perf_count[ DetailTraceReportCount ] = 0;
    cli_perf_count[ DetailCallStackReportCount ] = 0;
    cli_perf_count[ DetailButterFlyReportCount ] = 0;

// Initialize time totals to zero at start of gathering

    cli_perf_count[CLITotal] = 0 ;
    cli_perf_count[ViewGenerationTotal] = 0;
    cli_perf_count[ expAttachTotal ] = 0;
    cli_perf_count[ expCreateTotal ] = 0;
    cli_perf_count[ expCompareTotal ] = 0;
    cli_perf_count[ cvClustersTotal ] = 0;
    cli_perf_count[ cvInfoTotal ] = 0;
    cli_perf_count[ DetailBaseReportTotal ] = 0;
    cli_perf_count[ DetailTraceReportTotal ] = 0;
    cli_perf_count[ DetailCallStackReportTotal ] = 0;
    cli_perf_count[ DetailButterFlyReportTotal ] = 0;
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


bool SS_Timings::isStartEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == ViewGenerationStart ||
      eventType == expAttachStart ||
      eventType == expCreateStart ||
      eventType == expCompareStart ||
      eventType == cvClustersStart ||
      eventType == cvInfoStart ||
      eventType == DetailBaseReportStart ||
      eventType == DetailTraceReportStart ||
      eventType == DetailCallStackReportStart ||
      eventType == DetailButterFlyReportStart ||
      eventType == CLIStart ) {
    return true;
  }
  return false;
}


bool SS_Timings::isEndEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == ViewGenerationEnd ||
      eventType == expAttachEnd ||
      eventType == expCreateEnd ||
      eventType == expCompareEnd ||
      eventType == cvClustersEnd ||
      eventType == cvInfoEnd ||
      eventType == DetailBaseReportEnd ||
      eventType == DetailTraceReportEnd ||
      eventType == DetailCallStackReportEnd ||
      eventType == DetailButterFlyReportEnd ||
      eventType == CLIEnd ) {
    return true;
  }
  return false;
}

bool SS_Timings::isCountEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == ViewGenerationCount ||
      eventType == expAttachCount ||
      eventType == expCreateCount ||
      eventType == expCompareCount ||
      eventType == cvClustersCount ||
      eventType == cvInfoCount ||
      eventType == DetailBaseReportCount ||
      eventType == DetailTraceReportCount ||
      eventType == DetailCallStackReportCount ||
      eventType == DetailButterFlyReportCount ||
      eventType == CLICount ) {
    return true;
  }
  return false;
}

bool SS_Timings::isTotalEvent(CLIPerformanceDataEvents eventType)
{
  if (eventType == ViewGenerationTotal ||
      eventType == expAttachTotal ||
      eventType == expCreateTotal ||
      eventType == expCompareTotal ||
      eventType == cvClustersTotal ||
      eventType == cvInfoTotal ||
      eventType == DetailBaseReportTotal ||
      eventType == DetailTraceReportTotal ||
      eventType == DetailCallStackReportTotal ||
      eventType == DetailButterFlyReportTotal ||
      eventType == CLITotal ) {
    return true;
  }
  return false;
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

    } Table[] = {

        { SS_Timings::ViewGenerationStart, "Time the CLI Started Generation of a report" },
        { SS_Timings::ViewGenerationCount, "Number of times view generation was/is being timed" },
        { SS_Timings::ViewGenerationTotal, "Total time spent in view generation, possibly over multiple calls" },
        { SS_Timings::ViewGenerationEnd, "Time the CLI Completed Generation of a report" },
        { SS_Timings::expAttachStart, "Time the last expAttach Command Started" },
        { SS_Timings::expAttachCount, "Number of expAttach commands being timed" },
        { SS_Timings::expAttachTotal, "Total expAttach command time possibly over multiple calls" },
        { SS_Timings::expAttachEnd, "Time the last expAttach Command Ended" },
        { SS_Timings::expCreateStart, "Time the last expCreate Command Started" },
        { SS_Timings::expCreateCount, "Number of expCreate commands being timed" },
        { SS_Timings::expCreateTotal, "Total expCreate command time possibly over multiple calls" },
        { SS_Timings::expCreateEnd, "Time the last expCreate Comman Ended" },
        { SS_Timings::expCompareStart, "Time the last expCompare Command Started" },
        { SS_Timings::expCompareCount, "Number of expCompare commands being timed" },
        { SS_Timings::expCompareTotal, "Total expCompare command time possibly over multiple calls" },
        { SS_Timings::expCompareEnd, "Time the last expCompare Comman Ended" },
        { SS_Timings::cvClustersStart, "Time the last cvClusters Command Started" },
        { SS_Timings::cvClustersCount, "Number of cvClusters commands being timed" },
        { SS_Timings::cvClustersTotal, "Total cvClusters command time possibly over multiple calls" },
        { SS_Timings::cvClustersEnd, "Time the last cvClusters Command Ended" },
        { SS_Timings::cvInfoStart, "Time the last cvInfo Command Started" },
        { SS_Timings::cvInfoCount, "Number of cvInfo commands being timed" },
        { SS_Timings::cvInfoTotal, "Total cvInfo command time possibly over multiple calls" },
        { SS_Timings::cvInfoEnd, "Time the last cvInfo Command Ended" },
        { SS_Timings::DetailBaseReportStart, "Time the last DetailBaseReport generation Started" },
        { SS_Timings::DetailBaseReportCount, "Number of DetailBaseReport generations being timed" },
        { SS_Timings::DetailBaseReportTotal, "Total DetailBaseReport generation time possibly over multiple calls" },
        { SS_Timings::DetailBaseReportEnd, "Time the last DetailBaseReport Generation Ended" },
        { SS_Timings::DetailTraceReportStart, "Time the last DetailTraceReport generation Started" },
        { SS_Timings::DetailTraceReportCount, "Number of DetailTraceReport generations being timed" },
        { SS_Timings::DetailTraceReportTotal, "Total DetailTraceReport generation time possibly over multiple calls" },
        { SS_Timings::DetailTraceReportEnd, "Time the last DetailTraceReport Generation Ended" },
        { SS_Timings::DetailCallStackReportStart, "Time the last DetailCallStackReport generation Started" },
        { SS_Timings::DetailCallStackReportCount, "Number of DetailCallStackReport generations being timed" },
        { SS_Timings::DetailCallStackReportTotal, "Total DetailCallStackReport generation time possibly over multiple calls" },
        { SS_Timings::DetailCallStackReportEnd, "Time the last DetailCallStackReport Generation Ended" },
        { SS_Timings::DetailButterFlyReportStart, "Time the last DetailButterFlyReport generation Started" },
        { SS_Timings::DetailButterFlyReportCount, "Number of DetailButterFlyReport generations being timed" },
        { SS_Timings::DetailButterFlyReportTotal, "Total DetailButterFlyReport generation time possibly over multiple calls" },
        { SS_Timings::DetailButterFlyReportEnd, "Time the last DetailButterFlyReport Generation Ended" },
        { SS_Timings::CLIStart, "Time the CLI execution started (Time CLI Entered)" },
        { SS_Timings::CLICount, "Number of times the CLI was/is being timed" },
        { SS_Timings::CLITotal, "Total time spent in CLI, possibly over multiple calls" },
        { SS_Timings::CLIEnd, "Time CLI execution ended (Time CLI Ended/Exited" },

        { SS_Timings::CLIEnd, "" }  // End Of Table Entry

    };

    // Display the header
    std::cerr << std::endl << std::endl << std::endl
              << std::setw(InitialIndent) << " "
              << "INDIVIDUAL VALUES FOR CLI TASK PERFORMANCE " << ""
              << std::endl 
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "" << std::endl
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "Time(mS)" << "  "
              << "Event" << std::endl << std::endl;

// Re-enable if it is desirable to see the start time and end time for the
// event performance statistics gathering.  The start and end time will be
// related to the last timing event.  Intermediate start and end times are
// not saved at this time.   12/8/2006 jeg
#if 0
    // Iterate over each performance data event to be shown
    for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {

            // Skip this event entry if it didn't record this event
            if(cli_perf_data.find(Table[i].cli_event) == cli_perf_data.end())
                continue;

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
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ] / 1000000 << "  "
                         << Table[i].cli_description << std::endl;
            }
            else if (isEndEvent(Table[i].cli_event)) {

               // Display this line of statistics
               std::cerr << std::setw(InitialIndent) << " "
                         << std::setw(TimeWidth) << cli_perf_data[ Table[i].cli_event ] / 1000000 << "  "
                         << Table[i].cli_description << std::endl;
            }


    } // end for i
#endif

// Iterate over the performance count data now

    // Iterate over each performance count event to be shown
    for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {


            // Skip this event entry if it didn't record this event
            if(cli_perf_count.find(Table[i].cli_event) == cli_perf_count.end())
                continue;

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
              << "SUMMARY OF CLI TASK PERFORMANCE IF MULTIPLE EVENTS " << ""
              << std::endl 
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "" << std::endl
              << std::setw(InitialIndent) << " "
              << std::setw(TimeWidth) << "Time(mS)" << "  "
              << "Event" << std::endl << std::endl;


      std::cerr << std::setw(InitialIndent) << " "
                << "These events had multiple executions" << ""
                << "" << std::endl << std::endl;

      for(unsigned i = 0; !Table[i].cli_description.empty(); ++i) {
         if (isCountEvent(Table[i].cli_event) && cli_perf_count[Table[i].cli_event] > 1) {
                 std::cerr << std::setw(InitialIndent) << " "
                           << std::setw(TimeWidth) <<  cli_perf_count[ Table[i].cli_event ] << "  "
                           << Table[i].cli_description << std::endl;
         }
      }
    }

    // Display the tailer
    std::cerr << std::endl << std::endl << std::endl;
}

