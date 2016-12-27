/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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


#include "SS_Input_Manager.hxx"

//#define DEBUG_CLI 1

#include "Queries.hxx"

// Global View management utilities

template <typename TE>
void GetMetricBySet (CommandObject *cmd,
                     ExperimentObject *exp,
                     ThreadGroup& tgrp,
                     Collector& collector,
                     std::string& metric,
                     std::set<TE>& objects,
                     SmartPtr<std::map<TE, CommandResult *> >& items) {

  std::vector<std::pair<Time,Time> > intervals;
  Parse_Interval_Specification (cmd, exp, intervals);

  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

#if DEBUG_CLI
  printf("GetMetricBySet  - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif

  if( m.isType(typeid(unsigned int)) ) {
    SmartPtr<std::map<TE, uint> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, uint>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, uint> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(uint64_t)) ) {
    SmartPtr<std::map<TE, uint64_t> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, uint64_t>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, uint64_t> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(int)) ) {
    SmartPtr<std::map<TE, int> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, int>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, int> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(int64_t)) ) {
    SmartPtr<std::map<TE, int64_t> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, int64_t>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, int64_t> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(float)) ) {
    SmartPtr<std::map<TE, float> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, float>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, float> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(double)) ) {
    SmartPtr<std::map<TE, double> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, double>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, double> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(std::string)) ) {
    SmartPtr<std::map<TE, std::string> > data;
    GetMetricInThreadGroup (collector, metric, intervals, tgrp, objects, data);
    for(typename std::map<TE, std::string>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, std::string> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  }

#if DEBUG_CLI
  printf("Exit GetMetricBySet  - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif
  return;
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Function>& objects,
                           SmartPtr<std::map<Function, CommandResult *> >& items) {
#if DEBUG_CLI
  printf("Enter GetMetricByObjectSet1 - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif
  GetMetricBySet (cmd, exp, tgrp, collector, metric, objects, items);
#if DEBUG_CLI
  printf("Exit GetMetricByObjectSet1 - SS_View_util.cxx, metric=%s, objects.size()=%d\n", metric.c_str(), objects.size());
#endif
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Statement>& objects,
                           SmartPtr<std::map<Statement, CommandResult *> >& items) {
#if DEBUG_CLI
  printf("Enter GetMetricByObjectSet2 - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif
  GetMetricBySet (cmd, exp, tgrp, collector, metric, objects, items);
#if DEBUG_CLI
  printf("Exit GetMetricByObjectSet2 - SS_View_util.cxx, metric=%s, objects.size()=%d\n", metric.c_str(), objects.size());
#endif
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<LinkedObject>& objects,
                           SmartPtr<std::map<LinkedObject, CommandResult *> >& items) {
#if DEBUG_CLI
  printf("Enter GetMetricByObjectSet3 - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif
  GetMetricBySet (cmd, exp, tgrp, collector, metric, objects, items);
#if DEBUG_CLI
  printf("Exit GetMetricByObjectSet3 - SS_View_util.cxx, metric=%s, objects.size()=%d\n", metric.c_str(), objects.size());
#endif
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Loop>& objects,
                           SmartPtr<std::map<Loop, CommandResult *> >& items) {
#if DEBUG_CLI
  printf("Enter GetMetricByObjectSet4 - SS_View_util.cxx, metric=%s\n", metric.c_str());
#endif
  GetMetricBySet (cmd, exp, tgrp, collector, metric, objects, items);
#if DEBUG_CLI
  printf("Exit GetMetricByObjectSet4 - SS_View_util.cxx, metric=%s, objects.size()=%d\n", metric.c_str(), objects.size());
#endif
}


template <typename TE>
bool GetAllReducedMetrics(
                       CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<TE>& objects,
                       std::vector<SmartPtr<std::map<TE, CommandResult *> > >& Values) {
    Assert (!objects.empty());
    bool thereAreExtraMetrics = false;
    int64_t i;
    int64_t num_vinst = IV.size();

#if DEBUG_CLI
    printf("Enter GetAllReducedMetrics  - SS_View_util.cxx, num_vinst=%d\n", num_vinst);
    printf("Enter GetAllReducedMetrics  - SS_View_util.cxx, Values.size()=%d\n", Values.size());
#endif
   // Get all the metric values.
    for ( i=0; i < Values.size(); i++) {
      Values[i] = Framework::SmartPtr<std::map<TE, CommandResult *> >(
                                  new std::map<TE, CommandResult * >()
                                  );
    }

    for ( i=0; i < num_vinst; i++) {
      ViewInstruction *vinst = IV[i];

      if (vinst->OpCode() == VIEWINST_Define_ByThread_Metric) {
        int64_t reductionIndex = vinst->TR();
        int64_t CM_Index = vinst->TMP1();
        int64_t reductionType  = vinst->TMP2();
#if DEBUG_CLI
        printf("In GetAllReducedMetrics  - SS_View_util.cxx, reductionIndex=%d, ViewReduction_Count=%d, CM_Index=%d\n", reductionIndex, ViewReduction_Count,CM_Index);
#endif
       if (reductionIndex < ViewReduction_Count) {
          Assert ((reductionType == ViewReduction_sum) ||
                  (reductionType == ViewReduction_mean) ||
                  (reductionType == ViewReduction_min) ||
                  (reductionType == ViewReduction_imin) ||
                  (reductionType == ViewReduction_max) ||
                  (reductionType == ViewReduction_imax));
         // Get a by-thread reduced metric.
#if DEBUG_CLI
          printf("In GetAllReducedMetrics  - SS_View_util.cxx, Get a by-thread reduced metric %lld\n",reductionType);
#endif
          if ((reductionType == ViewReduction_min) ||
              (reductionType == ViewReduction_imin) ||
              (reductionType == ViewReduction_max) ||
              (reductionType == ViewReduction_imax) ||
              (reductionType == ViewReduction_mean)) {
            if (Values[ViewReduction_min]->empty()) {  // else already determined so skip and return existing values.
              GetReducedMaxMinIdxAvg (cmd, exp, tgrp, CV[CM_Index], MV[CM_Index], vinst->TMP3(),
                                      objects,
                                      Values[ViewReduction_min],
                                      Values[ViewReduction_imin],
                                      Values[ViewReduction_max],
                                      Values[ViewReduction_imax],
                                      Values[ViewReduction_mean]);
            }
          } else {
           // (reductionType == ViewReduction_sum)
            GetReducedType (cmd, exp, tgrp, CV[CM_Index], MV[CM_Index], objects, reductionType, Values[reductionType]);
          }
        } else {
         // Get a simple metric for the entire tgrp.
#if DEBUG_CLI
          printf("In GetAllReducedMetrics  - SS_View_util.cxx, Get a simple metric for entire thread group\n");
#endif
          GetMetricByObjectSet (cmd, exp, tgrp, CV[CM_Index], MV[CM_Index], objects, Values[reductionIndex]);
        }
        thereAreExtraMetrics = true;
      }

    }

#if DEBUG_CLI
    printf("Exit GetAllReducedMetrics  - SS_View_util.cxx, thereAreExtraMetrics=%d\n", thereAreExtraMetrics);
#endif
    return thereAreExtraMetrics;
}

bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Function>& objects,
                        std::vector<SmartPtr<std::map<Function, CommandResult *> > >& Values) {
#if DEBUG_CLI
  printf("Enter GetReducedMetrics1 %s - SS_View_util.cxx\n",MV[0].c_str());
#endif
  return GetAllReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Values);
}

bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Statement>& objects,
                       std::vector<SmartPtr<std::map<Statement, CommandResult *> > >& Values) {
#if DEBUG_CLI
  printf("Enter GetReducedMetrics2  - SS_View_util.cxx\n");
#endif
  return GetAllReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Values);
}

bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<LinkedObject>& objects,
                       std::vector<SmartPtr<std::map<LinkedObject, CommandResult *> > >& Values) {
#if DEBUG_CLI
  printf("Enter GetReducedMetrics3  - SS_View_util.cxx\n");
#endif
  return GetAllReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Values);
}

