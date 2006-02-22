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
#include "MPICollector.hxx"
#include "MPIDetail.hxx"
#include "MPITCollector.hxx"
#include "MPITDetail.hxx"

// Indicate where in the predefined-temporay table certain vaules are.
// Note: index '0' is reserved for the sort key and will be filled in
//       by the general utilities when needed.
#define sort_temp 0
#define start_temp 1
#define stop_temp 2
#define time_temp 3
#define min_temp 4
#define max_temp 5
#define cnt_temp 6
#define ssq_temp 7

#define source_temp 8
#define destination_temp 9
#define size_temp 10
#define tag_temp 11
#define communicator_temp 12
#define datatype_temp 13
#define retval_temp 14

enum View_Form_Category {
      VFC_Unknown,
      VFC_Trace,
      VFC_Function,
      VFC_CallStack,
};

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
template <class T>
struct sort_ascending_CallStacks : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        SmartPtr<std::vector<CommandResult *> > xs = ((CommandResult_CallStackEntry *)x.first)->Value();
        SmartPtr<std::vector<CommandResult *> > ys = ((CommandResult_CallStackEntry *)y.first)->Value();
        vector<CommandResult *>::iterator i;
        vector<CommandResult *>::iterator j;
       // Always go for call stack ordering.
        for (i = xs->begin(), j = ys->begin();
             (i != xs->end()) && (j != ys->end()); i++, j++) {
          if ((*i)->Type() != (*j)->Type()) {
            return ((*i)->Type() < (*j)->Type());
          }
          if (CommandResult_lt (*i, *j)) return true;
          if (CommandResult_lt (*j, *i)) return false;
        }
        if (xs->size() < ys->size()) return true;
        if (xs->size() > ys->size()) return false;
       // Ascending order is used for value compares
       // when the call stacks are identical.
        return CommandResult_lt ((*x.second)[0], (*y.second)[0]);
    }
};
template <class T>
struct sort_descending_CallStacks : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        SmartPtr<std::vector<CommandResult *> > xs = ((CommandResult_CallStackEntry *)x.first)->Value();
        SmartPtr<std::vector<CommandResult *> > ys = ((CommandResult_CallStackEntry *)y.first)->Value();
        vector<CommandResult *>::iterator i;
        vector<CommandResult *>::iterator j;
       // Always go for call stack ordering.
        for (i = xs->begin(), j = ys->begin();
             (i != xs->end()) && (j != ys->end()); i++, j++) {
          if ((*i)->Type() != (*j)->Type()) {
            return ((*i)->Type() > (*j)->Type());
          }
          if (CommandResult_lt (*i, *j)) return true;
          if (CommandResult_lt (*j, *i)) return false;
        }
        if (xs->size() < ys->size()) return true;
        if (xs->size() > ys->size()) return false;
       // Descending order is used for value compares
       // when the call stacks are identical.
        return CommandResult_gt ((*x.second)[0], (*y.second)[0]);
    }
};

static bool Determine_TraceBack_Ordering (CommandObject *cmd) {
 // Determine call stack ordering
  if (Look_For_KeyWord(cmd, "CallTree") ||
      Look_For_KeyWord(cmd, "CallTrees")) {
      return false;
  } else if (Look_For_KeyWord(cmd, "TraceBack") ||
             Look_For_KeyWord(cmd, "TraceBacks")) {
    return true;
  }
  return false;
}

static void Dump_items (std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
    std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
cerr << "\nDump items.  Number of items is " << c_items.size() << "\n";
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the sort_temp field.
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
  if (temp_index == sort_temp) return;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[sort_temp];
    if (Old != NULL) delete Old;
    CommandResult *V1 = (*cp.second)[temp_index];
    CommandResult *New = Dup_CommandResult (V1);
    Assert (New != NULL);
    (*cp.second)[sort_temp] = New;
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
   // Foreach CallStack entry, set the desired sort value into the sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[sort_temp];
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
    (*cp.second)[sort_temp] = New;
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

static inline
CommandResult *Build_CallBack_Entry (Framework::StackTrace& st, int64_t i, bool add_stmts) {
    CommandResult *SE = NULL;
    std::pair<bool, Function> fp = st.getFunctionAt(i);
    if (fp.first) {
     // Use Function.
      if (add_stmts) {
        std::set<Statement> ss = st.getStatementsAt(i);
        SE = new CommandResult_Function (fp.second, ss);
      } else {
        SE = new CommandResult_Function (fp.second);
      }
    } else {
     // Use Absolute Address.
      SE = new CommandResult_Uint (st[i].getValue());
    }
    return SE;
}

static SmartPtr<std::vector<CommandResult *> >
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st) {
  SmartPtr<std::vector<CommandResult *> > call_stack
             = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  int64_t len = st.size();
  int64_t i;
  if (len == 0) return call_stack;
  if (TraceBack_Order)
    for ( i = 0;  i < len; i++) {
      call_stack->push_back(Build_CallBack_Entry(st, i, add_stmts));
    }
  else
    for ( i = len-1; i >= 0; i--) {
      call_stack->push_back(Build_CallBack_Entry(st, i, add_stmts));
    }
  return call_stack;
}

