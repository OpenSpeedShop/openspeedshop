/******************************************************************************e
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2011 Krell Institute  All Rights Reserved.
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
extern "C" void loadTheGUI(ArgStruct *);

//#define DEBUG_CLI 1

#include <fstream>
#include <iostream>

const char *Current_OpenSpeedShop_Prompt = "openss>>";
const char *Alternate_Current_OpenSpeedShop_Prompt = "....ss>>";

pthread_mutex_t Async_Input_Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  Async_Input_Available = PTHREAD_COND_INITIALIZER;
bool            Shut_Down = false;
static bool     Waiting_For_Complex_Cmd = false;
int		AsyncInputLockCount = 1000;

int64_t History_Count = 0;
std::list<std::string> History;

static FILE *ttyin = NULL;  // Read directly from this xterm window.

static ss_ostream *ss_err = NULL;
static ss_ostream *ss_out = NULL;
static ss_ostream *ss_ttyout = NULL;
static std::ofstream *ttyout_stream = NULL;

// Forward definitions of local functions
CommandWindowID *Find_Command_Window (CMDWID WindowID);
void Default_TLI_Command_Output (CommandObject *C);
bool All_Command_Objects_Are_Used (InputLineObject *clip);

inline std::string ptr2str (void *p) {
  char s[40];
  sprintf ( s, "%p", p);
  return std::string (s);
}

// Local Macros

static inline
std::ostream *Predefined_ofstream (std::string oname)
{
  if (oname.length() == 0) {
    return NULL;
  } if (oname == "stdout") {
    return (ss_out != NULL) ? &ss_out->mystream () : &std::cout;
  } else if (oname == "stderr") {
    return (ss_err != NULL) ? &ss_err->mystream () : &std::cerr;
  } else if (oname == "/dev/tty") {
    return (ss_ttyout != NULL) ? &ss_ttyout->mystream () : &std::cout;
  } else {
    return NULL;
  }
}

ss_ostream *Predefined_ostream (std::string oname)
{
  if (oname.length() == 0) {
    return NULL;
  } if (oname == "stdout") {
    return ss_out;
  } else if (oname == "stderr") {
    return ss_err;
  } else if (oname == "/dev/tty") {
    return ss_ttyout;
  } else {
    return NULL;
  }
}

static inline
FILE *predefined_filename (std::string filename)
{
  if (filename.length() == 0) {
    return NULL;
  } if (!strcmp( filename.c_str(), "stdout")) {
    return stdout;
  } else if (!strcmp( filename.c_str(), "stderr")) {
    return stderr;
  } else if (!strcmp( filename.c_str(), "stdin" )) {
    return stdin;
  } else if (!strcmp( filename.c_str(), "/dev/tty")) {
    return stdout;
  } else {
    return NULL;
  }
}

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
static CMDID Command_Sequence_Number = 0;

// To the outside world, Window_IDs are just simple integers.
// Allow only one thread at a time to allocate a Window ID or to
// add, remove or search the list of defined windows.
static pthread_mutex_t Window_List_Lock = PTHREAD_MUTEX_INITIALIZER;
static std::list<CommandWindowID *> CommandWindowID_list;
static CommandWindowID *Embedded_CommandWindow = NULL;
static CMDWID Command_Window_ID = 0;
static CMDWID Last_ReadWindow = 0;
static bool Async_Inputs = false;
static bool Looking_for_Async_Inputs = false;
static CMDWID More_Input_Needed_From_Window = 0;

// How may "!" commands are being executed?
static pthread_mutex_t Escape_Command_Lock = PTHREAD_MUTEX_INITIALIZER;
static int64_t Number_of_Escape_Commands = 0;
static std::list<pid_t> escape_processes;

// Input_Source
#define DEFAULT_INPUT_BUFFER_SIZE 4096

class Input_Source
{
 protected:
  Input_Source *Next_Source;
  bool Predefined;
  std::string Name;
  FILE *Fp;
  int64_t Buffer_Size;
  int64_t Next_Line_At;
  int64_t Last_Valid_Data;
  char *Buffer;
  InputLineObject *Input_Object;
  bool Record_To_A_Predefined_File;
  std::string Record_Name;
  std::ostream *Record_Stream;

 // Used if files are to be read
 // These pointers are copied to the InputLineObject generated for each line from the file.
  void (*CallBackLine) (InputLineObject *b);  // Optional call back function to notify output routine.
  void (*CallBackCmd) (CommandObject *b);  // Optional call back function for Command Objects.


 public:
  // Constructor & Destructor
  Input_Source (std::string my_name) {
    Next_Source = NULL;
    Name = my_name;
    Fp = predefined_filename (my_name);
    if (Fp == NULL) {
      Predefined = false;
      Fp = fopen (my_name.c_str(), "r");
      if (Fp == NULL) {
       // Calling routines must use InFileError() to check for error.
      }
    } else {
      Predefined = true;
    }
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = DEFAULT_INPUT_BUFFER_SIZE;
    Buffer = (char *)malloc(Buffer_Size);
    Input_Object = NULL;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_Stream = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  Input_Source (InputLineObject *clip) {
    Next_Source = NULL;
    Name = std::string("");
    Fp = NULL;
    Predefined = false;
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = 0;
    Buffer = NULL;
    Input_Object = clip;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_Stream = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  Input_Source (int64_t buffsize, char *buffer) {
    Next_Source = NULL;
    Name = std::string("");
    Fp = NULL;
    Predefined = false;
    Next_Line_At = 0;
    Last_Valid_Data = buffsize-1;
    Buffer_Size = buffsize;
    Buffer = buffer;
    Input_Object = NULL;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_Stream = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  ~Input_Source () {
   /* Assume that this routine has ownership of any buffer it was given. */
    if (Buffer) {
      free (Buffer);
    }
   /* Close input files. */
    if (Fp) {
      fclose (Fp);
    }
   /* Close Record files. */
    if (Record_Stream && !Record_To_A_Predefined_File) {
      delete Record_Stream;
    }
  }

  void Set_CallBackL  (void (*cbf) (InputLineObject *b)) { CallBackLine = cbf; }
  void Set_CallBackC  (void (*cbf) (CommandObject *b)) { CallBackCmd = cbf; }
  void Copy_CallBack  (InputLineObject *clip) {
    if (CallBackLine != NULL) clip->Set_CallBackL (CallBackLine);
    if (CallBackCmd  != NULL) clip->Set_CallBackC (CallBackCmd); 
  }

  void Link (Input_Source *inp) { Next_Source = inp; }
  Input_Source *Next () { return Next_Source; }
  InputLineObject *InObj () { return Input_Object; }
  bool InFileError () { return (Fp == NULL); }
  bool Is_File () { return (Fp != NULL); }
  std::string File_Name () { return Name; }

  char *Get_Next_Line () {

    if (Next_Line_At >= Last_Valid_Data) {
      if (Fp == NULL) {
        return NULL;
      }
      Buffer[0] = *("\0");
      Buffer[Buffer_Size-1] = (char)0;
      Next_Line_At = 0;
      Last_Valid_Data = 0;
      fgets (&Buffer[0], Buffer_Size, Fp);
      if (Buffer[Buffer_Size-1] != (char)0) {
        ss_ostream *this_ss_stream = ((ss_ttyout != NULL) && isatty(fileno(stderr)))
                                            ? ss_ttyout : ss_err;
        if (this_ss_stream != NULL) {
          this_ss_stream->acquireLock();
          this_ss_stream->mystream()
                << "ERROR: Input line from " << Name << " is too long for buffer.\n" << std::flush;
          if (ss_ttyout == this_ss_stream) {
	    this_ss_stream->Issue_Prompt();
	  }
          this_ss_stream->releaseLock();

        } else {
          std::cerr << "ERROR: Input line from " << Name << " is too long for buffer.\n" << std::flush;
        }
        return NULL;
      }
      Last_Valid_Data = strlen(&Buffer[0]);
    }
    char *next_line = &Buffer[Next_Line_At];
    int64_t line_len = strlen(next_line);
    int64_t i;
    for (i = 1; i < line_len; i++) {  
      if (next_line[i] == *("\0")) {
        line_len = i;
        break;
      }
    }
    if (next_line[0] == *("\0")) {
     /* This indicates an EOF */
      return NULL;
    }
    Next_Line_At += line_len;
    return next_line;
  }

  bool Set_Record ( std::string tofname) {
    if (Record_Stream && !Record_To_A_Predefined_File) {
      delete Record_Stream;
    }
    std::ostream *tof = Predefined_ofstream (tofname);
    Record_Name = tofname;
    Record_To_A_Predefined_File = (tof != NULL);
    if (tof == NULL) {
      tof = new std::ofstream (tofname.c_str(), std::ios::out);
    }
    Record_Stream = tof;
    return (tof != NULL);
  }
  void Remove_Record () {
    if (Record_Stream && !Record_To_A_Predefined_File) {
      delete Record_Stream;
    }
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_Stream = NULL;
  }
  bool Record_File_Is_Predefined () { return Record_To_A_Predefined_File; }
  std::string Record_File_Name () { return Record_Name; }
  std::ostream *Record_Ostream () { return Record_Stream; }

  // Debug aids
  void Dump (std::ostream &mystream) {
    bool nl_at_eol = false;
    mystream << "    Read from: " << ((Fp) ? Name : (Input_Object) ? "image " : "buffer ");
    if (Input_Object != NULL) {
      Input_Object->Print (mystream);
      nl_at_eol = true;
    } else if (!Fp) {
      mystream << "len=" << Buffer_Size << ", next=" << Next_Line_At;
      if (Buffer_Size > Next_Line_At) {
        int64_t nline = MIN (20,strlen (&(Buffer[Next_Line_At])));
        nl_at_eol = (Buffer[Next_Line_At+nline] == *("\n") );
        if (nline > 2) {
          mystream << ": " << std::setiosflags(std::ios::left) << std::setw(20) << Buffer;
          if (nline > 20) mystream << "...";
        }
      }
    }
    if (!nl_at_eol) {
      mystream << std::endl;
    }
    if (Record_Stream) {
      mystream << "     record to: "<< Record_Name << std::endl;
    }
  }
};

// CommandWindowID
class CommandWindowID
{
 protected:
  CMDWID id;
  ss_ostream *default_outstream;
  ss_ostream *default_errstream;
  bool remote;
  std::string I_Call_Myself;
  std::string Host_ID;
  pid_t Process_ID;
  int64_t Panel_ID;
  int64_t *MsgWaitingFlag;
  int64_t Current_Input_Level;
  int64_t Cmd_Count_In_Log_File;

