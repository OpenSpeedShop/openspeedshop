/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
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
 * Definition of the InputLineObject class.
 * Definition of the TLI_InputLineObject class.
 * Definition of the GUI_InputLineObject class.
 * Definition of the RLI_InputLineObject class.
 *
 */


// Define the basic input line structure
//
// The intent is that this will become the primary method for tracking
// user commands.  This object needs to be allocated as early as possible
// and needs to contain the current status of the command.  Later processing
// can define the COmmand object and associated methods.


enum Input_Line_Status
{
  ILO_UNKNOWN,
  ILO_QUEUED_INPUT,
  ILO_IN_PARSER,
  ILO_EXECUTING,
  ILO_COMPLETE,
  ILO_ERROR
};

class CommandObject;

// InputLineObject
class InputLineObject
{
 private:
  Input_Line_Status status;	// What is the current state.
  CMDID seq_num;		// Where in sequence that this command was parsed.
  time_t cmd_time;		// When the status field was set.
  CMDWID who;			// Who constructed the command.  (i.e. "gui", "cli", or "?")
  bool complex_expression;	// Is this command part of a more complex expression?

  std::string command;		// The actual command to be executed.

 // The following fields contain "result" information.
  bool semantics_complete;      // Only status and results will be accessed.
  bool results_used;            // This object is no longer needed
  void *LocalCmdId;             // Optional ID to be used by the output routine.
  void (*CallBackLine) (InputLineObject *b);  // Optional call back function to notify output routine.
  void (*CallBackCmd) (CommandObject *b);  // Optional call back function for Command Objects.
  std::string msg_string;	// Intermediate processing or error information.
  oss_cmd_enum cmd_type;	// Filled in when the parser determines the type of command.
  int64_t Num_Cmd_Objs;         // Count the number of CommandObjects on the list.
  std::list<CommandObject *> Cmd_Obj;  // list of associated command objects

 private:
  void Basic_Initialization ()
    { 
      status = ILO_UNKNOWN;
      seq_num = 0;
      cmd_time = time(0);
      who = 0;
      complex_expression = false;
      command = std::string("");
      msg_string = std::string("");
      semantics_complete = false;
      results_used = false;
      LocalCmdId = NULL;
      CallBackLine = NULL;
      CallBackCmd = NULL;
      Num_Cmd_Objs = 0;
    }

 public:
  InputLineObject ()
    { this->Basic_Initialization();
    }
  InputLineObject (CMDWID From, std::string Cmd)
    { this->Basic_Initialization();
      who = From;
      command = Cmd;
    }
  InputLineObject (oss_cmd_enum type)
    { this->Basic_Initialization();
      cmd_type = type;
    }
  InputLineObject (CMDWID From, oss_cmd_enum type)
    { this->Basic_Initialization();
      who = From;
      cmd_type = type;
    }
  InputLineObject (CMDWID From, oss_cmd_enum type, std::string Cmd)
    { this->Basic_Initialization();
      who = From;
      cmd_type = type;
      command = Cmd;
    }
  ~InputLineObject ()
    {
      std::list<CommandObject *> cmd_object = Cmd_Obj;
      std::list<CommandObject *>::iterator coi;
      for (coi = cmd_object.begin(); coi != cmd_object.end(); ) {
        CommandObject *C = (*coi);
        coi++;
        delete C;
      }
    }

  void SetStatus (Input_Line_Status st) {
   // Capture only the first error
    if (status != ILO_ERROR) {
      cmd_time = time(0);
      status = st;
      Default_Log_Output (this);
    }
  }
  void SetSeq (CMDID seq) {seq_num = seq;}
  CMDID Seq () { return seq_num; }
  CMDWID Who () { return who;}
  Input_Line_Status What() { return status; }
  time_t When () { return cmd_time; }
  CMDID Where () { return seq_num; }
  int64_t How_Many () { return Num_Cmd_Objs; }
  bool Complex_Exp () { return complex_expression; }
  bool Semantics_Complete () { return semantics_complete; }
  void Set_Semantics_Complete () { semantics_complete = true; }
  bool Results_Used () { return results_used; }
  std::list<CommandObject *> CmdObj_List () { return Cmd_Obj; }
  void *CallBackId () { return LocalCmdId; }
  bool CallBackL () {
    if (CallBackLine) {
      (*CallBackLine) (this);
      return true;
    } else return false;
  }
  bool CallBackC (CommandObject *c) {
    if (CallBackCmd) {
     (*CallBackCmd) (c);
      return true;
    } else return false;
  }

