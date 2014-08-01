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

/** @file
 *
 * Definition of the CommandObject class.
 *
 */

//#define DEBUG_SYNC_SIGNAL 1

enum Command_Status
{
  CMD_UNKNOWN,
  CMD_PARSED,
  CMD_EXECUTING,
  CMD_COMPLETE,
  CMD_ERROR,
  CMD_ABORTED 
};

class CommandObject
{
  InputLineObject *Associated_Clip; // The input line that caused generation of this object.
  int64_t Seq_Num; // The order this object was generated in from the input line.
  Command_Status Cmd_Status;
  oss_cmd_enum Cmd_Type; // A copy of information in the Parse_Result.
  OpenSpeedShop::cli::ParseResult *PR;
  bool result_needed_in_python;  // Don't Print to ostream if this is set!
  bool results_used; // Once used, this object can be deleted!
  std::list<CommandResult *> CMD_Result;
  std::list<CommandResult_RawString *> CMD_Annotation;
  pthread_cond_t wait_on_dependency;
  bool save_result;
  savedViewInfo *save_result_ViewInfo;
  std::string save_result_filename;
  int save_result_file_header_offset;
  std::ostream *save_result_file_stream;
  std::string save_eoc;
  std::string save_eol;
  int save_exp_id;

  void Associate_Input ()
  {
    Associated_Clip = Current_ILO;
    Link_Cmd_Obj_to_Input (Associated_Clip, this);
  }

  void Attach_Result (CommandResult *R) {
    CMD_Result.push_back(R);
  }

public:
  CommandObject()
  {
    Associated_Clip = NULL;
    Seq_Num = 0;
    Cmd_Status = CMD_ERROR;
    Cmd_Type =  CMD_HEAD_ERROR;
    PR = NULL;
    result_needed_in_python = false;
    results_used = false;
    pthread_cond_init(&wait_on_dependency, (pthread_condattr_t *)NULL);
    save_result = false;
    save_result_ViewInfo = NULL;
    save_result_filename = "";
    save_result_file_header_offset = 0;
    save_result_file_stream = NULL;
    save_eoc = "";
    save_eol = "";
    save_exp_id = -1;
  }
  CommandObject(OpenSpeedShop::cli::ParseResult *pr, bool use_by_python)
  {
    this->Associate_Input ();
    Cmd_Status = CMD_PARSED;
    Cmd_Type =  pr->getCommandType();
    PR = pr;
    result_needed_in_python = use_by_python;
    results_used = false;
    pthread_cond_init(&wait_on_dependency, (pthread_condattr_t *)NULL);
    save_result = false;
    save_result_ViewInfo = NULL;
    save_result_filename = "";
    save_result_file_header_offset = 0;
    save_result_file_stream = NULL;
    save_eoc = "";
    save_eol = "";
    save_exp_id = -1;
  }
  ~CommandObject() {
   // Destroy ParseResult object
    if (PR != NULL) delete PR;

   // Reclaim results
    Reclaim_CR_Space (CMD_Result);

   // Reclaim annotations.
    Reclaim_CR_Space (CMD_Annotation);

   // Safety check.
    pthread_cond_destroy (&wait_on_dependency);

   // Free file descriptor.
    if (save_result_file_stream != NULL) {
      delete save_result_file_stream;
    }
  }

  InputLineObject *Clip () { return Associated_Clip; }
  Command_Status Status () { return Cmd_Status; }
  oss_cmd_enum Type () { return Cmd_Type; }
  bool Needed_By_Python () { return result_needed_in_python; }
  void SetNeeded_By_Python (bool needed) { result_needed_in_python = needed; }
  bool Results_Used () { return results_used; }
  OpenSpeedShop::cli::ParseResult *P_Result () { return PR; }
  void clearParseResult () { PR = NULL; }
  OpenSpeedShop::cli::ParseResult *swapParseResult (OpenSpeedShop::cli::ParseResult *P) {
    OpenSpeedShop::cli::ParseResult *pr = PR;
    PR = P;
    return pr;
  }
  // command_t *P_Result () { return Parse_Result; }
  //command_type_t *P_Result () { return Parse_Result; }
  bool SaveResult() { return save_result; }
  void setSaveResult( bool b ) { save_result = b; }
  savedViewInfo *SaveResultViewInfo() { return save_result_ViewInfo; }
  void setSaveResultViewInfo( savedViewInfo *svi) { save_result_ViewInfo = svi; }
  std::string SaveResultFile() { return save_result_filename; }
  void setSaveResultFile( std::string s ) { save_result_filename = s; }
  void setSaveResultOstream( std::ostream *sp ) { save_result_file_stream = sp; }
  std::ostream *SaveResultOstream() { return save_result_file_stream; }
  int  SaveResultDataOffset() { return save_result_file_header_offset; }
  void setSaveResultDataOffset( int l ) { save_result_file_header_offset = l; }
  void setSaveEoc( std::string s ) { save_eoc = s; }
  void setSaveExpId( int exp_id ) { save_exp_id = exp_id; }
  std::string SaveEoc() { return save_eoc; }
  int SaveExpId() { return save_exp_id; }
  void setSaveEol( std::string s ) { save_eol = s; }
  std::string SaveEol() { return save_eol; }

