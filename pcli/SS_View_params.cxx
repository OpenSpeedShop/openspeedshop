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

static CommandResult *Get_Collector_Metadata (Collector c, Metadata m) {
  CommandResult *Param_Value = NULL;
  std::string id = m.getUniqueId();
  if( m.isType(typeid(int)) ) {
    int Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(int64_t)) ) {
    int64_t Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Int (Value);
  } else if( m.isType(typeid(unsigned int)) ) {
    uint Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(uint64_t)) ) {
    int64_t Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Uint (Value);
  } else if( m.isType(typeid(float)) ) {
    float Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(double)) ) {
    double Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_Float (Value);
  } else if( m.isType(typeid(string)) ) {
    std::string Value;
    c.getParameterValue(id, Value);
    Param_Value = new CommandResult_String (Value);
  } else {
    Param_Value = new CommandResult_String("Unknown type.");
  }
  return Param_Value;
}

// parameter views

static std::string VIEW_params_brief = "collector : parameter : value";
static std::string VIEW_params_short = "Report the current values of parameters.";
static std::string VIEW_params_long  = "Report the current values assigned to all the parameters"
                                       " that are part of a specific experiment.";
static std::string VIEW_params_metrics[] =
  { ""
  };
static std::string VIEW_params_collectors[] =
  { ""
  };
static std::string VIEW_params_header[] =
  { "Collector           ",
    "Parameter           ",
    "               Value",
    ""
  };
static bool VIEW_params (CommandObject *cmd, ExperimentObject *exp) {
  CollectorGroup cgrp;
  try {
    cgrp = exp->FW()->getCollectors();
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return false;
  }

 // Build a Header for the table.
  Add_Header (cmd, &VIEW_params_header[0]);

  CollectorGroup::iterator ci = cgrp.begin();
  for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
    Collector c = *ci;
    Metadata cm = c.getMetadata();
    std::set<Metadata> md = c.getParameters();
    std::set<Metadata>::const_iterator mi;
    for (mi = md.begin(); mi != md.end(); mi++) {
      Metadata m = *mi;
      CommandResult_Columns *C = new CommandResult_Columns (3);
      std::string S = cm.getUniqueId();
      for(int i = S.length(); i < 20; i++) S += " ";
      C->CommandResult_Columns::Add_Column (new CommandResult_String (S));
      S = m.getUniqueId();
      for(int i = S.length(); i < 20; i++) S += " ";
      C->CommandResult_Columns::Add_Column (new CommandResult_String (S));
      C->CommandResult_Columns::Add_Column (Get_Collector_Metadata (c, m));
      cmd->Result_Predefined (C);
    }
  }

  return true;
}
class params_view : public ViewType {

 public: 
  params_view() : ViewType ("params",
                             VIEW_params_brief,
                             VIEW_params_short,
                             VIEW_params_long,
                            &VIEW_params_metrics[0],
                            &VIEW_params_collectors[0],
                            &VIEW_params_header[0],
                             false,
                             false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn,
                         ThreadGroup tgrp, std::vector<Collector> CV, std::vector<std::string> MV) {
    return VIEW_params (cmd, exp);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void params_LTX_ViewFactory () {
  Define_New_View (new params_view());
}