bool GetReducedMetrics(CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::vector<Collector>& CV,
                       std::vector<std::string>& MV,
                       std::vector<ViewInstruction *>& IV,
                       std::set<Loop>& objects,
                       std::vector<SmartPtr<std::map<Loop, CommandResult *> > >& Values) {
#if DEBUG_CLI
  printf("Enter GetReducedMetrics4  - SS_View_util.cxx\n");
#endif
  return GetAllReducedMetrics (cmd, exp, tgrp, CV, MV, IV, objects, Values);
}

CommandResult *Init_Collector_Metric (CommandObject *cmd,
                                      Collector collector,
                                      std::string metric) {
  CommandResult *Param_Value = NULL;
  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();
#if DEBUG_CLI
  printf("Enter Init_Collector_Metric  - SS_View_util.cxx, id.c_str()=%s\n", id.c_str());
#endif
  if( m.isType(typeid(unsigned int)) ) {
    Param_Value = new CommandResult_Uint ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, UNSIGNED INT\n");
#endif
  } else if( m.isType(typeid(uint64_t)) ) {
    Param_Value = new CommandResult_Uint ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, UNSIGNED 64 INT\n");
#endif
  } else if( m.isType(typeid(int)) ) {
    Param_Value = new CommandResult_Int ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, INT\n");
#endif
  } else if( m.isType(typeid(int64_t)) ) {
    Param_Value = new CommandResult_Int ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, INT 64\n");
#endif
  } else if( m.isType(typeid(float)) ) {
    Param_Value = new CommandResult_Float ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, FLOAT\n");
#endif
  } else if( m.isType(typeid(double)) ) {
    Param_Value = new CommandResult_Float ();
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, FLOAT64\n");
#endif
  } else if( m.isType(typeid(std::string)) ) {
    Param_Value = new CommandResult_String ("");
#if DEBUG_CLI
    printf("In Init_Collector_Metric  - SS_View_util.cxx, STRING\n");
#endif
  }
#if DEBUG_CLI
  printf("Exit Init_Collector_Metric  - SS_View_util.cxx\n");
#endif
  return Param_Value;
}

template <typename T>
void GetMetricsforThreads (CommandObject *cmd,
                           ThreadGroup tgrp,
                           Collector collector,
                           std::string metric,
                           T& value)
{

 // Time interval covering earliest to latest possible time
  const TimeInterval Forever =
      TimeInterval(Time::TheBeginning(), Time::TheEnd());

 // Define the maximum address range of any thread
  AddressRange range = AddressRange( Address::TheLowest(), Address::TheHighest() );

 // Evalute the metric over this address range
  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
   // Check for asnychonous abort command
    if (cmd->Status() == CMD_ABORTED) {
      return;
    }

    Thread thread = *ti;

    T new_value;
    collector.getMetricValue(metric, thread, range, Forever, new_value);

   // Add this function and its metric value to the map
    value += new_value;
  }

  return;
}

CommandResult *Get_Total_Metric (CommandObject *cmd,
                                 ThreadGroup tgrp,
                                 Collector collector,
                                 std::string metric) {
  CommandResult *Param_Value = NULL;
  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

#if DEBUG_CLI
  printf("Enter Get_Total_Metric  - SS_View_util.cxx, id.c_str()=%s, metric.c_str()=%s \n",id.c_str(), metric.c_str());
  ThreadGroup::iterator ti;
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    std::cerr << " ProcessId in tgrp=" << (*ti).getProcessId() << " HostID in tgrp=" << (*ti).getHost() << std::endl;
    Thread t = *ti;
    std::pair<bool, int> prank = t.getMPIRank();
    std::cerr << " Rank in tgrp=" << prank.second << "\n" <<  std::endl;

  }

#endif

  if( m.isType(typeid(unsigned int)) ) {
    uint Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    uint64_t Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(int)) ) {
    int Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0) Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0) Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value = 0.0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0.0000000001) Param_Value = CRPTR (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value = 0.0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0.0000000001) Param_Value = CRPTR (Value);
  }
#if DEBUG_CLI
  printf("Exit Get_Total_Metric  - SS_View_util.cxx\n");
#endif
  return Param_Value;
}

static bool Remainaing_Length_Is_Numeric (std::string viewname, int64_t L) {
  for (int64_t i = L; i < viewname.length(); i++) {
    char a = viewname[i];
    if ((a < *"0") || (a > *"9")) return false;
  }
  return true;
}

ViewType *Find_View (std::string viewname) {
  std::list<ViewType *>::iterator vi;

 // First, attempt a case sensitive search for the name.
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    if (!strncmp (viewname.c_str(), vt->Unique_Name().c_str(), vt->Unique_Name().length()) &&
        Remainaing_Length_Is_Numeric (viewname, vt->Unique_Name().length())) {
      return vt;
    }
  }

 // if the case sensitive search fails, attempt a case insensitive search for the name.
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    if (!strncasecmp (viewname.c_str(), vt->Unique_Name().c_str(), vt->Unique_Name().length()) &&
        Remainaing_Length_Is_Numeric (viewname, vt->Unique_Name().length())) {
      return vt;
    }
  }

  return NULL;
}

void Define_New_View (ViewType *vnew) {
  ViewType *existing = Find_View (vnew->Unique_Name());
  if (existing != NULL) {
    if (vnew->Unique_Name() == existing->Unique_Name()) return;
    std::cerr << "WARNING: Definition of View named "
         << vnew->Unique_Name().c_str()
         << " may hide the existing definition of "
         << existing->Unique_Name().c_str() << std::endl;
  }
  Available_Views.push_front (vnew);
}

void Add_Column_Headers (CommandResult_Headers *H, std::string *column_titles)
{
  int64_t i = 0;
  while (column_titles[i].length() != 0) {
    CommandResult *T = CRPTR ( column_titles[i] );
    H->CommandResult_Headers::Add_Header (T);
    i++;
  }
}

void Add_Header (CommandObject *cmd, std::string *column_titles)
{
  CommandResult_Headers *H = new CommandResult_Headers ();
  Add_Column_Headers (H, column_titles);
  cmd->Result_Predefined (H);
}

bool Collector_Generates_Metric (Collector C, std::string Metric_Name) {
 // Does a given collector produce a specific metric?
  std::set<Metadata> md = C.getMetrics();
  std::set<Metadata>::const_iterator mi;

 // Check that all the required metrics are in this collector.
  for (mi = md.begin(); mi != md.end(); mi++) {
    Metadata m = *mi;
    if (!strcasecmp(Metric_Name.c_str(), m.getUniqueId().c_str())) {
     // Match succeeds!
      return true;
    }
  }

  return false;
}

std::string Find_Collector_With_Metric (CollectorGroup cgrp,
                                        std::string Metric_Name) {
#if DEBUG_CLI
  printf("Find_Collector_With_Metric  - SS_View_util.cxx, Metric_Name=%s\n", Metric_Name.c_str());
#endif
 // Look for all elements of the list of metrics in one of the collectors in the group.
  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
   // Return the first collector that generates the requested metric.
    Collector C = *ci;
    try {
      if (Collector_Generates_Metric ( C, Metric_Name )) {
       // Found a collector that generates the metric!
        return C.getMetadata().getUniqueId();
      }
    }
    catch(const Exception& error) {
     // Guess not.
      continue;
    }
  }

 // Failed to find a collector that generated the metric.
  return std::string("");
}

bool Collector_Generates_Metrics (Collector C, std::string *Metric_List) {
 // Does a given collector produce all the metrics in the list?
  std::set<Metadata> md = C.getMetrics();
  std::set<Metadata>::const_iterator mi;

 // Check that all the required metrics are in this collector.
  int i = 0;
  while (Metric_List[i].length() > 0) {
    bool metric_found = false;
    std::string rm = Metric_List[i];
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      if (!strcasecmp(rm.c_str(), m.getUniqueId().c_str())) {
       // Match succeeds!
       // Exit loop and check next required metric.
        metric_found = true;
        break;
      }
    }

    if (!metric_found) {
      return false;
    }
    i++;
  }

  return true;
}

