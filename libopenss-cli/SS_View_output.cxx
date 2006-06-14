/*******************************************************************************
      std::vector<CommandResult *>& Total_Values) {
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

static void Accumulate_PreDefined_Temps (std::vector<ViewInstruction *>& IV,
                                         std::vector<CommandResult *>& A,
                                         std::vector<CommandResult *>& B) {
  int64_t len = min(IV.size(),A.size());
  for (int64_t i = 0; i < len; i++) {
    ViewInstruction *vp = IV[i];
    if (vp != NULL) {
      ViewOpCode Vop = vp->OpCode();
      if (Vop == VIEWINST_Add) {
        Accumulate_CommandResult (A[i], B[i]);
      } else if (Vop == VIEWINST_Min) {
        Accumulate_Min_CommandResult (A[i], B[i]);
      } else if ((Vop == VIEWINST_Max) ||
                 (Vop == VIEWINST_Summary_Max)) {
        Accumulate_Max_CommandResult (A[i], B[i]);
      }
    }
  }
}

void Construct_View_Output (CommandObject *cmd,
                            ExperimentObject *exp,
                            ThreadGroup& tgrp,
                            std::vector<Collector>& CV,
                            std::vector<std::string>& MV,
                            std::vector<ViewInstruction *>& IV,
                            std::vector<CommandResult *>& Total_Value,
                            std::vector<std::pair<CommandResult *,
                                                  SmartPtr<std::vector<CommandResult *> > > >& items,
                            std::list<CommandResult *>& view_output ) {
// Print_View_Params (cerr, CV,MV,IV);
  int64_t num_columns = Find_Max_Column_Def (IV) + 1;
  int64_t i;
  bool report_Column_summary = false;

  std::vector<ViewInstruction *> ViewInst(num_columns);
  int64_t num_input_temps = (items.empty()) ? 0 : items[0].second->size();
  std::vector<ViewInstruction *> AccumulateInst(num_input_temps);
  for ( i=0; i < num_input_temps; i++) AccumulateInst[i] = NULL;
  bool input_temp_used[num_input_temps];
  std::vector<CommandResult *> summary_temp(num_input_temps);
  for ( i=0; i < num_input_temps; i++) summary_temp[i] = NULL;

 // Reformat the instructions for easier access.
  for (i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if ((vp->OpCode() == VIEWINST_Display_Metric) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
     // Assert (vp->TR() < num_input_temps);
      if (vp->TR() < num_input_temps) ViewInst[vp->TR()] = vp;
    } else if (vp->OpCode() == VIEWINST_Display_Tmp) {
     // Assert (vp->TR() < num_input_temps);
      if (vp->TR() < num_input_temps) ViewInst[vp->TR()] = vp;
    } else if ((vp->OpCode() == VIEWINST_Add) ||
               (vp->OpCode() == VIEWINST_Min) ||
               (vp->OpCode() == VIEWINST_Max)) {
      if (vp->TMP1() < num_input_temps) AccumulateInst[vp->TMP1()] = vp;
    } else if (vp->OpCode() == VIEWINST_Display_Summary) {
      report_Column_summary = (num_input_temps != 0);
    } else if (vp->OpCode() == VIEWINST_Summary_Max) {
      if (vp->TMP1() < num_input_temps) AccumulateInst[vp->TMP1()] = vp;
    }
  }

 // Calculate any Totals that are needed to do percentages.
  int64_t percentofcolumn = -1;
  bool Gen_Total_Percent = (Total_Value.size() > 0);
  if (Gen_Total_Percent) {
    for (i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      if (vp->OpCode() == VIEWINST_Display_Percent_Column) {
       // We only support 1 column selection.
        Assert(percentofcolumn == -1);
        percentofcolumn = vp->TMP1();
        break;
      }
    }
  }

   // Format the report with the items that are in the vector.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator it;
    for(it = items.begin(); it != items.end(); it++ ) {

     // Check for asnychonous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

     // Local data values
      CommandResult *percent_of = NULL;
      CommandResult_Columns *C = new CommandResult_Columns ();

      for ( i=0; i < num_input_temps; i++) {
        input_temp_used[i] = false;
      }

     // Place values into columns.
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *vinst = ViewInst[i];
        int64_t CM_Index = vinst->TMP1();

        CommandResult *Next_Metric_Value = NULL;
        if (vinst->OpCode() == VIEWINST_Display_Metric) {
          Next_Metric_Value = input_temp_used[i] ? Dup_CommandResult( (*it->second)[i] )
                                                 : (*it->second)[i];
          input_temp_used[i] = true;
        } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if ((V != NULL) &&
              !V->isNullValue ()) {
            Next_Metric_Value = input_temp_used[CM_Index] ? Dup_CommandResult( V ) : V;
            input_temp_used[CM_Index] = true;
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
          if (Gen_Total_Percent) {
            if ((i > percentofcolumn) &&
                (percent_of != NULL) &&
                (!percent_of->isNullValue ())) {
              Next_Metric_Value = Calculate_Percent (percent_of, Total_Value[vinst->TMP2()]);
            }
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if (!V->isNullValue ()) {
            Next_Metric_Value = Calculate_Percent (V, Total_Value[vinst->TMP2()]);
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Average_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if (!V->isNullValue ()) {
            Next_Metric_Value = Calculate_Average (V, (*it->second)[vinst->TMP2()]);
          }
        } else if (vinst->OpCode() ==VIEWINST_Display_StdDeviation_Tmp) {
          CommandResult *V1 = (*it->second)[vinst->TMP1()];
          CommandResult *V2 = (*it->second)[vinst->TMP2()];
          CommandResult *V3 = (*it->second)[vinst->TMP3()];
          Next_Metric_Value = Calculate_StdDev (V1, V2, V3);
        }
        if (Next_Metric_Value == NULL) {
          Next_Metric_Value = CRPTR ("");
          Next_Metric_Value->setNullValue();
        }
        C->CommandResult_Columns::Add_Column (Next_Metric_Value);
        if (Gen_Total_Percent &&
            (i == percentofcolumn)) {
          percent_of = Next_Metric_Value;
        }
      }
     // Add ID for row
      // C->CommandResult_Columns::Add_Column (it->first);
      C->CommandResult_Columns::Add_Column (Dup_CommandResult(it->first));
      // it->first = NULL;  // allow only 1 pointer to a CommandResult object
      view_output.push_back (C);  // attach column list to output

     // Accumulate Summary Information
      if (report_Column_summary) {
        if (it == items.begin()) {
         // Copy the first row to initialize the summary values.
          std::vector<CommandResult *> first_row = *(items.begin()->second);
          for ( i=0; i < num_input_temps; i++) {
            // summary_temp[i] = Dup_CommandResult (first_row[i]);
            summary_temp[i] = Dup_CommandResult ((*it->second)[i]);
          }
        } else {
          Accumulate_PreDefined_Temps (AccumulateInst, summary_temp, (*it->second));
        }
      }

     // Reclaim space for unused temps.
      for ( i=0; i < num_input_temps; i++) {
        if (input_temp_used[i]) {
         // allow only 1 pointer to a CommandResult object
          (*it->second)[i] = NULL;
        }
      }
    }

    if (report_Column_summary) {
     // Build an Ender summary for the table.
      CommandResult_Enders *E = new CommandResult_Enders ();
      CommandResult *percent_of = NULL;
     // Add Metrics Summary
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *sinst = ViewInst[i];
        CommandResult *summary = NULL;
        Assert (sinst != NULL);
        if ((sinst->OpCode() == VIEWINST_Display_Tmp) &&
            (sinst->TMP1() < AccumulateInst.size()) &&
            (AccumulateInst[sinst->TMP1()] != NULL)) {
         // Only display the temp if we accumulation is defined.
          summary = Dup_CommandResult (summary_temp[sinst->TMP1()]);
        } else if (sinst->OpCode() == VIEWINST_Display_Average_Tmp) {
          CommandResult *V = summary_temp[sinst->TMP1()];
          if (!V->isNullValue ()) {
            summary = Calculate_Average (V, summary_temp[sinst->TMP2()]);
          }
        } else if (sinst->OpCode() == VIEWINST_Display_StdDeviation_Tmp) {
          CommandResult *V1 = summary_temp[sinst->TMP1()];
          CommandResult *V2 = summary_temp[sinst->TMP2()];
          CommandResult *V3 = summary_temp[sinst->TMP3()];
          summary = Calculate_StdDev (V1, V2, V3);
        } else if (sinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          CommandResult *V = summary_temp[sinst->TMP1()];
          summary = Calculate_Percent (V, Total_Value[sinst->TMP2()]);
        } else if (sinst->OpCode() == VIEWINST_Display_Percent_Column) {
          if (Gen_Total_Percent &&
              (i > percentofcolumn) &&
              (percent_of != NULL) &&
              (!percent_of->isNullValue ())) {
            summary = Calculate_Percent (percent_of, Total_Value[sinst->TMP2()]);
          }
        }
        if (summary == NULL) {
          summary = CRPTR ("");
        }
        if (Gen_Total_Percent &&
            (i == percentofcolumn)) {
          percent_of = summary;
        }

        E->CommandResult_Enders::Add_Ender (summary);
      }
     // Add ID
      E->CommandResult_Enders::Add_Ender ( CRPTR ( "Report Summary" ) );
      view_output.push_back (E);  // attach summary list to output
    }

 // Release summary temporaries
  if (report_Column_summary) {
    for ( i=0; i < num_input_temps; i++) {
      if (summary_temp[i] != NULL) {
        delete summary_temp[i];
      }
    }
  }

 // Release Total temporaries
  for ( i=0; i < Total_Value.size(); i++) {
    if (Total_Value[i] != NULL) {
      delete Total_Value[i];
    }
  }

}
