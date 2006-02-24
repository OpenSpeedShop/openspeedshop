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

template <class T>
struct sort_ascending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return CommandResult_lt ((*x.second)[0], (*y.second)[0]);
    }
};
template <class T>
struct sort_descending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return CommandResult_gt ((*x.second)[0], (*y.second)[0]);
    }
};

static void Dump_items (std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
    std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
cerr << "\nDump items.  Number of items is " << c_items.size() << "\n";
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    int64_t i;
    for (i = 0; i < (*cp.second).size(); i++ ) {
      cerr << "  ";
      CommandResult *p = (*cp.second)[i];
      if (p != NULL) {
        p->Print(cerr); cerr << "\n";
      } else {
        cerr << "NULL\n";
      }
    }

  }

}

static void Setup_Sort( 
       int64_t temp_index,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  if (temp_index == VMulti_sort_temp) return;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[VMulti_sort_temp];
    if (Old != NULL) delete Old;
    CommandResult *V1 = (*cp.second)[temp_index];
    CommandResult *New = Dup_CommandResult (V1);
    Assert (New != NULL);
    (*cp.second)[VMulti_sort_temp] = New;
  }
}

static void Setup_Sort( 
       ViewInstruction *vinst,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  if ((vinst->OpCode() == VIEWINST_Display_Metric) ||
      (vinst->OpCode() == VIEWINST_Display_Percent_Metric) ||
      (vinst->OpCode() == VIEWINST_Display_Percent_Column)) {
    return;
  }
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, set the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[VMulti_sort_temp];
    if (Old != NULL) delete Old;
    CommandResult *New = NULL;
    CommandResult *V1 = (*cp.second)[vinst->TMP1()];
    if (vinst->OpCode() == VIEWINST_Display_Tmp) {
      New = Dup_CommandResult (V1);
    } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
     // Use value without calculating percent - order will be the same.
      New = Dup_CommandResult (V1);
    } else if (vinst->OpCode() == VIEWINST_Display_Average_Tmp) {
      if (!V1->isNullValue ()) {
        New = Calculate_Average (V1, (*cp.second)[vinst->TMP2()]);
      }
    } else if (vinst->OpCode() ==VIEWINST_Display_StdDeviation_Tmp) {
      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];
      CommandResult *V3 = (*cp.second)[vinst->TMP3()];
      New = Calculate_StdDev (V1, V2, V3);
    }
    Assert (New != NULL);
    (*cp.second)[VMulti_sort_temp] = New;
  }
}

static void Dump_Intermediate_CallStack (ostream &tostream,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, dump the corresponding value and the last call stack function name.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    tostream << "    ";
    for (int64_t i = 0; i < (*cp.second).size(); i++) {
      (*cp.second)[i]->Print(tostream); tostream << "  ";
    }
    ((CommandResult *)(cp.first))->Print(tostream); tostream << std::endl;
  }
}

static bool Calculate_Total_For_Percent (
              CommandObject *cmd,
              ThreadGroup& tgrp,
              std::vector<Collector>& CV,
              std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV,
              int64_t &percentofcolumn,
              CommandResult *&TotalValue,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
   // Calculate %?
    ViewInstruction *totalInst = Find_Total_Def (IV);
    int64_t totalIndex = 0;
    TotalValue = NULL;
    percentofcolumn = -1;
    bool Gen_Total_Percent = true;

    if (totalInst == NULL) {
      ViewInstruction *vinst = Find_Percent_Def (IV);
      if ((vinst != NULL) &&
          (vinst->OpCode() == VIEWINST_Display_Percent_Tmp)) {
       // Sum the specified temp.
        int64_t use_temp = vinst->TMP1();
        std::vector<std::pair<CommandResult *,
                              SmartPtr<std::vector<CommandResult *> > > >::iterator ci;
        ci = c_items.begin();
        if (use_temp >= (*ci++).second->size()) {
         // Clearly, this is an error.
          Gen_Total_Percent = false;
        } else {
          if (ci != c_items.end()) {
            TotalValue = Dup_CommandResult( (*(*ci++).second)[use_temp] );
          }
          for ( ; ci != c_items.end(); ci++) {
            Accumulate_CommandResult (TotalValue, (*(*ci).second)[use_temp]);
          }
        }
      }
    } else {
      totalIndex = totalInst->TMP1(); // this is a CV/MV index, not a column number!
      ViewInstruction *vinst = Find_Percent_Def (IV);
      if (vinst != NULL) {
        if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
         // This is the column number!  Save to avoid recalculateion.
          percentofcolumn = vinst->TMP1(); // this is the column number!
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
         // We will recalcualte the value when we generate the %.
        } else {
         // Not yet implemented??
          Gen_Total_Percent = false;
        }
      } else {
       // No % displayed, so why calcualte total?
        Gen_Total_Percent = false;
      }
      if (Gen_Total_Percent) {
       // We calculate Total by adding all the values that were recorded for the thread group.
        TotalValue = Get_Total_Metric ( cmd, tgrp, CV[totalIndex], MV[totalIndex] );
      }
    }
    if (Gen_Total_Percent) {
      if (TotalValue == NULL) {
       // Something went wrong, delete the column of % from the report.
        Gen_Total_Percent = false;
      }
    }
  return Gen_Total_Percent;
}

