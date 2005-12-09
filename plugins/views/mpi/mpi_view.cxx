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
        return CommandResult_gt ((*x.second)[0], (*y.second)[0]);
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
        SmartPtr<std::vector<CommandResult *> > xs = x.first->Value();
        SmartPtr<std::vector<CommandResult *> > ys = y.first->Value();
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
        SmartPtr<std::vector<CommandResult *> > xs = x.first->Value();
        SmartPtr<std::vector<CommandResult *> > ys = y.first->Value();
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

template <typename TO, typename TS>
void GetMetricInThreadGroup(
    const Collector& collector,
    const std::string& metric,
    const ThreadGroup& tgrp,
    const std::set<TO >& objects,
    SmartPtr<std::map<TO, TS> >& result)
{ 
  ThreadGroup::iterator ti;
  
  // Allocate a new map of functions to type TS
  if (result.isNull()) {
    result = SmartPtr<std::map<TO, TS> >(
      new std::map<TO, TS>()
      );
  }
  Assert(!result.isNull());
  
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread thread = *ti;
    Queries::GetMetricInThread(collector, metric,
                               TimeInterval(Time::TheBeginning(), Time::TheEnd()),
                               thread, objects, result);
  }
}

static void Dump_Intermediate_CallStack (ostream &tostream,
       std::vector<std::pair<CommandResult_CallStackEntry *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, dump the corresponding value and the last call stack function name.
    std::pair<CommandResult_CallStackEntry *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    tostream << "    ";
    for (int64_t i = 0; i < (*cp.second).size(); i++) {
      (*cp.second)[i]->Print(tostream); tostream << "  ";
    }
    ((CommandResult *)(cp.first))->Print(tostream); tostream << std::endl;
  }
}

static
bool Raw_Data (CommandObject *cmd,
               Collector& collector,
               std::string& metric,
               ThreadGroup& tgrp,
               SmartPtr<std::map<Function,
                        std::map<Framework::StackTrace,
                                 std::vector<double> > > >& items) {

   // Pick up the raw data samples for the measured fucntions.
    std::set<Function> objects;
    Get_Filtered_Objects (cmd, tgrp, objects);
    if (objects.empty()) {
      return false;
    }
    GetMetricInThreadGroup (collector, metric, tgrp, objects, items);

    return !(items->empty());
}

static void Summary_Report(
              SmartPtr<std::map<Function,
                                std::map<Framework::StackTrace,
                                         std::vector<double> > > >& f_items,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Combine all the items for each function.
 // Input data is sorted by function.
    std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
    for (fi = f_items->begin(); fi != f_items->end(); fi++) {
      int64_t cnt = 0;
      double sum = 0.0;
      double vmax = 0.0;
      double vmin = LONG_MAX;
      std::map<Framework::StackTrace, std::vector<double> >:: iterator si;
      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
        std::vector<double>::iterator vi;
        for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
          cnt ++;
          vmin = min(vmin,*vi);
          vmax = max(vmax,*vi);
          sum += *vi;
        }
      }
      SmartPtr<std::vector<CommandResult *> > vcs = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
      vcs->push_back ( CRPTR (sum) );
      vcs->push_back ( CRPTR (vmin) );
      vcs->push_back ( CRPTR (vmax) );
      vcs->push_back ( CRPTR (cnt) );
     // Construct callstack for last entry in the stack trace.
      Function F = (*fi).first;
      std::map<Framework::StackTrace,
               std::vector<double> >::iterator first_si = 
                                  (*fi).second.begin();
      Framework::StackTrace st = (*first_si).first;
      std::set<Statement> T = st.getStatementsAt(st.size()-1);

      SmartPtr<std::vector<CommandResult *> > call_stack =
               Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
      call_stack->push_back(new CommandResult_Function (F, T));
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, vcs));
    }
}

