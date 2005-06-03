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

void CommandObject::set_Status (Command_Status S) {
 // Once in the ERROR or ABORTED state, it must stay there.
 // Furthermore, propagate these states to the Clip only once.
  if ((Cmd_Status != CMD_ERROR) &&
      (Cmd_Status != CMD_ABORTED) &&
      (Cmd_Status != S)) {
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
}


// For tracing commands to the log file.
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

// For printing the results to an Xterm Window.
bool CommandObject::Print_Results (ostream &to, std::string list_seperator, std::string termination_char) {
 // Print only the result information
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult *>::iterator cri = cmd_result.begin();
  if  (cri != cmd_result.end()) {
    if (((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) ||
         (++cri != cmd_result.end())) {
      to << std::endl;
    }
    bool list_seperator_needed = false;
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
      if (list_seperator_needed) to << list_seperator;
      (*cri)->Print (to, 20, true);

      list_seperator_needed = true;
      if ((*cri)->Type() == CMD_RESULT_STRING) {
        std::string S;
        ((CommandResult_String *)(*cri))->Value(S);
        if (S.substr(S.length()-1,1) == list_seperator) {
          list_seperator_needed = false;
        }
      }
    }
    if (list_seperator_needed ||
        (list_seperator != termination_char)) {
      to << termination_char;
    }
    return true;
  } else {
    return false;
  }
}
bool CommandObject::Print_Results (FILE *TFile, std::string list_seperator, std::string termination_char) {
 // Print only the result information
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult *>::iterator cri = cmd_result.begin();
  if  (cri != cmd_result.end()) {
    if (((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) ||
         (++cri != cmd_result.end())) fprintf(TFile,"\n");
    int cnt = 0;
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
      if (cnt++ > 0) fprintf(TFile,"%s",list_seperator.c_str());
      // (*cri)->Print (TFile);
      (*cri)->Print (TFile, 20, true);
    }
    fprintf(TFile,"%s",termination_char.c_str());
    return true;
  } else {
    return false;
  }
}