  bool Input_Is_Async;
  EXPID FocusedExp;

  pthread_mutex_t Input_List_Lock;
  Input_Source *Input;

  pthread_mutex_t Log_File_Lock;
  std::string Log_File_Name;
  std::ostream *Log_Stream;
  bool Log_File_Is_A_Temporary_File;

  std::string Record_File_Name;
  std::ostream *Record_Stream;
  bool Record_File_Is_A_Temporary_File;

  pthread_mutex_t Cmds_List_Lock;
  int CmdsListLockCount ; 
  bool Waiting_For_Cmds_Complete;
  pthread_cond_t  Wait_For_Cmds_Complete;
  std::list<InputLineObject *>Complete_Cmds;

 public:
  // Virtual functions for creating InputLineObjects
  // This is the default for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new InputLineObject (From, Cmd);
    }

  // Constructor & Destructor
 public:
  CommandWindowID () {
   // Disallow default constructor.
   // It can not be private because of inheritance.
   Assert(0);
  }

 public:
  CommandWindowID ( std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async)
    {
      default_outstream = ss_out;
      default_errstream = ss_err;
      remote = false;
      I_Call_Myself = IAM;
      Host_ID = Host;
      Process_ID = Process;
      Panel_ID = Panel;
      Current_Input_Level = 0;
      Cmd_Count_In_Log_File = 0;
      Log_File_Name = "";
      Log_Stream = NULL;
      Log_File_Is_A_Temporary_File = false;
      Record_File_Name = "";
      Record_Stream = NULL;
      Record_File_Is_A_Temporary_File = false;
      Input = NULL;
      Input_Is_Async = async;

      Assert(pthread_mutex_init(&Input_List_Lock, NULL) == 0); // dynamic initialization

      Assert(pthread_mutex_init(&Log_File_Lock, NULL) == 0);   // dynamic initialization

      Assert(pthread_mutex_init(&Cmds_List_Lock, NULL) == 0);  // dynamic initialization
      Waiting_For_Cmds_Complete = false;

      Assert(pthread_cond_init(&Wait_For_Cmds_Complete, (pthread_condattr_t *)NULL) == 0);

      FocusedExp = -1;  // This is a "not yet intitialized" flag.  The user should never see it.

     // Generate a unique ID and remember it

     // Get exclusive access to the lock so that only one
     // add/remove/search of the list is done at a time.
      Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

      id = ++Command_Window_ID;
      CommandWindowID_list.push_front(this);

     // Release the lock.
      Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

     // Allocate a log file for commands associated with this window
      if (OPENSS_LOG_BY_DEFAULT) {
        char base[20];
        snprintf(base, 20, "sstr%lld.XXXXXX",id);
        Log_File_Name = std::string(tempnam ("./", &base[0] )) + ".openss";
        Log_Stream = new std::ofstream (Log_File_Name.c_str(), std::ios::out);
        Log_File_Is_A_Temporary_File = true;
      }
      Record_File_Name = "";
      Record_Stream = NULL;
      Record_File_Is_A_Temporary_File = false;
    }
  ~CommandWindowID()
    {
     // Be sure that all the output has been generated.
      Remove_Completed_Input_Lines(false);

     // Clear the identification field in case someone trys to reference
     // this entry again.
      id = 0;
     // Remove the log files
      if ((Log_Stream != NULL) &&
          (predefined_filename (Log_File_Name) == NULL)) {
        delete Log_Stream;
        Log_Stream = NULL;
        if (Log_File_Is_A_Temporary_File) {
          (void ) remove (Log_File_Name.c_str());
          Log_File_Is_A_Temporary_File = false;
        }
      }
     // Remove the record files
      if ((Record_Stream != NULL) &&
          (Predefined_ofstream (Record_File_Name) == NULL)) {
        delete Record_Stream;
      }
     // Remove the input specifiers
      if (Input) {
        for (Input_Source *inp = Input; inp != NULL; ) {
          Input_Source *next = inp->Next();
          delete inp;
          inp = next;
        }
        Input = NULL;
      }
     // Remove the control structures associate with the lock
      pthread_mutex_destroy(&Input_List_Lock);
      pthread_mutex_destroy(&Log_File_Lock);
      pthread_mutex_destroy(&Cmds_List_Lock);
      pthread_cond_destroy (&Wait_For_Cmds_Complete);

     // Unlink from the chain of windows
      if ((Embedded_CommandWindow != this) &&
          ((*CommandWindowID_list.begin()) != (*CommandWindowID_list.end()))) {

       // Get exclusive access to the lock so that only one
       // add/remove/search of the list is done at a time.
        Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

        CommandWindowID_list.remove(this);

       // Release the lock.
        Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);
      }

     // Reclaim ss_ostream structures if not used in another window.
      if (command_line_window != 0) {
#if DEBUG_CLI
        std::cerr << " CommandWindowID, command_line_window=" << command_line_window << std::endl;
#endif
        CommandWindowID *dw = Find_Command_Window (command_line_window);

        if (default_outstream == dw->ss_outstream()) default_outstream = NULL;
        if (default_outstream == dw->ss_errstream()) default_outstream = NULL;
        if (default_errstream == dw->ss_outstream()) default_errstream = NULL;
        if (default_errstream == dw->ss_errstream()) default_errstream = NULL;
      }
      if (default_errstream &&
          (default_errstream != default_outstream) &&
          (default_errstream != ss_ttyout) &&
          (default_errstream != ss_out) &&
          (default_errstream != ss_err)) {
        delete default_errstream;
        default_errstream = NULL;
      }
      if (default_outstream &&
          (default_outstream != ss_ttyout) &&
          (default_errstream != ss_out) &&
          (default_errstream != ss_err)) {
        delete default_outstream;
        default_outstream = NULL;
      }
    }

  void Purge_All_Input () {

   // Get the lock to this window's inputs
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

   // Go throught the list of remaining inputs and delete them.
    for ( ; Input != NULL; ) { this->Pop_Input_Source(); }

   // Release the lock
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

  }

  void Abort_Executing_Input_Lines () {

   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Look through the list for unneeded Commands
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = Complete_Cmds.begin(); cmi != Complete_Cmds.end(); cmi++) {
      InputLineObject *clip = *cmi;
      std::list<CommandObject *> cmd_object = clip->CmdObj_List();
      std::list<CommandObject *>::iterator coi;
      for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
        if (((*coi)->Status() == CMD_PARSED) ||
            ((*coi)->Status() == CMD_EXECUTING)) {
          (*coi)->set_Status (CMD_ABORTED);
          (*coi)->set_Results_Used ();
        }
      }
     // Terminate the Clip after aborting the CommandObjects so that
     // the final status of commands is entered in the log file.
      clip->SetStatus (ILO_ERROR);
      clip->Set_Results_Used ();
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }

  void Wake_Up_Reader () {

   // After a new comand is placed in the input window,
   // wake up a sleeping input reader.
    if (Looking_for_Async_Inputs) {

      Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);

     // After we get the lock, be sure that the reader
     // is still waiting for input. No need to send a
     // signal if it grabbed the last line before we
     // got ahold of the lock.
      if (Looking_for_Async_Inputs) {
        Looking_for_Async_Inputs = false;

        Assert(pthread_cond_signal(&Async_Input_Available) == 0);

      }

      Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);

    }
  }

  void Append_Input_Source (Input_Source *inp) {

   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    if (Input == NULL) {
      Input = inp;
    } else {
      Input_Source *previous_inp = Input;
      while (previous_inp->Next() != NULL) {
        previous_inp = previous_inp->Next();
      }
      previous_inp->Link(inp);
    }

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
    Wake_Up_Reader ();
}


  void Push_Input_Source (Input_Source *inp) {

   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    Input_Source *previous_inp = Input;
    inp->Link(previous_inp);
    Input = inp;

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    Wake_Up_Reader ();
  }
  void TrackCmd (InputLineObject *Clip) {

   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Add a new in-process command to the list.
    Complete_Cmds.push_back (Clip);

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);

  }

  void Remove_Completed_Input_Lines (bool issue_prompt) {

   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Look through the list for unneeded lines
    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); ) {
      InputLineObject *L = (*cmi);
      if (!L->Results_Used ()) {
        if (L->Semantics_Complete () &&
            (All_Command_Objects_Are_Used( (*cmi) ))) {
          if (!(L->CallBackL ()) &&
              (L->What() != ILO_COMPLETE) &&
              (L->What() != ILO_ERROR)) {
            L->SetStatus (ILO_COMPLETE);
          }
        } else {
          break;  // so that things are removed in order
        }
      }
      ++cmi;
      if (L->Semantics_Complete () &&
          (L->Results_Used ())) {
        Complete_Cmds.remove(L);
        delete L;
      }
    }

   // When all input has been processed,
   // issue a new prompt to the user.
    if (issue_prompt &&
        !Shut_Down &&
        !Waiting_For_Complex_Cmd &&
        (Complete_Cmds.begin() == Complete_Cmds.end()) &&
        (Input == NULL) &&
        has_outstream ()) {
     // Re-issue the prompt

     // If the output is associated with the command line,
     // determine if the output stream is shared between windows.
      CommandWindowID *cw = NULL;
      if (command_line_window == id) {
#if DEBUG_CLI
        std::cerr << " Remove_Completed_Input_Lines, tli_window=" << tli_window << std::endl;
#endif
        if (tli_window != 0) {
         // Command line and TLI share a window.  

          cw = Find_Command_Window (tli_window);

        } else if (gui_window != 0) {
         // Command line and GUI share a window.  
#if DEBUG_CLI
          std::cerr << " Remove_Completed_Input_Lines, gui_window=" << gui_window << std::endl;
#endif
          cw = Find_Command_Window (gui_window);
        }
      }
        
      if ((cw != NULL) &&
          (cw->has_outstream())) {
       // Let the other window issue the prompt, when it's ready.
        cw->Remove_Completed_Input_Lines (true);
      } else {
       // Issue the prompt here.
        ss_ostream *this_ss_stream = ss_outstream();
        this_ss_stream->acquireLock();
        this_ss_stream->Issue_Prompt();
        this_ss_stream->releaseLock();
      }
    } else if (Waiting_For_Cmds_Complete &&
               !Shut_Down &&
               !Waiting_For_Complex_Cmd &&
               (Complete_Cmds.size() == 1)) {
     // Input is waiting at a nested Python command.
     // We only the command that started the transitiion
     // is left, feed more input to Python.
      Waiting_For_Cmds_Complete = false;

      Assert(pthread_cond_signal(&Wait_For_Cmds_Complete) == 0);
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }

  void Remove_Null_Input_Lines () {
   // This routine can only safely be called when Python
   // is outside a nested construct.

   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Look through the list for lines with no CommandObjects.
   // These may have been processed by Python and may not have
   // generated any CommandObjects.
    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); cmi++) {
      InputLineObject *L = (*cmi);
      if (L->Complex_Exp()) {
       // Part of a Complex_Exp.
       // It must be complete, or we wouldn't be here!
        L->SetStatus (ILO_COMPLETE);
        L->Set_Semantics_Complete ();
      } else if ((L->What () == ILO_IN_PARSER) &&
                 (L->How_Many () == 0)) {
       // Parser command or error.
        L->SetStatus (ILO_COMPLETE);
        L->Set_Semantics_Complete ();
      }
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);

  }

