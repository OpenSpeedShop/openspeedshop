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
                   std::set<TE>& objects,
                   std::vector<std::pair<TE, CommandResult *> >& items) {
    int64_t i;

   // Generate data for the first column.
   // Be sure we sort the items based on the metric displayed in the first column.
    SmartPtr<std::map<TE, CommandResult *> > initial_items =
            Framework::SmartPtr<std::map<TE, CommandResult *> >(
                new std::map<TE, CommandResult * >()
                );
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
                     std::vector<std::pair<TE, CommandResult *> >& items,
                     std::list<CommandResult *>& view_output) {
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

     // Check for asnychonous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      CommandResult_Columns *C = new CommandResult_Columns ();
      CommandResult *percent_of = NULL;
      bool column_one_used = false;

     // Add Metrics
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *vinst = ViewInst[i];
        int64_t CM_Index = vinst->TMP1();

        CommandResult *Next_Metric_Value = NULL;
        if (vinst->OpCode() == VIEWINST_Display_Metric) {
          if (i == 0) {
            Next_Metric_Value = it->second;
            column_one_used = true;
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
      view_output.push_back (C);

     // These CommandObjects are reused. Avoid deletion in Reclaim_CR_Space.
      if (column_one_used) {
        it->second = NULL;
      }
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
      view_output.push_back (E);
    }

}

// Generic routine to generate a simple view

static std::string allowed_stats_V_options[] = {
  "LinkedObject",
  "LinkedObjects",
  "Dso",
  "Dsos",
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Summary",
  "data",   	// Raw data output for scripting
  ""
};


/**
 * Method: Generic_View
 *
 * Generic routine to generate a simple view
 * 
 *
 * @param   CommandObject *cmd
 * @param   ExperimentObject *exp
 * @param   int64_t topn
 * @param   ThreadGroup& tgrp
 * @param   std::vector<Collector>& CV
 * @param   std::vector<std::string>& MV
 * @param   std::vector<ViewInstruction *>& IV
 * @param   std::vector<std::string>& HV
 * @param   std::list<CommandResult *>& view_output
 *
 * @return  true/false
 *
 *
 */
bool Generic_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                   ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                   std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                   std::list<CommandResult *>& view_output) {
  bool success = false;
  // Print_View_Params (cerr, CV,MV,IV);

 // Warn about misspelled of meaningless options.
  Validate_V_Options (cmd, allowed_stats_V_options);

  bool report_Column_summary = false;
  CommandResult *TotalValue = NULL;
  std::vector<std::pair<Function, CommandResult *> > f_items;
  std::vector<std::pair<Statement, CommandResult *> > s_items;
  std::vector<std::pair<LinkedObject, CommandResult *> > l_items;
  int64_t i;
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
    if (Look_For_KeyWord(cmd, "Statement") ||
        Look_For_KeyWord(cmd, "Statements")) {
      vg = VIEW_STATEMENTS;
    } else if (Look_For_KeyWord(cmd, "LinkedObject") ||
               Look_For_KeyWord(cmd, "LinkedObjects") ||
               Look_For_KeyWord(cmd, "Dso") ||
               Look_For_KeyWord(cmd, "Dsos")) {
      vg = VIEW_LINKEDOBJECTS;
    }

   // Acquire base set of metric values.
    ViewInstruction *vinst0 = ViewInst[0];
    int64_t Column0index = vinst0->TMP1();
    std::set<Function> f_objects;
    std::set<Statement> s_objects;
    std::set<LinkedObject> l_objects;
    bool first_column_found = false;
    std::string EO_Title;
    switch (vg) {
     case VIEW_STATEMENTS:
      Get_Filtered_Objects (cmd, exp, tgrp, s_objects);
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, s_objects, s_items);
      topn = min(topn, (int64_t)s_items.size());
      EO_Title = "Statement Location (Line Number)";
      break;
     case VIEW_LINKEDOBJECTS:
      Get_Filtered_Objects (cmd, exp, tgrp, l_objects);
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, l_objects, l_items);
      topn = min(topn, (int64_t)l_items.size());
      EO_Title = "LinkedObject";
      break;
     default:
      Get_Filtered_Objects (cmd, exp, tgrp, f_objects);
      first_column_found = First_Column (cmd, tgrp, CV, MV, Column0index, f_objects, f_items);
      topn = min(topn, (int64_t)f_items.size());
      EO_Title = "Function Name";
      break;
    }
/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (!first_column_found) {
      std::string s("(There are no data samples for " + MV[Column0index] + " available.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no data, return.
    }
*/

   // Calculate %?
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
        if (HV.begin() != HV.end()) {
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
    view_output.push_back (H);

   // Now format the view.
    switch (vg) {
     case VIEW_STATEMENTS:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      s_items, view_output);
      break;
     case VIEW_LINKEDOBJECTS:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      l_items, view_output);
      break;
     default:
      Construct_View (cmd, topn, tgrp, CV, MV, IV,
                      num_columns,
                      ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                      f_items, view_output);
      break;
    }

    success = true;
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
  }

 // Release space for no longer needed items.
  Reclaim_CR_Space (s_items);
  Reclaim_CR_Space (l_items);
  Reclaim_CR_Space (f_items);
  if (TotalValue != NULL) delete TotalValue;

 // Release instructions
  for (i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }

  return success;
}

