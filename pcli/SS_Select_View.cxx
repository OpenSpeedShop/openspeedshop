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

static bool VIEWN_vtop (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {

 // Build a Header for the table.
  CommandResult_Headers *H = new CommandResult_Headers ();
  CommandResult *T = new CommandResult_String ( "  CPU Time (Seconds)" );
  H->CommandResult_Headers::Add_Header (T);
  T = new CommandResult_String ( "Function" );
  H->CommandResult_Headers::Add_Header (T);
  cmd->Result_Predefined (H);

 // For each requested metric, try to pick up the associated data.
  Experiment *fw_experiment = exp->FW();
  CollectorGroup cgrp = exp->FW()->getCollectors();
  CollectorGroup::iterator ci;
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
    Collector c = *ci;

   // Evaluate the collector's time metric for all functions in the thread
    SmartPtr<std::map<Function, double> > data;
    ThreadGroup tgrp = fw_experiment->getThreads();
    if (tgrp.begin() == tgrp.end()) {
      break;
    }
    ThreadGroup::iterator ti = tgrp.begin();
// TODO: extend beyond the limited demo requirements.
    Thread t1 = *ti;

    try {
      Queries::GetMetricByFunctionInThread(c, "time", t1, data);
    }
    catch(const std::exception& error) {
      Mark_Cmd_With_Std_Error (cmd, error);
      return false;
    }

   // If there is no data, return.
    if (data->begin() == data->end()) {
      cmd->Result_String ( "(no data available)" );
      cmd->set_Status(CMD_ERROR);
      return false;
    }

   // Now we can sort the data.
    typedef std::pair<std::string, double> item_type;
    std::vector<item_type> items;
    for(std::map<Function, double>::const_iterator
        item = data->begin(); item != data->end(); ++item)
    {
      items.push_back( std::pair<std::string, double>(item->first.getName(), item->second ) );
    }

   // std::sort(items.begin(), items.end(), sort_ascending<item_type>());
    std::sort(items.begin(), items.end(), sort_decending<item_type>());

   // Extract the top "n" items fromt he sorted list.
    std::vector<item_type>::const_iterator it = items.begin();
    int64_t foundn = 0;
    for( ; it != items.end(); it++ ) {
      if (foundn++ >= topn) {
       // We ahve all that was asked for!
        break;
      }
      CommandResult_Columns *C = new CommandResult_Columns (2);
      CommandResult *F = new CommandResult_Float (it->second);
      CommandResult *S = new CommandResult_String (it->first);
      C->CommandResult_Columns::Add_Column (F);
      C->CommandResult_Columns::Add_Column (S);
      cmd->Result_Predefined (C);

//  printf("%s %f\n", it->first.c_str(), it->second );
    }

return true;
    for(std::map<Function, double>::const_iterator
          item = data->begin(); item != data->end(); ++item)
    {
      CommandResult_Columns *C = new CommandResult_Columns (2);
      CommandResult *F = new CommandResult_Float (item->second);
      CommandResult *S = new CommandResult_String (item->first.getName());
      C->CommandResult_Columns::Add_Column (F);
      C->CommandResult_Columns::Add_Column (S);
      cmd->Result_Predefined (C);
    }

  }
  return true;
}

bool SS_Determine_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname) {
  if (!strncasecmp (viewname.c_str(), "vtop", 4)) {
    int64_t topn = 0;
    for (int i = 4; i < viewname.length(); i++) {
      char c = viewname[i];
      if ((c >= *"0") && (c <= *"9")) {
        topn = (topn * 10) + (c - *"0");
      }
    }
    return VIEWN_vtop (cmd, exp, topn);
  }
  return true;
}
