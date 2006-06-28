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

/**
 * Method: CommandObject::set_Status ()
 *
 * Attempt to set the field "Cmd_Status" with a particular value.
 * If the desired status is 'complete' or ' error', propagate
 * this status to the Clip that points to this CommandObject.
 *
 * @param  Command_Status, the value we want to set.
 *
 * @return  void
 *
 * @error Once set, and 'error' or ' aborted' status can not be changed.
 *
 */
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

/**
 * Method: CommandObject::Print ()
 *
 * Do a compressed dump of the CommandObject to the log file.
 * This is intended for debugging. Consequently, the result does
 * not need to be nice looking.
 *
 * @param ostream, the file to write to.
 *
 * @return  void
 *
 */
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

/**
 * Utility: Break_At ()
 *
 * Scan a character string and determine how best to break it into
 * segments that will fit in an output field of length OPENSS_VIEW_FIELD_SIZE.
 *
 * Breaks are prefered at spaces or commas and the resulting substring
 * will contain no leading blanks.
 *
 * @param  std::string, the one to scan.
 * @param  int64_t, where to start scanning from.
 *
 * @return int64_t, the selected end of the substring.
 *
 */
static inline int64_t Break_At (std::string S, int64_t start) {
  int64_t last_index = S.size() - 1;
  int64_t start_segment = start;

 // Skip over any leading blanks.
  while ((start_segment <= last_index) &&
         (S[start_segment] == *" ")) {
    start_segment++;
  }

  int64_t end_segment = start;
  while (end_segment <= last_index) {
    int64_t start_scan = end_segment + 1;
    int64_t nextBreak = start_scan;

   // Look for a reasonable place to break the string.
    for ( ; nextBreak <= last_index; nextBreak++) {
      if ((S[nextBreak] == *",") ||
          (S[nextBreak] == *" ")) {
        break;
      }
    }

    if ((nextBreak - start_segment) > OPENSS_VIEW_FIELD_SIZE) {
     // Including the new segment will cause S.substr(start_segment,nextBreak)
     // to overflow the field.  Return the last segment - without trailing blanks.
      while ((end_segment > start_segment) &&
             (S[end_segment] == *" ")) {
        end_segment--;
      }

      return ((start_segment == end_segment) ? (start + OPENSS_VIEW_FIELD_SIZE) : end_segment);
    }

    end_segment = nextBreak;
  }
  return last_index;
}

/**
 * Utility: Count_Partitions ()
 *
 * Scan a string and determine how many segments it should be
 * broken down into so that each one fits into a field of length
 * OPENSS_VIEW_FIELD_SIZE.
 *
 * @param  std::string, the original string.
 *
 * @return int64_t the number of segments.
 *
 */
static inline int64_t Count_Partitions(std::string S) {
  int64_t len = S.size();
  if (len <= OPENSS_VIEW_FIELD_SIZE) {
    return 1;
  } else {
    int64_t cnt = 0;
    int64_t start_scan = 0;
    while (start_scan < len) {
      start_scan = Break_At (S, start_scan) + 1;
      cnt++;
    }
    return cnt;
  }
}

/**
 * Utility: Print_Header ()
 *
 * Format a CommandResult_Headers entry so that each column value fits within
 * a field of length OPENSS_VIEW_FIELD_SIZE.
 *
 * Each header is scanned to determine how many segments are required.
 * New CommandResult_Headers are created for the maximum number of segments.
 * The original header is re-scanned and each segment is placed into one of
 * the new CommandResult_Headers.  Finally, the new headers are printed, one
 * row at a time.
 *
 * @param  ostream, the file to send the output to.
 * @param  std::string, a seperator that is used at the end of each row of output.
 * @param  CommandResult *, the CommandResult_Headers object that describes the
 *         strings that need to fit into the field.
 *
 * @return void.
 *
 */
