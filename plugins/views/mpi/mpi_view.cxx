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

inline std::string ptr2str (void *p) {
  char s[40];
  sprintf ( s, "%p", p);
  return std::string (s);
}

enum View_Granularity {
  VIEW_FUNCTIONS,
  VIEW_STATEMENTS,
  VIEW_LINKEDOBJECTS
};

class CommandResult_CallStackEntry : public CommandResult {
 private:
  bool Bottom_up;
  SmartPtr<std::vector<CommandResult *> > CallStack;

  CommandResult_CallStackEntry () : CommandResult(CMD_RESULT_EXTENSION) {
    Bottom_up = false;
  }

 public:
  CommandResult_CallStackEntry (SmartPtr<std::vector<CommandResult *> >& call_stack,
                                bool Reverse=false)
      : CommandResult(CMD_RESULT_CALLTRACE) {
    Bottom_up = Reverse;
    CallStack = call_stack;
  }
  virtual ~CommandResult_CallStackEntry () {
    std::vector<CommandResult *>::iterator csi;
    for (csi = CallStack->begin(); csi != CallStack->end(); csi++) {
      delete (*csi);
    }
  }

  SmartPtr<std::vector<CommandResult *> > Value () {
    return CallStack;
  };
  void Value (SmartPtr<std::vector<CommandResult *> >& call_stack) {
    call_stack = CallStack;
  };

  virtual std::string Form () {
    int64_t sz = CallStack->size();
    if (sz <= 0) return std::string("");
    CommandResult *CE = (*CallStack)[sz - 1];
    std::string Name;
   // Add indentation.
    for (int64_t i = 1; i < sz; i++) {
      Name += ((Bottom_up) ? "<" : ">");
    }
   // Add line number.
    if (sz > 1) {
      if (CE->Type() == CMD_RESULT_FUNCTION) {
        std::set<Statement> T;
        ((CommandResult_Function *)CE)->Value(T);
        if (T.begin() != T.end()) {
          std::set<Statement>::const_iterator sti = T.begin();;
          Statement S = *sti;
          char l[50];
          sprintf( &l[0], "%lld", (int64_t)(S.getLine()));
          Name = Name + " @ " + l + " in ";
        }
      } else if (CE->Type() == CMD_RESULT_LINKEDOBJECT) {
        uint64_t V;
        ((CommandResult_LinkedObject *)CE)->Value(V);
        char l[50];
        sprintf( &l[0], "+0x%llx", V);
        std::string l_name;
        ((CommandResult_LinkedObject *)CE)->Value(l_name);
        Name = Name + " @ " + l_name + l + " in ";
      } else if (CE->Type() == CMD_RESULT_UINT) {
        Name += " @ ";
      }
    }
   // Add function name and location information.
    Name += CE->Form();
    return Name;
  }
  virtual PyObject * pyValue () {
    std::string F = Form ();
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (ostream &to, int64_t fieldsize, bool leftjustified) {
    std::string string_value = Form ();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
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

static void Dump_Intermediate_CallStack (
       std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >& c_items) {
  std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, dump the corresponding value and the last call stack function name.
    std::pair<CommandResult_CallStackEntry *, CommandResult *> cp = *vpi;
    cerr << "    "; ((CommandResult *)(cp.second))->Print(cerr); cerr << "  ";
     ((CommandResult *)(cp.first))->Print(cerr); cerr << std::endl;
  }
}

static
bool Raw_Data (CommandObject *cmd,
               Collector& collector,
               std::string& metric,
               ThreadGroup& tgrp,
               SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<double> > > >& items) {

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
              SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<double> > > >& f_items,
              std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >& c_items) {
 // Combine all the items for each function.
 // Input data is sorted by function.
    std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
    for (fi = f_items->begin(); fi != f_items->end(); fi++) {
      double sum = 0.0;
      std::map<Framework::StackTrace, std::vector<double> >:: iterator si;
      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
        std::vector<double>::iterator vi;
        for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
          sum += *vi;
        }
      }
      CommandResult *Sum = CRPTR (sum);
     // Construct callstack for last entry in the stack trace.
      Function F = (*fi).first;
      std::map<Framework::StackTrace, std::vector<double> >::iterator first_si = 
                                  (*fi).second.begin();
      Framework::StackTrace st = (*first_si).first;
      std::set<Statement> T = st.getStatementsAt(st.size()-1);

      SmartPtr<std::vector<CommandResult *> > call_stack =
               Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
      call_stack->push_back(new CommandResult_Function (F, T));
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, Sum));
    }
}

