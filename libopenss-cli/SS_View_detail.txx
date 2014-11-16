/********************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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


#include "SS_View_detail.hxx"
#include "SS_Timings.hxx"


/* Uncomment define immediately below for debug tracing */
/* DEBUG_CLI provides tracing info, DEBUG_DUMP_CITEMS dumps a data */
/* entry structure and can create large trace text files, it might be
/* wise to only use when really needing to analyse the citem 
/* structure entries  */

/*
#define DEBUG_CLI 1 
#define DEBUG_DUMP_CITEMS 1 
*/

/**
 * Four templates are provided to generate the four differently formatted reports
 * that are required to support the 'expView' command from collectors that provide
 * 'Detail' information. They are:
 *
 *   Detail_Base_Report which is used to report aggregate information for
 *     linked Objects, functions or statements.
 *   Detail_CallStack_Report which is used to report aggregate information for
 *     unique sequences of calls.
 *   Detail_ButterFly_Report which is used to report aggregate information for
 *     the callers and callees of particular functions
 *   Detail_Trace_Report which is used to report detailed information for
 *     individual sequences of calls.
 *
 * Each template will read the database, merge the information into the
 * appropriate intermediate form, and then call the output formatter to complete
 * the construction of the report.  The completed report is returned through the
 * final passed-in argument to the template.
 *
 * With appropriate support from macros, the templates for 'Detail_Base_Report',
 * 'Detail_CallStack_Report' and 'Detail_ButterFly_Report' can process individual
 * either simple elements or std::vector's of elements for the 'TDETAIL' argument.
 * It is meaningless for The template for 'Detail_Trace_Report' to process single
 * elements so it is only able to handle 'std::vector's of elements.
 * 
 * @pre    Several macros are required to use these templates.  They are needed
 *         to tailor a report to the information available to specific collectors.
 *         Thee required macros are:
 *
 *         def_Detail_values - to allocate intermediate temporaries of the proper
 *            type.
 *         get_inclusive_values - to copy values from the Detail structure to the  
 *            intermediate temporaries associated with the "inclusive" metrics.
 *         get_exclusive_values - to copy values from the Detail structure to the  
 *            intermediate temporaries associated with the "exclusive" metrics.
 *         set_Detail_values - to convert values to type-independant CommandResult
 *            objects and store them into the intermediate array that the
 *            ViewInstructions that the output routine will extract them from.
 *         set_ExtraMetric_values - to merge any independetly calculated values
 *            into the intermediate array that the will be used to generate the
 *            output values.
 *         Accumulate_Stack - can be used in place of 'get_inclusive_values', and
 *            'get_exclusive_values' while providing common code to check the
 *            uniqueness of a call stack and to handle recusion.
 *
 * @pre    In additiopn the utilites in the Framework and the utilites that
 *         construct type independant CommandResult Objects, several additional
 *         utilites must be provided. They are:
 *
 *         Find_Max_Temp - to determine how big the intermediate array neds to be.
 *         Determine_TraceBack_Ordering - to determine whether the user wants data
 *            call stacks in top-down, or bottom-up order.
 *         Determine_Metric_Ordering - to determine the left most value so that
 *             we can determine how to calculate percent.
 *         Look_For_KeyWord - to help check the presence on the command of a
 *             particular "-v" keyword.
 *         Determine_Objects - to collect the desired set of LinkedObject,
 *             Function, or Statement objects from the framework.
 *         stack_contains_N_calls - to identify recursive calls that may be in a
 *             call stack.
 *         topStack_In_Subextent - to recognize that a particular call stack ends
 *             with a particular Function.
 *
 * @pre    After accumulating and converting information from the database, a call
 *         is made to 'Generic_Multi_View' to continue processing the intermediate
 *         information and generate the final report.
 */

template <typename TDETAIL>
bool Detail_Trace_Report(
              CommandObject *cmd, 
              ExperimentObject *exp, 
              int64_t topn,
              ThreadGroup& tgrp, 
              std::vector<Collector>& CV, 
              std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, 
              std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TDETAIL *dummy,
              std::list<CommandResult *>& view_output) {

  std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
  int64_t num_temps = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "DontExpand") &&
                    !Look_For_KeyWord(cmd, "ButterFly") &&
                    (Look_For_KeyWord(cmd, "FullStack") ||
                     Look_For_KeyWord(cmd, "FullStacks")));

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
     cli_timing_handle->cli_perf_data[SS_Timings::detailTraceReportStart] = Time::Now();
  }
#endif


#if DEBUG_CLI
  printf("Enter Detail_Trace_Report, SS_View_detail.txx, num_temps=%d, TraceBack_Order=%d, add_stmts=%d\n",
         num_temps, TraceBack_Order, add_stmts);
#endif

  std::vector<std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);
  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no functions found for the 'Detail -v Trace' report.)");
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent.getTimeInterval().getBegin();

