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

// Local utilities

static int64_t Get_Trailing_Int (std::string viewname, int64_t start) {
  int64_t topn = 0;
  for (int i = start; i < viewname.length(); i++) {
    char c = viewname[i];
    if ((c >= *"0") && (c <= *"9")) {
      topn = (topn * 10) + (c - *"0");
    }
  }
  return topn;
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
 // List all views that require a specific collector.
  std::list<ViewType *>::iterator vi;
  for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
    ViewType *vt = (*vi);
    std::string *Collector_List = vt->Collectors();
    std::string *Metric_List = vt->Metrics();
    bool all_collectors_available = false;

    if (Collector_List[0].length() > 0) {
     // Check that all the required collectors are available.
      std::string cs = Collector_List[0];
      all_collectors_available = true;

      while (cs.length() != 0) {
        if ( strcasecmp (cs.c_str(), collector_name.c_str()) ) {
          all_collectors_available = false;
          break;
        }
        cs = *(++Collector_List);
      }

    }

    if (!all_collectors_available &&
        (Metric_List[0].length() > 0)) {
     // Look for an available collector that produces the required metrics.
      try {
        std::string tmpdb = std::string("./ssdbtmpview.openss");
        OpenSpeedShop::Framework::Experiment::create (tmpdb);
        OpenSpeedShop::Framework::Experiment *fw_exp = new OpenSpeedShop::Framework::Experiment (tmpdb);
        Collector C = Get_Collector (fw_exp, collector_name);
        if (Collector_Generates_Metrics ( C, &Metric_List[0] )) {
          all_collectors_available = true;
        }
        delete fw_exp;
        (void) remove (tmpdb.c_str());
      }
      catch(const Exception& error) {
       // Guess not.
        all_collectors_available = false;
      }
    }

    if (all_collectors_available) {
        cmd->Result_String ( vt->Unique_Name() );
    }
  }
  
}

void SS_Get_Views (CommandObject *cmd, OpenSpeedShop::Framework::Experiment *fexp, std::string s) {
 // List all views that can be generate from the collectors used in an experiment.
 // Skip views that don't depend on any collector.
  CollectorGroup cgrp;
  try {
    cgrp = fexp->getCollectors();

    std::list<ViewType *>::iterator vi;
    for (vi = Available_Views.begin(); vi != Available_Views.end(); vi++) {
     // All the required collectors must exist in the CollectorGroup.
      ViewType *vt = (*vi);
      std::string *Collector_List = vt->Collectors();
      std::string *Metric_List = vt->Metrics();
      bool all_collectors_available = false;

      if (Collector_List[0].length() != 0) {
       // Check that all the required collectors are available.
        std::string C = Collector_List[0];
        all_collectors_available = true;

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

      }

      if (!all_collectors_available &&
          (Metric_List[0].length() > 0)) {
       // Look for an available collector that produces the required metrics.
        CollectorGroup::iterator ci;

        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector C = *ci;
          if (Collector_Generates_Metrics ( C, &Metric_List[0] )) {
            all_collectors_available = true;
            break;
          }
        }

      }

      if (all_collectors_available) {
        cmd->Result_String ( s + vt->Unique_Name() );
      }
    }
  }
  catch (const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    return;
  }
  
}