std::string Find_Collector_With_Metrics (CollectorGroup cgrp,
                                         std::string *Metric_List) {
 // Look for all elements of the list of metrics in one of the collectors in the group.
  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
   // See if all the required metrics are in this collector.
    Collector C = *ci;
    try {
      if (Collector_Generates_Metrics ( C, &Metric_List[0] )) {
       // Found a collector that generates all required metrics!
        return C.getMetadata().getUniqueId();
      }
    }
    catch(const Exception& error) {
     // Guess not.
      continue;
    }
  }

 // Failed to find a collector that generated all the required metrics.
  return std::string("");
}

bool Metadata_hasName (Collector C, std::string name) {
  try {
    std::set<Metadata> M = C.getMetrics();
    std::set<Metadata>::const_iterator mi;
    for (mi = M.begin(); mi != M.end(); mi++) {
      Metadata m = *mi;
      if (m.getUniqueId() == name) {
       // The one we want!
        return true;
      }
    }
  }
  catch(const Exception& error) {
   // Ignore problems - the calling routine can figure something out.
  }

 // We didn't find the Meta-Name in this set.
  return false;
}

Metadata Find_Metadata (Collector C, std::string name) {
  try {
    std::set<Metadata> M = C.getMetrics();
    std::set<Metadata>::const_iterator mi;
    for (mi = M.begin(); mi != M.end(); mi++) {
      Metadata m = *mi;
      if (m.getUniqueId() == name) {
       // The one we want!
        return m;
      }
    }
  }
  catch(const Exception& error) {
   // Ignore problems - the calling routine can figure something out.
  }

 // Shouldn't ever be here, if Metadata_hasName was called first.
  Metadata m;
  return m;
}

std::string gen_F_name (Function F) {
#if DEBUG_CLI
  printf("gen_F_name  - SS_View_util.cxx, FUNCTION\n");
#endif
  std::string S = F.getName();
  LinkedObject L = F.getLinkedObject();
  std::set<Statement> T = F.getDefinitions();

  S = S + "(" + L.getPath().getBaseName();

  if (T.size() > 0) {
    std::set<Statement>::const_iterator ti;
    for (ti = T.begin(); ti != T.end(); ti++) {
      if (ti != T.begin()) {
        S += "  &...";
        break;
      }
      Statement s = *ti;
      char l[20];
      sprintf( &l[0], "%lld", (int64_t)s.getLine());
      S = S + ": " + s.getPath().getBaseName() + "," + &l[0];
    }
  }
  S += ")";

  return S;
}


// Utilities for processing '-f' specifier on expView commands.

static inline bool Is_Full_Path_Name (std::string F_Name) {
 // If the name starts with "/" or contains "*" it is a full path names.
  if (F_Name.size() != 0) {
    int64_t len = F_Name.size();
    if (F_Name[0] == *"/") {
      return true;
    }
    int64_t pos = F_Name.find( "*" );
    if ((pos >= 0) && (pos < len)) {
      return true;
    }
  }
  return false; 
}

static inline bool Is_Path_Name (std::string F_Name) {
 // If the name contains a "/" or "." we need to compare path names.
  if (F_Name.size() != 0) {
    int64_t len = F_Name.size();
    int64_t pos = F_Name.find( "/" );
    if ((pos >= 0) && (pos < len)) {
      return true;
    } else {
      int64_t dot = F_Name.find( "." );
      if ((pos >= 0) && (dot < len)) {
        return true;
      }
    }
  }
  return false;
}

static void Merge_LinkedObject_Into_Objects (std::set<LinkedObject>& named_linkedobjects,
                                             std::set<LinkedObject>& objects) {
  if (!named_linkedobjects.empty()) {
    objects.insert (named_linkedobjects.begin(), named_linkedobjects.end());
  }
}

static void Merge_LinkedObject_Into_Objects (std::set<LinkedObject>& named_linkedobjects,
                                             std::set<Statement>& objects) {
 // This routine should never be called.
}

static void Merge_LinkedObject_Into_Objects (std::set<LinkedObject>& named_linkedobjects,
                                             std::set<Loop>& objects) {
 // This routine should never be called.
}

static void Merge_LinkedObject_Into_Objects (std::set<LinkedObject>& named_linkedobjects,
                                             std::set<Function>& objects) {
 // This routine should never be called.
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<LinkedObject>& objects) {
#if DEBUG_CLI
  printf("Get_Objects_By_Function  - SS_View_util.cxx, LINKEDOBJECTS\n");
#endif
  std::set<Function>::iterator fi;
  for (fi = named_functions.begin(); fi != named_functions.end(); fi++) {
    Function f = *fi;
    LinkedObject l = f.getLinkedObject();
    objects.insert (l);
  }
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<Loop>& objects) {
#if DEBUG_CLI
  printf("Get_Objects_By_Function  - SS_View_util.cxx, LOOPS\n");
#endif
  std::set<Function>::iterator fi;
  for (fi = named_functions.begin(); fi != named_functions.end(); fi++) {
    Function f = *fi;
    std::set<Loop> l = f.getLoops();
    if (!l.empty()) {
      objects.insert (l.begin(), l.end());
    }
  }
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<Statement>& objects) {
#if DEBUG_CLI
  printf("Get_Objects_By_Function  - SS_View_util.cxx, STATEMENT\n");
#endif
  std::set<Function>::iterator fi;
  for (fi = named_functions.begin(); fi != named_functions.end(); fi++) {
    Function f = *fi;
    std::set<Statement> s = f.getStatements();
    if (!s.empty()) {
      objects.insert (s.begin(), s.end());
    }
  }
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<Function>& objects) {
  if (!named_functions.empty()) {
    objects.insert (named_functions.begin(), named_functions.end());
  }
}

static void Get_Source_Objects(const Thread& thread,
			       std::set<LinkedObject>& objects)
{
    objects = thread.getLinkedObjects();
}

static void Get_Source_Objects(const Thread& thread,
			       std::set<Function>& objects)
{
    objects = thread.getFunctions();
}

static void Get_Source_Objects(const Thread& thread,
			       std::set<Statement>& objects)
{
    objects = thread.getStatements();
}

static void Get_Source_Objects(const Thread& thread,
			       std::set<Loop>& objects)
{
#if DEBUG_CLI
    printf("Enter Get_Source_Objects-SS_View_util.cxx, LOOP, objects.size()=%d\n", objects.size());
#endif
    objects = thread.getLoops();
#if DEBUG_CLI
    printf("Exit Get_Source_Objects-SS_View_util.cxx, LOOP, objects.size()=%d\n", objects.size());
#endif
}

static void Get_TGRP_Source_Objects(const ThreadGroup& tgrp,
			       std::set<LinkedObject>& objects)
{
    objects = tgrp.getLinkedObjects();
}

static void Get_TGRP_Source_Objects(const ThreadGroup& tgrp,
			       std::set<Function>& objects)
{
    objects = tgrp.getFunctions();
}

static void Get_TGRP_Source_Objects(const ThreadGroup& tgrp,
			       std::set<Statement>& objects)
{
    objects = tgrp.getStatements();
}


static void Get_TGRP_Source_Objects(const ThreadGroup& tgrp,
			       std::set<Loop>& objects)
{
#if DEBUG_CLI
    printf("Enter Get_TGRP_Source_Objects-SS_View_util.cxx, LOOP, objects.size()=%d\n", objects.size());
#endif
    objects = tgrp.getLoops();
#if DEBUG_CLI
    printf("Exit Get_TGRP_Source_Objects-SS_View_util.cxx, LOOP, objects.size()=%d\n", objects.size());
#endif
}


static inline bool Object_Is_LinkedObject (std::set<LinkedObject>& objects) { return true; }
static inline bool Object_Is_LinkedObject (std::set<Function>& objects) { return false; }
static inline bool Object_Is_LinkedObject (std::set<Statement>& objects) { return false; }
static inline bool Object_Is_LinkedObject (std::set<Loop>& objects) { return false; }