#if DEBUG_CLI
    Framework::TimeInterval basetime = databaseExtent.getTimeInterval();
    Framework::AddressRange baseaddr = databaseExtent.getAddressRange();
    std::cerr << "In Detail_Trace_Report, databaseExtent: basetime interval=" << basetime << " baseaddr range=" << baseaddr << std::endl;
#endif

   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;

#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, calling Parse_Interval_Specification\n");
#endif

    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<TDETAIL> > > > raw_items;

#if DEBUG_CLI
  for (std::vector<std::pair<Time,Time> >::iterator iv = intervals.begin(); iv != intervals.end(); iv++) {
         std::cerr << " In Detail_Trace_Report, SS_View_detail.txx, before GetMetricInThreadGroup, iv->first=" 
              << iv->first << " iv->second=" << iv->second << std::endl;
  }
#endif

    // Create raw_items which contains the items that are active in the thread group,
    // for the objects, metric and collector specified.
    //
    // For the trace view, the objects are the functions being traced/wrapped.
    // This appears to be working correctly, although we are getting extra functions
    // included. They are part of the callstack for the wrapped functions.

    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, after GetMetricInThreadGroup\n");
#endif

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested Detail functions.)");
#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, There are no data samples available for the requested Detail functions\n");
#endif
      return false;
    }
*/
  
   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(ViewReduction_Count);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);
#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, after GetReducedMetrics, ExtraTemps=%d\n", ExtraTemps);
#endif

   // Combine all the items for each function.
    std::map<Address, CommandResult *> knownTraces;
    std::set<Framework::StackTrace, ltST> StackTraces_Processed;
    typename std::map<Function, std::map<Framework::StackTrace, std::vector<TDETAIL> > >::iterator fi;
#if DEBUG_CLI
    int rawcount = 0;
    std::cerr << "In Detail_Trace_Report, before raw_items loop, rawcount=" << rawcount << std::endl;
#endif
    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
     // Foreach Detail function ...

      Function F = (*fi).first;

#if DEBUG_CLI
      rawcount = rawcount + 1; 
      std::cerr << "In Detail_Trace_Report, rawcount=" << rawcount << " F.getName()=" << F.getName() << std::endl;
#endif

      // Go off and get the subextents for the function that is being processed.
      // The function is at the top of one of the stack traces being processed and
      // it's event time needs to be reported.

      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

#if DEBUG_CLI
      std::cerr << "In Detail_Trace_Report, after Get_Subextents_To_Object_Map, SubExtents_Map.size()=" 
                << SubExtents_Map.size() << std::endl;
