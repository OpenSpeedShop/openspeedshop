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

typedef std::pair<std::string, double> item_type;
template <class T>
struct sort_ascending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second < y.second;
    }
};
template <class T>
struct sort_decending : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return x.second > y.second;
    }
};

// Utilities

typedef std::pair<std::string, double> func_time_pair;

static std::vector<func_time_pair>
                 GetTimeByFunction (CommandObject *cmd,
                                    OpenSpeedShop::Framework::Experiment *fexp,
                                    std::string C_Name,
                                    std::string metric)
{
  std::vector<func_time_pair> items;
  SmartPtr<std::map<Function, double> > data;

  if (!Collector_Used_In_Experiment (fexp, C_Name)) {
    cmd->Result_String ("The required collector, " + C_Name + ", was not used.");
    cmd->set_Status(CMD_ERROR);
    return items;
  }
  Collector C = Get_Collector (fexp, C_Name);

 // Retrieve the specified metric for all functions in all the thread
  ThreadGroup tgrp = fexp->getThreads();
  if (tgrp.begin() == tgrp.end()) {
    cmd->Result_String ( "There is no data available - there was no application run." );
    cmd->set_Status(CMD_ERROR);
    return items;
  }
  ThreadGroup::iterator ti;
  for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
    Thread T = *ti;
    try {
      Queries::GetMetricByFunctionInThread(C, metric, T, data);
    }
    catch(const std::exception& error) {
      //Mark_Cmd_With_Std_Error (cmd, error);
      //return data;
    }
  }

 // If there is no data, return.
  if (data->begin() == data->end()) {
    cmd->Result_String ( "There is no data available - there were no samples generated." );
    cmd->set_Status(CMD_ERROR);
    return items;
  }

 // Now we can sort the data.
  for(std::map<Function, double>::const_iterator
      item = data->begin(); item != data->end(); ++item)
  {
    items.push_back( std::pair<std::string, double>(item->first.getName(), item->second ) );
  }

 // std::sort(items.begin(), items.end(), sort_ascending<item_type>());
  std::sort(items.begin(), items.end(), sort_decending<func_time_pair>());

  return items;
}

static double Get_Total_Time (std::vector<func_time_pair> items) {
 // Calculate the total time for this set of samples.
  double TotalTime = 0.0;
  std::vector<func_time_pair>::const_iterator itt = items.begin();
  for( ; itt != items.end(); itt++ ) {
    TotalTime += itt->second;
  }
  return TotalTime;
}

/*
static void Add_Header (CommandObject *cmd, std::string *column_titles)
{
  CommandResult_Headers *H = new CommandResult_Headers ();
  int64_t i = 0;
  while (column_titles[i].length() != 0) {
    CommandResult *T = new CommandResult_String ( column_titles[i] );
    H->CommandResult_Headers::Add_Header (T);
    i++;
  }
  cmd->Result_Predefined (H);
}
*/

// pcfunc view

static std::string VIEW_pcfunc_brief = "pctime : % total : % cumulative : function";
static std::string VIEW_pcfunc_short = "Report the amount and percent of program time spent in a function.";
static std::string VIEW_pcfunc_long  = "Use the program timer to produce a sorted report of the functions"
                                       " that use the most time.  Calculate"
                                       " the percent of total time that each function uses."
                                       " A positive integer can be added to the end of the keyword"
                                       " ""pcfunc"" to indicate the maximum number of items in"
                                       " the report.";
static std::string VIEW_pcfunc_collectors[] =
  { "pcsamp",
    ""
  };
static std::string VIEW_pcfunc_header[] =
  { "  CPU Time (Seconds)",
    "          % of Total",
    "        Cumulative %",
    "Name",
    ""
  };
static bool VIEW_pcfunc (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
  if (topn == 0) topn = INT_MAX;
  try {
    std::vector<item_type> items = GetTimeByFunction (cmd, exp->FW(), "pcsamp", "time");
    if (items.begin() == items.end()) {
      return false;   // There is no data, return.
    }
    double TotalTime = Get_Total_Time (items);
    if (TotalTime < 0.0000000001) {
      cmd->Result_String ( "(the measure time interval is too small)" );
      cmd->set_Status(CMD_ERROR);
      return false;
    }

   // Build a Header for the table.
    Add_Header (cmd, &VIEW_pcfunc_header[0]);

   // convert time to %
    double percent_factor = 100.0 / TotalTime;

   // Extract the top "n" items from the sorted list.
    std::vector<func_time_pair>::const_iterator it = items.begin();
    double a_percent = 0; // accumulated percent
    for(int64_t foundn = 0; (foundn < topn) && (it != items.end()); foundn++, it++ ) {
      double c_percent = it->second*percent_factor;  // current item's percent of total time
      a_percent += c_percent;

      CommandResult_Columns *C = new CommandResult_Columns (4);
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (it->second));
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (c_percent));
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (a_percent));
      C->CommandResult_Columns::Add_Column (new CommandResult_String (it->first));
      cmd->Result_Predefined (C);
    }

  }
  catch(const std::exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return true;
}
class pcfunc_view : public ViewType {

 public: 
  pcfunc_view() : ViewType ("pcfunc",
                             VIEW_pcfunc_brief,
                             VIEW_pcfunc_short,
                             VIEW_pcfunc_long,
                            &VIEW_pcfunc_collectors[0],
                            &VIEW_pcfunc_header[0]) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    return VIEW_pcfunc (cmd, exp, topn);
  }
};


// vtop view

static std::string VIEW_vtop_brief = "pctime : function";
static std::string VIEW_vtop_short = "Report the amount of program time spent in a function.";
static std::string VIEW_vtop_long  = "Produce a decending report of the functions that use the "
                                     " most time as measured by the program counter."
                                     " A positive integer can be added to the end of the keyword"
                                     " ""vtop"" to indicate the maximum number of items in"
                                     " the report.";
static std::string VIEW_vtop_collectors[] =
  { "pcsamp",
    ""
  };
static std::string VIEW_vtop_header[] =
  { "  CPU Time (Seconds)",
    "Function",
    ""
  };
static bool VIEW_vtop (CommandObject *cmd, ExperimentObject *exp, int64_t topn)
{
  if (topn == 0) topn = INT_MAX;
  try {
    std::vector<item_type> items = GetTimeByFunction (cmd, exp->FW(), "pcsamp", "time");
    if (items.begin() == items.end()) {
      return false;   // There is no data, return.
    }

   // Build a Header for the table - just two items - time and function name.
    Add_Header (cmd, &VIEW_vtop_header[0]);

   // Extract the top "n" items from the sorted list.
    std::vector<item_type>::const_iterator it = items.begin();
    for(int64_t foundn = 0; (foundn < topn) && (it != items.end()); foundn++, it++ ) {
      CommandResult_Columns *C = new CommandResult_Columns (2);
      C->CommandResult_Columns::Add_Column (new CommandResult_Float (it->second));
      C->CommandResult_Columns::Add_Column (new CommandResult_String (it->first));
      cmd->Result_Predefined (C);
    }
  }
  catch(const std::exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

  return true;
}
class vtop_view : public ViewType {

 public: 
  vtop_view() : ViewType ("vtop",
                           VIEW_vtop_brief,
                           VIEW_vtop_short,
                           VIEW_vtop_long,
                          &VIEW_vtop_collectors[0],
                          &VIEW_vtop_header[0]) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    return VIEW_vtop (cmd, exp, topn);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void pcfunc_LTX_ViewFactory () {
  Define_New_View (new vtop_view());
  Define_New_View (new pcfunc_view());
}