static SmartPtr<std::vector<CommandResult *> >
       Construct_CallBack (Framework::StackTrace& st) {
  SmartPtr<std::vector<CommandResult *> > call_stack
             = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  int64_t len = st.size();
  if (len == 0) return call_stack;
  for (int64_t i = len-1; i >= 0; i--) {
    CommandResult *SE = NULL;
    std::pair<bool, Function> fp = st.getFunctionAt(i);
    if (fp.first) {
     // Use Function.
      std::set<Statement> ss = st.getStatementsAt(i);
      SE = new CommandResult_Function (fp.second, ss);
    } else {
     // Use Absolute Address.
      SE = new CommandResult_Uint (st[i].getValue());
    }
    call_stack->push_back(SE);
  }
  return call_stack;
}

static void CallStack_Report (
              SmartPtr<std::map<Function,
                                std::map<Framework::StackTrace,
                                         std::vector<double> > > >& f_items,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Sum the data items for each call stack.
  std::map<Function,
           std::map<Framework::StackTrace,
                    std::vector<double> > >::iterator fi;
  for (fi = f_items->begin(); fi != f_items->end(); fi++) {
   // Foreach MPI function ...
    std::map<Framework::StackTrace,
             std::vector<double> >::iterator sti;
    for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
     // Foreach call stack ...
      Framework::StackTrace st = (*sti).first;
      int64_t cnt = 0;
      double sum = 0.0;
      double vmax = 0.0;
      double vmin = LONG_MAX;
      int64_t len = (*sti).second.size();
      for (int64_t i = 0; i < len; i++) {
       // Combine all the values.
        double v = (*sti).second[i];
        cnt ++;
        vmin = min(vmin,v);
        vmax = max(vmax,v);
        sum += v;
      }
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
      vcs->push_back ( CRPTR (sum) );
      vcs->push_back ( CRPTR (vmin) );
      vcs->push_back ( CRPTR (vmax) );
      vcs->push_back ( CRPTR (cnt) );
      SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (st);
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, vcs));
    }
  }
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
    cmd_result_type_enum ty = CE->Type();
    NCE = Dup_CommandResult (CE);
    call_stack->push_back(NCE);
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
    } else {
      return (i - 1);
    }

  }
  return minsz;
}

static void Combine_Duplicate_CallStacks (
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult_CallStackEntry *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult_CallStackEntry *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      SmartPtr<std::vector<CommandResult *> > ncs = ncp.first->Value();
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
       // Call stacks are identical - combine entries.
        if ((*ncp.second).begin() != (*ncp.second).end()) {
          if ((*cp.second).begin() != (*cp.second).end()) {
            for (int64_t i = 0; i < (*cp.second).size(); i++) {
              Accumulate_CommandResult ((*cp.second)[i], (*ncp.second)[i]);
            }
          } else {
            cp.second = ncp.second;
          }
        }
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

static void Expand_Straight (
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
// Process base report.
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    std::pair<CommandResult_CallStackEntry *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();
    // cp.first->Value(cs);

    nvpi = vpi - 1;
    int64_t xcnt = 0;
    int64_t lcnt = 0;
    if (vpi == c_items.begin()) {
     // Fully expand current call tree.
      lcnt = cs->size();
    } else {
      std::pair<CommandResult_CallStackEntry *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      SmartPtr<std::vector<CommandResult *> > ncs = ncp.first->Value();
      // ncp.first->Value(ncs);
      xcnt = Match_Call_Stack (cs, ncs);
      if ((xcnt > 0) &&
          (xcnt > cs->size()) &&
          (xcnt < ncs->size())) {
       // Partial match - Expand current tree from matchpoint to end.
        lcnt = cs->size();
      } else {
       // No overlap - Fully expand current call tree.
        xcnt = 0;
        lcnt = cs->size();
      }
    }
    for (int64_t i = (xcnt + 1); i < lcnt; i++) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs
              = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
     // Generte initial value for each column.
      for (int64_t j = 0; j < cp.second->size(); j++) {
        CommandResult *old = (*cp.second)[j];
        CommandResult *v = NULL;
        switch (old->Type()) {
         case CMD_RESULT_UINT:
          v = new CommandResult_Uint ();
          break;
         case CMD_RESULT_INT:
          v = new CommandResult_Int ();
          break;
         case CMD_RESULT_FLOAT:
          v = new CommandResult_Float ();
          break;
         default:
          v = new CommandResult_String ("");
          break;
        }
        vcs->push_back ( v );
      }
      SmartPtr<std::vector<CommandResult *> > ncs = Dup_Call_Stack (i, cs);
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs);
      nvpi = c_items.insert(vpi, std::make_pair(CSE, vcs));
      vpi = nvpi + 1;
    }
  }
}

