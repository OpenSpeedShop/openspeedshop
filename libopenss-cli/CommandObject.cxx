/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
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

// This define (DATABASE_SAVEDVIEWS) switches the code in this file making the saved view 
// storage location to the database instead of writing disk files
#define DATABASE_SAVEDVIEWS 1

// This define (DEBUG_REUSEVIEWS) turns on save/reuse view debug code (need to uncomment)
/*
#define DEBUG_REUSEVIEWS 1
#define DEBUG_REUSEVIEWS_EXTENDED 1
*/

/** * Copy a file.
 *
 * Copies the contents of one file over the contents of another. The source and
 * destination file must both already exist and the original contents of the
 * destination file are completely destroyed.
 *
 * @param source         File to be copied.
 * @param destination    File to be overwritten.
 */
// void copyFile(std::string source, std::string destination)
void CommandObject::copyFile(std::string orig_cmd, std::string source, std::ostream &destination , int starting_offset )
{
  const int copyBufferSize = 65536;
  int size_of_view_data = 0;

#if DATABASE_SAVEDVIEWS

#if DEBUG_REUSEVIEWS
  std::cerr << "ENTER copyFile, note: skipping open of view file"  << std::endl;
#endif

#else
  // Open the source file for read-only access
  int source_fd = open(source.c_str(), O_RDONLY);
  Assert(source_fd != -1);

  // Perform the copy
  int header_length = starting_offset;
#endif

  // Allocate a buffer for performing the copy
  char* buffer = new char[copyBufferSize];

#if DEBUG_REUSEVIEWS
  std::cerr << "ENTER copyFile, THIS IS WHERE WE READ FROM THE DATABASE view_data and write to the output view" 
            << " orig_cmd=" << orig_cmd << " file to read from is=" << source.c_str() << std::endl;
#endif

  std::string buffer_string;
  ExperimentObject *exp = Find_Experiment_Object (SaveExpId());

  if (exp && exp->FW()) {
    std::string db_name = exp->FW()->getName();
    bool get_view_success = exp->FW()->getViewFromExistingCommandEntry(db_name,  orig_cmd, buffer_string, size_of_view_data);

    if (get_view_success) {
     
#if DEBUG_REUSEVIEWS_EXTENDED
      std::cerr << "size_of_view_data=" << size_of_view_data << std::endl;
#endif

    } // end if exp
  } // end get_view_success


#if DATABASE_SAVEDVIEWS
   // New database method of retrieving data follows

#if DEBUG_REUSEVIEWS
  std::cerr << "IN copyFile, buffer_string.size()=" << buffer_string.size() << " size_of_view_data=" << size_of_view_data << std::endl;
#endif

  // copy the string obtained from the database to the destination stream for output
  destination << buffer_string;

#else
  //
  // Original method of reading from the view files follows
  //
  for(int num = 1; num > 0;) {

    // Read bytes from the source file
    num = read(source_fd, buffer, copyBufferSize);
    Assert((num >= 0) || ((num == -1) && (errno == EINTR)));

#if DEBUG_REUSEVIEWS
    std::cerr << "IN copyFile, COPY TO OUTPUT VIEW, copy num=" << num << " bytes to the OUTPUT VIEW" << std::endl;
#endif

    // Perform the copy

    // Write bytes until none remain
    if(num > 0) {

      for(int i = header_length; i < num; i++) {
        // Write bytes to the destination file
        destination << buffer[i];
      }

      header_length = 0;
      if (num != copyBufferSize) break;
    }
  }
#endif

  // Destroy the copy buffer
  delete [] buffer;

#ifndef DATABASE_SAVEDVIEWS
  // Close the input file.
  Assert(close(source_fd) == 0);

#endif

#if DEBUG_REUSEVIEWS
  std::cerr << "EXIT copyFile, COPY TO OUTPUT VIEW" << std::endl;
#endif

}



/**
 * utility: Look_For_Format_Specification
 *       Search for specific options the the list.
 *     
 * @param   std::vector<ParseRange> that lists declared format specifiers.
 * @param   std::string the option being searched for.
 *
 * @return  ParseRange * of the matching entry, or NULL if not found.
 *
 */
