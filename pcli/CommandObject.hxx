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

class CommandObject
{
  InputLineObject *Associated_Clip; // The input line that caused generation of this object.
  int Seq_Num; // The order this object was generated in from the input line.
  Command_Status Cmd_Status;
  oss_cmd_enum Cmd_Type; // A copy of information in the Parse_Result.
  command_t *Parse_Result;
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
  CommandObject(command_t *P)
  {
    this->Associate_Input ();
    Cmd_Status = CMD_PARSED;
    Cmd_Type =  P->type;
    Parse_Result = P;
    results_used = false;
  }
  ~CommandObject() {
  }

  InputLineObject *Clip () { return Associated_Clip; }
  Command_Status Status () { return Cmd_Status; }
  oss_cmd_enum Type () { return Cmd_Type; }
  bool Results_Used () { return results_used; }
  command_t *P_Result () { return Parse_Result; }
    
  void SetSeqNum (int a) { Seq_Num = a; }
  void set_Status (Command_Status S); // defined in CommandObject.cxx
  void set_Results_Used () { results_used = true; }

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

  void Print (FILE *TFile); // defined in CommandObject.cxx
};