static
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
                     std::vector<std::pair<CommandResult_CallStackEntry *,
                                           SmartPtr<std::vector<CommandResult *> > > >& items) {
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

   // Extract the top "n" items from the sorted list.
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator it;
    for(it = items.begin(); it != items.end(); it++ ) {
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
          Next_Metric_Value = (*it->second)[i];
        } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
          Next_Metric_Value = (*it->second)[CM_Index];
        } else if (vinst->OpCode() == VIEWINST_Display_Average_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if (!V->isNullValue ()) {
            Next_Metric_Value = Calculate_Average (V, (*it->second)[vinst->TMP2()]);
          }
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
            // CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
            //                                                    CV[percentIndex], MV[percentIndex] );
            // Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if (!V->isNullValue ()) {
            Next_Metric_Value = Calculate_Percent (V, TotalValue);
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
          if (Gen_Total_Percent) {
          // CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
          //                                                      CV[CM_Index], MV[CM_Index] );
          // Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
          }
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
      C->CommandResult_Columns::Add_Column (it->first);
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

static bool Generic_mpi_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                       ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                       std::vector<ViewInstruction *> IV, std::vector<std::string> HV) {
  // Print_View_Params (cerr, CV,MV,IV);

  bool report_Column_summary = false;

  if (topn == 0) topn = LONG_MAX;

  try {
    if (CV.size() == 0) {
      std::string s("(There are no metrics specified to report.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no collector, return.
    }

   // Set up quick access to instructions for columns.
    int64_t num_columns = 0;
    std::vector<ViewInstruction *> ViewInst(IV.size());
    int64_t i;
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
      std::string s("(There is no display requested.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no column[0] defined, return.
    }
    if ((ViewInst[0]->OpCode() != VIEWINST_Display_Metric)  &&
        (ViewInst[0]->OpCode() != VIEWINST_Display_Tmp)) {
      std::string s("(The first column is not a metric.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is nothing to sort on.
    }

   // Acquire base set of metric values.
    ViewInstruction *vinst0 = ViewInst[0];
    int64_t Column0index = vinst0->TMP1();
    SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<double> > > > f_items;
    bool first_column_found = false;
    first_column_found = Raw_Data (cmd, CV[Column0index], MV[Column0index], tgrp, f_items);
    if (!first_column_found) {
      std::string s("(There are no data samples for " + MV[Column0index] + " available.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no data, return.
    }

   // Calculate %?
    CommandResult *TotalValue = NULL;
    ViewInstruction *totalInst = Find_Total_Def (IV);
    int64_t totalIndex = 0;
    int64_t percentofcolumn = -1;
    bool Gen_Total_Percent = true;

    if (totalInst == NULL) {
      ViewInstruction *vinst = Find_Percent_Def (IV);
      if ((vinst != NULL) &&
          (vinst->OpCode() == VIEWINST_Display_Percent_Tmp)) {
       // 
        if (vinst->TMP1() >= num_columns) {
         // Clearly, this is an error.
          Gen_Total_Percent = false;
        } else {
         // Sum the time temp.
          double sum = 0.0;
          std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
          for (fi = f_items->begin(); fi != f_items->end(); fi++) {
            std::map<Framework::StackTrace, std::vector<double> >:: iterator si;
            for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
              std::vector<double>::iterator vi;
              for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
                sum += *vi;
              }
            }
          }
          if (sum > 0.0) {
            TotalValue = new CommandResult_Float(sum);
          } else {
            Gen_Total_Percent = false;
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
         // Note yet implemented??
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


   // What granularity has been requested?
    View_Granularity vg = VIEW_FUNCTIONS;
    std::string EO_Title;
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > > c_items;
    if (Look_For_KeyWord(cmd, "Statements")) {
     // Straight Report will break down report by call stack.
      vg = VIEW_STATEMENTS;
      EO_Title = "Call Stack Function (defining location)";
      CallStack_Report (f_items, c_items);
      Combine_Duplicate_CallStacks (c_items);
      if (topn < (int64_t)c_items.size()) {
       // Determine the topn items.
        std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult_CallStackEntry *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
     // Sort report in calling tree order.
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CallStacks<std::pair<CommandResult_CallStackEntry *,
                                                     SmartPtr<std::vector<CommandResult *> > > >());
      Expand_Straight (c_items);
      Combine_Duplicate_CallStacks (c_items);
    } else if (Look_For_KeyWord(cmd, "LinkedObjects")) {
     // LinkedObjects doesn't seem to be meaningful.
      vg = VIEW_LINKEDOBJECTS;
      Mark_Cmd_With_Soft_Error(cmd,"(LinkedObject View is not supported)");
      return false;   // There is no data, return.
    } else {
     // Default is the summary report by MPI function.
      EO_Title = "Function (defining location)";
      Summary_Report (f_items, c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult_CallStackEntry *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if (topn < (int64_t)c_items.size()) {
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
    }

    topn = min(topn, (int64_t)c_items.size());


   // Build a Header for each column in the table.
    CommandResult_Headers *H = new CommandResult_Headers ();
   // Add Metrics
    for ( i=0; i < num_columns; i++) {
      ViewInstruction *vinst = ViewInst[i];
      int64_t CM_Index = vinst->TMP1();

      std::string column_header;
      if ((HV.size() > i) &&
          (HV[i].length() > 0)) {
        column_header = HV[i];
      } else if (vinst->OpCode() == VIEWINST_Display_Metric) {
        if (Metadata_hasName( CV[CM_Index], MV[CM_Index] )) {
          Metadata m = Find_Metadata ( CV[CM_Index], MV[CM_Index] );
          column_header = m.getShortName();
        } else {
          column_header = MV[CM_Index];
        }
      } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
        if (HV.size() > i) {
          column_header = HV[i];
        } else {
          column_header = std::string("Temp" + CM_Index);
        }
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

   // Convert "0" values to blanks.
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator xvpi;
    for (xvpi = c_items.begin(); xvpi != c_items.end(); xvpi++) {
     // Foreach CallStack entry, look for duplicates and missing intermediates.
      std::pair<CommandResult_CallStackEntry *,
                SmartPtr<std::vector<CommandResult *> > > cp = *xvpi;
      double V;
      ((CommandResult_Float *)(*cp.second)[0])->Value(V);
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
    Construct_View (cmd, topn, tgrp, CV, MV, IV,
                    num_columns,
                    ViewInst, Gen_Total_Percent, percentofcolumn, TotalValue, report_Column_summary,
                    c_items);

  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return ((cmd->Status() != CMD_ERROR) && (cmd->Status() != CMD_ABORTED));
}

static void define_columns (CommandObject *cmd,
                           std::vector<ViewInstruction *>& IV,
                           std::vector<std::string>& HV) {

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();

 // Initial instructions to insert time into first column.
  int64_t last_column = 0;
  IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, 0));  // first is total time
  HV.push_back("Exclusive Time");
  
  if (p_slist->begin() != p_slist->end()) {
   // Add modifiers to output list.
    int64_t i = 0;
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
        if (!strcasecmp(M_Name.c_str(), "min")) {
         // second temp is min time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, 1));
          HV.push_back("Min Time");
        } else if (!strcasecmp(M_Name.c_str(), "max")) {
         // third temp is max time
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, 2));
          HV.push_back("Max Time");
        } else if (!strcasecmp(M_Name.c_str(), "count")) {
         // fourth temp is total counts
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, 3));
          HV.push_back("Number of Calls");
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum (#0) and total counts (#3).
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column++, 0, 3));
          HV.push_back("Average Time");
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // sixth temp is percent of first tmp
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column++, 0));
          HV.push_back("% of Total");
        }
      }
    }
  }
}

