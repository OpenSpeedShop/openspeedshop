/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

#include "Python.h"

using namespace std;

#include "SS_Parse_Result.hxx"
#include "SS_Parse_Target.hxx"

using namespace OpenSpeedShop::cli;


// pcfunc view

static std::string VIEW_pcfunc_brief = "pctime : % total : % cumulative : function";
static std::string VIEW_pcfunc_short = "Report the amount and percent of program time spent in a function.";
static std::string VIEW_pcfunc_long  = "Use the program timer to produce a sorted report of the functions"
                                       " that use the most time.  Calculate"
                                       " the percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " ""pcfunc"" to indicate the maximum number of items in"
                                       " the report.";
static std::string VIEW_pcfunc_metrics[] =
  { "time",
    ""
  };
static std::string VIEW_pcfunc_collectors[] =
  { ""
  };
static std::string VIEW_pcfunc_header[] =
  { ""
  };
class pcfunc_view : public ViewType {

 public: 
  pcfunc_view() : ViewType ("pcfunc",
                             VIEW_pcfunc_brief,
                             VIEW_pcfunc_short,
                             VIEW_pcfunc_long,
                            &VIEW_pcfunc_metrics[0],
                            &VIEW_pcfunc_collectors[0],
                            &VIEW_pcfunc_header[0],
                             true,
                             true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    MV.push_back(VIEW_pcfunc_metrics[0]);  // Use the Collector with the first metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Metric, 1, 0));  // second column is %
    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};


// pcsamp view

static std::string VIEW_pcsamp_brief = "pctime : % total : % cumulative : function";
static std::string VIEW_pcsamp_short = "Report the amount and percent of program time spent in a function.";
static std::string VIEW_pcsamp_long  = "Use the program timer to produce a sorted report of the functions"
                                       " that use the most time.  Calculate"
                                       " the percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " ""pcsamp"" to indicate the maximum number of items in"
                                       " the report.";
static std::string VIEW_pcsamp_metrics[] =
  { "time",
    ""
  };
static std::string VIEW_pcsamp_collectors[] =
  { ""
  };
static std::string VIEW_pcsamp_header[] =
  { ""
  };
class pcsamp_view : public ViewType {

 public: 
  pcsamp_view() : ViewType ("pcsamp",
                             VIEW_pcsamp_brief,
                             VIEW_pcsamp_short,
                             VIEW_pcsamp_long,
                            &VIEW_pcsamp_metrics[0],
                            &VIEW_pcsamp_collectors[0],
                            &VIEW_pcsamp_header[0],
                             true,
                             true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    MV.push_back(VIEW_pcsamp_metrics[0]);  // Use the Collector with the first metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Metric, 1, 0));  // second column is %
    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};


// Inclusive Time

static std::string VIEW_intime_brief = "Inclusive Time Report";
static std::string VIEW_intime_short = "Report the amount of time spent in a function.";
static std::string VIEW_intime_long  = "Produce a decending report of the functions that use the"
                                     " most time.  Th reported time includes all the time spent"
                                     " in the functions that are called from the reported function."
                                     " A positive integer can be added to the end of the keyword"
                                     " ""intime"" to indicate the maximum number of items in"
                                     " the report.";
static std::string VIEW_intime_metrics[] =
  { "inclusive_time",
    ""
  };
static std::string VIEW_intime_collectors[] =
  { ""
  };
static std::string VIEW_intime_header[] =
  { ""
  };
class intime_view : public ViewType {

 public: 
  intime_view() : ViewType ("intime",
                             VIEW_intime_brief,
                             VIEW_intime_short,
                             VIEW_intime_long,
                            &VIEW_intime_metrics[0],
                            &VIEW_intime_collectors[0],
                            &VIEW_intime_header[0],
                             true,
                             true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    CV.erase(++CV.begin(), CV.end());
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    MV.push_back(VIEW_intime_metrics[0]);  // Use the Collector with the first metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, 1, 0)); // second column is %

   // The Total Time (used for % calculation) is always the total exclusive time.
   // (Otherwise, we measure a unit of time multiple times.)
    CV.push_back (CV[0]);
    MV.push_back ("exclusive_time");
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, MV.size()-1));

    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};


// Exclusive Time

static std::string VIEW_extime_brief = "Exclusive Time Report";
static std::string VIEW_extime_short = "Report the amount of time spent in a function.";
static std::string VIEW_extime_long  = "Produce a decending report of the functions that use the "
                                     " most time."
                                     " A positive integer can be added to the end of the keyword"
                                     " ""extime"" to indicate the maximum number of items in"
                                     " the report.";
static std::string VIEW_extime_metrics[] =
  { "exclusive_time",
    ""
  };
static std::string VIEW_extime_collectors[] =
  { ""
  };
static std::string VIEW_extime_header[] =
  { ""
  };
class extime_view : public ViewType {