// Select the metrics that the user specified and display only them.
bool Select_User_Metrics (CommandObject *cmd, ExperimentObject *exp,
                          std::vector<Collector>& CV, std::vector<std::string>& MV,
                          std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  int64_t i = 0;
  vector<ParseRange>::iterator mi;
  for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
    parse_range_t *m_range = (*mi).getRange();
    std::string C_Name;
    std::string M_Name;
    if (m_range->is_range) {
      C_Name = m_range->start_range.name;
      M_Name = m_range->end_range.name;
      if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
        std::string s("The specified collector, " + C_Name +
                      ", was not used in the experiment.");
        Mark_Cmd_With_Soft_Error(cmd,s);
        return false;
      }
    } else {
      M_Name = m_range->start_range.name;
      if ((exp != NULL) &&
          (exp->FW() != NULL) &&
          (M_Name.length() > 0)) {
        CollectorGroup cgrp = exp->FW()->getCollectors();
        // CollectorGroup cgrp = exp->FW()->getCollectors();
        C_Name = Find_Collector_With_Metric ( cgrp, M_Name);
        if (C_Name.length() == 0) {
          std::string s("The specified metric, " + M_Name +
                          " was not generated for the experiment.");
          Mark_Cmd_With_Soft_Error(cmd,s);
          return false;
        }
      }
    }

    Collector C = Get_Collector (exp->FW(), C_Name);
    if (!Collector_Generates_Metric ( C, M_Name)) {
      std::string s("The specified collector, " + C_Name +
                    ", does not generate the specified metric, " + M_Name);
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    CV.push_back(C);
    MV.push_back(M_Name);
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, i, i));

    std::string column_header;
    if (Metadata_hasName( C, M_Name)) {
      Metadata m = Find_Metadata ( C, M_Name );
      column_header = m.getShortName();
    } else {
      column_header = M_Name;
    }
    HV.push_back(column_header);

    i++;
  }

  return true;
}

// Select every metric defined in the experiment and display it.
static bool Select_All_Metrics (CommandObject *cmd, ExperimentObject *exp,
                                std::vector<Collector>& CV, std::vector<std::string>& MV,
                                std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV) {
 // Use the metrics specified in the experiment definition.
  if ((exp == NULL) ||
      (exp->FW() == NULL)) {
    std::string s("There was no data collected for the experiment.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    return false;
  }

  bool collector_found = false;
  int64_t column = 0;
  CollectorGroup cgrp = exp->FW()->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
    Collector c = *ci;
    Metadata cm = c.getMetadata();
    std::set<Metadata> md = c.getMetrics();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      std::string M_Name = m.getUniqueId();
      CV.push_back(c);
      MV.push_back(M_Name);
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, column, column));

      std::string column_header;
      if (Metadata_hasName( c, M_Name)) {
        Metadata m = Find_Metadata ( c, M_Name );
        column_header = m.getShortName();
      } else {
        column_header = M_Name;
      }
      HV.push_back(column_header);

      column++;
      collector_found = true;
    }
  }

  return collector_found;
}

// stats view

static std::string VIEW_stats_brief = "Generic Report";
static std::string VIEW_stats_short = "Report the metric values gathered for each function in a program.";
static std::string VIEW_stats_long  = "The report is sorted in descending order by the first metric."
                                      " A positive integer can be added to the end of the keyword"
                                      " 'stats' to indicate the maximum number of items in the report."
                                      "\n\nThe general form of the display can be selected with the '-v'"
                                      " option."
                                      "\n\t'-v LinkedObjects' will report metrics by linked object."
                                      "\n\t'-v Functions' will report metrics by function. This is the default."
                                      "\n\t'-v Statements' will report metrics by statement."
                                      " \n\nThe user can select individual metrics for display by listing"
                                      " them after the '-m' option key.  Multiple selections will be"
                                      " displayed in the order they are listed.  Only the metrics in"
                                      " the list will be displayed."
                                      " \n\nIf the '-m' option is not specified, an attempt is made to"
                                      " include all the metrics gathered for the experiment."
                                      "\n\nOnly simple data values that can be associated with"
                                      " single function, statement or linkedobject can be handled.";
static std::string VIEW_stats_example = "\texpView stats\n"
                                        "\texpView -v LinkedObjects stats5 -m SomeCollector::time\n";
static std::string VIEW_stats_metrics[] =
  { ""
  };
static std::string VIEW_stats_collectors[] =
  { ""
  };
class stats_view : public ViewType {

 public: 
  stats_view() : ViewType ("stats",
                            VIEW_stats_brief,
                            VIEW_stats_short,
                            VIEW_stats_long,
                            VIEW_stats_example,
                           &VIEW_stats_metrics[0],
                           &VIEW_stats_collectors[0],
                            true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
    vector<ParseRange> *p_slist = p_result->getexpMetricList();
    if (p_slist->empty()) {
     // If we are attempting a default 'stats' view, show every available data metric.
      if (!Select_All_Metrics (cmd, exp, CV, MV, IV, HV)) {
        return false;
      }
    } else {
     // Otherwise, use the metrics that the user listed.
      if (!Select_User_Metrics (cmd, exp, CV, MV, IV, HV)) {
        return false;
      }
    }
    return Generic_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
  }
};


// This is not the only external entrypoint.
//   Also exported are: 'Generic_View' and 'Select_User_Metrics'
// However, Calls to the 'stats' VIEW needs to be done through the ViewType class objects.
extern "C" void stats_LTX_ViewFactory () {
  Define_New_View (new stats_view());
}