static bool mpi_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                             std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV) {
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
    std::string M_Name("exclusive_times");
    MV.push_back(M_Name);
    if (!Collector_Generates_Metric (*CV.begin(), M_Name)) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    define_columns (cmd, IV, HV);

    return true;
}


// mpi view

static std::string VIEW_mpi_brief = "Mpi Report";
static std::string VIEW_mpi_short = "Report the time spent in each mpi function.";
static std::string VIEW_mpi_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'mpi' to indicate the maximum number of items in the report."
                                      "\nA summary report can be requested with the '-v Functions' option"
                                      " and will be sorted in descending order of the time spent in each"
                                      " function."
                                      " A call stack report can be requested with the '-v Statements' option"
                                      " and will be presented in calling tree order."
                                      " If no '-v' option is specified, the default report is equivalent to"
                                      " '-v Functions'."
                                      "\nAdditional information can be requested with the '-m' option.  Items"
                                      " listed after the option will cause additiional columns to be added"
                                      " to the report. More than one option can be selected."
                                      " The full set available options is: 'min', 'max', 'average', 'count'"
                                      " and 'percent'."
                                      " Each option reports information about the set of mpi calls that is"
                                      " reported for the function on that particular line in the report."
                                      " \n\t'-m min' reports the minimum time spent in the function."
                                      " \n\t'-m max' reports the maximum time spent in the function."
                                      " \n\t'-m average' reports the average time spent in the function."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of mpi time the function represents.";