#endif

      typename std::map<Framework::StackTrace, std::vector<TDETAIL> >:: iterator si;
      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {

        Framework::StackTrace st = (*si).first;
   

#if DEBUG_CLI
        std::cerr << "In Detail_Trace_Report, getting new stacktrace=" << std::endl;
#endif

        // Get the details associated with this stacktrace entry (Event?)
        std::vector<TDETAIL> details = (*si).second;

       // If we have already processed this StackTrace, skip it!
        if (StackTraces_Processed.find(st) != StackTraces_Processed.end()) {
#if DEBUG_CLI
          std::cerr << "In Detail_Trace_Report, skipping stacktrace" << std::endl;
#endif
          // skipping the reprocessing of a stacktrace item
          continue;
        }

        // JEG - It seems like we should be getting the first extent address (lowest)
        // and then the last extent for the thread (highest address).  We seem to be taking
        // the 1st and very narrow extent for this check.

#if DEBUG_CLI
        Thread debugThread = st.getThread();
        std::cerr << "In Detail_Trace_Report, st.getThread(), debugThread.getProcessId()=" 
                  << debugThread.getProcessId() << std::endl;
#endif

        // Find the extents associated with the stack trace's thread.
        // We should have subextents in the SubExtents_Map for this thread.  
        // They were calculated for the  thread group above.   Now let us 
        // find those associated with this particular thread.

        std::map<Framework::Thread, Framework::ExtentGroup>::iterator tei = SubExtents_Map.find(st.getThread());
        Framework::ExtentGroup SubExtents;

        if (tei != SubExtents_Map.end()) {
          SubExtents = (*tei).second;

#ifdef DEBUG_CLI
          std::cerr << "In Detail_Trace_Report, SubExtents, SUBEXTENTS.SIZE()=" << SubExtents.size() << std::endl;
          for (Framework::ExtentGroup::iterator debug_ei = SubExtents.begin(); debug_ei != SubExtents.end(); debug_ei++) {
            Framework::Extent check = *debug_ei;
            if (!check.isEmpty()) {
              Framework::TimeInterval time = check.getTimeInterval();
              Framework::AddressRange addr = check.getAddressRange();
              std::cerr << "In Detail_Trace_Report, SubExtents: time interval=" << time << " address range=" << addr << std::endl;
           }
        }
#endif

        } else {

#ifdef DEBUG_CLI
         std::cerr << "In Detail_Trace_Report, DIDNT FIND SUBEXTENT ENTRY ERROR SubExtents.size()=" 
                   << SubExtents.size() << std::endl;
#endif

        }

       // So we should have the subextents associated with the object and the thread 
       // and they should be in the vector SubExtents.

       // Count the number of recursive calls in the stack.
       // The count in the Detail metric includes each call,
       // but the inclusive time has been incremented for each
       // call and we only want the time for the stack trace.
        int64_t calls_In_stack = (SubExtents.begin() == SubExtents.end())
                                   ? 1 : stack_contains_N_calls (st, SubExtents);

        CommandResult *base_CSE = NULL;
        typename std::vector<TDETAIL>::iterator vi;
        for (vi = details.begin(); vi != details.end(); vi++) {
          TDETAIL detail = *vi;

         // Use macro to alocate temporaries
          def_Detail_values

#if DEBUG_CLI
          std::cerr << "In Detail_Trace_Report, SS_View_detail.txx, calling get_inclusive_trace, calls_In_stack=" << calls_In_stack << " F.getName()=" << F.getName() << std::endl;
#endif
          // Use macro to assign to temporaries
          get_inclusive_trace(detail, calls_In_stack, F.getName() );
 
         // JEG - the SubExtents appears to be only one range.  Should this be all the 
         // subextents for the thread?
         // We decided to do the get_exclusive_trace unconditionally, 
         // so we've commented the topStack_In_Subextent if check out.
#if 0
         // Decide if we accumulate exclusive_time, as well.
          if (topStack_In_Subextent (st, SubExtents)) {
           // Bottom of trace is current function.
           // Exclusive_time is the same as inclusive_time.
           // Deeper calls must go without exclusive_time.
#if DEBUG_CLI
            printf("In Detail_Trace_Report, SS_View_detail.txx, calling get_exclusive_trace, calls_In_stack=%d\n", calls_In_stack);
#endif
            get_exclusive_trace(detail, calls_In_stack);
          }
#else
          get_exclusive_trace(detail, calls_In_stack);
#if DEBUG_CLI
          printf("In Detail_Trace_Report, calling get_exclusive_trace, calls_In_stack=%d\n", calls_In_stack);
#endif 
#endif

         // Use macro to assign temporaries to the result array
          SmartPtr<std::vector<CommandResult *> > vcs
                   = Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>(num_temps)
                               );
          set_Detail_values((*vcs), primary_is_inclusive)
          set_ExtraMetric_values((*vcs), Extra_Values, F)

          CommandResult *CSE;
          if (base_CSE == NULL) {
            std::vector<CommandResult *> *call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st, knownTraces);

#if DEBUG_CLI
            printf("In Detail_Trace_Report, SS_View_detail.txx, after new CommandResult_CallStackEntry\n");
#endif

            base_CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
            CSE = base_CSE;

#if DEBUG_CLI
            printf("In Detail_Trace_Report, SS_View_detail.txx, use existing base_CSE,after new CommandResult_CallStackEntry\n");
#endif

          } else {

            CSE = base_CSE->Copy();
#if DEBUG_CLI
            printf("In Detail_Trace_Report, SS_View_detail.txx, make copy of base_CSE,after new CommandResult_CallStackEntry\n");
#endif
          }
          c_items.push_back(std::make_pair(CSE, vcs));

#if DEBUG_DUMP_CITEMS
// -- BEGIN DEBUG CITEMS
        std::cerr << "\nDump items.  Detail_Trace_Report, SS_View_detail.txx, after pushing back vcs, Number of items is " << c_items.size() << "\n";
        std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
        for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
         // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
          std::pair<CommandResult *,
                    SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
          int64_t i;
          for (i = 0; i < (*cp.second).size(); i++ ) {
            CommandResult *p = (*cp.second)[i];
            std::cerr << " Entry i= " << i << "  ";
            if (p != NULL) {
              p->Print(std::cerr); std::cerr << "\n";
            } else {
              std::cerr << "NULL\n";
            }
          }

        }
        fflush(stderr);
// -- END DEBUG CITEMS
#endif

        }

       // Remember that we have now processed this particular StackTrace.
#if DEBUG_CLI
        printf("In Detail_Trace_Report, SS_View_detail.txx, setting that we PROCESSED stacktrace st\n");
#endif
        StackTraces_Processed.insert(st);
      }
    }

    if (ExtraTemps) {
      for (int64_t i = 0; i < Extra_Values.size(); i++) {
        if (!Extra_Values[i].isNull() &&
            !Extra_Values[i]->empty()) {
          Reclaim_CR_Space (Extra_Values[i]);
        }
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
#if DEBUG_CLI
  printf("In Detail_Trace_Report, SS_View_detail.txx, before calling Generic_Multi_View\n");
#endif

#if DEBUG_DUMP_CITEMS
// -- BEGIN DEBUG CITEMS
        std::cerr << "\nDump items.  Detail_Trace_Report, SS_View_detail.txx, before calling Generic_Multi_View, Number of items is " << c_items.size() << "\n";
        std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
        for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
         // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
          std::pair<CommandResult *,
                    SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
          int64_t i;
          for (i = 0; i < (*cp.second).size(); i++ ) {
            CommandResult *p = (*cp.second)[i];
            std::cerr << " Entry i= " << i << "  ";
            if (p != NULL) {
              p->Print(std::cerr); std::cerr << "\n";
            } else {
              std::cerr << "NULL\n";
            }
          }

        }
        fflush(stderr);
// -- END DEBUG CITEMS
#endif

  bool view_built = Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Trace, c_items, view_output);