/**
 * Template: Filtered_Objects
 *
 * Determine the set of objects that needs to be processed
 * after applying the "-f" filtering rules.
 *
 * The semantic rules are:
 *
 * 1) If there is no "-f" specification, call Queries::GetSourceObjects
 *    to do all the work.
 * 2) Otherwise, go through the list of "-f" strings, in the parse object,
 *    and for each one, look for:
 *        a) LinkedObjects with that name,
 *        b) Files with that name, and
 *        c) Functions with that name.
 *    For all the found items, Add all the associated objects of the result
 *    type to the result set.
 * 3) If step 2) doesn't find a match with at least one search and the string
 *    does not specify a full path name (i.e. starts with a "/" or contains
 *    a "*"), try the setp 2) search again with a wildcard character (i.e. "*")
 *    at the end of the string.
 *
 * @param CommandObject *cmd to access the parse objects.
 * @param ExperimentObject *exp to access the experiment
 * @param ThreadGroup& tgrp to define the set of Threads that is
 *        the result of a call to 'Filter_ThreadGroup' which will
 *        have applied all the other filtering rules for the
 *        <target> specification.
 * @param std::set<TE >& objects the result is returned here.
 *
 * @return through the last param.
 *
 */
template <typename TE>
void Filtered_Objects (CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::set<TE >& objects ) {

#if DEBUG_CLI
  printf("Filtered_Objects  - SS_View_util.cxx, objects.size()=%d\n", objects.size());
#endif
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  std::vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  std::vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;

  if (p_tlist->begin() != p_tlist->end()) {
    // There is a list.  Is there a "-f" specifier?
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();
  }

  if ((f_list == NULL) || (f_list->empty())) {
   // There is no Function filtering requested.
   // Get all the objects in the already selected thread groups.
   // Updated to use the threadgroup directly for the getFunctions,
   // getStatements, or getLinkedObjects used here.
   // For large parallel jobs this improves the performance of
   // the underlying database queries.
    std::set<TE> new_objects;
    Get_TGRP_Source_Objects(tgrp, new_objects);
    objects.insert(new_objects.begin(), new_objects.end());
#if DEBUG_CLI
    printf("EXIT 1 Filtered_Objects  - SS_View_util.cxx, objects.size()=%d\n", objects.size());
#endif
    return;
  } else if (!Object_Is_LinkedObject(objects)) {
   // There is some sort of "-f name" filter specified.
   // We get here because we want Function or Statement objects.
   // Determine the names of desired objects and get Function objects for them.
   // Then get the objects that we want for these functions.
    std::set<Function> new_functions;
    std::vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
    for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
      OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
      Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);

      std::set<Function> new_f;
      std::string F_Name = pval1.name;
      std::string Wild_Name = Is_Full_Path_Name(F_Name) ? F_Name : "*" + F_Name;
      bool found_something = false;

     // First, find any LinkedObject names that match.
      std::set<LinkedObject> new_LinkedObjects = exp->FW()->getLinkedObjectsByPathPattern (Wild_Name);
      if (!new_LinkedObjects.empty()) {
        std::set<LinkedObject>::iterator loi;
        for (loi = new_LinkedObjects.begin(); loi != new_LinkedObjects.end(); loi++) {
          new_f = (*loi).getFunctions();
          if (!new_f.empty()) {
            found_something = true;
            new_functions.insert(new_f.begin(), new_f.end());
          }
        }
      }

     // Second, find any files that match.
      new_f = exp->FW()->getFunctionsByPathPattern (Wild_Name);
      if (!new_f.empty()) {
        found_something = true;
        new_functions.insert(new_f.begin(), new_f.end());
      }

     // Third, the name might also be a function name.
      if (!Is_Path_Name(F_Name)) {
        new_f = exp->FW()->getFunctionsByNamePattern (F_Name);
        if (!new_f.empty()) {
          found_something = true;
          new_functions.insert(new_f.begin(), new_f.end());
        }
      }

     // Final attempt by appending a wildcard to the function name.
      if (!found_something &&
          !Is_Path_Name(F_Name) &&
          (F_Name[F_Name.size()-1] != *"*")) {
        new_f.empty();
        Wild_Name = Wild_Name + "*";

       // Look for LinkedObject names.
        std::set<LinkedObject> new_LinkedObjects = exp->FW()->getLinkedObjectsByPathPattern (Wild_Name);
        if (!new_LinkedObjects.empty()) {
          std::set<LinkedObject>::iterator loi;
          for (loi = new_LinkedObjects.begin(); loi != new_LinkedObjects.end(); loi++) {
            new_f = (*loi).getFunctions();
            if (!new_f.empty()) {
              found_something = true;
              new_functions.insert(new_f.begin(), new_f.end());
            }
          }
        }

       // Look for file names.
        new_f = exp->FW()->getFunctionsByPathPattern (Wild_Name);
        if (!new_f.empty()) {
          found_something = true;
          new_functions.insert(new_f.begin(), new_f.end());
        }

       // Look for function names.
        new_f = exp->FW()->getFunctionsByNamePattern (F_Name + "*");
        if (!new_f.empty()) {
          found_something = true;
          new_functions.insert(new_f.begin(), new_f.end());
        }
      }
    }

    if (!new_functions.empty()) {
     // There maybe too many functions in the list.
     // Restrict the set to ones in the desired ThreadGroup.

     // First, build a set of all the LinkedObjects in the desired Threads.
      std::set<LinkedObject> allowed_L;
      for (ThreadGroup::const_iterator tgi = tgrp.begin(); tgi != tgrp.end(); ++tgi) {
        Thread T = *tgi;
        std::set<LinkedObject> Ls = T.getLinkedObjects();
        if (!Ls.empty()) {
          allowed_L.insert(Ls.begin(), Ls.end());
        }
      }

     // Then, if one of the new functions isn't in the desired LinkedObject set, toss it.
      for (std::set<Function>::iterator fi = new_functions.begin(); fi != new_functions.end(); fi++) {
        Function F = *fi;
        LinkedObject L = F.getLinkedObject();
        if (allowed_L.find(L) == allowed_L.end()) {
          new_functions.erase(F);
        }
      }

     // Get the desired function, statement, or loop objects.
      Get_Objects_By_Function (new_functions, objects );
    }
    return;
  }

 // There is some sort of "-f name" filter specified.
 // We get here because we want LinkedObjects.
    std::set<LinkedObject> new_objects;
    std::vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
    for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
      OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
      Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);

      std::set<Function> new_f;
      std::string F_Name = pval1.name;

#if DEBUG_CLI
      printf("In Filtered_Objects, found -f argument, named=%s\n", F_Name.c_str());
