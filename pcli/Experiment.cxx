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

CommandResult *Experiment_Create (CMDWID WindowID, EXPID ExperimentID,
                                ApplicationGroupObject *App, Collector *Inst)
{
  ExperimentObject *exp = NULL;
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
    return new CommandResult_String("The requested experiment could not be found");
  }
  if (App != NULL) {
    exp->ExperimentObject_Add_Application (App);
  }
  if (Inst != NULL) {
    exp->ExperimentObject_Add_Instrumention (Inst);
  }
  return new  CommandResult_Int (exp->ExperimentObject_ID());
}

bool Experiment_Create (CommandObject *cmd) {
  CommandResult *cmr = NULL;
  InputLineObject *clip = cmd->Clip();
  CMDWID WindowID = (clip != NULL) ? clip->Who() : 0;
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
  cmr = new CommandResult_Int (exp->ExperimentObject_ID());
  cmd->Add_Result(cmr);
  cmd->set_Status(CMD_COMPLETE);

  return true;
}