static std::string VIEW_mpi_metrics[] =
  { ""
  };
static std::string VIEW_mpi_collectors[] =
  { "mpi",
    ""
  };
static std::string VIEW_mpi_header[] =
  { ""
  };
class mpi_view : public ViewType {

 public: 
  mpi_view() : ViewType ("mpi",
                          VIEW_mpi_brief,
                          VIEW_mpi_short,
                          VIEW_mpi_long,
                         &VIEW_mpi_metrics[0],
                         &VIEW_mpi_collectors[0],
                         &VIEW_mpi_header[0],
                           true,
                           false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {

    std::vector<std::string> HV;
    CV.erase(CV.begin(), CV.end());
    CV.push_back (Get_Collector (exp->FW(), "mpi"));  // Define the collector
    if (mpi_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV)) {
       return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
    }
    return false;
  }
};

// mpit view

static std::string VIEW_mpit_brief = "Mpit Report";
static std::string VIEW_mpit_short = "Report the time spend in each each mpi function.";
static std::string VIEW_mpit_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'mpit' to indicate the maximum number of items in the report."
                                      "\nA summary report can be requested with the '-v Functions' option"
                                      " and will be sorted in descending order of the time spent in each"
                                      " function."
                                      " A call stack report can be requested with the '-v Statements' option"
                                      " and will be presented in calling tree order."
                                      " If no '-v' option is specified, the default report is equivalent to"
                                      " '-v Functions'";
static std::string VIEW_mpit_metrics[] =
  { ""
  };
static std::string VIEW_mpit_collectors[] =
  { "mpit",
    ""
  };
static std::string VIEW_mpit_header[] =
  { ""
  };
class mpit_view : public ViewType {

 public: 
  mpit_view() : ViewType ("mpit",
                          VIEW_mpit_brief,
                          VIEW_mpit_short,
                          VIEW_mpit_long,
                         &VIEW_mpit_metrics[0],
                         &VIEW_mpit_collectors[0],
                         &VIEW_mpit_header[0],
                           true,
                           false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
    std::vector<std::string> HV;
    CV.erase(CV.begin(), CV.end());
    CV.push_back (Get_Collector (exp->FW(), "mpit"));  // Define the collector
    if (mpi_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV)) {
       return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
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