private:
  void Pop_Input_Source () {
   // We do not need to get exclusive access to Input_List_Lock
   // Because the only path to this routine is through Read_Command,
   // which already has exclusive use of the lock.

    Input_Source *old = Input;
    Input = Input->Next();
    delete old;
  }
public:
  InputLineObject *Read_Command () {
   // Is there a good chance of finding something to read?
   // We aren't going to go to the trouble of getting the lock
   // unless there is something waiting to be processed.
    if (Input == NULL) {
     // We don't think there is anything to read.
     //
     // Yes, there is a race condiiton here, but the worst that
     // can happen is that we miss reading a line of input as
     // soon as we could.  Since this read is in a loop, we will
     // eventually get back here to read the new line.
      return NULL;
    }

   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    InputLineObject *clip;
    char *next_line = NULL;

    while (Input != NULL) {
      clip = Input->InObj ();
      if (clip != NULL) {
       // Return this line and remove it from the input stack
        Pop_Input_Source();
        break;
      } else {
       // Read file or buffer to get input.
        next_line = Input->Get_Next_Line ();
        if (next_line != NULL) {
          break;
        }
        Pop_Input_Source();
      }
    }

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    if (clip == NULL) {
      if (next_line == NULL) {
        return NULL;
      }
      clip = New_InputLineObject ( ID(), next_line);
      Input->Copy_CallBack (clip);
    }

    return clip;
  }

  bool Input_Async () {
    return  ((Input == NULL) ? Input_Is_Async : true);
  }
  bool Input_Queued () {
    bool there_is_input = false;

    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    if (Input != NULL) {
      there_is_input = (Input->InObj() != NULL);
    }

    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    return there_is_input;
  }
  bool Input_File () {
    bool there_is_input = false;

    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    if (Input != NULL) {
      there_is_input = (Input->Is_File());
    }

    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    return there_is_input;
  }
  std::string Input_File_Name () {
    std::string N = std::string ("");

    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    if (Input != NULL) {
      if (Input->Is_File()) {
        N = Input->File_Name();
      }
    }

    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    return N;
  }
  bool Input_Available () {
    return (Input_Async() || Input_Queued() || Input_File());
  }
  bool Cmds_BeingProcessed () {

    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

    bool there_are_some = (Complete_Cmds.size() != 0);

    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);

    return there_are_some;
  }
  void Wait_Until_Cmds_Complete () {

    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

    if (Complete_Cmds.size() > 1) {

      Waiting_For_Cmds_Complete = true;

      Assert(pthread_cond_wait(&Wait_For_Cmds_Complete,&Cmds_List_Lock) == 0);
    }

    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
    return;
  }

  // Field access
  void    set_outstream (ss_ostream *output) { default_outstream = output; }
  void    set_errstream (ss_ostream *errput) { default_errstream = errput; }
  bool has_outstream () { return (default_outstream != NULL); }
  std::ostream &outstream () { return default_outstream->mystream(); }
  ss_ostream *ss_outstream () { return default_outstream; }
  bool has_errstream () { return (default_errstream != NULL); }
  std::ostream &errstream () { return default_errstream->mystream(); }
  ss_ostream *ss_errstream () { return default_errstream; }
  CMDWID  ID () { return id; }
  std::string IAM () { return I_Call_Myself; }
  EXPID   Focus () { return FocusedExp; }
  void    Set_Focus (EXPID exp) { FocusedExp = exp; }
  bool    Async() { return Input_Is_Async; }
  int64_t Input_Level () { return Current_Input_Level; }
  void    Increment_Level () { Current_Input_Level++; }
  void    Decrement_Level () { Current_Input_Level--; }

 // The "Log" command will causes us to echo state changes that
 // are associate with a particular input stream to a user defined file.
  void   Print_Log_File ( InputLineObject *clip ) {
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    if (Log_Stream != NULL) {
      clip->Print (*Log_Stream);
    }
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
  }
  void   Print_Log_File ( CommandObject *C ) {
    // Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    if (Log_Stream != NULL) {
      C->Print (*Log_Stream);
    }
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
    // Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }
  bool   Set_Log_File ( std::string tofname ) {
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    std::ostream *tof = Predefined_ofstream (tofname);
    std::ios_base::openmode open_mode = std::ios::out;  // The default is to overwrite existing files.
    if ((Log_Stream != NULL) &&
        (predefined_filename (Log_File_Name) == NULL)) {
     // Copy the old file to the new file.
      if ((tof == NULL)  &&
          (tofname != Log_File_Name)) {
        int64_t len1 = Log_File_Name.length();
        int64_t len2 = tofname.length();
        std::string scmd = std::string("mv ") + Log_File_Name + " " + tofname;
        int64_t ret = system(scmd.c_str());
        if (ret != 0) {
         // Some system error.  Keep the old log file around.
          Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
          return false;
        }
        open_mode = std::ios::app;  // Save recently generated records.
        Log_File_Is_A_Temporary_File = false;
      }
    }
    if (Log_File_Is_A_Temporary_File) {
      delete Log_Stream;
      (void) remove (Log_File_Name.c_str());
    }
    if (tof == NULL) {
      tof = new std::ofstream (tofname.c_str(), open_mode);
    }
    Log_File_Name = tofname;
    Log_Stream = tof;
    Log_File_Is_A_Temporary_File = false;
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
    return (Log_Stream != NULL);
  }
  void   Remove_Log_File () {
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    if ((Log_Stream != NULL)  &&
        (predefined_filename (Log_File_Name) == NULL)) {
      delete Log_Stream;
      if (Log_File_Is_A_Temporary_File) {
        (void) remove (Log_File_Name.c_str());
      }
    }
    Log_File_Is_A_Temporary_File = false;
    Log_File_Name = std::string("");
    Log_Stream = NULL;
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
  }

 // The "Record" command will causes us to echo statements that
 // come from a particular input stream to a user defined file.
  bool Set_Record_File ( std::string tofname ) {
    bool file_was_set = false;
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    if (Input) {
      file_was_set = (Input->Set_Record ( tofname ));
    } else {
      if (Record_Stream && !Record_File_Is_A_Temporary_File) {
        delete Record_Stream;
      }
      std::ostream *tof = Predefined_ofstream (tofname);
      Record_File_Name = tofname;
      Record_File_Is_A_Temporary_File = (tof != NULL);
      if (tof == NULL) {
        tof = new std::ofstream (tofname.c_str(), std::ios::out);
      }
      Record_Stream = tof;
      file_was_set = (tof != NULL);
    }
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
    return file_was_set;
  }
  void   Remove_Record_File () {
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    if (Input) {
      Input->Remove_Record ();
    } else {
      if (Record_Stream && !Record_File_Is_A_Temporary_File) {
        delete Record_Stream;
      }
      Record_File_Is_A_Temporary_File = false;
      Record_File_Name = std::string("");
      Record_Stream = NULL;
    }
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
  }
  void Record (InputLineObject *clip) {
    Assert(pthread_mutex_lock(&Log_File_Lock) == 0);
    std::ostream *tof = NULL;
    bool is_predefined = false;
    if (Input) {
      tof = Input->Record_Ostream();
      is_predefined = Input->Record_File_Is_Predefined();
    } else {
      tof = Record_Stream;
      is_predefined = Record_File_Is_A_Temporary_File;
    }
    if (tof != NULL) {
      *tof << clip->Command();  // Commands have a "\n" at the end.
      if (is_predefined) {
       // Record_File is something like stdout, so push the message out to user.
        *tof << std::flush;
      }
    }
    Assert(pthread_mutex_unlock(&Log_File_Lock) == 0);
  }

  // For error reporting
  void Print_Location(std::ostream &mystream) {
    mystream << Host_ID << " " << Process_ID;
  }
  // Debug aids
  bool Print_Queued_Cmds (std::ostream &mystream) {
   // Print the list of waiting commands
    bool there_are_some = false;

   // Get exclusive access to the lock
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    Input_Source *inp = Input;
    while (inp != NULL) {
      InputLineObject *clip = inp->InObj ();
      if (clip != NULL) {
       // Print the source command
        there_are_some = true;
        std::string cmd = clip->Command ();
        if (cmd.length() > 0) {
          mystream << cmd;
          if (cmd.substr(cmd.length()-1,1) != "\n")  {
            mystream << std::endl;
          }
        }
      }
      inp = inp->Next();
    }

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
    return there_are_some;
  }
  bool Print_Active_Cmds (std::ostream &mystream) {
   // Print the list of completed commands
    bool there_are_some = false;

   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); cmi++) {
      there_are_some = true;
      InputLineObject *clip =  *cmi;
      std::string cmd = clip->Command ();
      if (cmd.length() > 0) {
        mystream << cmd;
        if (cmd.substr(cmd.length()-1,1) != "\n")  {
          mystream << std::endl;
        }
      }
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
    return there_are_some;
  }
  void Print_Header(std::ostream &mystream) {
    mystream << "W " << id << ":";
    mystream << (Input_Is_Async?" async":" sync") << (remote?" remote":" local");
    mystream << "IAM:" << I_Call_Myself << " " << Host_ID << " " << Process_ID;
    mystream << " focus at " << Focus() << " log->" << Log_File_Name << std::endl;

    mystream << "      outstreams=" << ptr2str(default_outstream);
    mystream << ", errstream=" << ptr2str (default_errstream) << std::endl;
  }
};