static void Determine_Objects (
               CommandObject *cmd,
               ExperimentObject *exp,
               ThreadGroup& tgrp,
               std::set<Function>& objects) {
 // Get the list of desired functions.
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  if (p_tlist->begin() == p_tlist->end()) {
   // There is no <target> list for filtering.
   // Get all the mpi functions for all the threads.
    objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
  } else {
   // There is a list.  Is there a "-f" specifier?
    vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();

    if ((f_list == NULL) || (f_list->empty()) ||
        Look_For_KeyWord(cmd, "ButterFly")) {
     // There is no Function filtering requested or a ButerFly views is requested.
     // Get all the functions in the already selected thread groups.
     // Function filtering will be done later for ButerFly views.
      for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

       // Check for asnychonous abort command
        if (cmd->Status() == CMD_ABORTED) {
          return;
        }

        Thread thread = *ti;
        std::set<Function> threadObjects;
        OpenSpeedShop::Queries::GetSourceObjects(thread, threadObjects);
        objects.insert(threadObjects.begin(), threadObjects.end());
      }
    } else {
     // There is some sort of file filter specified.
     // Determine the names of desired functions and get Function objects for them.
     // Thread filtering will be done in GetMetricInThreadGroup.
        vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
        for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
          OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
          OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
          Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
          std::string F_Name = pval1.name;
          std::set<Function> new_objects = exp->FW()->getFunctionsByNamePattern (F_Name);
          if (!new_objects.empty()) {
            objects.insert (new_objects.begin(), new_objects.end());
          }
        }
    }
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

static bool Generic_mpi_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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
    int64_t num_temps_used = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
    std::vector<ViewInstruction *> AccumulateInst(num_temps_used);
    for ( i = 0; i < num_temps_used; i++) AccumulateInst[i] = NULL;
    for ( i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      if ((vp->OpCode() == VIEWINST_Add) ||
          (vp->OpCode() == VIEWINST_Min) ||
          (vp->OpCode() == VIEWINST_Max)) {
        if (vp->TMP1() < num_temps_used) {
          AccumulateInst[vp->TMP1()] = vp;
        }
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
        Setup_Sort (time_temp, c_items);
        std::sort(c_items.begin(), c_items.end(),
                  sort_descending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
     // Sort report in the order that each trace was generated.
      Setup_Sort (start_temp, c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_ascending_CommandResult<std::pair<CommandResult *,
                                                       SmartPtr<std::vector<CommandResult *> > > >());

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
      ((CommandResult_Float *)(*cp.second)[time_temp])->Value(V);
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

// utility

static View_Form_Category Determine_Form_Category (CommandObject *cmd) {
  if (Look_For_KeyWord(cmd, "Trace")) {
    return VFC_Trace;
  } else if (Look_For_KeyWord(cmd, "Statement") ||
             Look_For_KeyWord(cmd, "Statements") ||
             Look_For_KeyWord(cmd, "CallTree") ||
             Look_For_KeyWord(cmd, "CallTrees") ||
             Look_For_KeyWord(cmd, "TraceBack") ||
             Look_For_KeyWord(cmd, "TraceBacks") ||
             Look_For_KeyWord(cmd, "FullStack") ||
             Look_For_KeyWord(cmd, "FullStacks") ||
             Look_For_KeyWord(cmd, "ButterFly")) {
    return VFC_CallStack;
  }
  return VFC_Function;
}

// mpi view

#define def_MPI_values \
            Time start = Time::TheEnd(); \
            Time end = Time::TheBeginning(); \
            int64_t cnt = 0; \
            double sum = 0.0; \
            double vmax = 0.0; \
            double vmin = LONG_MAX; \
            double sum_squares = 0.0;

#define get_MPI_values(vi) \
              double v = (*vi).dm_time; \
              start = min(start,(*vi).dm_interval.getBegin()); \
              end = max(end,(*vi).dm_interval.getEnd()); \
              cnt ++; \
              vmin = min(vmin,v); \
              vmax = max(vmax,v); \
              sum += v; \
              sum_squares += v * v;

#define set_MPI_values  \
              if (num_temps > sort_temp) (*vcs)[sort_temp] = NULL; \
              if (num_temps > start_temp) (*vcs)[start_temp] = CRPTR (start); \
              if (num_temps > stop_temp) (*vcs)[stop_temp] = CRPTR (end); \
              if (num_temps > time_temp) (*vcs)[time_temp] = CRPTR (vmin); \
              if (num_temps > min_temp) (*vcs)[min_temp] = CRPTR (vmax); \
              if (num_temps > max_temp) (*vcs)[max_temp] = CRPTR (sum); \
              if (num_temps > cnt_temp) (*vcs)[cnt_temp] = CRPTR (cnt); \
              if (num_temps > ssq_temp) (*vcs)[ssq_temp] = CRPTR (sum_squares);

static bool MPI_Trace_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

 // Acquire base set of metric values.
  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
          std::map<Framework::StackTrace, std::vector<MPIDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            CommandResult *base_CSE = NULL;
            Framework::StackTrace st = (*si).first;
            std::vector<MPIDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
             // Use macro to alocate temporaries
              def_MPI_values
             // Use macro to assign to temporaries
              get_MPI_values(vi)

             // Use macro to assign temporaries to the result array
              SmartPtr<std::vector<CommandResult *> > vcs
                       = Framework::SmartPtr<std::vector<CommandResult *> >(
                                   new std::vector<CommandResult *>(num_temps)
                                   );
              set_MPI_values

              CommandResult *CSE;
              if (base_CSE == NULL) {
                SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, false, st);
                base_CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
                CSE = base_CSE;
              } else {
                CSE = Dup_CommandResult (base_CSE);
              }
              c_items.push_back(std::make_pair(CSE, vcs));
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
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Trace, c_items, view_output);
}

