/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Definition of the inlined function topStack_In_Subextent
 * Definition of the inlined function topCallStack_In_Subextent
 *
 */


/* Uncomment these lines if you want to get debugging traces
#include "EntrySpy.hxx"
#define DEBUG_CLI 1
*/


/**
 * 
 * inlined function topStack_In_Subextent 
 * 
 * Determine if this top of stack frame is in the Subextent being searched
 *
 *
 * @param st         Stack trace frames to be examined.
 * @param subextents The time and address ranges for the subextent to be searched.
 */



inline bool topStack_In_Subextent (
    const Framework::StackTrace& st,
    Framework::ExtentGroup& subextents)
{

// Get the time associated with this stack of frames
  Time t = st.getTime();

// Look at the frame at bottom of the stack to get the address
  Address a = st[0];

#if DEBUG_CLI
  printf("Enter topStack_In_Subextent, last callstack entry address value==> a.getValue()=%x\n", a.getValue());
  printf("Enter topStack_In_Subextent, t.getValue()=%u\n", t.getValue());
  printf("Enter topStack_In_Subextent, SUBEXTENTS.SIZE()=%d\n", subextents.size());
#endif

  for (ExtentGroup::iterator ei = subextents.begin(); ei != subextents.end(); ei++) {
    Extent check = *ei;
    if (!check.isEmpty()) {

      TimeInterval time = check.getTimeInterval();
      AddressRange addr = check.getAddressRange(); 
#if DEBUG_CLI_extended
     std::cerr << "In topStack_In_Subextent, subextents: time interval=" << time << " address range=" << addr << std::endl;
#endif
      
// Is the address and time for the stack frame being searched inside the subextent?
      if (time.doesContain(t) &&
          addr.doesContain(a)) {
          
#if DEBUG_CLI
        printf("Exit topStack_In_Subextent, return true\n");
#endif
        // yes the address and time for the stack frame being searched IS inside the subextent
        return true;
      }
    } else {
#if DEBUG_CLI
        printf("In topStack_In_Subextent, subextent is empty\n");
#endif
    }
  }
#if DEBUG_CLI
  printf("Exit topStack_In_Subextent, return false\n");
#endif
          // no the address and time for the stack frame being searched is NOT inside the subextent.
	  return false;
}


/**
 * 
 * inlined function topCallStack_In_Subextent 
 * 
 * Determine if this top of stack frame is in the Subextent being searched
 * but this routine is called in the context of a calltree or traceback view
 * so we won't look at the bottom frame from the set of stack frames.  We 
 * obtain the address of the perf data from the top frame instead.
 *
 * @param st         Stack trace frames to be examined.
 * @param subextents The time and address ranges for the subextent to be searched.
 */

