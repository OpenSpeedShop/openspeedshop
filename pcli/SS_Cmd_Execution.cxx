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

// Experiment Building BLock Commands
  case CMD_EXP_CLOSE:
    cmd_successful = SS_expClose(cmd);
    break;
  case CMD_EXP_ATTACH:
    cmd_successful = SS_expAttach (cmd);
    break;
  case CMD_EXP_CREATE:
    cmd_successful = SS_expCreate (cmd);
    break;
  case CMD_EXP_DETACH:
    cmd_successful = SS_expDetach(cmd);
    break;
  case CMD_EXP_DISABLE:
    cmd_successful = SS_expDisable(cmd);
    break;
  case CMD_EXP_ENABLE:
    cmd_successful = SS_expEnable(cmd);
    break;
  case CMD_EXP_FOCUS:
    cmd_successful = SS_expFocus (cmd);
    break;
  case CMD_EXP_PAUSE:
    cmd_successful = SS_expPause (cmd);
    break;
  case CMD_EXP_GO:
    cmd_successful = SS_expGo (cmd);
    break;
  case CMD_EXP_RESTORE:
    cmd_successful = SS_expRestore (cmd);
    break;
  case CMD_EXP_SAVE:
    cmd_successful = SS_expSave (cmd);
    break;
  case CMD_EXP_SETPARAM:
    cmd_successful = SS_expSetParam (cmd);
    break;
  case CMD_EXP_VIEW:
    cmd_successful = SS_expView (cmd);
    break;

// Information Commands
  case CMD_LIST_BREAKS:
    cmd_successful = SS_ListBreaks (cmd);
    break;
  case CMD_LIST_EXP:
    cmd_successful = SS_ListExp (cmd);
    break;
  case CMD_LIST_HOSTS:
    cmd_successful = SS_ListHosts (cmd);
    break;
  case CMD_LIST_OBJ:
    cmd_successful = SS_ListObj (cmd);
    break;
  case CMD_LIST_PIDS:
    cmd_successful = SS_ListPids (cmd);
    break;
  case CMD_LIST_METRICS:
    cmd_successful = SS_ListMetrics (cmd);
    break;
  case CMD_LIST_PARAMS:
    cmd_successful = SS_ListParams (cmd);
    break;
  case CMD_LIST_REPORTS:
    cmd_successful = SS_ListReports (cmd);
    break;
  case CMD_LIST_SRC:
    cmd_successful = SS_ListSrc (cmd);
    break;
  case CMD_LIST_TYPES:
    cmd_successful = SS_ListTypes (cmd);
    break;

// Session Commands
  case CMD_CLEAR_BREAK:
    cmd_successful = SS_ClearBreaks (cmd);
    break;
  case CMD_EXIT:
    cmd_successful = SS_Exit (cmd);
    break;
  case CMD_HELP:
    cmd_successful = SS_Help (cmd);
    break;
  case CMD_HISTORY:
    cmd_successful = SS_History (cmd);
    break;
  case CMD_LOG:
    cmd_successful = SS_Log (cmd);
    break;
  case CMD_OPEN_GUI:
    cmd_successful = SS_OpenGui (cmd);
    break;
  case CMD_PLAYBACK:
    cmd_successful = SS_Playback (cmd);
    break;
  case CMD_RECORD:
    cmd_successful = SS_Record (cmd);
    break;
  case CMD_SETBREAK:
    cmd_successful = SS_SetBreak (cmd);
    break;
  default:
    cmd->Result_String ("This command has not been implimented yet");
    cmd->set_Status(CMD_ERROR);
  }

}