// For the Text-Line-Interface
class TLI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new TLI_InputLineObject (From, Cmd);
    }

// Constructors
 private:
  TLI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  TLI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
};

// For the Graphacial-User-Interface
class GUI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new GUI_InputLineObject (From, Cmd);
    }

  // Constructors
 private:
  GUI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  GUI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
};

// For the Remote-Line-Interface
class RLI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new RLI_InputLineObject (From, Cmd);
    }

  // Constructors
 private:
  RLI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  RLI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
};

// Local Utilities

CommandWindowID *Find_Command_Window (CMDWID WindowID)
{
  CommandWindowID *found_window = NULL;

 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.

  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

#if DEBUG_CLI
   std::cerr << "IN Find_Command_Window, WindowID=" << WindowID << std::endl;
#endif

// Search for existing entry.
  if (WindowID > 0) {
    std::list<CommandWindowID *>::iterator cwi;
    for (cwi = CommandWindowID_list.begin(); cwi != CommandWindowID_list.end(); cwi++) {
      if (WindowID == (*cwi)->ID ()) {
        found_window = *cwi;
        break;
      }
    }
  }
  Assert (found_window != NULL);

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

  return found_window;
}

void Send_Message_To_Window (CMDWID to_window, std::string S)
{
#if DEBUG_CLI
    std::cerr << " Send_Message_To_Window, to_window=" << to_window << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (to_window);
  if ((cw != NULL) &&
      (cw->has_outstream())) {
   // Print a message to the window.
    ss_ostream *this_ss_stream = cw->ss_outstream();
    this_ss_stream->acquireLock();
    this_ss_stream->mystream() << S << std::endl;
    this_ss_stream->releaseLock();

   // Clean up window and (maybe) issue a new prompt.
    cw->Remove_Completed_Input_Lines (true);
  }
}

void Link_Cmd_Obj_to_Input (InputLineObject *I, CommandObject *C)
{

#if DEBUG_CLI
    std::cerr << " Link_Cmd_Obj_to_Input, I->Who()=" << I->Who() << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (I->Who());

  Assert(cw != NULL);

 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.
  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

  I->Push_Cmd_Obj(C);

  if (I->What() == ILO_ERROR) {
   // Something is wrong with the original commanad.
   // Abort processing of the CommandObject.
    C->set_Status (CMD_ABORTED);
  }

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);
}

bool All_Command_Objects_Are_Used (InputLineObject *clip) {
  std::list<CommandObject *> cmd_object = clip->CmdObj_List();
  std::list<CommandObject *>::iterator coi;
  for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
    if (!((*coi)->Results_Used())) {
     // Try to outputthe results and set Results_Used.
      CommandObject *co = *coi;
      if ((co->Status() == CMD_COMPLETE) ||
          (co->Status() == CMD_ERROR)) {
        if (clip->CallBackC (co)) {
          continue;
        }
      }
      return false;
    }
  }
  return true;
}

bool All_Command_Objects_Have_Executed (InputLineObject *clip) {
  std::list<CommandObject *> cmd_object = clip->CmdObj_List();
  std::list<CommandObject *>::iterator coi;
  for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
    Command_Status S = (*coi)->Status();
    if ((S != CMD_COMPLETE) &&
        (S != CMD_ERROR) &&
        (S != CMD_ABORTED)) {
      return false;
    }
  }
  return true;
}

void Clip_Complete (InputLineObject *clip) {
}

void Cmd_Obj_Complete (CommandObject *C) {


  InputLineObject *clip = C->Clip();

  if (!clip->Complex_Exp()) {

   // Helgrind flaging this call, but how to protect
   // Remove_Completed_Input_Lines has protection via
   // Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);
   // in it's routine body

   // If output has been completed, issue a prompt
    clip->Set_Semantics_Complete ();

    CMDWID w = clip->Who();
#if DEBUG_CLI
    std::cerr << " Cmd_Obj_Complete, w=" << w << std::endl;
#endif
    CommandWindowID *cw = (w) ? Find_Command_Window (w) : NULL;
    if (cw != NULL) cw->Remove_Completed_Input_Lines (true);
  }
}

int64_t Find_Command_Level (CMDWID WindowID)
{

#if DEBUG_CLI
  std::cerr << " Find_Command_Level, WindowID=" << WindowID << std::endl;
#endif

  CommandWindowID *cwi = Find_Command_Window (WindowID);
  return (cwi != NULL) ? cwi->Input_Level() : 0;
}

// Semantic Utilities
bool Window_Is_Async (CMDWID WindowID)
{
#if DEBUG_CLI
    std::cerr << " Window_Is_Async, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *cwi = Find_Command_Window (WindowID);


  return (cwi != NULL) ? cwi->Async() : false;
}

// Low Level Semantic Routines

// Remove all knowledge of the given EXPID.
void Experiment_Purge_Focus (EXPID ExperimentID)
{
  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
    if (ExperimentID == (*cwi)->Focus ()) {
      (*cwi)->Set_Focus(0);
    }
  }
}

// What is the current focus associated with a CommandWindow?
EXPID Experiment_Focus (CMDWID WindowID)
{
  if (WindowID == 0) WindowID = Last_ReadWindow;

#if DEBUG_CLI
    std::cerr << " Experiment_Focus, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *my_window = Find_Command_Window (WindowID);

  EXPID f = my_window ? my_window->Focus() : 0;
  if (f < 0) {
   // This window has not been given a focus, use the last experiment.
    f = Experiment_Sequence_Number;

   // Find the last one that still exist.
    while ((f > 0) && (Find_Experiment_Object (f) == NULL)) {
      f--;
    }

    if (f > 0) {
     // This is the initial value we are going to use.
      my_window->Set_Focus (f);

      if (f > 1) {
        char a[100];
        const char *m = "The Focus has been initialized to Experiment ";
        bcopy (m, a, strlen(m));
        sprintf (&a[strlen(m)], "%lld\n", pthread_self(), f);
        Send_Message_To_Window ( WindowID, a);
      }
    }
  }
  return f;
}

// Set the focus for a particular CommandWindow.
EXPID Experiment_Focus (CMDWID WindowID, EXPID ExperimentID)
{
  if (WindowID == 0) WindowID = Last_ReadWindow;
#if DEBUG_CLI
    std::cerr << " Experiment_Focus2, WindowID=" << WindowID << " Last_ReadWindow=" << Last_ReadWindow << std::endl;
#endif
  CommandWindowID *my_window = Find_Command_Window (WindowID);
  if (my_window) {
    ExperimentObject *Experiment = (ExperimentID) ? Find_Experiment_Object (ExperimentID) : NULL;
    my_window->Set_Focus(ExperimentID);
    return ExperimentID;
  }
  return 0;
}

static bool Read_Log_File_History (CommandObject *cmd, enum Log_Entry_Type log_type,
                                   std::string fname, std::ostream *toStream)
{
  FILE *cmdf = fopen (fname.c_str(), "r");
  struct stat stat_buf;
  if (!cmdf) {
    return false;
  }
  if (stat (fname.c_str(), &stat_buf) != 0) {
    return false;
  }
  char *s = (char *)malloc(stat_buf.st_size+1);
  for (int i=0; i < stat_buf.st_size; ) {
    fgets (s, (stat_buf.st_size), cmdf);
    int64_t len  = strlen(s);
    if (len > 2) {
      bool dump_this_record = false;
      char *t = s;
      switch (log_type)
      {
        case CMDW_TRACE_ALL :
         // Dump raw data - i.e. all of every record.
          dump_this_record = true;
          break;
        case CMDW_TRACE_COMMANDS:
         // Dump records with leading "C ".
          if ((*s) ==  (*"C ")) {
            dump_this_record = true;
            t+=2;
          }
          break;
        case CMDW_TRACE_ORIGINAL_COMMANDS:
         // Dump records with leading "C " but strip off time stamp
          if ((*s) ==  (*"C ")) {
            char *S = strstr(t,"PARSING:");
            if (S != NULL) {
              S += 9;  // One more to get rid of blank separator.
              if (strlen(t) > 2) {
                dump_this_record = true;
                t = S;
                break;
              }
            }
          }
          break;
        case CMDW_TRACE_RESULTS:
         // Dump records with leading "R ".
          if ((*s) ==  (*"R ")) {
            dump_this_record = true;
            t+=2;
          }
          break;
      }
      if (dump_this_record) {
        if (toStream == NULL) {
          *(s+len-1) = *("\0");
          cmd->Result_String (t);
        } else {
          *(s+len-1) = *("\n");
          *toStream << t;
        }
      }
    }
    i+=len;
  }
  free (s);
  fclose (cmdf);
  return true;
}

// Set up an alternative log file at user request.
bool Command_Log_ON (CMDWID WindowID, std::string tofname)
{
#if DEBUG_CLI
    std::cerr << " Command_Log_ON, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  return (cmdw->Set_Log_File (tofname));
}
bool Command_Log_OFF (CMDWID WindowID)
{
#if DEBUG_CLI
    std::cerr << " Command_Log_OFF, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Log_File ();
  return true;
}

// Set up an alternative record file at user request.
bool Command_Record_ON (CMDWID WindowID, std::string tofname)
{
#if DEBUG_CLI
    std::cerr << " Command_Record_ON, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  return (cmdw->Set_Record_File (tofname));
}

bool Command_Record_OFF (CMDWID WindowID)
{
#if DEBUG_CLI
    std::cerr << " Command_Record_OFF, WindowID=" << WindowID << std::endl;
#endif
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Record_File ();
  return true;
}

// This is the start of the Command Line Processing Routines.

void Commander_Initialization () {

 // Define default err and out streams.
  class err_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      std::cerr << s;
    }
    virtual void flush_ostream () {
      std::cerr << std::flush;
    }
  };
  class out_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      std::cout << s;
    }
    virtual void flush_ostream () {
      std::cout << std::flush;
    }
  };
  ss_err = new err_ostream ();
  ss_out = new out_ostream ();
  ss_ttyout = NULL;

 // Set up History Buffer.
  History_Count = 0;
  History.empty();
}