ParseRange *OpenSpeedShop::cli::Look_For_Format_Specification (std::vector<ParseRange> *f_list,
                                                               std::string Key) {

 //  Look at general modifier types for a specific KeyWord option.
  if (f_list == NULL) return NULL;
  std::vector<ParseRange>::iterator j;

#if DEBUG_REUSEVIEWS
  std::cerr << "ENTER OpenSpeedShop::cli::Look_For_Format_Specification, Key=" << Key << std::endl;
#endif
 
  for (j=f_list->begin();j != f_list->end(); j++) {
    if ( ((*j).getParseType() == PARSE_RANGE_VALUE) &&
         ((*j).getRange()->start_range.tag == VAL_STRING ) &&
         (!strcasecmp(((*j).getRange()->start_range.name).c_str(), Key.c_str())) ) {
      return &(*j);
    }
  }

  return NULL;
}

/**
 *  Utility: Capture_User_Format_Information(PrintControl &pc)
 *
 *  Scan the format list specifier, look for valid keywords and transfer
 *  information into the PrintControl entry.
 * 
 **/
static void Capture_User_Format_Information( std::vector<ParseRange> *f_list, PrintControl &format_spec ) {
  if (f_list == NULL) return;

 // Look for End_Of_Column specifier.
  ParseRange *key_range = Look_For_Format_Specification ( f_list, "viewEOC");
  if ( (key_range  != NULL) &&
       (key_range->getRange()->is_range) &&
       (key_range->getRange()->end_range.tag == VAL_STRING) ) {
    format_spec.Set_PrintControl_eoc(key_range->getRange()->end_range.name);
  }
 // Look for End_Of_Line specifier.
  key_range = Look_For_Format_Specification ( f_list, "viewEOL");
  if ( (key_range  != NULL) &&
       (key_range->getRange()->is_range) &&
       (key_range->getRange()->end_range.tag == VAL_STRING) ) {
    format_spec.Set_PrintControl_eol(key_range->getRange()->end_range.name);
  }
 // Look for End_Of_View specifier.
  key_range = Look_For_Format_Specification ( f_list, "viewEOV");
  if ( (key_range  != NULL) &&
       (key_range->getRange()->is_range) &&
       (key_range->getRange()->end_range.tag == VAL_STRING) ) {
    format_spec.Set_PrintControl_eov(key_range->getRange()->end_range.name);
  }
 // Look for dynamic fieldsize specifier.
  key_range = Look_For_Format_Specification ( f_list, "viewFieldSizeIsDynamic");
  if (key_range != NULL) {
    if ( key_range->getRange()->is_range &&
         ( ( (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
             (key_range->getRange()->end_range.num == 0) ) ||
           ( (key_range->getRange()->end_range.tag == VAL_STRING) &&
             (strcasecmp( key_range->getRange()->end_range.name.c_str(), "false") == 0) ) ) ) {
      format_spec.Set_PrintControl_dynamic_size( false );
    } else {
      format_spec.Set_PrintControl_dynamic_size( true );
    }
  }
 // Look for fieldsize specifier.
  key_range = Look_For_Format_Specification ( f_list, "viewFieldSize" );
  if ( (key_range != NULL) &&
       (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
       (key_range->getRange()->end_range.num != 0) ) {
    format_spec.Set_PrintControl_field_size( key_range->getRange()->end_range.num);
  }
 // Look for left blank-in-place-of-zero formatting.
  key_range = Look_For_Format_Specification ( f_list, "viewBlankInPlaceOfZero");
  if (key_range != NULL) {
    if ( key_range->getRange()->is_range &&
         ( ( (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
             (key_range->getRange()->end_range.num == 0) ) ||
           ( (key_range->getRange()->end_range.tag == VAL_STRING) &&
             (strcasecmp( key_range->getRange()->end_range.name.c_str(), "false") == 0) ) ) ) {
      format_spec.Set_PrintControl_blank_in_place_of_zero( false );
    } else {
      format_spec.Set_PrintControl_blank_in_place_of_zero( true );
    }
  }
 // Look for left justified formatting.
  key_range = Look_For_Format_Specification ( f_list, "viewLeftJustify" );
  if (key_range != NULL) {
    if ( key_range->getRange()->is_range &&
         ( ( (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
             (key_range->getRange()->end_range.num == 0) ) ||
           ( (key_range->getRange()->end_range.tag == VAL_STRING) &&
             (strcasecmp( key_range->getRange()->end_range.name.c_str(), "false") == 0) ) ) ) {
      format_spec.Set_PrintControl_left_justify_all( false );
    } else {
      format_spec.Set_PrintControl_left_justify_all( true );
    }
  }
 // Look for Comma Separated Values specifier.
  key_range = Look_For_Format_Specification ( f_list, "CSV" );
  if ( (key_range != NULL) &&
       (Look_For_Format_Specification ( f_list, "CSV") != NULL) ) {
    format_spec.Set_PrintControl_eoc(",");
  }
}

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
void CommandObject::Print (std::ostream &mystream) {
 // Header information

  InputLineObject *clip = Associated_Clip;
  CMDID when = clip->Seq ();
  mystream << "X " << when << "." << Seq_Num;

 // Status information
  const char *S;
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
 * Utility: Min_Header_Column_width ()
 *
 * Scan a character string and determine the length of each
 * word in an attempt to avoid splitting words when they
 * are placed in a header field.
 *
 * @param  std::string, the one to scan.
 *
 * @return int64_t, the size of the largest blank seperated word.
 *
 */
static inline int64_t Min_Header_Column_Width (std::string S) {
  int64_t last_index = S.size() - 1;
  int64_t max_word_size = 0;
  int64_t current_word_size = 0;

  for (int64_t i=0; i<last_index; i++) {
    if (S[i] == *" ") {
      if (current_word_size > max_word_size) {
        max_word_size = current_word_size;
      }
      current_word_size = 0;
      continue;
    }
    current_word_size++;
  }

  return (current_word_size > max_word_size) ? current_word_size : max_word_size;
}

/**
 * Utility: Break_At ()
 *
 * Scan a character string and determine how best to break it into
 * segments that will fit in an output field of length use_columnsize.
 *
 * Breaks are prefered at spaces and the resulting substring
 * will ignore leading blanks when calculating the length.
 *
 * @param  std::string, the one to scan.
 * @param  int64_t, where to start scanning from.
 *
 * @return int64_t, the selected end of the substring.
 *
 */
static inline int64_t Break_At (std::string S, int64_t start, int64_t use_columnsize) {
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
      if (S[nextBreak] == *" ") {
        break;
      }
    }

    if ((nextBreak - start_segment) > use_columnsize) {
     // Including the new segment will cause S.substr(start_segment,nextBreak)
     // to overflow the field.  Return the last segment - without trailing blanks.
      while ((end_segment > start_segment) &&
             (S[end_segment] == *" ")) {
        end_segment--;
      }

      return ((start_segment == end_segment) ? (start + use_columnsize) : end_segment);
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
 * use_columnsize.
 *
 * @param  std::string, the original string.
 *
 * @return int64_t the number of segments.
 *
 */
static inline int64_t Count_Partitions(std::string S, int64_t use_columnsize) {
  int64_t len = S.size();
  if (len <= use_columnsize) {
    return 1;
  } else {
    int64_t cnt = 0;
    int64_t start_scan = 0;
    while (start_scan < len) {
      start_scan = Break_At (S, start_scan, use_columnsize) + 1;
      cnt++;
    }
    return cnt;
  }
}

/**
 * Utility: Print_Header ()
 *
 * Format a CommandResult_Headers entry so that each column value fits within
 * a field of length column_widths[] or OPENSS_VIEW_FIELD_SIZE.
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
static void Print_Header (std::ostream &to, PrintControl &pc, CommandResult *H) {
  std::list<CommandResult *> Headers;
  ((CommandResult_Headers *)H)->Value(Headers);
  int64_t len = Headers.size() - 1; // ignore right most header - it can be as long as it wants.

 // Determine the maximum number of lines required for the headers.
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
      int64_t use_columnsize = (pc.column_widths != NULL) ? pc.column_widths[i] : pc.field_size;
      max_Breaks = std::max(max_Breaks,Count_Partitions(next_str, use_columnsize));
    }
  }

  if (max_Breaks == 1) {
    H->Print(to, pc, 0);
  } else {
   // Reformat each header, except the last, into the desired number of lines.
    std::vector<CommandResult_Headers> HL(max_Breaks);
    hi = Headers.begin();
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
      int64_t use_columnsize = (pc.column_widths != NULL) ? pc.column_widths[i] : pc.field_size;
  
      while (start_scan < next_len) {

       // Skip over any leading blanks, since Break_At does the same.
        while ((start_scan < next_len) &&
               (next_str[start_scan] == *" ")) {
          start_scan++;
        }

        int64_t next_scan = Break_At (next_str, start_scan, use_columnsize) + 1;
        //Assert ((next_scan - start_scan) <= use_columnsize);
        HL[nextH++].Add_Header(new CommandResult_String(next_str.substr(start_scan,(next_scan - start_scan))));
        start_scan = next_scan;
      }
      while (nextH < max_Breaks) {
        HL[nextH++].Add_Header(new CommandResult_String (""));
      }
    }

   // Attach right most header to end of first line of the re-write list
   // and put dummy strings at the end of all the other re-writes.
    HL[0].Add_Header (Headers.back()->Copy());
    for (int64_t BreakNum = 1; BreakNum < max_Breaks; BreakNum++) {
      HL[BreakNum].Add_Header(new CommandResult_String (""));
    }

   // Now we can print the re-formatted headers.
    for (int64_t BreakNum = 0; BreakNum < max_Breaks; BreakNum++) {
      HL[BreakNum].Print(to, pc, 0);
      if ((BreakNum + 1) < max_Breaks) {
       // Start next header line at left margin.
        to << pc.eol;
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
bool CommandObject::Print_Results (std::ostream &to, std::string list_seperator, std::string termination_char) {

  bool save_for_reuse = false;
  std::ostream *tof = &to;
  savedViewInfo *svi = SaveResultViewInfo();


#if DEBUG_REUSEVIEWS
  std::cerr << "ENTER Print_Results, svi=" << svi << " tof=" << tof << " to=" << to 
            << " Clip()->Who()=" << Clip()->Who() << " gui_window=" << gui_window 
            << " command_line_window=" << command_line_window <<  std::endl;

  if ( OPENSS_SAVE_EXPERIMENT_DATABASE && (svi != NULL) ) {
     std::cerr << "ENTER Print_Results, svi->FileName().length()=" << svi->FileName().length() 
               << " svi->DoNotSave()=" << svi->DoNotSave()
               << " SaveResult()=" << SaveResult()
               << " svi->FileName()=" << svi->FileName() << std::endl;
  }
#endif

  std::string SavedResultFile;
  std::string svi_orignal_cmd;
  if ( OPENSS_SAVE_EXPERIMENT_DATABASE &&
//       Clip()->Who() != gui_window &&
       (svi != NULL) &&
       (svi->FileName().length() > 0) ) {
    SavedResultFile = svi->FileName();
    svi_orignal_cmd = svi->GenCmd();

    if ( !SaveResult()) {

     // Required output has already been generated.

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, output has already been generated, READ FROM DATABASE AND WRITE TO OUTPUT STREAM (tof)," 
                << " COPYING SavedResultFile=" << SavedResultFile 
                << " to: tof ostream at SaveResultDataOffset()=" << SaveResultDataOffset() 
                << " tof=" << tof << " to=" << to << std::endl;
#endif

      copyFile( svi_orignal_cmd, SavedResultFile, *tof, SaveResultDataOffset() );
      return true;

    } else {

     // The output generated here will be saved for future use.

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, Follow this path to STORE VIEW TO DATABASE FOR FUTURE RETREIVAL, if view NOT already saved" 
                << " and save the SaveResultOstream to tof" << std::endl;
#endif

      if ( (!svi->DoNotSave()) ) {
        save_for_reuse = TRUE;
        // Retrieve the save ostream from the saved view info structure
        tof = SaveResultOstream();
      }
    }
  }

 // Pick up information lists from CommandObject.
  std::list<CommandResult *> cmd_result = Result_List();

  std::list<CommandResult_RawString *> cmd_annotation = Annotation_List ();
  std::vector<ParseRange> *f_list = (P_Result() != NULL) ? P_Result()->getexpFormatList() : NULL;

 // Look for user format control.
  ParseRange *key_range;
  PrintControl format_spec;
  Capture_User_Format_Information( f_list, format_spec );
  if ( (format_spec.eoc == format_spec.eol) &&
       (OPENSS_VIEW_EOC != OPENSS_VIEW_EOL) ) {
    Result_Annotation ("viewEOC and viewEol may not be set to the same value.  Defaults used.\n");
    format_spec.Set_PrintControl_eoc(OPENSS_VIEW_EOC);
    format_spec.Set_PrintControl_eol(OPENSS_VIEW_EOL);
    cmd_annotation = Annotation_List ();
  }

 // Did the user want control over formatting?
  bool EOC_specified = strcasecmp(format_spec.eoc.c_str(), OPENSS_VIEW_EOC.c_str()) ? true : false;

 // Print any Annotation information
  bool annotation_printed = false;
  std::list<CommandResult_RawString *>::iterator ari;
  if (cmd_annotation.begin() != cmd_annotation.end()) {
    int64_t annotation_column = 0; // Just 1 column.
    format_spec.Set_PrintControl_column_widths( 1, &annotation_column );
    for (ari = cmd_annotation.begin(); ari != cmd_annotation.end(); ari++) {
      (*ari)->Print (*tof, format_spec, 0);
      annotation_printed = true;
    }
  }

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, after annotation_printed checks annotation_printed=" << annotation_printed << std::endl;
#endif

  if (annotation_printed &&
      (cmd_result.begin() == cmd_result.end())) {
   // There is result, but we did print something and need a new prompt.
    return true;
  }

 // Determine the required width of each column.
  int64_t num_columns = 1;
  std::list<CommandResult *>::iterator cri = cmd_result.begin();
  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
    if ((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) {
      int64_t cnt = 0;
      ((CommandResult_Headers *)(*cri))->Value(cnt);
      num_columns = std::max (num_columns, cnt);
      break; // Yes, we are assuming there is only 1 header associated with each view.
    } else if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES) {
      int64_t cnt = 0;
      ((CommandResult_Columns *)(*cri))->Value(cnt);
      num_columns = std::max (num_columns, cnt);
    } else if ((*cri)->Type() == CMD_RESULT_COLUMN_ENDER) {
      int64_t cnt = 0;
      ((CommandResult_Enders *)(*cri))->Value(cnt);
      num_columns = std::max (num_columns, cnt);
    }
  }

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, after num_columns checks num_columns=" << num_columns << std::endl;
#endif

  int64_t Column_Width[num_columns];
  if (num_columns > 0) {
   // Initialize column width to defaults.
    ParseRange *lookup = NULL;
    int64_t default_value = 0;
    key_range = Look_For_Format_Specification ( f_list, "viewFieldSizeIsDynamic" );
    if ( (key_range  != NULL) &&
         ( (!key_range->getRange()->is_range) ||
           ( (key_range->getRange()->end_range.tag == VAL_STRING) &&
             (!strcasecmp((key_range->getRange()->end_range.name).c_str(), "true")) ) ||
           ( (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
             (key_range->getRange()->end_range.num != 0)  ) ) ) {
      default_value = 0;
    } else {
      key_range = Look_For_Format_Specification ( f_list, "viewFieldSize" );
      if ( (key_range != NULL) &&
           (key_range->getRange()->end_range.tag == VAL_NUMBER) &&
           (key_range->getRange()->end_range.num != 0) ) {
        default_value = key_range->getRange()->end_range.num;
      } else {
        default_value = (format_spec.field_size_dynamic) ? 0 : format_spec.field_size;
      }
    }
    for (int64_t i = 0; i < num_columns; i++) {
      Column_Width[i] = default_value;
    }

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, after default_value checks default_value=" << default_value << std::endl;
#endif

    if ( format_spec.field_size_dynamic &&
         !EOC_specified &&
         ( (Clip() == NULL) ||
           (Clip()->Who() == command_line_window) ||
           (Clip()->Who() == tli_window) ) ) {
     // Need to look at every value in each column.
      std::list<CommandResult *>::iterator cri = cmd_result.begin();
      for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
        if ( ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES) ||
             ((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) ||
             ((*cri)->Type() == CMD_RESULT_COLUMN_ENDER) ) {
          CommandResult *V = (*cri);
          std::list<CommandResult *> cr;
          ((CommandResult_Columns *)(*cri))->Value(cr);
          std::list<CommandResult *>::iterator coi = cr.begin();
          int64_t num_results = 0;
          int64_t num_columns_to_check = cr.size() - 1;
          for (num_results = 0, coi = cr.begin();
               ((num_results < num_columns_to_check) && (coi != cr.end()));
               num_results++,  coi++) {
            std::string F = (*coi)->Form();
            int64_t min_size = 0;
            if ((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) {
              min_size = Min_Header_Column_Width (F);
            } else {
              min_size = F.length();
            }
            int64_t leading_blank_cnt = 0;
            for (int64_t j=0; j < min_size; j++) {
              if (F.substr(j,1) == " ") {
                leading_blank_cnt++;
              } else break;
            }
            min_size -= leading_blank_cnt;
            if (min_size > Column_Width[num_results]) {
              Column_Width[num_results] = min_size;
#if DEBUG_REUSEVIEWS
              std::cerr << "In Print_Results, during min_size checks min_size=" << min_size << " num_results=" << num_results << std::endl;
#endif
            }
          }
        }
      }
    }
  }

#if DEBUG_REUSEVIEWS
   std::cerr << "In Print_Results, AFTER min_size checks " << std::endl;
#endif

 // Print the result information
  bool data_written = false;
  format_spec.Set_PrintControl_column_widths( num_columns, Column_Width );
  cri = cmd_result.begin();
  if  (cri != cmd_result.end()) {
    data_written = true;
    if (((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) ||
         (++cri != cmd_result.end())) {
      to << std::endl;
    }
    bool list_seperator_needed = false;
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
      if (list_seperator_needed) *tof << list_seperator;

      if ((*cri)->Type() == CMD_RESULT_COLUMN_HEADER) {
        if ( EOC_specified ||
            ( (Clip() != NULL) &&
              (Clip()->Who() != command_line_window) &&
              (Clip()->Who() != tli_window) ) ) {
         // Print each column header without regard to size.
          (*cri)->Print (*tof, format_spec, 0);
        } else {
         // Special processing required, because the headers
         // may need multiple lines.
          Print_Header (*tof, format_spec, *cri);
        }
      } else if ((*cri)->Type() == CMD_RESULT_COLUMN_VALUES) {
        (*cri)->Print (*tof, format_spec, 0);
      } else if ((*cri)->Type() == CMD_RESULT_COLUMN_ENDER) {
        (*cri)->Print (*tof, format_spec, 0);
      } else {
        (*cri)->Print (*tof, format_spec, 0);
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

     // Check for asynchonous abort command
      if (Cmd_Status == CMD_ABORTED) {
        break;
      }
    }
    if (list_seperator_needed ||
        (list_seperator != termination_char)) {
      *tof << termination_char;
    }

    *tof << std::flush;
    
#if DEBUG_REUSEVIEWS
    if (save_for_reuse) { std::cerr << "In Print_Results, tof is flushed-1, save_for_use is true" << std::endl;
    } else { std::cerr << "In Print_Results, tof is flushed-1, save_for_use is false" << std::endl; } 
#endif

  }

  //jeg  *tof << std::endl << std::flush;

  if (save_for_reuse ) {


#if DEBUG_REUSEVIEWS
    std::cerr << "In Print_Results, -path 2- CALL TO ADD DATA TO THE DATABASE FOR REUSE, COPYING SavedResultFile=" 
              << SavedResultFile << " to: to ostream to=" << to << " tof=" << tof << std::endl; 
#endif


    *tof << std::endl << std::flush;

    ExperimentObject *exp = Find_Experiment_Object (SaveExpId());
    std::string Data_File_Name;
    Data_File_Name = exp->FW()->getName();
    exp->FW()->addViewCommandAndDataEntries( Data_File_Name, svi->GenCmd(), SavedResultFile, tof, SaveResultDataOffset()  );
    // This will force the removal of the temporary file at the end of this CLI session
    svi->setDoNotSave();

     // was here but no data in *.view file  *tof << std::endl << std::flush;
#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, tof is flushed-2" << std::endl;
#endif

    delete tof;
    setSaveResult( false );
    setSaveResultOstream( NULL );
    if ( (Clip() == NULL) ||
         (Clip()->Who() == command_line_window) ||
         (Clip()->Who() == tli_window) ) {
     // Copy data from save file to desired output stream.

#if DEBUG_REUSEVIEWS
      std::cerr << "In Print_Results, calling copyFile to READ FROM DATABASE AND WRITE TO OUTPUT STREAM (to), to=" << to 
                << " (if not database enabled, then use file) COPYING SavedResultFile=" << SavedResultFile 
                << " to: to ostream at SaveResultDataOffset()=" << SaveResultDataOffset() << std::endl;
#endif

      copyFile( svi_orignal_cmd, SavedResultFile, to, SaveResultDataOffset() );
    }
  }

#if DEBUG_REUSEVIEWS
  std::cerr << "EXIT Print_Results, data_written=" << data_written << std::endl;
#endif

  return data_written;
}
