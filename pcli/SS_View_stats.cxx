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

// stats view

static std::string VIEW_stats_brief = "Generic Report";
static std::string VIEW_stats_short = "Report the metric values gathered for each function in a program";
static std::string VIEW_stats_long  = "For each function, produce a report containing the metrics that"
                                      " were gathered.  The report is sorted in descending order by the"
                                      " first metric preorted."
                                      " A positive integer can be added to the end of the keyword"
                                      " ""stats"" to indicate the maximum number of items in"
                                      " the report.";
static std::string VIEW_stats_metrics[] =
  { ""
  };
static std::string VIEW_stats_collectors[] =
  { ""
  };
static std::string VIEW_pcfunc_header[] =
  { ""
  };
static bool VIEW_stats (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                        ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV) {
  bool report_accumulated_percent = true;  // Is this useful nformation?
  bool report_Column_summary = false;

  if (topn == 0) topn = INT_MAX;

  try {
    if (CV.size() == 0) {
      return false;   // There is no collector, return.
    }
    std::vector<Function_double_pair> items = GetDoubleByFunction (cmd, false, tgrp, CV[0], MV[0]);
    if (items.begin() == items.end()) {
      return false;   // There is no data, return.
    }

   // Calculate %?
    bool Gen_Total_Percent = (CV.size() == 1);
    double TotalTime = 0.0;
    if (Gen_Total_Percent) {
      TotalTime = Total_second (items);
      if (TotalTime < 0.0000000001) {
        cmd->Result_String ( "(the measured time interval is too small)" );
        cmd->set_Status(CMD_ERROR);
        return false;
      }
    }

   // convert time to %
    bool Gen_Cumulative_Percent = Gen_Total_Percent;
    double percent_factor = 0;
    if (Gen_Cumulative_Percent) {
      percent_factor = 100.0 / TotalTime;
    }

   // Build a Header for the table.
    CommandResult_Headers *H = new CommandResult_Headers ();
   // Add Metrics
    int64_t i;
    for ( i=0; i < MV.size(); i++) {
      Metadata m = Find_Metadata ( CV[i], MV[i] );
      H->CommandResult_Headers::Add_Header ( new CommandResult_String ( m.getShortName() ) );
      if ((i == 0) && Gen_Total_Percent) {
        H->CommandResult_Headers::Add_Header ( new CommandResult_String ( "% of Total" ) );
      }
      if (report_accumulated_percent) {
       // There is some doubt that this is useful information
        if ((i == 0) && Gen_Cumulative_Percent) {
          H->CommandResult_Headers::Add_Header ( new CommandResult_String ( "Cumulative %" ) );
        }
      }
    }
   // Add Function
    H->CommandResult_Headers::Add_Header ( new CommandResult_String ( "Name" ) );
    cmd->Result_Predefined (H);

   // Extract the top "n" items from the sorted list.
    std::vector<Function_double_pair>::const_iterator it = items.begin();
    double CumulativePercent = 0; // accumulated percent
    for(int64_t foundn = 0; (foundn < topn) && (it != items.end()); foundn++, it++ ) {
      CommandResult_Columns *C = new CommandResult_Columns ();

     // Add Metrics
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (it->second));
      if (Gen_Total_Percent) {
        double c_percent = it->second*percent_factor;  // current item's percent of total time
        CumulativePercent += c_percent;
        C->CommandResult_Columns::Add_Column (new CommandResult_Float (c_percent));
      }
      if (report_accumulated_percent) {
       // There is some doubt that this is useful information
        if (Gen_Cumulative_Percent) {
          C->CommandResult_Columns::Add_Column (new CommandResult_Float (CumulativePercent));
        }
      }
      for ( i=1; i < MV.size(); i++) {
        C->CommandResult_Columns::Add_Column ( Get_Collector_Metric( cmd, it->first, tgrp, CV[i], MV[i] ) );
      }
     // Add Function
      // C->CommandResult_Columns::Add_Column (new CommandResult_String (it->first.getName()));
      C->CommandResult_Columns::Add_Column (gen_F_name (it->first));
      cmd->Result_Predefined (C);
    }

    if (report_Column_summary) {
     // Build an Ender summary for the table.
      CommandResult_Enders *E = new CommandResult_Enders ();
     // Add Metrics
      E->CommandResult_Enders::Add_Ender ( new CommandResult_Float ( TotalTime ) );
      if (Gen_Total_Percent) {
        E->CommandResult_Enders::Add_Ender ( new CommandResult_Float ( CumulativePercent ) );
      }
      for ( i=1; i < MV.size(); i++) {
        E->CommandResult_Enders::Add_Ender ( new CommandResult_String ( " " ) );
      }
     // Add Function
      E->CommandResult_Enders::Add_Ender ( new CommandResult_String ( "Report Totals" ) );
      cmd->Result_Predefined (E);
    }

  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return true;
}
class stats_view : public ViewType {

 public: 
  stats_view() : ViewType ("stats",
                            VIEW_stats_brief,
                            VIEW_stats_short,
                            VIEW_stats_long,
                           &VIEW_stats_metrics[0],
                           &VIEW_stats_collectors[0],
                           &VIEW_pcfunc_header[0],
                           true,
                           true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV) {
    return VIEW_stats (cmd, exp, topn, tgrp, CV, MV);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void stats_LTX_ViewFactory () {
  Generic_View = new stats_view();
  Define_New_View (Generic_View);
}