#if DEBUG_CLI
  printf("In Detail_Trace_Report, SS_View_detail.txx, after calling Generic_Multi_View\n");
#endif

 // Release instructions
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }

#if DEBUG_CLI
  printf("Exit Detail_Trace_Report, SS_View_detail.txx, view_built=%d, topn=%d, c_items.size()=%d\n",
         view_built, topn, c_items.size());
#endif

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::detailTraceReportStart,
                                                SS_Timings::detailTraceReportCount,
                                                SS_Timings::detailTraceReportMax,
                                                SS_Timings::detailTraceReportMin,
                                                SS_Timings::detailTraceReportTotal,
                                                SS_Timings::detailTraceReportEnd);
  }
#endif

  return view_built;
}

template <typename TOBJECT, typename TDETAIL>
bool Detail_Base_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TOBJECT *dummyObject, View_Form_Category vfc, TDETAIL *dummyDetail,
              std::list<CommandResult *>& view_output) {

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::detailBaseReportStart] = Time::Now();
  }
#endif

  int64_t num_temps = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;

#if DEBUG_CLI
  printf("Enter Detail_Base_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
          num_temps, primary_is_inclusive);
#endif

  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<TOBJECT> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);
  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no objects specified for the basic Detail report.)");
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent .getTimeInterval().getBegin();

#if DEBUG_CLI
    printf("In Detail_Base_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
#endif


   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;

#if DEBUG_CLI
    printf("In Detail_Base_Report, SS_View_detail.txx, calling Parse_Interval_Specification\n");
#endif

    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<TOBJECT,
                      std::map<Framework::StackTrace,
                               TDETAIL> > > raw_items;

#if DEBUG_CLI
    printf("In Detail_Base_Report, about to call GetMetricInThreadGroup, tgrp.size()=%d\n", tgrp.size());
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      std::pair<bool, int> prank = ti->getMPIRank();
      int64_t rank = prank.first ? prank.second : -1;
      std::pair<bool, pthread_t> xtid = ti->getPosixThreadId();
      std::cerr << "In Detail_Base_Report, about to call GetMetricInThreadGroup, getPosixThreadId(), xtid.first=" << xtid.first << std::endl;
      std::cerr << "In Detail_Base_Report, about to call GetMetricInThreadGroup, getPosixThreadId(), xtid.second=" << xtid.second << " rank=" << rank << std::endl;
   }
#endif


    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested Detail functions.)");
#if DEBUG_CLI
    printf("In Detail_Base_Report, SS_View_detail.txx, There are no data samples available for the requested Detail functions\n");
#endif
      return false;
    }
*/

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<TOBJECT, CommandResult *> > > Extra_Values(ViewReduction_Count);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);

   // Combine all the items for each function, statement or linked object.
    typename std::map<TOBJECT, std::map<Framework::StackTrace, TDETAIL > >::iterator fi;

    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
     // Use macro to allocate imtermediate temporaries
      def_Detail_values

      TOBJECT F = (*fi).first;
      std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

      std::set<Framework::StackTrace, ltST> StackTraces_Processed;
      typename std::map<Framework::StackTrace, TDETAIL >:: iterator si;

      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
        Framework::StackTrace st = (*si).first;
        TDETAIL details = (*si).second;

        bool DEBUG_FLAG = false;

#if DEBUG_CLI
        DEBUG_FLAG = true;
#endif

#if DEBUG_CLI
        //std::cerr << "In Detail_Base_Report, SS_View_detail.txx, calling Accumulate_Stack, for F.getName=" << F.getName() << std::endl;
        printf("In Detail_Base_Report, SS_View_detail.txx, calling Accumulate_Stack, for (*si)\n");