static bool MPI_Function_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Combine all the items for each function.
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
         // Use macro to allocate imtermediate temporaries
          def_MPI_values
          std::map<Framework::StackTrace, std::vector<MPIDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            std::vector<MPIDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
             // Use macro to accumulate all the separate samples
              get_MPI_values(vi)
            }
          }

         // Use macro to construct result array
          SmartPtr<std::vector<CommandResult *> > vcs
                   = Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>(num_temps)
                               );
          set_MPI_values

         // Construct callstack for last entry in the stack trace.
          Function F = (*fi).first;
          std::map<Framework::StackTrace,
                   std::vector<MPIDetail> >::iterator first_si = 
                                      (*fi).second.begin();
          Framework::StackTrace st = (*first_si).first;
          std::set<Statement> T = st.getStatementsAt(st.size()-1);

          SmartPtr<std::vector<CommandResult *> > call_stack =
                   Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>()
                               );
          call_stack->push_back(new CommandResult_Function (F, T));
          CommandResult *CSE = new CommandResult_CallStackEntry (call_stack);
          c_items.push_back(std::make_pair(CSE, vcs));
        }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Function, c_items, view_output);
}

static bool MPI_CallStack_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPIDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Construct complete call stack
        std::map<Function,
                 std::map<Framework::StackTrace,
                          std::vector<MPIDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
         // Foreach MPI function ...
          std::map<Framework::StackTrace,
                   std::vector<MPIDetail> >::iterator sti;
          for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
           // Use macro to allocate temporary array
            def_MPI_values
            int64_t len = (*sti).second.size();
            for (int64_t i = 0; i < len; i++) {
             // Use macro to combine all the values.
              get_MPI_values(&(*sti).second[i])
            }

           // Use macro to set values into return structure.
            SmartPtr<std::vector<CommandResult *> > vcs
                     = Framework::SmartPtr<std::vector<CommandResult *> >(
                                 new std::vector<CommandResult *>(num_temps)
                                 );
            set_MPI_values

           // Foreach call stack ...
            Framework::StackTrace st = (*sti).first;

           // Construct result entry
            SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st);
            CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
            c_items.push_back(std::make_pair(CSE, vcs));
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
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack,c_items, view_output);
}

static std::string allowed_mpi_V_options[] = {
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Trace",
  "ButterFly",
  "CallTree",
  "CallTrees",
  "TraceBack",
  "TraceBacks",
  "FullStack",
  "FullStacks",
  "DontExpand",
  "Summary",
  ""
};

