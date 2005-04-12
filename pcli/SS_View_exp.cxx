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

// experiment views

static std::string VIEW_exp_brief = "";
static std::string VIEW_exp_short = "Report the current state of an experiment.";
static std::string VIEW_exp_long  = "Report the current state of an experiment and describe the"
                                    " executable segments and data collection modules"
                                    " that are being used."
                                    " Executable modules are listed"
                                    " after an ""expAttach"" directive and any data collectors that"
                                    " are liked to that module are listed on the same line."
                                    " Data collectores that were, at one time, incldued in the"
                                    " experiment but are not currently linked to an excutable"
                                    " module are listed after an ""expDetach"" directive.";
static std::string VIEW_exp_collectors[] =
  { ""
  };
static std::string VIEW_exp_header[] =
  { ""
  };
static bool VIEW_exp (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
  char id[20]; sprintf(&id[0],"%lld",exp->ExperimentObject_ID());
  cmd->Result_String ("expDefine");
  std::string TmpDB = exp->Data_Base_Is_Tmp() ? "Temporary" : "Saved";
  cmd->Result_String ("{ # ExpId is " + std::string(&id[0])
                         + ", Status is " + exp->ExpStatus_Name()
                         + ", " + TmpDB + " Data Base is " + exp->Data_Base_Name ());
  try {
      if (exp->FW() != NULL) {
        ThreadGroup tgrp = exp->FW()->getThreads();
        ThreadGroup::iterator ti;
        bool atleastone = false;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          std::string host = t.getHost();
          pid_t pid = t.getProcessId();
          if (!atleastone) {
            atleastone = true;
          }
          char spid[20]; sprintf(&spid[0],"%lld",pid);
          std::string S = "  expAttach -h " + host + " -p " + std::string(&spid[0]);
          std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
          if (pthread.first) {
            char tid[20]; sprintf(&tid[0],"%lld",pthread.second);
            S = S + std::string(&tid[0]);
          }
#ifdef HAVE_MPI
          std::pair<bool, int> rank = t.getMPIRank();
          if (rank.first) {
            char rid[20]; sprintf(&rid[0],"%lld",rank.second);
            S = S + std::string(&rid[0]);
          }
#endif
          CollectorGroup cgrp = t.getCollectors();
          CollectorGroup::iterator ci;
          int collector_count = 0;
          for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
            Collector c = *ci;
            Metadata m = c.getMetadata();
            if (collector_count) {
              S = S + ",";
            } else {
              S = S + " ";
              collector_count = 1;
            }
            S = S + m.getUniqueId();
          }
          cmd->Result_String ( S );
        }

        CollectorGroup cgrp = exp->FW()->getCollectors();
        CollectorGroup::iterator ci;
        atleastone = false;
        std::string S ;
        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector c = *ci;
          ThreadGroup tgrp = c.getThreads();
          if (tgrp.empty()) {
            Metadata m = c.getMetadata();
            if (atleastone) {
              S = S + ",";
            } else {
              S = S + "  expDetach ";
              atleastone = true;
            }
            S = S + m.getUniqueId();
          }
        }
        if (atleastone) {
          cmd->Result_String ( S );
        }
      }
    cmd->Result_String ( "}");
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
    cmd->Result_String ( "}");
    return false;
  }

  return true;
}
class exp_view : public ViewType {

 public: 
  exp_view() : ViewType ("exp",
                          VIEW_exp_brief,
                          VIEW_exp_short,
                          VIEW_exp_long,
                         &VIEW_exp_collectors[0],
                         &VIEW_exp_header[0]) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    return VIEW_exp (cmd, exp, topn);
  }
};


static std::string VIEW_allexp_brief = "";
static std::string VIEW_allexp_short = "Report the current state of all experiments.";
static std::string VIEW_allexp_long  = "Report the current state of all experiments and describe the"
                                       " executable segments and data collection modules that are being"
                                       " used."
                                       " Executable modules are listed, after an ""expAttach"" directive,"
                                       " and any data collectors that are liked to that module are listed"
                                       " on the same line. Data collectors that were, at one time, incldued"
                                       " in the experiment but are not currently linked to an excutable"
                                       " module are listed after an ""expDetach"" directive.";
static std::string VIEW_allexp_collectors[] =
  { ""
  };
static std::string VIEW_allexp_header[] =
  { ""
  };
static bool VIEW_allexp (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
  std::list<ExperimentObject *>::reverse_iterator expi;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
  {
    (void)VIEW_exp (cmd, *expi, topn);
  }
  return true;
}
class allexp_view : public ViewType {

 public: 
  allexp_view() : ViewType ("allexp",
                             VIEW_allexp_brief,
                             VIEW_allexp_short,
                             VIEW_allexp_long,
                            &VIEW_allexp_collectors[0],
                            &VIEW_allexp_header[0],
                             false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    return VIEW_allexp (cmd, exp, topn);
  }
};


static std::string VIEW_expstatus_brief = "ExperimentID : Current Status";
static std::string VIEW_expstatus_short = "Report the current status of all known experiments.";
static std::string VIEW_expstatus_long  = "Check each executable segment that is part of each"
                                          " experiment an report the current status of each experiment.";
static std::string VIEW_expstatus_collectors[] =
  { ""
  };
static std::string VIEW_expstatus_header[] =
  { "               ExpID",
    "Current Status      ",
    ""
  };
static bool VIEW_expstatus (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
  if (ExperimentObject_list.rbegin() != ExperimentObject_list.rend()) {
   // Build a Header for the table - just two items - time and function name.
    Add_Header (cmd, &VIEW_expstatus_header[0]);

    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
     // Return the EXPID  and Status for every known experiment.
      CommandResult_Columns *C = new CommandResult_Columns (2);
      C->CommandResult_Columns::Add_Column (new CommandResult_Int ((*expi)->ExperimentObject_ID()));
      C->CommandResult_Columns::Add_Column (new CommandResult_String ((*expi)->ExpStatus_Name()));
      cmd->Result_Predefined (C);
    }
  }
  return true;
}
class expstatus_view : public ViewType {

 public: 
  expstatus_view() : ViewType ("expstatus",
                                VIEW_expstatus_brief,
                                VIEW_expstatus_short,
                                VIEW_expstatus_long,
                               &VIEW_expstatus_collectors[0],
                               &VIEW_expstatus_header[0],
                                false) {
  }
  virtual bool GenerateView (CommandObject *cmd, ExperimentObject *exp, int64_t topn) {
    return VIEW_expstatus (cmd, exp, topn);
  }
};


// This is the only external entrypoint.
// Calls to the VIEWs needs to be done through the ViewType class objects.
extern "C" void exp_LTX_ViewFactory () {
  Define_New_View (new exp_view());
  Define_New_View (new allexp_view());
  Define_New_View (new expstatus_view());
}
