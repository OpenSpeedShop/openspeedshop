/********************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

/* Uncomment define immediately below for debug tracing */
/* #define DEBUG_CLI 1  */

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
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              bool primary_is_inclusive,
              TDETAIL *dummy,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));

#if DEBUG_CLI
  printf("Enter Detail_Trace_Report, SS_View_detail.txx, num_temps=%d, TraceBack_Order=%d, add_stmts=%d\n",
         num_temps, TraceBack_Order, add_stmts);
#endif

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

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
    printf("In Detail_Trace_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
#endif

   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;

#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, calling Parse_Interval_Specification\n");
#endif

    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<TDETAIL> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);
#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, after GetMetricInThreadGroup\n");
#endif

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested Detail functions.)");
      return false;
    }
*/
  
   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(Find_Max_ExtraMetrics(IV)+1);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);
#if DEBUG_CLI
    printf("In Detail_Trace_Report, SS_View_detail.txx, after GetReducedMetrics, ExtraTemps=%d\n",
           ExtraTemps);
#endif

   // Combine all the items for each function.
    std::map<Address, CommandResult *> knownTraces;
    std::set<Framework::StackTrace, ltST> StackTraces_Processed;
    typename std::map<Function, std::map<Framework::StackTrace, std::vector<TDETAIL> > >::iterator fi;
    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
     // Foreach Detail function ...

      Function F = (*fi).first;
      std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

      typename std::map<Framework::StackTrace, std::vector<TDETAIL> >:: iterator si;
      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
        Framework::StackTrace st = (*si).first;
        std::vector<TDETAIL> details = (*si).second;

       // If we have already processed this StackTrace, skip it!
        if (StackTraces_Processed.find(st) != StackTraces_Processed.end()) {
          continue;
        }

       // Find the extents associated with the stack trace's thread.
        std::map<Framework::Thread, Framework::ExtentGroup>::iterator tei
                                   = SubExtents_Map.find(st.getThread());
        Framework::ExtentGroup SubExtents;
        if (tei != SubExtents_Map.end()) {
          SubExtents = (*tei).second;
        }

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

         // Use macro to assign to temporaries
          get_inclusive_trace(detail, calls_In_stack);

         // Decide if we accumulate exclusive_time, as well.
          if (topStack_In_Subextent (st, SubExtents)) {
           // Bottom of trace is current function.
           // Exclusive_time is the same as inclusive_time.
           // Deeper calls must go without exclusive_time.
            get_exclusive_trace(detail, calls_In_stack);
          }

         // Use macro to assign temporaries to the result array
          SmartPtr<std::vector<CommandResult *> > vcs
                   = Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>(num_temps)
                               );
          set_Detail_values((*vcs), primary_is_inclusive)
          set_ExtraMetric_values((*vcs), Extra_Values, F)

          CommandResult *CSE;
          if (base_CSE == NULL) {
            std::vector<CommandResult *> *call_stack =
                                  Construct_CallBack (TraceBack_Order, add_stmts, st, knownTraces);
#if DEBUG_CLI
            printf("In Detail_Trace_Report, SS_View_detail.txx, after new CommandResult_CallStackEntry\n");
#endif
            base_CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
            CSE = base_CSE;
          } else {
            CSE = base_CSE->Copy();
          }
          c_items.push_back(std::make_pair(CSE, vcs));
        }

       // Remember that we have now processed this particular StackTrace.
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

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;

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
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested Detail functions.)");
      return false;
    }
*/

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<TOBJECT, CommandResult *> > > Extra_Values(Find_Max_ExtraMetrics(IV)+1);
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

        Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG);
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

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;

#if DEBUG_CLI
  printf("Enter Detail_CallStack_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
          num_temps, primary_is_inclusive);
#endif

  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));
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
      return false;
    }
*/

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(Find_Max_ExtraMetrics(IV)+1);
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
      cerr << "In Detail_CallStack_Report, SS_View_detail.txx, calling Get_Subextents_To_Object_Map for Function " 
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
        printf("In Detail_CallStack_Report, SS_View_detail.txx, calling Accumulate_Stack\n");
#endif
      bool DEBUG_FLAG = false;

#if DEBUG_CLI
      DEBUG_FLAG = true;
#endif

	Accumulate_CallStack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG);

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

  int64_t num_temps = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
#if DEBUG_CLI
  printf("Enter Detail_ButterFly_Report, SS_View_detail.txx, num_temps=%d, primary_is_inclusive=%d\n",
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
    printf("In Detail_ButterFly_Report, SS_View_detail.txx, base_time.getValue()=%u\n", base_time.getValue());
#endif

   // Acquire the specified set of time intervals.
    std::vector<std::pair<Time,Time> > intervals;
    Parse_Interval_Specification (cmd, exp, intervals);

   // Acquire base set of metric values.
    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               TDETAIL > > > raw_items;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, raw_items);

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (raw_items->begin() == raw_items->end()) {
      Mark_Cmd_With_Soft_Error(cmd, "(There are no data samples available for the requested functions.)");
      return false;
    }
*/

   // Get any required intermediate reduction temps.
    std::vector<SmartPtr<std::map<Function, CommandResult *> > > Extra_Values(Find_Max_ExtraMetrics(IV)+1);
    bool ExtraTemps = GetReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Extra_Values);

   // Generate a separate butterfly view for each function in the list.
    std::map<Address, CommandResult *> knownTraces;
    typename std::map<Function, std::map<Framework::StackTrace, TDETAIL > >::iterator fi1;
    for (fi1 = raw_items->begin(); fi1 != raw_items->end(); fi1++) {
     // Define set of data for the current function.
      std::vector<std::pair<CommandResult *,
                            SmartPtr<std::vector<CommandResult *> > > > c_items;

      Function F = (*fi1).first;
      std::map<Framework::Thread, Framework::ExtentGroup> SubExtents_Map;
      Get_Subextents_To_Object_Map (tgrp, F, SubExtents_Map);

     // Capture each StackTrace in the inclusive_detail list.
      std::set<Framework::StackTrace, ltST> StackTraces_Processed;
      typename std::map<Framework::StackTrace, TDETAIL >:: iterator si1;
      for (si1 = (*fi1).second.begin(); si1 != (*fi1).second.end(); si1++) {
        Framework::StackTrace st = (*si1).first;
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
        Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG);

       // Use macro to construct result array
        SmartPtr<std::vector<CommandResult *> > vcs
                 = Framework::SmartPtr<std::vector<CommandResult *> >(
                             new std::vector<CommandResult *>(num_temps)
                             );
        set_Detail_values((*vcs), primary_is_inclusive)
        set_ExtraMetric_values((*vcs), Extra_Values, F)

       // Construct result entry
        std::vector<CommandResult *> *call_stack
                 = Construct_CallBack (TraceBack_Order, true, st, knownTraces);
        CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
        c_items.push_back(std::make_pair(CSE, vcs));
      }

     // Generate the report.
/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
      if (c_items.empty()) {
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
  printf("Exit Detail_ButterFly_Report, SS_View_detail.txx,  topn=%d\n", topn);
#endif
  return true;
}
