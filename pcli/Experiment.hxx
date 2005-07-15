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

// ExperimentObject
// Note: ALL the instrumentation in the experiment's list is applied
//       to ALL of the executables in the experiment's list.
class ExperimentObject;
extern EXPID Experiment_Sequence_Number;
extern std::list<ExperimentObject *> ExperimentObject_list;

#define ExpStatus_NonExistent 0
#define ExpStatus_Paused      1
#define ExpStatus_Running     2
#define ExpStatus_Terminated  3
#define ExpStatus_InError     4

class ExperimentObject
{
 private:
  EXPID Exp_ID;
  int ExpStatus;
  bool Data_File_Has_A_Generated_Name;
  OpenSpeedShop::Framework::Experiment *FW_Experiment;

 public:
  ExperimentObject (std::string data_base_name = std::string("")) {
    Exp_ID = ++Experiment_Sequence_Number;
    ExpStatus = ExpStatus_Paused;

   // Allocate a data base file for the information connected with the experiment.
    std::string Data_File_Name;
    if (data_base_name.length() == 0) {
      char base[20];
      snprintf(base, 20, "ssdb%lld.XXXXXX",Exp_ID);
      Data_File_Name = std::string(tempnam ("/tmp/", &base[0] ) ) + ".openss";
      Data_File_Has_A_Generated_Name = true;
      try {
        OpenSpeedShop::Framework::Experiment::create (Data_File_Name);
      }
      catch(const Exception& error) {
       // Don't really care why.
       // Calling routine must handle the problem.
        Exp_ID = 0;
        Data_File_Has_A_Generated_Name = false;
        FW_Experiment = NULL;
      }
    } else {
      Data_File_Name = data_base_name;
      Data_File_Has_A_Generated_Name = false;
    }

   // Create and open an experiment
    try {
      FW_Experiment = new OpenSpeedShop::Framework::Experiment (Data_File_Name);
      ExperimentObject_list.push_front(this);
    }
    catch(const Exception& error) {
     // Don't really care why.
     // Calling routine must handle the problem.
      Exp_ID = 0;
      Data_File_Has_A_Generated_Name = false;
      FW_Experiment = NULL;
    }

  }
  ~ExperimentObject () {
    ExperimentObject_list.remove (this);
    if (FW_Experiment != NULL) {
      try {
        if (Data_File_Has_A_Generated_Name) {
          FW_Experiment->remove (FW_Experiment->getName());
        }
        delete FW_Experiment;
      }
      catch(const Exception& error) {
       // Don't really care why.
      }
      Data_File_Has_A_Generated_Name = false;
      FW_Experiment = NULL;
    }
    Exp_ID = 0;
    ExpStatus = ExpStatus_NonExistent;
  }

  EXPID ExperimentObject_ID() {return Exp_ID;}
  Experiment *FW() {return FW_Experiment;}
  bool Data_Base_Is_Tmp () {return Data_File_Has_A_Generated_Name;}
  std::string Data_Base_Name () {
    if (FW() == NULL) {
      return "";
    } else {
      try {
        return FW()->getName();
      }
      catch(const Exception& error) {
       // Don't really care why.
        return "(Unable to determine.)";
      }
    }
  }
  int Status() {return ExpStatus;}
  int Determine_Status() {
    int S = ExpStatus;
    if (FW() == NULL) {
      ExpStatus = ExpStatus_NonExistent;
    } else {
      ThreadGroup tgrp = FW()->getThreads();
      int A = ExpStatus_NonExistent;
      if (tgrp.empty()) {
        A = ExpStatus_Paused;
      } else {
        ThreadGroup::iterator ti;
        for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
          Thread t = *ti;
          try {
            if (t.getState() == Thread::Running) {
             // if any thread is Running, the experiment is also.
              A = ExpStatus_Running;
              break;
            } else if (t.getState() == Thread::Suspended) {
             // Paused can override Terminated
              A = ExpStatus_Paused;
            } else if (t.getState() == Thread::Terminated) {
             // The experiment is terminated only if all the threads are.
              if (A != ExpStatus_Paused) {
                A = ExpStatus_Terminated;
              }
            } else if ((t.getState() == Thread::Connecting) ||
                       (t.getState() == Thread::Disconnected) ||
                       (t.getState() == Thread::Nonexistent)) {
             // These are 'Don't care" states at the user level.
             // Note: we might default to ExpStatus_NonExistent.
            } else {
              A = ExpStatus_InError;
              break;
            }
          }
          catch(const Exception& error) {
           // Don't really care why.
           // Mark the experiment with an error and continue on.
            A = ExpStatus_InError;
            break;
          }
        }
      }
      ExpStatus = A;
    }
    return ExpStatus;
  }

  void setStatus (int S) {ExpStatus = S;}

  void RenameDB (std::string New_DB) {
   // Rename the data base file.
    if (FW_Experiment != NULL) {
      FW_Experiment->renameTo(New_DB);
      Data_File_Has_A_Generated_Name = false;
    }
  }

  void CopyDB (std::string New_DB) {
   // Make a copy of the data base file.
    if (FW_Experiment != NULL) {
      FW_Experiment->copyTo(New_DB);
    }
  }

  std::string ExpStatus_Name () {
    Determine_Status();
    if ((this == NULL) || (ExpStatus == ExpStatus_NonExistent)) return std::string("NonExistent");
    if (ExpStatus == ExpStatus_Paused) return std::string("Paused");
    if (ExpStatus == ExpStatus_Running) return std::string("Running");
    if (ExpStatus == ExpStatus_Terminated) return std::string("Terminated");
    if (ExpStatus == ExpStatus_InError) return std::string("Error");
    return std::string("Unknown");
  }

  void Print(ostream &mystream) {
    mystream << "Experiment " << ExperimentObject_ID() << " " << ExpStatus_Name() << " data->";
    mystream << ((FW_Experiment != NULL) ? FW_Experiment->getName() : "(null)") << std::endl;
    if (FW_Experiment != NULL) {
      ThreadGroup tgrp = FW_Experiment->getThreads();
      ThreadGroup::iterator ti;
      bool atleastone = false;
      for (ti = tgrp.begin(); ti != tgrp.end(); ti++) {
        Thread t = *ti;
        std::string host = t.getHost();
        pid_t pid = t.getProcessId();
        if (!atleastone) {
          atleastone = true;
        }
        mystream << "    -h " << host << " -p " << pid;
        std::pair<bool, pthread_t> pthread = t.getPosixThreadId();
        if (pthread.first) {
          mystream << " -t " << pthread.second;
        }
#ifdef HAVE_MPI
        std::pair<bool, int> rank = t.getMPIRank();
        if (rank.first) {
          mystream << " -r " << rank.second;
        }
#endif
        CollectorGroup cgrp = t.getCollectors();
        CollectorGroup::iterator ci;
        int collector_count = 0;
        for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
          Collector c = *ci;
          Metadata m = c.getMetadata();
          if (collector_count) {
            mystream << ",";
          } else {
            mystream << " ";
            collector_count = 1;
          }
          mystream << " " << m.getUniqueId();
        }
        mystream << std::endl;
      }

      CollectorGroup cgrp = FW_Experiment->getCollectors();
      CollectorGroup::iterator ci;
      atleastone = false;
      for (ci = cgrp.begin(); ci != cgrp.end(); ci++) {
        Collector c = *ci;
        ThreadGroup tgrp = c.getThreads();
        if (tgrp.empty()) {
          Metadata m = c.getMetadata();
          if (atleastone) {
            mystream << ",";
          } else {
            mystream << "   ";
            atleastone = true;
          }
          mystream << " " << m.getUniqueId();
        }
      }
      if (atleastone) {
        mystream << std::endl;
      }
    }
  }
  static void Dump(ostream &mystream) {
    std::list<ExperimentObject *>::reverse_iterator expi;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
    {
      (*expi)->Print(mystream);
    }
  }
};

