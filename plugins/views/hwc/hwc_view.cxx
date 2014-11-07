/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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

//using namespace OpenSpeedShop::cli;


// Hardware Counter Report
 
// #include "HWCCollector.hxx"


#define excnt_temp VMulti_free_temp

#define First_ByThread_Temp VMulti_free_temp+1
#define ByThread_use_intervals 2 // "1" => times reported in milliseconds,
                                 // "2" => times reported in seconds,
                                 //  otherwise don't add anything.
#include "SS_View_bythread_locations.hxx"
#include "SS_View_bythread_setmetrics.hxx"

#define def_hwc_values          \
            CommandResult *excnt = CRPTR (0);

#define get_inclusive_values(primary, num_calls, function_name)

#define get_exclusive_hwc_values(secondary, num_calls)        \
             if (excnt == NULL) excnt =  secondary->Copy();   \
             else excnt->Accumulate_Value (secondary);

#define set_hwc_values(value_array, sort_extime)                                       \
              if (num_temps > VMulti_sort_temp) value_array[VMulti_sort_temp] = NULL;  \
              if (num_temps > excnt_temp) value_array[excnt_temp] = excnt;

#define def_Detail_values def_hwc_values
#define set_Detail_values set_hwc_values
#define get_exclusive_values get_exclusive_hwc_values
#define Determine_Objects Get_Filtered_Objects
#include "SS_View_detail.txx"

static std::string allowed_hwc_V_options[] = {
  "LinkedObject",
  "LinkedObjects",
  "Dso",
  "Dsos",
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Loop",
  "Loops",
  "Summary",
  "SummaryOnly",
  "data",       // Raw data output for scripting
  ""
};


