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

// Global data definition
std::list<ViewType *> Available_Views;

// View management utilities

static bool Reaminaing_Length_Is_Numeric (std::string viewname, int64_t L) {
  for (int64_t i = L; i < viewname.length(); i++) {
    char a = viewname[i];
    if ((a < *"0") || (a > *"9")) return false;
  }
  return true;
}

static int64_t Get_Traling_Int (std::string viewname, int64_t start) {
  int64_t topn = 0;
  for (int i = start; i < viewname.length(); i++) {
    char c = viewname[i];
    if ((c >= *"0") && (c <= *"9")) {
      topn = (topn * 10) + (c - *"0");
    }
  }
  return topn;
}
 
ViewType *Find_View (std::string viewname) {
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    if (!strncasecmp (viewname.c_str(), vt->Unique_Name().c_str(), vt->Unique_Name().length()) &&
        Reaminaing_Length_Is_Numeric (viewname, vt->Unique_Name().length())) {
      return vt;
    }
  }

  return NULL;
}

void Add_Header (CommandObject *cmd, std::string *column_titles)
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

// Supporting semantic routines

void SS_Get_Views (CommandObject *cmd) {
 // List all known views.
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    cmd->Result_String ( vt->Unique_Name() );
  }
}

void SS_Get_Views (CommandObject *cmd, std::string collector_name) {
 // List all views that require a specific colelctor.
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    std::string *Collector_List = vt->Collectors();
    std::string C = Collector_List[0];
    while (C.length() != 0) {
      if ( !strcasecmp (C.c_str(), collector_name.c_str()) ) {
        cmd->Result_String ( vt->Unique_Name() );
      }
      C = *(++Collector_List);
    }
  }
  
}

void SS_Get_Views (CommandObject *cmd, OpenSpeedShop::Framework::Experiment *fexp) {
 // List all views that can be generate from the collectors used in an experiment.
  CollectorGroup cgrp;
  try {
    cgrp = fexp->getCollectors();
  }
  catch (const std::exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }

  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
   // All the required collectors must exist in the CollectorGroup.
    bool all_collectors_available = true;
    ViewType *vt = (*vi);
    std::string *Collector_List = vt->Collectors();
    std::string C = Collector_List[0];

    while (C.length() != 0) {
      bool required_collector_found = false;
      CollectorGroup::iterator ci;

      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        Metadata m = c.getMetadata();
        std::string collector_name = m.getUniqueId();
        if ( !strcasecmp (C.c_str(), collector_name.c_str()) ) {
          required_collector_found = true;
          break;
        }
      }

      if (required_collector_found) {
        C = *(++Collector_List);
        continue;
      } else {
        all_collectors_available = false;
        break;
      }
    }

    if (all_collectors_available) {
      cmd->Result_String ( vt->Unique_Name() );
    }
  }
  
}

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname) {
  // pcfun_view *pcf = new pcfun_view();
  // Define_New_View (pcf);

  ViewType *vt = Find_View (viewname);
  if (vt != NULL) {
    return vt->GenerateView (cmd, exp, Get_Traling_Int (viewname, vt->Unique_Name().length()));
  }

  cmd->Result_String ("The requested view is unavailable.");
  cmd->set_Status(CMD_ERROR);
  return false;
}

// Initialize definitions of the predefined views.
extern "C" void exp_LTX_ViewFactory ();
extern "C" void pcfunc_LTX_ViewFactory ();

void SS_Init_BuiltIn_Views () {
  exp_LTX_ViewFactory ();
  pcfunc_LTX_ViewFactory ();
}
