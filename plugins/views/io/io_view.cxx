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

// Indicate where in the predefined-temporay table certain vaules are.
#define sum_temp 0
#define min_temp 1
#define max_temp 2
#define cnt_temp 3
#define ssq_temp 4
#define num_predefine_temps 5

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

// Create a new CommandResult object, with an initial default value,
// that has the same type as another CommandResult object.
static inline CommandResult *Copy_CRType (CommandResult *old) {
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
  return v;
}

static
bool Raw_Data (CommandObject *cmd,
               ExperimentObject *exp,
               Collector& collector,
               std::string& metric,
               ThreadGroup& tgrp,
               SmartPtr<std::map<Function,
                        std::map<Framework::StackTrace,
                                 std::vector<double> > > >& items) {
// Pick up the raw data samples for the measured functions.

 // Get the list of desired functions.
  std::set<Function> objects;

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;

  vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
  if (p_tlist->begin() != p_tlist->end()) {
    // There is a list.  Is there a "-f" specifier?
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();
  }

/*
  if (p_tlist->begin() == p_tlist->end()) {
   // There is no <target> list for filtering.
   // Get all the io functions for all the threads.
    objects = exp->FW()->getFunctionsByNamePattern ("PMPI*");
  } else {
*/{

    if ((f_list == NULL) || (f_list->empty()) ||
        Look_For_KeyWord(cmd, "ButterFly")) {
     // There is no Function filtering requested or a ButerFly views is requested.
     // Get all the functions in the already selected thread groups.
     // Function filtering will be done later for ButerFly views.
      for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

       // Check for asnychonous abort command
        if (cmd->Status() == CMD_ABORTED) {
          return false;
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
        std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
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

if (objects.empty()) cerr << "No objects in Raw_Data\n";
  if (objects.empty()) {
    return false;
  }

 // Get the callstacks and data from the collector.
  GetMetricInThreadGroup (collector, metric, tgrp, objects, items);

if (items->empty()) cerr << "No tiems in Raw_Data\n";
  return !(items->empty());
}

static void Function_Report(
              SmartPtr<std::map<Function,
                                std::map<Framework::StackTrace,
                                         std::vector<double> > > >& raw_items,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Combine all the items for each function.
 // Input data is sorted by function.
    std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
    for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
      int64_t cnt = 0;
      double sum = 0.0;
      double vmax = 0.0;
      double vmin = LONG_MAX;
      double sum_squares = 0.0;
      std::map<Framework::StackTrace, std::vector<double> >:: iterator si;
      for (si = (*fi).second.begin(); si != (*fi).second.end(); si++) {
        std::vector<double>::iterator vi;
        for (vi = (*si).second.begin(); vi != (*si).second.end(); vi++) {
          double v = *vi;
          cnt ++;
          vmin = min(vmin,v);
          vmax = max(vmax,v);
          sum += v;
          sum_squares += (v * v);
        }
      }
      SmartPtr<std::vector<CommandResult *> > vcs = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_predefine_temps)
                           );
      (*vcs)[sum_temp] = CRPTR (sum);
      (*vcs)[min_temp] = CRPTR (vmin);
      (*vcs)[max_temp] = CRPTR (vmax);
      (*vcs)[cnt_temp] = CRPTR (cnt);
      (*vcs)[ssq_temp] = CRPTR (sum_squares);
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

static void CallStack_Report (
              bool TraceBack_Order,
              bool add_stmts,
              SmartPtr<std::map<Function,
                                std::map<Framework::StackTrace,
                                         std::vector<double> > > >& raw_items,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Sum the data items for each call stack.
  std::map<Function,
           std::map<Framework::StackTrace,
                    std::vector<double> > >::iterator fi;
  for (fi = raw_items->begin(); fi != raw_items->end(); fi++) {
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
      double sum_squares = 0.0;
      int64_t len = (*sti).second.size();
      for (int64_t i = 0; i < len; i++) {
       // Combine all the values.
        double v = (*sti).second[i];
        cnt ++;
        vmin = min(vmin,v);
        vmax = max(vmax,v);
        sum += v;
        sum_squares += v * v;
      }
      SmartPtr<std::vector<CommandResult *> > vcs
               = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>(num_predefine_temps)
                           );
      (*vcs)[sum_temp] = CRPTR (sum);
      (*vcs)[min_temp] = CRPTR (vmin);
      (*vcs)[max_temp] = CRPTR (vmax);
      (*vcs)[cnt_temp] = CRPTR (cnt);
      (*vcs)[ssq_temp] = CRPTR (sum_squares);
      SmartPtr<std::vector<CommandResult *> > call_stack = Construct_CallBack (TraceBack_Order, add_stmts, st);
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (call_stack,
                                                                            TraceBack_Order);
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

static inline void Accumulate_PreDefined_Temps (std::vector<CommandResult *>& A,
                                                std::vector<CommandResult *>& B) {
  Accumulate_CommandResult (A[sum_temp], B[sum_temp]);
  Accumulate_Min_CommandResult (A[min_temp], B[min_temp]);
  Accumulate_Max_CommandResult (A[max_temp], B[max_temp]);
  Accumulate_CommandResult (A[cnt_temp], B[cnt_temp]);
  Accumulate_CommandResult (A[ssq_temp], B[ssq_temp]);
}

static void Combine_Duplicate_CallStacks (
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
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
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps ((*cp.second), (*ncp.second));
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
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
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
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps ((*cp.second), (*ncp.second));
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
        vcs->push_back ( Copy_CRType ((*crv)[j]) );
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
              bool TraceBack_Order,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items,
              Function& func,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& result) {
  bool pivot_added = false;
  std::pair<CommandResult_CallStackEntry *,
            SmartPtr<std::vector<CommandResult *> > > pivot;
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > > pred;
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > > succ;

  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for the matching function name.
    std::pair<CommandResult_CallStackEntry *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();

    for (int64_t i = 0; i < cs->size(); i++) {
      CommandResult *cof = (*cs)[i];
      if ((cof->Type() == CMD_RESULT_FUNCTION) &&
          ((*(CommandResult_Function *)cof) == func)) {
       // Insert intermediate, dummy entry to fill a gap in the trace.
        if (!pivot_added) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, 0, cs);
          CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs,
                                                                                TraceBack_Order);
          pivot = std::make_pair(CSE, vcs);
          pivot_added = true;
        } else {
          Accumulate_PreDefined_Temps ((*pivot.second), (*cp.second));
        }
        if (i != 0) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, -1, cs);
          CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs,
                                                                                !TraceBack_Order);
          pred.push_back (std::make_pair(CSE, vcs));
        }
        if ((i+1) < cs->size()) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          SmartPtr<std::vector<CommandResult *> > ncs = Copy_Call_Stack_Entry (i, +1, cs);
          CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs,
                                                                                TraceBack_Order);
          succ.push_back (std::make_pair(CSE, vcs));
        }
        break;
      }
    }
  }
  if (pivot_added) {
    if (!pred.empty()) {
      Combine_Short_Stacks (pred);
      result.insert(result.end(), pred.begin(), pred.end());
    }
    result.push_back (pivot);
    if (!succ.empty()) {
      Combine_Short_Stacks (succ);
      result.insert(result.end(), succ.begin(), succ.end());
    }
  }
}

