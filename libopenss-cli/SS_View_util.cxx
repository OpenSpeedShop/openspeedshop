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

#include "Queries.hxx"

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

template <typename TE>
void GetMetricBySet (CommandObject *cmd,
                     ThreadGroup& tgrp,
                     Collector& collector,
                     std::string& metric,
                     std::set<TE>& objects,
                     SmartPtr<std::map<TE, CommandResult *> >& items) {

  Metadata m = Find_Metadata ( collector, metric );
  std::string id = m.getUniqueId();

  if( m.isType(typeid(unsigned int)) ) {
    SmartPtr<std::map<TE, uint> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, uint>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, uint> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(uint64_t)) ) {
    SmartPtr<std::map<TE, uint64_t> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, uint64_t>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, uint64_t> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(int)) ) {
    SmartPtr<std::map<TE, int> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, int>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, int> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(int64_t)) ) {
    SmartPtr<std::map<TE, int64_t> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, int64_t>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, int64_t> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(float)) ) {
    SmartPtr<std::map<TE, float> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, float>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, float> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(double)) ) {
    SmartPtr<std::map<TE, double> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, double>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, double> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  } else if( m.isType(typeid(string)) ) {
    SmartPtr<std::map<TE, string> > data;
    GetMetricInThreadGroup (collector, metric, tgrp, objects, data);
    for(typename std::map<TE, string>::const_iterator
        item = data->begin(); item != data->end(); ++item) {
      std::pair<TE, string> p = *item;
      items->insert( std::make_pair(p.first, CRPTR (p.second)) );
    }
  }

  return;
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Function>& objects,
                           SmartPtr<std::map<Function, CommandResult *> >& items) {
  GetMetricBySet (cmd, tgrp, collector, metric, objects, items);
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<Statement>& objects,
                           SmartPtr<std::map<Statement, CommandResult *> >& items) {
  GetMetricBySet (cmd, tgrp, collector, metric, objects, items);
}

void GetMetricByObjectSet (CommandObject *cmd,
                           ThreadGroup& tgrp,
                           Collector& collector,
                           std::string& metric,
                           std::set<LinkedObject>& objects,
                           SmartPtr<std::map<LinkedObject, CommandResult *> >& items) {
  GetMetricBySet (cmd, tgrp, collector, metric, objects, items);
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
  }
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
    cerr << "WARNING: Definition of View named "
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

template <typename TE>
bool Cmp_F2Object (std::string F_Name, TE object) {
  if (F_Name.size() == 0) return false;
  int64_t len = F_Name.size();
  int64_t pos = F_Name.find( "/" );
  if ((pos >= 0) &&
      (pos < len) &&
      ((pos != 1) || (F_Name[0] != *"."))) {
   // Compare path name.
    return (F_Name == object.getPath());
  } else {
   // Compare basename.
    int64_t dot = F_Name.find( "." );
    if ((dot >= 0) &&
        (dot < len)) {
     // look for exact match.
      return (F_Name == object.getPath().getBaseName());
    } else {
     // Look for partial match.
      std::string baseName = object.getPath().getBaseName();
      int64_t subl = baseName.size();
      int64_t subs = baseName.find (F_Name);
      return ((subs >= 0) && (subs < subl));
    }
  }
}

static bool Include_Object (std::string F_Name, Thread thread, LinkedObject object) {
 // F_Name can only be a LinkedObject name.
  return Cmp_F2Object (F_Name, object);
}

static  bool Include_Object (std::string F_Name, Thread thread, Function object) {
 // Is the object in the set of previously selected threads?
  std::set<Thread> tt = object.getThreads();
  if (tt.find(thread) == tt.end()) return false;

 // Is F_Name a function name?
  if (F_Name == object.getName()) return true;

 // Is F_Name a LinkedObject name?
  LinkedObject L = object.getLinkedObject();
  if (Cmp_F2Object (F_Name, L)) return true;

 // Is F_Name a containing file name?
  std::set<Statement> T = object.getDefinitions();
  for (std::set<Statement>::iterator ti = T.begin(); ti != T.end(); ti++) {
    Statement st = *ti;
    std::set<Thread> ts = st.getThreads();
    if (ts.find(thread) == ts.end()) continue;
    std::set<Function> tf = st.getFunctions();
    if (tf.find(object) == tf.end()) continue;
    if (Cmp_F2Object (F_Name, st)) return true;
  }

  return false;
}