inline bool topCallStack_In_Subextent (
    const Framework::StackTrace& st,
    Framework::ExtentGroup& subextents,
    bool TraceBack_Order)
{

// Get the time of the sample from the stack trace (set of frames included in the st argument/class
  Time t = st.getTime();
// For callstack and traceback views we want to use the "top of stack" entry to get exclusive time
  Address a = st[st.size()-1];

#if DEBUG_CLI
  printf("Enter topCallStack_In_Subextent, TOP CALLSTACK ENTRY address value==> a.getValue()=%x\n", a.getValue());
  Address b = st[0];
  printf("Enter topCallStack_In_Subextent, BOTTOM CALLSTACK ENTRY address value==> b.getValue()=%x\n", b.getValue());
  printf("Enter topCallStack_In_Subextent, t.getValue()=%u\n", t.getValue());
#endif

  for (ExtentGroup::iterator ei = subextents.begin(); ei != subextents.end(); ei++) {
    Extent check = *ei;
    if (!check.isEmpty()) {
      TimeInterval time = check.getTimeInterval();
      AddressRange addr = check.getAddressRange();

#if DEBUG_CLI_extended
     std::cerr << "In topCallStack_In_Subextent, subextents: time interval=" << time << " address range=" << addr << std::endl;
#endif
      
// Is the address and time for the stack frame being searched inside the subextent?
      if (time.doesContain(t) &&
          addr.doesContain(a)) {
#if DEBUG_CLI
        printf("Exit topCallStack_In_Subextent, return true\n");
#endif
        // yes the address and time for the stack frame being searched IS inside the subextent
        return true;
      }
    }
  }
#if DEBUG_CLI
  printf("Exit topCallStack_In_Subextent, return false\n");
#endif
          // no the address and time for the stack frame being searched is NOT inside the subextent.
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

#if DEBUG_CLI
    printf("Enter Get_Subextents_To_Object, NEWEXTENTS.SIZE()=%d\n", newExtents.size());
    std::pair<bool, int> prank = ti->getMPIRank();
    int64_t rank = prank.first ? prank.second : -1;
    std::pair<bool, pthread_t> xtid = ti->getPosixThreadId();
    std::cerr << "Enter Get_Subextents_To_Object,  xtid.first=" << xtid.first << std::endl;
    std::cerr << "Enter Get_Subextents_To_Object,  xtid.second=" << xtid.second << " rank=" << rank << std::endl;
#endif

    for (ExtentGroup::iterator ei = newExtents.begin(); ei != newExtents.end(); ei++) {
      // if (subextents.find(*ei) != newExtents.end()) {
#if DEBUG_CLI_extended
    printf("In Get_Subextents_To_Object, calling pushing back subextents(*ei)\n");
#endif
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


#if DEBUG_CLI
  printf("Enter Get_Subextents_To_Object_Map,  tgrp.size()=%d\n", tgrp.size());
  std::cerr << "Enter Get_Subextents_To_Object_Map,  tgrp.size()=" << tgrp.size() << std::endl;
  fflush(stderr);
#endif

  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {


#if DEBUG_CLI_extended
    std::cerr << "In Get_Subextents_To_Object_Map,  thread group loop" << std::endl;
    Thread debugThread = *ti;
    //std::cerr << "In Function::getExtentIn, EntrySpy(thread).getEntry()=" << EntrySpy(debugThread).getEntry() << std::endl;
    std::pair<bool, int> prank = ti->getMPIRank();
    int64_t rank = prank.first ? prank.second : -1;
    std::pair<bool, pthread_t> xtid = ti->getPosixThreadId();
    std::cerr << "In Get_Subextents_To_Object_Map,  xtid.first=" << xtid.first << std::endl;
    std::cerr << "In Get_Subextents_To_Object_Map,  xtid.second=" << xtid.second << " rank=" << rank << std::endl;
    printf("In Get_Subextents_To_Object_Map, calling object.getExtentIn(*ti)\n");
    fflush(stderr);
#endif

    ExtentGroup newExtents = object.getExtentIn (*ti);
    Framework::ExtentGroup subextents;

    for (ExtentGroup::iterator ei = newExtents.begin(); ei != newExtents.end(); ei++) {

#if DEBUG_CLI_extended
      Extent Echeck = *ei;
      if (!Echeck.isEmpty()) {
        TimeInterval Etime = Echeck.getTimeInterval();
        AddressRange Eaddr = Echeck.getAddressRange();
        std::cerr << "In Get_Subextents_To_Object_Map, newExtents: Etime (time interval)=" << Etime << " Eaddr (address range)=" << Eaddr << std::endl;
      }

      printf("In Get_Subextents_To_Object_Map, pushing back *ei subextent to subextents\n");
#endif

      subextents.push_back(*ei);

    }
    
    subextents_map[*ti] = subextents;

#if DEBUG_CLI
    printf("Exit Get_Subextents_To_Object_Map, adding subextents to the subextents_map[*ti]\n");
    std::cerr << "Exit Get_Subextents_To_Object_Map,  subextents.size()=" << subextents.size() << std::endl;
    std::cerr << "Exit Get_Subextents_To_Object_Map,  subextents_map.size()=" << subextents_map.size() << std::endl;
#endif
  }

}

inline int64_t stack_contains_N_calls (
    const Framework::StackTrace& st,
    Framework::ExtentGroup& subextents)
{
  Time t = st.getTime();


#if DEBUG_CLI
  printf("Enter stack_contains_N_calls, t.getValue()=%u\n", t.getValue());
#endif

  int64_t num_calls = 0;
  for (ExtentGroup::iterator ei = subextents.begin(); ei != subextents.end(); ei ++) {

    Extent check = *ei;
    if (!check.isEmpty()) {
      TimeInterval time = check.getTimeInterval();
      AddressRange addr = check.getAddressRange();
#if DEBUG_CLI_details
      std::cerr << "In stack_contains_N_calls, subextents: time interval=" << time << " address range =" << addr << std::endl;
#endif

      int64_t sti;
      for (sti = 0; sti < st.size(); sti++) {

        Address a = st[sti];

#if DEBUG_CLI_details
        printf("In stack_contains_N_calls, stack index=%d, a.getValue()=%x\n", sti, a.getValue());
#endif

        if (time.doesContain(t) && addr.doesContain(a)) {
          num_calls++;
#if DEBUG_CLI_details
          printf("In stack_contains_N_calls, if time contains t and addr contains a then bump num_calls=%d\n", num_calls);
         std::cerr << "In stack_contains_N_calls, bump addr: address range=" << addr << std::endl;
#endif
       }
      }
    }
  }

#if DEBUG_CLI
  printf("EXIT stack_contains_N_calls, return num_calls=%d\n", num_calls);
#endif

  return num_calls;
}

inline void Dump_StackTrace (const Framework::StackTrace& st) {
  std::cerr << "StackTrace:\n";
  int64_t len = st.size();
  for (int64_t i = 0;  i < len; i++) {
    std::pair<bool, Function> fp = st.getFunctionAt(i);
    if (fp.first) {
      std::cerr << "  " << fp.second.getName() << "\n";;
    }
  }
}

inline void Dump_CallStack (std::vector<CommandResult *> *call_stack,
                            std::vector<CommandResult *> *vcs) {
  std::cerr << "CallStack: ";
  for (int64_t i = 0; i < vcs->size(); i++) {
    CommandResult *p = (*vcs)[i];
    if (p != NULL) {
      p->Print(std::cerr);
    }
  }
  std::cerr << "\n";

  int64_t len = call_stack->size();
  for (int64_t j = 0; j < len; j++) {
    CommandResult *p = (*call_stack)[j];
    if (p != NULL) {
      std::cerr << "  " << j << "  ";
      std::string S = p->Form();
      std::cerr << S << "\n";
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

#define  Accumulate_Stack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG, function_name)    \
{                                                                                \
                                                                                 \
   if (DEBUG_FLAG) {                                                             \
    printf("Enter Accumulate_Stack \n"); 				 	 \
   }                                                                             \
                                                                                 \
  /* If we have already processed this StackTrace, skip it! */                   \
    if (StackTraces_Processed.find(st) != StackTraces_Processed.end()) {         \
      continue;                                                                  \
    }                                						 \
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
    if (DEBUG_FLAG) {                                                            \
      printf("IN Accumulate_Stack, calls_In_stack=%d \n", static_cast<int>(calls_In_stack)); \
    }                                                                            \
                                                                                 \
   /* Use macro to accumulate all the separate samples. */                       \
    get_inclusive_values (details, calls_In_stack, function_name)                \
                                                                                 \
   /* Decide if we accumulate exclusive_time, as well. */                        \
    if (topStack_In_Subextent (st, SubExtents)) {                                \
     /* Bottom of trace is current function.                                     \
        Exclusive_time is the same as inclusive_time.                            \
        Deeper calls must go without exclusive_time. */                          \
      if (DEBUG_FLAG) {                                                          \
        printf("IN Accumulate_Stack, before calling get_exclusive_values, calls_In_stack=%d\n", static_cast<int>(calls_In_stack)); \
      }                                                                          \
                                                                                 \
      get_exclusive_values (details, calls_In_stack)                             \
                                                                                 \
      if (DEBUG_FLAG) {                                                          \
        printf("IN Accumulate_Stack, after calling get_exclusive_values, calls_In_stack=%d\n", static_cast<int>(calls_In_stack)); \
      }                                                                          \
    }                                                                            \
                                                                                 \
   /* Remember that we have now processed this particular StackTrace. */         \
    StackTraces_Processed.insert(st);                                            \
}

#define  Accumulate_CallStack(st, details, StackTraces_Processed, SubExtents_Map, DEBUG_FLAG, function_name)    \
{                                                                                \
										 \
   /* This define is used in the context of a calltree/traceback view request */ \
   /* This define is used in the context of a calltree/traceback view request */ \
   if (DEBUG_FLAG) {                                                             \
    printf("Enter Accumulate_CallStack \n"); 				 	 \
   }                                                                             \
										 \
  /* If we have already processed this StackTrace, skip it! */                   \
    if (StackTraces_Processed.find(st) != StackTraces_Processed.end()) {         \
      continue;                                                                  \
    }                                 					 	 \
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
   if (DEBUG_FLAG) {                                                             \
     printf("IN Accumulate_CallStack, calls_In_stack=%d \n", static_cast<int>(calls_In_stack)); \
   }                                                                             \
                                                                                 \
   /* Use macro to accumulate all the separate samples. */                       \
    get_inclusive_values (details, calls_In_stack, function_name)                \
                                                                                 \
   /* Always record exclusive_time. */                                           \
                                                                                 \
      if (DEBUG_FLAG) {                                                          \
        printf("IN Accumulate_CallStack, before calling get_exclusive_values, calls_In_stack=%d\n", static_cast<int>(calls_In_stack)); \
      }                                                                          \
                                                                                 \
      get_exclusive_values (details, calls_In_stack)                             \
                                                                                 \
      if (DEBUG_FLAG) {                                                          \
        printf("IN Accumulate_CallStack, after calling get_exclusive_values, calls_In_stack=%d\n", static_cast<int>(calls_In_stack)); \
      }                                                                          \
                                                                                 \
   /* Remember that we have now processed this particular StackTrace. */         \
    StackTraces_Processed.insert(st);                                            \
}