static void Expand_CallStack (
              bool TraceBack_Order,
              std::vector<std::pair<CommandResult_CallStackEntry *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
 // Rewrite base report by expanding the call stack.
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > > result;
  std::vector<std::pair<CommandResult_CallStackEntry *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    std::pair<CommandResult_CallStackEntry *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    SmartPtr<std::vector<CommandResult *> > cs = cp.first->Value();

    // for (int64_t i = cs->size()-1; i > 0; i--) {
    for (int64_t i = 1; i < cs->size(); i++) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs = Dup_CRVector (cp.second);
      SmartPtr<std::vector<CommandResult *> > ncs = Dup_Call_Stack (i, cs);
      CommandResult_CallStackEntry *CSE = new CommandResult_CallStackEntry (ncs,
                                                                            TraceBack_Order);
      result.push_back (std::make_pair(CSE, vcs));
    }
    result.push_back (cp);
  }
  c_items = result;
}

static
void Form_CallStack_Output (CommandObject *cmd,
                            int64_t topn,
                            ThreadGroup& tgrp,
                            std::vector<Collector>& CV,
                            std::vector<std::string>& MV,
                            std::vector<ViewInstruction *>& IV,
                            int64_t num_columns,
                            bool Gen_Total_Percent,
                            int64_t percentofcolumn,
                            CommandResult *TotalValue,
                            std::vector<std::pair<CommandResult_CallStackEntry *,
                                                  SmartPtr<std::vector<CommandResult *> > > >& items) {
  if (items.empty()) return;

  int64_t i;
  bool report_Column_summary = false;

  std::vector<ViewInstruction *> ViewInst(num_columns);
  int64_t num_input_temps = items[0].second->size();
  int64_t num_total_temps = max(num_input_temps, Find_Max_Tmp_Def(IV)+1);
  bool input_temp_used[num_input_temps];
  std::vector<ViewInstruction *> Summary_Display(num_columns);
  for ( i=0; i <num_columns; i++) Summary_Display[i] = NULL;
  std::vector<ViewInstruction *> SummaryInst(num_input_temps);
  for ( i=0; i <num_input_temps; i++) SummaryInst[i] = NULL;
  std::vector<CommandResult *> temp(num_total_temps);
  for ( i=0; i < num_total_temps; i++) temp[i] = NULL;

 // Reformat the instructions for easier access.
  std::vector<CommandResult *> first_row = *(items.begin()->second);
  for (i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
      if ((vp->OpCode() == VIEWINST_Display_Metric) ||
          (vp->OpCode() == VIEWINST_Display_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Metric) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
        ViewInst[vp->TR()] = vp;
      }
      if ((vp->OpCode() == VIEWINST_Display_Summary_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Summary_StdDev) ||
          (vp->OpCode() == VIEWINST_Display_Summary_Average)) {
        report_Column_summary = true;
        Summary_Display[vp->TR()] = vp;
      }
      if ((vp->OpCode() == VIEWINST_Column_Summary_Add) ||
          (vp->OpCode() == VIEWINST_Column_Summary_Min) ||
          (vp->OpCode() == VIEWINST_Column_Summary_Max)) {
        SummaryInst[vp->TMP1()] = vp;
        temp[vp->TR()] = Copy_CRType (first_row[vp->TMP1()]);
      }
  }

   // Format the report with the items that are in the vector.
    std::vector<std::pair<CommandResult_CallStackEntry *,
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
          Next_Metric_Value = (*it->second)[i];
          input_temp_used[i] = true;
        } else if (vinst->OpCode() == VIEWINST_Display_Tmp) {
          Next_Metric_Value = (*it->second)[vinst->TMP1()];
          input_temp_used[CM_Index] = true;
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Column) {
          if (!Gen_Total_Percent) {
           // The measured time interval is too small.
            continue;
          }
          if ((i > percentofcolumn) &&
              (percent_of != NULL)) {
            Next_Metric_Value = Calculate_Percent (percent_of, TotalValue);
          } else {
            Mark_Cmd_With_Soft_Error(cmd,"Percent of column is not available for io view");
            return;
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Metric) {
          if (Gen_Total_Percent) {
            Mark_Cmd_With_Soft_Error(cmd,"Percent Metric value is not available for io view");
            return;
          }
        } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {
          CommandResult *V = (*it->second)[CM_Index];
          if (!V->isNullValue ()) {
            Next_Metric_Value = Calculate_Percent (V, TotalValue);
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
        }
        C->CommandResult_Columns::Add_Column (Next_Metric_Value);
        if (Gen_Total_Percent &&
            (i == percentofcolumn)) {
          percent_of = Next_Metric_Value;
        }
      }
     // Add ID for row
      C->CommandResult_Columns::Add_Column (it->first);
      cmd->Result_Predefined (C);

     // Accumulate Summary Information
      if (report_Column_summary) {
        for ( i=0; i < num_input_temps; i++) {
          ViewInstruction *sinst = SummaryInst[i];
          if (sinst != NULL) {
            int64_t def_tmp = sinst->TR();
            CommandResult *v = (*it->second)[sinst->TMP1()];
            if (temp[def_tmp] != NULL) {
              if (sinst->OpCode() == VIEWINST_Column_Summary_Add) {
                Accumulate_CommandResult (temp[def_tmp], v);
              } else if (sinst->OpCode() == VIEWINST_Column_Summary_Min) {
                Accumulate_Min_CommandResult (temp[def_tmp], v);
              } else if (sinst->OpCode() == VIEWINST_Column_Summary_Max) {
                Accumulate_Max_CommandResult (temp[def_tmp], v);
              }
            }
          }
        }
      }

     // Reclaim space for unused temps.
      for ( i=0; i < num_input_temps; i++) {
        if (!input_temp_used[i]) {
          delete (*it->second)[i];
        }
      }
    }

    if (report_Column_summary) {
     // Build an Ender summary for the table.
      CommandResult_Enders *E = new CommandResult_Enders ();
     // Add Metrics Summary
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *sinst = Summary_Display[i];
        CommandResult *summary = NULL;
        Assert (sinst != NULL);
        if (sinst->OpCode() == VIEWINST_Display_Summary_Tmp) {
          summary = Dup_CommandResult (temp[sinst->TMP1()]);
        } else if (sinst->OpCode() == VIEWINST_Display_Summary_Average) {
          CommandResult *V = temp[sinst->TMP1()];
          if (!V->isNullValue ()) {
            summary = Calculate_Average (V, temp[sinst->TMP2()]);
          }
        } else if (sinst->OpCode() == VIEWINST_Display_Summary_StdDev) {
          CommandResult *V1 = temp[sinst->TMP1()];
          CommandResult *V2 = temp[sinst->TMP2()];
          CommandResult *V3 = temp[sinst->TMP3()];
          summary = Calculate_StdDev (V1, V2, V3);
        } else if (sinst->OpCode() == VIEWINST_Display_Summary_Percent) {
          CommandResult *V = temp[sinst->TMP1()];
          summary = Calculate_Percent (V, TotalValue);
        }
        if (summary == NULL) {
          summary = CRPTR ("");
        }

        E->CommandResult_Enders::Add_Ender (summary);
      }
     // Add ID
      E->CommandResult_Enders::Add_Ender ( CRPTR ( "Report Summary" ) );
      cmd->Result_Predefined (E);
    }

 // Release summary temporaries
  for ( i=0; i < num_total_temps; i++) {
    if (temp[i] != NULL) {
      delete temp[i];
    }
  }

}

