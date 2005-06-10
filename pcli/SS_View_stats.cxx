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
                        ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                        std::vector<ViewInstruction *>IV) {
  // Print_View_Params (stderr, CV,MV,IV);

  bool report_Column_summary = false;
  std::vector<CommandResult *> Column_Sum;

  if (topn == 0) topn = INT_MAX;

  try {
    if (CV.size() == 0) {
      cmd->Result_String ("(There are no metrics specified to report.)");
      cmd->set_Status(CMD_ERROR);
      return false;   // There is no collector, return.
    }
    ViewInstruction *baseInst = Find_Base_Def (IV);
    int64_t baseIndex = 0;
    if (baseInst != NULL) {
      baseIndex = baseInst->TMP1();
    }
    ViewInstruction *totalInst = Find_Total_Def (IV);
    if (totalInst == NULL) {
      totalInst = Find_Percent_Def (IV);
    }
    bool Gen_Total_Percent = (totalInst != NULL);
    int64_t totalIndex = 0;
    if (Gen_Total_Percent) {
      totalIndex = totalInst->TMP1();
      if (baseInst == NULL) {
        baseIndex = totalIndex;
      }
    }
    std::vector<Function_double_pair> items = GetDoubleByFunction (cmd, false, tgrp, CV[baseIndex], MV[baseIndex]);
    if (items.begin() == items.end()) {
      cmd->Result_String ("(There are no data samples for " + MV[baseIndex] + " available.)");
      cmd->set_Status(CMD_ERROR);
      return false;   // There is no data, return.
    }

   // Calculate %?
    CommandResult *TotalValue = NULL;
    if (Gen_Total_Percent) {
     // We calculate Total Time by adding all the time in the base metric.
     // Unfortunately, there may be time intervals that are not included
     // in the base metric, so the total we calcualte may be less than the
     // actual total.  Worst yet, it ma be less than the total for an item
     // displayed in Column[0] leading to a % reported that is over 100.
      TotalValue = new CommandResult_Float();

      std::vector<Function_double_pair>::const_iterator itt = items.begin();
      for( ; itt != items.end(); itt++ ) {
        Function_double_pair f = *itt;
        CommandResult *Metric_Value = Get_Collector_Metric( cmd, f.first, tgrp,
                                                            CV[totalIndex], MV[totalIndex] );
        Accumulate_CommandResult (TotalValue, Metric_Value);
      }

      double TotalTime = 0.0;
      ((CommandResult_Float *)TotalValue)->Value(TotalTime);
      if (TotalTime < 0.0000000001) {
        cmd->Result_String ( "(The measured time interval is too small.)" );
        cmd->set_Status(CMD_ERROR);
        return false;
      }
    }

   // Build a Header for the table.
    CommandResult_Headers *H = new CommandResult_Headers ();
   // Add Metrics
    int64_t i;
    for ( i=0; i < IV.size(); i++) {
      ViewInstruction *vinst = Find_Column_Def (IV, i);
      if (vinst == NULL) {
       // Exit if we didn't find a definition
        break;
      }
      int64_t CM_Index = vinst->TMP1();

      std::string column_header;
      if (vinst->OpCode() == VIEWINST_Display_Metric) {
        if (Metadata_hasName( CV[CM_Index], MV[CM_Index] )) {
          Metadata m = Find_Metadata ( CV[CM_Index], MV[CM_Index] );
          column_header = m.getShortName();
        } else {
          column_header = MV[CM_Index];
        }
      } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
        column_header = std::string("Temp" + CM_Index);
      } else if ((vinst->OpCode() == VIEWINST_Display_Percent_Column) ||
                 (vinst->OpCode() == VIEWINST_Display_Percent_Metric) ||
                 (vinst->OpCode() == VIEWINST_Display_Percent_Tmp)) {
        column_header = "% of Total";
      }
      H->CommandResult_Headers::Add_Header ( new CommandResult_String ( column_header ) );
      if (report_Column_summary) {
        CommandResult *S = new CommandResult_Float();
        Column_Sum.push_back(S);
      }
    }
   // Add Function
    H->CommandResult_Headers::Add_Header ( new CommandResult_String ( "Function Name" ) );
    if (report_Column_summary) {
      Column_Sum.push_back(NULL);
    }
    cmd->Result_Predefined (H);

   // Be sure we sort the items based on the metric displayed in the first column.
    ViewInstruction *vinst0 = Find_Column_Def (IV, 0);
    if (vinst0 != NULL) {
      int64_t Column0index = vinst0->TMP1();
      if ((Column0index != baseIndex) &&
          ((CV[Column0index] != CV[baseIndex]) ||
           (MV[Column0index] != MV[baseIndex]))) {
        items = GetDoubleByFunction (cmd, false, tgrp, CV[Column0index], MV[Column0index]);
      }
    }

   // Extract the top "n" items from the sorted list.
    std::vector<Function_double_pair>::const_iterator it = items.begin();
    double CumulativePercent = 0; // accumulated percent
    for(int64_t foundn = 0; (foundn < topn) && (it != items.end()); foundn++, it++ ) {
      CommandResult_Columns *C = new CommandResult_Columns ();

     // Add Metrics
      for ( i=0; i < IV.size(); i++) {
        ViewInstruction *vinst = Find_Column_Def (IV, i);
        if (vinst == NULL) {
         // Exit if we didn't find a definition
          break;
        }
        int64_t CM_Index = vinst->TMP1();

        CommandResult *Next_Metric_Value = NULL;
        if (vinst->OpCode() == VIEWINST_Display_Metric) {
          Next_Metric_Value = Get_Collector_Metric( cmd, it->first, tgrp,
                                                    CV[CM_Index], MV[CM_Index] );
        } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
          // Next_Metric_Value  = ???
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
          ViewInstruction *percentInst = Find_Column_Def (IV, vinst->TMP1());
          int64_t percentIndex = percentInst->TMP1();
          CommandResult *Metric_Result = Get_Collector_Metric( cmd, it->first, tgrp,
                                                               CV[percentIndex], MV[percentIndex] );
          Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
        } else if ((vinst->OpCode() == VIEWINST_Display_Percent_Metric) ||
                   (vinst->OpCode() == VIEWINST_Display_Percent_Tmp)) {
          CommandResult *Metric_Result = Get_Collector_Metric( cmd, it->first, tgrp,
                                                               CV[CM_Index], MV[CM_Index] );
          Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
        }
        C->CommandResult_Columns::Add_Column (Next_Metric_Value);
        if (report_Column_summary) {
          Accumulate_CommandResult (Column_Sum[i], Next_Metric_Value);
        }
      }
     // Add ID for row
      C->CommandResult_Columns::Add_Column (gen_F_name (it->first));
      cmd->Result_Predefined (C);
    }

    if (report_Column_summary) {
     // Build an Ender summary for the table.
      CommandResult_Enders *E = new CommandResult_Enders ();
     // Add Metrics Summary
      E->CommandResult_Enders::Add_Ender (Column_Sum[0]);
      if (Gen_Total_Percent) {
        E->CommandResult_Enders::Add_Ender (Column_Sum[1]);
      }
      for ( i=1; i < MV.size(); i++) {
        E->CommandResult_Enders::Add_Ender (Column_Sum[i+(Gen_Total_Percent?1:0)]);
      }
     // Add ID
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
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    return VIEW_stats (cmd, exp, topn, tgrp, CV, MV, IV);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void stats_LTX_ViewFactory () {
  Generic_View = new stats_view();
  Define_New_View (Generic_View);
}
