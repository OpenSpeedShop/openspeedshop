typedef int64_t EXPID;
typedef int64_t CMDID;
typedef int64_t CMDWID;

// ResultObject
enum RStatus { SUCCESS,
               PARTIAL_SUCCESS,
               FAILURE,
               EXIT };
class ResultObject
{
 private:
  RStatus status;    // Did the command complete or not...
  std::string result_type;
  std::string msg_string;
  int64_t num_results;
  void *result;      // A pointer to the data in the FrameWork.    The
                     // gui or cli should know how to interpret the data
                     // base on the associated commandObject and the
                     // result_type.

 public:
  void SetResultObject_StatusError (RStatus s, std::string e)
    { status = s;
      msg_string = e;
    }
  void SetResultObject_TypeValue (std::string t, void *r)
    { result_type = t;
      result = r;
    }

  // Constructors
  ResultObject ()
    { status = FAILURE;
      result_type = std::string("");
      result = NULL;
      msg_string = std::string("");
    }
  ResultObject (enum RStatus s)
    { *this = ResultObject ();
      status = s;
    }
  ResultObject (enum RStatus s, std::string t, void *r, std::string e)
    { *this = ResultObject ();
      SetResultObject_StatusError (s, e);
      SetResultObject_TypeValue (t, r);
    }
  ResultObject (enum RStatus s, char *t, void *r, char * e)
    { *this = ResultObject ( s, std::string(t ? t : ""), r, std::string(e ? e : ""));
    }
  ResultObject (RStatus s, std::string e)
    { *this = ResultObject ();
      SetResultObject_StatusError (s, e);
    }
  ResultObject (RStatus s, char * e)
    { *this = ResultObject ( s, std::string(e ? e : ""));
    }

  RStatus Status () { return status; }
  std::string Type () { return result_type; }
  void *Result () { return result; }
  std:: string ResultMsg () { return msg_string; }
};

class CodeObjectLocator
{
 private:
  std::string Host_Name;
  std::string File_Name;
  std::string PID_Name;
  std::string Rank_Name;
  std::string Thread_Name;

 public:
  CodeObjectLocator () {
    Host_Name = File_Name = PID_Name = Rank_Name = Thread_Name = std::string("");
  }
  CodeObjectLocator (std::string H, std::string F, std::string P, std::string R, std::string T) {
    Host_Name = H;
    File_Name = F;
    PID_Name  = P;
    Rank_Name = R;
    Thread_Name = T;
  }

  void Dump (FILE *TFile) {
    if (Host_Name.length()) fprintf(TFile,"-h %s ",Host_Name.c_str());
    if (File_Name.length()) fprintf(TFile,"-f %s ",File_Name.c_str());
    if (PID_Name.length()) fprintf(TFile,"-p %s ",PID_Name.c_str());
    if (Rank_Name.length()) fprintf(TFile,"-r %s ",Rank_Name.c_str());
    if (Thread_Name.length()) fprintf(TFile,"-t %s ",Thread_Name.c_str());
  }
  void dump (FILE *TFile) {
    this->Dump(TFile);
  }
};

class CommandWindowID;
class CommandObject;

CMDWID Commander_Initialization (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel);
void Commander_Termination (CMDWID my_window);

// Selection of items in the trace file are controlled throught his enum.
// Except for raw data dumps, the record identifier is stripped from output.
enum Trace_Entry_Type
  { CMDW_TRACE_ALL,                  // Dump raw data
    CMDW_TRACE_COMMANDS,             // Dump command line records with added information - "C "
    CMDW_TRACE_ORIGINAL_COMMANDS,    // Dump command line recoreds as entered by user - "C "
    CMDW_TRACE_RESULTS               // Dump result records  - "R "
  };

void Command_Trace (enum Trace_Entry_Type trace_type, CMDWID cmdwinid, std::string tofname);
ResultObject Command_Trace_OFF (CMDWID WindowID);
ResultObject Command_Trace_ON (CMDWID WindowID, std::string tofname);

ResultObject Attach_Input_File (CMDWID issuedbywindow, std::string fromfname);
ResultObject Attach_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr);

void List_CommandWindows ( FILE *TFile );
