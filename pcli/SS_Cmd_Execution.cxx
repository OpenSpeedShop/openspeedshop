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
  case CMD_EXP_CLOSE:
  case CMD_EXP_DETACH:
  case CMD_EXP_DISABLE:
  case CMD_EXP_ENABLE:
  case CMD_EXP_PAUSE:
  case CMD_EXP_GO:
  case CMD_EXP_SAVE:
  case CMD_EXP_SETPARAM:
  case CMD_EXP_VIEW:
  case CMD_CLEAR_BREAK:
  case CMD_EXIT:
  case CMD_OPEN_GUI:
  case CMD_HISTORY:
  case CMD_LOG:
  case CMD_PLAYBACK:
    cmd_successful = SS_no_value (cmd);
    break;
  case CMD_EXP_RESTORE:
  case CMD_SETBREAK:
    cmd_successful = SS_int_value (cmd);
    break;
  case CMD_LIST_SRC:
  case CMD_HELP:
    cmd_successful = SS_string_value (cmd);
    break;
  case CMD_LIST_EXP:
  case CMD_LIST_HOSTS:
  case CMD_LIST_OBJ:
  case CMD_LIST_PIDS:
  case CMD_LIST_METRICS:
  case CMD_LIST_PARAMS:
  case CMD_LIST_REPORTS:
  case CMD_LIST_BREAKS:
  case CMD_LIST_TYPES:
  default:
    fprintf(stderr,"Command %d has not been implimented yet\n",cmd->Type());
  }

 // For DEBUGING:
 // if (!cmd_successful) fprintf(stdout,"Command Error: ");
 // cmd->Print(stdout);
}