static bool define_hwc_columns (
            CommandObject *cmd,
            ExperimentObject *exp,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;
  int64_t totalIndex  = 0;  // Number of totals needed to perform % calculations.
  int64_t last_used_temp = Last_ByThread_Temp; // Track maximum temps - needed for expressions.
  bool Generate_Summary = false;
  bool Generate_Summary_Only = Look_For_KeyWord(cmd, "SummaryOnly");
  if (!Generate_Summary_Only) {
     Generate_Summary = Look_For_KeyWord(cmd, "Summary");
  }

  bool Generate_ButterFly = Look_For_KeyWord(cmd, "ButterFly");
  int64_t View_ByThread_Identifier = Determine_ByThread_Id (exp, cmd);

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, VMulti_time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, excnt_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<ParseRange> *p_slist = p_result->getexpMetricList();
  std::string ByThread_Header = Find_Metadata ( CV[0], MV[0] ).getShortName();

  if (Generate_ButterFly) {
   // No stack trace captured by this collector.
    Generate_ButterFly = FALSE;
    Mark_Cmd_With_Soft_Error(cmd,"Warning: '-v ButterFly' is not supported with this view.");
  }
  
  //if (Generate_Summary) {
  // // Total counts is always displayed - also add display of the summary time.
  //  IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
  //}

 // Define map for metrics to metric temp.
   std::map<std::string, int64_t> MetricMap;
   MetricMap["count"] = excnt_temp;
   MetricMap["counts"] = excnt_temp;
   MetricMap["overflow"] = excnt_temp;
   MetricMap["overflows"] = excnt_temp;

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    std::vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {

// Look for a metric expression and invoke processing.
#include "SS_View_metric_expressions.hxx"

      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        if (strcasecmp(C_Name.c_str(), "hwc")) {
         // We only know what to do with the hwc collector.
          std::string s("The specified collector, " + C_Name +
                        ", can not be displayed as part of a 'hwc' view.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          return false;
        }
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

      if (!strcasecmp(M_Name.c_str(), "count") ||
          !strcasecmp(M_Name.c_str(), "counts") ||
          !strcasecmp(M_Name.c_str(), "overflow") ||
          !strcasecmp(M_Name.c_str(), "overflows")) {
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
       // Get the name of the event that we were collecting.
       // Use this for the column header in the report rather then the name of the metric.
        std::string H;
        CV[0].getParameterValue ("event", H);
        HV.push_back(std::string("Exclusive ") + H + " Counts");
      } else if (!strcasecmp(M_Name.c_str(), "percent") ||
                 !strcmp(M_Name.c_str(), "%")           ||
                 !strcasecmp(M_Name.c_str(), "%count")   ||
                 !strcasecmp(M_Name.c_str(), "%counts")   ||
                 !strcasecmp(M_Name.c_str(), "%overflow")   ||
                 !strcasecmp(M_Name.c_str(), "%overflows")) {
       // percent is calculate from 2 temps: time for this row and total time.
        IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));
        std::string H;
        CV[0].getParameterValue ("event", H);
        HV.push_back(std::string("% of Total ") + H + " Counts");
      } else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                 !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
       // Do a By-Thread average.
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_mean,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
        HV.push_back("Average Counts Across Threads");
      } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
       // Find the By-Thread Min.
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_mean,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
        HV.push_back("Minimum Counts Across Threads");
      } else if (!strcasecmp(M_Name.c_str(), "ThreadMinIndex")) {
       // Find the Rank of the By-Thread Min.
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_imin,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
        HV.push_back(std::string(View_ByThread_Id_name[View_ByThread_Identifier]) + " of Min");
      } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
       // Find the By-Thread Max.
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_max,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
        HV.push_back("Maximum Counts Across Threads");
      } else if (!strcasecmp(M_Name.c_str(), "ThreadMaxIndex")) {
       // Find the Rank of the By-Thread Max.
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_imax,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
        HV.push_back(std::string(View_ByThread_Id_name[View_ByThread_Identifier]) + " of Max");
      } else if (!strcasecmp(M_Name.c_str(), "loadbalance")) {
       // ByThreadMax 
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_max,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
        HV.push_back("Maximum Counts Across Threads");
       // ByThreadMaxIndex
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_imax,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
        HV.push_back(std::string(View_ByThread_Id_name[View_ByThread_Identifier]) + " of Max");
       // ByThreadMin 
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_mean,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
        HV.push_back("Minimum Counts Across Threads");
       // ByThreadMinIndex
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_imin,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
        HV.push_back(std::string(View_ByThread_Id_name[View_ByThread_Identifier]) + " of Min");
       // ByThreadAverage
        IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 0,
                                          ViewReduction_mean,
                                          View_ByThread_Identifier));
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
        HV.push_back("Average Counts Across Threads");
      } else if (!strcasecmp(M_Name.c_str(), "absdiff")) {
        // Ignore this because cview -c 3 -c 5 -mtime,absdiff actually works outside of this view code
        //Mark_Cmd_With_Soft_Error(cmd,"AbsDiff option, '-m " + M_Name + "'");
      } else {
       // Unrecognized '-m' option.
        Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        return false;
      }
    }

  } else {
   // If nothing is requested ...
   // There is only 1 supported metric.  Use it and also generate the percent.
   IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, excnt_temp));
   IV.push_back(new ViewInstruction (VIEWINST_Define_Total_Tmp, totalIndex, excnt_temp));
   IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, excnt_temp, totalIndex++));

   // Get the name of the event that we were collecting.
   // Use this for the column header in the report rather then the name of the metric.
    std::string H;
    CV[0].getParameterValue ("event", H);
    HV.push_back(std::string("Exclusive ") + H + " Counts");
    HV.push_back(std::string("% of Total ") + H + " Counts");
  }

  // Add display of the summary time.
  if (Generate_Summary_Only) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Only));
   } else if (Generate_Summary) {
     IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
   }

  return (HV.size() > 0);
}