static bool Generic_io_View (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                       ThreadGroup& tgrp, std::vector<Collector>& CV, std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV, std::vector<std::string>& HV) {
   Print_View_Params (cerr, CV,MV,IV);

  if (CV.size() == 0) {
    Mark_Cmd_With_Soft_Error(cmd, "(There are no metrics specified to report.)");
    return false;   // There is no collector, return.
  }

  if (Look_For_KeyWord(cmd, "ButterFly") &&
      !Filter_Uses_F (cmd)) {
    Mark_Cmd_With_Soft_Error(cmd, "(The required function list ('-f ...') for ButterFly views is missing.)");
    return false;   // There is no collector, return.
  }

  if (topn == 0) topn = LONG_MAX;

  try {
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
      Mark_Cmd_With_Soft_Error(cmd, "(There is no display requested.)");
      return false;   // There is no column[0] defined, return.
    }
    if ((ViewInst[0]->OpCode() != VIEWINST_Display_Metric)  &&
        (ViewInst[0]->OpCode() != VIEWINST_Display_Tmp)) {
      Mark_Cmd_With_Soft_Error(cmd, "(The first column is not a metric.)");
      return false;   // There is nothing to sort on.
    }

   // Acquire base set of metric values.
    ViewInstruction *vinst0 = ViewInst[0];
    int64_t Column0index = vinst0->TMP1();
    SmartPtr<std::map<Function, std::map<Framework::StackTrace, std::vector<double> > > > f_items;
    bool first_column_found = false;
    first_column_found = Raw_Data (cmd, exp, CV[Column0index], MV[Column0index], tgrp, f_items);
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
    std::string EO_Title;
    std::vector<std::pair<CommandResult_CallStackEntry *,
                          SmartPtr<std::vector<CommandResult *> > > > c_items;
    bool TraceBack_Order = false;
    if (Look_For_KeyWord(cmd, "Statement") ||
        Look_For_KeyWord(cmd, "Statements") ||
        Look_For_KeyWord(cmd, "CallTree") ||
        Look_For_KeyWord(cmd, "CallTrees") ||
        Look_For_KeyWord(cmd, "TraceBack") ||
        Look_For_KeyWord(cmd, "TraceBacks") ||
        Look_For_KeyWord(cmd, "FullStack") ||
        Look_For_KeyWord(cmd, "FullStacks") ||
        Look_For_KeyWord(cmd, "ButterFly")) {
     // Straight Report will break down report by call stack.
      EO_Title = "Call Stack Function (defining location)";
      bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                        Look_For_KeyWord(cmd, "FullStack") ||
                        Look_For_KeyWord(cmd, "FullStacks"));

     // Determine ordering
      if (Look_For_KeyWord(cmd, "TraceBack") ||
          Look_For_KeyWord(cmd, "TraceBacks")) {
        TraceBack_Order = true;
        if (Look_For_KeyWord(cmd, "CallTree") ||
            Look_For_KeyWord(cmd, "CallTrees")) {
         // Two ordering requests.  Pick CallOrder.
         // Should there be some sort of warning message issued?
          TraceBack_Order = false;
        }
      }

     // Locking the DataBase is not necessary, but helps performance.
     // This is especially helpful if the DataBase file is not local
     // to the where this code is running.
     // We only lock around the point where we are converting the address
     // stack to a Function stack because this operation requires multiple
     // accesses to the database for each address.
     // The downside is that it may block access to other processes -
     // which is not really necessary because we only need read access
     // to do these conversions.
      CV[0].lockDatabase();
      CallStack_Report (TraceBack_Order, add_stmts, f_items, c_items);
      CV[0].unlockDatabase();

      Combine_Duplicate_CallStacks (c_items);
      if ((topn < (int64_t)c_items.size()) &&
          !Look_For_KeyWord(cmd, "ButterFly")) {
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

     // Should we expand the call stack entries in the report?
      if (!Look_For_KeyWord(cmd, "ButterFly")) {
        if (!Look_For_KeyWord(cmd, "DontExpand")) {
          Expand_CallStack (TraceBack_Order, c_items);
        }

       // Should we eliminate redundant entries in the report?
        if (!Look_For_KeyWord(cmd, "FullStack") &&
            !Look_For_KeyWord(cmd, "FullStacks")) {
          Combine_Duplicate_CallStacks (c_items);
        }
      }
    } else if (Look_For_KeyWord(cmd, "LinkedObject") ||
               Look_For_KeyWord(cmd, "LinkedObjects")) {
     // LinkedObjects doesn't seem to be meaningful.
      Mark_Cmd_With_Soft_Error(cmd,"(LinkedObject View is not supported)");
      return false;   // There is no data, return.
    } else {
     // Default is the summary report by MPI function.
      EO_Title = "Function (defining location)";
      Function_Report (f_items, c_items);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult_CallStackEntry *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if (topn < (int64_t)c_items.size()) {
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
    }

    topn = min(topn, (int64_t)c_items.size());


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
          std::vector<std::pair<CommandResult_CallStackEntry *,
                                SmartPtr<std::vector<CommandResult *> > > > result;
          Function func = *fsi;
          Extract_Pivot_Items (cmd, exp, TraceBack_Order, c_items, func, result);
          if (!result.empty()) {
            if (MoreThanOne) cmd->Result_RawString("");  // Delimiter between items is a null string.
            Form_CallStack_Output (cmd, topn, tgrp, CV, MV, IV,
                                   num_columns,
                                   Gen_Total_Percent, percentofcolumn, TotalValue,
                                   result);
            MoreThanOne = true;
          }
        }
      }

     // NOW, we need to delete the base vector and its data.
      std::vector<std::pair<CommandResult_CallStackEntry *,
                            SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
      for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
       // Foreach CallStack entry, look for duplicates and missing intermediates.
        std::pair<CommandResult_CallStackEntry *,
                  SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
        delete cp.first;
        for (int64_t i = 0; i < (*cp.second).size(); i++) {
          delete (*cp.second)[i];
        }
      }

    } else {
      Form_CallStack_Output (cmd, topn, tgrp, CV, MV, IV,
                             num_columns,
                             Gen_Total_Percent, percentofcolumn, TotalValue,
                             c_items);
    }

   // Release space for no longer needed items.
    if (TotalValue != NULL) delete TotalValue;

   // Release instructions
    for (i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      delete vp;
      IV[i] = NULL;
    }

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
 // Because we use the use the generated CommandResult temp in the final report,
 // and then delete it, we must be sure that we don't try to use it twice.
  bool min_used = false;
  bool max_used = false;
  bool count_used = false;

  int64_t last_column = 0;  // Total time is always placed in first column.

 // Initial instructions to insert time into first column.
  IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, sum_temp));  // first is total time
  HV.push_back("Exclusive Time");

  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  bool Generate_Summary = (Look_For_KeyWord(cmd, "Summary") & !Look_For_KeyWord(cmd, "ButterFly"));

  if (Generate_Summary) {
   // Total time is always displayed - also add display of the summary time.
    IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Tmp, last_column,
                                      (num_predefine_temps+sum_temp)));

   // Accumulate all intermediate values if one is needed.
    IV.push_back(new ViewInstruction (VIEWINST_Column_Summary_Add, (num_predefine_temps+sum_temp), sum_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Column_Summary_Min, (num_predefine_temps+min_temp), min_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Column_Summary_Max, (num_predefine_temps+max_temp), max_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Column_Summary_Add, (num_predefine_temps+cnt_temp), cnt_temp));
    IV.push_back(new ViewInstruction (VIEWINST_Column_Summary_Add, (num_predefine_temps+ssq_temp), ssq_temp));
  }

  last_column++;
  
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
        if (!strcasecmp(M_Name.c_str(), "min") && !min_used) {
         // second temp is min time
          min_used = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, min_temp));
          HV.push_back("Min Time");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Tmp, last_column,
                                              (num_predefine_temps+min_temp)));
          }
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "max") && !max_used) {
         // third temp is max time
          max_used = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, max_temp));
          HV.push_back("Max Time");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Tmp, last_column,
                                              (num_predefine_temps+max_temp)));
          }
          last_column++;
        } else if (!count_used &&
                   (!strcasecmp(M_Name.c_str(), "count") ||
                    !strcasecmp(M_Name.c_str(), "counts") ||
                    !strcasecmp(M_Name.c_str(), "call") ||
                    !strcasecmp(M_Name.c_str(), "calls"))) {
         // fourth temp is total counts
          count_used = true;
          IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column, cnt_temp));
          HV.push_back("Number of Calls");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Tmp, last_column,
                                              (num_predefine_temps+cnt_temp)));
          }
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "average")) {
         // average time is calculated from two temps: sum (#0) and total counts (#3).
          IV.push_back(new ViewInstruction (VIEWINST_Display_Average_Tmp, last_column, sum_temp, cnt_temp));
          HV.push_back("Average Time");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Average, last_column,
                                              (num_predefine_temps+sum_temp), (num_predefine_temps+cnt_temp)));
          }
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "percent")) {
         // percent is calculate from 2 temps: time for this row (#0) and total time.
          IV.push_back(new ViewInstruction (VIEWINST_Display_Percent_Tmp, last_column, sum_temp));
          HV.push_back("% of Total");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_Percent, last_column,
                                              (num_predefine_temps+cnt_temp)));
          }
          last_column++;
        } else if (!strcasecmp(M_Name.c_str(), "stddev")) {
         // The standard deviation is calculated from 3 temps: sum (#0), sum of squares (#4) and total counts (#3).
          IV.push_back(new ViewInstruction (VIEWINST_Display_StdDeviation_Tmp, last_column,
                                            sum_temp, ssq_temp, cnt_temp));
          HV.push_back("Standard Deviation");
          if (Generate_Summary) {
            IV.push_back(new ViewInstruction (VIEWINST_Display_Summary_StdDev, last_column,
                                              (num_predefine_temps+sum_temp),
                                              (num_predefine_temps+ssq_temp),
                                              (num_predefine_temps+cnt_temp)));
          }
          last_column++;
        } else {
          Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-m " + M_Name + "'");
          return;
        }
      }
    }
  }
}