  void Wait_On_Dependency (pthread_mutex_t& exp_lock) {
   // Suspend processing of the command.

   // Release the lock and wait for the all-clear signal.
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], PTHREAD_COND_WAIT, Wait_On_Dependency before calling pthread_cond_wait(&wait_on_dependency=%ld,&exp_lock=%ld)\n", pthread_self(), wait_on_dependency, exp_lock);
#endif
    Assert(pthread_cond_wait(&wait_on_dependency,&exp_lock) == 0);
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], PTHREAD_COND_WAIT, Wait_On_Dependency after calling pthread_cond_wait(&wait_on_dependency=%ld,&exp_lock=%ld)\n", pthread_self(), wait_on_dependency, exp_lock);
#endif

   // Release the recently acquired lock and continue processing the command.
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], UNLOCK, Wait_On_Dependency before calling pthread_mutex_unlock(&exp_lock=%ld)\n", pthread_self(), exp_lock);
#endif
    Assert(pthread_mutex_unlock(&exp_lock) == 0);
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], UNLOCK, Wait_On_Dependency after calling pthread_mutex_unlock(&exp_lock=%ld)\n", pthread_self(), exp_lock);
#endif
  }
  void All_Clear () {
   // Release the suspended command.
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], PTHREAD_COND_SIGNAL, All_Clear before calling pthread_cond_signal(&wait_on_dependency=%ld)\n", pthread_self(), wait_on_dependency);
#endif
    Assert(pthread_cond_signal(&wait_on_dependency) == 0);
#ifdef DEBUG_SYNC_SIGNAL 
    printf("[TID=%ld], PTHREAD_COND_SIGNAL, All_Clear after calling pthread_cond_signal(&wait_on_dependency=%ld)\n", pthread_self(), wait_on_dependency);
#endif
  }
    
  void SetSeqNum (int64_t a) { Seq_Num = a; }
  void set_Status (Command_Status S); // defined in CommandObject.cxx
  void set_Results_Used () { results_used = true; }

  void Result_Uint (uint64_t U) {
    Attach_Result (CRPTR (U));
  }
  void Result_Int (int64_t I) {
    Attach_Result (CRPTR (I));
  }
  void Result_Float (double F) {
    Attach_Result (CRPTR (F));
  }
  void Result_String (std::string S) {
    Attach_Result (CRPTR (S));
  }
  void Result_String (const char *C) {
    Attach_Result (CRPTR (C));
  }
  void Result_RawString (std::string S) {
    Attach_Result (new CommandResult_RawString (S));
  }
  void Result_RawString (const char *C) {
    Attach_Result (new CommandResult_RawString (C));
  }
  void Result_Predefined (CommandResult *C) {
    Attach_Result (C);
  }
  void Result_Predefined (std::list<CommandResult *>& R) {
    CMD_Result.splice( CMD_Result.end(), R);
  }

  std::list<CommandResult *>& Result_List () {
    return CMD_Result;
  }

  void Result_Annotation (std::string S) {
    CMD_Annotation.push_back(new CommandResult_RawString (S));
  }

  std::list<CommandResult_RawString *> Annotation_List () {
    return CMD_Annotation;
  }

 // The following are defined in CommandObject.cxx

 // The simple Print is for dumping information to a trace file.
  void Print (std::ostream& mystream);
 // The Print_Results routine is for sending results to the user.
 // The result returned is "true" if there was information printed.
  bool Print_Results (std::ostream& to, std::string list_seperator, std::string termination_char);


  void copyFile(std::string orig_cmd, std::string source, std::ostream &destination , int starting_offset );

};