CMDWID Default_Window (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
  // Assert(command_line_window == 0);
 // Create a new Window
  CommandWindowID *cwid = new CommandWindowID(std::string(my_name ? my_name : ""),
                                              std::string(my_host ? my_host : ""),
                                              my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  command_line_window = cwid->ID();
  return command_line_window;
}

CMDWID TLI_Window (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
  Assert(tli_window == 0);
 // Define the output control stream for the command terminal.
  class tli_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      *ttyout_stream << s;
    }
    virtual void flush_stream () {
      *ttyout_stream << std::flush;
    }
  };

  ss_ttyout = new tli_ostream ();

  ss_ttyout->Set_Issue_Prompt (true);

  ttyout_stream = new std::ofstream ("/dev/tty", std::ios::out);

 // Create a new Window
  CommandWindowID *cwid = new TLI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);

 // Setup output redirection.
 // The user can redirect stdout for the tool and we
 // will still send critical output to the terminal window.
  if (isatty(fileno(stdout))) {
   // Replace uses of stdout with ttyout.
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      CommandWindowID *my_window = *cwi;
      if (my_window->ss_outstream() == ss_out) {
        my_window->set_outstream ( ss_ttyout );
      }
    }
  }
  if (isatty(fileno(stderr))) {
   // Replace uses of stderr with ttyout.
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      CommandWindowID *my_window = *cwi;
      if (my_window->ss_errstream() == ss_err) {
        my_window->set_errstream ( ss_ttyout );
      }
    }
  }

  Async_Inputs |= Input_is_Async;
  tli_window = cwid->ID();
  return tli_window;
}

CMDWID GUI_Window (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
#if DEBUG_CLI
  std::cerr << "ENTER GUI_Window, gui_window=" << gui_window << " my_pid=" << my_pid << std::endl;
#endif
  Assert(gui_window == 0);
  CommandWindowID *cwid = new GUI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);

#if DEBUG_CLI
  std::cerr << "IN GUI_Window, after calling GUI_CommandWindowID, cwid=" << cwid << " my_host=" << my_host << std::endl;
  std::cerr << "IN GUI_Window, after calling GUI_CommandWindowID, cwid=" << cwid << " my_panel=" << my_panel << std::endl;
#endif

  Async_Inputs |= Input_is_Async;
  gui_window = cwid->ID();
#if DEBUG_CLI
  std::cerr << "EXIT GUI_Window, after calling GUI_CommandWindowID, gui_window=" << gui_window << " Async_Inputs=" << Async_Inputs << std::endl;
#endif
  return gui_window;
}

CMDWID RLI_Window (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new RLI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
}

CMDWID Embedded_Window (const char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Check for any previous initialization.
  Assert(command_line_window == 0);
  Assert(tli_window == 0);
  Assert(gui_window == 0);
  Assert(Embedded_WindowID == 0);

 // Create a new Window
  Embedded_CommandWindow  = new CommandWindowID(std::string(my_name ? my_name : ""),
                                                std::string(my_host ? my_host : ""),
                                                my_pid, my_panel, Input_is_Async);
  Embedded_WindowID = Embedded_CommandWindow->ID();
  return Embedded_WindowID;
}

void Window_Termination (CMDWID im)
{
  if (im) {

#if DEBUG_CLI
    std::cerr << " Window_Termination, im=" << im << std::endl;
#endif
    CommandWindowID *my_window = Find_Command_Window (im);

   // Clear base ID's that are used in the destructor.
    if (im == command_line_window) {
      command_line_window = 0;
    } else if (im == tli_window) {
      tli_window = 0;
    } else if (im == gui_window) {
      gui_window = 0;
    }

   // Now we can remove the CommandWindowID structure.
    if (my_window) delete my_window;
  }
  return;
}

void Commander_Termination () {

 // Remove all remaining input windows.
  if (Embedded_WindowID != 0) {
   // Because this routine is called through a static destructor,
   // we may encoutner an ordering problem: CommandWindowID_list,
   // which is also called through a static destructor, may have
   // already been purged.  Try to reclaim the Embedded_WindowID
   // without looking through the list.
    Assert (Embedded_CommandWindow != NULL);
    delete Embedded_CommandWindow;
    return;
  }

  std::list<CommandWindowID *>::iterator cwi;
  for (cwi = CommandWindowID_list.begin(); cwi != CommandWindowID_list.end(); )
  {
    CommandWindowID *my_window = *cwi;
    cwi++;
    if (my_window) {
      delete my_window;
    }
  }

 // Remove default ss_ostream definitions.
 // Copy and clear them first, so that catching a signal will not cause
 // a problem if we re-enter this same routine.
  ss_ostream *old_ss_ttyout = ss_ttyout;
  ss_ostream *old_ss_out = ss_out;
  ss_ostream *old_ss_err = ss_err;
  ss_ttyout = NULL;
  ss_out = NULL;
  ss_err = NULL;
  if (old_ss_ttyout && (old_ss_ttyout != old_ss_out)) delete old_ss_ttyout;
  if (old_ss_out) delete old_ss_out;
  if (old_ss_err) delete old_ss_err;

  return;
}

void Redirect_Window_Output (CMDWID for_window, ss_ostream *for_out, ss_ostream *for_err) {
#if DEBUG_CLI
    std::cerr << " Redirect_Window_Output, for_window=" << for_window << std::endl;
#endif
  CommandWindowID *my_window = Find_Command_Window (for_window);
  if ((for_out == NULL) &&
      (for_window != tli_window) &&
      (tli_window != 0)) {
   // Find a default value to use for the stream.
#if DEBUG_CLI
    std::cerr << " Redirect_Window_Output, tli_window=" << tli_window << std::endl;
#endif
    CommandWindowID *tliw = Find_Command_Window (tli_window);
    for_out = tliw->has_outstream() ? tliw->ss_outstream() : ss_out;
  }
 // Overwrite the previous stream - note we don't close it!
  ss_ostream *old_outstream = my_window->ss_outstream();
  if (old_outstream != NULL) {
   // Wait until in-process-output is complete
    old_outstream->acquireLock();
  }
  my_window->set_outstream (for_out);
  if (old_outstream != NULL) {
    old_outstream->releaseLock();
  }

  if ((for_err == NULL) &&
      (for_window != tli_window) &&
      (tli_window != 0)) {
   // Find a default value to use for the stream.
#if DEBUG_CLI
    std::cerr << " Redirect_Window_Output, tli_window=" << tli_window << std::endl;
#endif
    CommandWindowID *tliw = Find_Command_Window (tli_window);
    for_err = tliw->has_errstream() ? tliw->ss_errstream() : ss_err;
  }
 // Overwrite the previous stream - note we don't close it!
  ss_ostream *old_errstream = my_window->ss_errstream();
  if (old_errstream != NULL) {
   // Wait until in-process-output is complete
    old_errstream->acquireLock();
  }
  my_window->set_errstream (for_err);
  if (old_errstream != NULL) {
    old_errstream->releaseLock();
  }

 // We may want to redirect command line output.
  if ((gui_window == for_window) &&
      (command_line_window != 0) &&
      (tli_window == 0)) {
    if (isatty(fileno(stdout))) {
     // Replace uses of stdout on the command line with the GUI text window.
#if DEBUG_CLI
    std::cerr << " Redirect_Window_Output, command_line_window=" << command_line_window << std::endl;
#endif
      CommandWindowID *my_window = Find_Command_Window (command_line_window);
      my_window->set_outstream ( for_out );
    }
    if (isatty(fileno(stderr))) {
     // Replace uses of stderr on the command line with the GUI text window.
#if DEBUG_CLI
    std::cerr << " Redirect_Window_Output2, command_line_window=" << command_line_window << std::endl;
#endif
      CommandWindowID *my_window = Find_Command_Window (command_line_window);
      my_window->set_errstream ( for_err );
    }
  }
}

ss_ostream *Window_outstream (CMDWID for_window) {
#if DEBUG_CLI
    std::cerr << " Window_outstream, for_window=" << for_window << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (for_window);
  return (cw != NULL) ? cw->ss_outstream() : NULL;
}

ss_ostream *Window_errstream (CMDWID for_window) {
#if DEBUG_CLI
    std::cerr << " Window_errstream, for_window=" << for_window << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (for_window);
  return (cw != NULL) ? cw->ss_errstream() : NULL;
}

static std::string Window_Name (CMDWID issuedbywindow) {
#if DEBUG_CLI
    std::cerr << " Window_Name, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  return ((cw == NULL) ? "DEAD" : cw->IAM());
}

void Internal_Info_Dump (CMDWID issuedbywindow) {
  bool Fatal_Error_Encountered = false;
#if DEBUG_CLI
    std::cerr << " Internal_Info_Dump, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  if ((cw == NULL) || (cw->ID() == 0)) {
    std::cerr << "    ERROR: the window(" << issuedbywindow << ") this command came from is illegal" << std::endl;
    return;
  }
  if (!(cw->has_outstream())) {
    std::cerr << "    ERROR: window(" << issuedbywindow << ") thas no defined output stream" << std::endl;
    return;
  }
  ss_ostream *this_ss_stream = Window_outstream (issuedbywindow);
  this_ss_stream->acquireLock();
  std::ostream &mystream = this_ss_stream->mystream();
  mystream << PACKAGE_STRING << " Status:" << std::endl;
  mystream << "    " << (Looking_for_Async_Inputs?" ":"Not") << " Waiting for Async input" << std::endl;
  if (Looking_for_Async_Inputs) {
    if (More_Input_Needed_From_Window) {
      mystream << "    " << "Processing of a complex statement requires input from W "
               << More_Input_Needed_From_Window  << std::endl;
#if DEBUG_CLI
    std::cerr << " Internal_Info_Dump, More_Input_Needed_From_Window=" << More_Input_Needed_From_Window << std::endl;
#endif
      CommandWindowID *lw = Find_Command_Window (More_Input_Needed_From_Window);
      if ((lw == NULL) || (!lw->Input_Available())) {
        mystream << "    ERROR: this window can not provide more input!!" << std::endl;
        Fatal_Error_Encountered = true;
      }
    }
  }
  mystream << "      ss_ostreams: stdout=" << ptr2str(ss_out);
  mystream << ", stderr=" << ptr2str (ss_err);
  mystream << ", ttyout=" << ptr2str (ss_ttyout) << std::endl;
  mystream << "      " << EXT_Created << " Command Execution Threads have been created" << std::endl;

 // Summarize the defined input windows
  if (*CommandWindowID_list.begin()) {
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      (*cwi)->Print_Header(mystream);
    }
  } else {
    mystream << std::endl << "(there are no defined windows)" << std::endl;
  }

 // Dump the ExperimentObject_list
  std::list<ExperimentObject *>::reverse_iterator expi;
  Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
  {
    (*expi)->Print(mystream);
  }
  Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);

  this_ss_stream->releaseLock();
}