#endif
        Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG, F.getName());
      }

     // Use macro to construct result array
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_temps)
                           );
      set_Detail_values((*vcs), primary_is_inclusive)
      set_ExtraMetric_values((*vcs), Extra_Values, F)

     // Construct callstack for last entry in the stack trace.
      std::vector<CommandResult *> *call_stack = new std::vector<CommandResult *>();
      call_stack->push_back(CRPTR (F));
      CommandResult *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, vcs));
    }

    if (ExtraTemps) {
      for (int64_t i = 0; i < Extra_Values.size(); i++) {
        if (!Extra_Values[i].isNull() &&
            !Extra_Values[i]->empty()) {
          Reclaim_CR_Space (Extra_Values[i]);
        }
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.

#if DEBUG_CLI
  printf("In Detail_Base_Report, SS_View_detail.txx, before Generic_Multi_View\n");
  printf("In Detail_Base_Report, about to call Generic_Multi_View, tgrp.size()=%d\n", tgrp.size());
  for (ThreadGroup::iterator dbti = tgrp.begin(); dbti != tgrp.end(); dbti++) {
    std::pair<bool, int> dbprank = dbti->getMPIRank();
    int64_t dbrank = dbprank.first ? dbprank.second : -1;
    std::pair<bool, pthread_t> dbxtid = dbti->getPosixThreadId();
    std::cerr << "In Detail_Base_Report, about to call Generic_Multi_View, getPosixThreadId(), dbxtid.first=" << dbxtid.first << std::endl;
    std::cerr << "In Detail_Base_Report, about to call Generic_Multi_View, getPosixThreadId(), dbxtid.second=" << dbxtid.second << " dbrank=" << dbrank << std::endl;
  }
#endif

  bool view_built = Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc, c_items, view_output);

#if DEBUG_CLI
  printf("In Detail_Base_Report, SS_View_detail.txx, after Generic_Multi_View\n");
#endif

 // Release instructions
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }

#if DEBUG_CLI
  printf("Exit Detail_Base_Report, SS_View_detail.txx, view_built=%d, topn=%d\n",
          view_built, topn);
#endif

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::detailBaseReportStart,
                                                SS_Timings::detailBaseReportCount,
                                                SS_Timings::detailBaseReportMax,
                                                SS_Timings::detailBaseReportMin,
                                                SS_Timings::detailBaseReportTotal,
                                                SS_Timings::detailBaseReportEnd);
  }
#endif

  return view_built;
}

template <typename TDETAIL>
bool Detail_CallStack_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TDETAIL *dummy,
              std::list<CommandResult *>& view_output) {

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::detailCallStackReportStart] = Time::Now();
  }
#endif

  int64_t num_temps = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;

#if DEBUG_CLI
  printf("Enter Detail_CallStack_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
          num_temps, primary_is_inclusive);
#endif

  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "DontExpand") &&
                    (!(Look_For_KeyWord(cmd, "ButterFly") ||
                       Look_For_KeyWord(cmd, "CallTree") ||
                       Look_For_KeyWord(cmd, "CallTrees")) ||
                     Look_For_KeyWord(cmd, "FullStack") ||
                     Look_For_KeyWord(cmd, "FullStacks")));
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);
  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no functions found for the 'Detail -v CallStack' report.)");
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent.getTimeInterval().getBegin();
#if DEBUG_CLI
    printf("In Detail_CallStack_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
#endif

   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;

#if DEBUG_CLI
    printf("In Detail_CallStack_Report, SS_View_detail.txx, calling Parse_Interval_Specification\n");
#endif

    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               TDETAIL > > > raw_items;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

#if DEBUG_CLI
    printf("In Detail_CallStack_Report, SS_View_detail.txx, after GetMetricInThreadGroup\n");
#endif

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested Detail functions.)");
#if DEBUG_CLI
    printf("In Detail_CallStack_Report, SS_View_detail.txx, There are no data samples available for the requested Detail functions\n");
#endif
      return false;
    }
*/

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(ViewReduction_Count);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);

#if DEBUG_CLI
    printf("In Detail_CallStack_Report, SS_View_detail.txx, after GetReducedMetrics, ExtraTemps=%d\n",
           ExtraTemps);
#endif

   // Combine all the items for each function.
    std::map<Address, CommandResult *> knownTraces;
    std::set<Framework::StackTrace, ltST> StackTraces_Processed;
    typename std::map<Function,
                      std::map<Framework::StackTrace,
                               TDETAIL > >::iterator fi;
    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
     // Foreach Detail function ...

      Function F = (*fi).first;

#if DEBUG_CLI
      std::cerr << "In Detail_CallStack_Report, SS_View_detail.txx, calling Get_Subextents_To_Object_Map for Function " 
           <<   F.getName() << "\n" ;
#endif

      std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

      typename std::map<Framework::StackTrace,
                        TDETAIL>::iterator sti;
      for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
        Framework::StackTrace st = (*sti).first;
        TDETAIL details = (*sti).second;

       // Use macro to allocate temporary array
        def_Detail_values

#if DEBUG_CLI
        printf("In Detail_CallStack_Report, SS_View_detail.txx, calling Accumulate_CallStack\n");
#endif
      bool DEBUG_FLAG = false;

#if DEBUG_CLI
      DEBUG_FLAG = true;
#endif

	Accumulate_CallStack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG, F.getName());

       // Use macro to set values into return structure.
        SmartPtr<std::vector<CommandResult *> > vcs
                 = Framework::SmartPtr<std::vector<CommandResult *> >(
                             new std::vector<CommandResult *>(num_temps)
                             );
        set_Detail_values((*vcs), primary_is_inclusive)
        set_ExtraMetric_values((*vcs), Extra_Values, F)