static void define_mpi_columns (
            CommandObject *cmd,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Total time is always placed in first column.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, stop_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, cnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, ssq_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_Summary = (Look_For_KeyWord(cmd, "Summary") & !Look_For_KeyWord(cmd, "ButterFly"));

  if (Generate_Summary) {
   // Total time is always displayed - also add display of the summary time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    bool time_metric_selected = false;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
        if (!time_metric_selected &&
            !strcasecmp(M_Name.c_str(), "exclusive_times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
          HV.push_back("Exclusive Time");
          last_column++;
        } else if (!time_metric_selected &&
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
          HV.push_back("Inclusive Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "min")) {
         // display min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, min_temp));
          HV.push_back("Min Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // display max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, max_temp));
          HV.push_back("Max Time");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, cnt_temp));
          HV.push_back("Number of Calls");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column, time_temp, cnt_temp));
          HV.push_back("Average Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, time_temp));
          HV.push_back("% of Total");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum, sum of squares and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column,
                                            time_temp, ssq_temp, cnt_temp));
          HV.push_back("Standard Deviation");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "start_time")) {
          if (vfc == VFC_Trace) {
           // display start time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
            HV.push_back("Start Time");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "stop_time")) {
          if (vfc == VFC_Trace) {
           // display stop time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
            HV.push_back("Stop Time");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m stop_time' only supported for '-v Trace' option.");
          }
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
    }
  } else {
   // If nothing is requested ...
    if (vfc == VFC_Trace) {
      // Insert start and end times into report.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
      HV.push_back("Start Time");
      last_column++;
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
      HV.push_back("Stop Time");
      last_column++;
    }
   // Always display elapsed time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
    HV.push_back("Exclusive Time");
  }
}

static bool mpi_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
    Assert (CV.begin() != CV.end());
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
   // Clean the other vectors
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());
    HV.erase(HV.begin(), HV.end());

    CollectorGroup cgrp = exp->FW()->getCollectors();
    Collector C = *CV.begin();
    if (cgrp.find(C) == std::set<Collector>::iterator(cgrp.end())) {
      std::string C_Name = C.getMetadata().getUniqueId();
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
/* TEST
    std::string M_Name("exclusive_times");
TEST */
    std::string M_Name("exclusive_details");
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_mpi_V_options);
    define_mpi_columns (cmd, IV, HV, vfc);

    return true;
}


static std::string VIEW_mpi_brief = "Mpi Report";
static std::string VIEW_mpi_short = "Report the time spent in each mpi function.";
static std::string VIEW_mpi_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'mpi' to indicate the maximum number of items in the report."
                                      " When the '-v Trace' option is selected, the selected items are"
                                      " the ones that use the most time.  In all other cases"
                                      " the selection will be based on the values displayed in"
                                      " left most column of the report."
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option.  Except for the '-v Trace' option, the report will"
                                      " be sorted in descending order of the value in the left most column"
                                      " displayed on a line. [See '-m' option for controlling this field.]"
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option."
                                      "\n\t'-v Functions' will produce a summary report that"
                                      " will be sorted in descending order of the value in the left most"
                                      " column (see the '-m' option).  This is the default display."
                                      "\n\t'-v Trace' will produce a report of each individual  call to an mpi"
                                      " function."
                                      " It will be sorted in ascending order of the starting time for the event."
                                      " The information available for display from an 'mpi' experiment is very"
                                      " limited when compared to what is available from an 'mpit' experiment."
                                      "\n\t'-v CallTrees' will produce a calling stack report that is presented"
                                      " in calling tree order - from the start of the program to the measured"
                                      " program."
                                      "\n\t'-v TraceBacks' will produce a calling stack report that is presented"
                                      " in traceback order - from the measured function to the start of the"
                                      " program."
                                      "\n\tThe addition of 'FullStack' with either 'CallTrees' of 'TraceBacks'"
                                      " will cause the report to include the full call stack for each measured"
                                      " function.  Redundant portions of a call stack are suppressed by default."
                                      "\n\tThe addition of 'Summary' to the '-v' option list along with 'Functions',"
                                      " 'CallTrees' or 'TraceBacks' will result in an additional line of output at"
                                      " the end of the report that summarizes the information in each column."
                                      "\n\t'-v ButterFly' along with a '-f <function_list>' will produce a report"
                                      " that summarizes the calls to a function and the calls from the function."
                                      " The calling functions will be listed before the named function and the"
                                      " called functions afterwards, by default, although the addition of"
                                      " 'TraceBacks' to the '-v' specifier will reverse this ordering."
                                      "\n\nThe information included in the report can be controlled with the"
                                      " '-m' option.  More than one item can be selected but only the items"
                                      " listed after the option will be printed and they will be printed in"
                                      " the order that they are listed."
                                      " If no '-m' option is specified, the default is equivalent to"
                                      " '-m exclusive times'."
                                      " The full set of available options is: 'exclusive_times',"
                                      " 'min', 'max', 'average', 'count', 'percent', and 'stddev'."
                                      " Each option reports information about the set of mpi calls that is"
                                      " reported for the function on that particular line in the report."
                                      " \n\t'-m exclusive_times' reports the wall clock time used in the function."
                                      " \n\t'-m min' reports the minimum time spent in the function."
                                      " \n\t'-m max' reports the maximum time spent in the function."
                                      " \n\t'-m average' reports the average time spent in the function."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of mpi time the function represents."
                                      " \n\t'-m stddev' reports the standard deviation of the average mpi time"
                                      " that the function represents.";
