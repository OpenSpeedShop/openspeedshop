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

#include "Python.h"

using namespace std;

#include "SS_Parse_Result.hxx"
#include "SS_Parse_Target.hxx"

using namespace OpenSpeedShop::cli;

// Global View management utilities

template <class T>
struct sort_ascending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second < y.second;
    }
};
template <class T>
struct sort_descending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second > y.second;
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

template <typename T>
void GetMetricsForFunction (CommandObject *cmd,
                            Function F,
                            ThreadGroup tgrp,
                            Collector collector,
                            std::string metric,
                            T& value)
{

 // Time interval covering earliest to latest possible time
  const TimeInterval Forever =
      TimeInterval(Time::TheBeginning(), Time::TheEnd());

 // Get the thread containing this function
  Thread thread = F.getThread();

 // Get the address range of this function
  AddressRange range = F.getAddressRange();

 // Evalute the metric over this address range
  T new_value;
  collector.getMetricValue(metric, thread, range, Forever, new_value);

 // Add this function and its metric value to the map
  value += new_value;
  return;
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
  AddressRange range = AddressRange( 0, 0xffffffffffffffff );

 // Evalute the metric over this address range
  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread thread = *ti;

    T new_value;
    collector.getMetricValue(metric, thread, range, Forever, new_value);

   // Add this function and its metric value to the map
    value += new_value;
  }

  return;
}

CommandResult *Init_Collector_Metric (CommandObject *cmd,
                                      Collector collector,
                                      std::string metric) {
  CommandResult *Param_Value = NULL;
  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();
  if( m.isType(typeid(unsigned int)) ) {
    Param_Value = new CommandResult_Uint ();
  } else if( m.isType(typeid(uint64_t)) ) {
    Param_Value = new CommandResult_Uint ();
  } else if( m.isType(typeid(int)) ) {
    Param_Value = new CommandResult_Int ();
  } else if( m.isType(typeid(int64_t)) ) {
    Param_Value = new CommandResult_Int ();
  } else if( m.isType(typeid(float)) ) {
    Param_Value = new CommandResult_Float ();
  } else if( m.isType(typeid(double)) ) {
    Param_Value = new CommandResult_Float ();
  } else if( m.isType(typeid(string)) ) {
    Param_Value = new CommandResult_String ("");
  } else {
    cmd->Result_String ("(The metric, " + id + ", does not have a supported type.)");
    cmd->set_Status(CMD_ERROR);
    Param_Value = NULL;
  }
  return Param_Value;
}

CommandResult *Get_Function_Metric (CommandObject *cmd,
                                    Function F,
                                    ThreadGroup tgrp,
                                    Collector collector,
                                    std::string metric) {
  CommandResult *Param_Value = NULL;
  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();
  if( m.isType(typeid(unsigned int)) ) {
    uint Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    int64_t Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(int)) ) {
    int Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value = 0.0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0.0) Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value = 0.0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != 0.0) Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(string)) ) {
    std::string Value = "";
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    if (Value != "") Param_Value = new CommandResult_String (Value);
  } else {
    Param_Value = NULL;
  }
  return Param_Value;
}

CommandResult *Get_Total_Metric (CommandObject *cmd,
                                 ThreadGroup tgrp,
                                 Collector collector,
                                 std::string metric) {
  CommandResult *Param_Value = NULL;
  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();
  if( m.isType(typeid(unsigned int)) ) {
    uint Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    int64_t Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value != 0) Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(int)) ) {
    int Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0) Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value = 0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0) Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value = 0.0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0.0000000001) Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value = 0.0;
    GetMetricsforThreads(cmd, tgrp, collector, metric, Value);
    if (Value > 0.0000000001) Param_Value = new CommandResult_Float (Value);
  }
  return Param_Value;
}

std::set<Function> GetFunctions (CommandObject *cmd,
                                 ThreadGroup tgrp)
{
  std::set<Function> functions;

 // Iterate over all the threads and find all the functions
  ThreadGroup::iterator ti;
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread T = *ti;
    try {
      std::set<Function> fset = T.getFunctions();

     // Iterate over each function and add it to the base set.
      for(std::set<Function>::const_iterator
              it = fset.begin(); it != fset.end(); ++it) {
        functions.insert(*it);
      }

    }
    catch(const Exception& error) {
     // Ignore errors and do with what is available.
    }
  }

  return functions;
}

