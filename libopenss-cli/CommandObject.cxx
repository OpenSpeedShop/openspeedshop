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
      if (!clip->Complex_Exp()) {
       // GUI waits on level-1 commands.  Mark them complete.
        clip->SetStatus (ILO_COMPLETE);
      }
    } else if (S == CMD_ERROR) {
      clip->SetStatus (ILO_ERROR);
    } else if (S == CMD_ABORTED) {
    }
  }
}


// For tracing commands to the log file.
void CommandObject::Print (ostream &mystream) {
 // Header information
  InputLineObject *clip = Associated_Clip;
  CMDID when = clip->Seq ();
  mystream << "X " << when << "." << Seq_Num;

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
  mystream << " " << S << ": ";

 // result information
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult *>::iterator cri;
  int cnt = 0;
  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
    if (cnt++ > 0) mystream << ". ";
    (*cri)->Print (mystream);

   // Check for asnychonous abort command
    if (Cmd_Status == CMD_ABORTED) {
      break;
    }
  }
  mystream << std::endl;
}

// For printing the results to an Xterm Window.

bool CommandObject::Print_Results (ostream &to, std::string list_seperator, std::string termination_char) {

 // Pick up information lists from CommandObject.
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult_RawString *> cmd_annotation = Annotation_List ();

 // Print any Annotation information
  bool annotation_printed = false;
  std::list<CommandResult_RawString *>::iterator ari;
  for (ari = cmd_annotation.begin(); ari != cmd_annotation.end(); ari++) {
    (*ari)->Print (to, OPENSSS_VIEW_FIELD_SIZE, true);
    annotation_printed = true;
  }

  if (annotation_printed &&
      (cmd_result.begin() == cmd_result.end())) {
   // There is result, but we did print something and need a new prompt.
    return true;
  }

 // Print the result information
  std::list<CommandResult *>::iterator cri = cmd_result.begin();
  if  (cri != cmd_result.end()) {
    if (((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) ||
         (++cri != cmd_result.end())) {
      to << std::endl;
    }
    bool list_seperator_needed = false;
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
      if (list_seperator_needed) to << list_seperator;
      (*cri)->Print (to, OPENSSS_VIEW_FIELD_SIZE, true);

      list_seperator_needed = true;
      if ((*cri)->Type() == CMD_RESULT_STRING) {
        std::string S;
        ((CommandResult_String *)(*cri))->Value(S);
        if (S.length() > 0) {
          if (S.substr(S.length()-1,1) == list_seperator) {
            list_seperator_needed = false;
          }
        }
      }

     // Check for asnychonous abort command
      if (Cmd_Status == CMD_ABORTED) {
        break;
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