static bool Include_Object (std::string F_Name, Thread thread, Statement object) {
 // Is the object in the set of previously selected threads?
  std::set<Thread> tt = object.getThreads();
  if (tt.find(thread) == tt.end()) return false;

 // Is F_Name a function name?
  std::set<Function> tf = object.getFunctions();
  for (std::set<Function>::iterator fi = tf.begin(); fi != tf.end(); fi++) {
    Function f = *fi;
    std::set<Thread> tt = f.getThreads();
    if (tt.find(thread) == tt.end()) continue;
    if (F_Name == f.getName()) return true;
  }

 // Is F_Name a LinkedObject name?
  LinkedObject L = object.getLinkedObject();
  if (Cmp_F2Object (F_Name, L)) return true;

 // Is F_Name a containing file name?
  if (Cmp_F2Object (F_Name, object)) return true;

  return false;
}

static bool Uses_Path_Name (CommandObject *cmd) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  bool has_f = false;
  OpenSpeedShop::cli::ParseTarget pt;
  vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;
  if (p_tlist->begin() != p_tlist->end()) {
    // There is a list.  Is there a "-f" specifier?
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();
    has_f = !((f_list == NULL) || f_list->empty());
  }
  if (!has_f) return false;
  vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
  for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
    OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
    OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
    Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
    std::string F_Name = pval1.name;

   // If the name contains a "/" or "." we need to compare path names.
    if (F_Name.size() == 0) continue;
    int64_t len = F_Name.size();
    int64_t pos = F_Name.find( "/" );
    if ((pos >= 0) &&
        (pos < len)) {
      return true;
    } else {
      int64_t dot = F_Name.find( "." );
      if ((dot >= 0) &&
          (dot < len)) {
        return true;
      }
    }

  }
  return false;
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<LinkedObject>& objects) {
  std::set<Function>::iterator fi;
  for (fi = named_functions.begin(); fi != named_functions.end(); fi++) {
    Function f = *fi;
    LinkedObject l = f.getLinkedObject();
    objects.insert (l);
  }
}

static void Get_Objects_By_Function (std::set<Function>& named_functions,
                                     std::set<Statement>& objects) {
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



static inline bool Force_Long_Compare (std::set<LinkedObject>& objects) { return true; }
static inline bool Force_Long_Compare (std::set<Function>& objects) { return false; }
static inline bool Force_Long_Compare (std::set<Statement>& objects) { return false; }

template <typename TE>
void Filtered_Objects (CommandObject *cmd,
                       ExperimentObject *exp,
                       ThreadGroup& tgrp,
                       std::set<TE >& objects ) {
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();
  OpenSpeedShop::cli::ParseTarget pt;
  vector<OpenSpeedShop::cli::ParseRange> *f_list = NULL;

  if (p_tlist->begin() != p_tlist->end()) {
    // There is a list.  Is there a "-f" specifier?
    pt = *p_tlist->begin(); // There can only be one!
    f_list = pt.getFileList();
  }

  if ((f_list == NULL) || (f_list->empty())) {
   // There is no Function filtering requested.
   // Get all the functions in the already selected thread groups.
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      Thread thread = *ti;
      std::set<TE> new_objects;
      Get_Source_Objects(thread, new_objects);
      objects.insert(new_objects.begin(), new_objects.end());
    }
  } else if (Force_Long_Compare(objects) || Uses_Path_Name(cmd)) {
    for (ThreadGroup::iterator ti = tgrp.begin(); ti != tgrp.end(); ti++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

     // Only include selected objects.
      Thread thread = *ti;
      std::set<TE> new_objects;
      Get_Source_Objects(thread, new_objects);
      vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
      for (typename std::set<TE>::iterator newi = new_objects.begin();
                newi != new_objects.end();
                newi++) {

       // Check for asynchronous abort command
        if (cmd->Status() == CMD_ABORTED) {
          return;
        }

        TE new_object = *newi;
        if (objects.find(new_object) == objects.end()) {
         // If the new object is not already included,
         // look to see if it is selected by any of the items in the list.
          for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
            OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
            OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
            Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
            std::string F_Name = pval1.name;
            if (Include_Object (F_Name, thread, new_object)) {
              objects.insert(new_object);
              break;
            }
          }
        }
      }
    }
  } else {
   // There is some sort of function name filter specified.
   // Determine the names of desired functions and get Function objects for them.
   // Then get the objects that we want for these fucntions.
   // Thread filtering will be done in GetMetricInThreadGroup.
    std::map<Function, std::map<Framework::StackTrace, std::vector<double> > >::iterator fi;
    vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;
    for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {

     // Check for asynchronous abort command
      if (cmd->Status() == CMD_ABORTED) {
        return;
      }

      OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
      OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
      Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
      std::string F_Name = pval1.name;
      std::set<Function> new_functions = exp->FW()->getFunctionsByNamePattern (F_Name);
      if (!new_functions.empty()) {
        Get_Objects_By_Function (new_functions, objects );
      }
    }
  }
}

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

