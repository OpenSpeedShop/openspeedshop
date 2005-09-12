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


// pcsamp view

static std::string VIEW_pcsamp_brief = "PC (Program Counter) report";
static std::string VIEW_pcsamp_short = "Report the amount and percent of program time spent in a function.";
static std::string VIEW_pcsamp_long  = "The report is sorted in descending order by the amount of time that"
                                       " was used in each function.  Also included in the report is the"
                                       " percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " 'pcsamp' to indicate the maximum number of items in"
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
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, 0));  // total the metric in first column
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, 1, 0));  // second column is %
    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};

// UserTime Report

static std::string VIEW_usertime_brief = "Usertime call stack report";
static std::string VIEW_usertime_short = "Report the amount of time spent in a function.";
static std::string VIEW_usertime_long  = "The report is sorted in descending order by the amount of time that"
                                       " was used in each function.  Included in the report is both the time"
                                       " used individually by each function (exclusive_time) and the time used"
                                       " in aggregate by the function and all the functions it calls"
                                       " (inclusive_time).  Also included in the report is the" 
                                       " percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " 'usertime' to indicate the maximum number of items in"
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
                             false) {
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
    MV.push_back ("inclusive_time");

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

   // The Total Time (used for % calculation) is always the total exclusive time.
   // (Otherwise, we measure a unit of time multiple times.)
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, extime_index));

   // Column[2] is % of  whatever is the first metric in the list.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, ++Max_Column, 0));

    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};

// Hardware Counter Report

static std::string VIEW_hwc_brief = "Hardware counter report";
static std::string VIEW_hwc_short = "Report the hardware counts spent in a function.";
static std::string VIEW_hwc_long  = "The report is sorted in descending order by the number of counts that"
                                    " were accumulated in each function.  The reported counter is the one"
                                    " set in the 'event' parameter when the experiment was run."
                                    "  Also included in the report is the percent of total cycles"
                                    " that each function uses."
                                    "  A positive integer can be added to the end of the keyword 'hwc'"
                                    " to indicate the maximum number of items in the report.";
static std::string VIEW_hwc_metrics[] =
  { "overflows",
    ""
  };
static std::string VIEW_hwc_collectors[] =
  { "hwc",
    ""
  };
static std::string VIEW_hwc_header[] =
  { ""
  };
class hwc_view : public ViewType {

 public: 
  hwc_view() : ViewType ("hwc",
                             VIEW_hwc_brief,
                             VIEW_hwc_short,
                             VIEW_hwc_long,
                            &VIEW_hwc_metrics[0],
                            &VIEW_hwc_collectors[0],
                            &VIEW_hwc_header[0],
                             true,
                             false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
   // Start with a clean slate.
    CV.erase(CV.begin(), CV.end());
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    CV.push_back (Get_Collector (exp->FW(), VIEW_hwc_collectors[0]));  // Get the collector
    MV.push_back(VIEW_hwc_metrics[0]);  // Get the name of the metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, 0));  // metric is total
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, 1, 0));  // second column is % of first

   // Get the name of the event that we were collecting.
   // Use this for the column header in the report rather then the name of the metric.
    std::string HV[1];
    CV[0].getParameterValue ("event", HV[0]);

    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
  }
};

// Hardware Counter UserTime Report

static std::string VIEW_hwctime_brief = "Usertime call stack report";
static std::string VIEW_hwctime_short = "Report the amount of time spent in a function.";
static std::string VIEW_hwctime_long  = "The report is sorted in descending order by the amount of time that"
                                       " was used in each function.  Included in the report is both the time"
                                       " used individually by each function (exclusive_time) and the time used"
                                       " in aggregate by the function and all the functions it calls"
                                       " (inclusive_time).  Also included in the report is the" 
                                       " percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " 'hwctime' to indicate the maximum number of items in"
                                       " the report.";
static std::string VIEW_hwctime_metrics[] =
  { "inclusive_overflows",
    "exclusive_overflows",
    ""
  };
static std::string VIEW_hwctime_collectors[] =
  { "hwctime",
    ""
  };
static std::string VIEW_hwctime_header[] =
  { ""
  };
class hwctime_view : public ViewType {

 public: 
  hwctime_view() : ViewType ("hwctime",
                             VIEW_hwctime_brief,
                             VIEW_hwctime_short,
                             VIEW_hwctime_long,
                            &VIEW_hwctime_metrics[0],
                            &VIEW_hwctime_collectors[0],
                            &VIEW_hwctime_header[0],
                             true,
                             false) {
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
    Collector c = Get_Collector (exp->FW(), VIEW_hwctime_collectors[0]);

   // Define inclusive time metric.
    int intime_index = MV.size();
    CV.push_back (c);
    MV.push_back ("inclusive_overflows");

   // Define exclusive time metric.
    int extime_index = intime_index + 1;
    CV.push_back (c);
    MV.push_back ("exclusive_overflows");

   // If the user didn't define the metrics - set the default columns for the report.
    if (IV.size() == 0) {
     // Column[0] is inclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, ++Max_Column, intime_index));
     // Column[1] is exclusive time
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, ++Max_Column, extime_index));
      Max_Column = 1;
    }

   // The Total Time (used for % calculation) is always the total exclusive time.
   // (Otherwise, we measure a unit of time multiple times.)
    IV.push_back(new ViewInstruction (VIEWINST_Define_Total, extime_index));

   // Column[2] is % of  whatever is the first metric in the list.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Column, ++Max_Column, 0));

   // Get the name of the event that we were collecting.
   // Use this for the column header in the report rather then the name of the metric.
    std::string HV[2];
    std::string name;
    CV[0].getParameterValue ("event", name);
    std::string prename;
    for (int64_t CM_Index = 0; CM_Index < 2; CM_Index++) {
      if (Metadata_hasName( CV[CM_Index], MV[CM_Index] )) {
        Metadata m = Find_Metadata ( CV[CM_Index], MV[CM_Index] );
        prename = m.getShortName();
      } else {
        prename = MV[CM_Index];
      }
      int64_t blank_at = prename.find(" ");
      if (blank_at > 0) {
        prename = std::string(prename, 0, blank_at);
      }
      HV[CM_Index] = prename + " " + name;
    }

    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
void Define_Basic_Views () {
  Define_New_View (new usertime_view());
  Define_New_View (new pcsamp_view());
  Define_New_View (new hwc_view());
  Define_New_View (new hwctime_view());
}