#endif

      std::string Wild_Name = Is_Full_Path_Name(F_Name) ? F_Name : "*" + F_Name;
      bool found_something = false;

     // First, find any LinkedObject names that match.
      std::set<LinkedObject> new_lo = exp->FW()->getLinkedObjectsByPathPattern (Wild_Name);
      if (!new_lo.empty()) {
        found_something = true;
        new_objects.insert(new_lo.begin(), new_lo.end());
#if DEBUG_CLI
        printf("In Filtered_Objects, found and saved to new_objects a LinkedObject that matched name=%s\n", F_Name.c_str());
#endif
      }

     // Second, find any files that match and add their LinkedObject to the set.
      new_f = exp->FW()->getFunctionsByPathPattern (Wild_Name);
      if (!new_f.empty()) {
        for (std::set<Function>::iterator fi = new_f.begin(); fi != new_f.end(); fi++) {
          Function F = *fi;
          LinkedObject L = F.getLinkedObject();
          new_objects.insert(L);
        }
      }

     // Third, the name might also be a function name.
     // Find those functions andd add their LinkedObject to the set.
      if (!Is_Path_Name(F_Name)) {
        new_f = exp->FW()->getFunctionsByNamePattern (F_Name);
        if (!new_f.empty()) {
          for (std::set<Function>::iterator fi = new_f.begin(); fi != new_f.end(); fi++) {
            Function F = *fi;
            LinkedObject L = F.getLinkedObject();
            new_objects.insert(L);
#if DEBUG_CLI
            printf("In Filtered_Objects, found function name and saved to new_objects a LinkedObject that matched name=%s\n", F_Name.c_str());
#endif
          }
        }
      }

     // Final attempt by appending a wildcard to the function name.
      if (!found_something &&
          !Is_Path_Name(F_Name) &&
          (F_Name[F_Name.size()-1] != *"*")) {
        new_f.empty();
        Wild_Name = Wild_Name + "*";

       // Look for LinkedObject names.
        std::set<LinkedObject> new_lo = exp->FW()->getLinkedObjectsByPathPattern (Wild_Name);
        if (!new_lo.empty()) {
          found_something = true;
          new_objects.insert(new_lo.begin(), new_lo.end());
#if DEBUG_CLI
          printf("In Filtered_Objects, found wild card searched function name and saved to new_objects a LinkedObject that matched name=%s\n", F_Name.c_str());
#endif
        }

       // Look for file names.
        new_f = exp->FW()->getFunctionsByPathPattern (Wild_Name);
        if (!new_f.empty()) {
          for (std::set<Function>::iterator fi = new_f.begin(); fi != new_f.end(); fi++) {
            Function F = *fi;
            LinkedObject L = F.getLinkedObject();
            new_objects.insert(L);
#if DEBUG_CLI
          printf("In Filtered_Objects, found wild card searched filename and saved to new_objects a LinkedObject that matched name=%s\n", F_Name.c_str());
#endif
          }
        }

       // Look for function names.
        if (!Is_Path_Name(F_Name)) {
          new_f = exp->FW()->getFunctionsByNamePattern (F_Name + "*");
          if (!new_f.empty()) {
            for (std::set<Function>::iterator fi = new_f.begin(); fi != new_f.end(); fi++) {
              Function F = *fi;
              LinkedObject L = F.getLinkedObject();
              new_objects.insert(L);
#if DEBUG_CLI
              printf("In Filtered_Objects, found wild card searched function names and saved to new_objects a LinkedObject that matched name=%s\n", F_Name.c_str());
#endif
            }
          }
        }
      }
    }

    if (!new_objects.empty()) {
     // There maybe too many functions in the list.
     // Restrict the set to ones in the desired ThreadGroup.

     // First, build a set of all the LinkedObjects in the desired Threads.
      std::set<LinkedObject> allowed_L;
      for (ThreadGroup::const_iterator tgi = tgrp.begin(); tgi != tgrp.end(); ++tgi) {
        Thread T = *tgi;
        std::set<LinkedObject> Ls = T.getLinkedObjects();
        if (!Ls.empty()) {
          allowed_L.insert(Ls.begin(), Ls.end());
        }
      }

     // Then, if one of the new LinkedObjects isn't in the desired LinkedObject set, toss it.
      for (std::set<LinkedObject>::iterator li = new_objects.begin(); li != new_objects.end(); li++) {
        LinkedObject L = *li;
        if (allowed_L.find(L) == allowed_L.end()) {
          new_objects.erase(L);
        }
      }

     // The resulting set is the result!
      Merge_LinkedObject_Into_Objects (new_objects, objects);
#if DEBUG_CLI
      printf("In Filtered_Objects, merged the new_objects with the objects as the result\n");
#endif
    }
#if DEBUG_CLI
    printf("Exit Filtered_Objects\n");
#endif
    return;
}

/**
 * Utility: Get_Filtered_Objects
 *
 * Overloaded function for types: LinkedObject, Function, Statement or Loop.
 *
 * The call to these routines is made after determining which type
 * is needed for a view that is being generated, and after first calling
 * 'Filter_ThreadGroup' to handle all the other components of the <target>
 * specification.
 *
 * These routines just call the template 'Filtered_Objects' to complete
 * the semantic filtering rules for the "-f" component of a <target>
 * specification.
 *
 */
void Get_Filtered_Objects (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           std::set<LinkedObject>& objects ) {
  Filtered_Objects ( cmd, exp, tgrp, objects);
}

void Get_Filtered_Objects (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           std::set<Function>& objects ) {
  Filtered_Objects ( cmd, exp, tgrp, objects);
}

void Get_Filtered_Objects (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           std::set<Statement>& objects ) {
  Filtered_Objects ( cmd, exp, tgrp, objects);
}

void Get_Filtered_Objects (CommandObject *cmd,
                           ExperimentObject *exp,
                           ThreadGroup& tgrp,
                           std::set<Loop>& objects ) {
  Filtered_Objects ( cmd, exp, tgrp, objects);
}

// Utilites to deciding what data to retrieve from a database.

/**
 * Utility: Validate_V_Options(CommandObject *cmd, std::string allowed[])
 *
 * Check that every option listed after a '-v' specifier is valid.
 * The option is valid if it is within the 'allowed' list.
 *
 * @param   cmd indicates the CommandObject that points to the parse
 *          object, that lists the options.   
 * @param   allowed[] is a null terminated array of <std::string>
 *          that must contain all supported '-v' options for the view.
 *
 * @return  'true' is all options are valid; 'false' if one or more
 *          option is invalid.  It is up to the calling routine to
 *          decide what to do with this result.
 *
 *          A 'Warning' message is attaached to the command if an
 *          invalid option is found.
 *
 */
bool Validate_V_Options (CommandObject *cmd,
                         std::string allowed[]) {
  bool all_valid = true;
  Assert (cmd->P_Result() != NULL);

 // Look at general modifier types for a specific KeyWord option.
  std::vector<std::string> *p_slist = cmd->P_Result()->getModifierList();
  std::vector<std::string>::iterator j;

  for (j=p_slist->begin();j != p_slist->end(); j++) {
    std::string S = *j;
    if (S.length() != 0) {
      int64_t i = 0;

      bool match_found = false;
      while (allowed[i].length() != 0) {
        if (!strcasecmp (S.c_str(), allowed[i].c_str()) ) {
          match_found = true;
          break;
        }
        i++;
      }

      if (!match_found) {
        all_valid = false;
        Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported option, '-v " + S + "'");
      }

    }
  }

  return all_valid;
}


/**
 * Utility: Determine_Form_Category(CommandObject *cmd)
 * 
 * Look at the standard set of '-v' options for a 'view' command
 * and determine which type fof report needs to be generated.
 * 
 * @param   cmd indicates the CommandObject that points to the parse
 *          object, that lists the options.   
 *
 * @return  the View_Form_Category enumerator of the required report type.
 *
 */
View_Form_Category Determine_Form_Category (CommandObject *cmd) {
  if (Look_For_KeyWord(cmd, "Trace") &&
      !Look_For_KeyWord(cmd, "ButterFly")) {
    return VFC_Trace;
  } else if (Look_For_KeyWord(cmd, "CallTree") ||
             Look_For_KeyWord(cmd, "CallTrees") ||
             Look_For_KeyWord(cmd, "TraceBack") ||
             Look_For_KeyWord(cmd, "TraceBacks") ||
             Look_For_KeyWord(cmd, "FullStack") ||
             Look_For_KeyWord(cmd, "FullStacks") ||
             Look_For_KeyWord(cmd, "ButterFly")) {
    return VFC_CallStack;
  } else if (Look_For_KeyWord(cmd, "LinkedObject") ||
             Look_For_KeyWord(cmd, "LinkedObjects") ||
             Look_For_KeyWord(cmd, "Dso") ||
             Look_For_KeyWord(cmd, "Dsos")) {
    return VFC_LinkedObject;
  } else if (Look_For_KeyWord(cmd, "Statement") ||
             Look_For_KeyWord(cmd, "Statements")) {
    return VFC_Statement;
  } else if (Look_For_KeyWord(cmd, "Loop") ||
             Look_For_KeyWord(cmd, "Loops")) {
    return VFC_Loop;
  }
  return VFC_Function;
}

/**
 * Utility: Determine_TraceBack_Ordering (CommandObject *cmd)
 *
 * Look at the standard set of '-v' options for a 'view' command
 * and determine which order the calling stack needs to be presented
 * to the user.
 *
 * @param   cmd indicates the CommandObject that points to the parse
 *          object, that lists the options.
 *
 * @return  'true' if the requested order is with the root routine first
 *          followed by the routiens that called it, or 'false' if the
 *          the program entry routine is first followed by the routines
 *          it calls until the root routine is encoutnered.
 *
 */
bool Determine_TraceBack_Ordering (CommandObject *cmd) {
 // Determine call stack ordering
#if DEBUG_CLI
  printf("Determine_TraceBack_Ordering  - SS_View_util.cxx %p\n",cmd);
#endif
  if (Look_For_KeyWord(cmd, "CallTree") ||
      Look_For_KeyWord(cmd, "CallTrees")) {
      return false;
  } else if (Look_For_KeyWord(cmd, "TraceBack") ||
             Look_For_KeyWord(cmd, "TraceBacks")) {
    return true;
  }
  return false;
}


