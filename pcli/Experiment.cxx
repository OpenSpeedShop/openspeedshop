#include "SS_Input_Manager.hxx"

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
EXPID Experiment_Sequence_Number = 0;
std::list<ExperimentObject *> ExperimentObject_list;


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

#define Define_Line_Info( a, b, c) (b = a->Clip(), \
                                    c = ((b != NULL) ? b->Who() : 0))

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

bool SS_Record (CommandObject *cmd) {
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;

  char *tofile = NULL;

  if (tofile == NULL) {
   (void)SpeedShop_Trace_OFF ();
  } else {
    (void)SpeedShop_Trace_ON(tofile);
  }

 // There is no result returned for this command.

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


