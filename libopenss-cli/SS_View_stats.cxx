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

enum View_Granularity {
  VIEW_FUNCTIONS,
  VIEW_STATEMENTS,
  VIEW_LINKEDOBJECTS
};

template <class T>
struct sort_ascending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return CommandResult_lt (x.second, y.second);
    }
};
template <class T>
struct sort_descending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return CommandResult_gt (x.second, y.second);
    }
};

template <typename TE>
bool First_Column (CommandObject *cmd,
                   ThreadGroup& tgrp,
                   std::vector<Collector>& CV,
                   std::vector<std::string>& MV,
                   int64_t Column0index,
                   std::vector<std::pair<TE, CommandResult *> >& items) {
    int64_t i;

   // Generate data for the first column.
   // Be sure we sort the items based on the metric displayed in the first column.
    SmartPtr<std::map<TE, CommandResult *> > initial_items =
            Framework::SmartPtr<std::map<TE, CommandResult *> >(
                new std::map<TE, CommandResult * >()
                );
    std::set<TE> objects;
    Get_Filtered_Objects (cmd, tgrp, objects);
    if (objects.empty()) {
      return false;
    }
    GetMetricByObjectSet (cmd, tgrp, CV[Column0index], MV[Column0index], objects, initial_items);
    typename std::map <TE, CommandResult *>::const_iterator ii;
    for(ii = initial_items->begin(); ii != initial_items->end(); ii++ ) {
      items.push_back (std::make_pair(ii->first, ii->second));
    }

   // Now we can sort the data.
    std::sort(items.begin(), items.end(), sort_descending_CommandResult<std::pair<TE, CommandResult *> >());

    return !items.empty();
}

template <typename TE>
void Construct_View (CommandObject *cmd,
                     int64_t topn,
                     ThreadGroup& tgrp,
                     std::vector<Collector>& CV,
                     std::vector<std::string>& MV,
                     std::vector<ViewInstruction *>& IV,
                     int64_t num_columns,
                     std::vector<ViewInstruction *>& ViewInst,
                     bool Gen_Total_Percent,
                     int64_t percentofcolumn,
                     CommandResult *TotalValue,
                     bool report_Column_summary,
                     std::vector<std::pair<TE, CommandResult *> >& items) {
    int64_t i;

   // Should we accumulate column sums?
    std::vector<CommandResult *> Column_Sum(topn);
    if (report_Column_summary) {
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *vinst = ViewInst[i];
        int64_t CM_Index = vinst->TMP1();
        Column_Sum[i] = Init_Collector_Metric ( cmd, CV[CM_Index], MV[CM_Index] );
      }
    }

   // Extract the top "n" items from the sorted list and get all the metric values.
    std::set<TE> objects;   // Build set of objects only once.
    std::vector<SmartPtr<std::map<TE, CommandResult *> > > Values(num_columns);
    for ( i=1; i < num_columns; i++) {
      ViewInstruction *vinst = ViewInst[i];
      if (vinst->OpCode() == VIEWINST_Display_Metric) {
        int64_t CM_Index = vinst->TMP1();

        if (objects.empty()) {
          typename std::vector<std::pair<TE, CommandResult *> >::const_iterator it = items.begin();
          for(int64_t foundn = 0; (foundn < topn); foundn++, it++ ) {
            objects.insert(it->first);
          }
        }

        SmartPtr<std::map<TE, CommandResult *> > column_values =
            Framework::SmartPtr<std::map<TE, CommandResult *> >(
                new std::map<TE, CommandResult * >()
                );
        GetMetricByObjectSet (cmd, tgrp, CV[CM_Index], MV[CM_Index], objects, column_values);
        Values[i] = column_values;
      }
    }

   // Extract the top "n" items from the sorted list.
    typename std::vector<std::pair<TE, CommandResult *> >::iterator it = items.begin();
    for(int64_t foundn = 0; (foundn < topn); foundn++, it++ ) {
      CommandResult *percent_of = NULL;

     // Check for asnychonous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      CommandResult_Columns *C = new CommandResult_Columns ();

     // Add Metrics
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *vinst = ViewInst[i];
        int64_t CM_Index = vinst->TMP1();

        CommandResult *Next_Metric_Value = NULL;
        if (vinst->OpCode() == VIEWINST_Display_Metric) {
          if (i == 0) {
            Next_Metric_Value = it->second;
          } else if (Values[i].isNull()) {
           // There is no map - look up the individual function.
            Next_Metric_Value = Get_Object_Metric( cmd, it->first, tgrp,
                                                     CV[CM_Index], MV[CM_Index] );
          } else {
           // The entry should be in the column's values map for this function.
            Next_Metric_Value = NULL;
            SmartPtr<std::map<TE, CommandResult *> > column_values = Values[i];
            typename std::map<TE, CommandResult *>::iterator sm = column_values->find(it->first);
            if (sm != column_values->end()) {
              Next_Metric_Value = sm->second;
            }
          }
          if (Next_Metric_Value == NULL) {
            Next_Metric_Value = Init_Collector_Metric ( cmd, CV[CM_Index], MV[CM_Index] );
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
          // Next_Metric_Value  = ???
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
          if (!Gen_Total_Percent) {
           // The measured time interval is too small.
            continue;
          }
          if ((i > percentofcolumn) &&
              (percent_of != NULL)) {
            Next_Metric_Value = Calculate_Percent (percent_of, TotalValue);
          } else {
            ViewInstruction *percentInst = Find_Column_Def (IV, vinst->TMP1());
            int64_t percentIndex = percentInst->TMP1();
            CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
                                                               CV[percentIndex], MV[percentIndex] );
            Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          // Next_Metric_Value = NULL; /?? not sure how to implement this
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
          if (!Gen_Total_Percent) {
           // The measured time interval is too small.
            continue;
          }
          CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
                                                               CV[CM_Index], MV[CM_Index] );
          Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
        }
        if (Next_Metric_Value == NULL) {
          Next_Metric_Value = CRPTR ("");
        }
        C->CommandResult_Columns::Add_Column (Next_Metric_Value);
        if (report_Column_summary) {
          Accumulate_CommandResult (Column_Sum[i], Next_Metric_Value);
        }
        if (Gen_Total_Percent &&
            (i == percentofcolumn)) {
          percent_of = Next_Metric_Value;
        }
      }
     // Add ID for row
      C->CommandResult_Columns::Add_Column ( CRPTR(it->first) );
      cmd->Result_Predefined (C);
    }

    if (report_Column_summary) {
     // Build an Ender summary for the table.
      CommandResult_Enders *E = new CommandResult_Enders ();
     // Add Metrics Summary
      for ( i=0; i < num_columns; i++) {
        E->CommandResult_Enders::Add_Ender (Column_Sum[i]);
      }
     // Add ID
      E->CommandResult_Enders::Add_Ender ( CRPTR ( "Report Summary" ) );
      cmd->Result_Predefined (E);
    }

}