  void Set_Complex_Exp () { complex_expression = true; }
  void Set_Results_Used () { results_used = true; }
  void Set_CallBackId (void *cbid) { LocalCmdId = cbid; }
  void Set_CallBackL  (void (*cbf) (InputLineObject *b)) { CallBackLine = cbf; }
  void Set_CallBackC  (void (*cbf) (CommandObject *b)) { CallBackCmd = cbf; }
  oss_cmd_enum Action () { return cmd_type; }
  std::string Command () {return command;}

  void Push_Cmd_Obj (CommandObject *C)
  {
    C->SetSeqNum(++Num_Cmd_Objs);
    Cmd_Obj.push_back(C);
  }

  void Print (std::ostream &mystream) {
    CMDWID who = Who();
    Input_Line_Status what = What();
    CMDID seq_num = Where();
    time_t cmd_time = When();
    std::string command = Command();
    mystream << "C " << seq_num << "(" << Num_Cmd_Objs << ")";
    std::string Time = ctime(&cmd_time);
    Time.resize(24);  // Remove any trailing "\n" characters.
    mystream << " (W" << who << "@" << Time << ") ";
    const char *what_c;
    switch (what)
    { 
      case ILO_UNKNOWN:      what_c = "UNKNOWN"; break;
      case ILO_QUEUED_INPUT: what_c = "QUEUED"; break;
      case ILO_IN_PARSER:    what_c = "PARSING"; break;
      case ILO_EXECUTING:    what_c = "EXECUTING"; break;
      case ILO_COMPLETE:     what_c = "COMPLETE"; break;
      case ILO_ERROR:        what_c = "ERROR"; break;
      default:               what_c = "ILLEGAL"; break;
    }
    mystream << what_c << ": ";
    if (command.length() != 0) {
      mystream << command;
      int nline = strlen (command.c_str()) - 1;
      if ((nline <= 0) || (command.c_str()[nline] != *("\n"))) {
        mystream << std::endl;
      }
    } else {
      mystream << std::endl;
    }

   // CommandObject list
    std::list<CommandObject *> cmd_object = Cmd_Obj;
    std::list<CommandObject *>::iterator coi;
    for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
      (*coi)->Print (mystream);
 
     // Check for asnychonous abort command
      if (What() == ILO_ERROR) {
        break;
      }
    }

    mystream << std::flush;  // So that we can look at the file while still running
  }

  void Print_Results (std::ostream &to, std::string list_seperator, std::string termination_char) {
   // Print only the result information attached to each CommandObject
    std::list<CommandObject *> cmd_object = Cmd_Obj;
    std::list<CommandObject *>::iterator coi;
    for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
      (*coi)->Print_Results (to, list_seperator, termination_char);
 
     // Check for asnychonous abort command
      if (What() == ILO_ERROR) {
        break;
      }
    }
  }

};

class TLI_InputLineObject : public InputLineObject
{
 public:
  TLI_InputLineObject (CMDWID From, std::string Cmd) :
    InputLineObject (From, Cmd)
    { }
};

class GUI_InputLineObject : public InputLineObject
{
 public:
  GUI_InputLineObject (CMDWID From, std::string Cmd) :
    InputLineObject (From, Cmd)
    { }
};

class RLI_InputLineObject : public InputLineObject
{
 public:
  RLI_InputLineObject (CMDWID From, std::string Cmd) :
    InputLineObject (From, Cmd)
    { }
};