static SmartPtr<std::vector<CommandResult *> > 
       Construct_CallBack (Framework::StackTrace& st) {
  SmartPtr<std::vector<CommandResult *> > call_stack;
  int64_t len = st.size();
  if (len == 0) return call_stack;
  call_stack = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  // for (int64_t i = 0; i < len; i++) {
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
              SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<double> > > >& f_items,
              std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >& c_items) {
 // Sum the data items for each call stack.
  std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
  for (fi = f_items->begin(); fi != f_items->end(); fi++) {
   // Foreach MPI function ...
    std::map<Framework::StackTrace, std::vector<double> >::iterator sti;
    for (sti = (*fi).second.begin(); sti != (*fi).second.end(); sti++) {
     // Foreach call stack ...
      Framework::StackTrace st = (*sti).first;
      double sum = 0.0;
      for (int64_t i = 0; i < (*sti).second.size(); i++) {
       // Combine all the values.
        sum += (*sti).second[i];
      }
      CommandResult *Sum = CRPTR (sum);
      SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (st);
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (call_stack);
      c_items.push_back(std::make_pair(CSE, Sum));
    }
  }
}

CommandResult * Dup_CommandResult (CommandResult *C) {
  if (C == NULL) return NULL;
  switch (C->Type()) {
   case CMD_RESULT_UINT:
     return new CommandResult_Uint((CommandResult_Uint *)C);
   case CMD_RESULT_INT:
     return new CommandResult_Int((CommandResult_Int *)C);
   case CMD_RESULT_FLOAT:
     return new CommandResult_Float((CommandResult_Float *)C);
   case CMD_RESULT_STRING:
     return new CommandResult_String((CommandResult_String *)C);
   case CMD_RESULT_RAWSTRING:
     return new CommandResult_RawString((CommandResult_RawString *)C);
   case CMD_RESULT_FUNCTION:
     return new CommandResult_Function((CommandResult_Function *)C);
   case CMD_RESULT_STATEMENT:
     return new CommandResult_Statement((CommandResult_Statement *)C);
   case CMD_RESULT_LINKEDOBJECT:
     return new CommandResult_LinkedObject((CommandResult_LinkedObject *)C);
  }
  return NULL;
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
// cerr << "  new cse "; NCE->Print(cerr); cerr << std::endl;
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
              std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >& c_items) {
/* TEST
  cerr << "Initial CallStack list:\n";
  Dump_Intermediate_CallStack (c_items);
TEST */
  std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult_CallStackEntry *, CommandResult *> cp = *vpi;
// cerr << "Pick up vector(1) from "; ((CommandResult *)(cp.first))->Print(cerr); cerr << std::endl;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult_CallStackEntry *, CommandResult *> ncp = *nvpi;
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
        if (ncp.second != NULL) {
          if (cp.second != NULL) {
            Accumulate_CommandResult (cp.second, ncp.second);
          } else {
            cp.second = ncp.second;
            ncp.second = NULL;
          }
        }
// cerr << "erase identical item "; ((CommandResult *)(ncp.first))->Print(cerr); cerr << std::endl;
        nvpi = c_items.erase(nvpi);
        delete ncp.first;
        if (ncp.second != NULL) {
          delete ncp.second;
        }
        continue;
      }
     // Match failed.
      if (cs_size == ncs->size()) {
// cerr << "  equal level match failed\n";
        break;
      }
      nvpi++;
    }

  }
/* TEST
  cerr << "Merged CallStack list:\n";
  Dump_Intermediate_CallStack (c_items);
TEST */
}