void User_Info_Dump (CMDWID issuedbywindow) {
  bool Fatal_Error_Encountered = false;
#if DEBUG_CLI
    std::cerr << " User_Info_Dump, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  std::list<CommandWindowID *>::reverse_iterator cwi;
  Assert (cw != NULL);
  Assert (cw->ID() != 0);
  Assert (cw->has_outstream());

 // Clean out backlog
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
    (*cwi)->Remove_Completed_Input_Lines (false);
  }

  ss_ostream *this_ss_stream = Window_outstream (issuedbywindow);
  this_ss_stream->acquireLock();
  std::ostream &mystream = this_ss_stream->mystream();

  mystream << std::endl << "************************" << std::endl;
  mystream << "Current status of " << PACKAGE_STRING << std::endl;
  mystream << "If you want information about commands, ask for 'help'" << std::endl;

 // An error check
  if (More_Input_Needed_From_Window) {
    mystream << "Processing of a complex statement requires input from "
             << Window_Name(More_Input_Needed_From_Window) << std::endl;
#if DEBUG_CLI
    std::cerr << " User_Info_Dump, More_Input_Needed_From_Window=" << More_Input_Needed_From_Window << std::endl;
#endif
    CommandWindowID *lw = Find_Command_Window (More_Input_Needed_From_Window);
    if ((lw == NULL) || (!lw->Input_Available())) {
      mystream << "    ERROR: this window can not provide more input!!" << std::endl;
      Fatal_Error_Encountered = true;
    }
  }

 // How many escape comands are in-process?
  Assert(pthread_mutex_lock(&Escape_Command_Lock) == 0);
  if (Number_of_Escape_Commands > 0) {
    mystream << std::endl << "There " << ((Number_of_Escape_Commands == 1) ? " is " : " are ")
             << Number_of_Escape_Commands << " escape('!') command"
             << ((Number_of_Escape_Commands == 1) ? " " : "s ") << "being executed independently"
             << std::endl;
  }
  Assert(pthread_mutex_unlock(&Escape_Command_Lock) == 0);

 // Check available input
  bool input_available = false;

 // Which files are we reading?
  bool header_added = false;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
    CommandWindowID *cw = *cwi;
    if (cw->Input_File()) {
      if (!header_added) {
        header_added = true;
        mystream << std::endl << "Input is being read from the following files:" << std::endl;
      }
      mystream << cw->Input_File_Name() << " from the " << cw->IAM() << std::endl;
      input_available = true;
    }
  }

 // What commands are queued?
  header_added = false;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
    CommandWindowID *cw = *cwi;
    if (cw->Input_Queued()) {
      if (!header_added) {
        header_added = true;
        mystream << std::endl << "Input has been queued from the " << cw->IAM() << " input window." << std::endl;
      }
      (void) cw->Print_Queued_Cmds(mystream);
      input_available = true;
    }
  }

  if (!input_available) {
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      CommandWindowID *cw = *cwi;
      if (cw->Input_Async()) {
        mystream << std::endl << "Waiting for a new command to be entered" << std::endl;
        input_available = true;
        break;
      }
    }
    if (!input_available) {
      mystream << "ERROR: It is not possible to input new commands" << std::endl;
      Fatal_Error_Encountered = true;
    }
  }

 // What are the active commands?
  bool there_are_active_cmds = false;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
    if ((*cwi)->Cmds_BeingProcessed()) {
      there_are_active_cmds = true;
      break;
    }
  }
  if (there_are_active_cmds) {
    mystream << std::endl << "Commands currently being processed are:" << std::endl;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
      CommandWindowID *cw = *cwi;
      (void) cw->Print_Active_Cmds(mystream);
    }
  } else {
    mystream << std::endl << "There are no Commands being processed" << std::endl;
  }

 // Determine the number of active experiments
  Assert(pthread_mutex_lock(&Experiment_List_Lock) == 0);
  int64_t ecnt = 0;
  std::list<ExperimentObject *>::reverse_iterator expi;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
    ecnt++;
  }

  if (ecnt == 0) {
    mystream << std::endl << "There are no defined experiments" << std::endl;
  } else {
   // Report the status of each experiment
    mystream << std::endl;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      int expstatus = exp->Status ();
      if (exp->TS_Lock ()) {
        expstatus = exp->Determine_Status ();
        exp->Q_UnLock ();
      }
      mystream << "Experiment " << exp->ExperimentObject_ID();
      if (expstatus == ExpStatus_NonExistent) {
        mystream << " is not currently connected to any executable program";
      } else if (expstatus == ExpStatus_Paused) {
        if (exp->CanExecute()) {
          mystream << " is suspended and is not executing";
        } else {
          mystream << " contains no executable programs";
        }
      } else if (expstatus == ExpStatus_Running) {
        mystream << " is running";
      } else if (expstatus == ExpStatus_Terminated) {
        mystream << " has terminated";
      } else {
        mystream << " is in the error state";
      }
      mystream << std::endl;
    }
  }

 // Identify those experiments where the database will not be saved
  int64_t nosave_cnt = 0;
  for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
    ExperimentObject *exp = *expi;
    int expstatus = exp->Status ();
    if (exp->TS_Lock ()) {
      expstatus = exp->Determine_Status ();
      exp->Q_UnLock ();
    }
    if ((expstatus != ExpStatus_NonExistent) &&
        (exp->Data_Base_Is_Tmp())) {
      nosave_cnt++;
    }
  }

  if (nosave_cnt > 0) {
    mystream << std::endl << "Performance data for the following "
             << "experiments will not be saved on exit:" << std::endl;
    for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++) {
      ExperimentObject *exp = *expi;
      int expstatus = exp->Status ();
      if ((expstatus != ExpStatus_NonExistent) &&
          (exp->Data_Base_Is_Tmp())) {
        mystream << "Experiment " << exp->ExperimentObject_ID() << std::endl;
      }
    }
  }
  Assert(pthread_mutex_unlock(&Experiment_List_Lock) == 0);

  mystream << "************************" << std::endl << std::endl;
  this_ss_stream->releaseLock();
}

// Send a command to the shell for execution.
// Do this by forking a process that will execute the command
// and redirect output from it to a pipe that send it
// back to the initiating proccess.  That process will
// read the output and redirected to the proper window.
static int
do_cmd (const char *cmd_string, pid_t &child_pid) {
  int input[2];

  (void) pipe(input);

  pid_t pid = fork();
  if (pid < 0)
  {
    perror("Can't fork command");
    return (-1);
  }

  if (pid == 0)     // child
  {
    SET_SIGNAL (SIGCHLD, SIG_IGN);  // Ignore child process interrupt
    SET_SIGNAL (SIGINT, SIG_IGN);   // Ingore CNTRL-C interrupt
    (void) close(1);
    (void) dup2(input[1], 1);  // redirect stdout
    (void) dup2(input[1], 2);  // redirect stderr
    (void) close(input[1]);
    (void) system(cmd_string);
    _exit(0);
  }

  // Don't need pipe output fd
  close (input[1]);

  child_pid = pid;
  return (input[0]);
}

static void
catch_escape_sigchld (int sig, int error_num) {
  return;
}

static void
do_escape_cmd (void *arg) {
  std::pair<CMDWID,const char *> *args = (std::pair<CMDWID,const char *> *)arg;
  CMDWID issuedbywindow = args->first;
  const char *command = args->second;

#if DEBUG_CLI
    std::cerr << " do_escape_cmd, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

  if ((cw == NULL) || (cw->ID() == 0)) {
    std::cerr << "    ERROR: the window(" << issuedbywindow << ") this command came from is illegal" << std::endl;
    return;
  }
  if (!(cw->has_outstream())) {
    std::cerr << "    ERROR: window(" << issuedbywindow << ") thas no defined output stream" << std::endl;
    return;
  }

  pid_t new_pid = -1;
  pid_t child_pid;

 // fork a process to execute the command
 // and get the handle to the output back.
  int in_fd = do_cmd ( command, child_pid );

  if (in_fd < 0) {
    return;
  }

 // Request notification that child is terminating.
  SET_SIGNAL (SIGCHLD, catch_escape_sigchld);
  escape_processes.push_back(child_pid);

  Assert(pthread_mutex_lock(&Escape_Command_Lock) == 0);
  Number_of_Escape_Commands++;
  Assert(pthread_mutex_unlock(&Escape_Command_Lock) == 0);

  ss_ostream *this_ss_stream = Window_outstream (issuedbywindow);
  std::ostream &mystream = this_ss_stream->mystream();

  FILE *input = fdopen(in_fd, "r");
  char line [DEFAULT_INPUT_BUFFER_SIZE];
  if ( fgets (line, DEFAULT_INPUT_BUFFER_SIZE-1, input) ) {

    do {
     // Output each line as it is received.
     // But prevent intermixing of lines coming from other sources.
      this_ss_stream->acquireLock();
      mystream << line;
      this_ss_stream->releaseLock();
    } while ( fgets (line, DEFAULT_INPUT_BUFFER_SIZE-1, input) );

    mystream << std::endl;
  }

  fclose (input);
  close (in_fd);
  int statptr = 0;
  (void)waitpid (child_pid, &statptr, 0);  // call wait to clear <defunc> process

  Assert(pthread_mutex_lock(&Escape_Command_Lock) == 0);
  Number_of_Escape_Commands--;
  Assert(pthread_mutex_unlock(&Escape_Command_Lock) == 0);
  pthread_exit(0);
}

static bool Isa_SS_Command (CMDWID issuedbywindow, const char *b_ptr) {
  int64_t fc;
  bool itis = true;

  for (fc = 0; fc < strlen(b_ptr); fc++) {
    if (b_ptr[fc] != *(" ")) break;
  }

  if (b_ptr[fc] == *("\?")) {
    if (b_ptr[fc+1] == *("\?")) {
      Internal_Info_Dump  (issuedbywindow);
    } else {
      User_Info_Dump  (issuedbywindow);
    }
    itis = false;
  } else if (b_ptr[fc] == *("!")) {
   // Create a monitor process which will fork the actual command.
   // This is done so that Openss can accept new commands if
   // this one never completes.
    pthread_t phandle;
    std::pair<CMDWID,const char *> args(issuedbywindow, &b_ptr[fc+1]);
    int stat = pthread_create(&phandle,
                              0,
                              (void   *(*)(void *))do_escape_cmd,
                              (void   *)&args);
    if (stat != 0) {
      perror("Can't fork command");
      return false;
    }

   // Save the Process ID for later reference.
    // escape_processes.push_back(phandle);

   // Wait a little while so the command can start executing
   // and, perhaps, even complete execution.
    usleep (100000);

   // return and look for new input.
    itis = false;
  }

  if (!itis) {
   // Clean out completed commands and issue prompt
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++) {
      (*cwi)->Remove_Completed_Input_Lines (true);
    }
  }

  return itis;
}