#if DEBUG_CLI
        printf("In Detail_CallStack_Report, SS_View_detail.txx, about to construct result entry\n");
#endif
       // Construct result entry
        std::vector<CommandResult *> *call_stack
                 = Construct_CallBack (TraceBack_Order, add_stmts, st, knownTraces);
        CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
#if DEBUG_CLI
        printf("In Detail_CallStack_Report, SS_View_detail.txx, after new CommandResult_CallStackEntry\n");
#endif

#if DEBUG_DUMP_CITEMS
// -- BEGIN DEBUG CITEMS
        std::cerr << "\nDump items.  Detail_CallStack_Report, SS_View_detail.txx, after new CommandResult_CallStackEntry, Number of items is " << c_items.size() << "\n";
        std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
        for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
         // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
          std::pair<CommandResult *,
                    SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
          int64_t i;
          for (i = 0; i < (*cp.second).size(); i++ ) {
            CommandResult *p = (*cp.second)[i];
            std::cerr << "  ";
            if (p != NULL) {
              p->Print(std::cerr); std::cerr << "\n";
            } else {
              std::cerr << "NULL\n";
            }
          }

        }
        fflush(stderr);
// -- END DEBUG CITEMS

//        *vcs->Print(std::cerr);
#endif
        c_items.push_back(std::make_pair(CSE, vcs));
      }
    }

    if (ExtraTemps) {
      for (int64_t i = 0; i < Extra_Values.size(); i++) {
        if (!Extra_Values[i].isNull() &&
            !Extra_Values[i]->empty()) {
          Reclaim_CR_Space (Extra_Values[i]);
        }
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
#if DEBUG_CLI
  printf("In Detail_CallStack_Report, SS_View_detail.txx, before calling Generic_Multi_View\n");
#endif

  bool view_built = Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack,c_items, view_output);

#if DEBUG_CLI
  printf("In Detail_CallStack_Report, SS_View_detail.txx, after calling Generic_Multi_View\n");
#endif

 // Release instructions
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }
#if DEBUG_CLI
  printf("Exit Detail_CallStack_Report, SS_View_detail.txx, view_built=%d, topn=%d\n",
          view_built, topn);
#endif

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::detailCallStackReportStart,
                                                SS_Timings::detailCallStackReportCount,
                                                SS_Timings::detailCallStackReportMax,
                                                SS_Timings::detailCallStackReportMin,
                                                SS_Timings::detailCallStackReportTotal,
                                                SS_Timings::detailCallStackReportEnd);
  }
#endif

  return view_built;
}


template <typename TDETAIL>
bool Detail_ButterFly_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TDETAIL *dummy,
              std::list<CommandResult *>& view_output) {

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::detailButterFlyReportStart] = Time::Now();
  }
#endif

  int64_t num_temps = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "DontExpand") &&
                    (Look_For_KeyWord(cmd, "FullStack") ||
                     Look_For_KeyWord(cmd, "FullStacks")));
#if DEBUG_CLI
  printf("Enter detail_ButterFly_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
          num_temps, primary_is_inclusive);
#endif

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects (cmd, exp, tgrp, objects);

  if (objects.empty()) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no functions found for the 'Detail -v ButterFly' report.)");
    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent.getTimeInterval().getBegin();
#if DEBUG_CLI
    printf("In detail_ButterFly_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
#endif

   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;
    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               TDETAIL > > > raw_items;

    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

    if (raw_items->begin() == raw_items->end()) {

#if DEBUG_CLI
      printf("In detail_Butterfly_Report, SS_View_detail.txx, ******ERROR****** There are no data samples available for the requested Detail functions\n");
#endif

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested functions.)");
      return false;
*/
    } // raw_items->begin() ....

   // Get any required intermediate reduction temps.

    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(ViewReduction_Count);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);

   // Generate a separate butterfly view for each function in the list.

    std::map<Address, CommandResult *> knownTraces;
    typename std::map<Function, std::map<Framework::StackTrace, TDETAIL > >::iterator fi1;
