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

  for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread thread = *ti;

    try {
     // Get the current list of functions in this thread
      std::set<Function> functions = thread.getFunctions();
      std::set<Function>::iterator fi = functions.find(F);

      if (F == *fi) {

       // Get the address range of this function
        AddressRange range = fi->getAddressRange();

       // Evalute the metric over this address range
        T new_value;
        collector.getMetricValue(metric, thread, range, Forever, new_value);

       // Add this function and its metric value to the map
        value += new_value;
      }
    }
    catch(const Exception& error) {
      // Ignore problem and do the best we can.
    }
  }

  return;
}

CommandResult *Get_Collector_Metric (CommandObject *cmd,
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
    Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    int64_t Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(int)) ) {
    int Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value = 0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value = 0.0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value = 0.0;
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(string)) ) {
    std::string Value = "";
    GetMetricsForFunction(cmd, F, tgrp, collector, metric, Value);
    Param_Value = new CommandResult_String (Value);
  } else {
    Param_Value = new CommandResult_String("Unknown type.");
  }
  return Param_Value;
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
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    if (!strncasecmp (viewname.c_str(), vt->Unique_Name().c_str(), vt->Unique_Name().length()) &&
        Remainaing_Length_Is_Numeric (viewname, vt->Unique_Name().length())) {
      return vt;
    }
  }

  return NULL;
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

bool Metadata_hasName (std::set<Metadata> M, std::string name) {
  try {
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

 // We didn't find the Mete-Name in this set.
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
      sprintf( &l[0], "%lld", s.getLine());
      S = S + ": " + s.getPath().getBaseName() + "," + &l[0];
    }
  }
  S += ")";

  return (new CommandResult_String (S));
}
