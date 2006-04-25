/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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


inline bool topStack_In_Subextent (
    const Framework::StackTrace& st,
    Framework::ExtentGroup& subextents)
{
  Time t = st.getTime();
  Address a = st[0];
  for (ExtentGroup::iterator ei = subextents.begin(); ei != subextents.end(); ei++) {
    Extent check = *ei;
    if (!check.isEmpty()) {
      TimeInterval time = check.getTimeInterval();
      AddressRange addr = check.getAddressRange();
      if (time.doesContain(t) &&
          addr.doesContain(a)) return true;
    }
  }
  return false;
}

template <typename TS>
void Get_Subextents_To_Object (
    const Framework::ThreadGroup& tgrp,
    TS& object,
    Framework::ExtentGroup& subextents)
{

  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    ExtentGroup newExtents = object.getExtentIn (*ti);
    for (ExtentGroup::iterator ei = newExtents.begin(); ei != newExtents.end(); ei++) {
      // if (subextents.find(*ei) != newExtents.end()) {
        subextents.push_back(*ei);
      // }
    }
  }

}

template <typename TS>
void Get_Subextents_To_Object_Map (
    const Framework::ThreadGroup& tgrp,
    TS& object,
    std::map<Framework::Thread, Framework::ExtentGroup>& subextents_map)
{

  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    ExtentGroup newExtents = object.getExtentIn (*ti);
    Framework::ExtentGroup subextents;
    for (ExtentGroup::iterator ei = newExtents.begin(); ei != newExtents.end(); ei++) {
      subextents.push_back(*ei);
    }
    subextents_map[*ti] = subextents;
  }

}

inline int64_t stack_contains_N_calls (
    const Framework::StackTrace& st,
    Framework::ExtentGroup& subextents)
{
  Time t = st.getTime();
  int64_t num_calls = 0;
  for (ExtentGroup::iterator ei = subextents.begin(); ei != subextents.end(); ei ++) {
    Extent check = *ei;
    if (!check.isEmpty()) {
      TimeInterval time = check.getTimeInterval();
      AddressRange addr = check.getAddressRange();
      int64_t sti;
      for (sti = 0; sti < st.size(); sti++) {
        Address a = st[sti];
        if (time.doesContain(t) &&
            addr.doesContain(a)) {
          num_calls++;
       }
      }
    }
  }
  return num_calls;
}

inline void Dump_StackTrace (const Framework::StackTrace& st) {
  cerr << "StackTrace:\n";
  int64_t len = st.size();
  for (int64_t i = 0;  i < len; i++) {
    std::pair<bool, Function> fp = st.getFunctionAt(i);
    if (fp.first) {
      cerr << "  " << fp.second.getName() << "\n";;
    }
  }
}

inline void Dump_CallStack (std::vector<CommandResult *> *call_stack,
                            std::vector<CommandResult *> *vcs) {
  cerr << "CallStack: ";
  for (int64_t i = 0; i < vcs->size(); i++) {
    CommandResult *p = (*vcs)[i];
    if (p != NULL) {
      p->Print(cerr);
    }
  }
  cerr << "\n";

  int64_t len = call_stack->size();
  for (int64_t j = 0; j < len; j++) {
    CommandResult *p = (*call_stack)[j];
    if (p != NULL) {
      cerr << "  " << j << "  ";
      std::string S = p->Form();
      cerr << S << "\n";
    }
  }
}

struct ltST {
  bool operator() (Framework::StackTrace stl, Framework::StackTrace str) {
    if (stl.getTime() < str.getTime()) { return true; }
    if (stl.getTime() > str.getTime()) { return false; }

    if (stl.getThread() < str.getThread()) { return true; }
    if (stl.getThread() > str.getThread()) { return false; }

    return stl < str;
  }
};

#define  Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map)    \
{                                                                                \
  /* If we have already processed this StackTrace, skip it! */                   \
    if (StackTraces_Processed.find(st) != StackTraces_Processed.end()) {         \
      continue;                                                                  \
    }                                                                            \
                                                                                 \
   /* Find the extents associated with the stack trace's thread. */              \
    std::map<Framework::Thread, Framework::ExtentGroup>::iterator tei            \
                               = SubExtents_Map.find(st.getThread());            \
    Framework::ExtentGroup SubExtents;                                           \
    if (tei != SubExtents_Map.end()) {                                           \
      SubExtents = (*tei).second;                                                \
    }                                                                            \
                                                                                 \
   /* Count the number of recursive calls in the stack.                          \
      The count in the Detail metric includes each call,                         \
      but the inclusive time has been incremented for each                       \
      call and we only want the time for the stack trace. */                     \
    int64_t calls_In_stack = (SubExtents.begin() == SubExtents.end())            \
                               ? 1 : stack_contains_N_calls (st, SubExtents);    \
                                                                                 \
   /* Use macro to accumulate all the separate samples. */                       \
    get_inclusive_values (details, calls_In_stack)                               \
                                                                                 \
   /* Decide if we accumulate exclusive_time, as well. */                        \
    if (topStack_In_Subextent (st, SubExtents)) {                                \
     /* Bottom of trace is current function.                                     \
        Exclusive_time is the same as inclusive_time.                            \
        Deeper calls must go without exclusive_time. */                          \
      get_exclusive_values (details, calls_In_stack)                             \
    }                                                                            \
                                                                                 \
   /* Remember that we have now processed this particular StackTrace. */         \
    StackTraces_Processed.insert(st);                                            \
}