#if 0
Dump_CallStack (std::vector<CommandResult *> *call_stack,
                std::vector<CommandResult *> *vcs) {
#endif


#if DEBUG_CLI
    int rawcount = 0;
#endif
    for (fi1 = raw_items->begin(); fi1 != raw_items->end(); fi1++) {

#if DEBUG_CLI
      rawcount = rawcount + 1; 
#endif
     // Define set of data for the current function.
      std::vector<std::pair<CommandResult *,
                            SmartPtr<std::vector<CommandResult *> > > > c_items;

      Function F = (*fi1).first;
#if DEBUG_CLI
      std::cerr << "In Detail_Butterfly_Report, rawcount=" << rawcount << " F.getName()=" << F.getName() << std::endl;
#endif

      std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

     // Capture each StackTrace in the inclusive_detail list.
      std::set<Framework::StackTrace, ltST> StackTraces_Processed;
      typename std::map<Framework::StackTrace, TDETAIL >:: iterator si1;
      for (si1 = (*fi1).second.begin(); si1 != (*fi1).second.end(); si1++) {
        Framework::StackTrace st = (*si1).first;
#if DEBUG_CLI
        printf("Butterfly view, begin dumping stack st\n");
        Dump_StackTrace(st);
        printf("Butterfly view, end dumping stack st\n");
#endif

        TDETAIL details = (*si1).second;

       // Use macro to allocate imtermediate temporaries
        def_Detail_values

       // Pass a debug flag so that a runtime check can be used because #if's can't
       // be used inside a #define which is what Accumulate_Stack is
        bool DEBUG_FLAG = false;

#if DEBUG_CLI
      DEBUG_FLAG = true;
#endif
     // Go off and get the values 
        //printf("In Detail_ButterFly, SS_View_detail.txx, calling Accumulate_Stack, for (*si)\n");
        Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG, F.getName());

       // Use macro to construct result array
        SmartPtr<std::vector<CommandResult *> > vcs
                 = Framework::SmartPtr<std::vector<CommandResult *> >(
                             new std::vector<CommandResult *>(num_temps)
                             );
        set_Detail_values((*vcs), primary_is_inclusive)
        set_ExtraMetric_values((*vcs), Extra_Values, F)

       // Construct result entry
        std::vector<CommandResult *> *call_stack;
        call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st, knownTraces);

        CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);

#if DEBUG_CLI
        printf("In detail_ButterFly_Report, SS_View_detail.txx, pushing back vcs (c_items)\n");
#endif

        c_items.push_back(std::make_pair(CSE, vcs));


#if DEBUG_DUMP_CITEMS

// -- BEGIN DEBUG CITEMS
        std::cerr << "\nDump items.  Detail_Trace_Report, SS_View_detail.txx, after pushing back vcs, Number of items is " << c_items.size() << "\n";
        std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
        for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
         // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
          std::pair<CommandResult *,
                    SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
          int64_t i;
          for (i = 0; i < (*cp.second).size(); i++ ) {
            CommandResult *p = (*cp.second)[i];
            std::cerr << " Entry i= " << i << "  ";
            if (p != NULL) {
              p->Print(std::cerr); std::cerr << "\n";
            } else {
              std::cerr << "NULL\n";
            }
          }

        }
        fflush(stderr);
// -- END DEBUG CITEMS
#endif

      }

     // Generate the report.
/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
      if (c_items.empty()) {

#if DEBUG_CLI
        printf("In detail_Butterfly_Report, SS_View_detail.txx, There are no data samples available for the requested function=%s\n", F.getName());
#endif


        std::string S = "(There are no data samples available for function '";
        S = S + F.getName() + "'.)";
        Mark_Cmd_With_Soft_Error(cmd, S);

      } else
*/

      (void) Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack, c_items, view_output);
    }

    if (ExtraTemps) {
      for (int64_t i = 0; i < Extra_Values.size(); i++) {
        if (!Extra_Values[i].isNull() &&
            !Extra_Values[i]->empty()) {
          Reclaim_CR_Space (Extra_Values[i]);
        }
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Release instructions
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }
#if DEBUG_CLI
  printf("Exit detail_ButterFly_Report, SS_View_detail.txx,  topn=%d\n", topn);
#endif

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::detailButterFlyReportStart,
                                                SS_Timings::detailButterFlyReportCount,
                                                SS_Timings::detailButterFlyReportMax,
                                                SS_Timings::detailButterFlyReportMin,
                                                SS_Timings::detailButterFlyReportTotal,
                                                SS_Timings::detailButterFlyReportEnd);
  }
#endif

  return true;
}

//
//  Simple_Base_Report generates views for collectors that use a simple metric value
//                     and do not accumulate call stacks.
//  At the moment, this template is only used with 2 collectors: pcsamp and hwc.
//
template <typename TOBJECT>
bool Simple_Base_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TOBJECT *dummyObject, View_Form_Category vfc,
              std::list<CommandResult *>& view_output) {

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::detailBaseReportStart] = Time::Now();
  }
#endif

  int64_t num_temps = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;

#if DEBUG_CLI
  printf("Enter Simple_Base_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
          num_temps, primary_is_inclusive);
