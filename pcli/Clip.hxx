/* There would seem to be a need to extend oss_cmd_enum with the following: */
enum Missing_oss_cmd_enum
             { CMD_NONE,
               CMD_COMPLETE,
               CMD_ERROR,
               CMD_PARSE,

               CMD_LOCATOR,
             };

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
  Input_Line_Status status;	// Current state
  CMDID seq_num;		// Order this command was processed.
  time_t cmd_time;		// The time this command was sent.
  CMDWID who;			// Who constructed the command.  (i.e. "gui:PanelID" or "cli")
  bool waitForReturn;		// Always true for now...
  oss_cmd_enum cmd_type;	// What type of information is being communicated.

  std::string command;		// The actual command to be executed.
  std::string msg_string;	// Intermediate processing information.
  CodeObjectLocator *COL;	// For CMD_LOCATOR types
  int64_t num_results;		// If result field is used, how big is it?
  void *result;			// Pointer to unlimited buffer space.
  a_command_struct *Cmd_Obj;	// Pointer to associated Command Object

  void Basic_Initialization ()
    { 
      status = ILO_UNKNOWN;
      seq_num = 0;
      cmd_time = time(0);
      who = 0;
      waitForReturn = true;
      // cmd_type = CMD_NONE;
      command = std::string("");
      msg_string = std::string("");
      COL = NULL;
      num_results = 0;
      result = NULL;
      Cmd_Obj = NULL;
    }

 public:
  InputLineObject ()
    { this->Basic_Initialization();
    }
  InputLineObject (CMDWID From, std::string Cmd)
    { this->Basic_Initialization();
      who = From;
      // cmd_type = CMD_PARSE,
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

  Input_Line_Status Status () {return status;}
  void SetStatus (Input_Line_Status st) {status = st; cmd_time = time(0);}
  void SetSeq (CMDID seq) {seq_num = seq;}
  CMDID Seq () { return seq_num; }
  CMDWID Who () { return who;}
  Input_Line_Status What() {return status;}
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
        fprintf(TFile,"%s\n", command.c_str());
      }
  }

};