static bool io_definition ( CommandObject *cmd, ExperimentObject *exp, int64_t topn,
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


// io view

static std::string VIEW_io_brief = "IO Report";
static std::string VIEW_io_short = "Report the time spent in each io function.";
static std::string VIEW_io_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'io' to indicate the maximum number of items in the report."
                                      "\n\nThe form of the information displayed can be controlled through"
                                      " the  '-v' option."
                                      "\n\t'-v Functions' will produce a summary report that"
                                      " will be sorted in descending order of the time spent in each"
                                      " function.  This is the default display."
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
                                      "\n\nAdditional information can be requested with the '-m' option.  Items"
                                      " listed after the option will cause additiional columns to be added"
                                      " to the report. More than one option can be selected."
                                      " The full set available options is: 'min', 'max', 'average', 'count'"
                                      " and 'percent'."
                                      " Each option reports information about the set of io calls that is"
                                      " reported for the function on that particular line in the report."
                                      " \n\t'-m min' reports the minimum time spent in the function."
                                      " \n\t'-m max' reports the maximum time spent in the function."
                                      " \n\t'-m average' reports the average time spent in the function."
                                      " \n\t'-m count' reports the number of times the function was called."
                                      " \n\t'-m percent' reports the percent of io time the function represents."
                                      " \n\t'-m stddev' reports the standard deviation of the average io time"
                                      " that the function represents.";
static std::string VIEW_io_example = "\texpView io\n"
                                      "\texpView -v CallTrees,FullStack io10 -m min,max,count\n";
static std::string VIEW_io_metrics[] =
  { ""
  };
static std::string VIEW_io_collectors[] =
  { "io",
    ""
  };
class io_view : public ViewType {

 public: 
  io_view() : ViewType ("io",
                         VIEW_io_brief,
                         VIEW_io_short,
                         VIEW_io_long,
                         VIEW_io_example,
                        &VIEW_io_metrics[0],
                        &VIEW_io_collectors[0],
                         true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "io"));  // Define the collector
    if (io_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV)) {
       return Generic_io_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
    }
    return false;
  }
};