/**
 * Utility: Determine_ByThread_Id(ExperimentObject *exp, CommandObj *cmd)
 * 
 * Look at the threads in an experiment and determine the
 * identifier that varies between threads and, as such,
 * is the simplest way to identify a thread.
 * 
 * @param   exp indicates the ExperimentObject.
 *
 * @return  the int_64_t that is one of the View_ByThread_* values.
 *
 */
std::string View_ByThread_Id_name[5] = {
       "??",
       "Rank",
       "ThreadId",
       "Posix ThreadId",
       "ProccessId" }; 
int64_t Determine_ByThread_Id (ExperimentObject *exp, CommandObject *cmd) {

  int thread_count = 0;
  int rank_count = 0;
  int prev_rank = -1;

  ThreadGroup tgrp = exp->FW()->getThreads();
  Filter_ThreadGroup (cmd->P_Result(), tgrp);
  bool skip_first = false;
  bool first_thread_has_no_rank = false;

#if DEBUG_CLI
  printf("Enter Determine_ByThread_Id - SS_View_util.cxx\n");
#endif

  // Do a loop prior to the sorting so we know if we have the case where we have a non-rank
  // thread followed by threads with rank information.  This is the case with the online mpi
  // experiment databases.  See comments below.

  bool first_time = false;
  ThreadGroup::iterator dti;
  for (dti = tgrp.begin(); dti != tgrp.end(); dti++) {

#if DEBUG_CLI
    std::cerr << " Determine_ByThread_Id, ProcessId in tgrp=" << (*dti).getProcessId() << " HostID in tgrp=" << (*dti).getHost() << std::endl;
#endif

    // Count ranks, but if the rank shows up multiple times, such as in hybrid MPI and openMP codes
    // only count the rank once.  We see -r 0 -t thread1, -r 0 -t thread2, etc. so we need to only count 
    // that as one rank.
    Thread t = *dti;
    std::pair<bool, int> prank = t.getMPIRank();
    thread_count = thread_count + 1;
    if (prank.first) {
#if DEBUG_CLI
      std::cerr << " Rank in tgrp=" << prank.second << "\n" <<  std::endl;
#endif
      if (!first_time) {
         if (prev_rank != prank.second) {
            rank_count = rank_count + 1;
            prev_rank = prank.second;
         } else {  
            // skipping adding rank because it is the same as previous
         } 

      } else {
         prev_rank = prank.second;
         rank_count = 1;
      } 

    } else {
      if (thread_count == 1) first_thread_has_no_rank = true;

#if DEBUG_CLI
      std::cerr << " NO Rank in tgrp=" << "\n" <<  std::endl;
#endif

    }
  }

  // There are cases where there are multiple threads generated for each rank
  // We account for these by the 2,3,4 times the rank count.  The difference should
  // always be one.  This indicates the one thread for the mpirun, orterun, srun, or other
  // mpi start up process.
  if (thread_count > 2 && 
     ( (thread_count-rank_count == 1) || 
       ((thread_count-(2*rank_count)) == 1) ||
       ((thread_count-(3*rank_count)) == 1) ||
       ((thread_count-(4*rank_count)) == 1)
     ) &&
      first_thread_has_no_rank ) {
     skip_first = true;
  }
#if DEBUG_CLI
  std::cerr << " Determine_ByThread_Id, thread_count=" << thread_count << " rank_count=" << rank_count << std::endl;
  std::cerr << " Determine_ByThread_Id, thread_count-rank_count=" << thread_count-rank_count << " (thread_count-(2*rank_count))=" << (thread_count-(2*rank_count)) << std::endl;
  std::cerr << " Determine_ByThread_Id, (thread_count-(3*rank_count))=" << (thread_count-(3*rank_count)) << " (thread_count-(4*rank_count))=" << (thread_count-(4*rank_count)) << std::endl;
  std::cerr << " Determine_ByThread_Id, skip_first=" << skip_first << " first_thread_has_no_rank=" << first_thread_has_no_rank << std::endl;
#endif


  ThreadGroup::iterator ti = tgrp.begin();
  // For online we have a thread for the orterun, mpirun process which does not have a rank
  // but all the other threads in the thread group do and we want to return ranks in that situation.
  // So, we will skip the first thread (orterun or mpirun, etc.) and look at the threads that follow
  // as long as there is only 1 thread without rank information and more than 2 total threads.
  if (skip_first) ti = ++ti;
  if (ti != tgrp.end()) {
    Thread t1 = *ti;
    if ((++ti) != tgrp.end()) {
     // Nothing will vary unless there are at least 2 threads.
      Thread t2 = *ti;
      std::pair<bool, int> prank1 = t1.getMPIRank();
      std::pair<bool, int> prank2 = t2.getMPIRank();
#if DEBUG_CLI
    std::cerr << " Determine_ByThread_Id, In rank code, ProcessId in tgrp=" << (*ti).getProcessId() << " HostID in tgrp=" << (*ti).getHost() << std::endl;
    std::cerr << " Determine_ByThread_Id, In rank code, prank1.first=" << prank1.first << " prank2.first=" << prank2.first << std::endl;

    if (prank1.first) {
       std::cerr << " Determine_ByThread_Id, In rank code, prank1.second=" << prank1.second << std::endl;
    }
    if (prank2.first) {
       std::cerr << " Determine_ByThread_Id, In rank code, prank2.second=" << prank2.second << std::endl;
    }
#endif

#if 1
      // If there multiple ranks then use RANKS to distinguish
      if ( rank_count > 1) {
       // Use Ranks to distinguish threads.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, multiple ranks, RETURN View_ByThread_Rank=" << View_ByThread_Rank << std::endl;
#endif
        return View_ByThread_Rank;
      }

#else
      // If there is a rank for the top two entries and they are not the same rank
      if ( prank1.first && prank2.first && (prank1.second != prank2.second) ) {
       // Use Ranks to distinguish threads.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, RETURN View_ByThread_Rank=" << View_ByThread_Rank << std::endl;
#endif
        return View_ByThread_Rank;
      }
#endif

#if DEBUG_CLI
      printf("In Determine_ByThread_Id - SS_View_util.cxx, fall past the RANK CODE INTO THREAD CODE\n");
//      std::cerr << " Determine_ByThread_Id, fall past rank code, t1.getPosixThreadId()=" << t1.getPosixThreadId() << " t2.getPosixThreadId()=" << t2.getPosixThreadId() << std::endl;
#endif

      // NOTE: Always prefer this openmp thread id... therefore is should
      // always preceed check for posix thread id.
      std::pair<bool, int> pthread1 = t1.getOpenMPThreadId();
      std::pair<bool, int> pthread2 = t2.getOpenMPThreadId();
      if ( pthread1.first && pthread2.first &&
           (pthread1.second != pthread2.second) ) {
       // Use Thread ids.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, RETURN View_ByThread_OpenMPThread=" << View_ByThread_OpenMPThread << std::endl;
#endif
        return View_ByThread_OpenMPThread;
      }

      // NOTE: In reality we really never want to use posix thread ids as an id.
      std::pair<bool, pthread_t> posixthread1 = t1.getPosixThreadId();
      std::pair<bool, pthread_t> posixthread2 = t2.getPosixThreadId();
      if ( posixthread1.first && posixthread2.first &&
           (posixthread1.second != posixthread2.second) ) {
       // Use Thread ids.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, RETURN View_ByThread_PosixThread=" << View_ByThread_PosixThread << std::endl;
#endif
        return View_ByThread_PosixThread;
      }

      pid_t pid1 = t1.getProcessId();
      pid_t pid2 = t2.getProcessId();
      if (pid1 != pid2) {
       // Use Process ids.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, RETURN View_ByThread_Process=" << View_ByThread_Process << std::endl;
#endif
        return View_ByThread_Process;
      }
    } else {
     // There is only 1 thread - pick a field for the ID.
      std::pair<bool, int> prank1 = t1.getMPIRank();
      if ( prank1.first ) {
       // Use Rank.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, 2RETURN View_ByThread_Rank=" << View_ByThread_Rank << std::endl;
#endif
        return View_ByThread_Rank;
      }

      // NOTE: Always prefer this openmp thread id... therefore is should
      // always preceed check for posix thread id.
      std::pair<bool, int> pthread1 = t1.getOpenMPThreadId();
      if ( pthread1.first ) {
       // Use Thread.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, 2RETURN View_ByThread_OpenMPThread=" << View_ByThread_OpenMPThread << std::endl;
#endif
        return View_ByThread_OpenMPThread;
      }

      // NOTE: In reality we really never want to use posix thread ids as an id.
      std::pair<bool, pthread_t> posixthread1 = t1.getPosixThreadId();
      if ( posixthread1.first ) {
       // Use Thread.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, 2RETURN View_ByThread_PosixThread=" << View_ByThread_PosixThread << std::endl;
#endif
        return View_ByThread_PosixThread;
      }
      pid_t pid1 = t1.getProcessId();
      if ( pid1 ) {
       // Use PID.
#if DEBUG_CLI
        std::cerr << "Determine_ByThread_Id, 2RETURN View_ByThread_Process=" << View_ByThread_Process << std::endl;
#endif
        return View_ByThread_Process;
      }
    }
  }
#if DEBUG_CLI
  std::cerr << "Determine_ByThread_Id, RETURN View_ByThread_NotSpecified=" << View_ByThread_NotSpecified << std::endl;
#endif
  return View_ByThread_NotSpecified;
}