static InputLineObject *Append_Input_String (CMDWID issuedbywindow, InputLineObject *clip) {
  if (clip != NULL) {
    if (Isa_SS_Command(issuedbywindow,clip->Command().c_str())) {

#if DEBUG_CLI
    std::cerr << " Append_Input_String, issuedbywindow=" << issuedbywindow << std::endl;
#endif
      CommandWindowID *cw = Find_Command_Window (issuedbywindow);

      Assert (cw);

      Input_Source *inp = new Input_Source (clip);
      clip->SetStatus (ILO_QUEUED_INPUT);
      cw->Append_Input_Source (inp);
      return clip;
    } else {
      delete clip;
    }
  }
  return NULL;
}

InputLineObject *Append_Input_String (CMDWID issuedbywindow, const char *b_ptr,
                                      void *LocalCmdId,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {
  InputLineObject *clip = NULL;;

  if (Isa_SS_Command(issuedbywindow, (const char *)b_ptr)) {

#if DEBUG_CLI
    std::cerr << " Append_Input_String4, issuedbywindow=" << issuedbywindow << std::endl;
#endif
    CommandWindowID *cw = Find_Command_Window (issuedbywindow);

    Assert (cw);
    clip = cw->New_InputLineObject(issuedbywindow, std::string(b_ptr));
    Input_Source *inp = new Input_Source (clip);
    clip->Set_CallBackId (LocalCmdId);
    clip->Set_CallBackL (CallBackLine);
    clip->Set_CallBackC (CallBackCmd);
    clip->SetStatus (ILO_QUEUED_INPUT);
    cw->Append_Input_Source (inp);
  }
  return clip;
}

static bool Append_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {

#if DEBUG_CLI
    std::cerr << " Append_Input_Buffer, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

  Assert (cw);

  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Append_Input_Source (inp);
  return true;
}

bool Append_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {

#if DEBUG_CLI
    std::cerr << " Append_Input_File3, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  if (inp->InFileError()) {
    return FALSE;
  }
  inp->Set_CallBackL (CallBackLine);
  inp->Set_CallBackC (CallBackCmd);
  cw->Append_Input_Source (inp);
  return true;
}

static bool Push_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {

#if DEBUG_CLI
    std::cerr << " Push_Input_Buffer, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

  Assert (cw);
  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Push_Input_Source (inp);
  return true;
}

bool Push_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {

#if DEBUG_CLI
    std::cerr << " Push_Input_File, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  if (inp->InFileError()) {
    return false;
  }
  inp->Set_CallBackL (CallBackLine);
  inp->Set_CallBackC (CallBackCmd);
  cw->Push_Input_Source (inp);
  return true;
}

void ReDirect_User_Stdout (const char *S, const int &len, void *tag) {

  CMDWID to_window = (CMDWID)tag;

#if DEBUG_CLI
    std::cerr << " ReDirect_User_Stdout, to_window=" << to_window << std::endl;
#endif
  CommandWindowID *cw = (to_window) ? Find_Command_Window (to_window) : NULL;

  ss_ostream *this_ss_stream = ((cw != NULL) &&
                                cw->has_outstream()) ? cw->ss_outstream() : ss_out;

  if (this_ss_stream != NULL) {
    this_ss_stream->acquireLock();
    std::ostream &mystream = this_ss_stream->mystream();
    mystream.write( S, len);
    this_ss_stream->releaseLock();
  } else {
    std::cout.write( S, len );
    std::cout << std::flush;
  }
}

void ReDirect_User_Stderr (const char *S, const int &len, void *tag) {
  CMDWID to_window = (CMDWID)tag;
#if DEBUG_CLI
    std::cerr << " ReDirect_User_Stderr, to_window=" << to_window << std::endl;
#endif
  CommandWindowID *cw = (to_window) ? Find_Command_Window (to_window) : NULL;
  ss_ostream *this_ss_stream = ((cw != NULL) &&
                                cw->has_errstream()) ? cw->ss_errstream() : ss_err;

  if (this_ss_stream != NULL) {
    this_ss_stream->acquireLock();
    std::ostream &mystream = this_ss_stream->mystream();
    mystream.write( S, len);
    this_ss_stream->releaseLock();
  } else {
    std::cerr.write( S, len );
    std::cerr << std::flush;
  }
}

void Default_TLI_Line_Output (InputLineObject *clip) {

 // Make sure all the output, associated with every CommandObject, has been printed.
  Assert(clip != NULL);
  if (!(clip->Results_Used())) {
    std::list<CommandObject *> cmd_list = clip->CmdObj_List();
    std::list<CommandObject *>::iterator cmi;
    for (cmi = cmd_list.begin(); cmi != cmd_list.end(); cmi++) {
      if (!(*cmi)->Results_Used ()) {
        Default_TLI_Command_Output (*cmi);
        if (!(*cmi)->Results_Used ()) return;
      }
    }
    clip->Set_Results_Used();
  }
}

void Default_TLI_Command_Output (CommandObject *C) {
  if (!(C->Results_Used()) &&
      !(C->Needed_By_Python())) {
    std::list<CommandResult *> cmd_result = C->Result_List();
    std::list<CommandResult_RawString *> cmd_annotation = C->Annotation_List();
    if (((cmd_result.begin() != cmd_result.end()) ||
         (C->SaveResultFile().length() > 0) ||
         (cmd_annotation.begin() != cmd_annotation.end())) &&
        (C->Status() != CMD_ABORTED)) {
      InputLineObject *clip = C->Clip ();
      CMDWID w = (clip) ? clip->Who() : 0;
#if DEBUG_CLI
    std::cerr << " Default_TLI_Command_Output, w=" << w << std::endl;
#endif
      CommandWindowID *cw = (w) ? Find_Command_Window (w) : NULL;

     // Print the ResultdObject list
      if ((cw != NULL) &&
          (cw->has_outstream())) {
        ss_ostream *this_ss_stream = ((C->Status() == CMD_ERROR) && cw->has_errstream())
                                                   ? cw->ss_errstream() : cw->ss_outstream();
        this_ss_stream->acquireLock();
        C->Print_Results (this_ss_stream->mystream(), "\n", "\n" );
        this_ss_stream->releaseLock();
      }
    }
    C->set_Results_Used (); // Everything is where it belongs.
  }
}

void Default_Log_Output (CommandObject *C) {
#if DEBUG_CLI
    std::cerr << " Default_Log_Output, C->Clip()->Who()=" << C->Clip()->Who() << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (C->Clip()->Who());
  if (cw != NULL) cw->Print_Log_File (C);
}
void Default_Log_Output (InputLineObject *clip) {
#if DEBUG_CLI
    std::cerr << " Default_Log_Output, clip->Who()=" << clip->Who() << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (clip->Who());
  if (cw != NULL) cw->Print_Log_File (clip);
}

// Get rid of commands.

void   Purge_Input_Sources () {
  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
   // Get rid of all queued commands from this window.
    CommandWindowID *cw = *cwi;
    cw->Purge_All_Input ();
  }
}

void   Purge_Executing_Commands () {
  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
   // Get rid of all executing commands from this window.
    CommandWindowID *cw = *cwi;
    cw->Abort_Executing_Input_Lines ();
  }
}

// Catch keyboard interrupt signals from TLI window.

static void User_Interrupt (CMDWID issuedbywindow) {
#if DEBUG_CLI
    std::cerr << " User_Interrupt, issuedbywindow=" << issuedbywindow << std::endl;
#endif
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  if (cw != NULL) {
   // Get rid of all queued commands from this window.
    cw->Purge_All_Input ();

   // Get rid of all commands waiting for dispatch.
    Purge_Dispatch_Queue ();

   // Stop commands that are being processed.
    cw->Abort_Executing_Input_Lines ();

   // Clean out comands-in-process list.
    cw->Remove_Completed_Input_Lines (false);

  }

 // Awaken "wait" commands.
  Purge_Watcher_Waits ();
}

static void
catch_TLI_signal (int sig, int error_num)
{
  if (sig == SIGINT) {
   // CNTRL-C means that we want to stop commands issued from the TLI.
    User_Interrupt (tli_window);
    if (command_line_window) {
     // Also, stop commands issued from the command line.
      User_Interrupt (command_line_window);
    }

    ss_ttyout->acquireLock();
    ss_ttyout->Set_Issue_Prompt(true);
    ss_ttyout->Issue_Prompt();
    ss_ttyout->releaseLock();
    return;
  }

 // Now, kill all remaining escape commands.
  for (std::list<pid_t>::iterator pi = escape_processes.begin(); pi != escape_processes.end(); pi++) {
    kill (*pi, SIGTERM);

   // call wait to clear <defunc> process
    int statptr = 0;
    (void)waitpid (*pi      , &statptr, 0);
  }

 // This isn't graceful, but it gets the job done.
  pthread_exit(0);
}