static SmartPtr<std::vector<CommandResult *> > 
       Dup_Call_Stack (int64_t len,
                       SmartPtr<std::vector<CommandResult *> >& cs) {
  SmartPtr<std::vector<CommandResult *> > call_stack;
  if (len == 0) return call_stack;
  Assert (len <= cs->size());
  call_stack = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  for (int64_t i = 0; i < len; i++) {
    CommandResult *CE = (*cs)[i];
    CommandResult *NCE;
    NCE = Dup_CommandResult (CE);
    call_stack->push_back(NCE);
  }
  return call_stack;
}

static SmartPtr<std::vector<CommandResult *> > 
       Copy_Call_Stack_Entry (int64_t idx,
                              int64_t bias,
                              SmartPtr<std::vector<CommandResult *> >& cs) {
  SmartPtr<std::vector<CommandResult *> > call_stack;
  call_stack = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  if (bias < 0) {
    call_stack->push_back (CRPTR(""));
    call_stack->push_back (Dup_CommandResult ((*cs)[idx-1]) );
  }
  if (bias == 0) {
    call_stack->push_back (Dup_CommandResult ((*cs)[idx]) );
  }
  if (bias > 0) {
    call_stack->push_back (CRPTR(""));
    call_stack->push_back (Dup_CommandResult ((*cs)[idx+1]) );
  }

  return call_stack;
}

