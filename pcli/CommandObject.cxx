#include "SS_Input_Manager.hxx"

void CommandObject::set_Status (Command_Status S) {
  Cmd_Status = S;
  InputLineObject *clip = Clip();
  if (S == CMD_COMPLETE) {
    clip->SetStatus (ILO_COMPLETE);
    Cmd_Obj_Complete (this);
  } else if (S == CMD_ERROR) {
    clip->SetStatus (ILO_ERROR);
    Cmd_Obj_Complete (this);
  } else if (S == CMD_ABORTED) {
    Cmd_Obj_Complete (this);
  }
}


void CommandObject::Print (FILE *TFile) {
 // Header information
  InputLineObject *clip = Associated_Clip;
  CMDID when = clip->Seq ();
  fprintf(TFile,"X %lld.%d ",when,Seq_Num);

 // Status information
  char *S;
  switch (Cmd_Status) {
  case CMD_UNKNOWN:   S = "UNKNOWN"; break;
  case CMD_PARSED:    S = "PARSED"; break;
  case CMD_EXECUTING: S = "EXECUTING"; break;
  case CMD_COMPLETE:  S = "COMPLETE"; break;
  case CMD_ERROR:     S = "ERROR"; break;
  case CMD_ABORTED:   S = "ABORTED"; break;
  default:            S = "ILLEGAL"; break;
  }
  fprintf(TFile,"%s: ",S);

 // result information
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult *>::iterator cri;
  int cnt = 0;
  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
    if (cnt++ > 0) fprintf(TFile,", ");
    (*cri)->Print (TFile);
  }
  fprintf(TFile,"\n");
}