static std::string VIEW_mpi_example = "\texpView mpi\n"
                                      "\texpView -v CallTrees,FullStack mpi10 -m min,max,count\n";
static std::string VIEW_mpi_metrics[] =
  { "exclusive_details",
    "exclusive_times",
    "inclusive_details",
    "inclusive_times",
    ""
  };
static std::string VIEW_mpi_collectors[] =
  { "mpi",
    ""
  };
class mpi_view : public ViewType {

 public: 
  mpi_view() : ViewType ("mpi",
                         VIEW_mpi_brief,
                         VIEW_mpi_short,
                         VIEW_mpi_long,
                         VIEW_mpi_example,
                        &VIEW_mpi_metrics[0],
                        &VIEW_mpi_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "mpi"));  // Define the collector
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (mpi_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      switch (vfc) {
       case VFC_Trace:
        return MPI_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_CallStack:
        return MPI_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_Function:
        return MPI_Function_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
      }
    }
    return false;
  }
};

// mpit view

#define def_MPIT_values \
            Time start = Time::TheEnd(); \
            Time end = Time::TheBeginning(); \
            int64_t cnt = 0; \
            double sum = 0.0; \
            double vmax = 0.0; \
            double vmin = LONG_MAX; \
            double sum_squares = 0.0; \
            int64_t detail_source = 0; \
            int64_t detail_destination = 0; \
            uint64_t detail_size = 0; \
            int64_t detail_tag = 0; \
            int64_t detail_communicator = 0; \
            int64_t detail_datatype = 0; \
            int64_t detail_retval = 0;

#define get_MPIT_values(vi) \
              double v = (*vi).dm_time; \
              start = min(start,(*vi).dm_interval.getBegin()); \
              end = max(end,(*vi).dm_interval.getEnd()); \
              cnt ++; \
              vmin = min(vmin,v); \
              vmax = max(vmax,v); \
              sum += v; \
              sum_squares += v * v; \
              detail_source = (*vi).dm_source; \
              detail_destination = (*vi).dm_destination; \
              detail_size += (*vi).dm_size; \
              detail_tag = (*vi).dm_tag; \
              detail_communicator = (*vi).dm_communicator; \
              detail_datatype = (*vi).dm_datatype; \
              detail_retval = (*vi).dm_retval;

#define set_MPIT_values  \
              if (num_temps > sort_temp) (*vcs)[sort_temp] = NULL; \
              if (num_temps > start_temp) (*vcs)[start_temp] = CRPTR (start); \
              if (num_temps > stop_temp) (*vcs)[stop_temp] = CRPTR (end); \
              if (num_temps > time_temp) (*vcs)[time_temp] = CRPTR (vmin); \
              if (num_temps > min_temp) (*vcs)[min_temp] = CRPTR (vmax); \
              if (num_temps > max_temp) (*vcs)[max_temp] = CRPTR (sum); \
              if (num_temps > cnt_temp) (*vcs)[cnt_temp] = CRPTR (cnt); \
              if (num_temps > ssq_temp) (*vcs)[ssq_temp] = CRPTR (sum_squares); \
              if (num_temps > source_temp) (*vcs)[source_temp] = CRPTR (detail_source); \
              if (num_temps > destination_temp) (*vcs)[destination_temp] = CRPTR (detail_destination); \
              if (num_temps > size_temp) (*vcs)[size_temp] = CRPTR (detail_size); \
              if (num_temps > tag_temp) (*vcs)[tag_temp] = CRPTR (detail_tag); \
              if (num_temps > communicator_temp) (*vcs)[communicator_temp] = CRPTR (detail_communicator); \
              if (num_temps > datatype_temp) (*vcs)[datatype_temp] = CRPTR (detail_datatype); \
              if (num_temps > retval_temp) (*vcs)[retval_temp] = CRPTR (detail_retval);


static bool MPIT_Trace_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

 // Acquire base set of metric values.
  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPITDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPITDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
          std::map<Framework::StackTrace, std::vector<MPITDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            CommandResult *base_CSE = NULL;
            Framework::StackTrace st = (*si).first;
            std::vector<MPITDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
              def_MPIT_values
              get_MPIT_values(vi)

            // Use macro to copy desired values.  
              SmartPtr<std::vector<CommandResult *> > vcs
                       = Framework::SmartPtr<std::vector<CommandResult *> >(
                                   new std::vector<CommandResult *>(num_temps)
                                   );
              set_MPIT_values 

             // Construct the type-independent return entry.
              CommandResult *CSE;
              if (base_CSE == NULL) {
                SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, false, st);
                base_CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
                CSE = base_CSE;
              } else {
                CSE = Dup_CommandResult (base_CSE);
              }
              c_items.push_back(std::make_pair(CSE, vcs));
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
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Trace, c_items, view_output);
}