static void Print_Header (ostream &to, std::string list_seperator, CommandResult *H) {
  std::list<CommandResult *> Headers;
  ((CommandResult_Headers *)H)->Value(Headers);
  int64_t len = Headers.size() - 1; // ignore right most header - it can be as long as it wants.
  int64_t max_Breaks = 0;
  std::list<CommandResult *>::iterator hi = Headers.begin();
  for (int64_t i = 0; i < len; i++) {
    CommandResult *next = *hi++;
    std::string next_str;
    if (next->Type() == CMD_RESULT_STRING) {
      ((CommandResult_String *)next)->Value(next_str);
    } else if (next->Type() == CMD_RESULT_RAWSTRING) {
      ((CommandResult_RawString *)next)->Value(next_str);
    }
    if (!next_str.empty()) {
      max_Breaks = max(max_Breaks,Count_Partitions(next_str));
    }
  }
  if (max_Breaks == 1) {
    H->Print(to, OPENSS_VIEW_FIELD_SIZE, true);
  } else {
    std::vector<CommandResult_Headers> HL(max_Breaks);
    std::list<CommandResult *>::iterator hi = Headers.begin();
    for (int64_t i = 0; i < len; i++) {
      CommandResult *next = *hi++;
      std::string next_str;
      if (next->Type() == CMD_RESULT_STRING) {
        ((CommandResult_String *)next)->Value(next_str);
      } else if (next->Type() == CMD_RESULT_RAWSTRING) {
        ((CommandResult_RawString *)next)->Value(next_str);
      }
      int64_t next_len = next_str.size();
      int64_t nextH = 0;
      int64_t start_scan = 0;
      while (start_scan < next_len) {
        int64_t next_scan = Break_At (next_str, start_scan) + 1;
        HL[nextH++].Add_Header(new CommandResult_String(next_str.substr(start_scan,(next_scan - start_scan))));
        start_scan = next_scan;
      }
      while (nextH < max_Breaks) {
        HL[nextH++].Add_Header(new CommandResult_String (""));
      }
    }

   // Attach right most header to end of first re-write
   // and put dummy strings at the end of all the other re-writes.
    HL[0].Add_Header (Headers.back()->Copy());
    for (int64_t BreakNum = 1; BreakNum < max_Breaks; BreakNum++) {
      HL[BreakNum].Add_Header(new CommandResult_String (""));
    }

   // Now we can print the re-formatted headers.
    for (int64_t BreakNum = 0; BreakNum < max_Breaks; BreakNum++) {
      HL[BreakNum].Print(to, OPENSS_VIEW_FIELD_SIZE, true);
      if ((BreakNum + 1) < max_Breaks) {
       // Start next header line at left margin.
        to << list_seperator;
      }
    }
  }
}

/**
 * Method: CommandObject::Print_Results ()
 *
 * Format and print the data associated with a CommandObject.
 *
 * @param  ostream, the file to send the output to.
 * @param  std::string, a seperator that is used at the end of each row of output.
 * @param  std::string, a seperator that is used at the completion of output.
 *
 * @return bool, set to true if we sent data to the output file. False if we did not.
 *         This tells the calling routine if a new prompt is needed on the output file,
 *         if it can also be used as an input file.
 *
 */
bool CommandObject::Print_Results (ostream &to, std::string list_seperator, std::string termination_char) {

 // Pick up information lists from CommandObject.
  std::list<CommandResult *> cmd_result = Result_List();
  std::list<CommandResult_RawString *> cmd_annotation = Annotation_List ();

 // Print any Annotation information
  bool annotation_printed = false;
  std::list<CommandResult_RawString *>::iterator ari;
  for (ari = cmd_annotation.begin(); ari != cmd_annotation.end(); ari++) {
    (*ari)->Print (to, OPENSS_VIEW_FIELD_SIZE, true);
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

      if ((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) {
       // Special processing required, because the headers
       // may need multiple lines.
        Print_Header (to, list_seperator, *cri);
      } else {
        (*cri)->Print (to, OPENSS_VIEW_FIELD_SIZE, true);
      }

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