#endif

  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<TOBJECT> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {

#if DEBUG_CLI
    std::cerr << "IN Simple_Base_Report, SS_View_detail.txx, RETURN EARLY WITH no objects message." << std::endl;
#endif

    Mark_Cmd_With_Soft_Error(cmd, "(There are no objects specified for the basic Detail report.)");

    return false;
  }

  try {
    collector.lockDatabase();
    Extent databaseExtent = exp->FW()->getPerformanceDataExtent();
    Time base_time = databaseExtent .getTimeInterval().getBegin();

#if DEBUG_CLI
    printf("In Simple_Base_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
    printf("In Simple_Base_Report, SS_View_detail.txx, objects.size()=%d\n", objects.size());
#endif


   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;

#if DEBUG_CLI
    printf("In Simple_Base_Report, SS_View_detail.txx, calling Parse_Interval_Specification\n");
#endif

    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<TOBJECT, CommandResult *> > raw_items =
            Framework::SmartPtr<std::map<TOBJECT, CommandResult *> >(
                new std::map<TOBJECT, CommandResult * >()
                );
//                      std::map<TOBJECT, CommandResult *> > >  raw_items;
//                      std::map<Framework::StackTrace,
//                               TDETAIL> > > raw_items;

#if DEBUG_CLI
    printf("In Simple_Base_Report, about to call GetMetricInThreadGroup, tgrp.size()=%d\n", tgrp.size());
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
      std::pair<bool, int> prank = ti->getMPIRank();
      int64_t rank = prank.first ? prank.second : -1;
      std::pair<bool, pthread_t> xtid = ti->getPosixThreadId();
      std::cerr << "In Simple_Base_Report, about to call GetMetricInThreadGroup, getPosixThreadId(), xtid.first=" << xtid.first << std::endl;
      std::cerr << "In Simple_Base_Report, about to call GetMetricInThreadGroup, getPosixThreadId(), xtid.second=" << xtid.second << " rank=" << rank << std::endl;
   }
#endif


#if DEBUG_CLI
    printf("In Simple_Base_Report, about to call GetMetricByObjectSet, tgrp.size()=%d\n", tgrp.size());
#endif
    GetMetricByObjectSet   (cmd, exp, tgrp, collector, metric, objects, raw_items);
#if DEBUG_CLI
    printf("In Simple_Base_Report, after calling GetMetricByObjectSet, tgrp.size()=%d\n", tgrp.size());
#endif

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<TOBJECT, CommandResult *> > > Extra_Values(ViewReduction_Count);

#if DEBUG_CLI
    printf("In Simple_Base_Report, before 2 calling GetReducedMetrics, tgrp.size()=%d\n", tgrp.size());
#endif

    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);

#if DEBUG_CLI
    printf("In Simple_Base_Report, after calling GetReducedMetrics, tgrp.size()=%d\n", tgrp.size());
#endif

   // Combine all the items for each function, statement or linked object.
    typename std::map<TOBJECT, CommandResult *>::iterator fi;

    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
     // Use macro to allocate imtermediate temporaries
      def_Detail_values

      TOBJECT F = (*fi).first;
      get_exclusive_values ((*fi).second, 1)
      CommandResult *second = (*fi).second;

     // Use macro to construct result array
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_temps)
                           );
      set_Detail_values((*vcs), primary_is_inclusive)
      set_ExtraMetric_values((*vcs), Extra_Values, F)

     // Construct callstack for last entry in the stack trace.
      std::vector<CommandResult *> *call_stack = new std::vector<CommandResult *>(1);
      (*call_stack)[0] = CRPTR (F);
      CommandResult *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, vcs));
    }

    if (ExtraTemps) {
      for (int64_t i = 0; i < Extra_Values.size(); i++) {
        if (!Extra_Values[i].isNull() &&
            !Extra_Values[i]->empty()) {
          Reclaim_CR_Space (Extra_Values[i]);
        }
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.

#if DEBUG_CLI
  printf("In Simple_Base_Report, SS_View_detail.txx, before Generic_Multi_View\n");
  printf("In Simple_Base_Report, about to call Generic_Multi_View, tgrp.size()=%d\n", tgrp.size());
  for (ThreadGroup::iterator dbti = tgrp.begin(); dbti != tgrp.end(); dbti++) {
    std::pair<bool, int> dbprank = dbti->getMPIRank();
    int64_t dbrank = dbprank.first ? dbprank.second : -1;
    std::pair<bool, pthread_t> dbxtid = dbti->getPosixThreadId();
    std::cerr << "In Simple_Base_Report, about to call Generic_Multi_View, getPosixThreadId(), dbxtid.first=" << dbxtid.first << std::endl;
    std::cerr << "In Simple_Base_Report, about to call Generic_Multi_View, getPosixThreadId(), dbxtid.second=" << dbxtid.second << " dbrank=" << dbrank << std::endl;
  }
#endif

  bool view_built = Generic_Multi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc, c_items, view_output);

#if DEBUG_CLI
  printf("In Simple_Base_Report, SS_View_detail.txx, after Generic_Multi_View\n");
#endif

 // Release instructions
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }

#if DEBUG_CLI
  printf("Exit Simple_Base_Report, SS_View_detail.txx, view_built=%d, topn=%d\n",
          view_built, topn);
#endif

#if BUILD_CLI_TIMING
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::detailBaseReportStart,
                                                SS_Timings::detailBaseReportCount,
                                                SS_Timings::detailBaseReportMax,
                                                SS_Timings::detailBaseReportMin,
                                                SS_Timings::detailBaseReportTotal,
                                                SS_Timings::detailBaseReportEnd);
  }
#endif

  return view_built;
}