// iot view

static std::string VIEW_iot_brief = "Mpit Report";
static std::string VIEW_iot_short = "Report the time spend in each each io function.";
static std::string VIEW_iot_long  = "\nA positive integer can be added to the end of the keyword"
                                      " 'iot' to indicate the maximum number of items in the report.";
static std::string VIEW_iot_example = "\texpView iot\n"
                                       "\texpView -v CallTree iot10\n";
static std::string VIEW_iot_metrics[] =
  { ""
  };
static std::string VIEW_iot_collectors[] =
  { "iot",
    ""
  };
class iot_view : public ViewType {

 public: 
  iot_view() : ViewType ("iot",
                          VIEW_iot_brief,
                          VIEW_iot_short,
                          VIEW_iot_long,
                          VIEW_iot_example,
                         &VIEW_iot_metrics[0],
                         &VIEW_iot_collectors[0],
                          true) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                             ThreadGroup& tgrp) {
    std::vector<Collector> CV;
    std::vector<std::string> MV;
    std::vector<ViewInstruction *>IV;
    std::vector<std::string> HV;

    CV.push_back (Get_Collector (exp->FW(), "iot"));  // Define the collector
    if (io_definition (cmd, exp, topn, tgrp, CV, MV, IV, HV)) {
       return Generic_io_View (cmd, exp, topn, tgrp, CV, MV, IV, HV);
    }
    return false;
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void io_view_LTX_ViewFactory () {
  Define_New_View (new io_view());
  Define_New_View (new iot_view());
}
