enum RAction { CMD_NONE,
               CMD_COMPLETE,
               CMD_ERROR,
               CMD_PARSE,
               CMD_OPEN_CLI,
               CMD_OPEN_GUI,
               CMD_EXTENDED,
               CMD_TRACE_ON,
               CMD_TRACE_OFF,
               CMD_HISTORY,
               CMD_REDIRECT_INPUT,
               CMD_FOCUS,

               CMD_EXPARG,
               CMD_EXPCLOSE,
               CMD_EXPCREATE,
               CMD_EXPFOCUS,
               CMD_EXPGETMETRICS,
               CMD_EXPLIST,
               CMD_EXPRPT,
               CMD_EXPTGRPDESELECT,
               CMD_EXPSAVEFILE,
               CMD_EXPSHOWPARAMS,
               CMD_EXPSHOWREPORTS,
               CMD_EXPSETPARAM,
               CMD_TGRPGETCG,
               CMD_EXPVIEW,
               CMD_EXPVIEWREPLAY,
               CMD_EXPENABLE,
               CMD_EXPDISABLE,

               CMD_SESSIONHISTORY,
               CMD_SESSIONRECORD,
               CMD_SESSIONREPLAY,
               CMD_ATTACH,
               CMD_CONTINUE,
               CMD_DETACH,
               CMD_HALT,
               CMD_LOAD,
               CMD_RUN,

               CMD_LIST,
               CMD_LISTALIAS,
               CMD_LISTOBJ,
               CMD_LISTEXPERIMENTS,
               CMD_LISTHOSTS,
               CMD_LISTPIDS,
               CMD_LOADSOURCEFILE,
               CMD_ALIAS,
               CMD_UNALIAS,
               CMD_BREAK,
               CMD_HELP,
               CMD_INPUT,
               CMD_LOG,
               CMD_RECORD,
               CMD_SET,
               CMD_UNSET,
               CMD_EXIT,

               CMD_LOCATOR,
             };


// CommandObject
class CommandObject
{
 private:
  CMDID seq_num;		// Order this command was processed.
  time_t cmd_time;		// The time this command was sent.
  CMDWID who;			// Who constructed the command.  (i.e. "gui:PanelID" or "cli")
  bool waitForReturn;		// Always true for now...
  RAction cmd_type;		// What type of information is being communicated.

  std::string command;		// The actual command to be executed.
  std::string msg_string;	// Intermediate processing information.
  CodeObjectLocator *COL;	// For CMD_LOCATOR types
  int64_t num_results;		// If result field is used, how big is it?
  void *result;			// Pointer to unlimited buffer space.

 public:
  CommandObject ()
    { 
      seq_num = 0;
      cmd_time = time(0);
      who = 0;
      waitForReturn = true;
      cmd_type = CMD_NONE;
      command = std::string("");
      msg_string = std::string("");
      COL = NULL;
      num_results = 0;
      result = NULL;
    }
  CommandObject (CMDWID From, std::string Cmd)
    { *this = CommandObject ();
      who = From;
      cmd_type = CMD_PARSE,
      command = Cmd;
    }
  CommandObject (RAction type)
    { *this = CommandObject ();
      cmd_type = type;
    }
  CommandObject (CMDWID From, RAction type)
    { *this = CommandObject ();
      who = From;
      cmd_type = type;
    }
  CommandObject (CMDWID From, RAction type, std::string Cmd)
    { *this = CommandObject ();
      who = From;
      cmd_type = type;
      command = Cmd;
    }

  void SetSeq (CMDID seq) {seq_num = seq;}
  CMDID Seq () { return seq_num; }
  CMDWID Who () { return who;}
  time_t When () { return cmd_time; }
  RAction Action () { return cmd_type; }
  std::string Command () {return command;}

};
