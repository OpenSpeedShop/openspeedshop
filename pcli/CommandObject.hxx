// The CommandObject

// types of results that can be returned in a CommandObject
enum cmd_result_type_enum {
  CMD_RESULT_NULL,
  CMD_RESULT_INT,
  CMD_RESULT_STRING,
};

class CommandResult {
  cmd_result_type_enum Result_Type;

 public:
  CommandResult () {
    Result_Type = CMD_RESULT_NULL; }
  void Set_Type (cmd_result_type_enum T) {
    Result_Type = T; }
  cmd_result_type_enum Type () {
    return Result_Type;
  }

  virtual void Value (char **C) {
    *C = NULL;
  }
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"(none)");
  }
};

class CommandResult_Int : public CommandResult {
  int64_t int_value;

 public:
  CommandResult_Int (int64_t I) {
    Set_Type (CMD_RESULT_INT);
    int_value = I;
  }


  virtual void Value (int64_t *I) {
    *I = int_value;
  };
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"%lld",int_value);
  }
};

class CommandResult_String : public CommandResult {
    std::string string_value;

 public:
  CommandResult_String (std::string S) {
    Set_Type (CMD_RESULT_STRING);
    string_value = S;
  }
  CommandResult_String (char *S) {
    Set_Type (CMD_RESULT_STRING);
    string_value = std::string(S);
  }

  virtual void Value (std::string *S) {
    *S = string_value;
  }
  virtual void Print (FILE *TFile) {
    fprintf(TFile,"\"%s\"",string_value.c_str());
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

// Forward definitions
extern CMDWID Current_ILO_CMDWID ();
extern void Link_Cmd_Obj_to_Input (InputLineObject *I, CommandObject *);

class CommandObject
{
  InputLineObject *Associated_Clip;
  Command_Status Cmd_Status;
  oss_cmd_enum Cmd_Type;
  command_t *Parse_Result;
  std::list<CommandResult *> CMD_Result;

  void Associate_Input ()
  {
    Associated_Clip = Current_ILO;
    Link_Cmd_Obj_to_Input (Associated_Clip, this);
  }

public:
  CommandObject()
  {
    this->Associate_Input ();
    Cmd_Status = CMD_UNKNOWN;
    Parse_Result = NULL;
  }
  CommandObject(oss_cmd_enum T)
  {
    this->Associate_Input ();
    Cmd_Status = CMD_PARSED;
    Cmd_Type = T;
    Parse_Result = NULL;
  }
  CommandObject(command_t *P)
  {
    this->Associate_Input ();
    Cmd_Status = CMD_PARSED;
    Cmd_Type =  P->type;
    Parse_Result = P;
  }

  InputLineObject *Clip () { return Associated_Clip; }
  Command_Status Status () { return Cmd_Status; }
  oss_cmd_enum Type () { return Cmd_Type; }
  command_t *P_Result () { return Parse_Result; }
    
  void set_Status (Command_Status S) { Cmd_Status = S; }
  void set_Type (oss_cmd_enum T) { Cmd_Type = T; }
  void set_P_Result (command_t *P) { Parse_Result = P; }

  void Add_Result (CommandResult *R) {
    CMD_Result.push_back(R);
  }
  void Result_Int (int64_t I) {
    Add_Result (new CommandResult_Int(I));
  }
  void Result_String (std::string S) {
    Add_Result (new CommandResult_String (S));
  }
  void Result_String (char *C) {
    Add_Result (new CommandResult_String (C));
  }

  std::list<CommandResult *> Result_List () {
    return CMD_Result;
  }

  void Print (FILE *TFile) {
    std::list<CommandResult *> cmd_result = Result_List();
    std::list<CommandResult *>::iterator cri;
    int cnt = 0;
    for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
      if (cnt++ > 0) fprintf(TFile,", ");
      (*cri)->Print (TFile);
    }
    if (cnt > 0) fprintf(TFile,"\n");
  }
    
};