/**
 * Utility: Build_CallBack_Entry (Framework::StackTrace& st, int64_t i, bool add_stmts)
 *
 * Convert a specific element in a Framework::StackTrace array to a
 * CommandResult entry.
 *
 * @param   st - the Framework::StackTrace array
 * @param   i  - the index to the element in the array
 * @param   add_stmts - a boolean variable, if 'true' the statements
 *          associated with the related element are also included in
 *          the created CommandResult entry.
 *
 * @return  a pointer to the created CommandResult entry.
 *
 */
static inline
CommandResult *Build_CallBack_Entry (Framework::StackTrace& st, int64_t i, bool add_stmts) {
    CommandResult *SE = NULL;
    std::pair<bool, Function> fp = st.getFunctionAt(i);
#if DEBUG_CLI
    printf("Build_CallBack_Entry  - SS_View_util.cxx, i=%d, add_stmts=%d\n", i, add_stmts);
    printf("Build_CallBack_Entry  - SS_View_util.cxx, i=%d, address=%x\n", i, st[i].getValue());
#endif
    if (fp.first) {

#if DEBUG_CLI
       std::cerr << "Build_CallBack_Entry  - SS_View_util.cxx, fp.first "
            << fp.first <<  std::endl ;
       std::cerr << "Build_CallBack_Entry  - SS_View_util.cxx, fp.second.getName() "
            <<  fp.second.getName() << std::endl ;
#endif

     // Use Function.
      if (add_stmts) {
       // Include associated statements.
#if DEBUG_CLI
        printf("Build_CallBack_Entry  - SS_View_util.cxx, calling getStatementsAt, i=%d\n", i);
#endif
        std::set<Statement> ss = st.getStatementsAt(i);
        SE = new CommandResult_Function (fp.second, ss);
      } else {
        SE = new CommandResult_Function (fp.second);
      }
    } else {
     // There is no Function entry available - use the absolute address.
      SE = new CommandResult_Address (st[i].getValue());
    }
    return SE;
}


/**
 * Utility: Construct_CallBack [with 3 arguments]
 *
 * Convert an array of Framework::StackTrace elements into a vector
 * of CommandResult entries.
 *
 * @param   TraceBack_Order - 'true' if the root routine needs to be
 *          first, followed by the routines that call it.
 * @param   add_stmts - a boolean variable, if 'true' the statements
 *          associated with the related element are also included in
 *          the created CommandResult entry.
 * @param   st - the Framework::StackTrace array
 *
 * @return  a pointer to the created std::vector<CommandResult *> entry.
 *
 */
std::vector<CommandResult *> *
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st) {
  std::vector<CommandResult *> *call_stack
             = new std::vector<CommandResult *>();
#if DEBUG_CLI
  printf("Enter Construct_CallBack 3 args - SS_View_util.cxx, TraceBack_Order=%d, add_stmts=%d\n", 
         TraceBack_Order, add_stmts);
#endif
  int64_t len = st.size();
  int64_t i;

#if DEBUG_CLI
  printf("Enter Construct_CallBack 3 args - SS_View_util.cxx, len=%d\n", len);
#endif

  if (len == 0) return call_stack;
  if (TraceBack_Order)
    for ( i = 0;  i < len; i++) {
      call_stack->push_back(Build_CallBack_Entry(st, i, add_stmts));
    }
  else
    for ( i = len-1; i >= 0; i--) {
      call_stack->push_back(Build_CallBack_Entry(st, i, add_stmts));
    }

#if DEBUG_CLI
  printf("Exit Construct_CallBack 3 args - SS_View_util.cxx, TraceBack_Order=%d, add_stmts=%d\n", 
         TraceBack_Order, add_stmts);
#endif

  return call_stack;
}

/**
 * Utility: Construct_CallBack [with 4 arguments]
 *
 * Convert an array of Framework::StackTrace elements into a vector
 * of CommandResult entries. 
 *
 * @param   TraceBack_Order - 'true' if the root routine needs to be
 *          first, followed by the routines that call it.
 * @param   add_stmts - a boolean variable, if 'true' the statements
 *          associated with the related element are also included in
 *          the created CommandResult entry.
 * @param   st - the Framework::StackTrace array
 * @param   knownTraces a map that records previous element conversions
 *          so that previous copies can be used and database accesses,
 *          that are needed to construct new entries, can be avoided.
 *
 * @return  a pointer to the created std::vector<CommandResult *> entry.
 *
 * Note: since the lifetime of the data returned from this routine may be
 *       different from the lifetime of the 'knownTraces' structure, it
 *       is necessary to use different entries in each structure.
 *
 */
std::vector<CommandResult *> *
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st,
                           std::map<Address, CommandResult *>& knownTraces) {
  std::vector<CommandResult *> *call_stack
             = new std::vector<CommandResult *>();
  int64_t len = st.size();
#if DEBUG_CLI
  printf("Enter Construct_CallBack 4 args-SS_View_util.cxx, add_stmts=%d, TraceBack_Order=%d, len=%d\n",
           add_stmts, TraceBack_Order, len);
#endif
  int64_t i;
  if (len == 0) return call_stack;
  if (TraceBack_Order) {
   // Framework::StackTrace entries are in this order, so go through the array
   // in a forward direction.
    for ( i = 0;  i < len; i++) {
     // Check to see if we have already converted this entry.
      Address nextAddr = st[i];
#if DEBUG_CLI
      printf("In Construct_CallBack 4 args-SS_View_util.cxx, Traceback_Order, nextAddr.getValue()=%x\n", nextAddr.getValue());
#endif
      std::map<Address, CommandResult *>::iterator ki = knownTraces.find(nextAddr);
      CommandResult *NewCR;
      if (ki == knownTraces.end()) {
       // There is no existing entry, so a new entry needs to be created.
        NewCR = Build_CallBack_Entry(st, i, add_stmts);
        knownTraces[nextAddr] = NewCR;
      } else {
       // There is an existing entry, so just copy it.
        NewCR = (*ki).second;
      }
     // Always duplicate the saved entry.
      call_stack->push_back(NewCR->Copy());
    }
  } else {
   // Framework::StackTrace entries are in the reverse order, so go through the array
   // in a backward direction.
    for ( i = len-1; i >= 0; i--) {
      Address nextAddr = st[i];
#if DEBUG_CLI
      printf("In Construct_CallBack 4 args-SS_View_util.cxx,non-Traceback_Order nextAddr.getValue()=%x\n", nextAddr.getValue());
#endif
      std::map<Address, CommandResult *>::iterator ki = knownTraces.find(nextAddr);
      CommandResult *NewCR;
      if (ki == knownTraces.end()) {
       // There is no existing entry, so a new entry needs to be created.
        NewCR = Build_CallBack_Entry(st, i, add_stmts);
        knownTraces[nextAddr] = NewCR;
      } else {
       // There is an existing entry, so just use it.
        NewCR = (*ki).second;
      }
     // Always duplicate the saved entry.
      call_stack->push_back(NewCR->Copy());
    }
  }
#if DEBUG_CLI
  printf("Exit Construct_CallBack 4 args - SS_View_util.cxx\n");
#endif
  return call_stack;
}