// Utilites to deciding what data to retrieve from a database.

/**
 * Method: Validate_V_Options(CommandObject *cmd, std::string allowed[])
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
  vector<string> *p_slist = cmd->P_Result()->getModifierList();
  vector<string>::iterator j;

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


View_Form_Category Determine_Form_Category (CommandObject *cmd) {
  if (Look_For_KeyWord(cmd, "Trace")) {
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
  }
  return VFC_Function;
}

bool Determine_TraceBack_Ordering (CommandObject *cmd) {
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

SmartPtr<std::vector<CommandResult *> >
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

SmartPtr<std::vector<CommandResult *> >
       Construct_CallBack (bool TraceBack_Order, bool add_stmts, Framework::StackTrace& st,
                           std::map<Address, CommandResult *>& knownTraces) {
  SmartPtr<std::vector<CommandResult *> > call_stack
             = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
  int64_t len = st.size();
  int64_t i;
  if (len == 0) return call_stack;
  if (TraceBack_Order)
    for ( i = 0;  i < len; i++) {
      Address nextAddr = st[i];
      std::map<Address, CommandResult *>::iterator ki = knownTraces.find(nextAddr);
      CommandResult *newCR;
      if (ki == knownTraces.end()) {
        newCR = Build_CallBack_Entry(st, i, add_stmts);
        knownTraces[nextAddr] = newCR;
      } else {
        newCR = Dup_CommandResult ((*ki).second);
      }
      call_stack->push_back(newCR);
    }
  else
    for ( i = len-1; i >= 0; i--) {
      Address nextAddr = st[i];
      std::map<Address, CommandResult *>::iterator ki = knownTraces.find(nextAddr);
      CommandResult *newCR;
      if (ki == knownTraces.end()) {
        newCR = Build_CallBack_Entry(st, i, add_stmts);
        knownTraces[nextAddr] = newCR;
      } else {
        newCR = Dup_CommandResult ((*ki).second);
      }
      call_stack->push_back(newCR);
    }
  return call_stack;
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
    if ((vp->OpCode() == VIEWINST_Display_Percent_Column) ||
        (vp->OpCode() == VIEWINST_Display_Percent_Metric) ||
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
          (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
          (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
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
        (vp->OpCode() == VIEWINST_Display_Percent_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_Average_Tmp) ||
        (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp)) {
      if (vp->TR() > Max_Column) Max_Column = vp->TR();
    }
  }
  return Max_Column;
}

int64_t Find_Max_Temp (std::vector<ViewInstruction *>IV) {
  int64_t Max_Temp = -1;
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Display_Tmp) {
      Max_Temp = max (Max_Temp, vp->TMP1());
    } else if (vp->OpCode() == VIEWINST_Display_Percent_Tmp) {
      Max_Temp = max (Max_Temp, vp->TMP1());
    } else if (vp->OpCode() == VIEWINST_Display_Average_Tmp) {
      Max_Temp = max (Max_Temp, vp->TMP1());
      Max_Temp = max (Max_Temp, vp->TMP2());
    } else if (vp->OpCode() == VIEWINST_Display_StdDeviation_Tmp) {
      Max_Temp = max (Max_Temp, vp->TMP1());
      Max_Temp = max (Max_Temp, vp->TMP2());
      Max_Temp = max (Max_Temp, vp->TMP3());
    }
  }
  return Max_Temp;
}

void Print_View_Params (ostream &to,
                        std::vector<Collector> CV,
                        std::vector<std::string> MV,
                        std::vector<ViewInstruction *>IV) {
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
