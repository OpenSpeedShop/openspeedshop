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


// InputLineObject
class InputLineObject
{
 private:
  Input_Line_Status status;	// What is the current state.
  CMDID seq_num;		// Where in sequence that this command was parsed.
  time_t cmd_time;		// When the status field was set.
  CMDWID who;			// Who constructed the command.  (i.e. "gui", "cli", or "?")

  std::string command;		// The actual command to be executed.

 // The following fields contain "result" information.
  std::string msg_string;	// Intermediate processing or error information.
  oss_cmd_enum cmd_type;	// Filled in when the parser determines the type of command.
  a_command_struct *Cmd_Obj;	// Pointer to associated Command Object when constructed.

  void Basic_Initialization ()
    { 
      status = ILO_UNKNOWN;
      seq_num = 0;
      cmd_time = time(0);
      who = 0;
      command = std::string("");
      msg_string = std::string("");
      // cmd_type = CMD_NONE;
      Cmd_Obj = NULL;
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
    }

  void SetStatus (Input_Line_Status st) {status = st; cmd_time = time(0);}
  void SetSeq (CMDID seq) {seq_num = seq;}
  CMDID Seq () { return seq_num; }
  CMDWID Who () { return who;}
  Input_Line_Status What() { return status; }
  time_t When () { return cmd_time; }
  CMDID Where () { return seq_num; }
  oss_cmd_enum Action () { return cmd_type; }
  std::string Command () {return command;}

  void Print (FILE *TFile) {
      CMDID seq_num = Where();
      CMDWID who = Who();
      time_t cmd_time = When();
      std::string command = Command();
      fprintf(TFile,"C %lld (W%lld@%.24s): ",
                    seq_num,who,ctime(&cmd_time));
      if (command.length() != 0) {
        fprintf(TFile,"%s", command.c_str());
        int nline = strlen (command.c_str()) - 1;
        if ((nline <= 0) || (command.c_str()[nline] != *("\n"))) {
          fprintf(TFile,"\n");
        }
      }
  }

};