static bool MPIT_Function_Report(
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPITDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Combine all the items for each function.
        std::map<Function, std::map<Framework::StackTrace, std::vector<MPITDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
          def_MPIT_values

          std::map<Framework::StackTrace, std::vector<MPITDetail> >:: iterator si;
          for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
            std::vector<MPITDetail>::iterator vi;
            for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
              get_MPIT_values(vi)
            }
          }

         // Use macro to copy desired values.  
          SmartPtr<std::vector<CommandResult *> > vcs
                   = Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>(num_temps)
                               );
          set_MPIT_values 

         // Construct the type-independent return entry.
         // Construct callstack for last entry in the stack trace.
          Function F = (*fi).first;
          std::map<Framework::StackTrace,
                   std::vector<MPITDetail> >::iterator first_si = 
                                      (*fi).second.begin();
          Framework::StackTrace st = (*first_si).first;
          std::set<Statement> T = st.getStatementsAt(st.size()-1);

          SmartPtr<std::vector<CommandResult *> > call_stack =
                   Framework::SmartPtr<std::vector<CommandResult *> >(
                               new std::vector<CommandResult *>()
                               );
          call_stack->push_back(new CommandResult_Function (F, T));
          CommandResult *CSE = new CommandResult_CallStackEntry (call_stack);
          c_items.push_back(std::make_pair(CSE, vcs));
        }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    collector.unlockDatabase();
    return false;
  }

  collector.unlockDatabase();

 // Generate the report.
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_Function, c_items, view_output);
}

static bool MPIT_CallStack_Report (
              CommandObject *cmd, ExperimentObject *exp, int64_t topn,
              ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
              std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
              std::list<CommandResult *>& view_output) {

  int64_t num_temps = max ((int64_t)time_temp, Find_Max_Temp(IV)) + 1;
  bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);
  Collector collector = CV[0];
  std::string metric = MV[0];
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > c_items;
  bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                    Look_For_KeyWord(cmd, "FullStack") ||
                    Look_For_KeyWord(cmd, "FullStacks"));

 // Get the list of desired functions.
  std::set<Function> objects;
  Determine_Objects ( cmd, exp, tgrp, objects);

  if (objects.empty()) {
    return false;
  }

  try {
    collector.lockDatabase();

    SmartPtr<std::map<Function,
                      std::map<Framework::StackTrace,
                               std::vector<MPITDetail> > > > raw_items;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, raw_items);
       // Construct complete call stack
        std::map<Function,
                 std::map<Framework::StackTrace,
                          std::vector<MPITDetail> > >::iterator fi;
        for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
         // Foreach MPIT function ...
          std::map<Framework::StackTrace,
                   std::vector<MPITDetail> >::iterator sti;
          for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
           // Foreach call stack ...
            def_MPIT_values

            int64_t len = (*sti).second.size();
            for (int64_t i = 0; i < len; i++) {
             // Combine all the values.
              get_MPIT_values(&(*sti).second[i])
            }

            SmartPtr<std::vector<CommandResult *> > vcs
                     = Framework::SmartPtr<std::vector<CommandResult *> >(
                                 new std::vector<CommandResult *>(num_temps)
                                 );
           // Use macro to copy desired values.  
            set_MPIT_values 

           // Construct the type-independent return entry.
            Framework::StackTrace st = (*sti).first;
            SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st);
            CommandResult *CSE = new CommandResult_CallStackEntry (call_stack, TraceBack_Order);
            c_items.push_back(std::make_pair(CSE, vcs));
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
  return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV, VFC_CallStack,c_items, view_output);
}

static std::string allowed_mpit_V_options[] = {
  "Function",
  "Functions",
  "Statement",
  "Statements",
  "Trace",
  "ButterFly",
  "CallTree",
  "CallTrees",
  "TraceBack",
  "TraceBacks",
  "FullStack",
  "FullStacks",
  "DontExpand",
  "Summary",
  ""
};