// Utilities for working with class ViewInstruction

ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>& IV, int64_t Column) {
#if DEBUG_CLI
  printf("Enter Find_Column_Def - SS_View_util.cxx, Column=%d\n", Column);
#endif
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->TR() == Column) {
      if ((vp->OpCode() == VIEWINST_Display_Metric) ||
          (vp->OpCode() == VIEWINST_Display_ByThread_Metric) ||
          (vp->OpCode() == VIEWINST_Display_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Flops_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Ratio_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Ratio_Percent_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Inverse_Ratio_Percent_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
#if DEBUG_CLI
        printf("Exit Find_Column_Def - SS_View_util.cxx, return vp where i=%d\n", i);
#endif
        return vp;
      }
    }
  }
#if DEBUG_CLI
  printf("Exit Find_Column_Def - SS_View_util.cxx, return NULL\n");
#endif
  return NULL;
}

int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>& IV) {
  int64_t Max_Column = -1;
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if ((vp->OpCode() == VIEWINST_Display_Metric) ||
        (vp->OpCode() == VIEWINST_Display_ByThread_Metric) ||
        (vp->OpCode() == VIEWINST_Display_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Flops_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Ratio_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Ratio_Percent_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Inverse_Ratio_Percent_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
      if (vp->TR() > Max_Column) Max_Column = vp->TR();
    }
  }
  return Max_Column;
}

int64_t Find_Max_Temp (std::vector<ViewInstruction *>& IV) {
  int64_t Max_Temp = -1;
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Display_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
    } else if (vp->OpCode() == VIEWINST_Display_Percent_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
    } else if (vp->OpCode() == VIEWINST_Display_Average_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
      Max_Temp = std::max (Max_Temp, vp->TMP3());
    } else if (vp->OpCode() == VIEWINST_Display_Flops_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Display_Ratio_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Display_Ratio_Percent_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Display_Inverse_Ratio_Percent_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
      Max_Temp = std::max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Define_Total_Tmp) {
      Max_Temp = std::max (Max_Temp, vp->TMP1());
    } else if (vp->OpCode() == VIEWINST_Expression) {
      Max_Temp = std::max (Max_Temp, vp->TR());
    }
  }
  return Max_Temp;
}

int64_t Find_Max_ExtraMetrics (std::vector<ViewInstruction *> & IV) {
  int64_t Max_Temp = ViewReduction_Count-1;
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Define_ByThread_Metric) {
      Max_Temp = std::max (Max_Temp, vp->TR());
    }
  }
  return Max_Temp;
}

void Print_View_Params (std::ostream &to,
                        std::vector<Collector>& CV,
                        std::vector<std::string>& MV,
                        std::vector<ViewInstruction *>& IV) {
  int i;
  to << std::endl << "List Collectors" << std::endl;
  for ( i=0; i < CV.size(); i++) {
    to << "\t" << CV[i].getMetadata().getUniqueId() << std::endl;
  }
  to << "List Metrics" << std::endl;
  for ( i=0; i < MV.size(); i++) {
    to << "\t" << MV[i] << std::endl;
  }
  to << "List Instructions" << std::endl;
  for ( i=0; i < IV.size(); i++) {
    to << "\t";
    IV[i]->Print (to);
  }
}


/**
 * function: evaluate_parse_expression
 * 
 * Convert a parse expression into view instructions that can
 * be used to generate the value of a Metric Expression that
 * is needed for the user's requested report.
 *     
 * return: the index of the expression temporary that contains
 *         the result of the evaulated Metric Expression.
 *
 */

 int64_t evaluate_parse_expression (
            CommandObject *cmd,
            ExperimentObject *exp,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc,
            ParseRange *pr,
            int64_t &last_used_temp,
            std::string view_for_collector,
            std::map<std::string, int64_t> &MetricMap) {
  if (pr->getParseType() == PARSE_EXPRESSION_VALUE) {
   // It must be a nested expression.

    if (pr->getOperation() == EXPRESSION_OP_ERROR) {
     // Error has already been detected and reported.
      return -1;
    }
   
    if (pr->getOperation() == EXPRESSION_OP_CONST) {
     // These should not make it past the parser.
      return -1;
    }

    if (pr->getOperation() == EXPRESSION_OP_HEADER) {
     // Not meaningful to nest headers.
      std::string s("A HEADER expression may not be nested within another expression.");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return -1;
    }
   
   // Evaluate the nested expression.
    parse_expression_t *prt = pr->getExpression();
    std::vector<ParseRange *> *prexp = &(prt->exp_operands);
    std::vector<ParseRange *>::iterator prexpi;
    int64_t ops_count = 0;
    int64_t ops[3];
    for (int i=0; i<3; i++) ops[i] = -1;
    bool error_in_argument = false;
    for (prexpi = prexp->begin(); prexpi != prexp->end(); prexpi++) {
     // All input operands must be evaluated before creating a result temp.
      ParseRange *pr = (*prexpi);
      int64_t temp_result = evaluate_parse_expression (cmd, exp, CV, MV, IV, HV, vfc, pr, last_used_temp, view_for_collector, MetricMap);
      if (temp_result < 0) {
       // Save error condition but conttinue processing arguments.
        error_in_argument = true;
      }
      ops[ops_count++] = temp_result;
    }
    if (error_in_argument) {
     // return error condition back through the call tree.
      return -1;
     }

    // Create a temp for this result and return it.
    int64_t new_result = ++last_used_temp;
    ViewInstruction *VI = NULL;

    VI = new ViewInstruction (VIEWINST_Expression, new_result, ops[0], ops[1], ops[2]);
    VI->Set_ViewExprOpCode (pr->getOperation());
    IV.push_back(VI);
    return new_result;
  } else if ( (pr->getParseType() == PARSE_RANGE_VALUE) &&
              (pr->getRange()->start_range.tag != VAL_STRING) ) {
   // It must be a user defined constant.
    int64_t new_result = ++last_used_temp;
    ViewInstruction *VI = NULL;
    parse_range_t *p_range = pr->getRange();
    parse_val_t *p_vals = &p_range->start_range;
    if (p_vals->tag == VAL_STRING) {
      VI = ViewInstructionConstant (VIEWINST_SetConstString, new_result, CRPTR(p_vals->name));
    } else if (p_vals->tag == VAL_NUMBER) {
      VI = ViewInstructionConstant (VIEWINST_SetConstInt, new_result, CRPTR(p_vals->num));
    } else {
      VI = ViewInstructionConstant (VIEWINST_SetConstFloat, new_result, CRPTR(p_vals->val));
    }
    IV.push_back(VI);
    return new_result;
  } else {
   // It must be a collector defined metric.
    parse_range_t *m_range = pr->getRange();
    std::string M_Name;
    if (m_range->is_range) {
      std::string C_Name = m_range->start_range.name;
      M_Name = m_range->end_range.name;
    } else {
      M_Name = m_range->start_range.name;
    }
   // Try look up with the user defined string.
    int64_t metric_temp = (MetricMap.find(M_Name) != MetricMap.end()) ? MetricMap[M_Name] : -1;
    if (metric_temp < 0) {
     // Try again after converting to a lowercase string.
      std::string newmetric = lowerstring(M_Name);
      metric_temp = (MetricMap.find(newmetric) != MetricMap.end()) ? MetricMap[newmetric] : -1;
    }
    if (metric_temp < 0) {
      std::string s(M_Name +
                    " is not available for use within this Metric expression of this '" +
                    view_for_collector + "' view.");
      Mark_Cmd_With_Soft_Error(cmd,s);
    }
    return metric_temp;
  }
  return -1;
}