// This routine continuously reads from /dev/tty and appends the string to an input window.
// It is intended that this routine execute in it's own thread.
void SS_Direct_stdin_Input (void * attachtowindow) {
  try{
    Assert ((CMDWID)attachtowindow != 0);

#if DEBUG_CLI
    std::cerr << " SS_Direct_stdin_Input, (CMDWID)attachtowindow=" << (CMDWID)attachtowindow << std::endl;
#endif
    CommandWindowID *cw = Find_Command_Window ((CMDWID)attachtowindow);

    Assert (cw);
    int64_t Buffer_Size= DEFAULT_INPUT_BUFFER_SIZE;
    char Buffer[Buffer_Size];
//    size_t Buffer_Size= DEFAULT_INPUT_BUFFER_SIZE;
//    char * Buffer = (char *) malloc( Buffer_Size );
    Buffer[Buffer_Size-1] = *"\0";
    ttyin = fopen ( "/dev/tty", "r" );  // Read directly from the xterm window

   // Set up to catch keyboard control signals
    SET_SIGNAL (SIGINT, catch_TLI_signal);   // CNTRL-C
    SET_SIGNAL (SIGUSR1, catch_TLI_signal);  // request to terminate processing

   // Allow us to be terminated from the main thread.
    int previous_value;
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, &previous_value);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &previous_value);

   // If this is the only window, send a welcome message to the user.
    if ((command_line_window == 0) &&
        (gui_window == 0) &&
        (Embedded_WindowID == 0)) {
      ss_ttyout->acquireLock();
      ss_ttyout->mystream() << "Welcome to " << PACKAGE_STRING  << std::endl;
      ss_ttyout->Issue_Prompt();
      ss_ttyout->releaseLock();
    }

   // Infinite loop to read user input.
    for(;;) {
      // usleep (10000); /* DEBUG - give testing code time to react before splashing screen with prompt */
      Buffer[0] == *("\0");
      pthread_testcancel();
      char *read_result = fgets (&Buffer[0], Buffer_Size, ttyin);
//      char *read_result = (getline (&Buffer, &Buffer_Size, ttyin) >= 0) ? Buffer : NULL;
      pthread_testcancel();
      if (Buffer[Buffer_Size-1] != (char)0) {
        ss_ttyout->acquireLock();
        ss_ttyout->mystream() << "ERROR: Input line is too long for buffer." << std::endl;
        ss_ttyout->releaseLock();
        Buffer[Buffer_Size-1] = *"\0";
        continue; // Don't terminate the thread - allow user to retry.
      }
      if (read_result == NULL) {
       // This indicates an EOF or error.
       // This could be a CNTRL-D signal.
        break; // terminate the thread
      }
      if (Buffer[0] == *("\0")) {
       // This indicates an EOF.
        break; // terminate the thread
      }
      if (cw->ID() == 0) {
       // This indicates that someone freed the input window
        break; // terminate the thread
      }

      ss_ttyout->acquireLock();
      ss_ttyout->Set_Issue_Prompt (true);
      ss_ttyout->releaseLock();

      (void) Append_Input_String ((CMDWID)attachtowindow, &Buffer[0], 
                                   NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);

    }

   // We have exited the loop and will no longer read input.
    tli_window = 0;
    if (gui_window != 0) {
     // Stop this thread but keep the GUI runnning.
     // The user can stop OpenSS through the GUI.
      /* pthread_exit (0); */
    } else if (command_line_window == 0) {

     // This is the only window open, terminate Openss with a normal exit
     // so that all previously read commands finish first.
      (void) Append_Input_String ((CMDWID)attachtowindow, "exit",
                                   NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);

    } else {

     // Add an Exit command to the end of the command line input stream
     // so that the command line arguments and input files are processed first.
      (void) Append_Input_String (command_line_window, "exit",
                                  NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);

    }
  }

  catch (std::bad_alloc) {
    std::cerr << "ERROR: Unable to allocate memory to read input." << std::endl;
    abort();
  }

}

// The algorithm is to do a round-robin search of the defined window list
// by starting with the last widow that was read from.  The usual choice
// to read from the next window in the list but, if the parser indicates
// that it is processing a complex statement (e.g. a loop) the next line
// read MUST come from the previous window.
static CMDWID select_input_window (int is_more) {

  CMDWID selectwindow = Last_ReadWindow;

 // Dump results and clear tracking list of previous command
  if ((is_more == 0) &&
      (Last_ReadWindow != 0)) {

#if DEBUG_CLI
    std::cerr << " select_input_window, Last_ReadWindow=" << Last_ReadWindow << std::endl;
#endif
    CommandWindowID *cw = Find_Command_Window (Last_ReadWindow);

    if (cw) {

      cw->Remove_Null_Input_Lines ();
      cw->Remove_Completed_Input_Lines (true);
    }
  }

 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.
  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

  if (is_more == 0) {

    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      if (selectwindow == (*cwi)->ID ()) {

       // Choose the next window to return.
        std::list<CommandWindowID *>::reverse_iterator next_cwi = ++cwi;

        if (next_cwi == CommandWindowID_list.rend()) {
         // At end of list, wrap around to the beginning.
          next_cwi = CommandWindowID_list.rbegin();
        }

        selectwindow = (*next_cwi)->ID();
        goto window_found;
      }
    }

   // A fall through the loop indicates that we didn't find the Last_ReadWindow.
   // Do error recovery by choosing the first one on the list.
    cwi = CommandWindowID_list.rbegin();
    if ((*cwi)) selectwindow = (*cwi)->ID();

  }

window_found:

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

  Assert (selectwindow);
  Last_ReadWindow = selectwindow;
  return selectwindow;
}

static void There_Must_Be_More_Input (CommandWindowID *cw) {
  if ((cw != NULL) &&
      (!cw->Input_Available()) &&
      (cw->has_outstream())) {
    ss_ostream *this_ss_stream = (cw->has_errstream())
                                     ? cw->ss_errstream() : cw->ss_outstream();
    this_ss_stream->acquireLock();
    this_ss_stream->mystream()
                << "ERROR: The input source that started a complex statement" 
                   " failed to complete the expression.\n";

    this_ss_stream->releaseLock();
    Assert (cw->Input_Available());

  }
}

InputLineObject *SpeedShop_ReadLine (int is_more)
{

  try {
 
#if DEBUG_CLI
    std::cerr << " SpeedShop_ReadLine, Last_ReadWindow=" << Last_ReadWindow << std::endl;
#endif
    CommandWindowID *cw = (Last_ReadWindow != 0) ? Find_Command_Window (Last_ReadWindow) : NULL;

    if ((Waiting_For_Complex_Cmd == false) &&
        is_more &&
        (cw != 0) &&
        (cw->Cmds_BeingProcessed())) {
     // We are going through a transition!
     // Force previous commands to complete before
     // we allow Python to execute nested commands.
      cw->Wait_Until_Cmds_Complete ();
    }

    const char *save_prompt = Current_OpenSpeedShop_Prompt;
    InputLineObject *clip;

    CMDWID readfromwindow = select_input_window(is_more);
    CMDWID firstreadwindow = readfromwindow;
    bool I_HAVE_ASYNC_INPUT_LOCK = false;
    
    Waiting_For_Complex_Cmd = is_more;

read_another_window:
#if DEBUG_CLI
    std::cerr << " read_another_window:, readfromwindow=" << readfromwindow << std::endl;
#endif
    cw = Find_Command_Window (readfromwindow);

    Assert (cw);

    do {

      clip = cw->Read_Command ();

      if (clip == NULL) {
       // The read failed.  Why?  Can we find something else to read?

       // It might be possible to read from a different window.
       // Try all of them so we can pick up commands that are waiting.
        readfromwindow = select_input_window(is_more);
        if (readfromwindow != firstreadwindow) {
         // There is another window to read from.
          goto read_another_window;
        }

       // After checking all windows for waiting input,
       // we might look for input from the gui or a terminal.

        if (Async_Inputs) {

          if (I_HAVE_ASYNC_INPUT_LOCK) {

           // Force a wait until data is ready.
            More_Input_Needed_From_Window = (is_more) ? readfromwindow : 0;

            if (More_Input_Needed_From_Window &&
                cw->has_outstream ()) {

             // Re-issue the prompt
              Current_OpenSpeedShop_Prompt = Alternate_Current_OpenSpeedShop_Prompt;
              ss_ostream *this_ss_stream = cw->ss_outstream();

              this_ss_stream->acquireLock();
              this_ss_stream->Issue_Prompt();
              this_ss_stream->releaseLock();
            }

            Assert(pthread_cond_wait(&Async_Input_Available,&Async_Input_Lock) == 0);

            I_HAVE_ASYNC_INPUT_LOCK = false;

            Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);

          } else {
    
            if (is_more) {
              There_Must_Be_More_Input (cw);
            }

           // Get the lock and try again to read from each of the input windows
           // because something might have arrived after our first check of the
           // window.

            Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);

            Looking_for_Async_Inputs = true;
            I_HAVE_ASYNC_INPUT_LOCK = true;
            if (Shut_Down) {
              clip = NULL;  // Signal an EOF to Python
              break;
            }
          }
          goto read_another_window;
        }

       // The end of all window inputs has been reached.
        if (is_more) {
         // We MUST read from this window!
         // This is an error situation.  How can we recover?
          There_Must_Be_More_Input(NULL);
        }

       // Enter termination processing.
        if (!I_HAVE_ASYNC_INPUT_LOCK) {
          Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);
          I_HAVE_ASYNC_INPUT_LOCK = true;
        }
        if (!Shut_Down) {
         // We must have encountered an EOF on all the input streams.
         // Create an 'exit' command to terminate command processing.
          clip = new InputLineObject (readfromwindow, "exit\n" );
        } else {
         // An 'Exit' command has been processed.  
          I_HAVE_ASYNC_INPUT_LOCK = false;
          Assert(pthread_cond_wait(&Async_Input_Available,&Async_Input_Lock) == 0);
          clip = NULL;  // Signal an EOF to Python
        }
        I_HAVE_ASYNC_INPUT_LOCK = true;
        break;
      }
      const char *s = clip->Command().c_str();
      (void) strlen(s);
      if (!Isa_SS_Command(readfromwindow, s)) {
        clip = NULL;
        continue;
      }
    } while (clip == NULL);


    if (I_HAVE_ASYNC_INPUT_LOCK) {
      I_HAVE_ASYNC_INPUT_LOCK = false;
      Looking_for_Async_Inputs = false;
      Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);
    }

    Current_OpenSpeedShop_Prompt = save_prompt;
    if (clip == NULL) {
      return NULL;
    }

//   Initialize fields in the Clip:
   // Assign a sequence number to the command.
    clip->SetSeq (++Command_Sequence_Number);
   // Reflect internal state in the command and log it to the log file.
    clip->SetStatus (ILO_IN_PARSER);
   // Mark nested commands
    if (is_more)   clip-> Set_Complex_Exp ();

   // Add command to the history file.
    History.push_back(clip->Command());
    History_Count++;
    if (History_Count > OPENSS_HISTORY_LIMIT) {
      (void) History.pop_front();
      History_Count--;
    }

   // Track it until completion
    cw->TrackCmd(clip);

   // Log the command to any user defined record file.
    cw->Record(clip);

    return clip;
  }
  catch (std::bad_alloc) {
    std::cerr << "ERROR: Unable to allocate memory to process input." << std::endl;
    return NULL;
  }
}