static bool hwc_definition (
                             CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {

    // Warn about misspelled of meaningless options and exit command processing without generating a view.
    bool all_valid = Validate_V_Options (cmd, allowed_hwc_V_options);
    if ( all_valid == false ) {
      return false;
    }


   // Initialize the one supported metric.
    CV.push_back( Get_Collector (exp->FW(), "hwc") ); // use hwc collector
    MV.push_back("overflows");
   // The use of 'SS_View_bythread_recognize.hxx' would require the following:
   // CV.push_back ( CV[0] ); // Collector needed for ByThread calcualtions.
   // MV.push_back ( MV[0] ); // Metric needed for ByThread calcualtions.
   // This is not needed because ByThread recognition is done within this
   // module so that non-default headers can be used.  The definitions for
   // the ByThread Collector and metric ID is hard coded to "0".
    
    return define_hwc_columns (cmd, exp, CV, MV, IV, HV, vfc);
}

static std::string VIEW_hwc_brief = "Hardware counter report";
static std::string VIEW_hwc_short = "Report the hardware counts spent in a code unit.";
static std::string VIEW_hwc_long  = 
                "The report is sorted in descending order by the number of counts that"
                " were accumulated in each unit.  The reported counter is the one"
                " set in the 'event' parameter when the experiment was run."
                " Also included in the report is the percent of total cycles"
                " that each unit uses."
                " A positive integer can be added to the end of the keyword 'hwc'"
                " to indicate the maximum number of items in the report."
                "\n\nThe type of unit displayed can be selected with the '-v'"
                " option."
                "\n\t'-v LinkedObjects' will report counts by linked object."
                "\n\t'-v Functions' will report counts by function. This is the default."
                "\n\t'-v Statements' will report counts by statement."
                "\n\t'-v Loops' will report counts by loop."
                "\n\tThe addition of 'Summary' to the '-v' option list along with 'Functions',"
                " 'Statements', 'LinkedObjects' or 'Loops' will result in an additional line of output at"
                " the end of the report that summarizes the information in each column."
                "\n\tThe addition of 'SummaryOnly' to the '-v' option list along with 'Functions',"
                " 'Statements', 'LinkedObjects' or 'Loops' or without those options will cause only the"
                " one line of output at the end of the report that summarizes the information in each column."
                "\n\nThe information included in the report can be controlled with the"
                " '-m' option.  More than one item can be selected but only the items"
                " listed after the option will be printed and they will be printed in"
                " the order that they are listed."
                " If no '-m' option is specified, the default is equivalent to"
                " '-m counts, percent'."
                " Each value pertains to the function, statement or linked object that is"
                " on that row of the report.  The 'Thread...' selections pertain to the"
                " process unit that the program was partitioned into: Pid's,"
                " Posix threads, Mpi threads or Ranks."
                " \n\t'-m counts' reports the total hardware event counts for all the processes."
                " \n\t'-m percent' reports the percent of total counts for all the processes."
                " \n\t'-m ThreadMax' reports the maximum counts for a process."
                " \n\t'-m ThreadMaxIndex' reports the procecss Id for the thread of the 'ThreadMax'."
                " \n\t'-m ThreadMin' reports the minimum counts for a process."
                " \n\t'-m ThreadMinIndex' reports the procecss Id for the thread of the 'ThreadMin'."
                " \n\t'-m ThreadAverage' reports the average counts for a process."
                " \n\t'-m loadbalance' is the same as '-m ThreadMax, ThreadMaxIndex, ThreadMin,"
                " ThreadMinIndex, ThreadAverage'."
                "\n";
static std::string VIEW_hwc_example = "\texpView hwc\n"
                                      "\texpView -v Functions hwc10\n";
static std::string VIEW_hwc_metrics[] =
  { "overflows",
    ""
  };
static std::string VIEW_hwc_collectors[] =
  { "hwc",
    ""
  };
class hwc_view : public ViewType {

 public: 
  hwc_view() : ViewType ("hwc",
                         VIEW_hwc_brief,
                         VIEW_hwc_short,
                         VIEW_hwc_long,
                         VIEW_hwc_example,
                        &VIEW_hwc_metrics[0],
                        &VIEW_hwc_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (hwc_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      switch (vfc) {
       case VFC_Function:
        Function *fp;
        return Simple_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, fp, vfc, view_output);
       case VFC_LinkedObject:
        LinkedObject *lp;
        return Simple_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, lp, vfc, view_output);
       case VFC_Statement:
        Statement *sp;
        return Simple_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, sp, vfc, view_output);
       case VFC_Loop:
        Loop *loopp;
        return Simple_Base_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV,
                                   true, loopp, vfc, view_output);
      }
      Mark_Cmd_With_Soft_Error(cmd, "(We could not determine which format to use for the report.)");
      return false;
    }
    Mark_Cmd_With_Soft_Error(cmd, "(We could not determine what information to report for 'hwc' view.)");
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void hwc_view_LTX_ViewFactory () {
  Define_New_View (new hwc_view());
}
