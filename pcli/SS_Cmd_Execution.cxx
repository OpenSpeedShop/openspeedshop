#include "SS_Input_Manager.hxx"

void Execute_Cmd (CommandObject *cmd) {
  bool cmd_successful = false;

  switch (cmd->Type()) {
  case CMD_EXP_CREATE:
    cmd_successful = Experiment_Create (cmd);
    break;
  default:
    fprintf(stderr,"Command %d has not been implimented yet\n",cmd->Type());
  }

 // For DEBUGING:
  if (cmd_successful) cmd->Print(stdout);
}