static void define_mpit_columns (
            CommandObject *cmd,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc) {
  int64_t last_column = 0;  // Total time is always placed in first column.

 // Define combination instructions for predefined temporaries.
  IV.push_back(new ViewInstruction (VIEWINST_Add, sort_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, start_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, stop_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, time_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Min, min_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Max, max_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, cnt_temp));
  IV.push_back(new ViewInstruction (VIEWINST_Add, ssq_temp));

 // Most detail fields are not combinable in a meaningful way.
  IV.push_back(new ViewInstruction (VIEWINST_Add, size_temp));

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_Summary = (Look_For_KeyWord(cmd, "Summary") & !Look_For_KeyWord(cmd, "ButterFly"));

  if (Generate_Summary) {
   // Total time is always displayed - also add display of the summary time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Summary));
  }

  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
    bool time_metric_selected = false;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        M_Name = m_range->end_range.name;
      } else {
        M_Name = m_range->start_range.name;
      }

     // Try to match the name with built in values.
      if (M_Name.length() > 0) {
        // Select temp values for columns and build column headers
        if (!time_metric_selected &&
            !strcasecmp(M_Name.c_str(), "exclusive_times") ||
            !strcasecmp(M_Name.c_str(), "exclusive_details")) {
         // display sum of times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
          HV.push_back("Exclusive Time");
          last_column++;
        } else if (!time_metric_selected &&
                   !strcasecmp(M_Name.c_str(), "inclusive_times") ||
                   !strcasecmp(M_Name.c_str(), "inclusive_details")) {
         // display times
          time_metric_selected = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
          HV.push_back("Inclusive Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "min")) {
         // display min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, min_temp));
          HV.push_back("Min Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // display max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, max_temp));
          HV.push_back("Max Time");
          last_column++;
        } else if ( !strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls") ) {
         // display total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, cnt_temp));
          HV.push_back("Number of Calls");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column, time_temp, cnt_temp));
          HV.push_back("Average Time");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // percent is calculate from 2 temps: time for this row and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, time_temp));
          HV.push_back("% of Total");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum, sum of squares and total counts.
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column,
                                            time_temp, ssq_temp, cnt_temp));
          HV.push_back("Standard Deviation");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "start_time")) {
          if (vfc == VFC_Trace) {
           // display start time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
            HV.push_back("Start Time");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m start_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "stop_time")) {
          if (vfc == VFC_Trace) {
           // display stop time
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
            HV.push_back("Stop Time");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m stop_time' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "source")) {
          if (vfc == VFC_Trace) {
           // display source rank
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, source_temp));
            HV.push_back("Source Rank");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m source' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "destination")) {
          if (vfc == VFC_Trace) {
           // display destination rank
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, destination_temp));
            HV.push_back("Destination Rank");
            last_column++;
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m destination' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "size")) {
         // display size of message
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, size_temp));
          HV.push_back("Message Size");
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "tag")) {
          if (vfc == VFC_Trace) {
           // display tag of the message
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, tag_temp));
            HV.push_back("Message Tag");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m tag' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "communicator")) {
          if (vfc == VFC_Trace) {
           // display communicator used
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, communicator_temp));
            HV.push_back("Communicator Used");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m communicator' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "datatype")) {
          if (vfc == VFC_Trace) {
           // display data type of the message
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, datatype_temp));
            HV.push_back("Message Type");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m datatype' only supported for '-v Trace' option.");
          }
        } else if (!strcasecmp(M_Name.c_str(), "retval")) {
          if (vfc == VFC_Trace) {
           // display enumerated return value
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, retval_temp));
            HV.push_back("Return Value");
            last_column++;
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: '-m retval' only supported for '-v Trace' option.");
          }
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
        }
      }
    }
  } else {
   // If nothing is requested ...
    if (vfc == VFC_Trace) {
      // Insert start times, end times, source rank and destination rank into report.
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, start_temp));
      HV.push_back("Start Time");
      last_column++;
      IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, stop_temp));
      HV.push_back("Stop Time");
      last_column++;
    }
   // Always display elapsed time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, time_temp));
    HV.push_back("Exclusive Time");
  }
}

static bool mpit_definition (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV,
                             View_Form_Category vfc) {
    Assert (CV.begin() != CV.end());
    CV.erase(++CV.begin(), CV.end());  // Save the collector name
   // Clean the other vectors
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());
    HV.erase(HV.begin(), HV.end());

    CollectorGroup cgrp = exp->FW()->getCollectors();
    Collector C = *CV.begin();
    if (cgrp.find(C) == std::set<Collector>::iterator(cgrp.end())) {
      std::string C_Name = C.getMetadata().getUniqueId();
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
/* TEST
    std::string M_Name("exclusive_times");
TEST */
    std::string M_Name("exclusive_details");
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    Validate_V_Options (cmd, allowed_mpit_V_options);
    define_mpit_columns (cmd, IV, HV, vfc);

    return true;
}

