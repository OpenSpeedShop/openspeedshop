#include "SS_Input_Manager.hxx"

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
std::list<ExperimentObject *> ExperimentObject_list;

// Experiment Building Block Commands

ExperimentObject *Find_Experiment_Object (EXPID ExperimentID)
{
// Search for existing entry.
  if (ExperimentID > 0) {
    std::list<ExperimentObject *>::iterator exp;
    for (exp = ExperimentObject_list.begin(); exp != ExperimentObject_list.end(); exp++) {
      // if (ExperimentID == ((*exp)->Exp_ID)) {
      if (ExperimentID == (*exp)->ExperimentObject_ID()) {
        return *exp;
      }
    }
  }

  return NULL;
}

// Low Level Semantic Routines

bool SS_expAttach (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  ApplicationGroupObject *App = NULL;
  Collector *Inst = NULL;

  CommandResult *cmr = NULL;
  ExperimentObject *exp = NULL;
  EXPID ExperimentID = 0;

  if (ExperimentID <= 0) {
   // Use the current focused experiment.
    ExperimentID =  Experiment_Focus (WindowID);
  }
  if (ExperimentID > 0) {
    exp = Find_Experiment_Object (ExperimentID);
  }
  if (!exp) {
   // There is no existing experiment or something went wrong.  Allocate a new Experiment.
    exp = new ExperimentObject ();
   // When we allocate a new experiment, set the focus to point to it.
    (void)Experiment_Focus (WindowID, exp->ExperimentObject_ID());
  }
  if (exp == NULL) {
    cmd->Result_String ("The requested experiment could not be found");
    cmd->set_Status(CMD_ERROR);
    return false;
  }
  if (App != NULL) {
    exp->ExperimentObject_Add_Application (App);
  }
  if (Inst != NULL) {
    exp->ExperimentObject_Add_Instrumention (Inst);
  }

 // Return the EXPID for this command.
  cmd->Result_Int (exp->ExperimentObject_ID());
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expClose (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expCreate (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  CommandResult *cmr = NULL;
  ExperimentObject *exp = NULL;
  EXPID ExperimentID = 0;

  if (ExperimentID <= 0) {
   // There is no specified experiment.  Allocate a new Experiment.
    exp = new ExperimentObject ();
   // When we allocate a new experiment, set the focus to point to it.
    (void)Experiment_Focus (WindowID, exp->ExperimentObject_ID());
  } else {
    exp = Find_Experiment_Object (ExperimentID);
  }
//  if (App != NULL) {
//    exp->ExperimentObject_Add_Application (App);
//  }
//  if (Inst != NULL) {
//    exp->ExperimentObject_Add_Instrumention (Inst);
//  }

 // Return the EXPID for this command.
  cmd->Result_Int (exp->ExperimentObject_ID());
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expDetach (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expDisable (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expEnable (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expFocus  (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  CommandResult *cmr = NULL;
  ExperimentObject *exp = NULL;
  EXPID ExperimentID = 0;

  if (ExperimentID == 0) {
    ExperimentID = Experiment_Focus ( WindowID );
  } else {
    ExperimentID = Experiment_Focus ( WindowID, ExperimentID);
  }

 // Return the EXPID for this command.
  cmd->Result_Int (ExperimentID);
  cmd->set_Status(CMD_COMPLETE);

  return true;
} 

bool SS_expGo (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expPause (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expRestore (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expSave (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expSetParam (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_expView (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// Information Commands

bool SS_ListBreaks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListExp (CommandObject *cmd) {
  std::list<ExperimentObject *>::reverse_iterator expi;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
  {
   // Return the EXPID for every known experiment
    cmd->Result_Int ((*expi)->ExperimentObject_ID());
  }

  cmd->set_Status(CMD_COMPLETE);
  return true;
}

bool SS_ListHosts (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListObj (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListPids (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListMetrics (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListParams (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListReports (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListSrc (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_ListTypes (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// Session Commands

bool SS_ClearBreaks (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_Exit (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_Help (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_History (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

 // Default action with no arguments: Dump the history file to stdout
  bool R = Command_Trace (cmd, CMDW_TRACE_ORIGINAL_COMMANDS, WindowID, std::string(""));

  cmd->set_Status( R ? CMD_COMPLETE : CMD_ERROR);
  return true;
}

bool SS_Log (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_OpenGui (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_Playback (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  char *tofile = NULL;

  if (tofile == NULL) {
   (void)Command_Trace_OFF (WindowID);
  } else {
    (void)Command_Trace_ON(WindowID, tofile);
  }

 // There is no result returned for this command.

  return true;
}

bool SS_SetBreak (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  cmd->Result_String ("not yet implemented");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

//
// Dummy routines for commands not finished yet.
//

// No value returned to python.
bool SS_no_value (CommandObject *cmd) {

 // There is no result returned for this command.
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// String value returned to python.
bool SS_string_value (CommandObject *cmd) {

  cmd->Result_String ("fake string");
  cmd->set_Status(CMD_COMPLETE);

  return true;
}

// Integer (64 bit) value returned to python..
bool SS_int_value (CommandObject *cmd) {

  cmd->Result_Int (-1);
  cmd->set_Status(CMD_COMPLETE);

  return true;
}