static void Expand_Straight (
              std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >& c_items) {
// Process base report.
  std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator vpi;
/* TEST
  cerr << "Initial list is:\n";
  Dump_Intermediate_CallStack (c_items);
TEST */
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator nvpi = vpi+1;
    std::pair<CommandResult_CallStackEntry *, CommandResult *> cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();
    // cp.first->Value(cs);

    nvpi = vpi - 1;
    int64_t xcnt = 0;
    int64_t lcnt = 0;
    if (vpi == c_items.begin()) {
     // Fully expand current call tree.
      lcnt = cs->size();
    } else {
      std::pair<CommandResult_CallStackEntry *, CommandResult *> ncp = *nvpi;
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
      SmartPtr<std::vector<CommandResult *> > ncs = Dup_Call_Stack (i, cs);
      CommandResult *nv = new CommandResult_Float (0.0); // get "0" value
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs);
      nvpi = c_items.insert(vpi, std::make_pair(CSE, nv));
      vpi = nvpi + 1;
    }
  }
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

   // Extract the top "n" items from the sorted list.
    typename std::vector<std::pair<TE, CommandResult *> >::iterator it;
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
          Next_Metric_Value = it->second;
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
            // CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
            //                                                    CV[percentIndex], MV[percentIndex] );
            // Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          // Next_Metric_Value = NULL; /?? not sure how to implement this
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
          if (!Gen_Total_Percent) {
           // The measured time interval is too small.
            continue;
          }
          // CommandResult *Metric_Result = Get_Object_Metric( cmd, it->first, tgrp,
          //                                                      CV[CM_Index], MV[CM_Index] );
          // Next_Metric_Value = Calculate_Percent (Metric_Result, TotalValue);
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
                       std::vector<ViewInstruction *> IV, std::string *HV) {
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
    if (ViewInst[0]->OpCode() != VIEWINST_Display_Metric) {
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


   // What granularity has been requested?
    View_Granularity vg = VIEW_FUNCTIONS;
    std::string EO_Title;
    std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> > c_items;
    if (Look_For_KeyWord(cmd, "Statements")) {
     // Straight Report will break down report by call stack.
      vg = VIEW_STATEMENTS;
      EO_Title = "Call Stack Function (defining location)";
      CallStack_Report (f_items, c_items);
      Combine_Duplicate_CallStacks (c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult_CallStackEntry *, CommandResult *> >());
      if (topn < (int64_t)c_items.size()) {
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
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
                sort_descending_CommandResult<std::pair<CommandResult_CallStackEntry *, CommandResult *> >());
      if (topn < (int64_t)c_items.size()) {
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
    }

    topn = min(topn, (int64_t)c_items.size());


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

   // Convert "0" values to blanks.
    std::vector<std::pair<CommandResult_CallStackEntry *, CommandResult *> >::iterator xvpi;
    for (xvpi = c_items.begin(); xvpi != c_items.end(); xvpi++) {
     // Foreach CallStack entry, look for duplicates and missing intermediates.
      std::pair<CommandResult_CallStackEntry *, CommandResult *> cp = *xvpi;
      double V;
      ((CommandResult_Float *)cp.second)->Value(V);
      if (V == 0.0) {
       // Set flag in CommandResult to indicate null value.
       // The display logic may decide to replace the value with
       // blanks, if it is easier to read.
        cp.second->setNullValue();
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

// mpi view

static std::string VIEW_mpi_brief = "Mpi Report";
static std::string VIEW_mpi_short = "Report the metric values gathered for each mpi function in a program.";
static std::string VIEW_mpi_long  = "The report is sorted in descending order by the first metric."
                                      " A positive integer can be added to the end of the keyword"
                                      " ""mpi"" to indicate the maximum number of items in the report."
                                      " A summary report can be requested with the '-v Functions' option."
                                      " A call stack report can be requested with the '-v Statements' option.";
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
                           true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
   // Start with a clean slate.
    CV.erase(CV.begin(), CV.end());
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    CollectorGroup cgrp = exp->FW()->getCollectors();
    std::string M_Name("exclusive_times");
    std::string C_Name = Find_Collector_With_Metric ( cgrp, M_Name);
    if (C_Name .length() == 0) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
    if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    CV.push_back (Get_Collector (exp->FW(), "mpi"));  // Get the collector
    MV.push_back("exclusive_times");  // Get the name of the metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric

    return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, NULL);
  }
};

// mpit view

static std::string VIEW_mpit_brief = "Mpit Report";
static std::string VIEW_mpit_short = "Report the metric values gathered for each mpi function in a program.";
static std::string VIEW_mpit_long  = "The report is sorted in descending order by the first metric."
                                      " A positive integer can be added to the end of the keyword"
                                      " ""mpit"" to indicate the maximum number of items in the report."
                                      " A summary report can be requested with the '-v Functions' option."
                                      " A call stack report can be requested with the '-v Statements' option.";
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
                           true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV,
                         std::vector<ViewInstruction *>IV) {
   // Start with a clean slate.
    CV.erase(CV.begin(), CV.end());
    MV.erase(MV.begin(), MV.end());
    IV.erase(IV.begin(), IV.end());

    CollectorGroup cgrp = exp->FW()->getCollectors();
    std::string M_Name("exclusive_times");
    std::string C_Name = Find_Collector_With_Metric ( cgrp, M_Name);
    if (C_Name .length() == 0) {
      std::string s("The metrics required to generate the view are not available in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }
    if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
      std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;
    }

    CV.push_back (Get_Collector (exp->FW(), "mpit"));  // Get the collector
    MV.push_back("exclusive_times");  // Get the name of the metric
    IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, 0, 0));  // first column is metric

    return Generic_mpi_View (cmd, exp, topn, tgrp, CV, MV, IV, NULL);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void mpi_view_LTX_ViewFactory () {
  Define_New_View (new mpi_view());
  Define_New_View (new mpit_view());
}