static int64_t Match_Call_Stack (SmartPtr<std::vector<CommandResult *> >& cs,
                                 SmartPtr<std::vector<CommandResult *> >& ncs) {
  int64_t csz = cs->size();
  int64_t ncsz = ncs->size();
  int64_t minsz = min(csz, ncsz);
  for (int64_t i = 0; i < minsz; i++) {
    CommandResult *cse = (*cs)[i];
    CommandResult *ncse = (*ncs)[i];
    cmd_result_type_enum ty = cse->Type();
    if (ty != ncse->Type()) return (i - 1);

    if (ty == CMD_RESULT_FUNCTION) {
     // Compare functions and Statements.
      if (*((CommandResult_Function *)cse) != *((CommandResult_Function *)ncse)) return (i - 1);

      std::set<Statement> T;
      std::set<Statement> NT;
      ((CommandResult_Function *)cse)->Value(T);
      ((CommandResult_Function *)ncse)->Value(NT);
      if (T != NT) return (i - 1);
    } else if (ty == CMD_RESULT_LINKEDOBJECT) {
     // Compare LinkedObjects and offsets.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_LinkedObject *)cse)->Value(V);
      ((CommandResult_LinkedObject *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
      if (*((CommandResult_LinkedObject *)cse) != *((CommandResult_LinkedObject *)ncse)) return (i - 1);
    } else if (ty == CMD_RESULT_UINT) {
     // Compare absolute addresses.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_Uint *)cse)->Value(V);
      ((CommandResult_Uint *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
    } else if (ty == CMD_RESULT_STRING) {
     // Compare caracters.
      std::string V;
      std::string NV;
      ((CommandResult_String *)cse)->Value(V);
      ((CommandResult_String *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
    } else {
      return (i - 1);
    }

  }
  return minsz;
}

static int64_t Match_Short_Stack (SmartPtr<std::vector<CommandResult *> >& cs,
                                  SmartPtr<std::vector<CommandResult *> >& ncs) {
  int64_t csz = cs->size();
  int64_t ncsz = ncs->size();
  if ((csz <= 0) || (ncsz <= 0)) return -1;
  int64_t minsz = min(csz, ncsz);
  int64_t i = 0;
  // for (int64_t i = 0; i < minsz; i++) {
    CommandResult *cse = (*cs)[csz-1];
    CommandResult *ncse = (*ncs)[ncsz-1];
    cmd_result_type_enum ty = cse->Type();
    if (ty != ncse->Type()) return (i - 1);

    if (ty == CMD_RESULT_FUNCTION) {
     // Compare functions only.
      if (*((CommandResult_Function *)cse) != *((CommandResult_Function *)ncse)) return (i - 1);
    } else if (ty == CMD_RESULT_LINKEDOBJECT) {
     // Compare LinkedObjects and offsets.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_LinkedObject *)cse)->Value(V);
      ((CommandResult_LinkedObject *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
      if (*((CommandResult_LinkedObject *)cse) != *((CommandResult_LinkedObject *)ncse)) return (i - 1);
    } else if (ty == CMD_RESULT_UINT) {
     // Compare absolute addresses.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_Uint *)cse)->Value(V);
      ((CommandResult_Uint *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
    } else {
      return (i - 1);
    }

  // }
  return minsz;
}

static inline void Accumulate_PreDefined_Temps (std::vector<ViewInstruction *>& IV,
                                                std::vector<CommandResult *>& A,
                                                std::vector<CommandResult *>& B) {
  int64_t len = A.size();
  for (int64_t i = 0; i < len; i++) {
    ViewInstruction *vp = IV[i];
    if (vp != NULL) {
      ViewOpCode Vop = vp->OpCode();
      if (Vop == VIEWINST_Add) {
        Accumulate_CommandResult (A[i], B[i]);
      } else if (Vop == VIEWINST_Min) {
        Accumulate_Min_CommandResult (A[i], B[i]);
      } else if (Vop == VIEWINST_Max) {
        Accumulate_Max_CommandResult (A[i], B[i]);
      }
    }
  }
}

static void Combine_Duplicate_CallStacks (
              std::vector<ViewInstruction *>& IV,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = ((CommandResult_CallStackEntry *)cp.first)->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      SmartPtr<std::vector<CommandResult *> > ncs = ((CommandResult_CallStackEntry *)ncp.first)->Value();
      if (cs_size > ncs->size()) {
        break;
      }
      if (cs_size != ncs->size()) {
       // We can do this because the original call stacks are expanded in place.
        nvpi++;
        continue;
      }
      int64_t matchcount = Match_Call_Stack (cs, ncs);
      if ((matchcount >= 0) &&
          (matchcount == cs->size()) &&
          (matchcount == ncs->size())) {
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps (IV, (*cp.second), (*ncp.second));
        nvpi = c_items.erase(nvpi);
        delete ncp.first;
        if ((*ncp.second).begin() != (*ncp.second).end()) {
          for (int64_t i = 0; i < (*ncp.second).size(); i++) {
            delete (*ncp.second)[i];
          }
        }
        continue;
      }
     // Match failed.
      if (cs_size == ncs->size()) {
        break;
      }
      nvpi++;
    }

  }
}

static void Combine_Short_Stacks (
              std::vector<ViewInstruction *>& IV,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = ((CommandResult_CallStackEntry *)cp.first)->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      SmartPtr<std::vector<CommandResult *> > ncs = ((CommandResult_CallStackEntry *)ncp.first)->Value();
      if (cs_size > ncs->size()) {
        break;
      }
      if (cs_size != ncs->size()) {
       // We can do this because the original call stacks are expanded in place.
        nvpi++;
        continue;
      }
      int64_t matchcount = Match_Call_Stack (cs, ncs);
      if ((matchcount >= 0) &&
          (matchcount == cs->size()) &&
          (matchcount == ncs->size())) {
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps (IV, (*cp.second), (*ncp.second));
        nvpi = c_items.erase(nvpi);
        delete ncp.first;
        if ((*ncp.second).begin() != (*ncp.second).end()) {
          for (int64_t i = 0; i < (*ncp.second).size(); i++) {
            delete (*ncp.second)[i];
          }
        }
        continue;
      }
     // Match failed.  Keep looking.
      nvpi++;
    }

  }
}

static SmartPtr<std::vector<CommandResult *> >
  Dup_CRVector (SmartPtr<std::vector<CommandResult *> >& crv) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs
              = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
     // Generate initial value for each column.
      for (int64_t j = 0; j < crv->size(); j++) {
        vcs->push_back ( New_CommandResult ((*crv)[j]) );
      }
  return vcs;
}

static SmartPtr<std::vector<CommandResult *> >
  Copy_CRVector (SmartPtr<std::vector<CommandResult *> >& crv) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs
              = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
     // Generate initial value for each column.
      for (int64_t j = 0; j < crv->size(); j++) {
        vcs->push_back ( Dup_CommandResult ((*crv)[j]) );
      }
  return vcs;
}

static void Extract_Pivot_Items (
              CommandObject * cmd,
              ExperimentObject *exp,
              std::vector<ViewInstruction *>& IV,
              bool TraceBack_Order,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items,
              Function& func,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& result) {
  bool pivot_added = false;
  std::pair<CommandResult *,
            SmartPtr<std::vector<CommandResult *> > > pivot;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > pred;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > succ;

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for the matching function name.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = ((CommandResult_CallStackEntry *)cp.first)->Value();

    for (int64_t i = 0; i < cs->size(); i++) {
      CommandResult *cof = (*cs)[i];
      if ((cof->Type() == CMD_RESULT_FUNCTION) &&
          ((*(CommandResult_Function *)cof) == func)) {
       // Insert intermediate, dummy entry to fill a gap in the trace.
        if (!pivot_added) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, 0, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
          pivot = std::make_pair(CSE, vcs);
          pivot_added = true;
        } else {
          Accumulate_PreDefined_Temps (IV, (*pivot.second), (*cp.second));
        }
        if (i != 0) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, -1, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, !TraceBack_Order);
          pred.push_back (std::make_pair(CSE, vcs));
        }
        if ((i+1) < cs->size()) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, +1, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
          succ.push_back (std::make_pair(CSE, vcs));
        }
        break;
      }
    }
  }
  if (pivot_added) {
    if (!pred.empty()) {
      Combine_Short_Stacks (IV, pred);
      result.insert(result.end(), pred.begin(), pred.end());
    }
    result.push_back (pivot);
    if (!succ.empty()) {
      Combine_Short_Stacks (IV, succ);
      result.insert(result.end(), succ.begin(), succ.end());
    }
  }
}