static std::string VIEW_mpit_brief = "Mpit Report";
static std::string VIEW_mpit_short = "Report information about the calls to mpi functions.";
static std::string VIEW_mpit_long  = "\n\nA positive integer can be added to the end of the keyword"
                                      " 'mpit' to indicate the maximum number of items in the report."
                                      " When the '-v Trace' option is selected, the selected items are"
                                      " the ones that use the most time.  In all other cases"
                                      " the selection will be based on the values displayed in"
                                      " left most column of the report."
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option.  Except for the '-v Trace' option, the report will"
                                      " be sorted in descending order of the value in the left most column"
                                      " displayed on a line. [See '-m' option for controlling this field.]"
                                      "\n\t'-v Functions' will produce a summary report for each function."
                                      " This is the default report, if nothing else is requested."
                                      "\n\t'-v Trace' will produce a report of each call to an mpi function."
                                      " It will be sorted in ascending order of the starting time for the event."
                                      "\n\t'-v CallTrees' will produce report that expands the call stack for"
                                      " a trace, providing the sequence of functions called from the start of"
                                      " the program to the measured function."
                                      "\n\t'-v TraceBacks' will produce a report that expands the call stack for"
                                      " a trace, providing the sequence of functions called from the function"
                                      " back to the start of the program."
                                      "\n\tThe addition of 'FullStack' with either 'CallTrees' of 'TraceBacks'"
                                      " will cause the report to include the full call stack for each measured"
                                      " function.  Redundant portions of a call stack are suppressed if this"
                                      " option is not specified."
                                      "\n\tThe addition of 'Summary' to the '-v' option list will result in an"
                                      " additional line of output at"
                                      " the end of the report that summarizes the information in each column."
                                      "\n\t'-v ButterFly' along with a '-f <function_list>' will produce a report"
                                      " that summarizes the calls to a function and the calls from the function."
                                      " The calling functions will be listed before the named function and the"
                                      " called functions afterwards, by default, although the addition of"
                                      " 'TraceBacks' to the '-v' specifier will reverse this ordering."
                                      "\n\nThe information included in the report can be controlled with the"
                                      " '-m' option.  More than one item can be selected but only the items"
                                      " listed after the option will be printed and they will be printed in"
                                      " the order that they are listed."
                                      " If no '-m' option is specified, the default is equivalent to"
                                      " '-m start_time, end_time, exclusive times'."
                                      " Clearly, not every value will be meaningful with every '-v' option."
                                      " \n\t'-m exclusive_times' reports the wall clock time used in the event."
                                      " \n\t'-m min' reports the minimum time spent in the event."
                                      " \n\t'-m max' reports the maximum time spent in the event."
                                      " \n\t'-m average' reports the average time spent in the event."
                                      " \n\t'-m count' reports the number of times the event occured."
                                      " \n\t'-m percent' reports the percent of mpi time the event represents."
                                      " \n\t'-m stddev' reports the standard deviation of the average mpi time"
                                      " that the event represents."
                                      " \n\t'-m start_time' reports the starting time of the event."
                                      " \n\t'-m stop_time' reports the ending time of the event."
                                      " \n\t'-m source' reports the source rank of the event."
                                      " \n\t'-m destination' reports the destination rank of the event."
                                      " \n\t'-m size' reports the number of bytes in the message."
                                      " \n\t'-m tag' reports the tag of the event."
                                      " \n\t'-m communicator' reports the communicator used for the event."
                                      " \n\t'-m datatype' reports the data type of the message."
                                      " \n\t'-m retval' reports the return value of the event."
;
static std::string VIEW_mpit_example = "\texpView mpit\n"
                                       "\texpView -v Trace mpit10\n" 
                                       "\texpView -v Trace mpit100 -m start_time, inclusive_time, size\n";
static std::string VIEW_mpit_metrics[] =
  { "exclusive_details",
    "exclusive_times",
    "inclusive_details",
    "inclusive_times",
    ""
  };
static std::string VIEW_mpit_collectors[] =
  { "mpit",
    ""
  };
class mpit_view : public ViewType {

 public: 
  mpit_view() : ViewType ("mpit",
                          VIEW_mpit_brief,
                          VIEW_mpit_short,
                          VIEW_mpit_long,
                          VIEW_mpit_example,
                         &VIEW_mpit_metrics[0],
                         &VIEW_mpit_collectors[0],
                          true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::list<CommandResult *>& view_output) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "mpit"));  // Define the collector
    View_Form_Category vfc = Determine_Form_Category(cmd);
    if (mpit_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV, vfc)) {

      if ((CV.size() == 0) ||
          (MV.size() == 0)) {
        Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
        return false;   // There is no collector, return.
      }

      switch (Determine_Form_Category(cmd)) {
       case VFC_Trace:
        return MPIT_Trace_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_CallStack:
        return MPIT_CallStack_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
       case VFC_Function:
        return MPIT_Function_Report (cmd, exp, topn, tgrp, CV, MV, IV, HV, view_output);
      }
    }
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mpi_view_LTX_ViewFactory () {
  Define_New_View (new mpi_view());
  Define_New_View (new mpit_view());
}