bool SS_Generate_View (CommandObject *cmd, ExperimentObject *exp, std::string viewname) {
  std::vector<Collector> CV;
  std::vector<std::string> MV;
  std::vector<ViewInstruction *>IV;
 
  OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
  Assert(p_result != NULL);

 // Determine the availability of the view.
  ViewType *vt = Find_View (viewname);
  if (vt == NULL) {
#if 1
    std::string s("The requested view is unavailable.");
    Mark_Cmd_With_Soft_Error(cmd,s);
#else
    cmd->Result_String ("The requested view is unavailable.");
    cmd->set_Status(CMD_ERROR);
#endif
    return false;
  }

 // Check for a required experiment.
  if (vt->Need_Exp()) {
    if (exp == NULL) {
     // The requested view requires an ExperimentObject.
#if 1
      std::string s("An Experiment has not been specified.");
      Mark_Cmd_With_Soft_Error(cmd,s);
#else
      cmd->Result_String ("An Experiment has not been specified.");
      cmd->set_Status(CMD_ERROR);
#endif
      return false;
    } else if (exp->FW() == NULL) {
     // There should always be a link to the FrameWork.
#if 1
      std::string s("The experiment has been disconnected from the FrameWork.");
      Mark_Cmd_With_Soft_Error(cmd,s);
#else
      cmd->Result_String ("The experiment has been disconnected from the FrameWork.");
      cmd->set_Status(CMD_ERROR);
#endif
      return false;
    }
  }

 // Determine the required Collectors.
  vector<ParseRange> *p_slist = p_result->getexpMetricList();
  if (p_slist->begin() != p_slist->end()) {
   // Prefer user specified collector::metric specifications.
    int64_t i = 0;
    vector<ParseRange>::iterator mi;
    for (mi = p_slist->begin(); mi != p_slist->end(); mi++) {
      parse_range_t *m_range = (*mi).getRange();
      std::string C_Name;
      std::string M_Name;
      if (m_range->is_range) {
        C_Name = m_range->start_range.name;
        M_Name = m_range->end_range.name;
        if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
#if 1
      	  std::string s("The specified collector, " + C_Name + 
	    	    	", was not used in the experiment.");
      	  Mark_Cmd_With_Soft_Error(cmd,s);
#else
          cmd->Result_String ("The specified collector, " + C_Name + ", was not used in the experiment.");
          cmd->set_Status(CMD_ERROR);
#endif
          return false;
        }
      } else {
        M_Name = m_range->start_range.name;
        if ((exp != NULL) &&
            (exp->FW() != NULL)) {
          CollectorGroup cgrp = exp->FW()->getCollectors();
          C_Name = Find_Collector_With_Metric ( cgrp, M_Name);
          if (C_Name.length() == 0) {
#if 1
    	    std::string s("The specified metric, " + M_Name + 
	    	    	    " was not generated for the experiment.");
    	    Mark_Cmd_With_Soft_Error(cmd,s);
#else
            cmd->Result_String ("The specified metric, " + M_Name + " was not generated for the experiment.");
            cmd->set_Status(CMD_ERROR);
#endif
            return false;
          }
        }
      }

      Collector C = Get_Collector (exp->FW(), C_Name);
      if (!Collector_Generates_Metric ( C, M_Name)) {
#if 1
    	std::string s("The specified collector, " + C_Name +
    	    	      ", does not generate the specified metric, " + M_Name);
    	Mark_Cmd_With_Soft_Error(cmd,s);
#else
        cmd->Result_String ("The specified collector, " + C_Name +
                            ", does not generate the specified metric, " + M_Name);
        cmd->set_Status(CMD_ERROR);
#endif
        return false;
      }

      CV.push_back(C);
      MV.push_back(M_Name);
      IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, i, i));
      i++;
    }
  } else {
   // Use the metrics specified in the experiment definition.
    bool collector_found = false;
    std::string *Metric_List = vt->Metrics();
    std::string *Collector_List = vt->Collectors();
    std::string C_Name = Collector_List[0];
    if ((Collector_List[0].length() != 0) &&
        (Collector_List[1].length() == 0)) {
     // There is a single, required collector.
       C_Name = vt->Unique_Name();
       collector_found = true;
    } else if (!collector_found &&
                 (Metric_List[0].length() > 0) &&
                 (exp != NULL) &&
                 (exp->FW() != NULL)) {
     // Look for a collector that produces the required metrics.
      CollectorGroup cgrp = exp->FW()->getCollectors();
      C_Name = Find_Collector_With_Metrics (cgrp, &Metric_List[0]);
      collector_found = (C_Name.length() > 0);
    } else {
     // We handle multiple collectors by letting the View figure out what to do.
     // This also includes Views that require no collectors.
    }

    if (vt->Need_Exp() &&
        ((Collector_List[0].length() != 0) ||
         (Metric_List[0].length() != 0))) {
      if (!collector_found) {
#if 1
    	std::string s("The metrics required to generate the view are not available in the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
#else
        cmd->Result_String ("The metrics required to generate the view are not available in the experiment.");
        cmd->set_Status(CMD_ERROR);
#endif
        return false;
      }
      if (!Collector_Used_In_Experiment (exp->FW(), C_Name)) {
#if 1
    	std::string s("The required collector, " + C_Name + ", was not used in the experiment.");
    	Mark_Cmd_With_Soft_Error(cmd,s);
#else
        cmd->Result_String ("The required collector, " + C_Name + ", was not used in the experiment.");
        cmd->set_Status(CMD_ERROR);
#endif
        return false;
      }
    }

   // Determine the collectors and metrics that are required.
    if (vt->Deterimne_Metrics()  &&
        (exp != NULL) &&
        (exp->FW() != NULL)) {
      int64_t column = 0;
      CollectorGroup cgrp = exp->FW()->getCollectors();
      CollectorGroup::iterator ci;
      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        Metadata cm = c.getMetadata();
        std::set<Metadata> md = c.getMetrics();
        std::set<Metadata>::const_iterator mi;
        for (mi = md.begin(); mi != md.end(); mi++) {
          Metadata m = *mi;
          CV.push_back(c);
          MV.push_back(m.getUniqueId());
          IV.push_back(new ViewInstruction (VIEWINST_Display_Metric, column, column));
          column++;
        }
      }
    }

  }

 // Determine threads that are required.
  ThreadGroup tgrp;
  if ((exp != NULL) &&
      (exp->FW() != NULL)) {
    tgrp = exp->FW()->getThreads();
  }
  Filter_ThreadGroup (cmd, tgrp);

 // Try to Generate the Requested View!
  bool V = vt->GenerateView (cmd, exp, Get_Trailing_Int (viewname, vt->Unique_Name().length()),
                             tgrp, CV, MV, IV);
  return V;
}

// Initialize definitions of the predefined views.
extern "C" void stats_LTX_ViewFactory ();
void SS_Load_View_plugins ();

void SS_Init_BuiltIn_Views () {
  stats_LTX_ViewFactory ();  // This is the generic, built-in view
}
