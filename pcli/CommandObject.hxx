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

// The CommandObject

// types of results that can be returned in a CommandObject
enum cmd_result_type_enum {
  CMD_RESULT_NULL,
  CMD_RESULT_INT,
  CMD_RESULT_FLOAT,
  CMD_RESULT_STRING,
  CMD_RESULT_COLUMN_HEADER,
  CMD_RESULT_COLUMN_VALUES,
};

class CommandResult {
  cmd_result_type_enum Result_Type;

 private:
  CommandResult () {
    Result_Type = CMD_RESULT_NULL; }

 public:
  CommandResult (cmd_result_type_enum T) {
    Result_Type = T; }
  cmd_result_type_enum Type () {
    return Result_Type;
  }

  virtual void Value (char **C) {
    *C = NULL;
  }
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"               (none)");
  }
};

class CommandResult_Int : public CommandResult {
  int64_t int_value;

 public:
  CommandResult_Int (int64_t I) : CommandResult(CMD_RESULT_INT) {
    int_value = I;
  }


  virtual void Value (int64_t *I) {
    *I = int_value;
  };
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"%20lld",int_value);
  }
};

class CommandResult_Float : public CommandResult {
  double float_value;

 public:
  CommandResult_Float (double f) : CommandResult(CMD_RESULT_FLOAT) {
    float_value = f;
  }


  virtual void Value (double *F) {
    *F = float_value;
  };
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"%20f",float_value);
  }
};

class CommandResult_String : public CommandResult {
    std::string string_value;

 public:
  CommandResult_String (std::string S) : CommandResult(CMD_RESULT_STRING) {
    string_value = S;
  }
  CommandResult_String (char *S) : CommandResult(CMD_RESULT_STRING) {
    string_value = std::string(S);
  }

  virtual void Value (std::string *S) {
    *S = string_value;
  }
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"%s",string_value.c_str());
  }
};

class CommandResult_Headers : public CommandResult {
    std::string string_value;

 int64_t number_of_columns;
 std::list<CommandResult *> Headers;

 public:
  CommandResult_Headers () : CommandResult(CMD_RESULT_COLUMN_HEADER) {
    number_of_columns = 0;
  }
  void Add_Header (CommandResult *R) {
    number_of_columns++;
    Headers.push_back(R);
  }

  virtual void Value (int64_t *C) {
    *C = number_of_columns;
  }
  virtual void Print (FILE *TFile) {
    
    std::list<CommandResult *> cmd_object = Headers;
    std::list<CommandResult *>::iterator coi;
    int num_results = 0;
    for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
      if (num_results++ != 0) fprintf(TFile,"  ");
      (*coi)->Print (TFile);
    }

  }
};

class CommandResult_Columns : public CommandResult {
    std::string string_value;

 int64_t number_of_columns;
 std::list<CommandResult *> Columns;

 public:
  CommandResult_Columns (int64_t C) : CommandResult(CMD_RESULT_COLUMN_HEADER) {
    number_of_columns = C;
  }
  void Add_Column (CommandResult *R) {
    Columns.push_back(R);
  }

  virtual void Value (CommandResult *R) {
    *R = *this;
  }
  virtual void Print (FILE *TFile) {
    
    std::list<CommandResult *> cmd_object = Columns;
    std::list<CommandResult *>::iterator coi;
    int num_results = 0;
    for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
      if (num_results++ != 0) fprintf(TFile,"  ");
      (*coi)->Print (TFile);
    }

  }
};

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
  int Seq_Num; // The order this object was generated in from the input line.
  Command_Status Cmd_Status;
  oss_cmd_enum Cmd_Type; // A copy of information in the Parse_Result.
  // command_t *Parse_Result;
  // command_type_t *Parse_Result;
  OpenSpeedShop::cli::ParseResult *PR;
  bool results_used; // Once used, this object can be deleted!
  std::list<CommandResult *> CMD_Result;

  void Associate_Input ()
  {
    Associated_Clip = Current_ILO;
    Link_Cmd_Obj_to_Input (Associated_Clip, this);
  }

  void Add_Result (CommandResult *R) {
    CMD_Result.push_back(R);
  }

  CommandObject() { } // Hide default constructor to catch errors at compile time

public:
//  CommandObject(command_t *P)
//  {
//    this->Associate_Input ();
//    Cmd_Status = CMD_PARSED;
//    Cmd_Type =  P->type;
//    Parse_Result = P;
//    PR = NULL;
//    results_used = false;
//  }
  CommandObject(OpenSpeedShop::cli::ParseResult *pr)
  {
    this->Associate_Input ();
    Cmd_Status = CMD_PARSED;
    Cmd_Type =  pr->GetCommandType();
//    Parse_Result = NULL;
    PR = pr;
    results_used = false;
  }
  ~CommandObject() {
  }

  InputLineObject *Clip () { return Associated_Clip; }
  Command_Status Status () { return Cmd_Status; }
  oss_cmd_enum Type () { return Cmd_Type; }
  bool Results_Used () { return results_used; }
  OpenSpeedShop::cli::ParseResult *P_Result () { return PR; }
  // command_t *P_Result () { return Parse_Result; }
  //command_type_t *P_Result () { return Parse_Result; }
    
  void SetSeqNum (int a) { Seq_Num = a; }
  void set_Status (Command_Status S); // defined in CommandObject.cxx
  void set_Results_Used () { results_used = true; }

  void Result_Int (int64_t I) {
    Add_Result (new CommandResult_Int(I));
  }
  void Result_Float (double F) {
    Add_Result (new CommandResult_Float(F));
  }
  void Result_String (std::string S) {
    Add_Result (new CommandResult_String (S));
  }
  void Result_String (char *C) {
    Add_Result (new CommandResult_String (C));
  }
  void Result_Predefined (CommandResult *C) {
    Add_Result (C);
  }

  std::list<CommandResult *> Result_List () {
    return CMD_Result;
  }

 // defined in CommandObject.cxx
  void Print (FILE *TFile);
  void Print_Results (FILE *TFile, std::string list_seperator, std::string termination_char);
};