// stats view

static std::string VIEW_stats_brief = "Generic Report";
static std::string VIEW_stats_short = "Report the metric values gathered for each function in a program.";
static std::string VIEW_stats_long  = "The report is sorted in descending order by the first metric."
                                      " A positive integer can be added to the end of the keyword"
                                      " ""stats"" to indicate the maximum number of items in"
                                      " the report.";
static std::string VIEW_stats_metrics[] =
  { ""
  };
static std::string VIEW_stats_collectors[] =
  { ""
  };
static std::string VIEW_stats_header[] =
  { ""
  };
bool Generic_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                   ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                   std::vector<ViewInstruction *> IV, std::string *HV) {
  // Print_View_Params (cerr, CV,MV,IV);

  bool report_Column_summary = false;

  if (topn == 0) topn = INT_MAX;

  try {
    if (CV.size() == 0) {
      std::string s("(There are no metrics specified to report.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no collector, return.
    }

   // Set up quick access to instructions for columns.
    int64_t num_columns = 0;
    std::vector<ViewInstruction *> ViewInst;
    int64_t i;
    for ( i=0; i < IV.size(); i++) {
      ViewInstruction *vinst = Find_Column_Def (IV, i);
      if (vinst == NULL) {
       // Exit if we didn't find a definition
        break;
      }
      num_columns++;
      ViewInst.push_back(vinst);
    }   
    if (num_columns == 0) {
      std::string s("(There is no display requested.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no column[0] defined, return.
    }
    if (ViewInst[0]->OpCode() != VIEWINST_Display_Metric) {
      std::string s("(The first column is not a metric.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is nothing to sort on.
    }

   // What granularity has been requested?
    View_Granularity vg = VIEW_FUNCTIONS;
    if (Look_For_KeyWord(cmd, "Statements")) {
      vg = VIEW_STATEMENTS;
    } else if (Look_For_KeyWord(cmd, "LinkedObjects")) {
      vg = VIEW_LINKEDOBJECTS;
    }

   // Acquire base set of metric values.
    ViewInstruction *vinst0 = ViewInst[0];
    int64_t Column0index = vinst0->TMP1();
    std::vector<std::pair<Function, CommandResult *> > f_items;
    std::vector<std::pair<Statement, CommandResult *> > s_items;
    std::vector<std::pair<LinkedObject, CommandResult *> > l_items;
    bool first_column_found = false;
    std::string EO_Title;
    switch (vg) {
     case VIEW_STATEMENTS:
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, s_items);
      topn = min(topn, (int64_t)s_items.size());
      EO_Title = "Statement Location (Line Number)";
      break;
     case VIEW_LINKEDOBJECTS:
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, l_items);
      topn = min(topn, (int64_t)l_items.size());
      EO_Title = "LinkedObject";
      break;
     default:
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, f_items);
      topn = min(topn, (int64_t)f_items.size());
      EO_Title = "Function Name";
      break;
    }
    if (!first_column_found) {
      std::string s("(There are no data samples for " + MV[Column0index] + " available.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no data, return.
    }

   // Calculate %?
    CommandResult *TotalValue = NULL;
    ViewInstruction *totalInst = Find_Total_Def (IV);
    bool Gen_Total_Percent = (totalInst != NULL);
    int64_t totalIndex = 0;
    int64_t percentofcolumn = -1;
    if (Gen_Total_Percent) {
      totalIndex = totalInst->TMP1(); // this is a CV/MV index, not a column number!
      ViewInstruction *vinst = Find_Percent_Def (IV);
      if (vinst != NULL) {
        if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
         // This is the column number!  Save to avoid recalculateion.
          percentofcolumn = vinst->TMP1(); // this is the column number!
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
         // We will recalcualte the value when we generate the %.
        } else {
         // Note yet implemented??
          Gen_Total_Percent = false;
        }
      } else {
       // No % displayed, so why calcualte total?
        Gen_Total_Percent = false;
      }
    }
    if (Gen_Total_Percent) {
     // We calculate Total by adding all the values that were recorded for the thread group.
      TotalValue = Get_Total_Metric ( cmd, tgrp, CV[totalIndex], MV[totalIndex] );
      if (TotalValue == NULL) {
       // Something went wrong, delete the column of % from the report.
        Gen_Total_Percent = false;
      }
    }

   // Build a Header for each column in the table.
    CommandResult_Headers *H = new CommandResult_Headers ();
   // Add Metrics
    for ( i=0; i < num_columns; i++) {
      ViewInstruction *vinst = ViewInst[i];
      int64_t CM_Index = vinst->TMP1();

      std::string column_header;
      if (vinst->OpCode() == VIEWINST_Display_Metric) {
        if (HV != NULL) {
          column_header = HV[i];
        } else if (Metadata_hasName( CV[CM_Index], MV[CM_Index] )) {
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
        if (!Gen_Total_Percent) {
         // The measured time interval is too small.
          continue;
        }
        column_header = "% of Total";
      }
      H->CommandResult_Headers::Add_Header ( CRPTR ( column_header ) );
    }
   // Add Entry Object name
    H->CommandResult_Headers::Add_Header ( CRPTR ( EO_Title ) );
    cmd->Result_Predefined (H);

   // Now format the view.
    switch (vg) {
     case VIEW_STATEMENTS:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      s_items);
      break;
     case VIEW_LINKEDOBJECTS:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      l_items);
      break;
     default:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      f_items);
      break;
    }

  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return ((cmd->Status() != CMD_ERROR) && (cmd->Status() != CMD_ABORTED));
}
class stats_view : public ViewType {

 public: 
  stats_view() : ViewType ("stats",
                            VIEW_stats_brief,
                            VIEW_stats_short,
                            VIEW_stats_long,
                           &VIEW_stats_metrics[0],
                           &VIEW_stats_collectors[0],
                           &VIEW_stats_header[0],
                           true,
                           true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV, NULL);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void stats_LTX_ViewFactory () {
  Define_New_View (new stats_view());
}
