#include "SS_Input_Manager.hxx"

void SS_Execute_Cmd (CommandObject *cmd) {
  bool cmd_successful = false;
  InputLineObject *clip = cmd->Clip();

  switch (clip->What()) {
  case ILO_IN_PARSER:
    clip->SetStatus (ILO_EXECUTING);
    break;
  case ILO_EXECUTING:
    break;
  case ILO_COMPLETE:
  case ILO_ERROR:
    cmd->set_Status (CMD_ABORTED);
    return;
  }

  switch (cmd->Type()) {
  case CMD_EXP_ATTACH:
    cmd_successful = SS_expAttach (cmd);
    break;
  case CMD_EXP_CREATE:
    cmd_successful = SS_expCreate (cmd);
    break;
  case CMD_EXP_FOCUS:
    cmd_successful = SS_expFocus (cmd);
    break;
  case CMD_RECORD:
    cmd_successful = SS_Record (cmd);
    break;
  default:
    fprintf(stderr,"Command %d has not been implimented yet\n",cmd->Type());
  }

  if (!cmd_successful) {
    cmd->set_Status (CMD_ERROR);
  }
 // For DEBUGING:
 // if (!cmd_successful) fprintf(stdout,"Command Error: ");
 // cmd->Print(stdout);
}