std::vector<Function_CommandResult_pair>
                 GetMetricByFunction (CommandObject *cmd,
                                      bool ascending_sort,
                                      ThreadGroup tgrp,
                                      Collector C,
                                      std::string metric)
{
  std::vector<Function_CommandResult_pair> items;

 // Get the current list of functions in this thread
  std::set<Function> functions = GetFunctions (cmd, tgrp);

 // Iterate over each function
  for(std::set<Function>::const_iterator
            i = functions.begin(); i != functions.end(); ++i) {

   // Evalute the metric for this function
    CommandResult *value = Get_Function_Metric ( cmd, *i, tgrp, C, metric);

   // Add this function and its metric value to the map
    if(value != NULL) {
        items.push_back( std::make_pair(*i, value) );
    }

  }

 // Now we can sort the data.
  if (ascending_sort) {
    std::sort(items.begin(), items.end(), sort_ascending_CommandResult<Function_CommandResult_pair>());
  } else {
    std::sort(items.begin(), items.end(), sort_descending_CommandResult<Function_CommandResult_pair>());
  }

  return items;
}

std::vector<Function_double_pair>
                 GetDoubleByFunction (CommandObject *cmd,
                                      bool ascending_sort,
                                      ThreadGroup tgrp,
                                      Collector C,
                                      std::string metric)
{
  std::vector<Function_double_pair> items;
  SmartPtr<std::map<Function, double> > data;

  ThreadGroup::iterator ti;
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread T = *ti;
    try {
      Queries::GetMetricByFunctionInThread(C, metric, T, data);
    }
    catch(const Exception& error) {
      //Mark_Cmd_With_Std_Error (cmd, error);
      //return data;
    }
  }

 // Now we can sort the data.
  for(std::map<Function, double>::const_iterator
      item = data->begin(); item != data->end(); ++item)
  {
    items.push_back( *item );
  }

  if (ascending_sort) {
    std::sort(items.begin(), items.end(), sort_ascending<Function_double_pair>());
  } else {
    std::sort(items.begin(), items.end(), sort_descending<Function_double_pair>());
  }

  return items;
}

double Total_second (std::vector<Function_double_pair> items) {
 // Calculate the total time for this set of samples.
  double Total = 0.0;
  std::vector<Function_double_pair>::const_iterator itt = items.begin();
  for( ; itt != items.end(); itt++ ) {
    Total += itt->second;
  }
  return Total;
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
    fprintf(stderr,"WARNING: Definition of View named %s may hide the existing definition of %s\n",
                    vnew->Unique_Name().c_str(), existing->Unique_Name().c_str());
  }
  Available_Views.push_front (vnew);
}

void Add_Column_Headers (CommandResult_Headers *H, std::string *column_titles)
{
  int64_t i = 0;
  while (column_titles[i].length() != 0) {
    CommandResult *T = new CommandResult_String ( column_titles[i] );
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

CommandResult *gen_F_name (Function F) {
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

  return (new CommandResult_String (S));
}


// Utilities for working with class ViewInstruction

ViewInstruction *Find_Total_Def (std::vector<ViewInstruction *>IV) {
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Define_Total) {
      return vp;
    }
  }
  return NULL;
}

ViewInstruction *Find_Percent_Def (std::vector<ViewInstruction *>IV) {
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if ((vp->OpCode() == VIEWINST_Display_Percent_Metric) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Tmp)) {
      return vp;
    }
  }
  return NULL;
}

ViewInstruction *Find_Column_Def (std::vector<ViewInstruction *>IV, int64_t Column) {
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->TR() == Column) {
      if ((vp->OpCode() == VIEWINST_Display_Metric) ||
          (vp->OpCode() == VIEWINST_Display_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Metric) ||
          (vp->OpCode() == VIEWINST_Display_Percent_Tmp)) {
        return vp;
      }
    }
  }
  return NULL;
}

int64_t Find_Max_Column_Def (std::vector<ViewInstruction *>IV) {
  int64_t Max_Column = -1;
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if ((vp->OpCode() == VIEWINST_Display_Metric) ||
        (vp->OpCode() == VIEWINST_Display_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Column) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Metric) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Tmp)) {
      if (vp->TR() > Max_Column) Max_Column = vp->TR();
    }
  }
  return Max_Column;
}

void Print_View_Params (FILE *TFile,
                        std::vector<Collector> CV,
                        std::vector<std::string> MV,
                        std::vector<ViewInstruction *>IV) {
  int i;
  fprintf(TFile,"\nList Metrics\n");
  for ( i=0; i < MV.size(); i++) {
    fprintf(TFile,"\t%s\n",MV[i].c_str());
  }
  fprintf(TFile,"List Instructions\n");
  for ( i=0; i < IV.size(); i++) {
    fprintf(TFile,"\t");
    IV[i]->Print (TFile);
  }
}