static void Expand_CallStack (
              bool TraceBack_Order,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Rewrite base report by expanding the call stack.
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > result;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = ((CommandResult_CallStackEntry *)cp.first)->Value();

    // for (int64_t i = cs->size()-1; i > 0; i--) 
    for (int64_t i = 1; i < cs->size(); i++) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs = Dup_CRVector (cp.second);
      SmartPtr<std::vector<CommandResult *> > ncs = Dup_Call_Stack (i, cs);
      CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
      result.push_back (std::make_pair(CSE, vcs));
    }
    result.push_back (cp);
  }
  c_items = result;
}

bool Generic_Multi_View (
           CommandObject *cmd, ExperimentObject *exp, int64_t topn,
           ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
           std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
           View_Form_Category vfc,
           std::vector<std::pair<CommandResult *,
                                 SmartPtr<std::vector<CommandResult *> > > >& c_items,
           std::list<CommandResult *>& view_output) {
  bool success = false;
  // Print_View_Params (cerr, CV,MV,IV);
  Assert (vfc != VFC_Unknown);

  if (Look_For_KeyWord(cmd, "ButterFly") &&
      !Filter_Uses_F (cmd)) {
    Mark_Cmd_With_Soft_Error(cmd, "(The required function list ('-f ...') for ButterFly views is missing.)");
    return false;   // There is no collector, return.
  }

  CommandResult *TotalValue = NULL;
  bool Gen_Total_Percent = false;
  int64_t percentofcolumn = -1;
  int64_t i;
  if (topn == 0) topn = LONG_MAX;

  try {
   // Set up quick access to instructions for columns.
    int64_t num_columns = 0;
    std::vector<ViewInstruction *> ViewInst(IV.size());
    for ( i=0; i < IV.size(); i++) {
      ViewInstruction *vinst = Find_Column_Def (IV, i);
      if (vinst == NULL) {
       // Exit if we didn't find a definition
        break;
      }
      num_columns++;
      ViewInst[i] = vinst;
    }   
    if (num_columns == 0) {
      Mark_Cmd_With_Soft_Error(cmd, "(There is no display requested.)");
      return false;   // There is no column[0] defined, return.
    }

   // Set up quick access to instructions for data combining.
    int64_t num_temps_used = max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
    std::vector<ViewInstruction *> AccumulateInst(num_temps_used);
    ViewInstruction *sortInst = NULL;
    for ( i = 0; i < num_temps_used; i++) AccumulateInst[i] = NULL;
    for ( i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      if ((vp->OpCode() == VIEWINST_Add) ||
          (vp->OpCode() == VIEWINST_Min) ||
          (vp->OpCode() == VIEWINST_Max)) {
        if (vp->TMP1() < num_temps_used) {
          AccumulateInst[vp->TMP1()] = vp;
        }
      } else if (vp->OpCode() == VIEWINST_Sort_Ascending) {
       sortInst = vp;
      }
    }

   // Acquire base set of metric values.
    int64_t Column0index = (ViewInst[0]->OpCode() == VIEWINST_Display_Metric) ? ViewInst[0]->TMP1() : 0;

   // Determine call stack ordering
    bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);

   // What granularity has been requested?
    std::string EO_Title;
    if (vfc == VFC_Trace) {
      EO_Title = "Call Stack Function (defining location)";

      Gen_Total_Percent = Calculate_Total_For_Percent (cmd, tgrp, CV, MV, IV, percentofcolumn, TotalValue, c_items);

      if ((topn < (int64_t)c_items.size()) &&
          !Look_For_KeyWord(cmd, "ButterFly")) {
       // Determine the topn items based on the time spent in each call.
        Setup_Sort (VMulti_time_temp, c_items);
        std::sort(c_items.begin(), c_items.end(),
                  sort_descending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
     // Sort by the value displayed in the left most column.
      Setup_Sort (ViewInst[0], c_items);
      if ((sortInst == NULL) ||
          (sortInst->TMP1() == 0)) {
        std::sort(c_items.begin(), c_items.end(),
                  sort_descending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
      } else {
        std::sort(c_items.begin(), c_items.end(),
                  sort_ascending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
      }

     // Should we expand the call stack entries in the report?
      if (Look_For_KeyWord(cmd, "CallTree") ||
          Look_For_KeyWord(cmd, "CallTrees") ||
          Look_For_KeyWord(cmd, "TraceBack") ||
          Look_For_KeyWord(cmd, "TraceBacks") ||
          Look_For_KeyWord(cmd, "FullStack") ||
          Look_For_KeyWord(cmd, "FullStacks")) {
        if (!Look_For_KeyWord(cmd, "ButterFly")) {
          if (!Look_For_KeyWord(cmd, "DontExpand")) {
            Expand_CallStack (TraceBack_Order, c_items);
          }

/* We could compress the output, but this routine goes too far
   by combining the traces.
         // Should we eliminate redundant entries in the report?
          if (!Look_For_KeyWord(cmd, "FullStack") &&
              !Look_For_KeyWord(cmd, "FullStacks")) {
            Combine_Duplicate_CallStacks (AccumulateInst, c_items);
          }
*/
        }
      }
    } else if (vfc == VFC_CallStack) {
     // Straight Report will break down report by call stack.
      EO_Title = "Call Stack Function (defining location)";
      bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                        Look_For_KeyWord(cmd, "FullStack") ||
                        Look_For_KeyWord(cmd, "FullStacks"));

      Combine_Duplicate_CallStacks (AccumulateInst, c_items);
      Gen_Total_Percent = Calculate_Total_For_Percent (cmd, tgrp, CV, MV, IV, percentofcolumn, TotalValue, c_items);

     // Sort by the value displayed in the left most column.
      Setup_Sort (ViewInst[0], c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if ((topn < (int64_t)c_items.size()) &&
          !Look_For_KeyWord(cmd, "ButterFly")) {
       // Retain the topn items.
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
/* The preferred output seems to be by time spent, not call tree order.
     // Sort report in calling tree order.
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CallStacks<std::pair<CommandResult *,
                                                     SmartPtr<std::vector<CommandResult *> > > >());
*/

     // Should we expand the call stack entries in the report?
      if (!Look_For_KeyWord(cmd, "ButterFly")) {
        if (!Look_For_KeyWord(cmd, "DontExpand")) {
          Expand_CallStack (TraceBack_Order, c_items);
        }

       // Should we eliminate redundant entries in the report?
        if (!Look_For_KeyWord(cmd, "FullStack") &&
            !Look_For_KeyWord(cmd, "FullStacks")) {
          Combine_Duplicate_CallStacks (AccumulateInst, c_items);
        }
      }
    } else if (vfc == VFC_Function) {
     // Default is the summary report by MPI function.
      EO_Title = "Function (defining location)";
      Gen_Total_Percent = Calculate_Total_For_Percent (cmd, tgrp, CV, MV, IV, percentofcolumn, TotalValue, c_items);

     // Sort by the value displayed in the left most column.
      Setup_Sort (ViewInst[0], c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if (topn < (int64_t)c_items.size()) {
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
    } else {
      return false;
    }

    if (c_items.empty()) {
      std::string s("(There are no data samples for " + MV[Column0index] + " available.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no data, return.
    }


   // Add Header for each column in the table.
    CommandResult_Headers *H = new CommandResult_Headers ();
    for ( i=0; i < num_columns; i++) {
      ViewInstruction *vinst = ViewInst[i];
      std::string column_header;
      if ((HV.size() > i) &&
          (HV[i].length() > 0)) {
        column_header = HV[i];
      } else {
        column_header = "?";
      }
      H->CommandResult_Headers::Add_Header ( CRPTR ( column_header ) );
    }
   // Add Entry Object name
    H->CommandResult_Headers::Add_Header ( CRPTR ( EO_Title ) );
    view_output.push_back(H);

   // Convert "0" values to blanks.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator xvpi;
    for (xvpi = c_items.begin(); xvpi != c_items.end(); xvpi++) {
     // Foreach CallStack entry, look for duplicates and missing intermediates.
      std::pair<CommandResult *,
                SmartPtr<std::vector<CommandResult *> > > cp = *xvpi;
      double V;
      ((CommandResult_Float *)(*cp.second)[VMulti_time_temp])->Value(V);
      if (V == 0.0) {
       // Set flag in CommandResult to indicate null value.
       // The display logic may decide to replace the value with
       // blanks, if it is easier to read.
        for (int64_t i = 0; i < (*cp.second).size(); i++) {
          (*cp.second)[i]->setNullValue();
        }
      }
    }

   // Now format the view.
    if (Look_For_KeyWord(cmd, "ButterFly")) {
     // Foreach function name, build a ButterFly view.
     // Note: we have already verified that there is a '-f' list.
      OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
      vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
      Assert (p_tlist->begin() != p_tlist->end());
      OpenSpeedShop::cli::ParseTarget pt = *p_tlist->begin(); // There can only be one!
      vector<OpenSpeedShop::cli::ParseRange> *f_list = pt.getFileList();
      Assert ((f_list != NULL) && !(f_list->empty()));
      bool MoreThanOne = false;
      vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
      for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
        OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
        OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
        Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
        std::set<Function> FS = exp->FW()->getFunctionsByNamePattern (pval1.name);
        std::set<Function>::iterator fsi;
        for (fsi = FS.begin(); fsi != FS.end(); fsi++) {
          std::vector<std::pair<CommandResult *,
                                SmartPtr<std::vector<CommandResult *> > > > result;
          Function func = *fsi;
          Extract_Pivot_Items (cmd, exp, AccumulateInst, TraceBack_Order, c_items, func, result);
          if (!result.empty()) {
            std::list<CommandResult *> view_unit;
            Construct_View_Output (cmd, tgrp, CV, MV, IV,
                                   num_columns,
                                   Gen_Total_Percent, percentofcolumn, TotalValue,
                                   result,
                                   view_unit);
            if (!view_unit.empty()) {
              if (MoreThanOne) {
               // Delimiter between items is a null string.
                view_output.push_back (new CommandResult_RawString(""));
               }
              view_output.splice (view_output.end(), view_unit);
              MoreThanOne = true;
            }
            Reclaim_CR_Space (result);
          }
        }
      }

    } else {
      Construct_View_Output (cmd, tgrp, CV, MV, IV,
                             num_columns,
                             Gen_Total_Percent, percentofcolumn, TotalValue,
                             c_items,
                             view_output);
    }

    success = true;
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
  }

 // Release space for no longer needed items.
  Reclaim_CR_Space (c_items);
  if (TotalValue != NULL) delete TotalValue;

 // Release instructions
  for (i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    delete vp;
    IV[i] = NULL;
  }

  return success;
}