 public: 
  extime_view() : ViewType ("extime",
                             VIEW_extime_brief,
                             VIEW_extime_short,
                             VIEW_extime_long,
                            &VIEW_extime_metrics[0],
                            &VIEW_extime_collectors[0],
                            &VIEW_extime_header[0],
                             true,
                             true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    CV.erase(++CV.begin(), CV.end());
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    MV.push_back(VIEW_extime_metrics[0]);  // Use the Collector with the first metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Metric, 1, 0));  // second column is %
    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};

// UserTime Report

static std::string VIEW_usertime_brief = "User Time Report";
static std::string VIEW_usertime_short = "Report the amount of time spent in a function.";
static std::string VIEW_usertime_long  = "Produce a decending report of the functions that use the "
                                     " most time."
                                     " A positive integer can be added to the end of the keyword"
                                     " ""usertime"" to indicate the maximum number of items in"
                                     " the report.";
static std::string VIEW_usertime_metrics[] =
  { "inclusive_time",
    "exclusive_time",
    ""
  };
static std::string VIEW_usertime_collectors[] =
  { "usertime",
    ""
  };
static std::string VIEW_usertime_header[] =
  { ""
  };
class usertime_view : public ViewType {

 public: 
  usertime_view() : ViewType ("usertime",
                             VIEW_usertime_brief,
                             VIEW_usertime_short,
                             VIEW_usertime_long,
                            &VIEW_usertime_metrics[0],
                            &VIEW_usertime_collectors[0],
                            &VIEW_usertime_header[0],
                             true,
                             true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    if (MV.size() > 2) {
     // There are only two metrics.  Default to the basic report.
      CV.erase(CV.begin(), CV.end());
      MV.erase(MV.begin(), MV.end());
      IV.erase(IV.begin(), IV.end());
    }

    int64_t Max_Column = Find_Max_Column_Def (IV);

   // The user might have already define the metrics, but it doesn't hurt to do it again.
    Collector c = Get_Collector (exp->FW(), VIEW_usertime_collectors[0]);

   // Define inclusive time metric.
    int intime_index = MV.size();
    CV.push_back (c);
    MV.push_back (VIEW_intime_metrics[0]);

   // Define exclusive time metric.
    int extime_index = intime_index + 1;
    CV.push_back (c);
    MV.push_back ("exclusive_time");

   // If the user didn't define the metrics - set the default columns for the report.
    if (IV.size() == 0) {
     // Column[0] is inclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, ++Max_Column, intime_index));
     // Column[1] is exclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, ++Max_Column, extime_index));
      Max_Column = 1;
    }

   // Column[2] is % of  whatever is the first metric in the list.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, ++Max_Column, 0));

   // Report on all functions that have inclusive time.
   // (This offers the greatest coverage, since functions with no time are dropped.)
    IV.push_back(new ViewInstruction (VIEWINST_Define_Base, intime_index));

   // The Total Time (used for % calculation) is always the total exclusive time.
   // (Otherwise, we measure a unit of time multiple times.)
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, extime_index));

    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};


// vtop view

static std::string VIEW_vtop_brief = "pctime : function";
static std::string VIEW_vtop_short = "Report the amount of program time spent in a function.";
static std::string VIEW_vtop_long  = "Produce a decending report of the functions that use the "
                                     " most time as measured by the program counter."
                                     " A positive integer can be added to the end of the keyword"
                                     " ""vtop"" to indicate the maximum number of items in"
                                     " the report.";
static std::string VIEW_vtop_metrics[] =
  { "time",
    ""
  };
static std::string VIEW_vtop_collectors[] =
  { ""
  };
static std::string VIEW_vtop_header[] =
  { "  CPU Time (Seconds)",
    "Function",
    ""
  };
static bool VIEW_vtop (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                       ThreadGroup tgrp, std::vector<Collector>CV, std::vector<std::string> MV) {
  if (topn == 0) topn = INT_MAX;
  try {
    std::vector<Function_double_pair> items = GetDoubleByFunction (cmd, false, tgrp, CV[0], MV[0]);
    if (items.begin() == items.end()) {
      return false;   // There is no data, return.
    }

   // Build a Header for the table - just two items - time and function name.
    Add_Header (cmd, &VIEW_vtop_header[0]);

   // Extract the top "n" items from the sorted list.
    std::vector<Function_double_pair>::const_iterator it = items.begin();
    for(int64_t foundn = 0; (foundn < topn) && (it != items.end()); foundn++, it++ ) {
      CommandResult_Columns *C = new CommandResult_Columns (2);
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (it->second));
      C->CommandResult_Columns::Add_Column (new CommandResult_String (it->first.getName()));
      cmd->Result_Predefined (C);
    }
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return true;
}
class vtop_view : public ViewType {

 public: 
  vtop_view() : ViewType ("vtop",
                           VIEW_vtop_brief,
                           VIEW_vtop_short,
                           VIEW_vtop_long,
                          &VIEW_vtop_metrics[0],
                          &VIEW_vtop_collectors[0],
                          &VIEW_vtop_header[0],
                           true,
                           true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
/* TEST - replace with generic view 
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    MV.push_back(VIEW_vtop_metrics[0]);  // Use the Collector with the metric "time"
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    return Generic_View->GenerateView (cmd, exp, topn, tgrp, CV, MV, IV);
*/
    return VIEW_vtop (cmd, exp, topn, tgrp, CV, MV);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void pcfunc_LTX_ViewFactory () {
  Define_New_View (new vtop_view());
  Define_New_View (new intime_view());
  Define_New_View (new extime_view());
  Define_New_View (new usertime_view());
  Define_New_View (new pcfunc_view());
  Define_New_View (new pcsamp_view());
}