// Make sure all experiments are closed and associated files freed.
void Experiment_Termination ();

// Experiment Utilities
ExperimentObject *Find_Experiment_Object (EXPID ExperimentID);

bool Collector_Used_In_Experiment (OpenSpeedShop::Framework::Experiment *fexp, std::string myname);
Collector Get_Collector (OpenSpeedShop::Framework::Experiment *fexp, std::string myname);
void Filter_ThreadGroup (CommandObject *cmd, ThreadGroup& tgrp);

inline void Mark_Cmd_With_Std_Error (CommandObject *cmd, const Exception& error) {
   cmd->Result_String ( error.getDescription() );
   cmd->set_Status(CMD_ERROR);
   return;
}


// Experiment level commands
bool SS_expAttach (CommandObject *cmd);
bool SS_expClose (CommandObject *cmd);
bool SS_expCreate (CommandObject *cmd);
bool SS_expDetach (CommandObject *cmd);
bool SS_expDisable (CommandObject *cmd);
bool SS_expEnable (CommandObject *cmd);
bool SS_expFocus  (CommandObject *cmd);
bool SS_expGo (CommandObject *cmd);
bool SS_expPause (CommandObject *cmd);
bool SS_expRestore (CommandObject *cmd);
bool SS_expSave (CommandObject *cmd);
bool SS_expSetParam (CommandObject *cmd);
bool SS_expView (CommandObject *cmd);

// Information level commands
bool SS_ListBreaks (CommandObject *cmd);
bool SS_ListExp (CommandObject *cmd);
bool SS_ListHosts (CommandObject *cmd);
bool SS_ListObj (CommandObject *cmd);
bool SS_ListPids (CommandObject *cmd);
bool SS_ListMetrics (CommandObject *cmd);
bool SS_ListParams (CommandObject *cmd);
bool SS_ListRanks (CommandObject *cmd);
bool SS_ListSrc (CommandObject *cmd);
bool SS_ListStatus (CommandObject *cmd);
bool SS_ListThreads (CommandObject *cmd);
bool SS_ListTypes (CommandObject *cmd);
bool SS_ListViews (CommandObject *cmd);

// Session level commands
bool SS_ClearBreaks (CommandObject *cmd);
bool SS_Exit (CommandObject *cmd);
bool SS_Help (CommandObject *cmd);
bool SS_History (CommandObject *cmd);
bool SS_Log (CommandObject *cmd);
bool SS_OpenGui (CommandObject *cmd);
bool SS_Playback (CommandObject *cmd);
bool SS_Record (CommandObject *cmd);
bool SS_SetBreak (CommandObject *cmd);

// Place holders for comands that are not yet implemented
bool SS_no_value    	(CommandObject *cmd);
bool SS_string_value    (CommandObject *cmd);
bool SS_int_value   	(CommandObject *cmd);
